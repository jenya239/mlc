#include "exprs.hpp"

#include "predicates.hpp"
#include "types.hpp"
#include "comma_separated.hpp"
#include "expect_close.hpp"
#include "ast.hpp"
#include "ast_tokens.hpp"
#include "lexer.hpp"

namespace exprs {

using namespace predicates;
using namespace types;
using namespace comma_separated;
using namespace expect_close;
using namespace ast;
using namespace ast_tokens;
using namespace lexer;
using namespace ast_tokens;

using MatchArmParseOutcome = predicates::ParseResult<std::shared_ptr<ast::MatchArm>>;

predicates::StmtsResult parse_statements_until_end(predicates::Parser parser) noexcept;

predicates::StmtsResult parse_statements_until_end_or_eof(predicates::Parser parser) noexcept;

bool parser_at_toplevel_declaration_start(predicates::Parser parser) noexcept;

predicates::StmtsResult parse_statements_until_end_eof_or_toplevel_decl(predicates::Parser parser) noexcept;

predicates::StmtsResult parse_statements_until_else_end(predicates::Parser parser) noexcept;

predicates::StmtResult parse_statement_let_const(predicates::Parser parser, ast::Span statement_span) noexcept;

ast_tokens::TokenKind parser_next_kind(predicates::Parser p) noexcept;

predicates::TypeResult parse_after_let_pattern(predicates::Parser parser) noexcept;

predicates::PatternResult parse_array_pattern(predicates::Parser parser, ast::Span bracket_span) noexcept;

predicates::StmtResult parse_statement_let_pat_finish(std::shared_ptr<ast::Pattern> pattern, bool is_mut, predicates::Parser after_pattern, ast::Span statement_span) noexcept;

predicates::StmtResult parse_statement_let_mutable_body(predicates::Parser name_pos, bool is_mut, ast::Span statement_span) noexcept;

predicates::StmtResult parse_statement_let(predicates::Parser parser) noexcept;

predicates::StmtResult parse_statement_const(predicates::Parser parser) noexcept;

predicates::StmtResult parse_statement_return(predicates::Parser parser) noexcept;

predicates::StmtResult parse_statement_expression_or_assign(predicates::Parser parser) noexcept;

predicates::StmtResult parse_statement(predicates::Parser parser) noexcept;

predicates::PatternResult parse_pattern_identifier_branch(predicates::Parser parser, mlc::String name) noexcept;

predicates::PatternResult parse_pattern_integer(predicates::Parser parser, int value) noexcept;

predicates::PatternResult parse_pattern_string(predicates::Parser parser, mlc::String value) noexcept;

predicates::PatternResult parse_pattern_boolean(predicates::Parser parser, bool value) noexcept;

predicates::PatternResult parse_pattern_fallback_wildcard(predicates::Parser parser) noexcept;

predicates::PatternResult parse_or_pattern(predicates::Parser parser) noexcept;

predicates::PatternResult parse_pattern(predicates::Parser parser) noexcept;

predicates::PatternsResult parse_pattern_args(predicates::Parser parser) noexcept;

predicates::PatternsResult parse_record_pattern_fields(predicates::Parser parser) noexcept;

predicates::ExprResult parse_expr(predicates::Parser parser) noexcept;

std::shared_ptr<ast::Expr> pipe_desugar(std::shared_ptr<ast::Expr> left_expression, std::shared_ptr<ast::Expr> right_expression) noexcept;

predicates::ExprResult parse_pipe(predicates::Parser parser) noexcept;

predicates::ExprResult parse_or(predicates::Parser parser) noexcept;

predicates::ExprResult parse_and(predicates::Parser parser) noexcept;

predicates::ExprResult parse_equality(predicates::Parser parser) noexcept;

bool is_relational_comparison_operator(mlc::String operator_) noexcept;

predicates::ExprResult parse_comparison_relational(predicates::Parser parser) noexcept;

predicates::ExprResult parse_add(predicates::Parser parser) noexcept;

predicates::ExprResult parse_mul(predicates::Parser parser) noexcept;

predicates::ExprResult parse_unary(predicates::Parser parser) noexcept;

predicates::ExprResult parse_postfix(predicates::Parser parser) noexcept;

predicates::ExprsResult parse_arg_list(predicates::Parser parser) noexcept;

std::shared_ptr<ast::Expr> build_template_expr(mlc::Array<mlc::String> parts) noexcept;

predicates::ExprResult parse_primary_integer_literal(predicates::Parser parser, int value) noexcept;

predicates::ExprResult parse_primary_string_literal(predicates::Parser parser, mlc::String value) noexcept;

predicates::ExprResult parse_primary_template_literal(predicates::Parser parser, mlc::Array<mlc::String> parts) noexcept;

predicates::ExprResult parse_primary_boolean_literal(predicates::Parser parser, bool value) noexcept;

predicates::ExprResult parse_primary_parenthesized(predicates::Parser parser) noexcept;

predicates::ExprResult parse_primary_if_or_unless(predicates::Parser parser) noexcept;

predicates::ExprResult parse_primary_do_block(predicates::Parser parser) noexcept;

predicates::ExprResult parse_primary_while_loop(predicates::Parser parser) noexcept;

predicates::ExprResult parse_primary_for_loop(predicates::Parser parser) noexcept;

predicates::ExprResult parse_spawn_expr(predicates::Parser parser, ast::Span header_span) noexcept;

predicates::ExprResult parse_primary_spawn(predicates::Parser parser) noexcept;

predicates::ExprResult parse_primary_match(predicates::Parser parser) noexcept;

predicates::ExprResult parse_primary_return_as_block(predicates::Parser parser) noexcept;

predicates::ExprResult parse_primary_identifier(predicates::Parser parser, mlc::String name) noexcept;

predicates::ExprResult parse_primary_unit_fallback(predicates::Parser parser) noexcept;

predicates::ExprResult parse_primary(predicates::Parser parser) noexcept;

bool looks_like_typed_lambda_params(predicates::Parser parser) noexcept;

predicates::NamesResult parse_typed_lambda_params(predicates::Parser parser) noexcept;

bool looks_like_lambda_params(predicates::Parser parser) noexcept;

predicates::NamesResult parse_lambda_params(predicates::Parser parser) noexcept;

predicates::ExprResult parse_array_lit(predicates::Parser parser, ast::Span header_span) noexcept;

std::shared_ptr<ast::Expr> if_expression_condition(bool is_negated, std::shared_ptr<ast::Expr> expression) noexcept;

predicates::Parser parser_after_optional_end(predicates::Parser parser) noexcept;

predicates::ExprResult parse_if_expr(predicates::Parser parser) noexcept;

predicates::ExprResult parse_block(predicates::Parser parser, ast::Span header_span) noexcept;

bool is_unit_expr(std::shared_ptr<ast::Expr> expression) noexcept;

bool is_stmt_expr(std::shared_ptr<ast::Stmt> statement) noexcept;

std::shared_ptr<ast::Expr> stmt_expr_expression(std::shared_ptr<ast::Stmt> statement) noexcept;

std::shared_ptr<ast::Expr> block_result(mlc::Array<std::shared_ptr<ast::Stmt>> stmts) noexcept;

mlc::Array<std::shared_ptr<ast::Stmt>> block_body_without_last_statement(mlc::Array<std::shared_ptr<ast::Stmt>> stmts) noexcept;

mlc::Array<std::shared_ptr<ast::Stmt>> block_body(mlc::Array<std::shared_ptr<ast::Stmt>> stmts) noexcept;

std::shared_ptr<ast::Expr> statements_result_to_block_expr(predicates::StmtsResult statements_result, ast::Span header_span) noexcept;

predicates::ExprResult parse_while_expr(predicates::Parser parser, ast::Span header_span) noexcept;

predicates::ExprResult parse_for_expr(predicates::Parser parser, ast::Span header_span) noexcept;

predicates::ExprResult parse_with_expr(predicates::Parser parser, ast::Span header_span) noexcept;

predicates::ExprResult parse_match_expr(predicates::Parser parser, ast::Span header_span) noexcept;

exprs::MatchArmParseOutcome match_arm_parse_result(std::shared_ptr<ast::MatchArm> arm, predicates::Parser parser) noexcept;

exprs::MatchArmParseOutcome parse_match_arm(predicates::PatternResult pat_parsed) noexcept;

predicates::ArmsResult parse_arms_brace(predicates::Parser parser) noexcept;

predicates::ArmsResult parse_arms_do_delimited(predicates::Parser parser) noexcept;

predicates::ArmsResult parse_arms_pipe(predicates::Parser parser) noexcept;

predicates::ArmsResult parse_arms(predicates::Parser parser) noexcept;

predicates::Parser skip_comma_if_present(predicates::Parser parser) noexcept;

mlc::Array<ast::RecordLitPart> lit_parts_flush_fields(mlc::Array<ast::RecordLitPart> lit_parts, mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals) noexcept;

mlc::Array<std::shared_ptr<ast::FieldVal>> empty_record_lit_field_vals() noexcept;

predicates::RecordLitPartsResult parse_record_lit_field_with_value(predicates::Parser state, mlc::Array<ast::RecordLitPart> lit_parts, mlc::Array<std::shared_ptr<ast::FieldVal>> current_field_vals, mlc::String field_name, predicates::Parser after_colon) noexcept;

predicates::RecordLitPartsResult parse_record_lit_field_shorthand(predicates::Parser state, mlc::Array<ast::RecordLitPart> lit_parts, mlc::Array<std::shared_ptr<ast::FieldVal>> current_field_vals, mlc::String field_name, ast::Span field_span, predicates::Parser after_field_name) noexcept;

predicates::RecordLitPartsResult parse_record_lit_field(predicates::Parser state, mlc::Array<ast::RecordLitPart> lit_parts, mlc::Array<std::shared_ptr<ast::FieldVal>> current_field_vals) noexcept;

predicates::RecordLitPartsResult parse_record_lit_parts_loop(predicates::Parser state, mlc::Array<ast::RecordLitPart> lit_parts, mlc::Array<std::shared_ptr<ast::FieldVal>> current_field_vals) noexcept;

predicates::RecordLitPartsResult parse_record_lit_parts(predicates::Parser parser) noexcept;

predicates::ExprResult parse_record_expr(predicates::Parser parser, mlc::String record_name, ast::Span record_span) noexcept;

predicates::StmtsResult parse_statements_until_end(predicates::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::Stmt>> stmts = {};
predicates::Parser state = std::move(parser);
while (!predicates::TokenKind_is_end(predicates::Parser_kind(state)) && !predicates::Parser_at_eof(state)){
{
predicates::StmtResult parsed = parse_statement(state);
stmts.push_back(parsed.value);
state = predicates::Parser_skip_semi(parsed.parser);
}
}
if (predicates::TokenKind_is_end(predicates::Parser_kind(state))){
{
return predicates::statements_parse_result(stmts, predicates::Parser_advance(state));
}
}
if (predicates::Parser_at_eof(state)){
{
return predicates::statements_parse_result(stmts, predicates::Parser_record_parse_error(state, mlc::String("parse: expected end in block")));
}
}
return predicates::statements_parse_result(stmts, state);
}

predicates::StmtsResult parse_statements_until_end_or_eof(predicates::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::Stmt>> stmts = {};
predicates::Parser state = std::move(parser);
while (!predicates::TokenKind_is_end(predicates::Parser_kind(state)) && !predicates::Parser_at_eof(state)){
{
predicates::StmtResult parsed = parse_statement(state);
stmts.push_back(parsed.value);
state = predicates::Parser_skip_semi(parsed.parser);
}
}
if (predicates::TokenKind_is_end(predicates::Parser_kind(state))){
{
return predicates::statements_parse_result(stmts, predicates::Parser_advance(state));
}
}
return predicates::statements_parse_result(stmts, state);
}

bool parser_at_toplevel_declaration_start(predicates::Parser parser) noexcept{
ast_tokens::TokenKind kind = predicates::Parser_kind(parser);
if (predicates::TokenKind_is_fn(kind) || predicates::TokenKind_is_type(kind) || predicates::TokenKind_is_extend(kind) || predicates::TokenKind_is_import(kind)){
{
return true;
}
}
if (predicates::TokenKind_is_extern(kind) && predicates::TokenKind_is_fn(predicates::Parser_kind(predicates::Parser_advance(parser)))){
{
return true;
}
}
if (predicates::TokenKind_is_ident(kind) && predicates::TokenKind_ident(kind) == mlc::String("export") || predicates::TokenKind_ident(kind) == mlc::String("trait")){
{
return true;
}
}
return false;
}

predicates::StmtsResult parse_statements_until_end_eof_or_toplevel_decl(predicates::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::Stmt>> stmts = {};
predicates::Parser state = std::move(parser);
while (!predicates::TokenKind_is_end(predicates::Parser_kind(state)) && !predicates::Parser_at_eof(state) && !parser_at_toplevel_declaration_start(state)){
{
predicates::StmtResult parsed = parse_statement(state);
stmts.push_back(parsed.value);
state = predicates::Parser_skip_semi(parsed.parser);
}
}
if (predicates::TokenKind_is_end(predicates::Parser_kind(state))){
{
return predicates::statements_parse_result(stmts, predicates::Parser_advance(state));
}
}
return predicates::statements_parse_result(stmts, state);
}

predicates::StmtsResult parse_statements_until_else_end(predicates::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::Stmt>> stmts = {};
predicates::Parser state = std::move(parser);
while (!predicates::TokenKind_is_end(predicates::Parser_kind(state)) && !predicates::TokenKind_is_else(predicates::Parser_kind(state)) && !predicates::Parser_at_eof(state)){
{
predicates::StmtResult parsed = parse_statement(state);
stmts.push_back(parsed.value);
state = parsed.parser;
}
}
return predicates::statements_parse_result(stmts, state);
}

predicates::StmtResult parse_statement_let_const(predicates::Parser parser, ast::Span statement_span) noexcept{
predicates::Parser name_pos = predicates::Parser_advance(parser);
mlc::String var_name = predicates::TokenKind_ident(predicates::Parser_kind(name_pos));
predicates::TypeResult type_parsed = predicates::TokenKind_is_colon(predicates::Parser_kind(predicates::Parser_advance(name_pos))) ? types::parse_type(predicates::Parser_advance_by(name_pos, 2)) : predicates::type_parse_result(std::make_shared<ast::TypeExpr>((ast::TyUnit{})), predicates::Parser_advance(name_pos));
std::shared_ptr<ast::TypeExpr> type_expr = type_parsed.value;
predicates::ExprResult value_parsed = parse_expr(predicates::Parser_advance(type_parsed.parser));
std::shared_ptr<ast::Expr> expression = value_parsed.value;
return predicates::statement_parse_result(std::make_shared<ast::Stmt>(ast::StmtLetConst(var_name, type_expr, expression, statement_span)), value_parsed.parser);
}

ast_tokens::TokenKind parser_next_kind(predicates::Parser p) noexcept{
return p.position + 1 >= p.tokens.size() ? ast_tokens::TokenKind((ast_tokens::Eof{})) : ast_tokens::TokenKind(p.tokens[p.position + 1].kind);
}

predicates::TypeResult parse_after_let_pattern(predicates::Parser parser) noexcept{
return predicates::TokenKind_is_colon(predicates::Parser_kind(parser)) ? [&]() -> predicates::TypeResult { 
  predicates::TypeResult type_parsed = types::parse_type(predicates::Parser_advance(parser));
  std::shared_ptr<ast::TypeExpr> type_expr = type_parsed.value;
  return predicates::TokenKind_is_equal(predicates::Parser_kind(type_parsed.parser)) ? predicates::type_parse_result(type_expr, predicates::Parser_advance(type_parsed.parser)) : predicates::type_parse_result(type_expr, type_parsed.parser);
 }() : predicates::TokenKind_is_equal(predicates::Parser_kind(parser)) ? predicates::type_parse_result(std::make_shared<ast::TypeExpr>((ast::TyUnit{})), predicates::Parser_advance(parser)) : predicates::type_parse_result(std::make_shared<ast::TypeExpr>((ast::TyUnit{})), parser);
}

predicates::PatternResult parse_array_pattern(predicates::Parser parser, ast::Span bracket_span) noexcept{
return predicates::TokenKind_is_rbracket(predicates::Parser_kind(parser)) ? predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternArray({}, mlc::String(""), bracket_span)), predicates::Parser_advance(parser)) : [&]() -> predicates::PatternResult { 
  predicates::PatternResult first = parse_pattern(parser);
  mlc::Array<std::shared_ptr<ast::Pattern>> patterns = {};
  patterns.push_back(first.value);
  predicates::Parser parser_state = first.parser;
  mlc::String rest = mlc::String("");
  while (predicates::TokenKind_is_comma(predicates::Parser_kind(parser_state))){
{
parser_state = predicates::Parser_advance(parser_state);
if (predicates::TokenKind_is_spread(predicates::Parser_kind(parser_state))){
{
parser_state = predicates::Parser_advance(parser_state);
if (predicates::TokenKind_is_ident(predicates::Parser_kind(parser_state))){
{
rest = predicates::TokenKind_ident(predicates::Parser_kind(parser_state));
parser_state = predicates::Parser_advance(parser_state);
}
}
break;
}
} else {
{
predicates::PatternResult next = parse_pattern(parser_state);
patterns.push_back(next.value);
parser_state = next.parser;
}
}
}
}
  return predicates::TokenKind_is_rbracket(predicates::Parser_kind(parser_state)) ? predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternArray(patterns, rest, bracket_span)), predicates::Parser_advance(parser_state)) : [&]() -> predicates::PatternResult { 
  predicates::Parser error_parser = predicates::Parser_record_parse_error(parser_state, mlc::String("parse: expected ] in array pattern"));
  return predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternArray(patterns, rest, bracket_span)), error_parser);
 }();
 }();
}

predicates::StmtResult parse_statement_let_pat_finish(std::shared_ptr<ast::Pattern> pattern, bool is_mut, predicates::Parser after_pattern, ast::Span statement_span) noexcept{
predicates::TypeResult type_parsed = parse_after_let_pattern(after_pattern);
std::shared_ptr<ast::TypeExpr> type_expr = type_parsed.value;
predicates::ExprResult value_parsed = parse_expr(type_parsed.parser);
std::shared_ptr<ast::Expr> expression = value_parsed.value;
bool has_else = predicates::TokenKind_is_else(predicates::Parser_kind(value_parsed.parser));
predicates::StmtsResult else_parsed = has_else ? parse_statements_until_end(predicates::Parser_advance(value_parsed.parser)) : [&]() -> predicates::StmtsResult { 
  mlc::Array<std::shared_ptr<ast::Stmt>> empty_statements = {};
  return predicates::statements_parse_result(empty_statements, value_parsed.parser);
 }();
mlc::Array<std::shared_ptr<ast::Stmt>> block_statements = else_parsed.value;
std::shared_ptr<ast::Expr> else_body = std::make_shared<ast::Expr>(ast::ExprBlock(block_statements, std::make_shared<ast::Expr>(ast::ExprUnit(statement_span)), statement_span));
return predicates::statement_parse_result(std::make_shared<ast::Stmt>(ast::StmtLetPattern(pattern, is_mut, type_expr, expression, has_else, else_body, statement_span)), else_parsed.parser);
}

predicates::StmtResult parse_statement_let_mutable_body(predicates::Parser name_pos, bool is_mut, ast::Span statement_span) noexcept{
ast_tokens::TokenKind k0 = predicates::Parser_kind(name_pos);
if (predicates::TokenKind_is_lparen(k0)){
{
ast::Span paren_span = predicates::Parser_span_at_cursor(name_pos);
predicates::PatternsResult inner = parse_pattern_args(predicates::Parser_advance(name_pos));
std::shared_ptr<ast::Pattern> pattern = std::make_shared<ast::Pattern>(ast::PatternTuple(inner.value, paren_span));
return parse_statement_let_pat_finish(pattern, is_mut, inner.parser, statement_span);
}
}
if (predicates::TokenKind_is_lbracket(k0)){
{
ast::Span bracket_span = predicates::Parser_span_at_cursor(name_pos);
predicates::PatternResult array_pattern_parsed = parse_array_pattern(predicates::Parser_advance(name_pos), bracket_span);
return parse_statement_let_pat_finish(array_pattern_parsed.value, is_mut, array_pattern_parsed.parser, statement_span);
}
}
if (predicates::TokenKind_is_lbrace(k0)){
{
ast::Span brace_span = predicates::Parser_span_at_cursor(name_pos);
predicates::PatternsResult record_fields_parsed = parse_record_pattern_fields(predicates::Parser_advance(name_pos));
std::shared_ptr<ast::Pattern> pattern = std::make_shared<ast::Pattern>(ast::PatternRecord(mlc::String(""), record_fields_parsed.value, brace_span));
return parse_statement_let_pat_finish(pattern, is_mut, record_fields_parsed.parser, statement_span);
}
}
if (predicates::TokenKind_is_ident(k0) && predicates::is_ctor_name(predicates::TokenKind_ident(k0)) && predicates::TokenKind_is_lparen(parser_next_kind(name_pos))){
{
predicates::PatternResult pattern_branch_parsed = parse_pattern_identifier_branch(name_pos, predicates::TokenKind_ident(k0));
return parse_statement_let_pat_finish(pattern_branch_parsed.value, is_mut, pattern_branch_parsed.parser, statement_span);
}
}
if (predicates::TokenKind_is_ident(k0)){
{
mlc::String variable_name = predicates::TokenKind_ident(k0);
predicates::Parser after_name = predicates::Parser_advance(name_pos);
if (predicates::TokenKind_is_colon(predicates::Parser_kind(after_name))){
{
predicates::TypeResult type_parsed = types::parse_type(predicates::Parser_advance(after_name));
predicates::ExprResult value_parsed = parse_expr(predicates::Parser_advance(type_parsed.parser));
return predicates::statement_parse_result(std::make_shared<ast::Stmt>(ast::StmtLet(variable_name, is_mut, type_parsed.value, value_parsed.value, statement_span)), value_parsed.parser);
}
}
predicates::ExprResult value_parsed = parse_expr(predicates::Parser_advance(after_name));
return predicates::statement_parse_result(std::make_shared<ast::Stmt>(ast::StmtLet(variable_name, is_mut, std::make_shared<ast::TypeExpr>((ast::TyUnit{})), value_parsed.value, statement_span)), value_parsed.parser);
}
}
predicates::Parser error_parser = predicates::Parser_record_parse_error(name_pos, mlc::String("parse: expected pattern or identifier after let"));
return predicates::statement_parse_result(std::make_shared<ast::Stmt>(ast::StmtExpr(std::make_shared<ast::Expr>(ast::ExprUnit(predicates::Parser_span_at_cursor(name_pos))), statement_span)), error_parser);
}

predicates::StmtResult parse_statement_let(predicates::Parser parser) noexcept{
ast::Span statement_span = predicates::Parser_span_at_cursor(parser);
predicates::Parser after_let = predicates::Parser_advance(parser);
if (predicates::TokenKind_is_const(predicates::Parser_kind(after_let))){
{
return parse_statement_let_const(after_let, statement_span);
}
}
bool is_mut = predicates::TokenKind_is_mut(predicates::Parser_kind(after_let));
predicates::Parser name_pos = is_mut ? predicates::Parser_advance(after_let) : after_let;
return parse_statement_let_mutable_body(name_pos, is_mut, statement_span);
}

predicates::StmtResult parse_statement_const(predicates::Parser parser) noexcept{
ast::Span statement_span = predicates::Parser_span_at_cursor(parser);
predicates::Parser name_pos = predicates::Parser_advance(parser);
mlc::String var_name = predicates::TokenKind_ident(predicates::Parser_kind(name_pos));
predicates::TypeResult type_parsed = predicates::TokenKind_is_colon(predicates::Parser_kind(predicates::Parser_advance(name_pos))) ? types::parse_type(predicates::Parser_advance_by(name_pos, 2)) : predicates::type_parse_result(std::make_shared<ast::TypeExpr>((ast::TyUnit{})), predicates::Parser_advance(name_pos));
std::shared_ptr<ast::TypeExpr> type_expr = type_parsed.value;
predicates::ExprResult value_parsed = parse_expr(predicates::Parser_advance(type_parsed.parser));
std::shared_ptr<ast::Expr> expression = value_parsed.value;
return predicates::statement_parse_result(std::make_shared<ast::Stmt>(ast::StmtLet(var_name, false, type_expr, expression, statement_span)), value_parsed.parser);
}

predicates::StmtResult parse_statement_return(predicates::Parser parser) noexcept{
ast::Span statement_span = predicates::Parser_span_at_cursor(parser);
predicates::ExprResult value_parsed = parse_expr(predicates::Parser_advance(parser));
std::shared_ptr<ast::Expr> expression = value_parsed.value;
return predicates::statement_parse_result(std::make_shared<ast::Stmt>(ast::StmtReturn(expression, statement_span)), value_parsed.parser);
}

predicates::StmtResult parse_statement_expression_or_assign(predicates::Parser parser) noexcept{
ast::Span statement_span = predicates::Parser_span_at_cursor(parser);
predicates::ExprResult left_parsed = parse_expr(parser);
std::shared_ptr<ast::Expr> expression = left_parsed.value;
return predicates::TokenKind_is_equal(predicates::Parser_kind(left_parsed.parser)) ? [&]() -> predicates::StmtResult { 
  ast::Span assign_span = predicates::Parser_span_at_cursor(left_parsed.parser);
  predicates::ExprResult right_parsed = parse_expr(predicates::Parser_advance(left_parsed.parser));
  return predicates::statement_parse_result(std::make_shared<ast::Stmt>(ast::StmtExpr(std::make_shared<ast::Expr>(ast::ExprBin(mlc::String("="), expression, right_parsed.value, assign_span)), statement_span)), right_parsed.parser);
 }() : predicates::statement_parse_result(std::make_shared<ast::Stmt>(ast::StmtExpr(expression, statement_span)), left_parsed.parser);
}

predicates::StmtResult parse_statement(predicates::Parser parser) noexcept{
ast_tokens::TokenKind kind = predicates::Parser_kind(parser);
return predicates::TokenKind_is_let(kind) ? parse_statement_let(parser) : predicates::TokenKind_is_const(kind) ? parse_statement_const(parser) : predicates::TokenKind_is_break(kind) ? predicates::statement_parse_result(std::make_shared<ast::Stmt>(ast::StmtBreak(predicates::Parser_span_at_cursor(parser))), predicates::Parser_advance(parser)) : predicates::TokenKind_is_continue(kind) ? predicates::statement_parse_result(std::make_shared<ast::Stmt>(ast::StmtContinue(predicates::Parser_span_at_cursor(parser))), predicates::Parser_advance(parser)) : predicates::TokenKind_is_return(kind) ? parse_statement_return(parser) : predicates::TokenKind_is_while(kind) ? [&]() -> predicates::StmtResult { 
  ast::Span header_span = predicates::Parser_span_at_cursor(parser);
  predicates::ExprResult while_parsed = parse_while_expr(predicates::Parser_advance(parser), header_span);
  std::shared_ptr<ast::Expr> expression = while_parsed.value;
  return predicates::statement_parse_result(std::make_shared<ast::Stmt>(ast::StmtExpr(expression, header_span)), while_parsed.parser);
 }() : predicates::TokenKind_is_for(kind) ? [&]() -> predicates::StmtResult { 
  ast::Span header_span = predicates::Parser_span_at_cursor(parser);
  predicates::ExprResult for_parsed = parse_for_expr(predicates::Parser_advance(parser), header_span);
  std::shared_ptr<ast::Expr> expression = for_parsed.value;
  return predicates::statement_parse_result(std::make_shared<ast::Stmt>(ast::StmtExpr(expression, header_span)), for_parsed.parser);
 }() : predicates::TokenKind_is_do(kind) ? [&]() -> predicates::StmtResult { 
  ast::Span header_span = predicates::Parser_span_at_cursor(parser);
  predicates::ExprResult block_parsed = parse_block(predicates::Parser_advance(parser), header_span);
  std::shared_ptr<ast::Expr> expression = block_parsed.value;
  return predicates::statement_parse_result(std::make_shared<ast::Stmt>(ast::StmtExpr(expression, header_span)), block_parsed.parser);
 }() : predicates::TokenKind_is_with(kind) ? [&]() -> predicates::StmtResult { 
  ast::Span header_span = predicates::Parser_span_at_cursor(parser);
  predicates::ExprResult with_parsed = parse_with_expr(predicates::Parser_advance(parser), header_span);
  std::shared_ptr<ast::Expr> expression = with_parsed.value;
  return predicates::statement_parse_result(std::make_shared<ast::Stmt>(ast::StmtExpr(expression, header_span)), with_parsed.parser);
 }() : parse_statement_expression_or_assign(parser);
}

predicates::PatternResult parse_pattern_identifier_branch(predicates::Parser parser, mlc::String name) noexcept{
ast::Span binding_span = predicates::Parser_span_at_cursor(parser);
return name == mlc::String("_") ? predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternWild(binding_span)), predicates::Parser_advance(parser)) : predicates::is_ctor_name(name) ? [&]() -> predicates::PatternResult { 
  predicates::Parser after = predicates::Parser_advance(parser);
  return predicates::TokenKind_is_lparen(predicates::Parser_kind(after)) ? [&]() -> predicates::PatternResult { 
  predicates::PatternsResult sub_patterns = parse_pattern_args(predicates::Parser_advance(after));
  return predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternCtor(name, sub_patterns.value, binding_span)), sub_patterns.parser);
 }() : predicates::TokenKind_is_lbrace(predicates::Parser_kind(after)) ? [&]() -> predicates::PatternResult { 
  predicates::PatternsResult fields_parsed = parse_record_pattern_fields(predicates::Parser_advance(after));
  return predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternRecord(name, fields_parsed.value, binding_span)), fields_parsed.parser);
 }() : predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternCtor(name, {}, binding_span)), after);
 }() : predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternIdent(name, binding_span)), predicates::Parser_advance(parser));
}

predicates::PatternResult parse_pattern_integer(predicates::Parser parser, int value) noexcept{return predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternInt(value, predicates::Parser_span_at_cursor(parser))), predicates::Parser_advance(parser));}

predicates::PatternResult parse_pattern_string(predicates::Parser parser, mlc::String value) noexcept{return predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternStringLit(value, predicates::Parser_span_at_cursor(parser))), predicates::Parser_advance(parser));}

predicates::PatternResult parse_pattern_boolean(predicates::Parser parser, bool value) noexcept{return predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternBool(value, predicates::Parser_span_at_cursor(parser))), predicates::Parser_advance(parser));}

predicates::PatternResult parse_pattern_fallback_wildcard(predicates::Parser parser) noexcept{
predicates::Parser error_parser = predicates::Parser_record_parse_error(parser, mlc::String("parse: unexpected token in pattern"));
return predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternWild(predicates::Parser_span_at_cursor(parser))), predicates::Parser_advance(error_parser));
}

predicates::PatternResult parse_or_pattern(predicates::Parser parser) noexcept{
predicates::PatternResult first = parse_pattern(parser);
ast::Span span = predicates::Parser_span_at_cursor(parser);
mlc::Array<std::shared_ptr<ast::Pattern>> alts = mlc::Array<std::shared_ptr<ast::Pattern>>{first.value};
predicates::Parser state = first.parser;
while (predicates::TokenKind_is_bar(predicates::Parser_kind(state)) && !predicates::Parser_at_eof(predicates::Parser_advance(state)) && !predicates::TokenKind_is_fat_arrow(predicates::Parser_kind(predicates::Parser_advance(state)))){
{
predicates::PatternResult next = parse_pattern(predicates::Parser_advance(state));
alts.push_back(next.value);
state = next.parser;
}
}
return alts.size() == 1 ? first : predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternOr(alts, span)), state);
}

predicates::PatternResult parse_pattern(predicates::Parser parser) noexcept{
ast_tokens::TokenKind kind = predicates::Parser_kind(parser);
return predicates::TokenKind_is_ident(kind) ? parse_pattern_identifier_branch(parser, predicates::TokenKind_ident(kind)) : predicates::TokenKind_is_int(kind) ? parse_pattern_integer(parser, predicates::TokenKind_int_val(kind)) : predicates::TokenKind_is_str(kind) ? parse_pattern_string(parser, predicates::TokenKind_str_val(kind)) : predicates::TokenKind_is_true(kind) ? parse_pattern_boolean(parser, true) : predicates::TokenKind_is_false(kind) ? parse_pattern_boolean(parser, false) : predicates::TokenKind_is_else(kind) ? predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternWild(predicates::Parser_span_at_cursor(parser))), predicates::Parser_advance(parser)) : parse_pattern_fallback_wildcard(parser);
}

predicates::PatternsResult parse_pattern_args(predicates::Parser parser) noexcept{return predicates::TokenKind_is_rparen(predicates::Parser_kind(parser)) ? [&]() -> predicates::PatternsResult { 
  mlc::Array<std::shared_ptr<ast::Pattern>> empty_patterns = {};
  return predicates::patterns_parse_result(empty_patterns, predicates::Parser_advance(parser));
 }() : [&]() -> predicates::PatternsResult { 
  predicates::PatternsResult suffix = comma_separated::comma_separated_pats_suffix_from_first(parse_pattern(parser), parse_pattern);
  return predicates::patterns_parse_result(suffix.value, expect_close::parser_expect_rparen(suffix.parser, mlc::String("pattern list")));
 }();}

predicates::PatternsResult parse_record_pattern_fields(predicates::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::Pattern>> patterns = {};
predicates::Parser state = std::move(parser);
while (!predicates::TokenKind_is_rbrace(predicates::Parser_kind(state)) && !predicates::Parser_at_eof(state)){
{
if (predicates::TokenKind_is_ident(predicates::Parser_kind(state))){
{
patterns.push_back(std::make_shared<ast::Pattern>(ast::PatternIdent(predicates::TokenKind_ident(predicates::Parser_kind(state)), predicates::Parser_span_at_cursor(state))));
}
}
state = predicates::Parser_advance(state);
if (predicates::TokenKind_is_comma(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
}
}
}
}
return predicates::patterns_parse_result(patterns, expect_close::parser_expect_rbrace(state, mlc::String("record pattern fields")));
}

predicates::ExprResult parse_expr(predicates::Parser parser) noexcept{return parse_or(parser);}

std::shared_ptr<ast::Expr> pipe_desugar(std::shared_ptr<ast::Expr> left_expression, std::shared_ptr<ast::Expr> right_expression) noexcept{return [&]() -> std::shared_ptr<ast::Expr> { if (std::holds_alternative<ast::ExprCall>((*right_expression)._)) { auto _v_exprcall = std::get<ast::ExprCall>((*right_expression)._); auto [callee, existing_args, _w0] = _v_exprcall; return std::make_shared<ast::Expr>(ast::ExprCall(callee, mlc::Array<std::shared_ptr<ast::Expr>>{left_expression}.concat(existing_args), ast::span_unknown())); } return [&]() -> std::shared_ptr<ast::Expr> { 
  mlc::Array<std::shared_ptr<ast::Expr>> call_args = mlc::Array<std::shared_ptr<ast::Expr>>{left_expression};
  return std::make_shared<ast::Expr>(ast::ExprCall(right_expression, call_args, ast::span_unknown()));
 }(); }();}

predicates::ExprResult parse_pipe(predicates::Parser parser) noexcept{
predicates::ExprResult left = parse_comparison_relational(parser);
std::shared_ptr<ast::Expr> expression = left.value;
predicates::Parser state = left.parser;
while (predicates::TokenKind_is_pipe(predicates::Parser_kind(state))){
{
predicates::ExprResult right = parse_comparison_relational(predicates::Parser_advance(state));
expression = pipe_desugar(expression, right.value);
state = right.parser;
}
}
return predicates::expression_parse_result(expression, state);
}

predicates::ExprResult parse_or(predicates::Parser parser) noexcept{
predicates::ExprResult left = parse_and(parser);
std::shared_ptr<ast::Expr> expression = left.value;
predicates::Parser state = left.parser;
while (predicates::TokenKind_is_op(predicates::Parser_kind(state)) && predicates::TokenKind_op_val(predicates::Parser_kind(state)) == mlc::String("||")){
{
ast::Span operator_span = predicates::Parser_span_at_cursor(state);
predicates::ExprResult right = parse_and(predicates::Parser_advance(state));
expression = std::make_shared<ast::Expr>(ast::ExprBin(mlc::String("||"), expression, right.value, operator_span));
state = right.parser;
}
}
return predicates::expression_parse_result(expression, state);
}

predicates::ExprResult parse_and(predicates::Parser parser) noexcept{
predicates::ExprResult left = parse_equality(parser);
std::shared_ptr<ast::Expr> expression = left.value;
predicates::Parser state = left.parser;
while (predicates::TokenKind_is_op(predicates::Parser_kind(state)) && predicates::TokenKind_op_val(predicates::Parser_kind(state)) == mlc::String("&&")){
{
ast::Span operator_span = predicates::Parser_span_at_cursor(state);
predicates::ExprResult right = parse_equality(predicates::Parser_advance(state));
expression = std::make_shared<ast::Expr>(ast::ExprBin(mlc::String("&&"), expression, right.value, operator_span));
state = right.parser;
}
}
return predicates::expression_parse_result(expression, state);
}

predicates::ExprResult parse_equality(predicates::Parser parser) noexcept{
predicates::ExprResult left = parse_pipe(parser);
std::shared_ptr<ast::Expr> expression = left.value;
predicates::Parser state = left.parser;
bool go = true;
while (go){
{
ast_tokens::TokenKind kind = predicates::Parser_kind(state);
if (predicates::TokenKind_is_op(kind) && predicates::TokenKind_op_val(kind) == mlc::String("==") || predicates::TokenKind_op_val(kind) == mlc::String("!=")){
{
ast::Span operator_span = predicates::Parser_span_at_cursor(state);
mlc::String operator_value = predicates::TokenKind_op_val(kind);
predicates::ExprResult right = parse_pipe(predicates::Parser_advance(state));
expression = std::make_shared<ast::Expr>(ast::ExprBin(operator_value, expression, right.value, operator_span));
state = right.parser;
}
} else {
{
go = false;
}
}
}
}
return predicates::expression_parse_result(expression, state);
}

bool is_relational_comparison_operator(mlc::String operator_) noexcept{return operator_ == mlc::String("<") || operator_ == mlc::String(">") || operator_ == mlc::String("<=") || operator_ == mlc::String(">=");}

predicates::ExprResult parse_comparison_relational(predicates::Parser parser) noexcept{
predicates::ExprResult left = parse_add(parser);
std::shared_ptr<ast::Expr> expression = left.value;
predicates::Parser state = left.parser;
bool go = true;
while (go){
{
ast_tokens::TokenKind kind = predicates::Parser_kind(state);
if (predicates::TokenKind_is_op(kind) && is_relational_comparison_operator(predicates::TokenKind_op_val(kind))){
{
ast::Span operator_span = predicates::Parser_span_at_cursor(state);
mlc::String operator_value = predicates::TokenKind_op_val(kind);
predicates::ExprResult right = parse_add(predicates::Parser_advance(state));
expression = std::make_shared<ast::Expr>(ast::ExprBin(operator_value, expression, right.value, operator_span));
state = right.parser;
}
} else {
{
go = false;
}
}
}
}
return predicates::expression_parse_result(expression, state);
}

predicates::ExprResult parse_add(predicates::Parser parser) noexcept{
predicates::ExprResult left = parse_mul(parser);
std::shared_ptr<ast::Expr> expression = left.value;
predicates::Parser state = left.parser;
bool go = true;
while (go){
{
ast_tokens::TokenKind kind = predicates::Parser_kind(state);
if (predicates::TokenKind_is_op(kind) && predicates::TokenKind_op_val(kind) == mlc::String("+") || predicates::TokenKind_op_val(kind) == mlc::String("-")){
{
ast::Span operator_span = predicates::Parser_span_at_cursor(state);
predicates::ExprResult right = parse_mul(predicates::Parser_advance(state));
expression = std::make_shared<ast::Expr>(ast::ExprBin(predicates::TokenKind_op_val(kind), expression, right.value, operator_span));
state = right.parser;
}
} else {
{
go = false;
}
}
}
}
return predicates::expression_parse_result(expression, state);
}

predicates::ExprResult parse_mul(predicates::Parser parser) noexcept{
predicates::ExprResult left = parse_unary(parser);
std::shared_ptr<ast::Expr> expression = left.value;
predicates::Parser state = left.parser;
bool go = true;
while (go){
{
ast_tokens::TokenKind kind = predicates::Parser_kind(state);
if (predicates::TokenKind_is_op(kind) && predicates::TokenKind_op_val(kind) == mlc::String("*") || predicates::TokenKind_op_val(kind) == mlc::String("/") || predicates::TokenKind_op_val(kind) == mlc::String("%")){
{
ast::Span operator_span = predicates::Parser_span_at_cursor(state);
predicates::ExprResult right = parse_unary(predicates::Parser_advance(state));
expression = std::make_shared<ast::Expr>(ast::ExprBin(predicates::TokenKind_op_val(kind), expression, right.value, operator_span));
state = right.parser;
}
} else {
{
go = false;
}
}
}
}
return predicates::expression_parse_result(expression, state);
}

predicates::ExprResult parse_unary(predicates::Parser parser) noexcept{
ast_tokens::TokenKind kind = predicates::Parser_kind(parser);
return predicates::TokenKind_is_op(kind) && predicates::TokenKind_op_val(kind) == mlc::String("!") || predicates::TokenKind_op_val(kind) == mlc::String("-") ? [&]() -> predicates::ExprResult { 
  ast::Span operator_span = predicates::Parser_span_at_cursor(parser);
  predicates::ExprResult inner = parse_unary(predicates::Parser_advance(parser));
  return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprUn(predicates::TokenKind_op_val(kind), inner.value, operator_span)), inner.parser);
 }() : parse_postfix(parser);
}

predicates::ExprResult parse_postfix(predicates::Parser parser) noexcept{
predicates::ExprResult base = parse_primary(parser);
std::shared_ptr<ast::Expr> expression = base.value;
predicates::Parser state = base.parser;
bool go = true;
while (go){
{
ast_tokens::TokenKind kind = predicates::Parser_kind(state);
if (predicates::TokenKind_is_dot(kind)){
{
ast::Span dot_span = predicates::Parser_span_at_cursor(state);
mlc::String field_name = predicates::TokenKind_ident(predicates::Parser_kind(predicates::Parser_advance(state)));
if (predicates::TokenKind_is_lparen(predicates::Parser_kind(predicates::Parser_advance_by(state, 2))) && predicates::Parser_span_at_cursor(predicates::Parser_advance_by(state, 2)).line == predicates::Parser_span_at_cursor(predicates::Parser_advance_by(state, 1)).line){
predicates::ExprsResult margs = parse_arg_list(predicates::Parser_advance_by(state, 3));
expression = std::make_shared<ast::Expr>(ast::ExprMethod(expression, field_name, margs.value, dot_span));
state = margs.parser;
} else {
expression = std::make_shared<ast::Expr>(ast::ExprField(expression, field_name, dot_span));
state = predicates::Parser_advance_by(state, 2);
}
}
} else {
{
if (predicates::TokenKind_is_lparen(kind) && predicates::Parser_span_at_cursor(state).line == predicates::Parser_prev_line(state)){
ast::Span call_span = predicates::Parser_span_at_cursor(state);
predicates::ExprsResult call_args = parse_arg_list(predicates::Parser_advance(state));
expression = std::make_shared<ast::Expr>(ast::ExprCall(expression, call_args.value, call_span));
state = call_args.parser;
} else {
if (predicates::TokenKind_is_lbracket(kind)){
ast::Span bracket_span = predicates::Parser_span_at_cursor(state);
predicates::ExprResult index = parse_expr(predicates::Parser_advance(state));
expression = std::make_shared<ast::Expr>(ast::ExprIndex(expression, index.value, bracket_span));
state = expect_close::parser_expect_rbracket(index.parser, mlc::String("index expression"));
} else {
if (predicates::TokenKind_is_question(kind)){
ast::Span question_span = predicates::Parser_span_at_cursor(state);
expression = std::make_shared<ast::Expr>(ast::ExprQuestion(expression, question_span));
state = predicates::Parser_advance(state);
} else {
go = false;
}
}
}
}
}
}
}
return predicates::expression_parse_result(expression, state);
}

predicates::ExprsResult parse_arg_list(predicates::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::Expr>> expressions = {};
return predicates::TokenKind_is_rparen(predicates::Parser_kind(parser)) ? predicates::expressions_parse_result(expressions, predicates::Parser_advance(parser)) : [&]() -> predicates::ExprsResult { 
  predicates::Parser state = std::move(parser);
  while (!predicates::TokenKind_is_rparen(predicates::Parser_kind(state)) && !predicates::Parser_at_eof(state)){
{
ast::Span arg_span = predicates::Parser_span_at_cursor(state);
std::shared_ptr<ast::Expr> argument = predicates::TokenKind_is_ident(predicates::Parser_kind(state)) && predicates::TokenKind_is_colon(parser_next_kind(state)) ? [&]() -> std::shared_ptr<ast::Expr> { 
  mlc::String name = predicates::TokenKind_ident(predicates::Parser_kind(state));
  predicates::ExprResult value = parse_expr(predicates::Parser_advance_by(state, 2));
  state = value.parser;
  return std::make_shared<ast::Expr>(ast::ExprNamedArg(name, value.value, arg_span));
 }() : [&]() -> std::shared_ptr<ast::Expr> { 
  predicates::ExprResult value_parsed = parse_expr(state);
  state = value_parsed.parser;
  return value_parsed.value;
 }();
expressions.push_back(argument);
if (predicates::TokenKind_is_comma(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
}
}
}
}
  state = expect_close::parser_expect_rparen(state, mlc::String("argument list"));
  return predicates::expressions_parse_result(expressions, state);
 }();
}

std::shared_ptr<ast::Expr> build_template_expr(mlc::Array<mlc::String> parts) noexcept{
std::shared_ptr<ast::Expr> result = std::make_shared<ast::Expr>(ast::ExprStr(mlc::String(""), ast::span_unknown()));
int pi = 0;
while (pi < parts.size()){
{
std::shared_ptr<ast::Expr> part_expr = pi % 2 == 0 ? std::make_shared<ast::Expr>(ast::ExprStr(parts[pi], ast::span_unknown())) : parts[pi].length() == 0 ? std::make_shared<ast::Expr>(ast::ExprStr(mlc::String(""), ast::span_unknown())) : [&]() -> std::shared_ptr<ast::Expr> { 
  ast_tokens::LexOut sub_lex = lexer::tokenize(parts[pi]);
  predicates::ExprResult sub_parsed = parse_expr(predicates::parser_new(sub_lex.tokens));
  return std::make_shared<ast::Expr>(ast::ExprMethod(sub_parsed.value, mlc::String("to_string"), {}, ast::span_unknown()));
 }();
if (pi == 0){
{
result = part_expr;
}
} else {
{
result = std::make_shared<ast::Expr>(ast::ExprBin(mlc::String("+"), result, part_expr, ast::span_unknown()));
}
}
pi = pi + 1;
}
}
return result;
}

predicates::ExprResult parse_primary_integer_literal(predicates::Parser parser, int value) noexcept{
ast::Span source_span = predicates::Parser_span_at_cursor(parser);
return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprInt(value, source_span)), predicates::Parser_advance(parser));
}

predicates::ExprResult parse_primary_string_literal(predicates::Parser parser, mlc::String value) noexcept{
ast::Span source_span = predicates::Parser_span_at_cursor(parser);
return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprStr(value, source_span)), predicates::Parser_advance(parser));
}

predicates::ExprResult parse_primary_template_literal(predicates::Parser parser, mlc::Array<mlc::String> parts) noexcept{return predicates::expression_parse_result(build_template_expr(parts), predicates::Parser_advance(parser));}

predicates::ExprResult parse_primary_boolean_literal(predicates::Parser parser, bool value) noexcept{
ast::Span source_span = predicates::Parser_span_at_cursor(parser);
return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprBool(value, source_span)), predicates::Parser_advance(parser));
}

predicates::ExprResult parse_primary_parenthesized(predicates::Parser parser) noexcept{
ast::Span open_paren_span = predicates::Parser_span_at_cursor(parser);
return predicates::TokenKind_is_rparen(predicates::Parser_kind(predicates::Parser_advance(parser))) ? [&]() -> predicates::ExprResult { 
  predicates::Parser after_close = predicates::Parser_advance_by(parser, 2);
  return predicates::TokenKind_is_fat_arrow(predicates::Parser_kind(after_close)) && !predicates::Parser_lambda_shorthand_suppression_active(parser) ? [&]() -> predicates::ExprResult { 
  mlc::Array<mlc::String> params = {};
  predicates::ExprResult body = parse_expr(predicates::Parser_advance_by(parser, 3));
  return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprLambda(params, body.value, open_paren_span)), body.parser);
 }() : predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprUnit(open_paren_span)), after_close);
 }() : !predicates::Parser_lambda_shorthand_suppression_active(parser) && looks_like_typed_lambda_params(predicates::Parser_advance(parser)) ? [&]() -> predicates::ExprResult { 
  predicates::NamesResult params_parsed = parse_typed_lambda_params(predicates::Parser_advance(parser));
  predicates::ExprResult body = parse_expr(predicates::Parser_advance(params_parsed.parser));
  return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprLambda(params_parsed.value, body.value, open_paren_span)), body.parser);
 }() : !predicates::Parser_lambda_shorthand_suppression_active(parser) && looks_like_lambda_params(predicates::Parser_advance(parser)) ? [&]() -> predicates::ExprResult { 
  predicates::NamesResult params_parsed = parse_lambda_params(predicates::Parser_advance(parser));
  predicates::ExprResult body = parse_expr(predicates::Parser_advance(params_parsed.parser));
  return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprLambda(params_parsed.value, body.value, open_paren_span)), body.parser);
 }() : [&]() -> predicates::ExprResult { 
  predicates::ExprResult first_expression_parsed = parse_expr(predicates::Parser_advance(parser));
  return predicates::TokenKind_is_comma(predicates::Parser_kind(first_expression_parsed.parser)) ? [&]() -> predicates::ExprResult { 
  predicates::ExprsResult suffix = comma_separated::comma_separated_exprs_suffix_from_first(first_expression_parsed, parse_expr);
  return predicates::TokenKind_is_rparen(predicates::Parser_kind(suffix.parser)) ? predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprTuple(suffix.value, open_paren_span)), predicates::Parser_advance(suffix.parser)) : [&]() -> predicates::ExprResult { 
  predicates::Parser error_parser = predicates::Parser_record_parse_error(suffix.parser, mlc::String("parse: expected ) in tuple literal"));
  return predicates::expression_parse_result(first_expression_parsed.value, error_parser);
 }();
 }() : [&]() -> predicates::ExprResult { 
  predicates::Parser closed_parser = expect_close::parser_expect_rparen(first_expression_parsed.parser, mlc::String("parenthesized expression"));
  return predicates::expression_parse_result(first_expression_parsed.value, closed_parser);
 }();
 }();
}

predicates::ExprResult parse_primary_if_or_unless(predicates::Parser parser) noexcept{return parse_if_expr(parser);}

predicates::ExprResult parse_primary_do_block(predicates::Parser parser) noexcept{
ast::Span header_span = predicates::Parser_span_at_cursor(parser);
return parse_block(predicates::Parser_advance(parser), header_span);
}

predicates::ExprResult parse_primary_while_loop(predicates::Parser parser) noexcept{
ast::Span header_span = predicates::Parser_span_at_cursor(parser);
return parse_while_expr(predicates::Parser_advance(parser), header_span);
}

predicates::ExprResult parse_primary_for_loop(predicates::Parser parser) noexcept{
ast::Span header_span = predicates::Parser_span_at_cursor(parser);
return parse_for_expr(predicates::Parser_advance(parser), header_span);
}

predicates::ExprResult parse_spawn_expr(predicates::Parser parser, ast::Span header_span) noexcept{
return predicates::TokenKind_is_do(predicates::Parser_kind(parser)) ? [&]() -> predicates::ExprResult { 
  predicates::StmtsResult body_parsed = parse_statements_until_end(predicates::Parser_advance(parser));
  return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprSpawn(body_parsed.value, header_span)), body_parsed.parser);
 }() : predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprUnit(header_span)), predicates::Parser_record_parse_error(parser, mlc::String("spawn: expected do")));
}

predicates::ExprResult parse_primary_spawn(predicates::Parser parser) noexcept{
ast::Span header_span = predicates::Parser_span_at_cursor(parser);
return parse_spawn_expr(predicates::Parser_advance(parser), header_span);
}

predicates::ExprResult parse_primary_match(predicates::Parser parser) noexcept{
ast::Span header_span = predicates::Parser_span_at_cursor(parser);
return parse_match_expr(predicates::Parser_advance(parser), header_span);
}

predicates::ExprResult parse_primary_return_as_block(predicates::Parser parser) noexcept{
ast::Span header_span = predicates::Parser_span_at_cursor(parser);
predicates::ExprResult value = parse_expr(predicates::Parser_advance(parser));
mlc::Array<std::shared_ptr<ast::Stmt>> statements = {};
statements.push_back(std::make_shared<ast::Stmt>(ast::StmtReturn(value.value, header_span)));
return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprBlock(statements, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())), header_span)), value.parser);
}

predicates::ExprResult parse_primary_identifier(predicates::Parser parser, mlc::String name) noexcept{
ast::Span source_span = predicates::Parser_span_at_cursor(parser);
predicates::Parser after_name = predicates::Parser_advance(parser);
return predicates::TokenKind_is_fat_arrow(predicates::Parser_kind(after_name)) && !predicates::Parser_lambda_shorthand_suppression_active(parser) ? [&]() -> predicates::ExprResult { 
  mlc::Array<mlc::String> parameters = mlc::Array<mlc::String>{name};
  predicates::ExprResult body = parse_expr(predicates::Parser_advance(after_name));
  return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprLambda(parameters, body.value, source_span)), body.parser);
 }() : predicates::is_ctor_name(name) && predicates::TokenKind_is_lbrace(predicates::Parser_kind(after_name)) ? [&]() -> predicates::ExprResult { 
  ast::Span brace_span = predicates::Parser_span_at_cursor(after_name);
  return parse_record_expr(predicates::Parser_advance(after_name), name, brace_span);
 }() : predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprIdent(name, source_span)), after_name);
}

predicates::ExprResult parse_primary_unit_fallback(predicates::Parser parser) noexcept{
ast::Span source_span = predicates::Parser_span_at_cursor(parser);
predicates::Parser error_parser = predicates::Parser_record_parse_error(parser, mlc::String("parse: unexpected token in expression"));
return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprUnit(source_span)), predicates::Parser_advance(error_parser));
}

predicates::ExprResult parse_primary(predicates::Parser parser) noexcept{
ast_tokens::TokenKind kind = predicates::Parser_kind(parser);
return predicates::TokenKind_is_float(kind) ? [&]() -> predicates::ExprResult { 
  ast::Span source_span = predicates::Parser_span_at_cursor(parser);
  return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprFloat(predicates::TokenKind_float_val(kind), source_span)), predicates::Parser_advance(parser));
 }() : predicates::TokenKind_is_i64(kind) ? [&]() -> predicates::ExprResult { 
  ast::Span source_span = predicates::Parser_span_at_cursor(parser);
  return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprI64(predicates::TokenKind_i64_val(kind), source_span)), predicates::Parser_advance(parser));
 }() : predicates::TokenKind_is_u8(kind) ? [&]() -> predicates::ExprResult { 
  ast::Span source_span = predicates::Parser_span_at_cursor(parser);
  return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprU8(predicates::TokenKind_u8_val(kind), source_span)), predicates::Parser_advance(parser));
 }() : predicates::TokenKind_is_usize(kind) ? [&]() -> predicates::ExprResult { 
  ast::Span source_span = predicates::Parser_span_at_cursor(parser);
  return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprUsize(predicates::TokenKind_usize_val(kind), source_span)), predicates::Parser_advance(parser));
 }() : predicates::TokenKind_is_char_lit(kind) ? [&]() -> predicates::ExprResult { 
  ast::Span source_span = predicates::Parser_span_at_cursor(parser);
  return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprChar(predicates::TokenKind_char_val(kind), source_span)), predicates::Parser_advance(parser));
 }() : predicates::TokenKind_is_int(kind) ? parse_primary_integer_literal(parser, predicates::TokenKind_int_val(kind)) : predicates::TokenKind_is_str(kind) ? parse_primary_string_literal(parser, predicates::TokenKind_str_val(kind)) : predicates::TokenKind_is_template(kind) ? parse_primary_template_literal(parser, predicates::TokenKind_template_parts(kind)) : predicates::TokenKind_is_true(kind) ? parse_primary_boolean_literal(parser, true) : predicates::TokenKind_is_false(kind) ? parse_primary_boolean_literal(parser, false) : predicates::TokenKind_is_lparen(kind) ? parse_primary_parenthesized(parser) : predicates::TokenKind_is_lbracket(kind) ? [&]() -> predicates::ExprResult { 
  ast::Span bracket_span = predicates::Parser_span_at_cursor(parser);
  return parse_array_lit(predicates::Parser_advance(parser), bracket_span);
 }() : predicates::TokenKind_is_if(kind) || predicates::TokenKind_is_unless(kind) ? parse_primary_if_or_unless(parser) : predicates::TokenKind_is_do(kind) ? parse_primary_do_block(parser) : predicates::TokenKind_is_while(kind) ? parse_primary_while_loop(parser) : predicates::TokenKind_is_for(kind) ? parse_primary_for_loop(parser) : predicates::TokenKind_is_with(kind) ? [&]() -> predicates::ExprResult { 
  ast::Span header_span = predicates::Parser_span_at_cursor(parser);
  return parse_with_expr(predicates::Parser_advance(parser), header_span);
 }() : predicates::TokenKind_is_match(kind) ? parse_primary_match(parser) : predicates::TokenKind_is_spawn(kind) ? parse_primary_spawn(parser) : predicates::TokenKind_is_return(kind) ? parse_primary_return_as_block(parser) : predicates::TokenKind_is_ident(kind) ? parse_primary_identifier(parser, predicates::TokenKind_ident(kind)) : parse_primary_unit_fallback(parser);
}

bool looks_like_typed_lambda_params(predicates::Parser parser) noexcept{return predicates::TokenKind_is_ident(predicates::Parser_kind(parser)) && predicates::TokenKind_is_colon(predicates::Parser_kind(predicates::Parser_advance(parser)));}

predicates::NamesResult parse_typed_lambda_params(predicates::Parser parser) noexcept{
mlc::Array<mlc::String> names = {};
predicates::Parser state = std::move(parser);
names.push_back(predicates::TokenKind_ident(predicates::Parser_kind(state)));
state = types::parse_type(predicates::Parser_advance_by(state, 2)).parser;
while (predicates::TokenKind_is_comma(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
names.push_back(predicates::TokenKind_ident(predicates::Parser_kind(state)));
state = types::parse_type(predicates::Parser_advance_by(state, 2)).parser;
}
}
return predicates::bounds_parse_result(names, predicates::Parser_advance(state));
}

bool looks_like_lambda_params(predicates::Parser parser) noexcept{
predicates::Parser state = std::move(parser);
bool ok = true;
if (!predicates::TokenKind_is_ident(predicates::Parser_kind(state))){
{
ok = false;
}
} else {
{
state = predicates::Parser_advance(state);
while (predicates::TokenKind_is_comma(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
if (!predicates::TokenKind_is_ident(predicates::Parser_kind(state))){
{
ok = false;
state = predicates::Parser_advance(state);
}
} else {
{
state = predicates::Parser_advance(state);
}
}
}
}
if (!predicates::TokenKind_is_rparen(predicates::Parser_kind(state))){
ok = false;
} else {
ok = predicates::TokenKind_is_fat_arrow(predicates::Parser_kind(predicates::Parser_advance(state)));
}
}
}
return ok;
}

predicates::NamesResult parse_lambda_params(predicates::Parser parser) noexcept{
mlc::Array<mlc::String> names = {};
predicates::Parser state = std::move(parser);
names.push_back(predicates::TokenKind_ident(predicates::Parser_kind(state)));
state = predicates::Parser_advance(state);
while (predicates::TokenKind_is_comma(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
names.push_back(predicates::TokenKind_ident(predicates::Parser_kind(state)));
state = predicates::Parser_advance(state);
}
}
return predicates::bounds_parse_result(names, predicates::Parser_advance(state));
}

predicates::ExprResult parse_array_lit(predicates::Parser parser, ast::Span header_span) noexcept{return predicates::TokenKind_is_rbracket(predicates::Parser_kind(parser)) ? predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprArray({}, header_span)), predicates::Parser_advance(parser)) : [&]() -> predicates::ExprResult { 
  predicates::ExprsResult suffix = comma_separated::comma_separated_exprs_suffix_from_first(parse_expr(parser), parse_expr);
  predicates::Parser closed_parser = expect_close::parser_expect_rbracket(suffix.parser, mlc::String("array literal"));
  return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprArray(suffix.value, header_span)), closed_parser);
 }();}

std::shared_ptr<ast::Expr> if_expression_condition(bool is_negated, std::shared_ptr<ast::Expr> expression) noexcept{return is_negated ? std::make_shared<ast::Expr>(ast::ExprUn(mlc::String("!"), expression, ast::span_unknown())) : expression;}

predicates::Parser parser_after_optional_end(predicates::Parser parser) noexcept{return predicates::TokenKind_is_end(predicates::Parser_kind(parser)) ? predicates::Parser_advance(parser) : parser;}

predicates::ExprResult parse_if_expr(predicates::Parser parser) noexcept{
ast::Span header_span = predicates::Parser_span_at_cursor(parser);
bool is_negated = predicates::TokenKind_is_unless(predicates::Parser_kind(parser));
predicates::ExprResult cond_parsed = parse_expr(predicates::Parser_advance(parser));
std::shared_ptr<ast::Expr> expression = cond_parsed.value;
std::shared_ptr<ast::Expr> condition = if_expression_condition(is_negated, expression);
predicates::StmtsResult then_parsed = parse_statements_until_else_end(predicates::Parser_advance(cond_parsed.parser));
std::shared_ptr<ast::Expr> then_expr = statements_result_to_block_expr(then_parsed, header_span);
if (!predicates::TokenKind_is_else(predicates::Parser_kind(then_parsed.parser))){
{
return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprIf(condition, then_expr, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())), header_span)), predicates::Parser_advance(then_parsed.parser));
}
}
predicates::Parser after_else = predicates::Parser_advance(then_parsed.parser);
if (predicates::TokenKind_is_if(predicates::Parser_kind(after_else)) || predicates::TokenKind_is_unless(predicates::Parser_kind(after_else))){
{
predicates::ExprResult else_parsed = parse_if_expr(after_else);
return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprIf(condition, then_expr, else_parsed.value, header_span)), else_parsed.parser);
}
}
if (predicates::TokenKind_is_do(predicates::Parser_kind(after_else))){
{
predicates::ExprResult else_block_parsed = parse_block(predicates::Parser_advance(after_else), header_span);
return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprIf(condition, then_expr, else_block_parsed.value, header_span)), parser_after_optional_end(else_block_parsed.parser));
}
}
int else_keyword_line = predicates::Parser_prev_line(after_else);
int else_body_start_line = predicates::Parser_span_at_cursor(after_else).line;
if (else_body_start_line > else_keyword_line){
{
ast::Span else_span = predicates::Parser_span_at_cursor(after_else);
predicates::StmtsResult else_stmts_parsed = parse_statements_until_end_eof_or_toplevel_decl(after_else);
return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprIf(condition, then_expr, statements_result_to_block_expr(else_stmts_parsed, else_span), header_span)), else_stmts_parsed.parser);
}
}
predicates::ExprResult else_parsed = parse_expr(after_else);
return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprIf(condition, then_expr, else_parsed.value, header_span)), parser_after_optional_end(else_parsed.parser));
}

predicates::ExprResult parse_block(predicates::Parser parser, ast::Span header_span) noexcept{
predicates::StmtsResult statements_parsed = parse_statements_until_end(parser);
return predicates::expression_parse_result(statements_result_to_block_expr(statements_parsed, header_span), statements_parsed.parser);
}

bool is_unit_expr(std::shared_ptr<ast::Expr> expression) noexcept{return [&]() { if (std::holds_alternative<ast::ExprUnit>((*expression)._)) { auto _v_exprunit = std::get<ast::ExprUnit>((*expression)._); auto [_w0] = _v_exprunit; return true; } if (std::holds_alternative<ast::ExprBlock>((*expression)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expression)._); auto [_w0, result, _w1] = _v_exprblock; return is_unit_expr(result); } if (std::holds_alternative<ast::ExprIf>((*expression)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expression)._); auto [_w0, _w1, else_expression, _w2] = _v_exprif; return is_unit_expr(else_expression); } if (std::holds_alternative<ast::ExprBin>((*expression)._)) { auto _v_exprbin = std::get<ast::ExprBin>((*expression)._); auto [op, _w0, _w1, _w2] = _v_exprbin; return op == mlc::String("="); } return false; }();}

bool is_stmt_expr(std::shared_ptr<ast::Stmt> statement) noexcept{return [&]() { if (std::holds_alternative<ast::StmtExpr>((*statement)._)) { auto _v_stmtexpr = std::get<ast::StmtExpr>((*statement)._); auto [_w0, _w1] = _v_stmtexpr; return true; } return false; }();}

std::shared_ptr<ast::Expr> stmt_expr_expression(std::shared_ptr<ast::Stmt> statement) noexcept{return [&]() -> std::shared_ptr<ast::Expr> { if (std::holds_alternative<ast::StmtExpr>((*statement)._)) { auto _v_stmtexpr = std::get<ast::StmtExpr>((*statement)._); auto [expression, _w0] = _v_stmtexpr; return expression; } return std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())); }();}

std::shared_ptr<ast::Expr> block_result(mlc::Array<std::shared_ptr<ast::Stmt>> stmts) noexcept{
if (stmts.size() == 0){
{
return std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown()));
}
}
std::shared_ptr<ast::Stmt> last = stmts[stmts.size() - 1];
if (!is_stmt_expr(last)){
{
return std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown()));
}
}
std::shared_ptr<ast::Expr> expression = stmt_expr_expression(last);
if (is_unit_expr(expression)){
{
return std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown()));
}
}
return expression;
}

mlc::Array<std::shared_ptr<ast::Stmt>> block_body_without_last_statement(mlc::Array<std::shared_ptr<ast::Stmt>> stmts) noexcept{
mlc::Array<std::shared_ptr<ast::Stmt>> result = {};
int index = 0;
while (index < stmts.size() - 1){
{
result.push_back(stmts[index]);
index = index + 1;
}
}
return result;
}

mlc::Array<std::shared_ptr<ast::Stmt>> block_body(mlc::Array<std::shared_ptr<ast::Stmt>> stmts) noexcept{
if (stmts.size() == 0){
{
return stmts;
}
}
std::shared_ptr<ast::Stmt> last = stmts[stmts.size() - 1];
if (!is_stmt_expr(last)){
{
return stmts;
}
}
std::shared_ptr<ast::Expr> expression = stmt_expr_expression(last);
if (is_unit_expr(expression)){
{
return stmts;
}
}
return block_body_without_last_statement(stmts);
}

std::shared_ptr<ast::Expr> statements_result_to_block_expr(predicates::StmtsResult statements_result, ast::Span header_span) noexcept{return std::make_shared<ast::Expr>(ast::ExprBlock(block_body(statements_result.value), block_result(statements_result.value), header_span));}

predicates::ExprResult parse_while_expr(predicates::Parser parser, ast::Span header_span) noexcept{
predicates::ExprResult cond_parsed = parse_expr(parser);
std::shared_ptr<ast::Expr> expression = cond_parsed.value;
predicates::StmtsResult body_parsed = parse_statements_until_end(predicates::Parser_advance(cond_parsed.parser));
mlc::Array<std::shared_ptr<ast::Stmt>> block_statements = body_parsed.value;
return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprWhile(expression, block_statements, header_span)), body_parsed.parser);
}

predicates::ExprResult parse_for_expr(predicates::Parser parser, ast::Span header_span) noexcept{
mlc::String var_name = predicates::TokenKind_ident(predicates::Parser_kind(parser));
predicates::ExprResult iter_parsed = parse_expr(predicates::Parser_advance_by(parser, 2));
std::shared_ptr<ast::Expr> expression = iter_parsed.value;
predicates::StmtsResult body_parsed = parse_statements_until_end(predicates::Parser_advance(iter_parsed.parser));
mlc::Array<std::shared_ptr<ast::Stmt>> block_statements = body_parsed.value;
return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprFor(var_name, expression, block_statements, header_span)), body_parsed.parser);
}

predicates::ExprResult parse_with_expr(predicates::Parser parser, ast::Span header_span) noexcept{
predicates::ExprResult resource_parsed = parse_expr(parser);
std::shared_ptr<ast::Expr> expression = resource_parsed.value;
predicates::Parser after_as = predicates::Parser_advance(resource_parsed.parser);
mlc::String binder = predicates::TokenKind_ident(predicates::Parser_kind(after_as));
predicates::StmtsResult body_parsed = parse_statements_until_end(predicates::Parser_advance(predicates::Parser_advance(after_as)));
mlc::Array<std::shared_ptr<ast::Stmt>> block_statements = body_parsed.value;
return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprWith(expression, binder, block_statements, header_span)), body_parsed.parser);
}

predicates::ExprResult parse_match_expr(predicates::Parser parser, ast::Span header_span) noexcept{
predicates::ExprResult subject_parsed = parse_expr(parser);
std::shared_ptr<ast::Expr> expression = subject_parsed.value;
ast_tokens::TokenKind next = predicates::Parser_kind(subject_parsed.parser);
return predicates::TokenKind_is_lbrace(next) ? [&]() -> predicates::ExprResult { 
  predicates::ArmsResult arms_parsed = parse_arms_brace(predicates::Parser_advance(subject_parsed.parser));
  return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprMatch(expression, arms_parsed.value, header_span)), arms_parsed.parser);
 }() : predicates::TokenKind_is_do(next) ? [&]() -> predicates::ExprResult { 
  predicates::ArmsResult arms_parsed = parse_arms_do_delimited(predicates::Parser_advance(subject_parsed.parser));
  return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprMatch(expression, arms_parsed.value, header_span)), arms_parsed.parser);
 }() : [&]() -> predicates::ExprResult { 
  predicates::ArmsResult arms_parsed = parse_arms_pipe(subject_parsed.parser);
  return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprMatch(expression, arms_parsed.value, header_span)), arms_parsed.parser);
 }();
}

exprs::MatchArmParseOutcome match_arm_parse_result(std::shared_ptr<ast::MatchArm> arm, predicates::Parser parser) noexcept{return predicates::parse_result<std::shared_ptr<ast::MatchArm>>(arm, parser);}

exprs::MatchArmParseOutcome parse_match_arm(predicates::PatternResult pat_parsed) noexcept{
predicates::Parser after_pattern = pat_parsed.parser;
bool has_guard = false;
std::shared_ptr<ast::Expr> guard_expression = std::make_shared<ast::Expr>(ast::ExprBool(true, ast::span_unknown()));
if (predicates::TokenKind_is_if(predicates::Parser_kind(after_pattern))){
{
predicates::ExprResult guard_parsed = parse_expr(predicates::Parser_with_lambda_shorthand_suppressed(predicates::Parser_advance(after_pattern), true));
std::shared_ptr<ast::Expr> expression = guard_parsed.value;
guard_expression = expression;
has_guard = true;
after_pattern = guard_parsed.parser;
}
}
predicates::Parser after_fat_arrow = predicates::Parser_advance(after_pattern);
predicates::ExprResult body_parsed = parse_expr(after_fat_arrow);
std::shared_ptr<ast::Expr> expression = body_parsed.value;
return match_arm_parse_result(std::make_shared<ast::MatchArm>(ast::MatchArm{pat_parsed.value, has_guard, guard_expression, expression}), body_parsed.parser);
}

predicates::ArmsResult parse_arms_brace(predicates::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::MatchArm>> arms = {};
predicates::Parser state = std::move(parser);
while (!predicates::TokenKind_is_rbrace(predicates::Parser_kind(state)) && !predicates::Parser_at_eof(state)){
{
if (predicates::TokenKind_is_bar(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
}
}
predicates::PatternResult pat_parsed = parse_or_pattern(state);
exprs::MatchArmParseOutcome arm_parsed = parse_match_arm(pat_parsed);
arms.push_back(arm_parsed.value);
state = arm_parsed.parser;
if (predicates::TokenKind_is_comma(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
}
}
}
}
state = expect_close::parser_expect_rbrace(state, mlc::String("match arms"));
return predicates::arms_parse_result(arms, state);
}

predicates::ArmsResult parse_arms_do_delimited(predicates::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::MatchArm>> arms = {};
predicates::Parser state = predicates::Parser_skip_semi(parser);
while (!predicates::TokenKind_is_end(predicates::Parser_kind(state)) && !predicates::Parser_at_eof(state)){
{
if (predicates::TokenKind_is_bar(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
}
}
predicates::PatternResult pat_parsed = parse_or_pattern(state);
exprs::MatchArmParseOutcome arm_parsed = parse_match_arm(pat_parsed);
arms.push_back(arm_parsed.value);
state = predicates::Parser_skip_semi(arm_parsed.parser);
}
}
return predicates::arms_parse_result(arms, parser_after_optional_end(state));
}

predicates::ArmsResult parse_arms_pipe(predicates::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::MatchArm>> arms = {};
predicates::Parser state = std::move(parser);
while (predicates::TokenKind_is_bar(predicates::Parser_kind(state))){
{
predicates::PatternResult pat_parsed = parse_or_pattern(predicates::Parser_advance(state));
exprs::MatchArmParseOutcome arm_parsed = parse_match_arm(pat_parsed);
arms.push_back(arm_parsed.value);
state = arm_parsed.parser;
}
}
if (predicates::TokenKind_is_end(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
}
}
return predicates::arms_parse_result(arms, state);
}

predicates::ArmsResult parse_arms(predicates::Parser parser) noexcept{return parse_arms_brace(parser);}

predicates::Parser skip_comma_if_present(predicates::Parser parser) noexcept{return predicates::TokenKind_is_comma(predicates::Parser_kind(parser)) ? predicates::Parser_advance(parser) : parser;}

mlc::Array<ast::RecordLitPart> lit_parts_flush_fields(mlc::Array<ast::RecordLitPart> lit_parts, mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals) noexcept{
return field_vals.size() == 0 ? lit_parts : lit_parts.concat(mlc::Array<ast::RecordLitPart>{ast::RecordLitFields(field_vals)});
}

mlc::Array<std::shared_ptr<ast::FieldVal>> empty_record_lit_field_vals() noexcept{
mlc::Array<std::shared_ptr<ast::FieldVal>> empty_field_vals = {};
return empty_field_vals;
}

predicates::RecordLitPartsResult parse_record_lit_field_with_value(predicates::Parser state, mlc::Array<ast::RecordLitPart> lit_parts, mlc::Array<std::shared_ptr<ast::FieldVal>> current_field_vals, mlc::String field_name, predicates::Parser after_colon) noexcept{
predicates::ExprResult field_value_parsed = parse_expr(after_colon);
mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals_after = current_field_vals.concat(mlc::Array<std::shared_ptr<ast::FieldVal>>{std::make_shared<ast::FieldVal>(ast::FieldVal{field_name, field_value_parsed.value})});
return parse_record_lit_parts_loop(skip_comma_if_present(field_value_parsed.parser), lit_parts, field_vals_after);
}

predicates::RecordLitPartsResult parse_record_lit_field_shorthand(predicates::Parser state, mlc::Array<ast::RecordLitPart> lit_parts, mlc::Array<std::shared_ptr<ast::FieldVal>> current_field_vals, mlc::String field_name, ast::Span field_span, predicates::Parser after_field_name) noexcept{
mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals_after = current_field_vals.concat(mlc::Array<std::shared_ptr<ast::FieldVal>>{std::make_shared<ast::FieldVal>(ast::FieldVal{field_name, std::make_shared<ast::Expr>(ast::ExprIdent(field_name, field_span))})});
return parse_record_lit_parts_loop(skip_comma_if_present(after_field_name), lit_parts, field_vals_after);
}

predicates::RecordLitPartsResult parse_record_lit_field(predicates::Parser state, mlc::Array<ast::RecordLitPart> lit_parts, mlc::Array<std::shared_ptr<ast::FieldVal>> current_field_vals) noexcept{
ast::Span field_span = predicates::Parser_span_at_cursor(state);
mlc::String field_name = predicates::TokenKind_ident(predicates::Parser_kind(state));
predicates::Parser after_field_name = predicates::Parser_advance(state);
return predicates::TokenKind_is_colon(predicates::Parser_kind(after_field_name)) ? parse_record_lit_field_with_value(state, lit_parts, current_field_vals, field_name, predicates::Parser_advance(after_field_name)) : parse_record_lit_field_shorthand(state, lit_parts, current_field_vals, field_name, field_span, after_field_name);
}

predicates::RecordLitPartsResult parse_record_lit_parts_loop(predicates::Parser state, mlc::Array<ast::RecordLitPart> lit_parts, mlc::Array<std::shared_ptr<ast::FieldVal>> current_field_vals) noexcept{
return predicates::TokenKind_is_rbrace(predicates::Parser_kind(state)) || predicates::Parser_at_eof(state) ? [&]() -> predicates::RecordLitPartsResult { 
  mlc::Array<ast::RecordLitPart> flushed_lit_parts = lit_parts_flush_fields(lit_parts, current_field_vals);
  predicates::Parser closed_parser = expect_close::parser_expect_rbrace(state, mlc::String("record literal"));
  return predicates::record_lit_parts_parse_result(flushed_lit_parts, closed_parser);
 }() : predicates::TokenKind_is_spread(predicates::Parser_kind(state)) ? [&]() -> predicates::RecordLitPartsResult { 
  mlc::Array<ast::RecordLitPart> lit_parts_after_flush = lit_parts_flush_fields(lit_parts, current_field_vals);
  predicates::ExprResult spread_parsed = parse_expr(predicates::Parser_advance(state));
  mlc::Array<ast::RecordLitPart> lit_parts_after_spread = lit_parts_after_flush.concat(mlc::Array<ast::RecordLitPart>{ast::RecordLitSpread(spread_parsed.value)});
  return parse_record_lit_parts_loop(skip_comma_if_present(spread_parsed.parser), lit_parts_after_spread, empty_record_lit_field_vals());
 }() : parse_record_lit_field(state, lit_parts, current_field_vals);
}

predicates::RecordLitPartsResult parse_record_lit_parts(predicates::Parser parser) noexcept{
mlc::Array<ast::RecordLitPart> empty_lit_parts = {};
return parse_record_lit_parts_loop(parser, empty_lit_parts, empty_record_lit_field_vals());
}

predicates::ExprResult parse_record_expr(predicates::Parser parser, mlc::String record_name, ast::Span record_span) noexcept{
predicates::RecordLitPartsResult parsed_lit_parts = parse_record_lit_parts(parser);
mlc::Array<ast::RecordLitPart> lit_parts = parsed_lit_parts.value;
predicates::Parser after_record_body = parsed_lit_parts.parser;
return lit_parts.size() == 2 ? [&]() -> predicates::ExprResult { 
  ast::RecordLitPart first_part = lit_parts[0];
  ast::RecordLitPart second_part = lit_parts[1];
  return std::visit(overloaded{
  [&](const RecordLitSpread& recordlitspread) -> predicates::ExprResult { auto [base_expression] = recordlitspread; return std::visit(overloaded{
  [&](const RecordLitFields& recordlitfields) -> predicates::ExprResult { auto [override_fields] = recordlitfields; return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprRecordUpdate(record_name, base_expression, override_fields, record_span)), after_record_body); },
  [&](const RecordLitSpread& recordlitspread) -> predicates::ExprResult { auto [_w0] = recordlitspread; return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprRecord(record_name, lit_parts, record_span)), after_record_body); }
}, second_part); },
  [&](const RecordLitFields& recordlitfields) -> predicates::ExprResult { auto [_w0] = recordlitfields; return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprRecord(record_name, lit_parts, record_span)), after_record_body); }
}, first_part);
 }() : predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprRecord(record_name, lit_parts, record_span)), after_record_body);
}

} // namespace exprs
