#ifndef CALL_ARGUMENT_UNIFY_HPP
#define CALL_ARGUMENT_UNIFY_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"
#include "diagnostic_codes.hpp"
namespace call_argument_unify {

mlc::Array<ast::Diagnostic> empty_diagnostic_list() noexcept;
ast::Diagnostic argument_type_mismatch_diagnostic(std::shared_ptr<registry::Type> expected_type, std::shared_ptr<registry::Type> actual_type, ast::Span diagnostic_span) noexcept;
bool expected_type_is_extern_fn(std::shared_ptr<registry::Type> expected_type) noexcept;
ast::Diagnostic extern_fn_argument_must_be_top_level_diagnostic(ast::Span diagnostic_span) noexcept;
mlc::Array<ast::Diagnostic> extern_fn_argument_shape_diagnostics(std::shared_ptr<registry::Type> expected_type, std::shared_ptr<ast::Expr> argument_expression) noexcept;
void record_type_parameter_source_for_argument(mlc::HashMap<mlc::String, int>& type_parameter_source_argument_index, mlc::Array<mlc::String> type_parameter_names, mlc::String type_name, int argument_index) noexcept;
bool list_contains_string(mlc::Array<mlc::String> haystack, mlc::String needle) noexcept;
mlc::Array<ast::Diagnostic> unify_expected_and_actual_argument_types(std::shared_ptr<registry::Type> expected_type, std::shared_ptr<registry::Type> actual_type, mlc::Array<mlc::String> type_parameter_names, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>& substitution, ast::Span diagnostic_span, mlc::HashMap<mlc::String, int>& type_parameter_source_argument_index, int argument_index) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> build_call_type_substitution(mlc::Array<std::shared_ptr<registry::Type>> expected_parameter_types, mlc::Array<std::shared_ptr<registry::Type>> actual_argument_types, mlc::Array<mlc::String> type_parameter_names, mlc::HashMap<mlc::String, int>& type_parameter_source_argument_index, mlc::Array<std::shared_ptr<ast::Expr>> argument_expressions, ast::Span fallback_span) noexcept;
mlc::Array<ast::Diagnostic> call_argument_unification_diagnostics(mlc::Array<std::shared_ptr<registry::Type>> expected_parameter_types, mlc::Array<std::shared_ptr<registry::Type>> actual_argument_types, mlc::Array<mlc::String> type_parameter_names, mlc::Array<std::shared_ptr<ast::Expr>> argument_expressions) noexcept;
ast::Span type_parameter_source_argument_span(mlc::String type_parameter_name, mlc::HashMap<mlc::String, int> type_parameter_source_argument_index, mlc::Array<std::shared_ptr<ast::Expr>> argument_expressions, ast::Span fallback_span) noexcept;

} // namespace call_argument_unify

#endif // CALL_ARGUMENT_UNIFY_HPP
