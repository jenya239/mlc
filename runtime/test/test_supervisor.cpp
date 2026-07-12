// Supervisor smoke (TRACK_CONCURRENCY_SUPERVISOR STEP=1–2).
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

int main() {
  test_start_stop_runs_children();
  test_destructor_stops_token_child();
  test_explicit_stop_wakes_child();
  test_permanent_restarts_on_clean_exit();
  test_temporary_does_not_restart();
  test_transient_restarts_only_on_exception();
  test_transient_no_restart_on_clean_exit();
  if (failed == 0) {
    std::cout << "ALL " << passed << " checks PASSED\n";
  } else {
    std::cout << passed << " passed, " << failed << " FAILED\n";
  }
  return failed > 0 ? 1 : 0;
}
