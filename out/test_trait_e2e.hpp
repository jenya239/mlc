#ifndef TEST_TRAIT_E2E_HPP
#define TEST_TRAIT_E2E_HPP

#include "mlc.hpp"
#include <variant>


namespace test_trait_e2e {

mlc::String to_string(int self) noexcept;
template<typename T>
concept Display = requires(const T& self) { { to_string(self) } -> std::convertible_to<mlc::String>; };
template<typename T>
requires Display<T>
T id(T x) noexcept;
int main(int argc, char** argv) noexcept;

} // namespace test_trait_e2e

#endif // TEST_TRAIT_E2E_HPP
