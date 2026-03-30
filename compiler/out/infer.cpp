#include "infer.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "type_utils.hpp"
#include "type_diagnostics.hpp"
#include "pattern_env.hpp"

namespace infer {

using namespace ast;
using namespace registry;
using namespace check_context;
using namespace type_utils;
using namespace type_diagnostics;
using namespace pattern_env;
using namespace ast_tokens;

infer::InferResult InferResult_with_type(infer::InferResult self, std::shared_ptr<registry::Type> new_type) noexcept;

infer::InferResult InferResult_absorb(infer::InferResult self, infer::InferResult other) noexcept;

infer::InferResult InferResult_absorb_stmt(infer::InferResult self, infer::StmtInferResult statement_result) noexcept;

infer::InferResult infer_ok(std::shared_ptr<registry::Type> type_value) noexcept;

infer::InferResult infer_arguments_errors(infer::InferResult initial, mlc::Array<std::shared_ptr<ast::Expr>> expressions, check_context::CheckContext inference_context) noexcept;

infer::InferResult infer_field_values_errors(infer::InferResult initial, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context) noexcept;

infer::InferResult infer_expr_integer_literal() noexcept;

infer::InferResult infer_expr_string_literal() noexcept;

infer::InferResult infer_expr_boolean_literal() noexcept;

infer::InferResult infer_expr_unit_literal() noexcept;

infer::InferResult infer_expr_extern_placeholder() noexcept;

infer::InferResult infer_expr_identifier(mlc::String name, check_context::CheckContext inference_context) noexcept;

infer::InferResult infer_expr_binary(mlc::String operation, std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, ast::Span source_span, check_context::CheckContext inference_context) noexcept;

infer::InferResult infer_expr_unary(mlc::String operation, std::shared_ptr<ast::Expr> inner, ast::Span source_span, check_context::CheckContext inference_context) noexcept;

infer::InferResult infer_expr_call_for_constructor_name(mlc::String constructor_name, infer::InferResult with_arguments, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, check_context::CheckContext inference_context) noexcept;

infer::InferResult infer_expr_call_non_constructor_arity_only(infer::InferResult base, infer::InferResult function_result, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, std::shared_ptr<registry::Type> return_type) noexcept;

infer::InferResult infer_expr_call_non_constructor_not_callable(infer::InferResult base, infer::InferResult function_result, ast::Span call_source_span) noexcept;

infer::InferResult infer_expr_call_non_constructor(infer::InferResult function_result, infer::InferResult with_arguments, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span) noexcept;

infer::InferResult infer_expr_call(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, check_context::CheckContext inference_context) noexcept;

infer::InferResult infer_expr_method(std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context) noexcept;

infer::InferResult infer_expr_field(std::shared_ptr<ast::Expr> object, mlc::String field_name, ast::Span field_source_span, check_context::CheckContext inference_context) noexcept;

infer::InferResult infer_expr_index(std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expression, ast::Span bracket_source_span, check_context::CheckContext inference_context) noexcept;

infer::InferResult infer_expr_conditional(std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expression, std::shared_ptr<ast::Expr> else_expression, check_context::CheckContext inference_context) noexcept;

infer::InferResult infer_expr_block(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression, check_context::CheckContext inference_context) noexcept;

infer::InferResult infer_expr_while_loop(std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept;

infer::InferResult infer_expr_for_loop(mlc::String variable_name, std::shared_ptr<ast::Expr> iterator, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept;

infer::InferResult infer_expr_match(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> arms, check_context::CheckContext inference_context) noexcept;

infer::InferResult infer_expr_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context) noexcept;

infer::InferResult infer_expr_record_update(mlc::String type_name, std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context) noexcept;

infer::InferResult infer_expr_array_literal(mlc::Array<std::shared_ptr<ast::Expr>> elements, check_context::CheckContext inference_context) noexcept;

infer::InferResult infer_expr_question(std::shared_ptr<ast::Expr> inner, check_context::CheckContext inference_context) noexcept;

infer::InferResult infer_expr_lambda(mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body, check_context::CheckContext inference_context) noexcept;

infer::InferResult infer_expr(std::shared_ptr<ast::Expr> expression, check_context::CheckContext inference_context) noexcept;

infer::StmtInferResult infer_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept;

infer::InferResult InferResult_with_type(infer::InferResult self, std::shared_ptr<registry::Type> new_type) noexcept{return infer::InferResult{new_type, self.errors};}

infer::InferResult InferResult_absorb(infer::InferResult self, infer::InferResult other) noexcept{return infer::InferResult{self.inferred_type, ast::diagnostics_append(self.errors, other.errors)};}

infer::InferResult InferResult_absorb_stmt(infer::InferResult self, infer::StmtInferResult statement_result) noexcept{return infer::InferResult{self.inferred_type, ast::diagnostics_append(statement_result.errors, self.errors)};}

infer::InferResult infer_ok(std::shared_ptr<registry::Type> type_value) noexcept{return infer::InferResult{type_value, {}};}

infer::InferResult infer_arguments_errors(infer::InferResult initial, mlc::Array<std::shared_ptr<ast::Expr>> expressions, check_context::CheckContext inference_context) noexcept{
infer::InferResult result = std::move(initial);
int index = 0;
while (index < expressions.size()){
{
result = InferResult_absorb(result, infer_expr(expressions[index], inference_context));
index = index + 1;
}
}
return result;
}

infer::InferResult infer_field_values_errors(infer::InferResult initial, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context) noexcept{
infer::InferResult result = std::move(initial);
int index = 0;
while (index < field_values.size()){
{
result = InferResult_absorb(result, infer_expr(field_values[index]->val, inference_context));
index = index + 1;
}
}
return result;
}

infer::InferResult infer_expr_integer_literal() noexcept{return infer_ok(std::make_shared<registry::Type>((registry::TI32{})));}

infer::InferResult infer_expr_string_literal() noexcept{return infer_ok(std::make_shared<registry::Type>((registry::TString{})));}

infer::InferResult infer_expr_boolean_literal() noexcept{return infer_ok(std::make_shared<registry::Type>((registry::TBool{})));}

infer::InferResult infer_expr_unit_literal() noexcept{return infer_ok(std::make_shared<registry::Type>((registry::TUnit{})));}

infer::InferResult infer_expr_extern_placeholder() noexcept{return infer_ok(std::make_shared<registry::Type>((registry::TUnit{})));}

infer::InferResult infer_expr_identifier(mlc::String name, check_context::CheckContext inference_context) noexcept{return inference_context.type_env.has(name) ? infer_ok(inference_context.type_env.get(name)) : registry::TypeRegistry_has_fn(inference_context.registry, name) ? infer_ok(registry::TypeRegistry_fn_type(inference_context.registry, name)) : registry::TypeRegistry_has_ctor(inference_context.registry, name) ? infer_ok(registry::TypeRegistry_ctor_type(inference_context.registry, name)) : infer_ok(std::make_shared<registry::Type>((registry::TUnknown{})));}

infer::InferResult infer_expr_binary(mlc::String operation, std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, ast::Span source_span, check_context::CheckContext inference_context) noexcept{
infer::InferResult left_result = infer_expr(left, inference_context);
infer::InferResult right_result = infer_expr(right, inference_context);
infer::InferResult merged = InferResult_absorb(left_result, right_result);
mlc::Array<ast::Diagnostic> operand_errors = type_diagnostics::infer_binary_operand_diagnostics(operation, left_result.inferred_type, right_result.inferred_type, source_span);
return infer::InferResult{type_utils::binary_operation_result_type(operation, left_result.inferred_type), ast::diagnostics_append(merged.errors, operand_errors)};
}

infer::InferResult infer_expr_unary(mlc::String operation, std::shared_ptr<ast::Expr> inner, ast::Span source_span, check_context::CheckContext inference_context) noexcept{
infer::InferResult inner_result = infer_expr(inner, inference_context);
mlc::Array<ast::Diagnostic> minus_errors = type_diagnostics::unary_minus_diagnostic(operation, inner_result.inferred_type, source_span);
mlc::Array<ast::Diagnostic> bang_errors = type_diagnostics::unary_bang_diagnostic(operation, inner_result.inferred_type, source_span);
auto result_type = operation == mlc::String("!") ? std::make_shared<registry::Type>((registry::TBool{})) : inner_result.inferred_type;
return infer::InferResult{result_type, ast::diagnostics_append(ast::diagnostics_append(inner_result.errors, minus_errors), bang_errors)};
}

infer::InferResult infer_expr_call_for_constructor_name(mlc::String constructor_name, infer::InferResult with_arguments, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, check_context::CheckContext inference_context) noexcept{
mlc::Array<std::shared_ptr<registry::Type>> constructor_parameter_types = registry::TypeRegistry_ctor_params_for(inference_context.registry, constructor_name);
mlc::Array<ast::Diagnostic> call_errors = type_diagnostics::constructor_arity_diagnostics(constructor_parameter_types.size(), call_arguments.size(), call_source_span);
return infer::InferResult{registry::TypeRegistry_ctor_type(inference_context.registry, constructor_name), ast::diagnostics_append(with_arguments.errors, call_errors)};
}

infer::InferResult infer_expr_call_non_constructor_arity_only(infer::InferResult base, infer::InferResult function_result, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, std::shared_ptr<registry::Type> return_type) noexcept{
mlc::Array<std::shared_ptr<registry::Type>> parameter_types = type_utils::function_parameter_list(function_result.inferred_type);
mlc::Array<ast::Diagnostic> arity_errors = type_diagnostics::call_arity_diagnostics(parameter_types.size(), call_arguments.size(), call_source_span);
return infer::InferResult{return_type, ast::diagnostics_append(base.errors, arity_errors)};
}

infer::InferResult infer_expr_call_non_constructor_not_callable(infer::InferResult base, infer::InferResult function_result, ast::Span call_source_span) noexcept{return type_utils::type_is_unknown(function_result.inferred_type) ? base : infer::InferResult{std::make_shared<registry::Type>((registry::TUnknown{})), ast::diagnostics_append(base.errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("called value is not a function"), call_source_span)})};}

infer::InferResult infer_expr_call_non_constructor(infer::InferResult function_result, infer::InferResult with_arguments, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span) noexcept{
std::shared_ptr<registry::Type> return_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TFn>((*function_result.inferred_type))) { auto _v_tfn = std::get<registry::TFn>((*function_result.inferred_type)); auto [_w0, return_ty] = _v_tfn; return return_ty; } return std::make_shared<registry::Type>((registry::TUnknown{})); }();
infer::InferResult base = InferResult_with_type(with_arguments, return_type);
return type_utils::type_is_function(function_result.inferred_type) ? infer_expr_call_non_constructor_arity_only(base, function_result, call_arguments, call_source_span, return_type) : infer_expr_call_non_constructor_not_callable(base, function_result, call_source_span);
}

infer::InferResult infer_expr_call(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, check_context::CheckContext inference_context) noexcept{
infer::InferResult function_result = infer_expr(function, inference_context);
infer::InferResult with_arguments = infer_arguments_errors(function_result, call_arguments, inference_context);
mlc::String callee_name = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*function)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*function)._); auto [name, _w0] = _v_exprident; return name; } return mlc::String(""); }();
return callee_name != mlc::String("") && registry::TypeRegistry_has_ctor(inference_context.registry, callee_name) ? infer_expr_call_for_constructor_name(callee_name, with_arguments, call_arguments, call_source_span, inference_context) : infer_expr_call_non_constructor(function_result, with_arguments, call_arguments, call_source_span);
}

infer::InferResult infer_expr_method(std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context) noexcept{
infer::InferResult object_result = infer_expr(object, inference_context);
mlc::Array<ast::Diagnostic> receiver_errors = type_diagnostics::infer_builtin_method_receiver_diagnostics(object_result.inferred_type, method_name, method_span);
infer::InferResult with_arguments = infer_arguments_errors(object_result, method_arguments, inference_context);
mlc::Array<ast::Diagnostic> arity_errors = type_diagnostics::method_arity_after_receiver(receiver_errors, method_name, method_arguments.size(), method_span);
return infer::InferResult{type_utils::builtin_method_return_type(method_name), ast::diagnostics_append(ast::diagnostics_append(with_arguments.errors, receiver_errors), arity_errors)};
}

infer::InferResult infer_expr_field(std::shared_ptr<ast::Expr> object, mlc::String field_name, ast::Span field_source_span, check_context::CheckContext inference_context) noexcept{
infer::InferResult object_result = infer_expr(object, inference_context);
std::shared_ptr<registry::Type> resolved_field_type = type_utils::field_type_from_object(object_result.inferred_type, field_name, inference_context.registry);
mlc::Array<ast::Diagnostic> attached = type_diagnostics::infer_expr_field_diagnostics(object_result.inferred_type, field_name, field_source_span, inference_context.registry);
return infer::InferResult{resolved_field_type, ast::diagnostics_append(object_result.errors, attached)};
}

infer::InferResult infer_expr_index(std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expression, ast::Span bracket_source_span, check_context::CheckContext inference_context) noexcept{
infer::InferResult object_result = infer_expr(object, inference_context);
infer::InferResult index_result = infer_expr(index_expression, inference_context);
std::shared_ptr<registry::Type> element_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TArray>((*object_result.inferred_type))) { auto _v_tarray = std::get<registry::TArray>((*object_result.inferred_type)); auto [inner] = _v_tarray; return inner; } return std::make_shared<registry::Type>((registry::TUnknown{})); }();
mlc::Array<ast::Diagnostic> extra_not_array = type_diagnostics::index_not_array_diagnostic(object_result.inferred_type, bracket_source_span);
mlc::Array<ast::Diagnostic> extra_bad_index = type_diagnostics::index_not_i32_diagnostic(index_result.inferred_type, bracket_source_span);
infer::InferResult merged = InferResult_absorb(object_result, index_result);
return infer::InferResult{element_type, ast::diagnostics_append(ast::diagnostics_append(merged.errors, extra_not_array), extra_bad_index)};
}

infer::InferResult infer_expr_conditional(std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expression, std::shared_ptr<ast::Expr> else_expression, check_context::CheckContext inference_context) noexcept{
infer::InferResult condition_result = infer_expr(condition, inference_context);
infer::InferResult then_result = infer_expr(then_expression, inference_context);
infer::InferResult else_result = infer_expr(else_expression, inference_context);
infer::InferResult merged = InferResult_absorb(InferResult_absorb(then_result, condition_result), else_result);
bool branches_mismatch = !type_utils::type_is_unknown(then_result.inferred_type) && !type_utils::type_is_unknown(else_result.inferred_type) && !type_utils::types_structurally_equal(then_result.inferred_type, else_result.inferred_type);
mlc::Array<ast::Diagnostic> branch_errors = type_diagnostics::if_branch_mismatch_diagnostic(branches_mismatch, then_result.inferred_type, else_result.inferred_type, else_expression);
return infer::InferResult{branches_mismatch ? then_result.inferred_type : merged.inferred_type, ast::diagnostics_append(merged.errors, branch_errors)};
}

infer::InferResult infer_expr_block(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression, check_context::CheckContext inference_context) noexcept{
infer::StmtInferResult statements_result = infer_statements(statements, inference_context);
infer::InferResult result_inference = infer_expr(result_expression, check_context::check_context_new(statements_result.type_env, inference_context.registry));
return InferResult_absorb_stmt(result_inference, statements_result);
}

infer::InferResult infer_expr_while_loop(std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept{
infer::InferResult condition_result = infer_expr(condition, inference_context);
infer::StmtInferResult statements_result = infer_statements(statements, inference_context);
return InferResult_with_type(InferResult_absorb_stmt(condition_result, statements_result), std::make_shared<registry::Type>((registry::TUnit{})));
}

infer::InferResult infer_expr_for_loop(mlc::String variable_name, std::shared_ptr<ast::Expr> iterator, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept{
infer::InferResult iterator_base = infer_expr(iterator, inference_context);
mlc::Array<ast::Diagnostic> range_errors = type_diagnostics::for_loop_range_diagnostic(iterator_base.inferred_type, iterator);
infer::InferResult iterator_result = infer::InferResult{iterator_base.inferred_type, ast::diagnostics_append(iterator_base.errors, range_errors)};
std::shared_ptr<registry::Type> element_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TArray>((*iterator_result.inferred_type))) { auto _v_tarray = std::get<registry::TArray>((*iterator_result.inferred_type)); auto [inner] = _v_tarray; return inner; } return std::make_shared<registry::Type>((registry::TUnknown{})); }();
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> inner_environment = inference_context.type_env;
inner_environment.set(variable_name, element_type);
check_context::CheckContext loop_context = check_context::check_context_new(inner_environment, inference_context.registry);
infer::StmtInferResult statements_result = infer_statements(statements, loop_context);
return InferResult_with_type(InferResult_absorb_stmt(iterator_result, statements_result), std::make_shared<registry::Type>((registry::TUnit{})));
}

infer::InferResult infer_expr_match(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> arms, check_context::CheckContext inference_context) noexcept{
infer::InferResult subject_result = infer_expr(subject, inference_context);
mlc::Array<ast::Diagnostic> collected_errors = subject_result.errors;
std::shared_ptr<registry::Type> arm_type = std::make_shared<registry::Type>((registry::TUnknown{}));
int arm_index = 0;
while (arm_index < arms.size()){
{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> arm_environment = pattern_env::env_for_pattern(inference_context.type_env, arms[arm_index]->pat, inference_context.registry);
check_context::CheckContext arm_context = check_context::check_context_new(arm_environment, inference_context.registry);
infer::InferResult arm_result = infer_expr(arms[arm_index]->body, arm_context);
collected_errors = ast::diagnostics_append(collected_errors, arm_result.errors);
arm_type = arm_index == 0 ? arm_result.inferred_type : arm_type;
mlc::Array<ast::Diagnostic> arm_mismatch = type_diagnostics::match_arm_type_mismatch_diagnostic(arm_index, arm_type, arm_result.inferred_type, arms[arm_index]->body);
collected_errors = ast::diagnostics_append(collected_errors, arm_mismatch);
arm_index = arm_index + 1;
}
}
return infer::InferResult{arm_type, collected_errors};
}

infer::InferResult infer_expr_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context) noexcept{return infer_field_values_errors(infer_ok(std::make_shared<registry::Type>(registry::TNamed(type_name))), field_values, inference_context);}

infer::InferResult infer_expr_record_update(mlc::String type_name, std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context) noexcept{
infer::InferResult base_result = infer_expr(base, inference_context);
return infer_field_values_errors(InferResult_with_type(base_result, std::make_shared<registry::Type>(registry::TNamed(type_name))), field_values, inference_context);
}

infer::InferResult infer_expr_array_literal(mlc::Array<std::shared_ptr<ast::Expr>> elements, check_context::CheckContext inference_context) noexcept{
std::shared_ptr<registry::Type> first_element_type = elements.size() > 0 ? infer_expr(elements[0], inference_context).inferred_type : std::make_shared<registry::Type>((registry::TUnknown{}));
return infer_arguments_errors(infer_ok(std::make_shared<registry::Type>(registry::TArray(first_element_type))), elements, inference_context);
}

infer::InferResult infer_expr_question(std::shared_ptr<ast::Expr> inner, check_context::CheckContext inference_context) noexcept{return infer_expr(inner, inference_context);}

infer::InferResult infer_expr_lambda(mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body, check_context::CheckContext inference_context) noexcept{
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
infer::InferResult body_result = infer_expr(body, lambda_context);
return infer_ok(std::make_shared<registry::Type>(registry::TFn(parameter_types, body_result.inferred_type)));
}

infer::InferResult infer_expr(std::shared_ptr<ast::Expr> expression, check_context::CheckContext inference_context) noexcept{return std::visit(overloaded{
  [&](const ExprInt& exprint) -> infer::InferResult { auto [_w0, _w1] = exprint; return infer_expr_integer_literal(); },
  [&](const ExprStr& exprstr) -> infer::InferResult { auto [_w0, _w1] = exprstr; return infer_expr_string_literal(); },
  [&](const ExprBool& exprbool) -> infer::InferResult { auto [_w0, _w1] = exprbool; return infer_expr_boolean_literal(); },
  [&](const ExprUnit& exprunit) -> infer::InferResult { auto [_w0] = exprunit; return infer_expr_unit_literal(); },
  [&](const ExprExtern& exprextern) -> infer::InferResult { auto [_w0] = exprextern; return infer_expr_extern_placeholder(); },
  [&](const ExprIdent& exprident) -> infer::InferResult { auto [name, _w0] = exprident; return infer_expr_identifier(name, inference_context); },
  [&](const ExprBin& exprbin) -> infer::InferResult { auto [operation, left, right, span] = exprbin; return infer_expr_binary(operation, left, right, span, inference_context); },
  [&](const ExprUn& exprun) -> infer::InferResult { auto [operation, inner, span] = exprun; return infer_expr_unary(operation, inner, span, inference_context); },
  [&](const ExprCall& exprcall) -> infer::InferResult { auto [function, call_arguments, call_source_span] = exprcall; return infer_expr_call(function, call_arguments, call_source_span, inference_context); },
  [&](const ExprMethod& exprmethod) -> infer::InferResult { auto [object, method_name, margs, method_span] = exprmethod; return infer_expr_method(object, method_name, margs, method_span, inference_context); },
  [&](const ExprField& exprfield) -> infer::InferResult { auto [object, field_name, field_source_span] = exprfield; return infer_expr_field(object, field_name, field_source_span, inference_context); },
  [&](const ExprIndex& exprindex) -> infer::InferResult { auto [object, index_expression, bracket_source_span] = exprindex; return infer_expr_index(object, index_expression, bracket_source_span, inference_context); },
  [&](const ExprIf& exprif) -> infer::InferResult { auto [condition, then_expr, else_expr, _w0] = exprif; return infer_expr_conditional(condition, then_expr, else_expr, inference_context); },
  [&](const ExprBlock& exprblock) -> infer::InferResult { auto [statements, result, _w0] = exprblock; return infer_expr_block(statements, result, inference_context); },
  [&](const ExprWhile& exprwhile) -> infer::InferResult { auto [condition, statements, _w0] = exprwhile; return infer_expr_while_loop(condition, statements, inference_context); },
  [&](const ExprFor& exprfor) -> infer::InferResult { auto [variable, iterator, statements, _w0] = exprfor; return infer_expr_for_loop(variable, iterator, statements, inference_context); },
  [&](const ExprMatch& exprmatch) -> infer::InferResult { auto [subject, arms, _w0] = exprmatch; return infer_expr_match(subject, arms, inference_context); },
  [&](const ExprRecord& exprrecord) -> infer::InferResult { auto [name, field_vals, _w0] = exprrecord; return infer_expr_record(name, field_vals, inference_context); },
  [&](const ExprRecordUpdate& exprrecordupdate) -> infer::InferResult { auto [name, base, field_vals, _w0] = exprrecordupdate; return infer_expr_record_update(name, base, field_vals, inference_context); },
  [&](const ExprArray& exprarray) -> infer::InferResult { auto [elements, _w0] = exprarray; return infer_expr_array_literal(elements, inference_context); },
  [&](const ExprQuestion& exprquestion) -> infer::InferResult { auto [inner, _w0] = exprquestion; return infer_expr_question(inner, inference_context); },
  [&](const ExprLambda& exprlambda) -> infer::InferResult { auto [params, body, _w0] = exprlambda; return infer_expr_lambda(params, body, inference_context); }
}, (*expression)._);}

infer::StmtInferResult infer_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept{
mlc::Array<ast::Diagnostic> collected_errors = {};
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> current_environment = inference_context.type_env;
int statement_index = 0;
while (statement_index < statements.size()){
{
std::visit(overloaded{
  [&](const StmtLet& stmtlet) -> std::tuple<> { auto [binding_name, _w0, _w1, value_expression, _w2] = stmtlet; return [&]() -> std::tuple<> { 
  infer::InferResult value_result = infer_expr(value_expression, check_context::check_context_new(current_environment, inference_context.registry));
  current_environment.set(binding_name, value_result.inferred_type);
  collected_errors = ast::diagnostics_append(collected_errors, value_result.errors);
  return std::make_tuple();
 }(); },
  [&](const StmtExpr& stmtexpr) -> std::tuple<> { auto [expression, _w0] = stmtexpr; return [&]() -> std::tuple<> { 
  infer::InferResult expression_result = infer_expr(expression, check_context::check_context_new(current_environment, inference_context.registry));
  collected_errors = ast::diagnostics_append(collected_errors, expression_result.errors);
  return std::make_tuple();
 }(); },
  [&](const StmtReturn& stmtreturn) -> std::tuple<> { auto [return_expression, _w0] = stmtreturn; return [&]() -> std::tuple<> { 
  infer::InferResult return_result = infer_expr(return_expression, check_context::check_context_new(current_environment, inference_context.registry));
  collected_errors = ast::diagnostics_append(collected_errors, return_result.errors);
  return std::make_tuple();
 }(); },
  [&](const StmtBreak& stmtbreak) -> std::tuple<> { auto [_w0] = stmtbreak; return std::make_tuple(); },
  [&](const StmtContinue& stmtcontinue) -> std::tuple<> { auto [_w0] = stmtcontinue; return std::make_tuple(); }
}, (*statements[statement_index])._);
statement_index = statement_index + 1;
}
}
return infer::StmtInferResult{current_environment, collected_errors};
}

} // namespace infer
