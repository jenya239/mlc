#pragma once

#include "mlc/core/task.hpp"
#include "mlc/reactor/timer_heap.hpp"
#include "mlc/reactor/wakeup_descriptor.hpp"

#include "mlc/concurrency/stop.hpp"

#include <chrono>
#include <coroutine>
#include <cstdint>
#include <functional>
#include <limits>
#include <memory>
#include <optional>
#include <system_error>
#include <unordered_map>
#include <utility>
#include <vector>

#include <cerrno>
#include <curl/curl.h>
#include <poll.h>

namespace mlc::reactor {

struct CurlStopWatch {
    CURL* easy = nullptr;
    std::function<bool()> stop_requested;
};

class EventLoop {
    std::shared_ptr<WakeupDescriptor> wakeup_ = std::make_shared<WakeupDescriptor>();
    TimerHeap timers_;
    CURLM* curl_multi_handle_ = nullptr;
    std::unordered_map<curl_socket_t, short> socket_interest_;
    std::unordered_map<CURL*, std::function<std::coroutine_handle<>(CURLcode)>> curl_completions_;
    std::vector<CurlStopWatch> curl_stop_watches_;
    std::optional<std::chrono::steady_clock::time_point> curl_timer_deadline_;
    bool curl_timer_immediate_ = false;

    // Continuations resume only on this thread, after poll returns.
    void resume_ready(std::vector<std::coroutine_handle<>>& ready) {
        for (std::coroutine_handle<> continuation : ready) {
            if (continuation) {
                continuation.resume();
            }
        }
    }

    static int curl_socket_callback(
        CURL* /*easy*/,
        curl_socket_t descriptor,
        int what,
        void* user,
        void* /*socket_data*/) {
        static_cast<EventLoop*>(user)->note_socket(descriptor, what);
        return 0;
    }

    static int curl_timer_callback(CURLM* /*multi*/, long timeout_milliseconds, void* user) {
        static_cast<EventLoop*>(user)->note_curl_timeout(timeout_milliseconds);
        return 0;
    }

    void note_socket(curl_socket_t descriptor, int what) {
        if (what == CURL_POLL_REMOVE) {
            socket_interest_.erase(descriptor);
            return;
        }
        short events = 0;
        if (what == CURL_POLL_IN || what == CURL_POLL_INOUT) {
            events = static_cast<short>(events | POLLIN);
        }
        if (what == CURL_POLL_OUT || what == CURL_POLL_INOUT) {
            events = static_cast<short>(events | POLLOUT);
        }
        socket_interest_[descriptor] = events;
    }

    void note_curl_timeout(long timeout_milliseconds) {
        if (timeout_milliseconds < 0) {
            curl_timer_immediate_ = false;
            curl_timer_deadline_.reset();
            return;
        }
        if (timeout_milliseconds == 0) {
            curl_timer_immediate_ = true;
            curl_timer_deadline_.reset();
            return;
        }
        curl_timer_immediate_ = false;
        curl_timer_deadline_ =
            std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_milliseconds);
    }

    int poll_timeout_milliseconds() {
        int timeout_milliseconds = -1;
        if (const auto deadline = timers_.next_deadline()) {
            const auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(
                *deadline - std::chrono::steady_clock::now());
            if (remaining.count() <= 0) {
                timeout_milliseconds = 0;
            } else if (remaining.count() > std::numeric_limits<int>::max()) {
                timeout_milliseconds = std::numeric_limits<int>::max();
            } else {
                timeout_milliseconds = static_cast<int>(remaining.count());
            }
        }
        if (curl_multi_handle_ == nullptr) {
            return timeout_milliseconds;
        }
        if (curl_timer_immediate_) {
            return 0;
        }
        if (!curl_timer_deadline_) {
            return timeout_milliseconds;
        }
        const auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(
            *curl_timer_deadline_ - std::chrono::steady_clock::now());
        int curl_timeout = 0;
        if (remaining.count() > 0) {
            curl_timeout = remaining.count() > std::numeric_limits<int>::max()
                               ? std::numeric_limits<int>::max()
                               : static_cast<int>(remaining.count());
        }
        if (timeout_milliseconds < 0 || curl_timeout < timeout_milliseconds) {
            return curl_timeout;
        }
        return timeout_milliseconds;
    }

    void drive_curl_sockets(const std::vector<pollfd>& poll_entries, std::size_t socket_offset) {
        std::vector<std::pair<curl_socket_t, int>> ready_sockets;
        for (std::size_t index = socket_offset; index < poll_entries.size(); ++index) {
            const short revents = poll_entries[index].revents;
            if (revents == 0) continue;
            int action = 0;
            if ((revents & POLLIN) != 0) action |= CURL_CSELECT_IN;
            if ((revents & POLLOUT) != 0) action |= CURL_CSELECT_OUT;
            if ((revents & (POLLERR | POLLHUP)) != 0) action |= CURL_CSELECT_ERR;
            ready_sockets.emplace_back(poll_entries[index].fd, action);
        }
        for (const auto& ready_socket : ready_sockets) {
            int running = 0;
            curl_multi_socket_action(
                curl_multi_handle_, ready_socket.first, ready_socket.second, &running);
        }
        const auto now = std::chrono::steady_clock::now();
        const bool timer_due =
            curl_timer_immediate_ || (curl_timer_deadline_ && *curl_timer_deadline_ <= now);
        if (!timer_due) return;
        curl_timer_immediate_ = false;
        curl_timer_deadline_.reset();
        int running = 0;
        curl_multi_socket_action(curl_multi_handle_, CURL_SOCKET_TIMEOUT, 0, &running);
    }

    std::vector<std::coroutine_handle<>> take_finished_transfers() {
        std::vector<std::coroutine_handle<>> ready;
        if (curl_multi_handle_ == nullptr) return ready;
        int pending = 0;
        while (CURLMsg* message = curl_multi_info_read(curl_multi_handle_, &pending)) {
            if (message->msg != CURLMSG_DONE) continue;
            CURL* easy = message->easy_handle;
            const CURLcode code = message->data.result;
            curl_multi_remove_handle(curl_multi_handle_, easy);
            forget_curl_stop_watch(easy);
            const auto found = curl_completions_.find(easy);
            if (found == curl_completions_.end()) continue;
            auto completion = std::move(found->second);
            curl_completions_.erase(found);
            std::coroutine_handle<> continuation = completion(code);
            if (continuation) ready.push_back(continuation);
        }
        return ready;
    }

    void forget_curl_stop_watch(CURL* easy) {
        std::vector<CurlStopWatch> remaining;
        remaining.reserve(curl_stop_watches_.size());
        for (CurlStopWatch& watch : curl_stop_watches_) {
            if (watch.easy != easy) remaining.push_back(std::move(watch));
        }
        curl_stop_watches_ = std::move(remaining);
    }

    std::vector<std::coroutine_handle<>> take_stopped_transfers() {
        std::vector<std::coroutine_handle<>> ready;
        if (curl_multi_handle_ == nullptr) return ready;
        std::vector<CurlStopWatch> remaining;
        remaining.reserve(curl_stop_watches_.size());
        for (CurlStopWatch& watch : curl_stop_watches_) {
            const bool requested = static_cast<bool>(watch.stop_requested) && watch.stop_requested();
            const auto found = curl_completions_.find(watch.easy);
            if (!requested || found == curl_completions_.end()) {
                if (found != curl_completions_.end()) remaining.push_back(std::move(watch));
                continue;
            }
            curl_multi_remove_handle(curl_multi_handle_, watch.easy);
            auto completion = std::move(found->second);
            curl_completions_.erase(found);
            std::coroutine_handle<> continuation = completion(CURLE_ABORTED_BY_CALLBACK);
            if (continuation) ready.push_back(continuation);
        }
        curl_stop_watches_ = std::move(remaining);
        return ready;
    }

public:
    EventLoop() = default;
    EventLoop(const EventLoop&) = delete;
    EventLoop& operator=(const EventLoop&) = delete;
    EventLoop(EventLoop&&) = delete;
    EventLoop& operator=(EventLoop&&) = delete;

    ~EventLoop() {
        curl_stop_watches_.clear();
        if (curl_multi_handle_ == nullptr) return;
        for (const auto& entry : curl_completions_) {
            curl_multi_remove_handle(curl_multi_handle_, entry.first);
        }
        curl_completions_.clear();
        curl_multi_cleanup(curl_multi_handle_);
        curl_multi_handle_ = nullptr;
    }

    static EventLoop*& current_slot() noexcept {
        thread_local EventLoop* loop = nullptr;
        return loop;
    }

    static bool has_current() noexcept { return current_slot() != nullptr; }

    static EventLoop& current() {
        thread_local EventLoop loop;
        current_slot() = &loop;
        return loop;
    }

    void wake() { wakeup_->signal(); }

    // The callback locks the descriptor. After this loop drops it, request() does not write.
    [[nodiscard]] mlc::concurrency::StopSubscription subscribe_stop(
        const mlc::concurrency::StopToken& token) {
        std::weak_ptr<WakeupDescriptor> wakeup = wakeup_;
        return token.subscribe([wakeup] {
            const std::shared_ptr<WakeupDescriptor> descriptor = wakeup.lock();
            if (descriptor) descriptor->signal();
        });
    }

    std::uint64_t schedule_timer(
        std::chrono::steady_clock::time_point deadline,
        std::coroutine_handle<> continuation) {
        return timers_.push(deadline, continuation);
    }

    void cancel_timer(std::uint64_t identifier) { timers_.cancel(identifier); }

    CURLM* ensure_curl_multi() {
        if (curl_multi_handle_ != nullptr) return curl_multi_handle_;
        curl_multi_handle_ = curl_multi_init();
        if (curl_multi_handle_ == nullptr) return nullptr;
        curl_multi_setopt(
            curl_multi_handle_, CURLMOPT_SOCKETFUNCTION, &EventLoop::curl_socket_callback);
        curl_multi_setopt(curl_multi_handle_, CURLMOPT_SOCKETDATA, this);
        curl_multi_setopt(
            curl_multi_handle_, CURLMOPT_TIMERFUNCTION, &EventLoop::curl_timer_callback);
        curl_multi_setopt(curl_multi_handle_, CURLMOPT_TIMERDATA, this);
        return curl_multi_handle_;
    }

    void register_curl_completion(
        CURL* easy, std::function<std::coroutine_handle<>(CURLcode)> completion) {
        curl_completions_.insert_or_assign(easy, std::move(completion));
    }

    void register_curl_stop_watch(CURL* easy, std::function<bool()> stop_requested) {
        curl_stop_watches_.push_back(CurlStopWatch{easy, std::move(stop_requested)});
    }

    void request_curl_service() { curl_timer_immediate_ = true; }

    void run_until(const std::function<bool()>& predicate) {
        while (!predicate()) {
            const int timeout_milliseconds = poll_timeout_milliseconds();
            std::vector<pollfd> poll_entries;
            pollfd wakeup_entry{};
            wakeup_entry.fd = wakeup_->descriptor();
            wakeup_entry.events = POLLIN;
            poll_entries.push_back(wakeup_entry);
            const std::size_t socket_offset = poll_entries.size();
            for (const auto& interest : socket_interest_) {
                pollfd socket_entry{};
                socket_entry.fd = static_cast<int>(interest.first);
                socket_entry.events = interest.second;
                poll_entries.push_back(socket_entry);
            }

            int poll_result = 0;
            do {
                poll_result = ::poll(poll_entries.data(), static_cast<nfds_t>(poll_entries.size()), timeout_milliseconds);
            } while (poll_result < 0 && errno == EINTR);
            if (poll_result < 0) {
                throw std::system_error(errno, std::generic_category(), "poll");
            }
            if ((poll_entries[0].revents & POLLIN) != 0) {
                wakeup_->drain();
            }
            if (curl_multi_handle_ != nullptr) {
                drive_curl_sockets(poll_entries, socket_offset);
            }

            std::vector<std::coroutine_handle<>> ready =
                timers_.pop_expired(std::chrono::steady_clock::now());
            std::vector<std::coroutine_handle<>> finished = take_finished_transfers();
            std::vector<std::coroutine_handle<>> stopped = take_stopped_transfers();
            ready.insert(ready.end(), finished.begin(), finished.end());
            ready.insert(ready.end(), stopped.begin(), stopped.end());
            resume_ready(ready);
        }
    }
};

inline void pump_event_loop_until(bool (*is_done)(void*), void* context) {
    EventLoop::current().run_until([is_done, context] { return is_done(context); });
}

struct ReactorPumpRegistration {
    ReactorPumpRegistration() { mlc::reactor_pump_function() = &pump_event_loop_until; }
};

inline ReactorPumpRegistration reactor_pump_registration{};

}  // namespace mlc::reactor
