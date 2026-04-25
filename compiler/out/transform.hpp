#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"
#include "array_method_types.hpp"
#include "result_option_method_types.hpp"
#include "pattern_env.hpp"
#include "semantic_type_structure.hpp"
#include "semantic_ir.hpp"

namespace transform {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

struct TransformContext {mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env;registry::TypeRegistry registry;};

struct TransformStmtsResult {mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements;mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env;};

transform::TransformContext empty_transform_context() noexcept;

transform::TransformContext transform_context_with_env(transform::TransformContext base, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env) noexcept;

std::shared_ptr<semantic_ir::SExpr> coerce_expr_to_type(std::shared_ptr<semantic_ir::SExpr> expression, std::shared_ptr<registry::Type> target_type) noexcept;

std::shared_ptr<semantic_ir::SExpr> transform_expr(std::shared_ptr<ast::Expr> expression, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

} // namespace transform

#endif // TRANSFORM_HPP
