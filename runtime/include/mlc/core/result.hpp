#ifndef MLC_RESULT_HPP
#define MLC_RESULT_HPP

#include <variant>
#include <memory>

namespace mlc {
namespace result {

template<typename T>
struct Ok {
    T _0;
};

template<typename E>
struct Err {
    E _0;
};

template<typename T, typename E>
using Result = std::variant<Ok<T>, Err<E>>;

template<typename T>
Ok<T> ok(T value) {
    return Ok<T>{value};
}

template<typename E>
Err<E> err(E error) {
    return Err<E>{error};
}

}  // namespace result
}  // namespace mlc

#endif  // MLC_RESULT_HPP
