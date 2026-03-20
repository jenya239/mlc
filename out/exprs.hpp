#ifndef EXPRS_HPP
#define EXPRS_HPP

#include "mlc.hpp"
#include <variant>

#include "preds.hpp"
#include "types.hpp"

namespace exprs {

preds::StmtsResult parse_stmts_until_end(preds::Parser parser) noexcept;
preds::StmtsResult parse_stmts_until_else_end(preds::Parser parser) noexcept;
preds::StmtResult parse_stmt(preds::Parser parser) noexcept;
preds::PatResult parse_pat(preds::Parser parser) noexcept;
preds::PatsResult parse_pat_args(preds::Parser parser) noexcept;
preds::PatsResult parse_record_pat_fields(preds::Parser parser) noexcept;
preds::ExprResult parse_expr(preds::Parser parser) noexcept;
std::shared_ptr<ast::Expr> pipe_desugar(std::shared_ptr<ast::Expr> left_expr, std::shared_ptr<ast::Expr> right_expr) noexcept;
preds::ExprResult parse_pipe(preds::Parser parser) noexcept;
preds::ExprResult parse_or(preds::Parser parser) noexcept;
preds::ExprResult parse_and(preds::Parser parser) noexcept;
bool is_cmp_op(mlc::String op) noexcept;
preds::ExprResult parse_cmp(preds::Parser parser) noexcept;
preds::ExprResult parse_add(preds::Parser parser) noexcept;
preds::ExprResult parse_mul(preds::Parser parser) noexcept;
preds::ExprResult parse_unary(preds::Parser parser) noexcept;
preds::ExprResult parse_postfix(preds::Parser parser) noexcept;
preds::ExprsResult parse_arg_list(preds::Parser parser) noexcept;
std::shared_ptr<ast::Expr> fstr_to_string_expr(mlc::String part) noexcept;
std::shared_ptr<ast::Expr> build_fstr_part_expr(mlc::String part, bool is_literal) noexcept;
std::shared_ptr<ast::Expr> build_fstr_expr(mlc::Array<mlc::String> parts) noexcept;
preds::ExprResult parse_primary(preds::Parser parser) noexcept;
bool looks_like_lambda_params(preds::Parser parser) noexcept;
preds::NamesResult parse_lambda_params(preds::Parser parser) noexcept;
preds::ExprResult parse_array_lit(preds::Parser parser) noexcept;
preds::ExprResult parse_if_expr(preds::Parser parser) noexcept;
preds::ExprResult parse_block(preds::Parser parser) noexcept;
bool is_unit_expr(std::shared_ptr<ast::Expr> expr) noexcept;
std::shared_ptr<ast::Expr> block_result(mlc::Array<std::shared_ptr<ast::Stmt>> stmts) noexcept;
mlc::Array<std::shared_ptr<ast::Stmt>> block_body(mlc::Array<std::shared_ptr<ast::Stmt>> stmts) noexcept;
void StmtsResult_to_block_expr(preds::StmtsResult self) noexcept;
preds::ExprResult parse_while_expr(preds::Parser parser) noexcept;
preds::ExprResult parse_for_expr(preds::Parser parser) noexcept;
preds::ExprResult parse_match_expr(preds::Parser parser) noexcept;
preds::ArmsResult parse_arms_brace(preds::Parser parser) noexcept;
preds::ArmsResult parse_arms_pipe(preds::Parser parser) noexcept;
preds::ArmsResult parse_arms(preds::Parser parser) noexcept;
preds::FieldValsResult parse_record_fields(preds::Parser parser) noexcept;
preds::ExprResult parse_record_expr(preds::Parser parser, mlc::String record_name) noexcept;

} // namespace exprs

#endif // EXPRS_HPP
