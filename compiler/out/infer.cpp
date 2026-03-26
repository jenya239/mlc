#include "infer.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"

namespace infer {

using namespace ast;
using namespace registry;
using namespace check_context;
using namespace ast_tokens;

infer::InferResult InferResult_with_type(infer::InferResult self, std::shared_ptr<registry::Type> new_type) noexcept;

infer::InferResult InferResult_absorb(infer::InferResult self, infer::InferResult other) noexcept;

infer::InferResult InferResult_absorb_stmt(infer::InferResult self, infer::StmtInferResult statement_result) noexcept;

infer::InferResult infer_ok(std::shared_ptr<registry::Type> type_value) noexcept;

std::shared_ptr<registry::Type> binary_operation_result_type(mlc::String operation, std::shared_ptr<registry::Type> left_type) noexcept;

std::shared_ptr<registry::Type> builtin_method_return_type(mlc::String method_name) noexcept;

infer::InferResult infer_arguments_errors(infer::InferResult initial, mlc::Array<std::shared_ptr<ast::Expr>> expressions, check_context::CheckContext inference_context) noexcept;

infer::InferResult infer_field_values_errors(infer::InferResult initial, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context) noexcept;

std::shared_ptr<registry::Type> field_type_from_object(std::shared_ptr<registry::Type> object_type, mlc::String field_name, registry::TypeRegistry registry) noexcept;

infer::InferResult infer_expr_integer_literal() noexcept;

infer::InferResult infer_expr_string_literal() noexcept;

infer::InferResult infer_expr_boolean_literal() noexcept;

infer::InferResult infer_expr_unit_literal() noexcept;

infer::InferResult infer_expr_extern_placeholder() noexcept;

infer::InferResult infer_expr_identifier(mlc::String name, check_context::CheckContext inference_context) noexcept;

infer::InferResult infer_expr_binary(mlc::String operation, std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, check_context::CheckContext inference_context) noexcept;

infer::InferResult infer_expr_unary(mlc::String operation, std::shared_ptr<ast::Expr> inner, check_context::CheckContext inference_context) noexcept;

infer::InferResult infer_expr_call(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, check_context::CheckContext inference_context) noexcept;

infer::InferResult infer_expr_method(std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, check_context::CheckContext inference_context) noexcept;

infer::InferResult infer_expr_field(std::shared_ptr<ast::Expr> object, mlc::String field_name, ast::Span field_source_span, check_context::CheckContext inference_context) noexcept;

infer::InferResult infer_expr_index(std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expression, check_context::CheckContext inference_context) noexcept;

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

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pattern(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, std::shared_ptr<ast::Pat> pattern, registry::TypeRegistry registry) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pattern_with_type(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, std::shared_ptr<ast::Pat> pattern, std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept;

infer::InferResult InferResult_with_type(infer::InferResult self, std::shared_ptr<registry::Type> new_type) noexcept{return infer::InferResult{new_type, self.errors};}

infer::InferResult InferResult_absorb(infer::InferResult self, infer::InferResult other) noexcept{return infer::InferResult{self.inferred_type, ast::diagnostics_append(self.errors, other.errors)};}

infer::InferResult InferResult_absorb_stmt(infer::InferResult self, infer::StmtInferResult statement_result) noexcept{return infer::InferResult{self.inferred_type, ast::diagnostics_append(statement_result.errors, self.errors)};}

infer::InferResult infer_ok(std::shared_ptr<registry::Type> type_value) noexcept{return infer::InferResult{type_value, {}};}

std::shared_ptr<registry::Type> binary_operation_result_type(mlc::String operation, std::shared_ptr<registry::Type> left_type) noexcept{return operation == mlc::String("+") || operation == mlc::String("-") || operation == mlc::String("*") || operation == mlc::String("/") || operation == mlc::String("%") ? [&]() { if (std::holds_alternative<registry::TString>((*left_type))) {  return std::make_shared<registry::Type>((registry::TString{})); } return std::make_shared<registry::Type>((registry::TI32{})); }() : operation == mlc::String("=") ? std::make_shared<registry::Type>((registry::TUnit{})) : std::make_shared<registry::Type>((registry::TBool{}));}

std::shared_ptr<registry::Type> builtin_method_return_type(mlc::String method_name) noexcept{return method_name == mlc::String("length") || method_name == mlc::String("size") || method_name == mlc::String("to_i") ? std::make_shared<registry::Type>((registry::TI32{})) : method_name == mlc::String("push") || method_name == mlc::String("set") ? std::make_shared<registry::Type>((registry::TUnit{})) : method_name == mlc::String("char_at") || method_name == mlc::String("join") || method_name == mlc::String("to_string") || method_name == mlc::String("substring") || method_name == mlc::String("to_lower") ? std::make_shared<registry::Type>((registry::TString{})) : method_name == mlc::String("has") ? std::make_shared<registry::Type>((registry::TBool{})) : std::make_shared<registry::Type>((registry::TUnknown{}));}

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

std::shared_ptr<registry::Type> field_type_from_object(std::shared_ptr<registry::Type> object_type, mlc::String field_name, registry::TypeRegistry registry) noexcept{
mlc::String type_name = [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*object_type))) { auto _v_tnamed = std::get<registry::TNamed>((*object_type)); auto [name] = _v_tnamed; return name; } return mlc::String(""); }();
return type_name != mlc::String("") && registry::TypeRegistry_has_fields(registry, type_name) ? [&]() -> std::shared_ptr<registry::Type> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> field_map = registry::TypeRegistry_fields_for(registry, type_name);
  return field_map.has(field_name) ? field_map.get(field_name) : std::make_shared<registry::Type>((registry::TUnknown{}));
 }() : std::make_shared<registry::Type>((registry::TUnknown{}));
}

infer::InferResult infer_expr_integer_literal() noexcept{return infer_ok(std::make_shared<registry::Type>((registry::TI32{})));}

infer::InferResult infer_expr_string_literal() noexcept{return infer_ok(std::make_shared<registry::Type>((registry::TString{})));}

infer::InferResult infer_expr_boolean_literal() noexcept{return infer_ok(std::make_shared<registry::Type>((registry::TBool{})));}

infer::InferResult infer_expr_unit_literal() noexcept{return infer_ok(std::make_shared<registry::Type>((registry::TUnit{})));}

infer::InferResult infer_expr_extern_placeholder() noexcept{return infer_ok(std::make_shared<registry::Type>((registry::TUnit{})));}

infer::InferResult infer_expr_identifier(mlc::String name, check_context::CheckContext inference_context) noexcept{return inference_context.type_env.has(name) ? infer_ok(inference_context.type_env.get(name)) : registry::TypeRegistry_has_fn(inference_context.registry, name) ? infer_ok(registry::TypeRegistry_fn_type(inference_context.registry, name)) : registry::TypeRegistry_has_ctor(inference_context.registry, name) ? infer_ok(registry::TypeRegistry_ctor_type(inference_context.registry, name)) : infer_ok(std::make_shared<registry::Type>((registry::TUnknown{})));}

infer::InferResult infer_expr_binary(mlc::String operation, std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, check_context::CheckContext inference_context) noexcept{
infer::InferResult left_result = infer_expr(left, inference_context);
infer::InferResult right_result = infer_expr(right, inference_context);
return InferResult_with_type(InferResult_absorb(left_result, right_result), binary_operation_result_type(operation, left_result.inferred_type));
}

infer::InferResult infer_expr_unary(mlc::String operation, std::shared_ptr<ast::Expr> inner, check_context::CheckContext inference_context) noexcept{
infer::InferResult inner_result = infer_expr(inner, inference_context);
auto result_type = operation == mlc::String("!") ? std::make_shared<registry::Type>((registry::TBool{})) : inner_result.inferred_type;
return InferResult_with_type(inner_result, result_type);
}

infer::InferResult infer_expr_call(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, check_context::CheckContext inference_context) noexcept{
infer::InferResult function_result = infer_expr(function, inference_context);
infer::InferResult with_arguments = infer_arguments_errors(function_result, call_arguments, inference_context);
std::shared_ptr<registry::Type> return_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TFn>((*function_result.inferred_type))) { auto _v_tfn = std::get<registry::TFn>((*function_result.inferred_type)); auto [_w0, ret] = _v_tfn; return ret; } return std::make_shared<registry::Type>((registry::TUnknown{})); }();
return InferResult_with_type(with_arguments, return_type);
}

infer::InferResult infer_expr_method(std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, check_context::CheckContext inference_context) noexcept{
infer::InferResult object_result = infer_expr(object, inference_context);
return InferResult_with_type(infer_arguments_errors(object_result, method_arguments, inference_context), builtin_method_return_type(method_name));
}

infer::InferResult infer_expr_field(std::shared_ptr<ast::Expr> object, mlc::String field_name, ast::Span field_source_span, check_context::CheckContext inference_context) noexcept{
infer::InferResult object_result = infer_expr(object, inference_context);
std::shared_ptr<registry::Type> resolved_field_type = field_type_from_object(object_result.inferred_type, field_name, inference_context.registry);
mlc::String type_name = [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*object_result.inferred_type))) { auto _v_tnamed = std::get<registry::TNamed>((*object_result.inferred_type)); auto [name] = _v_tnamed; return name; } return mlc::String(""); }();
mlc::Array<ast::Diagnostic> attached = {};
if (type_name != mlc::String("") && registry::TypeRegistry_has_fields(inference_context.registry, type_name)){
{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> field_map = registry::TypeRegistry_fields_for(inference_context.registry, type_name);
if (!field_map.has(field_name)){
attached.push_back(ast::diagnostic_error(mlc::String("undefined field: ") + field_name + mlc::String(" on ") + type_name, field_source_span));
std::make_tuple();
}
}
}
return infer::InferResult{resolved_field_type, ast::diagnostics_append(object_result.errors, attached)};
}

infer::InferResult infer_expr_index(std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expression, check_context::CheckContext inference_context) noexcept{
infer::InferResult object_result = infer_expr(object, inference_context);
infer::InferResult index_result = infer_expr(index_expression, inference_context);
std::shared_ptr<registry::Type> element_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TArray>((*object_result.inferred_type))) { auto _v_tarray = std::get<registry::TArray>((*object_result.inferred_type)); auto [inner] = _v_tarray; return inner; } return std::make_shared<registry::Type>((registry::TUnknown{})); }();
return InferResult_with_type(InferResult_absorb(object_result, index_result), element_type);
}

infer::InferResult infer_expr_conditional(std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expression, std::shared_ptr<ast::Expr> else_expression, check_context::CheckContext inference_context) noexcept{
infer::InferResult condition_result = infer_expr(condition, inference_context);
infer::InferResult then_result = infer_expr(then_expression, inference_context);
infer::InferResult else_result = infer_expr(else_expression, inference_context);
return InferResult_absorb(InferResult_absorb(then_result, condition_result), else_result);
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
infer::InferResult iterator_result = infer_expr(iterator, inference_context);
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
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> arm_environment = env_for_pattern(inference_context.type_env, arms[arm_index]->pat, inference_context.registry);
check_context::CheckContext arm_context = check_context::check_context_new(arm_environment, inference_context.registry);
infer::InferResult arm_result = infer_expr(arms[arm_index]->body, arm_context);
collected_errors = ast::diagnostics_append(collected_errors, arm_result.errors);
if (arm_index == 0){
{
arm_type = arm_result.inferred_type;
}
}
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
int parameter_index = 0;
while (parameter_index < parameter_names.size()){
{
lambda_environment.set(parameter_names[parameter_index], std::make_shared<registry::Type>((registry::TUnknown{})));
parameter_index = parameter_index + 1;
}
}
check_context::CheckContext lambda_context = check_context::check_context_new(lambda_environment, inference_context.registry);
infer::InferResult body_result = infer_expr(body, lambda_context);
return infer_ok(std::make_shared<registry::Type>(registry::TFn({}, body_result.inferred_type)));
}

infer::InferResult infer_expr(std::shared_ptr<ast::Expr> expression, check_context::CheckContext inference_context) noexcept{return std::visit(overloaded{
  [&](const ExprInt& exprint) -> infer::InferResult { auto [_w0, _w1] = exprint; return infer_expr_integer_literal(); },
  [&](const ExprStr& exprstr) -> infer::InferResult { auto [_w0, _w1] = exprstr; return infer_expr_string_literal(); },
  [&](const ExprBool& exprbool) -> infer::InferResult { auto [_w0, _w1] = exprbool; return infer_expr_boolean_literal(); },
  [&](const ExprUnit& exprunit) -> infer::InferResult { auto [_w0] = exprunit; return infer_expr_unit_literal(); },
  [&](const ExprExtern& exprextern) -> infer::InferResult { auto [_w0] = exprextern; return infer_expr_extern_placeholder(); },
  [&](const ExprIdent& exprident) -> infer::InferResult { auto [name, _w0] = exprident; return infer_expr_identifier(name, inference_context); },
  [&](const ExprBin& exprbin) -> infer::InferResult { auto [operation, left, right, _w0] = exprbin; return infer_expr_binary(operation, left, right, inference_context); },
  [&](const ExprUn& exprun) -> infer::InferResult { auto [operation, inner, _w0] = exprun; return infer_expr_unary(operation, inner, inference_context); },
  [&](const ExprCall& exprcall) -> infer::InferResult { auto [function, call_arguments, _w0] = exprcall; return infer_expr_call(function, call_arguments, inference_context); },
  [&](const ExprMethod& exprmethod) -> infer::InferResult { auto [object, method_name, margs, _w0] = exprmethod; return infer_expr_method(object, method_name, margs, inference_context); },
  [&](const ExprField& exprfield) -> infer::InferResult { auto [object, field_name, field_source_span] = exprfield; return infer_expr_field(object, field_name, field_source_span, inference_context); },
  [&](const ExprIndex& exprindex) -> infer::InferResult { auto [object, index_expression, _w0] = exprindex; return infer_expr_index(object, index_expression, inference_context); },
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

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pattern(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, std::shared_ptr<ast::Pat> pattern, registry::TypeRegistry registry) noexcept{
return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { if (std::holds_alternative<ast::PatIdent>((*pattern))) { auto _v_patident = std::get<ast::PatIdent>((*pattern)); auto [binding_name, _w0] = _v_patident; return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> environment = type_environment;
  environment.set(binding_name, std::make_shared<registry::Type>((registry::TUnknown{})));
  return environment;
 }(); } if (std::holds_alternative<ast::PatCtor>((*pattern))) { auto _v_patctor = std::get<ast::PatCtor>((*pattern)); auto [constructor_name, sub_patterns, _w0] = _v_patctor; return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> parameter_types = registry::TypeRegistry_ctor_params_for(registry, constructor_name);
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> current_environment = type_environment;
  int sub_index = 0;
  while (sub_index < sub_patterns.size()){
{
std::shared_ptr<registry::Type> sub_type = sub_index < parameter_types.size() ? parameter_types[sub_index] : std::make_shared<registry::Type>((registry::TUnknown{}));
current_environment = env_for_pattern_with_type(current_environment, sub_patterns[sub_index], sub_type, registry);
sub_index = sub_index + 1;
}
}
  return current_environment;
 }(); } return type_environment; }();
}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> env_for_pattern_with_type(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, std::shared_ptr<ast::Pat> pattern, std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept{
return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { if (std::holds_alternative<ast::PatIdent>((*pattern))) { auto _v_patident = std::get<ast::PatIdent>((*pattern)); auto [binding_name, _w0] = _v_patident; return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> environment = type_environment;
  environment.set(binding_name, type_value);
  return environment;
 }(); } return env_for_pattern(type_environment, pattern, registry); }();
}

} // namespace infer
