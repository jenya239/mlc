#include "decl_extend.hpp"

#include "ast.hpp"
#include "ast.hpp"
#include "literals.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "type_gen.hpp"
#include "expr.hpp"

namespace decl_extend {

using namespace ast;
using namespace ast;
using namespace literals;
using namespace semantic_ir;
using namespace registry;
using namespace context;
using namespace cpp_naming;
using namespace type_gen;
using namespace expr;
using namespace ast_tokens;

mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept;

mlc::String default_expr_to_cpp(std::shared_ptr<ast::Expr> e) noexcept;

mlc::String param_item_def(context::CodegenContext context, std::shared_ptr<ast::Param> p) noexcept;

mlc::String param_item_proto(context::CodegenContext context, std::shared_ptr<ast::Param> p) noexcept;

mlc::String gen_params_proto(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::Array<mlc::String> gen_parameter_proto_items(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::String gen_params_def(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::Array<mlc::String> gen_parameter_def_items(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::String concept_declval_arguments_for_params(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::String extend_method_return_type_cpp(context::CodegenContext extend_context, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods) noexcept;

mlc::String generic_trait_static_assert_line(mlc::String trait_name, mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext extend_context, mlc::String implementor_type_cpp) noexcept;

mlc::String trait_method_field_line(context::CodegenContext context, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type) noexcept;

mlc::String gen_trait_struct(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods) noexcept;

mlc::String gen_trait_decl(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods) noexcept;

mlc::String gen_extend_wrapper_protos(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext context) noexcept;

mlc::String gen_extend_trait_wrapper(mlc::String type_name, mlc::String mangled, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> ret_type, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept;

mlc::String gen_extend_trait_wrappers(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept;

mlc::String gen_extend_extern_method(mlc::String mangled, mlc::String type_name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> ret_type, mlc::String trait_name, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept;

mlc::Array<mlc::String> gen_extend_method_parts(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext extend_context, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::function<mlc::String(std::shared_ptr<semantic_ir::SDecl>, context::CodegenContext)> gen_decl_fn) noexcept;

mlc::String gen_extend_trait_concept_adapter(mlc::String type_name, mlc::String trait_name, mlc::String mangled, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> ret_type, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept;

mlc::String gen_extend_trait_concept_adapters(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept;

mlc::String gen_decl_extend(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::function<mlc::String(std::shared_ptr<semantic_ir::SDecl>, context::CodegenContext)> gen_decl_fn) noexcept;

mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept{
mlc::String prefix = type_name + mlc::String("_");
return fn_name.length() > prefix.length() && fn_name.substring(0, prefix.length()) == prefix ? fn_name.substring(prefix.length(), fn_name.length() - prefix.length()) : fn_name;
}

mlc::String default_expr_to_cpp(std::shared_ptr<ast::Expr> e) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprInt>((*e)._)) { auto _v_exprint = std::get<ast::ExprInt>((*e)._); auto [v, _w0] = _v_exprint; return literals::gen_integer_literal(v); } if (std::holds_alternative<ast::ExprStr>((*e)._)) { auto _v_exprstr = std::get<ast::ExprStr>((*e)._); auto [s, _w0] = _v_exprstr; return literals::gen_string_literal(s); } if (std::holds_alternative<ast::ExprBool>((*e)._)) { auto _v_exprbool = std::get<ast::ExprBool>((*e)._); auto [b, _w0] = _v_exprbool; return literals::gen_boolean_literal(b); } if (std::holds_alternative<ast::ExprUnit>((*e)._)) { auto _v_exprunit = std::get<ast::ExprUnit>((*e)._); auto [_w0] = _v_exprunit; return literals::gen_unit_literal(); } if (std::holds_alternative<ast::ExprFloat>((*e)._)) { auto _v_exprfloat = std::get<ast::ExprFloat>((*e)._); auto [v, _w0] = _v_exprfloat; return literals::gen_float_literal(v); } if (std::holds_alternative<ast::ExprI64>((*e)._)) { auto _v_expri64 = std::get<ast::ExprI64>((*e)._); auto [v, _w0] = _v_expri64; return literals::gen_i64_literal(v); } if (std::holds_alternative<ast::ExprU8>((*e)._)) { auto _v_expru8 = std::get<ast::ExprU8>((*e)._); auto [v, _w0] = _v_expru8; return literals::gen_u8_literal(v); } if (std::holds_alternative<ast::ExprUsize>((*e)._)) { auto _v_exprusize = std::get<ast::ExprUsize>((*e)._); auto [v, _w0] = _v_exprusize; return literals::gen_usize_literal(v); } if (std::holds_alternative<ast::ExprChar>((*e)._)) { auto _v_exprchar = std::get<ast::ExprChar>((*e)._); auto [v, _w0] = _v_exprchar; return literals::gen_char_literal(v); } return mlc::String("0"); }();}

mlc::String param_item_def(context::CodegenContext context, std::shared_ptr<ast::Param> p) noexcept{return expr::parameter_declaration_item(type_gen::type_to_cpp(context, p->typ), cpp_naming::cpp_safe(p->name));}

mlc::String param_item_proto(context::CodegenContext context, std::shared_ptr<ast::Param> p) noexcept{
mlc::String base = param_item_def(context, p);
return p->has_default ? base + mlc::String(" = ") + default_expr_to_cpp(p->default_) : base;
}

mlc::String gen_params_proto(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
return gen_parameter_proto_items(context, params).join(mlc::String(", "));
}

mlc::Array<mlc::String> gen_parameter_proto_items(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < params.size()){
{
parts.push_back(param_item_proto(context, params[index]));
index = index + 1;
}
}
return parts;
}

mlc::String gen_params_def(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
return gen_parameter_def_items(context, params).join(mlc::String(", "));
}

mlc::Array<mlc::String> gen_parameter_def_items(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < params.size()){
{
parts.push_back(param_item_def(context, params[index]));
index = index + 1;
}
}
return parts;
}

mlc::String concept_declval_arguments_for_params(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
mlc::Array<mlc::String> parts = {};
int parameter_index = 0;
while (parameter_index < params.size()){
{
if (params[parameter_index]->name != mlc::String("self")){
{
parts.push_back(expr::std_declval_expression(type_gen::type_to_cpp(context, params[parameter_index]->typ)));
}
}
parameter_index = parameter_index + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String extend_method_return_type_cpp(context::CodegenContext extend_context, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods) noexcept{
mlc::String result = mlc::String("void");
int method_index = 0;
while (method_index < methods.size()){
{
std::visit(overloaded{
  [&](const SDeclFn& sdeclfn) {
auto [_w0, _w1, _w2, _w3, return_type, _w4, _w5] = sdeclfn;
{
result = type_gen::sem_type_to_cpp(extend_context, return_type);
}
},
  [&](const auto& _unused) {
{
}
}
}, (*methods[method_index]));
if (result != mlc::String("void")){
{
method_index = methods.size();
}
} else {
{
method_index = method_index + 1;
}
}
}
}
return result;
}

mlc::String generic_trait_static_assert_line(mlc::String trait_name, mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext extend_context, mlc::String implementor_type_cpp) noexcept{
mlc::String trait_safe = cpp_naming::cpp_safe(trait_name);
mlc::String message = type_name + mlc::String(" does not implement ") + trait_name;
return trait_name == mlc::String("ExprVisitor") ? [&]() -> mlc::String { 
  mlc::String result_type_cpp = extend_method_return_type_cpp(extend_context, methods);
  return expr::static_assert_concept_for_result_and_implementor_line(trait_safe, result_type_cpp, implementor_type_cpp, message);
 }() : expr::static_assert_concept_for_type_line(trait_safe, implementor_type_cpp, message);
}

mlc::String trait_method_field_line(context::CodegenContext context, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type) noexcept{
mlc::Array<mlc::String> parameter_types = {};
int parameter_index = 0;
while (parameter_index < params.size()){
{
if (params[parameter_index]->name != mlc::String("self")){
{
parameter_types.push_back(type_gen::type_to_cpp(context, params[parameter_index]->typ));
}
}
parameter_index = parameter_index + 1;
}
}
mlc::String return_cpp = type_gen::sem_type_to_cpp(context, return_type);
return expr::std_function_field_line(return_cpp, parameter_types.join(mlc::String(", ")), cpp_naming::cpp_safe(method_name));
}

mlc::String gen_trait_struct(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods) noexcept{
context::CodegenContext trait_context = context::CodegenContext_for_type_body(context, trait_name);
mlc::String template_header = type_params.size() > 0 ? expr::cpp_template_typename_header_line(type_params.join(mlc::String(", "))) : mlc::String("");
mlc::Array<mlc::String> field_lines = {};
int method_index = 0;
while (method_index < methods.size()){
{
[&]() -> void { if (std::holds_alternative<semantic_ir::SDeclFn>((*methods[method_index]))) { auto _v_sdeclfn = std::get<semantic_ir::SDeclFn>((*methods[method_index])); auto [mangled, _w0, _w1, params, return_type, _w2, _w3] = _v_sdeclfn; return [&]() { 
  mlc::String method_name = extract_method_name(mangled, trait_name);
  return field_lines.push_back(trait_method_field_line(trait_context, method_name, params, return_type));
 }(); } return; }();
method_index = method_index + 1;
}
}
return expr::trait_struct_definition_lines(template_header, expr::trait_vtable_struct_cpp_name(cpp_naming::cpp_safe(trait_name)), field_lines.join(mlc::String("")));
}

mlc::String gen_trait_decl(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods) noexcept{
context::CodegenContext trait_context = context::CodegenContext_for_type_body(context, trait_name);
mlc::String template_header = type_params.size() > 0 ? mlc::String("template<typename ") + type_params[0] + mlc::String(", typename Self>\n") : mlc::String("template<typename Self>\n");
mlc::Array<mlc::String> req_parts = {};
int method_index = 0;
while (method_index < methods.size()){
{
[&]() -> void { if (std::holds_alternative<semantic_ir::SDeclFn>((*methods[method_index]))) { auto _v_sdeclfn = std::get<semantic_ir::SDeclFn>((*methods[method_index])); auto [mangled, _w0, _w1, params, ret_type, _w2, _w3] = _v_sdeclfn; return [&]() { 
  mlc::String method_name = extract_method_name(mangled, trait_name);
  mlc::String prefixed_callee = cpp_naming::cpp_safe(mangled);
  mlc::String ret_cpp = type_gen::sem_type_to_cpp(trait_context, ret_type);
  mlc::String declval_arguments = concept_declval_arguments_for_params(trait_context, params);
  return req_parts.push_back(expr::concept_requires_expression_method_returns_convertible(prefixed_callee, declval_arguments, ret_cpp));
 }(); } return; }();
method_index = method_index + 1;
}
}
mlc::String req_body = req_parts.join(mlc::String("; "));
return expr::trait_concept_requires_definition_line(template_header, cpp_naming::cpp_safe(trait_name), mlc::String("Self"), req_body);
}

mlc::String gen_extend_wrapper_protos(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext context) noexcept{return trait_name.length() == 0 ? mlc::String("") : [&]() -> mlc::String { 
  mlc::String wrappers = mlc::String("");
  int method_index = 0;
  while (method_index < methods.size()){
{
std::visit(overloaded{
  [&](const SDeclFn& sdeclfn) {
auto [mangled, _w0, _w1, params, ret_type, _w2, _w3] = sdeclfn;
{
context::CodegenContext wrapper_context = context::CodegenContext_for_type_body(context, type_name);
mlc::String method_name = extract_method_name(mangled, type_name);
mlc::String ret_cpp = type_gen::sem_type_to_cpp(wrapper_context, ret_type);
mlc::String params_str = gen_params_proto(wrapper_context, params);
wrappers = wrappers + expr::noexcept_function_prototype(ret_cpp, cpp_naming::cpp_safe(method_name), params_str);
}
},
  [&](const auto& _unused) {
{
}
}
}, (*methods[method_index]));
method_index = method_index + 1;
}
}
  return wrappers;
 }();}

mlc::String gen_extend_trait_wrapper(mlc::String type_name, mlc::String mangled, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> ret_type, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept{
context::CodegenContext wrapper_context = context::CodegenContext_for_type_body(context, type_name);
mlc::String method_name = extract_method_name(mangled, type_name);
mlc::String ret_cpp = type_gen::sem_type_to_cpp(wrapper_context, ret_type);
mlc::String fn_resolved = context_resolve_fn(context, mangled);
mlc::String params_str = gen_params_proto(wrapper_context, params);
mlc::Array<mlc::String> call_args = {};
int param_index = 0;
while (param_index < params.size()){
{
call_args.push_back(cpp_naming::cpp_safe(params[param_index]->name));
param_index = param_index + 1;
}
}
return expr::inline_noexcept_forwarding_call(ret_cpp, cpp_naming::cpp_safe(method_name), params_str, fn_resolved, call_args.join(mlc::String(", ")));
}

mlc::String gen_extend_trait_wrappers(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept{
mlc::String wrappers = mlc::String("");
int method_index = 0;
while (method_index < methods.size()){
{
std::visit(overloaded{
  [&](const SDeclFn& sdeclfn) {
auto [mangled, _w0, _w1, params, ret_type, _w2, _w3] = sdeclfn;
{
wrappers = wrappers + gen_extend_trait_wrapper(type_name, mangled, params, ret_type, context, context_resolve_fn);
}
},
  [&](const auto& _unused) {
{
}
}
}, (*methods[method_index]));
method_index = method_index + 1;
}
}
return wrappers;
}

mlc::String gen_extend_extern_method(mlc::String mangled, mlc::String type_name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> ret_type, mlc::String trait_name, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept{
mlc::String method_name = trait_name.length() > 0 ? extract_method_name(mangled, type_name) : mlc::String("");
return type_name == mlc::String("i32") && method_name == mlc::String("to_string") && params.size() > 0 ? [&]() -> mlc::String { 
  mlc::String ret_cpp = type_gen::sem_type_to_cpp(context, ret_type);
  mlc::String params_str = gen_params_def(context, params);
  return expr::noexcept_one_line_returning_function(ret_cpp, context_resolve_fn(context, mangled), params_str, expr::runtime_to_string_call(cpp_naming::cpp_safe(params[0]->name)));
 }() : mlc::String("");
}

mlc::Array<mlc::String> gen_extend_method_parts(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext extend_context, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::function<mlc::String(std::shared_ptr<semantic_ir::SDecl>, context::CodegenContext)> gen_decl_fn) noexcept{
mlc::Array<mlc::String> parts = {};
int method_index = 0;
while (method_index < methods.size()){
{
[&]() -> void { if (std::holds_alternative<semantic_ir::SDeclFn>((*methods[method_index]))) { auto _v_sdeclfn = std::get<semantic_ir::SDeclFn>((*methods[method_index])); auto [mangled, _w0, _w1, params, ret_type, body, _w2] = _v_sdeclfn; return [&]() -> void { if (std::holds_alternative<semantic_ir::SExprExtern>((*body)._)) { auto _v_sexprextern = std::get<semantic_ir::SExprExtern>((*body)._); auto [_w0, _w1] = _v_sexprextern; return [&]() { 
  mlc::String generated = gen_extend_extern_method(mangled, type_name, params, ret_type, trait_name, context, context_resolve_fn);
  if (generated.length() > 0){
parts.push_back(generated);
}
 }(); } return parts.push_back(gen_decl_fn(methods[method_index], extend_context)); }(); } return parts.push_back(gen_decl_fn(methods[method_index], extend_context)); }();
method_index = method_index + 1;
}
}
return parts;
}

mlc::String gen_extend_trait_concept_adapter(mlc::String type_name, mlc::String trait_name, mlc::String mangled, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> ret_type, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept{
context::CodegenContext extend_context = context::CodegenContext_for_type_body(context, type_name);
mlc::String method_name = extract_method_name(mangled, type_name);
mlc::String trait_callee = cpp_naming::cpp_safe(trait_name + mlc::String("_") + method_name);
mlc::String ret_cpp = type_gen::sem_type_to_cpp(extend_context, ret_type);
mlc::String params_str = gen_params_proto(extend_context, params);
mlc::String fn_resolved = context_resolve_fn(context, mangled);
mlc::Array<mlc::String> call_args = {};
int param_index = 0;
while (param_index < params.size()){
{
call_args.push_back(cpp_naming::cpp_safe(params[param_index]->name));
param_index = param_index + 1;
}
}
return expr::inline_noexcept_forwarding_call(ret_cpp, trait_callee, params_str, fn_resolved, call_args.join(mlc::String(", ")));
}

mlc::String gen_extend_trait_concept_adapters(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept{
mlc::String adapters = mlc::String("");
int method_index = 0;
while (method_index < methods.size()){
{
std::visit(overloaded{
  [&](const SDeclFn& sdeclfn) {
auto [mangled, _w0, _w1, params, ret_type, _w2, _w3] = sdeclfn;
{
adapters = adapters + gen_extend_trait_concept_adapter(type_name, trait_name, mangled, params, ret_type, context, context_resolve_fn);
}
},
  [&](const auto& _unused) {
{
}
}
}, (*methods[method_index]));
method_index = method_index + 1;
}
}
return adapters;
}

mlc::String gen_decl_extend(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::function<mlc::String(std::shared_ptr<semantic_ir::SDecl>, context::CodegenContext)> gen_decl_fn) noexcept{
context::CodegenContext extend_context = context::CodegenContext_for_type_body(context, type_name);
mlc::Array<mlc::String> method_parts = gen_extend_method_parts(type_name, trait_name, methods, extend_context, context, context_resolve_fn, gen_decl_fn);
mlc::String methods_str = method_parts.join(mlc::String(""));
return trait_name.length() > 0 ? [&]() -> mlc::String { 
  mlc::String trait_wrappers = gen_extend_trait_wrappers(type_name, methods, context, context_resolve_fn);
  mlc::String trait_concept_adapters = gen_extend_trait_concept_adapters(type_name, trait_name, methods, context, context_resolve_fn);
  mlc::String cpp_type = type_gen::type_name_to_cpp(context, type_name);
  return methods_str + trait_wrappers + trait_concept_adapters + generic_trait_static_assert_line(trait_name, type_name, methods, extend_context, cpp_type);
 }() : methods_str;
}

} // namespace decl_extend
