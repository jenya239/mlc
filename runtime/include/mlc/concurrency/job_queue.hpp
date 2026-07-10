#pragma once

// In-process job queue on ThreadPool (TRACK_STDLIB_JOB_QUEUE STEP=2).
// enqueue / schedule_after / wait_idle / shutdown; fixed max_attempts retry.
// Timer thread for delays (does not block pool workers with sleep).

#include "mlc/concurrency/thread_pool.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <cstddef>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

namespace mlc {
namespace concurrency {

class JobQueue {
  struct DelayedJob {
    std::chrono::steady_clock::time_point due_time;
    std::function<void()> work;
    std::size_t attempt = 1;
  };

  struct DelayedJobLater {
    bool operator()(const DelayedJob& left, const DelayedJob& right) const {
      return left.due_time > right.due_time;
    }
  };

  ThreadPool pool_;
  std::size_t max_attempts_;
  std::atomic<std::size_t> pending_{0};
  std::mutex idle_mutex_;
  std::condition_variable idle_cv_;

  std::mutex timer_mutex_;
  std::condition_variable timer_cv_;
  std::priority_queue<DelayedJob, std::vector<DelayedJob>, DelayedJobLater> delayed_jobs_;
  std::thread timer_thread_;
  bool shut_down_ = false;

  void notify_idle_if_needed() {
    if (pending_.load() == 0) {
      std::lock_guard<std::mutex> lock(idle_mutex_);
      idle_cv_.notify_all();
    }
  }

  void finish_one_pending() {
    pending_.fetch_sub(1);
    notify_idle_if_needed();
  }

  bool submit_attempt(std::function<void()> work, std::size_t attempt) {
    if (shut_down_) {
      return false;
    }
    return pool_.submit([this, work = std::move(work), attempt]() mutable {
      try {
        work();
        finish_one_pending();
      } catch (...) {
        if (attempt < max_attempts_ && !shut_down_) {
          if (!submit_attempt(std::move(work), attempt + 1)) {
            finish_one_pending();
          }
        } else {
          finish_one_pending();
        }
      }
    });
  }

  void timer_loop() {
    while (true) {
      DelayedJob next_job;
      {
        std::unique_lock<std::mutex> lock(timer_mutex_);
        while (!shut_down_ && delayed_jobs_.empty()) {
          timer_cv_.wait(lock);
        }
        if (shut_down_) {
          break;
        }
        const auto due_time = delayed_jobs_.top().due_time;
        if (timer_cv_.wait_until(lock, due_time) == std::cv_status::timeout) {
          // Fall through to pop due jobs.
        }
        if (shut_down_) {
          break;
        }
        if (delayed_jobs_.empty()) {
          continue;
        }
        if (delayed_jobs_.top().due_time > std::chrono::steady_clock::now()) {
          continue;
        }
        next_job = delayed_jobs_.top();
        delayed_jobs_.pop();
      }
      if (!submit_attempt(std::move(next_job.work), next_job.attempt)) {
        finish_one_pending();
      }
    }
  }

public:
  JobQueue(
      std::size_t worker_count,
      std::size_t queue_capacity,
      std::size_t max_attempts = 1)
      : pool_(worker_count, queue_capacity),
        max_attempts_(max_attempts == 0 ? 1 : max_attempts) {
    timer_thread_ = std::thread([this] { timer_loop(); });
  }

  JobQueue(const JobQueue&) = delete;
  JobQueue& operator=(const JobQueue&) = delete;
  JobQueue(JobQueue&&) = delete;
  JobQueue& operator=(JobQueue&&) = delete;

  ~JobQueue() { shutdown(); }

  template <typename Callable>
  bool enqueue(Callable&& callable) {
    if (shut_down_) {
      return false;
    }
    pending_.fetch_add(1);
    if (!submit_attempt(std::function<void()>(std::forward<Callable>(callable)), 1)) {
      finish_one_pending();
      return false;
    }
    return true;
  }

  template <typename Callable>
  bool schedule_after(std::int64_t delay_milliseconds, Callable&& callable) {
    if (shut_down_) {
      return false;
    }
    if (delay_milliseconds < 0) {
      delay_milliseconds = 0;
    }
    pending_.fetch_add(1);
    DelayedJob delayed;
    delayed.due_time = std::chrono::steady_clock::now() +
                       std::chrono::milliseconds(delay_milliseconds);
    delayed.work = std::function<void()>(std::forward<Callable>(callable));
    delayed.attempt = 1;
    {
      std::lock_guard<std::mutex> lock(timer_mutex_);
      if (shut_down_) {
        finish_one_pending();
        return false;
      }
      delayed_jobs_.push(std::move(delayed));
    }
    timer_cv_.notify_one();
    return true;
  }

  void wait_idle() {
    std::unique_lock<std::mutex> lock(idle_mutex_);
    idle_cv_.wait(lock, [this] { return pending_.load() == 0; });
  }

  void shutdown() {
    {
      std::lock_guard<std::mutex> lock(timer_mutex_);
      if (shut_down_) {
        return;
      }
      shut_down_ = true;
      while (!delayed_jobs_.empty()) {
        delayed_jobs_.pop();
        finish_one_pending();
      }
    }
    timer_cv_.notify_all();
    if (timer_thread_.joinable()) {
      timer_thread_.join();
    }
    pool_.shutdown();
    // Drain any race: wait until in-flight jobs finish counting down.
    wait_idle();
  }

  [[nodiscard]] std::size_t pending_count() const noexcept {
    return pending_.load();
  }

  [[nodiscard]] std::size_t worker_count() const noexcept {
    return pool_.worker_count();
  }

  [[nodiscard]] bool is_shutdown() const noexcept { return shut_down_; }
};

} // namespace concurrency
} // namespace mlc
