#ifndef TEST_PARSER_HPP
#define TEST_PARSER_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "ast.hpp"
#include "exprs.hpp"
#include "preds.hpp"

namespace test_parser {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

mlc::Array<test_runner::TestResult> parser_tests() noexcept;

} // namespace test_parser

#endif // TEST_PARSER_HPP
