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

mlc::String gen_fn_proto(mlc::String name, mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type, context::CodegenContext context) noexcept;

context::CodegenContext compute_fn_body_context(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> parameters, context::CodegenContext context) noexcept;

mlc::String gen_fn_decl(mlc::String name, mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SemanticExpression> body, context::CodegenContext context) noexcept;

mlc::String CodegenContext_gen_decl(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;

mlc::String CodegenContext_gen_proto(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;

mlc::String gen_decl(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context) noexcept;

mlc::String gen_proto(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context) noexcept;

bool inner_declaration_is_main(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;

bool decls_have_main(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations) noexcept;

mlc::String extend_cpp_forward_segment(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext codegen_context) noexcept;

mlc::String decl_segment(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext codegen_context, int phase) noexcept;

mlc::Array<mlc::String> exported_type_names_from_decls(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations) noexcept;

mlc::Array<mlc::String> extend_method_prototypes(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context) noexcept;

mlc::Array<mlc::String> extend_helper_prototypes_for_declaration(mlc::Array<mlc::String> exported_types, context::CodegenContext context, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;

mlc::Array<mlc::String> extend_forward_prototypes_for_declaration(context::CodegenContext context, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;

mlc::Array<mlc::String> extend_helper_protos_for_exported_types(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept;

mlc::Array<mlc::String> extend_method_forward_segments(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept;

decl::DeclPartsBundle collect_all_decl_parts(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept;

mlc::Array<mlc::String> collect_decl_parts(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context, int phase) noexcept;

mlc::String gen_fn_proto(mlc::String name, mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type, context::CodegenContext context) noexcept{
mlc::String prefix = cpp_naming::template_prefix(type_parameters) + type_gen::requires_clause(type_parameters, type_bounds);
mlc::String safe_name = context::CodegenContext_resolve(context, name);
context::CodegenContext prototype_context = parameters.size() > 0 && parameters[0]->name == mlc::String("self") ? [&]() -> context::CodegenContext { if (std::holds_alternative<ast::TyNamed>((*parameters[0]->type_value))) { auto _v_tynamed = std::get<ast::TyNamed>((*parameters[0]->type_value)); auto [type_name] = _v_tynamed; return [&]() -> context::CodegenContext { 
  mlc::String resolved_type = type_name == mlc::String("Self") || type_name == mlc::String("self") ? context.self_type : type_name;
  return context::CodegenContext_for_type_body(context, resolved_type);
 }(); } return context; }() : context;
return name == mlc::String("main") && parameters.size() == 0 ? prefix + expr::noexcept_function_prototype(type_gen::sem_type_to_cpp(prototype_context, return_type), safe_name, expr::main_program_parameter_list()) : prefix + expr::noexcept_function_prototype(type_gen::sem_type_to_cpp(prototype_context, return_type), safe_name, decl_extend::gen_params_proto(prototype_context, parameters));
}

context::CodegenContext compute_fn_body_context(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> parameters, context::CodegenContext context) noexcept{
mlc::Array<mlc::String> fn_shared_params = param_analysis::collect_shared_params(parameters);
mlc::Array<mlc::String> fn_shared_array_params = param_analysis::collect_shared_array_params(parameters);
mlc::HashMap<mlc::String, mlc::String> fn_array_elem_types = param_analysis::collect_array_elem_types(parameters);
mlc::Array<mlc::String> fn_shared_map_params = param_analysis::collect_shared_map_params(parameters);
mlc::Array<mlc::String> fn_value_params = param_analysis::collect_value_params(parameters);
return parameters.size() > 0 && parameters[0]->name == mlc::String("self") ? [&]() -> context::CodegenContext { if (std::holds_alternative<ast::TyNamed>((*parameters[0]->type_value))) { auto _v_tynamed = std::get<ast::TyNamed>((*parameters[0]->type_value)); auto [self_type_name] = _v_tynamed; return [&]() -> context::CodegenContext { 
  mlc::String resolved_type = self_type_name == mlc::String("Self") || self_type_name == mlc::String("self") ? context.self_type : self_type_name;
  return context::make_body_context(context, fn_shared_params, fn_shared_array_params, fn_array_elem_types, fn_shared_map_params, resolved_type, fn_value_params, {});
 }(); } return context::make_body_context(context, fn_shared_params, fn_shared_array_params, fn_array_elem_types, fn_shared_map_params, context.self_type, fn_value_params, {}); }() : context::make_body_context(context, fn_shared_params, fn_shared_array_params, fn_array_elem_types, fn_shared_map_params, context.self_type, fn_value_params, {});
}

mlc::String gen_fn_decl(mlc::String name, mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SemanticExpression> body, context::CodegenContext context) noexcept{
mlc::String prefix = cpp_naming::template_prefix(type_parameters) + type_gen::requires_clause(type_parameters, type_bounds);
mlc::String safe_name = context::CodegenContext_resolve(context, name);
context::CodegenContext body_context = compute_fn_body_context(name, parameters, context);
context::CodegenContext prototype_context = parameters.size() > 0 && parameters[0]->name == mlc::String("self") ? body_context : context;
return name == mlc::String("main") && parameters.size() == 0 ? [&]() -> mlc::String { 
  mlc::String preamble = expr::user_main_arguments_copy_into_runtime_statement();
  return prefix + expr::noexcept_function_body_open(type_gen::sem_type_to_cpp(prototype_context, return_type), safe_name, expr::main_program_parameter_list()) + preamble + return_body::gen_fn_body(body, body_context) + expr::block_close_newline();
 }() : prefix + expr::noexcept_function_body_open(type_gen::sem_type_to_cpp(prototype_context, return_type), safe_name, decl_extend::gen_params_def(prototype_context, parameters)) + return_body::gen_fn_body(body, body_context) + expr::block_close_newline();
}

mlc::String CodegenContext_gen_decl(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{return std::visit(overloaded{
  [&](const SemanticDeclarationType& semanticdeclarationtype) -> mlc::String { auto [type_name, type_parameters, variants, derive_traits, _w0] = semanticdeclarationtype; return type_gen::gen_type_decl(self, type_name, type_parameters, variants) + type_gen::gen_derive_methods(self, type_name, variants, derive_traits); },
  [&](const SemanticDeclarationTypeAlias& semanticdeclarationtypealias) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticdeclarationtypealias; return mlc::String(""); },
  [&](const SemanticDeclarationTrait& semanticdeclarationtrait) -> mlc::String { auto [name, type_parameters, methods, _w0] = semanticdeclarationtrait; return decl_extend::gen_trait_decl(self, name, type_parameters, methods); },
  [&](const SemanticDeclarationFn& semanticdeclarationfn) -> mlc::String { auto [name, type_parameters, type_bounds, parameters, return_type, body, _w0, _w1] = semanticdeclarationfn; return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticExpressionExtern>((*body)._)) { auto _v_semanticexpressionextern = std::get<semantic_ir::SemanticExpressionExtern>((*body)._); auto [_w0, _w1] = _v_semanticexpressionextern; return mlc::String(""); } return gen_fn_decl(name, type_parameters, type_bounds, parameters, return_type, body, self); }(); },
  [&](const SemanticDeclarationExtend& semanticdeclarationextend) -> mlc::String { auto [type_name, trait_name, methods, _w0] = semanticdeclarationextend; return decl_extend::gen_decl_extend(type_name, trait_name, methods, self, [](context::CodegenContext codegen_context, mlc::String name) mutable { return context::CodegenContext_resolve(codegen_context, name); }, [](std::shared_ptr<semantic_ir::SemanticDeclaration> inner_declaration, context::CodegenContext codegen_context) mutable { return CodegenContext_gen_decl(codegen_context, inner_declaration); }); },
  [&](const SemanticDeclarationImport& semanticdeclarationimport) -> mlc::String { auto [_w0, _w1] = semanticdeclarationimport; return mlc::String(""); },
  [&](const SemanticDeclarationExported& semanticdeclarationexported) -> mlc::String { auto [inner_declaration] = semanticdeclarationexported; return CodegenContext_gen_decl(self, inner_declaration); },
  [&](const SemanticDeclarationAssocBind& semanticdeclarationassocbind) -> mlc::String { auto [_w0, _w1, _w2] = semanticdeclarationassocbind; return mlc::String(""); }
}, (*declaration));}

mlc::String CodegenContext_gen_proto(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{return std::visit(overloaded{
  [&](const SemanticDeclarationType& semanticdeclarationtype) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4] = semanticdeclarationtype; return mlc::String(""); },
  [&](const SemanticDeclarationTypeAlias& semanticdeclarationtypealias) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticdeclarationtypealias; return mlc::String(""); },
  [&](const SemanticDeclarationTrait& semanticdeclarationtrait) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticdeclarationtrait; return mlc::String(""); },
  [&](const SemanticDeclarationFn& semanticdeclarationfn) -> mlc::String { auto [name, type_parameters, type_bounds, parameters, return_type, body, _w0, _w1] = semanticdeclarationfn; return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticExpressionExtern>((*body)._)) { auto _v_semanticexpressionextern = std::get<semantic_ir::SemanticExpressionExtern>((*body)._); auto [_w0, _w1] = _v_semanticexpressionextern; return mlc::String(""); } return gen_fn_proto(name, type_parameters, type_bounds, parameters, return_type, self); }(); },
  [&](const SemanticDeclarationExtend& semanticdeclarationextend) -> mlc::String { auto [type_name, _w0, methods, _w1] = semanticdeclarationextend; return [&]() -> mlc::String { 
  context::CodegenContext extend_context = context::CodegenContext_for_type_body(self, type_name);
  return methods.map([extend_context](std::shared_ptr<semantic_ir::SemanticDeclaration> method) mutable { return CodegenContext_gen_proto(extend_context, method); }).join(mlc::String(""));
 }(); },
  [&](const SemanticDeclarationImport& semanticdeclarationimport) -> mlc::String { auto [_w0, _w1] = semanticdeclarationimport; return mlc::String(""); },
  [&](const SemanticDeclarationExported& semanticdeclarationexported) -> mlc::String { auto [inner_declaration] = semanticdeclarationexported; return CodegenContext_gen_proto(self, inner_declaration); },
  [&](const SemanticDeclarationAssocBind& semanticdeclarationassocbind) -> mlc::String { auto [_w0, _w1, _w2] = semanticdeclarationassocbind; return mlc::String(""); }
}, (*declaration));}

mlc::String gen_decl(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context) noexcept{return CodegenContext_gen_decl(context, declaration);}

mlc::String gen_proto(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context) noexcept{return CodegenContext_gen_proto(context, declaration);}

bool inner_declaration_is_main(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{return [&]() { if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*semantic_ir::sdecl_inner(declaration)))) { auto _v_semanticdeclarationfn = std::get<semantic_ir::SemanticDeclarationFn>((*semantic_ir::sdecl_inner(declaration))); auto [name, _w0, _w1, _w2, _w3, _w4, _w5, _w6] = _v_semanticdeclarationfn; return name == mlc::String("main"); } return false; }();}

bool decls_have_main(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations) noexcept{return declarations.any([](std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) mutable { return inner_declaration_is_main(declaration); });}

mlc::String extend_cpp_forward_segment(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext codegen_context) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticDeclarationExtend>((*declaration))) { auto _v_semanticdeclarationextend = std::get<semantic_ir::SemanticDeclarationExtend>((*declaration)); auto [type_name, trait_name, methods, _w0] = _v_semanticdeclarationextend; return [&]() -> mlc::String { 
  context::CodegenContext extend_context = context::CodegenContext_for_type_body(codegen_context, type_name);
  return decl_extend::gen_extend_wrapper_protos(type_name, trait_name, methods, codegen_context) + methods.map([extend_context](std::shared_ptr<semantic_ir::SemanticDeclaration> method) mutable { return gen_proto(method, extend_context); }).join(mlc::String(""));
 }(); } if (std::holds_alternative<semantic_ir::SemanticDeclarationExported>((*declaration))) { auto _v_semanticdeclarationexported = std::get<semantic_ir::SemanticDeclarationExported>((*declaration)); auto [inner_declaration] = _v_semanticdeclarationexported; return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticDeclarationExtend>((*semantic_ir::sdecl_inner(inner_declaration)))) { auto _v_semanticdeclarationextend = std::get<semantic_ir::SemanticDeclarationExtend>((*semantic_ir::sdecl_inner(inner_declaration))); auto [type_name, trait_name, methods, _w0] = _v_semanticdeclarationextend; return [&]() -> mlc::String { 
  context::CodegenContext extend_context = context::CodegenContext_for_type_body(codegen_context, type_name);
  return decl_extend::gen_extend_wrapper_protos(type_name, trait_name, methods, codegen_context) + methods.map([extend_context](std::shared_ptr<semantic_ir::SemanticDeclaration> method) mutable { return gen_proto(method, extend_context); }).join(mlc::String(""));
 }(); } return mlc::String(""); }(); } return mlc::String(""); }();}

mlc::String decl_segment(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext codegen_context, int phase) noexcept{return std::visit(overloaded{
  [&](const SemanticDeclarationType& semanticdeclarationtype) -> mlc::String { auto [type_name, type_parameters, variants, derive_traits, _w0] = semanticdeclarationtype; return phase == 0 ? type_gen::gen_type_decl_fwd_only(codegen_context, type_name, type_parameters, variants) : phase == 1 ? type_gen::gen_type_decl_body_only(codegen_context, type_name, type_parameters, variants) + type_gen::gen_derive_methods(codegen_context, type_name, variants, derive_traits) : mlc::String(""); },
  [&](const SemanticDeclarationTypeAlias& semanticdeclarationtypealias) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticdeclarationtypealias; return mlc::String(""); },
  [&](const SemanticDeclarationTrait& semanticdeclarationtrait) -> mlc::String { auto [name, type_parameters, methods, _w0] = semanticdeclarationtrait; return phase == 0 ? decl_extend::gen_trait_decl(codegen_context, name, type_parameters, methods) : phase == 4 ? decl_extend::gen_trait_struct(codegen_context, name, type_parameters, methods) : mlc::String(""); },
  [&](const SemanticDeclarationFn& semanticdeclarationfn) -> mlc::String { auto [_w0, _w1, _w2, _w3, _w4, _w5, _w6, _w7] = semanticdeclarationfn; return phase == 2 ? gen_proto(declaration, codegen_context) : phase == 3 ? gen_decl(declaration, codegen_context) : mlc::String(""); },
  [&](const SemanticDeclarationExtend& semanticdeclarationextend) -> mlc::String { auto [_w0, _w1, _w2, _w3] = semanticdeclarationextend; return phase == 3 ? gen_decl(declaration, codegen_context) : mlc::String(""); },
  [&](const SemanticDeclarationImport& semanticdeclarationimport) -> mlc::String { auto [_w0, _w1] = semanticdeclarationimport; return mlc::String(""); },
  [&](const SemanticDeclarationAssocBind& semanticdeclarationassocbind) -> mlc::String { auto [_w0, _w1, _w2] = semanticdeclarationassocbind; return mlc::String(""); },
  [&](const SemanticDeclarationExported& semanticdeclarationexported) -> mlc::String { auto [inner_declaration] = semanticdeclarationexported; return phase == 0 ? [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticDeclarationType>((*semantic_ir::sdecl_inner(inner_declaration)))) { auto _v_semanticdeclarationtype = std::get<semantic_ir::SemanticDeclarationType>((*semantic_ir::sdecl_inner(inner_declaration))); auto [type_name, type_parameters, variants, _w0, _w1] = _v_semanticdeclarationtype; return type_gen::gen_type_decl_fwd_only(codegen_context, type_name, type_parameters, variants); } if (std::holds_alternative<semantic_ir::SemanticDeclarationTrait>((*semantic_ir::sdecl_inner(inner_declaration)))) { auto _v_semanticdeclarationtrait = std::get<semantic_ir::SemanticDeclarationTrait>((*semantic_ir::sdecl_inner(inner_declaration))); auto [name, type_parameters, methods, _w0] = _v_semanticdeclarationtrait; return decl_extend::gen_trait_decl(codegen_context, name, type_parameters, methods); } return mlc::String(""); }() : phase == 1 ? [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticDeclarationType>((*semantic_ir::sdecl_inner(inner_declaration)))) { auto _v_semanticdeclarationtype = std::get<semantic_ir::SemanticDeclarationType>((*semantic_ir::sdecl_inner(inner_declaration))); auto [type_name, type_parameters, variants, derive_traits, _w0] = _v_semanticdeclarationtype; return type_gen::gen_type_decl_body_only(codegen_context, type_name, type_parameters, variants) + type_gen::gen_derive_methods(codegen_context, type_name, variants, derive_traits); } return mlc::String(""); }() : phase == 2 ? [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*semantic_ir::sdecl_inner(inner_declaration)))) { auto _v_semanticdeclarationfn = std::get<semantic_ir::SemanticDeclarationFn>((*semantic_ir::sdecl_inner(inner_declaration))); auto [_w0, type_parameters, _w1, _w2, _w3, body, _w4, _w5] = _v_semanticdeclarationfn; return type_parameters.size() > 0 ? [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticExpressionExtern>((*body)._)) { auto _v_semanticexpressionextern = std::get<semantic_ir::SemanticExpressionExtern>((*body)._); auto [_w0, _w1] = _v_semanticexpressionextern; return gen_proto(inner_declaration, codegen_context); } return gen_decl(inner_declaration, codegen_context); }() : gen_proto(inner_declaration, codegen_context); } return gen_proto(inner_declaration, codegen_context); }() : phase == 3 ? [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticDeclarationType>((*semantic_ir::sdecl_inner(inner_declaration)))) { auto _v_semanticdeclarationtype = std::get<semantic_ir::SemanticDeclarationType>((*semantic_ir::sdecl_inner(inner_declaration))); auto [_w0, _w1, _w2, _w3, _w4] = _v_semanticdeclarationtype; return mlc::String(""); } if (std::holds_alternative<semantic_ir::SemanticDeclarationTrait>((*semantic_ir::sdecl_inner(inner_declaration)))) { auto _v_semanticdeclarationtrait = std::get<semantic_ir::SemanticDeclarationTrait>((*semantic_ir::sdecl_inner(inner_declaration))); auto [_w0, _w1, _w2, _w3] = _v_semanticdeclarationtrait; return mlc::String(""); } if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*semantic_ir::sdecl_inner(inner_declaration)))) { auto _v_semanticdeclarationfn = std::get<semantic_ir::SemanticDeclarationFn>((*semantic_ir::sdecl_inner(inner_declaration))); auto [_w0, type_parameters, _w1, _w2, _w3, body, _w4, _w5] = _v_semanticdeclarationfn; return type_parameters.size() > 0 ? [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticExpressionExtern>((*body)._)) { auto _v_semanticexpressionextern = std::get<semantic_ir::SemanticExpressionExtern>((*body)._); auto [_w0, _w1] = _v_semanticexpressionextern; return gen_decl(inner_declaration, codegen_context); } return mlc::String(""); }() : gen_decl(inner_declaration, codegen_context); } if (std::holds_alternative<semantic_ir::SemanticDeclarationExtend>((*semantic_ir::sdecl_inner(inner_declaration)))) { auto _v_semanticdeclarationextend = std::get<semantic_ir::SemanticDeclarationExtend>((*semantic_ir::sdecl_inner(inner_declaration))); auto [_w0, _w1, _w2, _w3] = _v_semanticdeclarationextend; return gen_decl(inner_declaration, codegen_context); } return gen_decl(inner_declaration, codegen_context); }() : phase == 4 ? [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticDeclarationTrait>((*semantic_ir::sdecl_inner(inner_declaration)))) { auto _v_semanticdeclarationtrait = std::get<semantic_ir::SemanticDeclarationTrait>((*semantic_ir::sdecl_inner(inner_declaration))); auto [name, type_parameters, methods, _w0] = _v_semanticdeclarationtrait; return decl_extend::gen_trait_struct(codegen_context, name, type_parameters, methods); } return mlc::String(""); }() : mlc::String(""); }
}, (*declaration));}

mlc::Array<mlc::String> exported_type_names_from_decls(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations) noexcept{
mlc::Array<mlc::String> names = {};
int index = 0;
while (index < declarations.size()){
{
[&]() -> std::tuple<> { if (std::holds_alternative<semantic_ir::SemanticDeclarationExported>((*declarations[index]))) { auto _v_semanticdeclarationexported = std::get<semantic_ir::SemanticDeclarationExported>((*declarations[index])); auto [inner] = _v_semanticdeclarationexported; return [&]() -> std::tuple<> { 
  [&]() -> std::tuple<> { if (std::holds_alternative<semantic_ir::SemanticDeclarationType>((*semantic_ir::sdecl_inner(inner)))) { auto _v_semanticdeclarationtype = std::get<semantic_ir::SemanticDeclarationType>((*semantic_ir::sdecl_inner(inner))); auto [type_name, _w0, _w1, _w2, _w3] = _v_semanticdeclarationtype; return [&]() -> std::tuple<> { 
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

mlc::Array<mlc::String> extend_method_prototypes(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context) noexcept{return methods.map([context](std::shared_ptr<semantic_ir::SemanticDeclaration> method) mutable { return gen_proto(method, context); }).filter([](mlc::String prototype) mutable { return prototype.length() > 0; });}

mlc::Array<mlc::String> extend_helper_prototypes_for_declaration(mlc::Array<mlc::String> exported_types, context::CodegenContext context, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<semantic_ir::SemanticDeclarationExtend>((*declaration))) { auto _v_semanticdeclarationextend = std::get<semantic_ir::SemanticDeclarationExtend>((*declaration)); auto [type_name, _w0, methods, _w1] = _v_semanticdeclarationextend; return decl_index::list_contains(exported_types, type_name) ? extend_method_prototypes(methods, context) : [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty_prototypes = {};
  return empty_prototypes;
 }(); } return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty_prototypes = {};
  return empty_prototypes;
 }(); }();}

mlc::Array<mlc::String> extend_forward_prototypes_for_declaration(context::CodegenContext context, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<semantic_ir::SemanticDeclarationExtend>((*declaration))) { auto _v_semanticdeclarationextend = std::get<semantic_ir::SemanticDeclarationExtend>((*declaration)); auto [_w0, _w1, methods, _w2] = _v_semanticdeclarationextend; return extend_method_prototypes(methods, context); } return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty_forward_segments = {};
  return empty_forward_segments;
 }(); }();}

mlc::Array<mlc::String> extend_helper_protos_for_exported_types(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept{
mlc::Array<mlc::String> exported_types = exported_type_names_from_decls(declarations);
return mlc::collections::flat_map(declarations, [exported_types, context](std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) mutable { return extend_helper_prototypes_for_declaration(exported_types, context, declaration); });
}

mlc::Array<mlc::String> extend_method_forward_segments(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept{return mlc::collections::flat_map(declarations, [context](std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) mutable { return extend_forward_prototypes_for_declaration(context, declaration); });}

decl::DeclPartsBundle collect_all_decl_parts(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept{
mlc::Array<mlc::String> type_fwds = {};
mlc::Array<mlc::String> type_defs = {};
mlc::Array<mlc::String> fn_protos = {};
mlc::Array<mlc::String> extend_forward_segments = extend_method_forward_segments(declarations, context);
mlc::Array<mlc::String> fn_defs = extend_forward_segments;
int declaration_index = 0;
while (declaration_index < declarations.size()){
{
std::shared_ptr<semantic_ir::SemanticDeclaration> declaration = declarations[declaration_index];
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
mlc::Array<mlc::String> extend_helper_prototypes = extend_helper_protos_for_exported_types(declarations, context);
fn_protos = fn_protos.concat(extend_helper_prototypes);
return decl::DeclPartsBundle{type_fwds, type_defs, fn_protos, fn_defs};
}

mlc::Array<mlc::String> collect_decl_parts(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context, int phase) noexcept{
mlc::Array<mlc::String> extend_forward_segments = phase == 3 ? declarations.map([context](std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) mutable { return extend_cpp_forward_segment(declaration, context); }).filter([](mlc::String segment) mutable { return segment.length() > 0; }) : mlc::Array<mlc::String>{};
return extend_forward_segments.concat(declarations.map([context, phase](std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) mutable { return decl_segment(declaration, context, phase); }).filter([](mlc::String segment) mutable { return segment.length() > 0; }));
}

} // namespace decl
