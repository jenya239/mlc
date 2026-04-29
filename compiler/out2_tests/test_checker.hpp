#ifndef TEST_CHECKER_HPP
#define TEST_CHECKER_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "check.hpp"

namespace test_checker {

int check_error_count(mlc::String source) noexcept;
int check_with_context_error_count(mlc::String entry_source, mlc::String full_source) noexcept;
mlc::String first_checker_error_line(mlc::String source) noexcept;
mlc::String first_checker_error_line_with_path(mlc::String source, mlc::String source_path) noexcept;
bool checker_first_error_contains_nonfunction_i32(mlc::String program_source) noexcept;
mlc::Array<test_runner::TestResult> checker_tests() noexcept;

} // namespace test_checker

#endif // TEST_CHECKER_HPP
