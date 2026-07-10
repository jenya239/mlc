// JobQueue smoke (TRACK_STDLIB_JOB_QUEUE STEP=2).
// g++ -std=c++20 -pthread -I../include -o test_job_queue test_job_queue.cpp && ./test_job_queue

#include "mlc/concurrency/job_queue.hpp"

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

static int passed = 0;
static int failed = 0;

#define CHECK(expression) do { \
    if (expression) { ++passed; } \
    else { ++failed; std::cerr << "FAIL: " #expression " at line " << __LINE__ << "\n"; } \
} while (0)

void test_enqueue_n_jobs() {
  std::atomic<int> counter{0};
  {
    mlc::concurrency::JobQueue queue(4, 32, 1);
    for (int index = 0; index < 40; ++index) {
      CHECK(queue.enqueue([&] { counter.fetch_add(1); }));
    }
    queue.wait_idle();
    CHECK(counter.load() == 40);
    CHECK(queue.pending_count() == 0);
    queue.shutdown();
  }
}

void test_schedule_after_runs_later() {
  std::atomic<int> counter{0};
  const auto start = std::chrono::steady_clock::now();
  {
    mlc::concurrency::JobQueue queue(2, 8, 1);
    CHECK(queue.schedule_after(50, [&] { counter.fetch_add(1); }));
    queue.wait_idle();
    queue.shutdown();
  }
  const auto elapsed = std::chrono::steady_clock::now() - start;
  CHECK(counter.load() == 1);
  CHECK(elapsed >= std::chrono::milliseconds(40));
}

void test_retry_on_exception() {
  std::atomic<int> attempts{0};
  std::atomic<int> successes{0};
  {
    mlc::concurrency::JobQueue queue(2, 8, 3);
    CHECK(queue.enqueue([&] {
      const int current = attempts.fetch_add(1) + 1;
      if (current < 3) {
        throw std::runtime_error("retry");
      }
      successes.fetch_add(1);
    }));
    queue.wait_idle();
    queue.shutdown();
  }
  CHECK(attempts.load() == 3);
  CHECK(successes.load() == 1);
}

void test_shutdown_drops_delayed() {
  std::atomic<int> counter{0};
  {
    mlc::concurrency::JobQueue queue(2, 8, 1);
    CHECK(queue.schedule_after(5000, [&] { counter.fetch_add(1); }));
    queue.shutdown();
  }
  CHECK(counter.load() == 0);
}

int main() {
  test_enqueue_n_jobs();
  test_schedule_after_runs_later();
  test_retry_on_exception();
  test_shutdown_drops_delayed();
  std::cout << "job_queue smoke: " << passed << " passed, " << failed << " failed\n";
  return failed == 0 ? 0 : 1;
}
