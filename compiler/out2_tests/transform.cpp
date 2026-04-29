#define main mlc_user_main
#include "transform.hpp"

namespace transform {

TransformContext transform_context_new(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, registry::TypeRegistry registry) noexcept{
return TransformContext{type_env, registry};
}
TransformContext empty_transform_context() noexcept{
return TransformContext{{}, registry::empty_registry()};
}
TransformContext transform_context_with_env(TransformContext base, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env) noexcept{
return TransformContext{type_env, base.registry};
}
std::shared_ptr<semantic_ir::SExpr> coerce_expr_to_type(std::shared_ptr<semantic_ir::SExpr> expression, std::shared_ptr<registry::Type> target_type) noexcept{
return std::visit(overloaded{[&](const semantic_ir::SExprArray& sExprArray) { auto [elements, __1, source_span] = sExprArray; return ((elements.length() == 0) ? std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprArray{elements, target_type, source_span}) : expression); },
[&](const auto& __v) { return expression; }
}, (*expression));
}
mlc::Array<std::shared_ptr<semantic_ir::SExpr>> transform_exprs(mlc::Array<std::shared_ptr<ast::Expr>> expressions, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
auto result = mlc::Array<std::shared_ptr<semantic_ir::SExpr>>{};
auto index = 0;
while ((index < expressions.length())) {
result.push_back(transform_expr(expressions[index], transform_context, stmts_fn));
index = (index + 1);
}
return result;
}
mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> transform_field_vals(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
auto result = mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>>{};
auto index = 0;
while ((index < field_values.length())) {
auto typed_val = transform_expr(field_values[index]->val, transform_context, stmts_fn);
result.push_back(std::make_shared<semantic_ir::SFieldVal>(semantic_ir::SFieldVal{field_values[index]->name, typed_val}));
index = (index + 1);
}
return result;
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> build_transform_substitution(std::shared_ptr<registry::Type> subject_type, registry::TypeRegistry registry) noexcept{
auto substitution = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
std::visit(overloaded{[&](const registry::TGeneric& tGeneric) { auto [type_name, type_args] = tGeneric; return [&]() {
auto param_names = registry::TypeRegistry_algebraic_decl_type_parameter_names_for(registry, type_name);
auto i = 0;
return [&]() {
while (((i < param_names.length()) && (i < type_args.length()))) {
substitution.set(param_names[i], type_args[i]);
i = (i + 1);
}
}();
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*subject_type));
return substitution;
}
mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> transform_match_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, TransformContext transform_context, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
auto result = mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>>{};
auto index = 0;
while ((index < arms.length())) {
auto arm_env = pattern_env::env_for_pattern_substituted(transform_context.type_env, arms[index]->pat, transform_context.registry, substitution);
auto arm_context = transform_context_with_env(transform_context, arm_env);
auto typed_body = transform_expr(arms[index]->body, arm_context, stmts_fn);
result.push_back(std::make_shared<semantic_ir::SMatchArm>(semantic_ir::SMatchArm{arms[index]->pat, typed_body}));
index = (index + 1);
}
return result;
}
std::shared_ptr<semantic_ir::SExpr> transform_expr_lambda_with_param_types(mlc::Array<mlc::String> parameter_names, mlc::Array<std::shared_ptr<registry::Type>> parameter_types, std::shared_ptr<ast::Expr> body, ast::Span source_span, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
auto lambda_env = transform_context.type_env;
auto parameter_type_vec = mlc::Array<std::shared_ptr<registry::Type>>{};
auto parameter_index = 0;
while ((parameter_index < parameter_names.length())) {
auto pt = ((parameter_index < parameter_types.length()) ? parameter_types[parameter_index] : [&]() {
std::make_shared<registry::Type>(registry::TUnknown{});
parameter_type_vec.push_back(pt);
lambda_env.set(parameter_names[parameter_index], pt);
parameter_index = (parameter_index + 1);
return /* unit */;
}());
auto lambda_context = transform_context_with_env(transform_context, lambda_env);
auto typed_body = transform_expr(body, lambda_context, stmts_fn);
std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprLambda{parameter_names, typed_body, std::make_shared<registry::Type>(registry::TFn{parameter_type_vec, semantic_ir::sexpr_type(typed_body)}), source_span});
}
/* unit */;
transform_expr_maybe_lambda_context(expression);
(Shared{} < ast::Expr{});
/* unit */;
/* unit */;
expected_param_types;
/* unit */[(Shared{} < registry::Type{})];
/* unit */;
/* unit */;
transform_context;
/* unit */;
TransformContext{};
/* unit */;
stmts_fn;
/* unit */(mlc::Array{(Shared{} < ast::Stmt{})});
/* unit */;
TransformContext{};
/* unit */;
/* unit */;
TransformStmtsResult{};
/* unit */;
/* unit */;
(Shared{} < semantic_ir::SExpr{});
/* unit */ = std::visit(overloaded{[&](const ast::ExprLambda& exprLambda) { auto [parameter_names, body, source_span] = exprLambda; return transform_expr_lambda_with_param_types(parameter_names, expected_param_types, body, source_span, transform_context, stmts_fn); },
[&](const auto& __v) { return transform_expr(expression, transform_context, stmts_fn); }
}, expression);
/* unit */;
transform_array_hof_method_arguments(typed_object);
(Shared{} < semantic_ir::SExpr{});
/* unit */;
/* unit */;
method_name;
/* unit */;
string;
/* unit */;
method_arguments;
/* unit */[(Shared{} < ast::Expr{})];
/* unit */;
/* unit */;
transform_context;
/* unit */;
TransformContext{};
/* unit */;
stmts_fn;
/* unit */(mlc::Array{(Shared{} < ast::Stmt{})});
/* unit */;
TransformContext{};
/* unit */;
/* unit */;
TransformStmtsResult{};
/* unit */;
/* unit */[(Shared{} < semantic_ir::SExpr{})];
/* unit */ = [&]() {
auto element_type = semantic_type_structure::array_element_type_from_array_type(semantic_ir::sexpr_type(typed_object));
auto out = mlc::Array<std::shared_ptr<semantic_ir::SExpr>>{};
if ((method_name == mlc::String("fold", 4))) {
if ((method_arguments.length() > 0)) {
out.push_back(transform_expr(method_arguments[0], transform_context, stmts_fn));
if ((method_arguments.length() > 1)) {
auto acc_t = semantic_ir::sexpr_type(out[0]);
out.push_back(transform_expr_maybe_lambda_context(method_arguments[1], mlc::Array{acc_t, element_type}, transform_context, stmts_fn));
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
} else if ((method_arguments.length() > 0)) {
out.push_back(transform_expr_maybe_lambda_context(method_arguments[0], mlc::Array{element_type}, transform_context, stmts_fn));
}




return /* unit */;
}();
return out;
}
std::shared_ptr<semantic_ir::SExpr> transform_expr(std::shared_ptr<ast::Expr> expression, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept{
return std::visit(overloaded{[&](const ast::ExprInt& exprInt) { auto [value, source_span] = exprInt; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprInt{value, std::make_shared<registry::Type>(registry::TI32{}), source_span}); },
[&](const ast::ExprStr& exprStr) { auto [value, source_span] = exprStr; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprStr{value, std::make_shared<registry::Type>(registry::TString{}), source_span}); },
[&](const ast::ExprBool& exprBool) { auto [value, source_span] = exprBool; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBool{value, std::make_shared<registry::Type>(registry::TBool{}), source_span}); },
[&](const ast::ExprUnit& exprUnit) { auto [source_span] = exprUnit; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUnit{std::make_shared<registry::Type>(registry::TUnit{}), source_span}); },
[&](const ast::ExprExtern& exprExtern) { auto [source_span] = exprExtern; return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprExtern{std::make_shared<registry::Type>(registry::TUnit{}), source_span}); },
[&](const ast::ExprIdent& exprIdent) { auto [name, source_span] = exprIdent; return [&]() {
auto resolved_type = (transform_context.type_env.has(name) ? transform_context.type_env.get(name) : (registry::TypeRegistry_has_fn(transform_context.registry, name) ? registry::TypeRegistry_fn_type(transform_context.registry, name) : (registry::TypeRegistry_has_ctor(transform_context.registry, name) ? registry::TypeRegistry_ctor_type(transform_context.registry, name) : std::make_shared<registry::Type>(registry::TUnknown{}))));
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprIdent{name, resolved_type, source_span});
}(); },
[&](const ast::ExprBin& exprBin) { auto [operation, left, right, source_span] = exprBin; return [&]() {
auto typed_left = transform_expr(left, transform_context, stmts_fn);
auto typed_right = transform_expr(right, transform_context, stmts_fn);
auto result_type = semantic_type_structure::binary_operation_result_type(operation, semantic_ir::sexpr_type(typed_left));
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBin{operation, typed_left, typed_right, result_type, source_span});
}(); },
[&](const ast::ExprUn& exprUn) { auto [operation, inner, source_span] = exprUn; return [&]() {
auto typed_inner = transform_expr(inner, transform_context, stmts_fn);
auto result_type = ((operation == mlc::String("!", 1)) ? std::make_shared<registry::Type>(registry::TBool{}) : semantic_ir::sexpr_type(typed_inner));
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUn{operation, typed_inner, result_type, source_span});
}(); },
[&](const ast::ExprCall& exprCall) { auto [function, call_arguments, source_span] = exprCall; return [&]() {
auto typed_fn = transform_expr(function, transform_context, stmts_fn);
auto typed_args = transform_exprs(call_arguments, transform_context, stmts_fn);
auto callee_name = std::visit(overloaded{[&](const ast::ExprIdent& exprIdent) { auto [name, __1] = exprIdent; return name; },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*function));
auto result_type = (((callee_name != mlc::String("", 0)) && registry::TypeRegistry_has_ctor(transform_context.registry, callee_name)) ? registry::TypeRegistry_ctor_type(transform_context.registry, callee_name) : std::visit(overloaded{[&](const registry::TFn& tFn) { auto [__0, return_type] = tFn; return return_type; },
[&](const auto& __v) { return std::make_shared<registry::Type>(registry::TUnknown{}); }
}, (*semantic_ir::sexpr_type(typed_fn))));
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprCall{typed_fn, typed_args, result_type, source_span});
}(); },
[&](const ast::ExprMethod& exprMethod) { auto [object, method_name, method_arguments, source_span] = exprMethod; return [&]() {
auto typed_object = transform_expr(object, transform_context, stmts_fn);
return (result_option_method_types::should_infer_result_option_combinator(semantic_ir::sexpr_type(typed_object), method_name) ? [&]() {
auto typed_args = transform_exprs(method_arguments, transform_context, stmts_fn);
auto arg_types = mlc::Array<std::shared_ptr<registry::Type>>{};
auto arg_index = 0;
while ((arg_index < typed_args.length())) {
arg_types.push_back(semantic_ir::sexpr_type(typed_args[arg_index]));
arg_index = (arg_index + 1);
}
auto result_type = result_option_method_types::result_option_hof_call_result_type(semantic_ir::sexpr_type(typed_object), method_name, arg_types);
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMethod{typed_object, method_name, typed_args, result_type, source_span});
}() : ((semantic_type_structure::type_is_array(semantic_ir::sexpr_type(typed_object)) && array_method_types::is_array_hof_method(method_name)) ? [&]() {
auto typed_args = transform_array_hof_method_arguments(typed_object, method_name, method_arguments, transform_context, stmts_fn);
auto arg_types = mlc::Array<std::shared_ptr<registry::Type>>{};
auto arg_index = 0;
while ((arg_index < typed_args.length())) {
arg_types.push_back(semantic_ir::sexpr_type(typed_args[arg_index]));
arg_index = (arg_index + 1);
}
auto elem = semantic_type_structure::array_element_type_from_array_type(semantic_ir::sexpr_type(typed_object));
auto result_type = array_method_types::array_hof_call_result_type(elem, method_name, arg_types);
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMethod{typed_object, method_name, typed_args, result_type, source_span});
}() : [&]() {
auto typed_args = transform_exprs(method_arguments, transform_context, stmts_fn);
auto builtin_type = semantic_type_structure::builtin_method_return_type(method_name);
auto result_type = (semantic_type_structure::type_is_unknown(builtin_type) ? registry::method_return_type_from_object(semantic_ir::sexpr_type(typed_object), method_name, transform_context.registry) : builtin_type);
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMethod{typed_object, method_name, typed_args, result_type, source_span});
}()));
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
auto typed_else = transform_expr(else_expression, transform_context, stmts_fn);
auto result_type = semantic_ir::sexpr_type(typed_then);
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprIf{typed_condition, typed_then, typed_else, result_type, source_span});
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
auto typed_arms = transform_match_arms(arms, transform_context, substitution, stmts_fn);
auto result_type = ((typed_arms.length() > 0) ? semantic_ir::sexpr_type(typed_arms[0]->body) : std::make_shared<registry::Type>(registry::TUnknown{}));
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMatch{typed_subject, typed_arms, result_type, source_span});
}(); },
[&](const ast::ExprRecord& exprRecord) { auto [type_name, field_values, source_span] = exprRecord; return [&]() {
auto typed_fields = transform_field_vals(field_values, transform_context, stmts_fn);
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprRecord{type_name, typed_fields, std::make_shared<registry::Type>(registry::TNamed{type_name}), source_span});
}(); },
[&](const ast::ExprRecordUpdate& exprRecordUpdate) { auto [type_name, base, field_values, source_span] = exprRecordUpdate; return [&]() {
auto typed_base = transform_expr(base, transform_context, stmts_fn);
auto typed_fields = transform_field_vals(field_values, transform_context, stmts_fn);
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprRecordUpdate{type_name, typed_base, typed_fields, std::make_shared<registry::Type>(registry::TNamed{type_name}), source_span});
}(); },
[&](const ast::ExprArray& exprArray) { auto [elements, source_span] = exprArray; return [&]() {
auto typed_elements = transform_exprs(elements, transform_context, stmts_fn);
auto element_type = ((typed_elements.length() > 0) ? semantic_ir::sexpr_type(typed_elements[0]) : std::make_shared<registry::Type>(registry::TUnknown{}));
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprArray{typed_elements, std::make_shared<registry::Type>(registry::TArray{element_type}), source_span});
}(); },
[&](const ast::ExprTuple& exprTuple) { auto [elements, source_span] = exprTuple; return ((elements.length() < 2) ? std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUnit{std::make_shared<registry::Type>(registry::TUnit{}), ast::span_unknown()}) : [&]() {
auto typed = transform_exprs(elements, transform_context, stmts_fn);
auto tuple_types = mlc::Array<std::shared_ptr<registry::Type>>{};
auto ti = 0;
while ((ti < typed.length())) {
tuple_types.push_back(semantic_ir::sexpr_type(typed[ti]));
ti = (ti + 1);
}
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprTuple{typed, std::make_shared<registry::Type>(registry::TTuple{tuple_types}), source_span});
}()); },
[&](const ast::ExprQuestion& exprQuestion) { auto [inner, source_span] = exprQuestion; return [&]() {
auto typed_inner = transform_expr(inner, transform_context, stmts_fn);
auto inner_type = semantic_ir::sexpr_type(typed_inner);
auto unwrapped_type = std::visit(overloaded{[&](const registry::TGeneric& tGeneric) { auto [__0, type_args] = tGeneric; return ((type_args.length() > 0) ? type_args[0] : std::make_shared<registry::Type>(registry::TUnknown{})); },
[&](const auto& __v) { return std::make_shared<registry::Type>(registry::TUnknown{}); }
}, (*inner_type));
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprQuestion{typed_inner, unwrapped_type, source_span});
}(); },
[&](const ast::ExprLambda& exprLambda) { auto [parameter_names, body, source_span] = exprLambda; return [&]() {
auto lambda_env = transform_context.type_env;
auto parameter_types = mlc::Array<std::shared_ptr<registry::Type>>{};
auto parameter_index = 0;
while ((parameter_index < parameter_names.length())) {
parameter_types.push_back(std::make_shared<registry::Type>(registry::TUnknown{}));
lambda_env.set(parameter_names[parameter_index], std::make_shared<registry::Type>(registry::TUnknown{}));
parameter_index = (parameter_index + 1);
}
auto lambda_context = transform_context_with_env(transform_context, lambda_env);
auto typed_body = transform_expr(body, lambda_context, stmts_fn);
auto function_type = std::make_shared<registry::Type>(registry::TFn{parameter_types, semantic_ir::sexpr_type(typed_body)});
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprLambda{parameter_names, typed_body, function_type, source_span});
}(); },
[&](const auto& __v) { return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUnit{std::make_shared<registry::Type>(registry::TUnit{}), ast::span_unknown()}); }
}, (*expression));
}

} // namespace transform
