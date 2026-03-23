#ifndef TEST_BOUNDS_ONLY_HPP
#define TEST_BOUNDS_ONLY_HPP

#include "mlc.hpp"
#include <variant>


namespace test_bounds_only {

template<typename T>
concept Display = requires(const T& self) { { to_string(T) } -> std::convertible_to<mlc::String>; };
template<typename T>
requires Display<T>
T id(T x) noexcept;
int main(int argc, char** argv) noexcept;

} // namespace test_bounds_only

#endif // TEST_BOUNDS_ONLY_HPP
