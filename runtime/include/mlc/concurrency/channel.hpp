#pragma once

// Bounded / rendezvous channel: values are copied in and out.
// capacity >= 1: buffered; capacity 0: synchronous handoff (TRACK_CONCURRENCY_V2 STEP=2).

#include <condition_variable>
#include <cstddef>
#include <deque>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <utility>

namespace mlc::concurrency {

namespace detail {

inline size_t channel_validate_capacity(size_t capacity) {
    constexpr size_t maximum_capacity = 1048576;
    if (capacity > maximum_capacity)
        throw std::invalid_argument("channel capacity must be 0..1048576");
    return capacity;
}

} // namespace detail

template<typename Value>
class Channel {
    struct State {
        explicit State(size_t capacity_value)
            : capacity(capacity_value)
            , closed(false)
            , waiting_receivers(0) {}

        std::mutex mutex;
        std::condition_variable not_empty;
        std::condition_variable not_full;
        std::condition_variable handoff_done;
        std::deque<Value> queue;
        size_t capacity;
        bool closed;
        size_t waiting_receivers;
    };

    std::shared_ptr<State> state_;

    bool send_rendezvous(Value value) {
        std::unique_lock<std::mutex> lock(state_->mutex);
        state_->not_full.wait(lock, [&] {
            return state_->closed || state_->waiting_receivers > 0;
        });
        if (state_->closed) return false;
        state_->queue.push_back(std::move(value));
        lock.unlock();
        state_->not_empty.notify_one();
        lock.lock();
        state_->handoff_done.wait(lock, [&] {
            return state_->closed || state_->queue.empty();
        });
        if (!state_->queue.empty()) {
            state_->queue.clear();
            return false;
        }
        return true;
    }

    std::optional<Value> receive_rendezvous() {
        std::unique_lock<std::mutex> lock(state_->mutex);
        state_->waiting_receivers += 1;
        state_->not_full.notify_one();
        state_->not_empty.wait(lock, [&] {
            return state_->closed || !state_->queue.empty();
        });
        state_->waiting_receivers -= 1;
        if (state_->queue.empty()) return std::nullopt;
        Value value = std::move(state_->queue.front());
        state_->queue.pop_front();
        lock.unlock();
        state_->handoff_done.notify_one();
        return value;
    }

public:
    explicit Channel(size_t capacity = 64)
        : state_(std::make_shared<State>(detail::channel_validate_capacity(capacity))) {}

    Channel(const Channel&) = default;
    Channel& operator=(const Channel&) = default;

    size_t capacity() const { return state_->capacity; }

    bool is_closed() const {
        std::lock_guard<std::mutex> lock(state_->mutex);
        return state_->closed;
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(state_->mutex);
        return state_->queue.size();
    }

    void close() {
        {
            std::lock_guard<std::mutex> lock(state_->mutex);
            state_->closed = true;
        }
        state_->not_empty.notify_all();
        state_->not_full.notify_all();
        state_->handoff_done.notify_all();
    }

    bool send(const Value& value) {
        if (state_->capacity == 0) return send_rendezvous(value);
        std::unique_lock<std::mutex> lock(state_->mutex);
        state_->not_full.wait(lock, [&] {
            return state_->closed || state_->queue.size() < state_->capacity;
        });
        if (state_->closed) return false;
        state_->queue.push_back(value);
        lock.unlock();
        state_->not_empty.notify_one();
        return true;
    }

    bool send(Value&& value) {
        if (state_->capacity == 0) return send_rendezvous(std::move(value));
        std::unique_lock<std::mutex> lock(state_->mutex);
        state_->not_full.wait(lock, [&] {
            return state_->closed || state_->queue.size() < state_->capacity;
        });
        if (state_->closed) return false;
        state_->queue.push_back(std::move(value));
        lock.unlock();
        state_->not_empty.notify_one();
        return true;
    }

    std::optional<Value> receive() {
        if (state_->capacity == 0) return receive_rendezvous();
        std::unique_lock<std::mutex> lock(state_->mutex);
        state_->not_empty.wait(lock, [&] {
            return state_->closed || !state_->queue.empty();
        });
        if (state_->queue.empty()) return std::nullopt;
        Value value = std::move(state_->queue.front());
        state_->queue.pop_front();
        lock.unlock();
        state_->not_full.notify_one();
        return value;
    }

    bool try_send(const Value& value) {
        std::lock_guard<std::mutex> lock(state_->mutex);
        if (state_->closed) return false;
        if (state_->capacity == 0) {
            if (state_->waiting_receivers == 0 || !state_->queue.empty()) return false;
            state_->queue.push_back(value);
            state_->not_empty.notify_one();
            return true;
        }
        if (state_->queue.size() >= state_->capacity) return false;
        state_->queue.push_back(value);
        state_->not_empty.notify_one();
        return true;
    }

    std::optional<Value> try_receive() {
        std::unique_lock<std::mutex> lock(state_->mutex);
        if (state_->queue.empty()) return std::nullopt;
        Value value = std::move(state_->queue.front());
        state_->queue.pop_front();
        if (state_->capacity == 0) {
            lock.unlock();
            state_->handoff_done.notify_one();
        } else {
            lock.unlock();
            state_->not_full.notify_one();
        }
        return value;
    }
};

template<typename Value>
Channel<Value> make_channel(size_t capacity = 64) {
    return Channel<Value>(capacity);
}

} // namespace mlc::concurrency
