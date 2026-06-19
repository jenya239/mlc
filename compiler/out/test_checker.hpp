#ifndef TEST_CHECKER_HPP
#define TEST_CHECKER_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "check.hpp"
#include "diagnostic_codes.hpp"

namespace test_checker {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<test_runner::TestResult> checker_tests() noexcept;

} // namespace test_checker

#endif // TEST_CHECKER_HPP
