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
#include "infer_result_option_method.hpp"
#include "record_lit_merge.hpp"
#include "partial_application_desugar.hpp"

namespace infer {

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
Infer_tuple_literal_fold_state infer_tuple_literal_element_fold_step(Infer_tuple_literal_fold_state state, std::shared_ptr<ast::Expr> tuple_element, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_tuple_literal(mlc::Array<std::shared_ptr<ast::Expr>> elements, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_array_literal(mlc::Array<std::shared_ptr<ast::Expr>> elements, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_question(std::shared_ptr<ast::Expr> inner, ast::Span question_span, check_context::CheckContext inference_context) noexcept;
std::shared_ptr<registry::Type> inference_placeholder_unknown_type() noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> lambda_inference_environment_assign_unknown_placeholder(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment_for_parameters, mlc::String parameter_binding_name) noexcept;
infer_result::InferResult infer_expr_lambda(mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr(std::shared_ptr<ast::Expr> expression, check_context::CheckContext inference_context) noexcept;
Record_literal_spread_inference_fold_state accumulate_record_literal_spread_inference_for_literal_part(Record_literal_spread_inference_fold_state fold_state, ast::RecordLitPart literal_part_under_inference, check_context::CheckContext inference_context) noexcept;
Record_literal_spread_inference_fold_state infer_record_literal_fold_spread_inference_parts(infer_result::InferResult inference_before_spread_merge, mlc::Array<ast::RecordLitPart> literal_parts_under_inference, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_record(mlc::String type_name, mlc::Array<ast::RecordLitPart> lit_parts, check_context::CheckContext inference_context, ast::Span span) noexcept;
Statement_sequence_inference_fold_state infer_single_statement_augment_fold_state(Statement_sequence_inference_fold_state accumulator, std::shared_ptr<ast::Stmt> statement_under_inference, registry::TypeRegistry registry) noexcept;
infer_result::StmtInferResult infer_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept;

} // namespace infer

#endif // INFER_HPP
