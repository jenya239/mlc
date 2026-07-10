// JobQueue demo (TRACK_STDLIB_JOB_QUEUE).
// C++-only v1 — no MLC module. Gate: scripts/run_job_queue_gate.sh
//
// g++ -std=c++20 -pthread -I../../runtime/include -o job_queue_demo \
//   job_queue_demo.cpp && ./job_queue_demo

#include "mlc/concurrency/job_queue.hpp"

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

int main() {
  std::atomic<int> counter{0};
  mlc::concurrency::JobQueue queue(2, 16, /*max_attempts=*/2);

  for (int index = 0; index < 8; ++index) {
    if (!queue.enqueue([&] { counter.fetch_add(1); })) {
      std::cerr << "enqueue failed\n";
      return 1;
    }
  }
  if (!queue.schedule_after(30, [&] { counter.fetch_add(10); })) {
    std::cerr << "schedule_after failed\n";
    return 2;
  }

  queue.wait_idle();
  queue.shutdown();

  const int total = counter.load();
  std::cout << "job_queue_demo counter=" << total << "\n";
  return total == 18 ? 0 : 3;
}
