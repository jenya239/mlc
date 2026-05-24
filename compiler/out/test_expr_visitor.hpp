#ifndef TEST_EXPR_VISITOR_HPP
#define TEST_EXPR_VISITOR_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"

namespace test_expr_visitor {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

mlc::Array<test_runner::TestResult> expr_visitor_tests() noexcept;

} // namespace test_expr_visitor

#endif // TEST_EXPR_VISITOR_HPP
