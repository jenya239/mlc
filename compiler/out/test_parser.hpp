#ifndef TEST_PARSER_HPP
#define TEST_PARSER_HPP

#include "mlc.hpp"
#include <variant>

#include "test_runner.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "ast.hpp"
#include "exprs.hpp"
#include "types.hpp"
#include "predicates.hpp"

namespace test_parser {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<test_runner::TestResult> parser_tests() noexcept;

} // namespace test_parser

#endif // TEST_PARSER_HPP
