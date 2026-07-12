// Supervisor smoke (TRACK_CONCURRENCY_SUPERVISOR STEP=1–5).
// g++ -std=c++20 -pthread -I../include -o test_supervisor test_supervisor.cpp

#include "mlc/concurrency/supervisor.hpp"

#include <atomic>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>

static int passed = 0;
static int failed = 0;

#define CHECK(expression)                                                      \
  do {                                                                         \
    if (expression) {                                                          \
      ++passed;                                                                \
    } else {                                                                   \
      ++failed;                                                                \
      std::cerr << "FAIL: " #expression " at line " << __LINE__ << "\n";       \
    }                                                                          \
  } while (0)

void test_start_stop_runs_children() {
  std::atomic<int> counter{0};
  {
    mlc::concurrency::Supervisor supervisor;
    supervisor.add("a", [&] { counter.fetch_add(1); });
    supervisor.add("b", [&] { counter.fetch_add(1); });
    CHECK(supervisor.child_count() == 2);
    supervisor.start();
    supervisor.stop();
  }
  CHECK(counter.load() == 2);
}

void test_destructor_stops_token_child() {
  std::atomic<bool> saw_cancel{false};
  {
    mlc::concurrency::Supervisor supervisor;
    supervisor.add("waiter", [&](mlc::concurrency::StopToken token) {
      while (!token.requested()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }
      saw_cancel.store(true);
    });
    supervisor.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  CHECK(saw_cancel.load());
}

void test_explicit_stop_wakes_child() {
  std::atomic<bool> saw_cancel{false};
  mlc::concurrency::Supervisor supervisor;
  supervisor.add("waiter", [&](mlc::concurrency::StopToken token) {
    while (!token.requested()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    saw_cancel.store(true);
  });
  supervisor.start();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  supervisor.stop();
  CHECK(saw_cancel.load());
}

void test_permanent_restarts_on_clean_exit() {
  std::atomic<int> runs{0};
  mlc::concurrency::Supervisor supervisor;
  supervisor.add(
      "loop",
      mlc::concurrency::RestartPolicy::Permanent,
      [&] {
        runs.fetch_add(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
      }
  );
  supervisor.start();
  std::this_thread::sleep_for(std::chrono::milliseconds(40));
  supervisor.stop();
  CHECK(runs.load() >= 3);
}

void test_temporary_does_not_restart() {
  std::atomic<int> runs{0};
  mlc::concurrency::Supervisor supervisor;
  supervisor.add(
      "once",
      mlc::concurrency::RestartPolicy::Temporary,
      [&] { runs.fetch_add(1); }
  );
  supervisor.start();
  std::this_thread::sleep_for(std::chrono::milliseconds(30));
  supervisor.stop();
  CHECK(runs.load() == 1);
}

void test_transient_restarts_only_on_exception() {
  std::atomic<int> runs{0};
  std::atomic<int> sibling_runs{0};
  mlc::concurrency::Supervisor supervisor;
  supervisor.add(
      "flaky",
      mlc::concurrency::RestartPolicy::Transient,
      [&] {
        const int attempt = runs.fetch_add(1) + 1;
        if (attempt < 3) {
          throw std::runtime_error("boom");
        }
      }
  );
  supervisor.add(
      "sibling",
      mlc::concurrency::RestartPolicy::Temporary,
      [&] {
        sibling_runs.fetch_add(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
      }
  );
  supervisor.start();
  std::this_thread::sleep_for(std::chrono::milliseconds(80));
  supervisor.stop();
  CHECK(runs.load() == 3);
  CHECK(sibling_runs.load() == 1);
}

void test_transient_no_restart_on_clean_exit() {
  std::atomic<int> runs{0};
  mlc::concurrency::Supervisor supervisor;
  supervisor.add(
      "clean",
      mlc::concurrency::RestartPolicy::Transient,
      [&] { runs.fetch_add(1); }
  );
  supervisor.start();
  std::this_thread::sleep_for(std::chrono::milliseconds(30));
  supervisor.stop();
  CHECK(runs.load() == 1);
}

void test_storm_trips_permanent_spinner() {
  std::atomic<int> runs{0};
  mlc::concurrency::Supervisor supervisor;
  supervisor.set_restart_intensity(3, std::chrono::milliseconds(1000));
  supervisor.add(
      "spin",
      mlc::concurrency::RestartPolicy::Permanent,
      [&] { runs.fetch_add(1); }
  );
  supervisor.start();
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  CHECK(supervisor.storm_tripped());
  CHECK(supervisor.cancel_requested());
  const int total = runs.load();
  CHECK(total >= 4);
  CHECK(total <= 5);
  supervisor.stop();
  CHECK(runs.load() == total);
}

void test_storm_stops_sibling() {
  std::atomic<int> spinner_runs{0};
  std::atomic<bool> sibling_saw_cancel{false};
  mlc::concurrency::Supervisor supervisor;
  supervisor.set_restart_intensity(2, std::chrono::milliseconds(1000));
  supervisor.add(
      "spin",
      mlc::concurrency::RestartPolicy::Permanent,
      [&] { spinner_runs.fetch_add(1); }
  );
  supervisor.add(
      "sibling",
      mlc::concurrency::RestartPolicy::Temporary,
      [&](mlc::concurrency::StopToken token) {
        while (!token.requested()) {
          std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        sibling_saw_cancel.store(true);
      }
  );
  supervisor.start();
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  CHECK(supervisor.storm_tripped());
  supervisor.stop();
  CHECK(sibling_saw_cancel.load());
  CHECK(spinner_runs.load() >= 3);
}

// STEP=5: three children — flaky transient (2 throws → success); siblings once each.
void test_three_children_transient_sibling_isolation() {
  std::atomic<int> flaky_runs{0};
  std::atomic<int> sibling_a_runs{0};
  std::atomic<int> sibling_b_runs{0};
  mlc::concurrency::Supervisor supervisor;
  supervisor.add(
      "flaky",
      mlc::concurrency::RestartPolicy::Transient,
      [&] {
        const int attempt = flaky_runs.fetch_add(1) + 1;
        if (attempt < 3) {
          throw std::runtime_error("panic");
        }
      }
  );
  supervisor.add(
      "sibling_a",
      mlc::concurrency::RestartPolicy::Temporary,
      [&] {
        sibling_a_runs.fetch_add(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
      }
  );
  supervisor.add(
      "sibling_b",
      mlc::concurrency::RestartPolicy::Temporary,
      [&] {
        sibling_b_runs.fetch_add(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
      }
  );
  CHECK(supervisor.child_count() == 3);
  supervisor.start();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  supervisor.stop();
  CHECK(flaky_runs.load() == 3);
  CHECK(sibling_a_runs.load() == 1);
  CHECK(sibling_b_runs.load() == 1);
}

// STEP=5: permanent always-fails → storm after max/within.
void test_permanent_always_fails_trips_storm() {
  std::atomic<int> runs{0};
  mlc::concurrency::Supervisor supervisor;
  supervisor.set_restart_intensity(2, std::chrono::milliseconds(500));
  supervisor.add(
      "always_fail",
      mlc::concurrency::RestartPolicy::Permanent,
      [&] {
        runs.fetch_add(1);
        throw std::runtime_error("fail");
      }
  );
  supervisor.start();
  std::this_thread::sleep_for(std::chrono::milliseconds(80));
  CHECK(supervisor.storm_tripped());
  CHECK(supervisor.cancel_requested());
  const int total = runs.load();
  CHECK(total >= 3);
  CHECK(total <= 4);
  supervisor.stop();
  CHECK(runs.load() == total);
}

int main() {
  test_start_stop_runs_children();
  test_destructor_stops_token_child();
  test_explicit_stop_wakes_child();
  test_permanent_restarts_on_clean_exit();
  test_temporary_does_not_restart();
  test_transient_restarts_only_on_exception();
  test_transient_no_restart_on_clean_exit();
  test_storm_trips_permanent_spinner();
  test_storm_stops_sibling();
  test_three_children_transient_sibling_isolation();
  test_permanent_always_fails_trips_storm();
  if (failed == 0) {
    std::cout << "ALL " << passed << " checks PASSED\n";
  } else {
    std::cout << passed << " passed, " << failed << " FAILED\n";
  }
  return failed > 0 ? 1 : 0;
}
