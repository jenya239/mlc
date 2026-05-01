#define main mlc_user_main
#include "infer_result_option_method.hpp"

namespace infer_result_option_method {

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
mlc::Array<ast::Diagnostic> append_lambda_arity(mlc::Array<ast::Diagnostic> errors, std::shared_ptr<ast::Expr> expression, int expected, mlc::String method_name) noexcept{
return std::visit(overloaded{[&](const ast::ExprLambda& exprLambda) { auto [parameter_names, __1, __2] = exprLambda; return ((parameter_names.length() != expected) ? ast::diagnostics_append(errors, mlc::Array{ast::diagnostic_error(((((mlc::String("method ", 7) + method_name) + mlc::String(" expects a lambda with ", 23)) + mlc::to_string(expected)) + mlc::String(" parameter(s)", 13)), ast::expr_span(expression))}) : errors); },
[&](const auto& __v) { return errors; }
}, (*expression));
}
bool is_result_type(std::shared_ptr<registry::Type> t) noexcept{
return std::visit(overloaded{[&](const registry::TGeneric& tGeneric) { auto [name, __1] = tGeneric; return (name == mlc::String("Result", 6)); },
[&](const auto& __v) { return false; }
}, (*t));
}
std::shared_ptr<registry::Type> result_inner_err_from_ret(std::shared_ptr<registry::Type> lam_ret) noexcept{
return std::visit(overloaded{[&](const registry::TGeneric& tGeneric) { auto [name, a] = tGeneric; return (((name == mlc::String("Result", 6)) && (a.length() >= 2)) ? a[1] : std::make_shared<registry::Type>(registry::TUnknown{})); },
[&](const auto& __v) { return std::make_shared<registry::Type>(registry::TUnknown{}); }
}, (*lam_ret));
}
bool should_infer_as_result_option_hof(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept{
return result_option_method_types::should_infer_result_option_combinator(receiver_type, method_name);
}
infer_result::InferResult infer_result_option_hof_method_call(infer_result::InferResult object_result, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, InferExprFn infer_expr_fn) noexcept{
auto receiver_type = object_result.inferred_type;
auto receiver_errors = method_receiver_diagnostics::infer_builtin_method_receiver_diagnostics(receiver_type, method_name, method_span);
auto expected_arity = result_option_method_types::result_option_hof_expected_argument_count(receiver_type, method_name);
auto arity_errors = ((expected_arity >= 0) ? ((method_arguments.length() != expected_arity) ? mlc::Array{ast::diagnostic_error((((mlc::String("expected ", 9) + mlc::to_string(expected_arity)) + mlc::String(" arguments, got ", 16)) + mlc::to_string(method_arguments.length())), method_span)} : {}) : {});
auto merged = object_result;
auto arg_types = mlc::Array<std::shared_ptr<registry::Type>>{};
if (result_option_method_types::is_result_generic(receiver_type)) {
auto t_t = result_option_method_types::result_ok_type(receiver_type);
auto e_t = result_option_method_types::result_err_type(receiver_type);
if (((method_name == mlc::String("unwrap_or", 9)) && (method_arguments.length() > 0))) {
auto a = infer_expr_fn(method_arguments[0], inference_context);
merged = InferResult_absorb(merged, a);
arg_types.push_back(a.inferred_type);
if ((((!semantic_type_structure::type_is_unknown(t_t)) && (!semantic_type_structure::type_is_unknown(a.inferred_type))) && (!semantic_type_structure::types_structurally_equal(t_t, a.inferred_type)))) {
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, mlc::Array{ast::diagnostic_error(((mlc::String("method unwrap_or value must match Ok type (expected ", 52) + semantic_type_structure::type_description(t_t)) + mlc::String(")", 1)), ast::expr_span(method_arguments[0]))})};
}
} else if (((method_name == mlc::String("unwrap_or_else", 14)) && (method_arguments.length() > 0))) {
auto arg0 = method_arguments[0];
auto ar = append_lambda_arity({}, arg0, 1, method_name);
auto lam_r = std::visit(overloaded{[&](const ast::ExprLambda& exprLambda) { auto [parameter_names, body, __2] = exprLambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array{e_t}, body, inference_context, infer_expr_fn); },
[&](const auto& __v) { return infer_expr_fn(arg0, inference_context); }
}, (*arg0));
ar = append_lambda_arity(ar, arg0, 1, method_name);
merged = InferResult_absorb(merged, lam_r);
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, ar)};
auto lam_ret = semantic_type_structure::function_return_type(lam_r.inferred_type);
if ((((!semantic_type_structure::type_is_unknown(t_t)) && (!semantic_type_structure::type_is_unknown(lam_ret))) && (!semantic_type_structure::types_structurally_equal(t_t, lam_ret)))) {
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, mlc::Array{ast::diagnostic_error(((mlc::String("method unwrap_or_else must return the Ok type (expected ", 56) + semantic_type_structure::type_description(t_t)) + mlc::String(")", 1)), ast::expr_span(arg0))})};
}
arg_types.push_back(lam_r.inferred_type);
} else if (((method_name == mlc::String("map", 3)) && (method_arguments.length() > 0))) {
auto arg0 = method_arguments[0];
auto ar = append_lambda_arity({}, arg0, 1, method_name);
auto lam_r = std::visit(overloaded{[&](const ast::ExprLambda& exprLambda) { auto [parameter_names, body, __2] = exprLambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array{t_t}, body, inference_context, infer_expr_fn); },
[&](const auto& __v) { return infer_expr_fn(arg0, inference_context); }
}, (*arg0));
ar = append_lambda_arity(ar, arg0, 1, method_name);
merged = InferResult_absorb(merged, lam_r);
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, ar)};
arg_types.push_back(lam_r.inferred_type);
} else if (((method_name == mlc::String("map_err", 7)) && (method_arguments.length() > 0))) {
auto arg0 = method_arguments[0];
auto ar = append_lambda_arity({}, arg0, 1, method_name);
auto lam_r = std::visit(overloaded{[&](const ast::ExprLambda& exprLambda) { auto [parameter_names, body, __2] = exprLambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array{e_t}, body, inference_context, infer_expr_fn); },
[&](const auto& __v) { return infer_expr_fn(arg0, inference_context); }
}, (*arg0));
ar = append_lambda_arity(ar, arg0, 1, method_name);
merged = InferResult_absorb(merged, lam_r);
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, ar)};
arg_types.push_back(lam_r.inferred_type);
} else if (((method_name == mlc::String("and_then", 8)) && (method_arguments.length() > 0))) {
auto arg0 = method_arguments[0];
auto ar = append_lambda_arity({}, arg0, 1, method_name);
auto lam_r = std::visit(overloaded{[&](const ast::ExprLambda& exprLambda) { auto [parameter_names, body, __2] = exprLambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array{t_t}, body, inference_context, infer_expr_fn); },
[&](const auto& __v) { return infer_expr_fn(arg0, inference_context); }
}, (*arg0));
ar = append_lambda_arity(ar, arg0, 1, method_name);
merged = InferResult_absorb(merged, lam_r);
auto extra = ar;
auto lam_ret = semantic_type_structure::function_return_type(lam_r.inferred_type);
if (is_result_type(lam_ret)) {
auto inner_e = result_inner_err_from_ret(lam_ret);
if ((((!semantic_type_structure::type_is_unknown(e_t)) && (!semantic_type_structure::type_is_unknown(inner_e))) && (!semantic_type_structure::types_structurally_equal(e_t, inner_e)))) {
extra = ast::diagnostics_append(extra, mlc::Array{ast::diagnostic_error(((mlc::String("method and_then callback must return Result with the same error type (expected ", 79) + semantic_type_structure::type_description(e_t)) + mlc::String(")", 1)), ast::expr_span(arg0))});
}
} else {
extra = ast::diagnostics_append(extra, mlc::Array{ast::diagnostic_error(mlc::String("method and_then must return a Result type", 41), ast::expr_span(arg0))});
}
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, extra)};
arg_types.push_back(lam_r.inferred_type);
} else if ((method_name == mlc::String("ok", 2))) {
auto _noop = 0;
} else if (((method_name == mlc::String("or_else", 7)) && (method_arguments.length() > 0))) {
auto arg0 = method_arguments[0];
auto ar = append_lambda_arity({}, arg0, 1, method_name);
auto lam_r = std::visit(overloaded{[&](const ast::ExprLambda& exprLambda) { auto [parameter_names, body, __2] = exprLambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array{e_t}, body, inference_context, infer_expr_fn); },
[&](const auto& __v) { return infer_expr_fn(arg0, inference_context); }
}, (*arg0));
ar = append_lambda_arity(ar, arg0, 1, method_name);
merged = InferResult_absorb(merged, lam_r);
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, ar)};
arg_types.push_back(lam_r.inferred_type);
}






} else if (result_option_method_types::is_option_generic(receiver_type)) {
auto o_t = result_option_method_types::option_inner_type(receiver_type);
if (((method_name == mlc::String("unwrap_or", 9)) && (method_arguments.length() > 0))) {
auto a = infer_expr_fn(method_arguments[0], inference_context);
merged = InferResult_absorb(merged, a);
arg_types.push_back(a.inferred_type);
if ((((!semantic_type_structure::type_is_unknown(o_t)) && (!semantic_type_structure::type_is_unknown(a.inferred_type))) && (!semantic_type_structure::types_structurally_equal(o_t, a.inferred_type)))) {
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, mlc::Array{ast::diagnostic_error(((mlc::String("method unwrap_or value must match Some type (expected ", 54) + semantic_type_structure::type_description(o_t)) + mlc::String(")", 1)), ast::expr_span(method_arguments[0]))})};
}
} else if (((method_name == mlc::String("or_else", 7)) && (method_arguments.length() > 0))) {
auto arg0 = method_arguments[0];
auto ar = append_lambda_arity({}, arg0, 0, mlc::String("or_else", 7));
auto lam_r = std::visit(overloaded{[&](const ast::ExprLambda& exprLambda) { auto [parameter_names, body, __2] = exprLambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, {}, body, inference_context, infer_expr_fn); },
[&](const auto& __v) { return infer_expr_fn(arg0, inference_context); }
}, (*arg0));
ar = append_lambda_arity(ar, arg0, 0, mlc::String("or_else", 7));
merged = InferResult_absorb(merged, lam_r);
auto extra = ar;
auto inner_ret = semantic_type_structure::function_return_type(lam_r.inferred_type);
if (result_option_method_types::is_option_generic(inner_ret)) {
auto io = result_option_method_types::option_inner_type(inner_ret);
if ((((!semantic_type_structure::type_is_unknown(o_t)) && (!semantic_type_structure::type_is_unknown(io))) && (!semantic_type_structure::types_structurally_equal(o_t, io)))) {
extra = ast::diagnostics_append(extra, mlc::Array{ast::diagnostic_error(((mlc::String("method or_else must return Option with the same inner type (expected ", 69) + semantic_type_structure::type_description(o_t)) + mlc::String(")", 1)), ast::expr_span(arg0))});
}
} else {
extra = ast::diagnostics_append(extra, mlc::Array{ast::diagnostic_error(mlc::String("method or_else must return Option", 33), ast::expr_span(arg0))});
}
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, extra)};
arg_types.push_back(lam_r.inferred_type);
} else if (((method_name == mlc::String("ok_or", 5)) && (method_arguments.length() > 0))) {
auto a = infer_expr_fn(method_arguments[0], inference_context);
merged = InferResult_absorb(merged, a);
arg_types.push_back(a.inferred_type);
} else if (((method_name == mlc::String("map", 3)) && (method_arguments.length() > 0))) {
auto arg0 = method_arguments[0];
auto ar = append_lambda_arity({}, arg0, 1, method_name);
auto lam_r = std::visit(overloaded{[&](const ast::ExprLambda& exprLambda) { auto [parameter_names, body, __2] = exprLambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array{o_t}, body, inference_context, infer_expr_fn); },
[&](const auto& __v) { return infer_expr_fn(arg0, inference_context); }
}, (*arg0));
ar = append_lambda_arity(ar, arg0, 1, method_name);
merged = InferResult_absorb(merged, lam_r);
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, ar)};
arg_types.push_back(lam_r.inferred_type);
} else if (((method_name == mlc::String("and_then", 8)) && (method_arguments.length() > 0))) {
auto arg0 = method_arguments[0];
auto ar = append_lambda_arity({}, arg0, 1, method_name);
auto lam_r = std::visit(overloaded{[&](const ast::ExprLambda& exprLambda) { auto [parameter_names, body, __2] = exprLambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array{o_t}, body, inference_context, infer_expr_fn); },
[&](const auto& __v) { return infer_expr_fn(arg0, inference_context); }
}, (*arg0));
ar = append_lambda_arity(ar, arg0, 1, method_name);
merged = InferResult_absorb(merged, lam_r);
auto extra = ar;
auto ret = semantic_type_structure::function_return_type(lam_r.inferred_type);
if ((!result_option_method_types::is_option_generic(ret))) {
extra = ast::diagnostics_append(extra, mlc::Array{ast::diagnostic_error(mlc::String("method and_then must return Option", 34), ast::expr_span(arg0))});
}
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, extra)};
arg_types.push_back(lam_r.inferred_type);
} else if (((method_name == mlc::String("filter", 6)) && (method_arguments.length() > 0))) {
auto arg0 = method_arguments[0];
auto ar = append_lambda_arity({}, arg0, 1, method_name);
auto lam_r = std::visit(overloaded{[&](const ast::ExprLambda& exprLambda) { auto [parameter_names, body, __2] = exprLambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array{o_t}, body, inference_context, infer_expr_fn); },
[&](const auto& __v) { return infer_expr_fn(arg0, inference_context); }
}, (*arg0));
ar = append_lambda_arity(ar, arg0, 1, method_name);
merged = InferResult_absorb(merged, lam_r);
auto extra = ar;
auto lam_ret = semantic_type_structure::function_return_type(lam_r.inferred_type);
if (((!semantic_type_structure::type_is_unknown(lam_ret)) && (!semantic_type_structure::type_is_bool(lam_ret)))) {
extra = ast::diagnostics_append(extra, mlc::Array{ast::diagnostic_error(mlc::String("method filter expects a predicate returning bool", 48), ast::expr_span(arg0))});
}
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, extra)};
arg_types.push_back(lam_r.inferred_type);
}





}

auto out = result_option_method_types::result_option_hof_call_result_type(receiver_type, method_name, arg_types);
return infer_result::InferResult{out, ast::diagnostics_append(ast::diagnostics_append(merged.errors, receiver_errors), arity_errors)};
}

} // namespace infer_result_option_method
