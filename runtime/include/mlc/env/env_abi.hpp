#pragma once

// Thin getenv FFI (TRACK_STDLIB_LOGIC_TO_MLC STEP=2).
// Replaces env.hpp — public MLC API stays Env.get / get_or / has.
// By-value String matches mlcc FFI binder (same as tcp_bridge _mlc).

#include "mlc/core/string.hpp"

#include <cstdlib>
#include <optional>

namespace mlc {
namespace env {

inline std::optional<String> get(String key) {
  const char* value = std::getenv(key.c_str());
  if (value == nullptr) {
    return std::nullopt;
  }
  return String(value);
}

inline String get_or(String key, String default_value) {
  std::optional<String> value = get(key);
  if (value.has_value()) {
    return *value;
  }
  return default_value;
}

inline bool has(String key) {
  return std::getenv(key.c_str()) != nullptr;
}

} // namespace env
} // namespace mlc
