#ifndef TEST_CPP_EXPRS_HPP
#define TEST_CPP_EXPRS_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "cpp_lexer.hpp"
#include "cpp_exprs.hpp"
#include "cpp_ast.hpp"

namespace test_cpp_exprs {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<test_runner::TestResult> cpp_expr_parser_tests() noexcept;

} // namespace test_cpp_exprs

#endif // TEST_CPP_EXPRS_HPP
