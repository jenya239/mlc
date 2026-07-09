#ifndef TRANSFORM_STMTS_HPP
#define TRANSFORM_STMTS_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "registry.hpp"
#include "semantic_ir.hpp"
#include "transform.hpp"
#include "let_pattern_infer.hpp"
namespace transform_stmts {

struct Transform_stmts_fold_state {
  mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> typed_statements;
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment;
};
Transform_stmts_fold_state transform_stmts_fold_step(Transform_stmts_fold_state accumulator, std::shared_ptr<ast::Stmt> statement, transform::TransformContext transform_context) noexcept;
transform::TransformStmtsResult transform_stmts(mlc::Array<std::shared_ptr<ast::Stmt>> statements, transform::TransformContext transform_context) noexcept;

} // namespace transform_stmts

#endif // TRANSFORM_STMTS_HPP
