#include "infer_result_option_method.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "semantic_type_structure.hpp"
#include "result_option_method_types.hpp"
#include "infer_lambda_context.hpp"
#include "method_receiver_diagnostics.hpp"

namespace infer_result_option_method {

using namespace ast;
using namespace registry;
using namespace check_context;
using namespace infer_result;
using namespace semantic_type_structure;
using namespace result_option_method_types;
using namespace infer_lambda_context;
using namespace method_receiver_diagnostics;
using namespace ast_tokens;

using InferExprFn = std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)>;

mlc::Array<ast::Diagnostic> append_lambda_arity(mlc::Array<ast::Diagnostic> errors, std::shared_ptr<ast::Expr> expression, int expected, mlc::String method_name) noexcept;

bool is_result_type(std::shared_ptr<registry::Type> t) noexcept;

std::shared_ptr<registry::Type> result_inner_err_from_ret(std::shared_ptr<registry::Type> lam_ret) noexcept;

bool should_infer_as_result_option_hof(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept;

infer_result::InferResult infer_result_option_hof_method_call(infer_result::InferResult object_result, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

mlc::Array<ast::Diagnostic> append_lambda_arity(mlc::Array<ast::Diagnostic> errors, std::shared_ptr<ast::Expr> expression, int expected, mlc::String method_name) noexcept{return [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<ast::ExprLambda>((*expression)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*expression)._); auto [parameter_names, _w0, _w1] = _v_exprlambda; return parameter_names.size() != expected ? ast::diagnostics_append(errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method ") + method_name + mlc::String(" expects a lambda with ") + mlc::to_string(expected) + mlc::String(" parameter(s)"), ast::expr_span(expression))}) : errors; } return errors; }();}

bool is_result_type(std::shared_ptr<registry::Type> t) noexcept{return [&]() { if (std::holds_alternative<registry::TGeneric>((*t))) { auto _v_tgeneric = std::get<registry::TGeneric>((*t)); auto [name, _w0] = _v_tgeneric; return name == mlc::String("Result"); } return false; }();}

std::shared_ptr<registry::Type> result_inner_err_from_ret(std::shared_ptr<registry::Type> lam_ret) noexcept{return [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TGeneric>((*lam_ret))) { auto _v_tgeneric = std::get<registry::TGeneric>((*lam_ret)); auto [name, a] = _v_tgeneric; return name == mlc::String("Result") && a.size() >= 2 ? a[1] : std::make_shared<registry::Type>((registry::TUnknown{})); } return std::make_shared<registry::Type>((registry::TUnknown{})); }();}

bool should_infer_as_result_option_hof(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept{return result_option_method_types::should_infer_result_option_combinator(receiver_type, method_name);}

infer_result::InferResult infer_result_option_hof_method_call(infer_result::InferResult object_result, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept{
std::shared_ptr<registry::Type> receiver_type = object_result.inferred_type;
mlc::Array<ast::Diagnostic> receiver_errors = method_receiver_diagnostics::infer_builtin_method_receiver_diagnostics(receiver_type, method_name, method_span);
int expected_arity = result_option_method_types::result_option_hof_expected_argument_count(receiver_type, method_name);
mlc::Array<ast::Diagnostic> arity_errors = expected_arity >= 0 ? method_arguments.size() != expected_arity ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("expected ") + mlc::to_string(expected_arity) + mlc::String(" arguments, got ") + mlc::to_string(method_arguments.size()), method_span)} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
infer_result::InferResult merged = std::move(object_result);
mlc::Array<std::shared_ptr<registry::Type>> arg_types = {};
if (result_option_method_types::is_result_generic(receiver_type)){
{
std::shared_ptr<registry::Type> t_t = result_option_method_types::result_ok_type(receiver_type);
std::shared_ptr<registry::Type> e_t = result_option_method_types::result_err_type(receiver_type);
if (method_name == mlc::String("unwrap_or") && method_arguments.size() > 0){
infer_result::InferResult a = infer_expr_fn(method_arguments[0], inference_context);
merged = infer_result::InferResult_absorb(merged, a);
arg_types.push_back(a.inferred_type);
if (!semantic_type_structure::type_is_unknown(t_t) && !semantic_type_structure::type_is_unknown(a.inferred_type) && !semantic_type_structure::types_structurally_equal(t_t, a.inferred_type)){
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method unwrap_or value must match Ok type (expected ") + semantic_type_structure::type_description(t_t) + mlc::String(")"), ast::expr_span(method_arguments[0]))})};
}
} else {
if (method_name == mlc::String("unwrap_or_else") && method_arguments.size() > 0){
std::shared_ptr<ast::Expr> arg0 = method_arguments[0];
mlc::Array<ast::Diagnostic> ar = append_lambda_arity({}, arg0, 1, method_name);
infer_result::InferResult lam_r = [&]() -> infer_result::InferResult { if (std::holds_alternative<ast::ExprLambda>((*arg0)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*arg0)._); auto [parameter_names, body, _w0] = _v_exprlambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array<std::shared_ptr<registry::Type>>{e_t}, body, inference_context, infer_expr_fn); } return infer_expr_fn(arg0, inference_context); }();
ar = append_lambda_arity(ar, arg0, 1, method_name);
merged = infer_result::InferResult_absorb(merged, lam_r);
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, ar)};
std::shared_ptr<registry::Type> lam_ret = semantic_type_structure::function_return_type(lam_r.inferred_type);
if (!semantic_type_structure::type_is_unknown(t_t) && !semantic_type_structure::type_is_unknown(lam_ret) && !semantic_type_structure::types_structurally_equal(t_t, lam_ret)){
{
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method unwrap_or_else must return the Ok type (expected ") + semantic_type_structure::type_description(t_t) + mlc::String(")"), ast::expr_span(arg0))})};
}
}
arg_types.push_back(lam_r.inferred_type);
} else {
if (method_name == mlc::String("map") && method_arguments.size() > 0){
std::shared_ptr<ast::Expr> arg0 = method_arguments[0];
mlc::Array<ast::Diagnostic> ar = append_lambda_arity({}, arg0, 1, method_name);
infer_result::InferResult lam_r = [&]() -> infer_result::InferResult { if (std::holds_alternative<ast::ExprLambda>((*arg0)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*arg0)._); auto [parameter_names, body, _w0] = _v_exprlambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array<std::shared_ptr<registry::Type>>{t_t}, body, inference_context, infer_expr_fn); } return infer_expr_fn(arg0, inference_context); }();
ar = append_lambda_arity(ar, arg0, 1, method_name);
merged = infer_result::InferResult_absorb(merged, lam_r);
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, ar)};
arg_types.push_back(lam_r.inferred_type);
} else {
if (method_name == mlc::String("map_err") && method_arguments.size() > 0){
std::shared_ptr<ast::Expr> arg0 = method_arguments[0];
mlc::Array<ast::Diagnostic> ar = append_lambda_arity({}, arg0, 1, method_name);
infer_result::InferResult lam_r = [&]() -> infer_result::InferResult { if (std::holds_alternative<ast::ExprLambda>((*arg0)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*arg0)._); auto [parameter_names, body, _w0] = _v_exprlambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array<std::shared_ptr<registry::Type>>{e_t}, body, inference_context, infer_expr_fn); } return infer_expr_fn(arg0, inference_context); }();
ar = append_lambda_arity(ar, arg0, 1, method_name);
merged = infer_result::InferResult_absorb(merged, lam_r);
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, ar)};
arg_types.push_back(lam_r.inferred_type);
} else {
if (method_name == mlc::String("and_then") && method_arguments.size() > 0){
std::shared_ptr<ast::Expr> arg0 = method_arguments[0];
mlc::Array<ast::Diagnostic> ar = append_lambda_arity({}, arg0, 1, method_name);
infer_result::InferResult lam_r = [&]() -> infer_result::InferResult { if (std::holds_alternative<ast::ExprLambda>((*arg0)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*arg0)._); auto [parameter_names, body, _w0] = _v_exprlambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array<std::shared_ptr<registry::Type>>{t_t}, body, inference_context, infer_expr_fn); } return infer_expr_fn(arg0, inference_context); }();
ar = append_lambda_arity(ar, arg0, 1, method_name);
merged = infer_result::InferResult_absorb(merged, lam_r);
mlc::Array<ast::Diagnostic> extra = ar;
std::shared_ptr<registry::Type> lam_ret = semantic_type_structure::function_return_type(lam_r.inferred_type);
if (is_result_type(lam_ret)){
{
std::shared_ptr<registry::Type> inner_e = result_inner_err_from_ret(lam_ret);
if (!semantic_type_structure::type_is_unknown(e_t) && !semantic_type_structure::type_is_unknown(inner_e) && !semantic_type_structure::types_structurally_equal(e_t, inner_e)){
extra = ast::diagnostics_append(extra, mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method and_then callback must return Result with the same error type (expected ") + semantic_type_structure::type_description(e_t) + mlc::String(")"), ast::expr_span(arg0))});
}
}
} else {
{
extra = ast::diagnostics_append(extra, mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method and_then must return a Result type"), ast::expr_span(arg0))});
}
}
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, extra)};
arg_types.push_back(lam_r.inferred_type);
} else {
if (method_name == mlc::String("ok")){
int _noop = 0;
} else {
if (method_name == mlc::String("or_else") && method_arguments.size() > 0){
std::shared_ptr<ast::Expr> arg0 = method_arguments[0];
mlc::Array<ast::Diagnostic> ar = append_lambda_arity({}, arg0, 1, method_name);
infer_result::InferResult lam_r = [&]() -> infer_result::InferResult { if (std::holds_alternative<ast::ExprLambda>((*arg0)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*arg0)._); auto [parameter_names, body, _w0] = _v_exprlambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array<std::shared_ptr<registry::Type>>{e_t}, body, inference_context, infer_expr_fn); } return infer_expr_fn(arg0, inference_context); }();
ar = append_lambda_arity(ar, arg0, 1, method_name);
merged = infer_result::InferResult_absorb(merged, lam_r);
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, ar)};
arg_types.push_back(lam_r.inferred_type);
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
std::shared_ptr<registry::Type> o_t = result_option_method_types::option_inner_type(receiver_type);
if (method_name == mlc::String("unwrap_or") && method_arguments.size() > 0){
infer_result::InferResult a = infer_expr_fn(method_arguments[0], inference_context);
merged = infer_result::InferResult_absorb(merged, a);
arg_types.push_back(a.inferred_type);
if (!semantic_type_structure::type_is_unknown(o_t) && !semantic_type_structure::type_is_unknown(a.inferred_type) && !semantic_type_structure::types_structurally_equal(o_t, a.inferred_type)){
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method unwrap_or value must match Some type (expected ") + semantic_type_structure::type_description(o_t) + mlc::String(")"), ast::expr_span(method_arguments[0]))})};
}
} else {
if (method_name == mlc::String("or_else") && method_arguments.size() > 0){
std::shared_ptr<ast::Expr> arg0 = method_arguments[0];
mlc::Array<ast::Diagnostic> ar = append_lambda_arity({}, arg0, 0, mlc::String("or_else"));
infer_result::InferResult lam_r = [&]() -> infer_result::InferResult { if (std::holds_alternative<ast::ExprLambda>((*arg0)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*arg0)._); auto [parameter_names, body, _w0] = _v_exprlambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, {}, body, inference_context, infer_expr_fn); } return infer_expr_fn(arg0, inference_context); }();
ar = append_lambda_arity(ar, arg0, 0, mlc::String("or_else"));
merged = infer_result::InferResult_absorb(merged, lam_r);
mlc::Array<ast::Diagnostic> extra = ar;
std::shared_ptr<registry::Type> inner_ret = semantic_type_structure::function_return_type(lam_r.inferred_type);
if (result_option_method_types::is_option_generic(inner_ret)){
{
std::shared_ptr<registry::Type> io = result_option_method_types::option_inner_type(inner_ret);
if (!semantic_type_structure::type_is_unknown(o_t) && !semantic_type_structure::type_is_unknown(io) && !semantic_type_structure::types_structurally_equal(o_t, io)){
extra = ast::diagnostics_append(extra, mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method or_else must return Option with the same inner type (expected ") + semantic_type_structure::type_description(o_t) + mlc::String(")"), ast::expr_span(arg0))});
}
}
} else {
{
extra = ast::diagnostics_append(extra, mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method or_else must return Option"), ast::expr_span(arg0))});
}
}
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, extra)};
arg_types.push_back(lam_r.inferred_type);
} else {
if (method_name == mlc::String("ok_or") && method_arguments.size() > 0){
infer_result::InferResult a = infer_expr_fn(method_arguments[0], inference_context);
merged = infer_result::InferResult_absorb(merged, a);
arg_types.push_back(a.inferred_type);
} else {
if (method_name == mlc::String("map") && method_arguments.size() > 0){
std::shared_ptr<ast::Expr> arg0 = method_arguments[0];
mlc::Array<ast::Diagnostic> ar = append_lambda_arity({}, arg0, 1, method_name);
infer_result::InferResult lam_r = [&]() -> infer_result::InferResult { if (std::holds_alternative<ast::ExprLambda>((*arg0)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*arg0)._); auto [parameter_names, body, _w0] = _v_exprlambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array<std::shared_ptr<registry::Type>>{o_t}, body, inference_context, infer_expr_fn); } return infer_expr_fn(arg0, inference_context); }();
ar = append_lambda_arity(ar, arg0, 1, method_name);
merged = infer_result::InferResult_absorb(merged, lam_r);
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, ar)};
arg_types.push_back(lam_r.inferred_type);
} else {
if (method_name == mlc::String("and_then") && method_arguments.size() > 0){
std::shared_ptr<ast::Expr> arg0 = method_arguments[0];
mlc::Array<ast::Diagnostic> ar = append_lambda_arity({}, arg0, 1, method_name);
infer_result::InferResult lam_r = [&]() -> infer_result::InferResult { if (std::holds_alternative<ast::ExprLambda>((*arg0)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*arg0)._); auto [parameter_names, body, _w0] = _v_exprlambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array<std::shared_ptr<registry::Type>>{o_t}, body, inference_context, infer_expr_fn); } return infer_expr_fn(arg0, inference_context); }();
ar = append_lambda_arity(ar, arg0, 1, method_name);
merged = infer_result::InferResult_absorb(merged, lam_r);
mlc::Array<ast::Diagnostic> extra = ar;
std::shared_ptr<registry::Type> ret = semantic_type_structure::function_return_type(lam_r.inferred_type);
if (!result_option_method_types::is_option_generic(ret)){
{
extra = ast::diagnostics_append(extra, mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method and_then must return Option"), ast::expr_span(arg0))});
}
}
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, extra)};
arg_types.push_back(lam_r.inferred_type);
} else {
if (method_name == mlc::String("filter") && method_arguments.size() > 0){
std::shared_ptr<ast::Expr> arg0 = method_arguments[0];
mlc::Array<ast::Diagnostic> ar = append_lambda_arity({}, arg0, 1, method_name);
infer_result::InferResult lam_r = [&]() -> infer_result::InferResult { if (std::holds_alternative<ast::ExprLambda>((*arg0)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*arg0)._); auto [parameter_names, body, _w0] = _v_exprlambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, mlc::Array<std::shared_ptr<registry::Type>>{o_t}, body, inference_context, infer_expr_fn); } return infer_expr_fn(arg0, inference_context); }();
ar = append_lambda_arity(ar, arg0, 1, method_name);
merged = infer_result::InferResult_absorb(merged, lam_r);
mlc::Array<ast::Diagnostic> extra = ar;
std::shared_ptr<registry::Type> lam_ret = semantic_type_structure::function_return_type(lam_r.inferred_type);
if (!semantic_type_structure::type_is_unknown(lam_ret) && !semantic_type_structure::type_is_bool(lam_ret)){
{
extra = ast::diagnostics_append(extra, mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method filter expects a predicate returning bool"), ast::expr_span(arg0))});
}
}
merged = infer_result::InferResult{merged.inferred_type, ast::diagnostics_append(merged.errors, extra)};
arg_types.push_back(lam_r.inferred_type);
}
}
}
}
}
}
}
}
}
std::shared_ptr<registry::Type> out = result_option_method_types::result_option_hof_call_result_type(receiver_type, method_name, arg_types);
return infer_result::InferResult{out, ast::diagnostics_append(ast::diagnostics_append(merged.errors, receiver_errors), arity_errors)};
}

} // namespace infer_result_option_method
