#include "infer_result_option_method.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "semantic_type_structure.hpp"
#include "result_option_method_types.hpp"
#include "hof_method_spec.hpp"
#include "infer_lambda_context.hpp"
#include "method_receiver_diagnostics.hpp"
#include "diagnostic_codes.hpp"

namespace infer_result_option_method {

using namespace ast;
using namespace registry;
using namespace check_context;
using namespace infer_result;
using namespace semantic_type_structure;
using namespace result_option_method_types;
using namespace hof_method_spec;
using namespace infer_lambda_context;
using namespace method_receiver_diagnostics;
using namespace diagnostic_codes;
using namespace ast_tokens;

mlc::Array<ast::Diagnostic> append_lambda_arity(mlc::Array<ast::Diagnostic> errors, std::shared_ptr<ast::Expr> expression, int expected, mlc::String method_name) noexcept;

mlc::Array<std::shared_ptr<registry::Type>> append_argument_type(mlc::Array<std::shared_ptr<registry::Type>> argument_types_so_far, std::shared_ptr<registry::Type> type_to_append) noexcept;

bool is_result_type(std::shared_ptr<registry::Type> type_value) noexcept;

std::shared_ptr<registry::Type> result_inner_error_from_return_type(std::shared_ptr<registry::Type> lambda_return_type) noexcept;

bool should_infer_as_result_option_hof(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept;

infer_result::InferResult infer_result_option_hof_method_call(infer_result::InferResult object_parsed, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

mlc::Array<ast::Diagnostic> append_lambda_arity(mlc::Array<ast::Diagnostic> errors, std::shared_ptr<ast::Expr> expression, int expected, mlc::String method_name) noexcept{return [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<ast::ExprLambda>((*expression)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*expression)._); auto [parameter_names, _w0, _w1] = _v_exprlambda; return parameter_names.size() != expected ? ast::diagnostics_append(errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("method ") + method_name + mlc::String(" expects a lambda with ") + mlc::to_string(expected) + mlc::String(" parameter(s)"), ast::expr_span(expression), diagnostic_codes::diagnostic_code_e055())}) : errors; } return errors; }();}

mlc::Array<std::shared_ptr<registry::Type>> append_argument_type(mlc::Array<std::shared_ptr<registry::Type>> argument_types_so_far, std::shared_ptr<registry::Type> type_to_append) noexcept{return argument_types_so_far.concat(mlc::Array<std::shared_ptr<registry::Type>>{type_to_append});}

bool is_result_type(std::shared_ptr<registry::Type> type_value) noexcept{return [&]() { if (std::holds_alternative<registry::TGeneric>((*type_value))) { auto _v_tgeneric = std::get<registry::TGeneric>((*type_value)); auto [name, _w0] = _v_tgeneric; return name == mlc::String("Result"); } return false; }();}

std::shared_ptr<registry::Type> result_inner_error_from_return_type(std::shared_ptr<registry::Type> lambda_return_type) noexcept{return [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TGeneric>((*lambda_return_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*lambda_return_type)); auto [name, type_arguments] = _v_tgeneric; return name == mlc::String("Result") && type_arguments.size() >= 2 ? type_arguments[1] : std::make_shared<registry::Type>((registry::TUnknown{})); } return std::make_shared<registry::Type>((registry::TUnknown{})); }();}

bool should_infer_as_result_option_hof(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept{return hof_method_spec::is_result_option_hof_method(receiver_type, method_name);}

infer_result::InferResult infer_result_option_hof_method_call(infer_result::InferResult object_parsed, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept{
std::shared_ptr<registry::Type> receiver_type = object_parsed.inferred_type;
mlc::Array<ast::Diagnostic> receiver_errors = method_receiver_diagnostics::infer_builtin_method_receiver_diagnostics(receiver_type, method_name, method_span);
int expected_arity = result_option_method_types::result_option_hof_expected_argument_count(receiver_type, method_name);
mlc::Array<ast::Diagnostic> arity_errors = expected_arity >= 0 ? method_arguments.size() != expected_arity ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("expected ") + mlc::to_string(expected_arity) + mlc::String(" arguments, got ") + mlc::to_string(method_arguments.size()), method_span, diagnostic_codes::diagnostic_code_e014())} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
infer_result::InferResult merged = std::move(object_parsed);
mlc::Array<std::shared_ptr<registry::Type>> argument_types = {};
if (result_option_method_types::is_result_generic(receiver_type)){
{
std::shared_ptr<registry::Type> result_ok_type_value = result_option_method_types::result_ok_type(receiver_type);
std::shared_ptr<registry::Type> result_error_type_value = result_option_method_types::result_err_type(receiver_type);
if (method_name == mlc::String("unwrap_or") && method_arguments.size() > 0){
infer_result::InferResult argument_parsed = infer_expr_fn(method_arguments[0], inference_context);
merged = infer_result::InferResult_absorb(merged, argument_parsed);
argument_types = append_argument_type(argument_types, argument_parsed.inferred_type);
if (!semantic_type_structure::type_is_unknown(result_ok_type_value) && !semantic_type_structure::type_is_unknown(argument_parsed.inferred_type) && !semantic_type_structure::types_structurally_equal(result_ok_type_value, argument_parsed.inferred_type)){
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("method unwrap_or value must match Ok type (expected ") + semantic_type_structure::type_description(result_ok_type_value) + mlc::String(")"), ast::expr_span(method_arguments[0]), diagnostic_codes::diagnostic_code_e056())})};
}
} else {
if (method_name == mlc::String("unwrap_or_else") && method_arguments.size() > 0){
std::shared_ptr<ast::Expr> callback_argument = method_arguments[0];
mlc::Array<ast::Diagnostic> lambda_arity_errors = append_lambda_arity({}, callback_argument, 1, method_name);
infer_result::InferResult lambda_parsed = [&]() -> infer_result::InferResult { if (std::holds_alternative<ast::ExprLambda>((*callback_argument)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*callback_argument)._); auto [parameter_names, body, _w0] = _v_exprlambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array<std::shared_ptr<registry::Type>>{result_error_type_value}, body, inference_context, infer_expr_fn); } return infer_expr_fn(callback_argument, inference_context); }();
lambda_arity_errors = append_lambda_arity(lambda_arity_errors, callback_argument, 1, method_name);
merged = infer_result::InferResult_absorb(merged, lambda_parsed);
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, lambda_arity_errors)};
std::shared_ptr<registry::Type> lambda_return_type = semantic_type_structure::function_return_type(lambda_parsed.inferred_type);
if (!semantic_type_structure::type_is_unknown(result_ok_type_value) && !semantic_type_structure::type_is_unknown(lambda_return_type) && !semantic_type_structure::types_structurally_equal(result_ok_type_value, lambda_return_type)){
{
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("method unwrap_or_else must return the Ok type (expected ") + semantic_type_structure::type_description(result_ok_type_value) + mlc::String(")"), ast::expr_span(callback_argument), diagnostic_codes::diagnostic_code_e057())})};
}
}
argument_types = append_argument_type(argument_types, lambda_parsed.inferred_type);
} else {
if (method_name == mlc::String("map") && method_arguments.size() > 0){
std::shared_ptr<ast::Expr> callback_argument = method_arguments[0];
mlc::Array<ast::Diagnostic> lambda_arity_errors = append_lambda_arity({}, callback_argument, 1, method_name);
infer_result::InferResult lambda_parsed = [&]() -> infer_result::InferResult { if (std::holds_alternative<ast::ExprLambda>((*callback_argument)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*callback_argument)._); auto [parameter_names, body, _w0] = _v_exprlambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array<std::shared_ptr<registry::Type>>{result_ok_type_value}, body, inference_context, infer_expr_fn); } return infer_expr_fn(callback_argument, inference_context); }();
lambda_arity_errors = append_lambda_arity(lambda_arity_errors, callback_argument, 1, method_name);
merged = infer_result::InferResult_absorb(merged, lambda_parsed);
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, lambda_arity_errors)};
argument_types = append_argument_type(argument_types, lambda_parsed.inferred_type);
} else {
if (method_name == mlc::String("map_err") && method_arguments.size() > 0){
std::shared_ptr<ast::Expr> callback_argument = method_arguments[0];
mlc::Array<ast::Diagnostic> lambda_arity_errors = append_lambda_arity({}, callback_argument, 1, method_name);
infer_result::InferResult lambda_parsed = [&]() -> infer_result::InferResult { if (std::holds_alternative<ast::ExprLambda>((*callback_argument)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*callback_argument)._); auto [parameter_names, body, _w0] = _v_exprlambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array<std::shared_ptr<registry::Type>>{result_error_type_value}, body, inference_context, infer_expr_fn); } return infer_expr_fn(callback_argument, inference_context); }();
lambda_arity_errors = append_lambda_arity(lambda_arity_errors, callback_argument, 1, method_name);
merged = infer_result::InferResult_absorb(merged, lambda_parsed);
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, lambda_arity_errors)};
argument_types = append_argument_type(argument_types, lambda_parsed.inferred_type);
} else {
if (method_name == mlc::String("and_then") && method_arguments.size() > 0){
std::shared_ptr<ast::Expr> callback_argument = method_arguments[0];
mlc::Array<ast::Diagnostic> lambda_arity_errors = append_lambda_arity({}, callback_argument, 1, method_name);
infer_result::InferResult lambda_parsed = [&]() -> infer_result::InferResult { if (std::holds_alternative<ast::ExprLambda>((*callback_argument)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*callback_argument)._); auto [parameter_names, body, _w0] = _v_exprlambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array<std::shared_ptr<registry::Type>>{result_ok_type_value}, body, inference_context, infer_expr_fn); } return infer_expr_fn(callback_argument, inference_context); }();
lambda_arity_errors = append_lambda_arity(lambda_arity_errors, callback_argument, 1, method_name);
merged = infer_result::InferResult_absorb(merged, lambda_parsed);
mlc::Array<ast::Diagnostic> extra = lambda_arity_errors;
std::shared_ptr<registry::Type> lambda_return_type = semantic_type_structure::function_return_type(lambda_parsed.inferred_type);
if (is_result_type(lambda_return_type)){
{
std::shared_ptr<registry::Type> callback_result_error_type = result_inner_error_from_return_type(lambda_return_type);
if (!semantic_type_structure::type_is_unknown(result_error_type_value) && !semantic_type_structure::type_is_unknown(callback_result_error_type) && !semantic_type_structure::types_structurally_equal(result_error_type_value, callback_result_error_type)){
extra = ast::diagnostics_append(extra, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("method and_then callback must return Result with the same error type (expected ") + semantic_type_structure::type_description(result_error_type_value) + mlc::String(")"), ast::expr_span(callback_argument), diagnostic_codes::diagnostic_code_e058())});
}
}
} else {
{
extra = ast::diagnostics_append(extra, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("method and_then must return a Result type"), ast::expr_span(callback_argument), diagnostic_codes::diagnostic_code_e059())});
}
}
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, extra)};
argument_types = append_argument_type(argument_types, lambda_parsed.inferred_type);
} else {
if (method_name == mlc::String("ok")){
int _noop = 0;
} else {
if (method_name == mlc::String("or_else") && method_arguments.size() > 0){
std::shared_ptr<ast::Expr> callback_argument = method_arguments[0];
mlc::Array<ast::Diagnostic> lambda_arity_errors = append_lambda_arity({}, callback_argument, 1, method_name);
infer_result::InferResult lambda_parsed = [&]() -> infer_result::InferResult { if (std::holds_alternative<ast::ExprLambda>((*callback_argument)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*callback_argument)._); auto [parameter_names, body, _w0] = _v_exprlambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array<std::shared_ptr<registry::Type>>{result_error_type_value}, body, inference_context, infer_expr_fn); } return infer_expr_fn(callback_argument, inference_context); }();
lambda_arity_errors = append_lambda_arity(lambda_arity_errors, callback_argument, 1, method_name);
merged = infer_result::InferResult_absorb(merged, lambda_parsed);
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, lambda_arity_errors)};
argument_types = append_argument_type(argument_types, lambda_parsed.inferred_type);
}
}
}
}
}
}
}
}
} else {
{
if (result_option_method_types::is_option_generic(receiver_type)){
std::shared_ptr<registry::Type> option_inner_type_value = result_option_method_types::option_inner_type(receiver_type);
if (method_name == mlc::String("unwrap_or") && method_arguments.size() > 0){
infer_result::InferResult argument_parsed = infer_expr_fn(method_arguments[0], inference_context);
merged = infer_result::InferResult_absorb(merged, argument_parsed);
argument_types = append_argument_type(argument_types, argument_parsed.inferred_type);
if (!semantic_type_structure::type_is_unknown(option_inner_type_value) && !semantic_type_structure::type_is_unknown(argument_parsed.inferred_type) && !semantic_type_structure::types_structurally_equal(option_inner_type_value, argument_parsed.inferred_type)){
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("method unwrap_or value must match Some type (expected ") + semantic_type_structure::type_description(option_inner_type_value) + mlc::String(")"), ast::expr_span(method_arguments[0]), diagnostic_codes::diagnostic_code_e060())})};
}
} else {
if (method_name == mlc::String("or_else") && method_arguments.size() > 0){
std::shared_ptr<ast::Expr> callback_argument = method_arguments[0];
mlc::Array<ast::Diagnostic> lambda_arity_errors = append_lambda_arity({}, callback_argument, 0, mlc::String("or_else"));
infer_result::InferResult lambda_parsed = [&]() -> infer_result::InferResult { if (std::holds_alternative<ast::ExprLambda>((*callback_argument)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*callback_argument)._); auto [parameter_names, body, _w0] = _v_exprlambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, {}, body, inference_context, infer_expr_fn); } return infer_expr_fn(callback_argument, inference_context); }();
lambda_arity_errors = append_lambda_arity(lambda_arity_errors, callback_argument, 0, mlc::String("or_else"));
merged = infer_result::InferResult_absorb(merged, lambda_parsed);
mlc::Array<ast::Diagnostic> extra = lambda_arity_errors;
std::shared_ptr<registry::Type> lambda_return_type = semantic_type_structure::function_return_type(lambda_parsed.inferred_type);
if (result_option_method_types::is_option_generic(lambda_return_type)){
{
std::shared_ptr<registry::Type> callback_option_inner_type = result_option_method_types::option_inner_type(lambda_return_type);
if (!semantic_type_structure::type_is_unknown(option_inner_type_value) && !semantic_type_structure::type_is_unknown(callback_option_inner_type) && !semantic_type_structure::types_structurally_equal(option_inner_type_value, callback_option_inner_type)){
extra = ast::diagnostics_append(extra, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("method or_else must return Option with the same inner type (expected ") + semantic_type_structure::type_description(option_inner_type_value) + mlc::String(")"), ast::expr_span(callback_argument), diagnostic_codes::diagnostic_code_e061())});
}
}
} else {
{
extra = ast::diagnostics_append(extra, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("method or_else must return Option"), ast::expr_span(callback_argument), diagnostic_codes::diagnostic_code_e062())});
}
}
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, extra)};
argument_types = append_argument_type(argument_types, lambda_parsed.inferred_type);
} else {
if (method_name == mlc::String("ok_or") && method_arguments.size() > 0){
infer_result::InferResult argument_parsed = infer_expr_fn(method_arguments[0], inference_context);
merged = infer_result::InferResult_absorb(merged, argument_parsed);
argument_types = append_argument_type(argument_types, argument_parsed.inferred_type);
} else {
if (method_name == mlc::String("map") && method_arguments.size() > 0){
std::shared_ptr<ast::Expr> callback_argument = method_arguments[0];
mlc::Array<ast::Diagnostic> lambda_arity_errors = append_lambda_arity({}, callback_argument, 1, method_name);
infer_result::InferResult lambda_parsed = [&]() -> infer_result::InferResult { if (std::holds_alternative<ast::ExprLambda>((*callback_argument)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*callback_argument)._); auto [parameter_names, body, _w0] = _v_exprlambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array<std::shared_ptr<registry::Type>>{option_inner_type_value}, body, inference_context, infer_expr_fn); } return infer_expr_fn(callback_argument, inference_context); }();
lambda_arity_errors = append_lambda_arity(lambda_arity_errors, callback_argument, 1, method_name);
merged = infer_result::InferResult_absorb(merged, lambda_parsed);
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, lambda_arity_errors)};
argument_types = append_argument_type(argument_types, lambda_parsed.inferred_type);
} else {
if (method_name == mlc::String("and_then") && method_arguments.size() > 0){
std::shared_ptr<ast::Expr> callback_argument = method_arguments[0];
mlc::Array<ast::Diagnostic> lambda_arity_errors = append_lambda_arity({}, callback_argument, 1, method_name);
infer_result::InferResult lambda_parsed = [&]() -> infer_result::InferResult { if (std::holds_alternative<ast::ExprLambda>((*callback_argument)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*callback_argument)._); auto [parameter_names, body, _w0] = _v_exprlambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array<std::shared_ptr<registry::Type>>{option_inner_type_value}, body, inference_context, infer_expr_fn); } return infer_expr_fn(callback_argument, inference_context); }();
lambda_arity_errors = append_lambda_arity(lambda_arity_errors, callback_argument, 1, method_name);
merged = infer_result::InferResult_absorb(merged, lambda_parsed);
mlc::Array<ast::Diagnostic> extra = lambda_arity_errors;
std::shared_ptr<registry::Type> return_type = semantic_type_structure::function_return_type(lambda_parsed.inferred_type);
if (!result_option_method_types::is_option_generic(return_type)){
{
extra = ast::diagnostics_append(extra, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("method and_then must return Option"), ast::expr_span(callback_argument), diagnostic_codes::diagnostic_code_e063())});
}
}
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, extra)};
argument_types = append_argument_type(argument_types, lambda_parsed.inferred_type);
} else {
if (method_name == mlc::String("filter") && method_arguments.size() > 0){
std::shared_ptr<ast::Expr> callback_argument = method_arguments[0];
mlc::Array<ast::Diagnostic> lambda_arity_errors = append_lambda_arity({}, callback_argument, 1, method_name);
infer_result::InferResult lambda_parsed = [&]() -> infer_result::InferResult { if (std::holds_alternative<ast::ExprLambda>((*callback_argument)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*callback_argument)._); auto [parameter_names, body, _w0] = _v_exprlambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array<std::shared_ptr<registry::Type>>{option_inner_type_value}, body, inference_context, infer_expr_fn); } return infer_expr_fn(callback_argument, inference_context); }();
lambda_arity_errors = append_lambda_arity(lambda_arity_errors, callback_argument, 1, method_name);
merged = infer_result::InferResult_absorb(merged, lambda_parsed);
mlc::Array<ast::Diagnostic> extra = lambda_arity_errors;
std::shared_ptr<registry::Type> lambda_return_type = semantic_type_structure::function_return_type(lambda_parsed.inferred_type);
if (!semantic_type_structure::type_is_unknown(lambda_return_type) && !semantic_type_structure::type_is_bool(lambda_return_type)){
{
extra = ast::diagnostics_append(extra, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("method filter expects a predicate returning bool"), ast::expr_span(callback_argument), diagnostic_codes::diagnostic_code_e064())});
}
}
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, extra)};
argument_types = append_argument_type(argument_types, lambda_parsed.inferred_type);
}
}
}
}
}
}
}
}
}
std::shared_ptr<registry::Type> hof_result_type = hof_method_spec::hof_call_result_type(receiver_type, method_name, argument_types);
return infer_result::InferResult{hof_result_type, ast::diagnostics_append(ast::diagnostics_append(merged.errors, receiver_errors), arity_errors)};
}

} // namespace infer_result_option_method
