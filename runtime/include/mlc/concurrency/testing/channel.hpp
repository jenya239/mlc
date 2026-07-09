#pragma once

// TestChannel: same public API as mlc::concurrency::Channel, yields to TestScheduler (T1).

#include "mlc/concurrency/channel.hpp"
#include "mlc/concurrency/testing/scheduler.hpp"

#include <cstddef>
#include <deque>
#include <memory>
#include <optional>
#include <utility>

namespace mlc::concurrency::testing {

template<typename Value>
class TestChannel {
    struct State {
        explicit State(size_t capacity_value)
            : capacity(capacity_value), closed(false) {}

        std::deque<Value> queue;
        size_t capacity;
        bool closed;
    };

    TestScheduler& scheduler_;
    std::shared_ptr<State> state_;

public:
    explicit TestChannel(TestScheduler& scheduler, size_t capacity = 64)
        : scheduler_(scheduler)
        , state_(std::make_shared<State>(
              ::mlc::concurrency::detail::channel_validate_capacity(capacity))) {}

    TestChannel(const TestChannel&) = default;
    TestChannel& operator=(const TestChannel&) = default;

    size_t capacity() const { return state_->capacity; }

    bool is_closed() const { return state_->closed; }

    size_t size() const { return state_->queue.size(); }

    void close() {
        state_->closed = true;
        scheduler_.yield();
    }

    bool send(const Value& value) {
        while (!state_->closed && state_->queue.size() >= state_->capacity)
            scheduler_.yield();
        if (state_->closed) return false;
        state_->queue.push_back(value);
        scheduler_.yield();
        return true;
    }

    bool send(Value&& value) {
        while (!state_->closed && state_->queue.size() >= state_->capacity)
            scheduler_.yield();
        if (state_->closed) return false;
        state_->queue.push_back(std::move(value));
        scheduler_.yield();
        return true;
    }

    std::optional<Value> receive() {
        while (!state_->closed && state_->queue.empty())
            scheduler_.yield();
        if (state_->queue.empty()) return std::nullopt;
        Value value = std::move(state_->queue.front());
        state_->queue.pop_front();
        scheduler_.yield();
        return value;
    }

    bool try_send(const Value& value) {
        if (state_->closed || state_->queue.size() >= state_->capacity) return false;
        state_->queue.push_back(value);
        return true;
    }

    std::optional<Value> try_receive() {
        if (state_->queue.empty()) return std::nullopt;
        Value value = std::move(state_->queue.front());
        state_->queue.pop_front();
        return value;
    }
};

} // namespace mlc::concurrency::testing
