#ifndef NAMES_HPP
#define NAMES_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"

namespace names {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

struct NameCheckResult {mlc::Array<ast::Diagnostic> diagnostics;mlc::Array<mlc::String> scope;};

mlc::HashMap<mlc::String, bool> collect_globals(ast::Program program) noexcept;

mlc::Array<ast::Diagnostic> check_names_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

names::NameCheckResult check_names_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

} // namespace names

#endif // NAMES_HPP
