#include "infer_channel_method.hpp"

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

using namespace ast;
using namespace registry;
using namespace check_context;
using namespace infer_result;
using namespace semantic_type_structure;
using namespace channel_method_types;
using namespace send_safe;
using namespace type_diagnostics;
using namespace diagnostic_codes;
using namespace ast_tokens;

mlc::Array<ast::Diagnostic> channel_method_receiver_error(std::shared_ptr<registry::Type> channel_type, mlc::String method_name, ast::Span method_span) noexcept;

mlc::Array<ast::Diagnostic> channel_send_argument_errors(std::shared_ptr<registry::Type> element_type, std::shared_ptr<registry::Type> argument_type, ast::Span argument_span, registry::TypeRegistry registry) noexcept;

infer_result::InferResult infer_channel_method_call(infer_result::InferResult object_parsed, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

mlc::Array<ast::Diagnostic> channel_method_receiver_error(std::shared_ptr<registry::Type> channel_type, mlc::String method_name, ast::Span method_span) noexcept{return !semantic_type_structure::type_is_unknown(channel_type) && !semantic_type_structure::type_is_channel(channel_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("method ") + method_name + mlc::String(" expects a Channel receiver, got ") + semantic_type_structure::type_description(channel_type), method_span, diagnostic_codes::diagnostic_code_e082())} : mlc::Array<ast::Diagnostic>{};}

mlc::Array<ast::Diagnostic> channel_send_argument_errors(std::shared_ptr<registry::Type> element_type, std::shared_ptr<registry::Type> argument_type, ast::Span argument_span, registry::TypeRegistry registry) noexcept{
mlc::Array<ast::Diagnostic> errors = {};
if (!semantic_type_structure::type_is_unknown(argument_type) && !send_safe::type_is_send_safe(argument_type, registry)){
{
errors = ast::diagnostics_append(errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("channel send requires a Send-safe value, got ") + semantic_type_structure::type_description(argument_type), argument_span, diagnostic_codes::diagnostic_code_e082())});
}
}
if (!semantic_type_structure::type_is_unknown(element_type) && !semantic_type_structure::type_is_unknown(argument_type) && !semantic_type_structure::types_structurally_equal(element_type, argument_type)){
{
errors = ast::diagnostics_append(errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("channel send expected ") + semantic_type_structure::type_description(element_type) + mlc::String(", got ") + semantic_type_structure::type_description(argument_type), argument_span, diagnostic_codes::diagnostic_code_e082())});
}
}
return errors;
}

infer_result::InferResult infer_channel_method_call(infer_result::InferResult object_parsed, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept{
std::shared_ptr<registry::Type> channel_type = object_parsed.inferred_type;
mlc::Array<ast::Diagnostic> receiver_errors = channel_method_receiver_error(channel_type, method_name, method_span);
mlc::Array<ast::Diagnostic> arity_errors = type_diagnostics::method_arity_after_receiver(receiver_errors, method_name, method_arguments.size(), method_span);
infer_result::InferResult merged = std::move(object_parsed);
mlc::Array<ast::Diagnostic> errors = arity_errors;
std::shared_ptr<registry::Type> element_type = semantic_type_structure::channel_element_type_from_channel_type(channel_type);
if (method_name == mlc::String("send") && method_arguments.size() == 1){
{
infer_result::InferResult argument_parsed = infer_expr_fn(method_arguments[0], inference_context);
std::shared_ptr<registry::Type> argument_type = argument_parsed.inferred_type;
merged = infer_result::InferResult_absorb(merged, argument_parsed);
errors = ast::diagnostics_append(errors, channel_send_argument_errors(element_type, argument_type, ast::expr_span(method_arguments[0]), inference_context.registry));
}
}
return infer_result::InferResult{channel_method_types::channel_method_result_type(channel_type, method_name), ast::diagnostics_append(merged.errors, errors)};
}

} // namespace infer_channel_method
