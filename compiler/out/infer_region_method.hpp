#ifndef INFER_REGION_METHOD_HPP
#define INFER_REGION_METHOD_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "diagnostic_codes.hpp"
namespace infer_region_method {

std::shared_ptr<registry::Type> region_tag_type_from_span(ast::Span source_span) noexcept;
std::shared_ptr<registry::Type> region_handle_type(std::shared_ptr<registry::Type> tag_type) noexcept;
std::shared_ptr<registry::Type> region_pointer_type(std::shared_ptr<registry::Type> tag_type, std::shared_ptr<registry::Type> value_type) noexcept;
bool type_is_region_handle(std::shared_ptr<registry::Type> type_value) noexcept;
std::shared_ptr<registry::Type> region_handle_tag_type(std::shared_ptr<registry::Type> type_value) noexcept;
bool is_region_handle_method(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept;
bool is_region_handle_alloc_method(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept;
infer_result::InferResult infer_region_handle_method_call(infer_result::InferResult object_parsed, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;
template<typename __F4>
infer_result::InferResult infer_region_alloc_call(infer_result::InferResult object_parsed, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, __F4 infer_expr_fn) noexcept;
template<typename __F4>
infer_result::InferResult infer_region_alloc_call(infer_result::InferResult object_parsed, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, __F4 infer_expr_fn) noexcept{
  if ((method_arguments.length() != 1))   {
    return infer_result::InferResult{std::make_shared<registry::Type>(registry::TUnknown{}), ast::diagnostics_append(object_parsed.errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code((mlc::String("expected 1 arguments, got ", 26) + mlc::to_string(method_arguments.length())), method_span, diagnostic_codes::diagnostic_code_e014())})};
  } else   {
    auto argument_parsed = infer_expr_fn(method_arguments[0], inference_context);
    auto tag_type = region_handle_tag_type(object_parsed.inferred_type);
    return infer_result::InferResult{region_pointer_type(tag_type, argument_parsed.inferred_type), ast::diagnostics_append(object_parsed.errors, argument_parsed.errors)};
  }
}

} // namespace infer_region_method

#endif // INFER_REGION_METHOD_HPP
