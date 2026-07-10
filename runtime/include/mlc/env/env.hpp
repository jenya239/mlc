#pragma once

// Process environment helpers (TRACK_STDLIB_ENV_LOGGING STEP=2).
// getenv wrappers: get / get_or / has. Missing key is not an error.

#include "mlc/core/string.hpp"

#include <cstdlib>
#include <optional>

namespace mlc {
namespace env {

inline std::optional<String> get(const String& key) {
  const char* value = std::getenv(key.c_str());
  if (value == nullptr) {
    return std::nullopt;
  }
  return String(value);
}

inline String get_or(const String& key, const String& default_value) {
  std::optional<String> value = get(key);
  if (value.has_value()) {
    return *value;
  }
  return default_value;
}

inline bool has(const String& key) {
  return std::getenv(key.c_str()) != nullptr;
}

} // namespace env
} // namespace mlc
