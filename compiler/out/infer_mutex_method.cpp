#define main mlc_user_main
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

bool is_mutex_static_new(std::shared_ptr<ast::Expr> object, mlc::String method_name) noexcept{
  return [&]() -> bool {
auto __match_subject = object;
if (std::holds_alternative<ast::ExprIdent>((*__match_subject))) {
const ast::ExprIdent& exprIdent = std::get<ast::ExprIdent>((*__match_subject));
auto [name, __1] = exprIdent; return ((name == mlc::String("Mutex", 5)) && (method_name == mlc::String("new", 3)));
}
return false;
std::abort();
}();
}
infer_result::InferResult infer_mutex_static_new_call(infer_result::InferResult object_parsed, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept{
  auto receiver_errors = mlc::Array<ast::Diagnostic>{};
  auto arity_errors = type_diagnostics::method_arity_after_receiver(receiver_errors, mlc::String("new", 3), method_arguments.length(), method_span);
  if ((method_arguments.length() != 1))   {
    return infer_result::InferResult{std::make_shared<registry::Type>(registry::TUnknown{}), ast::diagnostics_append(object_parsed.errors, arity_errors)};
  } else   {
    auto argument_parsed = infer_expr_fn(method_arguments[0], inference_context);
    auto errors = ast::diagnostics_append(object_parsed.errors, arity_errors);
    (errors = ast::diagnostics_append(errors, argument_parsed.errors));
    return infer_result::InferResult{std::make_shared<registry::Type>(registry::TGeneric{mlc::String("Mutex", 5), mlc::Array<std::shared_ptr<registry::Type>>{argument_parsed.inferred_type}}), errors};
  }
}
infer_result::InferResult infer_expr_maybe_lambda(std::shared_ptr<ast::Expr> expression, mlc::Array<std::shared_ptr<registry::Type>> expected_param_types, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept{
  return [&]() -> infer_result::InferResult {
auto __match_subject = expression;
if (std::holds_alternative<ast::ExprLambda>((*__match_subject))) {
const ast::ExprLambda& exprLambda = std::get<ast::ExprLambda>((*__match_subject));
auto [parameter_names, body, __2] = exprLambda; return infer_lambda_context::infer_lambda_with_param_types(parameter_names, expected_param_types, body, inference_context, infer_expr_fn);
}
return infer_expr_fn(expression, inference_context);
std::abort();
}();
}
mlc::Array<ast::Diagnostic> mutex_lock_lambda_errors(std::shared_ptr<ast::Expr> callback_expression, ast::Span method_span) noexcept{
  return [&]() -> mlc::Array<ast::Diagnostic> {
auto __match_subject = callback_expression;
if (std::holds_alternative<ast::ExprLambda>((*__match_subject))) {
const ast::ExprLambda& exprLambda = std::get<ast::ExprLambda>((*__match_subject));
auto [parameter_names, __1, __2] = exprLambda; return [&]() -> mlc::Array<ast::Diagnostic> {
  if ((parameter_names.length() != 1))   {
    return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("mutex.lock expects a callback with one parameter", 48), ast::expr_span(callback_expression), diagnostic_codes::diagnostic_code_e082())};
  } else   {
    return [&]() {
auto empty_diagnostics = mlc::Array<ast::Diagnostic>{};
return empty_diagnostics;
}();
  }
}();
}
return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("mutex.lock expects a lambda callback", 36), ast::expr_span(callback_expression), diagnostic_codes::diagnostic_code_e082())};
std::abort();
}();
}
mlc::Array<ast::Diagnostic> mutex_lock_receiver_error(std::shared_ptr<registry::Type> mutex_type, ast::Span method_span) noexcept{
  if (((!semantic_type_structure::type_is_unknown(mutex_type)) && (!semantic_type_structure::type_is_mutex(mutex_type))))   {
    return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code((mlc::String("method lock expects a Mutex receiver, got ", 42) + semantic_type_structure::type_description(mutex_type)), method_span, diagnostic_codes::diagnostic_code_e082())};
  } else   {
    return mlc::Array<ast::Diagnostic>{};
  }
}
infer_result::InferResult infer_mutex_method_call(infer_result::InferResult object_parsed, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept{
  auto mutex_type = object_parsed.inferred_type;
  auto receiver_errors = mutex_lock_receiver_error(mutex_type, method_span);
  auto arity_errors = type_diagnostics::method_arity_after_receiver(receiver_errors, method_name, method_arguments.length(), method_span);
  auto errors = ast::diagnostics_append(object_parsed.errors, arity_errors);
  auto inner_type = semantic_type_structure::mutex_inner_type_from_mutex_type(mutex_type);
  if (((method_name == mlc::String("lock", 4)) && (method_arguments.length() == 1)))   {
    (errors = ast::diagnostics_append(errors, mutex_lock_lambda_errors(method_arguments[0], method_span)));
    auto callback_parsed = infer_expr_maybe_lambda(method_arguments[0], mlc::Array<std::shared_ptr<registry::Type>>{inner_type}, inference_context, infer_expr_fn);
    (errors = ast::diagnostics_append(errors, callback_parsed.errors));
    return infer_result::InferResult{semantic_type_structure::function_return_type(callback_parsed.inferred_type), errors};
  } else   {
    return infer_result::InferResult{std::make_shared<registry::Type>(registry::TUnknown{}), errors};
  }
}

} // namespace infer_mutex_method
