#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"
#include "array_method_types.hpp"
#include "result_option_method_types.hpp"
#include "pattern_env.hpp"
#include "semantic_type_structure.hpp"
#include "infer.hpp"
#include "check_context.hpp"
#include "record_lit_merge.hpp"
#include "partial_application_desugar.hpp"
#include "semantic_ir.hpp"

namespace transform {

struct TransformContext {mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env;registry::TypeRegistry registry;};
struct TransformStmtsResult {mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements;mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env;};
struct Transform_lambda_parameter_types_fold_state {mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment;mlc::Array<std::shared_ptr<registry::Type>> parameter_type_vector;int next_explicit_type_position;};
mlc::String receiver_type_key_for_method_dispatch(std::shared_ptr<registry::Type> receiver_semantic_type) noexcept;
mlc::Array<int> direct_call_parameter_mutability_flags(registry::TypeRegistry registry, mlc::String callee_name, int positional_argument_count) noexcept;
mlc::Array<int> instance_method_receiver_and_parameters_mutability_pattern(registry::TypeRegistry registry, std::shared_ptr<registry::Type> receiver_semantic_type, mlc::String method_name, int method_argument_count) noexcept;
std::shared_ptr<semantic_ir::SExpr> coerce_unknown_else_array_when_then_known_inner(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, ast::Span span_else, std::shared_ptr<registry::Type> inner_then, std::shared_ptr<semantic_ir::SExpr> typed_else) noexcept;
std::shared_ptr<semantic_ir::SExpr> conditional_else_coerce_empty_array_using_then_type(std::shared_ptr<semantic_ir::SExpr> typed_then, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, ast::Span span_else, std::shared_ptr<semantic_ir::SExpr> typed_else) noexcept;
std::shared_ptr<semantic_ir::SExpr> array_literal_else_maybe_coerce(std::shared_ptr<semantic_ir::SExpr> typed_then, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, ast::Span span_else, std::shared_ptr<semantic_ir::SExpr> typed_else) noexcept;
std::shared_ptr<semantic_ir::SExpr> conditional_else_empty_unknown_array_coerced_to_then_array_element(std::shared_ptr<semantic_ir::SExpr> typed_then, std::shared_ptr<semantic_ir::SExpr> typed_else) noexcept;
TransformContext transform_context_new(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, registry::TypeRegistry registry) noexcept;
TransformContext empty_transform_context() noexcept;
TransformContext transform_context_with_env(TransformContext base, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env) noexcept;
bool transform_expression_is_named_argument(std::shared_ptr<ast::Expr> argument_expression) noexcept;
bool transform_has_named_args(mlc::Array<std::shared_ptr<ast::Expr>> args) noexcept;
int transform_find_param_slot(mlc::Array<mlc::String> parameter_names, mlc::String sought_parameter_name) noexcept;
std::shared_ptr<ast::Expr> transform_inner_after_stripping_named_label(std::shared_ptr<ast::Expr> argument_expression) noexcept;
mlc::Array<std::shared_ptr<ast::Expr>> transform_strip_labels(mlc::Array<std::shared_ptr<ast::Expr>> args) noexcept;
std::shared_ptr<semantic_ir::SExpr> coerce_expr_to_type(std::shared_ptr<semantic_ir::SExpr> expression, std::shared_ptr<registry::Type> target_type) noexcept;
mlc::Array<std::shared_ptr<ast::Expr>> transform_reorder_to_positional(mlc::Array<std::shared_ptr<ast::Expr>> args, mlc::Array<mlc::String> param_names) noexcept;
mlc::Array<std::shared_ptr<ast::Expr>> reorder_named_args_for_transform(mlc::Array<std::shared_ptr<ast::Expr>> args, mlc::String callee_name, registry::TypeRegistry registry) noexcept;
std::shared_ptr<registry::Type> standalone_unknown_cell() noexcept;
mlc::Array<std::shared_ptr<semantic_ir::SExpr>> transform_exprs(mlc::Array<std::shared_ptr<ast::Expr>> expressions, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SExpr> transform_one_call_argument_using_optional_expected_type(std::shared_ptr<ast::Expr> argument_expression_shared, std::shared_ptr<registry::Type> expected_formal_parameter_type, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
mlc::Array<std::shared_ptr<semantic_ir::SExpr>> transform_call_arguments_using_callee_semantic_type(std::shared_ptr<semantic_ir::SExpr> callee_semantic_expression, mlc::Array<std::shared_ptr<ast::Expr>> argument_expressions_under_call, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SMatchArm> transform_single_match_arm(std::shared_ptr<ast::MatchArm> arm, TransformContext transform_context, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution, std::shared_ptr<registry::Type> scrutinee_type, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> transform_match_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, TransformContext transform_context, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution, std::shared_ptr<registry::Type> scrutinee_type, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> lambda_environment_assign_unknown_placeholder(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment_map, mlc::String parameter_binding_name) noexcept;
Transform_lambda_parameter_types_fold_state transform_lambda_parameter_types_environment_fold_step(Transform_lambda_parameter_types_fold_state state, mlc::String parameter_binding_name, mlc::Array<std::shared_ptr<registry::Type>> optional_explicit_parameter_types) noexcept;
std::shared_ptr<semantic_ir::SExpr> transform_expr_lambda_with_param_types(mlc::Array<mlc::String> parameter_names, mlc::Array<std::shared_ptr<registry::Type>> parameter_types, std::shared_ptr<ast::Expr> body, ast::Span source_span, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SExpr> transform_expr_maybe_lambda_context(std::shared_ptr<ast::Expr> expression, mlc::Array<std::shared_ptr<registry::Type>> expected_param_types, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
mlc::Array<std::shared_ptr<semantic_ir::SExpr>> transform_array_hof_method_arguments(std::shared_ptr<semantic_ir::SExpr> typed_object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
mlc::Array<std::shared_ptr<registry::Type>> inferred_types_from_record_literal_part_for_merge(ast::RecordLitPart literal_record_part, check_context::CheckContext inference_context_for_spread) noexcept;
std::shared_ptr<semantic_ir::SExpr> transform_expr(std::shared_ptr<ast::Expr> expression, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;

} // namespace transform

#endif // TRANSFORM_HPP
