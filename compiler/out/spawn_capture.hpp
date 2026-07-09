#ifndef SPAWN_CAPTURE_HPP
#define SPAWN_CAPTURE_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "names.hpp"
#include "diagnostic_codes.hpp"
#include "registry.hpp"
#include "send_safe.hpp"
namespace spawn_capture {

struct SpawnCaptureStmtWalkState {
  mlc::Array<ast::Diagnostic> diagnostics;
  mlc::Array<mlc::String> bound;
};
struct SpawnCaptureMutableWalkState {
  mlc::Array<ast::Diagnostic> diagnostics;
  mlc::Array<mlc::String> scope;
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment;
};
struct SpawnCaptureContext {
  registry::TypeRegistry registry;
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment;
};
bool type_is_sync_safe_shared(std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept;
mlc::Array<mlc::String> empty_names() noexcept;
bool names_contains(mlc::Array<mlc::String> names, mlc::String sought) noexcept;
mlc::Array<mlc::String> names_add(mlc::Array<mlc::String> names, mlc::String name) noexcept;
mlc::Array<mlc::String> names_add_all(mlc::Array<mlc::String> names, mlc::Array<mlc::String> additions) noexcept;
mlc::Array<ast::Diagnostic> empty_diagnostics() noexcept;
ast::Diagnostic mutable_capture_diagnostic(mlc::String binding_name, ast::Span source_span) noexcept;
bool type_annotation_is_elided(std::shared_ptr<ast::TypeExpr> type_expression) noexcept;
std::shared_ptr<registry::Type> type_hint_from_value(std::shared_ptr<ast::Expr> value, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, registry::TypeRegistry registry) noexcept;
std::shared_ptr<registry::Type> resolve_binding_type(std::shared_ptr<ast::TypeExpr> type_expression, std::shared_ptr<ast::Expr> value, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, registry::TypeRegistry registry) noexcept;
mlc::Array<mlc::String> mutable_scope_from_parameters(mlc::Array<std::shared_ptr<ast::Param>> parameters, registry::TypeRegistry registry) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment_from_parameters(mlc::Array<std::shared_ptr<ast::Param>> parameters, registry::TypeRegistry registry) noexcept;
mlc::Array<ast::Diagnostic> free_mutable_capture_in_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> local_bound, mlc::Array<mlc::String> outer_mutable) noexcept;
mlc::Array<ast::Diagnostic> free_mutable_capture_in_expr_list(mlc::Array<std::shared_ptr<ast::Expr>> expressions, mlc::Array<mlc::String> local_bound, mlc::Array<mlc::String> outer_mutable) noexcept;
mlc::Array<ast::Diagnostic> free_mutable_capture_in_field_values(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> local_bound, mlc::Array<mlc::String> outer_mutable) noexcept;
mlc::Array<ast::Diagnostic> free_mutable_capture_in_record_parts(mlc::Array<ast::RecordLitPart> parts, mlc::Array<mlc::String> local_bound, mlc::Array<mlc::String> outer_mutable) noexcept;
mlc::Array<ast::Diagnostic> free_mutable_capture_in_match_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, mlc::Array<mlc::String> local_bound, mlc::Array<mlc::String> outer_mutable) noexcept;
mlc::Array<mlc::String> statement_binding_names(std::shared_ptr<ast::Stmt> statement) noexcept;
mlc::Array<mlc::String> bindings_after_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> local_bound) noexcept;
mlc::Array<ast::Diagnostic> free_mutable_capture_in_stmts(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> local_bound, mlc::Array<mlc::String> outer_mutable) noexcept;
mlc::Array<ast::Diagnostic> walk_spawn_sites_in_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> mutable_scope, SpawnCaptureContext capture_context) noexcept;
mlc::Array<ast::Diagnostic> walk_spawn_sites_in_expr_list(mlc::Array<std::shared_ptr<ast::Expr>> expressions, mlc::Array<mlc::String> mutable_scope, SpawnCaptureContext capture_context) noexcept;
mlc::Array<ast::Diagnostic> walk_spawn_sites_in_field_values(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> mutable_scope, SpawnCaptureContext capture_context) noexcept;
mlc::Array<ast::Diagnostic> walk_spawn_sites_in_record_parts(mlc::Array<ast::RecordLitPart> parts, mlc::Array<mlc::String> mutable_scope, SpawnCaptureContext capture_context) noexcept;
mlc::Array<ast::Diagnostic> walk_spawn_sites_in_match_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, mlc::Array<mlc::String> mutable_scope, SpawnCaptureContext capture_context) noexcept;
SpawnCaptureMutableWalkState apply_statement_to_mutable_scope(std::shared_ptr<ast::Stmt> statement, mlc::Array<mlc::String> scope, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, registry::TypeRegistry registry) noexcept;
SpawnCaptureMutableWalkState walk_spawn_sites_in_stmts_state(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> mutable_scope, SpawnCaptureContext capture_context) noexcept;
mlc::Array<ast::Diagnostic> walk_spawn_sites_in_stmts(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> mutable_scope, SpawnCaptureContext capture_context) noexcept;
mlc::Array<ast::Diagnostic> spawn_mutable_capture_diagnostics(mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::Expr> body, registry::TypeRegistry registry) noexcept;

} // namespace spawn_capture

#endif // SPAWN_CAPTURE_HPP
