#ifndef TEST_RUNNER_HPP
#define TEST_RUNNER_HPP

#include "mlc.hpp"
#include <variant>


namespace test_runner {

struct Pass;
struct Fail;
using TestResult = std::variant<Pass, Fail>;
struct Pass {mlc::String name;};
struct Fail {mlc::String name;mlc::String message;};
TestResult assert_true(mlc::String test_name, bool condition) noexcept;
TestResult assert_eq_int(mlc::String test_name, int actual, int expected) noexcept;
TestResult assert_eq_str(mlc::String test_name, mlc::String actual, mlc::String expected) noexcept;
TestResult assert_diagnostic_at(mlc::String test_name, mlc::String formatted_diagnostic, int expected_line, int expected_column, mlc::String message_needle) noexcept;
mlc::String run_all(mlc::Array<TestResult> results) noexcept;

} // namespace test_runner

#endif // TEST_RUNNER_HPP
