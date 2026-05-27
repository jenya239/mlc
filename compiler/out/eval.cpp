#include "eval.hpp"

#include "semantic_ir.hpp"
#include "cpp_ast.hpp"
#include "context.hpp"
#include "expr_eval.hpp"
#include "stmt_eval.hpp"
#include "expr_visitor_cpp.hpp"
#include "stmt_cpp.hpp"

namespace eval {

using namespace semantic_ir;
using namespace cpp_ast;
using namespace context;
using namespace expr_eval;
using namespace stmt_eval;
using namespace expr_visitor_cpp;
using namespace stmt_cpp;

mlc::String gen_expr_via_string(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context) noexcept;

mlc::String gen_stmts_via_string(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context) noexcept;

mlc::String gen_expr(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context) noexcept;

mlc::String gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context) noexcept;

std::shared_ptr<cpp_ast::CppExpr> gen_expr_cpp(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context) noexcept;

mlc::String gen_argument_list(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> expressions, context::CodegenContext context) noexcept;

mlc::String gen_try_unwrap(std::shared_ptr<semantic_ir::SExpr> inner_expr, context::CodegenContext context, mlc::String try_identifier, mlc::String success_line) noexcept;

context::GenStmtResult gen_stmt_with_try(std::shared_ptr<semantic_ir::SStmt> statement, context::CodegenContext context, int try_counter) noexcept;

context::GenStmtsWithContext gen_stmts_str_with_try(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, int try_counter) noexcept;

mlc::String gen_expr_via_string(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context) noexcept{return expr_eval::eval_expr(expression, context, [](mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext codegen_context) mutable { return gen_stmts_via_string(statements, codegen_context); });}

mlc::String gen_stmts_via_string(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context) noexcept{return stmt_eval::eval_stmts_str(statements, context, [](std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext codegen_context) mutable { return gen_expr_via_string(expression, codegen_context); });}

mlc::String gen_expr(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context) noexcept{return expr_visitor_cpp::eval_expr_cpp_as_string(expression, context, gen_stmts_str);}

mlc::String gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context) noexcept{return stmt_cpp::gen_stmts_cpp_as_string(statements, context);}

std::shared_ptr<cpp_ast::CppExpr> gen_expr_cpp(std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext context) noexcept{return expr_visitor_cpp::eval_expr_cpp(expression, context, gen_stmts_str);}

mlc::String gen_argument_list(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> expressions, context::CodegenContext context) noexcept{return expressions.map([context](std::shared_ptr<semantic_ir::SExpr> expression) mutable { return gen_expr(expression, context); }).join(mlc::String(", "));}

mlc::String gen_try_unwrap(std::shared_ptr<semantic_ir::SExpr> inner_expr, context::CodegenContext context, mlc::String try_identifier, mlc::String success_line) noexcept{return stmt_eval::eval_try_unwrap(inner_expr, context, try_identifier, success_line, [](std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext codegen_context) mutable { return gen_expr(expression, codegen_context); });}

context::GenStmtResult gen_stmt_with_try(std::shared_ptr<semantic_ir::SStmt> statement, context::CodegenContext context, int try_counter) noexcept{return stmt_eval::eval_stmt_with_try(statement, context, try_counter, [](std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext codegen_context) mutable { return gen_expr(expression, codegen_context); });}

context::GenStmtsWithContext gen_stmts_str_with_try(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, int try_counter) noexcept{return stmt_eval::eval_stmts_str_with_try(statements, context, try_counter, [](std::shared_ptr<semantic_ir::SExpr> expression, context::CodegenContext codegen_context) mutable { return gen_expr(expression, codegen_context); });}

} // namespace eval
