#include "decls.hpp"

#include "preds.hpp"
#include "types.hpp"
#include "exprs.hpp"
#include "ast.hpp"

namespace decls {

using namespace preds;
using namespace types;
using namespace exprs;
using namespace ast;
using namespace ast_tokens;

struct DeriveResult {mlc::Array<mlc::String> traits;preds::Parser parser;};

preds::BoundsResult parse_one_param_bounds(preds::Parser state) noexcept;

preds::TypeParamsResult parse_type_params_opt(preds::Parser parser) noexcept;

preds::DeclResult parse_import_decl(preds::Parser parser) noexcept;

preds::DeclResult parse_extend_decl(preds::Parser parser) noexcept;

preds::Parser advance_past_optional_self_type(preds::Parser parser_after_self) noexcept;

preds::ParamsResult parse_extend_extern_no_self_params(preds::Parser after_lparen) noexcept;

preds::ParamsResult parse_extend_extern_rest_params(preds::Parser after_lparen, bool leading_self) noexcept;

preds::DeclResult parse_extend_extern_method(preds::Parser parser, mlc::String type_name) noexcept;

preds::DeclResult parse_extend_method(preds::Parser parser, mlc::String type_name) noexcept;

preds::DeclResult parse_decl(preds::Parser parser) noexcept;

preds::DeclResult parse_fn_decl(preds::Parser parser) noexcept;

preds::ParamsResult parse_params(preds::Parser parser) noexcept;

preds::ParamResult parse_param(preds::Parser parser) noexcept;

decls::DeriveResult parse_derive_clause(preds::Parser parser) noexcept;

preds::DeclResult parse_type_decl(preds::Parser parser) noexcept;

bool is_decl_start(ast_tokens::TKind kind) noexcept;

bool parse_variants_continue(preds::Parser state) noexcept;

preds::VariantsResult parse_variants(preds::Parser parser) noexcept;

preds::VariantResult parse_variant(preds::Parser parser) noexcept;

preds::TypesResult parse_tuple_types(preds::Parser parser) noexcept;

preds::TraitBodyResult parse_trait_body(preds::Parser parser, mlc::String trait_name, mlc::Array<mlc::String> type_params) noexcept;

preds::FieldDefsResult parse_field_defs(preds::Parser parser) noexcept;

ast::Program parse_program_with_source_path(mlc::Array<ast_tokens::Token> tokens, mlc::String source_path) noexcept;

ast::Program parse_program(mlc::Array<ast_tokens::Token> tokens) noexcept;

preds::BoundsResult parse_one_param_bounds(preds::Parser state) noexcept{
return preds::TKind_is_colon(preds::Parser_kind(state)) ? [&]() -> preds::BoundsResult { 
  preds::Parser s = preds::Parser_advance(state);
  mlc::Array<mlc::String> bounds = {};
  if (preds::TKind_is_ident(preds::Parser_kind(s))){
{
bounds.push_back(preds::TKind_ident(preds::Parser_kind(s)));
s = preds::Parser_advance(s);
[&]() { 
  while (preds::TKind_is_op(preds::Parser_kind(s)) && preds::TKind_op_val(preds::Parser_kind(s)) == mlc::String("+")){
{
s = preds::Parser_advance(s);
if (preds::TKind_is_ident(preds::Parser_kind(s))){
{
bounds.push_back(preds::TKind_ident(preds::Parser_kind(s)));
s = preds::Parser_advance(s);
}
}
}
}
 }();
}
}
  return preds::BoundsResult{bounds, s};
 }() : preds::BoundsResult{{}, state};
}

preds::TypeParamsResult parse_type_params_opt(preds::Parser parser) noexcept{
return preds::TKind_is_op(preds::Parser_kind(parser)) && preds::TKind_op_val(preds::Parser_kind(parser)) == mlc::String("<") ? [&]() -> preds::TypeParamsResult { 
  preds::Parser state = preds::Parser_advance(parser);
  mlc::Array<mlc::String> params = {};
  mlc::Array<mlc::Array<mlc::String>> bounds_list = {};
  if (preds::TKind_is_ident(preds::Parser_kind(state))){
{
params.push_back(preds::TKind_ident(preds::Parser_kind(state)));
preds::BoundsResult res = parse_one_param_bounds(preds::Parser_advance(state));
bounds_list.push_back(res.bounds);
state = res.parser;
[&]() { 
  while (preds::TKind_is_comma(preds::Parser_kind(state))){
{
state = preds::Parser_advance(state);
if (preds::TKind_is_ident(preds::Parser_kind(state))){
{
params.push_back(preds::TKind_ident(preds::Parser_kind(state)));
preds::BoundsResult res2 = parse_one_param_bounds(preds::Parser_advance(state));
bounds_list.push_back(res2.bounds);
state = res2.parser;
}
}
}
}
 }();
}
}
  state = preds::TKind_is_op(preds::Parser_kind(state)) && preds::TKind_op_val(preds::Parser_kind(state)) == mlc::String(">") ? preds::Parser_advance(state) : state;
  return preds::TypeParamsResult{params, bounds_list, state};
 }() : preds::TypeParamsResult{{}, {}, parser};
}

preds::DeclResult parse_import_decl(preds::Parser parser) noexcept{
preds::Parser state = std::move(parser);
mlc::Array<mlc::String> symbols = {};
if (preds::TKind_is_op(preds::Parser_kind(state)) && preds::TKind_op_val(preds::Parser_kind(state)) == mlc::String("*")){
{
symbols.push_back(mlc::String("*"));
state = preds::Parser_advance(state);
if (preds::TKind_is_as(preds::Parser_kind(state))){
state = preds::Parser_advance(state);
if (preds::TKind_is_ident(preds::Parser_kind(state))){
symbols.push_back(preds::TKind_ident(preds::Parser_kind(state)));
state = preds::Parser_advance(state);
}
}
}
} else {
{
if (preds::TKind_is_lbrace(preds::Parser_kind(state))){
state = preds::Parser_advance(state);
while (preds::TKind_is_ident(preds::Parser_kind(state)) || preds::TKind_is_op(preds::Parser_kind(state)) && preds::TKind_op_val(preds::Parser_kind(state)) == mlc::String("*")){
{
if (preds::TKind_is_op(preds::Parser_kind(state)) && preds::TKind_op_val(preds::Parser_kind(state)) == mlc::String("*")){
{
symbols.push_back(mlc::String("*"));
state = preds::Parser_advance(state);
}
} else {
{
symbols.push_back(preds::TKind_ident(preds::Parser_kind(state)));
state = preds::Parser_advance(state);
}
}
if (preds::TKind_is_comma(preds::Parser_kind(state))){
{
state = preds::Parser_advance(state);
}
}
}
}
state = preds::TKind_is_rbrace(preds::Parser_kind(state)) ? preds::Parser_advance(state) : state;
}
}
}
state = preds::TKind_is_from(preds::Parser_kind(state)) ? preds::Parser_advance(state) : state;
mlc::String path = preds::TKind_is_str(preds::Parser_kind(state)) ? preds::TKind_str_val(preds::Parser_kind(state)) : mlc::String("");
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclImport(path, symbols)), preds::Parser_advance(state)};
}

preds::DeclResult parse_extend_decl(preds::Parser parser) noexcept{
mlc::String type_name = preds::TKind_ident(preds::Parser_kind(parser));
preds::Parser state = preds::Parser_advance(parser);
mlc::String trait_name = mlc::String("");
if (preds::TKind_is_colon(preds::Parser_kind(state))){
{
state = preds::Parser_advance(state);
if (preds::TKind_is_ident(preds::Parser_kind(state))){
trait_name = preds::TKind_ident(preds::Parser_kind(state));
state = preds::Parser_advance(state);
}
}
}
if (preds::TKind_is_op(preds::Parser_kind(state)) && preds::TKind_op_val(preds::Parser_kind(state)) == mlc::String("<")){
{
state = preds::Parser_advance(state);
while (!preds::Parser_at_eof(state) && !(preds::TKind_is_op(preds::Parser_kind(state)) && preds::TKind_op_val(preds::Parser_kind(state)) == mlc::String(">"))){
{
state = preds::Parser_advance(state);
}
}
if (preds::TKind_is_op(preds::Parser_kind(state)) && preds::TKind_op_val(preds::Parser_kind(state)) == mlc::String(">")){
state = preds::Parser_advance(state);
}
}
}
preds::Parser methods_state = preds::TKind_is_lbrace(preds::Parser_kind(state)) ? preds::Parser_advance(state) : state;
mlc::Array<std::shared_ptr<ast::Decl>> methods = {};
while (!preds::TKind_is_rbrace(preds::Parser_kind(methods_state)) && !preds::Parser_at_eof(methods_state)){
{
if (preds::TKind_is_fn(preds::Parser_kind(methods_state))){
{
preds::DeclResult method_result = parse_extend_method(methods_state, type_name);
methods.push_back(method_result.decl);
methods_state = method_result.parser;
}
} else {
{
if (preds::TKind_is_extern(preds::Parser_kind(methods_state)) && preds::TKind_is_fn(preds::Parser_kind(preds::Parser_advance(methods_state)))){
preds::DeclResult method_result = parse_extend_extern_method(preds::Parser_advance(methods_state), type_name);
methods.push_back(method_result.decl);
methods_state = method_result.parser;
} else {
if (preds::TKind_is_type(preds::Parser_kind(methods_state))){
preds::Parser after_type = preds::Parser_advance(methods_state);
ast::Span assoc_span = preds::Parser_span_at_cursor(after_type);
mlc::String assoc_name = preds::TKind_ident(preds::Parser_kind(after_type));
preds::Parser after_name = preds::Parser_advance(after_type);
if (preds::TKind_is_equal(preds::Parser_kind(after_name))){
preds::TypeResult type_result = types::parse_type(preds::Parser_advance(after_name));
methods.push_back(std::make_shared<ast::Decl>(ast::DeclAssocBind(assoc_name, type_result.type_expr, assoc_span)));
methods_state = type_result.parser;
} else {
methods.push_back(std::make_shared<ast::Decl>(ast::DeclAssocType(assoc_name, assoc_span)));
methods_state = after_name;
}
} else {
methods_state = preds::Parser_advance(methods_state);
}
}
}
}
}
}
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclExtend(type_name, trait_name, methods)), preds::Parser_advance(methods_state)};
}

preds::Parser advance_past_optional_self_type(preds::Parser parser_after_self) noexcept{return preds::TKind_is_colon(preds::Parser_kind(parser_after_self)) ? types::parse_type(preds::Parser_advance(parser_after_self)).parser : parser_after_self;}

preds::ParamsResult parse_extend_extern_no_self_params(preds::Parser after_lparen) noexcept{return preds::TKind_is_rparen(preds::Parser_kind(after_lparen)) ? preds::ParamsResult{{}, preds::Parser_advance(after_lparen)} : parse_params(after_lparen);}

preds::ParamsResult parse_extend_extern_rest_params(preds::Parser after_lparen, bool leading_self) noexcept{return leading_self ? [&]() -> preds::ParamsResult { 
  preds::Parser after_self_type = advance_past_optional_self_type(preds::Parser_advance(after_lparen));
  return preds::TKind_is_rparen(preds::Parser_kind(after_self_type)) ? preds::ParamsResult{{}, preds::Parser_advance(after_self_type)} : parse_params(preds::TKind_is_comma(preds::Parser_kind(after_self_type)) ? preds::Parser_advance(after_self_type) : after_self_type);
 }() : parse_extend_extern_no_self_params(after_lparen);}

preds::DeclResult parse_extend_extern_method(preds::Parser parser, mlc::String type_name) noexcept{
ast::Span extern_span = preds::Parser_span_at_cursor(parser);
mlc::String fn_name = preds::TKind_ident(preds::Parser_kind(preds::Parser_advance(parser)));
mlc::String mangled_name = type_name + mlc::String("_") + fn_name;
preds::Parser after_lparen = preds::Parser_advance_by(parser, 3);
std::shared_ptr<ast::Param> self_param = std::make_shared<ast::Param>(ast::Param{mlc::String("self"), false, std::make_shared<ast::TypeExpr>(ast::TyNamed(type_name)), false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown()))});
mlc::Array<std::shared_ptr<ast::Param>> params = {};
bool leading_self = preds::TKind_is_ident(preds::Parser_kind(after_lparen)) && preds::TKind_ident(preds::Parser_kind(after_lparen)) == mlc::String("self");
if (leading_self){
{
params.push_back(self_param);
}
}
preds::ParamsResult rest_params = parse_extend_extern_rest_params(after_lparen, leading_self);
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
preds::Parser after_rparen = preds::Parser_advance(rest_params.parser);
preds::Parser type_parser = preds::TKind_is_arrow(preds::Parser_kind(after_rparen)) ? preds::Parser_advance(after_rparen) : after_rparen;
preds::TypeResult ret_type_result = types::parse_type(type_parser);
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclFn(mangled_name, {}, {}, params, ret_type_result.type_expr, std::make_shared<ast::Expr>(ast::ExprExtern(extern_span)))), ret_type_result.parser};
}

preds::DeclResult parse_extend_method(preds::Parser parser, mlc::String type_name) noexcept{
mlc::String fn_name = preds::TKind_ident(preds::Parser_kind(preds::Parser_advance(parser)));
mlc::String mangled_name = type_name + mlc::String("_") + fn_name;
preds::Parser after_lparen = preds::Parser_advance_by(parser, 3);
std::shared_ptr<ast::Param> self_param = std::make_shared<ast::Param>(ast::Param{mlc::String("self"), false, std::make_shared<ast::TypeExpr>(ast::TyNamed(type_name)), false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown()))});
mlc::Array<std::shared_ptr<ast::Param>> params = {};
params.push_back(self_param);
preds::Parser rest_start = preds::TKind_is_ident(preds::Parser_kind(after_lparen)) && preds::TKind_ident(preds::Parser_kind(after_lparen)) == mlc::String("self") ? advance_past_optional_self_type(preds::Parser_advance(after_lparen)) : after_lparen;
preds::ParamsResult rest_params = preds::TKind_is_rparen(preds::Parser_kind(rest_start)) ? preds::ParamsResult{{}, preds::Parser_advance(rest_start)} : parse_params(preds::TKind_is_comma(preds::Parser_kind(rest_start)) ? preds::Parser_advance(rest_start) : rest_start);
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
preds::Parser after_rparen = preds::Parser_advance(rest_params.parser);
preds::Parser type_parser = preds::TKind_is_arrow(preds::Parser_kind(after_rparen)) ? preds::Parser_advance(after_rparen) : after_rparen;
preds::TypeResult ret_type_result = types::parse_type(type_parser);
preds::Parser after_eq = preds::Parser_advance(ret_type_result.parser);
preds::ExprResult body_result = preds::TKind_is_extern(preds::Parser_kind(after_eq)) ? preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprExtern(preds::Parser_span_at_cursor(after_eq))), preds::Parser_advance(after_eq)} : [&]() -> preds::ExprResult { 
  preds::ExprResult parsed = exprs::parse_expr(after_eq);
  return [&]() -> preds::ExprResult { if (std::holds_alternative<ast::ExprIdent>((*parsed.expr)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*parsed.expr)._); auto [name, _w0] = _v_exprident; return name == mlc::String("extern") ? preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprExtern(ast::expr_span(parsed.expr))), parsed.parser} : parsed; } return parsed; }();
 }();
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclFn(mangled_name, {}, {}, params, ret_type_result.type_expr, body_result.expr)), body_result.parser};
}

preds::DeclResult parse_decl(preds::Parser parser) noexcept{
ast_tokens::TKind kind = preds::Parser_kind(parser);
return preds::TKind_is_ident(kind) && preds::TKind_ident(kind) == mlc::String("export") ? [&]() -> preds::DeclResult { 
  preds::DeclResult inner = parse_decl(preds::Parser_advance(parser));
  return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclExported(inner.decl)), inner.parser};
 }() : preds::TKind_is_fn(kind) || preds::TKind_is_extern(kind) && preds::TKind_is_fn(preds::Parser_kind(preds::Parser_advance(parser))) ? parse_fn_decl(parser) : preds::TKind_is_type(kind) ? parse_type_decl(preds::Parser_advance(parser)) : preds::TKind_is_extend(kind) ? parse_extend_decl(preds::Parser_advance(parser)) : preds::TKind_is_import(kind) ? parse_import_decl(preds::Parser_advance(parser)) : preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclFn(mlc::String("__skip__"), {}, {}, {}, std::make_shared<ast::TypeExpr>((ast::TyUnit{})), std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())))), preds::Parser_advance(parser)};
}

preds::DeclResult parse_fn_decl(preds::Parser parser) noexcept{
bool is_extern = preds::TKind_is_extern(preds::Parser_kind(parser));
ast::Span extern_keyword_span = is_extern ? preds::Parser_span_at_cursor(parser) : ast::span_unknown();
preds::Parser fn_start = is_extern ? preds::Parser_advance(parser) : parser;
preds::Parser after_name = preds::Parser_advance(fn_start);
mlc::String fn_name = preds::TKind_ident(preds::Parser_kind(after_name));
preds::TypeParamsResult type_params_res = parse_type_params_opt(preds::Parser_advance(after_name));
preds::ParamsResult params_result = parse_params(preds::Parser_advance(type_params_res.parser));
preds::Parser after_rparen = preds::Parser_advance(params_result.parser);
preds::Parser type_parser = preds::TKind_is_arrow(preds::Parser_kind(after_rparen)) ? preds::Parser_advance(after_rparen) : after_rparen;
preds::TypeResult ret_type_result = types::parse_type(type_parser);
mlc::Array<mlc::Array<mlc::String>> type_bounds = type_params_res.bounds;
return is_extern ? preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclFn(fn_name, type_params_res.params, type_bounds, params_result.params, ret_type_result.type_expr, std::make_shared<ast::Expr>(ast::ExprExtern(extern_keyword_span)))), ret_type_result.parser} : [&]() -> preds::DeclResult { 
  preds::ExprResult body_result = exprs::parse_expr(preds::Parser_advance(ret_type_result.parser));
  return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclFn(fn_name, type_params_res.params, type_bounds, params_result.params, ret_type_result.type_expr, body_result.expr)), body_result.parser};
 }();
}

preds::ParamsResult parse_params(preds::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::Param>> params = {};
return preds::TKind_is_rparen(preds::Parser_kind(parser)) ? preds::ParamsResult{params, preds::Parser_advance(parser)} : [&]() -> preds::ParamsResult { 
  preds::ParamResult first = parse_param(parser);
  params.push_back(first.param);
  preds::Parser state = first.parser;
  while (preds::TKind_is_comma(preds::Parser_kind(state))){
{
preds::ParamResult next = parse_param(preds::Parser_advance(state));
params.push_back(next.param);
state = next.parser;
}
}
  return preds::ParamsResult{params, preds::Parser_advance(state)};
 }();
}

preds::ParamResult parse_param(preds::Parser parser) noexcept{
bool is_mutable = preds::TKind_is_mut(preds::Parser_kind(parser));
preds::Parser name_pos = is_mutable ? preds::Parser_advance(parser) : parser;
preds::TypeResult type_result = types::parse_type(preds::Parser_advance_by(name_pos, 2));
preds::Parser s = type_result.parser;
return preds::TKind_is_equal(preds::Parser_kind(s)) ? [&]() -> preds::ParamResult { 
  preds::ExprResult def_result = exprs::parse_expr(preds::Parser_advance(s));
  return preds::ParamResult{std::make_shared<ast::Param>(ast::Param{preds::TKind_ident(preds::Parser_kind(name_pos)), is_mutable, type_result.type_expr, true, def_result.expr}), def_result.parser};
 }() : preds::ParamResult{std::make_shared<ast::Param>(ast::Param{preds::TKind_ident(preds::Parser_kind(name_pos)), is_mutable, type_result.type_expr, false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown()))}), s};
}

decls::DeriveResult parse_derive_clause(preds::Parser parser) noexcept{
return preds::TKind_is_ident(preds::Parser_kind(parser)) && preds::TKind_ident(preds::Parser_kind(parser)) == mlc::String("derive") ? [&]() -> decls::DeriveResult { 
  preds::Parser state = preds::Parser_advance(parser);
  mlc::Array<mlc::String> traits = {};
  if (preds::TKind_is_lbrace(preds::Parser_kind(state))){
{
state = preds::Parser_advance(state);
while (preds::TKind_is_ident(preds::Parser_kind(state))){
{
traits.push_back(preds::TKind_ident(preds::Parser_kind(state)));
state = preds::Parser_advance(state);
if (preds::TKind_is_comma(preds::Parser_kind(state))){
{
state = preds::Parser_advance(state);
}
}
}
}
if (preds::TKind_is_rbrace(preds::Parser_kind(state))){
state = preds::Parser_advance(state);
}
}
}
  return decls::DeriveResult{traits, state};
 }() : decls::DeriveResult{{}, parser};
}

preds::DeclResult parse_type_decl(preds::Parser parser) noexcept{
mlc::String type_name = preds::TKind_ident(preds::Parser_kind(parser));
preds::Parser state = preds::Parser_advance(parser);
preds::TypeParamsResult type_params_res = parse_type_params_opt(state);
state = type_params_res.parser;
return preds::TKind_is_lbrace(preds::Parser_kind(state)) ? [&]() -> preds::DeclResult { 
  preds::TraitBodyResult trait_result = parse_trait_body(preds::Parser_advance(state), type_name, type_params_res.params);
  return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclTrait(type_name, type_params_res.params, trait_result.methods)), trait_result.parser};
 }() : preds::TKind_is_equal(preds::Parser_kind(state)) ? [&]() -> preds::DeclResult { 
  state = preds::Parser_advance(state);
  ast_tokens::TKind first_kind = preds::Parser_kind(state);
  return preds::TKind_is_lbrace(first_kind) ? [&]() -> preds::DeclResult { 
  preds::FieldDefsResult field_defs_result = parse_field_defs(preds::Parser_advance(state));
  mlc::Array<std::shared_ptr<ast::TypeVariant>> variants = {};
  variants.push_back(std::make_shared<ast::TypeVariant>(ast::VarRecord(type_name, field_defs_result.field_defs, false)));
  decls::DeriveResult derive_res = parse_derive_clause(field_defs_result.parser);
  return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclType(type_name, type_params_res.params, variants, derive_res.traits)), derive_res.parser};
 }() : [&]() -> preds::DeclResult { 
  preds::VariantsResult variants_result = parse_variants(state);
  decls::DeriveResult derive_res = parse_derive_clause(variants_result.parser);
  return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclType(type_name, type_params_res.params, variants_result.variants, derive_res.traits)), derive_res.parser};
 }();
 }() : [&]() -> preds::DeclResult { 
  preds::VariantsResult variants_result = parse_variants(state);
  decls::DeriveResult derive_res = parse_derive_clause(variants_result.parser);
  return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclType(type_name, type_params_res.params, variants_result.variants, derive_res.traits)), derive_res.parser};
 }();
}

bool is_decl_start(ast_tokens::TKind kind) noexcept{return preds::TKind_is_type(kind) || preds::TKind_is_fn(kind) || preds::TKind_is_extend(kind) || preds::TKind_is_import(kind) || preds::TKind_is_ident(kind) && preds::TKind_ident(kind) == mlc::String("export") || preds::TKind_ident(kind) == mlc::String("type") || preds::TKind_ident(kind) == mlc::String("fn") || preds::TKind_ident(kind) == mlc::String("extend") || preds::TKind_ident(kind) == mlc::String("import");}

bool parse_variants_continue(preds::Parser state) noexcept{return preds::Parser_at_eof(state) ? false : is_decl_start(preds::Parser_kind(state)) ? false : preds::TKind_is_ident(preds::Parser_kind(state)) && preds::TKind_ident(preds::Parser_kind(state)) == mlc::String("derive") ? false : preds::TKind_is_bar(preds::Parser_kind(state)) || preds::TKind_is_ident(preds::Parser_kind(state));}

preds::VariantsResult parse_variants(preds::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::TypeVariant>> variants = {};
preds::Parser state = preds::TKind_is_bar(preds::Parser_kind(parser)) ? preds::Parser_advance(parser) : parser;
while (parse_variants_continue(state)){
{
preds::VariantResult variant_result = parse_variant(state);
variants.push_back(variant_result.variant);
state = variant_result.parser;
if (preds::TKind_is_bar(preds::Parser_kind(state))){
{
state = preds::Parser_advance(state);
}
}
}
}
return preds::VariantsResult{variants, state};
}

preds::VariantResult parse_variant(preds::Parser parser) noexcept{
bool is_private = preds::TKind_is_ident(preds::Parser_kind(parser)) && preds::TKind_ident(preds::Parser_kind(parser)) == mlc::String("private");
preds::Parser after_private = is_private ? preds::Parser_advance(parser) : parser;
mlc::String variant_name = preds::TKind_ident(preds::Parser_kind(after_private));
preds::Parser after_name = preds::Parser_advance(after_private);
return preds::TKind_is_lparen(preds::Parser_kind(after_name)) ? [&]() -> preds::VariantResult { 
  preds::TypesResult types_result = parse_tuple_types(preds::Parser_advance(after_name));
  return preds::VariantResult{std::make_shared<ast::TypeVariant>(ast::VarTuple(variant_name, types_result.types, is_private)), types_result.parser};
 }() : preds::TKind_is_lbrace(preds::Parser_kind(after_name)) ? [&]() -> preds::VariantResult { 
  preds::FieldDefsResult field_defs_result = parse_field_defs(preds::Parser_advance(after_name));
  return preds::VariantResult{std::make_shared<ast::TypeVariant>(ast::VarRecord(variant_name, field_defs_result.field_defs, is_private)), field_defs_result.parser};
 }() : preds::VariantResult{std::make_shared<ast::TypeVariant>(ast::VarUnit(variant_name, is_private)), after_name};
}

preds::TypesResult parse_tuple_types(preds::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::TypeExpr>> types = {};
preds::TypeResult first = types::parse_type(parser);
types.push_back(first.type_expr);
preds::Parser state = first.parser;
while (preds::TKind_is_comma(preds::Parser_kind(state))){
{
preds::TypeResult next = types::parse_type(preds::Parser_advance(state));
types.push_back(next.type_expr);
state = next.parser;
}
}
return preds::TypesResult{types, preds::Parser_advance(state)};
}

preds::TraitBodyResult parse_trait_body(preds::Parser parser, mlc::String trait_name, mlc::Array<mlc::String> type_params) noexcept{
mlc::Array<std::shared_ptr<ast::Decl>> methods = {};
preds::Parser state = std::move(parser);
while (!preds::TKind_is_rbrace(preds::Parser_kind(state)) && !preds::Parser_at_eof(state)){
{
if (preds::TKind_is_fn(preds::Parser_kind(state))){
{
preds::Parser after_fn = preds::Parser_advance(state);
mlc::String fn_name = preds::TKind_ident(preds::Parser_kind(after_fn));
mlc::String mangled = trait_name + mlc::String("_") + fn_name;
preds::ParamsResult params_result = parse_params(preds::Parser_advance(after_fn));
preds::Parser after_rparen = preds::Parser_advance(params_result.parser);
preds::Parser type_parser = preds::TKind_is_arrow(preds::Parser_kind(after_rparen)) ? preds::Parser_advance(after_rparen) : after_rparen;
preds::TypeResult ret_result = types::parse_type(type_parser);
preds::Parser after_ret = ret_result.parser;
std::shared_ptr<ast::Expr> body = preds::TKind_is_equal(preds::Parser_kind(after_ret)) ? [&]() -> std::shared_ptr<ast::Expr> { 
  preds::ExprResult body_res = exprs::parse_expr(preds::Parser_advance(after_ret));
  state = body_res.parser;
  return body_res.expr;
 }() : [&]() -> std::shared_ptr<ast::Expr> { 
  ast::Span extern_method_span = preds::Parser_span_at_cursor(after_ret);
  state = after_ret;
  return std::make_shared<ast::Expr>(ast::ExprExtern(extern_method_span));
 }();
mlc::Array<mlc::Array<mlc::String>> trait_bounds = {};
int bi = 0;
while (bi < type_params.size()){
{
trait_bounds.push_back({});
bi = bi + 1;
}
}
methods.push_back(std::make_shared<ast::Decl>(ast::DeclFn(mangled, type_params, trait_bounds, params_result.params, ret_result.type_expr, body)));
}
} else {
{
if (preds::TKind_is_extern(preds::Parser_kind(state)) && preds::TKind_is_fn(preds::Parser_kind(preds::Parser_advance(state)))){
preds::Parser after_extern = preds::Parser_advance(state);
mlc::String fn_name = preds::TKind_ident(preds::Parser_kind(after_extern));
mlc::String mangled = trait_name + mlc::String("_") + fn_name;
preds::ParamsResult params_result = parse_params(preds::Parser_advance(after_extern));
preds::Parser after_rparen = preds::Parser_advance(params_result.parser);
preds::Parser type_parser = preds::TKind_is_arrow(preds::Parser_kind(after_rparen)) ? preds::Parser_advance(after_rparen) : after_rparen;
preds::TypeResult ret_result = types::parse_type(type_parser);
mlc::Array<mlc::Array<mlc::String>> trait_bounds = {};
int bi = 0;
while (bi < type_params.size()){
{
trait_bounds.push_back({});
bi = bi + 1;
}
}
methods.push_back(std::make_shared<ast::Decl>(ast::DeclFn(mangled, type_params, trait_bounds, params_result.params, ret_result.type_expr, std::make_shared<ast::Expr>(ast::ExprExtern(ast::span_unknown())))));
state = ret_result.parser;
} else {
if (preds::TKind_is_type(preds::Parser_kind(state))){
preds::Parser after_type = preds::Parser_advance(state);
ast::Span assoc_span = preds::Parser_span_at_cursor(after_type);
mlc::String assoc_name = preds::TKind_ident(preds::Parser_kind(after_type));
methods.push_back(std::make_shared<ast::Decl>(ast::DeclAssocType(assoc_name, assoc_span)));
state = preds::Parser_advance(after_type);
} else {
state = preds::Parser_advance(state);
}
}
}
}
}
}
return preds::TraitBodyResult{methods, preds::Parser_advance(state)};
}

preds::FieldDefsResult parse_field_defs(preds::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs = {};
preds::Parser state = std::move(parser);
while (!preds::TKind_is_rbrace(preds::Parser_kind(state)) && !preds::Parser_at_eof(state)){
{
mlc::String field_name = preds::TKind_ident(preds::Parser_kind(state));
preds::TypeResult type_result = types::parse_type(preds::Parser_advance_by(state, 2));
field_defs.push_back(std::make_shared<ast::FieldDef>(ast::FieldDef{field_name, type_result.type_expr}));
state = type_result.parser;
if (preds::TKind_is_comma(preds::Parser_kind(state))){
{
state = preds::Parser_advance(state);
}
}
}
}
return preds::FieldDefsResult{field_defs, preds::Parser_advance(state)};
}

ast::Program parse_program_with_source_path(mlc::Array<ast_tokens::Token> tokens, mlc::String source_path) noexcept{
mlc::Array<std::shared_ptr<ast::Decl>> decls = {};
preds::Parser state = preds::parser_new_with_source_path(tokens, source_path);
while (!preds::Parser_at_eof(state)){
{
preds::DeclResult result = parse_decl(state);
decls.push_back(result.decl);
state = result.parser;
}
}
return ast::Program{decls};
}

ast::Program parse_program(mlc::Array<ast_tokens::Token> tokens) noexcept{return parse_program_with_source_path(tokens, mlc::String(""));}

} // namespace decls
