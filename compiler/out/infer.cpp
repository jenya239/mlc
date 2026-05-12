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
#include "infer_expr_ident.hpp"
#include "infer_match.hpp"
#include "let_pattern_infer.hpp"
#include "infer_array_method.hpp"
#include "infer_result_option_method.hpp"
#include "record_lit_merge.hpp"
#include "partial_application_desugar.hpp"

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
using namespace infer_expr_ident;
using namespace infer_match;
using namespace let_pattern_infer;
using namespace infer_array_method;
using namespace infer_result_option_method;
using namespace record_lit_merge;
using namespace partial_application_desugar;
using namespace ast_tokens;

struct Infer_tuple_literal_fold_state {infer_result::InferResult combined_inference;mlc::Array<std::shared_ptr<registry::Type>> member_types;};

struct Record_literal_spread_inference_fold_state {infer_result::InferResult inference_result_so_far;mlc::Array<std::shared_ptr<registry::Type>> spread_inferred_types_in_visit_order;};

struct Statement_sequence_inference_fold_state {mlc::Array<ast::Diagnostic> diagnostics_collected_so_far;mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment_under_construction;};

infer_result::InferResult infer_arguments_errors(infer_result::InferResult initial, mlc::Array<std::shared_ptr<ast::Expr>> expressions, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_record_field_binding_value_inference_step(infer_result::InferResult accumulator, std::shared_ptr<ast::FieldVal> field_value, check_context::CheckContext inference_context, mlc::String record_type_name_for_fields) noexcept;

infer_result::InferResult infer_field_values_errors(infer_result::InferResult initial, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context, mlc::String record_type_name_for_fields) noexcept;

infer_result::InferResult infer_explicit_record_literal_field_unknown_name_step(infer_result::InferResult accumulator, std::shared_ptr<ast::FieldVal> field_value_binding, mlc::String record_type_name, registry::TypeRegistry registry) noexcept;

infer_result::InferResult infer_explicit_record_literal_field_name_errors(infer_result::InferResult initial, mlc::String record_type_name, mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_field_values_flat, registry::TypeRegistry registry) noexcept;

infer_result::InferResult infer_expr_binary(mlc::String operation, std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, ast::Span source_span, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_unary(mlc::String operation, std::shared_ptr<ast::Expr> inner, ast::Span source_span, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_method(std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

infer_result::InferResult infer_expr_field(std::shared_ptr<ast::Expr> object, mlc::String field_name, ast::Span field_source_span, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_index(std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expression, ast::Span bracket_source_span, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_conditional(std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expression, std::shared_ptr<ast::Expr> else_expression, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_block(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_with(std::shared_ptr<ast::Expr> resource, mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> stmts, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_while_loop(std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_for_loop(mlc::String variable_name, std::shared_ptr<ast::Expr> iterator, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_record_update(mlc::String type_name, std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context) noexcept;

infer::Infer_tuple_literal_fold_state infer_tuple_literal_element_fold_step(infer::Infer_tuple_literal_fold_state state, std::shared_ptr<ast::Expr> tuple_element, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_tuple_literal(mlc::Array<std::shared_ptr<ast::Expr>> elements, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_array_literal(mlc::Array<std::shared_ptr<ast::Expr>> elements, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_question(std::shared_ptr<ast::Expr> inner, ast::Span question_span, check_context::CheckContext inference_context) noexcept;

std::shared_ptr<registry::Type> inference_placeholder_unknown_type() noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> lambda_inference_environment_assign_unknown_placeholder(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment_for_parameters, mlc::String parameter_binding_name) noexcept;

infer_result::InferResult infer_expr_lambda(mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr(std::shared_ptr<ast::Expr> expression, check_context::CheckContext inference_context) noexcept;

infer::Record_literal_spread_inference_fold_state accumulate_record_literal_spread_inference_for_literal_part(infer::Record_literal_spread_inference_fold_state fold_state, ast::RecordLitPart literal_part_under_inference, check_context::CheckContext inference_context) noexcept;

infer::Record_literal_spread_inference_fold_state infer_record_literal_fold_spread_inference_parts(infer_result::InferResult inference_before_spread_merge, mlc::Array<ast::RecordLitPart> literal_parts_under_inference, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_record(mlc::String type_name, mlc::Array<ast::RecordLitPart> lit_parts, check_context::CheckContext inference_context, ast::Span span) noexcept;

infer::Statement_sequence_inference_fold_state infer_single_statement_augment_fold_state(infer::Statement_sequence_inference_fold_state accumulator, std::shared_ptr<ast::Stmt> statement_under_inference, registry::TypeRegistry registry) noexcept;

infer_result::StmtInferResult infer_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_arguments_errors(infer_result::InferResult initial, mlc::Array<std::shared_ptr<ast::Expr>> expressions, check_context::CheckContext inference_context) noexcept{return expressions.fold(initial, [inference_context](infer_result::InferResult accumulated_inference, std::shared_ptr<ast::Expr> expression_under_inference)  { return infer_result::InferResult_absorb(accumulated_inference, infer_expr(expression_under_inference, inference_context)); });}

infer_result::InferResult infer_record_field_binding_value_inference_step(infer_result::InferResult accumulator, std::shared_ptr<ast::FieldVal> field_value, check_context::CheckContext inference_context, mlc::String record_type_name_for_fields) noexcept{
bool registry_has_ordered_fields_for_type = record_type_name_for_fields.length() > 0 && registry::TypeRegistry_has_fields(inference_context.registry, record_type_name_for_fields);
infer_result::InferResult next_result = registry_has_ordered_fields_for_type ? infer_result::InferResult{accumulator.inferred_type, ast::diagnostics_append(accumulator.errors, type_diagnostics::infer_expr_field_diagnostics(std::make_shared<registry::Type>(registry::TNamed(record_type_name_for_fields)), field_value->name, ast::expr_span(field_value->val), inference_context.registry))} : accumulator;
infer_result::InferResult value_inference = infer_expr(field_value->val, inference_context);
return registry_has_ordered_fields_for_type ? [&]() -> infer_result::InferResult { 
  std::shared_ptr<registry::Type> expected_field_type = registry::field_type_from_object(std::make_shared<registry::Type>(registry::TNamed(record_type_name_for_fields)), field_value->name, inference_context.registry);
  std::shared_ptr<registry::Type> inferred_value_type = value_inference.inferred_type;
  bool field_types_conflict = !semantic_type_structure::type_is_unknown(expected_field_type) && !semantic_type_structure::type_is_unknown(inferred_value_type) && !semantic_type_structure::types_structurally_equal(expected_field_type, inferred_value_type);
  mlc::Array<ast::Diagnostic> field_type_mismatch_diagnostics = field_types_conflict ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("field ") + field_value->name + mlc::String(": expected ") + semantic_type_structure::type_description(expected_field_type) + mlc::String(", got ") + semantic_type_structure::type_description(inferred_value_type), ast::expr_span(field_value->val))} : mlc::Array<ast::Diagnostic>{};
  return infer_result::InferResult{next_result.inferred_type, ast::diagnostics_append(ast::diagnostics_append(next_result.errors, value_inference.errors), field_type_mismatch_diagnostics)};
 }() : infer_result::InferResult_absorb(next_result, value_inference);
}

infer_result::InferResult infer_field_values_errors(infer_result::InferResult initial, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context, mlc::String record_type_name_for_fields) noexcept{return field_values.fold(initial, [inference_context, record_type_name_for_fields](infer_result::InferResult accumulator, std::shared_ptr<ast::FieldVal> field_value_under_inference)  { return infer_record_field_binding_value_inference_step(accumulator, field_value_under_inference, inference_context, record_type_name_for_fields); });}

infer_result::InferResult infer_explicit_record_literal_field_unknown_name_step(infer_result::InferResult accumulator, std::shared_ptr<ast::FieldVal> field_value_binding, mlc::String record_type_name, registry::TypeRegistry registry) noexcept{return infer_result::InferResult{accumulator.inferred_type, ast::diagnostics_append(accumulator.errors, type_diagnostics::infer_expr_field_diagnostics(std::make_shared<registry::Type>(registry::TNamed(record_type_name)), field_value_binding->name, ast::expr_span(field_value_binding->val), registry))};}

infer_result::InferResult infer_explicit_record_literal_field_name_errors(infer_result::InferResult initial, mlc::String record_type_name, mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_field_values_flat, registry::TypeRegistry registry) noexcept{return explicit_field_values_flat.fold(initial, [record_type_name, registry](infer_result::InferResult accumulator, std::shared_ptr<ast::FieldVal> field_value_binding)  { return infer_explicit_record_literal_field_unknown_name_step(accumulator, field_value_binding, record_type_name, registry); });}

infer_result::InferResult infer_expr_binary(mlc::String operation, std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, ast::Span source_span, check_context::CheckContext inference_context) noexcept{
infer_result::InferResult left_result = infer_expr(left, inference_context);
infer_result::InferResult right_result = infer_expr(right, inference_context);
mlc::String method = semantic_type_structure::operator_method_for(operation);
return method != mlc::String("") ? [&]() -> infer_result::InferResult { 
  std::shared_ptr<registry::Type> method_type = registry::method_return_type_from_object(left_result.inferred_type, method, inference_context.registry);
  return !semantic_type_structure::type_is_unknown(method_type) ? infer_result::InferResult{method_type, ast::diagnostics_append(left_result.errors, right_result.errors)} : infer_operand_combine::infer_binary_from_operand_results(operation, left_result, right_result, source_span);
 }() : infer_operand_combine::infer_binary_from_operand_results(operation, left_result, right_result, source_span);
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

infer_result::InferResult infer_expr_with(std::shared_ptr<ast::Expr> resource, mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> stmts, check_context::CheckContext inference_context) noexcept{
infer_result::InferResult res = infer_expr(resource, inference_context);
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> inner_env = inference_context.type_env;
inner_env.set(binder, res.inferred_type);
infer_result::StmtInferResult stmts_result = infer_statements(stmts, check_context::check_context_new(inner_env, inference_context.registry));
return infer_result::InferResult_with_type(infer_result::InferResult_absorb_stmt(res, stmts_result), std::make_shared<registry::Type>((registry::TUnit{})));
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

infer_result::InferResult infer_expr_record_update(mlc::String type_name, std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context) noexcept{
infer_result::InferResult base_result = infer_expr(base, inference_context);
return infer_field_values_errors(infer_result::InferResult_with_type(base_result, std::make_shared<registry::Type>(registry::TNamed(type_name))), field_values, inference_context, type_name);
}

infer::Infer_tuple_literal_fold_state infer_tuple_literal_element_fold_step(infer::Infer_tuple_literal_fold_state state, std::shared_ptr<ast::Expr> tuple_element, check_context::CheckContext inference_context) noexcept{
infer_result::InferResult element_inference = infer_expr(tuple_element, inference_context);
state.member_types.push_back(element_inference.inferred_type);
return infer::Infer_tuple_literal_fold_state{infer_result::InferResult_absorb(state.combined_inference, element_inference), state.member_types};
}

infer_result::InferResult infer_expr_tuple_literal(mlc::Array<std::shared_ptr<ast::Expr>> elements, check_context::CheckContext inference_context) noexcept{
return elements.size() < 2 ? infer_arguments_errors(infer_result::infer_ok(std::make_shared<registry::Type>((registry::TUnknown{}))), elements, inference_context) : [&]() -> infer_result::InferResult { 
  infer::Infer_tuple_literal_fold_state folded_tuple_inference = elements.fold(infer::Infer_tuple_literal_fold_state{infer_result::infer_ok(std::make_shared<registry::Type>((registry::TUnknown{}))), {}}, [inference_context](infer::Infer_tuple_literal_fold_state accumulator_state, std::shared_ptr<ast::Expr> tuple_element_under_inference)  { return infer_tuple_literal_element_fold_step(accumulator_state, tuple_element_under_inference, inference_context); });
  return infer_result::InferResult_with_type(folded_tuple_inference.combined_inference, std::make_shared<registry::Type>(registry::TTuple(folded_tuple_inference.member_types)));
 }();
}

infer_result::InferResult infer_expr_array_literal(mlc::Array<std::shared_ptr<ast::Expr>> elements, check_context::CheckContext inference_context) noexcept{
std::shared_ptr<registry::Type> first_element_type = elements.size() > 0 ? infer_expr(elements[0], inference_context).inferred_type : std::make_shared<registry::Type>((registry::TUnknown{}));
return infer_arguments_errors(infer_result::infer_ok(std::make_shared<registry::Type>(registry::TArray(first_element_type))), elements, inference_context);
}

infer_result::InferResult infer_expr_question(std::shared_ptr<ast::Expr> inner, ast::Span question_span, check_context::CheckContext inference_context) noexcept{return infer_question_expression::infer_question_from_inner_result(infer_expr(inner, inference_context), question_span);}

std::shared_ptr<registry::Type> inference_placeholder_unknown_type() noexcept{return std::make_shared<registry::Type>((registry::TUnknown{}));}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> lambda_inference_environment_assign_unknown_placeholder(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment_for_parameters, mlc::String parameter_binding_name) noexcept{
type_environment_for_parameters.set(parameter_binding_name, inference_placeholder_unknown_type());
return type_environment_for_parameters;
}

infer_result::InferResult infer_expr_lambda(mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body, check_context::CheckContext inference_context) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> lambda_type_environment = inference_context.type_env;
mlc::Array<std::shared_ptr<registry::Type>> inferred_unknown_parameter_types = parameter_names.map([](mlc::String unused_lambda_parameter_placeholder)  { return inference_placeholder_unknown_type(); });
parameter_names.fold(lambda_type_environment, [](mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> environment_map, mlc::String parameter_binding_name)  { return lambda_inference_environment_assign_unknown_placeholder(environment_map, parameter_binding_name); });
check_context::CheckContext lambda_inference_context = check_context::check_context_new(lambda_type_environment, inference_context.registry);
infer_result::InferResult body_inference = infer_expr(body, lambda_inference_context);
return infer_result::infer_ok(std::make_shared<registry::Type>(registry::TFn(inferred_unknown_parameter_types, body_inference.inferred_type)));
}

infer_result::InferResult infer_expr(std::shared_ptr<ast::Expr> expression, check_context::CheckContext inference_context) noexcept{
std::shared_ptr<ast::Expr> expression_partial_application = partial_application_desugar::partial_application_desugar_expr(expression);
return std::visit(overloaded{
  [&](const ExprInt& exprint) -> infer_result::InferResult { auto [_w0, _w1] = exprint; return infer_literals::infer_expr_integer_literal(); },
  [&](const ExprStr& exprstr) -> infer_result::InferResult { auto [_w0, _w1] = exprstr; return infer_literals::infer_expr_string_literal(); },
  [&](const ExprBool& exprbool) -> infer_result::InferResult { auto [_w0, _w1] = exprbool; return infer_literals::infer_expr_boolean_literal(); },
  [&](const ExprUnit& exprunit) -> infer_result::InferResult { auto [_w0] = exprunit; return infer_literals::infer_expr_unit_literal(); },
  [&](const ExprFloat& exprfloat) -> infer_result::InferResult { auto [_w0, _w1] = exprfloat; return infer_literals::infer_expr_f64_literal(); },
  [&](const ExprI64& expri64) -> infer_result::InferResult { auto [_w0, _w1] = expri64; return infer_literals::infer_expr_i64_literal(); },
  [&](const ExprU8& expru8) -> infer_result::InferResult { auto [_w0, _w1] = expru8; return infer_literals::infer_expr_u8_literal(); },
  [&](const ExprUsize& exprusize) -> infer_result::InferResult { auto [_w0, _w1] = exprusize; return infer_literals::infer_expr_usize_literal(); },
  [&](const ExprChar& exprchar) -> infer_result::InferResult { auto [_w0, _w1] = exprchar; return infer_literals::infer_expr_char_literal(); },
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
  [&](const ExprRecord& exprrecord) -> infer_result::InferResult { auto [name, lit_parts, span] = exprrecord; return infer_expr_record(name, lit_parts, inference_context, span); },
  [&](const ExprRecordUpdate& exprrecordupdate) -> infer_result::InferResult { auto [name, base, field_vals, _w0] = exprrecordupdate; return infer_expr_record_update(name, base, field_vals, inference_context); },
  [&](const ExprArray& exprarray) -> infer_result::InferResult { auto [elements, _w0] = exprarray; return infer_expr_array_literal(elements, inference_context); },
  [&](const ExprTuple& exprtuple) -> infer_result::InferResult { auto [elements, _w0] = exprtuple; return infer_expr_tuple_literal(elements, inference_context); },
  [&](const ExprQuestion& exprquestion) -> infer_result::InferResult { auto [inner, question_span] = exprquestion; return infer_expr_question(inner, question_span, inference_context); },
  [&](const ExprLambda& exprlambda) -> infer_result::InferResult { auto [params, body, _w0] = exprlambda; return infer_expr_lambda(params, body, inference_context); },
  [&](const ExprNamedArg& exprnamedarg) -> infer_result::InferResult { auto [_w0, inner, _w1] = exprnamedarg; return infer_expr(inner, inference_context); },
  [&](const ExprWith& exprwith) -> infer_result::InferResult { auto [resource, binder, stmts, _w0] = exprwith; return infer_expr_with(resource, binder, stmts, inference_context); }
}, (*expression_partial_application)._);
}

infer::Record_literal_spread_inference_fold_state accumulate_record_literal_spread_inference_for_literal_part(infer::Record_literal_spread_inference_fold_state fold_state, ast::RecordLitPart literal_part_under_inference, check_context::CheckContext inference_context) noexcept{return std::visit(overloaded{
  [&](const RecordLitSpread& recordlitspread) -> infer::Record_literal_spread_inference_fold_state { auto [spread_expression_under_inference] = recordlitspread; return [&]() -> infer::Record_literal_spread_inference_fold_state { 
  infer_result::InferResult spread_expression_inference = infer_expr(spread_expression_under_inference, inference_context);
  fold_state.spread_inferred_types_in_visit_order.push_back(spread_expression_inference.inferred_type);
  return infer::Record_literal_spread_inference_fold_state{infer_result::InferResult_absorb(fold_state.inference_result_so_far, spread_expression_inference), fold_state.spread_inferred_types_in_visit_order};
 }(); },
  [&](const RecordLitFields& recordlitfields) -> infer::Record_literal_spread_inference_fold_state { auto [_w0] = recordlitfields; return fold_state; }
}, literal_part_under_inference._);}

infer::Record_literal_spread_inference_fold_state infer_record_literal_fold_spread_inference_parts(infer_result::InferResult inference_before_spread_merge, mlc::Array<ast::RecordLitPart> literal_parts_under_inference, check_context::CheckContext inference_context) noexcept{return literal_parts_under_inference.fold(infer::Record_literal_spread_inference_fold_state{inference_before_spread_merge, {}}, [inference_context](infer::Record_literal_spread_inference_fold_state accumulator_fold_state, ast::RecordLitPart literal_part_under_inference)  { return accumulate_record_literal_spread_inference_for_literal_part(accumulator_fold_state, literal_part_under_inference, inference_context); });}

infer_result::InferResult infer_expr_record(mlc::String type_name, mlc::Array<ast::RecordLitPart> lit_parts, check_context::CheckContext inference_context, ast::Span span) noexcept{
mlc::Array<ast::Diagnostic> private_errors = registry::TypeRegistry_is_private_ctor(inference_context.registry, type_name) && inference_context.current_extend_type != type_name ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("private constructor: cannot construct ") + type_name + mlc::String(" outside its extend block"), span)} : mlc::Array<ast::Diagnostic>{};
infer_result::InferResult result = infer_result::InferResult{std::make_shared<registry::Type>(registry::TNamed(type_name)), private_errors};
mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_field_values_flat = record_lit_merge::collect_explicit_field_values_flat(lit_parts);
return !record_lit_merge::record_literal_contains_spread(lit_parts) ? [&]() -> infer_result::InferResult { 
  if (registry::TypeRegistry_has_fields(inference_context.registry, type_name)){
{
result = infer_explicit_record_literal_field_name_errors(result, type_name, explicit_field_values_flat, inference_context.registry);
}
}
  mlc::Array<std::shared_ptr<ast::FieldVal>> merged_with_optional_defaults = record_lit_merge::merge_explicit_record_literal_with_type_defaults(type_name, explicit_field_values_flat, inference_context.registry);
  result = infer_field_values_errors(result, merged_with_optional_defaults, inference_context, type_name);
  if (registry::TypeRegistry_has_fields(inference_context.registry, type_name)){
{
result = infer_result::InferResult{result.inferred_type, ast::diagnostics_append(result.errors, record_lit_merge::diagnostics_missing_fields_for_ordered_record(type_name, inference_context.registry, merged_with_optional_defaults, span))};
}
}
  return result;
 }() : !registry::TypeRegistry_has_fields(inference_context.registry, type_name) ? [&]() -> infer_result::InferResult { 
  result = infer_field_values_errors(result, explicit_field_values_flat, inference_context, mlc::String(""));
  return infer_result::InferResult{result.inferred_type, ast::diagnostics_append(result.errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("record literal spread requires a named record type with fields"), span)})};
 }() : [&]() -> infer_result::InferResult { 
  result = infer_field_values_errors(result, explicit_field_values_flat, inference_context, type_name);
  infer::Record_literal_spread_inference_fold_state spread_inference_after_literal_parts = infer_record_literal_fold_spread_inference_parts(result, lit_parts, inference_context);
  result = spread_inference_after_literal_parts.inference_result_so_far;
  mlc::Array<std::shared_ptr<registry::Type>> spread_types_in_order = spread_inference_after_literal_parts.spread_inferred_types_in_visit_order;
  mlc::Array<std::shared_ptr<ast::FieldVal>> merged_field_values = record_lit_merge::merge_record_literal_parts_to_field_values(type_name, lit_parts, inference_context.registry, spread_types_in_order);
  mlc::Array<std::shared_ptr<ast::FieldVal>> merged_including_decl_defaults = record_lit_merge::merge_explicit_record_literal_with_type_defaults(type_name, merged_field_values, inference_context.registry);
  result = infer_field_values_errors(result, merged_including_decl_defaults, inference_context, type_name);
  result = infer_result::InferResult{result.inferred_type, ast::diagnostics_append(result.errors, record_lit_merge::diagnostics_missing_fields_for_ordered_record(type_name, inference_context.registry, merged_including_decl_defaults, span))};
  return result;
 }();
}

infer::Statement_sequence_inference_fold_state infer_single_statement_augment_fold_state(infer::Statement_sequence_inference_fold_state accumulator, std::shared_ptr<ast::Stmt> statement_under_inference, registry::TypeRegistry registry) noexcept{return std::visit(overloaded{
  [&](const StmtLet& stmtlet) -> infer::Statement_sequence_inference_fold_state { auto [binding_name, _w0, _w1, value_expression, _w2] = stmtlet; return [&]() -> infer::Statement_sequence_inference_fold_state { 
  infer_result::InferResult value_inference = infer_expr(value_expression, check_context::check_context_new(accumulator.type_environment_under_construction, registry));
  accumulator.type_environment_under_construction.set(binding_name, value_inference.inferred_type);
  return infer::Statement_sequence_inference_fold_state{ast::diagnostics_append(accumulator.diagnostics_collected_so_far, value_inference.errors), accumulator.type_environment_under_construction};
 }(); },
  [&](const StmtLetPat& stmtletpat) -> infer::Statement_sequence_inference_fold_state { auto [pattern, _w0, _w1, value_expression, has_else, else_body, source_span] = stmtletpat; return [&]() -> infer::Statement_sequence_inference_fold_state { 
  infer_result::InferResult value_inference = infer_expr(value_expression, check_context::check_context_new(accumulator.type_environment_under_construction, registry));
  let_pattern_infer::infer_let_pattern_env(pattern, value_inference.inferred_type, accumulator.type_environment_under_construction, registry);
  mlc::Array<ast::Diagnostic> next_diagnostics = ast::diagnostics_append(accumulator.diagnostics_collected_so_far, value_inference.errors);
  bool pattern_is_refutable = [&]() { if (std::holds_alternative<ast::PatCtor>((*pattern))) { auto _v_patctor = std::get<ast::PatCtor>((*pattern)); auto [_w0, _w1, _w2] = _v_patctor; return true; } return false; }();
  if (pattern_is_refutable && !has_else){
{
next_diagnostics = ast::diagnostics_append(next_diagnostics, mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("refutable pattern in let requires `else` branch"), source_span)});
}
}
  if (has_else){
{
infer_result::InferResult else_branch_inference = infer_expr(else_body, check_context::check_context_new(accumulator.type_environment_under_construction, registry));
next_diagnostics = ast::diagnostics_append(next_diagnostics, else_branch_inference.errors);
}
}
  return infer::Statement_sequence_inference_fold_state{next_diagnostics, accumulator.type_environment_under_construction};
 }(); },
  [&](const StmtLetConst& stmtletconst) -> infer::Statement_sequence_inference_fold_state { auto [binding_name, _w0, value_expression, _w1] = stmtletconst; return [&]() -> infer::Statement_sequence_inference_fold_state { 
  infer_result::InferResult value_inference = infer_expr(value_expression, check_context::check_context_new(accumulator.type_environment_under_construction, registry));
  accumulator.type_environment_under_construction.set(binding_name, value_inference.inferred_type);
  return infer::Statement_sequence_inference_fold_state{ast::diagnostics_append(accumulator.diagnostics_collected_so_far, value_inference.errors), accumulator.type_environment_under_construction};
 }(); },
  [&](const StmtExpr& stmtexpr) -> infer::Statement_sequence_inference_fold_state { auto [expression, _w0] = stmtexpr; return [&]() -> infer::Statement_sequence_inference_fold_state { 
  infer_result::InferResult expression_inference = infer_expr(expression, check_context::check_context_new(accumulator.type_environment_under_construction, registry));
  return infer::Statement_sequence_inference_fold_state{ast::diagnostics_append(accumulator.diagnostics_collected_so_far, expression_inference.errors), accumulator.type_environment_under_construction};
 }(); },
  [&](const StmtReturn& stmtreturn) -> infer::Statement_sequence_inference_fold_state { auto [return_expression, _w0] = stmtreturn; return [&]() -> infer::Statement_sequence_inference_fold_state { 
  infer_result::InferResult return_inference = infer_expr(return_expression, check_context::check_context_new(accumulator.type_environment_under_construction, registry));
  return infer::Statement_sequence_inference_fold_state{ast::diagnostics_append(accumulator.diagnostics_collected_so_far, return_inference.errors), accumulator.type_environment_under_construction};
 }(); },
  [&](const StmtBreak& stmtbreak) -> infer::Statement_sequence_inference_fold_state { auto [_w0] = stmtbreak; return accumulator; },
  [&](const StmtContinue& stmtcontinue) -> infer::Statement_sequence_inference_fold_state { auto [_w0] = stmtcontinue; return accumulator; }
}, (*statement_under_inference)._);}

infer_result::StmtInferResult infer_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept{
infer::Statement_sequence_inference_fold_state inference_after_statement_sequence = statements.fold(infer::Statement_sequence_inference_fold_state{{}, inference_context.type_env}, [inference_context](infer::Statement_sequence_inference_fold_state accumulator_fold_state, std::shared_ptr<ast::Stmt> statement_under_inference_within_block)  { return infer_single_statement_augment_fold_state(accumulator_fold_state, statement_under_inference_within_block, inference_context.registry); });
return infer_result::StmtInferResult{inference_after_statement_sequence.type_environment_under_construction, inference_after_statement_sequence.diagnostics_collected_so_far};
}

} // namespace infer
