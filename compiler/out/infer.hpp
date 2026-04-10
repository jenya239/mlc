#ifndef INFER_HPP
#define INFER_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "infer_literals.hpp"
#include "infer_for_support.hpp"
#include "infer_question_expression.hpp"
#include "infer_operand_combine.hpp"
#include "infer_call_support.hpp"
#include "type_diagnostics.hpp"
#include "call_argument_unify.hpp"
#include "semantic_type_structure.hpp"
#include "infer_expr_ident.hpp"
#include "pattern_env.hpp"

namespace infer {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

infer_result::InferResult infer_expr(std::shared_ptr<ast::Expr> expression, check_context::CheckContext inference_context) noexcept;

infer_result::StmtInferResult infer_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept;

} // namespace infer

#endif // INFER_HPP
