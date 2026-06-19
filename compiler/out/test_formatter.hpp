#ifndef TEST_FORMATTER_HPP
#define TEST_FORMATTER_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "ast_printer.hpp"
#include "lexer.hpp"
#include "predicates.hpp"
#include "exprs.hpp"

namespace test_formatter {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<test_runner::TestResult> formatter_tests() noexcept;

} // namespace test_formatter

#endif // TEST_FORMATTER_HPP
