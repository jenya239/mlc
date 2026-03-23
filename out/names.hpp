#ifndef NAMES_HPP
#define NAMES_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"

namespace names {

struct NameCheckResult {mlc::Array<mlc::String> errors;mlc::Array<mlc::String> scope;};
bool scope_contains(mlc::Array<mlc::String> scope, mlc::String name) noexcept;
mlc::Array<mlc::String> pattern_bindings(std::shared_ptr<ast::Pat> pattern) noexcept;
mlc::Array<mlc::String> collect_pattern_bindings(std::shared_ptr<ast::Pat> pattern, mlc::Array<mlc::String> accumulator) noexcept;
mlc::HashMap<mlc::String, bool> collect_globals(ast::Program prog) noexcept;
mlc::Array<mlc::String> check_names_expr(std::shared_ptr<ast::Expr> expr, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;
mlc::Array<mlc::String> NameCheckResult_append_expr_errors(NameCheckResult self, mlc::Array<mlc::String> expr_errors) noexcept;
NameCheckResult check_names_stmts(mlc::Array<std::shared_ptr<ast::Stmt>> stmts, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

} // namespace names

#endif // NAMES_HPP
