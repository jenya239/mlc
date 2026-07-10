#ifndef MLC_INT_ARITH_HPP
#define MLC_INT_ARITH_HPP

#include <cstdint>
#include <limits>
#include <optional>
#include <type_traits>

namespace mlc {
namespace int_arith {

// Explicit integer ops (stdlib). Separate from mlc::arith operator helpers.

template <typename Integer>
constexpr Integer wrapping_add(Integer left, Integer right) {
  using Unsigned = std::make_unsigned_t<Integer>;
  return static_cast<Integer>(static_cast<Unsigned>(left) + static_cast<Unsigned>(right));
}

template <typename Integer>
constexpr Integer wrapping_sub(Integer left, Integer right) {
  using Unsigned = std::make_unsigned_t<Integer>;
  return static_cast<Integer>(static_cast<Unsigned>(left) - static_cast<Unsigned>(right));
}

template <typename Integer>
constexpr Integer wrapping_mul(Integer left, Integer right) {
  using Unsigned = std::make_unsigned_t<Integer>;
  return static_cast<Integer>(static_cast<Unsigned>(left) * static_cast<Unsigned>(right));
}

template <typename Integer>
constexpr Integer saturating_add(Integer left, Integer right) {
  Integer result{};
  if (__builtin_add_overflow(left, right, &result)) {
    if constexpr (std::is_signed_v<Integer>) {
      return (right > Integer{0}) ? std::numeric_limits<Integer>::max()
                                  : std::numeric_limits<Integer>::min();
    } else {
      return std::numeric_limits<Integer>::max();
    }
  }
  return result;
}

template <typename Integer>
constexpr Integer saturating_sub(Integer left, Integer right) {
  Integer result{};
  if (__builtin_sub_overflow(left, right, &result)) {
    if constexpr (std::is_signed_v<Integer>) {
      return (right > Integer{0}) ? std::numeric_limits<Integer>::min()
                                  : std::numeric_limits<Integer>::max();
    } else {
      return Integer{0};
    }
  }
  return result;
}

template <typename Integer>
constexpr Integer saturating_mul(Integer left, Integer right) {
  Integer result{};
  if (__builtin_mul_overflow(left, right, &result)) {
    if constexpr (std::is_signed_v<Integer>) {
      const bool negative = (left < Integer{0}) != (right < Integer{0});
      return negative ? std::numeric_limits<Integer>::min()
                      : std::numeric_limits<Integer>::max();
    } else {
      return std::numeric_limits<Integer>::max();
    }
  }
  return result;
}

template <typename Integer>
constexpr std::optional<Integer> checked_add(Integer left, Integer right) {
  Integer result{};
  if (__builtin_add_overflow(left, right, &result)) {
    return std::nullopt;
  }
  return result;
}

template <typename Integer>
constexpr std::optional<Integer> checked_sub(Integer left, Integer right) {
  Integer result{};
  if (__builtin_sub_overflow(left, right, &result)) {
    return std::nullopt;
  }
  return result;
}

template <typename Integer>
constexpr std::optional<Integer> checked_mul(Integer left, Integer right) {
  Integer result{};
  if (__builtin_mul_overflow(left, right, &result)) {
    return std::nullopt;
  }
  return result;
}

} // namespace int_arith
} // namespace mlc

#endif // MLC_INT_ARITH_HPP
