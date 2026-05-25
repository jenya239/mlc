#ifndef EXPRS_HPP
#define EXPRS_HPP

#include "mlc.hpp"
#include <variant>

#include "predicates.hpp"
#include "types.hpp"
#include "ast.hpp"
#include "ast_tokens.hpp"
#include "lexer.hpp"

namespace exprs {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

predicates::StmtsResult parse_stmts_until_end(predicates::Parser parser) noexcept;

predicates::StmtsResult parse_stmts_until_else_end(predicates::Parser parser) noexcept;

predicates::PatsResult parse_record_pat_fields(predicates::Parser parser) noexcept;

predicates::ExprResult parse_expr(predicates::Parser parser) noexcept;

std::shared_ptr<ast::Expr> pipe_desugar(std::shared_ptr<ast::Expr> left_expr, std::shared_ptr<ast::Expr> right_expr) noexcept;

predicates::ExprResult parse_if_expr(predicates::Parser parser) noexcept;

} // namespace exprs

#endif // EXPRS_HPP
