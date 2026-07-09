#include "infer_mutex_method.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "semantic_type_structure.hpp"
#include "mutex_method_types.hpp"
#include "type_diagnostics.hpp"
#include "infer_lambda_context.hpp"
#include "diagnostic_codes.hpp"

namespace infer_mutex_method {

using namespace ast;
using namespace registry;
using namespace check_context;
using namespace infer_result;
using namespace semantic_type_structure;
using namespace mutex_method_types;
using namespace type_diagnostics;
using namespace infer_lambda_context;
using namespace diagnostic_codes;
using namespace ast_tokens;

bool is_mutex_static_new(std::shared_ptr<ast::Expr> object, mlc::String method_name) noexcept;

infer_result::InferResult infer_mutex_static_new_call(infer_result::InferResult object_parsed, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

infer_result::InferResult infer_expr_maybe_lambda(std::shared_ptr<ast::Expr> expression, mlc::Array<std::shared_ptr<registry::Type>> expected_param_types, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

mlc::Array<ast::Diagnostic> mutex_lock_lambda_errors(std::shared_ptr<ast::Expr> callback_expression, ast::Span method_span) noexcept;

mlc::Array<ast::Diagnostic> mutex_lock_receiver_error(std::shared_ptr<registry::Type> mutex_type, ast::Span method_span) noexcept;

infer_result::InferResult infer_mutex_method_call(infer_result::InferResult object_parsed, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

bool is_mutex_static_new(std::shared_ptr<ast::Expr> object, mlc::String method_name) noexcept{return [&]() { if (std::holds_alternative<ast::ExprIdent>((*object)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*object)._); auto [name, _w0] = _v_exprident; return name == mlc::String("Mutex") && method_name == mlc::String("new"); } return false; }();}

infer_result::InferResult infer_mutex_static_new_call(infer_result::InferResult object_parsed, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept{
mlc::Array<ast::Diagnostic> receiver_errors = {};
mlc::Array<ast::Diagnostic> arity_errors = type_diagnostics::method_arity_after_receiver(receiver_errors, mlc::String("new"), method_arguments.size(), method_span);
return method_arguments.size() != 1 ? infer_result::InferResult{std::make_shared<registry::Type>((registry::TUnknown{})), ast::diagnostics_append(object_parsed.errors, arity_errors)} : [&]() -> infer_result::InferResult { 
  infer_result::InferResult argument_parsed = infer_expr_fn(method_arguments[0], inference_context);
  mlc::Array<ast::Diagnostic> errors = ast::diagnostics_append(object_parsed.errors, arity_errors);
  errors = ast::diagnostics_append(errors, argument_parsed.errors);
  return infer_result::InferResult{std::make_shared<registry::Type>(registry::TGeneric(mlc::String("Mutex"), mlc::Array<std::shared_ptr<registry::Type>>{argument_parsed.inferred_type})), errors};
 }();
}

infer_result::InferResult infer_expr_maybe_lambda(std::shared_ptr<ast::Expr> expression, mlc::Array<std::shared_ptr<registry::Type>> expected_param_types, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept{return [&]() -> infer_result::InferResult { if (std::holds_alternative<ast::ExprLambda>((*expression)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*expression)._); auto [parameter_names, body, _w0] = _v_exprlambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, expected_param_types, body, inference_context, infer_expr_fn); } return infer_expr_fn(expression, inference_context); }();}

mlc::Array<ast::Diagnostic> mutex_lock_lambda_errors(std::shared_ptr<ast::Expr> callback_expression, ast::Span method_span) noexcept{return [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<ast::ExprLambda>((*callback_expression)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*callback_expression)._); auto [parameter_names, _w0, _w1] = _v_exprlambda; return parameter_names.size() != 1 ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("mutex.lock expects a callback with one parameter"), ast::expr_span(callback_expression), diagnostic_codes::diagnostic_code_e082())} : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty_diagnostics = {};
  return empty_diagnostics;
 }(); } return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("mutex.lock expects a lambda callback"), ast::expr_span(callback_expression), diagnostic_codes::diagnostic_code_e082())}; }();}

mlc::Array<ast::Diagnostic> mutex_lock_receiver_error(std::shared_ptr<registry::Type> mutex_type, ast::Span method_span) noexcept{return !semantic_type_structure::type_is_unknown(mutex_type) && !semantic_type_structure::type_is_mutex(mutex_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("method lock expects a Mutex receiver, got ") + semantic_type_structure::type_description(mutex_type), method_span, diagnostic_codes::diagnostic_code_e082())} : mlc::Array<ast::Diagnostic>{};}

infer_result::InferResult infer_mutex_method_call(infer_result::InferResult object_parsed, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept{
std::shared_ptr<registry::Type> mutex_type = object_parsed.inferred_type;
mlc::Array<ast::Diagnostic> receiver_errors = mutex_lock_receiver_error(mutex_type, method_span);
mlc::Array<ast::Diagnostic> arity_errors = type_diagnostics::method_arity_after_receiver(receiver_errors, method_name, method_arguments.size(), method_span);
mlc::Array<ast::Diagnostic> errors = ast::diagnostics_append(object_parsed.errors, arity_errors);
std::shared_ptr<registry::Type> inner_type = semantic_type_structure::mutex_inner_type_from_mutex_type(mutex_type);
return method_name == mlc::String("lock") && method_arguments.size() == 1 ? [&]() -> infer_result::InferResult { 
  errors = ast::diagnostics_append(errors, mutex_lock_lambda_errors(method_arguments[0], method_span));
  infer_result::InferResult callback_parsed = infer_expr_maybe_lambda(method_arguments[0], mlc::Array<std::shared_ptr<registry::Type>>{inner_type}, inference_context, infer_expr_fn);
  errors = ast::diagnostics_append(errors, callback_parsed.errors);
  return infer_result::InferResult{semantic_type_structure::function_return_type(callback_parsed.inferred_type), errors};
 }() : infer_result::InferResult{std::make_shared<registry::Type>((registry::TUnknown{})), errors};
}

} // namespace infer_mutex_method
