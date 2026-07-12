#define main mlc_user_main
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
#include "infer_region_method.hpp"
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
using namespace infer_region_method;
using namespace record_lit_merge;
using namespace partial_application_desugar;
using namespace diagnostic_codes;
using namespace ast_tokens;

infer_result::InferResult InferPass_visit_int(InferPass self) noexcept;
infer_result::InferResult InferPass_visit_str(InferPass self) noexcept;
infer_result::InferResult InferPass_visit_bool(InferPass self) noexcept;
infer_result::InferResult InferPass_visit_unit(InferPass self) noexcept;
infer_result::InferResult InferPass_visit_float(InferPass self) noexcept;
infer_result::InferResult InferPass_visit_i64(InferPass self) noexcept;
infer_result::InferResult InferPass_visit_u8(InferPass self) noexcept;
infer_result::InferResult InferPass_visit_usize(InferPass self) noexcept;
infer_result::InferResult InferPass_visit_char(InferPass self) noexcept;
infer_result::InferResult InferPass_visit_extern(InferPass self) noexcept;
infer_result::InferResult InferPass_visit_ident(InferPass self, mlc::String name) noexcept;
infer_result::InferResult InferPass_visit_bin(InferPass self, mlc::String operation, std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, ast::Span source_span) noexcept;
infer_result::InferResult InferPass_visit_un(InferPass self, mlc::String operation, std::shared_ptr<ast::Expr> inner_expression, ast::Span source_span) noexcept;
infer_result::InferResult InferPass_visit_call(InferPass self, std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span) noexcept;
infer_result::InferResult InferPass_visit_method(InferPass self, std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span) noexcept;
infer_result::InferResult InferPass_visit_field(InferPass self, std::shared_ptr<ast::Expr> object, mlc::String field_name, ast::Span field_source_span) noexcept;
infer_result::InferResult InferPass_visit_index(InferPass self, std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expression, ast::Span bracket_source_span) noexcept;
infer_result::InferResult InferPass_visit_if(InferPass self, std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expression, std::shared_ptr<ast::Expr> else_expression) noexcept;
infer_result::InferResult InferPass_visit_block(InferPass self, mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression) noexcept;
infer_result::InferResult InferPass_visit_while(InferPass self, std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements) noexcept;
infer_result::InferResult InferPass_visit_for(InferPass self, mlc::String variable_name, std::shared_ptr<ast::Expr> iterator, mlc::Array<std::shared_ptr<ast::Stmt>> statements) noexcept;
infer_result::InferResult InferPass_visit_match(InferPass self, std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms) noexcept;
infer_result::InferResult InferPass_visit_record(InferPass self, mlc::String type_name, mlc::Array<ast::RecordLitPart> lit_parts, ast::Span span) noexcept;
infer_result::InferResult InferPass_visit_record_update(InferPass self, mlc::String type_name, std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values) noexcept;
infer_result::InferResult InferPass_visit_array(InferPass self, mlc::Array<std::shared_ptr<ast::Expr>> elements) noexcept;
infer_result::InferResult InferPass_visit_tuple(InferPass self, mlc::Array<std::shared_ptr<ast::Expr>> elements) noexcept;
infer_result::InferResult InferPass_visit_question(InferPass self, std::shared_ptr<ast::Expr> inner_expression, ast::Span question_span) noexcept;
infer_result::InferResult InferPass_visit_lambda(InferPass self, mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body) noexcept;
infer_result::InferResult InferPass_visit_named_arg(InferPass self, std::shared_ptr<ast::Expr> inner_expression) noexcept;
infer_result::InferResult InferPass_visit_with(InferPass self, std::shared_ptr<ast::Expr> resource, mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> statements) noexcept;
infer_result::InferResult InferPass_visit_spawn(InferPass self, mlc::Array<std::shared_ptr<ast::Stmt>> statements) noexcept;
infer_result::InferResult InferPass_visit_scope(InferPass self, mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> statements) noexcept;
infer_result::InferResult InferPass_visit_region(InferPass self, mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> statements, ast::Span source_span) noexcept;
infer_result::InferResult InferPass_visit_unsupported(InferPass self) noexcept;
infer_result::InferResult infer_arguments_errors(infer_result::InferResult initial, mlc::Array<std::shared_ptr<ast::Expr>> expressions, check_context::CheckContext inference_context) noexcept{
  return expressions.fold(initial, [=](infer_result::InferResult accumulated_inference, std::shared_ptr<ast::Expr> expression_under_inference) mutable { return infer_result::InferResult_absorb(accumulated_inference, infer_expr(expression_under_inference, inference_context)); });
}
infer_result::InferResult infer_record_field_binding_value_inference_step(infer_result::InferResult accumulator, std::shared_ptr<ast::FieldVal> field_value, check_context::CheckContext inference_context, mlc::String record_type_name_for_fields) noexcept{
  auto registry_has_ordered_fields_for_type = ((record_type_name_for_fields.length() > 0) && registry::TypeRegistry_has_fields(inference_context.registry, record_type_name_for_fields));
  auto next_parsed = (registry_has_ordered_fields_for_type ? (infer_result::InferResult{accumulator.inferred_type, ast::diagnostics_append(accumulator.errors, type_diagnostics::infer_expr_field_diagnostics(std::make_shared<registry::Type>(registry::TNamed{record_type_name_for_fields}), field_value->name, ast::expr_span(field_value->value), inference_context.registry))}) : (accumulator));
  auto value_inference = infer_expr(field_value->value, inference_context);
  if (registry_has_ordered_fields_for_type)   {
    auto expected_field_type = registry::field_type_from_object(std::make_shared<registry::Type>(registry::TNamed{record_type_name_for_fields}), field_value->name, inference_context.registry);
    auto inferred_value_type = value_inference.inferred_type;
    auto field_types_conflict = (((!semantic_type_structure::type_is_unknown(expected_field_type)) && (!semantic_type_structure::type_is_unknown(inferred_value_type))) && (!semantic_type_structure::types_structurally_equal(expected_field_type, inferred_value_type)));
    auto field_type_mismatch_diagnostics = (field_types_conflict ? (mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code((((((mlc::String("field ", 6) + field_value->name) + mlc::String(": expected ", 11)) + semantic_type_structure::type_description(expected_field_type)) + mlc::String(", got ", 6)) + semantic_type_structure::type_description(inferred_value_type)), ast::expr_span(field_value->value), diagnostic_codes::diagnostic_code_e075())}) : (mlc::Array<ast::Diagnostic>{}));
    return infer_result::InferResult{next_parsed.inferred_type, ast::diagnostics_append(ast::diagnostics_append(next_parsed.errors, value_inference.errors), field_type_mismatch_diagnostics)};
  } else   {
    return infer_result::InferResult_absorb(next_parsed, value_inference);
  }
}
infer_result::InferResult infer_field_values_errors(infer_result::InferResult initial, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context, mlc::String record_type_name_for_fields) noexcept{
  return field_values.fold(initial, [=](infer_result::InferResult accumulator, std::shared_ptr<ast::FieldVal> field_value_under_inference) mutable { return infer_record_field_binding_value_inference_step(accumulator, field_value_under_inference, inference_context, record_type_name_for_fields); });
}
infer_result::InferResult infer_explicit_record_literal_field_unknown_name_step(infer_result::InferResult accumulator, std::shared_ptr<ast::FieldVal> field_value_binding, mlc::String record_type_name, registry::TypeRegistry registry) noexcept{
  return infer_result::InferResult{accumulator.inferred_type, ast::diagnostics_append(accumulator.errors, type_diagnostics::infer_expr_field_diagnostics(std::make_shared<registry::Type>(registry::TNamed{record_type_name}), field_value_binding->name, ast::expr_span(field_value_binding->value), registry))};
}
infer_result::InferResult infer_explicit_record_literal_field_name_errors(infer_result::InferResult initial, mlc::String record_type_name, mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_field_values_flat, registry::TypeRegistry registry) noexcept{
  return explicit_field_values_flat.fold(initial, [=](infer_result::InferResult accumulator, std::shared_ptr<ast::FieldVal> field_value_binding) mutable { return infer_explicit_record_literal_field_unknown_name_step(accumulator, field_value_binding, record_type_name, registry); });
}
infer_result::InferResult infer_expr_binary(mlc::String operation, std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, ast::Span source_span, check_context::CheckContext inference_context) noexcept{
  auto left_parsed = infer_expr(left, inference_context);
  auto right_parsed = infer_expr(right, inference_context);
  auto method = semantic_type_structure::operator_method_for(operation);
  if ((method != mlc::String("", 0)))   {
    auto method_type = registry::method_return_type_from_object(left_parsed.inferred_type, method, inference_context.registry);
    if ((!semantic_type_structure::type_is_unknown(method_type)))     {
      return infer_result::InferResult{method_type, ast::diagnostics_append(left_parsed.errors, right_parsed.errors)};
    } else     {
      return infer_operand_combine::infer_binary_from_operand_results(operation, left_parsed, right_parsed, source_span);
    }
  } else   {
    return infer_operand_combine::infer_binary_from_operand_results(operation, left_parsed, right_parsed, source_span);
  }
}
infer_result::InferResult infer_expr_unary(mlc::String operation, std::shared_ptr<ast::Expr> inner_expression, ast::Span source_span, check_context::CheckContext inference_context) noexcept{
  return infer_operand_combine::infer_unary_from_inner_result(operation, infer_expr(inner_expression, inference_context), source_span);
}
infer_result::InferResult infer_expr_method(std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept{
  auto object_parsed = infer_expr_fn(object, inference_context);
  if (infer_arc_method::is_arc_static_new(object, method_name))   {
    return infer_arc_method::infer_arc_static_new_call(object_parsed, method_arguments, method_span, inference_context, infer_expr_fn);
  } else if (infer_weak_method::is_shared_static_new(object, method_name))   {
    return infer_weak_method::infer_shared_static_new_call(object_parsed, method_arguments, method_span, inference_context, infer_expr_fn);
  } else if (infer_mutex_method::is_mutex_static_new(object, method_name))   {
    return infer_mutex_method::infer_mutex_static_new_call(object_parsed, method_arguments, method_span, inference_context, infer_expr_fn);
  } else if (infer_weak_method::is_shared_weak_method_on_receiver(object_parsed.inferred_type, method_name))   {
    return infer_weak_method::infer_shared_weak_method_call(object_parsed, method_name, method_arguments, method_span, inference_context, infer_expr_fn);
  } else if (infer_weak_method::is_weak_upgrade_method_on_receiver(object_parsed.inferred_type, method_name))   {
    return infer_weak_method::infer_weak_upgrade_method_call(object_parsed, method_name, method_arguments, method_span, inference_context, infer_expr_fn);
  } else if (hof_method_spec::is_result_option_hof_method(object_parsed.inferred_type, method_name))   {
    return infer_result_option_method::infer_result_option_hof_method_call(object_parsed, method_name, method_arguments, method_span, inference_context, infer_expr_fn);
  } else if (channel_method_types::is_channel_method(method_name))   {
    return infer_channel_method::infer_channel_method_call(object_parsed, method_name, method_arguments, method_span, inference_context, infer_expr_fn);
  } else if ((mutex_method_types::is_mutex_method(method_name) && semantic_type_structure::type_is_mutex(object_parsed.inferred_type)))   {
    return infer_mutex_method::infer_mutex_method_call(object_parsed, method_name, method_arguments, method_span, inference_context, infer_expr_fn);
  } else if (is_task_scope_spawn_method(object_parsed.inferred_type, method_name))   {
    return infer_task_scope_spawn_call(object_parsed, method_arguments, method_span, inference_context, infer_expr_fn);
  } else if (infer_region_method::is_region_handle_method(object_parsed.inferred_type, method_name))   {
    return infer_region_method::infer_region_handle_method_call(object_parsed, method_name, method_arguments, method_span, inference_context, infer_expr_fn);
  } else if (hof_method_spec::is_array_hof_method_on_receiver(object_parsed.inferred_type, method_name))   {
    return infer_array_method::infer_array_hof_method_call(object_parsed, method_name, method_arguments, method_span, inference_context, infer_expr_fn);
  } else   {
    auto with_arguments = infer_arguments_errors(object_parsed, method_arguments, inference_context);
    return infer_operand_combine::infer_method_from_object_and_arguments(object_parsed, with_arguments, method_name, method_span, method_arguments.length(), inference_context.registry);
  }
}
bool type_is_task_scope(std::shared_ptr<registry::Type> type_value) noexcept{
  return [&]() -> bool {
auto __match_subject = type_value;
if (std::holds_alternative<registry::TNamed>((*__match_subject))) {
const registry::TNamed& tNamed = std::get<registry::TNamed>((*__match_subject));
auto [type_name] = tNamed; return (type_name == mlc::String("TaskScope", 9));
}
return false;
std::abort();
}();
}
bool is_task_scope_spawn_method(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept{
  return ((method_name == mlc::String("spawn", 5)) && type_is_task_scope(receiver_type));
}
infer_result::InferResult infer_expr_field(std::shared_ptr<ast::Expr> object, mlc::String field_name, ast::Span field_source_span, check_context::CheckContext inference_context) noexcept{
  return infer_operand_combine::infer_field_from_object_result(infer_expr(object, inference_context), field_name, field_source_span, inference_context.registry);
}
infer_result::InferResult infer_expr_index(std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expression, ast::Span bracket_source_span, check_context::CheckContext inference_context) noexcept{
  return infer_operand_combine::infer_index_from_operand_results(infer_expr(object, inference_context), infer_expr(index_expression, inference_context), bracket_source_span);
}
infer_result::InferResult infer_expr_conditional(std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expression, std::shared_ptr<ast::Expr> else_expression, check_context::CheckContext inference_context) noexcept{
  return infer_operand_combine::infer_conditional_from_branch_results(infer_expr(condition, inference_context), infer_expr(then_expression, inference_context), infer_expr(else_expression, inference_context), else_expression);
}
infer_result::InferResult infer_expr_block(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression, check_context::CheckContext inference_context) noexcept{
  auto statements_parsed = infer_statements(statements, inference_context);
  auto result_parsed = infer_expr(result_expression, check_context::check_context_child(inference_context, statements_parsed.type_env));
  return infer_result::InferResult_absorb_stmt(result_parsed, statements_parsed);
}
infer_result::InferResult infer_expr_with(std::shared_ptr<ast::Expr> resource, mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept{
  auto resource_parsed = infer_expr(resource, inference_context);
  auto with_body_environment = inference_context.type_env;
  with_body_environment.set(binder, resource_parsed.inferred_type);
  auto statements_parsed = infer_statements(statements, check_context::check_context_child(inference_context, with_body_environment));
  return infer_result::InferResult_with_type(infer_result::InferResult_absorb_stmt(resource_parsed, statements_parsed), std::make_shared<registry::Type>(registry::TUnit{}));
}
infer_result::InferResult infer_expr_while_loop(std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept{
  auto condition_parsed = infer_expr(condition, inference_context);
  auto statements_parsed = infer_statements(statements, inference_context);
  return infer_result::InferResult_with_type(infer_result::InferResult_absorb_stmt(condition_parsed, statements_parsed), std::make_shared<registry::Type>(registry::TUnit{}));
}
infer_result::InferResult infer_expr_spawn(mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept{
  auto body_statements = ast::expr_spawn_body_statements(statements);
  auto statements_parsed = infer_statements(body_statements, inference_context);
  auto body_context = check_context::check_context_child(inference_context, statements_parsed.type_env);
  auto result_parsed = infer_expr(ast::expr_spawn_body_result(statements), body_context);
  return infer_result::InferResult_with_type(infer_result::InferResult_absorb_stmt(result_parsed, statements_parsed), std::make_shared<registry::Type>(registry::TGeneric{mlc::String("Task", 4), mlc::Array<std::shared_ptr<registry::Type>>{result_parsed.inferred_type}}));
}
infer_result::InferResult infer_expr_scope(mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept{
  auto scope_body_environment = inference_context.type_env;
  scope_body_environment.set(binder, std::make_shared<registry::Type>(registry::TNamed{mlc::String("TaskScope", 9)}));
  auto statements_parsed = infer_statements(statements, check_context::check_context_child(inference_context, scope_body_environment));
  return infer_result::InferResult_absorb_stmt(infer_result::infer_ok(std::make_shared<registry::Type>(registry::TUnit{})), statements_parsed);
}
infer_result::InferResult infer_expr_region(mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> statements, ast::Span source_span, check_context::CheckContext inference_context) noexcept{
  auto region_body_environment = inference_context.type_env;
  region_body_environment.set(binder, infer_region_method::region_handle_type(infer_region_method::region_tag_type_from_span(source_span)));
  auto statements_parsed = infer_statements(statements, check_context::check_context_child(inference_context, region_body_environment));
  return infer_result::InferResult_absorb_stmt(infer_result::infer_ok(std::make_shared<registry::Type>(registry::TUnit{})), statements_parsed);
}
infer_result::InferResult infer_expr_for_loop(mlc::String variable_name, std::shared_ptr<ast::Expr> iterator, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept{
  auto iterator_base_parsed = infer_expr(iterator, inference_context);
  auto iterator_parsed = infer_for_support::infer_for_iterator_with_range_rules(iterator_base_parsed, iterator);
  auto element_type = infer_for_support::element_type_for_for_iterator(iterator_parsed.inferred_type);
  auto inner_environment = inference_context.type_env;
  inner_environment.set(variable_name, element_type);
  auto loop_context = check_context::check_context_child(inference_context, inner_environment);
  auto statements_parsed = infer_statements(statements, loop_context);
  return infer_result::InferResult_with_type(infer_result::InferResult_absorb_stmt(iterator_parsed, statements_parsed), std::make_shared<registry::Type>(registry::TUnit{}));
}
infer_result::InferResult infer_expr_record_update(mlc::String type_name, std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context) noexcept{
  auto base_parsed = infer_expr(base, inference_context);
  return infer_field_values_errors(infer_result::InferResult_with_type(base_parsed, std::make_shared<registry::Type>(registry::TNamed{type_name})), field_values, inference_context, type_name);
}
Infer_tuple_literal_fold_state infer_tuple_literal_element_fold_step(Infer_tuple_literal_fold_state state, std::shared_ptr<ast::Expr> tuple_element, check_context::CheckContext inference_context) noexcept{
  auto element_inference = infer_expr(tuple_element, inference_context);
  return Infer_tuple_literal_fold_state{infer_result::InferResult_absorb(state.combined_inference, element_inference), state.member_types.concat(mlc::Array<std::shared_ptr<registry::Type>>{element_inference.inferred_type})};
}
infer_result::InferResult infer_expr_tuple_literal(mlc::Array<std::shared_ptr<ast::Expr>> elements, check_context::CheckContext inference_context) noexcept{
  if ((elements.length() < 2))   {
    return infer_arguments_errors(infer_result::infer_ok(std::make_shared<registry::Type>(registry::TUnknown{})), elements, inference_context);
  } else   {
    auto folded_tuple_inference = elements.fold(Infer_tuple_literal_fold_state{infer_result::infer_ok(std::make_shared<registry::Type>(registry::TUnknown{})), {}}, [=](Infer_tuple_literal_fold_state accumulator_state, std::shared_ptr<ast::Expr> tuple_element_under_inference) mutable { return infer_tuple_literal_element_fold_step(accumulator_state, tuple_element_under_inference, inference_context); });
    return infer_result::InferResult_with_type(folded_tuple_inference.combined_inference, std::make_shared<registry::Type>(registry::TTuple{folded_tuple_inference.member_types}));
  }
}
infer_result::InferResult infer_expr_array_literal(mlc::Array<std::shared_ptr<ast::Expr>> elements, check_context::CheckContext inference_context) noexcept{
  auto first_element_type = ((elements.length() > 0) ? (infer_expr(elements[0], inference_context).inferred_type) : (std::make_shared<registry::Type>(registry::TUnknown{})));
  return infer_arguments_errors(infer_result::infer_ok(std::make_shared<registry::Type>(registry::TArray{first_element_type})), elements, inference_context);
}
infer_result::InferResult infer_expr_question(std::shared_ptr<ast::Expr> inner_expression, ast::Span question_span, check_context::CheckContext inference_context) noexcept{
  return infer_question_expression::infer_question_from_inner_result(infer_expr(inner_expression, inference_context), question_span, inference_context.expected_return_type, inference_context.registry);
}
std::shared_ptr<registry::Type> inference_placeholder_unknown_type() noexcept{
  return std::make_shared<registry::Type>(registry::TUnknown{});
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> lambda_inference_environment_assign_unknown_placeholder(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment_for_parameters, mlc::String parameter_binding_name) noexcept{
  type_environment_for_parameters.set(parameter_binding_name, inference_placeholder_unknown_type());
  return type_environment_for_parameters;
}
infer_result::InferResult infer_expr_lambda(mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body, check_context::CheckContext inference_context) noexcept{
  auto lambda_type_environment = inference_context.type_env;
  auto inferred_unknown_parameter_types = parameter_names.map([=](mlc::String unused_lambda_parameter_placeholder) mutable { return inference_placeholder_unknown_type(); });
  parameter_names.fold(lambda_type_environment, [=](mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> environment_map, mlc::String parameter_binding_name) mutable { return lambda_inference_environment_assign_unknown_placeholder(environment_map, parameter_binding_name); });
  auto lambda_inference_context = check_context::check_context_child(inference_context, lambda_type_environment);
  auto body_inference = infer_expr(body, lambda_inference_context);
  return infer_result::infer_ok(std::make_shared<registry::Type>(registry::TFn{inferred_unknown_parameter_types, body_inference.inferred_type}));
}
InferPass infer_pass_new(check_context::CheckContext inference_context) noexcept{
  return InferPass{inference_context, 0};
}
infer_result::InferResult dispatch_infer_pass(InferPass infer_pass, std::shared_ptr<ast::Expr> expression) noexcept{
  return [&]() -> infer_result::InferResult {
auto __match_subject = expression;
if (std::holds_alternative<ast::ExprInt>((*__match_subject))) {
const ast::ExprInt& exprInt = std::get<ast::ExprInt>((*__match_subject));
auto [__0, __1] = exprInt; return InferPass_visit_int(infer_pass);
}
if (std::holds_alternative<ast::ExprStr>((*__match_subject))) {
const ast::ExprStr& exprStr = std::get<ast::ExprStr>((*__match_subject));
auto [__0, __1] = exprStr; return InferPass_visit_str(infer_pass);
}
if (std::holds_alternative<ast::ExprBool>((*__match_subject))) {
const ast::ExprBool& exprBool = std::get<ast::ExprBool>((*__match_subject));
auto [__0, __1] = exprBool; return InferPass_visit_bool(infer_pass);
}
if (std::holds_alternative<ast::ExprUnit>((*__match_subject))) {
const ast::ExprUnit& exprUnit = std::get<ast::ExprUnit>((*__match_subject));
auto [__0] = exprUnit; return InferPass_visit_unit(infer_pass);
}
if (std::holds_alternative<ast::ExprFloat>((*__match_subject))) {
const ast::ExprFloat& exprFloat = std::get<ast::ExprFloat>((*__match_subject));
auto [__0, __1] = exprFloat; return InferPass_visit_float(infer_pass);
}
if (std::holds_alternative<ast::ExprI64>((*__match_subject))) {
const ast::ExprI64& exprI64 = std::get<ast::ExprI64>((*__match_subject));
auto [__0, __1] = exprI64; return InferPass_visit_i64(infer_pass);
}
if (std::holds_alternative<ast::ExprU8>((*__match_subject))) {
const ast::ExprU8& exprU8 = std::get<ast::ExprU8>((*__match_subject));
auto [__0, __1] = exprU8; return InferPass_visit_u8(infer_pass);
}
if (std::holds_alternative<ast::ExprUsize>((*__match_subject))) {
const ast::ExprUsize& exprUsize = std::get<ast::ExprUsize>((*__match_subject));
auto [__0, __1] = exprUsize; return InferPass_visit_usize(infer_pass);
}
if (std::holds_alternative<ast::ExprChar>((*__match_subject))) {
const ast::ExprChar& exprChar = std::get<ast::ExprChar>((*__match_subject));
auto [__0, __1] = exprChar; return InferPass_visit_char(infer_pass);
}
if (std::holds_alternative<ast::ExprExtern>((*__match_subject))) {
const ast::ExprExtern& exprExtern = std::get<ast::ExprExtern>((*__match_subject));
auto [__0, __1, __2, __3] = exprExtern; return InferPass_visit_extern(infer_pass);
}
if (std::holds_alternative<ast::ExprIdent>((*__match_subject))) {
const ast::ExprIdent& exprIdent = std::get<ast::ExprIdent>((*__match_subject));
auto [name, __1] = exprIdent; return InferPass_visit_ident(infer_pass, name);
}
if (std::holds_alternative<ast::ExprBin>((*__match_subject))) {
const ast::ExprBin& exprBin = std::get<ast::ExprBin>((*__match_subject));
auto [operation, left, right, span] = exprBin; return InferPass_visit_bin(infer_pass, operation, left, right, span);
}
if (std::holds_alternative<ast::ExprUn>((*__match_subject))) {
const ast::ExprUn& exprUn = std::get<ast::ExprUn>((*__match_subject));
auto [operation, inner_expression, span] = exprUn; return InferPass_visit_un(infer_pass, operation, inner_expression, span);
}
if (std::holds_alternative<ast::ExprCall>((*__match_subject))) {
const ast::ExprCall& exprCall = std::get<ast::ExprCall>((*__match_subject));
auto [function, call_arguments, call_source_span] = exprCall; return InferPass_visit_call(infer_pass, function, call_arguments, call_source_span);
}
if (std::holds_alternative<ast::ExprMethod>((*__match_subject))) {
const ast::ExprMethod& exprMethod = std::get<ast::ExprMethod>((*__match_subject));
auto [object, method_name, method_arguments, method_span] = exprMethod; return InferPass_visit_method(infer_pass, object, method_name, method_arguments, method_span);
}
if (std::holds_alternative<ast::ExprField>((*__match_subject))) {
const ast::ExprField& exprField = std::get<ast::ExprField>((*__match_subject));
auto [object, field_name, field_source_span] = exprField; return InferPass_visit_field(infer_pass, object, field_name, field_source_span);
}
if (std::holds_alternative<ast::ExprIndex>((*__match_subject))) {
const ast::ExprIndex& exprIndex = std::get<ast::ExprIndex>((*__match_subject));
auto [object, index_expression, bracket_source_span] = exprIndex; return InferPass_visit_index(infer_pass, object, index_expression, bracket_source_span);
}
if (std::holds_alternative<ast::ExprIf>((*__match_subject))) {
const ast::ExprIf& exprIf = std::get<ast::ExprIf>((*__match_subject));
auto [condition, then_expression, else_expression, __3] = exprIf; return InferPass_visit_if(infer_pass, condition, then_expression, else_expression);
}
if (std::holds_alternative<ast::ExprBlock>((*__match_subject))) {
const ast::ExprBlock& exprBlock = std::get<ast::ExprBlock>((*__match_subject));
auto [statements, result_expression, __2] = exprBlock; return InferPass_visit_block(infer_pass, statements, result_expression);
}
if (std::holds_alternative<ast::ExprWhile>((*__match_subject))) {
const ast::ExprWhile& exprWhile = std::get<ast::ExprWhile>((*__match_subject));
auto [condition, statements, __2] = exprWhile; return InferPass_visit_while(infer_pass, condition, statements);
}
if (std::holds_alternative<ast::ExprFor>((*__match_subject))) {
const ast::ExprFor& exprFor = std::get<ast::ExprFor>((*__match_subject));
auto [variable_name, iterator, statements, __3] = exprFor; return InferPass_visit_for(infer_pass, variable_name, iterator, statements);
}
if (std::holds_alternative<ast::ExprMatch>((*__match_subject))) {
const ast::ExprMatch& exprMatch = std::get<ast::ExprMatch>((*__match_subject));
auto [subject, match_arms, __2] = exprMatch; return InferPass_visit_match(infer_pass, subject, match_arms);
}
if (std::holds_alternative<ast::ExprRecord>((*__match_subject))) {
const ast::ExprRecord& exprRecord = std::get<ast::ExprRecord>((*__match_subject));
auto [name, lit_parts, span] = exprRecord; return InferPass_visit_record(infer_pass, name, lit_parts, span);
}
if (std::holds_alternative<ast::ExprRecordUpdate>((*__match_subject))) {
const ast::ExprRecordUpdate& exprRecordUpdate = std::get<ast::ExprRecordUpdate>((*__match_subject));
auto [name, base, field_values, __3] = exprRecordUpdate; return InferPass_visit_record_update(infer_pass, name, base, field_values);
}
if (std::holds_alternative<ast::ExprArray>((*__match_subject))) {
const ast::ExprArray& exprArray = std::get<ast::ExprArray>((*__match_subject));
auto [elements, __1] = exprArray; return InferPass_visit_array(infer_pass, elements);
}
if (std::holds_alternative<ast::ExprTuple>((*__match_subject))) {
const ast::ExprTuple& exprTuple = std::get<ast::ExprTuple>((*__match_subject));
auto [elements, __1] = exprTuple; return InferPass_visit_tuple(infer_pass, elements);
}
if (std::holds_alternative<ast::ExprQuestion>((*__match_subject))) {
const ast::ExprQuestion& exprQuestion = std::get<ast::ExprQuestion>((*__match_subject));
auto [inner_expression, question_span] = exprQuestion; return InferPass_visit_question(infer_pass, inner_expression, question_span);
}
if (std::holds_alternative<ast::ExprLambda>((*__match_subject))) {
const ast::ExprLambda& exprLambda = std::get<ast::ExprLambda>((*__match_subject));
auto [parameter_names, body, __2] = exprLambda; return InferPass_visit_lambda(infer_pass, parameter_names, body);
}
if (std::holds_alternative<ast::ExprNamedArg>((*__match_subject))) {
const ast::ExprNamedArg& exprNamedArg = std::get<ast::ExprNamedArg>((*__match_subject));
auto [__0, inner_expression, __2] = exprNamedArg; return InferPass_visit_named_arg(infer_pass, inner_expression);
}
if (std::holds_alternative<ast::ExprWith>((*__match_subject))) {
const ast::ExprWith& exprWith = std::get<ast::ExprWith>((*__match_subject));
auto [resource, binder, statements, __3] = exprWith; return InferPass_visit_with(infer_pass, resource, binder, statements);
}
if (std::holds_alternative<ast::ExprSpawn>((*__match_subject))) {
const ast::ExprSpawn& exprSpawn = std::get<ast::ExprSpawn>((*__match_subject));
auto [statements, __1] = exprSpawn; return InferPass_visit_spawn(infer_pass, statements);
}
if (std::holds_alternative<ast::ExprScope>((*__match_subject))) {
const ast::ExprScope& exprScope = std::get<ast::ExprScope>((*__match_subject));
auto [binder, statements, __2] = exprScope; return InferPass_visit_scope(infer_pass, binder, statements);
}
if (std::holds_alternative<ast::ExprRegion>((*__match_subject))) {
const ast::ExprRegion& exprRegion = std::get<ast::ExprRegion>((*__match_subject));
auto [binder, statements, source_span] = exprRegion; return InferPass_visit_region(infer_pass, binder, statements, source_span);
}
return InferPass_visit_unsupported(infer_pass);
std::abort();
}();
}
infer_result::InferResult InferPass_visit_int(InferPass self) noexcept;
infer_result::InferResult InferPass_visit_str(InferPass self) noexcept;
infer_result::InferResult InferPass_visit_bool(InferPass self) noexcept;
infer_result::InferResult InferPass_visit_unit(InferPass self) noexcept;
infer_result::InferResult InferPass_visit_float(InferPass self) noexcept;
infer_result::InferResult InferPass_visit_i64(InferPass self) noexcept;
infer_result::InferResult InferPass_visit_u8(InferPass self) noexcept;
infer_result::InferResult InferPass_visit_usize(InferPass self) noexcept;
infer_result::InferResult InferPass_visit_char(InferPass self) noexcept;
infer_result::InferResult InferPass_visit_extern(InferPass self) noexcept;
infer_result::InferResult InferPass_visit_ident(InferPass self, mlc::String name) noexcept;
infer_result::InferResult InferPass_visit_bin(InferPass self, mlc::String operation, std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, ast::Span source_span) noexcept;
infer_result::InferResult InferPass_visit_un(InferPass self, mlc::String operation, std::shared_ptr<ast::Expr> inner_expression, ast::Span source_span) noexcept;
infer_result::InferResult InferPass_visit_call(InferPass self, std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span) noexcept;
infer_result::InferResult InferPass_visit_method(InferPass self, std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span) noexcept;
infer_result::InferResult InferPass_visit_field(InferPass self, std::shared_ptr<ast::Expr> object, mlc::String field_name, ast::Span field_source_span) noexcept;
infer_result::InferResult InferPass_visit_index(InferPass self, std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expression, ast::Span bracket_source_span) noexcept;
infer_result::InferResult InferPass_visit_if(InferPass self, std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expression, std::shared_ptr<ast::Expr> else_expression) noexcept;
infer_result::InferResult InferPass_visit_block(InferPass self, mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression) noexcept;
infer_result::InferResult InferPass_visit_while(InferPass self, std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements) noexcept;
infer_result::InferResult InferPass_visit_for(InferPass self, mlc::String variable_name, std::shared_ptr<ast::Expr> iterator, mlc::Array<std::shared_ptr<ast::Stmt>> statements) noexcept;
infer_result::InferResult InferPass_visit_match(InferPass self, std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms) noexcept;
infer_result::InferResult InferPass_visit_record(InferPass self, mlc::String type_name, mlc::Array<ast::RecordLitPart> lit_parts, ast::Span span) noexcept;
infer_result::InferResult InferPass_visit_record_update(InferPass self, mlc::String type_name, std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values) noexcept;
infer_result::InferResult InferPass_visit_array(InferPass self, mlc::Array<std::shared_ptr<ast::Expr>> elements) noexcept;
infer_result::InferResult InferPass_visit_tuple(InferPass self, mlc::Array<std::shared_ptr<ast::Expr>> elements) noexcept;
infer_result::InferResult InferPass_visit_question(InferPass self, std::shared_ptr<ast::Expr> inner_expression, ast::Span question_span) noexcept;
infer_result::InferResult InferPass_visit_lambda(InferPass self, mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body) noexcept;
infer_result::InferResult InferPass_visit_named_arg(InferPass self, std::shared_ptr<ast::Expr> inner_expression) noexcept;
infer_result::InferResult InferPass_visit_with(InferPass self, std::shared_ptr<ast::Expr> resource, mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> statements) noexcept;
infer_result::InferResult InferPass_visit_spawn(InferPass self, mlc::Array<std::shared_ptr<ast::Stmt>> statements) noexcept;
infer_result::InferResult InferPass_visit_scope(InferPass self, mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> statements) noexcept;
infer_result::InferResult InferPass_visit_region(InferPass self, mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> statements, ast::Span source_span) noexcept;
infer_result::InferResult InferPass_visit_unsupported(InferPass self) noexcept;
infer_result::InferResult InferPass_visit_int(InferPass self) noexcept{
return infer_literals::infer_expr_integer_literal();
}
infer_result::InferResult InferPass_visit_str(InferPass self) noexcept{
return infer_literals::infer_expr_string_literal();
}
infer_result::InferResult InferPass_visit_bool(InferPass self) noexcept{
return infer_literals::infer_expr_boolean_literal();
}
infer_result::InferResult InferPass_visit_unit(InferPass self) noexcept{
return infer_literals::infer_expr_unit_literal();
}
infer_result::InferResult InferPass_visit_float(InferPass self) noexcept{
return infer_literals::infer_expr_f64_literal();
}
infer_result::InferResult InferPass_visit_i64(InferPass self) noexcept{
return infer_literals::infer_expr_i64_literal();
}
infer_result::InferResult InferPass_visit_u8(InferPass self) noexcept{
return infer_literals::infer_expr_u8_literal();
}
infer_result::InferResult InferPass_visit_usize(InferPass self) noexcept{
return infer_literals::infer_expr_usize_literal();
}
infer_result::InferResult InferPass_visit_char(InferPass self) noexcept{
return infer_literals::infer_expr_char_literal();
}
infer_result::InferResult InferPass_visit_extern(InferPass self) noexcept{
return infer_literals::infer_expr_extern_placeholder();
}
infer_result::InferResult InferPass_visit_ident(InferPass self, mlc::String name) noexcept{
return infer_expr_ident::infer_expr_identifier(name, self.inference_context);
}
infer_result::InferResult InferPass_visit_bin(InferPass self, mlc::String operation, std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, ast::Span source_span) noexcept{
return infer_expr_binary(operation, left, right, source_span, self.inference_context);
}
infer_result::InferResult InferPass_visit_un(InferPass self, mlc::String operation, std::shared_ptr<ast::Expr> inner_expression, ast::Span source_span) noexcept{
return infer_expr_unary(operation, inner_expression, source_span, self.inference_context);
}
infer_result::InferResult InferPass_visit_call(InferPass self, std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span) noexcept{
return infer_call::infer_expr_call(function, call_arguments, call_source_span, self.inference_context, infer_expr);
}
infer_result::InferResult InferPass_visit_method(InferPass self, std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span) noexcept{
return infer_expr_method(object, method_name, method_arguments, method_span, self.inference_context, infer_expr);
}
infer_result::InferResult InferPass_visit_field(InferPass self, std::shared_ptr<ast::Expr> object, mlc::String field_name, ast::Span field_source_span) noexcept{
return infer_expr_field(object, field_name, field_source_span, self.inference_context);
}
infer_result::InferResult InferPass_visit_index(InferPass self, std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expression, ast::Span bracket_source_span) noexcept{
return infer_expr_index(object, index_expression, bracket_source_span, self.inference_context);
}
infer_result::InferResult InferPass_visit_if(InferPass self, std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expression, std::shared_ptr<ast::Expr> else_expression) noexcept{
return infer_expr_conditional(condition, then_expression, else_expression, self.inference_context);
}
infer_result::InferResult InferPass_visit_block(InferPass self, mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression) noexcept{
return infer_expr_block(statements, result_expression, self.inference_context);
}
infer_result::InferResult InferPass_visit_while(InferPass self, std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements) noexcept{
return infer_expr_while_loop(condition, statements, self.inference_context);
}
infer_result::InferResult InferPass_visit_for(InferPass self, mlc::String variable_name, std::shared_ptr<ast::Expr> iterator, mlc::Array<std::shared_ptr<ast::Stmt>> statements) noexcept{
return infer_expr_for_loop(variable_name, iterator, statements, self.inference_context);
}
infer_result::InferResult InferPass_visit_match(InferPass self, std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms) noexcept{
return infer_match::infer_expr_match(subject, match_arms, self.inference_context, infer_expr);
}
infer_result::InferResult InferPass_visit_record(InferPass self, mlc::String type_name, mlc::Array<ast::RecordLitPart> lit_parts, ast::Span span) noexcept{
return infer_expr_record(type_name, lit_parts, self.inference_context, span);
}
infer_result::InferResult InferPass_visit_record_update(InferPass self, mlc::String type_name, std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values) noexcept{
return infer_expr_record_update(type_name, base, field_values, self.inference_context);
}
infer_result::InferResult InferPass_visit_array(InferPass self, mlc::Array<std::shared_ptr<ast::Expr>> elements) noexcept{
return infer_expr_array_literal(elements, self.inference_context);
}
infer_result::InferResult InferPass_visit_tuple(InferPass self, mlc::Array<std::shared_ptr<ast::Expr>> elements) noexcept{
return infer_expr_tuple_literal(elements, self.inference_context);
}
infer_result::InferResult InferPass_visit_question(InferPass self, std::shared_ptr<ast::Expr> inner_expression, ast::Span question_span) noexcept{
return infer_expr_question(inner_expression, question_span, self.inference_context);
}
infer_result::InferResult InferPass_visit_lambda(InferPass self, mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body) noexcept{
return infer_expr_lambda(parameter_names, body, self.inference_context);
}
infer_result::InferResult InferPass_visit_named_arg(InferPass self, std::shared_ptr<ast::Expr> inner_expression) noexcept{
return dispatch_infer_pass(self, inner_expression);
}
infer_result::InferResult InferPass_visit_with(InferPass self, std::shared_ptr<ast::Expr> resource, mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> statements) noexcept{
return infer_expr_with(resource, binder, statements, self.inference_context);
}
infer_result::InferResult InferPass_visit_spawn(InferPass self, mlc::Array<std::shared_ptr<ast::Stmt>> statements) noexcept{
return infer_expr_spawn(statements, self.inference_context);
}
infer_result::InferResult InferPass_visit_scope(InferPass self, mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> statements) noexcept{
return infer_expr_scope(binder, statements, self.inference_context);
}
infer_result::InferResult InferPass_visit_region(InferPass self, mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> statements, ast::Span source_span) noexcept{
return infer_expr_region(binder, statements, source_span, self.inference_context);
}
infer_result::InferResult InferPass_visit_unsupported(InferPass self) noexcept{
return infer_result::infer_ok(std::make_shared<registry::Type>(registry::TUnknown{}));
}
infer_result::InferResult infer_expr(std::shared_ptr<ast::Expr> expression, check_context::CheckContext inference_context) noexcept{
  auto expression_partial_application = partial_application_desugar::partial_application_desugar_expr(expression);
  return dispatch_infer_pass(infer_pass_new(inference_context), expression_partial_application);
}
Record_literal_spread_inference_fold_state accumulate_record_literal_spread_inference_for_literal_part(Record_literal_spread_inference_fold_state fold_state, ast::RecordLitPart literal_part_under_inference, check_context::CheckContext inference_context) noexcept{
  return std::visit(overloaded{[&](const ast::RecordLitSpread& recordLitSpread) -> Record_literal_spread_inference_fold_state { auto [spread_expression_under_inference] = recordLitSpread; return [&]() {
auto spread_expression_inference = infer_expr(spread_expression_under_inference, inference_context);
return Record_literal_spread_inference_fold_state{infer_result::InferResult_absorb(fold_state.inference_result_so_far, spread_expression_inference), fold_state.spread_inferred_types_in_visit_order.concat(mlc::Array<std::shared_ptr<registry::Type>>{spread_expression_inference.inferred_type})};
}(); },
[&](const ast::RecordLitFields& recordLitFields) -> Record_literal_spread_inference_fold_state { auto [__0] = recordLitFields; return fold_state; }
}, literal_part_under_inference);
}
Record_literal_spread_inference_fold_state infer_record_literal_fold_spread_inference_parts(infer_result::InferResult inference_before_spread_merge, mlc::Array<ast::RecordLitPart> literal_parts_under_inference, check_context::CheckContext inference_context) noexcept{
  return literal_parts_under_inference.fold(Record_literal_spread_inference_fold_state{inference_before_spread_merge, {}}, [=](Record_literal_spread_inference_fold_state accumulator_fold_state, ast::RecordLitPart literal_part_under_inference) mutable { return accumulate_record_literal_spread_inference_for_literal_part(accumulator_fold_state, literal_part_under_inference, inference_context); });
}
infer_result::InferResult infer_expr_record(mlc::String type_name, mlc::Array<ast::RecordLitPart> lit_parts, check_context::CheckContext inference_context, ast::Span span) noexcept{
  auto private_errors = ((registry::TypeRegistry_is_private_ctor(inference_context.registry, type_name) && (inference_context.current_extend_type != type_name)) ? (mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(((mlc::String("private constructor: cannot construct ", 38) + type_name) + mlc::String(" outside its extend block", 25)), span, diagnostic_codes::diagnostic_code_e065())}) : (mlc::Array<ast::Diagnostic>{}));
  auto record_inference = infer_result::InferResult{std::make_shared<registry::Type>(registry::TNamed{type_name}), private_errors};
  auto explicit_field_values_flat = record_lit_merge::collect_explicit_field_values_flat(lit_parts);
  if ((!record_lit_merge::record_literal_contains_spread(lit_parts)))   {
    if (registry::TypeRegistry_has_fields(inference_context.registry, type_name))     {
      (record_inference = infer_explicit_record_literal_field_name_errors(record_inference, type_name, explicit_field_values_flat, inference_context.registry));
    }
    auto merged_with_optional_defaults = record_lit_merge::merge_explicit_record_literal_with_type_defaults(type_name, explicit_field_values_flat, inference_context.registry);
    (record_inference = infer_field_values_errors(record_inference, merged_with_optional_defaults, inference_context, type_name));
    if (registry::TypeRegistry_has_fields(inference_context.registry, type_name))     {
      (record_inference = infer_result::InferResult{record_inference.inferred_type, ast::diagnostics_append(record_inference.errors, record_lit_merge::diagnostics_missing_fields_for_ordered_record(type_name, inference_context.registry, merged_with_optional_defaults, span))});
    }
    return record_inference;
  } else if ((!registry::TypeRegistry_has_fields(inference_context.registry, type_name)))   {
    (record_inference = infer_field_values_errors(record_inference, explicit_field_values_flat, inference_context, mlc::String("", 0)));
    return infer_result::InferResult{record_inference.inferred_type, ast::diagnostics_append(record_inference.errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("record literal spread requires a named record type with fields", 62), span, diagnostic_codes::diagnostic_code_e066())})};
  } else   {
    (record_inference = infer_field_values_errors(record_inference, explicit_field_values_flat, inference_context, type_name));
    auto spread_inference_after_literal_parts = infer_record_literal_fold_spread_inference_parts(record_inference, lit_parts, inference_context);
    (record_inference = spread_inference_after_literal_parts.inference_result_so_far);
    auto spread_types_in_order = spread_inference_after_literal_parts.spread_inferred_types_in_visit_order;
    auto merged_field_values = record_lit_merge::merge_record_literal_parts_to_field_values(type_name, lit_parts, inference_context.registry, spread_types_in_order);
    auto merged_including_decl_defaults = record_lit_merge::merge_explicit_record_literal_with_type_defaults(type_name, merged_field_values, inference_context.registry);
    (record_inference = infer_field_values_errors(record_inference, merged_including_decl_defaults, inference_context, type_name));
    (record_inference = infer_result::InferResult{record_inference.inferred_type, ast::diagnostics_append(record_inference.errors, record_lit_merge::diagnostics_missing_fields_for_ordered_record(type_name, inference_context.registry, merged_including_decl_defaults, span))});
    return record_inference;
  }
}
Statement_sequence_inference_fold_state infer_single_statement_augment_fold_state(Statement_sequence_inference_fold_state accumulator, std::shared_ptr<ast::Stmt> statement_under_inference, check_context::CheckContext inference_context) noexcept{
  return std::visit(overloaded{[&](const ast::StmtLet& stmtLet) -> Statement_sequence_inference_fold_state { auto [binding_name, __1, __2, value_expression, __4] = stmtLet; return [&]() {
auto value_inference = infer_expr(value_expression, check_context::check_context_child(inference_context, accumulator.type_environment_under_construction));
accumulator.type_environment_under_construction.set(binding_name, value_inference.inferred_type);
return Statement_sequence_inference_fold_state{ast::diagnostics_append(accumulator.diagnostics_collected_so_far, value_inference.errors), accumulator.type_environment_under_construction};
}(); },
[&](const ast::StmtLetPattern& stmtLetPattern) -> Statement_sequence_inference_fold_state { auto [pattern, __1, __2, value_expression, has_else, else_body, source_span] = stmtLetPattern; return [&]() {
auto value_inference = infer_expr(value_expression, check_context::check_context_child(inference_context, accumulator.type_environment_under_construction));
let_pattern_infer::infer_let_pattern_env(pattern, value_inference.inferred_type, accumulator.type_environment_under_construction, inference_context.registry);
auto next_diagnostics = ast::diagnostics_append(accumulator.diagnostics_collected_so_far, value_inference.errors);
auto pattern_is_refutable = [&]() -> bool {
auto __match_subject = pattern;
if (std::holds_alternative<ast::PatternCtor>((*__match_subject))) {
const ast::PatternCtor& patternCtor = std::get<ast::PatternCtor>((*__match_subject));
auto [__0, __1, __2] = patternCtor; return true;
}
return false;
std::abort();
}();
if ((pattern_is_refutable && (!has_else))) {
  (next_diagnostics = ast::diagnostics_append(next_diagnostics, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("refutable pattern in let requires `else` branch", 47), source_span, diagnostic_codes::diagnostic_code_e067())}));
}
if (has_else) {
  auto else_branch_inference = infer_expr(else_body, check_context::check_context_child(inference_context, accumulator.type_environment_under_construction));
  (next_diagnostics = ast::diagnostics_append(next_diagnostics, else_branch_inference.errors));
}
return Statement_sequence_inference_fold_state{next_diagnostics, accumulator.type_environment_under_construction};
}(); },
[&](const ast::StmtLetConst& stmtLetConst) -> Statement_sequence_inference_fold_state { auto [binding_name, __1, value_expression, __3] = stmtLetConst; return [&]() {
auto value_inference = infer_expr(value_expression, check_context::check_context_child(inference_context, accumulator.type_environment_under_construction));
accumulator.type_environment_under_construction.set(binding_name, value_inference.inferred_type);
return Statement_sequence_inference_fold_state{ast::diagnostics_append(accumulator.diagnostics_collected_so_far, value_inference.errors), accumulator.type_environment_under_construction};
}(); },
[&](const ast::StmtExpr& stmtExpr) -> Statement_sequence_inference_fold_state { auto [expression, __1] = stmtExpr; return [&]() {
auto expression_inference = infer_expr(expression, check_context::check_context_child(inference_context, accumulator.type_environment_under_construction));
return Statement_sequence_inference_fold_state{ast::diagnostics_append(accumulator.diagnostics_collected_so_far, expression_inference.errors), accumulator.type_environment_under_construction};
}(); },
[&](const ast::StmtReturn& stmtReturn) -> Statement_sequence_inference_fold_state { auto [return_expression, __1] = stmtReturn; return [&]() {
auto return_inference = infer_expr(return_expression, check_context::check_context_child(inference_context, accumulator.type_environment_under_construction));
return Statement_sequence_inference_fold_state{ast::diagnostics_append(accumulator.diagnostics_collected_so_far, return_inference.errors), accumulator.type_environment_under_construction};
}(); },
[&](const ast::StmtBreak& stmtBreak) -> Statement_sequence_inference_fold_state { auto [__0] = stmtBreak; return accumulator; },
[&](const ast::StmtContinue& stmtContinue) -> Statement_sequence_inference_fold_state { auto [__0] = stmtContinue; return accumulator; }
}, (*statement_under_inference));
}
infer_result::StmtInferResult infer_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept{
  auto inference_after_statement_sequence = statements.fold(Statement_sequence_inference_fold_state{{}, inference_context.type_env}, [=](Statement_sequence_inference_fold_state accumulator_fold_state, std::shared_ptr<ast::Stmt> statement_under_inference_within_block) mutable { return infer_single_statement_augment_fold_state(accumulator_fold_state, statement_under_inference_within_block, inference_context); });
  return infer_result::StmtInferResult{inference_after_statement_sequence.type_environment_under_construction, inference_after_statement_sequence.diagnostics_collected_so_far};
}

} // namespace infer
