#ifndef INFER_HPP
#define INFER_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "type_utils.hpp"
#include "type_diagnostics.hpp"
#include "pattern_env.hpp"

namespace infer {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

struct InferResult {std::shared_ptr<registry::Type> inferred_type;mlc::Array<ast::Diagnostic> errors;};

struct StmtInferResult {mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env;mlc::Array<ast::Diagnostic> errors;};

infer::InferResult infer_expr(std::shared_ptr<ast::Expr> expression, check_context::CheckContext inference_context) noexcept;

infer::StmtInferResult infer_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept;

} // namespace infer

#endif // INFER_HPP
