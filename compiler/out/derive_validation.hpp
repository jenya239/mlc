#ifndef DERIVE_VALIDATION_HPP
#define DERIVE_VALIDATION_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "diagnostic_codes.hpp"
namespace derive_validation {

struct Derive_hash_tuple_field_fold_state {
  mlc::Array<ast::Diagnostic> diagnostics;
  int field_index;
};
struct Derive_clause_trait_scan_fold_state {
  mlc::Array<ast::Diagnostic> diagnostics;
  mlc::HashMap<mlc::String, bool> seen_traits;
};
bool derive_trait_name_allowed(mlc::String trait_name) noexcept;
bool string_list_contains_derive_trait(mlc::Array<mlc::String> haystack, mlc::String needle) noexcept;
bool type_expr_allowed_for_derive_hash_field(std::shared_ptr<ast::TypeExpr> type_expression) noexcept;
mlc::Array<ast::Diagnostic> derive_hash_field_error(mlc::String variant_label, mlc::String field_label, std::shared_ptr<ast::TypeExpr> type_expression, ast::Span diagnostic_span) noexcept;
Derive_hash_tuple_field_fold_state derive_hash_tuple_field_fold_step(Derive_hash_tuple_field_fold_state state, std::shared_ptr<ast::TypeExpr> field_type, mlc::String variant_name, ast::Span diagnostic_span) noexcept;
mlc::Array<ast::Diagnostic> derive_hash_field_errors_for_variant(std::shared_ptr<ast::TypeVariant> variant, ast::Span diagnostic_span) noexcept;
mlc::Array<ast::Diagnostic> derive_hash_errors_for_variants(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, ast::Span diagnostic_span) noexcept;
Derive_clause_trait_scan_fold_state derive_clause_trait_scan_fold_step(Derive_clause_trait_scan_fold_state state, mlc::String trait_name, ast::Span diagnostic_span) noexcept;
mlc::Array<ast::Diagnostic> derive_clause_diagnostics_after_trait_scan(mlc::Array<mlc::String> type_parameter_names, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_trait_names, mlc::Array<ast::Diagnostic> trait_scan_diagnostics, ast::Span diagnostic_span) noexcept;
mlc::Array<ast::Diagnostic> derive_clause_diagnostics(mlc::Array<mlc::String> type_parameter_names, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_trait_names, ast::Span diagnostic_span) noexcept;

} // namespace derive_validation

#endif // DERIVE_VALIDATION_HPP
