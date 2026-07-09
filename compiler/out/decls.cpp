#include "decls.hpp"

#include "predicates.hpp"
#include "ast_tokens.hpp"
#include "types.hpp"
#include "exprs.hpp"
#include "comma_separated.hpp"
#include "expect_close.hpp"
#include "ast.hpp"

namespace decls {

using namespace predicates;
using namespace ast_tokens;
using namespace types;
using namespace exprs;
using namespace comma_separated;
using namespace expect_close;
using namespace ast;
using namespace ast_tokens;

using DeriveResult = predicates::ParseResult<mlc::Array<mlc::String>>;

mlc::String type_expression_display(std::shared_ptr<ast::TypeExpr> type_expression) noexcept;

mlc::String type_arguments_display(mlc::Array<std::shared_ptr<ast::TypeExpr>> type_arguments) noexcept;

predicates::Parser parser_expect_generic_close(predicates::Parser state, bool opened) noexcept;

predicates::Parser parser_expect_ident(predicates::Parser state, mlc::String context) noexcept;

bool decl_is_parse_skip(std::shared_ptr<ast::Decl> declaration) noexcept;

predicates::BoundsResult parse_trait_bound_ident_list(predicates::Parser parser_state) noexcept;

predicates::BoundsResult parse_one_param_bounds(predicates::Parser state) noexcept;

predicates::WhereClauseParseResult parse_where_clause_opt(predicates::Parser parser_state) noexcept;

bool string_list_contains(mlc::Array<mlc::String> haystack, mlc::String needle) noexcept;

mlc::Array<mlc::String> clone_string_row(mlc::Array<mlc::String> source) noexcept;

mlc::Array<mlc::Array<mlc::String>> clone_trait_bounds_matrix(mlc::Array<mlc::Array<mlc::String>> source) noexcept;

mlc::Array<mlc::String> union_trait_string_rows(mlc::Array<mlc::String> existing_row, mlc::Array<mlc::String> incoming_traits) noexcept;

int type_parameter_row_index(mlc::Array<mlc::String> type_parameter_names, mlc::String parameter_name) noexcept;

mlc::Array<mlc::Array<mlc::String>> replace_trait_bounds_row_at(mlc::Array<mlc::Array<mlc::String>> matrix, int row_index, mlc::Array<mlc::String> new_row) noexcept;

mlc::Array<mlc::Array<mlc::String>> merge_where_clause_trait_bounds(mlc::Array<mlc::String> type_parameter_names, mlc::Array<mlc::Array<mlc::String>> trait_bounds_rows, mlc::Array<ast::WhereClauseBound> where_entries) noexcept;

predicates::TypeParamsResult parse_type_params_opt(predicates::Parser parser) noexcept;

predicates::Parser parser_after_optional_rbrace(predicates::Parser parser) noexcept;

predicates::Parser parser_after_optional_from(predicates::Parser parser) noexcept;

mlc::String import_path_from_parser(predicates::Parser parser) noexcept;

predicates::Parser parser_after_optional_lbrace(predicates::Parser parser) noexcept;

predicates::Parser parser_after_optional_comma(predicates::Parser parser) noexcept;

predicates::Parser parser_after_optional_arrow(predicates::Parser parser) noexcept;

predicates::Parser parser_after_optional_mut(predicates::Parser parser) noexcept;

predicates::Parser parser_after_optional_bar(predicates::Parser parser) noexcept;

predicates::Parser parser_after_optional_private_keyword(predicates::Parser parser) noexcept;

ast::Span extern_keyword_span_for_parser(predicates::Parser parser, bool is_extern) noexcept;

predicates::Parser parser_after_optional_extern(predicates::Parser parser, bool is_extern) noexcept;

predicates::Parser trait_body_start_parser(predicates::Parser body_parser) noexcept;

predicates::Parser parser_record_error_if_eof_after_equal(predicates::Parser state) noexcept;

predicates::Parser parser_record_error_if_eof_expect_equal(predicates::Parser state) noexcept;

predicates::ExprResult parse_trait_method_body(predicates::Parser where_parser) noexcept;

predicates::DeclResult parse_import_decl(predicates::Parser parser) noexcept;

predicates::DeclResult parse_extend_decl(predicates::Parser parser) noexcept;

predicates::Parser advance_past_optional_self_type(predicates::Parser parser_after_self) noexcept;

predicates::ParamsResult parse_extend_extern_no_self_params(predicates::Parser after_lparen) noexcept;

predicates::ParamsResult parse_extend_extern_rest_params(predicates::Parser after_lparen, bool leading_self) noexcept;

predicates::DeclResult parse_extend_extern_method(predicates::Parser parser, mlc::String type_name) noexcept;

predicates::ParamsResult parse_extend_method_rest_params(predicates::Parser rest_start) noexcept;

bool is_extern_ident_expr(std::shared_ptr<ast::Expr> expression) noexcept;

predicates::ExprResult parse_extend_method_body(predicates::Parser after_eq) noexcept;

predicates::Parser extend_method_rest_start(predicates::Parser after_lparen) noexcept;

predicates::DeclResult parse_extend_method(predicates::Parser parser, mlc::String type_name) noexcept;

predicates::DeclResult parse_trait_decl(predicates::Parser parser) noexcept;

predicates::DeclResult parse_declaration(predicates::Parser parser) noexcept;

predicates::DeclResult parse_function_declaration(predicates::Parser parser) noexcept;

predicates::PatternResult parse_tuple_parameter_binding_element(predicates::Parser binding_parser) noexcept;

predicates::PatternResult parse_tuple_parameter_pattern(ast::Span tuple_span, predicates::Parser parser_after_open_paren) noexcept;

std::shared_ptr<ast::Pattern> plain_identifier_parameter_pattern_marker() noexcept;

predicates::ParamResult parse_param(predicates::Parser parser_for_parameter, int parameter_slot_index) noexcept;

predicates::ParamsResult parse_params(predicates::Parser parser) noexcept;

decls::DeriveResult parse_derive_clause(predicates::Parser parser) noexcept;

bool type_decl_after_equal_is_sum_variant(predicates::Parser parser) noexcept;

predicates::DeclResult parse_type_decl(predicates::Parser parser) noexcept;

bool is_decl_start(ast_tokens::TokenKind kind) noexcept;

bool parse_variants_continue(predicates::Parser state) noexcept;

predicates::VariantsResult parse_variants(predicates::Parser parser) noexcept;

predicates::VariantResult parse_variant(predicates::Parser parser) noexcept;

predicates::TypesResult parse_tuple_types(predicates::Parser parser) noexcept;

predicates::TraitBodyResult parse_trait_body(predicates::Parser parser, mlc::String trait_name, mlc::Array<mlc::String> type_params) noexcept;

predicates::FieldDefsResult parse_field_defs(predicates::Parser parser) noexcept;

ast::Program parse_program_with_source_path(mlc::Array<ast_tokens::Token> tokens, mlc::String source_path) noexcept;

predicates::ProgramParseValue parse_program_with_errors(mlc::Array<ast_tokens::Token> tokens, mlc::String source_path) noexcept;

ast::Program parse_program(mlc::Array<ast_tokens::Token> tokens) noexcept;

mlc::String type_expression_display(std::shared_ptr<ast::TypeExpr> type_expression) noexcept{return std::visit(overloaded{
  [&](const TyI32& tyi32) -> mlc::String { return mlc::String("i32"); },
  [&](const TyString& tystring) -> mlc::String { return mlc::String("string"); },
  [&](const TyBool& tybool) -> mlc::String { return mlc::String("bool"); },
  [&](const TyUnit& tyunit) -> mlc::String { return mlc::String("unit"); },
  [&](const TyNamed& tynamed) -> mlc::String { auto [name] = tynamed; return name; },
  [&](const TyArray& tyarray) -> mlc::String { auto [inner] = tyarray; return mlc::String("[") + type_expression_display(inner) + mlc::String("]"); },
  [&](const TyShared& tyshared) -> mlc::String { auto [inner] = tyshared; return mlc::String("Shared<") + type_expression_display(inner) + mlc::String(">"); },
  [&](const TyGeneric& tygeneric) -> mlc::String { auto [name, type_arguments] = tygeneric; return name + mlc::String("<") + type_arguments_display(type_arguments) + mlc::String(">"); },
  [&](const TyFn& tyfn) -> mlc::String { auto [parameters, return_type] = tyfn; return mlc::String("(") + type_arguments_display(parameters) + mlc::String(") -> ") + type_expression_display(return_type); },
  [&](const TyAssoc& tyassoc) -> mlc::String { auto [base_name, associated_name] = tyassoc; return base_name + mlc::String(".") + associated_name; }
}, (*type_expression));}

mlc::String type_arguments_display(mlc::Array<std::shared_ptr<ast::TypeExpr>> type_arguments) noexcept{return type_arguments.map(type_expression_display).join(mlc::String(", "));}

predicates::Parser parser_expect_generic_close(predicates::Parser state, bool opened) noexcept{return !opened ? state : types::parser_at_generic_close(state) ? types::parser_advance_generic_close(state) : predicates::Parser_record_parse_error(state, mlc::String("parse: unclosed generic parameter list"));}

predicates::Parser parser_expect_ident(predicates::Parser state, mlc::String context) noexcept{return predicates::TokenKind_is_ident(predicates::Parser_kind(state)) ? state : predicates::Parser_record_parse_error(state, mlc::String("parse: expected identifier in ") + context);}

bool decl_is_parse_skip(std::shared_ptr<ast::Decl> declaration) noexcept{return [&]() { if (std::holds_alternative<ast::DeclFn>((*declaration))) { auto _v_declfn = std::get<ast::DeclFn>((*declaration)); auto [name, _w0, _w1, _w2, _w3, _w4, _w5] = _v_declfn; return name == mlc::String("__skip__"); } return false; }();}

predicates::BoundsResult parse_trait_bound_ident_list(predicates::Parser parser_state) noexcept{
predicates::Parser state = std::move(parser_state);
mlc::Array<mlc::String> bounds = {};
if (predicates::TokenKind_is_ident(predicates::Parser_kind(state))){
{
bounds.push_back(predicates::TokenKind_ident(predicates::Parser_kind(state)));
state = predicates::Parser_advance(state);
[&]() { 
  while (predicates::TokenKind_is_op(predicates::Parser_kind(state)) && predicates::TokenKind_op_val(predicates::Parser_kind(state)) == mlc::String("+")){
{
state = predicates::Parser_advance(state);
if (predicates::TokenKind_is_ident(predicates::Parser_kind(state))){
{
bounds.push_back(predicates::TokenKind_ident(predicates::Parser_kind(state)));
state = predicates::Parser_advance(state);
}
}
}
}
 }();
}
}
return predicates::bounds_parse_result(bounds, state);
}

predicates::BoundsResult parse_one_param_bounds(predicates::Parser state) noexcept{
return predicates::TokenKind_is_colon(predicates::Parser_kind(state)) ? parse_trait_bound_ident_list(predicates::Parser_advance(state)) : predicates::bounds_parse_result({}, state);
}

predicates::WhereClauseParseResult parse_where_clause_opt(predicates::Parser parser_state) noexcept{
return !predicates::TokenKind_is_where(predicates::Parser_kind(parser_state)) ? [&]() -> predicates::WhereClauseParseResult { 
  mlc::Array<ast::WhereClauseBound> empty_where_bounds = {};
  return predicates::where_clause_parse_result(empty_where_bounds, parser_state);
 }() : [&]() -> predicates::WhereClauseParseResult { 
  predicates::Parser state = predicates::Parser_advance(parser_state);
  mlc::Array<ast::WhereClauseBound> collected = {};
  while (predicates::TokenKind_is_ident(predicates::Parser_kind(state))){
{
mlc::String parameter_name = predicates::TokenKind_ident(predicates::Parser_kind(state));
state = predicates::Parser_advance(state);
if (predicates::TokenKind_is_colon(predicates::Parser_kind(state))){
{
predicates::BoundsResult trait_parsed = parse_trait_bound_ident_list(predicates::Parser_advance(state));
state = trait_parsed.parser;
collected.push_back(ast::WhereClauseBound{parameter_name, trait_parsed.value});
}
}
if (predicates::TokenKind_is_comma(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
}
}
}
}
  return predicates::where_clause_parse_result(collected, state);
 }();
}

bool string_list_contains(mlc::Array<mlc::String> haystack, mlc::String needle) noexcept{
int index = 0;
while (index < haystack.size()){
{
if (haystack[index] == needle){
{
return true;
}
}
index = index + 1;
}
}
return false;
}

mlc::Array<mlc::String> clone_string_row(mlc::Array<mlc::String> source) noexcept{
mlc::Array<mlc::String> copy = {};
int index = 0;
while (index < source.size()){
{
copy.push_back(source[index]);
index = index + 1;
}
}
return copy;
}

mlc::Array<mlc::Array<mlc::String>> clone_trait_bounds_matrix(mlc::Array<mlc::Array<mlc::String>> source) noexcept{
mlc::Array<mlc::Array<mlc::String>> matrix = {};
int row_index = 0;
while (row_index < source.size()){
{
matrix.push_back(clone_string_row(source[row_index]));
row_index = row_index + 1;
}
}
return matrix;
}

mlc::Array<mlc::String> union_trait_string_rows(mlc::Array<mlc::String> existing_row, mlc::Array<mlc::String> incoming_traits) noexcept{
mlc::Array<mlc::String> combined = clone_string_row(existing_row);
int incoming_index = 0;
while (incoming_index < incoming_traits.size()){
{
mlc::String trait_name = incoming_traits[incoming_index];
if (!string_list_contains(combined, trait_name)){
{
combined.push_back(trait_name);
}
}
incoming_index = incoming_index + 1;
}
}
return combined;
}

int type_parameter_row_index(mlc::Array<mlc::String> type_parameter_names, mlc::String parameter_name) noexcept{
int index = 0;
while (index < type_parameter_names.size()){
{
if (type_parameter_names[index] == parameter_name){
{
return index;
}
}
index = index + 1;
}
}
return -1;
}

mlc::Array<mlc::Array<mlc::String>> replace_trait_bounds_row_at(mlc::Array<mlc::Array<mlc::String>> matrix, int row_index, mlc::Array<mlc::String> new_row) noexcept{
mlc::Array<mlc::Array<mlc::String>> output_matrix = {};
int matrix_row_index = 0;
while (matrix_row_index < matrix.size()){
{
if (matrix_row_index == row_index){
{
output_matrix.push_back(clone_string_row(new_row));
}
} else {
{
output_matrix.push_back(clone_string_row(matrix[matrix_row_index]));
}
}
matrix_row_index = matrix_row_index + 1;
}
}
return output_matrix;
}

mlc::Array<mlc::Array<mlc::String>> merge_where_clause_trait_bounds(mlc::Array<mlc::String> type_parameter_names, mlc::Array<mlc::Array<mlc::String>> trait_bounds_rows, mlc::Array<ast::WhereClauseBound> where_entries) noexcept{
mlc::Array<mlc::Array<mlc::String>> merged = clone_trait_bounds_matrix(trait_bounds_rows);
while (merged.size() < type_parameter_names.size()){
{
mlc::Array<mlc::String> empty_row = {};
merged.push_back(empty_row);
}
}
int entry_index = 0;
while (entry_index < where_entries.size()){
{
ast::WhereClauseBound entry = where_entries[entry_index];
int row_index = type_parameter_row_index(type_parameter_names, entry.parameter_name);
if (row_index >= 0){
{
mlc::Array<mlc::String> updated_row = union_trait_string_rows(merged[row_index], entry.traits);
merged = replace_trait_bounds_row_at(merged, row_index, updated_row);
}
}
entry_index = entry_index + 1;
}
}
return merged;
}

predicates::TypeParamsResult parse_type_params_opt(predicates::Parser parser) noexcept{
return predicates::TokenKind_is_op(predicates::Parser_kind(parser)) && predicates::TokenKind_op_val(predicates::Parser_kind(parser)) == mlc::String("<") ? [&]() -> predicates::TypeParamsResult { 
  predicates::Parser state = predicates::Parser_advance(parser);
  mlc::Array<mlc::String> params = {};
  mlc::Array<mlc::Array<mlc::String>> bounds_list = {};
  if (predicates::TokenKind_is_ident(predicates::Parser_kind(state))){
{
params.push_back(predicates::TokenKind_ident(predicates::Parser_kind(state)));
predicates::BoundsResult bounds_parsed = parse_one_param_bounds(predicates::Parser_advance(state));
bounds_list.push_back(bounds_parsed.value);
state = bounds_parsed.parser;
[&]() { 
  while (predicates::TokenKind_is_comma(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
if (predicates::TokenKind_is_ident(predicates::Parser_kind(state))){
{
params.push_back(predicates::TokenKind_ident(predicates::Parser_kind(state)));
predicates::BoundsResult bounds_parsed_loop = parse_one_param_bounds(predicates::Parser_advance(state));
bounds_list.push_back(bounds_parsed_loop.value);
state = bounds_parsed_loop.parser;
}
}
}
}
 }();
}
}
  state = parser_expect_generic_close(state, true);
  return predicates::type_params_parse_result(params, bounds_list, state);
 }() : predicates::type_params_parse_result({}, {}, parser);
}

predicates::Parser parser_after_optional_rbrace(predicates::Parser parser) noexcept{return predicates::TokenKind_is_rbrace(predicates::Parser_kind(parser)) ? predicates::Parser_advance(parser) : parser;}

predicates::Parser parser_after_optional_from(predicates::Parser parser) noexcept{return predicates::TokenKind_is_from(predicates::Parser_kind(parser)) ? predicates::Parser_advance(parser) : parser;}

mlc::String import_path_from_parser(predicates::Parser parser) noexcept{return predicates::TokenKind_is_str(predicates::Parser_kind(parser)) ? predicates::TokenKind_str_val(predicates::Parser_kind(parser)) : mlc::String("");}

predicates::Parser parser_after_optional_lbrace(predicates::Parser parser) noexcept{return predicates::TokenKind_is_lbrace(predicates::Parser_kind(parser)) ? predicates::Parser_advance(parser) : parser;}

predicates::Parser parser_after_optional_comma(predicates::Parser parser) noexcept{return predicates::TokenKind_is_comma(predicates::Parser_kind(parser)) ? predicates::Parser_advance(parser) : parser;}

predicates::Parser parser_after_optional_arrow(predicates::Parser parser) noexcept{return predicates::TokenKind_is_arrow(predicates::Parser_kind(parser)) ? predicates::Parser_advance(parser) : parser;}

predicates::Parser parser_after_optional_mut(predicates::Parser parser) noexcept{return predicates::TokenKind_is_mut(predicates::Parser_kind(parser)) ? predicates::Parser_advance(parser) : parser;}

predicates::Parser parser_after_optional_bar(predicates::Parser parser) noexcept{return predicates::TokenKind_is_bar(predicates::Parser_kind(parser)) ? predicates::Parser_advance(parser) : parser;}

predicates::Parser parser_after_optional_private_keyword(predicates::Parser parser) noexcept{return predicates::TokenKind_is_ident(predicates::Parser_kind(parser)) && predicates::TokenKind_ident(predicates::Parser_kind(parser)) == mlc::String("private") ? predicates::Parser_advance(parser) : parser;}

ast::Span extern_keyword_span_for_parser(predicates::Parser parser, bool is_extern) noexcept{return is_extern ? predicates::Parser_span_at_cursor(parser) : ast::span_unknown();}

predicates::Parser parser_after_optional_extern(predicates::Parser parser, bool is_extern) noexcept{return is_extern ? predicates::Parser_advance(parser) : parser;}

predicates::Parser trait_body_start_parser(predicates::Parser body_parser) noexcept{
if (!predicates::TokenKind_is_lbrace(predicates::Parser_kind(body_parser))){
{
return predicates::Parser_record_parse_error(body_parser, mlc::String("parse: expected { after trait header"));
}
}
return predicates::Parser_advance(body_parser);
}

predicates::Parser parser_record_error_if_eof_after_equal(predicates::Parser state) noexcept{return predicates::Parser_at_eof(state) ? predicates::Parser_record_parse_error(state, mlc::String("parse: expected type body after =")) : state;}

predicates::Parser parser_record_error_if_eof_expect_equal(predicates::Parser state) noexcept{return predicates::Parser_at_eof(state) ? predicates::Parser_record_parse_error(state, mlc::String("parse: expected = after type name")) : state;}

predicates::ExprResult parse_trait_method_body(predicates::Parser where_parser) noexcept{return predicates::TokenKind_is_equal(predicates::Parser_kind(where_parser)) ? exprs::parse_expr(predicates::Parser_advance(where_parser)) : predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprExtern(predicates::Parser_span_at_cursor(where_parser))), where_parser);}

predicates::DeclResult parse_import_decl(predicates::Parser parser) noexcept{
predicates::Parser state = std::move(parser);
mlc::Array<mlc::String> symbols = {};
if (predicates::TokenKind_is_op(predicates::Parser_kind(state)) && predicates::TokenKind_op_val(predicates::Parser_kind(state)) == mlc::String("*")){
{
symbols.push_back(mlc::String("*"));
state = predicates::Parser_advance(state);
if (predicates::TokenKind_is_as(predicates::Parser_kind(state))){
state = predicates::Parser_advance(state);
if (predicates::TokenKind_is_ident(predicates::Parser_kind(state))){
symbols.push_back(predicates::TokenKind_ident(predicates::Parser_kind(state)));
state = predicates::Parser_advance(state);
}
}
}
} else {
{
if (predicates::TokenKind_is_lbrace(predicates::Parser_kind(state))){
state = predicates::Parser_advance(state);
while (predicates::TokenKind_is_ident(predicates::Parser_kind(state)) || predicates::TokenKind_is_op(predicates::Parser_kind(state)) && predicates::TokenKind_op_val(predicates::Parser_kind(state)) == mlc::String("*")){
{
if (predicates::TokenKind_is_op(predicates::Parser_kind(state)) && predicates::TokenKind_op_val(predicates::Parser_kind(state)) == mlc::String("*")){
{
symbols.push_back(mlc::String("*"));
state = predicates::Parser_advance(state);
}
} else {
{
symbols.push_back(predicates::TokenKind_ident(predicates::Parser_kind(state)));
state = predicates::Parser_advance(state);
}
}
if (predicates::TokenKind_is_comma(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
}
}
}
}
state = parser_after_optional_rbrace(state);
}
}
}
state = parser_after_optional_from(state);
mlc::String path = import_path_from_parser(state);
return predicates::declaration_parse_result(std::make_shared<ast::Decl>(ast::DeclImport(path, symbols)), predicates::Parser_advance(state));
}

predicates::DeclResult parse_extend_decl(predicates::Parser parser) noexcept{
ast::Span type_name_span = predicates::Parser_span_at_cursor(parser);
mlc::String type_name = predicates::TokenKind_ident(predicates::Parser_kind(parser));
predicates::Parser state = predicates::Parser_advance(parser);
if (predicates::TokenKind_is_op(predicates::Parser_kind(state)) && predicates::TokenKind_op_val(predicates::Parser_kind(state)) == mlc::String("<")){
{
predicates::TypesResult type_name_arguments_parsed = types::parse_type_args(predicates::Parser_advance(state));
state = type_name_arguments_parsed.parser;
}
}
mlc::String trait_name = mlc::String("");
if (predicates::TokenKind_is_colon(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
if (predicates::TokenKind_is_ident(predicates::Parser_kind(state))){
{
trait_name = predicates::TokenKind_ident(predicates::Parser_kind(state));
state = predicates::Parser_advance(state);
}
}
if (trait_name.length() > 0 && predicates::TokenKind_is_op(predicates::Parser_kind(state)) && predicates::TokenKind_op_val(predicates::Parser_kind(state)) == mlc::String("<")){
predicates::TypesResult trait_type_arguments_parsed = types::parse_type_args(predicates::Parser_advance(state));
trait_name = trait_name + mlc::String("<") + type_arguments_display(trait_type_arguments_parsed.value) + mlc::String(">");
state = trait_type_arguments_parsed.parser;
}
}
}
predicates::Parser methods_state = parser_after_optional_lbrace(state);
mlc::Array<std::shared_ptr<ast::Decl>> methods = {};
while (!predicates::TokenKind_is_rbrace(predicates::Parser_kind(methods_state)) && !predicates::Parser_at_eof(methods_state)){
{
if (predicates::TokenKind_is_fn(predicates::Parser_kind(methods_state))){
{
predicates::DeclResult parsed = parse_extend_method(methods_state, type_name);
methods.push_back(parsed.value);
methods_state = parsed.parser;
}
} else {
{
if (predicates::TokenKind_is_extern(predicates::Parser_kind(methods_state)) && predicates::TokenKind_is_fn(predicates::Parser_kind(predicates::Parser_advance(methods_state)))){
predicates::DeclResult parsed = parse_extend_extern_method(predicates::Parser_advance(methods_state), type_name);
methods.push_back(parsed.value);
methods_state = parsed.parser;
} else {
if (predicates::TokenKind_is_type(predicates::Parser_kind(methods_state))){
predicates::Parser after_type = predicates::Parser_advance(methods_state);
ast::Span assoc_span = predicates::Parser_span_at_cursor(after_type);
mlc::String assoc_name = predicates::TokenKind_ident(predicates::Parser_kind(after_type));
predicates::Parser after_name = predicates::Parser_advance(after_type);
if (predicates::TokenKind_is_equal(predicates::Parser_kind(after_name))){
predicates::TypeResult type_parsed = types::parse_type(predicates::Parser_advance(after_name));
methods.push_back(std::make_shared<ast::Decl>(ast::DeclAssocBind(assoc_name, type_parsed.value, assoc_span)));
methods_state = type_parsed.parser;
} else {
methods.push_back(std::make_shared<ast::Decl>(ast::DeclAssocType(assoc_name, assoc_span)));
methods_state = after_name;
}
} else {
methods_state = predicates::Parser_advance(methods_state);
}
}
}
}
}
}
return predicates::declaration_parse_result(std::make_shared<ast::Decl>(ast::DeclExtend(type_name, trait_name, methods, type_name_span)), predicates::Parser_advance(methods_state));
}

predicates::Parser advance_past_optional_self_type(predicates::Parser parser_after_self) noexcept{return predicates::TokenKind_is_colon(predicates::Parser_kind(parser_after_self)) ? types::parse_type(predicates::Parser_advance(parser_after_self)).parser : parser_after_self;}

predicates::ParamsResult parse_extend_extern_no_self_params(predicates::Parser after_lparen) noexcept{return predicates::TokenKind_is_rparen(predicates::Parser_kind(after_lparen)) ? [&]() -> predicates::ParamsResult { 
  mlc::Array<std::shared_ptr<ast::Param>> empty_parameters = {};
  return predicates::parameters_parse_result(empty_parameters, predicates::Parser_advance(after_lparen));
 }() : parse_params(after_lparen);}

predicates::ParamsResult parse_extend_extern_rest_params(predicates::Parser after_lparen, bool leading_self) noexcept{return leading_self ? [&]() -> predicates::ParamsResult { 
  predicates::Parser after_self_type = advance_past_optional_self_type(predicates::Parser_advance(after_lparen));
  return predicates::TokenKind_is_rparen(predicates::Parser_kind(after_self_type)) ? [&]() -> predicates::ParamsResult { 
  mlc::Array<std::shared_ptr<ast::Param>> empty_parameters = {};
  return predicates::parameters_parse_result(empty_parameters, predicates::Parser_advance(after_self_type));
 }() : parse_params(parser_after_optional_comma(after_self_type));
 }() : parse_extend_extern_no_self_params(after_lparen);}

predicates::DeclResult parse_extend_extern_method(predicates::Parser parser, mlc::String type_name) noexcept{
ast::Span extern_span = predicates::Parser_span_at_cursor(parser);
mlc::String fn_name = predicates::TokenKind_ident(predicates::Parser_kind(predicates::Parser_advance(parser)));
mlc::String mangled_name = type_name + mlc::String("_") + fn_name;
predicates::Parser after_lparen = predicates::Parser_advance_by(parser, 3);
std::shared_ptr<ast::Param> self_param = std::make_shared<ast::Param>(ast::Param{mlc::String("self"), false, std::make_shared<ast::TypeExpr>(ast::TyNamed(type_name)), false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())), std::make_shared<ast::Pattern>(ast::PatternUnit(ast::span_unknown()))});
mlc::Array<std::shared_ptr<ast::Param>> params = {};
bool leading_self = predicates::TokenKind_is_ident(predicates::Parser_kind(after_lparen)) && predicates::TokenKind_ident(predicates::Parser_kind(after_lparen)) == mlc::String("self");
if (leading_self){
{
params.push_back(self_param);
}
}
predicates::ParamsResult rest_parsed = parse_extend_extern_rest_params(after_lparen, leading_self);
int index = 0;
while (index < rest_parsed.value.size()){
{
std::shared_ptr<ast::Param> rest_parameter = rest_parsed.value[index];
if (ast::param_name(rest_parameter) != mlc::String("self")){
{
params.push_back(rest_parameter);
}
}
index = index + 1;
}
}
predicates::Parser after_rparen = predicates::Parser_advance(rest_parsed.parser);
predicates::TypeResult return_type_parsed = types::parse_type(parser_after_optional_arrow(after_rparen));
return predicates::declaration_parse_result(std::make_shared<ast::Decl>(ast::DeclFn(mangled_name, {}, {}, params, return_type_parsed.value, std::make_shared<ast::Expr>(ast::ExprExtern(extern_span)), {})), return_type_parsed.parser);
}

predicates::ParamsResult parse_extend_method_rest_params(predicates::Parser rest_start) noexcept{return predicates::TokenKind_is_rparen(predicates::Parser_kind(rest_start)) ? [&]() -> predicates::ParamsResult { 
  mlc::Array<std::shared_ptr<ast::Param>> empty_parameters = {};
  return predicates::parameters_parse_result(empty_parameters, predicates::Parser_advance(rest_start));
 }() : parse_params(parser_after_optional_comma(rest_start));}

bool is_extern_ident_expr(std::shared_ptr<ast::Expr> expression) noexcept{return [&]() { if (std::holds_alternative<ast::ExprIdent>((*expression)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*expression)._); auto [name, _w0] = _v_exprident; return name == mlc::String("extern"); } return false; }();}

predicates::ExprResult parse_extend_method_body(predicates::Parser after_eq) noexcept{
if (predicates::TokenKind_is_extern(predicates::Parser_kind(after_eq))){
{
return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprExtern(predicates::Parser_span_at_cursor(after_eq))), predicates::Parser_advance(after_eq));
}
}
predicates::ExprResult body_expr_parsed = exprs::parse_expr(after_eq);
if (is_extern_ident_expr(body_expr_parsed.value)){
{
return predicates::expression_parse_result(std::make_shared<ast::Expr>(ast::ExprExtern(ast::expr_span(body_expr_parsed.value))), body_expr_parsed.parser);
}
}
return body_expr_parsed;
}

predicates::Parser extend_method_rest_start(predicates::Parser after_lparen) noexcept{return predicates::TokenKind_is_ident(predicates::Parser_kind(after_lparen)) && predicates::TokenKind_ident(predicates::Parser_kind(after_lparen)) == mlc::String("self") ? advance_past_optional_self_type(predicates::Parser_advance(after_lparen)) : after_lparen;}

predicates::DeclResult parse_extend_method(predicates::Parser parser, mlc::String type_name) noexcept{
mlc::String fn_name = predicates::TokenKind_ident(predicates::Parser_kind(predicates::Parser_advance(parser)));
mlc::String mangled_name = type_name + mlc::String("_") + fn_name;
predicates::Parser after_lparen = predicates::Parser_advance_by(parser, 3);
std::shared_ptr<ast::Param> self_param = std::make_shared<ast::Param>(ast::Param{mlc::String("self"), false, std::make_shared<ast::TypeExpr>(ast::TyNamed(type_name)), false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())), std::make_shared<ast::Pattern>(ast::PatternUnit(ast::span_unknown()))});
mlc::Array<std::shared_ptr<ast::Param>> params = {};
params.push_back(self_param);
predicates::ParamsResult rest_parsed = parse_extend_method_rest_params(extend_method_rest_start(after_lparen));
int index = 0;
while (index < rest_parsed.value.size()){
{
std::shared_ptr<ast::Param> rest_parameter = rest_parsed.value[index];
if (ast::param_name(rest_parameter) != mlc::String("self")){
{
params.push_back(rest_parameter);
}
}
index = index + 1;
}
}
predicates::Parser after_rparen = predicates::Parser_advance(rest_parsed.parser);
predicates::TypeResult return_type_parsed = types::parse_type(parser_after_optional_arrow(after_rparen));
predicates::ExprResult body_parsed = parse_extend_method_body(predicates::Parser_advance(return_type_parsed.parser));
return predicates::declaration_parse_result(std::make_shared<ast::Decl>(ast::DeclFn(mangled_name, {}, {}, params, return_type_parsed.value, body_parsed.value, {})), body_parsed.parser);
}

predicates::DeclResult parse_trait_decl(predicates::Parser parser) noexcept{
predicates::Parser after_trait = predicates::Parser_advance(parser);
predicates::Parser name_parser = parser_expect_ident(after_trait, mlc::String("trait name"));
mlc::String trait_name = predicates::TokenKind_ident(predicates::Parser_kind(name_parser));
ast::Span trait_name_span = predicates::Parser_span_at_cursor(name_parser);
predicates::TypeParamsResult type_parameters_parsed = parse_type_params_opt(predicates::Parser_advance(name_parser));
predicates::TraitBodyResult trait_parsed = parse_trait_body(trait_body_start_parser(type_parameters_parsed.parser), trait_name, type_parameters_parsed.value.params);
return predicates::declaration_parse_result(std::make_shared<ast::Decl>(ast::DeclTrait(trait_name, type_parameters_parsed.value.params, trait_parsed.value, trait_name_span)), trait_parsed.parser);
}

predicates::DeclResult parse_declaration(predicates::Parser parser) noexcept{
ast_tokens::TokenKind kind = predicates::Parser_kind(parser);
return predicates::TokenKind_is_ident(kind) && predicates::TokenKind_ident(kind) == mlc::String("export") ? [&]() -> predicates::DeclResult { 
  predicates::DeclResult inner_parsed = parse_declaration(predicates::Parser_advance(parser));
  return predicates::declaration_parse_result(std::make_shared<ast::Decl>(ast::DeclExported(inner_parsed.value)), inner_parsed.parser);
 }() : predicates::TokenKind_is_ident(kind) && predicates::TokenKind_ident(kind) == mlc::String("trait") ? parse_trait_decl(parser) : predicates::TokenKind_is_fn(kind) || predicates::TokenKind_is_extern(kind) && predicates::TokenKind_is_fn(predicates::Parser_kind(predicates::Parser_advance(parser))) ? parse_function_declaration(parser) : predicates::TokenKind_is_type(kind) ? parse_type_decl(predicates::Parser_advance(parser)) : predicates::TokenKind_is_extend(kind) ? parse_extend_decl(predicates::Parser_advance(parser)) : predicates::TokenKind_is_import(kind) ? parse_import_decl(predicates::Parser_advance(parser)) : [&]() -> predicates::DeclResult { 
  predicates::Parser error_parser = predicates::Parser_record_parse_error(parser, mlc::String("parse: unexpected token at declaration"));
  return predicates::declaration_parse_result(std::make_shared<ast::Decl>(ast::DeclFn(mlc::String("__skip__"), {}, {}, {}, std::make_shared<ast::TypeExpr>((ast::TyUnit{})), std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())), {})), predicates::Parser_advance(error_parser));
 }();
}

predicates::DeclResult parse_function_declaration(predicates::Parser parser) noexcept{
bool is_extern = predicates::TokenKind_is_extern(predicates::Parser_kind(parser));
ast::Span extern_keyword_span = extern_keyword_span_for_parser(parser, is_extern);
predicates::Parser fn_start = parser_after_optional_extern(parser, is_extern);
predicates::Parser name_parser = parser_expect_ident(predicates::Parser_advance(fn_start), mlc::String("function name"));
mlc::String fn_name = predicates::TokenKind_ident(predicates::Parser_kind(name_parser));
predicates::TypeParamsResult type_parameters_parsed = parse_type_params_opt(predicates::Parser_advance(name_parser));
predicates::ParamsResult params_parsed = parse_params(predicates::Parser_advance(type_parameters_parsed.parser));
predicates::Parser after_rparen = predicates::Parser_advance(params_parsed.parser);
predicates::TypeResult return_type_parsed = types::parse_type(parser_after_optional_arrow(after_rparen));
mlc::Array<mlc::Array<mlc::String>> type_bounds = type_parameters_parsed.value.bounds;
predicates::WhereClauseParseResult where_parse = parse_where_clause_opt(return_type_parsed.parser);
mlc::Array<mlc::Array<mlc::String>> merged_trait_bounds = merge_where_clause_trait_bounds(type_parameters_parsed.value.params, type_bounds, where_parse.value);
return is_extern ? predicates::declaration_parse_result(std::make_shared<ast::Decl>(ast::DeclFn(fn_name, type_parameters_parsed.value.params, merged_trait_bounds, params_parsed.value, return_type_parsed.value, std::make_shared<ast::Expr>(ast::ExprExtern(extern_keyword_span)), where_parse.value)), where_parse.parser) : [&]() -> predicates::DeclResult { 
  predicates::ExprResult body_parsed = exprs::parse_expr(predicates::Parser_advance(where_parse.parser));
  return predicates::declaration_parse_result(std::make_shared<ast::Decl>(ast::DeclFn(fn_name, type_parameters_parsed.value.params, merged_trait_bounds, params_parsed.value, return_type_parsed.value, body_parsed.value, where_parse.value)), body_parsed.parser);
 }();
}

predicates::PatternResult parse_tuple_parameter_binding_element(predicates::Parser binding_parser) noexcept{
ast::Span binding_span = predicates::Parser_span_at_cursor(binding_parser);
ast_tokens::TokenKind binding_kind = predicates::Parser_kind(binding_parser);
return predicates::TokenKind_is_ident(binding_kind) ? [&]() -> predicates::PatternResult { 
  mlc::String binding_label = predicates::TokenKind_ident(binding_kind);
  return binding_label == mlc::String("_") ? predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternWild(binding_span)), predicates::Parser_advance(binding_parser)) : predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternIdent(binding_label, binding_span)), predicates::Parser_advance(binding_parser));
 }() : predicates::TokenKind_is_op(binding_kind) && predicates::TokenKind_op_val(binding_kind) == mlc::String("_") ? predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternWild(binding_span)), predicates::Parser_advance(binding_parser)) : predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternWild(binding_span)), predicates::Parser_advance(binding_parser));
}

predicates::PatternResult parse_tuple_parameter_pattern(ast::Span tuple_span, predicates::Parser parser_after_open_paren) noexcept{
predicates::Parser cursor = std::move(parser_after_open_paren);
mlc::Array<std::shared_ptr<ast::Pattern>> tuple_elements = {};
return predicates::TokenKind_is_rparen(predicates::Parser_kind(cursor)) ? predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternTuple(tuple_elements, tuple_span)), predicates::Parser_advance(cursor)) : [&]() -> predicates::PatternResult { 
  bool tuple_parse_finished = false;
  while (!tuple_parse_finished){
{
predicates::PatternResult bound_element = parse_tuple_parameter_binding_element(cursor);
tuple_elements.push_back(bound_element.value);
cursor = bound_element.parser;
if (predicates::TokenKind_is_comma(predicates::Parser_kind(cursor))){
{
cursor = predicates::Parser_advance(cursor);
if (predicates::TokenKind_is_rparen(predicates::Parser_kind(cursor))){
tuple_parse_finished = true;
}
}
} else {
{
if (predicates::TokenKind_is_rparen(predicates::Parser_kind(cursor))){
tuple_parse_finished = true;
} else {
tuple_parse_finished = true;
}
}
}
}
}
  return predicates::pattern_parse_result(std::make_shared<ast::Pattern>(ast::PatternTuple(tuple_elements, tuple_span)), predicates::Parser_advance(cursor));
 }();
}

std::shared_ptr<ast::Pattern> plain_identifier_parameter_pattern_marker() noexcept{return std::make_shared<ast::Pattern>(ast::PatternUnit(ast::span_unknown()));}

predicates::ParamResult parse_param(predicates::Parser parser_for_parameter, int parameter_slot_index) noexcept{
bool mutable_binding_requested = predicates::TokenKind_is_mut(predicates::Parser_kind(parser_for_parameter));
predicates::Parser cursor_after_mutability_keyword = parser_after_optional_mut(parser_for_parameter);
return predicates::TokenKind_is_lbrace(predicates::Parser_kind(cursor_after_mutability_keyword)) ? [&]() -> predicates::ParseResult<std::shared_ptr<ast::Param>> { 
  ast::Span record_pattern_span = predicates::Parser_span_at_cursor(cursor_after_mutability_keyword);
  predicates::PatternsResult shorthand_record_fields = exprs::parse_record_pattern_fields(predicates::Parser_advance(cursor_after_mutability_keyword));
  predicates::Parser cursor_after_record_fields = shorthand_record_fields.parser;
  predicates::TypeResult record_field_type_parse = types::parse_type(predicates::Parser_advance(cursor_after_record_fields));
  predicates::Parser remainder_after_record_type = record_field_type_parse.parser;
  mlc::String synthetic_aggregate_identifier = mlc::String("__mlc_ds") + mlc::to_string(parameter_slot_index);
  std::shared_ptr<ast::Pattern> record_destructure_pattern = std::make_shared<ast::Pattern>(ast::PatternRecord(mlc::String(""), shorthand_record_fields.value, record_pattern_span));
  return predicates::TokenKind_is_equal(predicates::Parser_kind(remainder_after_record_type)) ? [&]() -> predicates::ParseResult<std::shared_ptr<ast::Param>> { 
  predicates::ExprResult parsed_default_expression = exprs::parse_expr(predicates::Parser_advance(remainder_after_record_type));
  return predicates::parse_result<std::shared_ptr<ast::Param>>(std::make_shared<ast::Param>(ast::Param{synthetic_aggregate_identifier, mutable_binding_requested, record_field_type_parse.value, true, parsed_default_expression.value, record_destructure_pattern}), parsed_default_expression.parser);
 }() : predicates::parse_result<std::shared_ptr<ast::Param>>(std::make_shared<ast::Param>(ast::Param{synthetic_aggregate_identifier, mutable_binding_requested, record_field_type_parse.value, false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())), record_destructure_pattern}), remainder_after_record_type);
 }() : predicates::TokenKind_is_lparen(predicates::Parser_kind(cursor_after_mutability_keyword)) ? [&]() -> predicates::ParseResult<std::shared_ptr<ast::Param>> { 
  ast::Span tuple_pattern_span = predicates::Parser_span_at_cursor(cursor_after_mutability_keyword);
  predicates::PatternResult tuple_pattern_parse = parse_tuple_parameter_pattern(tuple_pattern_span, predicates::Parser_advance(cursor_after_mutability_keyword));
  predicates::Parser cursor_after_tuple_pattern = tuple_pattern_parse.parser;
  predicates::TypeResult tuple_parameter_type_parse = types::parse_type(predicates::Parser_advance(cursor_after_tuple_pattern));
  predicates::Parser remainder_after_tuple_type = tuple_parameter_type_parse.parser;
  mlc::String synthetic_tuple_aggregate_identifier = mlc::String("__mlc_ds") + mlc::to_string(parameter_slot_index);
  std::shared_ptr<ast::Pattern> tuple_destructure_pattern = tuple_pattern_parse.value;
  return predicates::TokenKind_is_equal(predicates::Parser_kind(remainder_after_tuple_type)) ? [&]() -> predicates::ParseResult<std::shared_ptr<ast::Param>> { 
  predicates::ExprResult parsed_tuple_default_expression = exprs::parse_expr(predicates::Parser_advance(remainder_after_tuple_type));
  return predicates::parse_result<std::shared_ptr<ast::Param>>(std::make_shared<ast::Param>(ast::Param{synthetic_tuple_aggregate_identifier, mutable_binding_requested, tuple_parameter_type_parse.value, true, parsed_tuple_default_expression.value, tuple_destructure_pattern}), parsed_tuple_default_expression.parser);
 }() : predicates::parse_result<std::shared_ptr<ast::Param>>(std::make_shared<ast::Param>(ast::Param{synthetic_tuple_aggregate_identifier, mutable_binding_requested, tuple_parameter_type_parse.value, false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())), tuple_destructure_pattern}), remainder_after_tuple_type);
 }() : [&]() -> predicates::ParseResult<std::shared_ptr<ast::Param>> { 
  predicates::Parser binding_identifier_token_position = std::move(cursor_after_mutability_keyword);
  mlc::String binding_name = predicates::TokenKind_ident(predicates::Parser_kind(binding_identifier_token_position));
  predicates::Parser after_binding_name = predicates::Parser_advance(binding_identifier_token_position);
  predicates::TypeResult plain_binding_type_parse = types::parse_type(predicates::Parser_advance(after_binding_name));
  predicates::Parser remainder_after_plain_type = plain_binding_type_parse.parser;
  return predicates::TokenKind_is_equal(predicates::Parser_kind(remainder_after_plain_type)) ? [&]() -> predicates::ParseResult<std::shared_ptr<ast::Param>> { 
  predicates::ExprResult parsed_plain_default_expression = exprs::parse_expr(predicates::Parser_advance(remainder_after_plain_type));
  return predicates::parse_result<std::shared_ptr<ast::Param>>(std::make_shared<ast::Param>(ast::Param{binding_name, mutable_binding_requested, plain_binding_type_parse.value, true, parsed_plain_default_expression.value, plain_identifier_parameter_pattern_marker()}), parsed_plain_default_expression.parser);
 }() : predicates::parse_result<std::shared_ptr<ast::Param>>(std::make_shared<ast::Param>(ast::Param{binding_name, mutable_binding_requested, plain_binding_type_parse.value, false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())), plain_identifier_parameter_pattern_marker()}), remainder_after_plain_type);
 }();
}

predicates::ParamsResult parse_params(predicates::Parser parser) noexcept{return predicates::TokenKind_is_rparen(predicates::Parser_kind(parser)) ? [&]() -> predicates::ParamsResult { 
  mlc::Array<std::shared_ptr<ast::Param>> empty_parameters = {};
  return predicates::parameters_parse_result(empty_parameters, predicates::Parser_advance(parser));
 }() : [&]() -> predicates::ParamsResult { 
  predicates::ParamsResult suffix = comma_separated::comma_separated_params_suffix_from_first(parse_param(parser, 0), 1, parse_param);
  return predicates::parameters_parse_result(suffix.value, predicates::Parser_advance(suffix.parser));
 }();}

decls::DeriveResult parse_derive_clause(predicates::Parser parser) noexcept{
return predicates::TokenKind_is_ident(predicates::Parser_kind(parser)) && predicates::TokenKind_ident(predicates::Parser_kind(parser)) == mlc::String("derive") ? [&]() -> predicates::ParseResult<mlc::Array<mlc::String>> { 
  predicates::Parser state = predicates::Parser_advance(parser);
  mlc::Array<mlc::String> traits = {};
  if (predicates::TokenKind_is_lbrace(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
while (predicates::TokenKind_is_ident(predicates::Parser_kind(state))){
{
traits.push_back(predicates::TokenKind_ident(predicates::Parser_kind(state)));
state = predicates::Parser_advance(state);
if (predicates::TokenKind_is_comma(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
}
}
}
}
if (predicates::TokenKind_is_rbrace(predicates::Parser_kind(state))){
state = predicates::Parser_advance(state);
}
}
}
  return predicates::bounds_parse_result(traits, state);
 }() : predicates::bounds_parse_result({}, parser);
}

bool type_decl_after_equal_is_sum_variant(predicates::Parser parser) noexcept{
predicates::Parser state = std::move(parser);
if (predicates::TokenKind_is_ident(predicates::Parser_kind(state)) && predicates::TokenKind_ident(predicates::Parser_kind(state)) == mlc::String("private")){
{
state = predicates::Parser_advance(state);
}
}
return predicates::TokenKind_is_bar(predicates::Parser_kind(state)) ? true : predicates::TokenKind_is_ident(predicates::Parser_kind(state)) ? [&]() -> bool { 
  predicates::Parser after_ident = predicates::Parser_advance(state);
  return predicates::TokenKind_is_bar(predicates::Parser_kind(after_ident)) || predicates::TokenKind_is_lparen(predicates::Parser_kind(after_ident)) || predicates::TokenKind_is_lbrace(predicates::Parser_kind(after_ident));
 }() : false;
}

predicates::DeclResult parse_type_decl(predicates::Parser parser) noexcept{
predicates::Parser name_parser = parser_expect_ident(parser, mlc::String("type name"));
mlc::String type_name = predicates::TokenKind_ident(predicates::Parser_kind(name_parser));
ast::Span type_name_span = predicates::Parser_span_at_cursor(name_parser);
predicates::Parser state = predicates::Parser_advance(name_parser);
predicates::TypeParamsResult type_parameters_parsed = parse_type_params_opt(state);
state = type_parameters_parsed.parser;
return predicates::TokenKind_is_lbrace(predicates::Parser_kind(state)) ? [&]() -> predicates::DeclResult { 
  predicates::TraitBodyResult trait_parsed = parse_trait_body(predicates::Parser_advance(state), type_name, type_parameters_parsed.value.params);
  return predicates::declaration_parse_result(std::make_shared<ast::Decl>(ast::DeclTrait(type_name, type_parameters_parsed.value.params, trait_parsed.value, type_name_span)), trait_parsed.parser);
 }() : predicates::TokenKind_is_equal(predicates::Parser_kind(state)) ? [&]() -> predicates::DeclResult { 
  state = parser_record_error_if_eof_after_equal(predicates::Parser_advance(state));
  ast_tokens::TokenKind first_kind = predicates::Parser_kind(state);
  return predicates::TokenKind_is_lbrace(first_kind) ? [&]() -> predicates::DeclResult { 
  predicates::FieldDefsResult field_defs_parsed = parse_field_defs(predicates::Parser_advance(state));
  mlc::Array<std::shared_ptr<ast::TypeVariant>> variants = {};
  variants.push_back(std::make_shared<ast::TypeVariant>(ast::VarRecord(type_name, field_defs_parsed.value, false)));
  decls::DeriveResult derive_parsed = parse_derive_clause(field_defs_parsed.parser);
  return predicates::declaration_parse_result(std::make_shared<ast::Decl>(ast::DeclType(type_name, type_parameters_parsed.value.params, variants, derive_parsed.value, type_name_span)), derive_parsed.parser);
 }() : type_decl_after_equal_is_sum_variant(state) ? [&]() -> predicates::DeclResult { 
  predicates::VariantsResult variants_parsed = parse_variants(state);
  decls::DeriveResult derive_parsed = parse_derive_clause(variants_parsed.parser);
  return predicates::declaration_parse_result(std::make_shared<ast::Decl>(ast::DeclType(type_name, type_parameters_parsed.value.params, variants_parsed.value, derive_parsed.value, type_name_span)), derive_parsed.parser);
 }() : [&]() -> predicates::DeclResult { 
  predicates::TypeResult type_parsed = types::parse_type(state);
  return predicates::declaration_parse_result(std::make_shared<ast::Decl>(ast::DeclTypeAlias(type_name, type_parameters_parsed.value.params, type_parsed.value, type_name_span)), type_parsed.parser);
 }();
 }() : [&]() -> predicates::DeclResult { 
  predicates::VariantsResult variants_parsed = parse_variants(parser_record_error_if_eof_expect_equal(state));
  decls::DeriveResult derive_parsed = parse_derive_clause(variants_parsed.parser);
  return predicates::declaration_parse_result(std::make_shared<ast::Decl>(ast::DeclType(type_name, type_parameters_parsed.value.params, variants_parsed.value, derive_parsed.value, type_name_span)), derive_parsed.parser);
 }();
}

bool is_decl_start(ast_tokens::TokenKind kind) noexcept{return predicates::TokenKind_is_type(kind) || predicates::TokenKind_is_fn(kind) || predicates::TokenKind_is_extend(kind) || predicates::TokenKind_is_import(kind) || predicates::TokenKind_is_ident(kind) && (predicates::TokenKind_ident(kind) == mlc::String("export") || predicates::TokenKind_ident(kind) == mlc::String("trait") || predicates::TokenKind_ident(kind) == mlc::String("type") || predicates::TokenKind_ident(kind) == mlc::String("fn") || predicates::TokenKind_ident(kind) == mlc::String("extend") || predicates::TokenKind_ident(kind) == mlc::String("import"));}

bool parse_variants_continue(predicates::Parser state) noexcept{return predicates::Parser_at_eof(state) ? false : is_decl_start(predicates::Parser_kind(state)) ? false : predicates::TokenKind_is_ident(predicates::Parser_kind(state)) && predicates::TokenKind_ident(predicates::Parser_kind(state)) == mlc::String("derive") ? false : predicates::TokenKind_is_bar(predicates::Parser_kind(state)) || predicates::TokenKind_is_ident(predicates::Parser_kind(state));}

predicates::VariantsResult parse_variants(predicates::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::TypeVariant>> variants = {};
predicates::Parser state = parser_after_optional_bar(parser);
while (parse_variants_continue(state)){
{
predicates::VariantResult parsed = parse_variant(state);
variants.push_back(parsed.value);
state = parsed.parser;
if (predicates::TokenKind_is_bar(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
}
}
}
}
return predicates::variants_parse_result(variants, state);
}

predicates::VariantResult parse_variant(predicates::Parser parser) noexcept{
bool is_private = predicates::TokenKind_is_ident(predicates::Parser_kind(parser)) && predicates::TokenKind_ident(predicates::Parser_kind(parser)) == mlc::String("private");
predicates::Parser after_private = parser_after_optional_private_keyword(parser);
mlc::String variant_name = predicates::TokenKind_ident(predicates::Parser_kind(after_private));
predicates::Parser after_name = predicates::Parser_advance(after_private);
return predicates::TokenKind_is_lparen(predicates::Parser_kind(after_name)) ? [&]() -> predicates::VariantResult { 
  predicates::TypesResult types_parsed = parse_tuple_types(predicates::Parser_advance(after_name));
  return predicates::variant_parse_result(std::make_shared<ast::TypeVariant>(ast::VarTuple(variant_name, types_parsed.value, is_private)), types_parsed.parser);
 }() : predicates::TokenKind_is_lbrace(predicates::Parser_kind(after_name)) ? [&]() -> predicates::VariantResult { 
  predicates::FieldDefsResult field_defs_parsed = parse_field_defs(predicates::Parser_advance(after_name));
  return predicates::variant_parse_result(std::make_shared<ast::TypeVariant>(ast::VarRecord(variant_name, field_defs_parsed.value, is_private)), field_defs_parsed.parser);
 }() : predicates::variant_parse_result(std::make_shared<ast::TypeVariant>(ast::VarUnit(variant_name, is_private)), after_name);
}

predicates::TypesResult parse_tuple_types(predicates::Parser parser) noexcept{
predicates::TypesResult suffix = comma_separated::comma_separated_types_suffix_from_first(types::parse_type(parser), parse_type);
return predicates::types_parse_result(suffix.value, predicates::Parser_advance(suffix.parser));
}

predicates::TraitBodyResult parse_trait_body(predicates::Parser parser, mlc::String trait_name, mlc::Array<mlc::String> type_params) noexcept{
mlc::Array<std::shared_ptr<ast::Decl>> methods = {};
predicates::Parser state = std::move(parser);
while (!predicates::TokenKind_is_rbrace(predicates::Parser_kind(state)) && !predicates::Parser_at_eof(state)){
{
if (predicates::TokenKind_is_fn(predicates::Parser_kind(state))){
{
predicates::Parser after_fn = predicates::Parser_advance(state);
mlc::String fn_name = predicates::TokenKind_ident(predicates::Parser_kind(after_fn));
mlc::String mangled = trait_name + mlc::String("_") + fn_name;
predicates::ParamsResult params_parsed = parse_params(predicates::Parser_advance(predicates::Parser_advance(after_fn)));
predicates::Parser after_rparen = predicates::Parser_advance(params_parsed.parser);
predicates::TypeResult return_type_parsed = types::parse_type(parser_after_optional_arrow(after_rparen));
mlc::Array<mlc::Array<mlc::String>> initial_trait_bounds = {};
int bounds_index = 0;
while (bounds_index < type_params.size()){
{
initial_trait_bounds.push_back({});
bounds_index = bounds_index + 1;
}
}
predicates::WhereClauseParseResult where_parse = parse_where_clause_opt(return_type_parsed.parser);
mlc::Array<mlc::Array<mlc::String>> merged_trait_bounds = merge_where_clause_trait_bounds(type_params, initial_trait_bounds, where_parse.value);
predicates::ExprResult body_parsed = parse_trait_method_body(where_parse.parser);
state = body_parsed.parser;
methods.push_back(std::make_shared<ast::Decl>(ast::DeclFn(mangled, type_params, merged_trait_bounds, params_parsed.value, return_type_parsed.value, body_parsed.value, where_parse.value)));
}
} else {
{
if (predicates::TokenKind_is_extern(predicates::Parser_kind(state))){
predicates::Parser parser_after_extern = predicates::Parser_advance(state);
if (predicates::TokenKind_is_fn(predicates::Parser_kind(parser_after_extern))){
predicates::Parser parser_after_fn_keyword = predicates::Parser_advance(parser_after_extern);
mlc::String fn_name = predicates::TokenKind_ident(predicates::Parser_kind(parser_after_fn_keyword));
mlc::String mangled = trait_name + mlc::String("_") + fn_name;
predicates::ParamsResult params_parsed = parse_params(predicates::Parser_advance(predicates::Parser_advance(parser_after_fn_keyword)));
predicates::Parser after_rparen = predicates::Parser_advance(params_parsed.parser);
predicates::TypeResult return_type_parsed = types::parse_type(parser_after_optional_arrow(after_rparen));
mlc::Array<mlc::Array<mlc::String>> initial_trait_bounds_extern = {};
int bounds_index_extern = 0;
while (bounds_index_extern < type_params.size()){
{
initial_trait_bounds_extern.push_back({});
bounds_index_extern = bounds_index_extern + 1;
}
}
predicates::WhereClauseParseResult where_parse_extern = parse_where_clause_opt(return_type_parsed.parser);
mlc::Array<mlc::Array<mlc::String>> merged_trait_bounds_extern = merge_where_clause_trait_bounds(type_params, initial_trait_bounds_extern, where_parse_extern.value);
ast::Span extern_method_span = predicates::Parser_span_at_cursor(where_parse_extern.parser);
methods.push_back(std::make_shared<ast::Decl>(ast::DeclFn(mangled, type_params, merged_trait_bounds_extern, params_parsed.value, return_type_parsed.value, std::make_shared<ast::Expr>(ast::ExprExtern(extern_method_span)), where_parse_extern.value)));
state = where_parse_extern.parser;
} else {
state = predicates::Parser_record_parse_error(parser_after_extern, mlc::String("parse: expected fn after extern in trait body"));
}
} else {
if (predicates::TokenKind_is_type(predicates::Parser_kind(state))){
predicates::Parser after_type = predicates::Parser_advance(state);
ast::Span assoc_span = predicates::Parser_span_at_cursor(after_type);
mlc::String assoc_name = predicates::TokenKind_ident(predicates::Parser_kind(after_type));
methods.push_back(std::make_shared<ast::Decl>(ast::DeclAssocType(assoc_name, assoc_span)));
state = predicates::Parser_advance(after_type);
} else {
state = predicates::Parser_advance(predicates::Parser_record_parse_error(state, mlc::String("parse: expected fn or type in trait body")));
}
}
}
}
}
}
state = expect_close::parser_expect_rbrace(state, mlc::String("trait body"));
return predicates::trait_body_parse_result(methods, state);
}

predicates::FieldDefsResult parse_field_defs(predicates::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs = {};
predicates::Parser state = std::move(parser);
while (!predicates::TokenKind_is_rbrace(predicates::Parser_kind(state)) && !predicates::Parser_at_eof(state)){
{
mlc::String field_name = predicates::TokenKind_ident(predicates::Parser_kind(state));
predicates::TypeResult type_parsed = types::parse_type(predicates::Parser_advance_by(state, 2));
predicates::Parser remainder_after_field_type = type_parsed.parser;
if (predicates::TokenKind_is_equal(predicates::Parser_kind(remainder_after_field_type))){
{
predicates::ExprResult default_parsed = exprs::parse_expr(predicates::Parser_advance(remainder_after_field_type));
field_defs.push_back(std::make_shared<ast::FieldDef>(ast::FieldDef{field_name, type_parsed.value, true, default_parsed.value}));
state = default_parsed.parser;
}
} else {
{
field_defs.push_back(std::make_shared<ast::FieldDef>(ast::FieldDef{field_name, type_parsed.value, false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown()))}));
state = remainder_after_field_type;
}
}
if (predicates::TokenKind_is_comma(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
}
}
}
}
state = expect_close::parser_expect_rbrace(state, mlc::String("record type fields"));
return predicates::field_definitions_parse_result(field_defs, state);
}

ast::Program parse_program_with_source_path(mlc::Array<ast_tokens::Token> tokens, mlc::String source_path) noexcept{
mlc::Array<std::shared_ptr<ast::Decl>> declarations = {};
predicates::Parser state = predicates::parser_new_with_source_path(tokens, source_path);
while (!predicates::Parser_at_eof(state)){
{
predicates::DeclResult parsed = parse_declaration(state);
if (!decl_is_parse_skip(parsed.value)){
{
declarations.push_back(parsed.value);
}
}
state = parsed.parser;
}
}
return ast::Program{declarations};
}

predicates::ProgramParseValue parse_program_with_errors(mlc::Array<ast_tokens::Token> tokens, mlc::String source_path) noexcept{
mlc::Array<std::shared_ptr<ast::Decl>> declarations = {};
predicates::Parser state = predicates::parser_new_with_source_path(tokens, source_path);
while (!predicates::Parser_at_eof(state)){
{
predicates::DeclResult parsed = parse_declaration(state);
if (!decl_is_parse_skip(parsed.value)){
{
declarations.push_back(parsed.value);
}
}
state = parsed.parser;
}
}
return predicates::program_parse_result(ast::Program{declarations}, state.errors);
}

ast::Program parse_program(mlc::Array<ast_tokens::Token> tokens) noexcept{return parse_program_with_source_path(tokens, mlc::String(""));}

} // namespace decls
