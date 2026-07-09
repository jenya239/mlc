#pragma once

// Bounded / rendezvous channel + Sender/Receiver split (TRACK_CONCURRENCY_V2 STEP=2–3).
// capacity >= 1: buffered; capacity 0: synchronous handoff.
// Last Sender drop (or Sender::close) marks closed and wakes waiters.

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

template<typename Value>
struct ChannelState {
    explicit ChannelState(size_t capacity_value)
        : capacity(capacity_value)
        , closed(false)
        , waiting_receivers(0)
        , sender_count(0) {}

    std::mutex mutex;
    std::condition_variable not_empty;
    std::condition_variable not_full;
    std::condition_variable handoff_done;
    std::deque<Value> queue;
    size_t capacity;
    bool closed;
    size_t waiting_receivers;
    size_t sender_count;

    void mark_closed_locked() { closed = true; }

    void wake_all() {
        not_empty.notify_all();
        not_full.notify_all();
        handoff_done.notify_all();
    }
};

template<typename Value>
bool channel_send(std::shared_ptr<ChannelState<Value>> state, Value value) {
    if (state->capacity == 0) {
        std::unique_lock<std::mutex> lock(state->mutex);
        state->not_full.wait(lock, [&] {
            return state->closed || state->waiting_receivers > 0;
        });
        if (state->closed) return false;
        state->queue.push_back(std::move(value));
        lock.unlock();
        state->not_empty.notify_one();
        lock.lock();
        state->handoff_done.wait(lock, [&] {
            return state->closed || state->queue.empty();
        });
        if (!state->queue.empty()) {
            state->queue.clear();
            return false;
        }
        return true;
    }
    std::unique_lock<std::mutex> lock(state->mutex);
    state->not_full.wait(lock, [&] {
        return state->closed || state->queue.size() < state->capacity;
    });
    if (state->closed) return false;
    state->queue.push_back(std::move(value));
    lock.unlock();
    state->not_empty.notify_one();
    return true;
}

template<typename Value>
bool channel_try_send(std::shared_ptr<ChannelState<Value>> state, const Value& value) {
    std::lock_guard<std::mutex> lock(state->mutex);
    if (state->closed) return false;
    if (state->capacity == 0) {
        if (state->waiting_receivers == 0 || !state->queue.empty()) return false;
        state->queue.push_back(value);
        state->not_empty.notify_one();
        return true;
    }
    if (state->queue.size() >= state->capacity) return false;
    state->queue.push_back(value);
    state->not_empty.notify_one();
    return true;
}

template<typename Value>
std::optional<Value> channel_receive(std::shared_ptr<ChannelState<Value>> state) {
    if (state->capacity == 0) {
        std::unique_lock<std::mutex> lock(state->mutex);
        state->waiting_receivers += 1;
        state->not_full.notify_one();
        state->not_empty.wait(lock, [&] {
            return state->closed || !state->queue.empty();
        });
        state->waiting_receivers -= 1;
        if (state->queue.empty()) return std::nullopt;
        Value value = std::move(state->queue.front());
        state->queue.pop_front();
        lock.unlock();
        state->handoff_done.notify_one();
        return value;
    }
    std::unique_lock<std::mutex> lock(state->mutex);
    state->not_empty.wait(lock, [&] {
        return state->closed || !state->queue.empty();
    });
    if (state->queue.empty()) return std::nullopt;
    Value value = std::move(state->queue.front());
    state->queue.pop_front();
    lock.unlock();
    state->not_full.notify_one();
    return value;
}

template<typename Value>
std::optional<Value> channel_try_receive(std::shared_ptr<ChannelState<Value>> state) {
    std::unique_lock<std::mutex> lock(state->mutex);
    if (state->queue.empty()) return std::nullopt;
    Value value = std::move(state->queue.front());
    state->queue.pop_front();
    if (state->capacity == 0) {
        lock.unlock();
        state->handoff_done.notify_one();
    } else {
        lock.unlock();
        state->not_full.notify_one();
    }
    return value;
}

} // namespace detail

template<typename Value>
class Channel;

template<typename Value>
class Receiver;

template<typename Value>
class Sender {
    std::shared_ptr<detail::ChannelState<Value>> state_;
    bool owns_slot_ = false;

    void release_slot() {
        if (!owns_slot_ || !state_) return;
        bool should_close = false;
        {
            std::lock_guard<std::mutex> lock(state_->mutex);
            if (state_->sender_count > 0) state_->sender_count -= 1;
            if (state_->sender_count == 0 && !state_->closed) {
                state_->mark_closed_locked();
                should_close = true;
            }
        }
        owns_slot_ = false;
        if (should_close) state_->wake_all();
    }

    explicit Sender(std::shared_ptr<detail::ChannelState<Value>> state, bool take_slot)
        : state_(std::move(state)), owns_slot_(false) {
        if (take_slot && state_) {
            std::lock_guard<std::mutex> lock(state_->mutex);
            state_->sender_count += 1;
            owns_slot_ = true;
        }
    }

    friend class Channel<Value>;
    friend class Receiver<Value>;
    template<typename T>
    friend std::pair<Sender<T>, Receiver<T>> open_channel(size_t capacity);

public:
    Sender() = default;

    Sender(const Sender& other) : state_(other.state_), owns_slot_(false) {
        if (state_ && other.owns_slot_) {
            std::lock_guard<std::mutex> lock(state_->mutex);
            state_->sender_count += 1;
            owns_slot_ = true;
        }
    }

    Sender& operator=(const Sender& other) {
        if (this == &other) return *this;
        release_slot();
        state_ = other.state_;
        owns_slot_ = false;
        if (state_ && other.owns_slot_) {
            std::lock_guard<std::mutex> lock(state_->mutex);
            state_->sender_count += 1;
            owns_slot_ = true;
        }
        return *this;
    }

    Sender(Sender&& other) noexcept
        : state_(std::move(other.state_)), owns_slot_(other.owns_slot_) {
        other.owns_slot_ = false;
    }

    Sender& operator=(Sender&& other) noexcept {
        if (this == &other) return *this;
        release_slot();
        state_ = std::move(other.state_);
        owns_slot_ = other.owns_slot_;
        other.owns_slot_ = false;
        return *this;
    }

    ~Sender() { release_slot(); }

    Sender clone() const { return Sender(*this); }

    explicit operator bool() const { return static_cast<bool>(state_); }

    size_t capacity() const { return state_ ? state_->capacity : 0; }

    void close() {
        if (!state_) return;
        {
            std::lock_guard<std::mutex> lock(state_->mutex);
            state_->mark_closed_locked();
        }
        state_->wake_all();
    }

    bool send(const Value& value) {
        if (!state_) return false;
        return detail::channel_send(state_, value);
    }

    bool send(Value&& value) {
        if (!state_) return false;
        return detail::channel_send(state_, std::move(value));
    }

    bool try_send(const Value& value) {
        if (!state_) return false;
        return detail::channel_try_send(state_, value);
    }
};

template<typename Value>
class Receiver {
    std::shared_ptr<detail::ChannelState<Value>> state_;

    explicit Receiver(std::shared_ptr<detail::ChannelState<Value>> state)
        : state_(std::move(state)) {}

    friend class Channel<Value>;
    friend class Sender<Value>;
    template<typename T>
    friend std::pair<Sender<T>, Receiver<T>> open_channel(size_t capacity);

public:
    Receiver() = default;
    Receiver(const Receiver&) = default;
    Receiver& operator=(const Receiver&) = default;
    Receiver(Receiver&&) noexcept = default;
    Receiver& operator=(Receiver&&) noexcept = default;

    explicit operator bool() const { return static_cast<bool>(state_); }

    size_t capacity() const { return state_ ? state_->capacity : 0; }

    bool is_closed() const {
        if (!state_) return true;
        std::lock_guard<std::mutex> lock(state_->mutex);
        return state_->closed;
    }

    size_t size() const {
        if (!state_) return 0;
        std::lock_guard<std::mutex> lock(state_->mutex);
        return state_->queue.size();
    }

    std::optional<Value> receive() {
        if (!state_) return std::nullopt;
        return detail::channel_receive(state_);
    }

    std::optional<Value> try_receive() {
        if (!state_) return std::nullopt;
        return detail::channel_try_receive(state_);
    }
};

template<typename Value>
class Channel {
    std::shared_ptr<detail::ChannelState<Value>> state_;

public:
    explicit Channel(size_t capacity = 64)
        : state_(std::make_shared<detail::ChannelState<Value>>(
              detail::channel_validate_capacity(capacity))) {}

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
            state_->mark_closed_locked();
        }
        state_->wake_all();
    }

    bool send(const Value& value) { return detail::channel_send(state_, value); }
    bool send(Value&& value) { return detail::channel_send(state_, std::move(value)); }
    std::optional<Value> receive() { return detail::channel_receive(state_); }
    bool try_send(const Value& value) { return detail::channel_try_send(state_, value); }
    std::optional<Value> try_receive() { return detail::channel_try_receive(state_); }

    Sender<Value> sender() { return Sender<Value>(state_, true); }
    Receiver<Value> receiver() { return Receiver<Value>(state_); }
};

template<typename Value>
Channel<Value> make_channel(size_t capacity = 64) {
    return Channel<Value>(capacity);
}

template<typename Value>
std::pair<Sender<Value>, Receiver<Value>> open_channel(size_t capacity = 64) {
    auto state = std::make_shared<detail::ChannelState<Value>>(
        detail::channel_validate_capacity(capacity));
    Sender<Value> sender(state, true);
    Receiver<Value> receiver(state);
    return {std::move(sender), std::move(receiver)};
}

} // namespace mlc::concurrency
