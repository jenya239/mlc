#ifndef INFER_MUTEX_METHOD_HPP
#define INFER_MUTEX_METHOD_HPP

#include "mlc.hpp"
#include <variant>
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

bool is_mutex_static_new(std::shared_ptr<ast::Expr> object, mlc::String method_name) noexcept;
template<typename __F4>
infer_result::InferResult infer_mutex_static_new_call(infer_result::InferResult object_parsed, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, __F4 infer_expr_fn) noexcept;
infer_result::InferResult infer_expr_maybe_lambda(std::shared_ptr<ast::Expr> expression, mlc::Array<std::shared_ptr<registry::Type>> expected_param_types, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;
mlc::Array<ast::Diagnostic> mutex_lock_lambda_errors(std::shared_ptr<ast::Expr> callback_expression, ast::Span method_span) noexcept;
mlc::Array<ast::Diagnostic> mutex_lock_receiver_error(std::shared_ptr<registry::Type> mutex_type, ast::Span method_span) noexcept;
infer_result::InferResult infer_mutex_method_call(infer_result::InferResult object_parsed, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;
template<typename __F4>
infer_result::InferResult infer_mutex_static_new_call(infer_result::InferResult object_parsed, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, __F4 infer_expr_fn) noexcept{
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

} // namespace infer_mutex_method

#endif // INFER_MUTEX_METHOD_HPP
