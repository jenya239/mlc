#include "decl.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "type_gen.hpp"
#include "eval.hpp"
#include "expr.hpp"

namespace decl {

using namespace ast;
using namespace semantic_ir;
using namespace registry;
using namespace context;
using namespace cpp_naming;
using namespace type_gen;
using namespace eval;
using namespace expr;
using namespace ast_tokens;

context::CodegenContext CodegenContext_for_type_body(context::CodegenContext self, mlc::String type_name) noexcept;

mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept;

mlc::String gen_params(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::String gen_fn_proto(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, context::CodegenContext context) noexcept;

context::CodegenContext compute_fn_body_context(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, context::CodegenContext context) noexcept;

mlc::String gen_fn_decl(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SExpr> body, context::CodegenContext context) noexcept;

mlc::String gen_trait_decl(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods) noexcept;

mlc::String gen_extend_wrapper_protos(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext context) noexcept;

mlc::String gen_extend_trait_wrapper(mlc::String type_name, mlc::String mangled, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> ret_type, context::CodegenContext context) noexcept;

mlc::String gen_extend_trait_wrappers(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext context) noexcept;

mlc::String gen_extend_extern_method(mlc::String mangled, mlc::String type_name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> ret_type, mlc::String trait_name, context::CodegenContext context) noexcept;

mlc::Array<mlc::String> gen_extend_method_parts(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext extend_context, context::CodegenContext context) noexcept;

mlc::String gen_decl_extend(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext context) noexcept;

mlc::String gen_decl(std::shared_ptr<semantic_ir::SDecl> decl, context::CodegenContext context) noexcept;

mlc::String gen_proto(std::shared_ptr<semantic_ir::SDecl> decl, context::CodegenContext context) noexcept;

bool decls_have_main(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> decls) noexcept;

mlc::Array<mlc::String> collect_decl_parts(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> decls, context::CodegenContext context, int phase) noexcept;

context::CodegenContext CodegenContext_for_type_body(context::CodegenContext self, mlc::String type_name) noexcept{return context::make_body_context(self, {}, {}, mlc::HashMap<mlc::String, mlc::String>(), {}, type_name, {}, {});}

mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept{
mlc::String prefix = type_name + mlc::String("_");
return fn_name.length() > prefix.length() && fn_name.substring(0, prefix.length()) == prefix ? fn_name.substring(prefix.length(), fn_name.length() - prefix.length()) : fn_name;
}

mlc::String gen_params(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < params.size()){
{
parts.push_back(type_gen::type_to_cpp(context, params[index]->typ) + mlc::String(" ") + cpp_naming::cpp_safe(params[index]->name));
index = index + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String gen_fn_proto(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, context::CodegenContext context) noexcept{
mlc::String prefix = cpp_naming::template_prefix(type_params) + type_gen::requires_clause(type_params, type_bounds);
mlc::String safe_name = context::context_resolve(context, name);
context::CodegenContext prototype_context = params.size() > 0 && params[0]->name == mlc::String("self") ? [&]() -> context::CodegenContext { if (std::holds_alternative<ast::TyNamed>((*params[0]->typ))) { auto _v_tynamed = std::get<ast::TyNamed>((*params[0]->typ)); auto [type_name] = _v_tynamed; return [&]() -> context::CodegenContext { 
  mlc::String resolved_type = type_name == mlc::String("Self") || type_name == mlc::String("self") ? context.self_type : type_name;
  return CodegenContext_for_type_body(context, resolved_type);
 }(); } return context; }() : context;
return name == mlc::String("main") && params.size() == 0 ? prefix + expr::noexcept_function_prototype(type_gen::sem_type_to_cpp(prototype_context, return_type), safe_name, expr::main_program_parameter_list()) : prefix + expr::noexcept_function_prototype(type_gen::sem_type_to_cpp(prototype_context, return_type), safe_name, gen_params(prototype_context, params));
}

context::CodegenContext compute_fn_body_context(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, context::CodegenContext context) noexcept{
mlc::Array<mlc::String> fn_shared_params = param_analysis::collect_shared_params(params);
mlc::Array<mlc::String> fn_shared_array_params = param_analysis::collect_shared_array_params(params);
mlc::HashMap<mlc::String, mlc::String> fn_array_elem_types = param_analysis::collect_array_elem_types(params);
mlc::Array<mlc::String> fn_shared_map_params = param_analysis::collect_shared_map_params(params);
mlc::Array<mlc::String> fn_value_params = param_analysis::collect_value_params(params);
return params.size() > 0 && params[0]->name == mlc::String("self") ? [&]() -> context::CodegenContext { if (std::holds_alternative<ast::TyNamed>((*params[0]->typ))) { auto _v_tynamed = std::get<ast::TyNamed>((*params[0]->typ)); auto [self_type_name] = _v_tynamed; return [&]() -> context::CodegenContext { 
  mlc::String resolved_type = self_type_name == mlc::String("Self") || self_type_name == mlc::String("self") ? context.self_type : self_type_name;
  return context::make_body_context(context, fn_shared_params, fn_shared_array_params, fn_array_elem_types, fn_shared_map_params, resolved_type, fn_value_params, {});
 }(); } return context::make_body_context(context, fn_shared_params, fn_shared_array_params, fn_array_elem_types, fn_shared_map_params, context.self_type, fn_value_params, {}); }() : context::make_body_context(context, fn_shared_params, fn_shared_array_params, fn_array_elem_types, fn_shared_map_params, context.self_type, fn_value_params, {});
}

mlc::String gen_fn_decl(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SExpr> body, context::CodegenContext context) noexcept{
mlc::String prefix = cpp_naming::template_prefix(type_params) + type_gen::requires_clause(type_params, type_bounds);
mlc::String safe_name = context::context_resolve(context, name);
context::CodegenContext body_context = compute_fn_body_context(name, params, context);
context::CodegenContext prototype_context = params.size() > 0 && params[0]->name == mlc::String("self") ? body_context : context;
return name == mlc::String("main") && params.size() == 0 ? [&]() -> mlc::String { 
  mlc::String preamble = mlc::String("mlc::io::set_args(std::vector<mlc::String>(argv + 1, argv + argc));\n");
  return prefix + expr::noexcept_function_body_open(type_gen::sem_type_to_cpp(prototype_context, return_type), safe_name, expr::main_program_parameter_list()) + preamble + eval::gen_fn_body(body, body_context) + mlc::String("}\n");
 }() : prefix + expr::noexcept_function_body_open(type_gen::sem_type_to_cpp(prototype_context, return_type), safe_name, gen_params(prototype_context, params)) + eval::gen_fn_body(body, body_context) + mlc::String("}\n");
}

mlc::String gen_trait_decl(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods) noexcept{
mlc::String self_param = type_params.size() > 0 ? type_params[0] : mlc::String("T");
mlc::String tparam = type_params.size() > 0 ? type_params.join(mlc::String(", ")) : mlc::String("T");
mlc::String prefix = mlc::String("template<typename ") + tparam + mlc::String(">\n");
mlc::Array<mlc::String> req_parts = {};
int method_index = 0;
while (method_index < methods.size()){
{
[&]() -> void { if (std::holds_alternative<semantic_ir::SDeclFn>((*methods[method_index]))) { auto _v_sdeclfn = std::get<semantic_ir::SDeclFn>((*methods[method_index])); auto [mangled, _w0, _w1, params, ret_type, _w2] = _v_sdeclfn; return [&]() { 
  mlc::String method_name = extract_method_name(mangled, trait_name);
  mlc::String ret_cpp = type_gen::type_to_cpp(context, std::make_shared<ast::TypeExpr>((ast::TyUnit{})));
  return req_parts.push_back(mlc::String("{ ") + method_name + mlc::String("(self) } -> std::convertible_to<") + ret_cpp + mlc::String(">"));
 }(); } return; }();
method_index = method_index + 1;
}
}
mlc::String req_body = req_parts.join(mlc::String("; "));
return prefix + mlc::String("concept ") + cpp_naming::cpp_safe(trait_name) + mlc::String(" = requires(const ") + self_param + mlc::String("& self) { ") + req_body + mlc::String("; };\n");
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
context::CodegenContext wrapper_context = CodegenContext_for_type_body(context, type_name);
mlc::String method_name = extract_method_name(mangled, type_name);
mlc::String ret_cpp = type_gen::sem_type_to_cpp(wrapper_context, ret_type);
mlc::String params_str = gen_params(wrapper_context, params);
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

mlc::String gen_extend_trait_wrapper(mlc::String type_name, mlc::String mangled, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> ret_type, context::CodegenContext context) noexcept{
context::CodegenContext wrapper_context = CodegenContext_for_type_body(context, type_name);
mlc::String method_name = extract_method_name(mangled, type_name);
mlc::String ret_cpp = type_gen::sem_type_to_cpp(wrapper_context, ret_type);
mlc::String fn_resolved = context::context_resolve(context, mangled);
mlc::String params_str = gen_params(wrapper_context, params);
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

mlc::String gen_extend_trait_wrappers(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext context) noexcept{
mlc::String wrappers = mlc::String("");
int method_index = 0;
while (method_index < methods.size()){
{
std::visit(overloaded{
  [&](const SDeclFn& sdeclfn) {
auto [mangled, _w0, _w1, params, ret_type, _w2] = sdeclfn;
{
wrappers = wrappers + gen_extend_trait_wrapper(type_name, mangled, params, ret_type, context);
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

mlc::String gen_extend_extern_method(mlc::String mangled, mlc::String type_name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> ret_type, mlc::String trait_name, context::CodegenContext context) noexcept{
mlc::String method_name = trait_name.length() > 0 ? extract_method_name(mangled, type_name) : mlc::String("");
return type_name == mlc::String("i32") && method_name == mlc::String("to_string") && params.size() > 0 ? [&]() -> mlc::String { 
  mlc::String ret_cpp = type_gen::sem_type_to_cpp(context, ret_type);
  mlc::String params_str = gen_params(context, params);
  return expr::noexcept_one_line_returning_function(ret_cpp, context::context_resolve(context, mangled), params_str, expr::runtime_to_string_call(cpp_naming::cpp_safe(params[0]->name)));
 }() : mlc::String("");
}

mlc::Array<mlc::String> gen_extend_method_parts(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext extend_context, context::CodegenContext context) noexcept{
mlc::Array<mlc::String> parts = {};
int method_index = 0;
while (method_index < methods.size()){
{
[&]() -> void { if (std::holds_alternative<semantic_ir::SDeclFn>((*methods[method_index]))) { auto _v_sdeclfn = std::get<semantic_ir::SDeclFn>((*methods[method_index])); auto [mangled, _w0, _w1, params, ret_type, body] = _v_sdeclfn; return [&]() -> void { if (std::holds_alternative<semantic_ir::SExprExtern>((*body)._)) { auto _v_sexprextern = std::get<semantic_ir::SExprExtern>((*body)._); auto [_w0, _w1] = _v_sexprextern; return [&]() { 
  mlc::String generated = gen_extend_extern_method(mangled, type_name, params, ret_type, trait_name, context);
  if (generated.length() > 0){
parts.push_back(generated);
}
 }(); } return parts.push_back(gen_decl(methods[method_index], extend_context)); }(); } return parts.push_back(gen_decl(methods[method_index], extend_context)); }();
method_index = method_index + 1;
}
}
return parts;
}

mlc::String gen_decl_extend(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SDecl>> methods, context::CodegenContext context) noexcept{
context::CodegenContext extend_context = CodegenContext_for_type_body(context, type_name);
mlc::Array<mlc::String> method_parts = gen_extend_method_parts(type_name, trait_name, methods, extend_context, context);
mlc::String methods_str = method_parts.join(mlc::String(""));
return trait_name.length() > 0 ? [&]() -> mlc::String { 
  mlc::String trait_wrappers = gen_extend_trait_wrappers(type_name, methods, context);
  mlc::String cpp_type = type_gen::type_name_to_cpp(context, type_name);
  mlc::String trait_safe = cpp_naming::cpp_safe(trait_name);
  return methods_str + trait_wrappers + mlc::String("static_assert(") + trait_safe + mlc::String("<") + cpp_type + mlc::String(">, \"") + type_name + mlc::String(" does not implement ") + trait_name + mlc::String("\");\n");
 }() : methods_str;
}

mlc::String gen_decl(std::shared_ptr<semantic_ir::SDecl> decl, context::CodegenContext context) noexcept{return std::visit(overloaded{
  [&](const SDeclType& sdecltype) -> mlc::String { auto [type_name, type_params, variants] = sdecltype; return type_gen::gen_type_decl(context, type_name, type_params, variants); },
  [&](const SDeclTrait& sdecltrait) -> mlc::String { auto [name, type_params, methods] = sdecltrait; return gen_trait_decl(context, name, type_params, methods); },
  [&](const SDeclFn& sdeclfn) -> mlc::String { auto [name, type_params, type_bounds, params, return_type, body] = sdeclfn; return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprExtern>((*body)._)) { auto _v_sexprextern = std::get<semantic_ir::SExprExtern>((*body)._); auto [_w0, _w1] = _v_sexprextern; return mlc::String(""); } return gen_fn_decl(name, type_params, type_bounds, params, return_type, body, context); }(); },
  [&](const SDeclExtend& sdeclextend) -> mlc::String { auto [type_name, trait_name, methods] = sdeclextend; return gen_decl_extend(type_name, trait_name, methods, context); },
  [&](const SDeclImport& sdeclimport) -> mlc::String { auto [_w0, _w1] = sdeclimport; return mlc::String(""); },
  [&](const SDeclExported& sdeclexported) -> mlc::String { auto [d] = sdeclexported; return gen_decl(d, context); }
}, (*decl));}

mlc::String gen_proto(std::shared_ptr<semantic_ir::SDecl> decl, context::CodegenContext context) noexcept{return std::visit(overloaded{
  [&](const SDeclType& sdecltype) -> mlc::String { auto [_w0, _w1, _w2] = sdecltype; return mlc::String(""); },
  [&](const SDeclTrait& sdecltrait) -> mlc::String { auto [_w0, _w1, _w2] = sdecltrait; return mlc::String(""); },
  [&](const SDeclFn& sdeclfn) -> mlc::String { auto [name, type_params, type_bounds, params, ret, body] = sdeclfn; return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprExtern>((*body)._)) { auto _v_sexprextern = std::get<semantic_ir::SExprExtern>((*body)._); auto [_w0, _w1] = _v_sexprextern; return mlc::String(""); } return gen_fn_proto(name, type_params, type_bounds, params, ret, context); }(); },
  [&](const SDeclExtend& sdeclextend) -> mlc::String { auto [type_name, _w0, methods] = sdeclextend; return [&]() -> mlc::String { 
  context::CodegenContext extend_context = CodegenContext_for_type_body(context, type_name);
  mlc::Array<mlc::String> parts = {};
  int index = 0;
  while (index < methods.size()){
{
parts.push_back(gen_proto(methods[index], extend_context));
index = index + 1;
}
}
  return parts.join(mlc::String(""));
 }(); },
  [&](const SDeclImport& sdeclimport) -> mlc::String { auto [_w0, _w1] = sdeclimport; return mlc::String(""); },
  [&](const SDeclExported& sdeclexported) -> mlc::String { auto [d] = sdeclexported; return gen_proto(d, context); }
}, (*decl));}

bool decls_have_main(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> decls) noexcept{
bool found = false;
int index = 0;
while (index < decls.size()){
{
std::shared_ptr<semantic_ir::SDecl> inner = semantic_ir::sdecl_inner(decls[index]);
bool is_main = [&]() { if (std::holds_alternative<semantic_ir::SDeclFn>((*inner))) { auto _v_sdeclfn = std::get<semantic_ir::SDeclFn>((*inner)); auto [name, _w0, _w1, _w2, _w3, _w4] = _v_sdeclfn; return name == mlc::String("main"); } return false; }();
if (is_main){
{
found = true;
}
} else {
{
std::make_tuple();
}
}
index = index + 1;
}
}
return found;
}

mlc::Array<mlc::String> collect_decl_parts(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> decls, context::CodegenContext context, int phase) noexcept{
mlc::Array<mlc::String> parts = {};
if (phase == 0){
{
int index = 0;
[&]() { 
  while (index < decls.size()){
{
mlc::String wrapper = [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SDeclExtend>((*decls[index]))) { auto _v_sdeclextend = std::get<semantic_ir::SDeclExtend>((*decls[index])); auto [type_name, trait_name, methods] = _v_sdeclextend; return gen_extend_wrapper_protos(type_name, trait_name, methods, context); } if (std::holds_alternative<semantic_ir::SDeclExported>((*decls[index]))) { auto _v_sdeclexported = std::get<semantic_ir::SDeclExported>((*decls[index])); auto [d] = _v_sdeclexported; return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SDeclExtend>((*semantic_ir::sdecl_inner(d)))) { auto _v_sdeclextend = std::get<semantic_ir::SDeclExtend>((*semantic_ir::sdecl_inner(d))); auto [type_name, trait_name, methods] = _v_sdeclextend; return gen_extend_wrapper_protos(type_name, trait_name, methods, context); } return mlc::String(""); }(); } return mlc::String(""); }();
if (wrapper.length() > 0){
{
parts.push_back(wrapper);
}
}
index = index + 1;
}
}
 }();
}
}
int index = 0;
while (index < decls.size()){
{
mlc::String segment = std::visit(overloaded{
  [&](const SDeclType& sdecltype) -> mlc::String { auto [type_name, type_params, variants] = sdecltype; return phase == 0 ? type_gen::gen_type_decl_fwd_only(context, type_name, type_params, variants) : phase == 1 ? type_gen::gen_type_decl_body_only(context, type_name, type_params, variants) : mlc::String(""); },
  [&](const SDeclTrait& sdecltrait) -> mlc::String { auto [name, type_params, methods] = sdecltrait; return phase == 0 ? gen_trait_decl(context, name, type_params, methods) : mlc::String(""); },
  [&](const SDeclFn& sdeclfn) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4, _w5] = sdeclfn; return phase == 2 ? gen_proto(decls[index], context) : phase == 3 ? gen_decl(decls[index], context) : mlc::String(""); },
  [&](const SDeclExtend& sdeclextend) -> mlc::String { auto [_w0, _w1, _w2] = sdeclextend; return phase == 2 ? gen_proto(decls[index], context) : phase == 3 ? gen_decl(decls[index], context) : mlc::String(""); },
  [&](const SDeclImport& sdeclimport) -> mlc::String { auto [_w0, _w1] = sdeclimport; return mlc::String(""); },
  [&](const SDeclExported& sdeclexported) -> mlc::String { auto [d] = sdeclexported; return phase == 0 ? [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SDeclType>((*semantic_ir::sdecl_inner(d)))) { auto _v_sdecltype = std::get<semantic_ir::SDeclType>((*semantic_ir::sdecl_inner(d))); auto [type_name, type_params, variants] = _v_sdecltype; return type_gen::gen_type_decl_fwd_only(context, type_name, type_params, variants); } if (std::holds_alternative<semantic_ir::SDeclTrait>((*semantic_ir::sdecl_inner(d)))) { auto _v_sdecltrait = std::get<semantic_ir::SDeclTrait>((*semantic_ir::sdecl_inner(d))); auto [name, type_params, methods] = _v_sdecltrait; return gen_trait_decl(context, name, type_params, methods); } return mlc::String(""); }() : phase == 1 ? [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SDeclType>((*semantic_ir::sdecl_inner(d)))) { auto _v_sdecltype = std::get<semantic_ir::SDeclType>((*semantic_ir::sdecl_inner(d))); auto [type_name, type_params, variants] = _v_sdecltype; return type_gen::gen_type_decl_body_only(context, type_name, type_params, variants); } return mlc::String(""); }() : phase == 2 ? gen_proto(d, context) : phase == 3 ? [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SDeclType>((*semantic_ir::sdecl_inner(d)))) { auto _v_sdecltype = std::get<semantic_ir::SDeclType>((*semantic_ir::sdecl_inner(d))); auto [_w0, _w1, _w2] = _v_sdecltype; return mlc::String(""); } if (std::holds_alternative<semantic_ir::SDeclTrait>((*semantic_ir::sdecl_inner(d)))) { auto _v_sdecltrait = std::get<semantic_ir::SDeclTrait>((*semantic_ir::sdecl_inner(d))); auto [_w0, _w1, _w2] = _v_sdecltrait; return mlc::String(""); } return gen_decl(d, context); }() : mlc::String(""); }
}, (*decls[index]));
if (segment.length() > 0){
{
parts.push_back(segment);
}
}
index = index + 1;
}
}
return parts;
}

} // namespace decl
