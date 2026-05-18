#define main mlc_user_main
#include "transform.hpp"

namespace transform {

mlc::String receiver_type_key_for_method_dispatch(std::shared_ptr<registry::Type> receiver_semantic_type) noexcept{
return std::visit(overloaded{[&](const registry::TNamed& tNamed) { auto [name] = tNamed; return name; },
[&](const registry::TShared& tShared) { auto [inner] = tShared; return std::visit(overloaded{[&](const registry::TNamed& tNamed) { auto [inner_name] = tNamed; return inner_name; },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*inner)); },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*receiver_semantic_type));
}
mlc::Array<int> direct_call_parameter_mutability_flags(registry::TypeRegistry registry, mlc::String callee_name, int positional_argument_count) noexcept{
if ((callee_name == mlc::String("", 0))) {
auto empty_mutability_pattern = mlc::Array<int>{};
return empty_mutability_pattern;
} else {
auto stored = registry::TypeRegistry_parameter_mutability_flags_for(registry, callee_name);
if ((stored.length() != positional_argument_count)) {
auto empty_mutability_pattern = mlc::Array<int>{};
return empty_mutability_pattern;
} else {
return stored;
}
}
}
mlc::Array<int> instance_method_receiver_and_parameters_mutability_pattern(registry::TypeRegistry registry, std::shared_ptr<registry::Type> receiver_semantic_type, mlc::String method_name, int method_argument_count) noexcept{
auto receiver_key = receiver_type_key_for_method_dispatch(receiver_semantic_type);
if ((receiver_key == mlc::String("", 0))) {
auto empty_mutability_pattern = mlc::Array<int>{};
return empty_mutability_pattern;
} else {
auto mangled = ((receiver_key + mlc::String("_", 1)) + method_name);
if ((!registry::TypeRegistry_has_fn(registry, mangled))) {
auto empty_mutability_pattern = mlc::Array<int>{};
return empty_mutability_pattern;
} else {
auto stored = registry::TypeRegistry_parameter_mutability_flags_for(registry, mangled);
if ((stored.length() != (method_argument_count + 1))) {
auto empty_mutability_pattern = mlc::Array<int>{};
return empty_mutability_pattern;
} else {
return stored;
}
}
}
}
std::shared_ptr<semantic_ir::SExpr> coerce_unknown_else_array_when_then_known_inner(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, ast::Span span_else, std::shared_ptr<registry::Type> inner_then, std::shared_ptr<semantic_ir::SExpr> typed_else) noexcept{
return std::visit(overloaded{[&](const registry::TArray& tArray) { auto [inner_else] = tArray; return (semantic_type_structure::type_is_unknown(inner_else) ? (std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprArray{elements, std::make_shared<registry::Type>(registry::TArray{inner_then}), span_else})) : (typed_else)); },
[&](const auto& __v) { return typed_else; }
}, (*semantic_ir::sexpr_type(typed_else)));
}
std::shared_ptr<semantic_ir::SExpr> conditional_else_coerce_empty_array_using_then_type(std::shared_ptr<semantic_ir::SExpr> typed_then, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, ast::Span span_else, std::shared_ptr<semantic_ir::SExpr> typed_else) noexcept{
return std::visit(overloaded{[&](const registry::TArray& tArray) { auto [inner_then] = tArray; return coerce_unknown_else_array_when_then_known_inner(elements, span_else, inner_then, typed_else); },
[&](const auto& __v) { return typed_else; }
}, (*semantic_ir::sexpr_type(typed_then)));
}
std::shared_ptr<semantic_ir::SExpr> array_literal_else_maybe_coerce(std::shared_ptr<semantic_ir::SExpr> typed_then, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, ast::Span span_else, std::shared_ptr<semantic_ir::SExpr> typed_else) noexcept{
if ((elements.length() > 0)) {
return typed_else;
} else {
return conditional_else_coerce_empty_array_using_then_type(typed_then, elements, span_else, typed_else);
}
}
std::shared_ptr<semantic_ir::SExpr> conditional_else_empty_unknown_array_coerced_to_then_array_element(std::shared_ptr<semantic_ir::SExpr> typed_then, std::shared_ptr<semantic_ir::SExpr> typed_else) noexcept{
return std::visit(overloaded{[&](const semantic_ir::SExprArray& sExprArray) { auto [elements, __1, span_else] = sExprArray; return array_literal_else_maybe_coerce(typed_then, elements, span_else, typed_else); },
[&](const auto& __v) { return typed_else; }
}, (*typed_else));
}
TransformContext transform_context_new(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, registry::TypeRegistry registry) noexcept{
return TransformContext{type_env, registry};
}
TransformContext empty_transform_context() noexcept{
return TransformContext{{}, registry::empty_registry()};
}
TransformContext transform_context_with_env(TransformContext base, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env) noexcept{
return TransformContext{type_env, base.registry};
}
bool transform_expression_is_named_argument(std::shared_ptr<ast::Expr> argument_expression) noexcept{
return std::visit(overloaded{[&](const ast::ExprNamedArg& exprNamedArg) { auto [__0, __1, __2] = exprNamedArg; return true; },
[&](const auto& __v) { return false; }
}, (*argument_expression));
}
bool transform_has_named_args(mlc::Array<std::shared_ptr<ast::Expr>> args) noexcept{
return args.any(transform_expression_is_named_argument);
}
int transform_find_param_slot(mlc::Array<mlc::String> parameter_names, mlc::String sought_parameter_name) noexcept{
return parameter_names.find_index([=](mlc::String parameter_name) mutable { return (parameter_name == sought_parameter_name); });
}
std::shared_ptr<ast::Expr> transform_inner_after_stripping_named_label(std::shared_ptr<ast::Expr> argument_expression) noexcept{
return std::visit(overloaded{[&](const ast::ExprNamedArg& exprNamedArg) { auto [__0, inner_expression, __2] = exprNamedArg; return inner_expression; },
[&](const auto& __v) { return argument_expression; }
}, (*argument_expression));
}
mlc::Array<std::shared_ptr<ast::Expr>> transform_strip_labels(mlc::Array<std::shared_ptr<ast::Expr>> args) noexcept{
return args.map(transform_inner_after_stripping_named_label);
}
std::shared_ptr<semantic_ir::SExpr> coerce_expr_to_type(std::shared_ptr<semantic_ir::SExpr> expression, std::shared_ptr<registry::Type> target_type) noexcept{
return std::visit(overloaded{[&](const semantic_ir::SExprArray& sExprArray) { auto [elements, __1, source_span] = sExprArray; return ((elements.length() == 0) ? (std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprArray{elements, target_type, source_span})) : (expression)); },
[&](const semantic_ir::SExprRecord& sExprRecord) { auto [type_name, fields, expr_type, source_span] = sExprRecord; return std::visit(overloaded{[&](const registry::TGeneric& tGeneric) { auto [tgt_name, __1] = tGeneric; return std::visit(overloaded{[&](const registry::TGeneric& tGeneric) { auto [exp_name, __1] = tGeneric; return ((tgt_name == exp_name) ? (std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprRecord{type_name, fields, target_type, source_span})) : (expression)); },
[&](const auto& __v) { return expression; }
}, (*expr_type)); },
[&](const auto& __v) { return expression; }
}, (*target_type)); },
[&](const semantic_ir::SExprBlock& sExprBlock) { auto [statements, result_expression, __2, span] = sExprBlock; return std::visit(overloaded{[&](const registry::TArray& tArray) { auto [__0] = tArray; return [&]() {
auto coerced_result = coerce_expr_to_type(result_expression, target_type);
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBlock{statements, coerced_result, semantic_ir::sexpr_type(coerced_result), span});
}(); },
[&](const auto& __v) { return expression; }
}, (*target_type)); },
[&](const semantic_ir::SExprIf& sExprIf) { auto [condition, then_branch, else_branch, __3, source_span] = sExprIf; return std::visit(overloaded{[&](const registry::TArray& tArray) { auto [__0] = tArray; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprIf{condition, coerce_expr_to_type(then_branch, target_type), coerce_expr_to_type(else_branch, target_type), target_type, source_span}); },
[&](const auto& __v) { return expression; }
}, (*target_type)); },
[&](const auto& __v) { return expression; }
}, (*expression));
}
mlc::Array<std::shared_ptr<ast::Expr>> transform_reorder_to_positional(mlc::Array<std::shared_ptr<ast::Expr>> args, mlc::Array<mlc::String> param_names) noexcept{
auto n = param_names.length();
auto result = mlc::Array<std::shared_ptr<ast::Expr>>{};
auto filled = mlc::Array<int>{};
auto k = 0;
while ((k < n)) {
result.push_back(ast::expr_placeholder());
filled.push_back(0);
k = (k + 1);
}
auto pos_slot = 0;
auto i = 0;
while ((i < args.length())) {
std::visit(overloaded{[&](const ast::ExprNamedArg& exprNamedArg) { auto [name, inner, __2] = exprNamedArg; return [&]() {
auto slot = transform_find_param_slot(param_names, name);
if (((slot >= 0) && (filled[slot] == 0))) {
result.set(slot, inner);
filled.set(slot, 1);
}
/* unit */;
return /* unit */;
}(); },
[&](const auto& __v) { return [&]() {
while (((pos_slot < n) && (filled[pos_slot] == 1))) {
pos_slot = (pos_slot + 1);
}
if ((pos_slot < n)) {
result.set(pos_slot, args[i]);
filled.set(pos_slot, 1);
pos_slot = (pos_slot + 1);
}
/* unit */;
return /* unit */;
}(); }
}, (*args[i]));
i = (i + 1);
}
auto compact = mlc::Array<std::shared_ptr<ast::Expr>>{};
auto j = 0;
while ((j < n)) {
if ((filled[j] == 1)) {
compact.push_back(result[j]);
}
j = (j + 1);
}
return compact;
}
mlc::Array<std::shared_ptr<ast::Expr>> reorder_named_args_for_transform(mlc::Array<std::shared_ptr<ast::Expr>> args, mlc::String callee_name, registry::TypeRegistry registry) noexcept{
if ((!transform_has_named_args(args))) {
return args;
} else {
auto param_names = registry::TypeRegistry_parameter_names_for(registry, callee_name);
if ((param_names.length() == 0)) {
return transform_strip_labels(args);
} else {
return transform_reorder_to_positional(args, param_names);
}
}
}
std::shared_ptr<registry::Type> standalone_unknown_cell() noexcept{
return std::make_shared<registry::Type>(registry::TUnknown{});
}
mlc::Array<std::shared_ptr<semantic_ir::SExpr>> transform_exprs(mlc::Array<std::shared_ptr<ast::Expr>> expressions, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
return expressions.map([=](std::shared_ptr<ast::Expr> expression_under_transform) mutable { return transform_expr(expression_under_transform, transform_context, stmts_fn); });
}
std::shared_ptr<semantic_ir::SExpr> transform_one_call_argument_using_optional_expected_type(std::shared_ptr<ast::Expr> argument_expression_shared, std::shared_ptr<registry::Type> expected_formal_parameter_type, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
auto argument_partially_desugared_shared_expression = partial_application_desugar_expr(argument_expression_shared);
return std::visit(overloaded{[&](const ast::ExprLambda& exprLambda) { auto [parameter_names, lambda_body, lambda_span] = exprLambda; return std::visit(overloaded{[&](const registry::TFn& tFn) { auto [expected_lambda_parameter_semantic_types, __1] = tFn; return transform_expr_lambda_with_param_types(parameter_names, expected_lambda_parameter_semantic_types, lambda_body, lambda_span, transform_context, stmts_fn); },
[&](const auto& __v) { return transform_expr(argument_expression_shared, transform_context, stmts_fn); }
}, (*expected_formal_parameter_type)); },
[&](const auto& __v) { return coerce_expr_to_type(transform_expr(argument_expression_shared, transform_context, stmts_fn), expected_formal_parameter_type); }
}, argument_partially_desugared_shared_expression);
}
mlc::Array<std::shared_ptr<semantic_ir::SExpr>> transform_call_arguments_using_callee_semantic_type(std::shared_ptr<semantic_ir::SExpr> callee_semantic_expression, mlc::Array<std::shared_ptr<ast::Expr>> argument_expressions_under_call, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
std::visit(overloaded{[&](const registry::TFn& tFn) { auto [expected_formal_parameter_types, __1] = tFn; return [&]() {
auto transformed_arguments = mlc::Array<std::shared_ptr<semantic_ir::SExpr>>{};
auto argument_index_under_call = 0;
while ((argument_index_under_call < argument_expressions_under_call.length())) {
auto expected_placeholder_type = ((argument_index_under_call < expected_formal_parameter_types.length()) ? (expected_formal_parameter_types[argument_index_under_call]) : ([&]() {
standalone_unknown_cell();
transformed_arguments.push_back(transform_one_call_argument_using_optional_expected_type(argument_expressions_under_call[argument_index_under_call], expected_placeholder_type, transform_context, stmts_fn));
argument_index_under_call = (argument_index_under_call + 1);
return /* unit */;
}()));
transformed_arguments;
}
/* unit */;
[=](auto _) mutable { return transform_exprs(argument_expressions_under_call, transform_context, stmts_fn); };
/* unit */;
return /* unit */;
}(); },
[&](const auto& __v) { return [=](auto field_value_under_transform, auto transform_context, auto stmts_fn) mutable { return ((Shared{} < semantic_ir::SFieldVal{}) > /* unit */); }; },
[&](const Shared& shared) { return new_(semantic_ir::SFieldVal{field_value_under_transform.name, transform_expr(field_value_under_transform.val, transform_context, stmts_fn)}); },
[&](const auto& __v) { return [=](auto field_values, auto transform_context, auto stmts_fn) mutable { return mlc::Array<bool>{((Shared{} < semantic_ir::SFieldVal{}) > /* unit */)}; }; },
[&](const auto& field_values) { return map([=](auto field_value_under_transform) mutable { return semantic_field_val_from_source_field_after_transform(field_value_under_transform, transform_context, stmts_fn); }); },
[&](const auto& __v) { return [=](auto subject_type, auto registry) mutable { return (Map{} < string); }; },
[&](const Shared& shared) { return (registry::Type{} > /* unit */); },
[&](const auto& __v) { return /* unit */; },
[&](const auto& substitution) { return (Map{} < string); },
[&](const Shared& shared) { return (registry::Type{} > /* unit */); },
[&](const auto& __v) { return /* unit */; },
[&](const auto& new_) { return /* unit */; },
[&](const auto& __v) { return /* unit */; },
[&](const registry::TGeneric& tGeneric) { auto [type_name, type_arguments] = tGeneric; return [&]() {
auto algebraic_parameter_names = registry.algebraic_decl_type_parameter_names_for(type_name);
auto algebraic_argument_slot = 0;
return [&]() {
while (((algebraic_argument_slot < algebraic_parameter_names.length()) && (algebraic_argument_slot < type_arguments.length()))) {
substitution.set(algebraic_parameter_names[algebraic_argument_slot], type_arguments[algebraic_argument_slot]);
algebraic_argument_slot = (algebraic_argument_slot + 1);
}
}();
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*semantic_ir::sexpr_type(callee_semantic_expression)));
return substitution;
}
std::shared_ptr<semantic_ir::SMatchArm> transform_single_match_arm(std::shared_ptr<ast::MatchArm> arm, TransformContext transform_context, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution, std::shared_ptr<registry::Type> scrutinee_type, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
auto arm_environment_after_pattern = pattern_env::env_for_pattern_substituted(transform_context.type_env, arm->pat, transform_context.registry, substitution, scrutinee_type);
auto arm_transform_context = transform_context_with_env(transform_context, arm_environment_after_pattern);
auto typed_body_expression = transform_expr(arm->body, arm_transform_context, stmts_fn);
auto typed_guard_expression = (arm->has_guard ? (transform_expr(arm->when_condition, arm_transform_context, stmts_fn)) : (std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBool{true, std::make_shared<registry::Type>(registry::TBool{}), ast::span_unknown()})));
return std::make_shared<semantic_ir::SMatchArm>(semantic_ir::SMatchArm{arm->pat, arm->has_guard, typed_guard_expression, typed_body_expression});
}
mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> transform_match_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, TransformContext transform_context, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution, std::shared_ptr<registry::Type> scrutinee_type, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
return arms.map([=](std::shared_ptr<ast::MatchArm> arm_under_transform) mutable { return transform_single_match_arm(arm_under_transform, transform_context, substitution, scrutinee_type, stmts_fn); });
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> lambda_environment_assign_unknown_placeholder(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment_map, mlc::String parameter_binding_name) noexcept{
type_environment_map.set(parameter_binding_name, standalone_unknown_cell());
return type_environment_map;
}
Transform_lambda_parameter_types_fold_state transform_lambda_parameter_types_environment_fold_step(Transform_lambda_parameter_types_fold_state state, mlc::String parameter_binding_name, mlc::Array<std::shared_ptr<registry::Type>> optional_explicit_parameter_types) noexcept{
auto resolved_parameter_type = ((state.next_explicit_type_position < optional_explicit_parameter_types.length()) ? (optional_explicit_parameter_types[state.next_explicit_type_position]) : (standalone_unknown_cell()));
state.type_environment.set(parameter_binding_name, resolved_parameter_type);
state.parameter_type_vector.push_back(resolved_parameter_type);
return Transform_lambda_parameter_types_fold_state{state.type_environment, state.parameter_type_vector, (state.next_explicit_type_position + 1)};
}
std::shared_ptr<semantic_ir::SExpr> transform_expr_lambda_with_param_types(mlc::Array<mlc::String> parameter_names, mlc::Array<std::shared_ptr<registry::Type>> parameter_types, std::shared_ptr<ast::Expr> body, ast::Span source_span, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
auto lambda_environment = transform_context.type_env;
auto lambda_parameter_accumulator_after_bind = parameter_names.fold(Transform_lambda_parameter_types_fold_state{lambda_environment, {}, 0}, [=](Transform_lambda_parameter_types_fold_state accumulator, mlc::String parameter_binding_name) mutable { return transform_lambda_parameter_types_environment_fold_step(accumulator, parameter_binding_name, parameter_types); });
auto lambda_context = transform_context_with_env(transform_context, lambda_parameter_accumulator_after_bind.type_environment);
auto typed_body = transform_expr(body, lambda_context, stmts_fn);
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprLambda{parameter_names, typed_body, std::make_shared<registry::Type>(registry::TFn{lambda_parameter_accumulator_after_bind.parameter_type_vector, semantic_ir::sexpr_type(typed_body)}), source_span});
}
std::shared_ptr<semantic_ir::SExpr> transform_expr_maybe_lambda_context(std::shared_ptr<ast::Expr> expression, mlc::Array<std::shared_ptr<registry::Type>> expected_param_types, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
return std::visit(overloaded{[&](const ast::ExprLambda& exprLambda) { auto [parameter_names, body, source_span] = exprLambda; return transform_expr_lambda_with_param_types(parameter_names, expected_param_types, body, source_span, transform_context, stmts_fn); },
[&](const auto& __v) { return transform_expr(expression, transform_context, stmts_fn); }
}, (*expression));
}
mlc::Array<std::shared_ptr<semantic_ir::SExpr>> transform_array_hof_method_arguments(std::shared_ptr<semantic_ir::SExpr> typed_object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
auto element_type = semantic_type_structure::array_element_type_from_array_type(semantic_ir::sexpr_type(typed_object));
auto out = mlc::Array<std::shared_ptr<semantic_ir::SExpr>>{};
if ((method_name == mlc::String("fold", 4))) {
if ((method_arguments.length() > 0)) {
out.push_back(transform_expr(method_arguments[0], transform_context, stmts_fn));
if ((method_arguments.length() > 1)) {
auto acc_t = semantic_ir::sexpr_type(out[0]);
out.push_back(transform_expr_maybe_lambda_context(method_arguments[1], mlc::Array<std::shared_ptr<registry::Type>>{acc_t, element_type}, transform_context, stmts_fn));
}
}
} else if (((method_name == mlc::String("zip", 3)) || (method_name == mlc::String("join", 4)))) {
if ((method_arguments.length() > 0)) {
out.push_back(transform_expr(method_arguments[0], transform_context, stmts_fn));
}
} else if (((method_name == mlc::String("take", 4)) || (method_name == mlc::String("drop", 4)))) {
if ((method_arguments.length() > 0)) {
out.push_back(transform_expr(method_arguments[0], transform_context, stmts_fn));
}
} else if ((((method_name == mlc::String("enumerate", 9)) || (method_name == mlc::String("sum", 3))) || (method_name == mlc::String("flat", 4)))) {
auto _skip = 0;
} else {
if ((method_arguments.length() > 0)) {
out.push_back(transform_expr_maybe_lambda_context(method_arguments[0], mlc::Array<std::shared_ptr<registry::Type>>{element_type}, transform_context, stmts_fn));
}
}



return out;
}
mlc::Array<std::shared_ptr<registry::Type>> inferred_types_from_record_literal_part_for_merge(ast::RecordLitPart literal_record_part, check_context::CheckContext inference_context_for_spread) noexcept{
return std::visit(overloaded{[&](const ast::RecordLitSpread& recordLitSpread) { auto [spread_expression] = recordLitSpread; return mlc::Array<std::shared_ptr<registry::Type>>{infer::infer_expr(spread_expression, inference_context_for_spread).inferred_type}; },
[&](const ast::RecordLitFields& recordLitFields) { auto [__0] = recordLitFields; return {}; }
}, literal_record_part);
}
std::shared_ptr<semantic_ir::SExpr> transform_expr(std::shared_ptr<ast::Expr> expression, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
auto expression_partial_application = partial_application_desugar_expr(expression);
return std::visit(overloaded{[&](const ast::ExprInt& exprInt) { auto [value, source_span] = exprInt; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprInt{value, std::make_shared<registry::Type>(registry::TI32{}), source_span}); },
[&](const ast::ExprStr& exprStr) { auto [value, source_span] = exprStr; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprStr{value, std::make_shared<registry::Type>(registry::TString{}), source_span}); },
[&](const ast::ExprBool& exprBool) { auto [value, source_span] = exprBool; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBool{value, std::make_shared<registry::Type>(registry::TBool{}), source_span}); },
[&](const ast::ExprUnit& exprUnit) { auto [source_span] = exprUnit; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUnit{std::make_shared<registry::Type>(registry::TUnit{}), source_span}); },
[&](const ast::ExprFloat& exprFloat) { auto [value, source_span] = exprFloat; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprFloat{value, std::make_shared<registry::Type>(registry::TF64{}), source_span}); },
[&](const ast::ExprI64& exprI64) { auto [value, source_span] = exprI64; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprI64{value, std::make_shared<registry::Type>(registry::TI64{}), source_span}); },
[&](const ast::ExprU8& exprU8) { auto [value, source_span] = exprU8; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprU8{value, std::make_shared<registry::Type>(registry::TU8{}), source_span}); },
[&](const ast::ExprUsize& exprUsize) { auto [value, source_span] = exprUsize; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUsize{value, std::make_shared<registry::Type>(registry::TUsize{}), source_span}); },
[&](const ast::ExprChar& exprChar) { auto [value, source_span] = exprChar; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprChar{value, std::make_shared<registry::Type>(registry::TChar{}), source_span}); },
[&](const ast::ExprExtern& exprExtern) { auto [source_span] = exprExtern; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprExtern{std::make_shared<registry::Type>(registry::TUnit{}), source_span}); },
[&](const ast::ExprIdent& exprIdent) { auto [name, source_span] = exprIdent; return [&]() {
auto resolved_type = (transform_context.type_env.has(name) ? (transform_context.type_env.get(name)) : ((registry::TypeRegistry_has_fn(transform_context.registry, name) ? (registry::TypeRegistry_fn_type(transform_context.registry, name)) : ((registry::TypeRegistry_has_ctor(transform_context.registry, name) ? (registry::TypeRegistry_ctor_type(transform_context.registry, name)) : (std::make_shared<registry::Type>(registry::TUnknown{})))))));
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprIdent{name, resolved_type, source_span});
}(); },
[&](const ast::ExprBin& exprBin) { auto [operation, left, right, source_span] = exprBin; return [&]() {
auto typed_left = transform_expr(left, transform_context, stmts_fn);
auto typed_right = transform_expr(right, transform_context, stmts_fn);
auto left_type = semantic_ir::sexpr_type(typed_left);
auto method = semantic_type_structure::operator_method_for(operation);
auto result_type = ((method != mlc::String("", 0)) ? ([&]() {
auto from_reg = registry::method_return_type_from_object(left_type, method, transform_context.registry);
return (semantic_type_structure::type_is_unknown(from_reg) ? (semantic_type_structure::binary_operation_result_type(operation, left_type)) : (from_reg));
}()) : (semantic_type_structure::binary_operation_result_type(operation, left_type)));
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBin{operation, typed_left, typed_right, result_type, source_span});
}(); },
[&](const ast::ExprUn& exprUn) { auto [operation, inner, source_span] = exprUn; return [&]() {
auto typed_inner = transform_expr(inner, transform_context, stmts_fn);
auto result_type = ((operation == mlc::String("!", 1)) ? (std::make_shared<registry::Type>(registry::TBool{})) : (semantic_ir::sexpr_type(typed_inner)));
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUn{operation, typed_inner, result_type, source_span});
}(); },
[&](const ast::ExprCall& exprCall) { auto [function, call_arguments, source_span] = exprCall; return [&]() {
auto callee_name = std::visit(overloaded{[&](const ast::ExprIdent& exprIdent) { auto [name, __1] = exprIdent; return name; },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*function));
auto resolved_call_args = reorder_named_args_for_transform(call_arguments, callee_name, transform_context.registry);
auto typed_fn = transform_expr(function, transform_context, stmts_fn);
auto typed_args = transform_call_arguments_using_callee_semantic_type(typed_fn, resolved_call_args, transform_context, stmts_fn);
auto result_type = (((callee_name != mlc::String("", 0)) && registry::TypeRegistry_has_ctor(transform_context.registry, callee_name)) ? (registry::TypeRegistry_ctor_type(transform_context.registry, callee_name)) : (std::visit(overloaded{[&](const registry::TFn& tFn) { auto [__0, return_type] = tFn; return return_type; },
[&](const auto& __v) { return std::make_shared<registry::Type>(registry::TUnknown{}); }
}, (*semantic_ir::sexpr_type(typed_fn)))));
auto call_parameter_mutability_flags = direct_call_parameter_mutability_flags(transform_context.registry, callee_name, typed_args.length());
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprCall{typed_fn, typed_args, call_parameter_mutability_flags, result_type, source_span});
}(); },
[&](const ast::ExprMethod& exprMethod) { auto [object, method_name, method_arguments, source_span] = exprMethod; return [&]() {
auto typed_object = transform_expr(object, transform_context, stmts_fn);
return (result_option_method_types::should_infer_result_option_combinator(semantic_ir::sexpr_type(typed_object), method_name) ? ([&]() {
auto typed_args = transform_exprs(method_arguments, transform_context, stmts_fn);
auto arg_types = typed_args.map([=](std::shared_ptr<semantic_ir::SExpr> typed_argument) mutable { return semantic_ir::sexpr_type(typed_argument); });
auto result_type = result_option_method_types::result_option_hof_call_result_type(semantic_ir::sexpr_type(typed_object), method_name, arg_types);
auto method_parameter_mutability_flags = instance_method_receiver_and_parameters_mutability_pattern(transform_context.registry, semantic_ir::sexpr_type(typed_object), method_name, typed_args.length());
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMethod{typed_object, method_name, typed_args, method_parameter_mutability_flags, result_type, source_span});
}()) : (((semantic_type_structure::type_is_array(semantic_ir::sexpr_type(typed_object)) && array_method_types::is_array_hof_method(method_name)) ? ([&]() {
auto typed_args = transform_array_hof_method_arguments(typed_object, method_name, method_arguments, transform_context, stmts_fn);
auto arg_types = typed_args.map([=](std::shared_ptr<semantic_ir::SExpr> typed_argument) mutable { return semantic_ir::sexpr_type(typed_argument); });
auto elem = semantic_type_structure::array_element_type_from_array_type(semantic_ir::sexpr_type(typed_object));
auto result_type = array_method_types::array_hof_call_result_type(elem, method_name, arg_types);
auto method_parameter_mutability_flags = instance_method_receiver_and_parameters_mutability_pattern(transform_context.registry, semantic_ir::sexpr_type(typed_object), method_name, typed_args.length());
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMethod{typed_object, method_name, typed_args, method_parameter_mutability_flags, result_type, source_span});
}()) : ([&]() {
auto typed_args = transform_exprs(method_arguments, transform_context, stmts_fn);
auto builtin_type = semantic_type_structure::builtin_method_return_type(method_name);
auto result_type = (semantic_type_structure::type_is_unknown(builtin_type) ? (registry::method_return_type_from_object(semantic_ir::sexpr_type(typed_object), method_name, transform_context.registry)) : (builtin_type));
auto method_parameter_mutability_flags = instance_method_receiver_and_parameters_mutability_pattern(transform_context.registry, semantic_ir::sexpr_type(typed_object), method_name, typed_args.length());
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMethod{typed_object, method_name, typed_args, method_parameter_mutability_flags, result_type, source_span});
}()))));
}(); },
[&](const ast::ExprField& exprField) { auto [object, field_name, source_span] = exprField; return [&]() {
auto typed_object = transform_expr(object, transform_context, stmts_fn);
auto result_type = registry::field_type_from_object(semantic_ir::sexpr_type(typed_object), field_name, transform_context.registry);
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprField{typed_object, field_name, result_type, source_span});
}(); },
[&](const ast::ExprIndex& exprIndex) { auto [object, index_expression, source_span] = exprIndex; return [&]() {
auto typed_object = transform_expr(object, transform_context, stmts_fn);
auto typed_index = transform_expr(index_expression, transform_context, stmts_fn);
auto element_type = std::visit(overloaded{[&](const registry::TArray& tArray) { auto [inner] = tArray; return inner; },
[&](const auto& __v) { return std::make_shared<registry::Type>(registry::TUnknown{}); }
}, (*semantic_ir::sexpr_type(typed_object)));
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprIndex{typed_object, typed_index, element_type, source_span});
}(); },
[&](const ast::ExprIf& exprIf) { auto [condition, then_expression, else_expression, source_span] = exprIf; return [&]() {
auto typed_condition = transform_expr(condition, transform_context, stmts_fn);
auto typed_then = transform_expr(then_expression, transform_context, stmts_fn);
auto typed_else_raw = transform_expr(else_expression, transform_context, stmts_fn);
auto typed_else = conditional_else_empty_unknown_array_coerced_to_then_array_element(typed_then, typed_else_raw);
auto merged_conditional_type = std::visit(overloaded{[&](const registry::TArray& tArray) { auto [inner_then] = tArray; return std::visit(overloaded{[&](const registry::TArray& tArray) { auto [inner_else] = tArray; return (semantic_type_structure::type_is_unknown(inner_then) ? (std::make_shared<registry::Type>(registry::TArray{inner_else})) : (std::make_shared<registry::Type>(registry::TArray{inner_then}))); },
[&](const auto& __v) { return semantic_ir::sexpr_type(typed_then); }
}, (*semantic_ir::sexpr_type(typed_else))); },
[&](const auto& __v) { return semantic_ir::sexpr_type(typed_then); }
}, (*semantic_ir::sexpr_type(typed_then)));
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprIf{typed_condition, typed_then, typed_else, merged_conditional_type, source_span});
}(); },
[&](const ast::ExprBlock& exprBlock) { auto [statements, result, source_span] = exprBlock; return [&]() {
auto stmts_result = stmts_fn(statements, transform_context);
auto body_context = transform_context_with_env(transform_context, stmts_result.type_env);
auto typed_result = transform_expr(result, body_context, stmts_fn);
auto result_type = semantic_ir::sexpr_type(typed_result);
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBlock{stmts_result.statements, typed_result, result_type, source_span});
}(); },
[&](const ast::ExprWhile& exprWhile) { auto [condition, statements, source_span] = exprWhile; return [&]() {
auto typed_condition = transform_expr(condition, transform_context, stmts_fn);
auto stmts_result = stmts_fn(statements, transform_context);
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprWhile{typed_condition, stmts_result.statements, std::make_shared<registry::Type>(registry::TUnit{}), source_span});
}(); },
[&](const ast::ExprFor& exprFor) { auto [variable_name, iterator, statements, source_span] = exprFor; return [&]() {
auto typed_iterator = transform_expr(iterator, transform_context, stmts_fn);
auto element_type = std::visit(overloaded{[&](const registry::TArray& tArray) { auto [inner] = tArray; return inner; },
[&](const auto& __v) { return std::make_shared<registry::Type>(registry::TUnknown{}); }
}, (*semantic_ir::sexpr_type(typed_iterator)));
auto inner_env = transform_context.type_env;
inner_env.set(variable_name, element_type);
auto loop_context = transform_context_with_env(transform_context, inner_env);
auto stmts_result = stmts_fn(statements, loop_context);
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprFor{variable_name, typed_iterator, stmts_result.statements, std::make_shared<registry::Type>(registry::TUnit{}), source_span});
}(); },
[&](const ast::ExprMatch& exprMatch) { auto [subject, arms, source_span] = exprMatch; return [&]() {
auto typed_subject = transform_expr(subject, transform_context, stmts_fn);
auto substitution = build_transform_substitution(semantic_ir::sexpr_type(typed_subject), transform_context.registry);
auto typed_arms = transform_match_arms(arms, transform_context, substitution, semantic_ir::sexpr_type(typed_subject), stmts_fn);
auto result_type = ((typed_arms.length() > 0) ? (semantic_ir::sexpr_type(typed_arms[0]->body)) : (std::make_shared<registry::Type>(registry::TUnknown{})));
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMatch{typed_subject, typed_arms, result_type, source_span});
}(); },
[&](const ast::ExprRecord& exprRecord) { auto [type_name, lit_parts, source_span] = exprRecord; return [&]() {
auto merged_field_values_for_transform = mlc::Array<std::shared_ptr<ast::FieldVal>>{};
if ((!record_lit_merge::record_literal_contains_spread(lit_parts))) {
merged_field_values_for_transform = record_lit_merge::collect_explicit_field_values_flat(lit_parts);
} else {
auto inference_context_for_literal_spreads = check_context::check_context_new(transform_context.type_env, transform_context.registry);
auto spread_types_for_merge = lit_parts.flat_map([=](ast::RecordLitPart part_under_merge) mutable { return inferred_types_from_record_literal_part_for_merge(part_under_merge, inference_context_for_literal_spreads); });
merged_field_values_for_transform = record_lit_merge::merge_record_literal_parts_to_field_values(type_name, lit_parts, transform_context.registry, spread_types_for_merge);
}
merged_field_values_for_transform = record_lit_merge::merge_explicit_record_literal_with_type_defaults(type_name, merged_field_values_for_transform, transform_context.registry);
auto typed_fields = transform_field_vals(merged_field_values_for_transform, transform_context, stmts_fn);
auto phantom = registry::TypeRegistry_phantom_type_params_for(transform_context.registry, type_name);
auto all_params = registry::TypeRegistry_algebraic_decl_type_parameter_names_for(transform_context.registry, type_name);
auto expr_type = (((all_params.length() > 0) && (phantom.length() == all_params.length())) ? (std::make_shared<registry::Type>(registry::TGeneric{type_name, all_params.map([=](mlc::String _algebraic_type_parameter_placeholder) mutable { return standalone_unknown_cell(); })})) : (std::make_shared<registry::Type>(registry::TNamed{type_name})));
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprRecord{type_name, typed_fields, expr_type, source_span});
}(); },
[&](const ast::ExprRecordUpdate& exprRecordUpdate) { auto [type_name, base, field_values, source_span] = exprRecordUpdate; return [&]() {
auto typed_base = transform_expr(base, transform_context, stmts_fn);
auto typed_fields = transform_field_vals(field_values, transform_context, stmts_fn);
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprRecordUpdate{type_name, typed_base, typed_fields, std::make_shared<registry::Type>(registry::TNamed{type_name}), source_span});
}(); },
[&](const ast::ExprArray& exprArray) { auto [elements, source_span] = exprArray; return [&]() {
auto typed_elements = transform_exprs(elements, transform_context, stmts_fn);
auto element_type = ((typed_elements.length() > 0) ? (semantic_ir::sexpr_type(typed_elements[0])) : (std::make_shared<registry::Type>(registry::TUnknown{})));
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprArray{typed_elements, std::make_shared<registry::Type>(registry::TArray{element_type}), source_span});
}(); },
[&](const ast::ExprTuple& exprTuple) { auto [elements, source_span] = exprTuple; return ((elements.length() < 2) ? (std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUnit{std::make_shared<registry::Type>(registry::TUnit{}), ast::span_unknown()})) : ([&]() {
auto typed = transform_exprs(elements, transform_context, stmts_fn);
auto tuple_types = typed.map([=](std::shared_ptr<semantic_ir::SExpr> typed_tuple_member) mutable { return semantic_ir::sexpr_type(typed_tuple_member); });
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprTuple{typed, std::make_shared<registry::Type>(registry::TTuple{tuple_types}), source_span});
}())); },
[&](const ast::ExprQuestion& exprQuestion) { auto [inner, source_span] = exprQuestion; return [&]() {
auto typed_inner = transform_expr(inner, transform_context, stmts_fn);
auto inner_type = semantic_ir::sexpr_type(typed_inner);
auto unwrapped_type = std::visit(overloaded{[&](const registry::TGeneric& tGeneric) { auto [__0, type_args] = tGeneric; return ((type_args.length() > 0) ? (type_args[0]) : (std::make_shared<registry::Type>(registry::TUnknown{}))); },
[&](const auto& __v) { return std::make_shared<registry::Type>(registry::TUnknown{}); }
}, (*inner_type));
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprQuestion{typed_inner, unwrapped_type, source_span});
}(); },
[&](const ast::ExprLambda& exprLambda) { auto [parameter_names, body, source_span] = exprLambda; return [&]() {
auto lambda_environment = transform_context.type_env;
auto parameter_types = parameter_names.map([=](mlc::String parameter_binding_placeholder) mutable { return standalone_unknown_cell(); });
parameter_names.fold(lambda_environment, [=](mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> environment_map, mlc::String parameter_binding_name) mutable { return lambda_environment_assign_unknown_placeholder(environment_map, parameter_binding_name); });
auto lambda_context = transform_context_with_env(transform_context, lambda_environment);
auto typed_body = transform_expr(body, lambda_context, stmts_fn);
auto function_type = std::make_shared<registry::Type>(registry::TFn{parameter_types, semantic_ir::sexpr_type(typed_body)});
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprLambda{parameter_names, typed_body, function_type, source_span});
}(); },
[&](const ast::ExprNamedArg& exprNamedArg) { auto [__0, inner, __2] = exprNamedArg; return transform_expr(inner, transform_context, stmts_fn); },
[&](const ast::ExprWith& exprWith) { auto [resource, binder, stmts, source_span] = exprWith; return [&]() {
auto typed_resource = transform_expr(resource, transform_context, stmts_fn);
auto inner_env = transform_context.type_env;
inner_env.set(binder, semantic_ir::sexpr_type(typed_resource));
auto inner_context = transform_context_with_env(transform_context, inner_env);
auto stmts_result = stmts_fn(stmts, inner_context);
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprWith{typed_resource, binder, stmts_result.statements, std::make_shared<registry::Type>(registry::TUnit{}), source_span});
}(); },
[&](const auto& __v) { return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUnit{std::make_shared<registry::Type>(registry::TUnit{}), ast::span_unknown()}); }
}, expression_partial_application);
}

} // namespace transform
