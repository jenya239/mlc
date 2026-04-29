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

ast::Program parse_source(mlc::String source) noexcept;
std::shared_ptr<ast::Expr> parse_expr_source(mlc::String source) noexcept;
int decl_count(mlc::String source) noexcept;
std::shared_ptr<ast::Decl> first_decl(mlc::String source) noexcept;
std::shared_ptr<ast::Decl> second_decl(mlc::String source) noexcept;
mlc::Array<test_runner::TestResult> parser_tests() noexcept;

} // namespace test_parser

#endif // TEST_PARSER_HPP
