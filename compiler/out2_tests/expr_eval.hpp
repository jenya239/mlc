#ifndef EXPR_EVAL_HPP
#define EXPR_EVAL_HPP

#include "mlc.hpp"
#include <variant>

#include "semantic_ir.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "literals.hpp"
#include "identifiers.hpp"
#include "expression_support.hpp"
#include "match_gen.hpp"
#include "record_gen.hpp"
#include "method_gen.hpp"
#include "statement_context.hpp"
#include "expr.hpp"

namespace expr_eval {

mlc::String eval_argument_list(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> expressions, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;
mlc::String gen_binary_expr(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> left_expr, std::shared_ptr<semantic_ir::SExpr> right_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;
mlc::String gen_unary_expr(mlc::String operation, std::shared_ptr<semantic_ir::SExpr> inner_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;
mlc::String gen_call_function_code(std::shared_ptr<semantic_ir::SExpr> function_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;
mlc::String gen_call_expr(std::shared_ptr<semantic_ir::SExpr> function_expr, mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arguments, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;
mlc::String gen_field_expr(std::shared_ptr<semantic_ir::SExpr> object, mlc::String field_name, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;
mlc::String gen_index_expr(std::shared_ptr<semantic_ir::SExpr> object, std::shared_ptr<semantic_ir::SExpr> index_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;
mlc::String gen_if_expr(std::shared_ptr<semantic_ir::SExpr> condition, std::shared_ptr<semantic_ir::SExpr> then_expr, std::shared_ptr<semantic_ir::SExpr> else_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;
mlc::String gen_block_expr(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, std::shared_ptr<semantic_ir::SExpr> result_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;
mlc::String gen_while_expr(std::shared_ptr<semantic_ir::SExpr> condition, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;
mlc::String gen_for_expr(mlc::String variable, std::shared_ptr<semantic_ir::SExpr> iterator, mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;
mlc::String gen_array_expr(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> elements, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;
mlc::String gen_question_expr(std::shared_ptr<semantic_ir::SExpr> inner_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;
mlc::String gen_lambda_expr(mlc::Array<mlc::String> parameters, std::shared_ptr<semantic_ir::SExpr> body_expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;
mlc::String eval_expr(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

} // namespace expr_eval

#endif // EXPR_EVAL_HPP
