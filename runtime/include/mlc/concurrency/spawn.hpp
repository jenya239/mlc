#pragma once

// Spawn a callable on a new thread; returns mlc::Task with copied result (TRACK_CONCURRENCY STEP=3).
// Work starts eagerly so many spawn_task calls overlap before block_on.
// Callable is decay-copied outside the coroutine frame (no dangling parameter).

#include <future>
#include <type_traits>
#include <utility>
#include "mlc/core/task.hpp"

namespace mlc::concurrency {

namespace detail {

template<typename Result>
mlc::Task<Result> task_from_future(std::future<Result> future) {
    co_return future.get();
}

inline mlc::Task<void> task_from_future(std::future<void> future) {
    future.get();
    co_return;
}

} // namespace detail

template<typename Callable>
auto spawn_task(Callable&& callable) -> mlc::Task<std::invoke_result_t<std::decay_t<Callable>>> {
    using Decayed = std::decay_t<Callable>;
    using Result = std::invoke_result_t<Decayed>;
    auto future = std::async(std::launch::async, Decayed(std::forward<Callable>(callable)));
    return detail::task_from_future(std::move(future));
}

} // namespace mlc::concurrency
