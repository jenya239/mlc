#ifndef MOVE_CHECK_HPP
#define MOVE_CHECK_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "names.hpp"
#include "diagnostic_codes.hpp"
namespace move_check {

struct MoveCheckWalkState {
  mlc::Array<ast::Diagnostic> diagnostics;
  mlc::Array<mlc::String> moved;
};
mlc::Array<mlc::String> empty_names() noexcept;
bool names_contains(mlc::Array<mlc::String> names, mlc::String sought) noexcept;
mlc::Array<mlc::String> names_add(mlc::Array<mlc::String> names, mlc::String name) noexcept;
mlc::Array<mlc::String> names_add_all(mlc::Array<mlc::String> names, mlc::Array<mlc::String> additions) noexcept;
mlc::Array<mlc::String> names_remove(mlc::Array<mlc::String> names, mlc::String name) noexcept;
mlc::Array<mlc::String> names_remove_all(mlc::Array<mlc::String> names, mlc::Array<mlc::String> removals) noexcept;
mlc::Array<ast::Diagnostic> empty_diagnostics() noexcept;
ast::Diagnostic use_after_move_diagnostic(mlc::String binding_name, ast::Span source_span) noexcept;
mlc::Array<mlc::String> statement_binding_names(std::shared_ptr<ast::Stmt> statement) noexcept;
MoveCheckWalkState move_check_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> moved) noexcept;
MoveCheckWalkState move_check_expr_list(mlc::Array<std::shared_ptr<ast::Expr>> expressions, MoveCheckWalkState state) noexcept;
MoveCheckWalkState move_check_field_values(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, MoveCheckWalkState state) noexcept;
MoveCheckWalkState move_check_record_parts(mlc::Array<ast::RecordLitPart> parts, mlc::Array<mlc::String> moved) noexcept;
MoveCheckWalkState move_check_match_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, MoveCheckWalkState state) noexcept;
MoveCheckWalkState move_check_stmts(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> moved) noexcept;
mlc::Array<ast::Diagnostic> move_use_after_diagnostics(mlc::Array<std::shared_ptr<ast::Param>> _parameters, std::shared_ptr<ast::Expr> body) noexcept;

} // namespace move_check

#endif // MOVE_CHECK_HPP
