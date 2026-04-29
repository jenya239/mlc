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

struct TransformContext {mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env;registry::TypeRegistry registry;};
struct TransformStmtsResult {mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements;mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env;};
TransformContext transform_context_new(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env, registry::TypeRegistry registry) noexcept;
TransformContext empty_transform_context() noexcept;
TransformContext transform_context_with_env(TransformContext base, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env) noexcept;
std::shared_ptr<semantic_ir::SExpr> coerce_expr_to_type(std::shared_ptr<semantic_ir::SExpr> expression, std::shared_ptr<registry::Type> target_type) noexcept;
mlc::Array<std::shared_ptr<semantic_ir::SExpr>> transform_exprs(mlc::Array<std::shared_ptr<ast::Expr>> expressions, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> transform_field_vals(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> build_transform_substitution(std::shared_ptr<registry::Type> subject_type, registry::TypeRegistry registry) noexcept;
mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> transform_match_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, TransformContext transform_context, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SExpr> transform_expr_lambda_with_param_types(mlc::Array<mlc::String> parameter_names, mlc::Array<std::shared_ptr<registry::Type>> parameter_types, std::shared_ptr<ast::Expr> body, ast::Span source_span, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;
std::shared_ptr<semantic_ir::SExpr> transform_expr(std::shared_ptr<ast::Expr> expression, TransformContext transform_context, std::function<TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, TransformContext)> stmts_fn) noexcept;

} // namespace transform

#endif // TRANSFORM_HPP
