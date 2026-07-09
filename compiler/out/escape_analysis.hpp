#ifndef ESCAPE_ANALYSIS_HPP
#define ESCAPE_ANALYSIS_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
namespace escape_analysis {

mlc::Array<mlc::String> empty_string_list() noexcept;
bool string_list_contains(mlc::Array<mlc::String> haystack, mlc::String needle) noexcept;
mlc::Array<mlc::String> string_list_add(mlc::Array<mlc::String> names, mlc::String name) noexcept;
mlc::Array<mlc::String> string_list_difference(mlc::Array<mlc::String> left, mlc::Array<mlc::String> right) noexcept;
mlc::Array<mlc::String> mark_all_watched(mlc::Array<mlc::String> watched, mlc::Array<mlc::String> escaping) noexcept;
bool type_expr_is_function(std::shared_ptr<ast::TypeExpr> type_expression) noexcept;
mlc::Array<mlc::String> function_typed_param_names(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;
mlc::Array<mlc::String> visit_field_values(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> watched, mlc::Array<mlc::String> escaping) noexcept;
mlc::Array<mlc::String> visit_record_parts(mlc::Array<ast::RecordLitPart> parts, mlc::Array<mlc::String> watched, mlc::Array<mlc::String> escaping) noexcept;
mlc::Array<mlc::String> visit_match_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, mlc::Array<mlc::String> watched, mlc::Array<mlc::String> escaping) noexcept;
mlc::Array<mlc::String> visit_expr_list(mlc::Array<std::shared_ptr<ast::Expr>> expressions, mlc::Array<mlc::String> watched, mlc::Array<mlc::String> escaping) noexcept;
mlc::Array<mlc::String> visit_stmt_list(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> watched, mlc::Array<mlc::String> escaping) noexcept;
mlc::Array<mlc::String> visit_call_like(std::shared_ptr<ast::Expr> callee, mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::Array<mlc::String> watched, mlc::Array<mlc::String> escaping) noexcept;
mlc::Array<mlc::String> visit_stmt(std::shared_ptr<ast::Stmt> statement, mlc::Array<mlc::String> watched, mlc::Array<mlc::String> escaping) noexcept;
mlc::Array<mlc::String> visit_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> watched, mlc::Array<mlc::String> escaping) noexcept;
mlc::Array<mlc::String> non_escaping_params(std::shared_ptr<ast::Expr> body, mlc::Array<mlc::String> function_typed_names) noexcept;
mlc::Array<mlc::String> non_escaping_params_for_decl(std::shared_ptr<ast::Decl> declaration) noexcept;
bool declaration_is_function_named(std::shared_ptr<ast::Decl> declaration, mlc::String function_name) noexcept;
mlc::Array<mlc::String> non_escaping_params_for_named_fn(ast::Program program, mlc::String function_name) noexcept;
mlc::Array<mlc::String> collect_idents_used_as_values_in_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> names) noexcept;
mlc::Array<mlc::String> collect_idents_used_as_values_in_stmts(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> names) noexcept;
mlc::Array<mlc::String> collect_idents_used_as_values_in_decl(std::shared_ptr<ast::Decl> declaration, mlc::Array<mlc::String> names) noexcept;
mlc::Array<mlc::String> function_names_used_as_values(ast::Program program) noexcept;
mlc::Array<mlc::String> non_escaping_params_for_decl_if_template_safe(std::shared_ptr<ast::Decl> declaration, mlc::Array<mlc::String> functions_used_as_values) noexcept;

} // namespace escape_analysis

#endif // ESCAPE_ANALYSIS_HPP
