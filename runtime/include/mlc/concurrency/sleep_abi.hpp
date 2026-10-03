#pragma once

// FFI result is int32_t. A negative count sleeps zero. A count above 60000
// sleeps 60000. The call blocks the calling thread.

#include <chrono>
#include <cstdint>
#include <thread>

namespace mlc {
namespace sleep_abi {

inline std::int32_t sleep_milliseconds(std::int32_t milliseconds) {
  std::int32_t bounded = milliseconds;
  if (bounded < 0) bounded = 0;
  if (bounded > 60000) bounded = 60000;
  if (bounded > 0) {
    std::this_thread::sleep_for(std::chrono::milliseconds(bounded));
  }
  return 0;
}

}  // namespace sleep_abi
}  // namespace mlc
