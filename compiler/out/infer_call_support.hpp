#ifndef INFER_CALL_SUPPORT_HPP
#define INFER_CALL_SUPPORT_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "type_diagnostics.hpp"
#include "semantic_type_structure.hpp"

namespace infer_call_support {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

infer_result::InferResult infer_expr_call_for_constructor_name(mlc::String constructor_name, infer_result::InferResult with_arguments, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_call_non_constructor_arity_only(infer_result::InferResult base, infer_result::InferResult function_result, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, std::shared_ptr<registry::Type> return_type) noexcept;

infer_result::InferResult infer_expr_call_non_constructor_not_callable(infer_result::InferResult base, infer_result::InferResult function_result, ast::Span call_source_span) noexcept;

infer_result::InferResult infer_expr_call_non_constructor(infer_result::InferResult function_result, infer_result::InferResult with_arguments, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span) noexcept;

} // namespace infer_call_support

#endif // INFER_CALL_SUPPORT_HPP
