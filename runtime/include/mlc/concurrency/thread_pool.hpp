#pragma once

// Fixed-size worker pool + bounded job queue (TRACK_CONCURRENCY_ISOLATE STEP=1+3).
// submit blocks when the queue is full (no silent drop).
// shutdown/dtor: request_cancel → close queue → join workers.
// Blocked submit wakes on cancel (ChannelStatus::Cancelled).

#include "mlc/concurrency/channel.hpp"
#include "mlc/concurrency/stop.hpp"

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace mlc::concurrency {

class ThreadPool {
    StopSource stop_;
    Channel<std::function<void()>> jobs_;
    std::vector<std::thread> workers_;
    bool shut_down_ = false;

    void worker_loop() {
        while (true) {
            auto job = jobs_.receive();
            if (!job.has_value()) break;
            (*job)();
        }
    }

    void join_workers() {
        for (auto& worker : workers_) {
            if (worker.joinable()) worker.join();
        }
        workers_.clear();
    }

    bool enqueue(std::function<void()> job) {
        if (shut_down_) return false;
        const ChannelStatus status = jobs_.send(std::move(job), stop_.token());
        return status == ChannelStatus::Ok;
    }

public:
    ThreadPool(std::size_t worker_count, std::size_t queue_capacity)
        : jobs_(queue_capacity == 0 ? 1 : queue_capacity) {
        if (worker_count == 0) {
            throw std::invalid_argument("ThreadPool worker_count must be >= 1");
        }
        if (queue_capacity == 0) {
            throw std::invalid_argument("ThreadPool queue_capacity must be >= 1");
        }
        workers_.reserve(worker_count);
        for (std::size_t index = 0; index < worker_count; ++index) {
            workers_.emplace_back([this] { worker_loop(); });
        }
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    ~ThreadPool() { shutdown(); }

    [[nodiscard]] StopToken token() const noexcept { return stop_.token(); }

    void request_cancel() noexcept { stop_.request(); }

    [[nodiscard]] bool cancel_requested() const noexcept { return stop_.requested(); }

    // Enqueue work. Blocks if the queue is full. Returns false after shutdown/cancel.
    template<typename Callable>
    bool submit(Callable&& callable) {
        return enqueue(std::function<void()>(std::forward<Callable>(callable)));
    }

    // Enqueue work that receives this pool's StopToken.
    template<typename Callable>
    bool submit_with_token(Callable&& callable) {
        using Decayed = std::decay_t<Callable>;
        StopToken child_token = token();
        return enqueue(std::function<void()>(
            [callable = Decayed(std::forward<Callable>(callable)),
             child_token]() mutable { callable(child_token); }));
    }

    // request_cancel, close the job queue, join all workers. Idempotent.
    void shutdown() {
        if (shut_down_) return;
        shut_down_ = true;
        request_cancel();
        jobs_.close();
        join_workers();
    }

    [[nodiscard]] std::size_t worker_count() const noexcept { return workers_.size(); }

    [[nodiscard]] bool is_shutdown() const noexcept { return shut_down_; }
};

} // namespace mlc::concurrency
