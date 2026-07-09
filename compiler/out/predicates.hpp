#ifndef PREDICATES_HPP
#define PREDICATES_HPP

#include "mlc.hpp"
#include <variant>
#include "ast_tokens.hpp"
#include "ast.hpp"
namespace predicates {

struct Parser {
  mlc::Array<ast_tokens::Token> tokens;
  int position;
  mlc::String source_path;
  bool suppress_lambda_shorthand;
  mlc::Array<mlc::String> errors;
  bool has_unget_angle_close;
};
template<typename Value>
struct ParseResult {
  Value value;
  Parser parser;
};
struct TypeParamsValue {
  mlc::Array<mlc::String> params;
  mlc::Array<mlc::Array<mlc::String>> bounds;
};
struct ProgramParseValue {
  ast::Program program;
  mlc::Array<mlc::String> errors;
};
Parser parser_new_with_source_path(mlc::Array<ast_tokens::Token> tokens, mlc::String source_path) noexcept;
Parser parser_new(mlc::Array<ast_tokens::Token> tokens) noexcept;
template<typename Value>
ParseResult<Value> parse_result(Value value, Parser parser) noexcept;
ParseResult<mlc::Array<mlc::String>> bounds_parse_result(mlc::Array<mlc::String> bounds, Parser parser) noexcept;
ParseResult<std::shared_ptr<ast::TypeExpr>> type_parse_result(std::shared_ptr<ast::TypeExpr> type_expression, Parser parser) noexcept;
ParseResult<std::shared_ptr<ast::Pattern>> pattern_parse_result(std::shared_ptr<ast::Pattern> pattern, Parser parser) noexcept;
ParseResult<std::shared_ptr<ast::Stmt>> statement_parse_result(std::shared_ptr<ast::Stmt> statement, Parser parser) noexcept;
ParseResult<std::shared_ptr<ast::TypeVariant>> variant_parse_result(std::shared_ptr<ast::TypeVariant> variant, Parser parser) noexcept;
ParseResult<mlc::Array<std::shared_ptr<ast::Stmt>>> statements_parse_result(mlc::Array<std::shared_ptr<ast::Stmt>> statements, Parser parser) noexcept;
ParseResult<mlc::Array<std::shared_ptr<ast::Expr>>> expressions_parse_result(mlc::Array<std::shared_ptr<ast::Expr>> expressions, Parser parser) noexcept;
ParseResult<mlc::Array<std::shared_ptr<ast::MatchArm>>> arms_parse_result(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, Parser parser) noexcept;
ParseResult<mlc::Array<std::shared_ptr<ast::TypeVariant>>> variants_parse_result(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, Parser parser) noexcept;
ParseResult<mlc::Array<std::shared_ptr<ast::Param>>> parameters_parse_result(mlc::Array<std::shared_ptr<ast::Param>> parameters, Parser parser) noexcept;
ParseResult<mlc::Array<std::shared_ptr<ast::FieldVal>>> field_values_parse_result(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, Parser parser) noexcept;
ParseResult<mlc::Array<ast::RecordLitPart>> record_lit_parts_parse_result(mlc::Array<ast::RecordLitPart> lit_parts, Parser parser) noexcept;
ParseResult<mlc::Array<std::shared_ptr<ast::Pattern>>> patterns_parse_result(mlc::Array<std::shared_ptr<ast::Pattern>> patterns, Parser parser) noexcept;
ParseResult<mlc::Array<std::shared_ptr<ast::FieldDef>>> field_definitions_parse_result(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions, Parser parser) noexcept;
ParseResult<mlc::Array<std::shared_ptr<ast::TypeExpr>>> types_parse_result(mlc::Array<std::shared_ptr<ast::TypeExpr>> types, Parser parser) noexcept;
ParseResult<std::shared_ptr<ast::Decl>> declaration_parse_result(std::shared_ptr<ast::Decl> declaration, Parser parser) noexcept;
ParseResult<mlc::Array<ast::WhereClauseBound>> where_clause_parse_result(mlc::Array<ast::WhereClauseBound> where_bounds, Parser parser) noexcept;
ParseResult<mlc::Array<std::shared_ptr<ast::Decl>>> trait_body_parse_result(mlc::Array<std::shared_ptr<ast::Decl>> methods, Parser parser) noexcept;
ParseResult<std::shared_ptr<ast::Expr>> expression_parse_result(std::shared_ptr<ast::Expr> expression, Parser parser) noexcept;
ParseResult<TypeParamsValue> type_params_parse_result(mlc::Array<mlc::String> params, mlc::Array<mlc::Array<mlc::String>> bounds, Parser parser) noexcept;
ProgramParseValue program_parse_result(ast::Program program, mlc::Array<mlc::String> errors) noexcept;
bool is_ctor_name(mlc::String name) noexcept;
bool TokenKind_is_fn(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_type(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_let(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_mut(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_const(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_if(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_unless(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_else(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_end(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_do(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_while(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_for(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_with(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_spawn(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_move(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_match(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_return(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_break(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_continue(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_true(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_false(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_extern(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_extend(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_where(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_import(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_as(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_from(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_eof(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_ident(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_int(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_str(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_template(ast_tokens::TokenKind self) noexcept;
mlc::Array<mlc::String> TokenKind_template_parts(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_equal(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_bar(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_pipe(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_colon(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_comma(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_dot(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_spread(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_arrow(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_fat_arrow(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_question(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_lparen(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_rparen(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_lbrace(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_rbrace(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_lbracket(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_rbracket(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_op(ast_tokens::TokenKind self) noexcept;
mlc::String TokenKind_ident(ast_tokens::TokenKind self) noexcept;
int TokenKind_int_val(ast_tokens::TokenKind self) noexcept;
mlc::String TokenKind_str_val(ast_tokens::TokenKind self) noexcept;
mlc::String TokenKind_op_val(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_float(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_i64(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_u8(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_usize(ast_tokens::TokenKind self) noexcept;
bool TokenKind_is_char_lit(ast_tokens::TokenKind self) noexcept;
mlc::String TokenKind_float_val(ast_tokens::TokenKind self) noexcept;
mlc::String TokenKind_i64_val(ast_tokens::TokenKind self) noexcept;
mlc::String TokenKind_u8_val(ast_tokens::TokenKind self) noexcept;
mlc::String TokenKind_usize_val(ast_tokens::TokenKind self) noexcept;
mlc::String TokenKind_char_val(ast_tokens::TokenKind self) noexcept;
ast_tokens::TokenKind Parser_kind(Parser self) noexcept;
bool Parser_lambda_shorthand_suppression_active(Parser self) noexcept;
Parser Parser_with_lambda_shorthand_suppressed(Parser self, bool value) noexcept;
Parser Parser_record_parse_error(Parser self, mlc::String message) noexcept;
bool Parser_has_parse_errors(Parser self) noexcept;
Parser Parser_advance(Parser self) noexcept;
Parser Parser_advance_shift_right_as_angle_close(Parser self) noexcept;
Parser Parser_advance_by(Parser self, int count) noexcept;
bool Parser_at_eof(Parser self) noexcept;
Parser Parser_skip_semi(Parser self) noexcept;
ast::Span Parser_span_at_cursor(Parser self) noexcept;
int Parser_prev_line(Parser self) noexcept;
ast_tokens::TokenKind Parser_kind(Parser self) noexcept;
bool Parser_lambda_shorthand_suppression_active(Parser self) noexcept;
Parser Parser_with_lambda_shorthand_suppressed(Parser self, bool value) noexcept;
Parser Parser_record_parse_error(Parser self, mlc::String message) noexcept;
bool Parser_has_parse_errors(Parser self) noexcept;
Parser Parser_advance(Parser self) noexcept;
Parser Parser_advance_shift_right_as_angle_close(Parser self) noexcept;
Parser Parser_advance_by(Parser self, int count) noexcept;
bool Parser_at_eof(Parser self) noexcept;
Parser Parser_skip_semi(Parser self) noexcept;
ast::Span Parser_span_at_cursor(Parser self) noexcept;
int Parser_prev_line(Parser self) noexcept;
template<typename Value>
ParseResult<Value> parse_result(Value value, Parser parser) noexcept{
  return ParseResult{value, parser};
}

} // namespace predicates

#endif // PREDICATES_HPP
