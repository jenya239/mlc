#ifndef NAMES_HPP
#define NAMES_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"

namespace names {

struct Expr;
struct Stmt;

struct NameCheckResult {mlc::Array<mlc::String> errors;mlc::Array<mlc::String> scope;};

mlc::Array<mlc::String> collect_globals(ast::Program prog) noexcept;

mlc::Array<mlc::String> check_names_expr(std::shared_ptr<ast::Expr> expr, mlc::Array<mlc::String> locals, mlc::Array<mlc::String> globals) noexcept;

names::NameCheckResult check_names_stmts(mlc::Array<std::shared_ptr<ast::Stmt>> stmts, mlc::Array<mlc::String> locals, mlc::Array<mlc::String> globals) noexcept;

} // namespace names

#endif // NAMES_HPP
