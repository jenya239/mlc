#define main mlc_user_main
#include "decls.hpp"

namespace decls {

preds::BoundsResult parse_one_param_bounds(preds::Parser state) noexcept{
return (preds::Parser_kind(state).is_colon() ? [&]() {
auto s = state.advance();
auto bounds = mlc::Array<mlc::String>{};
if (preds::Parser_kind(s).is_ident()) {
bounds.push_back(preds::Parser_kind(s).ident());
s = s.advance();
[&]() {
while ((preds::Parser_kind(s).is_op() && (preds::Parser_kind(s).op_val() == mlc::String("+", 1)))) {
s = s.advance();
if (preds::Parser_kind(s).is_ident()) {
bounds.push_back(preds::Parser_kind(s).ident());
s = s.advance();
}
}
}();
}
return preds::BoundsResult{bounds, s};
}() : preds::BoundsResult{{}, state});
}
preds::TypeParamsResult parse_type_params_opt(preds::Parser parser) noexcept{
return ((preds::Parser_kind(parser).is_op() && (preds::Parser_kind(parser).op_val() == mlc::String("<", 1))) ? [&]() {
auto state = parser.advance();
auto params = mlc::Array<mlc::String>{};
auto bounds_list = mlc::Array<mlc::Array<mlc::String>>{};
if (preds::Parser_kind(state).is_ident()) {
params.push_back(preds::Parser_kind(state).ident());
auto res = parse_one_param_bounds(state.advance());
bounds_list.push_back(res.bounds);
state = res.parser;
[&]() {
while (preds::Parser_kind(state).is_comma()) {
state = state.advance();
if (preds::Parser_kind(state).is_ident()) {
params.push_back(preds::Parser_kind(state).ident());
auto res2 = parse_one_param_bounds(state.advance());
bounds_list.push_back(res2.bounds);
state = res2.parser;
}
}
}();
}
state = ((preds::Parser_kind(state).is_op() && (preds::Parser_kind(state).op_val() == mlc::String(">", 1))) ? state.advance() : state);
return preds::TypeParamsResult{params, bounds_list, state};
}() : preds::TypeParamsResult{{}, {}, parser});
}
preds::DeclResult parse_import_decl(preds::Parser parser) noexcept{
auto state = parser;
auto symbols = mlc::Array<mlc::String>{};
if ((preds::Parser_kind(state).is_op() && (preds::Parser_kind(state).op_val() == mlc::String("*", 1)))) {
symbols.push_back(mlc::String("*", 1));
state = state.advance();
} else if (preds::Parser_kind(state).is_lbrace()) {
state = state.advance();
while ((preds::Parser_kind(state).is_ident() || (preds::Parser_kind(state).is_op() && (preds::Parser_kind(state).op_val() == mlc::String("*", 1))))) {
if ((preds::Parser_kind(state).is_op() && (preds::Parser_kind(state).op_val() == mlc::String("*", 1)))) {
symbols.push_back(mlc::String("*", 1));
state = state.advance();
} else {
symbols.push_back(preds::Parser_kind(state).ident());
state = state.advance();
}
if (preds::Parser_kind(state).is_comma()) {
state = state.advance();
}
}
state = (preds::Parser_kind(state).is_rbrace() ? state.advance() : state);
}

state = (preds::Parser_kind(state).is_from() ? state.advance() : state);
auto path = (preds::Parser_kind(state).is_str() ? preds::Parser_kind(state).str_val() : mlc::String("", 0));
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclImport(path, symbols)), state.advance()};
}
preds::DeclResult parse_extend_decl(preds::Parser parser) noexcept{
auto type_name = preds::Parser_kind(parser).ident();
auto state = parser.advance();
auto trait_name = mlc::String("", 0);
if (preds::Parser_kind(state).is_colon()) {
state = state.advance();
if (preds::Parser_kind(state).is_ident()) {
trait_name = preds::Parser_kind(state).ident();
state = state.advance();
}
}
if ((preds::Parser_kind(state).is_op() && (preds::Parser_kind(state).op_val() == mlc::String("<", 1)))) {
state = state.advance();
while (((!state.at_eof()) && (!(preds::Parser_kind(state).is_op() && (preds::Parser_kind(state).op_val() == mlc::String(">", 1)))))) {
state = state.advance();
}
if ((preds::Parser_kind(state).is_op() && (preds::Parser_kind(state).op_val() == mlc::String(">", 1)))) {
state = state.advance();
}
}
auto methods_state = (preds::Parser_kind(state).is_lbrace() ? state.advance() : state);
auto methods = mlc::Array<std::shared_ptr<ast::Decl>>{};
while (((!preds::Parser_kind(methods_state).is_rbrace()) && (!methods_state.at_eof()))) {
if (preds::TKind_is_fn(preds::Parser_kind(methods_state))) {
auto method_result = parse_extend_method(methods_state, type_name);
methods.push_back(method_result.decl);
methods_state = method_result.parser;
} else if ((preds::Parser_kind(methods_state).is_extern() && preds::TKind_is_fn(preds::Parser_kind(methods_state.advance())))) {
auto method_result = parse_extend_extern_method(methods_state.advance(), type_name);
methods.push_back(method_result.decl);
methods_state = method_result.parser;
} else {
methods_state = methods_state.advance();
}

}
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclExtend(type_name, trait_name, methods)), methods_state.advance()};
}
preds::DeclResult parse_extend_extern_method(preds::Parser parser, mlc::String type_name) noexcept{
auto fn_name = preds::Parser_kind(parser.advance()).ident();
auto mangled_name = ((type_name + mlc::String("_", 1)) + fn_name);
auto after_lparen = parser.advance_by(3);
auto after_self = ((preds::Parser_kind(after_lparen).is_ident() && (preds::Parser_kind(after_lparen).ident() == mlc::String("self", 4))) ? after_lparen : after_lparen.advance());
auto self_param = std::make_shared<ast::Param>(ast::Param{mlc::String("self", 4), false, std::make_shared<ast::TypeExpr>(ast::TyNamed(type_name))});
auto params = mlc::Array<std::shared_ptr<ast::Param>>{};
params.push_back(self_param);
auto params_state = (preds::Parser_kind(after_self).is_comma() ? after_self.advance() : after_self);
auto rest_params = (preds::Parser_kind(params_state).is_rparen() ? preds::ParamsResult{{}, params_state.advance()} : parse_params(params_state));
auto index = 0;
while ((index < rest_params.params.length())) {
auto rp = rest_params.params[index];
if ((ast::param_name(rp) != mlc::String("self", 4))) {
params.push_back(rp);
}
index = (index + 1);
}
auto after_rparen = rest_params.parser.advance();
auto type_parser = (preds::Parser_kind(after_rparen).is_arrow() ? after_rparen.advance() : after_rparen);
auto ret_type_result = types::parse_type(type_parser);
auto p = ret_type_result.parser;
while (((!preds::Parser_kind(p).is_rbrace()) && (!p.at_eof()))) {
p = p.advance();
}
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclFn(mangled_name, {}, {}, params, ret_type_result.type_expr, std::make_shared<ast::Expr>(ast::ExprExtern{}))), p};
}
preds::DeclResult parse_extend_method(preds::Parser parser, mlc::String type_name) noexcept{
auto fn_name = preds::Parser_kind(parser.advance()).ident();
auto mangled_name = ((type_name + mlc::String("_", 1)) + fn_name);
auto after_lparen = parser.advance_by(3);
auto after_self = ((preds::Parser_kind(after_lparen).is_ident() && (preds::Parser_kind(after_lparen).ident() == mlc::String("self", 4))) ? after_lparen : after_lparen.advance());
auto self_param = std::make_shared<ast::Param>(ast::Param{mlc::String("self", 4), false, std::make_shared<ast::TypeExpr>(ast::TyNamed(type_name))});
auto params = mlc::Array<std::shared_ptr<ast::Param>>{};
params.push_back(self_param);
auto params_state = (preds::Parser_kind(after_self).is_comma() ? after_self.advance() : after_self);
auto rest_params = (preds::Parser_kind(params_state).is_rparen() ? preds::ParamsResult{{}, params_state.advance()} : parse_params(params_state));
auto index = 0;
while ((index < rest_params.params.length())) {
auto rp = rest_params.params[index];
if ((ast::param_name(rp) != mlc::String("self", 4))) {
params.push_back(rp);
}
index = (index + 1);
}
auto after_rparen = rest_params.parser.advance();
auto type_parser = (preds::Parser_kind(after_rparen).is_arrow() ? after_rparen.advance() : after_rparen);
auto ret_type_result = types::parse_type(type_parser);
auto after_eq = ret_type_result.parser.advance();
auto body_result = (preds::Parser_kind(after_eq).is_extern() ? preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprExtern{}), after_eq.advance()} : [&]() {
auto parsed = exprs::parse_expr(after_eq);
return std::visit(overloaded{[&](const ast::ExprIdent& exprIdent) { auto [name] = exprIdent; return ((name == mlc::String("extern", 6)) ? preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprExtern{}), parsed.parser} : parsed); },
[&](const auto& __v) { return parsed; }
}, (*parsed.expr));
}());
auto p = body_result.parser;
while (((!preds::Parser_kind(p).is_rbrace()) && (!p.at_eof()))) {
p = p.advance();
}
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclFn(mangled_name, {}, {}, params, ret_type_result.type_expr, body_result.expr)), p};
}
preds::DeclResult parse_decl(preds::Parser parser) noexcept{
auto kind = preds::Parser_kind(parser);
return ((kind.is_ident() && (kind.ident() == mlc::String("export", 6))) ? [&]() {
auto inner = parse_decl(parser.advance());
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclExported(inner.decl)), inner.parser};
}() : ((preds::TKind_is_fn(kind) || (kind.is_extern() && preds::TKind_is_fn(preds::Parser_kind(parser.advance())))) ? parse_fn_decl(parser) : (kind.is_type() ? parse_type_decl(parser.advance()) : (kind.is_extend() ? parse_extend_decl(parser.advance()) : (kind.is_import() ? parse_import_decl(parser.advance()) : preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclFn(mlc::String("__skip__", 8), {}, {}, {}, std::make_shared<ast::TypeExpr>(ast::TyUnit{}), std::make_shared<ast::Expr>(ast::ExprUnit{}))), parser.advance()})))));
}
preds::DeclResult parse_fn_decl(preds::Parser parser) noexcept{
auto is_extern = preds::Parser_kind(parser).is_extern();
auto fn_start = (is_extern ? parser.advance() : parser);
auto after_name = fn_start.advance();
auto fn_name = preds::Parser_kind(after_name).ident();
auto type_params_res = parse_type_params_opt(after_name.advance());
auto params_result = parse_params(type_params_res.parser.advance());
auto after_rparen = params_result.parser.advance();
auto type_parser = (preds::Parser_kind(after_rparen).is_arrow() ? after_rparen.advance() : after_rparen);
auto ret_type_result = types::parse_type(type_parser);
auto type_bounds = type_params_res.bounds;
return (is_extern ? preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclFn(fn_name, type_params_res.params, type_bounds, params_result.params, ret_type_result.type_expr, std::make_shared<ast::Expr>(ast::ExprExtern{}))), ret_type_result.parser} : [&]() {
auto body_result = exprs::parse_expr(ret_type_result.parser.advance());
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclFn(fn_name, type_params_res.params, type_bounds, params_result.params, ret_type_result.type_expr, body_result.expr)), body_result.parser};
}());
}
preds::ParamsResult parse_params(preds::Parser parser) noexcept{
auto params = mlc::Array<std::shared_ptr<ast::Param>>{};
return (preds::Parser_kind(parser).is_rparen() ? preds::ParamsResult{params, parser.advance()} : [&]() {
auto first = parse_param(parser);
params.push_back(first.param);
auto state = first.parser;
while (preds::Parser_kind(state).is_comma()) {
auto next = parse_param(state.advance());
params.push_back(next.param);
state = next.parser;
}
return preds::ParamsResult{params, state.advance()};
}());
}
preds::ParamResult parse_param(preds::Parser parser) noexcept{
auto is_mutable = preds::Parser_kind(parser).is_mut();
auto name_pos = (is_mutable ? parser.advance() : parser);
auto type_result = types::parse_type(name_pos.advance_by(2));
return preds::ParamResult{std::make_shared<ast::Param>(ast::Param{preds::Parser_kind(name_pos).ident(), is_mutable, type_result.type_expr}), type_result.parser};
}
preds::DeclResult parse_type_decl(preds::Parser parser) noexcept{
auto type_name = preds::Parser_kind(parser).ident();
auto state = parser.advance();
auto type_params_res = parse_type_params_opt(state);
state = type_params_res.parser;
return (preds::Parser_kind(state).is_lbrace() ? [&]() {
auto trait_result = parse_trait_body(state.advance(), type_name, type_params_res.params);
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclTrait(type_name, type_params_res.params, trait_result.methods)), trait_result.parser};
}() : (preds::Parser_kind(state).is_equal() ? [&]() {
state = state.advance();
auto first_kind = preds::Parser_kind(state);
return (first_kind.is_lbrace() ? [&]() {
auto field_defs_result = parse_field_defs(state.advance());
auto variants = mlc::Array<std::shared_ptr<ast::TypeVariant>>{};
variants.push_back(std::make_shared<ast::TypeVariant>(ast::VarRecord(type_name, field_defs_result.field_defs)));
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclType(type_name, type_params_res.params, variants)), field_defs_result.parser};
}() : [&]() {
auto variants_result = parse_variants(state);
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclType(type_name, type_params_res.params, variants_result.variants)), variants_result.parser};
}());
}() : [&]() {
auto variants_result = parse_variants(state);
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclType(type_name, type_params_res.params, variants_result.variants)), variants_result.parser};
}()));
}
bool is_decl_start(ast_tokens::TKind kind) noexcept{
return ((((kind.is_type() || preds::TKind_is_fn(kind)) || kind.is_extend()) || kind.is_import()) || (kind.is_ident() && (((((kind.ident() == mlc::String("export", 6)) || (kind.ident() == mlc::String("type", 4))) || (kind.ident() == mlc::String("fn", 2))) || (kind.ident() == mlc::String("extend", 6))) || (kind.ident() == mlc::String("import", 6)))));
}
bool parse_variants_continue(preds::Parser state) noexcept{
if (state.at_eof()) {
return false;
} else if (is_decl_start(preds::Parser_kind(state))) {
return false;
} else {
return (preds::Parser_kind(state).is_bar() || preds::Parser_kind(state).is_ident());
}

}
preds::VariantsResult parse_variants(preds::Parser parser) noexcept{
auto variants = mlc::Array<std::shared_ptr<ast::TypeVariant>>{};
auto state = (preds::Parser_kind(parser).is_bar() ? parser.advance() : parser);
while (parse_variants_continue(state)) {
auto variant_result = parse_variant(state);
variants.push_back(variant_result.variant);
state = variant_result.parser;
if (preds::Parser_kind(state).is_bar()) {
state = state.advance();
}
}
return preds::VariantsResult{variants, state};
}
preds::VariantResult parse_variant(preds::Parser parser) noexcept{
auto variant_name = preds::Parser_kind(parser).ident();
auto after_name = parser.advance();
return (preds::Parser_kind(after_name).is_lparen() ? [&]() {
auto types_result = parse_tuple_types(after_name.advance());
return preds::VariantResult{std::make_shared<ast::TypeVariant>(ast::VarTuple(variant_name, types_result.types)), types_result.parser};
}() : (preds::Parser_kind(after_name).is_lbrace() ? [&]() {
auto field_defs_result = parse_field_defs(after_name.advance());
return preds::VariantResult{std::make_shared<ast::TypeVariant>(ast::VarRecord(variant_name, field_defs_result.field_defs)), field_defs_result.parser};
}() : preds::VariantResult{std::make_shared<ast::TypeVariant>(ast::VarUnit(variant_name)), after_name}));
}
preds::TypesResult parse_tuple_types(preds::Parser parser) noexcept{
auto types = mlc::Array<std::shared_ptr<ast::TypeExpr>>{};
auto first = types::parse_type(parser);
types.push_back(first.type_expr);
auto state = first.parser;
while (preds::Parser_kind(state).is_comma()) {
auto next = types::parse_type(state.advance());
types.push_back(next.type_expr);
state = next.parser;
}
return preds::TypesResult{types, state.advance()};
}
preds::TraitBodyResult parse_trait_body(preds::Parser parser, mlc::String trait_name, mlc::Array<mlc::String> type_params) noexcept{
auto methods = mlc::Array<std::shared_ptr<ast::Decl>>{};
auto state = parser;
while (((!preds::Parser_kind(state).is_rbrace()) && (!state.at_eof()))) {
if (preds::TKind_is_fn(preds::Parser_kind(state))) {
auto after_fn = state.advance();
auto fn_name = preds::Parser_kind(after_fn).ident();
auto mangled = ((trait_name + mlc::String("_", 1)) + fn_name);
auto params_result = parse_params(after_fn.advance());
auto after_rparen = params_result.parser.advance();
auto type_parser = (preds::Parser_kind(after_rparen).is_arrow() ? after_rparen.advance() : after_rparen);
auto ret_result = types::parse_type(type_parser);
auto after_ret = ret_result.parser;
auto body = (preds::Parser_kind(after_ret).is_equal() ? [&]() {
auto body_res = exprs::parse_expr(after_ret.advance());
state = body_res.parser;
return body_res.expr;
}() : [&]() {
state = after_ret;
return std::make_shared<ast::Expr>(ast::ExprExtern{});
}());
auto trait_bounds = mlc::Array<mlc::Array<mlc::String>>{};
/* unit */;
auto bi = 0;
/* unit */;
while ((bi < type_params.length())) {
trait_bounds.push_back({});
bi = (bi + 1);
}
methods.push_back(std::make_shared<ast::Decl>(ast::DeclFn(mangled, type_params, trait_bounds, params_result.params, ret_result.type_expr, body)));
} else if ((preds::Parser_kind(state).is_extern() && preds::TKind_is_fn(preds::Parser_kind(state.advance())))) {
auto after_extern = state.advance();
auto fn_name = preds::Parser_kind(after_extern).ident();
auto mangled = ((trait_name + mlc::String("_", 1)) + fn_name);
auto params_result = parse_params(after_extern.advance());
auto after_rparen = params_result.parser.advance();
auto type_parser = (preds::Parser_kind(after_rparen).is_arrow() ? after_rparen.advance() : after_rparen);
auto ret_result = types::parse_type(type_parser);
auto trait_bounds = mlc::Array<mlc::Array<mlc::String>>{};
/* unit */;
auto bi = 0;
/* unit */;
while ((bi < type_params.length())) {
trait_bounds.push_back({});
bi = (bi + 1);
}
methods.push_back(std::make_shared<ast::Decl>(ast::DeclFn(mangled, type_params, trait_bounds, params_result.params, ret_result.type_expr, std::make_shared<ast::Expr>(ast::ExprExtern{}))));
state = ret_result.parser;
} else {
state = state.advance();
}

}
return preds::TraitBodyResult{methods, state.advance()};
}
preds::FieldDefsResult parse_field_defs(preds::Parser parser) noexcept{
auto field_defs = mlc::Array<std::shared_ptr<ast::FieldDef>>{};
auto state = parser;
while (((!preds::Parser_kind(state).is_rbrace()) && (!state.at_eof()))) {
auto field_name = preds::Parser_kind(state).ident();
auto type_result = types::parse_type(state.advance_by(2));
field_defs.push_back(std::make_shared<ast::FieldDef>(ast::FieldDef{field_name, type_result.type_expr}));
state = type_result.parser;
if (preds::Parser_kind(state).is_comma()) {
state = state.advance();
}
}
return preds::FieldDefsResult{field_defs, state.advance()};
}
ast::Program parse_program(mlc::Array<ast_tokens::Token> tokens) noexcept{
auto decls = mlc::Array<std::shared_ptr<ast::Decl>>{};
auto state = preds::parser_new(tokens);
while ((!state.at_eof())) {
auto result = parse_decl(state);
decls.push_back(result.decl);
state = result.parser;
}
return ast::Program{decls};
}

} // namespace decls
