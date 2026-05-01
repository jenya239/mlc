#ifndef INFER_CALL_HPP
#define INFER_CALL_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "infer_call_support.hpp"
#include "call_argument_unify.hpp"
#include "infer_trait_bounds.hpp"
#include "semantic_type_structure.hpp"

namespace infer_call {

using InferExprFn = std::variant<>;
struct ResolvedArgs {mlc::Array<std::shared_ptr<ast::Expr>> exprs;mlc::Array<ast::Diagnostic> errors;};
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
bool has_named_args(mlc::Array<std::shared_ptr<ast::Expr>> args) noexcept;
int find_param_slot(mlc::Array<mlc::String> param_names, mlc::String name) noexcept;
ResolvedArgs strip_named_labels(mlc::Array<std::shared_ptr<ast::Expr>> args) noexcept;
ResolvedArgs reorder_named_to_positional(mlc::Array<std::shared_ptr<ast::Expr>> args, mlc::Array<mlc::String> param_names) noexcept;
ResolvedArgs resolve_named_args(mlc::Array<std::shared_ptr<ast::Expr>> args, mlc::String callee_name, registry::TypeRegistry registry) noexcept;
mlc::Array<std::shared_ptr<registry::Type>> take_first_param_types(mlc::Array<std::shared_ptr<registry::Type>> param_types, int n) noexcept;
infer_result::InferResult infer_expr_call(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, check_context::CheckContext inference_context, InferExprFn infer_expr_fn) noexcept;

} // namespace infer_call

#endif // INFER_CALL_HPP
