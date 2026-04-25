// Free functions for Result<T,E> instance-style chaining (A2). Uses std::variant<Ok<T>,Err<E>>.

#ifndef MLC_RESULT_COMBINATORS_HPP
#define MLC_RESULT_COMBINATORS_HPP

#include <optional>
#include <type_traits>
#include <utility>
#include <variant>

#include "mlc/core/result.hpp"

namespace mlc::result {

template<typename T, typename E, typename F>
auto map(const Result<T, E>& r, F&& f) {
  using U = std::invoke_result_t<F, const T&>;
  if (const auto* ok = std::get_if<0>(&r)) {
    return Result<U, E>{ Ok<U>{ f(ok->_0) } };
  }
  return Result<U, E>{ std::get<1>(r) };
}

template<typename T, typename E, typename F>
auto map_err(const Result<T, E>& r, F&& f) {
  using F2 = std::invoke_result_t<F, const E&>;
  if (const auto* e = std::get_if<1>(&r)) {
    return Result<T, F2>{ Err<F2>{ f(e->_0) } };
  }
  return Result<T, F2>{ std::get<0>(r) };
}

template<typename T, typename E, typename F>
auto and_then(const Result<T, E>& r, F&& f) -> std::invoke_result_t<F, const T&> {
  if (const auto* ok = std::get_if<0>(&r)) {
    return f(ok->_0);
  }
  using Res = std::invoke_result_t<F, const T&>;
  return Res{ std::get<1>(r) };
}

template<typename T, typename E, typename F>
auto or_else(const Result<T, E>& r, F&& f) -> std::invoke_result_t<F, const E&> {
  if (const auto* ok = std::get_if<0>(&r)) {
    using Res = std::invoke_result_t<F, const E&>;
    return Res{ *ok };
  }
  return f(std::get<1>(r)._0);
}

template<typename T, typename E>
T unwrap_or(const Result<T, E>& r, const T& d) {
  if (const auto* ok = std::get_if<0>(&r)) {
    return ok->_0;
  }
  return d;
}

template<typename T, typename E, typename F>
T unwrap_or_else(const Result<T, E>& r, F&& f) {
  if (const auto* ok = std::get_if<0>(&r)) {
    return ok->_0;
  }
  return f(std::get<1>(r)._0);
}

template<typename T, typename E>
std::optional<T> ok_into_optional(const Result<T, E>& r) {
  if (const auto* ok = std::get_if<0>(&r)) {
    return ok->_0;
  }
  return std::nullopt;
}

}  // namespace mlc::result

#endif
