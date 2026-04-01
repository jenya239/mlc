#include "infer_call_support.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "type_diagnostics.hpp"
#include "type_utils.hpp"

namespace infer_call_support {

using namespace ast;
using namespace registry;
using namespace check_context;
using namespace infer_result;
using namespace type_diagnostics;
using namespace type_utils;
using namespace ast_tokens;

infer_result::InferResult infer_expr_call_for_constructor_name(mlc::String constructor_name, infer_result::InferResult with_arguments, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_call_non_constructor_arity_only(infer_result::InferResult base, infer_result::InferResult function_result, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, std::shared_ptr<registry::Type> return_type) noexcept;

infer_result::InferResult infer_expr_call_non_constructor_not_callable(infer_result::InferResult base, infer_result::InferResult function_result, ast::Span call_source_span) noexcept;

infer_result::InferResult infer_expr_call_non_constructor(infer_result::InferResult function_result, infer_result::InferResult with_arguments, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span) noexcept;

infer_result::InferResult infer_expr_call_for_constructor_name(mlc::String constructor_name, infer_result::InferResult with_arguments, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, check_context::CheckContext inference_context) noexcept{
mlc::Array<std::shared_ptr<registry::Type>> constructor_parameter_types = registry::TypeRegistry_ctor_params_for(inference_context.registry, constructor_name);
mlc::Array<ast::Diagnostic> call_errors = type_diagnostics::constructor_arity_diagnostics(constructor_parameter_types.size(), call_arguments.size(), call_source_span);
return infer_result::InferResult{registry::TypeRegistry_ctor_type(inference_context.registry, constructor_name), ast::diagnostics_append(with_arguments.errors, call_errors)};
}

infer_result::InferResult infer_expr_call_non_constructor_arity_only(infer_result::InferResult base, infer_result::InferResult function_result, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, std::shared_ptr<registry::Type> return_type) noexcept{
mlc::Array<std::shared_ptr<registry::Type>> parameter_types = type_utils::function_parameter_list(function_result.inferred_type);
mlc::Array<ast::Diagnostic> arity_errors = type_diagnostics::call_arity_diagnostics(parameter_types.size(), call_arguments.size(), call_source_span);
return infer_result::InferResult{return_type, ast::diagnostics_append(base.errors, arity_errors)};
}

infer_result::InferResult infer_expr_call_non_constructor_not_callable(infer_result::InferResult base, infer_result::InferResult function_result, ast::Span call_source_span) noexcept{return type_utils::type_is_unknown(function_result.inferred_type) ? base : infer_result::InferResult{std::make_shared<registry::Type>((registry::TUnknown{})), ast::diagnostics_append(base.errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("called value is not a function"), call_source_span)})};}

infer_result::InferResult infer_expr_call_non_constructor(infer_result::InferResult function_result, infer_result::InferResult with_arguments, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span) noexcept{
std::shared_ptr<registry::Type> return_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TFn>((*function_result.inferred_type))) { auto _v_tfn = std::get<registry::TFn>((*function_result.inferred_type)); auto [_w0, return_ty] = _v_tfn; return return_ty; } return std::make_shared<registry::Type>((registry::TUnknown{})); }();
infer_result::InferResult base = infer_result::InferResult{return_type, with_arguments.errors};
return type_utils::type_is_function(function_result.inferred_type) ? infer_expr_call_non_constructor_arity_only(base, function_result, call_arguments, call_source_span, return_type) : infer_expr_call_non_constructor_not_callable(base, function_result, call_source_span);
}

} // namespace infer_call_support
