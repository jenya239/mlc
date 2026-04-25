// Free functions for std::optional<T> (MLC Option<T> in C++) — A2 combinator chains.

#ifndef MLC_OPTIONAL_COMBINATORS_HPP
#define MLC_OPTIONAL_COMBINATORS_HPP

#include <optional>
#include <type_traits>
#include <utility>
#include <variant>

#include "mlc/core/result.hpp"

namespace mlc::opt {

template<typename T, typename F>
auto map(std::optional<T> o, F&& f) {
  if (!o) {
    return std::optional<std::invoke_result_t<F, T&>>{};
  }
  return std::optional<std::invoke_result_t<F, T&>>{ f(*o) };
}

template<typename T, typename F>
auto and_then(std::optional<T> o, F&& f) -> std::invoke_result_t<F, T&> {
  if (!o) {
    using R = std::invoke_result_t<F, T&>;
    return R{};
  }
  return f(*o);
}

template<typename T, typename F>
std::optional<T> or_else(std::optional<T> o, F&& f) {
  if (o) {
    return o;
  }
  return f();
}

template<typename T, typename D>
T unwrap_or(std::optional<T> o, D&& d) {
  if (o) {
    return *o;
  }
  return std::forward<D>(d);
}

template<typename T, typename P>
std::optional<T> filter(std::optional<T> o, P&& p) {
  if (!o) {
    return o;
  }
  if (p(*o)) {
    return o;
  }
  return std::nullopt;
}

template<typename T, typename E>
mlc::result::Result<T, E> ok_or(std::optional<T> o, E err_val) {
  using namespace mlc::result;
  if (o) {
    return Result<T, E>{ Ok<T>{ *o } };
  }
  return Result<T, E>{ Err<E>{ std::move(err_val) } };
}

}  // namespace mlc::opt

#endif
