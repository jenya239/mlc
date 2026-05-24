#include "decl.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "type_gen.hpp"
#include "return_body.hpp"
#include "expr.hpp"
#include "decl_extend.hpp"
#include "decl_index.hpp"

namespace decl {

using namespace ast;
using namespace semantic_ir;
using namespace registry;
using namespace context;
using namespace cpp_naming;
using namespace type_gen;
using namespace return_body;
using namespace expr;
using namespace decl_extend;
using namespace decl_index;
using namespace ast_tokens;

mlc::String gen_fn_proto(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, context::CodegenContext context) noexcept;

context::CodegenContext compute_fn_body_context(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, context::CodegenContext context) noexcept;

mlc::String gen_fn_decl(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SExpr> body, context::CodegenContext context) noexcept;

mlc::String gen_decl(std::shared_ptr<semantic_ir::SDecl> decl, context::CodegenContext context) noexcept;

mlc::String gen_proto(std::shared_ptr<semantic_ir::SDecl> decl, context::CodegenContext context) noexcept;

bool inner_declaration_is_main(std::shared_ptr<semantic_ir::SDecl> declaration) noexcept;

bool decls_have_main(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> decls) noexcept;

mlc::String extend_cpp_forward_segment(std::shared_ptr<semantic_ir::SDecl> declaration, context::CodegenContext codegen_context) noexcept;

mlc::String decl_segment(std::shared_ptr<semantic_ir::SDecl> declaration, context::CodegenContext codegen_context, int phase) noexcept;

mlc::Array<mlc::String> exported_type_names_from_decls(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> decls) noexcept;

mlc::Array<mlc::String> extend_helper_protos_for_exported_types(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> decls, context::CodegenContext context) noexcept;

mlc::Array<mlc::String> extend_method_forward_segments(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> decls, context::CodegenContext context) noexcept;

decl::DeclPartsBundle collect_all_decl_parts(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> decls, context::CodegenContext context) noexcept;

mlc::Array<mlc::String> collect_decl_parts(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> decls, context::CodegenContext context, int phase) noexcept;

mlc::String gen_fn_proto(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, context::CodegenContext context) noexcept{
mlc::String prefix = cpp_naming::template_prefix(type_params) + type_gen::requires_clause(type_params, type_bounds);
mlc::String safe_name = context::CodegenContext_resolve(context, name);
context::CodegenContext prototype_context = params.size() > 0 && params[0]->name == mlc::String("self") ? [&]() -> context::CodegenContext { if (std::holds_alternative<ast::TyNamed>((*params[0]->typ))) { auto _v_tynamed = std::get<ast::TyNamed>((*params[0]->typ)); auto [type_name] = _v_tynamed; return [&]() -> context::CodegenContext { 
  mlc::String resolved_type = type_name == mlc::String("Self") || type_name == mlc::String("self") ? context.self_type : type_name;
  return context::CodegenContext_for_type_body(context, resolved_type);
 }(); } return context; }() : context;
return name == mlc::String("main") && params.size() == 0 ? prefix + expr::noexcept_function_prototype(type_gen::sem_type_to_cpp(prototype_context, return_type), safe_name, expr::main_program_parameter_list()) : prefix + expr::noexcept_function_prototype(type_gen::sem_type_to_cpp(prototype_context, return_type), safe_name, decl_extend::gen_params_proto(prototype_context, params));
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
mlc::String safe_name = context::CodegenContext_resolve(context, name);
context::CodegenContext body_context = compute_fn_body_context(name, params, context);
context::CodegenContext prototype_context = params.size() > 0 && params[0]->name == mlc::String("self") ? body_context : context;
return name == mlc::String("main") && params.size() == 0 ? [&]() -> mlc::String { 
  mlc::String preamble = expr::user_main_arguments_copy_into_runtime_statement();
  return prefix + expr::noexcept_function_body_open(type_gen::sem_type_to_cpp(prototype_context, return_type), safe_name, expr::main_program_parameter_list()) + preamble + return_body::gen_fn_body(body, body_context) + expr::block_close_newline();
 }() : prefix + expr::noexcept_function_body_open(type_gen::sem_type_to_cpp(prototype_context, return_type), safe_name, decl_extend::gen_params_def(prototype_context, params)) + return_body::gen_fn_body(body, body_context) + expr::block_close_newline();
}

mlc::String gen_decl(std::shared_ptr<semantic_ir::SDecl> decl, context::CodegenContext context) noexcept{return std::visit(overloaded{
  [&](const SDeclType& sdecltype) -> mlc::String { auto [type_name, type_params, variants, derive_traits] = sdecltype; return type_gen::gen_type_decl(context, type_name, type_params, variants) + type_gen::gen_derive_methods(context, type_name, variants, derive_traits); },
  [&](const SDeclTrait& sdecltrait) -> mlc::String { auto [name, type_params, methods] = sdecltrait; return decl_extend::gen_trait_decl(context, name, type_params, methods); },
  [&](const SDeclFn& sdeclfn) -> mlc::String { auto [name, type_params, type_bounds, params, return_type, body, _w0] = sdeclfn; return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprExtern>((*body)._)) { auto _v_sexprextern = std::get<semantic_ir::SExprExtern>((*body)._); auto [_w0, _w1] = _v_sexprextern; return mlc::String(""); } return gen_fn_decl(name, type_params, type_bounds, params, return_type, body, context); }(); },
  [&](const SDeclExtend& sdeclextend) -> mlc::String { auto [type_name, trait_name, methods] = sdeclextend; return decl_extend::gen_decl_extend(type_name, trait_name, methods, context, [](context::CodegenContext ctx, mlc::String name) mutable { return context::CodegenContext_resolve(ctx, name); }, [](std::shared_ptr<semantic_ir::SDecl> decl, context::CodegenContext ctx) mutable { return gen_decl(decl, ctx); }); },
  [&](const SDeclImport& sdeclimport) -> mlc::String { auto [_w0, _w1] = sdeclimport; return mlc::String(""); },
  [&](const SDeclExported& sdeclexported) -> mlc::String { auto [d] = sdeclexported; return gen_decl(d, context); },
  [&](const SDeclAssocBind& sdeclassocbind) -> mlc::String { auto [_w0, _w1, _w2] = sdeclassocbind; return mlc::String(""); }
}, (*decl));}

mlc::String gen_proto(std::shared_ptr<semantic_ir::SDecl> decl, context::CodegenContext context) noexcept{return std::visit(overloaded{
  [&](const SDeclType& sdecltype) -> mlc::String { auto [_w0, _w1, _w2, _w3] = sdecltype; return mlc::String(""); },
  [&](const SDeclTrait& sdecltrait) -> mlc::String { auto [_w0, _w1, _w2] = sdecltrait; return mlc::String(""); },
  [&](const SDeclFn& sdeclfn) -> mlc::String { auto [name, type_params, type_bounds, params, ret, body, _w0] = sdeclfn; return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprExtern>((*body)._)) { auto _v_sexprextern = std::get<semantic_ir::SExprExtern>((*body)._); auto [_w0, _w1] = _v_sexprextern; return mlc::String(""); } return gen_fn_proto(name, type_params, type_bounds, params, ret, context); }(); },
  [&](const SDeclExtend& sdeclextend) -> mlc::String { auto [type_name, _w0, methods] = sdeclextend; return [&]() -> mlc::String { 
  context::CodegenContext extend_context = context::CodegenContext_for_type_body(context, type_name);
  return methods.map([extend_context](std::shared_ptr<semantic_ir::SDecl> method) mutable { return gen_proto(method, extend_context); }).join(mlc::String(""));
 }(); },
  [&](const SDeclImport& sdeclimport) -> mlc::String { auto [_w0, _w1] = sdeclimport; return mlc::String(""); },
  [&](const SDeclExported& sdeclexported) -> mlc::String { auto [d] = sdeclexported; return gen_proto(d, context); },
  [&](const SDeclAssocBind& sdeclassocbind) -> mlc::String { auto [_w0, _w1, _w2] = sdeclassocbind; return mlc::String(""); }
}, (*decl));}

bool inner_declaration_is_main(std::shared_ptr<semantic_ir::SDecl> declaration) noexcept{return [&]() { if (std::holds_alternative<semantic_ir::SDeclFn>((*semantic_ir::sdecl_inner(declaration)))) { auto _v_sdeclfn = std::get<semantic_ir::SDeclFn>((*semantic_ir::sdecl_inner(declaration))); auto [name, _w0, _w1, _w2, _w3, _w4, _w5] = _v_sdeclfn; return name == mlc::String("main"); } return false; }();}

bool decls_have_main(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> decls) noexcept{return decls.any([](std::shared_ptr<semantic_ir::SDecl> declaration) mutable { return inner_declaration_is_main(declaration); });}

mlc::String extend_cpp_forward_segment(std::shared_ptr<semantic_ir::SDecl> declaration, context::CodegenContext codegen_context) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SDeclExtend>((*declaration))) { auto _v_sdeclextend = std::get<semantic_ir::SDeclExtend>((*declaration)); auto [type_name, trait_name, methods] = _v_sdeclextend; return [&]() -> mlc::String { 
  context::CodegenContext extend_context = context::CodegenContext_for_type_body(codegen_context, type_name);
  return decl_extend::gen_extend_wrapper_protos(type_name, trait_name, methods, codegen_context) + methods.map([extend_context](std::shared_ptr<semantic_ir::SDecl> method) mutable { return gen_proto(method, extend_context); }).join(mlc::String(""));
 }(); } if (std::holds_alternative<semantic_ir::SDeclExported>((*declaration))) { auto _v_sdeclexported = std::get<semantic_ir::SDeclExported>((*declaration)); auto [inner_declaration] = _v_sdeclexported; return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SDeclExtend>((*semantic_ir::sdecl_inner(inner_declaration)))) { auto _v_sdeclextend = std::get<semantic_ir::SDeclExtend>((*semantic_ir::sdecl_inner(inner_declaration))); auto [type_name, trait_name, methods] = _v_sdeclextend; return [&]() -> mlc::String { 
  context::CodegenContext extend_context = context::CodegenContext_for_type_body(codegen_context, type_name);
  return decl_extend::gen_extend_wrapper_protos(type_name, trait_name, methods, codegen_context) + methods.map([extend_context](std::shared_ptr<semantic_ir::SDecl> method) mutable { return gen_proto(method, extend_context); }).join(mlc::String(""));
 }(); } return mlc::String(""); }(); } return mlc::String(""); }();}

mlc::String decl_segment(std::shared_ptr<semantic_ir::SDecl> declaration, context::CodegenContext codegen_context, int phase) noexcept{return std::visit(overloaded{
  [&](const SDeclType& sdecltype) -> mlc::String { auto [type_name, type_params, variants, derive_traits] = sdecltype; return phase == 0 ? type_gen::gen_type_decl_fwd_only(codegen_context, type_name, type_params, variants) : phase == 1 ? type_gen::gen_type_decl_body_only(codegen_context, type_name, type_params, variants) + type_gen::gen_derive_methods(codegen_context, type_name, variants, derive_traits) : mlc::String(""); },
  [&](const SDeclTrait& sdecltrait) -> mlc::String { auto [name, type_params, methods] = sdecltrait; return phase == 0 ? decl_extend::gen_trait_decl(codegen_context, name, type_params, methods) : phase == 4 ? decl_extend::gen_trait_struct(codegen_context, name, type_params, methods) : mlc::String(""); },
  [&](const SDeclFn& sdeclfn) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4, _w5, _w6] = sdeclfn; return phase == 2 ? gen_proto(declaration, codegen_context) : phase == 3 ? gen_decl(declaration, codegen_context) : mlc::String(""); },
  [&](const SDeclExtend& sdeclextend) -> mlc::String { auto [_w0, _w1, _w2] = sdeclextend; return phase == 3 ? gen_decl(declaration, codegen_context) : mlc::String(""); },
  [&](const SDeclImport& sdeclimport) -> mlc::String { auto [_w0, _w1] = sdeclimport; return mlc::String(""); },
  [&](const SDeclAssocBind& sdeclassocbind) -> mlc::String { auto [_w0, _w1, _w2] = sdeclassocbind; return mlc::String(""); },
  [&](const SDeclExported& sdeclexported) -> mlc::String { auto [inner_declaration] = sdeclexported; return phase == 0 ? [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SDeclType>((*semantic_ir::sdecl_inner(inner_declaration)))) { auto _v_sdecltype = std::get<semantic_ir::SDeclType>((*semantic_ir::sdecl_inner(inner_declaration))); auto [type_name, type_params, variants, _w0] = _v_sdecltype; return type_gen::gen_type_decl_fwd_only(codegen_context, type_name, type_params, variants); } if (std::holds_alternative<semantic_ir::SDeclTrait>((*semantic_ir::sdecl_inner(inner_declaration)))) { auto _v_sdecltrait = std::get<semantic_ir::SDeclTrait>((*semantic_ir::sdecl_inner(inner_declaration))); auto [name, type_params, methods] = _v_sdecltrait; return decl_extend::gen_trait_decl(codegen_context, name, type_params, methods); } return mlc::String(""); }() : phase == 1 ? [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SDeclType>((*semantic_ir::sdecl_inner(inner_declaration)))) { auto _v_sdecltype = std::get<semantic_ir::SDeclType>((*semantic_ir::sdecl_inner(inner_declaration))); auto [type_name, type_params, variants, derive_traits] = _v_sdecltype; return type_gen::gen_type_decl_body_only(codegen_context, type_name, type_params, variants) + type_gen::gen_derive_methods(codegen_context, type_name, variants, derive_traits); } return mlc::String(""); }() : phase == 2 ? [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SDeclFn>((*semantic_ir::sdecl_inner(inner_declaration)))) { auto _v_sdeclfn = std::get<semantic_ir::SDeclFn>((*semantic_ir::sdecl_inner(inner_declaration))); auto [_w0, type_parameters, _w1, _w2, _w3, body, _w4] = _v_sdeclfn; return type_parameters.size() > 0 ? [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprExtern>((*body)._)) { auto _v_sexprextern = std::get<semantic_ir::SExprExtern>((*body)._); auto [_w0, _w1] = _v_sexprextern; return gen_proto(inner_declaration, codegen_context); } return gen_decl(inner_declaration, codegen_context); }() : gen_proto(inner_declaration, codegen_context); } return gen_proto(inner_declaration, codegen_context); }() : phase == 3 ? [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SDeclType>((*semantic_ir::sdecl_inner(inner_declaration)))) { auto _v_sdecltype = std::get<semantic_ir::SDeclType>((*semantic_ir::sdecl_inner(inner_declaration))); auto [_w0, _w1, _w2, _w3] = _v_sdecltype; return mlc::String(""); } if (std::holds_alternative<semantic_ir::SDeclTrait>((*semantic_ir::sdecl_inner(inner_declaration)))) { auto _v_sdecltrait = std::get<semantic_ir::SDeclTrait>((*semantic_ir::sdecl_inner(inner_declaration))); auto [_w0, _w1, _w2] = _v_sdecltrait; return mlc::String(""); } if (std::holds_alternative<semantic_ir::SDeclFn>((*semantic_ir::sdecl_inner(inner_declaration)))) { auto _v_sdeclfn = std::get<semantic_ir::SDeclFn>((*semantic_ir::sdecl_inner(inner_declaration))); auto [_w0, type_parameters, _w1, _w2, _w3, body, _w4] = _v_sdeclfn; return type_parameters.size() > 0 ? [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprExtern>((*body)._)) { auto _v_sexprextern = std::get<semantic_ir::SExprExtern>((*body)._); auto [_w0, _w1] = _v_sexprextern; return gen_decl(inner_declaration, codegen_context); } return mlc::String(""); }() : gen_decl(inner_declaration, codegen_context); } if (std::holds_alternative<semantic_ir::SDeclExtend>((*semantic_ir::sdecl_inner(inner_declaration)))) { auto _v_sdeclextend = std::get<semantic_ir::SDeclExtend>((*semantic_ir::sdecl_inner(inner_declaration))); auto [_w0, _w1, _w2] = _v_sdeclextend; return gen_decl(inner_declaration, codegen_context); } return gen_decl(inner_declaration, codegen_context); }() : phase == 4 ? [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SDeclTrait>((*semantic_ir::sdecl_inner(inner_declaration)))) { auto _v_sdecltrait = std::get<semantic_ir::SDeclTrait>((*semantic_ir::sdecl_inner(inner_declaration))); auto [name, type_params, methods] = _v_sdecltrait; return decl_extend::gen_trait_struct(codegen_context, name, type_params, methods); } return mlc::String(""); }() : mlc::String(""); }
}, (*declaration));}

mlc::Array<mlc::String> exported_type_names_from_decls(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> decls) noexcept{
mlc::Array<mlc::String> names = {};
int index = 0;
while (index < decls.size()){
{
[&]() -> std::tuple<> { if (std::holds_alternative<semantic_ir::SDeclExported>((*decls[index]))) { auto _v_sdeclexported = std::get<semantic_ir::SDeclExported>((*decls[index])); auto [inner] = _v_sdeclexported; return [&]() -> std::tuple<> { 
  [&]() -> std::tuple<> { if (std::holds_alternative<semantic_ir::SDeclType>((*semantic_ir::sdecl_inner(inner)))) { auto _v_sdecltype = std::get<semantic_ir::SDeclType>((*semantic_ir::sdecl_inner(inner))); auto [type_name, _w0, _w1, _w2] = _v_sdecltype; return [&]() -> std::tuple<> { 
  names.push_back(type_name);
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
index = index + 1;
}
}
return names;
}

mlc::Array<mlc::String> extend_helper_protos_for_exported_types(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> decls, context::CodegenContext context) noexcept{
mlc::Array<mlc::String> exported_types = exported_type_names_from_decls(decls);
mlc::Array<mlc::String> result = {};
int index = 0;
while (index < decls.size()){
{
[&]() -> std::tuple<> { if (std::holds_alternative<semantic_ir::SDeclExtend>((*decls[index]))) { auto _v_sdeclextend = std::get<semantic_ir::SDeclExtend>((*decls[index])); auto [type_name, _w0, methods] = _v_sdeclextend; return [&]() -> std::tuple<> { 
  if (decl_index::list_contains(exported_types, type_name)){
{
int method_index = 0;
[&]() { 
  while (method_index < methods.size()){
{
mlc::String prototype = gen_proto(methods[method_index], context);
if (prototype.length() > 0){
{
result.push_back(prototype);
}
}
method_index = method_index + 1;
}
}
 }();
}
}
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
index = index + 1;
}
}
return result;
}

mlc::Array<mlc::String> extend_method_forward_segments(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> decls, context::CodegenContext context) noexcept{
mlc::Array<mlc::String> result = {};
int index = 0;
while (index < decls.size()){
{
[&]() -> std::tuple<> { if (std::holds_alternative<semantic_ir::SDeclExtend>((*decls[index]))) { auto _v_sdeclextend = std::get<semantic_ir::SDeclExtend>((*decls[index])); auto [_w0, _w1, methods] = _v_sdeclextend; return [&]() -> std::tuple<> { 
  int method_index = 0;
  while (method_index < methods.size()){
{
mlc::String prototype = gen_proto(methods[method_index], context);
if (prototype.length() > 0){
{
result.push_back(prototype);
}
}
method_index = method_index + 1;
}
}
  return std::make_tuple();
 }(); } return std::make_tuple(); }();
index = index + 1;
}
}
return result;
}

decl::DeclPartsBundle collect_all_decl_parts(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> decls, context::CodegenContext context) noexcept{
mlc::Array<mlc::String> type_fwds = {};
mlc::Array<mlc::String> type_defs = {};
mlc::Array<mlc::String> fn_protos = {};
mlc::Array<mlc::String> fn_defs = {};
mlc::Array<mlc::String> extend_forward_segments = extend_method_forward_segments(decls, context);
int forward_index = 0;
while (forward_index < extend_forward_segments.size()){
{
fn_defs.push_back(extend_forward_segments[forward_index]);
forward_index = forward_index + 1;
}
}
int declaration_index = 0;
while (declaration_index < decls.size()){
{
std::shared_ptr<semantic_ir::SDecl> declaration = decls[declaration_index];
mlc::String extend_forward_segment = extend_cpp_forward_segment(declaration, context);
if (extend_forward_segment.length() > 0){
{
fn_defs.push_back(extend_forward_segment);
}
}
mlc::String forward_segment = decl_segment(declaration, context, 0);
if (forward_segment.length() > 0){
{
type_fwds.push_back(forward_segment);
}
}
mlc::String definition_segment = decl_segment(declaration, context, 1);
if (definition_segment.length() > 0){
{
type_defs.push_back(definition_segment);
}
}
mlc::String prototype_segment = decl_segment(declaration, context, 2);
if (prototype_segment.length() > 0){
{
fn_protos.push_back(prototype_segment);
}
}
mlc::String definition_body_segment = decl_segment(declaration, context, 3);
if (definition_body_segment.length() > 0){
{
fn_defs.push_back(definition_body_segment);
}
}
mlc::String trait_struct_segment = decl_segment(declaration, context, 4);
if (trait_struct_segment.length() > 0){
{
fn_defs.push_back(trait_struct_segment);
}
}
declaration_index = declaration_index + 1;
}
}
mlc::Array<mlc::String> extend_helper_prototypes = extend_helper_protos_for_exported_types(decls, context);
int helper_index = 0;
while (helper_index < extend_helper_prototypes.size()){
{
fn_protos.push_back(extend_helper_prototypes[helper_index]);
helper_index = helper_index + 1;
}
}
return decl::DeclPartsBundle{type_fwds, type_defs, fn_protos, fn_defs};
}

mlc::Array<mlc::String> collect_decl_parts(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> decls, context::CodegenContext context, int phase) noexcept{
mlc::Array<mlc::String> extend_forward_segments = phase == 3 ? decls.map([context](std::shared_ptr<semantic_ir::SDecl> declaration) mutable { return extend_cpp_forward_segment(declaration, context); }).filter([](mlc::String segment) mutable { return segment.length() > 0; }) : mlc::Array<mlc::String>{};
return extend_forward_segments.concat(decls.map([context, phase](std::shared_ptr<semantic_ir::SDecl> declaration) mutable { return decl_segment(declaration, context, phase); }).filter([](mlc::String segment) mutable { return segment.length() > 0; }));
}

} // namespace decl
