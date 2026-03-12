#ifndef TEST_PARSER_HPP
#define TEST_PARSER_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "ast.hpp"

namespace test_parser {

struct Expr;
struct Stmt;

mlc::Array<test_runner::TestResult> parser_tests() noexcept;

} // namespace test_parser

#endif // TEST_PARSER_HPP
