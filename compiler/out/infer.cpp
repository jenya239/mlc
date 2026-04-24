#include "infer.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "infer_literals.hpp"
#include "infer_for_support.hpp"
#include "infer_question_expression.hpp"
#include "infer_operand_combine.hpp"
#include "infer_call_support.hpp"
#include "type_diagnostics.hpp"
#include "call_argument_unify.hpp"
#include "infer_trait_bounds.hpp"
#include "semantic_type_structure.hpp"
#include "registry.hpp"
#include "infer_expr_ident.hpp"
#include "pattern_env.hpp"

namespace infer {

using namespace ast;
using namespace registry;
using namespace check_context;
using namespace infer_result;
using namespace infer_literals;
using namespace infer_for_support;
using namespace infer_question_expression;
using namespace infer_operand_combine;
using namespace infer_call_support;
using namespace type_diagnostics;
using namespace call_argument_unify;
using namespace infer_trait_bounds;
using namespace semantic_type_structure;
using namespace registry;
using namespace infer_expr_ident;
using namespace pattern_env;
using namespace ast_tokens;

infer_result::InferResult infer_arguments_errors(infer_result::InferResult initial, mlc::Array<std::shared_ptr<ast::Expr>> expressions, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_field_values_errors(infer_result::InferResult initial, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context, mlc::String record_type_name_for_fields) noexcept;

infer_result::InferResult infer_expr_binary(mlc::String operation, std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, ast::Span source_span, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_unary(mlc::String operation, std::shared_ptr<ast::Expr> inner, ast::Span source_span, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_call(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_method(std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_field(std::shared_ptr<ast::Expr> object, mlc::String field_name, ast::Span field_source_span, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_index(std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expression, ast::Span bracket_source_span, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_conditional(std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expression, std::shared_ptr<ast::Expr> else_expression, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_block(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_while_loop(std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_for_loop(mlc::String variable_name, std::shared_ptr<ast::Expr> iterator, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> build_match_substitution(std::shared_ptr<registry::Type> subject_type, check_context::CheckContext context) noexcept;

infer_result::InferResult infer_expr_match(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> arms, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_record_update(mlc::String type_name, std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_array_literal(mlc::Array<std::shared_ptr<ast::Expr>> elements, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_question(std::shared_ptr<ast::Expr> inner, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_lambda(mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr(std::shared_ptr<ast::Expr> expression, check_context::CheckContext inference_context) noexcept;

infer_result::StmtInferResult infer_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_arguments_errors(infer_result::InferResult initial, mlc::Array<std::shared_ptr<ast::Expr>> expressions, check_context::CheckContext inference_context) noexcept{
infer_result::InferResult result = std::move(initial);
int index = 0;
while (index < expressions.size()){
{
result = infer_result::InferResult_absorb(result, infer_expr(expressions[index], inference_context));
index = index + 1;
}
}
return result;
}

infer_result::InferResult infer_field_values_errors(infer_result::InferResult initial, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context, mlc::String record_type_name_for_fields) noexcept{
infer_result::InferResult result = std::move(initial);
int index = 0;
while (index < field_values.size()){
{
std::shared_ptr<ast::FieldVal> field_value = field_values[index];
bool has_fields = record_type_name_for_fields.length() > 0 && registry::TypeRegistry_has_fields(inference_context.registry, record_type_name_for_fields);
if (has_fields){
{
mlc::Array<ast::Diagnostic> field_name_errors = type_diagnostics::infer_expr_field_diagnostics(std::make_shared<registry::Type>(registry::TNamed(record_type_name_for_fields)), field_value->name, ast::expr_span(field_value->val), inference_context.registry);
result = infer_result::InferResult{result.inferred_type, ast::diagnostics_append(result.errors, field_name_errors)};
}
}
infer_result::InferResult value_result = infer_expr(field_value->val, inference_context);
if (has_fields){
{
std::shared_ptr<registry::Type> expected_type = registry::field_type_from_object(std::make_shared<registry::Type>(registry::TNamed(record_type_name_for_fields)), field_value->name, inference_context.registry);
std::shared_ptr<registry::Type> actual_type = value_result.inferred_type;
bool type_mismatch = !semantic_type_structure::type_is_unknown(expected_type) && !semantic_type_structure::type_is_unknown(actual_type) && !semantic_type_structure::types_structurally_equal(expected_type, actual_type);
mlc::Array<ast::Diagnostic> type_errors = type_mismatch ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("field ") + field_value->name + mlc::String(": expected ") + semantic_type_structure::type_description(expected_type) + mlc::String(", got ") + semantic_type_structure::type_description(actual_type), ast::expr_span(field_value->val))} : mlc::Array<ast::Diagnostic>{};
result = infer_result::InferResult{result.inferred_type, ast::diagnostics_append(ast::diagnostics_append(result.errors, value_result.errors), type_errors)};
}
} else {
{
result = infer_result::InferResult_absorb(result, value_result);
}
}
index = index + 1;
}
}
return result;
}

infer_result::InferResult infer_expr_binary(mlc::String operation, std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, ast::Span source_span, check_context::CheckContext inference_context) noexcept{
infer_result::InferResult left_result = infer_expr(left, inference_context);
infer_result::InferResult right_result = infer_expr(right, inference_context);
return infer_operand_combine::infer_binary_from_operand_results(operation, left_result, right_result, source_span);
}

infer_result::InferResult infer_expr_unary(mlc::String operation, std::shared_ptr<ast::Expr> inner, ast::Span source_span, check_context::CheckContext inference_context) noexcept{return infer_operand_combine::infer_unary_from_inner_result(operation, infer_expr(inner, inference_context), source_span);}

infer_result::InferResult infer_expr_call(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, check_context::CheckContext inference_context) noexcept{
infer_result::InferResult function_result = infer_expr(function, inference_context);
mlc::Array<ast::Diagnostic> collected_errors = function_result.errors;
mlc::Array<std::shared_ptr<registry::Type>> argument_inferred_types = {};
int argument_index = 0;
while (argument_index < call_arguments.size()){
{
infer_result::InferResult argument_inference = infer_expr(call_arguments[argument_index], inference_context);
collected_errors = ast::diagnostics_append(collected_errors, argument_inference.errors);
argument_inferred_types.push_back(argument_inference.inferred_type);
argument_index = argument_index + 1;
}
}
infer_result::InferResult with_arguments = infer_result::InferResult{function_result.inferred_type, collected_errors};
mlc::String callee_name = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*function)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*function)._); auto [name, _w0] = _v_exprident; return name; } return mlc::String(""); }();
return callee_name != mlc::String("") && registry::TypeRegistry_has_ctor(inference_context.registry, callee_name) ? [&]() -> infer_result::InferResult { 
  infer_result::InferResult constructor_output = infer_call_support::infer_expr_call_for_constructor_name(callee_name, with_arguments, call_arguments, call_source_span, inference_context);
  mlc::Array<std::shared_ptr<registry::Type>> constructor_parameter_types = registry::TypeRegistry_ctor_params_for(inference_context.registry, callee_name);
  mlc::String algebraic_type_name = [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*registry::TypeRegistry_ctor_type(inference_context.registry, callee_name)))) { auto _v_tnamed = std::get<registry::TNamed>((*registry::TypeRegistry_ctor_type(inference_context.registry, callee_name))); auto [parent_name] = _v_tnamed; return parent_name; } return mlc::String(""); }();
  mlc::Array<mlc::String> constructor_type_parameter_names = registry::TypeRegistry_algebraic_decl_type_parameter_names_for(inference_context.registry, algebraic_type_name);
  return constructor_parameter_types.size() == call_arguments.size() ? infer_result::InferResult{constructor_output.inferred_type, ast::diagnostics_append(constructor_output.errors, call_argument_unify::call_argument_unification_diagnostics(constructor_parameter_types, argument_inferred_types, constructor_type_parameter_names, call_arguments))} : constructor_output;
 }() : [&]() -> infer_result::InferResult { 
  infer_result::InferResult non_constructor_output = infer_call_support::infer_expr_call_non_constructor(function_result, with_arguments, call_arguments, call_source_span);
  mlc::Array<std::shared_ptr<registry::Type>> callee_parameter_types = semantic_type_structure::function_parameter_list(function_result.inferred_type);
  mlc::Array<mlc::String> callee_type_parameter_names = callee_name != mlc::String("") && registry::TypeRegistry_has_fn(inference_context.registry, callee_name) ? registry::TypeRegistry_registered_function_type_parameter_names(inference_context.registry, callee_name) : [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty_callee_type_names = {};
  return empty_callee_type_names;
 }();
  return semantic_type_structure::type_is_function(function_result.inferred_type) && callee_parameter_types.size() == call_arguments.size() ? [&]() -> infer_result::InferResult { 
  mlc::Array<ast::Diagnostic> unification_errors = call_argument_unify::call_argument_unification_diagnostics(callee_parameter_types, argument_inferred_types, callee_type_parameter_names, call_arguments);
  mlc::Array<ast::Diagnostic> bound_errors = infer_trait_bounds::trait_bound_diagnostics(callee_name, callee_parameter_types, argument_inferred_types, callee_type_parameter_names, call_source_span, inference_context);
  return infer_result::InferResult{non_constructor_output.inferred_type, ast::diagnostics_append(ast::diagnostics_append(non_constructor_output.errors, unification_errors), bound_errors)};
 }() : non_constructor_output;
 }();
}

infer_result::InferResult infer_expr_method(std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context) noexcept{
infer_result::InferResult object_result = infer_expr(object, inference_context);
infer_result::InferResult with_arguments = infer_arguments_errors(object_result, method_arguments, inference_context);
return infer_operand_combine::infer_method_from_object_and_arguments(object_result, with_arguments, method_name, method_span, method_arguments.size(), inference_context.registry);
}

infer_result::InferResult infer_expr_field(std::shared_ptr<ast::Expr> object, mlc::String field_name, ast::Span field_source_span, check_context::CheckContext inference_context) noexcept{return infer_operand_combine::infer_field_from_object_result(infer_expr(object, inference_context), field_name, field_source_span, inference_context.registry);}

infer_result::InferResult infer_expr_index(std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expression, ast::Span bracket_source_span, check_context::CheckContext inference_context) noexcept{return infer_operand_combine::infer_index_from_operand_results(infer_expr(object, inference_context), infer_expr(index_expression, inference_context), bracket_source_span);}

infer_result::InferResult infer_expr_conditional(std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expression, std::shared_ptr<ast::Expr> else_expression, check_context::CheckContext inference_context) noexcept{return infer_operand_combine::infer_conditional_from_branch_results(infer_expr(condition, inference_context), infer_expr(then_expression, inference_context), infer_expr(else_expression, inference_context), else_expression);}

infer_result::InferResult infer_expr_block(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression, check_context::CheckContext inference_context) noexcept{
infer_result::StmtInferResult statements_result = infer_statements(statements, inference_context);
infer_result::InferResult result_inference = infer_expr(result_expression, check_context::check_context_new(statements_result.type_env, inference_context.registry));
return infer_result::InferResult_absorb_stmt(result_inference, statements_result);
}

infer_result::InferResult infer_expr_while_loop(std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept{
infer_result::InferResult condition_result = infer_expr(condition, inference_context);
infer_result::StmtInferResult statements_result = infer_statements(statements, inference_context);
return infer_result::InferResult_with_type(infer_result::InferResult_absorb_stmt(condition_result, statements_result), std::make_shared<registry::Type>((registry::TUnit{})));
}

infer_result::InferResult infer_expr_for_loop(mlc::String variable_name, std::shared_ptr<ast::Expr> iterator, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept{
infer_result::InferResult iterator_base = infer_expr(iterator, inference_context);
infer_result::InferResult iterator_result = infer_for_support::infer_for_iterator_with_range_rules(iterator_base, iterator);
std::shared_ptr<registry::Type> element_type = infer_for_support::element_type_for_for_iterator(iterator_result.inferred_type);
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> inner_environment = inference_context.type_env;
inner_environment.set(variable_name, element_type);
check_context::CheckContext loop_context = check_context::check_context_new(inner_environment, inference_context.registry);
infer_result::StmtInferResult statements_result = infer_statements(statements, loop_context);
return infer_result::InferResult_with_type(infer_result::InferResult_absorb_stmt(iterator_result, statements_result), std::make_shared<registry::Type>((registry::TUnit{})));
}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> build_match_substitution(std::shared_ptr<registry::Type> subject_type, check_context::CheckContext context) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
[&]() -> void { if (std::holds_alternative<registry::TGeneric>((*subject_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*subject_type)); auto [type_name, type_args] = _v_tgeneric; return [&]() { 
  mlc::Array<mlc::String> param_names = registry::TypeRegistry_algebraic_decl_type_parameter_names_for(context.registry, type_name);
  int i = 0;
  return [&]() { 
  while (i < param_names.size() && i < type_args.size()){
{
substitution.set(param_names[i], type_args[i]);
i = i + 1;
}
}
 }();
 }(); } return; }();
return substitution;
}

infer_result::InferResult infer_expr_match(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> arms, check_context::CheckContext inference_context) noexcept{
infer_result::InferResult subject_result = infer_expr(subject, inference_context);
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution = build_match_substitution(subject_result.inferred_type, inference_context);
mlc::Array<ast::Diagnostic> collected_errors = subject_result.errors;
std::shared_ptr<registry::Type> arm_type = std::make_shared<registry::Type>((registry::TUnknown{}));
int arm_index = 0;
while (arm_index < arms.size()){
{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> arm_environment = pattern_env::env_for_pattern_substituted(inference_context.type_env, arms[arm_index]->pat, inference_context.registry, substitution);
check_context::CheckContext arm_context = check_context::check_context_new(arm_environment, inference_context.registry);
infer_result::InferResult arm_result = infer_expr(arms[arm_index]->body, arm_context);
collected_errors = ast::diagnostics_append(collected_errors, arm_result.errors);
arm_type = arm_index == 0 ? arm_result.inferred_type : arm_type;
mlc::Array<ast::Diagnostic> arm_mismatch = type_diagnostics::match_arm_type_mismatch_diagnostic(arm_index, arm_type, arm_result.inferred_type, arms[arm_index]->body);
collected_errors = ast::diagnostics_append(collected_errors, arm_mismatch);
arm_index = arm_index + 1;
}
}
return infer_result::InferResult{arm_type, collected_errors};
}

infer_result::InferResult infer_expr_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context) noexcept{return infer_field_values_errors(infer_result::infer_ok(std::make_shared<registry::Type>(registry::TNamed(type_name))), field_values, inference_context, type_name);}

infer_result::InferResult infer_expr_record_update(mlc::String type_name, std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context) noexcept{
infer_result::InferResult base_result = infer_expr(base, inference_context);
return infer_field_values_errors(infer_result::InferResult_with_type(base_result, std::make_shared<registry::Type>(registry::TNamed(type_name))), field_values, inference_context, type_name);
}

infer_result::InferResult infer_expr_array_literal(mlc::Array<std::shared_ptr<ast::Expr>> elements, check_context::CheckContext inference_context) noexcept{
std::shared_ptr<registry::Type> first_element_type = elements.size() > 0 ? infer_expr(elements[0], inference_context).inferred_type : std::make_shared<registry::Type>((registry::TUnknown{}));
return infer_arguments_errors(infer_result::infer_ok(std::make_shared<registry::Type>(registry::TArray(first_element_type))), elements, inference_context);
}

infer_result::InferResult infer_expr_question(std::shared_ptr<ast::Expr> inner, check_context::CheckContext inference_context) noexcept{return infer_question_expression::infer_question_from_inner_result(infer_expr(inner, inference_context));}

infer_result::InferResult infer_expr_lambda(mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body, check_context::CheckContext inference_context) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> lambda_environment = inference_context.type_env;
mlc::Array<std::shared_ptr<registry::Type>> parameter_types = {};
int parameter_index = 0;
while (parameter_index < parameter_names.size()){
{
parameter_types.push_back(std::make_shared<registry::Type>((registry::TUnknown{})));
lambda_environment.set(parameter_names[parameter_index], std::make_shared<registry::Type>((registry::TUnknown{})));
parameter_index = parameter_index + 1;
}
}
check_context::CheckContext lambda_context = check_context::check_context_new(lambda_environment, inference_context.registry);
infer_result::InferResult body_result = infer_expr(body, lambda_context);
return infer_result::infer_ok(std::make_shared<registry::Type>(registry::TFn(parameter_types, body_result.inferred_type)));
}

infer_result::InferResult infer_expr(std::shared_ptr<ast::Expr> expression, check_context::CheckContext inference_context) noexcept{return std::visit(overloaded{
  [&](const ExprInt& exprint) -> infer_result::InferResult { auto [_w0, _w1] = exprint; return infer_literals::infer_expr_integer_literal(); },
  [&](const ExprStr& exprstr) -> infer_result::InferResult { auto [_w0, _w1] = exprstr; return infer_literals::infer_expr_string_literal(); },
  [&](const ExprBool& exprbool) -> infer_result::InferResult { auto [_w0, _w1] = exprbool; return infer_literals::infer_expr_boolean_literal(); },
  [&](const ExprUnit& exprunit) -> infer_result::InferResult { auto [_w0] = exprunit; return infer_literals::infer_expr_unit_literal(); },
  [&](const ExprExtern& exprextern) -> infer_result::InferResult { auto [_w0] = exprextern; return infer_literals::infer_expr_extern_placeholder(); },
  [&](const ExprIdent& exprident) -> infer_result::InferResult { auto [name, _w0] = exprident; return infer_expr_ident::infer_expr_identifier(name, inference_context); },
  [&](const ExprBin& exprbin) -> infer_result::InferResult { auto [operation, left, right, span] = exprbin; return infer_expr_binary(operation, left, right, span, inference_context); },
  [&](const ExprUn& exprun) -> infer_result::InferResult { auto [operation, inner, span] = exprun; return infer_expr_unary(operation, inner, span, inference_context); },
  [&](const ExprCall& exprcall) -> infer_result::InferResult { auto [function, call_arguments, call_source_span] = exprcall; return infer_expr_call(function, call_arguments, call_source_span, inference_context); },
  [&](const ExprMethod& exprmethod) -> infer_result::InferResult { auto [object, method_name, margs, method_span] = exprmethod; return infer_expr_method(object, method_name, margs, method_span, inference_context); },
  [&](const ExprField& exprfield) -> infer_result::InferResult { auto [object, field_name, field_source_span] = exprfield; return infer_expr_field(object, field_name, field_source_span, inference_context); },
  [&](const ExprIndex& exprindex) -> infer_result::InferResult { auto [object, index_expression, bracket_source_span] = exprindex; return infer_expr_index(object, index_expression, bracket_source_span, inference_context); },
  [&](const ExprIf& exprif) -> infer_result::InferResult { auto [condition, then_expr, else_expr, _w0] = exprif; return infer_expr_conditional(condition, then_expr, else_expr, inference_context); },
  [&](const ExprBlock& exprblock) -> infer_result::InferResult { auto [statements, result, _w0] = exprblock; return infer_expr_block(statements, result, inference_context); },
  [&](const ExprWhile& exprwhile) -> infer_result::InferResult { auto [condition, statements, _w0] = exprwhile; return infer_expr_while_loop(condition, statements, inference_context); },
  [&](const ExprFor& exprfor) -> infer_result::InferResult { auto [variable, iterator, statements, _w0] = exprfor; return infer_expr_for_loop(variable, iterator, statements, inference_context); },
  [&](const ExprMatch& exprmatch) -> infer_result::InferResult { auto [subject, arms, _w0] = exprmatch; return infer_expr_match(subject, arms, inference_context); },
  [&](const ExprRecord& exprrecord) -> infer_result::InferResult { auto [name, field_vals, _w0] = exprrecord; return infer_expr_record(name, field_vals, inference_context); },
  [&](const ExprRecordUpdate& exprrecordupdate) -> infer_result::InferResult { auto [name, base, field_vals, _w0] = exprrecordupdate; return infer_expr_record_update(name, base, field_vals, inference_context); },
  [&](const ExprArray& exprarray) -> infer_result::InferResult { auto [elements, _w0] = exprarray; return infer_expr_array_literal(elements, inference_context); },
  [&](const ExprQuestion& exprquestion) -> infer_result::InferResult { auto [inner, _w0] = exprquestion; return infer_expr_question(inner, inference_context); },
  [&](const ExprLambda& exprlambda) -> infer_result::InferResult { auto [params, body, _w0] = exprlambda; return infer_expr_lambda(params, body, inference_context); }
}, (*expression)._);}

infer_result::StmtInferResult infer_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept{
mlc::Array<ast::Diagnostic> collected_errors = {};
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> current_environment = inference_context.type_env;
int statement_index = 0;
while (statement_index < statements.size()){
{
std::visit(overloaded{
  [&](const StmtLet& stmtlet) -> std::tuple<> { auto [binding_name, _w0, _w1, value_expression, _w2] = stmtlet; return [&]() -> std::tuple<> { 
  infer_result::InferResult value_result = infer_expr(value_expression, check_context::check_context_new(current_environment, inference_context.registry));
  current_environment.set(binding_name, value_result.inferred_type);
  collected_errors = ast::diagnostics_append(collected_errors, value_result.errors);
  return std::make_tuple();
 }(); },
  [&](const StmtLetConst& stmtletconst) -> std::tuple<> { auto [binding_name, _w0, value_expression, _w1] = stmtletconst; return [&]() -> std::tuple<> { 
  infer_result::InferResult value_result = infer_expr(value_expression, check_context::check_context_new(current_environment, inference_context.registry));
  current_environment.set(binding_name, value_result.inferred_type);
  collected_errors = ast::diagnostics_append(collected_errors, value_result.errors);
  return std::make_tuple();
 }(); },
  [&](const StmtExpr& stmtexpr) -> std::tuple<> { auto [expression, _w0] = stmtexpr; return [&]() -> std::tuple<> { 
  infer_result::InferResult expression_result = infer_expr(expression, check_context::check_context_new(current_environment, inference_context.registry));
  collected_errors = ast::diagnostics_append(collected_errors, expression_result.errors);
  return std::make_tuple();
 }(); },
  [&](const StmtReturn& stmtreturn) -> std::tuple<> { auto [return_expression, _w0] = stmtreturn; return [&]() -> std::tuple<> { 
  infer_result::InferResult return_result = infer_expr(return_expression, check_context::check_context_new(current_environment, inference_context.registry));
  collected_errors = ast::diagnostics_append(collected_errors, return_result.errors);
  return std::make_tuple();
 }(); },
  [&](const StmtBreak& stmtbreak) -> std::tuple<> { auto [_w0] = stmtbreak; return std::make_tuple(); },
  [&](const StmtContinue& stmtcontinue) -> std::tuple<> { auto [_w0] = stmtcontinue; return std::make_tuple(); }
}, (*statements[statement_index])._);
statement_index = statement_index + 1;
}
}
return infer_result::StmtInferResult{current_environment, collected_errors};
}

} // namespace infer
