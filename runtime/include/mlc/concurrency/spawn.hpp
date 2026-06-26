#pragma once

// Spawn a callable on a new thread; returns mlc::Task with copied result (TRACK_CONCURRENCY STEP=3).

#include <future>
#include <type_traits>
#include <utility>
#include "mlc/core/task.hpp"

namespace mlc::concurrency {

template<typename Callable>
auto spawn_task(Callable&& callable) -> mlc::Task<std::invoke_result_t<Callable>> {
    using Result = std::invoke_result_t<Callable>;
    auto future = std::async(std::launch::async, std::forward<Callable>(callable));
    co_return future.get();
}

} // namespace mlc::concurrency
