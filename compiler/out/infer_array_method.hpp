#ifndef INFER_ARRAY_METHOD_HPP
#define INFER_ARRAY_METHOD_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "semantic_type_structure.hpp"
#include "array_method_types.hpp"
#include "hof_method_spec.hpp"
#include "infer_lambda_context.hpp"
#include "method_receiver_diagnostics.hpp"
#include "type_diagnostics.hpp"
#include "diagnostic_codes.hpp"
namespace infer_array_method {

mlc::Array<std::shared_ptr<registry::Type>> append_argument_type(mlc::Array<std::shared_ptr<registry::Type>> argument_types_so_far, std::shared_ptr<registry::Type> type_to_append) noexcept;
infer_result::InferResult infer_expr_maybe_lambda(std::shared_ptr<ast::Expr> expression, mlc::Array<std::shared_ptr<registry::Type>> expected_param_types, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;
mlc::Array<ast::Diagnostic> append_lambda_arity_diag(mlc::Array<ast::Diagnostic> errors, std::shared_ptr<ast::Expr> expression, int expected_params, mlc::String method_name) noexcept;
mlc::Array<ast::Diagnostic> append_predicate_bool_diag(mlc::Array<ast::Diagnostic> errors, std::shared_ptr<ast::Expr> lambda_expression, std::shared_ptr<registry::Type> function_type, mlc::String method_name) noexcept;
bool method_expects_bool_predicate(mlc::String method_name) noexcept;
infer_result::InferResult infer_array_hof_method_call(infer_result::InferResult object_parsed, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;
bool should_infer_as_array_hof(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept;

} // namespace infer_array_method

#endif // INFER_ARRAY_METHOD_HPP
