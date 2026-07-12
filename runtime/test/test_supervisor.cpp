// Supervisor skeleton smoke (TRACK_CONCURRENCY_SUPERVISOR STEP=1).
// g++ -std=c++20 -pthread -I../include -o test_supervisor test_supervisor.cpp

#include "mlc/concurrency/supervisor.hpp"

#include <atomic>
#include <chrono>
#include <iostream>
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

int main() {
  test_start_stop_runs_children();
  test_destructor_stops_token_child();
  test_explicit_stop_wakes_child();
  if (failed == 0) {
    std::cout << "ALL " << passed << " checks PASSED\n";
  } else {
    std::cout << passed << " passed, " << failed << " FAILED\n";
  }
  return failed > 0 ? 1 : 0;
}
