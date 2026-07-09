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
#include "infer_channel_method.hpp"
#include "infer_arc_method.hpp"
#include "infer_weak_method.hpp"
#include "infer_mutex_method.hpp"
#include "mutex_method_types.hpp"
#include "semantic_type_structure.hpp"
#include "channel_method_types.hpp"
#include "infer_result_option_method.hpp"
#include "hof_method_spec.hpp"
#include "record_lit_merge.hpp"
#include "partial_application_desugar.hpp"
#include "diagnostic_codes.hpp"

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
using namespace infer_channel_method;
using namespace infer_arc_method;
using namespace infer_weak_method;
using namespace infer_mutex_method;
using namespace mutex_method_types;
using namespace semantic_type_structure;
using namespace channel_method_types;
using namespace infer_result_option_method;
using namespace hof_method_spec;
using namespace record_lit_merge;
using namespace partial_application_desugar;
using namespace diagnostic_codes;
using namespace ast_tokens;

struct Infer_tuple_literal_fold_state {infer_result::InferResult combined_inference;mlc::Array<std::shared_ptr<registry::Type>> member_types;};

struct InferPass {check_context::CheckContext inference_context;int seed;};

struct Record_literal_spread_inference_fold_state {infer_result::InferResult inference_result_so_far;mlc::Array<std::shared_ptr<registry::Type>> spread_inferred_types_in_visit_order;};

struct Statement_sequence_inference_fold_state {mlc::Array<ast::Diagnostic> diagnostics_collected_so_far;mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment_under_construction;};

infer_result::InferResult infer_arguments_errors(infer_result::InferResult initial, mlc::Array<std::shared_ptr<ast::Expr>> expressions, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_record_field_binding_value_inference_step(infer_result::InferResult accumulator, std::shared_ptr<ast::FieldVal> field_value, check_context::CheckContext inference_context, mlc::String record_type_name_for_fields) noexcept;

infer_result::InferResult infer_field_values_errors(infer_result::InferResult initial, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context, mlc::String record_type_name_for_fields) noexcept;

infer_result::InferResult infer_explicit_record_literal_field_unknown_name_step(infer_result::InferResult accumulator, std::shared_ptr<ast::FieldVal> field_value_binding, mlc::String record_type_name, registry::TypeRegistry registry) noexcept;

infer_result::InferResult infer_explicit_record_literal_field_name_errors(infer_result::InferResult initial, mlc::String record_type_name, mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_field_values_flat, registry::TypeRegistry registry) noexcept;

infer_result::InferResult infer_expr_binary(mlc::String operation, std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, ast::Span source_span, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_unary(mlc::String operation, std::shared_ptr<ast::Expr> inner_expression, ast::Span source_span, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_method(std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

infer_result::InferResult infer_expr_field(std::shared_ptr<ast::Expr> object, mlc::String field_name, ast::Span field_source_span, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_index(std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expression, ast::Span bracket_source_span, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_conditional(std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expression, std::shared_ptr<ast::Expr> else_expression, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_block(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_with(std::shared_ptr<ast::Expr> resource, mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_while_loop(std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_spawn(mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_for_loop(mlc::String variable_name, std::shared_ptr<ast::Expr> iterator, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_record_update(mlc::String type_name, std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context) noexcept;

infer::Infer_tuple_literal_fold_state infer_tuple_literal_element_fold_step(infer::Infer_tuple_literal_fold_state state, std::shared_ptr<ast::Expr> tuple_element, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_tuple_literal(mlc::Array<std::shared_ptr<ast::Expr>> elements, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_array_literal(mlc::Array<std::shared_ptr<ast::Expr>> elements, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_question(std::shared_ptr<ast::Expr> inner_expression, ast::Span question_span, check_context::CheckContext inference_context) noexcept;

std::shared_ptr<registry::Type> inference_placeholder_unknown_type() noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> lambda_inference_environment_assign_unknown_placeholder(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment_for_parameters, mlc::String parameter_binding_name) noexcept;

infer_result::InferResult infer_expr_lambda(mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body, check_context::CheckContext inference_context) noexcept;

infer::InferPass infer_pass_new(check_context::CheckContext inference_context) noexcept;

infer_result::InferResult dispatch_infer_pass(infer::InferPass infer_pass, std::shared_ptr<ast::Expr> expression) noexcept;

infer_result::InferResult InferPass_visit_int(infer::InferPass self) noexcept;

infer_result::InferResult InferPass_visit_str(infer::InferPass self) noexcept;

infer_result::InferResult InferPass_visit_bool(infer::InferPass self) noexcept;

infer_result::InferResult InferPass_visit_unit(infer::InferPass self) noexcept;

infer_result::InferResult InferPass_visit_float(infer::InferPass self) noexcept;

infer_result::InferResult InferPass_visit_i64(infer::InferPass self) noexcept;

infer_result::InferResult InferPass_visit_u8(infer::InferPass self) noexcept;

infer_result::InferResult InferPass_visit_usize(infer::InferPass self) noexcept;

infer_result::InferResult InferPass_visit_char(infer::InferPass self) noexcept;

infer_result::InferResult InferPass_visit_extern(infer::InferPass self) noexcept;

infer_result::InferResult InferPass_visit_ident(infer::InferPass self, mlc::String name) noexcept;

infer_result::InferResult InferPass_visit_bin(infer::InferPass self, mlc::String operation, std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, ast::Span source_span) noexcept;

infer_result::InferResult InferPass_visit_un(infer::InferPass self, mlc::String operation, std::shared_ptr<ast::Expr> inner_expression, ast::Span source_span) noexcept;

infer_result::InferResult InferPass_visit_call(infer::InferPass self, std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span) noexcept;

infer_result::InferResult InferPass_visit_method(infer::InferPass self, std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span) noexcept;

infer_result::InferResult InferPass_visit_field(infer::InferPass self, std::shared_ptr<ast::Expr> object, mlc::String field_name, ast::Span field_source_span) noexcept;

infer_result::InferResult InferPass_visit_index(infer::InferPass self, std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expression, ast::Span bracket_source_span) noexcept;

infer_result::InferResult InferPass_visit_if(infer::InferPass self, std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expression, std::shared_ptr<ast::Expr> else_expression) noexcept;

infer_result::InferResult InferPass_visit_block(infer::InferPass self, mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression) noexcept;

infer_result::InferResult InferPass_visit_while(infer::InferPass self, std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements) noexcept;

infer_result::InferResult InferPass_visit_for(infer::InferPass self, mlc::String variable_name, std::shared_ptr<ast::Expr> iterator, mlc::Array<std::shared_ptr<ast::Stmt>> statements) noexcept;

infer_result::InferResult InferPass_visit_match(infer::InferPass self, std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms) noexcept;

infer_result::InferResult InferPass_visit_record(infer::InferPass self, mlc::String type_name, mlc::Array<ast::RecordLitPart> lit_parts, ast::Span span) noexcept;

infer_result::InferResult InferPass_visit_record_update(infer::InferPass self, mlc::String type_name, std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values) noexcept;

infer_result::InferResult InferPass_visit_array(infer::InferPass self, mlc::Array<std::shared_ptr<ast::Expr>> elements) noexcept;

infer_result::InferResult InferPass_visit_tuple(infer::InferPass self, mlc::Array<std::shared_ptr<ast::Expr>> elements) noexcept;

infer_result::InferResult InferPass_visit_question(infer::InferPass self, std::shared_ptr<ast::Expr> inner_expression, ast::Span question_span) noexcept;

infer_result::InferResult InferPass_visit_lambda(infer::InferPass self, mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body) noexcept;

infer_result::InferResult InferPass_visit_named_arg(infer::InferPass self, std::shared_ptr<ast::Expr> inner_expression) noexcept;

infer_result::InferResult InferPass_visit_with(infer::InferPass self, std::shared_ptr<ast::Expr> resource, mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> statements) noexcept;

infer_result::InferResult InferPass_visit_spawn(infer::InferPass self, mlc::Array<std::shared_ptr<ast::Stmt>> statements) noexcept;

infer_result::InferResult InferPass_visit_unsupported(infer::InferPass self) noexcept;

infer_result::InferResult infer_expr(std::shared_ptr<ast::Expr> expression, check_context::CheckContext inference_context) noexcept;

infer::Record_literal_spread_inference_fold_state accumulate_record_literal_spread_inference_for_literal_part(infer::Record_literal_spread_inference_fold_state fold_state, ast::RecordLitPart literal_part_under_inference, check_context::CheckContext inference_context) noexcept;

infer::Record_literal_spread_inference_fold_state infer_record_literal_fold_spread_inference_parts(infer_result::InferResult inference_before_spread_merge, mlc::Array<ast::RecordLitPart> literal_parts_under_inference, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_record(mlc::String type_name, mlc::Array<ast::RecordLitPart> lit_parts, check_context::CheckContext inference_context, ast::Span span) noexcept;

infer::Statement_sequence_inference_fold_state infer_single_statement_augment_fold_state(infer::Statement_sequence_inference_fold_state accumulator, std::shared_ptr<ast::Stmt> statement_under_inference, registry::TypeRegistry registry) noexcept;

infer_result::StmtInferResult infer_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_arguments_errors(infer_result::InferResult initial, mlc::Array<std::shared_ptr<ast::Expr>> expressions, check_context::CheckContext inference_context) noexcept{return expressions.fold(initial, [inference_context](infer_result::InferResult accumulated_inference, std::shared_ptr<ast::Expr> expression_under_inference) mutable { return infer_result::InferResult_absorb(accumulated_inference, infer_expr(expression_under_inference, inference_context)); });}

infer_result::InferResult infer_record_field_binding_value_inference_step(infer_result::InferResult accumulator, std::shared_ptr<ast::FieldVal> field_value, check_context::CheckContext inference_context, mlc::String record_type_name_for_fields) noexcept{
bool registry_has_ordered_fields_for_type = record_type_name_for_fields.length() > 0 && registry::TypeRegistry_has_fields(inference_context.registry, record_type_name_for_fields);
infer_result::InferResult next_parsed = registry_has_ordered_fields_for_type ? infer_result::InferResult{accumulator.inferred_type, ast::diagnostics_append(accumulator.errors, type_diagnostics::infer_expr_field_diagnostics(std::make_shared<registry::Type>(registry::TNamed(record_type_name_for_fields)), field_value->name, ast::expr_span(field_value->value), inference_context.registry))} : accumulator;
infer_result::InferResult value_inference = infer_expr(field_value->value, inference_context);
return registry_has_ordered_fields_for_type ? [&]() -> infer_result::InferResult { 
  std::shared_ptr<registry::Type> expected_field_type = registry::field_type_from_object(std::make_shared<registry::Type>(registry::TNamed(record_type_name_for_fields)), field_value->name, inference_context.registry);
  std::shared_ptr<registry::Type> inferred_value_type = value_inference.inferred_type;
  bool field_types_conflict = !semantic_type_structure::type_is_unknown(expected_field_type) && !semantic_type_structure::type_is_unknown(inferred_value_type) && !semantic_type_structure::types_structurally_equal(expected_field_type, inferred_value_type);
  mlc::Array<ast::Diagnostic> field_type_mismatch_diagnostics = field_types_conflict ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("field ") + field_value->name + mlc::String(": expected ") + semantic_type_structure::type_description(expected_field_type) + mlc::String(", got ") + semantic_type_structure::type_description(inferred_value_type), ast::expr_span(field_value->value), diagnostic_codes::diagnostic_code_e075())} : mlc::Array<ast::Diagnostic>{};
  return infer_result::InferResult{next_parsed.inferred_type, ast::diagnostics_append(ast::diagnostics_append(next_parsed.errors, value_inference.errors), field_type_mismatch_diagnostics)};
 }() : infer_result::InferResult_absorb(next_parsed, value_inference);
}

infer_result::InferResult infer_field_values_errors(infer_result::InferResult initial, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context, mlc::String record_type_name_for_fields) noexcept{return field_values.fold(initial, [inference_context, record_type_name_for_fields](infer_result::InferResult accumulator, std::shared_ptr<ast::FieldVal> field_value_under_inference) mutable { return infer_record_field_binding_value_inference_step(accumulator, field_value_under_inference, inference_context, record_type_name_for_fields); });}

infer_result::InferResult infer_explicit_record_literal_field_unknown_name_step(infer_result::InferResult accumulator, std::shared_ptr<ast::FieldVal> field_value_binding, mlc::String record_type_name, registry::TypeRegistry registry) noexcept{return infer_result::InferResult{accumulator.inferred_type, ast::diagnostics_append(accumulator.errors, type_diagnostics::infer_expr_field_diagnostics(std::make_shared<registry::Type>(registry::TNamed(record_type_name)), field_value_binding->name, ast::expr_span(field_value_binding->value), registry))};}

infer_result::InferResult infer_explicit_record_literal_field_name_errors(infer_result::InferResult initial, mlc::String record_type_name, mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_field_values_flat, registry::TypeRegistry registry) noexcept{return explicit_field_values_flat.fold(initial, [record_type_name, registry](infer_result::InferResult accumulator, std::shared_ptr<ast::FieldVal> field_value_binding) mutable { return infer_explicit_record_literal_field_unknown_name_step(accumulator, field_value_binding, record_type_name, registry); });}

infer_result::InferResult infer_expr_binary(mlc::String operation, std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, ast::Span source_span, check_context::CheckContext inference_context) noexcept{
infer_result::InferResult left_parsed = infer_expr(left, inference_context);
infer_result::InferResult right_parsed = infer_expr(right, inference_context);
mlc::String method = semantic_type_structure::operator_method_for(operation);
return method != mlc::String("") ? [&]() -> infer_result::InferResult { 
  std::shared_ptr<registry::Type> method_type = registry::method_return_type_from_object(left_parsed.inferred_type, method, inference_context.registry);
  return !semantic_type_structure::type_is_unknown(method_type) ? infer_result::InferResult{method_type, ast::diagnostics_append(left_parsed.errors, right_parsed.errors)} : infer_operand_combine::infer_binary_from_operand_results(operation, left_parsed, right_parsed, source_span);
 }() : infer_operand_combine::infer_binary_from_operand_results(operation, left_parsed, right_parsed, source_span);
}

infer_result::InferResult infer_expr_unary(mlc::String operation, std::shared_ptr<ast::Expr> inner_expression, ast::Span source_span, check_context::CheckContext inference_context) noexcept{return infer_operand_combine::infer_unary_from_inner_result(operation, infer_expr(inner_expression, inference_context), source_span);}

infer_result::InferResult infer_expr_method(std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept{
infer_result::InferResult object_parsed = infer_expr_fn(object, inference_context);
return infer_arc_method::is_arc_static_new(object, method_name) ? infer_arc_method::infer_arc_static_new_call(object_parsed, method_arguments, method_span, inference_context, infer_expr_fn) : infer_weak_method::is_shared_static_new(object, method_name) ? infer_weak_method::infer_shared_static_new_call(object_parsed, method_arguments, method_span, inference_context, infer_expr_fn) : infer_mutex_method::is_mutex_static_new(object, method_name) ? infer_mutex_method::infer_mutex_static_new_call(object_parsed, method_arguments, method_span, inference_context, infer_expr_fn) : infer_weak_method::is_shared_weak_method_on_receiver(object_parsed.inferred_type, method_name) ? infer_weak_method::infer_shared_weak_method_call(object_parsed, method_name, method_arguments, method_span, inference_context, infer_expr_fn) : infer_weak_method::is_weak_upgrade_method_on_receiver(object_parsed.inferred_type, method_name) ? infer_weak_method::infer_weak_upgrade_method_call(object_parsed, method_name, method_arguments, method_span, inference_context, infer_expr_fn) : hof_method_spec::is_result_option_hof_method(object_parsed.inferred_type, method_name) ? infer_result_option_method::infer_result_option_hof_method_call(object_parsed, method_name, method_arguments, method_span, inference_context, infer_expr_fn) : channel_method_types::is_channel_method(method_name) ? infer_channel_method::infer_channel_method_call(object_parsed, method_name, method_arguments, method_span, inference_context, infer_expr_fn) : mutex_method_types::is_mutex_method(method_name) && semantic_type_structure::type_is_mutex(object_parsed.inferred_type) ? infer_mutex_method::infer_mutex_method_call(object_parsed, method_name, method_arguments, method_span, inference_context, infer_expr_fn) : hof_method_spec::is_array_hof_method_on_receiver(object_parsed.inferred_type, method_name) ? infer_array_method::infer_array_hof_method_call(object_parsed, method_name, method_arguments, method_span, inference_context, infer_expr_fn) : [&]() -> infer_result::InferResult { 
  infer_result::InferResult with_arguments = infer_arguments_errors(object_parsed, method_arguments, inference_context);
  return infer_operand_combine::infer_method_from_object_and_arguments(object_parsed, with_arguments, method_name, method_span, method_arguments.size(), inference_context.registry);
 }();
}

infer_result::InferResult infer_expr_field(std::shared_ptr<ast::Expr> object, mlc::String field_name, ast::Span field_source_span, check_context::CheckContext inference_context) noexcept{return infer_operand_combine::infer_field_from_object_result(infer_expr(object, inference_context), field_name, field_source_span, inference_context.registry);}

infer_result::InferResult infer_expr_index(std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expression, ast::Span bracket_source_span, check_context::CheckContext inference_context) noexcept{return infer_operand_combine::infer_index_from_operand_results(infer_expr(object, inference_context), infer_expr(index_expression, inference_context), bracket_source_span);}

infer_result::InferResult infer_expr_conditional(std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expression, std::shared_ptr<ast::Expr> else_expression, check_context::CheckContext inference_context) noexcept{return infer_operand_combine::infer_conditional_from_branch_results(infer_expr(condition, inference_context), infer_expr(then_expression, inference_context), infer_expr(else_expression, inference_context), else_expression);}

infer_result::InferResult infer_expr_block(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression, check_context::CheckContext inference_context) noexcept{
infer_result::StmtInferResult statements_parsed = infer_statements(statements, inference_context);
infer_result::InferResult result_parsed = infer_expr(result_expression, check_context::check_context_new(statements_parsed.type_env, inference_context.registry));
return infer_result::InferResult_absorb_stmt(result_parsed, statements_parsed);
}

infer_result::InferResult infer_expr_with(std::shared_ptr<ast::Expr> resource, mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept{
infer_result::InferResult resource_parsed = infer_expr(resource, inference_context);
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> with_body_environment = inference_context.type_env;
with_body_environment.set(binder, resource_parsed.inferred_type);
infer_result::StmtInferResult statements_parsed = infer_statements(statements, check_context::check_context_new(with_body_environment, inference_context.registry));
return infer_result::InferResult_with_type(infer_result::InferResult_absorb_stmt(resource_parsed, statements_parsed), std::make_shared<registry::Type>((registry::TUnit{})));
}

infer_result::InferResult infer_expr_while_loop(std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept{
infer_result::InferResult condition_parsed = infer_expr(condition, inference_context);
infer_result::StmtInferResult statements_parsed = infer_statements(statements, inference_context);
return infer_result::InferResult_with_type(infer_result::InferResult_absorb_stmt(condition_parsed, statements_parsed), std::make_shared<registry::Type>((registry::TUnit{})));
}

infer_result::InferResult infer_expr_spawn(mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept{
infer_result::StmtInferResult statements_parsed = infer_statements(statements, inference_context);
check_context::CheckContext body_context = check_context::check_context_new(statements_parsed.type_env, inference_context.registry);
infer_result::InferResult result_parsed = infer_expr(ast::expr_spawn_body_result(statements), body_context);
return infer_result::InferResult_with_type(infer_result::InferResult_absorb_stmt(result_parsed, statements_parsed), std::make_shared<registry::Type>(registry::TGeneric(mlc::String("Task"), mlc::Array<std::shared_ptr<registry::Type>>{result_parsed.inferred_type})));
}

infer_result::InferResult infer_expr_for_loop(mlc::String variable_name, std::shared_ptr<ast::Expr> iterator, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept{
infer_result::InferResult iterator_base_parsed = infer_expr(iterator, inference_context);
infer_result::InferResult iterator_parsed = infer_for_support::infer_for_iterator_with_range_rules(iterator_base_parsed, iterator);
std::shared_ptr<registry::Type> element_type = infer_for_support::element_type_for_for_iterator(iterator_parsed.inferred_type);
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> inner_environment = inference_context.type_env;
inner_environment.set(variable_name, element_type);
check_context::CheckContext loop_context = check_context::check_context_new(inner_environment, inference_context.registry);
infer_result::StmtInferResult statements_parsed = infer_statements(statements, loop_context);
return infer_result::InferResult_with_type(infer_result::InferResult_absorb_stmt(iterator_parsed, statements_parsed), std::make_shared<registry::Type>((registry::TUnit{})));
}

infer_result::InferResult infer_expr_record_update(mlc::String type_name, std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context) noexcept{
infer_result::InferResult base_parsed = infer_expr(base, inference_context);
return infer_field_values_errors(infer_result::InferResult_with_type(base_parsed, std::make_shared<registry::Type>(registry::TNamed(type_name))), field_values, inference_context, type_name);
}

infer::Infer_tuple_literal_fold_state infer_tuple_literal_element_fold_step(infer::Infer_tuple_literal_fold_state state, std::shared_ptr<ast::Expr> tuple_element, check_context::CheckContext inference_context) noexcept{
infer_result::InferResult element_inference = infer_expr(tuple_element, inference_context);
return infer::Infer_tuple_literal_fold_state{infer_result::InferResult_absorb(state.combined_inference, element_inference), state.member_types.concat(mlc::Array<std::shared_ptr<registry::Type>>{element_inference.inferred_type})};
}

infer_result::InferResult infer_expr_tuple_literal(mlc::Array<std::shared_ptr<ast::Expr>> elements, check_context::CheckContext inference_context) noexcept{
return elements.size() < 2 ? infer_arguments_errors(infer_result::infer_ok(std::make_shared<registry::Type>((registry::TUnknown{}))), elements, inference_context) : [&]() -> infer_result::InferResult { 
  infer::Infer_tuple_literal_fold_state folded_tuple_inference = elements.fold(infer::Infer_tuple_literal_fold_state{infer_result::infer_ok(std::make_shared<registry::Type>((registry::TUnknown{}))), {}}, [inference_context](infer::Infer_tuple_literal_fold_state accumulator_state, std::shared_ptr<ast::Expr> tuple_element_under_inference) mutable { return infer_tuple_literal_element_fold_step(accumulator_state, tuple_element_under_inference, inference_context); });
  return infer_result::InferResult_with_type(folded_tuple_inference.combined_inference, std::make_shared<registry::Type>(registry::TTuple(folded_tuple_inference.member_types)));
 }();
}

infer_result::InferResult infer_expr_array_literal(mlc::Array<std::shared_ptr<ast::Expr>> elements, check_context::CheckContext inference_context) noexcept{
std::shared_ptr<registry::Type> first_element_type = elements.size() > 0 ? infer_expr(elements[0], inference_context).inferred_type : std::make_shared<registry::Type>((registry::TUnknown{}));
return infer_arguments_errors(infer_result::infer_ok(std::make_shared<registry::Type>(registry::TArray(first_element_type))), elements, inference_context);
}

infer_result::InferResult infer_expr_question(std::shared_ptr<ast::Expr> inner_expression, ast::Span question_span, check_context::CheckContext inference_context) noexcept{return infer_question_expression::infer_question_from_inner_result(infer_expr(inner_expression, inference_context), question_span);}

std::shared_ptr<registry::Type> inference_placeholder_unknown_type() noexcept{return std::make_shared<registry::Type>((registry::TUnknown{}));}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> lambda_inference_environment_assign_unknown_placeholder(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment_for_parameters, mlc::String parameter_binding_name) noexcept{
type_environment_for_parameters.set(parameter_binding_name, inference_placeholder_unknown_type());
return type_environment_for_parameters;
}

infer_result::InferResult infer_expr_lambda(mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body, check_context::CheckContext inference_context) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> lambda_type_environment = inference_context.type_env;
mlc::Array<std::shared_ptr<registry::Type>> inferred_unknown_parameter_types = parameter_names.map([](mlc::String unused_lambda_parameter_placeholder) mutable { return inference_placeholder_unknown_type(); });
parameter_names.fold(lambda_type_environment, [](mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> environment_map, mlc::String parameter_binding_name) mutable { return lambda_inference_environment_assign_unknown_placeholder(environment_map, parameter_binding_name); });
check_context::CheckContext lambda_inference_context = check_context::check_context_new(lambda_type_environment, inference_context.registry);
infer_result::InferResult body_inference = infer_expr(body, lambda_inference_context);
return infer_result::infer_ok(std::make_shared<registry::Type>(registry::TFn(inferred_unknown_parameter_types, body_inference.inferred_type)));
}

infer::InferPass infer_pass_new(check_context::CheckContext inference_context) noexcept{return infer::InferPass{inference_context, 0};}

infer_result::InferResult dispatch_infer_pass(infer::InferPass infer_pass, std::shared_ptr<ast::Expr> expression) noexcept{return [&]() -> infer_result::InferResult { if (std::holds_alternative<ast::ExprInt>((*expression)._)) { auto _v_exprint = std::get<ast::ExprInt>((*expression)._); auto [_w0, _w1] = _v_exprint; return InferPass_visit_int(infer_pass); } if (std::holds_alternative<ast::ExprStr>((*expression)._)) { auto _v_exprstr = std::get<ast::ExprStr>((*expression)._); auto [_w0, _w1] = _v_exprstr; return InferPass_visit_str(infer_pass); } if (std::holds_alternative<ast::ExprBool>((*expression)._)) { auto _v_exprbool = std::get<ast::ExprBool>((*expression)._); auto [_w0, _w1] = _v_exprbool; return InferPass_visit_bool(infer_pass); } if (std::holds_alternative<ast::ExprUnit>((*expression)._)) { auto _v_exprunit = std::get<ast::ExprUnit>((*expression)._); auto [_w0] = _v_exprunit; return InferPass_visit_unit(infer_pass); } if (std::holds_alternative<ast::ExprFloat>((*expression)._)) { auto _v_exprfloat = std::get<ast::ExprFloat>((*expression)._); auto [_w0, _w1] = _v_exprfloat; return InferPass_visit_float(infer_pass); } if (std::holds_alternative<ast::ExprI64>((*expression)._)) { auto _v_expri64 = std::get<ast::ExprI64>((*expression)._); auto [_w0, _w1] = _v_expri64; return InferPass_visit_i64(infer_pass); } if (std::holds_alternative<ast::ExprU8>((*expression)._)) { auto _v_expru8 = std::get<ast::ExprU8>((*expression)._); auto [_w0, _w1] = _v_expru8; return InferPass_visit_u8(infer_pass); } if (std::holds_alternative<ast::ExprUsize>((*expression)._)) { auto _v_exprusize = std::get<ast::ExprUsize>((*expression)._); auto [_w0, _w1] = _v_exprusize; return InferPass_visit_usize(infer_pass); } if (std::holds_alternative<ast::ExprChar>((*expression)._)) { auto _v_exprchar = std::get<ast::ExprChar>((*expression)._); auto [_w0, _w1] = _v_exprchar; return InferPass_visit_char(infer_pass); } if (std::holds_alternative<ast::ExprExtern>((*expression)._)) { auto _v_exprextern = std::get<ast::ExprExtern>((*expression)._); auto [_w0] = _v_exprextern; return InferPass_visit_extern(infer_pass); } if (std::holds_alternative<ast::ExprIdent>((*expression)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*expression)._); auto [name, _w0] = _v_exprident; return InferPass_visit_ident(infer_pass, name); } if (std::holds_alternative<ast::ExprBin>((*expression)._)) { auto _v_exprbin = std::get<ast::ExprBin>((*expression)._); auto [operation, left, right, span] = _v_exprbin; return InferPass_visit_bin(infer_pass, operation, left, right, span); } if (std::holds_alternative<ast::ExprUn>((*expression)._)) { auto _v_exprun = std::get<ast::ExprUn>((*expression)._); auto [operation, inner_expression, span] = _v_exprun; return InferPass_visit_un(infer_pass, operation, inner_expression, span); } if (std::holds_alternative<ast::ExprCall>((*expression)._)) { auto _v_exprcall = std::get<ast::ExprCall>((*expression)._); auto [function, call_arguments, call_source_span] = _v_exprcall; return InferPass_visit_call(infer_pass, function, call_arguments, call_source_span); } if (std::holds_alternative<ast::ExprMethod>((*expression)._)) { auto _v_exprmethod = std::get<ast::ExprMethod>((*expression)._); auto [object, method_name, method_arguments, method_span] = _v_exprmethod; return InferPass_visit_method(infer_pass, object, method_name, method_arguments, method_span); } if (std::holds_alternative<ast::ExprField>((*expression)._)) { auto _v_exprfield = std::get<ast::ExprField>((*expression)._); auto [object, field_name, field_source_span] = _v_exprfield; return InferPass_visit_field(infer_pass, object, field_name, field_source_span); } if (std::holds_alternative<ast::ExprIndex>((*expression)._)) { auto _v_exprindex = std::get<ast::ExprIndex>((*expression)._); auto [object, index_expression, bracket_source_span] = _v_exprindex; return InferPass_visit_index(infer_pass, object, index_expression, bracket_source_span); } if (std::holds_alternative<ast::ExprIf>((*expression)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expression)._); auto [condition, then_expression, else_expression, _w0] = _v_exprif; return InferPass_visit_if(infer_pass, condition, then_expression, else_expression); } if (std::holds_alternative<ast::ExprBlock>((*expression)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expression)._); auto [statements, result_expression, _w0] = _v_exprblock; return InferPass_visit_block(infer_pass, statements, result_expression); } if (std::holds_alternative<ast::ExprWhile>((*expression)._)) { auto _v_exprwhile = std::get<ast::ExprWhile>((*expression)._); auto [condition, statements, _w0] = _v_exprwhile; return InferPass_visit_while(infer_pass, condition, statements); } if (std::holds_alternative<ast::ExprFor>((*expression)._)) { auto _v_exprfor = std::get<ast::ExprFor>((*expression)._); auto [variable_name, iterator, statements, _w0] = _v_exprfor; return InferPass_visit_for(infer_pass, variable_name, iterator, statements); } if (std::holds_alternative<ast::ExprMatch>((*expression)._)) { auto _v_exprmatch = std::get<ast::ExprMatch>((*expression)._); auto [subject, match_arms, _w0] = _v_exprmatch; return InferPass_visit_match(infer_pass, subject, match_arms); } if (std::holds_alternative<ast::ExprRecord>((*expression)._)) { auto _v_exprrecord = std::get<ast::ExprRecord>((*expression)._); auto [name, lit_parts, span] = _v_exprrecord; return InferPass_visit_record(infer_pass, name, lit_parts, span); } if (std::holds_alternative<ast::ExprRecordUpdate>((*expression)._)) { auto _v_exprrecordupdate = std::get<ast::ExprRecordUpdate>((*expression)._); auto [name, base, field_values, _w0] = _v_exprrecordupdate; return InferPass_visit_record_update(infer_pass, name, base, field_values); } if (std::holds_alternative<ast::ExprArray>((*expression)._)) { auto _v_exprarray = std::get<ast::ExprArray>((*expression)._); auto [elements, _w0] = _v_exprarray; return InferPass_visit_array(infer_pass, elements); } if (std::holds_alternative<ast::ExprTuple>((*expression)._)) { auto _v_exprtuple = std::get<ast::ExprTuple>((*expression)._); auto [elements, _w0] = _v_exprtuple; return InferPass_visit_tuple(infer_pass, elements); } if (std::holds_alternative<ast::ExprQuestion>((*expression)._)) { auto _v_exprquestion = std::get<ast::ExprQuestion>((*expression)._); auto [inner_expression, question_span] = _v_exprquestion; return InferPass_visit_question(infer_pass, inner_expression, question_span); } if (std::holds_alternative<ast::ExprLambda>((*expression)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*expression)._); auto [parameter_names, body, _w0] = _v_exprlambda; return InferPass_visit_lambda(infer_pass, parameter_names, body); } if (std::holds_alternative<ast::ExprNamedArg>((*expression)._)) { auto _v_exprnamedarg = std::get<ast::ExprNamedArg>((*expression)._); auto [_w0, inner_expression, _w1] = _v_exprnamedarg; return InferPass_visit_named_arg(infer_pass, inner_expression); } if (std::holds_alternative<ast::ExprWith>((*expression)._)) { auto _v_exprwith = std::get<ast::ExprWith>((*expression)._); auto [resource, binder, statements, _w0] = _v_exprwith; return InferPass_visit_with(infer_pass, resource, binder, statements); } if (std::holds_alternative<ast::ExprSpawn>((*expression)._)) { auto _v_exprspawn = std::get<ast::ExprSpawn>((*expression)._); auto [statements, _w0] = _v_exprspawn; return InferPass_visit_spawn(infer_pass, statements); } return InferPass_visit_unsupported(infer_pass); }();}

infer_result::InferResult InferPass_visit_int(infer::InferPass self) noexcept{return infer_literals::infer_expr_integer_literal();}

infer_result::InferResult InferPass_visit_str(infer::InferPass self) noexcept{return infer_literals::infer_expr_string_literal();}

infer_result::InferResult InferPass_visit_bool(infer::InferPass self) noexcept{return infer_literals::infer_expr_boolean_literal();}

infer_result::InferResult InferPass_visit_unit(infer::InferPass self) noexcept{return infer_literals::infer_expr_unit_literal();}

infer_result::InferResult InferPass_visit_float(infer::InferPass self) noexcept{return infer_literals::infer_expr_f64_literal();}

infer_result::InferResult InferPass_visit_i64(infer::InferPass self) noexcept{return infer_literals::infer_expr_i64_literal();}

infer_result::InferResult InferPass_visit_u8(infer::InferPass self) noexcept{return infer_literals::infer_expr_u8_literal();}

infer_result::InferResult InferPass_visit_usize(infer::InferPass self) noexcept{return infer_literals::infer_expr_usize_literal();}

infer_result::InferResult InferPass_visit_char(infer::InferPass self) noexcept{return infer_literals::infer_expr_char_literal();}

infer_result::InferResult InferPass_visit_extern(infer::InferPass self) noexcept{return infer_literals::infer_expr_extern_placeholder();}

infer_result::InferResult InferPass_visit_ident(infer::InferPass self, mlc::String name) noexcept{return infer_expr_ident::infer_expr_identifier(name, self.inference_context);}

infer_result::InferResult InferPass_visit_bin(infer::InferPass self, mlc::String operation, std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, ast::Span source_span) noexcept{return infer_expr_binary(operation, left, right, source_span, self.inference_context);}

infer_result::InferResult InferPass_visit_un(infer::InferPass self, mlc::String operation, std::shared_ptr<ast::Expr> inner_expression, ast::Span source_span) noexcept{return infer_expr_unary(operation, inner_expression, source_span, self.inference_context);}

infer_result::InferResult InferPass_visit_call(infer::InferPass self, std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span) noexcept{return infer_call::infer_expr_call(function, call_arguments, call_source_span, self.inference_context, infer_expr);}

infer_result::InferResult InferPass_visit_method(infer::InferPass self, std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span) noexcept{return infer_expr_method(object, method_name, method_arguments, method_span, self.inference_context, infer_expr);}

infer_result::InferResult InferPass_visit_field(infer::InferPass self, std::shared_ptr<ast::Expr> object, mlc::String field_name, ast::Span field_source_span) noexcept{return infer_expr_field(object, field_name, field_source_span, self.inference_context);}

infer_result::InferResult InferPass_visit_index(infer::InferPass self, std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expression, ast::Span bracket_source_span) noexcept{return infer_expr_index(object, index_expression, bracket_source_span, self.inference_context);}

infer_result::InferResult InferPass_visit_if(infer::InferPass self, std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expression, std::shared_ptr<ast::Expr> else_expression) noexcept{return infer_expr_conditional(condition, then_expression, else_expression, self.inference_context);}

infer_result::InferResult InferPass_visit_block(infer::InferPass self, mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression) noexcept{return infer_expr_block(statements, result_expression, self.inference_context);}

infer_result::InferResult InferPass_visit_while(infer::InferPass self, std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements) noexcept{return infer_expr_while_loop(condition, statements, self.inference_context);}

infer_result::InferResult InferPass_visit_for(infer::InferPass self, mlc::String variable_name, std::shared_ptr<ast::Expr> iterator, mlc::Array<std::shared_ptr<ast::Stmt>> statements) noexcept{return infer_expr_for_loop(variable_name, iterator, statements, self.inference_context);}

infer_result::InferResult InferPass_visit_match(infer::InferPass self, std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms) noexcept{return infer_match::infer_expr_match(subject, match_arms, self.inference_context, infer_expr);}

infer_result::InferResult InferPass_visit_record(infer::InferPass self, mlc::String type_name, mlc::Array<ast::RecordLitPart> lit_parts, ast::Span span) noexcept{return infer_expr_record(type_name, lit_parts, self.inference_context, span);}

infer_result::InferResult InferPass_visit_record_update(infer::InferPass self, mlc::String type_name, std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values) noexcept{return infer_expr_record_update(type_name, base, field_values, self.inference_context);}

infer_result::InferResult InferPass_visit_array(infer::InferPass self, mlc::Array<std::shared_ptr<ast::Expr>> elements) noexcept{return infer_expr_array_literal(elements, self.inference_context);}

infer_result::InferResult InferPass_visit_tuple(infer::InferPass self, mlc::Array<std::shared_ptr<ast::Expr>> elements) noexcept{return infer_expr_tuple_literal(elements, self.inference_context);}

infer_result::InferResult InferPass_visit_question(infer::InferPass self, std::shared_ptr<ast::Expr> inner_expression, ast::Span question_span) noexcept{return infer_expr_question(inner_expression, question_span, self.inference_context);}

infer_result::InferResult InferPass_visit_lambda(infer::InferPass self, mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body) noexcept{return infer_expr_lambda(parameter_names, body, self.inference_context);}

infer_result::InferResult InferPass_visit_named_arg(infer::InferPass self, std::shared_ptr<ast::Expr> inner_expression) noexcept{return dispatch_infer_pass(self, inner_expression);}

infer_result::InferResult InferPass_visit_with(infer::InferPass self, std::shared_ptr<ast::Expr> resource, mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> statements) noexcept{return infer_expr_with(resource, binder, statements, self.inference_context);}

infer_result::InferResult InferPass_visit_spawn(infer::InferPass self, mlc::Array<std::shared_ptr<ast::Stmt>> statements) noexcept{return infer_expr_spawn(statements, self.inference_context);}

infer_result::InferResult InferPass_visit_unsupported(infer::InferPass self) noexcept{return infer_result::infer_ok(std::make_shared<registry::Type>((registry::TUnknown{})));}

infer_result::InferResult infer_expr(std::shared_ptr<ast::Expr> expression, check_context::CheckContext inference_context) noexcept{
std::shared_ptr<ast::Expr> expression_partial_application = partial_application_desugar::partial_application_desugar_expr(expression);
return dispatch_infer_pass(infer_pass_new(inference_context), expression_partial_application);
}

infer::Record_literal_spread_inference_fold_state accumulate_record_literal_spread_inference_for_literal_part(infer::Record_literal_spread_inference_fold_state fold_state, ast::RecordLitPart literal_part_under_inference, check_context::CheckContext inference_context) noexcept{return std::visit(overloaded{
  [&](const RecordLitSpread& recordlitspread) -> infer::Record_literal_spread_inference_fold_state { auto [spread_expression_under_inference] = recordlitspread; return [&]() -> infer::Record_literal_spread_inference_fold_state { 
  infer_result::InferResult spread_expression_inference = infer_expr(spread_expression_under_inference, inference_context);
  return infer::Record_literal_spread_inference_fold_state{infer_result::InferResult_absorb(fold_state.inference_result_so_far, spread_expression_inference), fold_state.spread_inferred_types_in_visit_order.concat(mlc::Array<std::shared_ptr<registry::Type>>{spread_expression_inference.inferred_type})};
 }(); },
  [&](const RecordLitFields& recordlitfields) -> infer::Record_literal_spread_inference_fold_state { auto [_w0] = recordlitfields; return fold_state; }
}, literal_part_under_inference);}

infer::Record_literal_spread_inference_fold_state infer_record_literal_fold_spread_inference_parts(infer_result::InferResult inference_before_spread_merge, mlc::Array<ast::RecordLitPart> literal_parts_under_inference, check_context::CheckContext inference_context) noexcept{return literal_parts_under_inference.fold(infer::Record_literal_spread_inference_fold_state{inference_before_spread_merge, {}}, [inference_context](infer::Record_literal_spread_inference_fold_state accumulator_fold_state, ast::RecordLitPart literal_part_under_inference) mutable { return accumulate_record_literal_spread_inference_for_literal_part(accumulator_fold_state, literal_part_under_inference, inference_context); });}

infer_result::InferResult infer_expr_record(mlc::String type_name, mlc::Array<ast::RecordLitPart> lit_parts, check_context::CheckContext inference_context, ast::Span span) noexcept{
mlc::Array<ast::Diagnostic> private_errors = registry::TypeRegistry_is_private_ctor(inference_context.registry, type_name) && inference_context.current_extend_type != type_name ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("private constructor: cannot construct ") + type_name + mlc::String(" outside its extend block"), span, diagnostic_codes::diagnostic_code_e065())} : mlc::Array<ast::Diagnostic>{};
infer_result::InferResult record_inference = infer_result::InferResult{std::make_shared<registry::Type>(registry::TNamed(type_name)), private_errors};
mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_field_values_flat = record_lit_merge::collect_explicit_field_values_flat(lit_parts);
return !record_lit_merge::record_literal_contains_spread(lit_parts) ? [&]() -> infer_result::InferResult { 
  if (registry::TypeRegistry_has_fields(inference_context.registry, type_name)){
{
record_inference = infer_explicit_record_literal_field_name_errors(record_inference, type_name, explicit_field_values_flat, inference_context.registry);
}
}
  mlc::Array<std::shared_ptr<ast::FieldVal>> merged_with_optional_defaults = record_lit_merge::merge_explicit_record_literal_with_type_defaults(type_name, explicit_field_values_flat, inference_context.registry);
  record_inference = infer_field_values_errors(record_inference, merged_with_optional_defaults, inference_context, type_name);
  if (registry::TypeRegistry_has_fields(inference_context.registry, type_name)){
{
record_inference = infer_result::InferResult{record_inference.inferred_type, ast::diagnostics_append(record_inference.errors, record_lit_merge::diagnostics_missing_fields_for_ordered_record(type_name, inference_context.registry, merged_with_optional_defaults, span))};
}
}
  return record_inference;
 }() : !registry::TypeRegistry_has_fields(inference_context.registry, type_name) ? [&]() -> infer_result::InferResult { 
  record_inference = infer_field_values_errors(record_inference, explicit_field_values_flat, inference_context, mlc::String(""));
  return infer_result::InferResult{record_inference.inferred_type, ast::diagnostics_append(record_inference.errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("record literal spread requires a named record type with fields"), span, diagnostic_codes::diagnostic_code_e066())})};
 }() : [&]() -> infer_result::InferResult { 
  record_inference = infer_field_values_errors(record_inference, explicit_field_values_flat, inference_context, type_name);
  infer::Record_literal_spread_inference_fold_state spread_inference_after_literal_parts = infer_record_literal_fold_spread_inference_parts(record_inference, lit_parts, inference_context);
  record_inference = spread_inference_after_literal_parts.inference_result_so_far;
  mlc::Array<std::shared_ptr<registry::Type>> spread_types_in_order = spread_inference_after_literal_parts.spread_inferred_types_in_visit_order;
  mlc::Array<std::shared_ptr<ast::FieldVal>> merged_field_values = record_lit_merge::merge_record_literal_parts_to_field_values(type_name, lit_parts, inference_context.registry, spread_types_in_order);
  mlc::Array<std::shared_ptr<ast::FieldVal>> merged_including_decl_defaults = record_lit_merge::merge_explicit_record_literal_with_type_defaults(type_name, merged_field_values, inference_context.registry);
  record_inference = infer_field_values_errors(record_inference, merged_including_decl_defaults, inference_context, type_name);
  record_inference = infer_result::InferResult{record_inference.inferred_type, ast::diagnostics_append(record_inference.errors, record_lit_merge::diagnostics_missing_fields_for_ordered_record(type_name, inference_context.registry, merged_including_decl_defaults, span))};
  return record_inference;
 }();
}

infer::Statement_sequence_inference_fold_state infer_single_statement_augment_fold_state(infer::Statement_sequence_inference_fold_state accumulator, std::shared_ptr<ast::Stmt> statement_under_inference, registry::TypeRegistry registry) noexcept{return std::visit(overloaded{
  [&](const StmtLet& stmtlet) -> infer::Statement_sequence_inference_fold_state { auto [binding_name, _w0, _w1, value_expression, _w2] = stmtlet; return [&]() -> infer::Statement_sequence_inference_fold_state { 
  infer_result::InferResult value_inference = infer_expr(value_expression, check_context::check_context_new(accumulator.type_environment_under_construction, registry));
  accumulator.type_environment_under_construction.set(binding_name, value_inference.inferred_type);
  return infer::Statement_sequence_inference_fold_state{ast::diagnostics_append(accumulator.diagnostics_collected_so_far, value_inference.errors), accumulator.type_environment_under_construction};
 }(); },
  [&](const StmtLetPattern& stmtletpattern) -> infer::Statement_sequence_inference_fold_state { auto [pattern, _w0, _w1, value_expression, has_else, else_body, source_span] = stmtletpattern; return [&]() -> infer::Statement_sequence_inference_fold_state { 
  infer_result::InferResult value_inference = infer_expr(value_expression, check_context::check_context_new(accumulator.type_environment_under_construction, registry));
  let_pattern_infer::infer_let_pattern_env(pattern, value_inference.inferred_type, accumulator.type_environment_under_construction, registry);
  mlc::Array<ast::Diagnostic> next_diagnostics = ast::diagnostics_append(accumulator.diagnostics_collected_so_far, value_inference.errors);
  bool pattern_is_refutable = [&]() { if (std::holds_alternative<ast::PatternCtor>((*pattern))) { auto _v_patternctor = std::get<ast::PatternCtor>((*pattern)); auto [_w0, _w1, _w2] = _v_patternctor; return true; } return false; }();
  if (pattern_is_refutable && !has_else){
{
next_diagnostics = ast::diagnostics_append(next_diagnostics, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("refutable pattern in let requires `else` branch"), source_span, diagnostic_codes::diagnostic_code_e067())});
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
infer::Statement_sequence_inference_fold_state inference_after_statement_sequence = statements.fold(infer::Statement_sequence_inference_fold_state{{}, inference_context.type_env}, [inference_context](infer::Statement_sequence_inference_fold_state accumulator_fold_state, std::shared_ptr<ast::Stmt> statement_under_inference_within_block) mutable { return infer_single_statement_augment_fold_state(accumulator_fold_state, statement_under_inference_within_block, inference_context.registry); });
return infer_result::StmtInferResult{inference_after_statement_sequence.type_environment_under_construction, inference_after_statement_sequence.diagnostics_collected_so_far};
}

} // namespace infer
