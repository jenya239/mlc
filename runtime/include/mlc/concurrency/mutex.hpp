#pragma once

// Value-scoped mutex (TRACK_CONCURRENCY STEP=6). lock() holds guard for callback duration only.

#include <mutex>
#include <type_traits>
#include <utility>

namespace mlc::concurrency {

template<typename Value>
class Mutex {
    mutable std::mutex mutex_;
    Value value_;

public:
    template<typename... Arguments>
    explicit Mutex(Arguments&&... arguments)
        : value_(std::forward<Arguments>(arguments)...) {}

    template<typename Callback>
    auto lock(Callback&& callback) -> std::invoke_result_t<Callback, Value&> {
        std::lock_guard<std::mutex> guard(mutex_);
        return callback(value_);
    }
};

} // namespace mlc::concurrency
