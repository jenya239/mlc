#ifndef INFER_CALL_SUPPORT_HPP
#define INFER_CALL_SUPPORT_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "registry.hpp"
#include "infer_result.hpp"
#include "type_diagnostics.hpp"
#include "semantic_type_structure.hpp"
#include "diagnostic_codes.hpp"
namespace infer_call_support {

mlc::String owner_name_from_constructor_type(std::shared_ptr<registry::Type> constructor_type) noexcept;
int callee_min_arity(registry::TypeRegistry registry, mlc::String callee_name, int max_arity) noexcept;
std::shared_ptr<registry::Type> return_type_from_inferred_function_type(std::shared_ptr<registry::Type> inferred_type) noexcept;
infer_result::InferResult infer_expr_call_for_constructor_name(mlc::String constructor_name, infer_result::InferResult with_arguments, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_call_non_constructor_arity_only(infer_result::InferResult base, infer_result::InferResult function_parsed, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, std::shared_ptr<registry::Type> return_type, registry::TypeRegistry registry, mlc::String callee_name) noexcept;
infer_result::InferResult infer_expr_call_non_constructor_not_callable(infer_result::InferResult base, infer_result::InferResult function_parsed, ast::Span call_source_span) noexcept;
infer_result::InferResult infer_expr_call_non_constructor(infer_result::InferResult function_parsed, infer_result::InferResult with_arguments, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, registry::TypeRegistry registry, mlc::String callee_name) noexcept;

} // namespace infer_call_support

#endif // INFER_CALL_SUPPORT_HPP
