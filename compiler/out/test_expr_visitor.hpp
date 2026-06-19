#ifndef TEST_EXPR_VISITOR_HPP
#define TEST_EXPR_VISITOR_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "expr_visitor.hpp"

namespace test_expr_visitor {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<test_runner::TestResult> expr_visitor_tests() noexcept;

} // namespace test_expr_visitor

#endif // TEST_EXPR_VISITOR_HPP
