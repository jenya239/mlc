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

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

transform::TransformStmtsResult transform_stmts(mlc::Array<std::shared_ptr<ast::Stmt>> statements, transform::TransformContext transform_context) noexcept;

} // namespace transform_stmts

#endif // TRANSFORM_STMTS_HPP
