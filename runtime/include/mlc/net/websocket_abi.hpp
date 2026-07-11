#pragma once

// Byte helpers for TRACK_STDLIB_WEBSOCKET_TO_MLC STEP=2 (no libsodium).

#include "mlc/core/string.hpp"

#include <cstdint>

namespace mlc {
namespace net {

inline std::int32_t string_byte_u8(String text, std::int32_t index) {
  if (index < 0 || static_cast<std::size_t>(index) >= text.size()) {
    return -1;
  }
  return static_cast<std::int32_t>(
      static_cast<unsigned char>(text.view().data()[static_cast<std::size_t>(index)]));
}

inline String string_from_byte_u8(std::int32_t value) {
  const char byte = static_cast<char>(static_cast<unsigned char>(value & 0xff));
  return String(&byte, 1);
}

} // namespace net
} // namespace mlc
