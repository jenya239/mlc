#ifndef TEST_TRAIT_IMPL_HPP
#define TEST_TRAIT_IMPL_HPP

#include "mlc.hpp"
#include <variant>


namespace test_trait_impl {

template<typename T>
concept Display = requires(const T& self) { { to_string(T) } -> std::convertible_to<mlc::String>; };
void i32_to_string(i32 self, void ) noexcept;
void main(void ) noexcept;

} // namespace test_trait_impl

#endif // TEST_TRAIT_IMPL_HPP
