#ifndef INFER_LAMBDA_CONTEXT_HPP
#define INFER_LAMBDA_CONTEXT_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"

namespace infer_lambda_context {

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
infer_result::InferResult infer_lambda_with_param_types(mlc::Array<mlc::String> parameter_names, mlc::Array<std::shared_ptr<registry::Type>> parameter_types, std::shared_ptr<ast::Expr> body, check_context::CheckContext inference_context, InferExprFn infer_expr_fn) noexcept;

} // namespace infer_lambda_context

#endif // INFER_LAMBDA_CONTEXT_HPP
