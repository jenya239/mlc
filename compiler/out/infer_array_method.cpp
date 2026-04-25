#include "infer_array_method.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "semantic_type_structure.hpp"
#include "array_method_types.hpp"
#include "infer_lambda_context.hpp"
#include "method_receiver_diagnostics.hpp"
#include "type_diagnostics.hpp"

namespace infer_array_method {

using namespace ast;
using namespace registry;
using namespace check_context;
using namespace infer_result;
using namespace semantic_type_structure;
using namespace array_method_types;
using namespace infer_lambda_context;
using namespace method_receiver_diagnostics;
using namespace type_diagnostics;
using namespace ast_tokens;

using InferExprFn = std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)>;

infer_result::InferResult infer_expr_maybe_lambda(std::shared_ptr<ast::Expr> expression, mlc::Array<std::shared_ptr<registry::Type>> expected_param_types, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

mlc::Array<ast::Diagnostic> append_lambda_arity_diag(mlc::Array<ast::Diagnostic> errors, std::shared_ptr<ast::Expr> expression, int expected_params, mlc::String method_name) noexcept;

mlc::Array<ast::Diagnostic> append_predicate_bool_diag(mlc::Array<ast::Diagnostic> errors, std::shared_ptr<ast::Expr> lambda_expr, std::shared_ptr<registry::Type> fn_type, mlc::String method_name) noexcept;

bool method_expects_bool_predicate(mlc::String method_name) noexcept;

infer_result::InferResult infer_array_hof_method_call(infer_result::InferResult object_result, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

bool should_infer_as_array_hof(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept;

infer_result::InferResult infer_expr_maybe_lambda(std::shared_ptr<ast::Expr> expression, mlc::Array<std::shared_ptr<registry::Type>> expected_param_types, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept{return [&]() -> infer_result::InferResult { if (std::holds_alternative<ast::ExprLambda>((*expression)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*expression)._); auto [parameter_names, body, _w0] = _v_exprlambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, expected_param_types, body, inference_context, infer_expr_fn); } return infer_expr_fn(expression, inference_context); }();}

mlc::Array<ast::Diagnostic> append_lambda_arity_diag(mlc::Array<ast::Diagnostic> errors, std::shared_ptr<ast::Expr> expression, int expected_params, mlc::String method_name) noexcept{return [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<ast::ExprLambda>((*expression)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*expression)._); auto [parameter_names, _w0, _w1] = _v_exprlambda; return parameter_names.size() != expected_params ? ast::diagnostics_append(errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method ") + method_name + mlc::String(" expects a lambda with ") + mlc::to_string(expected_params) + mlc::String(" parameter(s)"), ast::expr_span(expression))}) : errors; } return errors; }();}

mlc::Array<ast::Diagnostic> append_predicate_bool_diag(mlc::Array<ast::Diagnostic> errors, std::shared_ptr<ast::Expr> lambda_expr, std::shared_ptr<registry::Type> fn_type, mlc::String method_name) noexcept{
std::shared_ptr<registry::Type> ret = semantic_type_structure::function_return_type(fn_type);
return !semantic_type_structure::type_is_unknown(ret) && !semantic_type_structure::type_is_bool(ret) ? ast::diagnostics_append(errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method ") + method_name + mlc::String(" expects a predicate returning bool, got ") + semantic_type_structure::type_description(ret), ast::expr_span(lambda_expr))}) : errors;
}

bool method_expects_bool_predicate(mlc::String method_name) noexcept{return method_name == mlc::String("filter") || method_name == mlc::String("any") || method_name == mlc::String("all") || method_name == mlc::String("find_index") || method_name == mlc::String("find");}

infer_result::InferResult infer_array_hof_method_call(infer_result::InferResult object_result, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept{
std::shared_ptr<registry::Type> element_type = semantic_type_structure::array_element_type_from_array_type(object_result.inferred_type);
mlc::Array<ast::Diagnostic> receiver_errors = method_receiver_diagnostics::infer_builtin_method_receiver_diagnostics(object_result.inferred_type, method_name, method_span);
mlc::Array<ast::Diagnostic> arity_errors = type_diagnostics::method_arity_after_receiver(receiver_errors, method_name, method_arguments.size(), method_span);
infer_result::InferResult merged = std::move(object_result);
mlc::Array<std::shared_ptr<registry::Type>> arg_types = {};
if (method_name == mlc::String("fold")){
{
if (method_arguments.size() > 0){
infer_result::InferResult init_result = infer_expr_fn(method_arguments[0], inference_context);
merged = infer_result::InferResult_absorb(merged, init_result);
arg_types.push_back(init_result.inferred_type);
if (method_arguments.size() > 1){
std::shared_ptr<registry::Type> acc_type = init_result.inferred_type;
mlc::Array<ast::Diagnostic> lam_errors = append_lambda_arity_diag({}, method_arguments[1], 2, method_name);
infer_result::InferResult lam_result = infer_expr_maybe_lambda(method_arguments[1], mlc::Array<std::shared_ptr<registry::Type>>{acc_type, element_type}, inference_context, infer_expr_fn);
merged = infer_result::InferResult_absorb(merged, infer_result::InferResult{lam_result.inferred_type, lam_result.errors});
lam_errors = append_lambda_arity_diag(lam_errors, method_arguments[1], 2, method_name);
std::shared_ptr<registry::Type> lam_ret = semantic_type_structure::function_return_type(lam_result.inferred_type);
lam_errors = !semantic_type_structure::type_is_unknown(acc_type) && !semantic_type_structure::type_is_unknown(lam_ret) && !semantic_type_structure::types_structurally_equal(lam_ret, acc_type) ? ast::diagnostics_append(lam_errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("fold step function must return the same type as the accumulator (expected ") + semantic_type_structure::type_description(acc_type) + mlc::String(", got ") + semantic_type_structure::type_description(lam_ret) + mlc::String(")"), ast::expr_span(method_arguments[1]))}) : lam_errors;
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, lam_errors)};
arg_types.push_back(lam_result.inferred_type);
}
}
}
} else {
{
if (method_name == mlc::String("zip") || method_name == mlc::String("join")){
if (method_arguments.size() > 0){
infer_result::InferResult arg0_result = infer_expr_fn(method_arguments[0], inference_context);
merged = infer_result::InferResult_absorb(merged, arg0_result);
arg_types.push_back(arg0_result.inferred_type);
mlc::Array<ast::Diagnostic> extra = {};
if (method_name == mlc::String("zip")){
{
if (!semantic_type_structure::type_is_unknown(arg0_result.inferred_type) && !semantic_type_structure::type_is_array(arg0_result.inferred_type)){
extra = mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method zip expects an array argument, got ") + semantic_type_structure::type_description(arg0_result.inferred_type), ast::expr_span(method_arguments[0]))};
}
}
} else {
{
if (!semantic_type_structure::type_is_unknown(element_type) && !semantic_type_structure::type_is_string(element_type)){
{
extra = mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method join expects an array of string elements, got ") + semantic_type_structure::type_description(element_type), method_span)};
}
}
if (!semantic_type_structure::type_is_unknown(arg0_result.inferred_type) && !semantic_type_structure::type_is_string(arg0_result.inferred_type)){
extra = ast::diagnostics_append(extra, mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method join expects string separator, got ") + semantic_type_structure::type_description(arg0_result.inferred_type), ast::expr_span(method_arguments[0]))});
}
}
}
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, extra)};
}
} else {
if (method_name == mlc::String("take") || method_name == mlc::String("drop")){
if (method_arguments.size() > 0){
infer_result::InferResult n_result = infer_expr_fn(method_arguments[0], inference_context);
merged = infer_result::InferResult_absorb(merged, n_result);
arg_types.push_back(n_result.inferred_type);
mlc::Array<ast::Diagnostic> count_errors = {};
if (!semantic_type_structure::type_is_unknown(n_result.inferred_type) && !semantic_type_structure::type_is_i32(n_result.inferred_type)){
{
count_errors = mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method ") + method_name + mlc::String(" expects i32, got ") + semantic_type_structure::type_description(n_result.inferred_type), ast::expr_span(method_arguments[0]))};
}
}
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, count_errors)};
}
} else {
if (method_name == mlc::String("enumerate") || method_name == mlc::String("sum") || method_name == mlc::String("flat")){
mlc::Array<ast::Diagnostic> extra = {};
if (method_name == mlc::String("sum") && !semantic_type_structure::type_is_unknown(element_type) && !semantic_type_structure::type_is_i32(element_type)){
{
extra = mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method sum expects i32 array elements, got ") + semantic_type_structure::type_description(element_type), method_span)};
}
}
if (method_name == mlc::String("flat") && !semantic_type_structure::type_is_array(element_type) && !semantic_type_structure::type_is_unknown(element_type)){
{
extra = mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method flat expects an array of arrays, got ") + semantic_type_structure::type_description(element_type), method_span)};
}
}
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, extra)};
} else {
if (method_arguments.size() > 0){
mlc::Array<ast::Diagnostic> lam_errors = append_lambda_arity_diag({}, method_arguments[0], 1, method_name);
infer_result::InferResult lam_result = infer_expr_maybe_lambda(method_arguments[0], mlc::Array<std::shared_ptr<registry::Type>>{element_type}, inference_context, infer_expr_fn);
merged = infer_result::InferResult_absorb(merged, lam_result);
lam_errors = append_lambda_arity_diag(lam_errors, method_arguments[0], 1, method_name);
lam_errors = method_expects_bool_predicate(method_name) ? append_predicate_bool_diag(lam_errors, method_arguments[0], lam_result.inferred_type, method_name) : lam_errors;
lam_errors = method_name == mlc::String("flat_map") ? [&]() -> mlc::Array<ast::Diagnostic> { 
  std::shared_ptr<registry::Type> cb_ret = semantic_type_structure::function_return_type(lam_result.inferred_type);
  return !semantic_type_structure::type_is_unknown(cb_ret) && !semantic_type_structure::type_is_array(cb_ret) ? ast::diagnostics_append(lam_errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("flat_map callback must return an array, got ") + semantic_type_structure::type_description(cb_ret), ast::expr_span(method_arguments[0]))}) : lam_errors;
 }() : lam_errors;
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, lam_errors)};
arg_types.push_back(lam_result.inferred_type);
}
}
}
}
}
}
std::shared_ptr<registry::Type> out_type = array_method_types::array_hof_call_result_type(element_type, method_name, arg_types);
return infer_result::InferResult{out_type, ast::diagnostics_append(ast::diagnostics_append(merged.errors, receiver_errors), arity_errors)};
}

bool should_infer_as_array_hof(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept{return array_method_types::is_array_hof_method(method_name) && semantic_type_structure::type_is_array(receiver_type);}

} // namespace infer_array_method
