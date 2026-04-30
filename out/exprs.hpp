#ifndef EXPRS_HPP
#define EXPRS_HPP

#include "mlc.hpp"
#include <variant>

#include "preds.hpp"
#include "types.hpp"
#include "ast.hpp"
#include "ast_tokens.hpp"
#include "lexer.hpp"

namespace exprs {

preds::StmtsResult parse_stmts_until_end(preds::Parser parser) noexcept;
preds::StmtsResult parse_stmts_until_else_end(preds::Parser parser) noexcept;
preds::StmtResult parse_statement_let_const(preds::Parser parser, ast::Span statement_span) noexcept;
ast_tokens::TKind parser_next_kind(preds::Parser p) noexcept;
preds::TypeResult parse_after_let_pattern(preds::Parser parser) noexcept;
void __skip__() noexcept;
preds::PatResult parse_array_pattern(preds::Parser parser, ast::Span bracket_span) noexcept;
preds::StmtResult parse_statement_let(preds::Parser parser) noexcept;
preds::StmtResult parse_statement_const(preds::Parser parser) noexcept;
preds::StmtResult parse_statement_return(preds::Parser parser) noexcept;
preds::StmtResult parse_statement_expression_or_assign(preds::Parser parser) noexcept;
preds::StmtResult parse_stmt(preds::Parser parser) noexcept;
preds::PatResult parse_pattern_identifier_branch(preds::Parser parser, mlc::String name) noexcept;
preds::PatResult parse_pattern_integer(preds::Parser parser, int value) noexcept;
preds::PatResult parse_pattern_string(preds::Parser parser, mlc::String value) noexcept;
preds::PatResult parse_pattern_boolean(preds::Parser parser, bool value) noexcept;
preds::PatResult parse_pattern_fallback_wildcard(preds::Parser parser) noexcept;
preds::PatResult parse_or_pat(preds::Parser parser) noexcept;
preds::PatResult parse_pat(preds::Parser parser) noexcept;
preds::PatsResult parse_pat_args(preds::Parser parser) noexcept;
preds::PatsResult parse_record_pat_fields(preds::Parser parser) noexcept;
preds::ExprResult parse_expr(preds::Parser parser) noexcept;
std::shared_ptr<ast::Expr> pipe_desugar(std::shared_ptr<ast::Expr> left_expr, std::shared_ptr<ast::Expr> right_expr) noexcept;
preds::ExprResult parse_pipe(preds::Parser parser) noexcept;
preds::ExprResult parse_or(preds::Parser parser) noexcept;
preds::ExprResult parse_and(preds::Parser parser) noexcept;
bool is_cmp_op(mlc::String op) noexcept;
preds::ExprResult parse_cmp(preds::Parser parser) noexcept;
preds::ExprResult parse_add(preds::Parser parser) noexcept;
preds::ExprResult parse_mul(preds::Parser parser) noexcept;
preds::ExprResult parse_unary(preds::Parser parser) noexcept;
preds::ExprResult parse_postfix(preds::Parser parser) noexcept;
preds::ExprsResult parse_arg_list(preds::Parser parser) noexcept;
std::shared_ptr<ast::Expr> build_template_expr(mlc::Array<mlc::String> parts) noexcept;
preds::ExprResult parse_primary_integer_literal(preds::Parser parser, int value) noexcept;
preds::ExprResult parse_primary_string_literal(preds::Parser parser, mlc::String value) noexcept;
preds::ExprResult parse_primary_template_literal(preds::Parser parser, mlc::Array<mlc::String> parts) noexcept;
preds::ExprResult parse_primary_boolean_literal(preds::Parser parser, bool value) noexcept;
preds::ExprResult parse_primary_parenthesized(preds::Parser parser) noexcept;
preds::ExprResult parse_primary_if_or_unless(preds::Parser parser) noexcept;
preds::ExprResult parse_primary_do_block(preds::Parser parser) noexcept;
preds::ExprResult parse_primary_while_loop(preds::Parser parser) noexcept;
preds::ExprResult parse_primary_for_loop(preds::Parser parser) noexcept;
preds::ExprResult parse_primary_match(preds::Parser parser) noexcept;
preds::ExprResult parse_primary_return_as_block(preds::Parser parser) noexcept;
preds::ExprResult parse_primary_identifier(preds::Parser parser, mlc::String name) noexcept;
preds::ExprResult parse_primary_unit_fallback(preds::Parser parser) noexcept;
preds::ExprResult parse_primary(preds::Parser parser) noexcept;
bool looks_like_typed_lambda_params(preds::Parser parser) noexcept;
preds::NamesResult parse_typed_lambda_params(preds::Parser parser) noexcept;
bool looks_like_lambda_params(preds::Parser parser) noexcept;
preds::NamesResult parse_lambda_params(preds::Parser parser) noexcept;
preds::ExprResult parse_array_lit(preds::Parser parser, ast::Span header_span) noexcept;
preds::ExprResult parse_if_expr(preds::Parser parser) noexcept;
preds::ExprResult parse_block(preds::Parser parser, ast::Span header_span) noexcept;
bool is_unit_expr(std::shared_ptr<ast::Expr> expr) noexcept;
std::shared_ptr<ast::Expr> block_result(mlc::Array<std::shared_ptr<ast::Stmt>> stmts) noexcept;
mlc::Array<std::shared_ptr<ast::Stmt>> block_body(mlc::Array<std::shared_ptr<ast::Stmt>> stmts) noexcept;
std::shared_ptr<ast::Expr> StmtsResult_to_block_expr(preds::StmtsResult self, ast::Span header_span) noexcept;
preds::ExprResult parse_while_expr(preds::Parser parser, ast::Span header_span) noexcept;
preds::ExprResult parse_for_expr(preds::Parser parser, ast::Span header_span) noexcept;
preds::ExprResult parse_with_expr(preds::Parser parser, ast::Span header_span) noexcept;
preds::ExprResult parse_match_expr(preds::Parser parser, ast::Span header_span) noexcept;
preds::ArmsResult parse_arms_brace(preds::Parser parser) noexcept;
preds::ArmsResult parse_arms_pipe(preds::Parser parser) noexcept;
preds::ArmsResult parse_arms(preds::Parser parser) noexcept;
preds::FieldValsResult parse_record_fields(preds::Parser parser) noexcept;
preds::ExprResult parse_record_expr(preds::Parser parser, mlc::String record_name, ast::Span record_span) noexcept;

} // namespace exprs

#endif // EXPRS_HPP
