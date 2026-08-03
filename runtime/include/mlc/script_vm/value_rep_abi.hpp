#pragma once

// TRACK_MLC_SCRIPT_VM §103a — NaN-box ValueRep helpers (header-only).
// Quiet-NaN payload space for Nil / Bool / Int32; IEEE f64 bits otherwise.
// Layout: high 16 bits = 0x7FFC (box marker); bits 32-47 = tag; bits 0-31 = payload.

#include <cstdint>
#include <cstring>

namespace mlc {
namespace script_vm {

inline constexpr std::int64_t k_box_mask = 0xFFFF000000000000LL;
inline constexpr std::int64_t k_box_base = 0x7FFC000000000000LL;
inline constexpr std::int64_t k_tag_shift = 32;
inline constexpr std::int64_t k_tag_nil = 1;
inline constexpr std::int64_t k_tag_bool = 2;
inline constexpr std::int64_t k_tag_int32 = 3;
inline constexpr std::int64_t k_payload_mask = 0x00000000FFFFFFFFLL;
// Canonical quiet-NaN outside our box prefix (float NaN collision escape).
inline constexpr std::int64_t k_float_nan_escape = 0x7FF8000000000001LL;

inline bool is_boxed(std::int64_t raw) {
  return (raw & k_box_mask) == k_box_base;
}

inline std::int64_t encode_nil() {
  return k_box_base | (k_tag_nil << k_tag_shift);
}

inline std::int64_t encode_bool(std::int32_t flag) {
  return k_box_base | (k_tag_bool << k_tag_shift) | (flag != 0 ? 1LL : 0LL);
}

inline std::int64_t encode_int32(std::int32_t value) {
  return k_box_base | (k_tag_int32 << k_tag_shift) |
         static_cast<std::int64_t>(static_cast<std::uint32_t>(value));
}

inline std::int64_t encode_float64(double value) {
  std::int64_t bits = 0;
  std::memcpy(&bits, &value, sizeof(bits));
  if (is_boxed(bits)) {
    bits = k_float_nan_escape;
  }
  return bits;
}

// 0 = Float64, 1 = Nil, 2 = Bool, 3 = Int32
inline std::int32_t decode_kind(std::int64_t raw) {
  if (!is_boxed(raw)) {
    return 0;
  }
  const std::int64_t tag = (raw >> k_tag_shift) & 0xFFFFLL;
  if (tag == k_tag_nil) {
    return 1;
  }
  if (tag == k_tag_bool) {
    return 2;
  }
  if (tag == k_tag_int32) {
    return 3;
  }
  return 0;
}

inline std::int32_t decode_bool(std::int64_t raw) {
  return (raw & 1LL) != 0 ? 1 : 0;
}

inline std::int32_t decode_int32(std::int64_t raw) {
  return static_cast<std::int32_t>(static_cast<std::uint32_t>(raw & k_payload_mask));
}

inline double decode_float64(std::int64_t raw) {
  double value = 0.0;
  std::memcpy(&value, &raw, sizeof(value));
  return value;
}

inline std::int32_t value_byte_size() { return 8; }

}  // namespace script_vm
}  // namespace mlc
