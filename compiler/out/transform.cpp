#include "transform.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"
#include "hof_method_spec.hpp"
#include "pattern_env.hpp"
#include "semantic_type_structure.hpp"
#include "substitution.hpp"
#include "infer.hpp"
#include "check_context.hpp"
#include "record_lit_merge.hpp"
#include "partial_application_desugar.hpp"
#include "named_args.hpp"
#include "semantic_ir.hpp"

namespace transform {

using namespace ast;
using namespace registry;
using namespace semantic_type_structure;
using namespace hof_method_spec;
using namespace pattern_env;
using namespace semantic_type_structure;
using namespace substitution;
using namespace infer;
using namespace check_context;
using namespace record_lit_merge;
using namespace partial_application_desugar;
using namespace named_args;
using namespace semantic_ir;
using namespace ast_tokens;

struct Transform_call_arguments_fold_state {mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> transformed_arguments;int next_argument_index;};

struct Transform_lambda_parameter_types_fold_state {mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment;mlc::Array<std::shared_ptr<registry::Type>> parameter_type_vector;int next_explicit_type_position;};

struct TransformPass {transform::TransformContext transform_context;int seed;};

mlc::String receiver_type_key_for_method_dispatch(std::shared_ptr<registry::Type> receiver_semantic_type) noexcept;

mlc::Array<int> direct_call_parameter_mutability_flags(registry::TypeRegistry registry, mlc::String callee_name, int positional_argument_count) noexcept;

mlc::Array<int> instance_method_receiver_and_parameters_mutability_pattern(registry::TypeRegistry registry, std::shared_ptr<registry::Type> receiver_semantic_type, mlc::String method_name, int method_argument_count) noexcept;

mlc::String extend_method_mangled_name(registry::TypeRegistry registry, std::shared_ptr<registry::Type> receiver_semantic_type, mlc::String method_name) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> transform_extend_method_as_call(registry::TypeRegistry registry, std::shared_ptr<semantic_ir::SemanticExpression> typed_object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> typed_arguments, std::shared_ptr<registry::Type> receiver_type, std::shared_ptr<registry::Type> result_type, ast::Span source_span) noexcept;

bool semantic_type_is_tarray(std::shared_ptr<registry::Type> type_value) noexcept;

std::shared_ptr<registry::Type> array_element_type_from_semantic_type(std::shared_ptr<registry::Type> type_value) noexcept;

mlc::String generic_type_name(std::shared_ptr<registry::Type> type_value) noexcept;

bool callee_semantic_type_is_function(std::shared_ptr<registry::Type> type_value) noexcept;

mlc::Array<std::shared_ptr<registry::Type>> function_parameter_types_from_callee_type(std::shared_ptr<registry::Type> type_value) noexcept;

mlc::String call_callee_ident_name(std::shared_ptr<ast::Expr> function) noexcept;

bool call_argument_is_lambda(std::shared_ptr<ast::Expr> argument_expression) noexcept;

std::shared_ptr<registry::Type> binary_result_type_for_operator(registry::TypeRegistry registry, std::shared_ptr<registry::Type> left_type, mlc::String operation, mlc::String method) noexcept;

std::shared_ptr<registry::Type> method_result_type_for_dispatch(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name, registry::TypeRegistry registry) noexcept;

std::shared_ptr<registry::Type> merge_conditional_expression_types(std::shared_ptr<semantic_ir::SemanticExpression> typed_then, std::shared_ptr<semantic_ir::SemanticExpression> typed_else) noexcept;

std::shared_ptr<registry::Type> array_element_type_from_semantic_expression(std::shared_ptr<semantic_ir::SemanticExpression> typed_expression) noexcept;

mlc::Array<std::shared_ptr<registry::Type>> type_arguments_from_generic_type(std::shared_ptr<registry::Type> type_value) noexcept;

std::shared_ptr<registry::Type> question_unwrapped_type_from_inner(std::shared_ptr<registry::Type> inner_expression_type) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> transform_result_option_hof_method_call(registry::TypeRegistry registry, std::shared_ptr<semantic_ir::SemanticExpression> typed_object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, std::shared_ptr<registry::Type> receiver_type, transform::TransformContext transform_context, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> transform_array_hof_method_call(registry::TypeRegistry registry, std::shared_ptr<semantic_ir::SemanticExpression> typed_object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, std::shared_ptr<registry::Type> receiver_type, transform::TransformContext transform_context, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> transform_regular_method_call(registry::TypeRegistry registry, std::shared_ptr<semantic_ir::SemanticExpression> typed_object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, std::shared_ptr<registry::Type> receiver_type, transform::TransformContext transform_context, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> coerce_unknown_else_array_when_then_known_inner(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, ast::Span span_else, std::shared_ptr<registry::Type> then_element_type, std::shared_ptr<semantic_ir::SemanticExpression> typed_else) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> conditional_else_coerce_empty_array_using_then_type(std::shared_ptr<semantic_ir::SemanticExpression> typed_then, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, ast::Span span_else, std::shared_ptr<semantic_ir::SemanticExpression> typed_else) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> array_literal_else_maybe_coerce(std::shared_ptr<semantic_ir::SemanticExpression> typed_then, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, ast::Span span_else, std::shared_ptr<semantic_ir::SemanticExpression> typed_else) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> conditional_else_empty_unknown_array_coerced_to_then_array_element(std::shared_ptr<semantic_ir::SemanticExpression> typed_then, std::shared_ptr<semantic_ir::SemanticExpression> typed_else) noexcept;

transform::TransformContext transform_context_new(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, registry::TypeRegistry registry) noexcept;

transform::TransformContext empty_transform_context() noexcept;

transform::TransformContext transform_context_with_env(transform::TransformContext base, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env) noexcept;

transform::TransformContext transform_context_with_lambda_parameter_types(transform::TransformContext base, mlc::Array<std::shared_ptr<registry::Type>> lambda_parameter_types) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> coerce_array_semantic_expression_to_type(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, ast::Span source_span, std::shared_ptr<registry::Type> target_type, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> coerce_record_semantic_expression_to_type(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> fields, std::shared_ptr<registry::Type> expr_type, ast::Span source_span, std::shared_ptr<registry::Type> target_type, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> coerce_block_semantic_expression_to_type(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, ast::Span span, std::shared_ptr<registry::Type> target_type, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> coerce_if_semantic_expression_to_type(std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_branch, std::shared_ptr<semantic_ir::SemanticExpression> else_branch, ast::Span source_span, std::shared_ptr<registry::Type> target_type, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> coerce_expr_to_type(std::shared_ptr<semantic_ir::SemanticExpression> expression, std::shared_ptr<registry::Type> target_type) noexcept;

std::shared_ptr<registry::Type> standalone_unknown_cell() noexcept;

mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> transform_exprs(mlc::Array<std::shared_ptr<ast::Expr>> expressions, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<registry::Type> expected_call_argument_type_at_index(int argument_index, mlc::Array<std::shared_ptr<registry::Type>> expected_formal_parameter_types) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> transform_lambda_call_argument(mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> lambda_body, ast::Span lambda_span, std::shared_ptr<registry::Type> expected_formal_parameter_type, std::shared_ptr<ast::Expr> argument_expression_shared, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> transform_one_call_argument_using_optional_expected_type(std::shared_ptr<ast::Expr> argument_expression_shared, std::shared_ptr<registry::Type> expected_formal_parameter_type, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<registry::Type> function_return_type_from_callee_type(std::shared_ptr<registry::Type> type_value) noexcept;

transform::Transform_call_arguments_fold_state transform_call_arguments_fold_step(transform::Transform_call_arguments_fold_state state, std::shared_ptr<ast::Expr> argument_expression, mlc::Array<std::shared_ptr<registry::Type>> expected_formal_parameter_types, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> transform_call_arguments_using_callee_semantic_type(std::shared_ptr<semantic_ir::SemanticExpression> callee_semantic_expression, mlc::Array<std::shared_ptr<ast::Expr>> argument_expressions_under_call, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticFieldVal> semantic_field_val_from_source_field_after_transform(std::shared_ptr<ast::FieldVal> field_value_under_transform, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> transform_field_values(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticMatchArm> transform_single_match_arm(std::shared_ptr<ast::MatchArm> match_arm, transform::TransformContext transform_context, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution, std::shared_ptr<registry::Type> scrutinee_type, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> transform_match_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms, transform::TransformContext transform_context, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution, std::shared_ptr<registry::Type> scrutinee_type, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> lambda_environment_assign_unknown_placeholder(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment_map, mlc::String parameter_binding_name) noexcept;

transform::Transform_lambda_parameter_types_fold_state transform_lambda_parameter_types_environment_fold_step(transform::Transform_lambda_parameter_types_fold_state state, mlc::String parameter_binding_name, mlc::Array<std::shared_ptr<registry::Type>> optional_explicit_parameter_types) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> transform_expr_lambda_with_param_types(mlc::Array<mlc::String> parameter_names, mlc::Array<std::shared_ptr<registry::Type>> parameter_types, std::shared_ptr<ast::Expr> body, ast::Span source_span, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> transform_expr_with_lambda_parameter_types(std::shared_ptr<ast::Expr> expression, mlc::Array<std::shared_ptr<registry::Type>> expected_param_types, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> semantic_expression_list_singleton(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;

mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> transform_array_hof_method_arguments(std::shared_ptr<semantic_ir::SemanticExpression> typed_object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

mlc::Array<std::shared_ptr<registry::Type>> inferred_types_from_record_literal_part_for_merge(ast::RecordLitPart literal_record_part, check_context::CheckContext inference_context_for_spread) noexcept;

transform::TransformPass transform_pass_new(transform::TransformContext transform_context) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> dispatch_transform_pass(transform::TransformPass transform_pass, std::shared_ptr<ast::Expr> expression, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_int(transform::TransformPass self, int value, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_str(transform::TransformPass self, mlc::String value, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_bool(transform::TransformPass self, bool value, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_unit(transform::TransformPass self, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_float(transform::TransformPass self, mlc::String value, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_i64(transform::TransformPass self, mlc::String value, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_u8(transform::TransformPass self, mlc::String value, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_usize(transform::TransformPass self, mlc::String value, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_char(transform::TransformPass self, mlc::String value, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_extern(transform::TransformPass self, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_ident(transform::TransformPass self, mlc::String name, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_bin(transform::TransformPass self, mlc::String operation, std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_un(transform::TransformPass self, mlc::String operation, std::shared_ptr<ast::Expr> inner_expression, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_call(transform::TransformPass self, std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_method(transform::TransformPass self, std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_field(transform::TransformPass self, std::shared_ptr<ast::Expr> object, mlc::String field_name, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_index(transform::TransformPass self, std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expression, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_if(transform::TransformPass self, std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expression, std::shared_ptr<ast::Expr> else_expression, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_block(transform::TransformPass self, mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_while(transform::TransformPass self, std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_spawn(transform::TransformPass self, mlc::Array<std::shared_ptr<ast::Stmt>> statements, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_for(transform::TransformPass self, mlc::String variable_name, std::shared_ptr<ast::Expr> iterator, mlc::Array<std::shared_ptr<ast::Stmt>> statements, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_match(transform::TransformPass self, std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_record(transform::TransformPass self, mlc::String type_name, mlc::Array<ast::RecordLitPart> lit_parts, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_record_update(transform::TransformPass self, mlc::String type_name, std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_array(transform::TransformPass self, mlc::Array<std::shared_ptr<ast::Expr>> elements, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_tuple(transform::TransformPass self, mlc::Array<std::shared_ptr<ast::Expr>> elements, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_question(transform::TransformPass self, std::shared_ptr<ast::Expr> inner_expression, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_lambda(transform::TransformPass self, mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_named_arg(transform::TransformPass self, std::shared_ptr<ast::Expr> inner_expression, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_with(transform::TransformPass self, std::shared_ptr<ast::Expr> resource, mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> statements, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_unsupported(transform::TransformPass self, std::shared_ptr<ast::Expr> expression, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> transform_expr(std::shared_ptr<ast::Expr> expression, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

mlc::String receiver_type_key_for_method_dispatch(std::shared_ptr<registry::Type> receiver_semantic_type) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*receiver_semantic_type))) { auto _v_tnamed = std::get<registry::TNamed>((*receiver_semantic_type)); auto [name] = _v_tnamed; return name; } if (std::holds_alternative<registry::TShared>((*receiver_semantic_type))) { auto _v_tshared = std::get<registry::TShared>((*receiver_semantic_type)); auto [shared_inner_type] = _v_tshared; return [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*shared_inner_type))) { auto _v_tnamed = std::get<registry::TNamed>((*shared_inner_type)); auto [inner_name] = _v_tnamed; return inner_name; } return mlc::String(""); }(); } return mlc::String(""); }();}

mlc::Array<int> direct_call_parameter_mutability_flags(registry::TypeRegistry registry, mlc::String callee_name, int positional_argument_count) noexcept{
return callee_name == mlc::String("") ? [&]() -> mlc::Array<int> { 
  mlc::Array<int> empty_mutability_pattern = {};
  return empty_mutability_pattern;
 }() : [&]() -> mlc::Array<int> { 
  mlc::Array<int> stored = registry::TypeRegistry_parameter_mutability_flags_for(registry, callee_name);
  return stored.size() != positional_argument_count ? [&]() -> mlc::Array<int> { 
  mlc::Array<int> empty_mutability_pattern = {};
  return empty_mutability_pattern;
 }() : stored;
 }();
}

mlc::Array<int> instance_method_receiver_and_parameters_mutability_pattern(registry::TypeRegistry registry, std::shared_ptr<registry::Type> receiver_semantic_type, mlc::String method_name, int method_argument_count) noexcept{
mlc::String receiver_key = receiver_type_key_for_method_dispatch(receiver_semantic_type);
return receiver_key == mlc::String("") ? [&]() -> mlc::Array<int> { 
  mlc::Array<int> empty_mutability_pattern = {};
  return empty_mutability_pattern;
 }() : [&]() -> mlc::Array<int> { 
  mlc::String mangled = receiver_key + mlc::String("_") + method_name;
  return !registry::TypeRegistry_has_fn(registry, mangled) ? [&]() -> mlc::Array<int> { 
  mlc::Array<int> empty_mutability_pattern = {};
  return empty_mutability_pattern;
 }() : [&]() -> mlc::Array<int> { 
  mlc::Array<int> stored = registry::TypeRegistry_parameter_mutability_flags_for(registry, mangled);
  return stored.size() != method_argument_count + 1 ? [&]() -> mlc::Array<int> { 
  mlc::Array<int> empty_mutability_pattern = {};
  return empty_mutability_pattern;
 }() : stored;
 }();
 }();
}

mlc::String extend_method_mangled_name(registry::TypeRegistry registry, std::shared_ptr<registry::Type> receiver_semantic_type, mlc::String method_name) noexcept{
mlc::String receiver_key = receiver_type_key_for_method_dispatch(receiver_semantic_type);
return receiver_key == mlc::String("") ? mlc::String("") : [&]() -> mlc::String { 
  mlc::String mangled = receiver_key + mlc::String("_") + method_name;
  return registry::TypeRegistry_has_fn(registry, mangled) ? mangled : mlc::String("");
 }();
}

std::shared_ptr<semantic_ir::SemanticExpression> transform_extend_method_as_call(registry::TypeRegistry registry, std::shared_ptr<semantic_ir::SemanticExpression> typed_object, mlc::String method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> typed_arguments, std::shared_ptr<registry::Type> receiver_type, std::shared_ptr<registry::Type> result_type, ast::Span source_span) noexcept{
mlc::String mangled = extend_method_mangled_name(registry, receiver_type, method_name);
mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> call_arguments = {};
call_arguments.push_back(typed_object);
int argument_index = 0;
while (argument_index < typed_arguments.size()){
{
call_arguments.push_back(typed_arguments[argument_index]);
argument_index = argument_index + 1;
}
}
std::shared_ptr<semantic_ir::SemanticExpression> callee = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionIdent(mangled, registry::TypeRegistry_fn_type(registry, mangled), source_span));
mlc::Array<int> call_parameter_mutability_flags = instance_method_receiver_and_parameters_mutability_pattern(registry, receiver_type, method_name, typed_arguments.size());
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionCall(callee, call_arguments, call_parameter_mutability_flags, result_type, source_span));
}

bool semantic_type_is_tarray(std::shared_ptr<registry::Type> type_value) noexcept{return std::visit(overloaded{
  [&](const TArray& tarray) -> bool { auto [_w0] = tarray; return true; },
  [&](const TI32& ti32) -> bool { return false; },
  [&](const TString& tstring) -> bool { return false; },
  [&](const TBool& tbool) -> bool { return false; },
  [&](const TUnit& tunit) -> bool { return false; },
  [&](const TI64& ti64) -> bool { return false; },
  [&](const TF64& tf64) -> bool { return false; },
  [&](const TU8& tu8) -> bool { return false; },
  [&](const TUsize& tusize) -> bool { return false; },
  [&](const TChar& tchar) -> bool { return false; },
  [&](const TShared& tshared) -> bool { auto [_w0] = tshared; return false; },
  [&](const TNamed& tnamed) -> bool { auto [_w0] = tnamed; return false; },
  [&](const TGeneric& tgeneric) -> bool { auto [_w0, _w1] = tgeneric; return false; },
  [&](const TPair& tpair) -> bool { auto [_w0, _w1] = tpair; return false; },
  [&](const TTuple& ttuple) -> bool { auto [_w0] = ttuple; return false; },
  [&](const TFn& tfn) -> bool { auto [_w0, _w1] = tfn; return false; },
  [&](const TAssoc& tassoc) -> bool { auto [_w0, _w1] = tassoc; return false; },
  [&](const TUnknown& tunknown) -> bool { return false; }
}, (*type_value));}

std::shared_ptr<registry::Type> array_element_type_from_semantic_type(std::shared_ptr<registry::Type> type_value) noexcept{return std::visit(overloaded{
  [&](const TArray& tarray) -> std::shared_ptr<registry::Type> { auto [inner] = tarray; return inner; },
  [&](const TI32& ti32) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TString& tstring) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TBool& tbool) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TUnit& tunit) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TI64& ti64) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TF64& tf64) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TU8& tu8) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TUsize& tusize) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TChar& tchar) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TShared& tshared) -> std::shared_ptr<registry::Type> { auto [_w0] = tshared; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TNamed& tnamed) -> std::shared_ptr<registry::Type> { auto [_w0] = tnamed; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TGeneric& tgeneric) -> std::shared_ptr<registry::Type> { auto [_w0, _w1] = tgeneric; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TPair& tpair) -> std::shared_ptr<registry::Type> { auto [_w0, _w1] = tpair; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TTuple& ttuple) -> std::shared_ptr<registry::Type> { auto [_w0] = ttuple; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TFn& tfn) -> std::shared_ptr<registry::Type> { auto [_w0, _w1] = tfn; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TAssoc& tassoc) -> std::shared_ptr<registry::Type> { auto [_w0, _w1] = tassoc; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TUnknown& tunknown) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); }
}, (*type_value));}

mlc::String generic_type_name(std::shared_ptr<registry::Type> type_value) noexcept{return std::visit(overloaded{
  [&](const TGeneric& tgeneric) -> mlc::String { auto [name, _w0] = tgeneric; return name; },
  [&](const TI32& ti32) -> mlc::String { return mlc::String(""); },
  [&](const TString& tstring) -> mlc::String { return mlc::String(""); },
  [&](const TBool& tbool) -> mlc::String { return mlc::String(""); },
  [&](const TUnit& tunit) -> mlc::String { return mlc::String(""); },
  [&](const TI64& ti64) -> mlc::String { return mlc::String(""); },
  [&](const TF64& tf64) -> mlc::String { return mlc::String(""); },
  [&](const TU8& tu8) -> mlc::String { return mlc::String(""); },
  [&](const TUsize& tusize) -> mlc::String { return mlc::String(""); },
  [&](const TChar& tchar) -> mlc::String { return mlc::String(""); },
  [&](const TShared& tshared) -> mlc::String { auto [_w0] = tshared; return mlc::String(""); },
  [&](const TNamed& tnamed) -> mlc::String { auto [_w0] = tnamed; return mlc::String(""); },
  [&](const TArray& tarray) -> mlc::String { auto [_w0] = tarray; return mlc::String(""); },
  [&](const TPair& tpair) -> mlc::String { auto [_w0, _w1] = tpair; return mlc::String(""); },
  [&](const TTuple& ttuple) -> mlc::String { auto [_w0] = ttuple; return mlc::String(""); },
  [&](const TFn& tfn) -> mlc::String { auto [_w0, _w1] = tfn; return mlc::String(""); },
  [&](const TAssoc& tassoc) -> mlc::String { auto [_w0, _w1] = tassoc; return mlc::String(""); },
  [&](const TUnknown& tunknown) -> mlc::String { return mlc::String(""); }
}, (*type_value));}

bool callee_semantic_type_is_function(std::shared_ptr<registry::Type> type_value) noexcept{return std::visit(overloaded{
  [&](const TFn& tfn) -> bool { auto [_w0, _w1] = tfn; return true; },
  [&](const TI32& ti32) -> bool { return false; },
  [&](const TString& tstring) -> bool { return false; },
  [&](const TBool& tbool) -> bool { return false; },
  [&](const TUnit& tunit) -> bool { return false; },
  [&](const TI64& ti64) -> bool { return false; },
  [&](const TF64& tf64) -> bool { return false; },
  [&](const TU8& tu8) -> bool { return false; },
  [&](const TUsize& tusize) -> bool { return false; },
  [&](const TChar& tchar) -> bool { return false; },
  [&](const TShared& tshared) -> bool { auto [_w0] = tshared; return false; },
  [&](const TNamed& tnamed) -> bool { auto [_w0] = tnamed; return false; },
  [&](const TGeneric& tgeneric) -> bool { auto [_w0, _w1] = tgeneric; return false; },
  [&](const TArray& tarray) -> bool { auto [_w0] = tarray; return false; },
  [&](const TPair& tpair) -> bool { auto [_w0, _w1] = tpair; return false; },
  [&](const TTuple& ttuple) -> bool { auto [_w0] = ttuple; return false; },
  [&](const TAssoc& tassoc) -> bool { auto [_w0, _w1] = tassoc; return false; },
  [&](const TUnknown& tunknown) -> bool { return false; }
}, (*type_value));}

mlc::Array<std::shared_ptr<registry::Type>> function_parameter_types_from_callee_type(std::shared_ptr<registry::Type> type_value) noexcept{return std::visit(overloaded{
  [&](const TFn& tfn) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [parameter_types, _w0] = tfn; return parameter_types; },
  [&](const TI32& ti32) -> mlc::Array<std::shared_ptr<registry::Type>> { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TString& tstring) -> mlc::Array<std::shared_ptr<registry::Type>> { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TBool& tbool) -> mlc::Array<std::shared_ptr<registry::Type>> { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TUnit& tunit) -> mlc::Array<std::shared_ptr<registry::Type>> { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TI64& ti64) -> mlc::Array<std::shared_ptr<registry::Type>> { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TF64& tf64) -> mlc::Array<std::shared_ptr<registry::Type>> { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TU8& tu8) -> mlc::Array<std::shared_ptr<registry::Type>> { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TUsize& tusize) -> mlc::Array<std::shared_ptr<registry::Type>> { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TChar& tchar) -> mlc::Array<std::shared_ptr<registry::Type>> { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TShared& tshared) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0] = tshared; return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TNamed& tnamed) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0] = tnamed; return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TGeneric& tgeneric) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0, _w1] = tgeneric; return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TArray& tarray) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0] = tarray; return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TPair& tpair) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0, _w1] = tpair; return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TTuple& ttuple) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0] = ttuple; return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TAssoc& tassoc) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0, _w1] = tassoc; return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TUnknown& tunknown) -> mlc::Array<std::shared_ptr<registry::Type>> { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); }
}, (*type_value));}

mlc::String call_callee_ident_name(std::shared_ptr<ast::Expr> function) noexcept{return std::visit(overloaded{
  [&](const ExprIdent& exprident) -> mlc::String { auto [name, _w0] = exprident; return name; },
  [&](const ExprInt& exprint) -> mlc::String { auto [_w0, _w1] = exprint; return mlc::String(""); },
  [&](const ExprStr& exprstr) -> mlc::String { auto [_w0, _w1] = exprstr; return mlc::String(""); },
  [&](const ExprBool& exprbool) -> mlc::String { auto [_w0, _w1] = exprbool; return mlc::String(""); },
  [&](const ExprUnit& exprunit) -> mlc::String { auto [_w0] = exprunit; return mlc::String(""); },
  [&](const ExprFloat& exprfloat) -> mlc::String { auto [_w0, _w1] = exprfloat; return mlc::String(""); },
  [&](const ExprI64& expri64) -> mlc::String { auto [_w0, _w1] = expri64; return mlc::String(""); },
  [&](const ExprU8& expru8) -> mlc::String { auto [_w0, _w1] = expru8; return mlc::String(""); },
  [&](const ExprUsize& exprusize) -> mlc::String { auto [_w0, _w1] = exprusize; return mlc::String(""); },
  [&](const ExprChar& exprchar) -> mlc::String { auto [_w0, _w1] = exprchar; return mlc::String(""); },
  [&](const ExprBin& exprbin) -> mlc::String { auto [_w0, _w1, _w2, _w3] = exprbin; return mlc::String(""); },
  [&](const ExprUn& exprun) -> mlc::String { auto [_w0, _w1, _w2] = exprun; return mlc::String(""); },
  [&](const ExprCall& exprcall) -> mlc::String { auto [_w0, _w1, _w2] = exprcall; return mlc::String(""); },
  [&](const ExprMethod& exprmethod) -> mlc::String { auto [_w0, _w1, _w2, _w3] = exprmethod; return mlc::String(""); },
  [&](const ExprField& exprfield) -> mlc::String { auto [_w0, _w1, _w2] = exprfield; return mlc::String(""); },
  [&](const ExprIndex& exprindex) -> mlc::String { auto [_w0, _w1, _w2] = exprindex; return mlc::String(""); },
  [&](const ExprIf& exprif) -> mlc::String { auto [_w0, _w1, _w2, _w3] = exprif; return mlc::String(""); },
  [&](const ExprBlock& exprblock) -> mlc::String { auto [_w0, _w1, _w2] = exprblock; return mlc::String(""); },
  [&](const ExprWhile& exprwhile) -> mlc::String { auto [_w0, _w1, _w2] = exprwhile; return mlc::String(""); },
  [&](const ExprFor& exprfor) -> mlc::String { auto [_w0, _w1, _w2, _w3] = exprfor; return mlc::String(""); },
  [&](const ExprMatch& exprmatch) -> mlc::String { auto [_w0, _w1, _w2] = exprmatch; return mlc::String(""); },
  [&](const ExprRecord& exprrecord) -> mlc::String { auto [_w0, _w1, _w2] = exprrecord; return mlc::String(""); },
  [&](const ExprRecordUpdate& exprrecordupdate) -> mlc::String { auto [_w0, _w1, _w2, _w3] = exprrecordupdate; return mlc::String(""); },
  [&](const ExprArray& exprarray) -> mlc::String { auto [_w0, _w1] = exprarray; return mlc::String(""); },
  [&](const ExprTuple& exprtuple) -> mlc::String { auto [_w0, _w1] = exprtuple; return mlc::String(""); },
  [&](const ExprQuestion& exprquestion) -> mlc::String { auto [_w0, _w1] = exprquestion; return mlc::String(""); },
  [&](const ExprExtern& exprextern) -> mlc::String { auto [_w0] = exprextern; return mlc::String(""); },
  [&](const ExprLambda& exprlambda) -> mlc::String { auto [_w0, _w1, _w2] = exprlambda; return mlc::String(""); },
  [&](const ExprSpawn& exprspawn) -> mlc::String { auto [_w0, _w1] = exprspawn; return mlc::String(""); },
  [&](const ExprNamedArg& exprnamedarg) -> mlc::String { auto [_w0, _w1, _w2] = exprnamedarg; return mlc::String(""); },
  [&](const ExprWith& exprwith) -> mlc::String { auto [_w0, _w1, _w2, _w3] = exprwith; return mlc::String(""); }
}, (*function)._);}

bool call_argument_is_lambda(std::shared_ptr<ast::Expr> argument_expression) noexcept{return std::visit(overloaded{
  [&](const ExprLambda& exprlambda) -> bool { auto [_w0, _w1, _w2] = exprlambda; return true; },
  [&](const ExprInt& exprint) -> bool { auto [_w0, _w1] = exprint; return false; },
  [&](const ExprStr& exprstr) -> bool { auto [_w0, _w1] = exprstr; return false; },
  [&](const ExprBool& exprbool) -> bool { auto [_w0, _w1] = exprbool; return false; },
  [&](const ExprUnit& exprunit) -> bool { auto [_w0] = exprunit; return false; },
  [&](const ExprFloat& exprfloat) -> bool { auto [_w0, _w1] = exprfloat; return false; },
  [&](const ExprI64& expri64) -> bool { auto [_w0, _w1] = expri64; return false; },
  [&](const ExprU8& expru8) -> bool { auto [_w0, _w1] = expru8; return false; },
  [&](const ExprUsize& exprusize) -> bool { auto [_w0, _w1] = exprusize; return false; },
  [&](const ExprChar& exprchar) -> bool { auto [_w0, _w1] = exprchar; return false; },
  [&](const ExprIdent& exprident) -> bool { auto [_w0, _w1] = exprident; return false; },
  [&](const ExprBin& exprbin) -> bool { auto [_w0, _w1, _w2, _w3] = exprbin; return false; },
  [&](const ExprUn& exprun) -> bool { auto [_w0, _w1, _w2] = exprun; return false; },
  [&](const ExprCall& exprcall) -> bool { auto [_w0, _w1, _w2] = exprcall; return false; },
  [&](const ExprMethod& exprmethod) -> bool { auto [_w0, _w1, _w2, _w3] = exprmethod; return false; },
  [&](const ExprField& exprfield) -> bool { auto [_w0, _w1, _w2] = exprfield; return false; },
  [&](const ExprIndex& exprindex) -> bool { auto [_w0, _w1, _w2] = exprindex; return false; },
  [&](const ExprIf& exprif) -> bool { auto [_w0, _w1, _w2, _w3] = exprif; return false; },
  [&](const ExprBlock& exprblock) -> bool { auto [_w0, _w1, _w2] = exprblock; return false; },
  [&](const ExprWhile& exprwhile) -> bool { auto [_w0, _w1, _w2] = exprwhile; return false; },
  [&](const ExprFor& exprfor) -> bool { auto [_w0, _w1, _w2, _w3] = exprfor; return false; },
  [&](const ExprMatch& exprmatch) -> bool { auto [_w0, _w1, _w2] = exprmatch; return false; },
  [&](const ExprRecord& exprrecord) -> bool { auto [_w0, _w1, _w2] = exprrecord; return false; },
  [&](const ExprRecordUpdate& exprrecordupdate) -> bool { auto [_w0, _w1, _w2, _w3] = exprrecordupdate; return false; },
  [&](const ExprArray& exprarray) -> bool { auto [_w0, _w1] = exprarray; return false; },
  [&](const ExprTuple& exprtuple) -> bool { auto [_w0, _w1] = exprtuple; return false; },
  [&](const ExprQuestion& exprquestion) -> bool { auto [_w0, _w1] = exprquestion; return false; },
  [&](const ExprExtern& exprextern) -> bool { auto [_w0] = exprextern; return false; },
  [&](const ExprSpawn& exprspawn) -> bool { auto [_w0, _w1] = exprspawn; return false; },
  [&](const ExprNamedArg& exprnamedarg) -> bool { auto [_w0, _w1, _w2] = exprnamedarg; return false; },
  [&](const ExprWith& exprwith) -> bool { auto [_w0, _w1, _w2, _w3] = exprwith; return false; }
}, (*argument_expression)._);}

std::shared_ptr<registry::Type> binary_result_type_for_operator(registry::TypeRegistry registry, std::shared_ptr<registry::Type> left_type, mlc::String operation, mlc::String method) noexcept{
std::shared_ptr<registry::Type> from_registry = registry::method_return_type_from_object(left_type, method, registry);
return semantic_type_structure::type_is_unknown(from_registry) ? semantic_type_structure::binary_operation_result_type(operation, left_type) : from_registry;
}

std::shared_ptr<registry::Type> method_result_type_for_dispatch(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name, registry::TypeRegistry registry) noexcept{
std::shared_ptr<registry::Type> builtin_type = semantic_type_structure::builtin_method_return_type(method_name);
return semantic_type_structure::type_is_unknown(builtin_type) ? registry::method_return_type_from_object(receiver_type, method_name, registry) : builtin_type;
}

std::shared_ptr<registry::Type> merge_conditional_expression_types(std::shared_ptr<semantic_ir::SemanticExpression> typed_then, std::shared_ptr<semantic_ir::SemanticExpression> typed_else) noexcept{
std::shared_ptr<registry::Type> then_type = semantic_ir::sexpr_type(typed_then);
std::shared_ptr<registry::Type> else_type = semantic_ir::sexpr_type(typed_else);
return semantic_type_is_tarray(then_type) ? semantic_type_is_tarray(else_type) ? [&]() -> std::shared_ptr<registry::Type> { 
  std::shared_ptr<registry::Type> then_element_type = array_element_type_from_semantic_type(then_type);
  std::shared_ptr<registry::Type> else_element_type = array_element_type_from_semantic_type(else_type);
  return semantic_type_structure::type_is_unknown(then_element_type) ? std::make_shared<registry::Type>(registry::TArray(else_element_type)) : std::make_shared<registry::Type>(registry::TArray(then_element_type));
 }() : then_type : then_type;
}

std::shared_ptr<registry::Type> array_element_type_from_semantic_expression(std::shared_ptr<semantic_ir::SemanticExpression> typed_expression) noexcept{
std::shared_ptr<registry::Type> expression_type = semantic_ir::sexpr_type(typed_expression);
return semantic_type_is_tarray(expression_type) ? array_element_type_from_semantic_type(expression_type) : std::make_shared<registry::Type>((registry::TUnknown{}));
}

mlc::Array<std::shared_ptr<registry::Type>> type_arguments_from_generic_type(std::shared_ptr<registry::Type> type_value) noexcept{return std::visit(overloaded{
  [&](const TGeneric& tgeneric) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0, type_arguments] = tgeneric; return type_arguments; },
  [&](const TI32& ti32) -> mlc::Array<std::shared_ptr<registry::Type>> { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TString& tstring) -> mlc::Array<std::shared_ptr<registry::Type>> { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TBool& tbool) -> mlc::Array<std::shared_ptr<registry::Type>> { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TUnit& tunit) -> mlc::Array<std::shared_ptr<registry::Type>> { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TI64& ti64) -> mlc::Array<std::shared_ptr<registry::Type>> { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TF64& tf64) -> mlc::Array<std::shared_ptr<registry::Type>> { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TU8& tu8) -> mlc::Array<std::shared_ptr<registry::Type>> { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TUsize& tusize) -> mlc::Array<std::shared_ptr<registry::Type>> { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TChar& tchar) -> mlc::Array<std::shared_ptr<registry::Type>> { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TShared& tshared) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0] = tshared; return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TNamed& tnamed) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0] = tnamed; return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TArray& tarray) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0] = tarray; return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TPair& tpair) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0, _w1] = tpair; return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TTuple& ttuple) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0] = ttuple; return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TFn& tfn) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0, _w1] = tfn; return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TAssoc& tassoc) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0, _w1] = tassoc; return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); },
  [&](const TUnknown& tunknown) -> mlc::Array<std::shared_ptr<registry::Type>> { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty = {};
  return empty;
 }(); }
}, (*type_value));}

std::shared_ptr<registry::Type> question_unwrapped_type_from_inner(std::shared_ptr<registry::Type> inner_expression_type) noexcept{
mlc::Array<std::shared_ptr<registry::Type>> type_arguments = type_arguments_from_generic_type(inner_expression_type);
return type_arguments.size() > 0 ? type_arguments[0] : std::make_shared<registry::Type>((registry::TUnknown{}));
}

std::shared_ptr<semantic_ir::SemanticExpression> transform_result_option_hof_method_call(registry::TypeRegistry registry, std::shared_ptr<semantic_ir::SemanticExpression> typed_object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, std::shared_ptr<registry::Type> receiver_type, transform::TransformContext transform_context, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> typed_arguments = transform_exprs(method_arguments, transform_context, stmts_fn);
mlc::Array<std::shared_ptr<registry::Type>> argument_types = typed_arguments.map([](std::shared_ptr<semantic_ir::SemanticExpression> typed_argument) mutable { return semantic_ir::sexpr_type(typed_argument); });
std::shared_ptr<registry::Type> result_type = hof_method_spec::hof_call_result_type(receiver_type, method_name, argument_types);
mlc::Array<int> method_parameter_mutability_flags = instance_method_receiver_and_parameters_mutability_pattern(registry, receiver_type, method_name, typed_arguments.size());
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionMethod(typed_object, method_name, typed_arguments, method_parameter_mutability_flags, result_type, source_span));
}

std::shared_ptr<semantic_ir::SemanticExpression> transform_array_hof_method_call(registry::TypeRegistry registry, std::shared_ptr<semantic_ir::SemanticExpression> typed_object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, std::shared_ptr<registry::Type> receiver_type, transform::TransformContext transform_context, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> typed_arguments = transform_array_hof_method_arguments(typed_object, method_name, method_arguments, transform_context, stmts_fn);
mlc::Array<std::shared_ptr<registry::Type>> argument_types = typed_arguments.map([](std::shared_ptr<semantic_ir::SemanticExpression> typed_argument) mutable { return semantic_ir::sexpr_type(typed_argument); });
std::shared_ptr<registry::Type> result_type = hof_method_spec::hof_call_result_type(receiver_type, method_name, argument_types);
mlc::Array<int> method_parameter_mutability_flags = instance_method_receiver_and_parameters_mutability_pattern(registry, receiver_type, method_name, typed_arguments.size());
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionMethod(typed_object, method_name, typed_arguments, method_parameter_mutability_flags, result_type, source_span));
}

std::shared_ptr<semantic_ir::SemanticExpression> transform_regular_method_call(registry::TypeRegistry registry, std::shared_ptr<semantic_ir::SemanticExpression> typed_object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, std::shared_ptr<registry::Type> receiver_type, transform::TransformContext transform_context, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> typed_arguments = transform_exprs(method_arguments, transform_context, stmts_fn);
std::shared_ptr<registry::Type> result_type = method_result_type_for_dispatch(receiver_type, method_name, registry);
mlc::String mangled = extend_method_mangled_name(registry, receiver_type, method_name);
return mangled != mlc::String("") ? transform_extend_method_as_call(registry, typed_object, method_name, typed_arguments, receiver_type, result_type, source_span) : [&]() -> std::shared_ptr<semantic_ir::SemanticExpression> { 
  mlc::Array<int> method_parameter_mutability_flags = instance_method_receiver_and_parameters_mutability_pattern(registry, receiver_type, method_name, typed_arguments.size());
  return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionMethod(typed_object, method_name, typed_arguments, method_parameter_mutability_flags, result_type, source_span));
 }();
}

std::shared_ptr<semantic_ir::SemanticExpression> coerce_unknown_else_array_when_then_known_inner(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, ast::Span span_else, std::shared_ptr<registry::Type> then_element_type, std::shared_ptr<semantic_ir::SemanticExpression> typed_else) noexcept{
std::shared_ptr<registry::Type> else_type = semantic_ir::sexpr_type(typed_else);
return semantic_type_is_tarray(else_type) && semantic_type_structure::type_is_unknown(array_element_type_from_semantic_type(else_type)) ? std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionArray(elements, std::make_shared<registry::Type>(registry::TArray(then_element_type)), span_else)) : typed_else;
}

std::shared_ptr<semantic_ir::SemanticExpression> conditional_else_coerce_empty_array_using_then_type(std::shared_ptr<semantic_ir::SemanticExpression> typed_then, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, ast::Span span_else, std::shared_ptr<semantic_ir::SemanticExpression> typed_else) noexcept{
std::shared_ptr<registry::Type> then_type = semantic_ir::sexpr_type(typed_then);
return semantic_type_is_tarray(then_type) ? coerce_unknown_else_array_when_then_known_inner(elements, span_else, array_element_type_from_semantic_type(then_type), typed_else) : typed_else;
}

std::shared_ptr<semantic_ir::SemanticExpression> array_literal_else_maybe_coerce(std::shared_ptr<semantic_ir::SemanticExpression> typed_then, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, ast::Span span_else, std::shared_ptr<semantic_ir::SemanticExpression> typed_else) noexcept{return elements.size() > 0 ? typed_else : conditional_else_coerce_empty_array_using_then_type(typed_then, elements, span_else, typed_else);}

std::shared_ptr<semantic_ir::SemanticExpression> conditional_else_empty_unknown_array_coerced_to_then_array_element(std::shared_ptr<semantic_ir::SemanticExpression> typed_then, std::shared_ptr<semantic_ir::SemanticExpression> typed_else) noexcept{return std::visit(overloaded{
  [&](const SemanticExpressionArray& semanticexpressionarray) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [elements, _w0, span_else] = semanticexpressionarray; return array_literal_else_maybe_coerce(typed_then, elements, span_else, typed_else); },
  [&](const SemanticExpressionInt& semanticexpressionint) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = semanticexpressionint; return typed_else; },
  [&](const SemanticExpressionStr& semanticexpressionstr) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = semanticexpressionstr; return typed_else; },
  [&](const SemanticExpressionFloat& semanticexpressionfloat) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = semanticexpressionfloat; return typed_else; },
  [&](const SemanticExpressionI64& semanticexpressioni64) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = semanticexpressioni64; return typed_else; },
  [&](const SemanticExpressionU8& semanticexpressionu8) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = semanticexpressionu8; return typed_else; },
  [&](const SemanticExpressionUsize& semanticexpressionusize) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = semanticexpressionusize; return typed_else; },
  [&](const SemanticExpressionChar& semanticexpressionchar) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = semanticexpressionchar; return typed_else; },
  [&](const SemanticExpressionBool& semanticexpressionbool) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = semanticexpressionbool; return typed_else; },
  [&](const SemanticExpressionUnit& semanticexpressionunit) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1] = semanticexpressionunit; return typed_else; },
  [&](const SemanticExpressionExtern& semanticexpressionextern) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1] = semanticexpressionextern; return typed_else; },
  [&](const SemanticExpressionIdent& semanticexpressionident) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = semanticexpressionident; return typed_else; },
  [&](const SemanticExpressionBin& semanticexpressionbin) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionbin; return typed_else; },
  [&](const SemanticExpressionUn& semanticexpressionun) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3] = semanticexpressionun; return typed_else; },
  [&](const SemanticExpressionCall& semanticexpressioncall) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressioncall; return typed_else; },
  [&](const SemanticExpressionMethod& semanticexpressionmethod) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3, _w4, _w5] = semanticexpressionmethod; return typed_else; },
  [&](const SemanticExpressionField& semanticexpressionfield) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3] = semanticexpressionfield; return typed_else; },
  [&](const SemanticExpressionIndex& semanticexpressionindex) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3] = semanticexpressionindex; return typed_else; },
  [&](const SemanticExpressionIf& semanticexpressionif) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionif; return typed_else; },
  [&](const SemanticExpressionBlock& semanticexpressionblock) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3] = semanticexpressionblock; return typed_else; },
  [&](const SemanticExpressionWhile& semanticexpressionwhile) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3] = semanticexpressionwhile; return typed_else; },
  [&](const SemanticExpressionFor& semanticexpressionfor) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionfor; return typed_else; },
  [&](const SemanticExpressionMatch& semanticexpressionmatch) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3] = semanticexpressionmatch; return typed_else; },
  [&](const SemanticExpressionRecord& semanticexpressionrecord) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3] = semanticexpressionrecord; return typed_else; },
  [&](const SemanticExpressionRecordUpdate& semanticexpressionrecordupdate) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionrecordupdate; return typed_else; },
  [&](const SemanticExpressionTuple& semanticexpressiontuple) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = semanticexpressiontuple; return typed_else; },
  [&](const SemanticExpressionQuestion& semanticexpressionquestion) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = semanticexpressionquestion; return typed_else; },
  [&](const SemanticExpressionLambda& semanticexpressionlambda) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3] = semanticexpressionlambda; return typed_else; },
  [&](const SemanticExpressionWith& semanticexpressionwith) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionwith; return typed_else; }
}, (*typed_else)._);}

transform::TransformContext transform_context_new(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, registry::TypeRegistry registry) noexcept{return transform::TransformContext{type_env, registry, {}};}

transform::TransformContext empty_transform_context() noexcept{return transform::TransformContext{mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>(), registry::empty_registry(), {}};}

transform::TransformContext transform_context_with_env(transform::TransformContext base, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env) noexcept{return transform::TransformContext{type_env, base.registry, base.lambda_parameter_types};}

transform::TransformContext transform_context_with_lambda_parameter_types(transform::TransformContext base, mlc::Array<std::shared_ptr<registry::Type>> lambda_parameter_types) noexcept{return transform::TransformContext{base.type_env, base.registry, lambda_parameter_types};}

std::shared_ptr<semantic_ir::SemanticExpression> coerce_array_semantic_expression_to_type(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, ast::Span source_span, std::shared_ptr<registry::Type> target_type, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{return elements.size() == 0 ? std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionArray(elements, target_type, source_span)) : expression;}

std::shared_ptr<semantic_ir::SemanticExpression> coerce_record_semantic_expression_to_type(mlc::String type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> fields, std::shared_ptr<registry::Type> expr_type, ast::Span source_span, std::shared_ptr<registry::Type> target_type, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
mlc::String target_generic_name = generic_type_name(target_type);
mlc::String expression_generic_name = generic_type_name(expr_type);
return target_generic_name != mlc::String("") && target_generic_name == expression_generic_name ? std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionRecord(type_name, fields, target_type, source_span)) : expression;
}

std::shared_ptr<semantic_ir::SemanticExpression> coerce_block_semantic_expression_to_type(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, ast::Span span, std::shared_ptr<registry::Type> target_type, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
return semantic_type_is_tarray(target_type) ? [&]() -> std::shared_ptr<semantic_ir::SemanticExpression> { 
  std::shared_ptr<semantic_ir::SemanticExpression> coerced_result = coerce_expr_to_type(result_expression, target_type);
  return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBlock(statements, coerced_result, semantic_ir::sexpr_type(coerced_result), span));
 }() : expression;
}

std::shared_ptr<semantic_ir::SemanticExpression> coerce_if_semantic_expression_to_type(std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_branch, std::shared_ptr<semantic_ir::SemanticExpression> else_branch, ast::Span source_span, std::shared_ptr<registry::Type> target_type, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
return semantic_type_is_tarray(target_type) ? std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionIf(condition, coerce_expr_to_type(then_branch, target_type), coerce_expr_to_type(else_branch, target_type), target_type, source_span)) : expression;
}

std::shared_ptr<semantic_ir::SemanticExpression> coerce_expr_to_type(std::shared_ptr<semantic_ir::SemanticExpression> expression, std::shared_ptr<registry::Type> target_type) noexcept{return std::visit(overloaded{
  [&](const SemanticExpressionArray& semanticexpressionarray) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [elements, _w0, source_span] = semanticexpressionarray; return coerce_array_semantic_expression_to_type(elements, source_span, target_type, expression); },
  [&](const SemanticExpressionRecord& semanticexpressionrecord) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [type_name, fields, expr_type, source_span] = semanticexpressionrecord; return coerce_record_semantic_expression_to_type(type_name, fields, expr_type, source_span, target_type, expression); },
  [&](const SemanticExpressionBlock& semanticexpressionblock) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [statements, result_expression, _w0, span] = semanticexpressionblock; return coerce_block_semantic_expression_to_type(statements, result_expression, span, target_type, expression); },
  [&](const SemanticExpressionIf& semanticexpressionif) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [condition, then_branch, else_branch, _w0, source_span] = semanticexpressionif; return coerce_if_semantic_expression_to_type(condition, then_branch, else_branch, source_span, target_type, expression); },
  [&](const SemanticExpressionInt& semanticexpressionint) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = semanticexpressionint; return expression; },
  [&](const SemanticExpressionStr& semanticexpressionstr) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = semanticexpressionstr; return expression; },
  [&](const SemanticExpressionFloat& semanticexpressionfloat) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = semanticexpressionfloat; return expression; },
  [&](const SemanticExpressionI64& semanticexpressioni64) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = semanticexpressioni64; return expression; },
  [&](const SemanticExpressionU8& semanticexpressionu8) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = semanticexpressionu8; return expression; },
  [&](const SemanticExpressionUsize& semanticexpressionusize) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = semanticexpressionusize; return expression; },
  [&](const SemanticExpressionChar& semanticexpressionchar) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = semanticexpressionchar; return expression; },
  [&](const SemanticExpressionBool& semanticexpressionbool) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = semanticexpressionbool; return expression; },
  [&](const SemanticExpressionUnit& semanticexpressionunit) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1] = semanticexpressionunit; return expression; },
  [&](const SemanticExpressionExtern& semanticexpressionextern) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1] = semanticexpressionextern; return expression; },
  [&](const SemanticExpressionIdent& semanticexpressionident) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = semanticexpressionident; return expression; },
  [&](const SemanticExpressionBin& semanticexpressionbin) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionbin; return expression; },
  [&](const SemanticExpressionUn& semanticexpressionun) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3] = semanticexpressionun; return expression; },
  [&](const SemanticExpressionCall& semanticexpressioncall) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressioncall; return expression; },
  [&](const SemanticExpressionMethod& semanticexpressionmethod) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3, _w4, _w5] = semanticexpressionmethod; return expression; },
  [&](const SemanticExpressionField& semanticexpressionfield) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3] = semanticexpressionfield; return expression; },
  [&](const SemanticExpressionIndex& semanticexpressionindex) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3] = semanticexpressionindex; return expression; },
  [&](const SemanticExpressionWhile& semanticexpressionwhile) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3] = semanticexpressionwhile; return expression; },
  [&](const SemanticExpressionFor& semanticexpressionfor) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionfor; return expression; },
  [&](const SemanticExpressionMatch& semanticexpressionmatch) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3] = semanticexpressionmatch; return expression; },
  [&](const SemanticExpressionRecordUpdate& semanticexpressionrecordupdate) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionrecordupdate; return expression; },
  [&](const SemanticExpressionTuple& semanticexpressiontuple) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = semanticexpressiontuple; return expression; },
  [&](const SemanticExpressionQuestion& semanticexpressionquestion) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = semanticexpressionquestion; return expression; },
  [&](const SemanticExpressionLambda& semanticexpressionlambda) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3] = semanticexpressionlambda; return expression; },
  [&](const SemanticExpressionWith& semanticexpressionwith) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3, _w4] = semanticexpressionwith; return expression; }
}, (*expression)._);}

std::shared_ptr<registry::Type> standalone_unknown_cell() noexcept{return std::make_shared<registry::Type>((registry::TUnknown{}));}

mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> transform_exprs(mlc::Array<std::shared_ptr<ast::Expr>> expressions, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{return expressions.map([transform_context, stmts_fn](std::shared_ptr<ast::Expr> expression_under_transform) mutable { return transform_expr(expression_under_transform, transform_context, stmts_fn); });}

std::shared_ptr<registry::Type> expected_call_argument_type_at_index(int argument_index, mlc::Array<std::shared_ptr<registry::Type>> expected_formal_parameter_types) noexcept{return argument_index < expected_formal_parameter_types.size() ? expected_formal_parameter_types[argument_index] : standalone_unknown_cell();}

std::shared_ptr<semantic_ir::SemanticExpression> transform_lambda_call_argument(mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> lambda_body, ast::Span lambda_span, std::shared_ptr<registry::Type> expected_formal_parameter_type, std::shared_ptr<ast::Expr> argument_expression_shared, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
return callee_semantic_type_is_function(expected_formal_parameter_type) ? transform_expr_lambda_with_param_types(parameter_names, function_parameter_types_from_callee_type(expected_formal_parameter_type), lambda_body, lambda_span, transform_context, stmts_fn) : transform_expr(argument_expression_shared, transform_context, stmts_fn);
}

std::shared_ptr<semantic_ir::SemanticExpression> transform_one_call_argument_using_optional_expected_type(std::shared_ptr<ast::Expr> argument_expression_shared, std::shared_ptr<registry::Type> expected_formal_parameter_type, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
std::shared_ptr<ast::Expr> argument_partially_desugared_shared_expression = partial_application_desugar::partial_application_desugar_expr(argument_expression_shared);
return call_argument_is_lambda(argument_partially_desugared_shared_expression) ? std::visit(overloaded{
  [&](const ExprLambda& exprlambda) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [parameter_names, lambda_body, lambda_span] = exprlambda; return transform_lambda_call_argument(parameter_names, lambda_body, lambda_span, expected_formal_parameter_type, argument_expression_shared, transform_context, stmts_fn); },
  [&](const ExprInt& exprint) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1] = exprint; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
  [&](const ExprStr& exprstr) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1] = exprstr; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
  [&](const ExprBool& exprbool) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1] = exprbool; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
  [&](const ExprUnit& exprunit) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0] = exprunit; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
  [&](const ExprFloat& exprfloat) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1] = exprfloat; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
  [&](const ExprI64& expri64) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1] = expri64; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
  [&](const ExprU8& expru8) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1] = expru8; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
  [&](const ExprUsize& exprusize) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1] = exprusize; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
  [&](const ExprChar& exprchar) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1] = exprchar; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
  [&](const ExprIdent& exprident) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1] = exprident; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
  [&](const ExprBin& exprbin) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3] = exprbin; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
  [&](const ExprUn& exprun) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = exprun; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
  [&](const ExprCall& exprcall) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = exprcall; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
  [&](const ExprMethod& exprmethod) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3] = exprmethod; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
  [&](const ExprField& exprfield) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = exprfield; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
  [&](const ExprIndex& exprindex) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = exprindex; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
  [&](const ExprIf& exprif) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3] = exprif; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
  [&](const ExprBlock& exprblock) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = exprblock; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
  [&](const ExprWhile& exprwhile) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = exprwhile; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
  [&](const ExprFor& exprfor) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3] = exprfor; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
  [&](const ExprMatch& exprmatch) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = exprmatch; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
  [&](const ExprRecord& exprrecord) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = exprrecord; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
  [&](const ExprRecordUpdate& exprrecordupdate) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3] = exprrecordupdate; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
  [&](const ExprArray& exprarray) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1] = exprarray; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
  [&](const ExprTuple& exprtuple) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1] = exprtuple; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
  [&](const ExprQuestion& exprquestion) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1] = exprquestion; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
  [&](const ExprExtern& exprextern) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0] = exprextern; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
  [&](const ExprSpawn& exprspawn) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1] = exprspawn; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
  [&](const ExprNamedArg& exprnamedarg) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2] = exprnamedarg; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); },
  [&](const ExprWith& exprwith) -> std::shared_ptr<semantic_ir::SemanticExpression> { auto [_w0, _w1, _w2, _w3] = exprwith; return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); }
}, (*argument_partially_desugared_shared_expression)._) : coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type);
}

std::shared_ptr<registry::Type> function_return_type_from_callee_type(std::shared_ptr<registry::Type> type_value) noexcept{return std::visit(overloaded{
  [&](const TFn& tfn) -> std::shared_ptr<registry::Type> { auto [_w0, return_type] = tfn; return return_type; },
  [&](const TI32& ti32) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TString& tstring) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TBool& tbool) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TUnit& tunit) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TI64& ti64) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TF64& tf64) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TU8& tu8) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TUsize& tusize) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TChar& tchar) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TShared& tshared) -> std::shared_ptr<registry::Type> { auto [_w0] = tshared; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TNamed& tnamed) -> std::shared_ptr<registry::Type> { auto [_w0] = tnamed; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TGeneric& tgeneric) -> std::shared_ptr<registry::Type> { auto [_w0, _w1] = tgeneric; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TArray& tarray) -> std::shared_ptr<registry::Type> { auto [_w0] = tarray; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TPair& tpair) -> std::shared_ptr<registry::Type> { auto [_w0, _w1] = tpair; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TTuple& ttuple) -> std::shared_ptr<registry::Type> { auto [_w0] = ttuple; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TAssoc& tassoc) -> std::shared_ptr<registry::Type> { auto [_w0, _w1] = tassoc; return std::make_shared<registry::Type>((registry::TUnknown{})); },
  [&](const TUnknown& tunknown) -> std::shared_ptr<registry::Type> { return std::make_shared<registry::Type>((registry::TUnknown{})); }
}, (*type_value));}

transform::Transform_call_arguments_fold_state transform_call_arguments_fold_step(transform::Transform_call_arguments_fold_state state, std::shared_ptr<ast::Expr> argument_expression, mlc::Array<std::shared_ptr<registry::Type>> expected_formal_parameter_types, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{return transform::Transform_call_arguments_fold_state{state.transformed_arguments.concat(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>>{transform_one_call_argument_using_optional_expected_type(argument_expression, expected_call_argument_type_at_index(state.next_argument_index, expected_formal_parameter_types), transform_context, stmts_fn)}), state.next_argument_index + 1};}

mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> transform_call_arguments_using_callee_semantic_type(std::shared_ptr<semantic_ir::SemanticExpression> callee_semantic_expression, mlc::Array<std::shared_ptr<ast::Expr>> argument_expressions_under_call, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
std::shared_ptr<registry::Type> callee_type = semantic_ir::sexpr_type(callee_semantic_expression);
return callee_semantic_type_is_function(callee_type) ? argument_expressions_under_call.fold(transform::Transform_call_arguments_fold_state{{}, 0}, [callee_type, transform_context, stmts_fn](transform::Transform_call_arguments_fold_state state, std::shared_ptr<ast::Expr> argument_expression) mutable { return transform_call_arguments_fold_step(state, argument_expression, function_parameter_types_from_callee_type(callee_type), transform_context, stmts_fn); }).transformed_arguments : transform_exprs(argument_expressions_under_call, transform_context, stmts_fn);
}

std::shared_ptr<semantic_ir::SemanticFieldVal> semantic_field_val_from_source_field_after_transform(std::shared_ptr<ast::FieldVal> field_value_under_transform, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{return std::make_shared<semantic_ir::SemanticFieldVal>(semantic_ir::SemanticFieldVal{field_value_under_transform->name, transform_expr(field_value_under_transform->value, transform_context, stmts_fn)});}

mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> transform_field_values(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{return field_values.map([transform_context, stmts_fn](std::shared_ptr<ast::FieldVal> field_value_under_transform) mutable { return semantic_field_val_from_source_field_after_transform(field_value_under_transform, transform_context, stmts_fn); });}

std::shared_ptr<semantic_ir::SemanticMatchArm> transform_single_match_arm(std::shared_ptr<ast::MatchArm> match_arm, transform::TransformContext transform_context, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution, std::shared_ptr<registry::Type> scrutinee_type, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> arm_environment_after_pattern = pattern_env::env_for_pattern_substituted(transform_context.type_env, match_arm->pattern, transform_context.registry, substitution, scrutinee_type);
transform::TransformContext arm_transform_context = transform_context_with_env(transform_context, arm_environment_after_pattern);
std::shared_ptr<semantic_ir::SemanticExpression> typed_body_expression = transform_expr(match_arm->body, arm_transform_context, stmts_fn);
std::shared_ptr<semantic_ir::SemanticExpression> typed_guard_expression = match_arm->has_guard ? transform_expr(match_arm->when_condition, arm_transform_context, stmts_fn) : std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBool(true, std::make_shared<registry::Type>((registry::TBool{})), ast::pattern_span(match_arm->pattern)));
return std::make_shared<semantic_ir::SemanticMatchArm>(semantic_ir::SemanticMatchArm{match_arm->pattern, match_arm->has_guard, typed_guard_expression, typed_body_expression});
}

mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> transform_match_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms, transform::TransformContext transform_context, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution, std::shared_ptr<registry::Type> scrutinee_type, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{return match_arms.map([transform_context, substitution, scrutinee_type, stmts_fn](std::shared_ptr<ast::MatchArm> match_arm_under_transform) mutable { return transform_single_match_arm(match_arm_under_transform, transform_context, substitution, scrutinee_type, stmts_fn); });}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> lambda_environment_assign_unknown_placeholder(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment_map, mlc::String parameter_binding_name) noexcept{
type_environment_map.set(parameter_binding_name, standalone_unknown_cell());
return type_environment_map;
}

transform::Transform_lambda_parameter_types_fold_state transform_lambda_parameter_types_environment_fold_step(transform::Transform_lambda_parameter_types_fold_state state, mlc::String parameter_binding_name, mlc::Array<std::shared_ptr<registry::Type>> optional_explicit_parameter_types) noexcept{
std::shared_ptr<registry::Type> resolved_parameter_type = state.next_explicit_type_position < optional_explicit_parameter_types.size() ? optional_explicit_parameter_types[state.next_explicit_type_position] : standalone_unknown_cell();
state.type_environment.set(parameter_binding_name, resolved_parameter_type);
return transform::Transform_lambda_parameter_types_fold_state{state.type_environment, state.parameter_type_vector.concat(mlc::Array<std::shared_ptr<registry::Type>>{resolved_parameter_type}), state.next_explicit_type_position + 1};
}

std::shared_ptr<semantic_ir::SemanticExpression> transform_expr_lambda_with_param_types(mlc::Array<mlc::String> parameter_names, mlc::Array<std::shared_ptr<registry::Type>> parameter_types, std::shared_ptr<ast::Expr> body, ast::Span source_span, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> lambda_environment = transform_context.type_env;
transform::Transform_lambda_parameter_types_fold_state lambda_parameter_accumulator_after_bind = parameter_names.fold(transform::Transform_lambda_parameter_types_fold_state{lambda_environment, {}, 0}, [parameter_types](transform::Transform_lambda_parameter_types_fold_state accumulator, mlc::String parameter_binding_name) mutable { return transform_lambda_parameter_types_environment_fold_step(accumulator, parameter_binding_name, parameter_types); });
transform::TransformContext lambda_context = transform_context_with_lambda_parameter_types(transform_context_with_env(transform_context, lambda_parameter_accumulator_after_bind.type_environment), {});
std::shared_ptr<semantic_ir::SemanticExpression> typed_body = transform_expr(body, lambda_context, stmts_fn);
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionLambda(parameter_names, typed_body, std::make_shared<registry::Type>(registry::TFn(lambda_parameter_accumulator_after_bind.parameter_type_vector, semantic_ir::sexpr_type(typed_body))), source_span));
}

std::shared_ptr<semantic_ir::SemanticExpression> transform_expr_with_lambda_parameter_types(std::shared_ptr<ast::Expr> expression, mlc::Array<std::shared_ptr<registry::Type>> expected_param_types, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{return transform_expr(expression, transform_context_with_lambda_parameter_types(transform_context, expected_param_types), stmts_fn);}

mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> semantic_expression_list_singleton(std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{
mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> empty_transformed_arguments = {};
return empty_transformed_arguments.concat(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>>{expression});
}

mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> transform_array_hof_method_arguments(std::shared_ptr<semantic_ir::SemanticExpression> typed_object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
std::shared_ptr<registry::Type> element_type = semantic_type_structure::array_element_type_from_array_type(semantic_ir::sexpr_type(typed_object));
mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> empty_transformed_arguments = {};
return method_name == mlc::String("fold") ? method_arguments.size() == 0 ? empty_transformed_arguments : method_arguments.size() == 1 ? semantic_expression_list_singleton(transform_expr(method_arguments[0], transform_context, stmts_fn)) : [&]() -> mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> { 
  std::shared_ptr<semantic_ir::SemanticExpression> first_transformed = transform_expr(method_arguments[0], transform_context, stmts_fn);
  std::shared_ptr<semantic_ir::SemanticExpression> second_transformed = transform_expr_with_lambda_parameter_types(method_arguments[1], mlc::Array<std::shared_ptr<registry::Type>>{semantic_ir::sexpr_type(first_transformed), element_type}, transform_context, stmts_fn);
  return semantic_expression_list_singleton(first_transformed).concat(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>>{second_transformed});
 }() : method_name == mlc::String("zip") || method_name == mlc::String("join") ? method_arguments.size() > 0 ? semantic_expression_list_singleton(transform_expr(method_arguments[0], transform_context, stmts_fn)) : empty_transformed_arguments : method_name == mlc::String("take") || method_name == mlc::String("drop") ? method_arguments.size() > 0 ? semantic_expression_list_singleton(transform_expr(method_arguments[0], transform_context, stmts_fn)) : empty_transformed_arguments : method_name == mlc::String("enumerate") || method_name == mlc::String("sum") || method_name == mlc::String("flat") ? empty_transformed_arguments : method_arguments.size() > 0 ? semantic_expression_list_singleton(transform_expr_with_lambda_parameter_types(method_arguments[0], mlc::Array<std::shared_ptr<registry::Type>>{element_type}, transform_context, stmts_fn)) : empty_transformed_arguments;
}

mlc::Array<std::shared_ptr<registry::Type>> inferred_types_from_record_literal_part_for_merge(ast::RecordLitPart literal_record_part, check_context::CheckContext inference_context_for_spread) noexcept{return std::visit(overloaded{
  [&](const RecordLitSpread& recordlitspread) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [spread_expression] = recordlitspread; return mlc::Array<std::shared_ptr<registry::Type>>{infer::infer_expr(spread_expression, inference_context_for_spread).inferred_type}; },
  [&](const RecordLitFields& recordlitfields) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0] = recordlitfields; return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty_types = {};
  return empty_types;
 }(); }
}, literal_record_part);}

transform::TransformPass transform_pass_new(transform::TransformContext transform_context) noexcept{return transform::TransformPass{transform_context, 0};}

std::shared_ptr<semantic_ir::SemanticExpression> dispatch_transform_pass(transform::TransformPass transform_pass, std::shared_ptr<ast::Expr> expression, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{return [&]() -> std::shared_ptr<semantic_ir::SemanticExpression> { if (std::holds_alternative<ast::ExprInt>((*expression)._)) { auto _v_exprint = std::get<ast::ExprInt>((*expression)._); auto [value, source_span] = _v_exprint; return TransformPass_visit_int(transform_pass, value, source_span, stmts_fn); } if (std::holds_alternative<ast::ExprStr>((*expression)._)) { auto _v_exprstr = std::get<ast::ExprStr>((*expression)._); auto [value, source_span] = _v_exprstr; return TransformPass_visit_str(transform_pass, value, source_span, stmts_fn); } if (std::holds_alternative<ast::ExprBool>((*expression)._)) { auto _v_exprbool = std::get<ast::ExprBool>((*expression)._); auto [value, source_span] = _v_exprbool; return TransformPass_visit_bool(transform_pass, value, source_span, stmts_fn); } if (std::holds_alternative<ast::ExprUnit>((*expression)._)) { auto _v_exprunit = std::get<ast::ExprUnit>((*expression)._); auto [source_span] = _v_exprunit; return TransformPass_visit_unit(transform_pass, source_span, stmts_fn); } if (std::holds_alternative<ast::ExprFloat>((*expression)._)) { auto _v_exprfloat = std::get<ast::ExprFloat>((*expression)._); auto [value, source_span] = _v_exprfloat; return TransformPass_visit_float(transform_pass, value, source_span, stmts_fn); } if (std::holds_alternative<ast::ExprI64>((*expression)._)) { auto _v_expri64 = std::get<ast::ExprI64>((*expression)._); auto [value, source_span] = _v_expri64; return TransformPass_visit_i64(transform_pass, value, source_span, stmts_fn); } if (std::holds_alternative<ast::ExprU8>((*expression)._)) { auto _v_expru8 = std::get<ast::ExprU8>((*expression)._); auto [value, source_span] = _v_expru8; return TransformPass_visit_u8(transform_pass, value, source_span, stmts_fn); } if (std::holds_alternative<ast::ExprUsize>((*expression)._)) { auto _v_exprusize = std::get<ast::ExprUsize>((*expression)._); auto [value, source_span] = _v_exprusize; return TransformPass_visit_usize(transform_pass, value, source_span, stmts_fn); } if (std::holds_alternative<ast::ExprChar>((*expression)._)) { auto _v_exprchar = std::get<ast::ExprChar>((*expression)._); auto [value, source_span] = _v_exprchar; return TransformPass_visit_char(transform_pass, value, source_span, stmts_fn); } if (std::holds_alternative<ast::ExprExtern>((*expression)._)) { auto _v_exprextern = std::get<ast::ExprExtern>((*expression)._); auto [source_span] = _v_exprextern; return TransformPass_visit_extern(transform_pass, source_span, stmts_fn); } if (std::holds_alternative<ast::ExprIdent>((*expression)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*expression)._); auto [name, source_span] = _v_exprident; return TransformPass_visit_ident(transform_pass, name, source_span, stmts_fn); } if (std::holds_alternative<ast::ExprBin>((*expression)._)) { auto _v_exprbin = std::get<ast::ExprBin>((*expression)._); auto [operation, left, right, source_span] = _v_exprbin; return TransformPass_visit_bin(transform_pass, operation, left, right, source_span, stmts_fn); } if (std::holds_alternative<ast::ExprUn>((*expression)._)) { auto _v_exprun = std::get<ast::ExprUn>((*expression)._); auto [operation, inner_expression, source_span] = _v_exprun; return TransformPass_visit_un(transform_pass, operation, inner_expression, source_span, stmts_fn); } if (std::holds_alternative<ast::ExprCall>((*expression)._)) { auto _v_exprcall = std::get<ast::ExprCall>((*expression)._); auto [function, call_arguments, source_span] = _v_exprcall; return TransformPass_visit_call(transform_pass, function, call_arguments, source_span, stmts_fn); } if (std::holds_alternative<ast::ExprMethod>((*expression)._)) { auto _v_exprmethod = std::get<ast::ExprMethod>((*expression)._); auto [object, method_name, method_arguments, source_span] = _v_exprmethod; return TransformPass_visit_method(transform_pass, object, method_name, method_arguments, source_span, stmts_fn); } if (std::holds_alternative<ast::ExprField>((*expression)._)) { auto _v_exprfield = std::get<ast::ExprField>((*expression)._); auto [object, field_name, source_span] = _v_exprfield; return TransformPass_visit_field(transform_pass, object, field_name, source_span, stmts_fn); } if (std::holds_alternative<ast::ExprIndex>((*expression)._)) { auto _v_exprindex = std::get<ast::ExprIndex>((*expression)._); auto [object, index_expression, source_span] = _v_exprindex; return TransformPass_visit_index(transform_pass, object, index_expression, source_span, stmts_fn); } if (std::holds_alternative<ast::ExprIf>((*expression)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expression)._); auto [condition, then_expression, else_expression, source_span] = _v_exprif; return TransformPass_visit_if(transform_pass, condition, then_expression, else_expression, source_span, stmts_fn); } if (std::holds_alternative<ast::ExprBlock>((*expression)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expression)._); auto [statements, result_expression, source_span] = _v_exprblock; return TransformPass_visit_block(transform_pass, statements, result_expression, source_span, stmts_fn); } if (std::holds_alternative<ast::ExprWhile>((*expression)._)) { auto _v_exprwhile = std::get<ast::ExprWhile>((*expression)._); auto [condition, statements, source_span] = _v_exprwhile; return TransformPass_visit_while(transform_pass, condition, statements, source_span, stmts_fn); } if (std::holds_alternative<ast::ExprFor>((*expression)._)) { auto _v_exprfor = std::get<ast::ExprFor>((*expression)._); auto [variable_name, iterator, statements, source_span] = _v_exprfor; return TransformPass_visit_for(transform_pass, variable_name, iterator, statements, source_span, stmts_fn); } if (std::holds_alternative<ast::ExprMatch>((*expression)._)) { auto _v_exprmatch = std::get<ast::ExprMatch>((*expression)._); auto [subject, match_arms, source_span] = _v_exprmatch; return TransformPass_visit_match(transform_pass, subject, match_arms, source_span, stmts_fn); } if (std::holds_alternative<ast::ExprRecord>((*expression)._)) { auto _v_exprrecord = std::get<ast::ExprRecord>((*expression)._); auto [type_name, lit_parts, source_span] = _v_exprrecord; return TransformPass_visit_record(transform_pass, type_name, lit_parts, source_span, stmts_fn); } if (std::holds_alternative<ast::ExprRecordUpdate>((*expression)._)) { auto _v_exprrecordupdate = std::get<ast::ExprRecordUpdate>((*expression)._); auto [type_name, base, field_values, source_span] = _v_exprrecordupdate; return TransformPass_visit_record_update(transform_pass, type_name, base, field_values, source_span, stmts_fn); } if (std::holds_alternative<ast::ExprArray>((*expression)._)) { auto _v_exprarray = std::get<ast::ExprArray>((*expression)._); auto [elements, source_span] = _v_exprarray; return TransformPass_visit_array(transform_pass, elements, source_span, stmts_fn); } if (std::holds_alternative<ast::ExprTuple>((*expression)._)) { auto _v_exprtuple = std::get<ast::ExprTuple>((*expression)._); auto [elements, source_span] = _v_exprtuple; return TransformPass_visit_tuple(transform_pass, elements, source_span, stmts_fn); } if (std::holds_alternative<ast::ExprQuestion>((*expression)._)) { auto _v_exprquestion = std::get<ast::ExprQuestion>((*expression)._); auto [inner_expression, source_span] = _v_exprquestion; return TransformPass_visit_question(transform_pass, inner_expression, source_span, stmts_fn); } if (std::holds_alternative<ast::ExprLambda>((*expression)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*expression)._); auto [parameter_names, body, source_span] = _v_exprlambda; return TransformPass_visit_lambda(transform_pass, parameter_names, body, source_span, stmts_fn); } if (std::holds_alternative<ast::ExprNamedArg>((*expression)._)) { auto _v_exprnamedarg = std::get<ast::ExprNamedArg>((*expression)._); auto [_w0, inner_expression, _w1] = _v_exprnamedarg; return TransformPass_visit_named_arg(transform_pass, inner_expression, stmts_fn); } if (std::holds_alternative<ast::ExprWith>((*expression)._)) { auto _v_exprwith = std::get<ast::ExprWith>((*expression)._); auto [resource, binder, statements, source_span] = _v_exprwith; return TransformPass_visit_with(transform_pass, resource, binder, statements, source_span, stmts_fn); } if (std::holds_alternative<ast::ExprSpawn>((*expression)._)) { auto _v_exprspawn = std::get<ast::ExprSpawn>((*expression)._); auto [statements, source_span] = _v_exprspawn; return TransformPass_visit_spawn(transform_pass, statements, source_span, stmts_fn); } return TransformPass_visit_unsupported(transform_pass, expression, stmts_fn); }();}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_int(transform::TransformPass self, int value, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionInt(value, std::make_shared<registry::Type>((registry::TI32{})), source_span));}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_str(transform::TransformPass self, mlc::String value, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionStr(value, std::make_shared<registry::Type>((registry::TString{})), source_span));}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_bool(transform::TransformPass self, bool value, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBool(value, std::make_shared<registry::Type>((registry::TBool{})), source_span));}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_unit(transform::TransformPass self, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionUnit(std::make_shared<registry::Type>((registry::TUnit{})), source_span));}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_float(transform::TransformPass self, mlc::String value, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionFloat(value, std::make_shared<registry::Type>((registry::TF64{})), source_span));}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_i64(transform::TransformPass self, mlc::String value, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionI64(value, std::make_shared<registry::Type>((registry::TI64{})), source_span));}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_u8(transform::TransformPass self, mlc::String value, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionU8(value, std::make_shared<registry::Type>((registry::TU8{})), source_span));}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_usize(transform::TransformPass self, mlc::String value, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionUsize(value, std::make_shared<registry::Type>((registry::TUsize{})), source_span));}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_char(transform::TransformPass self, mlc::String value, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionChar(value, std::make_shared<registry::Type>((registry::TChar{})), source_span));}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_extern(transform::TransformPass self, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionExtern(std::make_shared<registry::Type>((registry::TUnit{})), source_span));}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_ident(transform::TransformPass self, mlc::String name, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
std::shared_ptr<registry::Type> resolved_type = self.transform_context.type_env.has(name) ? self.transform_context.type_env.get(name) : registry::TypeRegistry_has_fn(self.transform_context.registry, name) ? registry::TypeRegistry_fn_type(self.transform_context.registry, name) : registry::TypeRegistry_has_ctor(self.transform_context.registry, name) ? registry::TypeRegistry_ctor_type(self.transform_context.registry, name) : std::make_shared<registry::Type>((registry::TUnknown{}));
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionIdent(name, resolved_type, source_span));
}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_bin(transform::TransformPass self, mlc::String operation, std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
std::shared_ptr<semantic_ir::SemanticExpression> typed_left = dispatch_transform_pass(self, left, stmts_fn);
std::shared_ptr<semantic_ir::SemanticExpression> typed_right = dispatch_transform_pass(self, right, stmts_fn);
std::shared_ptr<registry::Type> left_type = semantic_ir::sexpr_type(typed_left);
mlc::String method = semantic_type_structure::operator_method_for(operation);
std::shared_ptr<registry::Type> result_type = method != mlc::String("") ? binary_result_type_for_operator(self.transform_context.registry, left_type, operation, method) : semantic_type_structure::binary_operation_result_type(operation, left_type);
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBin(operation, typed_left, typed_right, result_type, source_span));
}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_un(transform::TransformPass self, mlc::String operation, std::shared_ptr<ast::Expr> inner_expression, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
std::shared_ptr<semantic_ir::SemanticExpression> typed_inner = dispatch_transform_pass(self, inner_expression, stmts_fn);
auto result_type = operation == mlc::String("!") ? std::make_shared<registry::Type>((registry::TBool{})) : semantic_ir::sexpr_type(typed_inner);
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionUn(operation, typed_inner, result_type, source_span));
}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_call(transform::TransformPass self, std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
mlc::String callee_name = call_callee_ident_name(function);
mlc::Array<std::shared_ptr<ast::Expr>> resolved_call_args = named_args::resolve_named_call_arguments_for_transform(call_arguments, callee_name, self.transform_context.registry);
std::shared_ptr<semantic_ir::SemanticExpression> typed_function = dispatch_transform_pass(self, function, stmts_fn);
mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> typed_arguments = transform_call_arguments_using_callee_semantic_type(typed_function, resolved_call_args, self.transform_context, stmts_fn);
std::shared_ptr<registry::Type> result_type = callee_name != mlc::String("") && registry::TypeRegistry_has_ctor(self.transform_context.registry, callee_name) ? registry::TypeRegistry_ctor_type(self.transform_context.registry, callee_name) : function_return_type_from_callee_type(semantic_ir::sexpr_type(typed_function));
mlc::Array<int> call_parameter_mutability_flags = direct_call_parameter_mutability_flags(self.transform_context.registry, callee_name, typed_arguments.size());
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionCall(typed_function, typed_arguments, call_parameter_mutability_flags, result_type, source_span));
}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_method(transform::TransformPass self, std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
std::shared_ptr<semantic_ir::SemanticExpression> typed_object = dispatch_transform_pass(self, object, stmts_fn);
std::shared_ptr<registry::Type> receiver_type = semantic_ir::sexpr_type(typed_object);
return hof_method_spec::is_result_option_hof_method(receiver_type, method_name) ? transform_result_option_hof_method_call(self.transform_context.registry, typed_object, method_name, method_arguments, receiver_type, self.transform_context, source_span, stmts_fn) : hof_method_spec::is_array_hof_method_on_receiver(receiver_type, method_name) ? transform_array_hof_method_call(self.transform_context.registry, typed_object, method_name, method_arguments, receiver_type, self.transform_context, source_span, stmts_fn) : transform_regular_method_call(self.transform_context.registry, typed_object, method_name, method_arguments, receiver_type, self.transform_context, source_span, stmts_fn);
}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_field(transform::TransformPass self, std::shared_ptr<ast::Expr> object, mlc::String field_name, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
std::shared_ptr<semantic_ir::SemanticExpression> typed_object = dispatch_transform_pass(self, object, stmts_fn);
std::shared_ptr<registry::Type> result_type = registry::field_type_from_object(semantic_ir::sexpr_type(typed_object), field_name, self.transform_context.registry);
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionField(typed_object, field_name, result_type, source_span));
}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_index(transform::TransformPass self, std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expression, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
std::shared_ptr<semantic_ir::SemanticExpression> typed_object = dispatch_transform_pass(self, object, stmts_fn);
std::shared_ptr<semantic_ir::SemanticExpression> typed_index = dispatch_transform_pass(self, index_expression, stmts_fn);
std::shared_ptr<registry::Type> element_type = array_element_type_from_semantic_expression(typed_object);
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionIndex(typed_object, typed_index, element_type, source_span));
}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_if(transform::TransformPass self, std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expression, std::shared_ptr<ast::Expr> else_expression, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
std::shared_ptr<semantic_ir::SemanticExpression> typed_condition = dispatch_transform_pass(self, condition, stmts_fn);
std::shared_ptr<semantic_ir::SemanticExpression> typed_then = dispatch_transform_pass(self, then_expression, stmts_fn);
std::shared_ptr<semantic_ir::SemanticExpression> typed_else_raw = dispatch_transform_pass(self, else_expression, stmts_fn);
std::shared_ptr<semantic_ir::SemanticExpression> typed_else = conditional_else_empty_unknown_array_coerced_to_then_array_element(typed_then, typed_else_raw);
std::shared_ptr<registry::Type> merged_conditional_type = merge_conditional_expression_types(typed_then, typed_else);
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionIf(typed_condition, typed_then, typed_else, merged_conditional_type, source_span));
}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_block(transform::TransformPass self, mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
transform::TransformStmtsResult statements_parsed = stmts_fn(statements, self.transform_context);
transform::TransformContext body_context = transform_context_with_env(self.transform_context, statements_parsed.type_env);
transform::TransformPass body_pass = transform_pass_new(body_context);
std::shared_ptr<semantic_ir::SemanticExpression> result_parsed = dispatch_transform_pass(body_pass, result_expression, stmts_fn);
std::shared_ptr<registry::Type> result_type = semantic_ir::sexpr_type(result_parsed);
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBlock(statements_parsed.statements, result_parsed, result_type, source_span));
}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_while(transform::TransformPass self, std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
std::shared_ptr<semantic_ir::SemanticExpression> typed_condition = dispatch_transform_pass(self, condition, stmts_fn);
transform::TransformStmtsResult statements_parsed = stmts_fn(statements, self.transform_context);
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionWhile(typed_condition, statements_parsed.statements, std::make_shared<registry::Type>((registry::TUnit{})), source_span));
}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_spawn(transform::TransformPass self, mlc::Array<std::shared_ptr<ast::Stmt>> statements, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
transform::TransformStmtsResult statements_parsed = stmts_fn(statements, self.transform_context);
std::shared_ptr<semantic_ir::SemanticExpression> result_parsed = dispatch_transform_pass(self, ast::expr_spawn_body_result(statements), stmts_fn);
std::shared_ptr<registry::Type> result_type = semantic_ir::sexpr_type(result_parsed);
std::shared_ptr<semantic_ir::SemanticExpression> block = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBlock(statements_parsed.statements, result_parsed, result_type, source_span));
std::shared_ptr<registry::Type> lambda_type = std::make_shared<registry::Type>(registry::TFn({}, result_type));
std::shared_ptr<semantic_ir::SemanticExpression> lambda = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionLambda({}, block, lambda_type, source_span));
std::shared_ptr<semantic_ir::SemanticExpression> spawn_callee = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionIdent(mlc::String("spawn_task"), std::make_shared<registry::Type>((registry::TUnknown{})), source_span));
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionCall(spawn_callee, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>>{lambda}, mlc::Array<int>{0}, std::make_shared<registry::Type>(registry::TGeneric(mlc::String("Task"), mlc::Array<std::shared_ptr<registry::Type>>{result_type})), source_span));
}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_for(transform::TransformPass self, mlc::String variable_name, std::shared_ptr<ast::Expr> iterator, mlc::Array<std::shared_ptr<ast::Stmt>> statements, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
std::shared_ptr<semantic_ir::SemanticExpression> typed_iterator = dispatch_transform_pass(self, iterator, stmts_fn);
std::shared_ptr<registry::Type> element_type = array_element_type_from_semantic_expression(typed_iterator);
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> loop_body_environment = self.transform_context.type_env;
loop_body_environment.set(variable_name, element_type);
transform::TransformContext loop_context = transform_context_with_env(self.transform_context, loop_body_environment);
transform::TransformStmtsResult statements_parsed = stmts_fn(statements, loop_context);
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionFor(variable_name, typed_iterator, statements_parsed.statements, std::make_shared<registry::Type>((registry::TUnit{})), source_span));
}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_match(transform::TransformPass self, std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
std::shared_ptr<semantic_ir::SemanticExpression> typed_subject = dispatch_transform_pass(self, subject, stmts_fn);
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution = substitution::substitution_from_generic(semantic_ir::sexpr_type(typed_subject), self.transform_context.registry);
mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> typed_arms = transform_match_arms(match_arms, self.transform_context, substitution, semantic_ir::sexpr_type(typed_subject), stmts_fn);
std::shared_ptr<registry::Type> result_type = typed_arms.size() > 0 ? semantic_ir::sexpr_type(typed_arms[0]->body) : std::make_shared<registry::Type>((registry::TUnknown{}));
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionMatch(typed_subject, typed_arms, result_type, source_span));
}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_record(transform::TransformPass self, mlc::String type_name, mlc::Array<ast::RecordLitPart> lit_parts, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
mlc::Array<std::shared_ptr<ast::FieldVal>> merged_field_values_for_transform = {};
if (!record_lit_merge::record_literal_contains_spread(lit_parts)){
{
merged_field_values_for_transform = record_lit_merge::collect_explicit_field_values_flat(lit_parts);
}
} else {
{
check_context::CheckContext inference_context_for_literal_spreads = check_context::check_context_new(self.transform_context.type_env, self.transform_context.registry);
auto spread_types_for_merge = mlc::collections::flat_map(lit_parts, [inference_context_for_literal_spreads](ast::RecordLitPart part_under_merge) mutable { return inferred_types_from_record_literal_part_for_merge(part_under_merge, inference_context_for_literal_spreads); });
merged_field_values_for_transform = record_lit_merge::merge_record_literal_parts_to_field_values(type_name, lit_parts, self.transform_context.registry, spread_types_for_merge);
}
}
merged_field_values_for_transform = record_lit_merge::merge_explicit_record_literal_with_type_defaults(type_name, merged_field_values_for_transform, self.transform_context.registry);
mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> typed_fields = transform_field_values(merged_field_values_for_transform, self.transform_context, stmts_fn);
mlc::Array<mlc::String> phantom = registry::TypeRegistry_phantom_type_params_for(self.transform_context.registry, type_name);
mlc::Array<mlc::String> all_params = registry::TypeRegistry_algebraic_decl_type_parameter_names_for(self.transform_context.registry, type_name);
std::shared_ptr<registry::Type> expr_type = all_params.size() > 0 && phantom.size() == all_params.size() ? std::make_shared<registry::Type>(registry::TGeneric(type_name, all_params.map([](mlc::String _algebraic_type_parameter_placeholder) mutable { return standalone_unknown_cell(); }))) : std::make_shared<registry::Type>(registry::TNamed(type_name));
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionRecord(type_name, typed_fields, expr_type, source_span));
}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_record_update(transform::TransformPass self, mlc::String type_name, std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
std::shared_ptr<semantic_ir::SemanticExpression> typed_base = dispatch_transform_pass(self, base, stmts_fn);
mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> typed_fields = transform_field_values(field_values, self.transform_context, stmts_fn);
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionRecordUpdate(type_name, typed_base, typed_fields, std::make_shared<registry::Type>(registry::TNamed(type_name)), source_span));
}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_array(transform::TransformPass self, mlc::Array<std::shared_ptr<ast::Expr>> elements, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> typed_elements = transform_exprs(elements, self.transform_context, stmts_fn);
std::shared_ptr<registry::Type> element_type = typed_elements.size() > 0 ? semantic_ir::sexpr_type(typed_elements[0]) : std::make_shared<registry::Type>((registry::TUnknown{}));
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionArray(typed_elements, std::make_shared<registry::Type>(registry::TArray(element_type)), source_span));
}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_tuple(transform::TransformPass self, mlc::Array<std::shared_ptr<ast::Expr>> elements, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
return elements.size() < 2 ? std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionUnit(std::make_shared<registry::Type>((registry::TUnit{})), source_span)) : [&]() -> std::shared_ptr<semantic_ir::SemanticExpression> { 
  mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> typed = transform_exprs(elements, self.transform_context, stmts_fn);
  mlc::Array<std::shared_ptr<registry::Type>> tuple_types = typed.map([](std::shared_ptr<semantic_ir::SemanticExpression> typed_tuple_member) mutable { return semantic_ir::sexpr_type(typed_tuple_member); });
  return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionTuple(typed, std::make_shared<registry::Type>(registry::TTuple(tuple_types)), source_span));
 }();
}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_question(transform::TransformPass self, std::shared_ptr<ast::Expr> inner_expression, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
std::shared_ptr<semantic_ir::SemanticExpression> typed_inner = dispatch_transform_pass(self, inner_expression, stmts_fn);
std::shared_ptr<registry::Type> inner_expression_type = semantic_ir::sexpr_type(typed_inner);
std::shared_ptr<registry::Type> unwrapped_type = question_unwrapped_type_from_inner(inner_expression_type);
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionQuestion(typed_inner, unwrapped_type, source_span));
}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_lambda(transform::TransformPass self, mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
return self.transform_context.lambda_parameter_types.size() > 0 ? transform_expr_lambda_with_param_types(parameter_names, self.transform_context.lambda_parameter_types, body, source_span, transform_context_with_lambda_parameter_types(self.transform_context, {}), stmts_fn) : [&]() -> std::shared_ptr<semantic_ir::SemanticExpression> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> lambda_environment = self.transform_context.type_env;
  mlc::Array<std::shared_ptr<registry::Type>> parameter_types = parameter_names.map([](mlc::String parameter_binding_placeholder) mutable { return standalone_unknown_cell(); });
  parameter_names.fold(lambda_environment, [](mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> environment_map, mlc::String parameter_binding_name) mutable { return lambda_environment_assign_unknown_placeholder(environment_map, parameter_binding_name); });
  transform::TransformContext lambda_context = transform_context_with_env(self.transform_context, lambda_environment);
  transform::TransformPass lambda_pass = transform_pass_new(lambda_context);
  std::shared_ptr<semantic_ir::SemanticExpression> typed_body = dispatch_transform_pass(lambda_pass, body, stmts_fn);
  std::shared_ptr<registry::Type> function_type = std::make_shared<registry::Type>(registry::TFn(parameter_types, semantic_ir::sexpr_type(typed_body)));
  return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionLambda(parameter_names, typed_body, function_type, source_span));
 }();
}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_named_arg(transform::TransformPass self, std::shared_ptr<ast::Expr> inner_expression, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{return dispatch_transform_pass(self, inner_expression, stmts_fn);}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_with(transform::TransformPass self, std::shared_ptr<ast::Expr> resource, mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> statements, ast::Span source_span, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
std::shared_ptr<semantic_ir::SemanticExpression> typed_resource = dispatch_transform_pass(self, resource, stmts_fn);
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> with_body_environment = self.transform_context.type_env;
with_body_environment.set(binder, semantic_ir::sexpr_type(typed_resource));
transform::TransformContext with_body_context = transform_context_with_env(self.transform_context, with_body_environment);
transform::TransformStmtsResult statements_parsed = stmts_fn(statements, with_body_context);
return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionWith(typed_resource, binder, statements_parsed.statements, std::make_shared<registry::Type>((registry::TUnit{})), source_span));
}

std::shared_ptr<semantic_ir::SemanticExpression> TransformPass_visit_unsupported(transform::TransformPass self, std::shared_ptr<ast::Expr> expression, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionUnit(std::make_shared<registry::Type>((registry::TUnit{})), ast::expr_span(expression)));}

std::shared_ptr<semantic_ir::SemanticExpression> transform_expr(std::shared_ptr<ast::Expr> expression, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
std::shared_ptr<ast::Expr> expression_partial_application = partial_application_desugar::partial_application_desugar_expr(expression);
return dispatch_transform_pass(transform_pass_new(transform_context), expression_partial_application, stmts_fn);
}

} // namespace transform
