#ifndef INFER_CHANNEL_METHOD_HPP
#define INFER_CHANNEL_METHOD_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "semantic_type_structure.hpp"
#include "channel_method_types.hpp"
#include "send_safe.hpp"
#include "type_diagnostics.hpp"
#include "diagnostic_codes.hpp"
namespace infer_channel_method {

mlc::Array<ast::Diagnostic> channel_method_receiver_error(std::shared_ptr<registry::Type> channel_type, mlc::String method_name, ast::Span method_span) noexcept;
mlc::Array<ast::Diagnostic> channel_send_argument_errors(std::shared_ptr<registry::Type> element_type, std::shared_ptr<registry::Type> argument_type, ast::Span argument_span, registry::TypeRegistry registry) noexcept;
template<typename __F5>
infer_result::InferResult infer_channel_method_call(infer_result::InferResult object_parsed, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, __F5 infer_expr_fn) noexcept;
template<typename __F5>
infer_result::InferResult infer_channel_method_call(infer_result::InferResult object_parsed, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, __F5 infer_expr_fn) noexcept{
  auto channel_type = object_parsed.inferred_type;
  auto receiver_errors = channel_method_receiver_error(channel_type, method_name, method_span);
  auto arity_errors = type_diagnostics::method_arity_after_receiver(receiver_errors, method_name, method_arguments.length(), method_span);
  auto merged = object_parsed;
  auto errors = arity_errors;
  auto element_type = semantic_type_structure::channel_element_type_from_channel_type(channel_type);
  if (((method_name == mlc::String("send", 4)) && (method_arguments.length() == 1)))   {
    auto argument_parsed = infer_expr_fn(method_arguments[0], inference_context);
    auto argument_type = argument_parsed.inferred_type;
    (merged = infer_result::InferResult_absorb(merged, argument_parsed));
    (errors = ast::diagnostics_append(errors, channel_send_argument_errors(element_type, argument_type, ast::expr_span(method_arguments[0]), inference_context.registry)));
  }
  return infer_result::InferResult{channel_method_types::channel_method_result_type(channel_type, method_name), ast::diagnostics_append(merged.errors, errors)};
}

} // namespace infer_channel_method

#endif // INFER_CHANNEL_METHOD_HPP
