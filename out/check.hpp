#ifndef CHECK_HPP
#define CHECK_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "ast.hpp"
#include "names.hpp"
#include "check_mutations.hpp"
#include "registry.hpp"
#include "trait_param_expand.hpp"
#include "param_destructure_expand.hpp"
#include "derive_validation.hpp"
#include "infer.hpp"
#include "check_context.hpp"
#include "semantic_type_structure.hpp"
#include "record_field_default_initializer.hpp"
#include "partial_application_desugar.hpp"

namespace check {

struct CheckOut {mlc::Array<mlc::String> errors;registry::TypeRegistry registry;};
bool param_defaults_in_tail(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;
bool default_expr_mvp_ok(std::shared_ptr<ast::Expr> e) noexcept;
mlc::Array<ast::Diagnostic> accumulate_diagnostics_when_record_fields_have_defaults_under_generic_type_parameters(mlc::Array<ast::Diagnostic> diagnostics_accumulator_so_far, std::shared_ptr<ast::TypeVariant> variant_under_generic_scan) noexcept;
mlc::Array<ast::Diagnostic> accumulate_diagnostics_after_literal_record_default_checks(mlc::Array<ast::Diagnostic> diagnostics_accumulator_so_far, std::shared_ptr<ast::TypeVariant> variant_under_literal_scan, check_context::CheckContext record_default_inference_context, registry::TypeRegistry registry) noexcept;
mlc::Array<ast::Diagnostic> record_field_default_value_diagnostics(mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, registry::TypeRegistry registry) noexcept;
bool is_extern_body(std::shared_ptr<ast::Expr> e) noexcept;
mlc::Array<ast::Diagnostic> param_default_diagnostics(mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::Expr> body, registry::TypeRegistry registry) noexcept;
bool type_parameter_name_known(mlc::Array<mlc::String> type_parameter_names, mlc::String candidate_name) noexcept;
mlc::Array<ast::Diagnostic> where_clause_unknown_parameter_diagnostics(mlc::Array<mlc::String> type_parameter_names, mlc::Array<ast::WhereClauseBound> where_entries, ast::Span source_span) noexcept;
mlc::HashMap<mlc::String, bool> global_names_merge_type_constructor_variant_into_map(mlc::HashMap<mlc::String, bool> global_names_so_far, std::shared_ptr<ast::TypeVariant> type_constructor_variant_under_scan) noexcept;
mlc::HashMap<mlc::String, bool> global_names_merge_trait_method_into_map(mlc::HashMap<mlc::String, bool> global_names_so_far, std::shared_ptr<ast::Decl> method_declaration_under_scan) noexcept;
mlc::HashMap<mlc::String, bool> global_names_merge_single_declaration_into_map(mlc::HashMap<mlc::String, bool> global_names_so_far, std::shared_ptr<ast::Decl> declaration_under_scan) noexcept;
mlc::HashMap<mlc::String, bool> collect_globals(ast::Program program) noexcept;
bool type_is_checkable(std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept;
bool CheckOut_has_errors(CheckOut self) noexcept;
mlc::Array<ast::Diagnostic> accumulate_diagnostics_for_single_extend_method(mlc::String extend_type_name, std::shared_ptr<ast::Decl> method_declaration_shared_under_scan, registry::TypeRegistry registry) noexcept;
ast::Result<CheckOut, mlc::Array<mlc::String>> check_program_against_full(ast::Program entry, ast::Program full_program) noexcept;
ast::Result<CheckOut, mlc::Array<mlc::String>> check_with_context(ast::Program entry, ast::Program full) noexcept;
ast::Result<CheckOut, mlc::Array<mlc::String>> check(ast::Program program) noexcept;

} // namespace check

#endif // CHECK_HPP
