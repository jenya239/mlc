#pragma once

// Validation helpers (TRACK_STDLIB_VALIDATION STEP=2).
// Explicit checks; Result<unit, string> first-failure messages.
// Pure-MLC stdlib bodies are not inlined by Ruby codegen — C++ implements.

#include "mlc/core/result.hpp"
#include "mlc/core/string.hpp"

#include <tuple>

namespace mlc {
namespace validate {

using Unit = std::tuple<>;
using ValidateResult = mlc::result::Result<Unit, String>;

inline ValidateResult non_empty(const String& value) {
  if (!value.is_empty()) {
    return mlc::result::ok(Unit{});
  }
  return mlc::result::err(String("must be non-empty"));
}

inline ValidateResult min_length(const String& value, std::int32_t minimum) {
  if (value.byte_size() >= minimum) {
    return mlc::result::ok(Unit{});
  }
  return mlc::result::err(String("length below minimum"));
}

inline ValidateResult max_length(const String& value, std::int32_t maximum) {
  if (value.byte_size() <= maximum) {
    return mlc::result::ok(Unit{});
  }
  return mlc::result::err(String("length above maximum"));
}

inline ValidateResult range_i32(std::int32_t value, std::int32_t minimum, std::int32_t maximum) {
  if (value >= minimum && value <= maximum) {
    return mlc::result::ok(Unit{});
  }
  return mlc::result::err(String("value out of range"));
}

} // namespace validate
} // namespace mlc
