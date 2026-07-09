#ifndef INFER_WEAK_METHOD_HPP
#define INFER_WEAK_METHOD_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "semantic_type_structure.hpp"
#include "type_diagnostics.hpp"
namespace infer_weak_method {

bool type_is_weak_pointer(std::shared_ptr<registry::Type> type_value) noexcept;
std::shared_ptr<registry::Type> weak_pointer_inner_type(std::shared_ptr<registry::Type> type_value) noexcept;
bool is_shared_static_new(std::shared_ptr<ast::Expr> object, mlc::String method_name) noexcept;
template<typename __F4>
infer_result::InferResult infer_shared_static_new_call(infer_result::InferResult object_parsed, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, __F4 infer_expr_fn) noexcept;
bool is_shared_weak_method(mlc::String method_name) noexcept;
bool is_weak_upgrade_method(mlc::String method_name) noexcept;
bool is_shared_weak_method_on_receiver(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept;
bool is_weak_upgrade_method_on_receiver(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept;
template<typename __F5>
infer_result::InferResult infer_shared_weak_method_call(infer_result::InferResult object_parsed, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext _inference_context, __F5 _infer_expr_fn) noexcept;
template<typename __F5>
infer_result::InferResult infer_weak_upgrade_method_call(infer_result::InferResult object_parsed, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext _inference_context, __F5 _infer_expr_fn) noexcept;
template<typename __F4>
infer_result::InferResult infer_shared_static_new_call(infer_result::InferResult object_parsed, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, __F4 infer_expr_fn) noexcept{
  auto empty_errors = mlc::Array<ast::Diagnostic>{};
  auto arity_errors = type_diagnostics::method_arity_after_receiver(empty_errors, mlc::String("new", 3), method_arguments.length(), method_span);
  if ((method_arguments.length() != 1))   {
    return infer_result::InferResult{std::make_shared<registry::Type>(registry::TUnknown{}), ast::diagnostics_append(object_parsed.errors, arity_errors)};
  } else   {
    auto argument_parsed = infer_expr_fn(method_arguments[0], inference_context);
    return infer_result::InferResult{std::make_shared<registry::Type>(registry::TShared{argument_parsed.inferred_type}), ast::diagnostics_append(ast::diagnostics_append(object_parsed.errors, arity_errors), argument_parsed.errors)};
  }
}
template<typename __F5>
infer_result::InferResult infer_shared_weak_method_call(infer_result::InferResult object_parsed, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext _inference_context, __F5 _infer_expr_fn) noexcept{
  auto empty_errors = mlc::Array<ast::Diagnostic>{};
  auto arity_errors = type_diagnostics::method_arity_after_receiver(empty_errors, method_name, method_arguments.length(), method_span);
  if ((method_arguments.length() != 0))   {
    return infer_result::InferResult{std::make_shared<registry::Type>(registry::TUnknown{}), ast::diagnostics_append(object_parsed.errors, arity_errors)};
  } else   {
    auto inner_type = semantic_type_structure::shared_pointer_inner_type(object_parsed.inferred_type);
    return infer_result::InferResult{std::make_shared<registry::Type>(registry::TGeneric{mlc::String("Weak", 4), mlc::Array<std::shared_ptr<registry::Type>>{inner_type}}), ast::diagnostics_append(object_parsed.errors, arity_errors)};
  }
}
template<typename __F5>
infer_result::InferResult infer_weak_upgrade_method_call(infer_result::InferResult object_parsed, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext _inference_context, __F5 _infer_expr_fn) noexcept{
  auto empty_errors = mlc::Array<ast::Diagnostic>{};
  auto arity_errors = type_diagnostics::method_arity_after_receiver(empty_errors, method_name, method_arguments.length(), method_span);
  if ((method_arguments.length() != 0))   {
    return infer_result::InferResult{std::make_shared<registry::Type>(registry::TUnknown{}), ast::diagnostics_append(object_parsed.errors, arity_errors)};
  } else   {
    auto inner_type = weak_pointer_inner_type(object_parsed.inferred_type);
    return infer_result::InferResult{std::make_shared<registry::Type>(registry::TGeneric{mlc::String("Option", 6), mlc::Array<std::shared_ptr<registry::Type>>{std::make_shared<registry::Type>(registry::TShared{inner_type})}}), ast::diagnostics_append(object_parsed.errors, arity_errors)};
  }
}

} // namespace infer_weak_method

#endif // INFER_WEAK_METHOD_HPP
