#ifndef INFER_TRAIT_BOUNDS_HPP
#define INFER_TRAIT_BOUNDS_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "call_argument_unify.hpp"

namespace infer_trait_bounds {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

mlc::Array<ast::Diagnostic> trait_bound_diagnostics(mlc::String callee_name, mlc::Array<std::shared_ptr<registry::Type>> callee_parameter_types, mlc::Array<std::shared_ptr<registry::Type>> argument_inferred_types, mlc::Array<mlc::String> callee_type_parameter_names, ast::Span call_source_span, check_context::CheckContext inference_context) noexcept;

} // namespace infer_trait_bounds

#endif // INFER_TRAIT_BOUNDS_HPP
