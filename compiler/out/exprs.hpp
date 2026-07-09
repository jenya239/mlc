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

predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Stmt>>> parse_statements_until_end(predicates::Parser parser) noexcept;
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Stmt>>> parse_statements_until_end_or_eof(predicates::Parser parser) noexcept;
bool parser_at_toplevel_declaration_start(predicates::Parser parser) noexcept;
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Stmt>>> parse_statements_until_end_eof_or_toplevel_decl(predicates::Parser parser) noexcept;
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Stmt>>> parse_statements_until_else_end(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Stmt>> parse_statement_let_const(predicates::Parser parser, ast::Span statement_span) noexcept;
ast_tokens::TokenKind parser_next_kind(predicates::Parser p) noexcept;
predicates::ParseResult<std::shared_ptr<ast::TypeExpr>> parse_after_let_pattern(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Pattern>> parse_array_pattern(predicates::Parser parser, ast::Span bracket_span) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Stmt>> parse_statement_let_pat_finish(std::shared_ptr<ast::Pattern> pattern, bool is_mut, predicates::Parser after_pattern, ast::Span statement_span) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Stmt>> parse_statement_let_mutable_body(predicates::Parser name_pos, bool is_mut, ast::Span statement_span) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Stmt>> parse_statement_let(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Stmt>> parse_statement_const(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Stmt>> parse_statement_return(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Stmt>> parse_statement_expression_or_assign(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Stmt>> parse_statement(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Pattern>> parse_pattern_identifier_branch(predicates::Parser parser, mlc::String name) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Pattern>> parse_pattern_integer(predicates::Parser parser, int value) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Pattern>> parse_pattern_string(predicates::Parser parser, mlc::String value) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Pattern>> parse_pattern_boolean(predicates::Parser parser, bool value) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Pattern>> parse_pattern_fallback_wildcard(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Pattern>> parse_or_pattern(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Pattern>> parse_pattern(predicates::Parser parser) noexcept;
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Pattern>>> parse_pattern_args(predicates::Parser parser) noexcept;
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Pattern>>> parse_record_pattern_fields(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_expr(predicates::Parser parser) noexcept;
std::shared_ptr<ast::Expr> pipe_desugar(std::shared_ptr<ast::Expr> left_expression, std::shared_ptr<ast::Expr> right_expression) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_pipe(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_or(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_and(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_and_no_pipe(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_or_no_pipe(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_bitwise_or(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_bitwise_xor(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_bitwise_and(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_equality(predicates::Parser parser) noexcept;
bool is_relational_comparison_operator(mlc::String operator_) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_comparison_relational(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_shift(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_add(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_mul(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_unary(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_postfix_conditional(predicates::Parser parser, std::shared_ptr<ast::Expr> expression) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_postfix(predicates::Parser parser) noexcept;
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Expr>>> parse_arg_list(predicates::Parser parser) noexcept;
std::shared_ptr<ast::Expr> build_template_expr(mlc::Array<mlc::String> parts) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_primary_integer_literal(predicates::Parser parser, int value) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_primary_string_literal(predicates::Parser parser, mlc::String value) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_primary_template_literal(predicates::Parser parser, mlc::Array<mlc::String> parts) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_primary_boolean_literal(predicates::Parser parser, bool value) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_primary_parenthesized(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_primary_if_or_unless(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_primary_do_block(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_primary_while_loop(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_primary_for_loop(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_spawn_expr(predicates::Parser parser, ast::Span header_span) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_primary_spawn(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_primary_match(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_primary_return_as_block(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_primary_identifier(predicates::Parser parser, mlc::String name) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_primary_unit_fallback(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_primary(predicates::Parser parser) noexcept;
bool looks_like_typed_lambda_params(predicates::Parser parser) noexcept;
predicates::ParseResult<mlc::Array<mlc::String>> parse_typed_lambda_params(predicates::Parser parser) noexcept;
bool looks_like_lambda_params(predicates::Parser parser) noexcept;
predicates::ParseResult<mlc::Array<mlc::String>> parse_lambda_params(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_array_lit(predicates::Parser parser, ast::Span header_span) noexcept;
std::shared_ptr<ast::Expr> if_expression_condition(bool is_negated, std::shared_ptr<ast::Expr> expression) noexcept;
predicates::Parser parser_after_optional_end(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_if_expr(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_block(predicates::Parser parser, ast::Span header_span) noexcept;
bool is_unit_expr(std::shared_ptr<ast::Expr> expression) noexcept;
bool is_stmt_expr(std::shared_ptr<ast::Stmt> statement) noexcept;
std::shared_ptr<ast::Expr> stmt_expr_expression(std::shared_ptr<ast::Stmt> statement) noexcept;
std::shared_ptr<ast::Expr> block_result(mlc::Array<std::shared_ptr<ast::Stmt>> stmts) noexcept;
mlc::Array<std::shared_ptr<ast::Stmt>> block_body_without_last_statement(mlc::Array<std::shared_ptr<ast::Stmt>> stmts) noexcept;
mlc::Array<std::shared_ptr<ast::Stmt>> block_body(mlc::Array<std::shared_ptr<ast::Stmt>> stmts) noexcept;
std::shared_ptr<ast::Expr> statements_result_to_block_expr(predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Stmt>>> statements_result, ast::Span header_span) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_while_expr(predicates::Parser parser, ast::Span header_span) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_for_expr(predicates::Parser parser, ast::Span header_span) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_with_expr(predicates::Parser parser, ast::Span header_span) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_match_scrutinee(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_match_expr(predicates::Parser parser, ast::Span header_span) noexcept;
predicates::ParseResult<std::shared_ptr<ast::MatchArm>> match_arm_parse_result(std::shared_ptr<ast::MatchArm> arm, predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_match_arm_body(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::MatchArm>> parse_match_arm(predicates::ParseResult<std::shared_ptr<ast::Pattern>> pat_parsed) noexcept;
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::MatchArm>>> parse_arms_brace(predicates::Parser parser) noexcept;
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::MatchArm>>> parse_arms_do_delimited(predicates::Parser parser) noexcept;
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::MatchArm>>> parse_arms_pipe(predicates::Parser parser) noexcept;
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::MatchArm>>> parse_arms(predicates::Parser parser) noexcept;
predicates::Parser skip_comma_if_present(predicates::Parser parser) noexcept;
mlc::Array<ast::RecordLitPart> lit_parts_flush_fields(mlc::Array<ast::RecordLitPart> lit_parts, mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals) noexcept;
mlc::Array<std::shared_ptr<ast::FieldVal>> empty_record_lit_field_vals() noexcept;
predicates::ParseResult<mlc::Array<ast::RecordLitPart>> parse_record_lit_field_with_value(predicates::Parser state, mlc::Array<ast::RecordLitPart> lit_parts, mlc::Array<std::shared_ptr<ast::FieldVal>> current_field_vals, mlc::String field_name, predicates::Parser after_colon) noexcept;
predicates::ParseResult<mlc::Array<ast::RecordLitPart>> parse_record_lit_field_shorthand(predicates::Parser state, mlc::Array<ast::RecordLitPart> lit_parts, mlc::Array<std::shared_ptr<ast::FieldVal>> current_field_vals, mlc::String field_name, ast::Span field_span, predicates::Parser after_field_name) noexcept;
predicates::ParseResult<mlc::Array<ast::RecordLitPart>> parse_record_lit_field(predicates::Parser state, mlc::Array<ast::RecordLitPart> lit_parts, mlc::Array<std::shared_ptr<ast::FieldVal>> current_field_vals) noexcept;
predicates::ParseResult<mlc::Array<ast::RecordLitPart>> parse_record_lit_parts_loop(predicates::Parser state, mlc::Array<ast::RecordLitPart> lit_parts, mlc::Array<std::shared_ptr<ast::FieldVal>> current_field_vals) noexcept;
predicates::ParseResult<mlc::Array<ast::RecordLitPart>> parse_record_lit_parts(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_record_expr(predicates::Parser parser, mlc::String record_name, ast::Span record_span) noexcept;

} // namespace exprs

#endif // EXPRS_HPP
