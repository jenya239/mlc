#define main mlc_user_main
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

mlc::String CodegenContext_gen_decl(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;
mlc::String CodegenContext_gen_proto(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;
mlc::String resolved_self_type_name(mlc::String type_name, context::CodegenContext context) noexcept{
  if (((type_name == mlc::String("Self", 4)) || (type_name == mlc::String("self", 4))))   {
    return context.self_type;
  } else   {
    return type_name;
  }
}
context::CodegenContext prototype_context_for_self_parameter(context::CodegenContext context, std::shared_ptr<ast::Param> first_parameter) noexcept{
  return [&]() -> context::CodegenContext {
auto __match_subject = first_parameter->type_value;
if (std::holds_alternative<ast::TyNamed>((*__match_subject))) {
const ast::TyNamed& tyNamed = std::get<ast::TyNamed>((*__match_subject));
auto [type_name] = tyNamed; return context::CodegenContext_for_type_body(context, resolved_self_type_name(type_name, context));
}
return context;
std::abort();
}();
}
mlc::String body_self_type_for_parameter(context::CodegenContext context, std::shared_ptr<ast::Param> first_parameter) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = first_parameter->type_value;
if (std::holds_alternative<ast::TyNamed>((*__match_subject))) {
const ast::TyNamed& tyNamed = std::get<ast::TyNamed>((*__match_subject));
auto [self_type_name] = tyNamed; return resolved_self_type_name(self_type_name, context);
}
return context.self_type;
std::abort();
}();
}
context::CodegenContext prototype_context_for_function(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{
  if (((parameters.length() > 0) && (parameters[0]->name == mlc::String("self", 4))))   {
    return prototype_context_for_self_parameter(context, parameters[0]);
  } else   {
    return context;
  }
}
mlc::String gen_fn_proto(mlc::String name, mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type, context::CodegenContext context) noexcept{
  auto prefix = (cpp_naming::template_prefix(type_parameters) + type_gen::requires_clause(type_parameters, type_bounds));
  auto safe_name = context::CodegenContext_resolve(context, name);
  auto prototype_context = prototype_context_for_function(context, parameters);
  if (((name == mlc::String("main", 4)) && (parameters.length() == 0)))   {
    return (prefix + expr::noexcept_function_prototype(type_gen::sem_type_to_cpp(prototype_context, return_type), safe_name, expr::main_program_parameter_list()));
  } else   {
    return (prefix + expr::noexcept_function_prototype(type_gen::sem_type_to_cpp(prototype_context, return_type), safe_name, decl_extend::gen_params_proto(prototype_context, parameters)));
  }
}
mlc::String body_self_type_for_parameters(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{
  if (((parameters.length() > 0) && (parameters[0]->name == mlc::String("self", 4))))   {
    return body_self_type_for_parameter(context, parameters[0]);
  } else   {
    return context.self_type;
  }
}
context::CodegenContext decl_prototype_context_for_body(mlc::Array<std::shared_ptr<ast::Param>> parameters, context::CodegenContext body_context, context::CodegenContext context) noexcept{
  if (((parameters.length() > 0) && (parameters[0]->name == mlc::String("self", 4))))   {
    return body_context;
  } else   {
    return context;
  }
}
context::CodegenContext compute_fn_body_context(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> parameters, context::CodegenContext context) noexcept{
  return context::make_body_context(context, param_analysis::collect_shared_params(parameters), param_analysis::collect_shared_array_params(parameters), param_analysis::collect_array_elem_types(parameters), param_analysis::collect_shared_map_params(parameters), body_self_type_for_parameters(context, parameters), param_analysis::collect_value_params(parameters), mlc::Array<mlc::String>{});
}
mlc::String gen_fn_decl(mlc::String name, mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SemanticExpression> body, context::CodegenContext context) noexcept{
  auto prefix = (cpp_naming::template_prefix(type_parameters) + type_gen::requires_clause(type_parameters, type_bounds));
  auto safe_name = context::CodegenContext_resolve(context, name);
  auto body_context = context::CodegenContext_with_enclosing_function_return_type(compute_fn_body_context(name, parameters, context), return_type);
  auto prototype_context = decl_prototype_context_for_body(parameters, body_context, context);
  if (((name == mlc::String("main", 4)) && (parameters.length() == 0)))   {
    auto preamble = expr::user_main_arguments_copy_into_runtime_statement();
    return ((((prefix + expr::noexcept_function_body_open(type_gen::sem_type_to_cpp(prototype_context, return_type), safe_name, expr::main_program_parameter_list())) + preamble) + return_body::gen_fn_body(body, body_context)) + expr::block_close_newline());
  } else   {
    return (((prefix + expr::noexcept_function_body_open(type_gen::sem_type_to_cpp(prototype_context, return_type), safe_name, decl_extend::gen_params_def(prototype_context, parameters))) + return_body::gen_fn_body(body, body_context)) + expr::block_close_newline());
  }
}
mlc::String CodegenContext_gen_decl(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;
mlc::String CodegenContext_gen_proto(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept;
mlc::String CodegenContext_gen_decl(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{
return std::visit(overloaded{[&](const semantic_ir::SemanticDeclarationType& semanticDeclarationType) { auto [type_name, type_parameters, variants, derive_traits, __4] = semanticDeclarationType; return (type_gen::gen_type_decl(self, type_name, type_parameters, variants) + type_gen::gen_derive_methods(self, type_name, variants, derive_traits)); },
[&](const semantic_ir::SemanticDeclarationTypeAlias& semanticDeclarationTypeAlias) { auto [__0, __1, __2, __3] = semanticDeclarationTypeAlias; return mlc::String("", 0); },
[&](const semantic_ir::SemanticDeclarationTrait& semanticDeclarationTrait) { auto [name, type_parameters, methods, __3] = semanticDeclarationTrait; return decl_extend::gen_trait_decl(self, name, type_parameters, methods); },
[&](const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn) { auto [name, type_parameters, type_bounds, parameters, return_type, body, __6, __7] = semanticDeclarationFn; return [&]() -> mlc::String {
auto __match_subject = body;
if (std::holds_alternative<semantic_ir::SemanticExpressionExtern>((*__match_subject))) {
const semantic_ir::SemanticExpressionExtern& semanticExpressionExtern = std::get<semantic_ir::SemanticExpressionExtern>((*__match_subject));
auto [__0, __1] = semanticExpressionExtern; return mlc::String("", 0);
}
return gen_fn_decl(name, type_parameters, type_bounds, parameters, return_type, body, self);
std::abort();
}(); },
[&](const semantic_ir::SemanticDeclarationExtend& semanticDeclarationExtend) { auto [type_name, trait_name, methods, __3] = semanticDeclarationExtend; return decl_extend::gen_decl_extend(type_name, trait_name, methods, self, [=](context::CodegenContext codegen_context, mlc::String name) mutable { return context::CodegenContext_resolve(codegen_context, name); }, [=](std::shared_ptr<semantic_ir::SemanticDeclaration> inner_declaration, context::CodegenContext codegen_context) mutable { return CodegenContext_gen_decl(codegen_context, inner_declaration); }); },
[&](const semantic_ir::SemanticDeclarationImport& semanticDeclarationImport) { auto [__0, __1] = semanticDeclarationImport; return mlc::String("", 0); },
[&](const semantic_ir::SemanticDeclarationExported& semanticDeclarationExported) { auto [inner_declaration] = semanticDeclarationExported; return CodegenContext_gen_decl(self, inner_declaration); },
[&](const semantic_ir::SemanticDeclarationAssocBind& semanticDeclarationAssocBind) { auto [__0, __1, __2] = semanticDeclarationAssocBind; return mlc::String("", 0); }
}, (*declaration));
}
mlc::String CodegenContext_gen_proto(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{
return std::visit(overloaded{[&](const semantic_ir::SemanticDeclarationType& semanticDeclarationType) { auto [__0, __1, __2, __3, __4] = semanticDeclarationType; return mlc::String("", 0); },
[&](const semantic_ir::SemanticDeclarationTypeAlias& semanticDeclarationTypeAlias) { auto [__0, __1, __2, __3] = semanticDeclarationTypeAlias; return mlc::String("", 0); },
[&](const semantic_ir::SemanticDeclarationTrait& semanticDeclarationTrait) { auto [__0, __1, __2, __3] = semanticDeclarationTrait; return mlc::String("", 0); },
[&](const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn) { auto [name, type_parameters, type_bounds, parameters, return_type, body, __6, __7] = semanticDeclarationFn; return [&]() -> mlc::String {
auto __match_subject = body;
if (std::holds_alternative<semantic_ir::SemanticExpressionExtern>((*__match_subject))) {
const semantic_ir::SemanticExpressionExtern& semanticExpressionExtern = std::get<semantic_ir::SemanticExpressionExtern>((*__match_subject));
auto [__0, __1] = semanticExpressionExtern; return mlc::String("", 0);
}
return gen_fn_proto(name, type_parameters, type_bounds, parameters, return_type, self);
std::abort();
}(); },
[&](const semantic_ir::SemanticDeclarationExtend& semanticDeclarationExtend) { auto [type_name, __1, methods, __3] = semanticDeclarationExtend; return [&]() {
auto extend_context = context::CodegenContext_for_type_body(self, type_name);
return methods.map([=](std::shared_ptr<semantic_ir::SemanticDeclaration> method) mutable { return CodegenContext_gen_proto(extend_context, method); }).join(mlc::String("", 0));
}(); },
[&](const semantic_ir::SemanticDeclarationImport& semanticDeclarationImport) { auto [__0, __1] = semanticDeclarationImport; return mlc::String("", 0); },
[&](const semantic_ir::SemanticDeclarationExported& semanticDeclarationExported) { auto [inner_declaration] = semanticDeclarationExported; return CodegenContext_gen_proto(self, inner_declaration); },
[&](const semantic_ir::SemanticDeclarationAssocBind& semanticDeclarationAssocBind) { auto [__0, __1, __2] = semanticDeclarationAssocBind; return mlc::String("", 0); }
}, (*declaration));
}
mlc::String gen_decl(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context) noexcept{
  return CodegenContext_gen_decl(context, declaration);
}
mlc::String gen_proto(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext context) noexcept{
  return CodegenContext_gen_proto(context, declaration);
}
bool inner_declaration_is_main(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{
  return [&]() -> bool {
auto __match_subject = semantic_ir::sdecl_inner(declaration);
if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*__match_subject))) {
const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn = std::get<semantic_ir::SemanticDeclarationFn>((*__match_subject));
auto [name, __1, __2, __3, __4, __5, __6, __7] = semanticDeclarationFn; return (name == mlc::String("main", 4));
}
return false;
std::abort();
}();
}
bool decls_have_main(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations) noexcept{
  return declarations.any([=](std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) mutable { return inner_declaration_is_main(declaration); });
}
mlc::String extend_cpp_forward_segment(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext codegen_context) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = declaration;
if (std::holds_alternative<semantic_ir::SemanticDeclarationExtend>((*__match_subject))) {
const semantic_ir::SemanticDeclarationExtend& semanticDeclarationExtend = std::get<semantic_ir::SemanticDeclarationExtend>((*__match_subject));
auto [type_name, trait_name, methods, __3] = semanticDeclarationExtend; return [&]() {
auto extend_context = context::CodegenContext_for_type_body(codegen_context, type_name);
return (decl_extend::gen_extend_wrapper_protos(type_name, trait_name, methods, codegen_context) + methods.map([=](std::shared_ptr<semantic_ir::SemanticDeclaration> method) mutable { return gen_proto(method, extend_context); }).join(mlc::String("", 0)));
}();
}
if (std::holds_alternative<semantic_ir::SemanticDeclarationExported>((*__match_subject))) {
const semantic_ir::SemanticDeclarationExported& semanticDeclarationExported = std::get<semantic_ir::SemanticDeclarationExported>((*__match_subject));
auto [inner_declaration] = semanticDeclarationExported; return [&]() -> mlc::String {
auto __match_subject = semantic_ir::sdecl_inner(inner_declaration);
if (std::holds_alternative<semantic_ir::SemanticDeclarationExtend>((*__match_subject))) {
const semantic_ir::SemanticDeclarationExtend& semanticDeclarationExtend = std::get<semantic_ir::SemanticDeclarationExtend>((*__match_subject));
auto [type_name, trait_name, methods, __3] = semanticDeclarationExtend; return [&]() {
auto extend_context = context::CodegenContext_for_type_body(codegen_context, type_name);
return (decl_extend::gen_extend_wrapper_protos(type_name, trait_name, methods, codegen_context) + methods.map([=](std::shared_ptr<semantic_ir::SemanticDeclaration> method) mutable { return gen_proto(method, extend_context); }).join(mlc::String("", 0)));
}();
}
return mlc::String("", 0);
std::abort();
}();
}
return mlc::String("", 0);
std::abort();
}();
}
mlc::String decl_segment_for_type(mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_traits, context::CodegenContext codegen_context, int phase) noexcept{
  if ((phase == 0))   {
    return type_gen::gen_type_decl_fwd_only(codegen_context, type_name, type_parameters, variants);
  } else if ((phase == 1))   {
    return (type_gen::gen_type_decl_body_only(codegen_context, type_name, type_parameters, variants) + type_gen::gen_derive_methods(codegen_context, type_name, variants, derive_traits));
  } else   {
    return mlc::String("", 0);
  }
}
mlc::String decl_segment_for_trait(mlc::String name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext codegen_context, int phase) noexcept{
  if ((phase == 0))   {
    return decl_extend::gen_trait_decl(codegen_context, name, type_parameters, methods);
  } else if ((phase == 4))   {
    return decl_extend::gen_trait_struct(codegen_context, name, type_parameters, methods);
  } else   {
    return mlc::String("", 0);
  }
}
mlc::String decl_segment_for_function(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext codegen_context, int phase) noexcept{
  if ((phase == 2))   {
    return gen_proto(declaration, codegen_context);
  } else if ((phase == 3))   {
    return gen_decl(declaration, codegen_context);
  } else   {
    return mlc::String("", 0);
  }
}
mlc::String decl_segment_for_extend(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext codegen_context, int phase) noexcept{
  if ((phase == 2))   {
    return gen_proto(declaration, codegen_context);
  } else if ((phase == 3))   {
    return gen_decl(declaration, codegen_context);
  } else   {
    return mlc::String("", 0);
  }
}
mlc::String decl_segment_for_import_or_assoc(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext codegen_context, int phase) noexcept{
  if ((phase == 2))   {
    return gen_proto(declaration, codegen_context);
  } else if ((phase == 3))   {
    return gen_decl(declaration, codegen_context);
  } else   {
    return mlc::String("", 0);
  }
}
bool extern_body_is_extern(std::shared_ptr<semantic_ir::SemanticExpression> body) noexcept{
  return [&]() -> bool {
auto __match_subject = body;
if (std::holds_alternative<semantic_ir::SemanticExpressionExtern>((*__match_subject))) {
const semantic_ir::SemanticExpressionExtern& semanticExpressionExtern = std::get<semantic_ir::SemanticExpressionExtern>((*__match_subject));
auto [__0, __1] = semanticExpressionExtern; return true;
}
return false;
std::abort();
}();
}
mlc::String decl_segment_for_exported_function_phase_two(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, mlc::Array<mlc::String> type_parameters, std::shared_ptr<semantic_ir::SemanticExpression> body, context::CodegenContext codegen_context) noexcept{
  if ((type_parameters.length() == 0))   {
    return gen_proto(declaration, codegen_context);
  } else if (extern_body_is_extern(body))   {
    return gen_proto(declaration, codegen_context);
  } else   {
    return gen_decl(declaration, codegen_context);
  }
}
mlc::String decl_segment_for_exported_function_phase_three(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, mlc::Array<mlc::String> type_parameters, std::shared_ptr<semantic_ir::SemanticExpression> body, context::CodegenContext codegen_context) noexcept{
  if ((type_parameters.length() == 0))   {
    return gen_decl(declaration, codegen_context);
  } else if (extern_body_is_extern(body))   {
    return gen_decl(declaration, codegen_context);
  } else   {
    return mlc::String("", 0);
  }
}
mlc::String decl_segment_for_exported_function(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, mlc::Array<mlc::String> type_parameters, std::shared_ptr<semantic_ir::SemanticExpression> body, context::CodegenContext codegen_context, int phase) noexcept{
  if ((phase == 2))   {
    return decl_segment_for_exported_function_phase_two(declaration, type_parameters, body, codegen_context);
  } else if ((phase == 3))   {
    return decl_segment_for_exported_function_phase_three(declaration, type_parameters, body, codegen_context);
  } else   {
    return mlc::String("", 0);
  }
}
mlc::String decl_segment_for_exported(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext codegen_context, int phase) noexcept{
  return std::visit(overloaded{[&](const semantic_ir::SemanticDeclarationType& semanticDeclarationType) { auto [type_name, type_parameters, variants, derive_traits, __4] = semanticDeclarationType; return decl_segment_for_type(type_name, type_parameters, variants, derive_traits, codegen_context, phase); },
[&](const semantic_ir::SemanticDeclarationTypeAlias& semanticDeclarationTypeAlias) { auto [__0, __1, __2, __3] = semanticDeclarationTypeAlias; return mlc::String("", 0); },
[&](const semantic_ir::SemanticDeclarationTrait& semanticDeclarationTrait) { auto [name, type_parameters, methods, __3] = semanticDeclarationTrait; return decl_segment_for_trait(name, type_parameters, methods, codegen_context, phase); },
[&](const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn) { auto [__0, type_parameters, __2, __3, __4, body, __6, __7] = semanticDeclarationFn; return decl_segment_for_exported_function(declaration, type_parameters, body, codegen_context, phase); },
[&](const semantic_ir::SemanticDeclarationExtend& semanticDeclarationExtend) { auto [__0, __1, __2, __3] = semanticDeclarationExtend; return decl_segment_for_extend(declaration, codegen_context, phase); },
[&](const semantic_ir::SemanticDeclarationImport& semanticDeclarationImport) { auto [__0, __1] = semanticDeclarationImport; return decl_segment_for_import_or_assoc(declaration, codegen_context, phase); },
[&](const semantic_ir::SemanticDeclarationExported& semanticDeclarationExported) { auto [inner_declaration] = semanticDeclarationExported; return decl_segment_for_exported(inner_declaration, codegen_context, phase); },
[&](const semantic_ir::SemanticDeclarationAssocBind& semanticDeclarationAssocBind) { auto [__0, __1, __2] = semanticDeclarationAssocBind; return mlc::String("", 0); }
}, (*semantic_ir::sdecl_inner(declaration)));
}
mlc::String decl_segment(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration, context::CodegenContext codegen_context, int phase) noexcept{
  return std::visit(overloaded{[&](const semantic_ir::SemanticDeclarationType& semanticDeclarationType) { auto [type_name, type_parameters, variants, derive_traits, __4] = semanticDeclarationType; return decl_segment_for_type(type_name, type_parameters, variants, derive_traits, codegen_context, phase); },
[&](const semantic_ir::SemanticDeclarationTypeAlias& semanticDeclarationTypeAlias) { auto [__0, __1, __2, __3] = semanticDeclarationTypeAlias; return mlc::String("", 0); },
[&](const semantic_ir::SemanticDeclarationTrait& semanticDeclarationTrait) { auto [name, type_parameters, methods, __3] = semanticDeclarationTrait; return decl_segment_for_trait(name, type_parameters, methods, codegen_context, phase); },
[&](const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn) { auto [__0, __1, __2, __3, __4, __5, __6, __7] = semanticDeclarationFn; return decl_segment_for_function(declaration, codegen_context, phase); },
[&](const semantic_ir::SemanticDeclarationExtend& semanticDeclarationExtend) { auto [__0, __1, __2, __3] = semanticDeclarationExtend; return decl_segment_for_extend(declaration, codegen_context, phase); },
[&](const semantic_ir::SemanticDeclarationImport& semanticDeclarationImport) { auto [__0, __1] = semanticDeclarationImport; return mlc::String("", 0); },
[&](const semantic_ir::SemanticDeclarationAssocBind& semanticDeclarationAssocBind) { auto [__0, __1, __2] = semanticDeclarationAssocBind; return mlc::String("", 0); },
[&](const semantic_ir::SemanticDeclarationExported& semanticDeclarationExported) { auto [inner_declaration] = semanticDeclarationExported; return decl_segment_for_exported(inner_declaration, codegen_context, phase); }
}, (*declaration));
}
mlc::Array<mlc::String> exported_type_name_from_declaration(std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{
  return [&]() -> mlc::Array<mlc::String> {
auto __match_subject = declaration;
if (std::holds_alternative<semantic_ir::SemanticDeclarationExported>((*__match_subject))) {
const semantic_ir::SemanticDeclarationExported& semanticDeclarationExported = std::get<semantic_ir::SemanticDeclarationExported>((*__match_subject));
auto [inner] = semanticDeclarationExported; return [&]() -> mlc::Array<mlc::String> {
auto __match_subject = semantic_ir::sdecl_inner(inner);
if (std::holds_alternative<semantic_ir::SemanticDeclarationType>((*__match_subject))) {
const semantic_ir::SemanticDeclarationType& semanticDeclarationType = std::get<semantic_ir::SemanticDeclarationType>((*__match_subject));
auto [type_name, __1, __2, __3, __4] = semanticDeclarationType; return mlc::Array<mlc::String>{type_name};
}
return [&]() {
auto empty = mlc::Array<mlc::String>{};
return empty;
}();
std::abort();
}();
}
return [&]() {
auto empty = mlc::Array<mlc::String>{};
return empty;
}();
std::abort();
}();
}
mlc::Array<mlc::String> exported_type_names_from_decls(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations) noexcept{
  return declarations.flat_map(exported_type_name_from_declaration);
}
mlc::Array<mlc::String> extend_method_prototypes(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context) noexcept{
  return methods.map([=](std::shared_ptr<semantic_ir::SemanticDeclaration> method) mutable { return gen_proto(method, context); }).filter([=](mlc::String prototype) mutable { return (prototype.length() > 0); });
}
mlc::Array<mlc::String> extend_helper_prototypes_for_declaration(mlc::Array<mlc::String> exported_types, context::CodegenContext context, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{
  return [&]() -> mlc::Array<mlc::String> {
auto __match_subject = declaration;
if (std::holds_alternative<semantic_ir::SemanticDeclarationExtend>((*__match_subject))) {
const semantic_ir::SemanticDeclarationExtend& semanticDeclarationExtend = std::get<semantic_ir::SemanticDeclarationExtend>((*__match_subject));
auto [type_name, __1, methods, __3] = semanticDeclarationExtend; return [&]() -> mlc::Array<mlc::String> {
  if (decl_index::list_contains(exported_types, type_name))   {
    return extend_method_prototypes(methods, context);
  } else   {
    return [&]() {
auto empty_prototypes = mlc::Array<mlc::String>{};
return empty_prototypes;
}();
  }
}();
}
return [&]() {
auto empty_prototypes = mlc::Array<mlc::String>{};
return empty_prototypes;
}();
std::abort();
}();
}
mlc::Array<mlc::String> extend_forward_prototypes_for_declaration(context::CodegenContext context, std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) noexcept{
  return [&]() -> mlc::Array<mlc::String> {
auto __match_subject = declaration;
if (std::holds_alternative<semantic_ir::SemanticDeclarationExtend>((*__match_subject))) {
const semantic_ir::SemanticDeclarationExtend& semanticDeclarationExtend = std::get<semantic_ir::SemanticDeclarationExtend>((*__match_subject));
auto [__0, __1, methods, __3] = semanticDeclarationExtend; return extend_method_prototypes(methods, context);
}
return [&]() {
auto empty_forward_segments = mlc::Array<mlc::String>{};
return empty_forward_segments;
}();
std::abort();
}();
}
mlc::Array<mlc::String> extend_helper_protos_for_exported_types(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept{
  auto exported_types = exported_type_names_from_decls(declarations);
  return declarations.flat_map([=](std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) mutable { return extend_helper_prototypes_for_declaration(exported_types, context, declaration); });
}
mlc::Array<mlc::String> extend_method_forward_segments(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept{
  return declarations.flat_map([=](std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) mutable { return extend_forward_prototypes_for_declaration(context, declaration); });
}
DeclPartsBundle collect_all_decl_parts(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context) noexcept{
  auto type_fwds = mlc::Array<mlc::String>{};
  auto type_defs = mlc::Array<mlc::String>{};
  auto fn_protos = mlc::Array<mlc::String>{};
  auto extend_forward_segments = extend_method_forward_segments(declarations, context);
  auto fn_defs = extend_forward_segments;
  auto declaration_index = 0;
  while ((declaration_index < declarations.length()))   {
    auto declaration = declarations[declaration_index];
    auto extend_forward_segment = extend_cpp_forward_segment(declaration, context);
    if ((extend_forward_segment.length() > 0))     {
      fn_defs.push_back(extend_forward_segment);
    }
    auto forward_segment = decl_segment(declaration, context, 0);
    if ((forward_segment.length() > 0))     {
      type_fwds.push_back(forward_segment);
    }
    auto definition_segment = decl_segment(declaration, context, 1);
    if ((definition_segment.length() > 0))     {
      type_defs.push_back(definition_segment);
    }
    auto prototype_segment = decl_segment(declaration, context, 2);
    if ((prototype_segment.length() > 0))     {
      fn_protos.push_back(prototype_segment);
    }
    auto definition_body_segment = decl_segment(declaration, context, 3);
    if ((definition_body_segment.length() > 0))     {
      fn_defs.push_back(definition_body_segment);
    }
    auto trait_struct_segment = decl_segment(declaration, context, 4);
    if ((trait_struct_segment.length() > 0))     {
      fn_defs.push_back(trait_struct_segment);
    }
    (declaration_index = (declaration_index + 1));
  }
  auto extend_helper_prototypes = extend_helper_protos_for_exported_types(declarations, context);
  (fn_protos = fn_protos.concat(extend_helper_prototypes));
  return DeclPartsBundle{type_fwds, type_defs, fn_protos, fn_defs};
}
mlc::Array<mlc::String> collect_decl_parts(mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> declarations, context::CodegenContext context, int phase) noexcept{
  auto extend_forward_segments = ((phase == 3) ? (declarations.map([=](std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) mutable { return extend_cpp_forward_segment(declaration, context); }).filter([=](mlc::String segment) mutable { return (segment.length() > 0); })) : (mlc::Array<mlc::String>{}));
  return extend_forward_segments.concat(declarations.map([=](std::shared_ptr<semantic_ir::SemanticDeclaration> declaration) mutable { return decl_segment(declaration, context, phase); }).filter([=](mlc::String segment) mutable { return (segment.length() > 0); }));
}

} // namespace decl
