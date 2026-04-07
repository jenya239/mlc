#ifndef EVAL_HPP
#define EVAL_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "type_gen.hpp"
#include "literals.hpp"
#include "identifiers.hpp"
#include "match_analysis.hpp"
#include "expression_support.hpp"
#include "statement_context.hpp"
#include "expr.hpp"

namespace eval {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

mlc::String gen_argument_list(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> expressions, context::CodegenContext context) noexcept;

mlc::String gen_expr(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context) noexcept;

context::GenStmtResult gen_stmt_with_try(std::shared_ptr<semantic_ir::SStmt> stmt, context::CodegenContext context, int try_counter) noexcept;

context::GenStmtsResult gen_stmts_str_with_try(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context, int try_counter) noexcept;

mlc::String gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context) noexcept;

mlc::String gen_return_body(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context) noexcept;

mlc::String gen_fn_body(std::shared_ptr<semantic_ir::SExpr> body_expr, context::CodegenContext context) noexcept;

} // namespace eval

#endif // EVAL_HPP
