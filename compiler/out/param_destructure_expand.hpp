#ifndef PARAM_DESTRUCTURE_EXPAND_HPP
#define PARAM_DESTRUCTURE_EXPAND_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "diagnostic_codes.hpp"
namespace param_destructure_expand {

struct Expand_fn_parameter_destructure_fold_state {
  mlc::Array<std::shared_ptr<ast::Stmt>> prelude_statements;
  mlc::Array<std::shared_ptr<ast::Param>> new_parameters;
  int parameter_position_index;
};
mlc::String synthetic_destructure_parameter_name(int parameter_position_index) noexcept;
bool parameter_binding_is_plain_identifier(std::shared_ptr<ast::Param> parameter) noexcept;
bool parameter_pattern_is_plain(std::shared_ptr<ast::Param> parameter) noexcept;
std::shared_ptr<ast::Param> strip_parameter_to_synthetic_plain(std::shared_ptr<ast::Param> parameter, mlc::String synthetic_name) noexcept;
std::shared_ptr<ast::Stmt> prelude_statement_destructure(std::shared_ptr<ast::Param> parameter, mlc::String synthetic_name, ast::Span binding_span) noexcept;
std::shared_ptr<ast::Expr> wrap_expression_with_statement_prelude(mlc::Array<std::shared_ptr<ast::Stmt>> prelude_statements, std::shared_ptr<ast::Expr> body_expression) noexcept;
Expand_fn_parameter_destructure_fold_state expand_decl_fn_parameter_destructure_fold_step(Expand_fn_parameter_destructure_fold_state state, std::shared_ptr<ast::Param> current_parameter) noexcept;
std::shared_ptr<ast::Decl> expand_decl_fn_parameter_destructuring_inner(mlc::String function_name, mlc::Array<mlc::String> type_parameter_names, mlc::Array<mlc::Array<mlc::String>> trait_bounds_rows, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::TypeExpr> return_type_expression, std::shared_ptr<ast::Expr> body_expression, mlc::Array<ast::WhereClauseBound> where_clause_bounds_entries) noexcept;
std::shared_ptr<ast::Decl> expand_parameter_destructuring_decl_shared(std::shared_ptr<ast::Decl> declaration) noexcept;
mlc::Array<std::shared_ptr<ast::Decl>> expand_extend_methods_parameter_destructuring(mlc::Array<std::shared_ptr<ast::Decl>> methods) noexcept;
ast::Program expand_parameter_destructuring_in_program(ast::Program program) noexcept;
mlc::Array<ast::Diagnostic> concatenate_diagnostic_arrays(mlc::Array<ast::Diagnostic> existing, mlc::Array<ast::Diagnostic> extra) noexcept;
mlc::Array<ast::Diagnostic> extern_parameter_destructure_errors_for_decl_shared(std::shared_ptr<ast::Decl> declaration) noexcept;
mlc::Array<ast::Diagnostic> extern_parameter_destructure_diagnostics(ast::Program program) noexcept;

} // namespace param_destructure_expand

#endif // PARAM_DESTRUCTURE_EXPAND_HPP
