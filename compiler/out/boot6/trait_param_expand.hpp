#ifndef TRAIT_PARAM_EXPAND_HPP
#define TRAIT_PARAM_EXPAND_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"

namespace trait_param_expand {

struct TraitExpandChunk {std::shared_ptr<ast::TypeExpr> type_expression;int next_counter;mlc::Array<mlc::String> appended_type_parameter_names;mlc::Array<mlc::Array<mlc::String>> appended_trait_bounds_rows;};
mlc::Array<ast::Diagnostic> trait_and_type_name_conflict_diagnostics(ast::Program program) noexcept;
void fill_trait_and_nominal_maps(ast::Program program, mlc::HashMap<mlc::String, bool> trait_names, mlc::HashMap<mlc::String, bool> nominal_type_names) noexcept;
mlc::Array<mlc::Array<mlc::String>> align_trait_bounds_matrix(mlc::Array<mlc::String> type_parameter_names, mlc::Array<mlc::Array<mlc::String>> trait_bounds_rows) noexcept;
mlc::Array<mlc::Array<mlc::String>> append_string_rows(mlc::Array<mlc::Array<mlc::String>> destination, mlc::Array<mlc::Array<mlc::String>> source) noexcept;
mlc::Array<mlc::String> append_strings(mlc::Array<mlc::String> destination, mlc::Array<mlc::String> source) noexcept;
TraitExpandChunk expand_type_expression_for_trait_param(std::shared_ptr<ast::TypeExpr> type_expression, mlc::HashMap<mlc::String, bool> explicit_type_parameter_environment, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names, int synthetic_counter) noexcept;
std::shared_ptr<ast::Decl> expand_decl_fn_trait_parameters(mlc::String function_name, mlc::Array<mlc::String> type_parameter_names, mlc::Array<mlc::Array<mlc::String>> trait_bounds_rows, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::TypeExpr> return_type_expression, std::shared_ptr<ast::Expr> body_expression, mlc::Array<ast::WhereClauseBound> where_clause_bounds_entries, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept;
mlc::Array<std::shared_ptr<ast::Decl>> expand_extend_methods(mlc::Array<std::shared_ptr<ast::Decl>> methods, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept;
std::shared_ptr<ast::Decl> expand_decl_shared(std::shared_ptr<ast::Decl> declaration, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept;
mlc::Array<std::shared_ptr<ast::Decl>> expand_declarations_with_trait_and_nominal_maps(mlc::Array<std::shared_ptr<ast::Decl>> declarations, mlc::HashMap<mlc::String, bool> trait_declaration_names, mlc::HashMap<mlc::String, bool> nominal_type_declaration_names) noexcept;
mlc::Array<std::shared_ptr<ast::Decl>> expand_declarations_with_trait_context(mlc::Array<std::shared_ptr<ast::Decl>> declarations, ast::Program program_for_maps) noexcept;
ast::Program expand_trait_as_param_entry_using_full(ast::Program entry, ast::Program full_program) noexcept;
ast::Program expand_trait_as_param_program(ast::Program program) noexcept;

} // namespace trait_param_expand

#endif // TRAIT_PARAM_EXPAND_HPP
