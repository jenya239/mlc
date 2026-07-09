#include "infer_array_method.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "semantic_type_structure.hpp"
#include "array_method_types.hpp"
#include "hof_method_spec.hpp"
#include "infer_lambda_context.hpp"
#include "method_receiver_diagnostics.hpp"
#include "type_diagnostics.hpp"
#include "diagnostic_codes.hpp"

namespace infer_array_method {

using namespace ast;
using namespace registry;
using namespace check_context;
using namespace infer_result;
using namespace semantic_type_structure;
using namespace array_method_types;
using namespace hof_method_spec;
using namespace infer_lambda_context;
using namespace method_receiver_diagnostics;
using namespace type_diagnostics;
using namespace diagnostic_codes;
using namespace ast_tokens;

mlc::Array<std::shared_ptr<registry::Type>> append_argument_type(mlc::Array<std::shared_ptr<registry::Type>> argument_types_so_far, std::shared_ptr<registry::Type> type_to_append) noexcept;

infer_result::InferResult infer_expr_maybe_lambda(std::shared_ptr<ast::Expr> expression, mlc::Array<std::shared_ptr<registry::Type>> expected_param_types, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

mlc::Array<ast::Diagnostic> append_lambda_arity_diag(mlc::Array<ast::Diagnostic> errors, std::shared_ptr<ast::Expr> expression, int expected_params, mlc::String method_name) noexcept;

mlc::Array<ast::Diagnostic> append_predicate_bool_diag(mlc::Array<ast::Diagnostic> errors, std::shared_ptr<ast::Expr> lambda_expression, std::shared_ptr<registry::Type> function_type, mlc::String method_name) noexcept;

bool method_expects_bool_predicate(mlc::String method_name) noexcept;

infer_result::InferResult infer_array_hof_method_call(infer_result::InferResult object_parsed, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

bool should_infer_as_array_hof(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept;

mlc::Array<std::shared_ptr<registry::Type>> append_argument_type(mlc::Array<std::shared_ptr<registry::Type>> argument_types_so_far, std::shared_ptr<registry::Type> type_to_append) noexcept{return argument_types_so_far.concat(mlc::Array<std::shared_ptr<registry::Type>>{type_to_append});}

infer_result::InferResult infer_expr_maybe_lambda(std::shared_ptr<ast::Expr> expression, mlc::Array<std::shared_ptr<registry::Type>> expected_param_types, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept{return [&]() -> infer_result::InferResult { if (std::holds_alternative<ast::ExprLambda>((*expression)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*expression)._); auto [parameter_names, body, _w0] = _v_exprlambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, expected_param_types, body, inference_context, infer_expr_fn); } return infer_expr_fn(expression, inference_context); }();}

mlc::Array<ast::Diagnostic> append_lambda_arity_diag(mlc::Array<ast::Diagnostic> errors, std::shared_ptr<ast::Expr> expression, int expected_params, mlc::String method_name) noexcept{return [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<ast::ExprLambda>((*expression)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*expression)._); auto [parameter_names, _w0, _w1] = _v_exprlambda; return parameter_names.size() != expected_params ? ast::diagnostics_append(errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("method ") + method_name + mlc::String(" expects a lambda with ") + mlc::to_string(expected_params) + mlc::String(" parameter(s)"), ast::expr_span(expression), diagnostic_codes::diagnostic_code_e045())}) : errors; } return errors; }();}

mlc::Array<ast::Diagnostic> append_predicate_bool_diag(mlc::Array<ast::Diagnostic> errors, std::shared_ptr<ast::Expr> lambda_expression, std::shared_ptr<registry::Type> function_type, mlc::String method_name) noexcept{
std::shared_ptr<registry::Type> return_type = semantic_type_structure::function_return_type(function_type);
return !semantic_type_structure::type_is_unknown(return_type) && !semantic_type_structure::type_is_bool(return_type) ? ast::diagnostics_append(errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("method ") + method_name + mlc::String(" expects a predicate returning bool, got ") + semantic_type_structure::type_description(return_type), ast::expr_span(lambda_expression), diagnostic_codes::diagnostic_code_e046())}) : errors;
}

bool method_expects_bool_predicate(mlc::String method_name) noexcept{return method_name == mlc::String("filter") || method_name == mlc::String("any") || method_name == mlc::String("all") || method_name == mlc::String("find_index") || method_name == mlc::String("find");}

infer_result::InferResult infer_array_hof_method_call(infer_result::InferResult object_parsed, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept{
std::shared_ptr<registry::Type> element_type = semantic_type_structure::array_element_type_from_array_type(object_parsed.inferred_type);
mlc::Array<ast::Diagnostic> receiver_errors = method_receiver_diagnostics::infer_builtin_method_receiver_diagnostics(object_parsed.inferred_type, method_name, method_span);
mlc::Array<ast::Diagnostic> arity_errors = type_diagnostics::method_arity_after_receiver(receiver_errors, method_name, method_arguments.size(), method_span);
infer_result::InferResult merged = std::move(object_parsed);
mlc::Array<std::shared_ptr<registry::Type>> argument_types = {};
if (method_name == mlc::String("fold")){
{
if (method_arguments.size() > 0){
infer_result::InferResult init_parsed = infer_expr_fn(method_arguments[0], inference_context);
merged = infer_result::InferResult_absorb(merged, init_parsed);
argument_types = append_argument_type(argument_types, init_parsed.inferred_type);
if (method_arguments.size() > 1){
std::shared_ptr<registry::Type> accumulator_type = init_parsed.inferred_type;
mlc::Array<ast::Diagnostic> lambda_errors = append_lambda_arity_diag({}, method_arguments[1], 2, method_name);
infer_result::InferResult lambda_parsed = infer_expr_maybe_lambda(method_arguments[1], mlc::Array<std::shared_ptr<registry::Type>>{accumulator_type, element_type}, inference_context, infer_expr_fn);
merged = infer_result::InferResult_absorb(merged, infer_result::InferResult{lambda_parsed.inferred_type, lambda_parsed.errors});
lambda_errors = append_lambda_arity_diag(lambda_errors, method_arguments[1], 2, method_name);
std::shared_ptr<registry::Type> lambda_return_type = semantic_type_structure::function_return_type(lambda_parsed.inferred_type);
lambda_errors = !semantic_type_structure::type_is_unknown(accumulator_type) && !semantic_type_structure::type_is_unknown(lambda_return_type) && !semantic_type_structure::types_structurally_equal(lambda_return_type, accumulator_type) ? ast::diagnostics_append(lambda_errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("fold step function must return the same type as the accumulator (expected ") + semantic_type_structure::type_description(accumulator_type) + mlc::String(", got ") + semantic_type_structure::type_description(lambda_return_type) + mlc::String(")"), ast::expr_span(method_arguments[1]), diagnostic_codes::diagnostic_code_e047())}) : lambda_errors;
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, lambda_errors)};
argument_types = append_argument_type(argument_types, lambda_parsed.inferred_type);
}
}
}
} else {
{
if (method_name == mlc::String("zip") || method_name == mlc::String("join")){
mlc::Array<ast::Diagnostic> zip_or_join_diagnostics = {};
if (method_arguments.size() > 0){
infer_result::InferResult argument_parsed = infer_expr_fn(method_arguments[0], inference_context);
merged = infer_result::InferResult_absorb(merged, argument_parsed);
argument_types = append_argument_type(argument_types, argument_parsed.inferred_type);
if (method_name == mlc::String("zip")){
{
if (!semantic_type_structure::type_is_unknown(argument_parsed.inferred_type) && !semantic_type_structure::type_is_array(argument_parsed.inferred_type)){
zip_or_join_diagnostics = mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("method zip expects an array argument, got ") + semantic_type_structure::type_description(argument_parsed.inferred_type), ast::expr_span(method_arguments[0]), diagnostic_codes::diagnostic_code_e048())};
}
}
}
if (method_name == mlc::String("join")){
{
if (!semantic_type_structure::type_is_unknown(element_type) && !semantic_type_structure::type_is_string(element_type)){
{
zip_or_join_diagnostics = mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("method join expects an array of string elements, got ") + semantic_type_structure::type_description(element_type), method_span, diagnostic_codes::diagnostic_code_e049())};
}
}
if (!semantic_type_structure::type_is_unknown(argument_parsed.inferred_type) && !semantic_type_structure::type_is_string(argument_parsed.inferred_type)){
zip_or_join_diagnostics = ast::diagnostics_append(zip_or_join_diagnostics, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("method join expects string separator, got ") + semantic_type_structure::type_description(argument_parsed.inferred_type), ast::expr_span(method_arguments[0]), diagnostic_codes::diagnostic_code_e050())});
}
}
}
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, zip_or_join_diagnostics)};
}
} else {
if (method_name == mlc::String("take") || method_name == mlc::String("drop")){
if (method_arguments.size() > 0){
infer_result::InferResult count_parsed = infer_expr_fn(method_arguments[0], inference_context);
merged = infer_result::InferResult_absorb(merged, count_parsed);
argument_types = append_argument_type(argument_types, count_parsed.inferred_type);
mlc::Array<ast::Diagnostic> count_errors = {};
if (!semantic_type_structure::type_is_unknown(count_parsed.inferred_type) && !semantic_type_structure::type_is_i32(count_parsed.inferred_type)){
{
count_errors = mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("method ") + method_name + mlc::String(" expects i32, got ") + semantic_type_structure::type_description(count_parsed.inferred_type), ast::expr_span(method_arguments[0]), diagnostic_codes::diagnostic_code_e051())};
}
}
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, count_errors)};
}
} else {
if (method_name == mlc::String("enumerate") || method_name == mlc::String("sum") || method_name == mlc::String("flat")){
mlc::Array<ast::Diagnostic> extra = {};
if (method_name == mlc::String("sum") && !semantic_type_structure::type_is_unknown(element_type) && !semantic_type_structure::type_is_i32(element_type)){
{
extra = mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("method sum expects i32 array elements, got ") + semantic_type_structure::type_description(element_type), method_span, diagnostic_codes::diagnostic_code_e052())};
}
}
if (method_name == mlc::String("flat") && !semantic_type_structure::type_is_array(element_type) && !semantic_type_structure::type_is_unknown(element_type)){
{
extra = mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("method flat expects an array of arrays, got ") + semantic_type_structure::type_description(element_type), method_span, diagnostic_codes::diagnostic_code_e053())};
}
}
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, extra)};
} else {
if (method_arguments.size() > 0){
mlc::Array<ast::Diagnostic> lambda_errors = append_lambda_arity_diag({}, method_arguments[0], 1, method_name);
infer_result::InferResult lambda_parsed = infer_expr_maybe_lambda(method_arguments[0], mlc::Array<std::shared_ptr<registry::Type>>{element_type}, inference_context, infer_expr_fn);
merged = infer_result::InferResult_absorb(merged, lambda_parsed);
lambda_errors = append_lambda_arity_diag(lambda_errors, method_arguments[0], 1, method_name);
lambda_errors = method_expects_bool_predicate(method_name) ? append_predicate_bool_diag(lambda_errors, method_arguments[0], lambda_parsed.inferred_type, method_name) : lambda_errors;
lambda_errors = method_name == mlc::String("flat_map") ? [&]() -> mlc::Array<ast::Diagnostic> { 
  std::shared_ptr<registry::Type> callback_return = semantic_type_structure::function_return_type(lambda_parsed.inferred_type);
  return !semantic_type_structure::type_is_unknown(callback_return) && !semantic_type_structure::type_is_array(callback_return) ? ast::diagnostics_append(lambda_errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("flat_map callback must return an array, got ") + semantic_type_structure::type_description(callback_return), ast::expr_span(method_arguments[0]), diagnostic_codes::diagnostic_code_e054())}) : lambda_errors;
 }() : lambda_errors;
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, lambda_errors)};
argument_types = append_argument_type(argument_types, lambda_parsed.inferred_type);
}
}
}
}
}
}
return infer_result::InferResult{hof_method_spec::hof_call_result_type(merged.inferred_type, method_name, argument_types), ast::diagnostics_append(ast::diagnostics_append(merged.errors, receiver_errors), arity_errors)};
}

bool should_infer_as_array_hof(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept{return hof_method_spec::is_array_hof_method_on_receiver(receiver_type, method_name);}

} // namespace infer_array_method
