#pragma once

// Structured concurrency scope (TRACK_CONCURRENCY_TASKSCOPE STEP=2).
// Invariant: destructor / join() waits for all spawned children.
// Children may observe StopToken from scope.token(); request_cancel() wakes them.

#include "mlc/concurrency/stop.hpp"

#include <future>
#include <type_traits>
#include <utility>
#include <vector>

namespace mlc::concurrency {

class TaskScope {
    StopSource stop_;
    std::vector<std::future<void>> children_;
    bool joined_ = false;

    void join_unlocked() {
        for (auto& child : children_) {
            if (child.valid()) child.get();
        }
        children_.clear();
        joined_ = true;
    }

public:
    TaskScope() = default;
    TaskScope(const TaskScope&) = delete;
    TaskScope& operator=(const TaskScope&) = delete;

    TaskScope(TaskScope&& other) noexcept
        : stop_(std::move(other.stop_))
        , children_(std::move(other.children_))
        , joined_(other.joined_) {
        other.joined_ = true;
        other.children_.clear();
    }

    TaskScope& operator=(TaskScope&& other) noexcept {
        if (this == &other) return *this;
        request_cancel();
        join();
        stop_ = std::move(other.stop_);
        children_ = std::move(other.children_);
        joined_ = other.joined_;
        other.joined_ = true;
        other.children_.clear();
        return *this;
    }

    ~TaskScope() {
        request_cancel();
        join();
    }

    [[nodiscard]] StopToken token() const noexcept { return stop_.token(); }

    void request_cancel() noexcept { stop_.request(); }

    [[nodiscard]] bool cancel_requested() const noexcept { return stop_.requested(); }

    // Spawn work that does not take a token.
    template<typename Callable>
    void spawn(Callable&& callable) {
        using Decayed = std::decay_t<Callable>;
        joined_ = false;
        children_.push_back(std::async(
            std::launch::async,
            [callable = Decayed(std::forward<Callable>(callable))]() mutable {
                callable();
            }));
    }

    // Spawn work that receives this scope's StopToken.
    template<typename Callable>
    void spawn_with_token(Callable&& callable) {
        using Decayed = std::decay_t<Callable>;
        joined_ = false;
        StopToken child_token = token();
        children_.push_back(std::async(
            std::launch::async,
            [callable = Decayed(std::forward<Callable>(callable)),
             child_token]() mutable {
                callable(child_token);
            }));
    }

    void join() {
        if (joined_ && children_.empty()) return;
        join_unlocked();
    }
};

} // namespace mlc::concurrency
