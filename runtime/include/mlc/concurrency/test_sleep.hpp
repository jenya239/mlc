#pragma once

// Test-only sleep for concurrency wall-time smokes (TRACK_CONCURRENCY_SPAWN_DETACH).

#include <chrono>
#include <cstdint>
#include <thread>

namespace mlc::concurrency {

inline int32_t test_sleep_ms(int32_t milliseconds) {
  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
  return 0;
}

} // namespace mlc::concurrency
