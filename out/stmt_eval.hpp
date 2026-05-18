#ifndef STMT_EVAL_HPP
#define STMT_EVAL_HPP

#include "mlc.hpp"
#include <variant>

#include "semantic_ir.hpp"
#include "let_pat.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "expression_support.hpp"
#include "type_gen.hpp"
#include "literals.hpp"
#include "statement_context.hpp"
#include "expr.hpp"

namespace stmt_eval {

struct StmtsFoldState {context::GenStmtsResult partial_result;context::CodegenContext codegen_context;};
mlc::String constexpr_binding_value_code(std::shared_ptr<semantic_ir::SExpr> value, std::shared_ptr<registry::Type> binding_semantic_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept;
mlc::String eval_try_unwrap(std::shared_ptr<semantic_ir::SExpr> inner_expr, context::CodegenContext context, mlc::String try_identifier, mlc::String success_line, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept;
mlc::String eval_stmt_if_else_suffix(std::shared_ptr<semantic_ir::SExpr> else_expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept;
mlc::String eval_stmt_if_nested(std::shared_ptr<semantic_ir::SExpr> if_expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept;
mlc::String eval_block_body(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept;
mlc::String eval_stmt_expr(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept;
mlc::String eval_elements_code(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept;
context::GenStmtResult eval_stmt_with_try(std::shared_ptr<semantic_ir::SStmt> stmt, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept;
StmtsFoldState stmts_fold_step(StmtsFoldState fold_state, std::shared_ptr<semantic_ir::SStmt> statement, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept;
context::GenStmtsResult eval_stmts_str_with_try(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, int try_counter, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept;
mlc::String eval_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> gen_expr_fn) noexcept;

} // namespace stmt_eval

#endif // STMT_EVAL_HPP
