#pragma once

#include "mlc/reactor/timer_heap.hpp"
#include "mlc/reactor/wakeup_descriptor.hpp"

#include <chrono>
#include <coroutine>
#include <cstdint>
#include <functional>
#include <limits>
#include <system_error>
#include <utility>
#include <vector>

#include <cerrno>
#include <poll.h>

namespace mlc::reactor {

class EventLoop {
    WakeupDescriptor wakeup_;
    TimerHeap timers_;

    // Continuations resume only on this thread, after poll returns.
    void resume_ready(std::vector<std::coroutine_handle<>>& ready) {
        for (std::coroutine_handle<> continuation : ready) {
            if (continuation) {
                continuation.resume();
            }
        }
    }

public:
    EventLoop() = default;
    EventLoop(const EventLoop&) = delete;
    EventLoop& operator=(const EventLoop&) = delete;
    EventLoop(EventLoop&&) = delete;
    EventLoop& operator=(EventLoop&&) = delete;

    static EventLoop& current() {
        thread_local EventLoop loop;
        return loop;
    }

    void wake() { wakeup_.signal(); }

    std::uint64_t schedule_timer(
        std::chrono::steady_clock::time_point deadline,
        std::coroutine_handle<> continuation) {
        return timers_.push(deadline, continuation);
    }

    void cancel_timer(std::uint64_t identifier) { timers_.cancel(identifier); }

    void run_until(const std::function<bool()>& predicate) {
        while (!predicate()) {
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

            pollfd poll_entry{};
            poll_entry.fd = wakeup_.descriptor();
            poll_entry.events = POLLIN;
            int poll_result = 0;
            do {
                poll_result = ::poll(&poll_entry, 1, timeout_milliseconds);
            } while (poll_result < 0 && errno == EINTR);
            if (poll_result < 0) {
                throw std::system_error(errno, std::generic_category(), "poll");
            }
            if ((poll_entry.revents & POLLIN) != 0) {
                wakeup_.drain();
            }

            std::vector<std::coroutine_handle<>> ready =
                timers_.pop_expired(std::chrono::steady_clock::now());
            resume_ready(ready);
        }
    }
};

}  // namespace mlc::reactor
