#ifndef REGION_ESCAPE_HPP
#define REGION_ESCAPE_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "names.hpp"
#include "diagnostic_codes.hpp"
namespace region_escape {

struct RegionEscapeWalkState {
  mlc::Array<ast::Diagnostic> diagnostics;
  mlc::Array<mlc::String> region_values;
  mlc::Array<mlc::String> region_locals;
};
mlc::Array<mlc::String> empty_names() noexcept;
mlc::Array<ast::Diagnostic> empty_diagnostics() noexcept;
bool names_contains(mlc::Array<mlc::String> names, mlc::String sought) noexcept;
mlc::Array<mlc::String> names_add(mlc::Array<mlc::String> names, mlc::String name) noexcept;
mlc::Array<mlc::String> names_add_all(mlc::Array<mlc::String> names, mlc::Array<mlc::String> additions) noexcept;
ast::Diagnostic region_escape_return_diagnostic(ast::Span source_span) noexcept;
ast::Diagnostic region_escape_closure_diagnostic(ast::Span source_span) noexcept;
ast::Diagnostic region_escape_assign_diagnostic(ast::Span source_span) noexcept;
ast::Diagnostic region_escape_field_diagnostic(mlc::String type_name, mlc::String field_name, ast::Span source_span) noexcept;
bool type_expression_contains_region(std::shared_ptr<ast::TypeExpr> type_expression) noexcept;
bool expression_is_region_alloc(std::shared_ptr<ast::Expr> expression, mlc::String region_binder) noexcept;
bool expression_mentions_region_value(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> region_values, mlc::String region_binder) noexcept;
bool expression_list_mentions_region_value(mlc::Array<std::shared_ptr<ast::Expr>> expressions, mlc::Array<mlc::String> region_values, mlc::String region_binder) noexcept;
bool lambda_captures_region_value(mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body, mlc::Array<mlc::String> region_values) noexcept;
bool expression_uses_ident(std::shared_ptr<ast::Expr> expression, mlc::String sought) noexcept;
bool expression_list_uses_ident(mlc::Array<std::shared_ptr<ast::Expr>> expressions, mlc::String sought) noexcept;
bool statements_use_ident(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::String sought) noexcept;
mlc::Array<ast::Diagnostic> walk_region_expression(std::shared_ptr<ast::Expr> expression, mlc::String region_binder, mlc::Array<mlc::String> region_values, mlc::Array<mlc::String> region_locals, mlc::Array<mlc::String> outer_bound) noexcept;
mlc::Array<ast::Diagnostic> walk_region_expression_list(mlc::Array<std::shared_ptr<ast::Expr>> expressions, mlc::String region_binder, mlc::Array<mlc::String> region_values, mlc::Array<mlc::String> region_locals, mlc::Array<mlc::String> outer_bound) noexcept;
mlc::Array<ast::Diagnostic> walk_region_record_parts(mlc::Array<ast::RecordLitPart> parts, mlc::String region_binder, mlc::Array<mlc::String> region_values, mlc::Array<mlc::String> region_locals, mlc::Array<mlc::String> outer_bound) noexcept;
mlc::Array<ast::Diagnostic> walk_region_match_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, mlc::String region_binder, mlc::Array<mlc::String> region_values, mlc::Array<mlc::String> region_locals, mlc::Array<mlc::String> outer_bound) noexcept;
RegionEscapeWalkState walk_region_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::String region_binder, mlc::Array<mlc::String> region_values, mlc::Array<mlc::String> region_locals, mlc::Array<mlc::String> outer_bound) noexcept;
RegionEscapeWalkState walk_region_statement(std::shared_ptr<ast::Stmt> statement, mlc::String region_binder, RegionEscapeWalkState state, mlc::Array<mlc::String> outer_bound) noexcept;
mlc::Array<ast::Diagnostic> walk_region_block(mlc::String region_binder, mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> outer_bound) noexcept;
mlc::Array<ast::Diagnostic> walk_expression_for_regions(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> outer_bound) noexcept;
mlc::Array<ast::Diagnostic> walk_expression_list_for_regions(mlc::Array<std::shared_ptr<ast::Expr>> expressions, mlc::Array<mlc::String> outer_bound) noexcept;
mlc::Array<ast::Diagnostic> walk_statements_for_regions(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> outer_bound) noexcept;
mlc::Array<ast::Diagnostic> field_def_region_diagnostics(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> fields) noexcept;
mlc::Array<ast::Diagnostic> variant_region_field_diagnostics(mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::Array<ast::Diagnostic> region_field_type_diagnostics(ast::Program program) noexcept;
mlc::Array<ast::Diagnostic> region_escape_diagnostics(mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::Expr> body) noexcept;

} // namespace region_escape

#endif // REGION_ESCAPE_HPP
