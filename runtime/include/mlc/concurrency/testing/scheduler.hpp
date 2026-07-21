#pragma once

// Deterministic turn-baton scheduler for concurrency tests (TRACK_CONCURRENCY_TEST_HARNESS T1).
// Virtual threads are real std::thread instances serialized by a seeded PRNG.

#include <condition_variable>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace mlc::concurrency::testing {

class TestScheduler {
    struct ThreadState {
        bool finished = false;
        bool parked = false;
        bool waiting_for_turn = true;
        bool has_turn = false;
        std::thread thread;
    };

    uint64_t seed_;
    std::mt19937_64 generator_;
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    std::vector<std::unique_ptr<ThreadState>> threads_;
    std::optional<size_t> running_thread_id_;
    bool joining_ = false;
    std::vector<std::string> events_;

    void grant_turn_locked(size_t thread_id) {
        ThreadState& state = *threads_[thread_id];
        state.has_turn = true;
        state.waiting_for_turn = false;
        running_thread_id_ = thread_id;
        condition_.notify_all();
    }

    void release_turn_locked() {
        if (!running_thread_id_.has_value()) return;
        size_t thread_id = *running_thread_id_;
        threads_[thread_id]->has_turn = false;
        threads_[thread_id]->waiting_for_turn = true;
        running_thread_id_ = std::nullopt;
        condition_.notify_all();
    }

    std::vector<size_t> runnable_ids_locked() const {
        std::vector<size_t> runnable;
        for (size_t index = 0; index < threads_.size(); ++index) {
            const ThreadState& state = *threads_[index];
            if (!state.finished && !state.parked && state.waiting_for_turn)
                runnable.push_back(index);
        }
        return runnable;
    }

    bool all_finished_locked() const {
        for (const auto& state : threads_) {
            if (!state->finished) return false;
        }
        return true;
    }

    bool deadlock_locked() const {
        if (all_finished_locked()) return false;
        if (running_thread_id_.has_value()) return false;
        for (const auto& state : threads_) {
            if (!state->finished && !state->parked) return false;
        }
        return true;
    }

    void schedule_next_locked() {
        std::vector<size_t> runnable = runnable_ids_locked();
        if (runnable.empty()) return;
        std::uniform_int_distribution<size_t> distribution(0, runnable.size() - 1);
        size_t chosen = runnable[distribution(generator_)];
        grant_turn_locked(chosen);
    }

    void wait_for_turn(size_t thread_id) {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [&] {
            return threads_[thread_id]->has_turn || threads_[thread_id]->finished;
        });
    }

public:
    explicit TestScheduler(uint64_t seed)
        : seed_(seed), generator_(seed) {}

    TestScheduler(const TestScheduler&) = delete;
    TestScheduler& operator=(const TestScheduler&) = delete;

    uint64_t seed() const { return seed_; }

    size_t current_thread_id() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!running_thread_id_.has_value())
            throw std::logic_error("TestScheduler: no running thread");
        return *running_thread_id_;
    }

    void log_event(std::string event) {
        std::lock_guard<std::mutex> lock(mutex_);
        events_.push_back(std::move(event));
    }

    const std::vector<std::string>& events() const { return events_; }

    // Comma-joined event log for MLC same-seed determinism gates.
    [[nodiscard]] std::string events_joined() const {
      std::string result;
      for (size_t index = 0; index < events_.size(); ++index) {
        if (index > 0) {
          result.push_back(',');
        }
        result += events_[index];
      }
      return result;
    }

    void spawn(std::function<void()> body) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (joining_)
            throw std::logic_error("TestScheduler: spawn after join");
        size_t thread_id = threads_.size();
        auto state = std::make_unique<ThreadState>();
        ThreadState* raw = state.get();
        threads_.push_back(std::move(state));
        raw->thread = std::thread([this, thread_id, body = std::move(body)]() mutable {
            wait_for_turn(thread_id);
            try {
                body();
            } catch (...) {
                // Keep scheduler alive; test assertions catch logic errors.
            }
            std::unique_lock<std::mutex> lock(mutex_);
            threads_[thread_id]->finished = true;
            release_turn_locked();
            condition_.notify_all();
        });
    }

    void yield() {
        size_t thread_id;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (!running_thread_id_.has_value())
                throw std::logic_error("TestScheduler: yield with no running thread");
            thread_id = *running_thread_id_;
            release_turn_locked();
            schedule_next_locked();
        }
        wait_for_turn(thread_id);
    }

    void park() {
        size_t thread_id;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (!running_thread_id_.has_value())
                throw std::logic_error("TestScheduler: park with no running thread");
            thread_id = *running_thread_id_;
            threads_[thread_id]->parked = true;
            release_turn_locked();
            schedule_next_locked();
        }
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [&] {
            return !threads_[thread_id]->parked && threads_[thread_id]->has_turn;
        });
    }

    void unpark(size_t thread_id) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (thread_id >= threads_.size())
            throw std::out_of_range("TestScheduler: unpark invalid thread");
        threads_[thread_id]->parked = false;
        if (!running_thread_id_.has_value())
            schedule_next_locked();
        condition_.notify_all();
    }

    void join() {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            joining_ = true;
            schedule_next_locked();
            while (!all_finished_locked()) {
                if (deadlock_locked())
                    throw std::runtime_error("TestScheduler: deadlock (all remaining threads parked)");
                if (!running_thread_id_.has_value())
                    schedule_next_locked();
                condition_.wait(lock);
            }
        }
        for (auto& state : threads_) {
            if (state->thread.joinable())
                state->thread.join();
        }
    }
};

} // namespace mlc::concurrency::testing
