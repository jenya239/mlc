#pragma once

// Supervisor (TRACK_CONCURRENCY_SUPERVISOR STEP=1–2).
// Register children, start all, stop all via StopToken + join.
// Restart policies: permanent / transient / temporary (one_for_one).
// Restart storm limits: STEP=3 (not here).

#include "mlc/concurrency/stop.hpp"

#include <functional>
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

class Supervisor {
  struct ChildSpec {
    std::string name;
    RestartPolicy policy = RestartPolicy::Temporary;
    std::function<void(StopToken)> body;
  };

  StopSource stop_;
  std::vector<ChildSpec> children_;
  std::vector<std::thread> running_;
  bool started_ = false;
  bool stopped_ = false;

  static void run_child_one_for_one(
      const ChildSpec& child,
      StopToken supervisor_token
  ) {
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

  // Launch every registered child (one thread each; one_for_one restart loops).
  void start() {
    if (started_) {
      throw std::logic_error("Supervisor::start called twice");
    }
    if (stopped_) {
      throw std::logic_error("Supervisor::start after stop");
    }
    started_ = true;
    running_.reserve(children_.size());
    StopToken child_token = token();
    for (const auto& child : children_) {
      running_.emplace_back(
          [child, child_token]() { run_child_one_for_one(child, child_token); }
      );
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
