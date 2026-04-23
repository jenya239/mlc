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
using namespace ast_tokens;

context::CodegenContext CodegenContext_for_type_body(context::CodegenContext self, mlc::String type_name) noexcept;

mlc::String gen_fn_proto(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, context::CodegenContext context) noexcept;

context::CodegenContext compute_fn_body_context(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, context::CodegenContext context) noexcept;

mlc::String gen_fn_decl(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SExpr> body, context::CodegenContext context) noexcept;

mlc::String gen_decl(std::shared_ptr<semantic_ir::SDecl> decl, context::CodegenContext context) noexcept;

mlc::String gen_proto(std::shared_ptr<semantic_ir::SDecl> decl, context::CodegenContext context) noexcept;

bool decls_have_main(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> decls) noexcept;

mlc::Array<mlc::String> collect_decl_parts(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> decls, context::CodegenContext context, int phase) noexcept;

context::CodegenContext CodegenContext_for_type_body(context::CodegenContext self, mlc::String type_name) noexcept{return context::make_body_context(self, {}, {}, mlc::HashMap<mlc::String, mlc::String>(), {}, type_name, {}, {});}

mlc::String gen_fn_proto(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, context::CodegenContext context) noexcept{
mlc::String prefix = cpp_naming::template_prefix(type_params) + type_gen::requires_clause(type_params, type_bounds);
mlc::String safe_name = context::context_resolve(context, name);
context::CodegenContext prototype_context = params.size() > 0 && params[0]->name == mlc::String("self") ? [&]() -> context::CodegenContext { if (std::holds_alternative<ast::TyNamed>((*params[0]->typ))) { auto _v_tynamed = std::get<ast::TyNamed>((*params[0]->typ)); auto [type_name] = _v_tynamed; return [&]() -> context::CodegenContext { 
  mlc::String resolved_type = type_name == mlc::String("Self") || type_name == mlc::String("self") ? context.self_type : type_name;
  return CodegenContext_for_type_body(context, resolved_type);
 }(); } return context; }() : context;
return name == mlc::String("main") && params.size() == 0 ? prefix + expr::noexcept_function_prototype(type_gen::sem_type_to_cpp(prototype_context, return_type), safe_name, expr::main_program_parameter_list()) : prefix + expr::noexcept_function_prototype(type_gen::sem_type_to_cpp(prototype_context, return_type), safe_name, decl_extend::gen_params(prototype_context, params));
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
  mlc::String preamble = expr::user_main_arguments_copy_into_runtime_statement();
  return prefix + expr::noexcept_function_body_open(type_gen::sem_type_to_cpp(prototype_context, return_type), safe_name, expr::main_program_parameter_list()) + preamble + return_body::gen_fn_body(body, body_context) + expr::block_close_newline();
 }() : prefix + expr::noexcept_function_body_open(type_gen::sem_type_to_cpp(prototype_context, return_type), safe_name, decl_extend::gen_params(prototype_context, params)) + return_body::gen_fn_body(body, body_context) + expr::block_close_newline();
}

mlc::String gen_decl(std::shared_ptr<semantic_ir::SDecl> decl, context::CodegenContext context) noexcept{return std::visit(overloaded{
  [&](const SDeclType& sdecltype) -> mlc::String { auto [type_name, type_params, variants] = sdecltype; return type_gen::gen_type_decl(context, type_name, type_params, variants); },
  [&](const SDeclTrait& sdecltrait) -> mlc::String { auto [name, type_params, methods] = sdecltrait; return decl_extend::gen_trait_decl(context, name, type_params, methods); },
  [&](const SDeclFn& sdeclfn) -> mlc::String { auto [name, type_params, type_bounds, params, return_type, body] = sdeclfn; return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SExprExtern>((*body)._)) { auto _v_sexprextern = std::get<semantic_ir::SExprExtern>((*body)._); auto [_w0, _w1] = _v_sexprextern; return mlc::String(""); } return gen_fn_decl(name, type_params, type_bounds, params, return_type, body, context); }(); },
  [&](const SDeclExtend& sdeclextend) -> mlc::String { auto [type_name, trait_name, methods] = sdeclextend; return decl_extend::gen_decl_extend(type_name, trait_name, methods, context, [](context::CodegenContext ctx, mlc::String name)  { return context::context_resolve(ctx, name); }, [](std::shared_ptr<semantic_ir::SDecl> decl, context::CodegenContext ctx)  { return gen_decl(decl, ctx); }); },
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
mlc::String wrapper = [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SDeclExtend>((*decls[index]))) { auto _v_sdeclextend = std::get<semantic_ir::SDeclExtend>((*decls[index])); auto [type_name, trait_name, methods] = _v_sdeclextend; return decl_extend::gen_extend_wrapper_protos(type_name, trait_name, methods, context); } if (std::holds_alternative<semantic_ir::SDeclExported>((*decls[index]))) { auto _v_sdeclexported = std::get<semantic_ir::SDeclExported>((*decls[index])); auto [d] = _v_sdeclexported; return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SDeclExtend>((*semantic_ir::sdecl_inner(d)))) { auto _v_sdeclextend = std::get<semantic_ir::SDeclExtend>((*semantic_ir::sdecl_inner(d))); auto [type_name, trait_name, methods] = _v_sdeclextend; return decl_extend::gen_extend_wrapper_protos(type_name, trait_name, methods, context); } return mlc::String(""); }(); } return mlc::String(""); }();
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
  [&](const SDeclTrait& sdecltrait) -> mlc::String { auto [name, type_params, methods] = sdecltrait; return phase == 0 ? decl_extend::gen_trait_decl(context, name, type_params, methods) : mlc::String(""); },
  [&](const SDeclFn& sdeclfn) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4, _w5] = sdeclfn; return phase == 2 ? gen_proto(decls[index], context) : phase == 3 ? gen_decl(decls[index], context) : mlc::String(""); },
  [&](const SDeclExtend& sdeclextend) -> mlc::String { auto [_w0, _w1, _w2] = sdeclextend; return phase == 2 ? gen_proto(decls[index], context) : phase == 3 ? gen_decl(decls[index], context) : mlc::String(""); },
  [&](const SDeclImport& sdeclimport) -> mlc::String { auto [_w0, _w1] = sdeclimport; return mlc::String(""); },
  [&](const SDeclExported& sdeclexported) -> mlc::String { auto [d] = sdeclexported; return phase == 0 ? [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SDeclType>((*semantic_ir::sdecl_inner(d)))) { auto _v_sdecltype = std::get<semantic_ir::SDeclType>((*semantic_ir::sdecl_inner(d))); auto [type_name, type_params, variants] = _v_sdecltype; return type_gen::gen_type_decl_fwd_only(context, type_name, type_params, variants); } if (std::holds_alternative<semantic_ir::SDeclTrait>((*semantic_ir::sdecl_inner(d)))) { auto _v_sdecltrait = std::get<semantic_ir::SDeclTrait>((*semantic_ir::sdecl_inner(d))); auto [name, type_params, methods] = _v_sdecltrait; return decl_extend::gen_trait_decl(context, name, type_params, methods); } return mlc::String(""); }() : phase == 1 ? [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SDeclType>((*semantic_ir::sdecl_inner(d)))) { auto _v_sdecltype = std::get<semantic_ir::SDeclType>((*semantic_ir::sdecl_inner(d))); auto [type_name, type_params, variants] = _v_sdecltype; return type_gen::gen_type_decl_body_only(context, type_name, type_params, variants); } return mlc::String(""); }() : phase == 2 ? gen_proto(d, context) : phase == 3 ? [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SDeclType>((*semantic_ir::sdecl_inner(d)))) { auto _v_sdecltype = std::get<semantic_ir::SDeclType>((*semantic_ir::sdecl_inner(d))); auto [_w0, _w1, _w2] = _v_sdecltype; return mlc::String(""); } if (std::holds_alternative<semantic_ir::SDeclTrait>((*semantic_ir::sdecl_inner(d)))) { auto _v_sdecltrait = std::get<semantic_ir::SDeclTrait>((*semantic_ir::sdecl_inner(d))); auto [_w0, _w1, _w2] = _v_sdecltrait; return mlc::String(""); } return gen_decl(d, context); }() : mlc::String(""); }
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
