#ifndef TEST_LEXER_HPP
#define TEST_LEXER_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "lexer.hpp"

namespace test_lexer {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

mlc::Array<test_runner::TestResult> lexer_tests() noexcept;

} // namespace test_lexer

#endif // TEST_LEXER_HPP
