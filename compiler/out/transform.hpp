#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"
#include "pattern_env.hpp"
#include "semantic_ir.hpp"

namespace transform {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

struct TransformContext {mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env;registry::TypeRegistry registry;};

struct TransformStmtsResult {mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements;mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env;};

transform::TransformContext empty_transform_context() noexcept;

std::shared_ptr<semantic_ir::SExpr> transform_expr(std::shared_ptr<ast::Expr> expression, transform::TransformContext transform_context) noexcept;

transform::TransformStmtsResult transform_stmts(mlc::Array<std::shared_ptr<ast::Stmt>> statements, transform::TransformContext transform_context) noexcept;

} // namespace transform

#endif // TRANSFORM_HPP
