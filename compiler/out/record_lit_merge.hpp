#ifndef RECORD_LIT_MERGE_HPP
#define RECORD_LIT_MERGE_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "registry.hpp"
#include "diagnostic_codes.hpp"
namespace record_lit_merge {

struct SpreadAccumulationState {
  mlc::Array<std::shared_ptr<ast::FieldVal>> spread_field_values;
  int spread_type_index;
};
bool flat_field_values_contains_name(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::String sought_name) noexcept;
std::shared_ptr<ast::FieldVal> flat_field_value_by_name(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::String sought_name) noexcept;
mlc::Array<std::shared_ptr<ast::FieldVal>> field_values_upsert_name(mlc::Array<std::shared_ptr<ast::FieldVal>> fields, std::shared_ptr<ast::FieldVal> field_value) noexcept;
mlc::Array<std::shared_ptr<ast::FieldVal>> empty_field_value_list() noexcept;
mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_field_values_for_part(ast::RecordLitPart part_under_walk) noexcept;
mlc::Array<std::shared_ptr<ast::FieldVal>> collect_explicit_field_values_flat(mlc::Array<ast::RecordLitPart> parts) noexcept;
mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_fields_flat_last_wins(mlc::Array<ast::RecordLitPart> parts) noexcept;
mlc::Array<std::shared_ptr<ast::FieldVal>> merge_ordered_slot(mlc::Array<std::shared_ptr<ast::FieldVal>> merged_so_far, mlc::String slot_field_name, mlc::String nominal_record_name, mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_fields_flat, registry::TypeRegistry registry) noexcept;
mlc::Array<std::shared_ptr<ast::FieldVal>> merge_explicit_record_literal_with_type_defaults_nonempty(mlc::String nominal_record_name, mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_fields_flat, registry::TypeRegistry registry) noexcept;
mlc::Array<std::shared_ptr<ast::FieldVal>> merge_explicit_record_literal_with_type_defaults(mlc::String nominal_record_name, mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_fields_flat, registry::TypeRegistry registry) noexcept;
mlc::String record_type_base_name(std::shared_ptr<registry::Type> value_type) noexcept;
bool part_is_record_spread(ast::RecordLitPart part_under_walk) noexcept;
bool record_literal_contains_spread(mlc::Array<ast::RecordLitPart> parts) noexcept;
mlc::Array<std::shared_ptr<ast::FieldVal>> spread_candidate_field(mlc::String candidate_field_name, std::shared_ptr<ast::Expr> spread_expression, mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_fields, mlc::Array<std::shared_ptr<ast::FieldVal>> accumulated_spread_fields, mlc::Array<std::shared_ptr<ast::FieldVal>> expanded_fields, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> spread_field_types) noexcept;
mlc::Array<std::shared_ptr<ast::FieldVal>> spread_fields_for_part(std::shared_ptr<ast::Expr> spread_expression, std::shared_ptr<registry::Type> spread_value_type, mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_fields, mlc::Array<std::shared_ptr<ast::FieldVal>> accumulated_spread_fields, mlc::Array<mlc::String> ordered_target_field_names, registry::TypeRegistry registry) noexcept;
SpreadAccumulationState accumulate_spread_from_part(SpreadAccumulationState state, ast::RecordLitPart part_under_walk, mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_fields, mlc::Array<mlc::String> ordered_target_field_names, mlc::Array<std::shared_ptr<registry::Type>> spread_types_in_order, registry::TypeRegistry registry) noexcept;
mlc::Array<std::shared_ptr<ast::FieldVal>> merge_spread_slot(mlc::Array<std::shared_ptr<ast::FieldVal>> merged_so_far, mlc::String merge_field_name, mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_fields, mlc::Array<std::shared_ptr<ast::FieldVal>> spread_field_values) noexcept;
mlc::Array<std::shared_ptr<ast::FieldVal>> merge_record_literal_parts_to_field_values(mlc::String type_name, mlc::Array<ast::RecordLitPart> parts, registry::TypeRegistry registry, mlc::Array<std::shared_ptr<registry::Type>> spread_types_in_order) noexcept;
mlc::Array<ast::Diagnostic> missing_field_diagnostic_if_needed(mlc::Array<ast::Diagnostic> diagnostics_so_far, mlc::String expected_field_name, mlc::String type_name, registry::TypeRegistry registry, mlc::Array<std::shared_ptr<ast::FieldVal>> merged_field_values, ast::Span span) noexcept;
mlc::Array<ast::Diagnostic> diagnostics_missing_fields_for_ordered_record(mlc::String type_name, registry::TypeRegistry registry, mlc::Array<std::shared_ptr<ast::FieldVal>> merged_field_values, ast::Span span) noexcept;

} // namespace record_lit_merge

#endif // RECORD_LIT_MERGE_HPP
