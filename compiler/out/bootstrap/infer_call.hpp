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

struct ResolvedArgs {mlc::Array<std::shared_ptr<ast::Expr>> exprs;mlc::Array<ast::Diagnostic> errors;};
struct Call_arguments_inference_accumulator {mlc::Array<ast::Diagnostic> accumulated_errors;mlc::Array<std::shared_ptr<registry::Type>> inferred_argument_types_list;};
bool expression_is_named_argument(std::shared_ptr<ast::Expr> argument_expression) noexcept;
bool has_named_args(mlc::Array<std::shared_ptr<ast::Expr>> args) noexcept;
int find_param_slot(mlc::Array<mlc::String> parameter_names, mlc::String sought_parameter_name) noexcept;
std::shared_ptr<ast::Expr> inner_expression_after_stripping_optional_named_label(std::shared_ptr<ast::Expr> argument_expression) noexcept;
ResolvedArgs strip_named_labels(mlc::Array<std::shared_ptr<ast::Expr>> args) noexcept;
Call_arguments_inference_accumulator accumulate_inference_for_one_call_argument(Call_arguments_inference_accumulator accumulator, std::shared_ptr<ast::Expr> argument_expression, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;
ResolvedArgs reorder_named_to_positional(mlc::Array<std::shared_ptr<ast::Expr>> args, mlc::Array<mlc::String> param_names) noexcept;
ResolvedArgs resolve_named_args(mlc::Array<std::shared_ptr<ast::Expr>> args, mlc::String callee_name, registry::TypeRegistry registry) noexcept;
mlc::Array<std::shared_ptr<registry::Type>> take_first_param_types(mlc::Array<std::shared_ptr<registry::Type>> param_types, int positional_argument_count) noexcept;
infer_result::InferResult infer_expr_call(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

} // namespace infer_call

#endif // INFER_CALL_HPP
