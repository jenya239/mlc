#include "infer.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "infer_literals.hpp"
#include "infer_for_support.hpp"
#include "infer_question_expression.hpp"
#include "infer_operand_combine.hpp"
#include "type_diagnostics.hpp"
#include "semantic_type_structure.hpp"
#include "infer_call.hpp"
#include "registry.hpp"
#include "infer_expr_ident.hpp"
#include "pattern_env.hpp"
#include "infer_match.hpp"
#include "let_pattern_infer.hpp"
#include "infer_array_method.hpp"
#include "infer_result_option_method.hpp"

namespace infer {

using namespace ast;
using namespace registry;
using namespace check_context;
using namespace infer_result;
using namespace infer_literals;
using namespace infer_for_support;
using namespace infer_question_expression;
using namespace infer_operand_combine;
using namespace type_diagnostics;
using namespace semantic_type_structure;
using namespace infer_call;
using namespace registry;
using namespace infer_expr_ident;
using namespace pattern_env;
using namespace infer_match;
using namespace let_pattern_infer;
using namespace infer_array_method;
using namespace infer_result_option_method;
using namespace ast_tokens;

infer_result::InferResult infer_arguments_errors(infer_result::InferResult initial, mlc::Array<std::shared_ptr<ast::Expr>> expressions, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_field_values_errors(infer_result::InferResult initial, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context, mlc::String record_type_name_for_fields) noexcept;

infer_result::InferResult infer_expr_binary(mlc::String operation, std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, ast::Span source_span, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_unary(mlc::String operation, std::shared_ptr<ast::Expr> inner, ast::Span source_span, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_method(std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

infer_result::InferResult infer_expr_field(std::shared_ptr<ast::Expr> object, mlc::String field_name, ast::Span field_source_span, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_index(std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expression, ast::Span bracket_source_span, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_conditional(std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expression, std::shared_ptr<ast::Expr> else_expression, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_block(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_while_loop(std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_for_loop(mlc::String variable_name, std::shared_ptr<ast::Expr> iterator, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context, ast::Span span) noexcept;

infer_result::InferResult infer_expr_record_update(mlc::String type_name, std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_tuple_literal(mlc::Array<std::shared_ptr<ast::Expr>> elements, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_array_literal(mlc::Array<std::shared_ptr<ast::Expr>> elements, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_question(std::shared_ptr<ast::Expr> inner, ast::Span question_span, check_context::CheckContext inference_context) noexcept;

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

infer_result::InferResult infer_expr_method(std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept{
infer_result::InferResult object_result = infer_expr_fn(object, inference_context);
return infer_result_option_method::should_infer_as_result_option_hof(object_result.inferred_type, method_name) ? infer_result_option_method::infer_result_option_hof_method_call(object_result, method_name, method_arguments, method_span, inference_context, infer_expr_fn) : infer_array_method::should_infer_as_array_hof(object_result.inferred_type, method_name) ? infer_array_method::infer_array_hof_method_call(object_result, method_name, method_arguments, method_span, inference_context, infer_expr_fn) : [&]() -> infer_result::InferResult { 
  infer_result::InferResult with_arguments = infer_arguments_errors(object_result, method_arguments, inference_context);
  return infer_operand_combine::infer_method_from_object_and_arguments(object_result, with_arguments, method_name, method_span, method_arguments.size(), inference_context.registry);
 }();
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

infer_result::InferResult infer_expr_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context, ast::Span span) noexcept{
mlc::Array<ast::Diagnostic> private_errors = registry::TypeRegistry_is_private_ctor(inference_context.registry, type_name) && inference_context.current_extend_type != type_name ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("private constructor: cannot construct ") + type_name + mlc::String(" outside its extend block"), span)} : mlc::Array<ast::Diagnostic>{};
infer_result::InferResult base = infer_result::InferResult{std::make_shared<registry::Type>(registry::TNamed(type_name)), private_errors};
return infer_field_values_errors(base, field_values, inference_context, type_name);
}

infer_result::InferResult infer_expr_record_update(mlc::String type_name, std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context) noexcept{
infer_result::InferResult base_result = infer_expr(base, inference_context);
return infer_field_values_errors(infer_result::InferResult_with_type(base_result, std::make_shared<registry::Type>(registry::TNamed(type_name))), field_values, inference_context, type_name);
}

infer_result::InferResult infer_expr_tuple_literal(mlc::Array<std::shared_ptr<ast::Expr>> elements, check_context::CheckContext inference_context) noexcept{
return elements.size() < 2 ? infer_arguments_errors(infer_result::infer_ok(std::make_shared<registry::Type>((registry::TUnknown{}))), elements, inference_context) : [&]() -> infer_result::InferResult { 
  infer_result::InferResult r = infer_result::infer_ok(std::make_shared<registry::Type>((registry::TUnknown{})));
  mlc::Array<std::shared_ptr<registry::Type>> types = {};
  int i = 0;
  while (i < elements.size()){
{
infer_result::InferResult er = infer_expr(elements[i], inference_context);
r = infer_result::InferResult_absorb(r, er);
types.push_back(er.inferred_type);
i = i + 1;
}
}
  return infer_result::InferResult_with_type(r, std::make_shared<registry::Type>(registry::TTuple(types)));
 }();
}

infer_result::InferResult infer_expr_array_literal(mlc::Array<std::shared_ptr<ast::Expr>> elements, check_context::CheckContext inference_context) noexcept{
std::shared_ptr<registry::Type> first_element_type = elements.size() > 0 ? infer_expr(elements[0], inference_context).inferred_type : std::make_shared<registry::Type>((registry::TUnknown{}));
return infer_arguments_errors(infer_result::infer_ok(std::make_shared<registry::Type>(registry::TArray(first_element_type))), elements, inference_context);
}

infer_result::InferResult infer_expr_question(std::shared_ptr<ast::Expr> inner, ast::Span question_span, check_context::CheckContext inference_context) noexcept{return infer_question_expression::infer_question_from_inner_result(infer_expr(inner, inference_context), question_span);}

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
  [&](const ExprCall& exprcall) -> infer_result::InferResult { auto [function, call_arguments, call_source_span] = exprcall; return infer_call::infer_expr_call(function, call_arguments, call_source_span, inference_context, infer_expr); },
  [&](const ExprMethod& exprmethod) -> infer_result::InferResult { auto [object, method_name, margs, method_span] = exprmethod; return infer_expr_method(object, method_name, margs, method_span, inference_context, infer_expr); },
  [&](const ExprField& exprfield) -> infer_result::InferResult { auto [object, field_name, field_source_span] = exprfield; return infer_expr_field(object, field_name, field_source_span, inference_context); },
  [&](const ExprIndex& exprindex) -> infer_result::InferResult { auto [object, index_expression, bracket_source_span] = exprindex; return infer_expr_index(object, index_expression, bracket_source_span, inference_context); },
  [&](const ExprIf& exprif) -> infer_result::InferResult { auto [condition, then_expr, else_expr, _w0] = exprif; return infer_expr_conditional(condition, then_expr, else_expr, inference_context); },
  [&](const ExprBlock& exprblock) -> infer_result::InferResult { auto [statements, result, _w0] = exprblock; return infer_expr_block(statements, result, inference_context); },
  [&](const ExprWhile& exprwhile) -> infer_result::InferResult { auto [condition, statements, _w0] = exprwhile; return infer_expr_while_loop(condition, statements, inference_context); },
  [&](const ExprFor& exprfor) -> infer_result::InferResult { auto [variable, iterator, statements, _w0] = exprfor; return infer_expr_for_loop(variable, iterator, statements, inference_context); },
  [&](const ExprMatch& exprmatch) -> infer_result::InferResult { auto [subject, arms, _w0] = exprmatch; return infer_match::infer_expr_match(subject, arms, inference_context, infer_expr); },
  [&](const ExprRecord& exprrecord) -> infer_result::InferResult { auto [name, field_vals, span] = exprrecord; return infer_expr_record(name, field_vals, inference_context, span); },
  [&](const ExprRecordUpdate& exprrecordupdate) -> infer_result::InferResult { auto [name, base, field_vals, _w0] = exprrecordupdate; return infer_expr_record_update(name, base, field_vals, inference_context); },
  [&](const ExprArray& exprarray) -> infer_result::InferResult { auto [elements, _w0] = exprarray; return infer_expr_array_literal(elements, inference_context); },
  [&](const ExprTuple& exprtuple) -> infer_result::InferResult { auto [elements, _w0] = exprtuple; return infer_expr_tuple_literal(elements, inference_context); },
  [&](const ExprQuestion& exprquestion) -> infer_result::InferResult { auto [inner, question_span] = exprquestion; return infer_expr_question(inner, question_span, inference_context); },
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
  [&](const StmtLetPat& stmtletpat) -> std::tuple<> { auto [pattern, _w0, _w1, value_expression, has_else, else_body, source_span] = stmtletpat; return [&]() -> std::tuple<> { 
  infer_result::InferResult value_result = infer_expr(value_expression, check_context::check_context_new(current_environment, inference_context.registry));
  let_pattern_infer::infer_let_pattern_env(pattern, value_result.inferred_type, current_environment, inference_context.registry);
  collected_errors = ast::diagnostics_append(collected_errors, value_result.errors);
  bool is_refutable = [&]() { if (std::holds_alternative<ast::PatCtor>((*pattern))) { auto _v_patctor = std::get<ast::PatCtor>((*pattern)); auto [_w0, _w1, _w2] = _v_patctor; return true; } return false; }();
  if (is_refutable && !has_else){
{
collected_errors = ast::diagnostics_append(collected_errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("refutable pattern in let requires `else` branch"), source_span)});
}
}
  if (has_else){
{
infer_result::InferResult else_result = infer_expr(else_body, check_context::check_context_new(current_environment, inference_context.registry));
collected_errors = ast::diagnostics_append(collected_errors, else_result.errors);
}
}
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
