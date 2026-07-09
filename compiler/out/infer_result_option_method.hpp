#ifndef INFER_RESULT_OPTION_METHOD_HPP
#define INFER_RESULT_OPTION_METHOD_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "semantic_type_structure.hpp"
#include "result_option_method_types.hpp"
#include "hof_method_spec.hpp"
#include "infer_lambda_context.hpp"
#include "method_receiver_diagnostics.hpp"
#include "diagnostic_codes.hpp"
namespace infer_result_option_method {

mlc::Array<ast::Diagnostic> append_lambda_arity(mlc::Array<ast::Diagnostic> errors, std::shared_ptr<ast::Expr> expression, int expected, mlc::String method_name) noexcept;
mlc::Array<std::shared_ptr<registry::Type>> append_argument_type(mlc::Array<std::shared_ptr<registry::Type>> argument_types_so_far, std::shared_ptr<registry::Type> type_to_append) noexcept;
bool is_result_type(std::shared_ptr<registry::Type> type_value) noexcept;
std::shared_ptr<registry::Type> result_inner_error_from_return_type(std::shared_ptr<registry::Type> lambda_return_type) noexcept;
bool should_infer_as_result_option_hof(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept;
infer_result::InferResult infer_result_option_hof_method_call(infer_result::InferResult object_parsed, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

} // namespace infer_result_option_method

#endif // INFER_RESULT_OPTION_METHOD_HPP
