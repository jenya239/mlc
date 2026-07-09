#ifndef DECL_EXTEND_HPP
#define DECL_EXTEND_HPP

#include "mlc.hpp"
#include <variant>
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

mlc::String trait_dispatch_name(mlc::String trait_name, mlc::String method_name) noexcept;
mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept;
mlc::String default_expr_to_cpp(std::shared_ptr<ast::Expr> expression) noexcept;
mlc::String generic_parameter_type_cpp(context::CodegenContext context, std::shared_ptr<ast::Param> parameter, mlc::String generic_type_name) noexcept;
mlc::String non_generic_parameter_type_cpp(context::CodegenContext context, std::shared_ptr<ast::Param> parameter) noexcept;
mlc::String parameter_type_cpp(context::CodegenContext context, std::shared_ptr<ast::Param> parameter) noexcept;
mlc::String param_item_def(context::CodegenContext context, std::shared_ptr<ast::Param> parameter) noexcept;
mlc::String param_item_proto(context::CodegenContext context, std::shared_ptr<ast::Param> parameter) noexcept;
mlc::String gen_params_proto(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;
mlc::Array<mlc::String> gen_parameter_proto_items(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;
mlc::String gen_params_def(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;
mlc::Array<mlc::String> gen_parameter_def_items(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;
mlc::Array<mlc::String> non_self_parameter_types_cpp(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;
mlc::Array<mlc::String> parameter_call_argument_names(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;
mlc::String concept_declval_arguments_for_params(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;
mlc::String extend_method_return_type_from_declaration(context::CodegenContext extend_context, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept;
mlc::String extend_method_return_type_cpp(context::CodegenContext extend_context, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept;
mlc::String trait_concept_cpp_name_for_static_assert(mlc::String trait_name) noexcept;
mlc::String generic_trait_static_assert_line(mlc::String trait_name, mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext extend_context, mlc::String implementor_type_cpp) noexcept;
mlc::String trait_struct_field_line_for_method(context::CodegenContext trait_context, mlc::String trait_name, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept;
mlc::Array<mlc::String> trait_requires_requirement_parts(context::CodegenContext trait_context, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept;
mlc::String extend_wrapper_proto_line(mlc::String type_name, context::CodegenContext context, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept;
mlc::String extend_trait_wrapper_line(mlc::String type_name, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept;
mlc::String extend_trait_concept_adapter_line(mlc::String type_name, mlc::String trait_name, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept;
template<typename __F5>
mlc::Array<mlc::String> extend_method_part_strings(mlc::String type_name, mlc::String trait_name, context::CodegenContext extend_context, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, __F5 gen_decl_fn, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept;
mlc::String trait_method_field_line(context::CodegenContext context, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type) noexcept;
mlc::String gen_trait_struct(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept;
bool trait_skips_concept_dispatch_forward_protos(mlc::String trait_name) noexcept;
mlc::String trait_concept_dispatch_forward_proto_line(mlc::String trait_name, mlc::String mangled, mlc::String return_type_cpp) noexcept;
mlc::String trait_concept_dispatch_forward_protos(context::CodegenContext context, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept;
mlc::String gen_trait_decl(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept;
mlc::String gen_trait_decl_with_dispatch_forward_protos(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept;
mlc::String gen_extend_wrapper_protos(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context) noexcept;
template<typename __F5>
mlc::String gen_extend_trait_wrapper(mlc::String type_name, mlc::String mangled, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type, context::CodegenContext context, __F5 context_resolve_fn) noexcept;
mlc::String gen_extend_trait_wrappers(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept;
template<typename __F6>
mlc::String gen_extend_extern_method(mlc::String mangled, mlc::String type_name, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type, mlc::String trait_name, context::CodegenContext context, __F6 context_resolve_fn) noexcept;
mlc::Array<mlc::String> gen_extend_method_parts(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext extend_context, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticDeclaration>, context::CodegenContext)> gen_decl_fn) noexcept;
template<typename __F6>
mlc::String gen_extend_trait_concept_adapter(mlc::String type_name, mlc::String trait_name, mlc::String mangled, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type, context::CodegenContext context, __F6 context_resolve_fn) noexcept;
mlc::String gen_extend_trait_concept_adapters(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept;
mlc::String gen_decl_extend_trait_suffix(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept;
mlc::String gen_decl_extend_methods_only(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticDeclaration>, context::CodegenContext)> gen_decl_fn) noexcept;
mlc::String gen_decl_extend(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticDeclaration>, context::CodegenContext)> gen_decl_fn) noexcept;
template<typename __F5>
mlc::Array<mlc::String> extend_method_part_strings(mlc::String type_name, mlc::String trait_name, context::CodegenContext extend_context, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, __F5 gen_decl_fn, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept{
  return [&]() -> mlc::Array<mlc::String> {
auto __match_subject = method;
if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*__match_subject))) {
const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn = std::get<semantic_ir::SemanticDeclarationFn>((*__match_subject));
auto [mangled, __1, __2, parameters, return_type, body, __6, __7, __8] = semanticDeclarationFn; return [&]() -> mlc::Array<mlc::String> {
auto __match_subject = body;
if (std::holds_alternative<semantic_ir::SemanticExpressionExtern>((*__match_subject))) {
const semantic_ir::SemanticExpressionExtern& semanticExpressionExtern = std::get<semantic_ir::SemanticExpressionExtern>((*__match_subject));
auto [__0, __1] = semanticExpressionExtern; return [&]() {
auto generated = gen_extend_extern_method(mangled, type_name, parameters, return_type, trait_name, context, context_resolve_fn);
return [&]() -> mlc::Array<mlc::String> {
  if ((generated.length() > 0))   {
    return [&]() {
auto extern_parts = mlc::Array<mlc::String>{};
extern_parts.push_back(generated);
return extern_parts;
}();
  } else   {
    return [&]() {
auto empty_extern_parts = mlc::Array<mlc::String>{};
return empty_extern_parts;
}();
  }
}();
}();
}
return [&]() {
auto body_parts = mlc::Array<mlc::String>{};
body_parts.push_back(gen_decl_fn(method, extend_context));
return body_parts;
}();
std::abort();
}();
}
return [&]() {
auto fallback_parts = mlc::Array<mlc::String>{};
fallback_parts.push_back(gen_decl_fn(method, extend_context));
return fallback_parts;
}();
std::abort();
}();
}
template<typename __F5>
mlc::String gen_extend_trait_wrapper(mlc::String type_name, mlc::String mangled, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type, context::CodegenContext context, __F5 context_resolve_fn) noexcept{
  auto wrapper_context = context::CodegenContext_for_type_body(context, type_name);
  auto method_name = extract_method_name(mangled, type_name);
  auto return_type_cpp = type_gen::sem_type_to_cpp(wrapper_context, return_type);
  auto fn_resolved = context_resolve_fn(context, mangled);
  auto parameters_string = gen_params_proto(wrapper_context, parameters);
  return expr::inline_noexcept_forwarding_call(return_type_cpp, cpp_naming::cpp_safe(method_name), parameters_string, fn_resolved, parameter_call_argument_names(parameters).join(mlc::String(", ", 2)));
}
template<typename __F6>
mlc::String gen_extend_extern_method(mlc::String mangled, mlc::String type_name, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type, mlc::String trait_name, context::CodegenContext context, __F6 context_resolve_fn) noexcept{
  auto method_name = ((trait_name.length() > 0) ? (extract_method_name(mangled, type_name)) : (mlc::String("", 0)));
  if ((((type_name == mlc::String("i32", 3)) && (method_name == mlc::String("to_string", 9))) && (parameters.length() > 0)))   {
    auto return_type_cpp = type_gen::sem_type_to_cpp(context, return_type);
    auto parameters_string = gen_params_def(context, parameters);
    return expr::noexcept_one_line_returning_function(return_type_cpp, context_resolve_fn(context, mangled), parameters_string, expr::runtime_to_string_call(cpp_naming::cpp_safe(parameters[0]->name)));
  } else   {
    return mlc::String("", 0);
  }
}
template<typename __F6>
mlc::String gen_extend_trait_concept_adapter(mlc::String type_name, mlc::String trait_name, mlc::String mangled, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type, context::CodegenContext context, __F6 context_resolve_fn) noexcept{
  auto extend_context = context::CodegenContext_for_type_body(context, type_name);
  auto method_name = extract_method_name(mangled, type_name);
  auto trait_callee = cpp_naming::cpp_safe(((trait_name + mlc::String("_", 1)) + method_name));
  auto return_type_cpp = type_gen::sem_type_to_cpp(extend_context, return_type);
  auto parameters_string = gen_params_proto(extend_context, parameters);
  auto fn_resolved = context_resolve_fn(context, mangled);
  return expr::inline_noexcept_forwarding_call(return_type_cpp, trait_callee, parameters_string, fn_resolved, parameter_call_argument_names(parameters).join(mlc::String(", ", 2)));
}

} // namespace decl_extend

#endif // DECL_EXTEND_HPP
