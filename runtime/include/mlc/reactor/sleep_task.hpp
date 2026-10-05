#pragma once

#include "mlc/core/task.hpp"
#include "mlc/reactor/event_loop.hpp"

#include <chrono>
#include <cstdint>
#include <coroutine>

namespace mlc::reactor {

struct SleepAwaiter {
    std::int64_t milliseconds;

    [[nodiscard]] bool await_ready() const noexcept { return milliseconds <= 0; }

    void await_suspend(std::coroutine_handle<> continuation) const {
        const auto deadline =
            std::chrono::steady_clock::now() + std::chrono::milliseconds(milliseconds);
        EventLoop::current().schedule_timer(deadline, continuation);
    }

    void await_resume() const noexcept {}
};

inline mlc::Task<void> sleep_for_milliseconds_body(std::int64_t milliseconds) {
    co_await SleepAwaiter{milliseconds};
}

inline mlc::Task<void> sleep_for_milliseconds(std::int64_t milliseconds) {
    mlc::Task<void> task = sleep_for_milliseconds_body(milliseconds);
    task.mark_as_reactor();
    return task;
}

}  // namespace mlc::reactor
