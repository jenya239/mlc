#pragma once

#include <coroutine>
#include <exception>
#include <variant>
#include <optional>
#include <stdexcept>

namespace mlc {

enum class TaskKind { Plain, Reactor };

using ReactorPumpFunction = void (*)(bool (*is_done)(void*), void* context);

inline ReactorPumpFunction& reactor_pump_function() {
    static ReactorPumpFunction function = nullptr;
    return function;
}

// Forward declaration
template<typename T>
class Task;

namespace detail {

// Promise type for coroutines
template<typename T>
struct TaskPromise {
    std::variant<std::monostate, T, std::exception_ptr> result;
    std::coroutine_handle<> continuation;
    TaskKind kind = TaskKind::Plain;
    bool left_initial_suspend = false;

    struct InitialAwaiter {
        TaskPromise* promise;

        bool await_ready() const noexcept { return false; }

        void await_suspend(std::coroutine_handle<>) const noexcept {}

        void await_resume() const noexcept { promise->left_initial_suspend = true; }
    };

    Task<T> get_return_object();

    InitialAwaiter initial_suspend() noexcept { return InitialAwaiter{this}; }

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
    TaskKind kind = TaskKind::Plain;
    bool left_initial_suspend = false;

    struct InitialAwaiter {
        TaskPromise* promise;

        bool await_ready() const noexcept { return false; }

        void await_suspend(std::coroutine_handle<>) const noexcept {}

        void await_resume() const noexcept { promise->left_initial_suspend = true; }
    };

    Task<void> get_return_object();

    InitialAwaiter initial_suspend() noexcept { return InitialAwaiter{this}; }

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

template<typename Handle>
void drive_reactor_task(Handle& handle) {
    if (handle && !handle.done() && !handle.promise().left_initial_suspend) {
        handle.resume();
    }
    if (handle && !handle.done()) {
        ReactorPumpFunction pump = reactor_pump_function();
        if (pump == nullptr) {
            throw std::logic_error("reactor task has no event loop pump");
        }
        pump([](void* context) { return static_cast<Handle*>(context)->done(); }, &handle);
    }
}

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

    bool is_ready() const noexcept {
        return handle_ && handle_.done();
    }

    void mark_as_reactor() noexcept {
        if (handle_) {
            handle_.promise().kind = TaskKind::Reactor;
        }
    }

    // Resume the coroutine (run until next suspension point)
    void resume() {
        if (handle_ && !handle_.done()) {
            handle_.resume();
        }
    }

    T block_on() {
        if (handle_ && handle_.promise().kind == TaskKind::Reactor) {
            detail::drive_reactor_task(handle_);
        } else {
            while (handle_ && !handle_.done()) {
                handle_.resume();
            }
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

    void mark_as_reactor() noexcept {
        if (handle_) {
            handle_.promise().kind = TaskKind::Reactor;
        }
    }

    void resume() {
        if (handle_ && !handle_.done()) {
            handle_.resume();
        }
    }

    void block_on() {
        if (handle_ && handle_.promise().kind == TaskKind::Reactor) {
            detail::drive_reactor_task(handle_);
        } else {
            while (handle_ && !handle_.done()) {
                handle_.resume();
            }
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

template<typename T>
T block_on(Task<T>&& task) {
    return task.block_on();
}

inline void block_on(Task<void>& task) {
    task.block_on();
}

inline void block_on(Task<void>&& task) {
    task.block_on();
}

} // namespace mlc
