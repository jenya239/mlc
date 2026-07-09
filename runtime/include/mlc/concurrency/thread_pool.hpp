#pragma once

// Fixed-size worker pool + bounded job queue (TRACK_CONCURRENCY_ISOLATE STEP=1).
// submit blocks when the queue is full (no silent drop). shutdown/dtor: close queue, join workers.

#include "mlc/concurrency/channel.hpp"

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

namespace mlc::concurrency {

class ThreadPool {
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

    // Enqueue work. Blocks if the queue is full. Returns false after shutdown.
    template<typename Callable>
    bool submit(Callable&& callable) {
        if (shut_down_) return false;
        return jobs_.send(std::function<void()>(std::forward<Callable>(callable)));
    }

    // Close the job queue and join all workers. Idempotent.
    void shutdown() {
        if (shut_down_) return;
        shut_down_ = true;
        jobs_.close();
        join_workers();
    }

    [[nodiscard]] std::size_t worker_count() const noexcept { return workers_.size(); }

    [[nodiscard]] bool is_shutdown() const noexcept { return shut_down_; }
};

} // namespace mlc::concurrency
