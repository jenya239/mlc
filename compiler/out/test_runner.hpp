#ifndef TEST_RUNNER_HPP
#define TEST_RUNNER_HPP

#include "mlc.hpp"
#include <variant>

namespace test_runner {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct Pass {mlc::String name;};struct Fail {mlc::String name;mlc::String message;};using TestResult = std::variant<Pass, Fail>;

test_runner::TestResult assert_true(mlc::String test_name, bool condition) noexcept;

test_runner::TestResult assert_eq_int(mlc::String test_name, int actual, int expected) noexcept;

test_runner::TestResult assert_eq_str(mlc::String test_name, mlc::String actual, mlc::String expected) noexcept;

test_runner::TestResult assert_diagnostic_at(mlc::String test_name, mlc::String formatted_diagnostic, int expected_line, int expected_column, mlc::String message_needle) noexcept;

test_runner::TestResult assert_diagnostic_at_with_code(mlc::String test_name, mlc::String formatted_diagnostic, int expected_line, int expected_column, mlc::String message_needle, mlc::String expected_code) noexcept;

mlc::String run_all(mlc::Array<test_runner::TestResult> results) noexcept;

} // namespace test_runner

#endif // TEST_RUNNER_HPP
