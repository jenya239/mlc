#pragma once

// Bounded thread-safe channel: values are copied in and out (TRACK_CONCURRENCY STEP=2).

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
    if (capacity == 0 || capacity > maximum_capacity)
        throw std::invalid_argument("channel capacity must be 1..1048576");
    return capacity;
}

} // namespace detail

template<typename Value>
class Channel {
    struct State {
        explicit State(size_t capacity_value)
            : capacity(capacity_value), closed(false) {}

        std::mutex mutex;
        std::condition_variable not_empty;
        std::condition_variable not_full;
        std::deque<Value> queue;
        size_t capacity;
        bool closed;
    };

    std::shared_ptr<State> state_;

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
    }

    bool send(const Value& value) {
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
        if (state_->closed || state_->queue.size() >= state_->capacity) return false;
        state_->queue.push_back(value);
        state_->not_empty.notify_one();
        return true;
    }

    std::optional<Value> try_receive() {
        std::lock_guard<std::mutex> lock(state_->mutex);
        if (state_->queue.empty()) return std::nullopt;
        Value value = std::move(state_->queue.front());
        state_->queue.pop_front();
        state_->not_full.notify_one();
        return value;
    }
};

template<typename Value>
Channel<Value> make_channel(size_t capacity = 64) {
    return Channel<Value>(capacity);
}

} // namespace mlc::concurrency
