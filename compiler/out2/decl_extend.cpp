#include "decl_extend.hpp"

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

mlc::String gen_params_def(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::String gen_params_proto(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::String gen_trait_decl(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods) noexcept;

mlc::String gen_extend_wrapper_protos(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext context) noexcept;

mlc::String gen_extend_trait_wrapper(mlc::String type_name, mlc::String mangled, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> ret_type, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept;

mlc::String gen_extend_trait_wrappers(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept;

mlc::String gen_extend_extern_method(mlc::String mangled, mlc::String type_name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> ret_type, mlc::String trait_name, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept;

mlc::Array<mlc::String> gen_extend_method_parts(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext extend_context, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::function<mlc::String(std::shared_ptr<semantic_ir::SDecl>, context::CodegenContext)> gen_decl_fn) noexcept;

mlc::String gen_decl_extend(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::function<mlc::String(std::shared_ptr<semantic_ir::SDecl>, context::CodegenContext)> gen_decl_fn) noexcept;

mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept{
mlc::String prefix = type_name + mlc::String("_");
return fn_name.length() > prefix.length() && fn_name.substring(0, prefix.length()) == prefix ? fn_name.substring(prefix.length(), fn_name.length() - prefix.length()) : fn_name;
}

mlc::String default_expr_to_cpp(std::shared_ptr<ast::Expr> e) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprInt>((*e)._)) { auto _v_exprint = std::get<ast::ExprInt>((*e)._); auto [v, _w0] = _v_exprint; return literals::gen_integer_literal(v); } if (std::holds_alternative<ast::ExprStr>((*e)._)) { auto _v_exprstr = std::get<ast::ExprStr>((*e)._); auto [s, _w0] = _v_exprstr; return literals::gen_string_literal(s); } if (std::holds_alternative<ast::ExprBool>((*e)._)) { auto _v_exprbool = std::get<ast::ExprBool>((*e)._); auto [b, _w0] = _v_exprbool; return literals::gen_boolean_literal(b); } if (std::holds_alternative<ast::ExprUnit>((*e)._)) { auto _v_exprunit = std::get<ast::ExprUnit>((*e)._); auto [_w0] = _v_exprunit; return literals::gen_unit_literal(); } return mlc::String("0"); }();}

mlc::String param_item_def(context::CodegenContext context, std::shared_ptr<ast::Param> p) noexcept{return expr::parameter_declaration_item(type_gen::type_to_cpp(context, p->typ), cpp_naming::cpp_safe(p->name));}

mlc::String param_item_proto(context::CodegenContext context, std::shared_ptr<ast::Param> p) noexcept{
mlc::String base = param_item_def(context, p);
return p->has_default ? base + mlc::String(" = ") + default_expr_to_cpp(p->default_) : base;
}

mlc::String gen_params_def(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < params.size()){
{
parts.push_back(param_item_def(context, params[index]));
index = index + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String gen_params_proto(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < params.size()){
{
parts.push_back(param_item_proto(context, params[index]));
index = index + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String gen_trait_decl(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods) noexcept{
mlc::String self_param = type_params.size() > 0 ? type_params[0] : mlc::String("T");
mlc::String tparam = type_params.size() > 0 ? type_params.join(mlc::String(", ")) : mlc::String("T");
mlc::String template_header = expr::cpp_template_typename_header_line(tparam);
mlc::Array<mlc::String> req_parts = {};
int method_index = 0;
while (method_index < methods.size()){
{
[&]() -> void { if (std::holds_alternative<semantic_ir::SDeclFn>((*methods[method_index]))) { auto _v_sdeclfn = std::get<semantic_ir::SDeclFn>((*methods[method_index])); auto [mangled, _w0, _w1, params, ret_type, _w2] = _v_sdeclfn; return [&]() { 
  mlc::String method_name = extract_method_name(mangled, trait_name);
  mlc::String ret_cpp = type_gen::type_to_cpp(context, std::make_shared<ast::TypeExpr>((ast::TyUnit{})));
  return req_parts.push_back(expr::concept_requires_expression_method_returns_convertible(method_name, ret_cpp));
 }(); } return; }();
method_index = method_index + 1;
}
}
mlc::String req_body = req_parts.join(mlc::String("; "));
return expr::trait_concept_requires_definition_line(template_header, cpp_naming::cpp_safe(trait_name), self_param, req_body);
}

mlc::String gen_extend_wrapper_protos(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext context) noexcept{return trait_name.length() == 0 ? mlc::String("") : [&]() -> mlc::String { 
  mlc::String wrappers = mlc::String("");
  int method_index = 0;
  while (method_index < methods.size()){
{
std::visit(overloaded{
  [&](const SDeclFn& sdeclfn) {
auto [mangled, _w0, _w1, params, ret_type, _w2] = sdeclfn;
{
context::CodegenContext wrapper_context = context::make_body_context(context, {}, {}, mlc::HashMap<mlc::String, mlc::String>(), {}, type_name, {}, {});
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
context::CodegenContext wrapper_context = context::make_body_context(context, {}, {}, mlc::HashMap<mlc::String, mlc::String>(), {}, type_name, {}, {});
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
auto [mangled, _w0, _w1, params, ret_type, _w2] = sdeclfn;
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
[&]() -> void { if (std::holds_alternative<semantic_ir::SDeclFn>((*methods[method_index]))) { auto _v_sdeclfn = std::get<semantic_ir::SDeclFn>((*methods[method_index])); auto [mangled, _w0, _w1, params, ret_type, body] = _v_sdeclfn; return [&]() -> void { if (std::holds_alternative<semantic_ir::SExprExtern>((*body)._)) { auto _v_sexprextern = std::get<semantic_ir::SExprExtern>((*body)._); auto [_w0, _w1] = _v_sexprextern; return [&]() { 
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

mlc::String gen_decl_extend(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::function<mlc::String(std::shared_ptr<semantic_ir::SDecl>, context::CodegenContext)> gen_decl_fn) noexcept{
context::CodegenContext extend_context = context::make_body_context(context, {}, {}, mlc::HashMap<mlc::String, mlc::String>(), {}, type_name, {}, {});
mlc::Array<mlc::String> method_parts = gen_extend_method_parts(type_name, trait_name, methods, extend_context, context, context_resolve_fn, gen_decl_fn);
mlc::String methods_str = method_parts.join(mlc::String(""));
return trait_name.length() > 0 ? [&]() -> mlc::String { 
  mlc::String trait_wrappers = gen_extend_trait_wrappers(type_name, methods, context, context_resolve_fn);
  mlc::String cpp_type = type_gen::type_name_to_cpp(context, type_name);
  mlc::String trait_safe = cpp_naming::cpp_safe(trait_name);
  return methods_str + trait_wrappers + expr::static_assert_concept_for_type_line(trait_safe, cpp_type, type_name + mlc::String(" does not implement ") + trait_name);
 }() : methods_str;
}

} // namespace decl_extend
