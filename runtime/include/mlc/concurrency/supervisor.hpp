#pragma once

// Supervisor skeleton (TRACK_CONCURRENCY_SUPERVISOR STEP=1).
// Register children, start all, stop all via StopToken + join.
// Restart policies / storm limits: STEP=2–3 (not here).

#include "mlc/concurrency/stop.hpp"

#include <functional>
#include <future>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace mlc::concurrency {

class Supervisor {
  struct ChildSpec {
    std::string name;
    std::function<void(StopToken)> body;
  };

  StopSource stop_;
  std::vector<ChildSpec> children_;
  std::vector<std::future<void>> running_;
  bool started_ = false;
  bool stopped_ = false;

  void join_running() {
    for (auto& child : running_) {
      if (child.valid()) {
        child.get();
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

  // Register a child that ignores the stop token.
  void add(std::string name, std::function<void()> body) {
    if (started_) {
      throw std::logic_error("Supervisor::add after start");
    }
    add(
        std::move(name),
        [body = std::move(body)](StopToken) mutable { body(); }
    );
  }

  // Register a child that receives the supervisor StopToken.
  void add(std::string name, std::function<void(StopToken)> body) {
    if (started_) {
      throw std::logic_error("Supervisor::add after start");
    }
    if (name.empty()) {
      throw std::invalid_argument("Supervisor::add: name must be non-empty");
    }
    children_.push_back(ChildSpec{std::move(name), std::move(body)});
  }

  [[nodiscard]] std::size_t child_count() const noexcept { return children_.size(); }

  // Launch every registered child (std::async, same style as TaskScope).
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
    for (auto& child : children_) {
      running_.push_back(std::async(
          std::launch::async,
          [body = child.body, child_token]() mutable { body(child_token); }
      ));
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
