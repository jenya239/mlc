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

mlc::Array<mlc::String> extend_method_part_strings(mlc::String type_name, mlc::String trait_name, context::CodegenContext extend_context, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticDeclaration>, context::CodegenContext)> gen_decl_fn, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept;

mlc::String trait_method_field_line(context::CodegenContext context, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type) noexcept;

mlc::String gen_trait_struct(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept;

bool trait_skips_concept_dispatch_forward_protos(mlc::String trait_name) noexcept;

mlc::String trait_concept_dispatch_forward_proto_line(mlc::String trait_name, mlc::String mangled, mlc::String return_type_cpp) noexcept;

mlc::String trait_concept_dispatch_forward_protos(context::CodegenContext context, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept;

mlc::String gen_trait_decl(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept;

mlc::String gen_trait_decl_with_dispatch_forward_protos(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept;

mlc::String gen_extend_wrapper_protos(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context) noexcept;

mlc::String gen_extend_trait_wrapper(mlc::String type_name, mlc::String mangled, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept;

mlc::String gen_extend_trait_wrappers(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept;

mlc::String gen_extend_extern_method(mlc::String mangled, mlc::String type_name, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type, mlc::String trait_name, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept;

mlc::Array<mlc::String> gen_extend_method_parts(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext extend_context, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticDeclaration>, context::CodegenContext)> gen_decl_fn) noexcept;

mlc::String gen_extend_trait_concept_adapter(mlc::String type_name, mlc::String trait_name, mlc::String mangled, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept;

mlc::String gen_extend_trait_concept_adapters(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept;

mlc::String gen_decl_extend_trait_suffix(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept;

mlc::String gen_decl_extend_methods_only(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticDeclaration>, context::CodegenContext)> gen_decl_fn) noexcept;

mlc::String gen_decl_extend(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticDeclaration>, context::CodegenContext)> gen_decl_fn) noexcept;

mlc::String trait_dispatch_name(mlc::String trait_name, mlc::String method_name) noexcept{return trait_name + mlc::String("_") + method_name;}

mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept{
mlc::String prefix = type_name + mlc::String("_");
return fn_name.length() > prefix.length() && fn_name.substring(0, prefix.length()) == prefix ? fn_name.substring(prefix.length(), fn_name.length() - prefix.length()) : fn_name;
}

mlc::String default_expr_to_cpp(std::shared_ptr<ast::Expr> expression) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprInt>((*expression)._)) { auto _v_exprint = std::get<ast::ExprInt>((*expression)._); auto [v, _w0] = _v_exprint; return literals::gen_integer_literal(v); } if (std::holds_alternative<ast::ExprStr>((*expression)._)) { auto _v_exprstr = std::get<ast::ExprStr>((*expression)._); auto [s, _w0] = _v_exprstr; return literals::gen_string_literal(s); } if (std::holds_alternative<ast::ExprBool>((*expression)._)) { auto _v_exprbool = std::get<ast::ExprBool>((*expression)._); auto [b, _w0] = _v_exprbool; return literals::gen_boolean_literal(b); } if (std::holds_alternative<ast::ExprUnit>((*expression)._)) { auto _v_exprunit = std::get<ast::ExprUnit>((*expression)._); auto [_w0] = _v_exprunit; return literals::gen_unit_literal(); } if (std::holds_alternative<ast::ExprFloat>((*expression)._)) { auto _v_exprfloat = std::get<ast::ExprFloat>((*expression)._); auto [v, _w0] = _v_exprfloat; return literals::gen_float_literal(v); } if (std::holds_alternative<ast::ExprI64>((*expression)._)) { auto _v_expri64 = std::get<ast::ExprI64>((*expression)._); auto [v, _w0] = _v_expri64; return literals::gen_i64_literal(v); } if (std::holds_alternative<ast::ExprU8>((*expression)._)) { auto _v_expru8 = std::get<ast::ExprU8>((*expression)._); auto [v, _w0] = _v_expru8; return literals::gen_u8_literal(v); } if (std::holds_alternative<ast::ExprUsize>((*expression)._)) { auto _v_exprusize = std::get<ast::ExprUsize>((*expression)._); auto [v, _w0] = _v_exprusize; return literals::gen_usize_literal(v); } if (std::holds_alternative<ast::ExprChar>((*expression)._)) { auto _v_exprchar = std::get<ast::ExprChar>((*expression)._); auto [v, _w0] = _v_exprchar; return literals::gen_char_literal(v); } return mlc::to_string(0); }();}

mlc::String generic_parameter_type_cpp(context::CodegenContext context, std::shared_ptr<ast::Param> parameter, mlc::String generic_type_name) noexcept{return generic_type_name == mlc::String("ref") ? type_gen::type_to_cpp(context, parameter->type_value) : parameter->is_mut ? expr::cpp_lvalue_reference_suffix(type_gen::type_to_cpp(context, parameter->type_value)) : type_gen::type_to_cpp(context, parameter->type_value);}

mlc::String non_generic_parameter_type_cpp(context::CodegenContext context, std::shared_ptr<ast::Param> parameter) noexcept{return parameter->is_mut ? expr::cpp_lvalue_reference_suffix(type_gen::type_to_cpp(context, parameter->type_value)) : type_gen::type_to_cpp(context, parameter->type_value);}

mlc::String parameter_type_cpp(context::CodegenContext context, std::shared_ptr<ast::Param> parameter) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::TyGeneric>((*parameter->type_value))) { auto _v_tygeneric = std::get<ast::TyGeneric>((*parameter->type_value)); auto [generic_type_name, _w0] = _v_tygeneric; return generic_parameter_type_cpp(context, parameter, generic_type_name); } return non_generic_parameter_type_cpp(context, parameter); }();}

mlc::String param_item_def(context::CodegenContext context, std::shared_ptr<ast::Param> parameter) noexcept{return expr::parameter_declaration_item(parameter_type_cpp(context, parameter), cpp_naming::cpp_safe(parameter->name));}

mlc::String param_item_proto(context::CodegenContext context, std::shared_ptr<ast::Param> parameter) noexcept{return parameter->has_default ? param_item_def(context, parameter) + mlc::String(" = ") + default_expr_to_cpp(parameter->default_) : param_item_def(context, parameter);}

mlc::String gen_params_proto(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{return gen_parameter_proto_items(context, parameters).join(mlc::String(", "));}

mlc::Array<mlc::String> gen_parameter_proto_items(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{return parameters.map([context](std::shared_ptr<ast::Param> parameter) mutable { return param_item_proto(context, parameter); });}

mlc::String gen_params_def(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{return gen_parameter_def_items(context, parameters).join(mlc::String(", "));}

mlc::Array<mlc::String> gen_parameter_def_items(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{return parameters.map([context](std::shared_ptr<ast::Param> parameter) mutable { return param_item_def(context, parameter); });}

mlc::Array<mlc::String> non_self_parameter_types_cpp(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{return parameters.filter([](std::shared_ptr<ast::Param> parameter) mutable { return parameter->name != mlc::String("self"); }).map([context](std::shared_ptr<ast::Param> parameter) mutable { return type_gen::type_to_cpp(context, parameter->type_value); });}

mlc::Array<mlc::String> parameter_call_argument_names(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{return parameters.map([](std::shared_ptr<ast::Param> parameter) mutable { return cpp_naming::cpp_safe(parameter->name); });}

mlc::String concept_declval_arguments_for_params(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{return non_self_parameter_types_cpp(context, parameters).map([](mlc::String type_code) mutable { return expr::std_declval_expression(type_code); }).join(mlc::String(", "));}

mlc::String extend_method_return_type_from_declaration(context::CodegenContext extend_context, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*method))) { auto _v_semanticdeclarationfn = std::get<semantic_ir::SemanticDeclarationFn>((*method)); auto [_w0, _w1, _w2, _w3, return_type, _w4, _w5, _w6] = _v_semanticdeclarationfn; return type_gen::sem_type_to_cpp(extend_context, return_type); } return mlc::String("void"); }();}

mlc::String extend_method_return_type_cpp(context::CodegenContext extend_context, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept{return methods.fold(mlc::String("void"), [extend_context](mlc::String accumulated, std::shared_ptr<semantic_ir::SemanticDeclaration> method) mutable { return accumulated != mlc::String("void") ? accumulated : extend_method_return_type_from_declaration(extend_context, method); });}

mlc::String trait_concept_cpp_name_for_static_assert(mlc::String trait_name) noexcept{return trait_name == mlc::String("ExprVisitor") ? mlc::String("expr_visitor::ExprVisitor") : trait_name == mlc::String("CompilerPass") ? mlc::String("pass::CompilerPass") : cpp_naming::cpp_safe(trait_name);}

mlc::String generic_trait_static_assert_line(mlc::String trait_name, mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext extend_context, mlc::String implementor_type_cpp) noexcept{return trait_name == mlc::String("ExprVisitor") ? expr::static_assert_concept_for_result_and_implementor_line(trait_concept_cpp_name_for_static_assert(trait_name), extend_method_return_type_cpp(extend_context, methods), implementor_type_cpp, type_name + mlc::String(" does not implement ") + trait_name) : expr::static_assert_concept_for_type_line(trait_concept_cpp_name_for_static_assert(trait_name), implementor_type_cpp, type_name + mlc::String(" does not implement ") + trait_name);}

mlc::String trait_struct_field_line_for_method(context::CodegenContext trait_context, mlc::String trait_name, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*method))) { auto _v_semanticdeclarationfn = std::get<semantic_ir::SemanticDeclarationFn>((*method)); auto [mangled, _w0, _w1, parameters, return_type, _w2, _w3, _w4] = _v_semanticdeclarationfn; return trait_method_field_line(trait_context, extract_method_name(mangled, trait_name), parameters, return_type); } return mlc::String(""); }();}

mlc::Array<mlc::String> trait_requires_requirement_parts(context::CodegenContext trait_context, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept{return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*method))) { auto _v_semanticdeclarationfn = std::get<semantic_ir::SemanticDeclarationFn>((*method)); auto [mangled, _w0, _w1, parameters, return_type, _w2, _w3, _w4] = _v_semanticdeclarationfn; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> requirement_parts = {};
  requirement_parts.push_back(expr::concept_requires_expression_method_returns_convertible(cpp_naming::cpp_safe(trait_dispatch_name(trait_context.self_type, extract_method_name(mangled, trait_context.self_type))), concept_declval_arguments_for_params(trait_context, parameters), type_gen::sem_type_to_cpp(trait_context, return_type)));
  return requirement_parts;
 }(); } return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty_requirements = {};
  return empty_requirements;
 }(); }();}

mlc::String extend_wrapper_proto_line(mlc::String type_name, context::CodegenContext context, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*method))) { auto _v_semanticdeclarationfn = std::get<semantic_ir::SemanticDeclarationFn>((*method)); auto [mangled, _w0, _w1, parameters, return_type, _w2, _w3, _w4] = _v_semanticdeclarationfn; return [&]() -> mlc::String { 
  context::CodegenContext wrapper_context = context::CodegenContext_for_type_body(context, type_name);
  return expr::noexcept_function_prototype(type_gen::sem_type_to_cpp(wrapper_context, return_type), cpp_naming::cpp_safe(extract_method_name(mangled, type_name)), gen_params_proto(wrapper_context, parameters));
 }(); } return mlc::String(""); }();}

mlc::String extend_trait_wrapper_line(mlc::String type_name, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*method))) { auto _v_semanticdeclarationfn = std::get<semantic_ir::SemanticDeclarationFn>((*method)); auto [mangled, _w0, _w1, parameters, return_type, _w2, _w3, _w4] = _v_semanticdeclarationfn; return gen_extend_trait_wrapper(type_name, mangled, parameters, return_type, context, context_resolve_fn); } return mlc::String(""); }();}

mlc::String extend_trait_concept_adapter_line(mlc::String type_name, mlc::String trait_name, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*method))) { auto _v_semanticdeclarationfn = std::get<semantic_ir::SemanticDeclarationFn>((*method)); auto [mangled, _w0, _w1, parameters, return_type, _w2, _w3, _w4] = _v_semanticdeclarationfn; return gen_extend_trait_concept_adapter(type_name, trait_name, mangled, parameters, return_type, context, context_resolve_fn); } return mlc::String(""); }();}

mlc::Array<mlc::String> extend_method_part_strings(mlc::String type_name, mlc::String trait_name, context::CodegenContext extend_context, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticDeclaration>, context::CodegenContext)> gen_decl_fn, std::shared_ptr<semantic_ir::SemanticDeclaration> method) noexcept{return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*method))) { auto _v_semanticdeclarationfn = std::get<semantic_ir::SemanticDeclarationFn>((*method)); auto [mangled, _w0, _w1, parameters, return_type, body, _w2, _w3] = _v_semanticdeclarationfn; return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<semantic_ir::SemanticExpressionExtern>((*body)._)) { auto _v_semanticexpressionextern = std::get<semantic_ir::SemanticExpressionExtern>((*body)._); auto [_w0, _w1] = _v_semanticexpressionextern; return [&]() -> mlc::Array<mlc::String> { 
  mlc::String generated = gen_extend_extern_method(mangled, type_name, parameters, return_type, trait_name, context, context_resolve_fn);
  return generated.length() > 0 ? [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> extern_parts = {};
  extern_parts.push_back(generated);
  return extern_parts;
 }() : [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty_extern_parts = {};
  return empty_extern_parts;
 }();
 }(); } return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> body_parts = {};
  body_parts.push_back(gen_decl_fn(method, extend_context));
  return body_parts;
 }(); }(); } return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> fallback_parts = {};
  fallback_parts.push_back(gen_decl_fn(method, extend_context));
  return fallback_parts;
 }(); }();}

mlc::String trait_method_field_line(context::CodegenContext context, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type) noexcept{
mlc::Array<mlc::String> parameter_types = non_self_parameter_types_cpp(context, parameters);
mlc::String return_cpp = type_gen::sem_type_to_cpp(context, return_type);
return expr::std_function_field_line(return_cpp, parameter_types.join(mlc::String(", ")), cpp_naming::cpp_safe(method_name));
}

mlc::String gen_trait_struct(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept{return context::trait_has_associated_types(context, trait_name) ? mlc::String("") : [&]() -> mlc::String { 
  context::CodegenContext trait_context = context::CodegenContext_for_type_body(context, trait_name);
  mlc::String template_header = type_parameters.size() > 0 ? expr::cpp_template_typename_header_line(type_parameters.join(mlc::String(", "))) : mlc::String("");
  mlc::Array<mlc::String> field_lines = methods.map([trait_context, trait_name](std::shared_ptr<semantic_ir::SemanticDeclaration> method) mutable { return trait_struct_field_line_for_method(trait_context, trait_name, method); }).filter([](mlc::String line) mutable { return line.length() > 0; });
  return expr::trait_struct_definition_lines(template_header, expr::trait_vtable_struct_cpp_name(cpp_naming::cpp_safe(trait_name)), field_lines.join(mlc::String("")));
 }();}

bool trait_skips_concept_dispatch_forward_protos(mlc::String trait_name) noexcept{return trait_name == mlc::String("ExprVisitor") || trait_name == mlc::String("CompilerPass");}

mlc::String trait_concept_dispatch_forward_proto_line(mlc::String trait_name, mlc::String mangled, mlc::String return_type_cpp) noexcept{return mlc::String("template<typename Self>\n") + return_type_cpp + mlc::String(" ") + cpp_naming::cpp_safe(trait_dispatch_name(trait_name, extract_method_name(mangled, trait_name))) + mlc::String("(const Self& self) noexcept;\n");}

mlc::String trait_concept_dispatch_forward_protos(context::CodegenContext context, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept{return trait_skips_concept_dispatch_forward_protos(trait_name) ? mlc::String("") : [&]() -> mlc::String { 
  context::CodegenContext trait_context = context::CodegenContext_for_type_body(context, trait_name);
  mlc::String forward_protos = mlc::String("");
  int method_index = 0;
  while (method_index < methods.size()){
{
[&]() -> void { if (std::holds_alternative<semantic_ir::SemanticDeclarationFn>((*methods[method_index]))) { auto _v_semanticdeclarationfn = std::get<semantic_ir::SemanticDeclarationFn>((*methods[method_index])); auto [mangled, _w0, _w1, parameters, return_type, _w2, _w3, _w4] = _v_semanticdeclarationfn; if (parameters.size() > 0 && parameters[0]->name == mlc::String("self")){
forward_protos = forward_protos + trait_concept_dispatch_forward_proto_line(trait_name, mangled, type_gen::sem_type_to_cpp(trait_context, return_type));
}; return; }  }();
method_index = method_index + 1;
}
}
  return forward_protos;
 }();}

mlc::String gen_trait_decl(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept{return context::trait_has_associated_types(context, trait_name) ? mlc::String("") : [&]() -> mlc::String { 
  context::CodegenContext trait_context = context::CodegenContext_for_type_body(context, trait_name);
  mlc::String template_header = type_parameters.size() > 0 ? mlc::String("template<typename ") + type_parameters[0] + mlc::String(", typename Self>\n") : mlc::String("template<typename Self>\n");
  mlc::String req_body = mlc::collections::flat_map(methods, [trait_context](std::shared_ptr<semantic_ir::SemanticDeclaration> method) mutable { return trait_requires_requirement_parts(trait_context, method); }).join(mlc::String("; "));
  return expr::trait_concept_requires_definition_line(template_header, cpp_naming::cpp_safe(trait_name), mlc::String("Self"), req_body);
 }();}

mlc::String gen_trait_decl_with_dispatch_forward_protos(context::CodegenContext context, mlc::String trait_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods) noexcept{return trait_concept_dispatch_forward_protos(context, trait_name, methods) + gen_trait_decl(context, trait_name, type_parameters, methods);}

mlc::String gen_extend_wrapper_protos(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context) noexcept{return trait_name.length() == 0 ? mlc::String("") : methods.map([type_name, context](std::shared_ptr<semantic_ir::SemanticDeclaration> method) mutable { return extend_wrapper_proto_line(type_name, context, method); }).join(mlc::String(""));}

mlc::String gen_extend_trait_wrapper(mlc::String type_name, mlc::String mangled, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept{
context::CodegenContext wrapper_context = context::CodegenContext_for_type_body(context, type_name);
mlc::String method_name = extract_method_name(mangled, type_name);
mlc::String return_type_cpp = type_gen::sem_type_to_cpp(wrapper_context, return_type);
mlc::String fn_resolved = context_resolve_fn(context, mangled);
mlc::String parameters_string = gen_params_proto(wrapper_context, parameters);
return expr::inline_noexcept_forwarding_call(return_type_cpp, cpp_naming::cpp_safe(method_name), parameters_string, fn_resolved, parameter_call_argument_names(parameters).join(mlc::String(", ")));
}

mlc::String gen_extend_trait_wrappers(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept{return methods.map([type_name, context, context_resolve_fn](std::shared_ptr<semantic_ir::SemanticDeclaration> method) mutable { return extend_trait_wrapper_line(type_name, context, context_resolve_fn, method); }).join(mlc::String(""));}

mlc::String gen_extend_extern_method(mlc::String mangled, mlc::String type_name, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type, mlc::String trait_name, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept{
mlc::String method_name = trait_name.length() > 0 ? extract_method_name(mangled, type_name) : mlc::String("");
return type_name == mlc::String("i32") && method_name == mlc::String("to_string") && parameters.size() > 0 ? [&]() -> mlc::String { 
  mlc::String return_type_cpp = type_gen::sem_type_to_cpp(context, return_type);
  mlc::String parameters_string = gen_params_def(context, parameters);
  return expr::noexcept_one_line_returning_function(return_type_cpp, context_resolve_fn(context, mangled), parameters_string, expr::runtime_to_string_call(cpp_naming::cpp_safe(parameters[0]->name)));
 }() : mlc::String("");
}

mlc::Array<mlc::String> gen_extend_method_parts(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext extend_context, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticDeclaration>, context::CodegenContext)> gen_decl_fn) noexcept{return mlc::collections::flat_map(methods, [type_name, trait_name, extend_context, context, context_resolve_fn, gen_decl_fn](std::shared_ptr<semantic_ir::SemanticDeclaration> method) mutable { return extend_method_part_strings(type_name, trait_name, extend_context, context, context_resolve_fn, gen_decl_fn, method); });}

mlc::String gen_extend_trait_concept_adapter(mlc::String type_name, mlc::String trait_name, mlc::String mangled, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<registry::Type> return_type, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept{
context::CodegenContext extend_context = context::CodegenContext_for_type_body(context, type_name);
mlc::String method_name = extract_method_name(mangled, type_name);
mlc::String trait_callee = cpp_naming::cpp_safe(trait_name + mlc::String("_") + method_name);
mlc::String return_type_cpp = type_gen::sem_type_to_cpp(extend_context, return_type);
mlc::String parameters_string = gen_params_proto(extend_context, parameters);
mlc::String fn_resolved = context_resolve_fn(context, mangled);
return expr::inline_noexcept_forwarding_call(return_type_cpp, trait_callee, parameters_string, fn_resolved, parameter_call_argument_names(parameters).join(mlc::String(", ")));
}

mlc::String gen_extend_trait_concept_adapters(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept{return methods.map([type_name, trait_name, context, context_resolve_fn](std::shared_ptr<semantic_ir::SemanticDeclaration> method) mutable { return extend_trait_concept_adapter_line(type_name, trait_name, context, context_resolve_fn, method); }).join(mlc::String(""));}

mlc::String gen_decl_extend_trait_suffix(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn) noexcept{return context::trait_has_associated_types(context, trait_name) ? gen_extend_trait_wrappers(type_name, methods, context, context_resolve_fn) : [&]() -> mlc::String { 
  context::CodegenContext extend_context = context::CodegenContext_for_type_body(context, type_name);
  mlc::String cpp_type = type_gen::type_name_to_cpp(context, type_name);
  return gen_extend_trait_wrappers(type_name, methods, context, context_resolve_fn) + gen_extend_trait_concept_adapters(type_name, trait_name, methods, context, context_resolve_fn) + generic_trait_static_assert_line(trait_name, type_name, methods, extend_context, cpp_type);
 }();}

mlc::String gen_decl_extend_methods_only(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticDeclaration>, context::CodegenContext)> gen_decl_fn) noexcept{
context::CodegenContext extend_context = context::CodegenContext_for_type_body(context, type_name);
return gen_extend_method_parts(type_name, trait_name, methods, extend_context, context, context_resolve_fn, gen_decl_fn).join(mlc::String(""));
}

mlc::String gen_decl_extend(mlc::String type_name, mlc::String trait_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticDeclaration>> methods, context::CodegenContext context, std::function<mlc::String(context::CodegenContext, mlc::String)> context_resolve_fn, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticDeclaration>, context::CodegenContext)> gen_decl_fn) noexcept{
mlc::String methods_str = gen_decl_extend_methods_only(type_name, trait_name, methods, context, context_resolve_fn, gen_decl_fn);
return trait_name.length() > 0 ? methods_str + gen_decl_extend_trait_suffix(type_name, trait_name, methods, context, context_resolve_fn) : methods_str;
}

} // namespace decl_extend
