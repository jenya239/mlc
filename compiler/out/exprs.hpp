#ifndef EXPRS_HPP
#define EXPRS_HPP

#include "mlc.hpp"
#include <variant>

#include "predicates.hpp"
#include "types.hpp"
#include "comma_separated.hpp"
#include "expect_close.hpp"
#include "ast.hpp"
#include "ast_tokens.hpp"
#include "lexer.hpp"

namespace exprs {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

predicates::StmtsResult parse_statements_until_end(predicates::Parser parser) noexcept;

predicates::StmtsResult parse_statements_until_else_end(predicates::Parser parser) noexcept;

predicates::PatternsResult parse_record_pattern_fields(predicates::Parser parser) noexcept;

predicates::ExprResult parse_expr(predicates::Parser parser) noexcept;

std::shared_ptr<ast::Expr> pipe_desugar(std::shared_ptr<ast::Expr> left_expression, std::shared_ptr<ast::Expr> right_expression) noexcept;

predicates::ExprResult parse_if_expr(predicates::Parser parser) noexcept;

} // namespace exprs

#endif // EXPRS_HPP
