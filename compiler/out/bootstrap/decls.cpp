#define main mlc_user_main
#include "decls.hpp"

namespace decls {

preds::BoundsResult parse_one_param_bounds(preds::Parser state) noexcept{
return (preds::TKind_is_colon(preds::Parser_kind(state)) ? [&]() {
auto s = preds::Parser_advance(state);
auto bounds = mlc::Array<mlc::String>{};
if (preds::TKind_is_ident(preds::Parser_kind(s))) {
bounds.push_back(preds::TKind_ident(preds::Parser_kind(s)));
s = preds::Parser_advance(s);
[&]() {
while ((preds::TKind_is_op(preds::Parser_kind(s)) && (preds::TKind_op_val(preds::Parser_kind(s)) == mlc::String("+", 1)))) {
s = preds::Parser_advance(s);
if (preds::TKind_is_ident(preds::Parser_kind(s))) {
bounds.push_back(preds::TKind_ident(preds::Parser_kind(s)));
s = preds::Parser_advance(s);
}
}
}();
}
return preds::BoundsResult{bounds, s};
}() : preds::BoundsResult{{}, state});
}
preds::TypeParamsResult parse_type_params_opt(preds::Parser parser) noexcept{
return ((preds::TKind_is_op(preds::Parser_kind(parser)) && (preds::TKind_op_val(preds::Parser_kind(parser)) == mlc::String("<", 1))) ? [&]() {
auto state = preds::Parser_advance(parser);
auto params = mlc::Array<mlc::String>{};
auto bounds_list = mlc::Array<mlc::Array<mlc::String>>{};
if (preds::TKind_is_ident(preds::Parser_kind(state))) {
params.push_back(preds::TKind_ident(preds::Parser_kind(state)));
auto res = parse_one_param_bounds(preds::Parser_advance(state));
bounds_list.push_back(res.bounds);
state = res.parser;
[&]() {
while (preds::TKind_is_comma(preds::Parser_kind(state))) {
state = preds::Parser_advance(state);
if (preds::TKind_is_ident(preds::Parser_kind(state))) {
params.push_back(preds::TKind_ident(preds::Parser_kind(state)));
auto res2 = parse_one_param_bounds(preds::Parser_advance(state));
bounds_list.push_back(res2.bounds);
state = res2.parser;
}
}
}();
}
state = ((preds::TKind_is_op(preds::Parser_kind(state)) && (preds::TKind_op_val(preds::Parser_kind(state)) == mlc::String(">", 1))) ? preds::Parser_advance(state) : state);
return preds::TypeParamsResult{params, bounds_list, state};
}() : preds::TypeParamsResult{{}, {}, parser});
}
preds::DeclResult parse_import_decl(preds::Parser parser) noexcept{
auto state = parser;
auto symbols = mlc::Array<mlc::String>{};
if ((preds::TKind_is_op(preds::Parser_kind(state)) && (preds::TKind_op_val(preds::Parser_kind(state)) == mlc::String("*", 1)))) {
symbols.push_back(mlc::String("*", 1));
state = preds::Parser_advance(state);
} else if (preds::TKind_is_lbrace(preds::Parser_kind(state))) {
state = preds::Parser_advance(state);
while ((preds::TKind_is_ident(preds::Parser_kind(state)) || (preds::TKind_is_op(preds::Parser_kind(state)) && (preds::TKind_op_val(preds::Parser_kind(state)) == mlc::String("*", 1))))) {
if ((preds::TKind_is_op(preds::Parser_kind(state)) && (preds::TKind_op_val(preds::Parser_kind(state)) == mlc::String("*", 1)))) {
symbols.push_back(mlc::String("*", 1));
state = preds::Parser_advance(state);
} else {
symbols.push_back(preds::TKind_ident(preds::Parser_kind(state)));
state = preds::Parser_advance(state);
}
if (preds::TKind_is_comma(preds::Parser_kind(state))) {
state = preds::Parser_advance(state);
}
}
state = (preds::TKind_is_rbrace(preds::Parser_kind(state)) ? preds::Parser_advance(state) : state);
}

state = (preds::TKind_is_from(preds::Parser_kind(state)) ? preds::Parser_advance(state) : state);
auto path = (preds::TKind_is_str(preds::Parser_kind(state)) ? preds::TKind_str_val(preds::Parser_kind(state)) : mlc::String("", 0));
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclImport(path, symbols)), preds::Parser_advance(state)};
}
preds::DeclResult parse_extend_decl(preds::Parser parser) noexcept{
auto type_name = preds::TKind_ident(preds::Parser_kind(parser));
auto state = preds::Parser_advance(parser);
auto trait_name = mlc::String("", 0);
if (preds::TKind_is_colon(preds::Parser_kind(state))) {
state = preds::Parser_advance(state);
if (preds::TKind_is_ident(preds::Parser_kind(state))) {
trait_name = preds::TKind_ident(preds::Parser_kind(state));
state = preds::Parser_advance(state);
}
}
if ((preds::TKind_is_op(preds::Parser_kind(state)) && (preds::TKind_op_val(preds::Parser_kind(state)) == mlc::String("<", 1)))) {
state = preds::Parser_advance(state);
while (((!preds::Parser_at_eof(state)) && (!(preds::TKind_is_op(preds::Parser_kind(state)) && (preds::TKind_op_val(preds::Parser_kind(state)) == mlc::String(">", 1)))))) {
state = preds::Parser_advance(state);
}
if ((preds::TKind_is_op(preds::Parser_kind(state)) && (preds::TKind_op_val(preds::Parser_kind(state)) == mlc::String(">", 1)))) {
state = preds::Parser_advance(state);
}
}
auto methods_state = (preds::TKind_is_lbrace(preds::Parser_kind(state)) ? preds::Parser_advance(state) : state);
auto methods = mlc::Array<std::shared_ptr<ast::Decl>>{};
while (((!preds::TKind_is_rbrace(preds::Parser_kind(methods_state))) && (!preds::Parser_at_eof(methods_state)))) {
if (preds::TKind_is_fn(preds::Parser_kind(methods_state))) {
auto method_result = parse_extend_method(methods_state, type_name);
methods.push_back(method_result.decl);
methods_state = method_result.parser;
} else if ((preds::TKind_is_extern(preds::Parser_kind(methods_state)) && preds::TKind_is_fn(preds::Parser_kind(preds::Parser_advance(methods_state))))) {
auto method_result = parse_extend_extern_method(preds::Parser_advance(methods_state), type_name);
methods.push_back(method_result.decl);
methods_state = method_result.parser;
} else {
methods_state = preds::Parser_advance(methods_state);
}

}
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclExtend(type_name, trait_name, methods)), preds::Parser_advance(methods_state)};
}
preds::ParamsResult parse_extend_extern_no_self_params(preds::Parser after_lparen) noexcept{
if (preds::TKind_is_rparen(preds::Parser_kind(after_lparen))) {
return preds::ParamsResult{{}, preds::Parser_advance(after_lparen)};
} else {
return parse_params(after_lparen);
}
}
preds::ParamsResult parse_extend_extern_rest_params(preds::Parser after_lparen, bool leading_self) noexcept{
if (leading_self) {
auto after_self = after_lparen;
auto after_self_skip = (preds::TKind_is_rparen(preds::Parser_kind(after_self)) ? after_self : (preds::TKind_is_comma(preds::Parser_kind(after_self)) ? preds::Parser_advance(after_self) : preds::Parser_advance(after_self)));
return (preds::TKind_is_rparen(preds::Parser_kind(after_self_skip)) ? preds::ParamsResult{{}, preds::Parser_advance(after_self_skip)} : parse_params((preds::TKind_is_comma(preds::Parser_kind(after_self_skip)) ? preds::Parser_advance(after_self_skip) : after_self_skip)));
} else {
return parse_extend_extern_no_self_params(after_lparen);
}
}
preds::DeclResult parse_extend_extern_method(preds::Parser parser, mlc::String type_name) noexcept{
auto fn_name = preds::TKind_ident(preds::Parser_kind(preds::Parser_advance(parser)));
auto mangled_name = ((type_name + mlc::String("_", 1)) + fn_name);
auto after_lparen = preds::Parser_advance_by(parser, 3);
auto self_param = std::make_shared<ast::Param>(ast::Param{mlc::String("self", 4), false, std::make_shared<ast::TypeExpr>(ast::TyNamed(type_name))});
auto params = mlc::Array<std::shared_ptr<ast::Param>>{};
auto leading_self = (preds::TKind_is_ident(preds::Parser_kind(after_lparen)) && (preds::TKind_ident(preds::Parser_kind(after_lparen)) == mlc::String("self", 4)));
if (leading_self) {
params.push_back(self_param);
}
auto rest_params = parse_extend_extern_rest_params(after_lparen, leading_self);
auto index = 0;
while ((index < rest_params.params.length())) {
auto rp = rest_params.params[index];
if ((ast::param_name(rp) != mlc::String("self", 4))) {
params.push_back(rp);
}
index = (index + 1);
}
auto after_rparen = preds::Parser_advance(rest_params.parser);
auto type_parser = (preds::TKind_is_arrow(preds::Parser_kind(after_rparen)) ? preds::Parser_advance(after_rparen) : after_rparen);
auto ret_type_result = types::parse_type(type_parser);
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclFn(mangled_name, {}, {}, params, ret_type_result.type_expr, std::make_shared<ast::Expr>(ast::ExprExtern{}))), ret_type_result.parser};
}
preds::DeclResult parse_extend_method(preds::Parser parser, mlc::String type_name) noexcept{
auto fn_name = preds::TKind_ident(preds::Parser_kind(preds::Parser_advance(parser)));
auto mangled_name = ((type_name + mlc::String("_", 1)) + fn_name);
auto after_lparen = preds::Parser_advance_by(parser, 3);
auto after_self = ((preds::TKind_is_ident(preds::Parser_kind(after_lparen)) && (preds::TKind_ident(preds::Parser_kind(after_lparen)) == mlc::String("self", 4))) ? after_lparen : preds::Parser_advance(after_lparen));
auto self_param = std::make_shared<ast::Param>(ast::Param{mlc::String("self", 4), false, std::make_shared<ast::TypeExpr>(ast::TyNamed(type_name))});
auto params = mlc::Array<std::shared_ptr<ast::Param>>{};
params.push_back(self_param);
auto after_self_skip = (preds::TKind_is_rparen(preds::Parser_kind(after_self)) ? after_self : (preds::TKind_is_comma(preds::Parser_kind(after_self)) ? preds::Parser_advance(after_self) : preds::Parser_advance(after_self)));
auto rest_params = (preds::TKind_is_rparen(preds::Parser_kind(after_self_skip)) ? preds::ParamsResult{{}, preds::Parser_advance(after_self_skip)} : parse_params((preds::TKind_is_comma(preds::Parser_kind(after_self_skip)) ? preds::Parser_advance(after_self_skip) : after_self_skip)));
auto index = 0;
while ((index < rest_params.params.length())) {
auto rp = rest_params.params[index];
if ((ast::param_name(rp) != mlc::String("self", 4))) {
params.push_back(rp);
}
index = (index + 1);
}
auto after_rparen = preds::Parser_advance(rest_params.parser);
auto type_parser = (preds::TKind_is_arrow(preds::Parser_kind(after_rparen)) ? preds::Parser_advance(after_rparen) : after_rparen);
auto ret_type_result = types::parse_type(type_parser);
auto after_eq = preds::Parser_advance(ret_type_result.parser);
auto body_result = (preds::TKind_is_extern(preds::Parser_kind(after_eq)) ? preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprExtern{}), preds::Parser_advance(after_eq)} : [&]() {
auto parsed = exprs::parse_expr(after_eq);
return std::visit(overloaded{[&](const ast::ExprIdent& exprIdent) { auto [name] = exprIdent; return ((name == mlc::String("extern", 6)) ? preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprExtern{}), parsed.parser} : parsed); },
[&](const auto& __v) { return parsed; }
}, (*parsed.expr));
}());
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclFn(mangled_name, {}, {}, params, ret_type_result.type_expr, body_result.expr)), body_result.parser};
}
preds::DeclResult parse_decl(preds::Parser parser) noexcept{
auto kind = preds::Parser_kind(parser);
return ((preds::TKind_is_ident(kind) && (preds::TKind_ident(kind) == mlc::String("export", 6))) ? [&]() {
auto inner = parse_decl(preds::Parser_advance(parser));
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclExported(inner.decl)), inner.parser};
}() : ((preds::TKind_is_fn(kind) || (preds::TKind_is_extern(kind) && preds::TKind_is_fn(preds::Parser_kind(preds::Parser_advance(parser))))) ? parse_fn_decl(parser) : (preds::TKind_is_type(kind) ? parse_type_decl(preds::Parser_advance(parser)) : (preds::TKind_is_extend(kind) ? parse_extend_decl(preds::Parser_advance(parser)) : (preds::TKind_is_import(kind) ? parse_import_decl(preds::Parser_advance(parser)) : preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclFn(mlc::String("__skip__", 8), {}, {}, {}, std::make_shared<ast::TypeExpr>(ast::TyUnit{}), std::make_shared<ast::Expr>(ast::ExprUnit{}))), preds::Parser_advance(parser)})))));
}
preds::DeclResult parse_fn_decl(preds::Parser parser) noexcept{
auto is_extern = preds::TKind_is_extern(preds::Parser_kind(parser));
auto fn_start = (is_extern ? preds::Parser_advance(parser) : parser);
auto after_name = preds::Parser_advance(fn_start);
auto fn_name = preds::TKind_ident(preds::Parser_kind(after_name));
auto type_params_res = parse_type_params_opt(preds::Parser_advance(after_name));
auto params_result = parse_params(preds::Parser_advance(type_params_res.parser));
auto after_rparen = preds::Parser_advance(params_result.parser);
auto type_parser = (preds::TKind_is_arrow(preds::Parser_kind(after_rparen)) ? preds::Parser_advance(after_rparen) : after_rparen);
auto ret_type_result = types::parse_type(type_parser);
auto type_bounds = type_params_res.bounds;
return (is_extern ? preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclFn(fn_name, type_params_res.params, type_bounds, params_result.params, ret_type_result.type_expr, std::make_shared<ast::Expr>(ast::ExprExtern{}))), ret_type_result.parser} : [&]() {
auto body_result = exprs::parse_expr(preds::Parser_advance(ret_type_result.parser));
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclFn(fn_name, type_params_res.params, type_bounds, params_result.params, ret_type_result.type_expr, body_result.expr)), body_result.parser};
}());
}
preds::ParamsResult parse_params(preds::Parser parser) noexcept{
auto params = mlc::Array<std::shared_ptr<ast::Param>>{};
return (preds::TKind_is_rparen(preds::Parser_kind(parser)) ? preds::ParamsResult{params, preds::Parser_advance(parser)} : [&]() {
auto first = parse_param(parser);
params.push_back(first.param);
auto state = first.parser;
while (preds::TKind_is_comma(preds::Parser_kind(state))) {
auto next = parse_param(preds::Parser_advance(state));
params.push_back(next.param);
state = next.parser;
}
return preds::ParamsResult{params, preds::Parser_advance(state)};
}());
}
preds::ParamResult parse_param(preds::Parser parser) noexcept{
auto is_mutable = preds::TKind_is_mut(preds::Parser_kind(parser));
auto name_pos = (is_mutable ? preds::Parser_advance(parser) : parser);
auto type_result = types::parse_type(preds::Parser_advance_by(name_pos, 2));
return preds::ParamResult{std::make_shared<ast::Param>(ast::Param{preds::TKind_ident(preds::Parser_kind(name_pos)), is_mutable, type_result.type_expr}), type_result.parser};
}
preds::DeclResult parse_type_decl(preds::Parser parser) noexcept{
auto type_name = preds::TKind_ident(preds::Parser_kind(parser));
auto state = preds::Parser_advance(parser);
auto type_params_res = parse_type_params_opt(state);
state = type_params_res.parser;
return (preds::TKind_is_lbrace(preds::Parser_kind(state)) ? [&]() {
auto trait_result = parse_trait_body(preds::Parser_advance(state), type_name, type_params_res.params);
return preds::DeclResult{std::make_shared<ast::Decl>(ast::DeclTrait(type_name, type_params_res.params, trait_result.methods)), trait_result.parser};
}() : (preds::TKind_is_equal(preds::Parser_kind(state)) ? [&]() {
state = preds::Parser_advance(state);
auto first_kind = preds::Parser_kind(state);
return (preds::TKind_is_lbrace(first_kind) ? [&]() {
auto field_defs_result = parse_field_defs(preds::Parser_advance(state));
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
return ((((preds::TKind_is_type(kind) || preds::TKind_is_fn(kind)) || preds::TKind_is_extend(kind)) || preds::TKind_is_import(kind)) || (preds::TKind_is_ident(kind) && (((((preds::TKind_ident(kind) == mlc::String("export", 6)) || (preds::TKind_ident(kind) == mlc::String("type", 4))) || (preds::TKind_ident(kind) == mlc::String("fn", 2))) || (preds::TKind_ident(kind) == mlc::String("extend", 6))) || (preds::TKind_ident(kind) == mlc::String("import", 6)))));
}
bool parse_variants_continue(preds::Parser state) noexcept{
if (preds::Parser_at_eof(state)) {
return false;
} else if (is_decl_start(preds::Parser_kind(state))) {
return false;
} else {
return (preds::TKind_is_bar(preds::Parser_kind(state)) || preds::TKind_is_ident(preds::Parser_kind(state)));
}

}
preds::VariantsResult parse_variants(preds::Parser parser) noexcept{
auto variants = mlc::Array<std::shared_ptr<ast::TypeVariant>>{};
auto state = (preds::TKind_is_bar(preds::Parser_kind(parser)) ? preds::Parser_advance(parser) : parser);
while (parse_variants_continue(state)) {
auto variant_result = parse_variant(state);
variants.push_back(variant_result.variant);
state = variant_result.parser;
if (preds::TKind_is_bar(preds::Parser_kind(state))) {
state = preds::Parser_advance(state);
}
}
return preds::VariantsResult{variants, state};
}
preds::VariantResult parse_variant(preds::Parser parser) noexcept{
auto variant_name = preds::TKind_ident(preds::Parser_kind(parser));
auto after_name = preds::Parser_advance(parser);
return (preds::TKind_is_lparen(preds::Parser_kind(after_name)) ? [&]() {
auto types_result = parse_tuple_types(preds::Parser_advance(after_name));
return preds::VariantResult{std::make_shared<ast::TypeVariant>(ast::VarTuple(variant_name, types_result.types)), types_result.parser};
}() : (preds::TKind_is_lbrace(preds::Parser_kind(after_name)) ? [&]() {
auto field_defs_result = parse_field_defs(preds::Parser_advance(after_name));
return preds::VariantResult{std::make_shared<ast::TypeVariant>(ast::VarRecord(variant_name, field_defs_result.field_defs)), field_defs_result.parser};
}() : preds::VariantResult{std::make_shared<ast::TypeVariant>(ast::VarUnit(variant_name)), after_name}));
}
preds::TypesResult parse_tuple_types(preds::Parser parser) noexcept{
auto types = mlc::Array<std::shared_ptr<ast::TypeExpr>>{};
auto first = types::parse_type(parser);
types.push_back(first.type_expr);
auto state = first.parser;
while (preds::TKind_is_comma(preds::Parser_kind(state))) {
auto next = types::parse_type(preds::Parser_advance(state));
types.push_back(next.type_expr);
state = next.parser;
}
return preds::TypesResult{types, preds::Parser_advance(state)};
}
preds::TraitBodyResult parse_trait_body(preds::Parser parser, mlc::String trait_name, mlc::Array<mlc::String> type_params) noexcept{
auto methods = mlc::Array<std::shared_ptr<ast::Decl>>{};
auto state = parser;
while (((!preds::TKind_is_rbrace(preds::Parser_kind(state))) && (!preds::Parser_at_eof(state)))) {
if (preds::TKind_is_fn(preds::Parser_kind(state))) {
auto after_fn = preds::Parser_advance(state);
auto fn_name = preds::TKind_ident(preds::Parser_kind(after_fn));
auto mangled = ((trait_name + mlc::String("_", 1)) + fn_name);
auto params_result = parse_params(preds::Parser_advance(after_fn));
auto after_rparen = preds::Parser_advance(params_result.parser);
auto type_parser = (preds::TKind_is_arrow(preds::Parser_kind(after_rparen)) ? preds::Parser_advance(after_rparen) : after_rparen);
auto ret_result = types::parse_type(type_parser);
auto after_ret = ret_result.parser;
auto body = (preds::TKind_is_equal(preds::Parser_kind(after_ret)) ? [&]() {
auto body_res = exprs::parse_expr(preds::Parser_advance(after_ret));
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
} else if ((preds::TKind_is_extern(preds::Parser_kind(state)) && preds::TKind_is_fn(preds::Parser_kind(preds::Parser_advance(state))))) {
auto after_extern = preds::Parser_advance(state);
auto fn_name = preds::TKind_ident(preds::Parser_kind(after_extern));
auto mangled = ((trait_name + mlc::String("_", 1)) + fn_name);
auto params_result = parse_params(preds::Parser_advance(after_extern));
auto after_rparen = preds::Parser_advance(params_result.parser);
auto type_parser = (preds::TKind_is_arrow(preds::Parser_kind(after_rparen)) ? preds::Parser_advance(after_rparen) : after_rparen);
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
state = preds::Parser_advance(state);
}

}
return preds::TraitBodyResult{methods, preds::Parser_advance(state)};
}
preds::FieldDefsResult parse_field_defs(preds::Parser parser) noexcept{
auto field_defs = mlc::Array<std::shared_ptr<ast::FieldDef>>{};
auto state = parser;
while (((!preds::TKind_is_rbrace(preds::Parser_kind(state))) && (!preds::Parser_at_eof(state)))) {
auto field_name = preds::TKind_ident(preds::Parser_kind(state));
auto type_result = types::parse_type(preds::Parser_advance_by(state, 2));
field_defs.push_back(std::make_shared<ast::FieldDef>(ast::FieldDef{field_name, type_result.type_expr}));
state = type_result.parser;
if (preds::TKind_is_comma(preds::Parser_kind(state))) {
state = preds::Parser_advance(state);
}
}
return preds::FieldDefsResult{field_defs, preds::Parser_advance(state)};
}
ast::Program parse_program(mlc::Array<ast_tokens::Token> tokens) noexcept{
auto decls = mlc::Array<std::shared_ptr<ast::Decl>>{};
auto state = preds::parser_new(tokens);
while ((!preds::Parser_at_eof(state))) {
auto result = parse_decl(state);
decls.push_back(result.decl);
state = result.parser;
}
return ast::Program{decls};
}

} // namespace decls
