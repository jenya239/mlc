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

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

infer_result::InferResult infer_expr_call(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

} // namespace infer_call

#endif // INFER_CALL_HPP
