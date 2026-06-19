#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"
#include "hof_method_spec.hpp"
#include "pattern_env.hpp"
#include "semantic_type_structure.hpp"
#include "substitution.hpp"
#include "infer.hpp"
#include "check_context.hpp"
#include "record_lit_merge.hpp"
#include "partial_application_desugar.hpp"
#include "named_args.hpp"
#include "semantic_ir.hpp"

namespace transform {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct TransformContext {mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env;registry::TypeRegistry registry;};

struct TransformStmtsResult {mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements;mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env;};

transform::TransformContext empty_transform_context() noexcept;

transform::TransformContext transform_context_with_env(transform::TransformContext base, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> coerce_expr_to_type(std::shared_ptr<semantic_ir::SemanticExpression> expression, std::shared_ptr<registry::Type> target_type) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> transform_expr(std::shared_ptr<ast::Expr> expression, transform::TransformContext transform_context, std::function<transform::TransformStmtsResult(mlc::Array<std::shared_ptr<ast::Stmt>>, transform::TransformContext)> stmts_fn) noexcept;

} // namespace transform

#endif // TRANSFORM_HPP
