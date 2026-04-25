#ifndef EXPRS_HPP
#define EXPRS_HPP

#include "mlc.hpp"
#include <variant>

#include "preds.hpp"
#include "types.hpp"
#include "ast.hpp"
#include "ast_tokens.hpp"

namespace exprs {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

preds::StmtsResult parse_stmts_until_end(preds::Parser parser) noexcept;

preds::StmtsResult parse_stmts_until_else_end(preds::Parser parser) noexcept;

preds::ExprResult parse_expr(preds::Parser parser) noexcept;

std::shared_ptr<ast::Expr> pipe_desugar(std::shared_ptr<ast::Expr> left_expr, std::shared_ptr<ast::Expr> right_expr) noexcept;

preds::ExprResult parse_if_expr(preds::Parser parser) noexcept;

} // namespace exprs

#endif // EXPRS_HPP
