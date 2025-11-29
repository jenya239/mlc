#pragma once

#include <coroutine>
#include <exception>
#include <variant>
#include <optional>
#include <stdexcept>

namespace mlc {

// Forward declaration
template<typename T>
class Task;

namespace detail {

// Promise type for coroutines
template<typename T>
struct TaskPromise {
    std::variant<std::monostate, T, std::exception_ptr> result;
    std::coroutine_handle<> continuation;

    Task<T> get_return_object();

    std::suspend_always initial_suspend() noexcept { return {}; }

    struct FinalAwaiter {
        bool await_ready() noexcept { return false; }

        std::coroutine_handle<> await_suspend(std::coroutine_handle<TaskPromise> h) noexcept {
            auto& promise = h.promise();
            if (promise.continuation) {
                return promise.continuation;
            }
            return std::noop_coroutine();
        }

        void await_resume() noexcept {}
    };

    FinalAwaiter final_suspend() noexcept { return {}; }

    void return_value(T value) {
        result.template emplace<1>(std::move(value));
    }

    void unhandled_exception() {
        result.template emplace<2>(std::current_exception());
    }

    T& get_result() {
        if (std::holds_alternative<std::exception_ptr>(result)) {
            std::rethrow_exception(std::get<std::exception_ptr>(result));
        }
        return std::get<T>(result);
    }
};

// Specialization for void
template<>
struct TaskPromise<void> {
    std::optional<std::exception_ptr> exception;
    std::coroutine_handle<> continuation;

    Task<void> get_return_object();

    std::suspend_always initial_suspend() noexcept { return {}; }

    struct FinalAwaiter {
        bool await_ready() noexcept { return false; }

        std::coroutine_handle<> await_suspend(std::coroutine_handle<TaskPromise<void>> h) noexcept {
            auto& promise = h.promise();
            if (promise.continuation) {
                return promise.continuation;
            }
            return std::noop_coroutine();
        }

        void await_resume() noexcept {}
    };

    FinalAwaiter final_suspend() noexcept { return {}; }

    void return_void() {}

    void unhandled_exception() {
        exception = std::current_exception();
    }

    void get_result() {
        if (exception) {
            std::rethrow_exception(*exception);
        }
    }
};

} // namespace detail

// Task<T> - lazy coroutine task that represents an async operation
// Awaitable and movable, but not copyable
template<typename T>
class Task {
public:
    using promise_type = detail::TaskPromise<T>;
    using handle_type = std::coroutine_handle<promise_type>;

    Task() noexcept : handle_(nullptr) {}

    explicit Task(handle_type h) noexcept : handle_(h) {}

    Task(Task&& other) noexcept : handle_(other.handle_) {
        other.handle_ = nullptr;
    }

    Task& operator=(Task&& other) noexcept {
        if (this != &other) {
            if (handle_) {
                handle_.destroy();
            }
            handle_ = other.handle_;
            other.handle_ = nullptr;
        }
        return *this;
    }

    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    ~Task() {
        if (handle_) {
            handle_.destroy();
        }
    }

    // Awaiter for co_await
    struct Awaiter {
        handle_type handle;

        bool await_ready() noexcept {
            return handle.done();
        }

        std::coroutine_handle<> await_suspend(std::coroutine_handle<> continuation) noexcept {
            handle.promise().continuation = continuation;
            return handle;
        }

        T await_resume() {
            return handle.promise().get_result();
        }
    };

    Awaiter operator co_await() noexcept {
        return Awaiter{handle_};
    }

    // Check if task is ready (completed)
    bool is_ready() const noexcept {
        return handle_ && handle_.done();
    }

    // Resume the coroutine (run until next suspension point)
    void resume() {
        if (handle_ && !handle_.done()) {
            handle_.resume();
        }
    }

    // Block and wait for completion, returning the result
    // Note: This runs the task synchronously
    T block_on() {
        while (handle_ && !handle_.done()) {
            handle_.resume();
        }
        return handle_.promise().get_result();
    }

private:
    handle_type handle_;
};

// Specialization for void
template<>
class Task<void> {
public:
    using promise_type = detail::TaskPromise<void>;
    using handle_type = std::coroutine_handle<promise_type>;

    Task() noexcept : handle_(nullptr) {}

    explicit Task(handle_type h) noexcept : handle_(h) {}

    Task(Task&& other) noexcept : handle_(other.handle_) {
        other.handle_ = nullptr;
    }

    Task& operator=(Task&& other) noexcept {
        if (this != &other) {
            if (handle_) {
                handle_.destroy();
            }
            handle_ = other.handle_;
            other.handle_ = nullptr;
        }
        return *this;
    }

    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    ~Task() {
        if (handle_) {
            handle_.destroy();
        }
    }

    struct Awaiter {
        handle_type handle;

        bool await_ready() noexcept {
            return handle.done();
        }

        std::coroutine_handle<> await_suspend(std::coroutine_handle<> continuation) noexcept {
            handle.promise().continuation = continuation;
            return handle;
        }

        void await_resume() {
            handle.promise().get_result();
        }
    };

    Awaiter operator co_await() noexcept {
        return Awaiter{handle_};
    }

    bool is_ready() const noexcept {
        return handle_ && handle_.done();
    }

    void resume() {
        if (handle_ && !handle_.done()) {
            handle_.resume();
        }
    }

    void block_on() {
        while (handle_ && !handle_.done()) {
            handle_.resume();
        }
        handle_.promise().get_result();
    }

private:
    handle_type handle_;
};

namespace detail {

template<typename T>
Task<T> TaskPromise<T>::get_return_object() {
    return Task<T>{std::coroutine_handle<TaskPromise<T>>::from_promise(*this)};
}

inline Task<void> TaskPromise<void>::get_return_object() {
    return Task<void>{std::coroutine_handle<TaskPromise<void>>::from_promise(*this)};
}

} // namespace detail

// Helper functions matching MLC stdlib
template<typename T>
bool is_ready(const Task<T>& task) {
    return task.is_ready();
}

template<typename T>
T block_on(Task<T>& task) {
    return task.block_on();
}

inline void block_on(Task<void>& task) {
    task.block_on();
}

} // namespace mlc
