#define main mlc_user_main
#include "decl_extend.hpp"

namespace decl_extend {

mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept{
auto prefix = (type_name + mlc::String("_", 1));
if (((fn_name.length() > prefix.length()) && (fn_name.substring(0, prefix.length()) == prefix))) {
return fn_name.substring(prefix.length(), (fn_name.length() - prefix.length()));
} else {
return fn_name;
}
}
mlc::String default_expr_to_cpp(std::shared_ptr<ast::Expr> e) noexcept{
return std::visit(overloaded{[&](const ast::ExprInt& exprInt) { auto [v, __1] = exprInt; return literals::gen_integer_literal(v); },
[&](const ast::ExprStr& exprStr) { auto [s, __1] = exprStr; return literals::gen_string_literal(s); },
[&](const ast::ExprBool& exprBool) { auto [b, __1] = exprBool; return literals::gen_boolean_literal(b); },
[&](const ast::ExprUnit& exprUnit) { auto [__0] = exprUnit; return literals::gen_unit_literal(); },
[&](const auto& __v) { return mlc::String("0", 1); }
}, (*e));
}
mlc::String param_item_def(context::CodegenContext context, std::shared_ptr<ast::Param> p) noexcept{
return expr::parameter_declaration_item(type_gen::type_to_cpp(context, p->typ), cpp_naming::cpp_safe(p->name));
}
mlc::String param_item_proto(context::CodegenContext context, std::shared_ptr<ast::Param> p) noexcept{
auto base = param_item_def(context, p);
if (p->has_default) {
return ((base + mlc::String(" = ", 3)) + default_expr_to_cpp(p->default_));
} else {
return base;
}
}
mlc::String gen_params_def(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < params.length())) {
parts.push_back(param_item_def(context, params[index]));
index = (index + 1);
}
return parts.join(mlc::String(", ", 2));
}
mlc::String gen_params_proto(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < params.length())) {
parts.push_back(param_item_proto(context, params[index]));
index = (index + 1);
}
return parts.join(mlc::String(", ", 2));
}
mlc::String gen_trait_decl(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods) noexcept{
auto self_param = ((type_params.length() > 0) ? type_params[0] : mlc::String("T", 1));
auto tparam = ((type_params.length() > 0) ? type_params.join(mlc::String(", ", 2)) : mlc::String("T", 1));
auto template_header = expr::cpp_template_typename_header_line(tparam);
auto req_parts = mlc::Array<mlc::String>{};
auto method_index = 0;
while ((method_index < methods.length())) {
std::visit(overloaded{[&](const semantic_ir::SDeclFn& sDeclFn) { auto [mangled, __1, __2, params, ret_type, __5] = sDeclFn; return [&]() {
auto method_name = extract_method_name(mangled, trait_name);
auto ret_cpp = type_gen::type_to_cpp(context, std::make_shared<ast::TypeExpr>(ast::TyUnit{}));
return req_parts.push_back(expr::concept_requires_expression_method_returns_convertible(method_name, ret_cpp));
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*methods[method_index]));
method_index = (method_index + 1);
}
auto req_body = req_parts.join(mlc::String("; ", 2));
return expr::trait_concept_requires_definition_line(template_header, cpp_naming::cpp_safe(trait_name), self_param, req_body);
}
mlc::String gen_extend_wrapper_protos(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext context) noexcept{
if ((trait_name.length() == 0)) {
return mlc::String("", 0);
} else {
auto wrappers = mlc::String("", 0);
auto method_index = 0;
while ((method_index < methods.length())) {
std::visit(overloaded{[&](const semantic_ir::SDeclFn& sDeclFn) { auto [mangled, __1, __2, params, ret_type, __5] = sDeclFn; return [&]() {
auto wrapper_context = context::make_body_context(context, {}, {}, {}, {}, type_name, {}, {});
auto method_name = extract_method_name(mangled, type_name);
auto ret_cpp = type_gen::sem_type_to_cpp(wrapper_context, ret_type);
auto params_str = gen_params_proto(wrapper_context, params);
wrappers = (wrappers + expr::noexcept_function_prototype(ret_cpp, cpp_naming::cpp_safe(method_name), params_str));
return /* unit */;
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*methods[method_index]));
method_index = (method_index + 1);
}
return wrappers;
}
}
mlc::String gen_extend_trait_wrapper(mlc::String type_name, mlc::String mangled, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> ret_type, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept{
auto wrapper_context = context::make_body_context(context, {}, {}, {}, {}, type_name, {}, {});
auto method_name = extract_method_name(mangled, type_name);
auto ret_cpp = type_gen::sem_type_to_cpp(wrapper_context, ret_type);
auto fn_resolved = context_resolve_fn(context, mangled);
auto params_str = gen_params_proto(wrapper_context, params);
auto call_args = mlc::Array<mlc::String>{};
auto param_index = 0;
while ((param_index < params.length())) {
call_args.push_back(cpp_naming::cpp_safe(params[param_index]->name));
param_index = (param_index + 1);
}
return expr::inline_noexcept_forwarding_call(ret_cpp, cpp_naming::cpp_safe(method_name), params_str, fn_resolved, call_args.join(mlc::String(", ", 2)));
}
mlc::String gen_extend_trait_wrappers(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept{
auto wrappers = mlc::String("", 0);
auto method_index = 0;
while ((method_index < methods.length())) {
std::visit(overloaded{[&](const semantic_ir::SDeclFn& sDeclFn) { auto [mangled, __1, __2, params, ret_type, __5] = sDeclFn; return [&]() {
wrappers = (wrappers + gen_extend_trait_wrapper(type_name, mangled, params, ret_type, context, context_resolve_fn));
return /* unit */;
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*methods[method_index]));
method_index = (method_index + 1);
}
return wrappers;
}
mlc::String gen_extend_extern_method(mlc::String mangled, mlc::String type_name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> ret_type, mlc::String trait_name, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept{
auto method_name = ((trait_name.length() > 0) ? extract_method_name(mangled, type_name) : mlc::String("", 0));
if ((((type_name == mlc::String("i32", 3)) && (method_name == mlc::String("to_string", 9))) && (params.length() > 0))) {
auto ret_cpp = type_gen::sem_type_to_cpp(context, ret_type);
auto params_str = gen_params_def(context, params);
return expr::noexcept_one_line_returning_function(ret_cpp, context_resolve_fn(context, mangled), params_str, expr::runtime_to_string_call(cpp_naming::cpp_safe(params[0]->name)));
} else {
return mlc::String("", 0);
}
}
mlc::Array<mlc::String> gen_extend_method_parts(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext extend_context, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::function<mlc::String(std::shared_ptr<semantic_ir::SDecl>, context::CodegenContext)> gen_decl_fn) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto method_index = 0;
while ((method_index < methods.length())) {
std::visit(overloaded{[&](const semantic_ir::SDeclFn& sDeclFn) { auto [mangled, __1, __2, params, ret_type, body] = sDeclFn; return std::visit(overloaded{[&](const semantic_ir::SExprExtern& sExprExtern) { auto [__0, __1] = sExprExtern; return [&]() {
auto generated = gen_extend_extern_method(mangled, type_name, params, ret_type, trait_name, context, context_resolve_fn);
if ((generated.length() > 0)) {
parts.push_back(generated);
}
return /* unit */;
}(); },
[&](const auto& __v) { return parts.push_back(gen_decl_fn(methods[method_index], extend_context)); }
}, (*body)); },
[&](const auto& __v) { return parts.push_back(gen_decl_fn(methods[method_index], extend_context)); }
}, (*methods[method_index]));
method_index = (method_index + 1);
}
return parts;
}
mlc::String gen_decl_extend(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::function<mlc::String(std::shared_ptr<semantic_ir::SDecl>, context::CodegenContext)> gen_decl_fn) noexcept{
auto extend_context = context::make_body_context(context, {}, {}, {}, {}, type_name, {}, {});
auto method_parts = gen_extend_method_parts(type_name, trait_name, methods, extend_context, context, context_resolve_fn, gen_decl_fn);
auto methods_str = method_parts.join(mlc::String("", 0));
if ((trait_name.length() > 0)) {
auto trait_wrappers = gen_extend_trait_wrappers(type_name, methods, context, context_resolve_fn);
auto cpp_type = type_gen::type_name_to_cpp(context, type_name);
auto trait_safe = cpp_naming::cpp_safe(trait_name);
return ((methods_str + trait_wrappers) + expr::static_assert_concept_for_type_line(trait_safe, cpp_type, ((type_name + mlc::String(" does not implement ", 20)) + trait_name)));
} else {
return methods_str;
}
}

} // namespace decl_extend
