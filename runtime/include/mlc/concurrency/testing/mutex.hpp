#pragma once

// TestMutex: same public API as mlc::concurrency::Mutex, yields to TestScheduler (T1).

#include "mlc/concurrency/testing/scheduler.hpp"

#include <type_traits>
#include <utility>

namespace mlc::concurrency::testing {

template<typename Value>
class TestMutex {
    TestScheduler& scheduler_;
    bool locked_ = false;
    Value value_;

public:
    template<typename... Arguments>
    explicit TestMutex(TestScheduler& scheduler, Arguments&&... arguments)
        : scheduler_(scheduler), value_(std::forward<Arguments>(arguments)...) {}

    template<typename Callback>
    auto lock(Callback&& callback) -> std::invoke_result_t<Callback, Value&> {
        using Result = std::invoke_result_t<Callback, Value&>;
        while (locked_)
            scheduler_.yield();
        locked_ = true;
        scheduler_.yield();
        if constexpr (std::is_void_v<Result>) {
            try {
                callback(value_);
                locked_ = false;
                scheduler_.yield();
            } catch (...) {
                locked_ = false;
                scheduler_.yield();
                throw;
            }
        } else {
            try {
                Result result = callback(value_);
                locked_ = false;
                scheduler_.yield();
                return result;
            } catch (...) {
                locked_ = false;
                scheduler_.yield();
                throw;
            }
        }
    }
};

} // namespace mlc::concurrency::testing
