#ifndef EVAL_HPP
#define EVAL_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "context.hpp"
#include "type_gen.hpp"

namespace eval {

struct Expr;
struct Stmt;

mlc::String gen_argument_list(mlc::Array<std::shared_ptr<ast::Expr>> expressions, context::CodegenContext context) noexcept;

bool subject_needs_deref(std::shared_ptr<ast::Expr> subject, context::CodegenContext context) noexcept;

bool first_arm_needs_deref(std::shared_ptr<ast::MatchArm> first_arm, std::shared_ptr<ast::Expr> subject, context::CodegenContext context) noexcept;

mlc::String gen_expr(std::shared_ptr<ast::Expr> expr, context::CodegenContext context) noexcept;

context::GenStmtResult gen_stmt_with_try(std::shared_ptr<ast::Stmt> stmt, context::CodegenContext context, int try_counter) noexcept;

context::GenStmtsResult gen_stmts_str_with_try(mlc::Array<std::shared_ptr<ast::Stmt>> statements, context::CodegenContext context, int try_counter) noexcept;

mlc::String gen_stmts_str(mlc::Array<std::shared_ptr<ast::Stmt>> statements, context::CodegenContext context) noexcept;

mlc::String gen_return_body(std::shared_ptr<ast::Expr> expr, context::CodegenContext context) noexcept;

mlc::String gen_fn_body(std::shared_ptr<ast::Expr> body_expr, context::CodegenContext context) noexcept;

} // namespace eval

#endif // EVAL_HPP
