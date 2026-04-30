#ifndef NAMES_HPP
#define NAMES_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"

namespace names {

struct NameCheckResult {mlc::Array<ast::Diagnostic> diagnostics;mlc::Array<mlc::String> scope;};
bool scope_contains(mlc::Array<mlc::String> scope, mlc::String name) noexcept;
mlc::Array<mlc::String> pattern_bindings(std::shared_ptr<ast::Pat> pattern) noexcept;
mlc::Array<mlc::String> collect_pattern_bindings(std::shared_ptr<ast::Pat> pattern, mlc::Array<mlc::String> accumulator) noexcept;
mlc::Array<ast::Diagnostic> check_names_identifier(mlc::String name, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals, ast::Span source_span) noexcept;
mlc::Array<ast::Diagnostic> check_names_binary_expression(std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;
mlc::Array<ast::Diagnostic> check_names_unary_expression(std::shared_ptr<ast::Expr> inner, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;
mlc::Array<ast::Diagnostic> check_names_call_expression(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;
mlc::Array<ast::Diagnostic> check_names_method_expression(std::shared_ptr<ast::Expr> object, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;
mlc::Array<ast::Diagnostic> check_names_field_expression(std::shared_ptr<ast::Expr> object, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;
mlc::Array<ast::Diagnostic> check_names_index_expression(std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;
mlc::Array<ast::Diagnostic> check_names_conditional_expression(std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expression, std::shared_ptr<ast::Expr> else_expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;
mlc::Array<ast::Diagnostic> check_names_block_expression(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;
mlc::Array<ast::Diagnostic> check_names_while_expression(std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;
mlc::Array<ast::Diagnostic> check_names_for_expression(mlc::String variable_name, std::shared_ptr<ast::Expr> iterator, mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;
mlc::Array<ast::Diagnostic> check_names_with_expression(std::shared_ptr<ast::Expr> resource, mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> stmts, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;
mlc::Array<ast::Diagnostic> check_names_match_expression(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> arms, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;
mlc::Array<ast::Diagnostic> check_names_record_expression(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;
mlc::Array<ast::Diagnostic> check_names_record_update_expression(std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;
mlc::Array<ast::Diagnostic> check_names_array_expression(mlc::Array<std::shared_ptr<ast::Expr>> elements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;
mlc::Array<ast::Diagnostic> check_names_question_expression(std::shared_ptr<ast::Expr> inner, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;
mlc::Array<ast::Diagnostic> check_names_lambda_expression(mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;
mlc::Array<ast::Diagnostic> check_names_empty() noexcept;
mlc::Array<ast::Diagnostic> check_names_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;
mlc::Array<ast::Diagnostic> NameCheckResult_append_expression_diagnostics(NameCheckResult self, mlc::Array<ast::Diagnostic> expression_diagnostics) noexcept;
NameCheckResult check_names_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

} // namespace names

#endif // NAMES_HPP
