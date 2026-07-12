#define main mlc_user_main
#include "infer_region_method.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "diagnostic_codes.hpp"

namespace infer_region_method {

using namespace ast;
using namespace registry;
using namespace check_context;
using namespace infer_result;
using namespace diagnostic_codes;
using namespace ast_tokens;

std::shared_ptr<registry::Type> region_tag_type_from_span(ast::Span source_span) noexcept{
  return std::make_shared<registry::Type>(registry::TNamed{(((mlc::String("RegionTag_", 10) + mlc::to_string(source_span.line)) + mlc::String("_", 1)) + mlc::to_string(source_span.column))});
}
std::shared_ptr<registry::Type> region_handle_type(std::shared_ptr<registry::Type> tag_type) noexcept{
  return std::make_shared<registry::Type>(registry::TGeneric{mlc::String("RegionHandle", 12), mlc::Array<std::shared_ptr<registry::Type>>{tag_type}});
}
std::shared_ptr<registry::Type> region_pointer_type(std::shared_ptr<registry::Type> tag_type, std::shared_ptr<registry::Type> value_type) noexcept{
  return std::make_shared<registry::Type>(registry::TGeneric{mlc::String("Region", 6), mlc::Array<std::shared_ptr<registry::Type>>{tag_type, value_type}});
}
bool type_is_region_handle(std::shared_ptr<registry::Type> type_value) noexcept{
  return [&]() -> bool {
auto __match_subject = type_value;
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [name, type_arguments] = tGeneric; return ((name == mlc::String("RegionHandle", 12)) && (type_arguments.length() == 1));
}
return false;
std::abort();
}();
}
std::shared_ptr<registry::Type> region_handle_tag_type(std::shared_ptr<registry::Type> type_value) noexcept{
  return [&]() -> std::shared_ptr<registry::Type> {
auto __match_subject = type_value;
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [__0, type_arguments] = tGeneric; return ((type_arguments.length() == 1) ? (type_arguments[0]) : (std::make_shared<registry::Type>(registry::TUnknown{})));
}
return std::make_shared<registry::Type>(registry::TUnknown{});
std::abort();
}();
}
bool is_region_handle_method(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept{
  return type_is_region_handle(receiver_type);
}
bool is_region_handle_alloc_method(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept{
  return (type_is_region_handle(receiver_type) && (method_name == mlc::String("alloc", 5)));
}
infer_result::InferResult infer_region_handle_method_call(infer_result::InferResult object_parsed, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept{
  if ((method_name != mlc::String("alloc", 5)))   {
    return infer_result::InferResult{std::make_shared<registry::Type>(registry::TUnknown{}), ast::diagnostics_append(object_parsed.errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(((mlc::String("undefined method: ", 18) + method_name) + mlc::String(" on RegionHandle", 16)), method_span, diagnostic_codes::diagnostic_code_e082())})};
  } else   {
    return infer_region_alloc_call(object_parsed, method_arguments, method_span, inference_context, infer_expr_fn);
  }
}

} // namespace infer_region_method
