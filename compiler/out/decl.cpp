#include "decl.hpp"

#include "ast.hpp"
#include "context.hpp"
#include "type_gen.hpp"
#include "eval.hpp"

namespace decl {

using namespace ast;
using namespace context;
using namespace type_gen;
using namespace eval;
using namespace ast_tokens;

context::CodegenContext CodegenContext_for_type_body(context::CodegenContext self, mlc::String type_name) noexcept;

mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept;

mlc::String gen_params(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

mlc::String gen_fn_proto(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::TypeExpr> return_type, context::CodegenContext context) noexcept;

context::CodegenContext compute_fn_body_context(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, context::CodegenContext context) noexcept;

mlc::String gen_fn_decl(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::TypeExpr> return_type, std::shared_ptr<ast::Expr> body, context::CodegenContext context) noexcept;

mlc::String gen_trait_decl(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::Decl>> methods) noexcept;

mlc::String gen_extend_wrapper_protos(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<ast::Decl>> methods, context::CodegenContext context) noexcept;

mlc::String gen_extend_trait_wrapper(mlc::String type_name, mlc::String mangled, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::TypeExpr> ret_type, context::CodegenContext context) noexcept;

mlc::String gen_extend_trait_wrappers(mlc::String type_name, mlc::Array<std::shared_ptr<ast::Decl>> methods, context::CodegenContext context) noexcept;

mlc::String gen_extend_extern_method(mlc::String mangled, mlc::String type_name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::TypeExpr> ret_type, mlc::String trait_name, context::CodegenContext context) noexcept;

mlc::Array<mlc::String> gen_extend_method_parts(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<ast::Decl>> methods, context::CodegenContext extend_context, context::CodegenContext context) noexcept;

mlc::String gen_decl_extend(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<ast::Decl>> methods, context::CodegenContext context) noexcept;

mlc::String gen_decl(std::shared_ptr<ast::Decl> decl, context::CodegenContext context) noexcept;

mlc::String gen_proto(std::shared_ptr<ast::Decl> decl, context::CodegenContext context) noexcept;

bool decls_have_main(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;

mlc::Array<mlc::String> collect_decl_parts(mlc::Array<std::shared_ptr<ast::Decl>> decls, context::CodegenContext context, int phase) noexcept;

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
parts.push_back(type_gen::type_to_cpp(context, params[index]->typ) + mlc::String(" ") + context::cpp_safe(params[index]->name));
index = index + 1;
}
}
return parts.join(mlc::String(", "));
}

mlc::String gen_fn_proto(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::TypeExpr> return_type, context::CodegenContext context) noexcept{
mlc::String prefix = context::template_prefix(type_params) + type_gen::requires_clause(type_params, type_bounds);
mlc::String safe_name = context::context_resolve(context, name);
context::CodegenContext prototype_context = params.size() > 0 && params[0]->name == mlc::String("self") ? [&]() -> context::CodegenContext { if (std::holds_alternative<ast::TyNamed>((*params[0]->typ))) { auto _v_tynamed = std::get<ast::TyNamed>((*params[0]->typ)); auto [type_name] = _v_tynamed; return [&]() -> context::CodegenContext { 
  mlc::String resolved_type = type_name == mlc::String("Self") || type_name == mlc::String("self") ? context.self_type : type_name;
  return CodegenContext_for_type_body(context, resolved_type);
 }(); } return context; }() : context;
return name == mlc::String("main") && params.size() == 0 ? prefix + type_gen::type_to_cpp(prototype_context, return_type) + mlc::String(" ") + safe_name + mlc::String("(int argc, char** argv) noexcept;\n") : prefix + type_gen::type_to_cpp(prototype_context, return_type) + mlc::String(" ") + safe_name + mlc::String("(") + gen_params(prototype_context, params) + mlc::String(") noexcept;\n");
}

context::CodegenContext compute_fn_body_context(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, context::CodegenContext context) noexcept{
mlc::Array<mlc::String> fn_shared_params = context::collect_shared_params(params);
mlc::Array<mlc::String> fn_shared_array_params = context::collect_shared_array_params(params);
mlc::HashMap<mlc::String, mlc::String> fn_array_elem_types = context::collect_array_elem_types(params);
mlc::Array<mlc::String> fn_shared_map_params = context::collect_shared_map_params(params);
mlc::Array<mlc::String> fn_value_params = context::collect_value_params(params);
return params.size() > 0 && params[0]->name == mlc::String("self") ? [&]() -> context::CodegenContext { if (std::holds_alternative<ast::TyNamed>((*params[0]->typ))) { auto _v_tynamed = std::get<ast::TyNamed>((*params[0]->typ)); auto [self_type_name] = _v_tynamed; return [&]() -> context::CodegenContext { 
  mlc::String resolved_type = self_type_name == mlc::String("Self") || self_type_name == mlc::String("self") ? context.self_type : self_type_name;
  return context::make_body_context(context, fn_shared_params, fn_shared_array_params, fn_array_elem_types, fn_shared_map_params, resolved_type, fn_value_params, {});
 }(); } return context::make_body_context(context, fn_shared_params, fn_shared_array_params, fn_array_elem_types, fn_shared_map_params, context.self_type, fn_value_params, {}); }() : context::make_body_context(context, fn_shared_params, fn_shared_array_params, fn_array_elem_types, fn_shared_map_params, context.self_type, fn_value_params, {});
}

mlc::String gen_fn_decl(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::TypeExpr> return_type, std::shared_ptr<ast::Expr> body, context::CodegenContext context) noexcept{
mlc::String prefix = context::template_prefix(type_params) + type_gen::requires_clause(type_params, type_bounds);
mlc::String safe_name = context::context_resolve(context, name);
context::CodegenContext body_context = compute_fn_body_context(name, params, context);
context::CodegenContext prototype_context = params.size() > 0 && params[0]->name == mlc::String("self") ? body_context : context;
return name == mlc::String("main") && params.size() == 0 ? [&]() -> mlc::String { 
  mlc::String preamble = mlc::String("mlc::io::set_args(std::vector<mlc::String>(argv + 1, argv + argc));\n");
  return prefix + type_gen::type_to_cpp(prototype_context, return_type) + mlc::String(" ") + safe_name + mlc::String("(int argc, char** argv) noexcept{\n") + preamble + eval::gen_fn_body(body, body_context) + mlc::String("}\n");
 }() : prefix + type_gen::type_to_cpp(prototype_context, return_type) + mlc::String(" ") + safe_name + mlc::String("(") + gen_params(prototype_context, params) + mlc::String(") noexcept{\n") + eval::gen_fn_body(body, body_context) + mlc::String("}\n");
}

mlc::String gen_trait_decl(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::Decl>> methods) noexcept{
mlc::String self_param = type_params.size() > 0 ? type_params[0] : mlc::String("T");
mlc::String tparam = type_params.size() > 0 ? type_params.join(mlc::String(", ")) : mlc::String("T");
mlc::String prefix = mlc::String("template<typename ") + tparam + mlc::String(">\n");
mlc::Array<mlc::String> req_parts = {};
int method_index = 0;
while (method_index < methods.size()){
{
[&]() -> void { if (std::holds_alternative<ast::DeclFn>((*methods[method_index]))) { auto _v_declfn = std::get<ast::DeclFn>((*methods[method_index])); auto [mangled, _w0, _w1, params, ret_type, _w2] = _v_declfn; return [&]() { 
  mlc::String method_name = extract_method_name(mangled, trait_name);
  mlc::String ret_cpp = type_gen::type_to_cpp(context, ret_type);
  return req_parts.push_back(mlc::String("{ ") + method_name + mlc::String("(self) } -> std::convertible_to<") + ret_cpp + mlc::String(">"));
 }(); } return; }();
method_index = method_index + 1;
}
}
mlc::String req_body = req_parts.join(mlc::String("; "));
return prefix + mlc::String("concept ") + context::cpp_safe(trait_name) + mlc::String(" = requires(const ") + self_param + mlc::String("& self) { ") + req_body + mlc::String("; };\n");
}

mlc::String gen_extend_wrapper_protos(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<ast::Decl>> methods, context::CodegenContext context) noexcept{return trait_name.length() == 0 ? mlc::String("") : [&]() -> mlc::String { 
  mlc::String wrappers = mlc::String("");
  int method_index = 0;
  while (method_index < methods.size()){
{
std::visit(overloaded{
  [&](const DeclFn& declfn) {
auto [mangled, _w0, _w1, params, ret_type, _w2] = declfn;
{
context::CodegenContext wrapper_context = CodegenContext_for_type_body(context, type_name);
mlc::String method_name = extract_method_name(mangled, type_name);
mlc::String ret_cpp = type_gen::type_to_cpp(wrapper_context, ret_type);
mlc::String params_str = gen_params(wrapper_context, params);
wrappers = wrappers + ret_cpp + mlc::String(" ") + context::cpp_safe(method_name) + mlc::String("(") + params_str + mlc::String(") noexcept;\n");
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

mlc::String gen_extend_trait_wrapper(mlc::String type_name, mlc::String mangled, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::TypeExpr> ret_type, context::CodegenContext context) noexcept{
context::CodegenContext wrapper_context = CodegenContext_for_type_body(context, type_name);
mlc::String method_name = extract_method_name(mangled, type_name);
mlc::String ret_cpp = type_gen::type_to_cpp(wrapper_context, ret_type);
mlc::String fn_resolved = context::context_resolve(context, mangled);
mlc::String params_str = gen_params(wrapper_context, params);
mlc::Array<mlc::String> call_args = {};
int param_index = 0;
while (param_index < params.size()){
{
call_args.push_back(context::cpp_safe(params[param_index]->name));
param_index = param_index + 1;
}
}
return mlc::String("inline ") + ret_cpp + mlc::String(" ") + context::cpp_safe(method_name) + mlc::String("(") + params_str + mlc::String(") noexcept { return ") + fn_resolved + mlc::String("(") + call_args.join(mlc::String(", ")) + mlc::String("); }\n");
}

mlc::String gen_extend_trait_wrappers(mlc::String type_name, mlc::Array<std::shared_ptr<ast::Decl>> methods, context::CodegenContext context) noexcept{
mlc::String wrappers = mlc::String("");
int method_index = 0;
while (method_index < methods.size()){
{
std::visit(overloaded{
  [&](const DeclFn& declfn) {
auto [mangled, _w0, _w1, params, ret_type, _w2] = declfn;
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

mlc::String gen_extend_extern_method(mlc::String mangled, mlc::String type_name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::TypeExpr> ret_type, mlc::String trait_name, context::CodegenContext context) noexcept{
mlc::String method_name = trait_name.length() > 0 ? extract_method_name(mangled, type_name) : mlc::String("");
return type_name == mlc::String("i32") && method_name == mlc::String("to_string") && params.size() > 0 ? [&]() -> mlc::String { 
  mlc::String ret_cpp = type_gen::type_to_cpp(context, ret_type);
  mlc::String params_str = gen_params(context, params);
  return ret_cpp + mlc::String(" ") + context::context_resolve(context, mangled) + mlc::String("(") + params_str + mlc::String(") noexcept { return mlc::to_string(") + context::cpp_safe(params[0]->name) + mlc::String("); }\n");
 }() : mlc::String("");
}

mlc::Array<mlc::String> gen_extend_method_parts(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<ast::Decl>> methods, context::CodegenContext extend_context, context::CodegenContext context) noexcept{
mlc::Array<mlc::String> parts = {};
int method_index = 0;
while (method_index < methods.size()){
{
[&]() -> void { if (std::holds_alternative<ast::DeclFn>((*methods[method_index]))) { auto _v_declfn = std::get<ast::DeclFn>((*methods[method_index])); auto [mangled, _w0, _w1, params, ret_type, body] = _v_declfn; return [&]() -> void { if (std::holds_alternative<ast::ExprExtern>((*body)._)) { auto _v_exprextern = std::get<ast::ExprExtern>((*body)._); auto [_w0] = _v_exprextern; return [&]() { 
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

mlc::String gen_decl_extend(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<ast::Decl>> methods, context::CodegenContext context) noexcept{
context::CodegenContext extend_context = CodegenContext_for_type_body(context, type_name);
mlc::Array<mlc::String> method_parts = gen_extend_method_parts(type_name, trait_name, methods, extend_context, context);
mlc::String methods_str = method_parts.join(mlc::String(""));
return trait_name.length() > 0 ? [&]() -> mlc::String { 
  mlc::String trait_wrappers = gen_extend_trait_wrappers(type_name, methods, context);
  mlc::String cpp_type = type_gen::type_name_to_cpp(context, type_name);
  mlc::String trait_safe = context::cpp_safe(trait_name);
  return methods_str + trait_wrappers + mlc::String("static_assert(") + trait_safe + mlc::String("<") + cpp_type + mlc::String(">, \"") + type_name + mlc::String(" does not implement ") + trait_name + mlc::String("\");\n");
 }() : methods_str;
}

mlc::String gen_decl(std::shared_ptr<ast::Decl> decl, context::CodegenContext context) noexcept{return std::visit(overloaded{
  [&](const DeclType& decltype_) -> mlc::String { auto [type_name, type_params, variants] = decltype_; return type_gen::gen_type_decl(context, type_name, type_params, variants); },
  [&](const DeclTrait& decltrait) -> mlc::String { auto [name, type_params, methods] = decltrait; return gen_trait_decl(context, name, type_params, methods); },
  [&](const DeclFn& declfn) -> mlc::String { auto [name, type_params, type_bounds, params, return_type, body] = declfn; return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprExtern>((*body)._)) { auto _v_exprextern = std::get<ast::ExprExtern>((*body)._); auto [_w0] = _v_exprextern; return mlc::String(""); } return gen_fn_decl(name, type_params, type_bounds, params, return_type, body, context); }(); },
  [&](const DeclExtend& declextend) -> mlc::String { auto [type_name, trait_name, methods] = declextend; return gen_decl_extend(type_name, trait_name, methods, context); },
  [&](const DeclImport& declimport) -> mlc::String { auto [_w0, _w1] = declimport; return mlc::String(""); },
  [&](const DeclExported& declexported) -> mlc::String { auto [d] = declexported; return gen_decl(d, context); }
}, (*decl));}

mlc::String gen_proto(std::shared_ptr<ast::Decl> decl, context::CodegenContext context) noexcept{return std::visit(overloaded{
  [&](const DeclType& decltype_) -> mlc::String { auto [_w0, _w1, _w2] = decltype_; return mlc::String(""); },
  [&](const DeclTrait& decltrait) -> mlc::String { auto [_w0, _w1, _w2] = decltrait; return mlc::String(""); },
  [&](const DeclFn& declfn) -> mlc::String { auto [name, type_params, type_bounds, params, ret, body] = declfn; return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprExtern>((*body)._)) { auto _v_exprextern = std::get<ast::ExprExtern>((*body)._); auto [_w0] = _v_exprextern; return mlc::String(""); } return gen_fn_proto(name, type_params, type_bounds, params, ret, context); }(); },
  [&](const DeclExtend& declextend) -> mlc::String { auto [type_name, _w0, methods] = declextend; return [&]() -> mlc::String { 
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
  [&](const DeclImport& declimport) -> mlc::String { auto [_w0, _w1] = declimport; return mlc::String(""); },
  [&](const DeclExported& declexported) -> mlc::String { auto [d] = declexported; return gen_proto(d, context); }
}, (*decl));}

bool decls_have_main(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept{
bool found = false;
int index = 0;
while (index < decls.size()){
{
std::shared_ptr<ast::Decl> inner = ast::decl_inner(decls[index]);
bool is_main = [&]() { if (std::holds_alternative<ast::DeclFn>((*inner))) { auto _v_declfn = std::get<ast::DeclFn>((*inner)); auto [name, _w0, _w1, _w2, _w3, _w4] = _v_declfn; return name == mlc::String("main"); } return false; }();
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

mlc::Array<mlc::String> collect_decl_parts(mlc::Array<std::shared_ptr<ast::Decl>> decls, context::CodegenContext context, int phase) noexcept{
mlc::Array<mlc::String> parts = {};
if (phase == 0){
{
int index = 0;
[&]() { 
  while (index < decls.size()){
{
mlc::String wrapper = [&]() -> mlc::String { if (std::holds_alternative<ast::DeclExtend>((*decls[index]))) { auto _v_declextend = std::get<ast::DeclExtend>((*decls[index])); auto [type_name, trait_name, methods] = _v_declextend; return gen_extend_wrapper_protos(type_name, trait_name, methods, context); } if (std::holds_alternative<ast::DeclExported>((*decls[index]))) { auto _v_declexported = std::get<ast::DeclExported>((*decls[index])); auto [d] = _v_declexported; return [&]() -> mlc::String { if (std::holds_alternative<ast::DeclExtend>((*ast::decl_inner(d)))) { auto _v_declextend = std::get<ast::DeclExtend>((*ast::decl_inner(d))); auto [type_name, trait_name, methods] = _v_declextend; return gen_extend_wrapper_protos(type_name, trait_name, methods, context); } return mlc::String(""); }(); } return mlc::String(""); }();
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
  [&](const DeclType& decltype_) -> mlc::String { auto [type_name, type_params, variants] = decltype_; return phase == 0 ? type_gen::gen_type_decl_fwd_only(context, type_name, type_params, variants) : phase == 1 ? type_gen::gen_type_decl_body_only(context, type_name, type_params, variants) : mlc::String(""); },
  [&](const DeclTrait& decltrait) -> mlc::String { auto [name, type_params, methods] = decltrait; return phase == 0 ? gen_trait_decl(context, name, type_params, methods) : mlc::String(""); },
  [&](const DeclFn& declfn) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4, _w5] = declfn; return phase == 2 ? gen_proto(decls[index], context) : phase == 3 ? gen_decl(decls[index], context) : mlc::String(""); },
  [&](const DeclExtend& declextend) -> mlc::String { auto [_w0, _w1, _w2] = declextend; return phase == 2 ? gen_proto(decls[index], context) : phase == 3 ? gen_decl(decls[index], context) : mlc::String(""); },
  [&](const DeclImport& declimport) -> mlc::String { auto [_w0, _w1] = declimport; return mlc::String(""); },
  [&](const DeclExported& declexported) -> mlc::String { auto [d] = declexported; return phase == 0 ? [&]() -> mlc::String { if (std::holds_alternative<ast::DeclType>((*ast::decl_inner(d)))) { auto _v_decltype = std::get<ast::DeclType>((*ast::decl_inner(d))); auto [type_name, type_params, variants] = _v_decltype; return type_gen::gen_type_decl_fwd_only(context, type_name, type_params, variants); } if (std::holds_alternative<ast::DeclTrait>((*ast::decl_inner(d)))) { auto _v_decltrait = std::get<ast::DeclTrait>((*ast::decl_inner(d))); auto [name, type_params, methods] = _v_decltrait; return gen_trait_decl(context, name, type_params, methods); } return mlc::String(""); }() : phase == 1 ? [&]() -> mlc::String { if (std::holds_alternative<ast::DeclType>((*ast::decl_inner(d)))) { auto _v_decltype = std::get<ast::DeclType>((*ast::decl_inner(d))); auto [type_name, type_params, variants] = _v_decltype; return type_gen::gen_type_decl_body_only(context, type_name, type_params, variants); } return mlc::String(""); }() : phase == 2 ? gen_proto(d, context) : phase == 3 ? [&]() -> mlc::String { if (std::holds_alternative<ast::DeclType>((*ast::decl_inner(d)))) { auto _v_decltype = std::get<ast::DeclType>((*ast::decl_inner(d))); auto [_w0, _w1, _w2] = _v_decltype; return mlc::String(""); } if (std::holds_alternative<ast::DeclTrait>((*ast::decl_inner(d)))) { auto _v_decltrait = std::get<ast::DeclTrait>((*ast::decl_inner(d))); auto [_w0, _w1, _w2] = _v_decltrait; return mlc::String(""); } return gen_decl(d, context); }() : mlc::String(""); }
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
