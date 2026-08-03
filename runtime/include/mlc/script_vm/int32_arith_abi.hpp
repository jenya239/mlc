#pragma once
// TRACK_MLC_SCRIPT_VM §103d — checked i32 arithmetic for script_vm interpreter.
#include <cstdint>
#include <limits>

namespace mlc {
namespace script_vm {

inline thread_local int32_t g_int32_arith_ok = 1;

inline int32_t int32_arith_ok() noexcept { return g_int32_arith_ok; }

inline int32_t int32_checked_add(int32_t left, int32_t right) noexcept {
  const int64_t sum = static_cast<int64_t>(left) + static_cast<int64_t>(right);
  if (sum < static_cast<int64_t>(std::numeric_limits<int32_t>::min()) ||
      sum > static_cast<int64_t>(std::numeric_limits<int32_t>::max())) {
    g_int32_arith_ok = 0;
    return 0;
  }
  g_int32_arith_ok = 1;
  return static_cast<int32_t>(sum);
}

inline int32_t int32_checked_sub(int32_t left, int32_t right) noexcept {
  const int64_t difference = static_cast<int64_t>(left) - static_cast<int64_t>(right);
  if (difference < static_cast<int64_t>(std::numeric_limits<int32_t>::min()) ||
      difference > static_cast<int64_t>(std::numeric_limits<int32_t>::max())) {
    g_int32_arith_ok = 0;
    return 0;
  }
  g_int32_arith_ok = 1;
  return static_cast<int32_t>(difference);
}

inline int32_t int32_checked_mul(int32_t left, int32_t right) noexcept {
  const int64_t product = static_cast<int64_t>(left) * static_cast<int64_t>(right);
  if (product < static_cast<int64_t>(std::numeric_limits<int32_t>::min()) ||
      product > static_cast<int64_t>(std::numeric_limits<int32_t>::max())) {
    g_int32_arith_ok = 0;
    return 0;
  }
  g_int32_arith_ok = 1;
  return static_cast<int32_t>(product);
}

}  // namespace script_vm
}  // namespace mlc
