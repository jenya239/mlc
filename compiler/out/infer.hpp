#ifndef INFER_HPP
#define INFER_HPP

#include "mlc.hpp"
#include <variant>
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

struct Infer_tuple_literal_fold_state {
  infer_result::InferResult combined_inference;
  mlc::Array<std::shared_ptr<registry::Type>> member_types;
};
struct InferPass {
  check_context::CheckContext inference_context;
  int seed;
};
struct Record_literal_spread_inference_fold_state {
  infer_result::InferResult inference_result_so_far;
  mlc::Array<std::shared_ptr<registry::Type>> spread_inferred_types_in_visit_order;
};
struct Statement_sequence_inference_fold_state {
  mlc::Array<ast::Diagnostic> diagnostics_collected_so_far;
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment_under_construction;
};
infer_result::InferResult infer_arguments_errors(infer_result::InferResult initial, mlc::Array<std::shared_ptr<ast::Expr>> expressions, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_record_field_binding_value_inference_step(infer_result::InferResult accumulator, std::shared_ptr<ast::FieldVal> field_value, check_context::CheckContext inference_context, mlc::String record_type_name_for_fields) noexcept;
infer_result::InferResult infer_field_values_errors(infer_result::InferResult initial, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context, mlc::String record_type_name_for_fields) noexcept;
infer_result::InferResult infer_explicit_record_literal_field_unknown_name_step(infer_result::InferResult accumulator, std::shared_ptr<ast::FieldVal> field_value_binding, mlc::String record_type_name, registry::TypeRegistry registry) noexcept;
infer_result::InferResult infer_explicit_record_literal_field_name_errors(infer_result::InferResult initial, mlc::String record_type_name, mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_field_values_flat, registry::TypeRegistry registry) noexcept;
infer_result::InferResult infer_expr_binary(mlc::String operation, std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, ast::Span source_span, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_unary(mlc::String operation, std::shared_ptr<ast::Expr> inner_expression, ast::Span source_span, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_method(std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;
bool type_is_task_scope(std::shared_ptr<registry::Type> type_value) noexcept;
bool is_task_scope_spawn_method(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept;
template<typename __F4>
infer_result::InferResult infer_task_scope_spawn_call(infer_result::InferResult object_parsed, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span _method_span, check_context::CheckContext inference_context, __F4 _infer_expr_fn) noexcept;
infer_result::InferResult infer_expr_field(std::shared_ptr<ast::Expr> object, mlc::String field_name, ast::Span field_source_span, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_index(std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expression, ast::Span bracket_source_span, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_conditional(std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expression, std::shared_ptr<ast::Expr> else_expression, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_block(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_with(std::shared_ptr<ast::Expr> resource, mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_while_loop(std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_spawn(mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_scope(mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_region(mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_for_loop(mlc::String variable_name, std::shared_ptr<ast::Expr> iterator, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_record_update(mlc::String type_name, std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context) noexcept;
Infer_tuple_literal_fold_state infer_tuple_literal_element_fold_step(Infer_tuple_literal_fold_state state, std::shared_ptr<ast::Expr> tuple_element, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_tuple_literal(mlc::Array<std::shared_ptr<ast::Expr>> elements, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_array_literal(mlc::Array<std::shared_ptr<ast::Expr>> elements, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_question(std::shared_ptr<ast::Expr> inner_expression, ast::Span question_span, check_context::CheckContext inference_context) noexcept;
std::shared_ptr<registry::Type> inference_placeholder_unknown_type() noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> lambda_inference_environment_assign_unknown_placeholder(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment_for_parameters, mlc::String parameter_binding_name) noexcept;
infer_result::InferResult infer_expr_lambda(mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body, check_context::CheckContext inference_context) noexcept;
InferPass infer_pass_new(check_context::CheckContext inference_context) noexcept;
infer_result::InferResult dispatch_infer_pass(InferPass infer_pass, std::shared_ptr<ast::Expr> expression) noexcept;
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
infer_result::InferResult InferPass_visit_region(InferPass self, mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> statements) noexcept;
infer_result::InferResult InferPass_visit_unsupported(InferPass self) noexcept;
infer_result::InferResult infer_expr(std::shared_ptr<ast::Expr> expression, check_context::CheckContext inference_context) noexcept;
Record_literal_spread_inference_fold_state accumulate_record_literal_spread_inference_for_literal_part(Record_literal_spread_inference_fold_state fold_state, ast::RecordLitPart literal_part_under_inference, check_context::CheckContext inference_context) noexcept;
Record_literal_spread_inference_fold_state infer_record_literal_fold_spread_inference_parts(infer_result::InferResult inference_before_spread_merge, mlc::Array<ast::RecordLitPart> literal_parts_under_inference, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_record(mlc::String type_name, mlc::Array<ast::RecordLitPart> lit_parts, check_context::CheckContext inference_context, ast::Span span) noexcept;
Statement_sequence_inference_fold_state infer_single_statement_augment_fold_state(Statement_sequence_inference_fold_state accumulator, std::shared_ptr<ast::Stmt> statement_under_inference, check_context::CheckContext inference_context) noexcept;
infer_result::StmtInferResult infer_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept;
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
infer_result::InferResult InferPass_visit_region(InferPass self, mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> statements) noexcept;
infer_result::InferResult InferPass_visit_unsupported(InferPass self) noexcept;
template<typename __F4>
infer_result::InferResult infer_task_scope_spawn_call(infer_result::InferResult object_parsed, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span _method_span, check_context::CheckContext inference_context, __F4 _infer_expr_fn) noexcept{
  auto with_arguments = infer_arguments_errors(object_parsed, method_arguments, inference_context);
  return infer_result::InferResult_with_type(with_arguments, std::make_shared<registry::Type>(registry::TUnit{}));
}

} // namespace infer

#endif // INFER_HPP
