#include "decl_cpp.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "cpp_ast.hpp"
#include "cpp_printer.hpp"
#include "emit_helpers.hpp"
#include "context.hpp"
#include "decl.hpp"
#include "decl_extend.hpp"
#include "type_gen.hpp"
#include "return_body.hpp"

namespace decl_cpp {

using namespace ast;
using namespace semantic_ir;
using namespace registry;
using namespace cpp_ast;
using namespace cpp_printer;
using namespace emit_helpers;
using namespace context;
using namespace decl;
using namespace decl_extend;
using namespace type_gen;
using namespace return_body;
using namespace ast_tokens;

std::shared_ptr<cpp_ast::CppDecl> cpp_decl_from_string_output(mlc::String output) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> cpp_decls_from_string_parts(mlc::Array<mlc::String> parts) noexcept;

mlc::String print_cpp_declaration(std::shared_ptr<cpp_ast::CppDecl> declaration) noexcept;

mlc::String print_cpp_declarations(mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> declarations) noexcept;

context::CodegenContext prototype_context_for_function(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

bool function_has_template_prefix(mlc::Array<mlc::String> type_params) noexcept;

bool function_needs_string_bridge(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept;

std::shared_ptr<cpp_ast::CppDecl> native_fn_proto_cpp(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, context::CodegenContext context) noexcept;

std::shared_ptr<cpp_ast::CppDecl> native_fn_decl_cpp(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SExpr> body, context::CodegenContext context) noexcept;

std::shared_ptr<cpp_ast::CppDecl> gen_fn_proto_cpp(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, context::CodegenContext context) noexcept;

std::shared_ptr<cpp_ast::CppDecl> gen_fn_decl_cpp(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SExpr> body, context::CodegenContext context) noexcept;

std::shared_ptr<cpp_ast::CppDecl> gen_decl_cpp(std::shared_ptr<semantic_ir::SDecl> declaration, context::CodegenContext context) noexcept;

std::shared_ptr<cpp_ast::CppDecl> gen_proto_cpp(std::shared_ptr<semantic_ir::SDecl> declaration, context::CodegenContext context) noexcept;

decl_cpp::DeclPartsBundleCpp collect_all_decl_parts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> declarations, context::CodegenContext context) noexcept;

mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> collect_decl_parts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> declarations, context::CodegenContext context, int phase) noexcept;

std::shared_ptr<cpp_ast::CppDecl> cpp_decl_from_string_output(mlc::String output) noexcept{return emit_helpers::make_fragment_cpp_declaration(output);}

mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> cpp_decls_from_string_parts(mlc::Array<mlc::String> parts) noexcept{return parts.map([](mlc::String part) mutable { return cpp_decl_from_string_output(part); });}

mlc::String print_cpp_declaration(std::shared_ptr<cpp_ast::CppDecl> declaration) noexcept{return cpp_printer::print_decl(declaration);}

mlc::String print_cpp_declarations(mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> declarations) noexcept{return declarations.map([](std::shared_ptr<cpp_ast::CppDecl> declaration) mutable { return print_cpp_declaration(declaration); }).join(mlc::String(""));}

context::CodegenContext prototype_context_for_function(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{return params.size() > 0 && params[0]->name == mlc::String("self") ? [&]() -> context::CodegenContext { if (std::holds_alternative<ast::TyNamed>((*params[0]->typ))) { auto _v_tynamed = std::get<ast::TyNamed>((*params[0]->typ)); auto [type_name] = _v_tynamed; return [&]() -> context::CodegenContext { 
  mlc::String resolved_type = type_name == mlc::String("Self") || type_name == mlc::String("self") ? context.self_type : type_name;
  return context::CodegenContext_for_type_body(context, resolved_type);
 }(); } return context; }() : context;}

bool function_has_template_prefix(mlc::Array<mlc::String> type_params) noexcept{return type_params.size() > 0;}

bool function_needs_string_bridge(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::Param>> params) noexcept{return function_has_template_prefix(type_params) || name == mlc::String("main") && params.size() == 0;}

std::shared_ptr<cpp_ast::CppDecl> native_fn_proto_cpp(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, context::CodegenContext context) noexcept{
context::CodegenContext prototype_context = prototype_context_for_function(context, params);
mlc::String safe_name = context::CodegenContext_resolve(context, name);
mlc::String return_type_cpp = type_gen::sem_type_to_cpp(prototype_context, return_type);
mlc::Array<mlc::String> parameters = decl_extend::gen_parameter_proto_items(prototype_context, params);
return std::make_shared<cpp_ast::CppDecl>(cpp_ast::CppFnProto(return_type_cpp, safe_name, parameters));
}

std::shared_ptr<cpp_ast::CppDecl> native_fn_decl_cpp(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SExpr> body, context::CodegenContext context) noexcept{
context::CodegenContext body_context = decl::compute_fn_body_context(name, params, context);
context::CodegenContext prototype_context = params.size() > 0 && params[0]->name == mlc::String("self") ? body_context : context;
mlc::String safe_name = context::CodegenContext_resolve(context, name);
mlc::String return_type_cpp = type_gen::sem_type_to_cpp(prototype_context, return_type);
mlc::Array<mlc::String> parameters = decl_extend::gen_parameter_def_items(prototype_context, params);
mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> body_statements = return_body::gen_return_body_cpp(body, body_context);
return std::make_shared<cpp_ast::CppDecl>(cpp_ast::CppFnDef(return_type_cpp, safe_name, parameters, body_statements));
}

std::shared_ptr<cpp_ast::CppDecl> gen_fn_proto_cpp(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, context::CodegenContext context) noexcept{return function_needs_string_bridge(name, type_params, params) ? cpp_decl_from_string_output(decl::gen_fn_proto(name, type_params, type_bounds, params, return_type, context)) : native_fn_proto_cpp(name, params, return_type, context);}

std::shared_ptr<cpp_ast::CppDecl> gen_fn_decl_cpp(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SExpr> body, context::CodegenContext context) noexcept{return function_needs_string_bridge(name, type_params, params) ? cpp_decl_from_string_output(decl::gen_fn_decl(name, type_params, type_bounds, params, return_type, body, context)) : native_fn_decl_cpp(name, params, return_type, body, context);}

std::shared_ptr<cpp_ast::CppDecl> gen_decl_cpp(std::shared_ptr<semantic_ir::SDecl> declaration, context::CodegenContext context) noexcept{return [&]() -> std::shared_ptr<cpp_ast::CppDecl> { if (std::holds_alternative<semantic_ir::SDeclFn>((*declaration))) { auto _v_sdeclfn = std::get<semantic_ir::SDeclFn>((*declaration)); auto [name, type_params, type_bounds, params, return_type, body, _w0] = _v_sdeclfn; return [&]() -> std::shared_ptr<cpp_ast::CppDecl> { if (std::holds_alternative<semantic_ir::SExprExtern>((*body)._)) { auto _v_sexprextern = std::get<semantic_ir::SExprExtern>((*body)._); auto [_w0, _w1] = _v_sexprextern; return cpp_decl_from_string_output(mlc::String("")); } return gen_fn_decl_cpp(name, type_params, type_bounds, params, return_type, body, context); }(); } return cpp_decl_from_string_output(decl::gen_decl(declaration, context)); }();}

std::shared_ptr<cpp_ast::CppDecl> gen_proto_cpp(std::shared_ptr<semantic_ir::SDecl> declaration, context::CodegenContext context) noexcept{return [&]() -> std::shared_ptr<cpp_ast::CppDecl> { if (std::holds_alternative<semantic_ir::SDeclFn>((*declaration))) { auto _v_sdeclfn = std::get<semantic_ir::SDeclFn>((*declaration)); auto [name, type_params, type_bounds, params, return_type, body, _w0] = _v_sdeclfn; return [&]() -> std::shared_ptr<cpp_ast::CppDecl> { if (std::holds_alternative<semantic_ir::SExprExtern>((*body)._)) { auto _v_sexprextern = std::get<semantic_ir::SExprExtern>((*body)._); auto [_w0, _w1] = _v_sexprextern; return cpp_decl_from_string_output(mlc::String("")); } return gen_fn_proto_cpp(name, type_params, type_bounds, params, return_type, context); }(); } return cpp_decl_from_string_output(decl::gen_proto(declaration, context)); }();}

decl_cpp::DeclPartsBundleCpp collect_all_decl_parts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> declarations, context::CodegenContext context) noexcept{
decl::DeclPartsBundle string_bundle = decl::collect_all_decl_parts(declarations, context);
return decl_cpp::DeclPartsBundleCpp{cpp_decls_from_string_parts(string_bundle.type_fwds), cpp_decls_from_string_parts(string_bundle.type_defs), cpp_decls_from_string_parts(string_bundle.fn_protos), cpp_decls_from_string_parts(string_bundle.fn_defs)};
}

mlc::Array<std::shared_ptr<cpp_ast::CppDecl>> collect_decl_parts_cpp(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> declarations, context::CodegenContext context, int phase) noexcept{return cpp_decls_from_string_parts(decl::collect_decl_parts(declarations, context, phase));}

} // namespace decl_cpp
