#define main mlc_user_main
#include "predicates.hpp"

#include "ast_tokens.hpp"
#include "ast.hpp"

namespace predicates {

using namespace ast_tokens;
using namespace ast;
using namespace ast_tokens;

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
Parser parser_new_with_source_path(mlc::Array<ast_tokens::Token> tokens, mlc::String source_path) noexcept{
  return Parser{tokens, 0, source_path, false, {}, false};
}
Parser parser_new(mlc::Array<ast_tokens::Token> tokens) noexcept{
  return parser_new_with_source_path(tokens, mlc::String("", 0));
}
ParseResult<mlc::Array<mlc::String>> bounds_parse_result(mlc::Array<mlc::String> bounds, Parser parser) noexcept{
  return parse_result(bounds, parser);
}
ParseResult<std::shared_ptr<ast::TypeExpr>> type_parse_result(std::shared_ptr<ast::TypeExpr> type_expression, Parser parser) noexcept{
  return parse_result(type_expression, parser);
}
ParseResult<std::shared_ptr<ast::Pattern>> pattern_parse_result(std::shared_ptr<ast::Pattern> pattern, Parser parser) noexcept{
  return parse_result(pattern, parser);
}
ParseResult<std::shared_ptr<ast::Stmt>> statement_parse_result(std::shared_ptr<ast::Stmt> statement, Parser parser) noexcept{
  return parse_result(statement, parser);
}
ParseResult<std::shared_ptr<ast::TypeVariant>> variant_parse_result(std::shared_ptr<ast::TypeVariant> variant, Parser parser) noexcept{
  return parse_result(variant, parser);
}
ParseResult<mlc::Array<std::shared_ptr<ast::Stmt>>> statements_parse_result(mlc::Array<std::shared_ptr<ast::Stmt>> statements, Parser parser) noexcept{
  return parse_result(statements, parser);
}
ParseResult<mlc::Array<std::shared_ptr<ast::Expr>>> expressions_parse_result(mlc::Array<std::shared_ptr<ast::Expr>> expressions, Parser parser) noexcept{
  return parse_result(expressions, parser);
}
ParseResult<mlc::Array<std::shared_ptr<ast::MatchArm>>> arms_parse_result(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, Parser parser) noexcept{
  return parse_result(arms, parser);
}
ParseResult<mlc::Array<std::shared_ptr<ast::TypeVariant>>> variants_parse_result(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, Parser parser) noexcept{
  return parse_result(variants, parser);
}
ParseResult<mlc::Array<std::shared_ptr<ast::Param>>> parameters_parse_result(mlc::Array<std::shared_ptr<ast::Param>> parameters, Parser parser) noexcept{
  return parse_result(parameters, parser);
}
ParseResult<mlc::Array<std::shared_ptr<ast::FieldVal>>> field_values_parse_result(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, Parser parser) noexcept{
  return parse_result(field_values, parser);
}
ParseResult<mlc::Array<ast::RecordLitPart>> record_lit_parts_parse_result(mlc::Array<ast::RecordLitPart> lit_parts, Parser parser) noexcept{
  return parse_result(lit_parts, parser);
}
ParseResult<mlc::Array<std::shared_ptr<ast::Pattern>>> patterns_parse_result(mlc::Array<std::shared_ptr<ast::Pattern>> patterns, Parser parser) noexcept{
  return parse_result(patterns, parser);
}
ParseResult<mlc::Array<std::shared_ptr<ast::FieldDef>>> field_definitions_parse_result(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions, Parser parser) noexcept{
  return parse_result(field_definitions, parser);
}
ParseResult<mlc::Array<std::shared_ptr<ast::TypeExpr>>> types_parse_result(mlc::Array<std::shared_ptr<ast::TypeExpr>> types, Parser parser) noexcept{
  return parse_result(types, parser);
}
ParseResult<std::shared_ptr<ast::Decl>> declaration_parse_result(std::shared_ptr<ast::Decl> declaration, Parser parser) noexcept{
  return parse_result(declaration, parser);
}
ParseResult<mlc::Array<ast::WhereClauseBound>> where_clause_parse_result(mlc::Array<ast::WhereClauseBound> where_bounds, Parser parser) noexcept{
  return parse_result(where_bounds, parser);
}
ParseResult<mlc::Array<std::shared_ptr<ast::Decl>>> trait_body_parse_result(mlc::Array<std::shared_ptr<ast::Decl>> methods, Parser parser) noexcept{
  return parse_result(methods, parser);
}
ParseResult<std::shared_ptr<ast::Expr>> expression_parse_result(std::shared_ptr<ast::Expr> expression, Parser parser) noexcept{
  return parse_result(expression, parser);
}
ParseResult<TypeParamsValue> type_params_parse_result(mlc::Array<mlc::String> params, mlc::Array<mlc::Array<mlc::String>> bounds, Parser parser) noexcept{
  return parse_result(TypeParamsValue{params, bounds}, parser);
}
ProgramParseValue program_parse_result(ast::Program program, mlc::Array<mlc::String> errors) noexcept{
  return ProgramParseValue{program, errors};
}
bool is_ctor_name(mlc::String name) noexcept{
  return ((name.char_at(0) >= mlc::String("A", 1)) && (name.char_at(0) <= mlc::String("Z", 1)));
}
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
bool TokenKind_is_fn(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::KFn>(__match_subject)) {
const ast_tokens::KFn& kFn = std::get<ast_tokens::KFn>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_type(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::KType>(__match_subject)) {
const ast_tokens::KType& kType = std::get<ast_tokens::KType>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_let(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::KLet>(__match_subject)) {
const ast_tokens::KLet& kLet = std::get<ast_tokens::KLet>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_mut(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::KMut>(__match_subject)) {
const ast_tokens::KMut& kMut = std::get<ast_tokens::KMut>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_const(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::KConst>(__match_subject)) {
const ast_tokens::KConst& kConst = std::get<ast_tokens::KConst>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_if(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::KIf>(__match_subject)) {
const ast_tokens::KIf& kIf = std::get<ast_tokens::KIf>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_unless(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::KUnless>(__match_subject)) {
const ast_tokens::KUnless& kUnless = std::get<ast_tokens::KUnless>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_else(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::KElse>(__match_subject)) {
const ast_tokens::KElse& kElse = std::get<ast_tokens::KElse>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_end(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::KEnd>(__match_subject)) {
const ast_tokens::KEnd& kEnd = std::get<ast_tokens::KEnd>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_do(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::KDo>(__match_subject)) {
const ast_tokens::KDo& kDo = std::get<ast_tokens::KDo>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_while(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::KWhile>(__match_subject)) {
const ast_tokens::KWhile& kWhile = std::get<ast_tokens::KWhile>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_for(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::KFor>(__match_subject)) {
const ast_tokens::KFor& kFor = std::get<ast_tokens::KFor>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_with(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::KWith>(__match_subject)) {
const ast_tokens::KWith& kWith = std::get<ast_tokens::KWith>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_spawn(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::KSpawn>(__match_subject)) {
const ast_tokens::KSpawn& kSpawn = std::get<ast_tokens::KSpawn>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_move(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::KMove>(__match_subject)) {
const ast_tokens::KMove& kMove = std::get<ast_tokens::KMove>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_match(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::KMatch>(__match_subject)) {
const ast_tokens::KMatch& kMatch = std::get<ast_tokens::KMatch>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_return(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::KReturn>(__match_subject)) {
const ast_tokens::KReturn& kReturn = std::get<ast_tokens::KReturn>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_break(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::KBreak>(__match_subject)) {
const ast_tokens::KBreak& kBreak = std::get<ast_tokens::KBreak>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_continue(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::KContinue>(__match_subject)) {
const ast_tokens::KContinue& kContinue = std::get<ast_tokens::KContinue>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_true(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::KTrue>(__match_subject)) {
const ast_tokens::KTrue& kTrue = std::get<ast_tokens::KTrue>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_false(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::KFalse>(__match_subject)) {
const ast_tokens::KFalse& kFalse = std::get<ast_tokens::KFalse>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_extern(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::KExtern>(__match_subject)) {
const ast_tokens::KExtern& kExtern = std::get<ast_tokens::KExtern>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_extend(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::KExtend>(__match_subject)) {
const ast_tokens::KExtend& kExtend = std::get<ast_tokens::KExtend>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_where(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::KWhere>(__match_subject)) {
const ast_tokens::KWhere& kWhere = std::get<ast_tokens::KWhere>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_import(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::KImport>(__match_subject)) {
const ast_tokens::KImport& kImport = std::get<ast_tokens::KImport>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_as(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::KAs>(__match_subject)) {
const ast_tokens::KAs& kAs = std::get<ast_tokens::KAs>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_from(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::KFrom>(__match_subject)) {
const ast_tokens::KFrom& kFrom = std::get<ast_tokens::KFrom>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_eof(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::Eof>(__match_subject)) {
const ast_tokens::Eof& eof = std::get<ast_tokens::Eof>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_ident(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::Ident>(__match_subject)) {
const ast_tokens::Ident& ident = std::get<ast_tokens::Ident>(__match_subject);
auto [__0] = ident; return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_int(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::LInt>(__match_subject)) {
const ast_tokens::LInt& lInt = std::get<ast_tokens::LInt>(__match_subject);
auto [__0] = lInt; return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_str(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::LStr>(__match_subject)) {
const ast_tokens::LStr& lStr = std::get<ast_tokens::LStr>(__match_subject);
auto [__0] = lStr; return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_template(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::LTemplate>(__match_subject)) {
const ast_tokens::LTemplate& lTemplate = std::get<ast_tokens::LTemplate>(__match_subject);
auto [__0] = lTemplate; return true;
}
return false;
std::abort();
}();
}
mlc::Array<mlc::String> TokenKind_template_parts(ast_tokens::TokenKind self) noexcept{
return [&]() -> mlc::Array<mlc::String> {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::LTemplate>(__match_subject)) {
const ast_tokens::LTemplate& lTemplate = std::get<ast_tokens::LTemplate>(__match_subject);
auto [parts] = lTemplate; return parts;
}
return [&]() {
auto r = mlc::Array<mlc::String>{};
return r;
}();
std::abort();
}();
}
bool TokenKind_is_equal(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::Equal>(__match_subject)) {
const ast_tokens::Equal& equal = std::get<ast_tokens::Equal>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_bar(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::Bar>(__match_subject)) {
const ast_tokens::Bar& bar = std::get<ast_tokens::Bar>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_pipe(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::Pipe>(__match_subject)) {
const ast_tokens::Pipe& pipe = std::get<ast_tokens::Pipe>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_colon(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::Colon>(__match_subject)) {
const ast_tokens::Colon& colon = std::get<ast_tokens::Colon>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_comma(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::Comma>(__match_subject)) {
const ast_tokens::Comma& comma = std::get<ast_tokens::Comma>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_dot(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::Dot>(__match_subject)) {
const ast_tokens::Dot& dot = std::get<ast_tokens::Dot>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_spread(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::Spread>(__match_subject)) {
const ast_tokens::Spread& spread = std::get<ast_tokens::Spread>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_arrow(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::Arrow>(__match_subject)) {
const ast_tokens::Arrow& arrow = std::get<ast_tokens::Arrow>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_fat_arrow(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::FatArrow>(__match_subject)) {
const ast_tokens::FatArrow& fatArrow = std::get<ast_tokens::FatArrow>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_question(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::Question>(__match_subject)) {
const ast_tokens::Question& question = std::get<ast_tokens::Question>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_lparen(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::LParen>(__match_subject)) {
const ast_tokens::LParen& lParen = std::get<ast_tokens::LParen>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_rparen(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::RParen>(__match_subject)) {
const ast_tokens::RParen& rParen = std::get<ast_tokens::RParen>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_lbrace(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::LBrace>(__match_subject)) {
const ast_tokens::LBrace& lBrace = std::get<ast_tokens::LBrace>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_rbrace(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::RBrace>(__match_subject)) {
const ast_tokens::RBrace& rBrace = std::get<ast_tokens::RBrace>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_lbracket(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::LBracket>(__match_subject)) {
const ast_tokens::LBracket& lBracket = std::get<ast_tokens::LBracket>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_rbracket(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::RBracket>(__match_subject)) {
const ast_tokens::RBracket& rBracket = std::get<ast_tokens::RBracket>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_op(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::Op>(__match_subject)) {
const ast_tokens::Op& op = std::get<ast_tokens::Op>(__match_subject);
auto [__0] = op; return true;
}
return false;
std::abort();
}();
}
mlc::String TokenKind_ident(ast_tokens::TokenKind self) noexcept{
return [&]() -> mlc::String {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::Ident>(__match_subject)) {
const ast_tokens::Ident& ident = std::get<ast_tokens::Ident>(__match_subject);
auto [name] = ident; return name;
}
return mlc::String("", 0);
std::abort();
}();
}
int TokenKind_int_val(ast_tokens::TokenKind self) noexcept{
return [&]() -> int {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::LInt>(__match_subject)) {
const ast_tokens::LInt& lInt = std::get<ast_tokens::LInt>(__match_subject);
auto [n] = lInt; return n;
}
return 0;
std::abort();
}();
}
mlc::String TokenKind_str_val(ast_tokens::TokenKind self) noexcept{
return [&]() -> mlc::String {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::LStr>(__match_subject)) {
const ast_tokens::LStr& lStr = std::get<ast_tokens::LStr>(__match_subject);
auto [s] = lStr; return s;
}
return mlc::String("", 0);
std::abort();
}();
}
mlc::String TokenKind_op_val(ast_tokens::TokenKind self) noexcept{
return [&]() -> mlc::String {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::Op>(__match_subject)) {
const ast_tokens::Op& op = std::get<ast_tokens::Op>(__match_subject);
auto [s] = op; return s;
}
return mlc::String("", 0);
std::abort();
}();
}
bool TokenKind_is_float(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::LFloat>(__match_subject)) {
const ast_tokens::LFloat& lFloat = std::get<ast_tokens::LFloat>(__match_subject);
auto [__0] = lFloat; return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_i64(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::LI64>(__match_subject)) {
const ast_tokens::LI64& lI64 = std::get<ast_tokens::LI64>(__match_subject);
auto [__0] = lI64; return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_u8(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::LU8>(__match_subject)) {
const ast_tokens::LU8& lU8 = std::get<ast_tokens::LU8>(__match_subject);
auto [__0] = lU8; return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_usize(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::LUsize>(__match_subject)) {
const ast_tokens::LUsize& lUsize = std::get<ast_tokens::LUsize>(__match_subject);
auto [__0] = lUsize; return true;
}
return false;
std::abort();
}();
}
bool TokenKind_is_char_lit(ast_tokens::TokenKind self) noexcept{
return [&]() -> bool {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::LChar>(__match_subject)) {
const ast_tokens::LChar& lChar = std::get<ast_tokens::LChar>(__match_subject);
auto [__0] = lChar; return true;
}
return false;
std::abort();
}();
}
mlc::String TokenKind_float_val(ast_tokens::TokenKind self) noexcept{
return [&]() -> mlc::String {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::LFloat>(__match_subject)) {
const ast_tokens::LFloat& lFloat = std::get<ast_tokens::LFloat>(__match_subject);
auto [s] = lFloat; return s;
}
return mlc::String("", 0);
std::abort();
}();
}
mlc::String TokenKind_i64_val(ast_tokens::TokenKind self) noexcept{
return [&]() -> mlc::String {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::LI64>(__match_subject)) {
const ast_tokens::LI64& lI64 = std::get<ast_tokens::LI64>(__match_subject);
auto [s] = lI64; return s;
}
return mlc::String("", 0);
std::abort();
}();
}
mlc::String TokenKind_u8_val(ast_tokens::TokenKind self) noexcept{
return [&]() -> mlc::String {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::LU8>(__match_subject)) {
const ast_tokens::LU8& lU8 = std::get<ast_tokens::LU8>(__match_subject);
auto [s] = lU8; return s;
}
return mlc::String("", 0);
std::abort();
}();
}
mlc::String TokenKind_usize_val(ast_tokens::TokenKind self) noexcept{
return [&]() -> mlc::String {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::LUsize>(__match_subject)) {
const ast_tokens::LUsize& lUsize = std::get<ast_tokens::LUsize>(__match_subject);
auto [s] = lUsize; return s;
}
return mlc::String("", 0);
std::abort();
}();
}
mlc::String TokenKind_char_val(ast_tokens::TokenKind self) noexcept{
return [&]() -> mlc::String {
auto __match_subject = self;
if (std::holds_alternative<ast_tokens::LChar>(__match_subject)) {
const ast_tokens::LChar& lChar = std::get<ast_tokens::LChar>(__match_subject);
auto [s] = lChar; return s;
}
return mlc::String("", 0);
std::abort();
}();
}
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
ast_tokens::TokenKind Parser_kind(Parser self) noexcept{
if (self.has_unget_angle_close) {
  return ast_tokens::Op{mlc::String(">", 1)};
} else if ((self.position >= self.tokens.length())) {
  return ast_tokens::Eof{};
} else {
  auto current_token = self.tokens[self.position];
  return current_token.kind;
}
}
bool Parser_lambda_shorthand_suppression_active(Parser self) noexcept{
return self.suppress_lambda_shorthand;
}
Parser Parser_with_lambda_shorthand_suppressed(Parser self, bool value) noexcept{
return Parser{self.tokens, self.position, self.source_path, value, self.errors, self.has_unget_angle_close};
}
Parser Parser_record_parse_error(Parser self, mlc::String message) noexcept{
auto updated_errors = self.errors;
updated_errors.push_back(message);
return Parser{self.tokens, self.position, self.source_path, self.suppress_lambda_shorthand, updated_errors, self.has_unget_angle_close};
}
bool Parser_has_parse_errors(Parser self) noexcept{
return (self.errors.length() > 0);
}
Parser Parser_advance(Parser self) noexcept{
if (self.has_unget_angle_close) {
  return Parser{self.tokens, self.position, self.source_path, self.suppress_lambda_shorthand, self.errors, false};
} else {
  return Parser{self.tokens, (self.position + 1), self.source_path, self.suppress_lambda_shorthand, self.errors, false};
}
}
Parser Parser_advance_shift_right_as_angle_close(Parser self) noexcept{
return Parser{self.tokens, (self.position + 1), self.source_path, self.suppress_lambda_shorthand, self.errors, true};
}
Parser Parser_advance_by(Parser self, int count) noexcept{
return Parser{self.tokens, (self.position + count), self.source_path, self.suppress_lambda_shorthand, self.errors, self.has_unget_angle_close};
}
bool Parser_at_eof(Parser self) noexcept{
if ((self.position >= self.tokens.length())) {
  return true;
} else {
  auto current_token = self.tokens[self.position];
  return TokenKind_is_eof(current_token.kind);
}
}
Parser Parser_skip_semi(Parser self) noexcept{
if ((self.position >= self.tokens.length())) {
  return self;
} else {
  auto current_token = self.tokens[self.position];
  return [&]() -> Parser {
auto __match_subject = current_token.kind;
if (std::holds_alternative<ast_tokens::Semicolon>(__match_subject)) {
const ast_tokens::Semicolon& semicolon = std::get<ast_tokens::Semicolon>(__match_subject);
return Parser_advance(self);
}
return self;
std::abort();
}();
}
}
ast::Span Parser_span_at_cursor(Parser self) noexcept{
if ((self.position >= self.tokens.length())) {
  return ast::span_make(self.source_path, 0, 0);
} else {
  auto current_token = self.tokens[self.position];
  return ast::span_make(self.source_path, current_token.line, current_token.column);
}
}
int Parser_prev_line(Parser self) noexcept{
if ((self.position > 0)) {
  auto current_token = self.tokens[(self.position - 1)];
  std::make_tuple();
  return current_token.line;
} else {
  return 0;
}
}

} // namespace predicates
