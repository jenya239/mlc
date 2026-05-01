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
#include "infer_lambda_context.hpp"
#include "method_receiver_diagnostics.hpp"

namespace infer_result_option_method {

using InferExprFn = std::variant<>;
void __skip__() noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
void __skip__() noexcept;
mlc::Array<ast::Diagnostic> append_lambda_arity(mlc::Array<ast::Diagnostic> errors, std::shared_ptr<ast::Expr> expression, int expected, mlc::String method_name) noexcept;
bool is_result_type(std::shared_ptr<registry::Type> t) noexcept;
std::shared_ptr<registry::Type> result_inner_err_from_ret(std::shared_ptr<registry::Type> lam_ret) noexcept;
bool should_infer_as_result_option_hof(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept;
infer_result::InferResult infer_result_option_hof_method_call(infer_result::InferResult object_result, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, InferExprFn infer_expr_fn) noexcept;

} // namespace infer_result_option_method

#endif // INFER_RESULT_OPTION_METHOD_HPP
