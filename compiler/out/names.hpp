#ifndef NAMES_HPP
#define NAMES_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "expr_visitor.hpp"
#include "diagnostic_codes.hpp"
#include "transform.hpp"
#include "transform_stmts.hpp"

namespace names {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct NameCheckResult {mlc::Array<ast::Diagnostic> diagnostics;mlc::Array<mlc::String> scope;};

mlc::Array<mlc::String> pattern_bindings(std::shared_ptr<ast::Pattern> pattern) noexcept;

names::NameCheckResult check_names_semantic_statements(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_semantic_expression(std::shared_ptr<semantic_ir::SemanticExpression> expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> check_names_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

names::NameCheckResult check_names_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<ast::Diagnostic> NameCheckResult_append_expression_diagnostics(names::NameCheckResult self, mlc::Array<ast::Diagnostic> expression_diagnostics) noexcept;

names::NameCheckResult NameCheckResult_append_diagnostics(names::NameCheckResult self, mlc::Array<ast::Diagnostic> extra_diagnostics) noexcept;

names::NameCheckResult NameCheckResult_with_scope(names::NameCheckResult self, mlc::Array<mlc::String> new_scope) noexcept;

names::NameCheckResult NameCheckResult_merge(names::NameCheckResult self, names::NameCheckResult other) noexcept;

} // namespace names

#endif // NAMES_HPP
