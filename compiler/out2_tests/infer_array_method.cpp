#define main mlc_user_main
#include "infer_array_method.hpp"

namespace infer_array_method {

void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
infer_result::InferResult infer_expr_maybe_lambda(std::shared_ptr<ast::Expr> expression, mlc::Array<std::shared_ptr<registry::Type>> expected_param_types, check_context::CheckContext inference_context, InferExprFn infer_expr_fn) noexcept{
return std::visit(overloaded{[&](const ast::ExprLambda& exprLambda) { auto [parameter_names, body, __2] = exprLambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, expected_param_types, body, inference_context, infer_expr_fn); },
[&](const auto& __v) { return infer_expr_fn(expression, inference_context); }
}, (*expression));
}
mlc::Array<ast::Diagnostic> append_lambda_arity_diag(mlc::Array<ast::Diagnostic> errors, std::shared_ptr<ast::Expr> expression, int expected_params, mlc::String method_name) noexcept{
return std::visit(overloaded{[&](const ast::ExprLambda& exprLambda) { auto [parameter_names, __1, __2] = exprLambda; return ((parameter_names.length() != expected_params) ? ast::diagnostics_append(errors, mlc::Array{ast::diagnostic_error(((((mlc::String("method ", 7) + method_name) + mlc::String(" expects a lambda with ", 23)) + mlc::to_string(expected_params)) + mlc::String(" parameter(s)", 13)), ast::expr_span(expression))}) : errors); },
[&](const auto& __v) { return errors; }
}, (*expression));
}
mlc::Array<ast::Diagnostic> append_predicate_bool_diag(mlc::Array<ast::Diagnostic> errors, std::shared_ptr<ast::Expr> lambda_expr, std::shared_ptr<registry::Type> fn_type, mlc::String method_name) noexcept{
auto ret = semantic_type_structure::function_return_type(fn_type);
if (((!semantic_type_structure::type_is_unknown(ret)) && (!semantic_type_structure::type_is_bool(ret)))) {
return ast::diagnostics_append(errors, mlc::Array{ast::diagnostic_error((((mlc::String("method ", 7) + method_name) + mlc::String(" expects a predicate returning bool, got ", 41)) + semantic_type_structure::type_description(ret)), ast::expr_span(lambda_expr))});
} else {
return errors;
}
}
bool method_expects_bool_predicate(mlc::String method_name) noexcept{
return (((((method_name == mlc::String("filter", 6)) || (method_name == mlc::String("any", 3))) || (method_name == mlc::String("all", 3))) || (method_name == mlc::String("find_index", 10))) || (method_name == mlc::String("find", 4)));
}
infer_result::InferResult infer_array_hof_method_call(infer_result::InferResult object_result, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, InferExprFn infer_expr_fn) noexcept{
auto element_type = semantic_type_structure::array_element_type_from_array_type(object_result.inferred_type);
auto receiver_errors = method_receiver_diagnostics::infer_builtin_method_receiver_diagnostics(object_result.inferred_type, method_name, method_span);
auto arity_errors = type_diagnostics::method_arity_after_receiver(receiver_errors, method_name, method_arguments.length(), method_span);
auto merged = object_result;
auto arg_types = mlc::Array<std::shared_ptr<registry::Type>>{};
if ((method_name == mlc::String("fold", 4))) {
if ((method_arguments.length() > 0)) {
auto init_result = infer_expr_fn(method_arguments[0], inference_context);
merged = InferResult_absorb(merged, init_result);
arg_types.push_back(init_result.inferred_type);
if ((method_arguments.length() > 1)) {
auto acc_type = init_result.inferred_type;
auto lam_errors = append_lambda_arity_diag({}, method_arguments[1], 2, method_name);
auto lam_result = infer_expr_maybe_lambda(method_arguments[1], mlc::Array{acc_type, element_type}, inference_context, infer_expr_fn);
merged = InferResult_absorb(merged, infer_result::InferResult{lam_result.inferred_type, lam_result.errors});
lam_errors = append_lambda_arity_diag(lam_errors, method_arguments[1], 2, method_name);
auto lam_ret = semantic_type_structure::function_return_type(lam_result.inferred_type);
lam_errors = ((((!semantic_type_structure::type_is_unknown(acc_type)) && (!semantic_type_structure::type_is_unknown(lam_ret))) && (!semantic_type_structure::types_structurally_equal(lam_ret, acc_type))) ? ast::diagnostics_append(lam_errors, mlc::Array{ast::diagnostic_error(((((mlc::String("fold step function must return the same type as the accumulator (expected ", 74) + semantic_type_structure::type_description(acc_type)) + mlc::String(", got ", 6)) + semantic_type_structure::type_description(lam_ret)) + mlc::String(")", 1)), ast::expr_span(method_arguments[1]))}) : lam_errors);
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, lam_errors)};
arg_types.push_back(lam_result.inferred_type);
}
}
} else if (((method_name == mlc::String("zip", 3)) || (method_name == mlc::String("join", 4)))) {
if ((method_arguments.length() > 0)) {
auto arg0_result = infer_expr_fn(method_arguments[0], inference_context);
merged = InferResult_absorb(merged, arg0_result);
arg_types.push_back(arg0_result.inferred_type);
auto extra = mlc::Array<ast::Diagnostic>{};
if ((method_name == mlc::String("zip", 3))) {
if (((!semantic_type_structure::type_is_unknown(arg0_result.inferred_type)) && (!semantic_type_structure::type_is_array(arg0_result.inferred_type)))) {
extra = mlc::Array{ast::diagnostic_error((mlc::String("method zip expects an array argument, got ", 42) + semantic_type_structure::type_description(arg0_result.inferred_type)), ast::expr_span(method_arguments[0]))};
}
} else if (((!semantic_type_structure::type_is_unknown(element_type)) && (!semantic_type_structure::type_is_string(element_type)))) {
extra = mlc::Array{ast::diagnostic_error((mlc::String("method join expects an array of string elements, got ", 53) + semantic_type_structure::type_description(element_type)), method_span)};
}

if (((!semantic_type_structure::type_is_unknown(arg0_result.inferred_type)) && (!semantic_type_structure::type_is_string(arg0_result.inferred_type)))) {
extra = ast::diagnostics_append(extra, mlc::Array{ast::diagnostic_error((mlc::String("method join expects string separator, got ", 42) + semantic_type_structure::type_description(arg0_result.inferred_type)), ast::expr_span(method_arguments[0]))});
}
}
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, extra)};
}

/* unit */;
if (((method_name == mlc::String("take", 4)) || (method_name == mlc::String("drop", 4)))) {
if ((method_arguments.length() > 0)) {
auto n_result = infer_expr_fn(method_arguments[0], inference_context);
merged = InferResult_absorb(merged, n_result);
arg_types.push_back(n_result.inferred_type);
auto count_errors = mlc::Array<ast::Diagnostic>{};
if (((!semantic_type_structure::type_is_unknown(n_result.inferred_type)) && (!semantic_type_structure::type_is_i32(n_result.inferred_type)))) {
count_errors = mlc::Array{ast::diagnostic_error((((mlc::String("method ", 7) + method_name) + mlc::String(" expects i32, got ", 18)) + semantic_type_structure::type_description(n_result.inferred_type)), ast::expr_span(method_arguments[0]))};
}
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, count_errors)};
}
} else if ((((method_name == mlc::String("enumerate", 9)) || (method_name == mlc::String("sum", 3))) || (method_name == mlc::String("flat", 4)))) {
auto extra = mlc::Array<ast::Diagnostic>{};
if ((((method_name == mlc::String("sum", 3)) && (!semantic_type_structure::type_is_unknown(element_type))) && (!semantic_type_structure::type_is_i32(element_type)))) {
extra = mlc::Array{ast::diagnostic_error((mlc::String("method sum expects i32 array elements, got ", 43) + semantic_type_structure::type_description(element_type)), method_span)};
}
if ((((method_name == mlc::String("flat", 4)) && (!semantic_type_structure::type_is_array(element_type))) && (!semantic_type_structure::type_is_unknown(element_type)))) {
extra = mlc::Array{ast::diagnostic_error((mlc::String("method flat expects an array of arrays, got ", 44) + semantic_type_structure::type_description(element_type)), method_span)};
}
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, extra)};
} else if ((method_arguments.length() > 0)) {
auto lam_errors = append_lambda_arity_diag({}, method_arguments[0], 1, method_name);
auto lam_result = infer_expr_maybe_lambda(method_arguments[0], mlc::Array{element_type}, inference_context, infer_expr_fn);
merged = InferResult_absorb(merged, lam_result);
lam_errors = append_lambda_arity_diag(lam_errors, method_arguments[0], 1, method_name);
lam_errors = (method_expects_bool_predicate(method_name) ? append_predicate_bool_diag(lam_errors, method_arguments[0], lam_result.inferred_type, method_name) : lam_errors);
lam_errors = ((method_name == mlc::String("flat_map", 8)) ? [&]() {
auto cb_ret = semantic_type_structure::function_return_type(lam_result.inferred_type);
return (((!semantic_type_structure::type_is_unknown(cb_ret)) && (!semantic_type_structure::type_is_array(cb_ret))) ? ast::diagnostics_append(lam_errors, mlc::Array{ast::diagnostic_error((mlc::String("flat_map callback must return an array, got ", 44) + semantic_type_structure::type_description(cb_ret)), ast::expr_span(method_arguments[0]))}) : lam_errors);
}() : lam_errors);
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, lam_errors)};
arg_types.push_back(lam_result.inferred_type);
}


}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
bool should_infer_as_array_hof(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept{
return (array_method_types::is_array_hof_method(method_name) && semantic_type_structure::type_is_array(receiver_type));
}

} // namespace infer_array_method
