#pragma once

// TRACK_MLC_SCRIPT_VM §103i — pointer helpers for MLC embedding load/call.
#include "mlc/core/string.hpp"

#include <cstdint>
#include <cstring>

namespace mlc {
namespace script_vm {

inline std::int32_t embedding_u8_at(std::int64_t data_pointer, std::int32_t index) {
  if (data_pointer == 0 || index < 0) {
    return 0;
  }
  const auto* bytes = reinterpret_cast<const std::uint8_t*>(data_pointer);
  return static_cast<std::int32_t>(bytes[static_cast<std::size_t>(index)]);
}

inline std::int64_t embedding_i64_at(std::int64_t data_pointer, std::int32_t index) {
  if (data_pointer == 0 || index < 0) {
    return 0;
  }
  const auto* values = reinterpret_cast<const std::int64_t*>(data_pointer);
  return values[static_cast<std::size_t>(index)];
}

inline std::int32_t embedding_i64_store(std::int64_t data_pointer, std::int64_t value) {
  if (data_pointer == 0) {
    return 0;
  }
  auto* destination = reinterpret_cast<std::int64_t*>(data_pointer);
  *destination = value;
  return 0;
}

inline std::int32_t embedding_i64_byte(std::int64_t value, std::int32_t byte_index) {
  if (byte_index < 0 || byte_index > 7) {
    return 0;
  }
  const auto shifted =
      static_cast<std::uint64_t>(value) >> static_cast<std::uint32_t>(byte_index * 8);
  return static_cast<std::int32_t>(shifted & 0xffu);
}

inline std::int64_t embedding_u8_to_i64(std::int32_t byte_value) {
  return static_cast<std::int64_t>(static_cast<std::uint8_t>(byte_value & 0xff));
}

inline String embedding_string_from_byte(std::int32_t value) {
  const char byte = static_cast<char>(static_cast<unsigned char>(value & 0xff));
  return String(&byte, 1);
}

inline std::int32_t embedding_string_byte(String text, std::int32_t index) {
  if (index < 0 || static_cast<std::size_t>(index) >= text.size()) {
    return -1;
  }
  return static_cast<std::int32_t>(
      static_cast<unsigned char>(text.view().data()[static_cast<std::size_t>(index)]));
}

} // namespace script_vm
} // namespace mlc
