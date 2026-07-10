#ifndef MLC_ARITH_HPP
#define MLC_ARITH_HPP

#include <limits>
#include <type_traits>
#include "mlc/core/string.hpp"
#include "mlc/io/io.hpp"

namespace mlc {
namespace arith {

// Signed + / - / *: debug panic on overflow; release = raw C++ (UB on signed overflow).
// Compile-time evaluation uses raw ops (C++ constexpr already rejects signed overflow).

template <typename Integer>
constexpr Integer checked_add(Integer left, Integer right) {
  if (std::is_constant_evaluated()) {
    return left + right;
  }
#ifndef NDEBUG
  Integer result{};
  if (__builtin_add_overflow(left, right, &result)) {
    mlc::io::panic(String("integer overflow"));
  }
  return result;
#else
  return left + right;
#endif
}

template <typename Integer>
constexpr Integer checked_sub(Integer left, Integer right) {
  if (std::is_constant_evaluated()) {
    return left - right;
  }
#ifndef NDEBUG
  Integer result{};
  if (__builtin_sub_overflow(left, right, &result)) {
    mlc::io::panic(String("integer overflow"));
  }
  return result;
#else
  return left - right;
#endif
}

template <typename Integer>
constexpr Integer checked_mul(Integer left, Integer right) {
  if (std::is_constant_evaluated()) {
    return left * right;
  }
#ifndef NDEBUG
  Integer result{};
  if (__builtin_mul_overflow(left, right, &result)) {
    mlc::io::panic(String("integer overflow"));
  }
  return result;
#else
  return left * right;
#endif
}

// / and %: panic on divisor 0 in all builds. Signed MIN/-1: debug panic / release UB.

template <typename Integer>
constexpr Integer checked_div(Integer left, Integer right) {
  if (!std::is_constant_evaluated()) {
    if (right == Integer{0}) {
      mlc::io::panic(String("division by zero"));
    }
#ifndef NDEBUG
    if constexpr (std::is_signed_v<Integer>) {
      if (left == std::numeric_limits<Integer>::min() && right == Integer{-1}) {
        mlc::io::panic(String("integer overflow"));
      }
    }
#endif
  }
  return left / right;
}

template <typename Integer>
constexpr Integer checked_mod(Integer left, Integer right) {
  if (!std::is_constant_evaluated()) {
    if (right == Integer{0}) {
      mlc::io::panic(String("division by zero"));
    }
#ifndef NDEBUG
    if constexpr (std::is_signed_v<Integer>) {
      if (left == std::numeric_limits<Integer>::min() && right == Integer{-1}) {
        mlc::io::panic(String("integer overflow"));
      }
    }
#endif
  }
  return left % right;
}

} // namespace arith
} // namespace mlc

#endif // MLC_ARITH_HPP
