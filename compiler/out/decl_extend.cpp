#define main mlc_user_main
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

mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept{
  auto prefix = (type_name + mlc::String("_", 1));
  if (((fn_name.length() > prefix.length()) && (fn_name.substring(0, prefix.length()) == prefix)))   {
    return fn_name.substring(prefix.length(), (fn_name.length() - prefix.length()));
  } else   {
    return fn_name;
  }
}
mlc::String default_expr_to_cpp(std::shared_ptr<ast::Expr> expression) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = expression;
if (std::holds_alternative<ast::ExprInt>((*__match_subject))) {
const ast::ExprInt& exprInt = std::get<ast::ExprInt>((*__match_subject));
auto [v, __1] = exprInt; return literals::gen_integer_literal(v);
}
if (std::holds_alternative<ast::ExprStr>((*__match_subject))) {
const ast::ExprStr& exprStr = std::get<ast::ExprStr>((*__match_subject));
auto [s, __1] = exprStr; return literals::gen_string_literal(s);
}
if (std::holds_alternative<ast::ExprBool>((*__match_subject))) {
const ast::ExprBool& exprBool = std::get<ast::ExprBool>((*__match_subject));
auto [b, __1] = exprBool; return literals::gen_boolean_literal(b);
}
if (std::holds_alternative<ast::ExprUnit>((*__match_subject))) {
const ast::ExprUnit& exprUnit = std::get<ast::ExprUnit>((*__match_subject));
auto [__0] = exprUnit; return literals::gen_unit_literal();
}
if (std::holds_alternative<ast::ExprFloat>((*__match_subject))) {
const ast::ExprFloat& exprFloat = std::get<ast::ExprFloat>((*__match_subject));
auto [v, __1] = exprFloat; return literals::gen_float_literal(v);
}
if (std::holds_alternative<ast::ExprI64>((*__match_subject))) {
const ast::ExprI64& exprI64 = std::get<ast::ExprI64>((*__match_subject));
auto [v, __1] = exprI64; return literals::gen_i64_literal(v);
}
if (std::holds_alternative<ast::ExprU8>((*__match_subject))) {
const ast::ExprU8& exprU8 = std::get<ast::ExprU8>((*__match_subject));
auto [v, __1] = exprU8; return literals::gen_u8_literal(v);
}
if (std::holds_alternative<ast::ExprUsize>((*__match_subject))) {
const ast::ExprUsize& exprUsize = std::get<ast::ExprUsize>((*__match_subject));
auto [v, __1] = exprUsize; return literals::gen_usize_literal(v);
}
if (std::holds_alternative<ast::ExprChar>((*__match_subject))) {
const ast::ExprChar& exprChar = std::get<ast::ExprChar>((*__match_subject));
auto [v, __1] = exprChar; return literals::gen_char_literal(v);
}
return mlc::to_string(0);
std::abort();
}();
}
mlc::String generic_parameter_type_cpp(context::CodegenContext context, std::shared_ptr<ast::Param> parameter, mlc::String generic_type_name) noexcept{
  if ((generic_type_name == mlc::String("ref", 3)))   {
    return type_gen::type_to_cpp(context, parameter->type_value);
  } else if (parameter->is_mut)   {
    return expr::cpp_lvalue_reference_suffix(type_gen::type_to_cpp(context, parameter->type_value));
  } else   {
    return type_gen::type_to_cpp(context, parameter->type_value);
  }
}
mlc::String non_generic_parameter_type_cpp(context::CodegenContext context, std::shared_ptr<ast::Param> parameter) noexcept{
  if (parameter->is_mut)   {
    return expr::cpp_lvalue_reference_suffix(type_gen::type_to_cpp(context, parameter->type_value));
  } else   {
    return type_gen::type_to_cpp(context, parameter->type_value);
  }
}
mlc::String parameter_type_cpp(context::CodegenContext context, std::shared_ptr<ast::Param> parameter) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = parameter->type_value;
if (std::holds_alternative<ast::TyGeneric>((*__match_subject))) {
const ast::TyGeneric& tyGeneric = std::get<ast::TyGeneric>((*__match_subject));
auto [generic_type_name, __1] = tyGeneric; return generic_parameter_type_cpp(context, parameter, generic_type_name);
}
return non_generic_parameter_type_cpp(context, parameter);
std::abort();
}();
}
mlc::String param_item_def(context::CodegenContext context, std::shared_ptr<ast::Param> parameter) noexcept{
  return expr::parameter_declaration_item(parameter_type_cpp(context, parameter), cpp_naming::cpp_safe(parameter->name));
}
mlc::String param_item_proto(context::CodegenContext context, std::shared_ptr<ast::Param> parameter) noexcept{
  if (parameter->has_default)   {
    return ((param_item_def(context, parameter) + mlc::String(" = ", 3)) + default_expr_to_cpp(parameter->default_));
  } else   {
    return param_item_def(context, parameter);
  }
}
mlc::String gen_params_proto(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{
  return gen_parameter_proto_items(context, parameters).join(mlc::String(", ", 2));
}
mlc::Array<mlc::String> gen_parameter_proto_items(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{
  return parameters.map([=](std::shared_ptr<ast::Param> parameter) mutable { return param_item_proto(context, parameter); });
}
mlc::String gen_params_def(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{
  return gen_parameter_def_items(context, parameters).join(mlc::String(", ", 2));
}
mlc::Array<mlc::String> gen_parameter_def_items(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{
  return parameters.map([=](std::shared_ptr<ast::Param> parameter) mutable { return param_item_def(context, parameter); });
}
mlc::Array<mlc::String> non_self_parameter_types_cpp(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{
  return parameters.filter([=](std::shared_ptr<ast::Param> parameter) mutable { return (parameter->name != mlc::String("self", 4)); }).map([=](std::shared_ptr<ast::Param> parameter) mutable { return type_gen::type_to_cpp(context, parameter->type_value); });
}
mlc::Array<mlc::String> parameter_call_argument_names(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{
  return parameters.map([=](std::shared_ptr<ast::Param> parameter) mutable { return cpp_naming::cpp_safe(parameter->name); });
}
mlc::String concept_declval_arguments_for_params(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{
  return non_self_parameter_types_cpp(context, parameters).map([=](mlc::String type_code) mutable { return expr::std_declval_expression(type_code); }).join(mlc::String(", ", 2));
}
mlc::String extend_method_return_type_from_declaration(context::CodegenContext extend_context, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = method;
if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*__match_subject))) {
const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn = std::get<semantic_ir::SemanticDeclarationFn>((*__match_subject));
auto [__0, __1, __2, __3, return_type, __5, __6, __7] = semanticDeclarationFn; return type_gen::sem_type_to_cpp(extend_context, return_type);
}
return mlc::String("void", 4);
std::abort();
}();
}
mlc::String extend_method_return_type_cpp(context::CodegenContext extend_context, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept{
  return methods.fold(mlc::String("void", 4), [=](mlc::String accumulated, std::shared_ptr<semantic_ir::SemanticDeclaration> method) mutable { return ((accumulated != mlc::String("void", 4)) ? (accumulated) : (extend_method_return_type_from_declaration(extend_context, method))); });
}
mlc::String generic_trait_static_assert_line(mlc::String trait_name, mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext extend_context, mlc::String implementor_type_cpp) noexcept{
  if ((trait_name == mlc::String("ExprVisitor", 11)))   {
    return expr::static_assert_concept_for_result_and_implementor_line(cpp_naming::cpp_safe(trait_name), extend_method_return_type_cpp(extend_context, methods), implementor_type_cpp, ((type_name + mlc::String(" does not implement ", 20)) + trait_name));
  } else   {
    return expr::static_assert_concept_for_type_line(cpp_naming::cpp_safe(trait_name), implementor_type_cpp, ((type_name + mlc::String(" does not implement ", 20)) + trait_name));
  }
}
mlc::String trait_struct_field_line_for_method(context::CodegenContext trait_context, mlc::String trait_name, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = method;
if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*__match_subject))) {
const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn = std::get<semantic_ir::SemanticDeclarationFn>((*__match_subject));
auto [mangled, __1, __2, parameters, return_type, __5, __6, __7] = semanticDeclarationFn; return trait_method_field_line(trait_context, extract_method_name(mangled, trait_name), parameters, return_type);
}
return mlc::String("", 0);
std::abort();
}();
}
mlc::Array<mlc::String> trait_requires_requirement_parts(context::CodegenContext trait_context, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept{
  return [&]() -> mlc::Array<mlc::String> {
auto __match_subject = method;
if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*__match_subject))) {
const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn = std::get<semantic_ir::SemanticDeclarationFn>((*__match_subject));
auto [mangled, __1, __2, parameters, return_type, __5, __6, __7] = semanticDeclarationFn; return [&]() {
auto requirement_parts = mlc::Array<mlc::String>{};
requirement_parts.push_back(expr::concept_requires_expression_method_returns_convertible(cpp_naming::cpp_safe(mangled), concept_declval_arguments_for_params(trait_context, parameters), type_gen::sem_type_to_cpp(trait_context, return_type)));
return requirement_parts;
}();
}
return [&]() {
auto empty_requirements = mlc::Array<mlc::String>{};
return empty_requirements;
}();
std::abort();
}();
}
mlc::String extend_wrapper_proto_line(mlc::String type_name, context::CodegenContext context, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = method;
if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*__match_subject))) {
const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn = std::get<semantic_ir::SemanticDeclarationFn>((*__match_subject));
auto [mangled, __1, __2, parameters, return_type, __5, __6, __7] = semanticDeclarationFn; return [&]() {
auto wrapper_context = context::CodegenContext_for_type_body(context, type_name);
return expr::noexcept_function_prototype(type_gen::sem_type_to_cpp(wrapper_context, return_type), cpp_naming::cpp_safe(extract_method_name(mangled, type_name)), gen_params_proto(wrapper_context, parameters));
}();
}
return mlc::String("", 0);
std::abort();
}();
}
mlc::String extend_trait_wrapper_line(mlc::String type_name, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = method;
if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*__match_subject))) {
const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn = std::get<semantic_ir::SemanticDeclarationFn>((*__match_subject));
auto [mangled, __1, __2, parameters, return_type, __5, __6, __7] = semanticDeclarationFn; return gen_extend_trait_wrapper(type_name, mangled, parameters, return_type, context, context_resolve_fn);
}
return mlc::String("", 0);
std::abort();
}();
}
mlc::String extend_trait_concept_adapter_line(mlc::String type_name, mlc::String trait_name, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = method;
if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*__match_subject))) {
const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn = std::get<semantic_ir::SemanticDeclarationFn>((*__match_subject));
auto [mangled, __1, __2, parameters, return_type, __5, __6, __7] = semanticDeclarationFn; return gen_extend_trait_concept_adapter(type_name, trait_name, mangled, parameters, return_type, context, context_resolve_fn);
}
return mlc::String("", 0);
std::abort();
}();
}
mlc::Array<mlc::String> extend_method_part_strings(mlc::String type_name, mlc::String trait_name, context::CodegenContext extend_context, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticDeclaration>, context::CodegenContext)> gen_decl_fn, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept{
  return [&]() -> mlc::Array<mlc::String> {
auto __match_subject = method;
if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*__match_subject))) {
const semantic_ir::SemanticDeclarationFn& semanticDeclarationFn = std::get<semantic_ir::SemanticDeclarationFn>((*__match_subject));
auto [mangled, __1, __2, parameters, return_type, body, __6, __7] = semanticDeclarationFn; return [&]() -> mlc::Array<mlc::String> {
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
mlc::String trait_method_field_line(context::CodegenContext context, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type) noexcept{
  auto parameter_types = non_self_parameter_types_cpp(context, parameters);
  auto return_cpp = type_gen::sem_type_to_cpp(context, return_type);
  return expr::std_function_field_line(return_cpp, parameter_types.join(mlc::String(", ", 2)), cpp_naming::cpp_safe(method_name));
}
mlc::String gen_trait_struct(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept{
  auto trait_context = context::CodegenContext_for_type_body(context, trait_name);
  auto template_header = ((type_parameters.length() > 0) ? (expr::cpp_template_typename_header_line(type_parameters.join(mlc::String(", ", 2)))) : (mlc::String("", 0)));
  auto field_lines = methods.map([=](std::shared_ptr<semantic_ir::SemanticDeclaration> method) mutable { return trait_struct_field_line_for_method(trait_context, trait_name, method); }).filter([=](mlc::String line) mutable { return (line.length() > 0); });
  return expr::trait_struct_definition_lines(template_header, expr::trait_vtable_struct_cpp_name(cpp_naming::cpp_safe(trait_name)), field_lines.join(mlc::String("", 0)));
}
mlc::String gen_trait_decl(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept{
  auto trait_context = context::CodegenContext_for_type_body(context, trait_name);
  auto template_header = ((type_parameters.length() > 0) ? (((mlc::String("template<typename ", 18) + type_parameters[0]) + mlc::String(", typename Self>\n", 17))) : (mlc::String("template<typename Self>\n", 24)));
  auto req_body = methods.flat_map([=](std::shared_ptr<semantic_ir::SemanticDeclaration> method) mutable { return trait_requires_requirement_parts(trait_context, method); }).join(mlc::String("; ", 2));
  return expr::trait_concept_requires_definition_line(template_header, cpp_naming::cpp_safe(trait_name), mlc::String("Self", 4), req_body);
}
mlc::String gen_extend_wrapper_protos(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context) noexcept{
  if ((trait_name.length() == 0))   {
    return mlc::String("", 0);
  } else   {
    return methods.map([=](std::shared_ptr<semantic_ir::SemanticDeclaration> method) mutable { return extend_wrapper_proto_line(type_name, context, method); }).join(mlc::String("", 0));
  }
}
mlc::String gen_extend_trait_wrapper(mlc::String type_name, mlc::String mangled, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept{
  auto wrapper_context = context::CodegenContext_for_type_body(context, type_name);
  auto method_name = extract_method_name(mangled, type_name);
  auto return_type_cpp = type_gen::sem_type_to_cpp(wrapper_context, return_type);
  auto fn_resolved = context_resolve_fn(context, mangled);
  auto parameters_string = gen_params_proto(wrapper_context, parameters);
  return expr::inline_noexcept_forwarding_call(return_type_cpp, cpp_naming::cpp_safe(method_name), parameters_string, fn_resolved, parameter_call_argument_names(parameters).join(mlc::String(", ", 2)));
}
mlc::String gen_extend_trait_wrappers(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept{
  return methods.map([=](std::shared_ptr<semantic_ir::SemanticDeclaration> method) mutable { return extend_trait_wrapper_line(type_name, context, context_resolve_fn, method); }).join(mlc::String("", 0));
}
mlc::String gen_extend_extern_method(mlc::String mangled, mlc::String type_name, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type, mlc::String trait_name, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept{
  auto method_name = ((trait_name.length() > 0) ? (extract_method_name(mangled, type_name)) : (mlc::String("", 0)));
  if ((((type_name == mlc::String("i32", 3)) && (method_name == mlc::String("to_string", 9))) && (parameters.length() > 0)))   {
    auto return_type_cpp = type_gen::sem_type_to_cpp(context, return_type);
    auto parameters_string = gen_params_def(context, parameters);
    return expr::noexcept_one_line_returning_function(return_type_cpp, context_resolve_fn(context, mangled), parameters_string, expr::runtime_to_string_call(cpp_naming::cpp_safe(parameters[0]->name)));
  } else   {
    return mlc::String("", 0);
  }
}
mlc::Array<mlc::String> gen_extend_method_parts(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext extend_context, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticDeclaration>, context::CodegenContext)> gen_decl_fn) noexcept{
  return methods.flat_map([=](std::shared_ptr<semantic_ir::SemanticDeclaration> method) mutable { return extend_method_part_strings(type_name, trait_name, extend_context, context, context_resolve_fn, gen_decl_fn, method); });
}
mlc::String gen_extend_trait_concept_adapter(mlc::String type_name, mlc::String trait_name, mlc::String mangled, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept{
  auto extend_context = context::CodegenContext_for_type_body(context, type_name);
  auto method_name = extract_method_name(mangled, type_name);
  auto trait_callee = cpp_naming::cpp_safe(((trait_name + mlc::String("_", 1)) + method_name));
  auto return_type_cpp = type_gen::sem_type_to_cpp(extend_context, return_type);
  auto parameters_string = gen_params_proto(extend_context, parameters);
  auto fn_resolved = context_resolve_fn(context, mangled);
  return expr::inline_noexcept_forwarding_call(return_type_cpp, trait_callee, parameters_string, fn_resolved, parameter_call_argument_names(parameters).join(mlc::String(", ", 2)));
}
mlc::String gen_extend_trait_concept_adapters(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept{
  return methods.map([=](std::shared_ptr<semantic_ir::SemanticDeclaration> method) mutable { return extend_trait_concept_adapter_line(type_name, trait_name, context, context_resolve_fn, method); }).join(mlc::String("", 0));
}
mlc::String gen_decl_extend_trait_suffix(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept{
  auto extend_context = context::CodegenContext_for_type_body(context, type_name);
  auto cpp_type = type_gen::type_name_to_cpp(context, type_name);
  return ((gen_extend_trait_wrappers(type_name, methods, context, context_resolve_fn) + gen_extend_trait_concept_adapters(type_name, trait_name, methods, context, context_resolve_fn)) + generic_trait_static_assert_line(trait_name, type_name, methods, extend_context, cpp_type));
}
mlc::String gen_decl_extend(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticDeclaration>, context::CodegenContext)> gen_decl_fn) noexcept{
  auto extend_context = context::CodegenContext_for_type_body(context, type_name);
  auto method_parts = gen_extend_method_parts(type_name, trait_name, methods, extend_context, context, context_resolve_fn, gen_decl_fn);
  auto methods_str = method_parts.join(mlc::String("", 0));
  if ((trait_name.length() > 0))   {
    return (methods_str + gen_decl_extend_trait_suffix(type_name, trait_name, methods, context, context_resolve_fn));
  } else   {
    return methods_str;
  }
}

} // namespace decl_extend
