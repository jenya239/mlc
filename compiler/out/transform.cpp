#include "transform.hpp"

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

using namespace ast;
using namespace registry;
using namespace semantic_type_structure;
using namespace array_method_types;
using namespace result_option_method_types;
using namespace pattern_env;
using namespace semantic_type_structure;
using namespace infer;
using namespace check_context;
using namespace record_lit_merge;
using namespace partial_application_desugar;
using namespace semantic_ir;
using namespace ast_tokens;

struct Transform_lambda_parameter_types_fold_state {mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment;mlc::Array<std::shared_ptr<registry::Type>> parameter_type_vector;int next_explicit_type_position;};

mlc::String receiver_type_key_for_method_dispatch(std::shared_ptr<registry::Type> receiver_semantic_type) noexcept;

mlc::Array<int> direct_call_parameter_mutability_flags(registry::TypeRegistry registry, mlc::String callee_name, int positional_argument_count) noexcept;

mlc::Array<int> instance_method_receiver_and_parameters_mutability_pattern(registry::TypeRegistry registry, std::shared_ptr<registry::Type> receiver_semantic_type, mlc::String method_name, int method_argument_count) noexcept;

std::shared_ptr<semantic_ir::SExpr> coerce_unknown_else_array_when_then_known_inner(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, ast::Span span_else, std::shared_ptr<registry::Type> inner_then, std::shared_ptr<semantic_ir::SExpr> typed_else) noexcept;

std::shared_ptr<semantic_ir::SExpr> conditional_else_coerce_empty_array_using_then_type(std::shared_ptr<semantic_ir::SExpr> typed_then, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, ast::Span span_else, std::shared_ptr<semantic_ir::SExpr> typed_else) noexcept;

std::shared_ptr<semantic_ir::SExpr> array_literal_else_maybe_coerce(std::shared_ptr<semantic_ir::SExpr> typed_then, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, ast::Span span_else, std::shared_ptr<semantic_ir::SExpr> typed_else) noexcept;

std::shared_ptr<semantic_ir::SExpr> conditional_else_empty_unknown_array_coerced_to_then_array_element(std::shared_ptr<semantic_ir::SExpr> typed_then, std::shared_ptr<semantic_ir::SExpr> typed_else) noexcept;

transform::TransformContext transform_context_new(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, registry::TypeRegistry registry) noexcept;

transform::TransformContext empty_transform_context() noexcept;

transform::TransformContext transform_context_with_env(transform::TransformContext base, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env) noexcept;

bool transform_expression_is_named_argument(std::shared_ptr<ast::Expr> argument_expression) noexcept;

bool transform_has_named_args(mlc::Array<std::shared_ptr<ast::Expr>> args) noexcept;

int transform_find_param_slot(mlc::Array<mlc::String> parameter_names, mlc::String sought_parameter_name) noexcept;

std::shared_ptr<ast::Expr> transform_inner_after_stripping_named_label(std::shared_ptr<ast::Expr> argument_expression) noexcept;

mlc::Array<std::shared_ptr<ast::Expr>> transform_strip_labels(mlc::Array<std::shared_ptr<ast::Expr>> args) noexcept;

std::shared_ptr<semantic_ir::SExpr> coerce_expr_to_type(std::shared_ptr<semantic_ir::SExpr> expression, std::shared_ptr<registry::Type> target_type) noexcept;

mlc::Array<std::shared_ptr<ast::Expr>> transform_reorder_to_positional(mlc::Array<std::shared_ptr<ast::Expr>> args, mlc::Array<mlc::String> param_names) noexcept;

mlc::Array<std::shared_ptr<ast::Expr>> reorder_named_args_for_transform(mlc::Array<std::shared_ptr<ast::Expr>> args, mlc::String callee_name, registry::TypeRegistry registry) noexcept;

std::shared_ptr<registry::Type> standalone_unknown_cell() noexcept;

mlc::Array<std::shared_ptr<semantic_ir::SExpr>> transform_exprs(mlc::Array<std::shared_ptr<ast::Expr>> expressions, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SExpr> transform_one_call_argument_using_optional_expected_type(std::shared_ptr<ast::Expr> argument_expression_shared, std::shared_ptr<registry::Type> expected_formal_parameter_type, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

mlc::Array<std::shared_ptr<semantic_ir::SExpr>> transform_call_arguments_using_callee_semantic_type(std::shared_ptr<semantic_ir::SExpr> callee_semantic_expression, mlc::Array<std::shared_ptr<ast::Expr>> argument_expressions_under_call, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SFieldVal> semantic_field_val_from_source_field_after_transform(std::shared_ptr<ast::FieldVal> field_value_under_transform, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> transform_field_vals(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> build_transform_substitution(std::shared_ptr<registry::Type> subject_type, registry::TypeRegistry registry) noexcept;

std::shared_ptr<semantic_ir::SMatchArm> transform_single_match_arm(std::shared_ptr<ast::MatchArm> arm, transform::TransformContext transform_context, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution, std::shared_ptr<registry::Type> scrutinee_type, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> transform_match_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, transform::TransformContext transform_context, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution, std::shared_ptr<registry::Type> scrutinee_type, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> lambda_environment_assign_unknown_placeholder(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment_map, mlc::String parameter_binding_name) noexcept;

transform::Transform_lambda_parameter_types_fold_state transform_lambda_parameter_types_environment_fold_step(transform::Transform_lambda_parameter_types_fold_state state, mlc::String parameter_binding_name, mlc::Array<std::shared_ptr<registry::Type>> optional_explicit_parameter_types) noexcept;

std::shared_ptr<semantic_ir::SExpr> transform_expr_lambda_with_param_types(mlc::Array<mlc::String> parameter_names, mlc::Array<std::shared_ptr<registry::Type>> parameter_types, std::shared_ptr<ast::Expr> body, ast::Span source_span, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

std::shared_ptr<semantic_ir::SExpr> transform_expr_maybe_lambda_context(std::shared_ptr<ast::Expr> expression, mlc::Array<std::shared_ptr<registry::Type>> expected_param_types, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

mlc::Array<std::shared_ptr<semantic_ir::SExpr>> transform_array_hof_method_arguments(std::shared_ptr<semantic_ir::SExpr> typed_object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

mlc::Array<std::shared_ptr<registry::Type>> inferred_types_from_record_literal_part_for_merge(ast::RecordLitPart literal_record_part, check_context::CheckContext inference_context_for_spread) noexcept;

std::shared_ptr<semantic_ir::SExpr> transform_expr(std::shared_ptr<ast::Expr> expression, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

mlc::String receiver_type_key_for_method_dispatch(std::shared_ptr<registry::Type> receiver_semantic_type) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*receiver_semantic_type))) { auto _v_tnamed = std::get<registry::TNamed>((*receiver_semantic_type)); auto [name] = _v_tnamed; return name; } if (std::holds_alternative<registry::TShared>((*receiver_semantic_type))) { auto _v_tshared = std::get<registry::TShared>((*receiver_semantic_type)); auto [inner] = _v_tshared; return [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*inner))) { auto _v_tnamed = std::get<registry::TNamed>((*inner)); auto [inner_name] = _v_tnamed; return inner_name; } return mlc::String(""); }(); } return mlc::String(""); }();}

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

std::shared_ptr<semantic_ir::SExpr> coerce_unknown_else_array_when_then_known_inner(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, ast::Span span_else, std::shared_ptr<registry::Type> inner_then, std::shared_ptr<semantic_ir::SExpr> typed_else) noexcept{return [&]() -> std::shared_ptr<semantic_ir::SExpr> { if (std::holds_alternative<registry::TArray>((*semantic_ir::sexpr_type(typed_else)))) { auto _v_tarray = std::get<registry::TArray>((*semantic_ir::sexpr_type(typed_else))); auto [inner_else] = _v_tarray; return semantic_type_structure::type_is_unknown(inner_else) ? std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprArray(elements, std::make_shared<registry::Type>(registry::TArray(inner_then)), span_else)) : typed_else; } return typed_else; }();}

std::shared_ptr<semantic_ir::SExpr> conditional_else_coerce_empty_array_using_then_type(std::shared_ptr<semantic_ir::SExpr> typed_then, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, ast::Span span_else, std::shared_ptr<semantic_ir::SExpr> typed_else) noexcept{return [&]() -> std::shared_ptr<semantic_ir::SExpr> { if (std::holds_alternative<registry::TArray>((*semantic_ir::sexpr_type(typed_then)))) { auto _v_tarray = std::get<registry::TArray>((*semantic_ir::sexpr_type(typed_then))); auto [inner_then] = _v_tarray; return coerce_unknown_else_array_when_then_known_inner(elements, span_else, inner_then, typed_else); } return typed_else; }();}

std::shared_ptr<semantic_ir::SExpr> array_literal_else_maybe_coerce(std::shared_ptr<semantic_ir::SExpr> typed_then, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, ast::Span span_else, std::shared_ptr<semantic_ir::SExpr> typed_else) noexcept{return elements.size() > 0 ? typed_else : conditional_else_coerce_empty_array_using_then_type(typed_then, elements, span_else, typed_else);}

std::shared_ptr<semantic_ir::SExpr> conditional_else_empty_unknown_array_coerced_to_then_array_element(std::shared_ptr<semantic_ir::SExpr> typed_then, std::shared_ptr<semantic_ir::SExpr> typed_else) noexcept{return [&]() -> std::shared_ptr<semantic_ir::SExpr> { if (std::holds_alternative<semantic_ir::SExprArray>((*typed_else)._)) { auto _v_sexprarray = std::get<semantic_ir::SExprArray>((*typed_else)._); auto [elements, _w0, span_else] = _v_sexprarray; return array_literal_else_maybe_coerce(typed_then, elements, span_else, typed_else); } return typed_else; }();}

transform::TransformContext transform_context_new(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, registry::TypeRegistry registry) noexcept{return transform::TransformContext{type_env, registry};}

transform::TransformContext empty_transform_context() noexcept{return transform::TransformContext{mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>(), registry::empty_registry()};}

transform::TransformContext transform_context_with_env(transform::TransformContext base, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env) noexcept{return transform::TransformContext{type_env, base.registry};}

bool transform_expression_is_named_argument(std::shared_ptr<ast::Expr> argument_expression) noexcept{return [&]() { if (std::holds_alternative<ast::ExprNamedArg>((*argument_expression)._)) { auto _v_exprnamedarg = std::get<ast::ExprNamedArg>((*argument_expression)._); auto [_w0, _w1, _w2] = _v_exprnamedarg; return true; } return false; }();}

bool transform_has_named_args(mlc::Array<std::shared_ptr<ast::Expr>> args) noexcept{return args.any(transform_expression_is_named_argument);}

int transform_find_param_slot(mlc::Array<mlc::String> parameter_names, mlc::String sought_parameter_name) noexcept{return parameter_names.find_index([sought_parameter_name](mlc::String parameter_name) mutable { return parameter_name == sought_parameter_name; });}

std::shared_ptr<ast::Expr> transform_inner_after_stripping_named_label(std::shared_ptr<ast::Expr> argument_expression) noexcept{return [&]() -> std::shared_ptr<ast::Expr> { if (std::holds_alternative<ast::ExprNamedArg>((*argument_expression)._)) { auto _v_exprnamedarg = std::get<ast::ExprNamedArg>((*argument_expression)._); auto [_w0, inner_expression, _w1] = _v_exprnamedarg; return inner_expression; } return argument_expression; }();}

mlc::Array<std::shared_ptr<ast::Expr>> transform_strip_labels(mlc::Array<std::shared_ptr<ast::Expr>> args) noexcept{return args.map(transform_inner_after_stripping_named_label);}

std::shared_ptr<semantic_ir::SExpr> coerce_expr_to_type(std::shared_ptr<semantic_ir::SExpr> expression, std::shared_ptr<registry::Type> target_type) noexcept{return [&]() -> std::shared_ptr<semantic_ir::SExpr> { if (std::holds_alternative<semantic_ir::SExprArray>((*expression)._)) { auto _v_sexprarray = std::get<semantic_ir::SExprArray>((*expression)._); auto [elements, _w0, source_span] = _v_sexprarray; return elements.size() == 0 ? std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprArray(elements, target_type, source_span)) : expression; } if (std::holds_alternative<semantic_ir::SExprRecord>((*expression)._)) { auto _v_sexprrecord = std::get<semantic_ir::SExprRecord>((*expression)._); auto [type_name, fields, expr_type, source_span] = _v_sexprrecord; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { if (std::holds_alternative<registry::TGeneric>((*target_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*target_type)); auto [tgt_name, _w0] = _v_tgeneric; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { if (std::holds_alternative<registry::TGeneric>((*expr_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*expr_type)); auto [exp_name, _w0] = _v_tgeneric; return tgt_name == exp_name ? std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprRecord(type_name, fields, target_type, source_span)) : expression; } return expression; }(); } return expression; }(); } if (std::holds_alternative<semantic_ir::SExprBlock>((*expression)._)) { auto _v_sexprblock = std::get<semantic_ir::SExprBlock>((*expression)._); auto [statements, result_expression, _w0, span] = _v_sexprblock; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { if (std::holds_alternative<registry::TArray>((*target_type))) { auto _v_tarray = std::get<registry::TArray>((*target_type)); auto [_w0] = _v_tarray; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> coerced_result = coerce_expr_to_type(result_expression, target_type);
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBlock(statements, coerced_result, semantic_ir::sexpr_type(coerced_result), span));
 }(); } return expression; }(); } if (std::holds_alternative<semantic_ir::SExprIf>((*expression)._)) { auto _v_sexprif = std::get<semantic_ir::SExprIf>((*expression)._); auto [condition, then_branch, else_branch, _w0, source_span] = _v_sexprif; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { if (std::holds_alternative<registry::TArray>((*target_type))) { auto _v_tarray = std::get<registry::TArray>((*target_type)); auto [_w0] = _v_tarray; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprIf(condition, coerce_expr_to_type(then_branch, target_type), coerce_expr_to_type(else_branch, target_type), target_type, source_span)); } return expression; }(); } return expression; }();}

mlc::Array<std::shared_ptr<ast::Expr>> transform_reorder_to_positional(mlc::Array<std::shared_ptr<ast::Expr>> args, mlc::Array<mlc::String> param_names) noexcept{
int n = param_names.size();
mlc::Array<std::shared_ptr<ast::Expr>> result = {};
mlc::Array<int> filled = {};
int k = 0;
while (k < n){
{
result.push_back(ast::expr_placeholder());
filled.push_back(0);
k = k + 1;
}
}
int pos_slot = 0;
int i = 0;
while (i < args.size()){
{
[&]() -> std::tuple<> { if (std::holds_alternative<ast::ExprNamedArg>((*args[i])._)) { auto _v_exprnamedarg = std::get<ast::ExprNamedArg>((*args[i])._); auto [name, inner, _w0] = _v_exprnamedarg; return [&]() -> std::tuple<> { 
  int slot = transform_find_param_slot(param_names, name);
  if (slot >= 0 && filled[slot] == 0){
{
result.set(slot, inner);
filled.set(slot, 1);
}
}
  return std::make_tuple();
 }(); } return [&]() -> std::tuple<> { 
  while (pos_slot < n && filled[pos_slot] == 1){
{
pos_slot = pos_slot + 1;
}
}
  if (pos_slot < n){
{
result.set(pos_slot, args[i]);
filled.set(pos_slot, 1);
pos_slot = pos_slot + 1;
}
}
  return std::make_tuple();
 }(); }();
i = i + 1;
}
}
mlc::Array<std::shared_ptr<ast::Expr>> compact = {};
int j = 0;
while (j < n){
{
if (filled[j] == 1){
{
compact.push_back(result[j]);
}
}
j = j + 1;
}
}
return compact;
}

mlc::Array<std::shared_ptr<ast::Expr>> reorder_named_args_for_transform(mlc::Array<std::shared_ptr<ast::Expr>> args, mlc::String callee_name, registry::TypeRegistry registry) noexcept{return !transform_has_named_args(args) ? args : [&]() -> mlc::Array<std::shared_ptr<ast::Expr>> { 
  mlc::Array<mlc::String> param_names = registry::TypeRegistry_parameter_names_for(registry, callee_name);
  return param_names.size() == 0 ? transform_strip_labels(args) : transform_reorder_to_positional(args, param_names);
 }();}

std::shared_ptr<registry::Type> standalone_unknown_cell() noexcept{return std::make_shared<registry::Type>((registry::TUnknown{}));}

mlc::Array<std::shared_ptr<semantic_ir::SExpr>> transform_exprs(mlc::Array<std::shared_ptr<ast::Expr>> expressions, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{return expressions.map([transform_context, stmts_fn](std::shared_ptr<ast::Expr> expression_under_transform) mutable { return transform_expr(expression_under_transform, transform_context, stmts_fn); });}

std::shared_ptr<semantic_ir::SExpr> transform_one_call_argument_using_optional_expected_type(std::shared_ptr<ast::Expr> argument_expression_shared, std::shared_ptr<registry::Type> expected_formal_parameter_type, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
std::shared_ptr<ast::Expr> argument_partially_desugared_shared_expression = partial_application_desugar::partial_application_desugar_expr(argument_expression_shared);
return [&]() -> std::shared_ptr<semantic_ir::SExpr> { if (std::holds_alternative<ast::ExprLambda>((*argument_partially_desugared_shared_expression)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*argument_partially_desugared_shared_expression)._); auto [parameter_names, lambda_body, lambda_span] = _v_exprlambda; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { if (std::holds_alternative<registry::TFn>((*expected_formal_parameter_type))) { auto _v_tfn = std::get<registry::TFn>((*expected_formal_parameter_type)); auto [expected_lambda_parameter_semantic_types, _w0] = _v_tfn; return transform_expr_lambda_with_param_types(parameter_names, expected_lambda_parameter_semantic_types, lambda_body, lambda_span, transform_context, stmts_fn); } return transform_expr(argument_expression_shared, transform_context, stmts_fn); }(); } return transform_expr(argument_expression_shared, transform_context, stmts_fn); }();
}

mlc::Array<std::shared_ptr<semantic_ir::SExpr>> transform_call_arguments_using_callee_semantic_type(std::shared_ptr<semantic_ir::SExpr> callee_semantic_expression, mlc::Array<std::shared_ptr<ast::Expr>> argument_expressions_under_call, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
return [&]() -> mlc::Array<std::shared_ptr<semantic_ir::SExpr>> { if (std::holds_alternative<registry::TFn>((*semantic_ir::sexpr_type(callee_semantic_expression)))) { auto _v_tfn = std::get<registry::TFn>((*semantic_ir::sexpr_type(callee_semantic_expression))); auto [expected_formal_parameter_types, _w0] = _v_tfn; return [&]() -> mlc::Array<std::shared_ptr<semantic_ir::SExpr>> { 
  mlc::Array<std::shared_ptr<semantic_ir::SExpr>> transformed_arguments = {};
  int argument_index_under_call = 0;
  while (argument_index_under_call < argument_expressions_under_call.size()){
{
std::shared_ptr<registry::Type> expected_placeholder_type = argument_index_under_call < expected_formal_parameter_types.size() ? expected_formal_parameter_types[argument_index_under_call] : standalone_unknown_cell();
transformed_arguments.push_back(transform_one_call_argument_using_optional_expected_type(argument_expressions_under_call[argument_index_under_call], expected_placeholder_type, transform_context, stmts_fn));
argument_index_under_call = argument_index_under_call + 1;
}
}
  return transformed_arguments;
 }(); } return transform_exprs(argument_expressions_under_call, transform_context, stmts_fn); }();
}

std::shared_ptr<semantic_ir::SFieldVal> semantic_field_val_from_source_field_after_transform(std::shared_ptr<ast::FieldVal> field_value_under_transform, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{return std::make_shared<semantic_ir::SFieldVal>(semantic_ir::SFieldVal{field_value_under_transform->name, transform_expr(field_value_under_transform->val, transform_context, stmts_fn)});}

mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> transform_field_vals(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{return field_values.map([transform_context, stmts_fn](std::shared_ptr<ast::FieldVal> field_value_under_transform) mutable { return semantic_field_val_from_source_field_after_transform(field_value_under_transform, transform_context, stmts_fn); });}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> build_transform_substitution(std::shared_ptr<registry::Type> subject_type, registry::TypeRegistry registry) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
[&]() -> void { if (std::holds_alternative<registry::TGeneric>((*subject_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*subject_type)); auto [type_name, type_arguments] = _v_tgeneric; return [&]() { 
  mlc::Array<mlc::String> algebraic_parameter_names = registry::TypeRegistry_algebraic_decl_type_parameter_names_for(registry, type_name);
  int algebraic_argument_slot = 0;
  return [&]() { 
  while (algebraic_argument_slot < algebraic_parameter_names.size() && algebraic_argument_slot < type_arguments.size()){
{
substitution.set(algebraic_parameter_names[algebraic_argument_slot], type_arguments[algebraic_argument_slot]);
algebraic_argument_slot = algebraic_argument_slot + 1;
}
}
 }();
 }(); } return; }();
return substitution;
}

std::shared_ptr<semantic_ir::SMatchArm> transform_single_match_arm(std::shared_ptr<ast::MatchArm> arm, transform::TransformContext transform_context, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution, std::shared_ptr<registry::Type> scrutinee_type, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> arm_environment_after_pattern = pattern_env::env_for_pattern_substituted(transform_context.type_env, arm->pat, transform_context.registry, substitution, scrutinee_type);
transform::TransformContext arm_transform_context = transform_context_with_env(transform_context, arm_environment_after_pattern);
std::shared_ptr<semantic_ir::SExpr> typed_body_expression = transform_expr(arm->body, arm_transform_context, stmts_fn);
std::shared_ptr<semantic_ir::SExpr> typed_guard_expression = arm->has_guard ? transform_expr(arm->when_condition, arm_transform_context, stmts_fn) : std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBool(true, std::make_shared<registry::Type>((registry::TBool{})), ast::span_unknown()));
return std::make_shared<semantic_ir::SMatchArm>(semantic_ir::SMatchArm{arm->pat, arm->has_guard, typed_guard_expression, typed_body_expression});
}

mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> transform_match_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, transform::TransformContext transform_context, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution, std::shared_ptr<registry::Type> scrutinee_type, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{return arms.map([transform_context, substitution, scrutinee_type, stmts_fn](std::shared_ptr<ast::MatchArm> arm_under_transform) mutable { return transform_single_match_arm(arm_under_transform, transform_context, substitution, scrutinee_type, stmts_fn); });}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> lambda_environment_assign_unknown_placeholder(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment_map, mlc::String parameter_binding_name) noexcept{
type_environment_map.set(parameter_binding_name, standalone_unknown_cell());
return type_environment_map;
}

transform::Transform_lambda_parameter_types_fold_state transform_lambda_parameter_types_environment_fold_step(transform::Transform_lambda_parameter_types_fold_state state, mlc::String parameter_binding_name, mlc::Array<std::shared_ptr<registry::Type>> optional_explicit_parameter_types) noexcept{
std::shared_ptr<registry::Type> resolved_parameter_type = state.next_explicit_type_position < optional_explicit_parameter_types.size() ? optional_explicit_parameter_types[state.next_explicit_type_position] : standalone_unknown_cell();
state.type_environment.set(parameter_binding_name, resolved_parameter_type);
state.parameter_type_vector.push_back(resolved_parameter_type);
return transform::Transform_lambda_parameter_types_fold_state{state.type_environment, state.parameter_type_vector, state.next_explicit_type_position + 1};
}

std::shared_ptr<semantic_ir::SExpr> transform_expr_lambda_with_param_types(mlc::Array<mlc::String> parameter_names, mlc::Array<std::shared_ptr<registry::Type>> parameter_types, std::shared_ptr<ast::Expr> body, ast::Span source_span, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> lambda_environment = transform_context.type_env;
transform::Transform_lambda_parameter_types_fold_state lambda_parameter_accumulator_after_bind = parameter_names.fold(transform::Transform_lambda_parameter_types_fold_state{lambda_environment, {}, 0}, [parameter_types](transform::Transform_lambda_parameter_types_fold_state accumulator, mlc::String parameter_binding_name) mutable { return transform_lambda_parameter_types_environment_fold_step(accumulator, parameter_binding_name, parameter_types); });
transform::TransformContext lambda_context = transform_context_with_env(transform_context, lambda_parameter_accumulator_after_bind.type_environment);
std::shared_ptr<semantic_ir::SExpr> typed_body = transform_expr(body, lambda_context, stmts_fn);
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprLambda(parameter_names, typed_body, std::make_shared<registry::Type>(registry::TFn(lambda_parameter_accumulator_after_bind.parameter_type_vector, semantic_ir::sexpr_type(typed_body))), source_span));
}

std::shared_ptr<semantic_ir::SExpr> transform_expr_maybe_lambda_context(std::shared_ptr<ast::Expr> expression, mlc::Array<std::shared_ptr<registry::Type>> expected_param_types, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{return [&]() -> std::shared_ptr<semantic_ir::SExpr> { if (std::holds_alternative<ast::ExprLambda>((*expression)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*expression)._); auto [parameter_names, body, source_span] = _v_exprlambda; return transform_expr_lambda_with_param_types(parameter_names, expected_param_types, body, source_span, transform_context, stmts_fn); } return transform_expr(expression, transform_context, stmts_fn); }();}

mlc::Array<std::shared_ptr<semantic_ir::SExpr>> transform_array_hof_method_arguments(std::shared_ptr<semantic_ir::SExpr> typed_object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
std::shared_ptr<registry::Type> element_type = semantic_type_structure::array_element_type_from_array_type(semantic_ir::sexpr_type(typed_object));
mlc::Array<std::shared_ptr<semantic_ir::SExpr>> out = {};
if (method_name == mlc::String("fold")){
{
if (method_arguments.size() > 0){
out.push_back(transform_expr(method_arguments[0], transform_context, stmts_fn));
if (method_arguments.size() > 1){
std::shared_ptr<registry::Type> acc_t = semantic_ir::sexpr_type(out[0]);
out.push_back(transform_expr_maybe_lambda_context(method_arguments[1], mlc::Array<std::shared_ptr<registry::Type>>{acc_t, element_type}, transform_context, stmts_fn));
}
}
}
} else {
{
if (method_name == mlc::String("zip") || method_name == mlc::String("join")){
if (method_arguments.size() > 0){
out.push_back(transform_expr(method_arguments[0], transform_context, stmts_fn));
}
} else {
if (method_name == mlc::String("take") || method_name == mlc::String("drop")){
if (method_arguments.size() > 0){
out.push_back(transform_expr(method_arguments[0], transform_context, stmts_fn));
}
} else {
if (method_name == mlc::String("enumerate") || method_name == mlc::String("sum") || method_name == mlc::String("flat")){
int _skip = 0;
} else {
if (method_arguments.size() > 0){
out.push_back(transform_expr_maybe_lambda_context(method_arguments[0], mlc::Array<std::shared_ptr<registry::Type>>{element_type}, transform_context, stmts_fn));
}
}
}
}
}
}
return out;
}

mlc::Array<std::shared_ptr<registry::Type>> inferred_types_from_record_literal_part_for_merge(ast::RecordLitPart literal_record_part, check_context::CheckContext inference_context_for_spread) noexcept{return std::visit(overloaded{
  [&](const RecordLitSpread& recordlitspread) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [spread_expression] = recordlitspread; return mlc::Array<std::shared_ptr<registry::Type>>{infer::infer_expr(spread_expression, inference_context_for_spread).inferred_type}; },
  [&](const RecordLitFields& recordlitfields) -> mlc::Array<std::shared_ptr<registry::Type>> { auto [_w0] = recordlitfields; return {}; }
}, literal_record_part._);}

std::shared_ptr<semantic_ir::SExpr> transform_expr(std::shared_ptr<ast::Expr> expression, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept{
std::shared_ptr<ast::Expr> expression_partial_application = partial_application_desugar::partial_application_desugar_expr(expression);
return [&]() -> std::shared_ptr<semantic_ir::SExpr> { if (std::holds_alternative<ast::ExprInt>((*expression_partial_application)._)) { auto _v_exprint = std::get<ast::ExprInt>((*expression_partial_application)._); auto [value, source_span] = _v_exprint; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprInt(value, std::make_shared<registry::Type>((registry::TI32{})), source_span)); } if (std::holds_alternative<ast::ExprStr>((*expression_partial_application)._)) { auto _v_exprstr = std::get<ast::ExprStr>((*expression_partial_application)._); auto [value, source_span] = _v_exprstr; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprStr(value, std::make_shared<registry::Type>((registry::TString{})), source_span)); } if (std::holds_alternative<ast::ExprBool>((*expression_partial_application)._)) { auto _v_exprbool = std::get<ast::ExprBool>((*expression_partial_application)._); auto [value, source_span] = _v_exprbool; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBool(value, std::make_shared<registry::Type>((registry::TBool{})), source_span)); } if (std::holds_alternative<ast::ExprUnit>((*expression_partial_application)._)) { auto _v_exprunit = std::get<ast::ExprUnit>((*expression_partial_application)._); auto [source_span] = _v_exprunit; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUnit(std::make_shared<registry::Type>((registry::TUnit{})), source_span)); } if (std::holds_alternative<ast::ExprFloat>((*expression_partial_application)._)) { auto _v_exprfloat = std::get<ast::ExprFloat>((*expression_partial_application)._); auto [value, source_span] = _v_exprfloat; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprFloat(value, std::make_shared<registry::Type>((registry::TF64{})), source_span)); } if (std::holds_alternative<ast::ExprI64>((*expression_partial_application)._)) { auto _v_expri64 = std::get<ast::ExprI64>((*expression_partial_application)._); auto [value, source_span] = _v_expri64; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprI64(value, std::make_shared<registry::Type>((registry::TI64{})), source_span)); } if (std::holds_alternative<ast::ExprU8>((*expression_partial_application)._)) { auto _v_expru8 = std::get<ast::ExprU8>((*expression_partial_application)._); auto [value, source_span] = _v_expru8; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprU8(value, std::make_shared<registry::Type>((registry::TU8{})), source_span)); } if (std::holds_alternative<ast::ExprUsize>((*expression_partial_application)._)) { auto _v_exprusize = std::get<ast::ExprUsize>((*expression_partial_application)._); auto [value, source_span] = _v_exprusize; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUsize(value, std::make_shared<registry::Type>((registry::TUsize{})), source_span)); } if (std::holds_alternative<ast::ExprChar>((*expression_partial_application)._)) { auto _v_exprchar = std::get<ast::ExprChar>((*expression_partial_application)._); auto [value, source_span] = _v_exprchar; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprChar(value, std::make_shared<registry::Type>((registry::TChar{})), source_span)); } if (std::holds_alternative<ast::ExprExtern>((*expression_partial_application)._)) { auto _v_exprextern = std::get<ast::ExprExtern>((*expression_partial_application)._); auto [source_span] = _v_exprextern; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprExtern(std::make_shared<registry::Type>((registry::TUnit{})), source_span)); } if (std::holds_alternative<ast::ExprIdent>((*expression_partial_application)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*expression_partial_application)._); auto [name, source_span] = _v_exprident; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<registry::Type> resolved_type = transform_context.type_env.has(name) ? transform_context.type_env.get(name) : registry::TypeRegistry_has_fn(transform_context.registry, name) ? registry::TypeRegistry_fn_type(transform_context.registry, name) : registry::TypeRegistry_has_ctor(transform_context.registry, name) ? registry::TypeRegistry_ctor_type(transform_context.registry, name) : std::make_shared<registry::Type>((registry::TUnknown{}));
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprIdent(name, resolved_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprBin>((*expression_partial_application)._)) { auto _v_exprbin = std::get<ast::ExprBin>((*expression_partial_application)._); auto [operation, left, right, source_span] = _v_exprbin; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_left = transform_expr(left, transform_context, stmts_fn);
  std::shared_ptr<semantic_ir::SExpr> typed_right = transform_expr(right, transform_context, stmts_fn);
  std::shared_ptr<registry::Type> left_type = semantic_ir::sexpr_type(typed_left);
  mlc::String method = semantic_type_structure::operator_method_for(operation);
  std::shared_ptr<registry::Type> result_type = method != mlc::String("") ? [&]() -> std::shared_ptr<registry::Type> { 
  std::shared_ptr<registry::Type> from_reg = registry::method_return_type_from_object(left_type, method, transform_context.registry);
  return semantic_type_structure::type_is_unknown(from_reg) ? semantic_type_structure::binary_operation_result_type(operation, left_type) : from_reg;
 }() : semantic_type_structure::binary_operation_result_type(operation, left_type);
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBin(operation, typed_left, typed_right, result_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprUn>((*expression_partial_application)._)) { auto _v_exprun = std::get<ast::ExprUn>((*expression_partial_application)._); auto [operation, inner, source_span] = _v_exprun; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_inner = transform_expr(inner, transform_context, stmts_fn);
  auto result_type = operation == mlc::String("!") ? std::make_shared<registry::Type>((registry::TBool{})) : semantic_ir::sexpr_type(typed_inner);
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUn(operation, typed_inner, result_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprCall>((*expression_partial_application)._)) { auto _v_exprcall = std::get<ast::ExprCall>((*expression_partial_application)._); auto [function, call_arguments, source_span] = _v_exprcall; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  mlc::String callee_name = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*function)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*function)._); auto [name, _w0] = _v_exprident; return name; } return mlc::String(""); }();
  mlc::Array<std::shared_ptr<ast::Expr>> resolved_call_args = reorder_named_args_for_transform(call_arguments, callee_name, transform_context.registry);
  std::shared_ptr<semantic_ir::SExpr> typed_fn = transform_expr(function, transform_context, stmts_fn);
  mlc::Array<std::shared_ptr<semantic_ir::SExpr>> typed_args = transform_call_arguments_using_callee_semantic_type(typed_fn, resolved_call_args, transform_context, stmts_fn);
  std::shared_ptr<registry::Type> result_type = callee_name != mlc::String("") && registry::TypeRegistry_has_ctor(transform_context.registry, callee_name) ? registry::TypeRegistry_ctor_type(transform_context.registry, callee_name) : [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TFn>((*semantic_ir::sexpr_type(typed_fn)))) { auto _v_tfn = std::get<registry::TFn>((*semantic_ir::sexpr_type(typed_fn))); auto [_w0, return_type] = _v_tfn; return return_type; } return std::make_shared<registry::Type>((registry::TUnknown{})); }();
  mlc::Array<int> call_parameter_mutability_flags = direct_call_parameter_mutability_flags(transform_context.registry, callee_name, typed_args.size());
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprCall(typed_fn, typed_args, call_parameter_mutability_flags, result_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprMethod>((*expression_partial_application)._)) { auto _v_exprmethod = std::get<ast::ExprMethod>((*expression_partial_application)._); auto [object, method_name, method_arguments, source_span] = _v_exprmethod; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_object = transform_expr(object, transform_context, stmts_fn);
  return result_option_method_types::should_infer_result_option_combinator(semantic_ir::sexpr_type(typed_object), method_name) ? [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  mlc::Array<std::shared_ptr<semantic_ir::SExpr>> typed_args = transform_exprs(method_arguments, transform_context, stmts_fn);
  mlc::Array<std::shared_ptr<registry::Type>> arg_types = typed_args.map([](std::shared_ptr<semantic_ir::SExpr> typed_argument) mutable { return semantic_ir::sexpr_type(typed_argument); });
  std::shared_ptr<registry::Type> result_type = result_option_method_types::result_option_hof_call_result_type(semantic_ir::sexpr_type(typed_object), method_name, arg_types);
  mlc::Array<int> method_parameter_mutability_flags = instance_method_receiver_and_parameters_mutability_pattern(transform_context.registry, semantic_ir::sexpr_type(typed_object), method_name, typed_args.size());
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMethod(typed_object, method_name, typed_args, method_parameter_mutability_flags, result_type, source_span));
 }() : semantic_type_structure::type_is_array(semantic_ir::sexpr_type(typed_object)) && array_method_types::is_array_hof_method(method_name) ? [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  mlc::Array<std::shared_ptr<semantic_ir::SExpr>> typed_args = transform_array_hof_method_arguments(typed_object, method_name, method_arguments, transform_context, stmts_fn);
  mlc::Array<std::shared_ptr<registry::Type>> arg_types = typed_args.map([](std::shared_ptr<semantic_ir::SExpr> typed_argument) mutable { return semantic_ir::sexpr_type(typed_argument); });
  std::shared_ptr<registry::Type> elem = semantic_type_structure::array_element_type_from_array_type(semantic_ir::sexpr_type(typed_object));
  std::shared_ptr<registry::Type> result_type = array_method_types::array_hof_call_result_type(elem, method_name, arg_types);
  mlc::Array<int> method_parameter_mutability_flags = instance_method_receiver_and_parameters_mutability_pattern(transform_context.registry, semantic_ir::sexpr_type(typed_object), method_name, typed_args.size());
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMethod(typed_object, method_name, typed_args, method_parameter_mutability_flags, result_type, source_span));
 }() : [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  mlc::Array<std::shared_ptr<semantic_ir::SExpr>> typed_args = transform_exprs(method_arguments, transform_context, stmts_fn);
  std::shared_ptr<registry::Type> builtin_type = semantic_type_structure::builtin_method_return_type(method_name);
  std::shared_ptr<registry::Type> result_type = semantic_type_structure::type_is_unknown(builtin_type) ? registry::method_return_type_from_object(semantic_ir::sexpr_type(typed_object), method_name, transform_context.registry) : builtin_type;
  mlc::Array<int> method_parameter_mutability_flags = instance_method_receiver_and_parameters_mutability_pattern(transform_context.registry, semantic_ir::sexpr_type(typed_object), method_name, typed_args.size());
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMethod(typed_object, method_name, typed_args, method_parameter_mutability_flags, result_type, source_span));
 }();
 }(); } if (std::holds_alternative<ast::ExprField>((*expression_partial_application)._)) { auto _v_exprfield = std::get<ast::ExprField>((*expression_partial_application)._); auto [object, field_name, source_span] = _v_exprfield; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_object = transform_expr(object, transform_context, stmts_fn);
  std::shared_ptr<registry::Type> result_type = registry::field_type_from_object(semantic_ir::sexpr_type(typed_object), field_name, transform_context.registry);
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprField(typed_object, field_name, result_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprIndex>((*expression_partial_application)._)) { auto _v_exprindex = std::get<ast::ExprIndex>((*expression_partial_application)._); auto [object, index_expression, source_span] = _v_exprindex; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_object = transform_expr(object, transform_context, stmts_fn);
  std::shared_ptr<semantic_ir::SExpr> typed_index = transform_expr(index_expression, transform_context, stmts_fn);
  std::shared_ptr<registry::Type> element_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TArray>((*semantic_ir::sexpr_type(typed_object)))) { auto _v_tarray = std::get<registry::TArray>((*semantic_ir::sexpr_type(typed_object))); auto [inner] = _v_tarray; return inner; } return std::make_shared<registry::Type>((registry::TUnknown{})); }();
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprIndex(typed_object, typed_index, element_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprIf>((*expression_partial_application)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expression_partial_application)._); auto [condition, then_expression, else_expression, source_span] = _v_exprif; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_condition = transform_expr(condition, transform_context, stmts_fn);
  std::shared_ptr<semantic_ir::SExpr> typed_then = transform_expr(then_expression, transform_context, stmts_fn);
  std::shared_ptr<semantic_ir::SExpr> typed_else_raw = transform_expr(else_expression, transform_context, stmts_fn);
  std::shared_ptr<semantic_ir::SExpr> typed_else = conditional_else_empty_unknown_array_coerced_to_then_array_element(typed_then, typed_else_raw);
  std::shared_ptr<registry::Type> merged_conditional_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TArray>((*semantic_ir::sexpr_type(typed_then)))) { auto _v_tarray = std::get<registry::TArray>((*semantic_ir::sexpr_type(typed_then))); auto [inner_then] = _v_tarray; return [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TArray>((*semantic_ir::sexpr_type(typed_else)))) { auto _v_tarray = std::get<registry::TArray>((*semantic_ir::sexpr_type(typed_else))); auto [inner_else] = _v_tarray; return semantic_type_structure::type_is_unknown(inner_then) ? std::make_shared<registry::Type>(registry::TArray(inner_else)) : std::make_shared<registry::Type>(registry::TArray(inner_then)); } return semantic_ir::sexpr_type(typed_then); }(); } return semantic_ir::sexpr_type(typed_then); }();
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprIf(typed_condition, typed_then, typed_else, merged_conditional_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprBlock>((*expression_partial_application)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expression_partial_application)._); auto [statements, result, source_span] = _v_exprblock; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  transform::TransformStmtsResult stmts_result = stmts_fn(statements, transform_context);
  transform::TransformContext body_context = transform_context_with_env(transform_context, stmts_result.type_env);
  std::shared_ptr<semantic_ir::SExpr> typed_result = transform_expr(result, body_context, stmts_fn);
  std::shared_ptr<registry::Type> result_type = semantic_ir::sexpr_type(typed_result);
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBlock(stmts_result.statements, typed_result, result_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprWhile>((*expression_partial_application)._)) { auto _v_exprwhile = std::get<ast::ExprWhile>((*expression_partial_application)._); auto [condition, statements, source_span] = _v_exprwhile; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_condition = transform_expr(condition, transform_context, stmts_fn);
  transform::TransformStmtsResult stmts_result = stmts_fn(statements, transform_context);
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprWhile(typed_condition, stmts_result.statements, std::make_shared<registry::Type>((registry::TUnit{})), source_span));
 }(); } if (std::holds_alternative<ast::ExprFor>((*expression_partial_application)._)) { auto _v_exprfor = std::get<ast::ExprFor>((*expression_partial_application)._); auto [variable_name, iterator, statements, source_span] = _v_exprfor; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_iterator = transform_expr(iterator, transform_context, stmts_fn);
  std::shared_ptr<registry::Type> element_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TArray>((*semantic_ir::sexpr_type(typed_iterator)))) { auto _v_tarray = std::get<registry::TArray>((*semantic_ir::sexpr_type(typed_iterator))); auto [inner] = _v_tarray; return inner; } return std::make_shared<registry::Type>((registry::TUnknown{})); }();
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> inner_env = transform_context.type_env;
  inner_env.set(variable_name, element_type);
  transform::TransformContext loop_context = transform_context_with_env(transform_context, inner_env);
  transform::TransformStmtsResult stmts_result = stmts_fn(statements, loop_context);
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprFor(variable_name, typed_iterator, stmts_result.statements, std::make_shared<registry::Type>((registry::TUnit{})), source_span));
 }(); } if (std::holds_alternative<ast::ExprMatch>((*expression_partial_application)._)) { auto _v_exprmatch = std::get<ast::ExprMatch>((*expression_partial_application)._); auto [subject, arms, source_span] = _v_exprmatch; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_subject = transform_expr(subject, transform_context, stmts_fn);
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution = build_transform_substitution(semantic_ir::sexpr_type(typed_subject), transform_context.registry);
  mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> typed_arms = transform_match_arms(arms, transform_context, substitution, semantic_ir::sexpr_type(typed_subject), stmts_fn);
  std::shared_ptr<registry::Type> result_type = typed_arms.size() > 0 ? semantic_ir::sexpr_type(typed_arms[0]->body) : std::make_shared<registry::Type>((registry::TUnknown{}));
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMatch(typed_subject, typed_arms, result_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprRecord>((*expression_partial_application)._)) { auto _v_exprrecord = std::get<ast::ExprRecord>((*expression_partial_application)._); auto [type_name, lit_parts, source_span] = _v_exprrecord; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  mlc::Array<std::shared_ptr<ast::FieldVal>> merged_field_values_for_transform = {};
  if (!record_lit_merge::record_literal_contains_spread(lit_parts)){
{
merged_field_values_for_transform = record_lit_merge::collect_explicit_field_values_flat(lit_parts);
}
} else {
{
check_context::CheckContext inference_context_for_literal_spreads = check_context::check_context_new(transform_context.type_env, transform_context.registry);
auto spread_types_for_merge = mlc::collections::flat_map(lit_parts, [inference_context_for_literal_spreads](ast::RecordLitPart part_under_merge) mutable { return inferred_types_from_record_literal_part_for_merge(part_under_merge, inference_context_for_literal_spreads); });
merged_field_values_for_transform = record_lit_merge::merge_record_literal_parts_to_field_values(type_name, lit_parts, transform_context.registry, spread_types_for_merge);
}
}
  merged_field_values_for_transform = record_lit_merge::merge_explicit_record_literal_with_type_defaults(type_name, merged_field_values_for_transform, transform_context.registry);
  mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> typed_fields = transform_field_vals(merged_field_values_for_transform, transform_context, stmts_fn);
  mlc::Array<mlc::String> phantom = registry::TypeRegistry_phantom_type_params_for(transform_context.registry, type_name);
  mlc::Array<mlc::String> all_params = registry::TypeRegistry_algebraic_decl_type_parameter_names_for(transform_context.registry, type_name);
  std::shared_ptr<registry::Type> expr_type = all_params.size() > 0 && phantom.size() == all_params.size() ? std::make_shared<registry::Type>(registry::TGeneric(type_name, all_params.map([](mlc::String _algebraic_type_parameter_placeholder) mutable { return standalone_unknown_cell(); }))) : std::make_shared<registry::Type>(registry::TNamed(type_name));
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprRecord(type_name, typed_fields, expr_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprRecordUpdate>((*expression_partial_application)._)) { auto _v_exprrecordupdate = std::get<ast::ExprRecordUpdate>((*expression_partial_application)._); auto [type_name, base, field_values, source_span] = _v_exprrecordupdate; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_base = transform_expr(base, transform_context, stmts_fn);
  mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> typed_fields = transform_field_vals(field_values, transform_context, stmts_fn);
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprRecordUpdate(type_name, typed_base, typed_fields, std::make_shared<registry::Type>(registry::TNamed(type_name)), source_span));
 }(); } if (std::holds_alternative<ast::ExprArray>((*expression_partial_application)._)) { auto _v_exprarray = std::get<ast::ExprArray>((*expression_partial_application)._); auto [elements, source_span] = _v_exprarray; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  mlc::Array<std::shared_ptr<semantic_ir::SExpr>> typed_elements = transform_exprs(elements, transform_context, stmts_fn);
  std::shared_ptr<registry::Type> element_type = typed_elements.size() > 0 ? semantic_ir::sexpr_type(typed_elements[0]) : std::make_shared<registry::Type>((registry::TUnknown{}));
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprArray(typed_elements, std::make_shared<registry::Type>(registry::TArray(element_type)), source_span));
 }(); } if (std::holds_alternative<ast::ExprTuple>((*expression_partial_application)._)) { auto _v_exprtuple = std::get<ast::ExprTuple>((*expression_partial_application)._); auto [elements, source_span] = _v_exprtuple; return elements.size() < 2 ? std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUnit(std::make_shared<registry::Type>((registry::TUnit{})), ast::span_unknown())) : [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  mlc::Array<std::shared_ptr<semantic_ir::SExpr>> typed = transform_exprs(elements, transform_context, stmts_fn);
  mlc::Array<std::shared_ptr<registry::Type>> tuple_types = typed.map([](std::shared_ptr<semantic_ir::SExpr> typed_tuple_member) mutable { return semantic_ir::sexpr_type(typed_tuple_member); });
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprTuple(typed, std::make_shared<registry::Type>(registry::TTuple(tuple_types)), source_span));
 }(); } if (std::holds_alternative<ast::ExprQuestion>((*expression_partial_application)._)) { auto _v_exprquestion = std::get<ast::ExprQuestion>((*expression_partial_application)._); auto [inner, source_span] = _v_exprquestion; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_inner = transform_expr(inner, transform_context, stmts_fn);
  std::shared_ptr<registry::Type> inner_type = semantic_ir::sexpr_type(typed_inner);
  std::shared_ptr<registry::Type> unwrapped_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TGeneric>((*inner_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*inner_type)); auto [_w0, type_args] = _v_tgeneric; return type_args.size() > 0 ? type_args[0] : std::make_shared<registry::Type>((registry::TUnknown{})); } return std::make_shared<registry::Type>((registry::TUnknown{})); }();
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprQuestion(typed_inner, unwrapped_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprLambda>((*expression_partial_application)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*expression_partial_application)._); auto [parameter_names, body, source_span] = _v_exprlambda; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> lambda_environment = transform_context.type_env;
  mlc::Array<std::shared_ptr<registry::Type>> parameter_types = parameter_names.map([](mlc::String parameter_binding_placeholder) mutable { return standalone_unknown_cell(); });
  parameter_names.fold(lambda_environment, [](mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> environment_map, mlc::String parameter_binding_name) mutable { return lambda_environment_assign_unknown_placeholder(environment_map, parameter_binding_name); });
  transform::TransformContext lambda_context = transform_context_with_env(transform_context, lambda_environment);
  std::shared_ptr<semantic_ir::SExpr> typed_body = transform_expr(body, lambda_context, stmts_fn);
  std::shared_ptr<registry::Type> function_type = std::make_shared<registry::Type>(registry::TFn(parameter_types, semantic_ir::sexpr_type(typed_body)));
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprLambda(parameter_names, typed_body, function_type, source_span));
 }(); } if (std::holds_alternative<ast::ExprNamedArg>((*expression_partial_application)._)) { auto _v_exprnamedarg = std::get<ast::ExprNamedArg>((*expression_partial_application)._); auto [_w0, inner, _w1] = _v_exprnamedarg; return transform_expr(inner, transform_context, stmts_fn); } if (std::holds_alternative<ast::ExprWith>((*expression_partial_application)._)) { auto _v_exprwith = std::get<ast::ExprWith>((*expression_partial_application)._); auto [resource, binder, stmts, source_span] = _v_exprwith; return [&]() -> std::shared_ptr<semantic_ir::SExpr> { 
  std::shared_ptr<semantic_ir::SExpr> typed_resource = transform_expr(resource, transform_context, stmts_fn);
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> inner_env = transform_context.type_env;
  inner_env.set(binder, semantic_ir::sexpr_type(typed_resource));
  transform::TransformContext inner_context = transform_context_with_env(transform_context, inner_env);
  transform::TransformStmtsResult stmts_result = stmts_fn(stmts, inner_context);
  return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprWith(typed_resource, binder, stmts_result.statements, std::make_shared<registry::Type>((registry::TUnit{})), source_span));
 }(); } return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUnit(std::make_shared<registry::Type>((registry::TUnit{})), ast::span_unknown())); }();
}

} // namespace transform
