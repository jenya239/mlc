#ifndef TEST_BOUNDS_ONLY_HPP
#define TEST_BOUNDS_ONLY_HPP

#include "mlc.hpp"
#include <variant>


namespace test_bounds_only {

template<typename T>
concept Display = requires(const T& self) { { to_string(T) } -> std::convertible_to<mlc::String>; };
T id(void ) noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
void main(void ) noexcept;

} // namespace test_bounds_only

#endif // TEST_BOUNDS_ONLY_HPP
