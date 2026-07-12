#pragma once

// Supervisor (TRACK_CONCURRENCY_SUPERVISOR STEP=1–3).
// Register children, start/stop via StopToken + join.
// Restart policies: permanent / transient / temporary (one_for_one).
// Restart storm: max restarts within a rolling window → stop supervisor.

#include "mlc/concurrency/stop.hpp"

#include <atomic>
#include <chrono>
#include <cstddef>
#include <deque>
#include <functional>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace mlc::concurrency {

enum class RestartPolicy {
  Permanent,  // restart on any exit until supervisor stop
  Transient,  // restart only after abnormal/exception exit
  Temporary   // never restart
};

struct RestartIntensity {
  std::size_t max_restarts = 0; // 0 = unlimited
  std::chrono::milliseconds within{0};
};

class Supervisor {
  struct ChildSpec {
    std::string name;
    RestartPolicy policy = RestartPolicy::Temporary;
    std::function<void(StopToken)> body;
  };

  StopSource stop_;
  std::vector<ChildSpec> children_;
  std::vector<std::thread> running_;
  RestartIntensity intensity_{};
  std::mutex intensity_mutex_;
  std::deque<std::chrono::steady_clock::time_point> restart_times_;
  std::atomic<bool> storm_tripped_{false};
  bool started_ = false;
  bool stopped_ = false;

  // Record a restart attempt. false → intensity exceeded; supervisor stop requested.
  bool allow_restart() {
    if (intensity_.max_restarts == 0) {
      return true;
    }
    const auto now = std::chrono::steady_clock::now();
    std::lock_guard<std::mutex> lock(intensity_mutex_);
    while (!restart_times_.empty() &&
           (now - restart_times_.front()) > intensity_.within) {
      restart_times_.pop_front();
    }
    if (restart_times_.size() >= intensity_.max_restarts) {
      storm_tripped_.store(true);
      stop_.request();
      return false;
    }
    restart_times_.push_back(now);
    return true;
  }

  void run_child_one_for_one(const ChildSpec& child) {
    const StopToken supervisor_token = token();
    while (true) {
      bool abnormal = false;
      try {
        child.body(supervisor_token);
      } catch (...) {
        abnormal = true;
      }
      if (supervisor_token.requested()) {
        break;
      }
      const bool should_restart =
          child.policy == RestartPolicy::Permanent ||
          (child.policy == RestartPolicy::Transient && abnormal);
      if (!should_restart) {
        break;
      }
      if (!allow_restart()) {
        break;
      }
    }
  }

  void join_running() {
    for (auto& worker : running_) {
      if (worker.joinable()) {
        worker.join();
      }
    }
    running_.clear();
  }

public:
  Supervisor() = default;
  Supervisor(const Supervisor&) = delete;
  Supervisor& operator=(const Supervisor&) = delete;
  Supervisor(Supervisor&&) = delete;
  Supervisor& operator=(Supervisor&&) = delete;

  ~Supervisor() { stop(); }

  [[nodiscard]] StopToken token() const noexcept { return stop_.token(); }

  [[nodiscard]] bool cancel_requested() const noexcept { return stop_.requested(); }

  [[nodiscard]] bool storm_tripped() const noexcept { return storm_tripped_.load(); }

  // max_restarts=0 disables intensity (unlimited). Call before start().
  void set_restart_intensity(
      std::size_t max_restarts,
      std::chrono::milliseconds within
  ) {
    if (started_) {
      throw std::logic_error("Supervisor::set_restart_intensity after start");
    }
    if (max_restarts > 0 && within.count() <= 0) {
      throw std::invalid_argument(
          "Supervisor::set_restart_intensity: within must be > 0 when max > 0"
      );
    }
    intensity_.max_restarts = max_restarts;
    intensity_.within = within;
  }

  void add(
      std::string name,
      RestartPolicy policy,
      std::function<void()> body
  ) {
    add(
        std::move(name),
        policy,
        [body = std::move(body)](StopToken) mutable { body(); }
    );
  }

  void add(std::string name, std::function<void()> body) {
    add(std::move(name), RestartPolicy::Temporary, std::move(body));
  }

  void add(
      std::string name,
      RestartPolicy policy,
      std::function<void(StopToken)> body
  ) {
    if (started_) {
      throw std::logic_error("Supervisor::add after start");
    }
    if (name.empty()) {
      throw std::invalid_argument("Supervisor::add: name must be non-empty");
    }
    children_.push_back(
        ChildSpec{std::move(name), policy, std::move(body)}
    );
  }

  void add(std::string name, std::function<void(StopToken)> body) {
    add(std::move(name), RestartPolicy::Temporary, std::move(body));
  }

  [[nodiscard]] std::size_t child_count() const noexcept { return children_.size(); }

  void start() {
    if (started_) {
      throw std::logic_error("Supervisor::start called twice");
    }
    if (stopped_) {
      throw std::logic_error("Supervisor::start after stop");
    }
    started_ = true;
    running_.reserve(children_.size());
    for (const auto& child : children_) {
      running_.emplace_back([this, child]() { run_child_one_for_one(child); });
    }
  }

  // Request cooperative stop, then join all children. Idempotent.
  void stop() {
    if (stopped_) {
      return;
    }
    stopped_ = true;
    stop_.request();
    join_running();
  }
};

} // namespace mlc::concurrency
