#include "decls.hpp"

#include "predicates.hpp"
#include "types.hpp"
#include "exprs.hpp"
#include "ast.hpp"

namespace decls {

using namespace predicates;
using namespace types;
using namespace exprs;
using namespace ast;
using namespace ast_tokens;

struct DeriveResult {mlc::Array<mlc::String> traits;predicates::Parser parser;};

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

predicates::DeclResult parse_import_decl(predicates::Parser parser) noexcept;

predicates::DeclResult parse_extend_decl(predicates::Parser parser) noexcept;

predicates::Parser advance_past_optional_self_type(predicates::Parser parser_after_self) noexcept;

predicates::ParamsResult parse_extend_extern_no_self_params(predicates::Parser after_lparen) noexcept;

predicates::ParamsResult parse_extend_extern_rest_params(predicates::Parser after_lparen, bool leading_self) noexcept;

predicates::DeclResult parse_extend_extern_method(predicates::Parser parser, mlc::String type_name) noexcept;

predicates::DeclResult parse_extend_method(predicates::Parser parser, mlc::String type_name) noexcept;

predicates::DeclResult parse_trait_decl(predicates::Parser parser) noexcept;

predicates::DeclResult parse_decl(predicates::Parser parser) noexcept;

predicates::DeclResult parse_fn_decl(predicates::Parser parser) noexcept;

predicates::PatResult parse_tuple_parameter_binding_element(predicates::Parser binding_parser) noexcept;

predicates::PatResult parse_tuple_parameter_pattern(ast::Span tuple_span, predicates::Parser parser_after_open_paren) noexcept;

std::shared_ptr<ast::Pat> plain_identifier_parameter_pattern_marker() noexcept;

predicates::ParamResult parse_param(predicates::Parser parser_for_parameter, int parameter_slot_index) noexcept;

predicates::ParamsResult comma_separated_params_suffix_from_first(predicates::ParamResult first) noexcept;

predicates::ParamsResult parse_params(predicates::Parser parser) noexcept;

decls::DeriveResult parse_derive_clause(predicates::Parser parser) noexcept;

predicates::DeclResult parse_type_decl(predicates::Parser parser) noexcept;

bool is_decl_start(ast_tokens::TKind kind) noexcept;

bool parse_variants_continue(predicates::Parser state) noexcept;

predicates::VariantsResult parse_variants(predicates::Parser parser) noexcept;

predicates::VariantResult parse_variant(predicates::Parser parser) noexcept;

predicates::TypesResult comma_separated_type_suffix_from_first(predicates::TypeResult first) noexcept;

predicates::TypesResult parse_tuple_types(predicates::Parser parser) noexcept;

predicates::TraitBodyResult parse_trait_body(predicates::Parser parser, mlc::String trait_name, mlc::Array<mlc::String> type_params) noexcept;

predicates::FieldDefsResult parse_field_defs(predicates::Parser parser) noexcept;

ast::Program parse_program_with_source_path(mlc::Array<ast_tokens::Token> tokens, mlc::String source_path) noexcept;

ast::Program parse_program(mlc::Array<ast_tokens::Token> tokens) noexcept;

predicates::BoundsResult parse_trait_bound_ident_list(predicates::Parser parser_state) noexcept{
predicates::Parser state = std::move(parser_state);
mlc::Array<mlc::String> bounds = {};
if (predicates::TKind_is_ident(predicates::Parser_kind(state))){
{
bounds.push_back(predicates::TKind_ident(predicates::Parser_kind(state)));
state = predicates::Parser_advance(state);
[&]() { 
  while (predicates::TKind_is_op(predicates::Parser_kind(state)) && predicates::TKind_op_val(predicates::Parser_kind(state)) == mlc::String("+")){
{
state = predicates::Parser_advance(state);
if (predicates::TKind_is_ident(predicates::Parser_kind(state))){
{
bounds.push_back(predicates::TKind_ident(predicates::Parser_kind(state)));
state = predicates::Parser_advance(state);
}
}
}
}
 }();
}
}
return predicates::BoundsResult{bounds, state};
}

predicates::BoundsResult parse_one_param_bounds(predicates::Parser state) noexcept{
return predicates::TKind_is_colon(predicates::Parser_kind(state)) ? parse_trait_bound_ident_list(predicates::Parser_advance(state)) : predicates::BoundsResult{{}, state};
}

predicates::WhereClauseParseResult parse_where_clause_opt(predicates::Parser parser_state) noexcept{
return !predicates::TKind_is_where(predicates::Parser_kind(parser_state)) ? predicates::WhereClauseParseResult{{}, parser_state} : [&]() -> predicates::WhereClauseParseResult { 
  predicates::Parser state = predicates::Parser_advance(parser_state);
  mlc::Array<ast::WhereClauseBound> collected = {};
  while (predicates::TKind_is_ident(predicates::Parser_kind(state))){
{
mlc::String parameter_name = predicates::TKind_ident(predicates::Parser_kind(state));
state = predicates::Parser_advance(state);
if (predicates::TKind_is_colon(predicates::Parser_kind(state))){
{
predicates::BoundsResult trait_result = parse_trait_bound_ident_list(predicates::Parser_advance(state));
state = trait_result.parser;
collected.push_back(ast::WhereClauseBound{parameter_name, trait_result.bounds});
}
}
if (predicates::TKind_is_comma(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
}
}
}
}
  return predicates::WhereClauseParseResult{collected, state};
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
return predicates::TKind_is_op(predicates::Parser_kind(parser)) && predicates::TKind_op_val(predicates::Parser_kind(parser)) == mlc::String("<") ? [&]() -> predicates::TypeParamsResult { 
  predicates::Parser state = predicates::Parser_advance(parser);
  mlc::Array<mlc::String> params = {};
  mlc::Array<mlc::Array<mlc::String>> bounds_list = {};
  if (predicates::TKind_is_ident(predicates::Parser_kind(state))){
{
params.push_back(predicates::TKind_ident(predicates::Parser_kind(state)));
predicates::BoundsResult res = parse_one_param_bounds(predicates::Parser_advance(state));
bounds_list.push_back(res.bounds);
state = res.parser;
[&]() { 
  while (predicates::TKind_is_comma(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
if (predicates::TKind_is_ident(predicates::Parser_kind(state))){
{
params.push_back(predicates::TKind_ident(predicates::Parser_kind(state)));
predicates::BoundsResult res2 = parse_one_param_bounds(predicates::Parser_advance(state));
bounds_list.push_back(res2.bounds);
state = res2.parser;
}
}
}
}
 }();
}
}
  state = predicates::TKind_is_op(predicates::Parser_kind(state)) && predicates::TKind_op_val(predicates::Parser_kind(state)) == mlc::String(">") ? predicates::Parser_advance(state) : state;
  return predicates::TypeParamsResult{params, bounds_list, state};
 }() : predicates::TypeParamsResult{{}, {}, parser};
}

predicates::DeclResult parse_import_decl(predicates::Parser parser) noexcept{
predicates::Parser state = std::move(parser);
mlc::Array<mlc::String> symbols = {};
if (predicates::TKind_is_op(predicates::Parser_kind(state)) && predicates::TKind_op_val(predicates::Parser_kind(state)) == mlc::String("*")){
{
symbols.push_back(mlc::String("*"));
state = predicates::Parser_advance(state);
if (predicates::TKind_is_as(predicates::Parser_kind(state))){
state = predicates::Parser_advance(state);
if (predicates::TKind_is_ident(predicates::Parser_kind(state))){
symbols.push_back(predicates::TKind_ident(predicates::Parser_kind(state)));
state = predicates::Parser_advance(state);
}
}
}
} else {
{
if (predicates::TKind_is_lbrace(predicates::Parser_kind(state))){
state = predicates::Parser_advance(state);
while (predicates::TKind_is_ident(predicates::Parser_kind(state)) || predicates::TKind_is_op(predicates::Parser_kind(state)) && predicates::TKind_op_val(predicates::Parser_kind(state)) == mlc::String("*")){
{
if (predicates::TKind_is_op(predicates::Parser_kind(state)) && predicates::TKind_op_val(predicates::Parser_kind(state)) == mlc::String("*")){
{
symbols.push_back(mlc::String("*"));
state = predicates::Parser_advance(state);
}
} else {
{
symbols.push_back(predicates::TKind_ident(predicates::Parser_kind(state)));
state = predicates::Parser_advance(state);
}
}
if (predicates::TKind_is_comma(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
}
}
}
}
state = predicates::TKind_is_rbrace(predicates::Parser_kind(state)) ? predicates::Parser_advance(state) : state;
}
}
}
state = predicates::TKind_is_from(predicates::Parser_kind(state)) ? predicates::Parser_advance(state) : state;
mlc::String path = predicates::TKind_is_str(predicates::Parser_kind(state)) ? predicates::TKind_str_val(predicates::Parser_kind(state)) : mlc::String("");
return predicates::DeclResult{std::make_shared<ast::Decl>(ast::DeclImport(path, symbols)), predicates::Parser_advance(state)};
}

predicates::DeclResult parse_extend_decl(predicates::Parser parser) noexcept{
mlc::String type_name = predicates::TKind_ident(predicates::Parser_kind(parser));
predicates::Parser state = predicates::Parser_advance(parser);
mlc::String trait_name = mlc::String("");
if (predicates::TKind_is_colon(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
if (predicates::TKind_is_ident(predicates::Parser_kind(state))){
trait_name = predicates::TKind_ident(predicates::Parser_kind(state));
state = predicates::Parser_advance(state);
}
}
}
if (predicates::TKind_is_op(predicates::Parser_kind(state)) && predicates::TKind_op_val(predicates::Parser_kind(state)) == mlc::String("<")){
{
state = predicates::Parser_advance(state);
while (!predicates::Parser_at_eof(state) && !(predicates::TKind_is_op(predicates::Parser_kind(state)) && predicates::TKind_op_val(predicates::Parser_kind(state)) == mlc::String(">"))){
{
state = predicates::Parser_advance(state);
}
}
if (predicates::TKind_is_op(predicates::Parser_kind(state)) && predicates::TKind_op_val(predicates::Parser_kind(state)) == mlc::String(">")){
state = predicates::Parser_advance(state);
}
}
}
predicates::Parser methods_state = predicates::TKind_is_lbrace(predicates::Parser_kind(state)) ? predicates::Parser_advance(state) : state;
mlc::Array<std::shared_ptr<ast::Decl>> methods = {};
while (!predicates::TKind_is_rbrace(predicates::Parser_kind(methods_state)) && !predicates::Parser_at_eof(methods_state)){
{
if (predicates::TKind_is_fn(predicates::Parser_kind(methods_state))){
{
predicates::DeclResult method_result = parse_extend_method(methods_state, type_name);
methods.push_back(method_result.decl);
methods_state = method_result.parser;
}
} else {
{
if (predicates::TKind_is_extern(predicates::Parser_kind(methods_state)) && predicates::TKind_is_fn(predicates::Parser_kind(predicates::Parser_advance(methods_state)))){
predicates::DeclResult method_result = parse_extend_extern_method(predicates::Parser_advance(methods_state), type_name);
methods.push_back(method_result.decl);
methods_state = method_result.parser;
} else {
if (predicates::TKind_is_type(predicates::Parser_kind(methods_state))){
predicates::Parser after_type = predicates::Parser_advance(methods_state);
ast::Span assoc_span = predicates::Parser_span_at_cursor(after_type);
mlc::String assoc_name = predicates::TKind_ident(predicates::Parser_kind(after_type));
predicates::Parser after_name = predicates::Parser_advance(after_type);
if (predicates::TKind_is_equal(predicates::Parser_kind(after_name))){
predicates::TypeResult type_result = types::parse_type(predicates::Parser_advance(after_name));
methods.push_back(std::make_shared<ast::Decl>(ast::DeclAssocBind(assoc_name, type_result.type_expr, assoc_span)));
methods_state = type_result.parser;
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
return predicates::DeclResult{std::make_shared<ast::Decl>(ast::DeclExtend(type_name, trait_name, methods)), predicates::Parser_advance(methods_state)};
}

predicates::Parser advance_past_optional_self_type(predicates::Parser parser_after_self) noexcept{return predicates::TKind_is_colon(predicates::Parser_kind(parser_after_self)) ? types::parse_type(predicates::Parser_advance(parser_after_self)).parser : parser_after_self;}

predicates::ParamsResult parse_extend_extern_no_self_params(predicates::Parser after_lparen) noexcept{return predicates::TKind_is_rparen(predicates::Parser_kind(after_lparen)) ? predicates::ParamsResult{{}, predicates::Parser_advance(after_lparen)} : parse_params(after_lparen);}

predicates::ParamsResult parse_extend_extern_rest_params(predicates::Parser after_lparen, bool leading_self) noexcept{return leading_self ? [&]() -> predicates::ParamsResult { 
  predicates::Parser after_self_type = advance_past_optional_self_type(predicates::Parser_advance(after_lparen));
  return predicates::TKind_is_rparen(predicates::Parser_kind(after_self_type)) ? predicates::ParamsResult{{}, predicates::Parser_advance(after_self_type)} : parse_params(predicates::TKind_is_comma(predicates::Parser_kind(after_self_type)) ? predicates::Parser_advance(after_self_type) : after_self_type);
 }() : parse_extend_extern_no_self_params(after_lparen);}

predicates::DeclResult parse_extend_extern_method(predicates::Parser parser, mlc::String type_name) noexcept{
ast::Span extern_span = predicates::Parser_span_at_cursor(parser);
mlc::String fn_name = predicates::TKind_ident(predicates::Parser_kind(predicates::Parser_advance(parser)));
mlc::String mangled_name = type_name + mlc::String("_") + fn_name;
predicates::Parser after_lparen = predicates::Parser_advance_by(parser, 3);
std::shared_ptr<ast::Param> self_param = std::make_shared<ast::Param>(ast::Param{mlc::String("self"), false, std::make_shared<ast::TypeExpr>(ast::TyNamed(type_name)), false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())), std::make_shared<ast::Pat>(ast::PatUnit(ast::span_unknown()))});
mlc::Array<std::shared_ptr<ast::Param>> params = {};
bool leading_self = predicates::TKind_is_ident(predicates::Parser_kind(after_lparen)) && predicates::TKind_ident(predicates::Parser_kind(after_lparen)) == mlc::String("self");
if (leading_self){
{
params.push_back(self_param);
}
}
predicates::ParamsResult rest_params = parse_extend_extern_rest_params(after_lparen, leading_self);
int index = 0;
while (index < rest_params.params.size()){
{
std::shared_ptr<ast::Param> rp = rest_params.params[index];
if (ast::param_name(rp) != mlc::String("self")){
{
params.push_back(rp);
}
}
index = index + 1;
}
}
predicates::Parser after_rparen = predicates::Parser_advance(rest_params.parser);
predicates::Parser type_parser = predicates::TKind_is_arrow(predicates::Parser_kind(after_rparen)) ? predicates::Parser_advance(after_rparen) : after_rparen;
predicates::TypeResult ret_type_result = types::parse_type(type_parser);
return predicates::DeclResult{std::make_shared<ast::Decl>(ast::DeclFn(mangled_name, {}, {}, params, ret_type_result.type_expr, std::make_shared<ast::Expr>(ast::ExprExtern(extern_span)), {})), ret_type_result.parser};
}

predicates::DeclResult parse_extend_method(predicates::Parser parser, mlc::String type_name) noexcept{
mlc::String fn_name = predicates::TKind_ident(predicates::Parser_kind(predicates::Parser_advance(parser)));
mlc::String mangled_name = type_name + mlc::String("_") + fn_name;
predicates::Parser after_lparen = predicates::Parser_advance_by(parser, 3);
std::shared_ptr<ast::Param> self_param = std::make_shared<ast::Param>(ast::Param{mlc::String("self"), false, std::make_shared<ast::TypeExpr>(ast::TyNamed(type_name)), false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())), std::make_shared<ast::Pat>(ast::PatUnit(ast::span_unknown()))});
mlc::Array<std::shared_ptr<ast::Param>> params = {};
params.push_back(self_param);
predicates::Parser rest_start = predicates::TKind_is_ident(predicates::Parser_kind(after_lparen)) && predicates::TKind_ident(predicates::Parser_kind(after_lparen)) == mlc::String("self") ? advance_past_optional_self_type(predicates::Parser_advance(after_lparen)) : after_lparen;
predicates::ParamsResult rest_params = predicates::TKind_is_rparen(predicates::Parser_kind(rest_start)) ? predicates::ParamsResult{{}, predicates::Parser_advance(rest_start)} : parse_params(predicates::TKind_is_comma(predicates::Parser_kind(rest_start)) ? predicates::Parser_advance(rest_start) : rest_start);
int index = 0;
while (index < rest_params.params.size()){
{
std::shared_ptr<ast::Param> rp = rest_params.params[index];
if (ast::param_name(rp) != mlc::String("self")){
{
params.push_back(rp);
}
}
index = index + 1;
}
}
predicates::Parser after_rparen = predicates::Parser_advance(rest_params.parser);
predicates::Parser type_parser = predicates::TKind_is_arrow(predicates::Parser_kind(after_rparen)) ? predicates::Parser_advance(after_rparen) : after_rparen;
predicates::TypeResult ret_type_result = types::parse_type(type_parser);
predicates::Parser after_eq = predicates::Parser_advance(ret_type_result.parser);
predicates::ExprResult body_result = predicates::TKind_is_extern(predicates::Parser_kind(after_eq)) ? predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprExtern(predicates::Parser_span_at_cursor(after_eq))), predicates::Parser_advance(after_eq)} : [&]() -> predicates::ExprResult { 
  predicates::ExprResult parsed = exprs::parse_expr(after_eq);
  return [&]() -> predicates::ExprResult { if (std::holds_alternative<ast::ExprIdent>((*parsed.expr)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*parsed.expr)._); auto [name, _w0] = _v_exprident; return name == mlc::String("extern") ? predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprExtern(ast::expr_span(parsed.expr))), parsed.parser} : parsed; } return parsed; }();
 }();
return predicates::DeclResult{std::make_shared<ast::Decl>(ast::DeclFn(mangled_name, {}, {}, params, ret_type_result.type_expr, body_result.expr, {})), body_result.parser};
}

predicates::DeclResult parse_trait_decl(predicates::Parser parser) noexcept{
predicates::Parser after_trait = predicates::Parser_advance(parser);
mlc::String trait_name = predicates::TKind_ident(predicates::Parser_kind(after_trait));
predicates::TypeParamsResult type_params_res = parse_type_params_opt(predicates::Parser_advance(after_trait));
predicates::TraitBodyResult trait_result = parse_trait_body(predicates::Parser_advance(type_params_res.parser), trait_name, type_params_res.params);
return predicates::DeclResult{std::make_shared<ast::Decl>(ast::DeclTrait(trait_name, type_params_res.params, trait_result.methods)), trait_result.parser};
}

predicates::DeclResult parse_decl(predicates::Parser parser) noexcept{
ast_tokens::TKind kind = predicates::Parser_kind(parser);
return predicates::TKind_is_ident(kind) && predicates::TKind_ident(kind) == mlc::String("export") ? [&]() -> predicates::DeclResult { 
  predicates::DeclResult inner = parse_decl(predicates::Parser_advance(parser));
  return predicates::DeclResult{std::make_shared<ast::Decl>(ast::DeclExported(inner.decl)), inner.parser};
 }() : predicates::TKind_is_ident(kind) && predicates::TKind_ident(kind) == mlc::String("trait") ? parse_trait_decl(parser) : predicates::TKind_is_fn(kind) || predicates::TKind_is_extern(kind) && predicates::TKind_is_fn(predicates::Parser_kind(predicates::Parser_advance(parser))) ? parse_fn_decl(parser) : predicates::TKind_is_type(kind) ? parse_type_decl(predicates::Parser_advance(parser)) : predicates::TKind_is_extend(kind) ? parse_extend_decl(predicates::Parser_advance(parser)) : predicates::TKind_is_import(kind) ? parse_import_decl(predicates::Parser_advance(parser)) : predicates::DeclResult{std::make_shared<ast::Decl>(ast::DeclFn(mlc::String("__skip__"), {}, {}, {}, std::make_shared<ast::TypeExpr>((ast::TyUnit{})), std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())), {})), predicates::Parser_advance(parser)};
}

predicates::DeclResult parse_fn_decl(predicates::Parser parser) noexcept{
bool is_extern = predicates::TKind_is_extern(predicates::Parser_kind(parser));
ast::Span extern_keyword_span = is_extern ? predicates::Parser_span_at_cursor(parser) : ast::span_unknown();
predicates::Parser fn_start = is_extern ? predicates::Parser_advance(parser) : parser;
predicates::Parser after_name = predicates::Parser_advance(fn_start);
mlc::String fn_name = predicates::TKind_ident(predicates::Parser_kind(after_name));
predicates::TypeParamsResult type_params_res = parse_type_params_opt(predicates::Parser_advance(after_name));
predicates::ParamsResult params_result = parse_params(predicates::Parser_advance(type_params_res.parser));
predicates::Parser after_rparen = predicates::Parser_advance(params_result.parser);
predicates::Parser type_parser = predicates::TKind_is_arrow(predicates::Parser_kind(after_rparen)) ? predicates::Parser_advance(after_rparen) : after_rparen;
predicates::TypeResult ret_type_result = types::parse_type(type_parser);
mlc::Array<mlc::Array<mlc::String>> type_bounds = type_params_res.bounds;
predicates::WhereClauseParseResult where_parse = parse_where_clause_opt(ret_type_result.parser);
mlc::Array<mlc::Array<mlc::String>> merged_trait_bounds = merge_where_clause_trait_bounds(type_params_res.params, type_bounds, where_parse.where_bounds);
return is_extern ? predicates::DeclResult{std::make_shared<ast::Decl>(ast::DeclFn(fn_name, type_params_res.params, merged_trait_bounds, params_result.params, ret_type_result.type_expr, std::make_shared<ast::Expr>(ast::ExprExtern(extern_keyword_span)), where_parse.where_bounds)), where_parse.parser} : [&]() -> predicates::DeclResult { 
  predicates::ExprResult body_result = exprs::parse_expr(predicates::Parser_advance(where_parse.parser));
  return predicates::DeclResult{std::make_shared<ast::Decl>(ast::DeclFn(fn_name, type_params_res.params, merged_trait_bounds, params_result.params, ret_type_result.type_expr, body_result.expr, where_parse.where_bounds)), body_result.parser};
 }();
}

predicates::PatResult parse_tuple_parameter_binding_element(predicates::Parser binding_parser) noexcept{
ast::Span binding_span = predicates::Parser_span_at_cursor(binding_parser);
ast_tokens::TKind binding_kind = predicates::Parser_kind(binding_parser);
return predicates::TKind_is_ident(binding_kind) ? [&]() -> predicates::PatResult { 
  mlc::String binding_label = predicates::TKind_ident(binding_kind);
  return binding_label == mlc::String("_") ? predicates::PatResult{std::make_shared<ast::Pat>(ast::PatWild(binding_span)), predicates::Parser_advance(binding_parser)} : predicates::PatResult{std::make_shared<ast::Pat>(ast::PatIdent(binding_label, binding_span)), predicates::Parser_advance(binding_parser)};
 }() : predicates::TKind_is_op(binding_kind) && predicates::TKind_op_val(binding_kind) == mlc::String("_") ? predicates::PatResult{std::make_shared<ast::Pat>(ast::PatWild(binding_span)), predicates::Parser_advance(binding_parser)} : predicates::PatResult{std::make_shared<ast::Pat>(ast::PatWild(binding_span)), predicates::Parser_advance(binding_parser)};
}

predicates::PatResult parse_tuple_parameter_pattern(ast::Span tuple_span, predicates::Parser parser_after_open_paren) noexcept{
predicates::Parser cursor = std::move(parser_after_open_paren);
mlc::Array<std::shared_ptr<ast::Pat>> tuple_elements = {};
return predicates::TKind_is_rparen(predicates::Parser_kind(cursor)) ? predicates::PatResult{std::make_shared<ast::Pat>(ast::PatTuple(tuple_elements, tuple_span)), predicates::Parser_advance(cursor)} : [&]() -> predicates::PatResult { 
  bool tuple_parse_finished = false;
  while (!tuple_parse_finished){
{
predicates::PatResult bound_element = parse_tuple_parameter_binding_element(cursor);
tuple_elements.push_back(bound_element.pat);
cursor = bound_element.parser;
if (predicates::TKind_is_comma(predicates::Parser_kind(cursor))){
{
cursor = predicates::Parser_advance(cursor);
if (predicates::TKind_is_rparen(predicates::Parser_kind(cursor))){
tuple_parse_finished = true;
}
}
} else {
{
if (predicates::TKind_is_rparen(predicates::Parser_kind(cursor))){
tuple_parse_finished = true;
} else {
tuple_parse_finished = true;
}
}
}
}
}
  return predicates::PatResult{std::make_shared<ast::Pat>(ast::PatTuple(tuple_elements, tuple_span)), predicates::Parser_advance(cursor)};
 }();
}

std::shared_ptr<ast::Pat> plain_identifier_parameter_pattern_marker() noexcept{return std::make_shared<ast::Pat>(ast::PatUnit(ast::span_unknown()));}

predicates::ParamResult parse_param(predicates::Parser parser_for_parameter, int parameter_slot_index) noexcept{
bool mutable_binding_requested = predicates::TKind_is_mut(predicates::Parser_kind(parser_for_parameter));
predicates::Parser cursor_after_mutability_keyword = mutable_binding_requested ? predicates::Parser_advance(parser_for_parameter) : parser_for_parameter;
return predicates::TKind_is_lbrace(predicates::Parser_kind(cursor_after_mutability_keyword)) ? [&]() -> predicates::ParamResult { 
  ast::Span record_pattern_span = predicates::Parser_span_at_cursor(cursor_after_mutability_keyword);
  predicates::PatsResult shorthand_record_fields = exprs::parse_record_pat_fields(predicates::Parser_advance(cursor_after_mutability_keyword));
  predicates::Parser cursor_after_record_fields = shorthand_record_fields.parser;
  predicates::TypeResult record_field_type_parse = types::parse_type(predicates::Parser_advance(cursor_after_record_fields));
  predicates::Parser remainder_after_record_type = record_field_type_parse.parser;
  mlc::String synthetic_aggregate_identifier = mlc::String("__mlc_ds") + mlc::to_string(parameter_slot_index);
  std::shared_ptr<ast::Pat> record_destructure_pattern = std::make_shared<ast::Pat>(ast::PatRecord(mlc::String(""), shorthand_record_fields.pats, record_pattern_span));
  return predicates::TKind_is_equal(predicates::Parser_kind(remainder_after_record_type)) ? [&]() -> predicates::ParamResult { 
  predicates::ExprResult parsed_default_expression = exprs::parse_expr(predicates::Parser_advance(remainder_after_record_type));
  return predicates::ParamResult{std::make_shared<ast::Param>(ast::Param{synthetic_aggregate_identifier, mutable_binding_requested, record_field_type_parse.type_expr, true, parsed_default_expression.expr, record_destructure_pattern}), parsed_default_expression.parser};
 }() : predicates::ParamResult{std::make_shared<ast::Param>(ast::Param{synthetic_aggregate_identifier, mutable_binding_requested, record_field_type_parse.type_expr, false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())), record_destructure_pattern}), remainder_after_record_type};
 }() : predicates::TKind_is_lparen(predicates::Parser_kind(cursor_after_mutability_keyword)) ? [&]() -> predicates::ParamResult { 
  ast::Span tuple_pattern_span = predicates::Parser_span_at_cursor(cursor_after_mutability_keyword);
  predicates::PatResult tuple_pattern_parse = parse_tuple_parameter_pattern(tuple_pattern_span, predicates::Parser_advance(cursor_after_mutability_keyword));
  predicates::Parser cursor_after_tuple_pattern = tuple_pattern_parse.parser;
  predicates::TypeResult tuple_parameter_type_parse = types::parse_type(predicates::Parser_advance(cursor_after_tuple_pattern));
  predicates::Parser remainder_after_tuple_type = tuple_parameter_type_parse.parser;
  mlc::String synthetic_tuple_aggregate_identifier = mlc::String("__mlc_ds") + mlc::to_string(parameter_slot_index);
  std::shared_ptr<ast::Pat> tuple_destructure_pattern = tuple_pattern_parse.pat;
  return predicates::TKind_is_equal(predicates::Parser_kind(remainder_after_tuple_type)) ? [&]() -> predicates::ParamResult { 
  predicates::ExprResult parsed_tuple_default_expression = exprs::parse_expr(predicates::Parser_advance(remainder_after_tuple_type));
  return predicates::ParamResult{std::make_shared<ast::Param>(ast::Param{synthetic_tuple_aggregate_identifier, mutable_binding_requested, tuple_parameter_type_parse.type_expr, true, parsed_tuple_default_expression.expr, tuple_destructure_pattern}), parsed_tuple_default_expression.parser};
 }() : predicates::ParamResult{std::make_shared<ast::Param>(ast::Param{synthetic_tuple_aggregate_identifier, mutable_binding_requested, tuple_parameter_type_parse.type_expr, false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())), tuple_destructure_pattern}), remainder_after_tuple_type};
 }() : [&]() -> predicates::ParamResult { 
  predicates::Parser binding_identifier_token_position = std::move(cursor_after_mutability_keyword);
  mlc::String binding_name = predicates::TKind_ident(predicates::Parser_kind(binding_identifier_token_position));
  predicates::Parser after_binding_name = predicates::Parser_advance(binding_identifier_token_position);
  predicates::TypeResult plain_binding_type_parse = types::parse_type(predicates::Parser_advance(after_binding_name));
  predicates::Parser remainder_after_plain_type = plain_binding_type_parse.parser;
  return predicates::TKind_is_equal(predicates::Parser_kind(remainder_after_plain_type)) ? [&]() -> predicates::ParamResult { 
  predicates::ExprResult parsed_plain_default_expression = exprs::parse_expr(predicates::Parser_advance(remainder_after_plain_type));
  return predicates::ParamResult{std::make_shared<ast::Param>(ast::Param{binding_name, mutable_binding_requested, plain_binding_type_parse.type_expr, true, parsed_plain_default_expression.expr, plain_identifier_parameter_pattern_marker()}), parsed_plain_default_expression.parser};
 }() : predicates::ParamResult{std::make_shared<ast::Param>(ast::Param{binding_name, mutable_binding_requested, plain_binding_type_parse.type_expr, false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())), plain_identifier_parameter_pattern_marker()}), remainder_after_plain_type};
 }();
}

predicates::ParamsResult comma_separated_params_suffix_from_first(predicates::ParamResult first) noexcept{
mlc::Array<std::shared_ptr<ast::Param>> params = {};
params.push_back(first.param);
predicates::Parser state = first.parser;
int parameter_slot_index = 1;
while (predicates::TKind_is_comma(predicates::Parser_kind(state))){
{
predicates::ParamResult next = parse_param(predicates::Parser_advance(state), parameter_slot_index);
params.push_back(next.param);
state = next.parser;
parameter_slot_index = parameter_slot_index + 1;
}
}
return predicates::ParamsResult{params, state};
}

predicates::ParamsResult parse_params(predicates::Parser parser) noexcept{return predicates::TKind_is_rparen(predicates::Parser_kind(parser)) ? predicates::ParamsResult{{}, predicates::Parser_advance(parser)} : [&]() -> predicates::ParamsResult { 
  predicates::ParamsResult suffix = comma_separated_params_suffix_from_first(parse_param(parser, 0));
  return predicates::ParamsResult{suffix.params, predicates::Parser_advance(suffix.parser)};
 }();}

decls::DeriveResult parse_derive_clause(predicates::Parser parser) noexcept{
return predicates::TKind_is_ident(predicates::Parser_kind(parser)) && predicates::TKind_ident(predicates::Parser_kind(parser)) == mlc::String("derive") ? [&]() -> decls::DeriveResult { 
  predicates::Parser state = predicates::Parser_advance(parser);
  mlc::Array<mlc::String> traits = {};
  if (predicates::TKind_is_lbrace(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
while (predicates::TKind_is_ident(predicates::Parser_kind(state))){
{
traits.push_back(predicates::TKind_ident(predicates::Parser_kind(state)));
state = predicates::Parser_advance(state);
if (predicates::TKind_is_comma(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
}
}
}
}
if (predicates::TKind_is_rbrace(predicates::Parser_kind(state))){
state = predicates::Parser_advance(state);
}
}
}
  return decls::DeriveResult{traits, state};
 }() : decls::DeriveResult{{}, parser};
}

predicates::DeclResult parse_type_decl(predicates::Parser parser) noexcept{
mlc::String type_name = predicates::TKind_ident(predicates::Parser_kind(parser));
predicates::Parser state = predicates::Parser_advance(parser);
predicates::TypeParamsResult type_params_res = parse_type_params_opt(state);
state = type_params_res.parser;
return predicates::TKind_is_lbrace(predicates::Parser_kind(state)) ? [&]() -> predicates::DeclResult { 
  predicates::TraitBodyResult trait_result = parse_trait_body(predicates::Parser_advance(state), type_name, type_params_res.params);
  return predicates::DeclResult{std::make_shared<ast::Decl>(ast::DeclTrait(type_name, type_params_res.params, trait_result.methods)), trait_result.parser};
 }() : predicates::TKind_is_equal(predicates::Parser_kind(state)) ? [&]() -> predicates::DeclResult { 
  state = predicates::Parser_advance(state);
  ast_tokens::TKind first_kind = predicates::Parser_kind(state);
  return predicates::TKind_is_lbrace(first_kind) ? [&]() -> predicates::DeclResult { 
  predicates::FieldDefsResult field_defs_result = parse_field_defs(predicates::Parser_advance(state));
  mlc::Array<std::shared_ptr<ast::TypeVariant>> variants = {};
  variants.push_back(std::make_shared<ast::TypeVariant>(ast::VarRecord(type_name, field_defs_result.field_defs, false)));
  decls::DeriveResult derive_res = parse_derive_clause(field_defs_result.parser);
  return predicates::DeclResult{std::make_shared<ast::Decl>(ast::DeclType(type_name, type_params_res.params, variants, derive_res.traits)), derive_res.parser};
 }() : [&]() -> predicates::DeclResult { 
  predicates::VariantsResult variants_result = parse_variants(state);
  decls::DeriveResult derive_res = parse_derive_clause(variants_result.parser);
  return predicates::DeclResult{std::make_shared<ast::Decl>(ast::DeclType(type_name, type_params_res.params, variants_result.variants, derive_res.traits)), derive_res.parser};
 }();
 }() : [&]() -> predicates::DeclResult { 
  predicates::VariantsResult variants_result = parse_variants(state);
  decls::DeriveResult derive_res = parse_derive_clause(variants_result.parser);
  return predicates::DeclResult{std::make_shared<ast::Decl>(ast::DeclType(type_name, type_params_res.params, variants_result.variants, derive_res.traits)), derive_res.parser};
 }();
}

bool is_decl_start(ast_tokens::TKind kind) noexcept{return predicates::TKind_is_type(kind) || predicates::TKind_is_fn(kind) || predicates::TKind_is_extend(kind) || predicates::TKind_is_import(kind) || predicates::TKind_is_ident(kind) && predicates::TKind_ident(kind) == mlc::String("export") || predicates::TKind_ident(kind) == mlc::String("trait") || predicates::TKind_ident(kind) == mlc::String("type") || predicates::TKind_ident(kind) == mlc::String("fn") || predicates::TKind_ident(kind) == mlc::String("extend") || predicates::TKind_ident(kind) == mlc::String("import");}

bool parse_variants_continue(predicates::Parser state) noexcept{return predicates::Parser_at_eof(state) ? false : is_decl_start(predicates::Parser_kind(state)) ? false : predicates::TKind_is_ident(predicates::Parser_kind(state)) && predicates::TKind_ident(predicates::Parser_kind(state)) == mlc::String("derive") ? false : predicates::TKind_is_bar(predicates::Parser_kind(state)) || predicates::TKind_is_ident(predicates::Parser_kind(state));}

predicates::VariantsResult parse_variants(predicates::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::TypeVariant>> variants = {};
predicates::Parser state = predicates::TKind_is_bar(predicates::Parser_kind(parser)) ? predicates::Parser_advance(parser) : parser;
while (parse_variants_continue(state)){
{
predicates::VariantResult variant_result = parse_variant(state);
variants.push_back(variant_result.variant);
state = variant_result.parser;
if (predicates::TKind_is_bar(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
}
}
}
}
return predicates::VariantsResult{variants, state};
}

predicates::VariantResult parse_variant(predicates::Parser parser) noexcept{
bool is_private = predicates::TKind_is_ident(predicates::Parser_kind(parser)) && predicates::TKind_ident(predicates::Parser_kind(parser)) == mlc::String("private");
predicates::Parser after_private = is_private ? predicates::Parser_advance(parser) : parser;
mlc::String variant_name = predicates::TKind_ident(predicates::Parser_kind(after_private));
predicates::Parser after_name = predicates::Parser_advance(after_private);
return predicates::TKind_is_lparen(predicates::Parser_kind(after_name)) ? [&]() -> predicates::VariantResult { 
  predicates::TypesResult types_result = parse_tuple_types(predicates::Parser_advance(after_name));
  return predicates::VariantResult{std::make_shared<ast::TypeVariant>(ast::VarTuple(variant_name, types_result.types, is_private)), types_result.parser};
 }() : predicates::TKind_is_lbrace(predicates::Parser_kind(after_name)) ? [&]() -> predicates::VariantResult { 
  predicates::FieldDefsResult field_defs_result = parse_field_defs(predicates::Parser_advance(after_name));
  return predicates::VariantResult{std::make_shared<ast::TypeVariant>(ast::VarRecord(variant_name, field_defs_result.field_defs, is_private)), field_defs_result.parser};
 }() : predicates::VariantResult{std::make_shared<ast::TypeVariant>(ast::VarUnit(variant_name, is_private)), after_name};
}

predicates::TypesResult comma_separated_type_suffix_from_first(predicates::TypeResult first) noexcept{
mlc::Array<std::shared_ptr<ast::TypeExpr>> types = {};
types.push_back(first.type_expr);
predicates::Parser state = first.parser;
while (predicates::TKind_is_comma(predicates::Parser_kind(state))){
{
predicates::TypeResult next = types::parse_type(predicates::Parser_advance(state));
types.push_back(next.type_expr);
state = next.parser;
}
}
return predicates::TypesResult{types, state};
}

predicates::TypesResult parse_tuple_types(predicates::Parser parser) noexcept{
predicates::TypesResult suffix = comma_separated_type_suffix_from_first(types::parse_type(parser));
return predicates::TypesResult{suffix.types, predicates::Parser_advance(suffix.parser)};
}

predicates::TraitBodyResult parse_trait_body(predicates::Parser parser, mlc::String trait_name, mlc::Array<mlc::String> type_params) noexcept{
mlc::Array<std::shared_ptr<ast::Decl>> methods = {};
predicates::Parser state = std::move(parser);
while (!predicates::TKind_is_rbrace(predicates::Parser_kind(state)) && !predicates::Parser_at_eof(state)){
{
if (predicates::TKind_is_fn(predicates::Parser_kind(state))){
{
predicates::Parser after_fn = predicates::Parser_advance(state);
mlc::String fn_name = predicates::TKind_ident(predicates::Parser_kind(after_fn));
mlc::String mangled = trait_name + mlc::String("_") + fn_name;
predicates::ParamsResult params_result = parse_params(predicates::Parser_advance(predicates::Parser_advance(after_fn)));
predicates::Parser after_rparen = predicates::Parser_advance(params_result.parser);
predicates::Parser type_parser = predicates::TKind_is_arrow(predicates::Parser_kind(after_rparen)) ? predicates::Parser_advance(after_rparen) : after_rparen;
predicates::TypeResult ret_result = types::parse_type(type_parser);
mlc::Array<mlc::Array<mlc::String>> initial_trait_bounds = {};
int bi = 0;
while (bi < type_params.size()){
{
initial_trait_bounds.push_back({});
bi = bi + 1;
}
}
predicates::WhereClauseParseResult where_parse = parse_where_clause_opt(ret_result.parser);
mlc::Array<mlc::Array<mlc::String>> merged_trait_bounds = merge_where_clause_trait_bounds(type_params, initial_trait_bounds, where_parse.where_bounds);
std::shared_ptr<ast::Expr> body = predicates::TKind_is_equal(predicates::Parser_kind(where_parse.parser)) ? [&]() -> std::shared_ptr<ast::Expr> { 
  predicates::ExprResult body_res = exprs::parse_expr(predicates::Parser_advance(where_parse.parser));
  state = body_res.parser;
  return body_res.expr;
 }() : [&]() -> std::shared_ptr<ast::Expr> { 
  ast::Span extern_method_span = predicates::Parser_span_at_cursor(where_parse.parser);
  state = where_parse.parser;
  return std::make_shared<ast::Expr>(ast::ExprExtern(extern_method_span));
 }();
methods.push_back(std::make_shared<ast::Decl>(ast::DeclFn(mangled, type_params, merged_trait_bounds, params_result.params, ret_result.type_expr, body, where_parse.where_bounds)));
}
} else {
{
if (predicates::TKind_is_extern(predicates::Parser_kind(state))){
predicates::Parser parser_after_extern = predicates::Parser_advance(state);
if (predicates::TKind_is_fn(predicates::Parser_kind(parser_after_extern))){
predicates::Parser parser_after_fn_keyword = predicates::Parser_advance(parser_after_extern);
mlc::String fn_name = predicates::TKind_ident(predicates::Parser_kind(parser_after_fn_keyword));
mlc::String mangled = trait_name + mlc::String("_") + fn_name;
predicates::ParamsResult params_result = parse_params(predicates::Parser_advance(predicates::Parser_advance(parser_after_fn_keyword)));
predicates::Parser after_rparen = predicates::Parser_advance(params_result.parser);
predicates::Parser type_parser = predicates::TKind_is_arrow(predicates::Parser_kind(after_rparen)) ? predicates::Parser_advance(after_rparen) : after_rparen;
predicates::TypeResult ret_result = types::parse_type(type_parser);
mlc::Array<mlc::Array<mlc::String>> initial_trait_bounds_extern = {};
int be = 0;
while (be < type_params.size()){
{
initial_trait_bounds_extern.push_back({});
be = be + 1;
}
}
predicates::WhereClauseParseResult where_parse_extern = parse_where_clause_opt(ret_result.parser);
mlc::Array<mlc::Array<mlc::String>> merged_trait_bounds_extern = merge_where_clause_trait_bounds(type_params, initial_trait_bounds_extern, where_parse_extern.where_bounds);
methods.push_back(std::make_shared<ast::Decl>(ast::DeclFn(mangled, type_params, merged_trait_bounds_extern, params_result.params, ret_result.type_expr, std::make_shared<ast::Expr>(ast::ExprExtern(ast::span_unknown())), where_parse_extern.where_bounds)));
state = where_parse_extern.parser;
} else {
state = parser_after_extern;
}
} else {
if (predicates::TKind_is_type(predicates::Parser_kind(state))){
predicates::Parser after_type = predicates::Parser_advance(state);
ast::Span assoc_span = predicates::Parser_span_at_cursor(after_type);
mlc::String assoc_name = predicates::TKind_ident(predicates::Parser_kind(after_type));
methods.push_back(std::make_shared<ast::Decl>(ast::DeclAssocType(assoc_name, assoc_span)));
state = predicates::Parser_advance(after_type);
} else {
state = predicates::Parser_advance(state);
}
}
}
}
}
}
return predicates::TraitBodyResult{methods, predicates::Parser_advance(state)};
}

predicates::FieldDefsResult parse_field_defs(predicates::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs = {};
predicates::Parser state = std::move(parser);
while (!predicates::TKind_is_rbrace(predicates::Parser_kind(state)) && !predicates::Parser_at_eof(state)){
{
mlc::String field_name = predicates::TKind_ident(predicates::Parser_kind(state));
predicates::TypeResult type_result = types::parse_type(predicates::Parser_advance_by(state, 2));
predicates::Parser remainder_after_field_type = type_result.parser;
if (predicates::TKind_is_equal(predicates::Parser_kind(remainder_after_field_type))){
{
predicates::ExprResult default_parse_result = exprs::parse_expr(predicates::Parser_advance(remainder_after_field_type));
field_defs.push_back(std::make_shared<ast::FieldDef>(ast::FieldDef{field_name, type_result.type_expr, true, default_parse_result.expr}));
state = default_parse_result.parser;
}
} else {
{
field_defs.push_back(std::make_shared<ast::FieldDef>(ast::FieldDef{field_name, type_result.type_expr, false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown()))}));
state = remainder_after_field_type;
}
}
if (predicates::TKind_is_comma(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
}
}
}
}
return predicates::FieldDefsResult{field_defs, predicates::Parser_advance(state)};
}

ast::Program parse_program_with_source_path(mlc::Array<ast_tokens::Token> tokens, mlc::String source_path) noexcept{
mlc::Array<std::shared_ptr<ast::Decl>> decls = {};
predicates::Parser state = predicates::parser_new_with_source_path(tokens, source_path);
while (!predicates::Parser_at_eof(state)){
{
predicates::DeclResult result = parse_decl(state);
decls.push_back(result.decl);
state = result.parser;
}
}
return ast::Program{decls};
}

ast::Program parse_program(mlc::Array<ast_tokens::Token> tokens) noexcept{return parse_program_with_source_path(tokens, mlc::String(""));}

} // namespace decls
