#ifndef TEST_BOUNDED_HPP
#define TEST_BOUNDED_HPP

#include "mlc.hpp"
#include <variant>


namespace test_bounded {

template<typename T>
concept Display = requires(const T& self) { { to_string(T) } -> std::convertible_to<mlc::String>; };
void i32_to_string(i32 self, void ) noexcept;
x i32_print_display(i32 self, Display T) noexcept;
int i32_main(i32 self) noexcept;

} // namespace test_bounded

#endif // TEST_BOUNDED_HPP
