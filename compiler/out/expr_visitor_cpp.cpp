#define main mlc_user_main
#include "expr_visitor_cpp.hpp"

#include "registry.hpp"
#include "semantic_ir.hpp"
#include "cpp_ast.hpp"
#include "print.hpp"
#include "emit_helpers.hpp"
#include "semantic_type_structure.hpp"
#include "literals.hpp"
#include "identifiers.hpp"
#include "expression_support.hpp"
#include "result_option_method_types.hpp"
#include "match_analysis.hpp"
#include "match_gen.hpp"
#include "mut_actual_argument.hpp"
#include "method_gen.hpp"
#include "record_gen.hpp"
#include "cpp_naming.hpp"
#include "type_gen.hpp"
#include "expr_visitor.hpp"
#include "context.hpp"
#include "statement_context.hpp"

namespace expr_visitor_cpp {

using namespace registry;
using namespace semantic_ir;
using namespace cpp_ast;
using namespace print;
using namespace emit_helpers;
using namespace semantic_type_structure;
using namespace literals;
using namespace identifiers;
using namespace expression_support;
using namespace result_option_method_types;
using namespace match_analysis;
using namespace match_gen;
using namespace mut_actual_argument;
using namespace method_gen;
using namespace record_gen;
using namespace cpp_naming;
using namespace type_gen;
using namespace expr_visitor;
using namespace context;
using namespace statement_context;

std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_int(CodegenPass self, int value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_str(CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_bool(CodegenPass self, bool value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_unit(CodegenPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_float(CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_i64(CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_u8(CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_usize(CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_char(CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_extern(CodegenPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_ident(CodegenPass self, mlc::String name, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_bin(CodegenPass self, mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> left_expression, std::shared_ptr<semantic_ir::SemanticExpression> right_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_un(CodegenPass self, mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_call(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> mutability_flags, std::shared_ptr<registry::Type> semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_method(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_field(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String field_name, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_index(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_if(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_block(CodegenPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_while(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_for(CodegenPass self, mlc::String variable, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_match(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_record(CodegenPass self, mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_record_update(CodegenPass self, mlc::String type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_array(CodegenPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_tuple(CodegenPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_question(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_lambda(CodegenPass self, mlc::Array<mlc::String> parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body_expression, std::shared_ptr<registry::Type> semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_with(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_unsupported(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> _expression) noexcept;
CodegenPass codegen_pass_new(context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept{
  return CodegenPass{context, gen_stmts, evaluate_expression, 0};
}
std::shared_ptr<cpp_ast::CppExpression> unsupported_cpp_expression() noexcept{
  return emit_helpers::make_identifier_cpp_expression(mlc::String("/* unsupported expr */", 22));
}
std::shared_ptr<cpp_ast::CppExpression> evaluate_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> _gen_stmts) noexcept{
  return gen_expr_via_cpp_visitor(expression, context);
}
mlc::String ignored_gen_stmts(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> _statements, context::CodegenContext _context) noexcept{
  return mlc::String("", 0);
}
std::shared_ptr<cpp_ast::CppExpression> eval_expr_cpp(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
  return eval_expr_with_cpp_visitor(expression, context, gen_stmts, eval_expr_cpp);
}
std::shared_ptr<cpp_ast::CppExpression> gen_call_function_cpp_expression(std::shared_ptr<semantic_ir::SemanticExpression> function_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
  return [&]() -> std::shared_ptr<cpp_ast::CppExpression> {
auto __match_subject = function_expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*__match_subject))) {
const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent = std::get<semantic_ir::SemanticExpressionIdent>((*__match_subject));
auto [name, __1, __2] = semanticExpressionIdent; return emit_helpers::make_identifier_cpp_expression(context::qualify_function_callee(cpp_naming::map_builtin(name), context));
}
return eval_expr_cpp(function_expression, context, gen_stmts);
std::abort();
}();
}
mlc::String result_wrapper_cpp_type_argument(mlc::String wrapped_cpp_type) noexcept{
  if (((wrapped_cpp_type == mlc::String("auto", 4)) || (wrapped_cpp_type.length() == 0)))   {
    return mlc::String("", 0);
  } else   {
    return ((mlc::String("<", 1) + mlc::to_string(wrapped_cpp_type)) + mlc::String(">", 1));
  }
}
int type_parameter_name_index(mlc::Array<mlc::String> parameter_names, mlc::String parameter_name) noexcept{
  auto index = 0;
  while ((index < parameter_names.length()))   {
    if ((parameter_names[index] == parameter_name))     {
      return index;
    }
    (index = (index + 1));
  }
  return (-1);
}
mlc::String generic_sum_variant_ctor_type_argument_from_enclosing(mlc::String owner_name, mlc::Array<mlc::String> all_parameter_names, mlc::Array<mlc::String> used_parameter_names, mlc::Array<std::shared_ptr<registry::Type>> type_arguments, context::CodegenContext context) noexcept{
  auto cpp_argument_parts = mlc::Array<mlc::String>{};
  auto used_index = 0;
  while ((used_index < used_parameter_names.length()))   {
    auto parameter_index = type_parameter_name_index(all_parameter_names, used_parameter_names[used_index]);
    if (((parameter_index < 0) || (parameter_index >= type_arguments.length())))     {
      return mlc::String("", 0);
    }
    if (semantic_type_structure::type_is_unknown(type_arguments[parameter_index]))     {
      return mlc::String("", 0);
    }
    cpp_argument_parts.push_back(type_gen::sem_type_to_cpp(context, type_arguments[parameter_index]));
    (used_index = (used_index + 1));
  }
  if ((cpp_argument_parts.length() == 0))   {
    return mlc::String("", 0);
  } else   {
    return ((mlc::String("<", 1) + mlc::to_string(cpp_argument_parts.join(mlc::String(", ", 2)))) + mlc::String(">", 1));
  }
}
mlc::String owner_type_name_from_semantic_type(std::shared_ptr<registry::Type> type_value) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = type_value;
if (std::holds_alternative<registry::TNamed>((*__match_subject))) {
const registry::TNamed& tNamed = std::get<registry::TNamed>((*__match_subject));
auto [type_name] = tNamed; return type_name;
}
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [type_name, __1] = tGeneric; return type_name;
}
return mlc::String("", 0);
std::abort();
}();
}
mlc::String generic_sum_variant_ctor_type_argument_for_enclosing(mlc::String owner_name, mlc::String enclosing_name, mlc::Array<std::shared_ptr<registry::Type>> type_arguments, mlc::String constructor_name, context::CodegenContext context) noexcept{
  if (((owner_name == mlc::String("", 0)) || (enclosing_name != owner_name)))   {
    return mlc::String("", 0);
  } else if ((context.variant_used_type_parameter_names.has(constructor_name) && context.sum_type_parameter_names.has(owner_name)))   {
    return generic_sum_variant_ctor_type_argument_from_enclosing(owner_name, context.sum_type_parameter_names.get(owner_name), context.variant_used_type_parameter_names.get(constructor_name), type_arguments, context);
  } else if (((type_arguments.length() == 1) && (!semantic_type_structure::type_is_unknown(type_arguments[0]))))   {
    return result_wrapper_cpp_type_argument(type_gen::sem_type_to_cpp(context, type_arguments[0]));
  } else   {
    return mlc::String("", 0);
  }
}
mlc::String generic_sum_variant_ctor_type_argument(mlc::String constructor_name, std::shared_ptr<registry::Type> call_semantic_type, context::CodegenContext context) noexcept{
  auto owner_name = owner_type_name_from_semantic_type(call_semantic_type);
  if (((owner_name == mlc::String("", 0)) && context.variant_types.has(constructor_name)))   {
    (owner_name = context.variant_types.get(constructor_name));
  }
  auto from_enclosing = [&]() -> mlc::String {
auto __match_subject = context.enclosing_function_return_type;
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [enclosing_name, type_arguments] = tGeneric; return generic_sum_variant_ctor_type_argument_for_enclosing(owner_name, enclosing_name, type_arguments, constructor_name, context);
}
return mlc::String("", 0);
std::abort();
}();
  if ((from_enclosing.length() > 0))   {
    return from_enclosing;
  } else   {
    return [&]() -> mlc::String {
auto __match_subject = call_semantic_type;
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [call_owner_name, type_arguments] = tGeneric; return generic_sum_variant_ctor_type_argument_for_enclosing(call_owner_name, call_owner_name, type_arguments, constructor_name, context);
}
return mlc::String("", 0);
std::abort();
}();
  }
}
mlc::String variant_constructor_template_suffix(mlc::String constructor_name, std::shared_ptr<registry::Type> call_semantic_type, context::CodegenContext context) noexcept{
  auto result_wrapper = result_wrapper_ctor_type_argument(constructor_name, context);
  if ((result_wrapper.length() > 0))   {
    return result_wrapper;
  } else   {
    return generic_sum_variant_ctor_type_argument(constructor_name, call_semantic_type, context);
  }
}
mlc::String result_wrapper_ctor_type_argument(mlc::String constructor_name, context::CodegenContext context) noexcept{
  auto enclosing_type = context.enclosing_function_return_type;
  if ((!result_option_method_types::is_result_generic(enclosing_type)))   {
    return mlc::String("", 0);
  } else if ((constructor_name == mlc::String("Ok", 2)))   {
    return result_wrapper_cpp_type_argument(type_gen::sem_type_to_cpp(context, result_option_method_types::result_ok_type(enclosing_type)));
  } else if ((constructor_name == mlc::String("Err", 3)))   {
    return result_wrapper_cpp_type_argument(type_gen::sem_type_to_cpp(context, result_option_method_types::result_err_type(enclosing_type)));
  } else   {
    return mlc::String("", 0);
  }
}
std::shared_ptr<cpp_ast::CppExpression> gen_constructor_call_cpp_expression(std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> argument_expressions, std::shared_ptr<registry::Type> call_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
  return [&]() -> std::shared_ptr<cpp_ast::CppExpression> {
auto __match_subject = function_expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*__match_subject))) {
const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent = std::get<semantic_ir::SemanticExpressionIdent>((*__match_subject));
auto [name, __1, __2] = semanticExpressionIdent; return [&]() {
auto callee_code = (context::qualify_function_callee(cpp_naming::map_builtin(name), context) + variant_constructor_template_suffix(name, call_semantic_type, context));
return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppAggregateInit{callee_code, argument_expressions});
}();
}
return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall{gen_call_function_cpp_expression(function_expression, context, gen_stmts), argument_expressions});
std::abort();
}();
}
std::shared_ptr<cpp_ast::CppExpression> gen_call_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> call_parameter_mutability_flags, std::shared_ptr<registry::Type> call_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
  auto materialization_outcome = mut_actual_argument::materialize_positional_actual_arguments_cpp(arguments, call_parameter_mutability_flags, context, gen_stmts, eval_expr_cpp);
  auto invoke_expression = (expression_support::is_constructor_call(function_expression) ? (gen_constructor_call_cpp_expression(function_expression, materialization_outcome.materialized_argument_expressions, call_semantic_type, context, gen_stmts)) : (std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall{gen_call_function_cpp_expression(function_expression, context, gen_stmts), materialization_outcome.materialized_argument_expressions})));
  return mut_actual_argument::wrap_invoke_with_mut_prelude_cpp(materialization_outcome.prelude_statements, invoke_expression);
}
std::shared_ptr<cpp_ast::CppExpression> gen_method_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> receiver_and_parameter_mutability_flags, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
  return method_gen::gen_method_expr_cpp(object, method_name, arguments, receiver_and_parameter_mutability_flags, context, gen_stmts, eval_expr_cpp);
}
std::shared_ptr<cpp_ast::CppCapture> reference_capture_for_immediate_lambda() noexcept{
  return std::make_shared<cpp_ast::CppCapture>(cpp_ast::CppCapture{mlc::String("", 0), true});
}
std::shared_ptr<cpp_ast::CppCapture> value_capture_for_lambda() noexcept{
  return std::make_shared<cpp_ast::CppCapture>(cpp_ast::CppCapture{mlc::String("=", 1), false});
}
std::shared_ptr<cpp_ast::CppExpression> invoke_immediate_lambda_cpp(std::shared_ptr<cpp_ast::CppType> return_type, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> body_statements) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppLambda{mlc::Array<std::shared_ptr<cpp_ast::CppCapture>>{reference_capture_for_immediate_lambda()}, {}, return_type, body_statements}), {}});
}
std::shared_ptr<cpp_ast::CppExpression> gen_if_else_default_cpp(std::shared_ptr<semantic_ir::SemanticExpression> else_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
  return eval_expr_cpp(else_expression, context, gen_stmts);
}
std::shared_ptr<registry::Type> empty_array_inner_type_for_if_else(std::shared_ptr<registry::Type> if_semantic_type, std::shared_ptr<semantic_ir::SemanticExpression> then_expression) noexcept{
  auto inner_preferred = semantic_type_structure::array_element_type_from_array_type(if_semantic_type);
  auto inner_from_then_branch = semantic_type_structure::array_element_type_from_array_type(semantic_ir::sexpr_type(then_expression));
  if ((!semantic_type_structure::type_is_unknown(inner_preferred)))   {
    return inner_preferred;
  }
  return inner_from_then_branch;
}
std::shared_ptr<cpp_ast::CppExpression> gen_if_else_for_array_else(std::shared_ptr<semantic_ir::SemanticExpression> else_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, ast::Span array_span, std::shared_ptr<registry::Type> if_semantic_type, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
  if ((elements.length() > 0))   {
    return eval_expr_cpp(else_expression, context, gen_stmts);
  }
  auto inner_for_empty_array = empty_array_inner_type_for_if_else(if_semantic_type, then_expression);
  if (semantic_type_structure::type_is_unknown(inner_for_empty_array))   {
    return eval_expr_cpp(else_expression, context, gen_stmts);
  }
  return eval_expr_cpp(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionArray{elements, std::make_shared<registry::Type>(registry::TArray{inner_for_empty_array}), array_span}), context, gen_stmts);
}
std::shared_ptr<cpp_ast::CppExpression> gen_if_else_cpp_expression(std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> if_semantic_type, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
  return [&]() -> std::shared_ptr<cpp_ast::CppExpression> {
auto __match_subject = else_expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionArray>((*__match_subject))) {
const semantic_ir::SemanticExpressionArray& semanticExpressionArray = std::get<semantic_ir::SemanticExpressionArray>((*__match_subject));
auto [elements, __1, array_span] = semanticExpressionArray; return gen_if_else_for_array_else(else_expression, elements, array_span, if_semantic_type, then_expression, context, gen_stmts);
}
return gen_if_else_default_cpp(else_expression, context, gen_stmts);
std::abort();
}();
}
bool semantic_type_is_unit(std::shared_ptr<registry::Type> semantic_type) noexcept{
  return [&]() -> bool {
auto __match_subject = semantic_type;
if (std::holds_alternative<registry::TUnit>((*__match_subject))) {
const registry::TUnit& tUnit = std::get<registry::TUnit>((*__match_subject));
return true;
}
return false;
std::abort();
}();
}
bool cpp_expression_is_inline(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept{
  return [&]() -> bool {
auto __match_subject = expression;
if (std::holds_alternative<cpp_ast::CppInvokedBlock>((*__match_subject))) {
const cpp_ast::CppInvokedBlock& cppInvokedBlock = std::get<cpp_ast::CppInvokedBlock>((*__match_subject));
auto [__0] = cppInvokedBlock; return false;
}
if (std::holds_alternative<cpp_ast::CppInvokedBlockWithReturn>((*__match_subject))) {
const cpp_ast::CppInvokedBlockWithReturn& cppInvokedBlockWithReturn = std::get<cpp_ast::CppInvokedBlockWithReturn>((*__match_subject));
auto [__0, __1] = cppInvokedBlockWithReturn; return false;
}
if (std::holds_alternative<cpp_ast::CppInvokedWhile>((*__match_subject))) {
const cpp_ast::CppInvokedWhile& cppInvokedWhile = std::get<cpp_ast::CppInvokedWhile>((*__match_subject));
auto [__0, __1] = cppInvokedWhile; return false;
}
if (std::holds_alternative<cpp_ast::CppInvokedFor>((*__match_subject))) {
const cpp_ast::CppInvokedFor& cppInvokedFor = std::get<cpp_ast::CppInvokedFor>((*__match_subject));
auto [__0, __1, __2] = cppInvokedFor; return false;
}
return true;
std::abort();
}();
}
bool if_branch_type_matches_result(std::shared_ptr<semantic_ir::SemanticExpression> branch_expression, mlc::String result_cpp, context::CodegenContext context) noexcept{
  return (type_gen::sem_type_to_cpp(context, semantic_ir::sexpr_type(branch_expression)) == result_cpp);
}
mlc::String if_branch_constructor_name(std::shared_ptr<semantic_ir::SemanticExpression> branch_expression) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = branch_expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionCall>((*__match_subject))) {
const semantic_ir::SemanticExpressionCall& semanticExpressionCall = std::get<semantic_ir::SemanticExpressionCall>((*__match_subject));
auto [function_expression, __1, __2, __3, __4] = semanticExpressionCall; return [&]() -> mlc::String {
auto __match_subject = function_expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*__match_subject))) {
const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent = std::get<semantic_ir::SemanticExpressionIdent>((*__match_subject));
auto [name, __1, __2] = semanticExpressionIdent; return (expression_support::is_constructor_call(function_expression) ? (name) : (mlc::String("", 0)));
}
return mlc::String("", 0);
std::abort();
}();
}
return mlc::String("", 0);
std::abort();
}();
}
bool if_ternary_is_safe(std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> if_semantic_type, context::CodegenContext context) noexcept{
  auto then_ctor = if_branch_constructor_name(then_expression);
  auto else_ctor = if_branch_constructor_name(else_expression);
  if ((((then_ctor.length() > 0) || (else_ctor.length() > 0)) && (then_ctor != else_ctor)))   {
    return false;
  } else   {
    auto result_cpp = match_gen::expression_result_cpp_type_for_codegen(context, if_semantic_type);
    if (((result_cpp == mlc::String("auto", 4)) || (result_cpp.length() == 0)))     {
      return true;
    } else     {
      return (if_branch_type_matches_result(then_expression, result_cpp, context) && if_branch_type_matches_result(else_expression, result_cpp, context));
    }
  }
}
std::shared_ptr<cpp_ast::CppExpression> gen_if_non_unit_result_cpp(std::shared_ptr<cpp_ast::CppExpression> condition_expression, std::shared_ptr<cpp_ast::CppExpression> then_expression_cpp, std::shared_ptr<cpp_ast::CppExpression> else_expression_cpp, bool ternary_is_safe, mlc::String result_cpp, context::CodegenContext context) noexcept{
  if (((ternary_is_safe && cpp_expression_is_inline(then_expression_cpp)) && cpp_expression_is_inline(else_expression_cpp)))   {
    return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppTernary{condition_expression, then_expression_cpp, else_expression_cpp});
  }
  auto return_type = ((result_cpp.length() > 0) ? (result_cpp) : (mlc::String("auto", 4)));
  return invoke_immediate_lambda_cpp(std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeName{return_type}), mlc::Array{emit_helpers::make_if_cpp_statement(condition_expression, emit_helpers::make_block_cpp_statement(mlc::Array{emit_helpers::make_return_cpp_statement(then_expression_cpp)}), emit_helpers::make_block_cpp_statement(mlc::Array{emit_helpers::make_return_cpp_statement(else_expression_cpp)}))});
}
mlc::String unit_if_block_trailing(std::shared_ptr<semantic_ir::SemanticExpression> result_expression, context::CodegenContext final_context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = result_expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*__match_subject))) {
const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit = std::get<semantic_ir::SemanticExpressionUnit>((*__match_subject));
auto [__0, __1] = semanticExpressionUnit; return mlc::String("", 0);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*__match_subject))) {
const semantic_ir::SemanticExpressionIf& semanticExpressionIf = std::get<semantic_ir::SemanticExpressionIf>((*__match_subject));
auto [condition, then_expression, else_expression, semantic_type, __4] = semanticExpressionIf; return print::print_statement(gen_if_cpp_statement(condition, then_expression, else_expression, semantic_type, final_context, gen_stmts));
}
return (print::print_expr(eval_expr_cpp(result_expression, final_context, gen_stmts)) + mlc::String(";\n", 2));
std::abort();
}();
}
std::shared_ptr<cpp_ast::CppStatement> unit_if_branch_block(std::shared_ptr<semantic_ir::SemanticExpression> branch_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
  return [&]() -> std::shared_ptr<cpp_ast::CppStatement> {
auto __match_subject = branch_expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*__match_subject))) {
const semantic_ir::SemanticExpressionBlock& semanticExpressionBlock = std::get<semantic_ir::SemanticExpressionBlock>((*__match_subject));
auto [statements, result_expression, __2, __3] = semanticExpressionBlock; return [&]() {
auto body_fragment = gen_stmts(statements, context);
auto final_context = statement_context::stmts_final_ctx(statements, context);
auto trailing = unit_if_block_trailing(result_expression, final_context, gen_stmts);
return emit_helpers::make_block_cpp_statement(mlc::Array{emit_helpers::make_fragment_cpp_statement((body_fragment + trailing))});
}();
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*__match_subject))) {
const semantic_ir::SemanticExpressionIf& semanticExpressionIf = std::get<semantic_ir::SemanticExpressionIf>((*__match_subject));
auto [condition, then_expression, else_expression, semantic_type, __4] = semanticExpressionIf; return emit_helpers::make_block_cpp_statement(mlc::Array{emit_helpers::make_expression_cpp_statement(gen_if_via_cpp_visitor(condition, then_expression, else_expression, semantic_type, context, gen_stmts))});
}
return emit_helpers::make_block_cpp_statement(mlc::Array{emit_helpers::make_expression_cpp_statement(eval_expr_cpp(branch_expression, context, gen_stmts))});
std::abort();
}();
}
std::shared_ptr<cpp_ast::CppStatement> unit_if_else_branch(std::shared_ptr<semantic_ir::SemanticExpression> else_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
  return [&]() -> std::shared_ptr<cpp_ast::CppStatement> {
auto __match_subject = else_expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*__match_subject))) {
const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit = std::get<semantic_ir::SemanticExpressionUnit>((*__match_subject));
auto [__0, __1] = semanticExpressionUnit; return emit_helpers::make_block_cpp_statement({});
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*__match_subject))) {
const semantic_ir::SemanticExpressionIf& semanticExpressionIf = std::get<semantic_ir::SemanticExpressionIf>((*__match_subject));
auto [condition, then_expression, else_expression, semantic_type, __4] = semanticExpressionIf; return gen_if_cpp_statement(condition, then_expression, else_expression, semantic_type, context, gen_stmts);
}
return unit_if_branch_block(else_expression, context, gen_stmts);
std::abort();
}();
}
std::shared_ptr<cpp_ast::CppStatement> gen_if_cpp_statement(std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> if_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
  return emit_helpers::make_if_cpp_statement(eval_expr_cpp(condition, context, gen_stmts), unit_if_branch_block(then_expression, context, gen_stmts), unit_if_else_branch(else_expression, context, gen_stmts));
}
std::shared_ptr<cpp_ast::CppExpression> gen_if_unit_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> if_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
  return invoke_immediate_lambda_cpp(std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeName{mlc::String("void", 4)}), mlc::Array<std::shared_ptr<cpp_ast::CppStatement>>{gen_if_cpp_statement(condition, then_expression, else_expression, if_semantic_type, context, gen_stmts)});
}
std::shared_ptr<cpp_ast::CppExpression> gen_if_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> if_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
  auto condition_expression = eval_expr_cpp(condition, context, gen_stmts);
  auto then_expression_cpp = eval_expr_cpp(then_expression, context, gen_stmts);
  auto else_expression_cpp = gen_if_else_cpp_expression(else_expression, if_semantic_type, then_expression, context, gen_stmts);
  if (semantic_type_is_unit(if_semantic_type))   {
    return gen_if_unit_via_cpp_visitor(condition, then_expression, else_expression, if_semantic_type, context, gen_stmts);
  }
  return gen_if_non_unit_result_cpp(condition_expression, then_expression_cpp, else_expression_cpp, if_ternary_is_safe(then_expression, else_expression, if_semantic_type, context), match_gen::expression_result_cpp_type_for_codegen(context, if_semantic_type), context);
}
std::shared_ptr<cpp_ast::CppExpression> gen_block_via_cpp_visitor(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
  if ((statements.length() == 0))   {
    return eval_expr_cpp(result_expression, context, gen_stmts);
  } else   {
    auto final_context = statement_context::stmts_final_ctx(statements, context);
    auto body_fragment = (((gen_stmts(statements, context) + mlc::String("return ", 7)) + print::print_expr(eval_expr_cpp(result_expression, final_context, gen_stmts))) + mlc::String(";\n", 2));
    return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInvokedBlock{body_fragment});
  }
}
std::shared_ptr<cpp_ast::CppExpression> gen_match_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> match_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
  auto expanded_arms = match_gen::expand_or_arms(arms);
  if (match_gen::expanded_any_guard(expanded_arms))   {
    return match_gen::gen_match_guarded_expression_cpp(subject, expanded_arms, match_semantic_type, context, gen_stmts, eval_expr_cpp);
  }
  if (match_analysis::should_use_string_match_if_chain(subject, expanded_arms))   {
    return match_gen::gen_match_string_literal_expression_cpp(subject, expanded_arms, match_semantic_type, context, gen_stmts, eval_expr_cpp);
  }
  if ((match_gen::expanded_any_wildcard(expanded_arms) || match_analysis::subject_is_bool_type(subject)))   {
    return match_gen::gen_match_guarded_expression_cpp(subject, expanded_arms, match_semantic_type, context, gen_stmts, eval_expr_cpp);
  }
  return match_gen::gen_match_std_visit_cpp(subject, expanded_arms, match_semantic_type, context, gen_stmts, eval_expr_cpp);
}
mlc::String match_return_cpp_type(context::CodegenContext context, std::shared_ptr<registry::Type> match_semantic_type, std::shared_ptr<semantic_ir::SemanticExpression> subject) noexcept{
  if (match_gen::match_visit_uses_void_lambdas(match_semantic_type))   {
    return mlc::String("", 0);
  } else   {
    return match_gen::match_expression_return_cpp_type(context, match_semantic_type, subject);
  }
}
std::shared_ptr<cpp_ast::CppExpression> gen_match_default_block_cpp(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> expanded_arms, std::shared_ptr<registry::Type> match_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
  auto return_cpp = match_return_cpp_type(context, match_semantic_type, subject);
  auto inner_body = match_gen::gen_match_guarded_body(subject, expanded_arms, match_semantic_type, context, gen_stmts, [=](auto expression, auto eval_context, auto gen_stmts_fn) mutable { return print::print_expr(eval_expr_cpp(expression, eval_context, gen_stmts_fn)); });
  if ((return_cpp.length() > 0))   {
    return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInvokedBlockWithReturn{return_cpp, inner_body});
  }
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInvokedBlock{inner_body});
}
mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> record_field_expressions(mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, mlc::String type_name, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
  auto field_order = context::lookup_fields_for_context(context, type_name);
  if ((field_order.length() > 0))   {
    return field_order.map([=](mlc::String field_name) mutable { return eval_expr_cpp(expression_support::find_field_value(field_values, field_name), context, gen_stmts); });
  }
  return field_values.map([=](std::shared_ptr<semantic_ir::SemanticFieldVal> field_value) mutable { return eval_expr_cpp(field_value->value, context, gen_stmts); });
}
mlc::String record_cpp_name_from_type(mlc::String type_name, std::shared_ptr<registry::Type> expression_type, context::CodegenContext context) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = expression_type;
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [__0, __1] = tGeneric; return type_gen::sem_type_to_cpp(context, expression_type);
}
return context::CodegenContext_resolve(context, type_name);
std::abort();
}();
}
std::shared_ptr<cpp_ast::CppExpression> gen_record_via_cpp_visitor(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> expression_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppAggregateInit{record_cpp_name_from_type(type_name, expression_type, context), record_field_expressions(field_values, type_name, context, gen_stmts)});
}
std::shared_ptr<cpp_ast::CppExpression> empty_array_cpp(mlc::String element_cpp) noexcept{
  if ((element_cpp == mlc::String("auto", 4)))   {
    return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInitList{{}});
  } else   {
    return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppAggregateInit{((mlc::String("mlc::Array<", 11) + mlc::to_string(element_cpp)) + mlc::String(">", 1)), {}});
  }
}
std::shared_ptr<cpp_ast::CppExpression> non_empty_array_cpp(mlc::String element_cpp, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> element_expressions) noexcept{
  if ((element_cpp == mlc::String("auto", 4)))   {
    return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppAggregateInit{mlc::String("mlc::Array", 10), element_expressions});
  } else   {
    return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppAggregateInit{((mlc::String("mlc::Array<", 11) + mlc::to_string(element_cpp)) + mlc::String(">", 1)), element_expressions});
  }
}
std::shared_ptr<cpp_ast::CppExpression> array_from_tarray_inner_type(std::shared_ptr<registry::Type> inner_type, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> element_expressions, context::CodegenContext context) noexcept{
  auto element_cpp = type_gen::sem_type_to_cpp(context, inner_type);
  if ((elements.length() == 0))   {
    return empty_array_cpp(element_cpp);
  }
  return non_empty_array_cpp(element_cpp, element_expressions);
}
std::shared_ptr<cpp_ast::CppExpression> array_from_unknown_element_type(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> element_expressions) noexcept{
  if ((elements.length() == 0))   {
    return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInitList{{}});
  } else   {
    return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppAggregateInit{mlc::String("mlc::Array", 10), element_expressions});
  }
}
std::shared_ptr<cpp_ast::CppExpression> gen_array_for_container_type(std::shared_ptr<registry::Type> element_container_semantic_type, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> element_expressions, context::CodegenContext context) noexcept{
  return [&]() -> std::shared_ptr<cpp_ast::CppExpression> {
auto __match_subject = element_container_semantic_type;
if (std::holds_alternative<registry::TArray>((*__match_subject))) {
const registry::TArray& tArray = std::get<registry::TArray>((*__match_subject));
auto [inner_type] = tArray; return array_from_tarray_inner_type(inner_type, elements, element_expressions, context);
}
return array_from_unknown_element_type(elements, element_expressions);
std::abort();
}();
}
std::shared_ptr<cpp_ast::CppExpression> gen_array_via_cpp_visitor(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> element_container_semantic_type, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
  auto element_expressions = elements.map([=](std::shared_ptr<semantic_ir::SemanticExpression> element) mutable { return eval_expr_cpp(element, context, gen_stmts); });
  return gen_array_for_container_type(element_container_semantic_type, elements, element_expressions, context);
}
mlc::Array<std::shared_ptr<cpp_ast::CppCapture>> lambda_capture_list(mlc::Array<mlc::String> parameter_binding_names) noexcept{
  if ((parameter_binding_names.length() == 0))   {
    return {};
  }
  auto captures = mlc::Array<std::shared_ptr<cpp_ast::CppCapture>>{};
  captures.push_back(value_capture_for_lambda());
  return captures;
}
std::shared_ptr<cpp_ast::CppType> lambda_parameter_type_cpp(context::CodegenContext context, std::shared_ptr<registry::Type> semantic_parameter_type) noexcept{
  auto parameter_type_cpp = type_gen::sem_type_to_cpp(context, semantic_parameter_type);
  if ((parameter_type_cpp == mlc::String("void", 4)))   {
    return std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeName{mlc::String("std::tuple<>", 12)});
  }
  return std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeName{parameter_type_cpp});
}
mlc::Array<std::shared_ptr<cpp_ast::CppParam>> lambda_auto_parameters_cpp(mlc::Array<mlc::String> parameter_binding_names) noexcept{
  return parameter_binding_names.map([=](mlc::String parameter_name) mutable { return std::make_shared<cpp_ast::CppParam>(cpp_ast::CppParam{cpp_naming::cpp_safe(parameter_name), std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeName{mlc::String("auto", 4)})}); });
}
mlc::Array<std::shared_ptr<cpp_ast::CppParam>> lambda_parameters_cpp(mlc::Array<mlc::String> parameter_binding_names, std::shared_ptr<registry::Type> semantic_function_type_for_lambda_expression, context::CodegenContext context) noexcept{
  return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppParam>> {
auto __match_subject = semantic_function_type_for_lambda_expression;
if (std::holds_alternative<registry::TFn>((*__match_subject))) {
const registry::TFn& tFn = std::get<registry::TFn>((*__match_subject));
auto [lambda_parameter_semantic_types, __1] = tFn; return [&]() -> mlc::Array<std::shared_ptr<cpp_ast::CppParam>> {
  if ((lambda_parameter_semantic_types.length() != parameter_binding_names.length()))   {
    return lambda_auto_parameters_cpp(parameter_binding_names);
  } else   {
    return [&]() {
auto parameters = mlc::Array<std::shared_ptr<cpp_ast::CppParam>>{};
auto lambda_parameter_index = 0;
while ((lambda_parameter_index < parameter_binding_names.length())) {
  parameters.push_back(std::make_shared<cpp_ast::CppParam>(cpp_ast::CppParam{cpp_naming::cpp_safe(parameter_binding_names[lambda_parameter_index]), lambda_parameter_type_cpp(context, lambda_parameter_semantic_types[lambda_parameter_index])}));
  (lambda_parameter_index = (lambda_parameter_index + 1));
}
return parameters;
}();
  }
}();
}
return lambda_auto_parameters_cpp(parameter_binding_names);
std::abort();
}();
}
std::shared_ptr<cpp_ast::CppExpression> gen_lambda_via_cpp_visitor(mlc::Array<mlc::String> parameter_binding_names, std::shared_ptr<semantic_ir::SemanticExpression> body_expression_under_lambda, std::shared_ptr<registry::Type> semantic_function_type_for_lambda_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMutableLambda{lambda_capture_list(parameter_binding_names), lambda_parameters_cpp(parameter_binding_names, semantic_function_type_for_lambda_expression, context), eval_expr_cpp(body_expression_under_lambda, context, gen_stmts)});
}
std::shared_ptr<cpp_ast::CppExpression> gen_field_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String field_name, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppMember{eval_expr_cpp(object, context, gen_stmts), cpp_naming::cpp_safe(field_name), (expression_support::field_access_operator(object, context) == mlc::String("->", 2))});
}
std::shared_ptr<cpp_ast::CppExpression> gen_index_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppIndex{eval_expr_cpp(object, context, gen_stmts), eval_expr_cpp(index_expression, context, gen_stmts)});
}
std::shared_ptr<cpp_ast::CppExpression> gen_while_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInvokedWhile{eval_expr_cpp(condition, context, gen_stmts), gen_stmts(statements, context)});
}
std::shared_ptr<cpp_ast::CppExpression> gen_for_via_cpp_visitor(mlc::String variable, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInvokedFor{cpp_naming::cpp_safe(variable), eval_expr_cpp(iterator, context, gen_stmts), gen_stmts(statements, context)});
}
std::shared_ptr<cpp_ast::CppExpression> gen_record_update_via_cpp_visitor(mlc::String type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
  return record_gen::gen_record_update_expr_cpp(type_name, base_expression, overrides, context, gen_stmts, eval_expr_cpp);
}
std::shared_ptr<cpp_ast::CppExpression> gen_tuple_via_cpp_visitor(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall{emit_helpers::make_identifier_cpp_expression(mlc::String("std::make_tuple", 15)), elements.map([=](std::shared_ptr<semantic_ir::SemanticExpression> element) mutable { return eval_expr_cpp(element, context, gen_stmts); })});
}
std::shared_ptr<cpp_ast::CppExpression> gen_question_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppQuestionTry{eval_expr_cpp(inner_expression, context, gen_stmts), expression_support::question_from_converter_name(context, semantic_ir::sexpr_type(inner_expression))});
}
std::shared_ptr<cpp_ast::CppExpression> gen_with_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppWithBlock{eval_expr_cpp(resource, context, gen_stmts), binder, gen_stmts(statements, context)});
}
mlc::String named_type_name_from_semantic_type(std::shared_ptr<registry::Type> semantic_type) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = semantic_type;
if (std::holds_alternative<registry::TNamed>((*__match_subject))) {
const registry::TNamed& tNamed = std::get<registry::TNamed>((*__match_subject));
auto [name] = tNamed; return name;
}
return mlc::String("", 0);
std::abort();
}();
}
std::shared_ptr<cpp_ast::CppExpression> visit_int(CodegenPass self, int value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> visit_str(CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> visit_bool(CodegenPass self, bool value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> visit_unit(CodegenPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> visit_float(CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> visit_i64(CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> visit_u8(CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> visit_usize(CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> visit_char(CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> visit_extern(CodegenPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> visit_ident(CodegenPass self, mlc::String name, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> visit_bin(CodegenPass self, mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> left_expression, std::shared_ptr<semantic_ir::SemanticExpression> right_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> visit_un(CodegenPass self, mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> visit_call(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> mutability_flags, std::shared_ptr<registry::Type> semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> visit_method(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> visit_field(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String field_name, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> visit_index(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> visit_if(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> visit_block(CodegenPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> visit_while(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> visit_for(CodegenPass self, mlc::String variable, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> visit_match(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> visit_record(CodegenPass self, mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> visit_record_update(CodegenPass self, mlc::String type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> visit_array(CodegenPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> visit_tuple(CodegenPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> visit_question(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> visit_lambda(CodegenPass self, mlc::Array<mlc::String> parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body_expression, std::shared_ptr<registry::Type> semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> visit_with(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> visit_unsupported(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> _expression) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_int(CodegenPass self, int value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_str(CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_bool(CodegenPass self, bool value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_unit(CodegenPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_float(CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_i64(CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_u8(CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_usize(CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_char(CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_extern(CodegenPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_ident(CodegenPass self, mlc::String name, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_bin(CodegenPass self, mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> left_expression, std::shared_ptr<semantic_ir::SemanticExpression> right_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_un(CodegenPass self, mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_call(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> mutability_flags, std::shared_ptr<registry::Type> semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_method(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_field(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String field_name, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_index(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_if(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_block(CodegenPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_while(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_for(CodegenPass self, mlc::String variable, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_match(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_record(CodegenPass self, mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_record_update(CodegenPass self, mlc::String type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_array(CodegenPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_tuple(CodegenPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_question(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_lambda(CodegenPass self, mlc::Array<mlc::String> parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body_expression, std::shared_ptr<registry::Type> semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_with(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_unsupported(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> _expression) noexcept;
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_int(CodegenPass self, int value, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return literals::gen_integer_literal_cpp(value);
}
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_str(CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return literals::gen_string_literal_cpp(value);
}
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_bool(CodegenPass self, bool value, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return literals::gen_boolean_literal_cpp(value);
}
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_unit(CodegenPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return literals::gen_unit_literal_cpp();
}
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_float(CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return literals::gen_float_literal_cpp(value);
}
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_i64(CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return literals::gen_i64_literal_cpp(value);
}
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_u8(CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return literals::gen_u8_literal_cpp(value);
}
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_usize(CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return literals::gen_usize_literal_cpp(value);
}
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_char(CodegenPass self, mlc::String value, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return literals::gen_char_literal_cpp(value);
}
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_extern(CodegenPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return literals::gen_extern_placeholder_cpp();
}
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_ident(CodegenPass self, mlc::String name, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return identifiers::gen_identifier_cpp(name, self.context);
}
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_bin(CodegenPass self, mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> left_expression, std::shared_ptr<semantic_ir::SemanticExpression> right_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return gen_binary_via_cpp_visitor(operation, left_expression, right_expression, self.context, self.gen_stmts, self.evaluate_expression);
}
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_un(CodegenPass self, mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return gen_unary_via_cpp_visitor(operation, inner_expression, self.context, self.gen_stmts, self.evaluate_expression);
}
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_call(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> mutability_flags, std::shared_ptr<registry::Type> semantic_type) noexcept{
return gen_call_via_cpp_visitor(function_expression, arguments, mutability_flags, semantic_type, self.context, self.gen_stmts);
}
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_method(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return gen_method_via_cpp_visitor(object, method_name, arguments, mutability_flags, self.context, self.gen_stmts);
}
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_field(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String field_name, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return gen_field_via_cpp_visitor(object, field_name, self.context, self.gen_stmts);
}
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_index(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return gen_index_via_cpp_visitor(object, index_expression, self.context, self.gen_stmts);
}
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_if(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> semantic_type) noexcept{
return gen_if_via_cpp_visitor(condition, then_expression, else_expression, semantic_type, self.context, self.gen_stmts);
}
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_block(CodegenPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return gen_block_via_cpp_visitor(statements, result_expression, self.context, self.gen_stmts);
}
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_while(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return gen_while_via_cpp_visitor(condition, statements, self.context, self.gen_stmts);
}
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_for(CodegenPass self, mlc::String variable, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return gen_for_via_cpp_visitor(variable, iterator, statements, self.context, self.gen_stmts);
}
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_match(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> semantic_type) noexcept{
return gen_match_via_cpp_visitor(subject, arms, semantic_type, self.context, self.gen_stmts);
}
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_record(CodegenPass self, mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> semantic_type) noexcept{
return gen_record_via_cpp_visitor(type_name, field_values, semantic_type, self.context, self.gen_stmts);
}
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_record_update(CodegenPass self, mlc::String type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return gen_record_update_via_cpp_visitor(type_name, base_expression, overrides, self.context, self.gen_stmts);
}
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_array(CodegenPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> semantic_type) noexcept{
return gen_array_via_cpp_visitor(elements, semantic_type, self.context, self.gen_stmts);
}
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_tuple(CodegenPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return gen_tuple_via_cpp_visitor(elements, self.context, self.gen_stmts);
}
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_question(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return gen_question_via_cpp_visitor(inner_expression, self.context, self.gen_stmts);
}
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_lambda(CodegenPass self, mlc::Array<mlc::String> parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body_expression, std::shared_ptr<registry::Type> semantic_type) noexcept{
return gen_lambda_via_cpp_visitor(parameter_names, body_expression, semantic_type, self.context, self.gen_stmts);
}
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_with(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> resource, mlc::String binder, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept{
return gen_with_via_cpp_visitor(resource, binder, statements, self.context, self.gen_stmts);
}
std::shared_ptr<cpp_ast::CppExpression> CodegenPass_visit_unsupported(CodegenPass self, std::shared_ptr<semantic_ir::SemanticExpression> _expression) noexcept{
return unsupported_cpp_expression();
}
std::shared_ptr<cpp_ast::CppExpression> dispatch_codegen_pass(CodegenPass codegen_pass, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
  return [&]() -> std::shared_ptr<cpp_ast::CppExpression> {
auto __match_subject = expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionInt>((*__match_subject))) {
const semantic_ir::SemanticExpressionInt& semanticExpressionInt = std::get<semantic_ir::SemanticExpressionInt>((*__match_subject));
auto [integer_value, type_value, __2] = semanticExpressionInt; return CodegenPass_visit_int(codegen_pass, integer_value, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionStr>((*__match_subject))) {
const semantic_ir::SemanticExpressionStr& semanticExpressionStr = std::get<semantic_ir::SemanticExpressionStr>((*__match_subject));
auto [string_value, type_value, __2] = semanticExpressionStr; return CodegenPass_visit_str(codegen_pass, string_value, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionBool>((*__match_subject))) {
const semantic_ir::SemanticExpressionBool& semanticExpressionBool = std::get<semantic_ir::SemanticExpressionBool>((*__match_subject));
auto [boolean_value, type_value, __2] = semanticExpressionBool; return CodegenPass_visit_bool(codegen_pass, boolean_value, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*__match_subject))) {
const semantic_ir::SemanticExpressionUnit& semanticExpressionUnit = std::get<semantic_ir::SemanticExpressionUnit>((*__match_subject));
auto [type_value, __1] = semanticExpressionUnit; return CodegenPass_visit_unit(codegen_pass, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionFloat>((*__match_subject))) {
const semantic_ir::SemanticExpressionFloat& semanticExpressionFloat = std::get<semantic_ir::SemanticExpressionFloat>((*__match_subject));
auto [float_value, type_value, __2] = semanticExpressionFloat; return CodegenPass_visit_float(codegen_pass, float_value, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionI64>((*__match_subject))) {
const semantic_ir::SemanticExpressionI64& semanticExpressionI64 = std::get<semantic_ir::SemanticExpressionI64>((*__match_subject));
auto [integer_value, type_value, __2] = semanticExpressionI64; return CodegenPass_visit_i64(codegen_pass, integer_value, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionU8>((*__match_subject))) {
const semantic_ir::SemanticExpressionU8& semanticExpressionU8 = std::get<semantic_ir::SemanticExpressionU8>((*__match_subject));
auto [byte_value, type_value, __2] = semanticExpressionU8; return CodegenPass_visit_u8(codegen_pass, byte_value, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionUsize>((*__match_subject))) {
const semantic_ir::SemanticExpressionUsize& semanticExpressionUsize = std::get<semantic_ir::SemanticExpressionUsize>((*__match_subject));
auto [size_value, type_value, __2] = semanticExpressionUsize; return CodegenPass_visit_usize(codegen_pass, size_value, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionChar>((*__match_subject))) {
const semantic_ir::SemanticExpressionChar& semanticExpressionChar = std::get<semantic_ir::SemanticExpressionChar>((*__match_subject));
auto [character_value, type_value, __2] = semanticExpressionChar; return CodegenPass_visit_char(codegen_pass, character_value, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionExtern>((*__match_subject))) {
const semantic_ir::SemanticExpressionExtern& semanticExpressionExtern = std::get<semantic_ir::SemanticExpressionExtern>((*__match_subject));
auto [type_value, __1, __2, __3] = semanticExpressionExtern; return CodegenPass_visit_extern(codegen_pass, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*__match_subject))) {
const semantic_ir::SemanticExpressionIdent& semanticExpressionIdent = std::get<semantic_ir::SemanticExpressionIdent>((*__match_subject));
auto [name_value, type_value, __2] = semanticExpressionIdent; return CodegenPass_visit_ident(codegen_pass, name_value, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionBin>((*__match_subject))) {
const semantic_ir::SemanticExpressionBin& semanticExpressionBin = std::get<semantic_ir::SemanticExpressionBin>((*__match_subject));
auto [operation, left_expression, right_expression, type_value, __4] = semanticExpressionBin; return CodegenPass_visit_bin(codegen_pass, operation, left_expression, right_expression, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionUn>((*__match_subject))) {
const semantic_ir::SemanticExpressionUn& semanticExpressionUn = std::get<semantic_ir::SemanticExpressionUn>((*__match_subject));
auto [operation, inner_expression, type_value, __3] = semanticExpressionUn; return CodegenPass_visit_un(codegen_pass, operation, inner_expression, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionCall>((*__match_subject))) {
const semantic_ir::SemanticExpressionCall& semanticExpressionCall = std::get<semantic_ir::SemanticExpressionCall>((*__match_subject));
auto [function_expression, arguments, mutability_flags, type_value, __4] = semanticExpressionCall; return CodegenPass_visit_call(codegen_pass, function_expression, arguments, mutability_flags, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionMethod>((*__match_subject))) {
const semantic_ir::SemanticExpressionMethod& semanticExpressionMethod = std::get<semantic_ir::SemanticExpressionMethod>((*__match_subject));
auto [object, method_name, arguments, mutability_flags, type_value, __5] = semanticExpressionMethod; return CodegenPass_visit_method(codegen_pass, object, method_name, arguments, mutability_flags, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionField>((*__match_subject))) {
const semantic_ir::SemanticExpressionField& semanticExpressionField = std::get<semantic_ir::SemanticExpressionField>((*__match_subject));
auto [object, field_name, type_value, __3] = semanticExpressionField; return CodegenPass_visit_field(codegen_pass, object, field_name, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIndex>((*__match_subject))) {
const semantic_ir::SemanticExpressionIndex& semanticExpressionIndex = std::get<semantic_ir::SemanticExpressionIndex>((*__match_subject));
auto [object, index_expression, type_value, __3] = semanticExpressionIndex; return CodegenPass_visit_index(codegen_pass, object, index_expression, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionQuestion>((*__match_subject))) {
const semantic_ir::SemanticExpressionQuestion& semanticExpressionQuestion = std::get<semantic_ir::SemanticExpressionQuestion>((*__match_subject));
auto [inner_expression, type_value, __2] = semanticExpressionQuestion; return CodegenPass_visit_question(codegen_pass, inner_expression, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionLambda>((*__match_subject))) {
const semantic_ir::SemanticExpressionLambda& semanticExpressionLambda = std::get<semantic_ir::SemanticExpressionLambda>((*__match_subject));
auto [parameter_names, body_expression, type_value, __3] = semanticExpressionLambda; return CodegenPass_visit_lambda(codegen_pass, parameter_names, body_expression, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionWith>((*__match_subject))) {
const semantic_ir::SemanticExpressionWith& semanticExpressionWith = std::get<semantic_ir::SemanticExpressionWith>((*__match_subject));
auto [resource, binder, statements, type_value, __4] = semanticExpressionWith; return CodegenPass_visit_with(codegen_pass, resource, binder, statements, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*__match_subject))) {
const semantic_ir::SemanticExpressionIf& semanticExpressionIf = std::get<semantic_ir::SemanticExpressionIf>((*__match_subject));
auto [condition, then_expression, else_expression, type_value, __4] = semanticExpressionIf; return CodegenPass_visit_if(codegen_pass, condition, then_expression, else_expression, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*__match_subject))) {
const semantic_ir::SemanticExpressionBlock& semanticExpressionBlock = std::get<semantic_ir::SemanticExpressionBlock>((*__match_subject));
auto [statements, result_expression, type_value, __3] = semanticExpressionBlock; return CodegenPass_visit_block(codegen_pass, statements, result_expression, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionWhile>((*__match_subject))) {
const semantic_ir::SemanticExpressionWhile& semanticExpressionWhile = std::get<semantic_ir::SemanticExpressionWhile>((*__match_subject));
auto [condition, statements, type_value, __3] = semanticExpressionWhile; return CodegenPass_visit_while(codegen_pass, condition, statements, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionFor>((*__match_subject))) {
const semantic_ir::SemanticExpressionFor& semanticExpressionFor = std::get<semantic_ir::SemanticExpressionFor>((*__match_subject));
auto [variable, iterator, statements, type_value, __4] = semanticExpressionFor; return CodegenPass_visit_for(codegen_pass, variable, iterator, statements, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionMatch>((*__match_subject))) {
const semantic_ir::SemanticExpressionMatch& semanticExpressionMatch = std::get<semantic_ir::SemanticExpressionMatch>((*__match_subject));
auto [subject, arms, type_value, __3] = semanticExpressionMatch; return CodegenPass_visit_match(codegen_pass, subject, arms, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionRecord>((*__match_subject))) {
const semantic_ir::SemanticExpressionRecord& semanticExpressionRecord = std::get<semantic_ir::SemanticExpressionRecord>((*__match_subject));
auto [type_name, field_values, type_value, __3] = semanticExpressionRecord; return CodegenPass_visit_record(codegen_pass, type_name, field_values, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionRecordUpdate>((*__match_subject))) {
const semantic_ir::SemanticExpressionRecordUpdate& semanticExpressionRecordUpdate = std::get<semantic_ir::SemanticExpressionRecordUpdate>((*__match_subject));
auto [type_name, base_expression, overrides, type_value, __4] = semanticExpressionRecordUpdate; return CodegenPass_visit_record_update(codegen_pass, type_name, base_expression, overrides, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionArray>((*__match_subject))) {
const semantic_ir::SemanticExpressionArray& semanticExpressionArray = std::get<semantic_ir::SemanticExpressionArray>((*__match_subject));
auto [elements, type_value, __2] = semanticExpressionArray; return CodegenPass_visit_array(codegen_pass, elements, type_value);
}
if (std::holds_alternative<semantic_ir::SemanticExpressionTuple>((*__match_subject))) {
const semantic_ir::SemanticExpressionTuple& semanticExpressionTuple = std::get<semantic_ir::SemanticExpressionTuple>((*__match_subject));
auto [elements, type_value, __2] = semanticExpressionTuple; return CodegenPass_visit_tuple(codegen_pass, elements, type_value);
}
return CodegenPass_visit_unsupported(codegen_pass, expression);
std::abort();
}();
}
std::shared_ptr<cpp_ast::CppExpression> eval_expr_with_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)> gen_stmts, std::function<std::shared_ptr<cpp_ast::CppExpression>(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>, context::CodegenContext)>)> evaluate_expression) noexcept{
  return dispatch_codegen_pass(codegen_pass_new(context, gen_stmts, evaluate_expression), expression);
}
std::shared_ptr<cpp_ast::CppExpression> gen_expr_via_cpp_visitor(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context) noexcept{
  return eval_expr_with_cpp_visitor(expression, context, ignored_gen_stmts, evaluate_via_cpp_visitor);
}

} // namespace expr_visitor_cpp
