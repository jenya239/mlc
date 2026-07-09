#include "record_lit_merge.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "diagnostic_codes.hpp"

namespace record_lit_merge {

using namespace ast;
using namespace registry;
using namespace diagnostic_codes;
using namespace ast_tokens;

struct SpreadAccumulationState {mlc::Array<std::shared_ptr<ast::FieldVal>> spread_field_values;int spread_type_index;};

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

record_lit_merge::SpreadAccumulationState accumulate_spread_from_part(record_lit_merge::SpreadAccumulationState state, ast::RecordLitPart part_under_walk, mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_fields, mlc::Array<mlc::String> ordered_target_field_names, mlc::Array<std::shared_ptr<registry::Type>> spread_types_in_order, registry::TypeRegistry registry) noexcept;

mlc::Array<std::shared_ptr<ast::FieldVal>> merge_spread_slot(mlc::Array<std::shared_ptr<ast::FieldVal>> merged_so_far, mlc::String merge_field_name, mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_fields, mlc::Array<std::shared_ptr<ast::FieldVal>> spread_field_values) noexcept;

mlc::Array<std::shared_ptr<ast::FieldVal>> merge_record_literal_parts_to_field_values(mlc::String type_name, mlc::Array<ast::RecordLitPart> parts, registry::TypeRegistry registry, mlc::Array<std::shared_ptr<registry::Type>> spread_types_in_order) noexcept;

mlc::Array<ast::Diagnostic> missing_field_diagnostic_if_needed(mlc::Array<ast::Diagnostic> diagnostics_so_far, mlc::String expected_field_name, mlc::String type_name, registry::TypeRegistry registry, mlc::Array<std::shared_ptr<ast::FieldVal>> merged_field_values, ast::Span span) noexcept;

mlc::Array<ast::Diagnostic> diagnostics_missing_fields_for_ordered_record(mlc::String type_name, registry::TypeRegistry registry, mlc::Array<std::shared_ptr<ast::FieldVal>> merged_field_values, ast::Span span) noexcept;

bool flat_field_values_contains_name(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::String sought_name) noexcept{return field_values.fold(false, [sought_name](bool found, std::shared_ptr<ast::FieldVal> row) mutable { return found ? true : row->name == sought_name; });}

std::shared_ptr<ast::FieldVal> flat_field_value_by_name(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::String sought_name) noexcept{return field_values.filter([sought_name](std::shared_ptr<ast::FieldVal> row) mutable { return row->name == sought_name; })[0];}

mlc::Array<std::shared_ptr<ast::FieldVal>> field_values_upsert_name(mlc::Array<std::shared_ptr<ast::FieldVal>> fields, std::shared_ptr<ast::FieldVal> field_value) noexcept{return fields.filter([field_value](std::shared_ptr<ast::FieldVal> row) mutable { return row->name != field_value->name; }).concat(mlc::Array<std::shared_ptr<ast::FieldVal>>{field_value});}

mlc::Array<std::shared_ptr<ast::FieldVal>> empty_field_value_list() noexcept{
mlc::Array<std::shared_ptr<ast::FieldVal>> empty_fields = {};
return empty_fields;
}

mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_field_values_for_part(ast::RecordLitPart part_under_walk) noexcept{return std::visit(overloaded{
  [&](const RecordLitFields& recordlitfields) -> mlc::Array<std::shared_ptr<ast::FieldVal>> { auto [field_values] = recordlitfields; return field_values; },
  [&](const RecordLitSpread& recordlitspread) -> mlc::Array<std::shared_ptr<ast::FieldVal>> { auto [_w0] = recordlitspread; return empty_field_value_list(); }
}, part_under_walk);}

mlc::Array<std::shared_ptr<ast::FieldVal>> collect_explicit_field_values_flat(mlc::Array<ast::RecordLitPart> parts) noexcept{return parts.fold([&]() -> mlc::Array<std::shared_ptr<ast::FieldVal>> { 
  mlc::Array<std::shared_ptr<ast::FieldVal>> empty_fields = {};
  return empty_fields;
 }(), [](mlc::Array<std::shared_ptr<ast::FieldVal>> accumulated_fields, ast::RecordLitPart part_under_walk) mutable { return accumulated_fields.concat(explicit_field_values_for_part(part_under_walk)); });}

mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_fields_flat_last_wins(mlc::Array<ast::RecordLitPart> parts) noexcept{return collect_explicit_field_values_flat(parts).fold([&]() -> mlc::Array<std::shared_ptr<ast::FieldVal>> { 
  mlc::Array<std::shared_ptr<ast::FieldVal>> empty_fields = {};
  return empty_fields;
 }(), [](mlc::Array<std::shared_ptr<ast::FieldVal>> flat_fields, std::shared_ptr<ast::FieldVal> field_value) mutable { return field_values_upsert_name(flat_fields, field_value); });}

mlc::Array<std::shared_ptr<ast::FieldVal>> merge_ordered_slot(mlc::Array<std::shared_ptr<ast::FieldVal>> merged_so_far, mlc::String slot_field_name, mlc::String nominal_record_name, mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_fields_flat, registry::TypeRegistry registry) noexcept{
return flat_field_values_contains_name(explicit_fields_flat, slot_field_name) ? merged_so_far.concat(mlc::Array<std::shared_ptr<ast::FieldVal>>{flat_field_value_by_name(explicit_fields_flat, slot_field_name)}) : registry::TypeRegistry_record_field_has_default_expression(registry, nominal_record_name, slot_field_name) ? merged_so_far.concat(mlc::Array<std::shared_ptr<ast::FieldVal>>{std::make_shared<ast::FieldVal>(ast::FieldVal{slot_field_name, registry::TypeRegistry_record_field_default_expression_ast(registry, nominal_record_name, slot_field_name)})}) : merged_so_far;
}

mlc::Array<std::shared_ptr<ast::FieldVal>> merge_explicit_record_literal_with_type_defaults_nonempty(mlc::String nominal_record_name, mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_fields_flat, registry::TypeRegistry registry) noexcept{
mlc::Array<mlc::String> ordered_target_names = registry::TypeRegistry_record_field_names_ordered_for(registry, nominal_record_name);
return ordered_target_names.fold([&]() -> mlc::Array<std::shared_ptr<ast::FieldVal>> { 
  mlc::Array<std::shared_ptr<ast::FieldVal>> empty_fields = {};
  return empty_fields;
 }(), [nominal_record_name, explicit_fields_flat, registry](mlc::Array<std::shared_ptr<ast::FieldVal>> merged_ordered_fields, mlc::String slot_field_name) mutable { return merge_ordered_slot(merged_ordered_fields, slot_field_name, nominal_record_name, explicit_fields_flat, registry); });
}

mlc::Array<std::shared_ptr<ast::FieldVal>> merge_explicit_record_literal_with_type_defaults(mlc::String nominal_record_name, mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_fields_flat, registry::TypeRegistry registry) noexcept{
return !registry::TypeRegistry_has_fields(registry, nominal_record_name) ? explicit_fields_flat : merge_explicit_record_literal_with_type_defaults_nonempty(nominal_record_name, explicit_fields_flat, registry);
}

mlc::String record_type_base_name(std::shared_ptr<registry::Type> value_type) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*value_type))) { auto _v_tnamed = std::get<registry::TNamed>((*value_type)); auto [name] = _v_tnamed; return name; } if (std::holds_alternative<registry::TGeneric>((*value_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*value_type)); auto [name, _w0] = _v_tgeneric; return name; } if (std::holds_alternative<registry::TShared>((*value_type))) { auto _v_tshared = std::get<registry::TShared>((*value_type)); auto [shared_inner_type] = _v_tshared; return record_type_base_name(shared_inner_type); } return mlc::String(""); }();}

bool part_is_record_spread(ast::RecordLitPart part_under_walk) noexcept{return std::visit(overloaded{
  [&](const RecordLitSpread& recordlitspread) -> bool { auto [_w0] = recordlitspread; return true; },
  [&](const RecordLitFields& recordlitfields) -> bool { auto [_w0] = recordlitfields; return false; }
}, part_under_walk);}

bool record_literal_contains_spread(mlc::Array<ast::RecordLitPart> parts) noexcept{return parts.fold(false, [](bool found, ast::RecordLitPart part_under_walk) mutable { return found ? true : part_is_record_spread(part_under_walk); });}

mlc::Array<std::shared_ptr<ast::FieldVal>> spread_candidate_field(mlc::String candidate_field_name, std::shared_ptr<ast::Expr> spread_expression, mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_fields, mlc::Array<std::shared_ptr<ast::FieldVal>> accumulated_spread_fields, mlc::Array<std::shared_ptr<ast::FieldVal>> expanded_fields, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> spread_field_types) noexcept{
return flat_field_values_contains_name(explicit_fields, candidate_field_name) ? expanded_fields : flat_field_values_contains_name(accumulated_spread_fields, candidate_field_name) ? expanded_fields : flat_field_values_contains_name(expanded_fields, candidate_field_name) ? expanded_fields : !spread_field_types.has(candidate_field_name) ? expanded_fields : expanded_fields.concat(mlc::Array<std::shared_ptr<ast::FieldVal>>{std::make_shared<ast::FieldVal>(ast::FieldVal{candidate_field_name, std::make_shared<ast::Expr>(ast::ExprField(spread_expression, candidate_field_name, ast::expr_span(spread_expression)))})});
}

mlc::Array<std::shared_ptr<ast::FieldVal>> spread_fields_for_part(std::shared_ptr<ast::Expr> spread_expression, std::shared_ptr<registry::Type> spread_value_type, mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_fields, mlc::Array<std::shared_ptr<ast::FieldVal>> accumulated_spread_fields, mlc::Array<mlc::String> ordered_target_field_names, registry::TypeRegistry registry) noexcept{
mlc::String spread_base_name = record_type_base_name(spread_value_type);
return spread_base_name == mlc::String("") || !registry::TypeRegistry_has_fields(registry, spread_base_name) ? empty_field_value_list() : [&]() -> mlc::Array<std::shared_ptr<ast::FieldVal>> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> spread_field_types = registry::TypeRegistry_fields_for(registry, spread_base_name);
  return ordered_target_field_names.fold([&]() -> mlc::Array<std::shared_ptr<ast::FieldVal>> { 
  mlc::Array<std::shared_ptr<ast::FieldVal>> empty_fields = {};
  return empty_fields;
 }(), [spread_expression, explicit_fields, accumulated_spread_fields, spread_field_types](mlc::Array<std::shared_ptr<ast::FieldVal>> expanded_fields, mlc::String candidate_field_name) mutable { return spread_candidate_field(candidate_field_name, spread_expression, explicit_fields, accumulated_spread_fields, expanded_fields, spread_field_types); });
 }();
}

record_lit_merge::SpreadAccumulationState accumulate_spread_from_part(record_lit_merge::SpreadAccumulationState state, ast::RecordLitPart part_under_walk, mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_fields, mlc::Array<mlc::String> ordered_target_field_names, mlc::Array<std::shared_ptr<registry::Type>> spread_types_in_order, registry::TypeRegistry registry) noexcept{return std::visit(overloaded{
  [&](const RecordLitSpread& recordlitspread) -> record_lit_merge::SpreadAccumulationState { auto [spread_expression] = recordlitspread; return [&]() -> record_lit_merge::SpreadAccumulationState { 
  mlc::Array<std::shared_ptr<ast::FieldVal>> expanded_from_spread = spread_fields_for_part(spread_expression, spread_types_in_order[state.spread_type_index], explicit_fields, state.spread_field_values, ordered_target_field_names, registry);
  return record_lit_merge::SpreadAccumulationState{state.spread_field_values.concat(expanded_from_spread), state.spread_type_index + 1};
 }(); },
  [&](const RecordLitFields& recordlitfields) -> record_lit_merge::SpreadAccumulationState { auto [_w0] = recordlitfields; return state; }
}, part_under_walk);}

mlc::Array<std::shared_ptr<ast::FieldVal>> merge_spread_slot(mlc::Array<std::shared_ptr<ast::FieldVal>> merged_so_far, mlc::String merge_field_name, mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_fields, mlc::Array<std::shared_ptr<ast::FieldVal>> spread_field_values) noexcept{
return flat_field_values_contains_name(explicit_fields, merge_field_name) ? merged_so_far.concat(mlc::Array<std::shared_ptr<ast::FieldVal>>{flat_field_value_by_name(explicit_fields, merge_field_name)}) : flat_field_values_contains_name(spread_field_values, merge_field_name) ? merged_so_far.concat(mlc::Array<std::shared_ptr<ast::FieldVal>>{flat_field_value_by_name(spread_field_values, merge_field_name)}) : merged_so_far;
}

mlc::Array<std::shared_ptr<ast::FieldVal>> merge_record_literal_parts_to_field_values(mlc::String type_name, mlc::Array<ast::RecordLitPart> parts, registry::TypeRegistry registry, mlc::Array<std::shared_ptr<registry::Type>> spread_types_in_order) noexcept{
mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_fields = explicit_fields_flat_last_wins(parts);
mlc::Array<mlc::String> ordered_target_field_names = registry::TypeRegistry_record_field_names_ordered_for(registry, type_name);
record_lit_merge::SpreadAccumulationState spread_state = parts.fold(record_lit_merge::SpreadAccumulationState{[&]() -> mlc::Array<std::shared_ptr<ast::FieldVal>> { 
  mlc::Array<std::shared_ptr<ast::FieldVal>> empty_fields = {};
  return empty_fields;
 }(), 0}, [explicit_fields, ordered_target_field_names, spread_types_in_order, registry](record_lit_merge::SpreadAccumulationState state, ast::RecordLitPart part_under_walk) mutable { return accumulate_spread_from_part(state, part_under_walk, explicit_fields, ordered_target_field_names, spread_types_in_order, registry); });
return ordered_target_field_names.fold([&]() -> mlc::Array<std::shared_ptr<ast::FieldVal>> { 
  mlc::Array<std::shared_ptr<ast::FieldVal>> empty_fields = {};
  return empty_fields;
 }(), [explicit_fields, spread_state](mlc::Array<std::shared_ptr<ast::FieldVal>> merged_field_values, mlc::String merge_field_name) mutable { return merge_spread_slot(merged_field_values, merge_field_name, explicit_fields, spread_state.spread_field_values); });
}

mlc::Array<ast::Diagnostic> missing_field_diagnostic_if_needed(mlc::Array<ast::Diagnostic> diagnostics_so_far, mlc::String expected_field_name, mlc::String type_name, registry::TypeRegistry registry, mlc::Array<std::shared_ptr<ast::FieldVal>> merged_field_values, ast::Span span) noexcept{
return flat_field_values_contains_name(merged_field_values, expected_field_name) ? diagnostics_so_far : registry::TypeRegistry_record_field_has_default_expression(registry, type_name, expected_field_name) ? diagnostics_so_far : diagnostics_so_far.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("missing field \"") + expected_field_name + mlc::String("\" in record literal"), span, diagnostic_codes::diagnostic_code_e030())});
}

mlc::Array<ast::Diagnostic> diagnostics_missing_fields_for_ordered_record(mlc::String type_name, registry::TypeRegistry registry, mlc::Array<std::shared_ptr<ast::FieldVal>> merged_field_values, ast::Span span) noexcept{
mlc::Array<mlc::String> expected_field_names = registry::TypeRegistry_record_field_names_ordered_for(registry, type_name);
return expected_field_names.fold([&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty_diagnostics = {};
  return empty_diagnostics;
 }(), [type_name, registry, merged_field_values, span](mlc::Array<ast::Diagnostic> missing_field_diagnostics, mlc::String expected_field_name) mutable { return missing_field_diagnostic_if_needed(missing_field_diagnostics, expected_field_name, type_name, registry, merged_field_values, span); });
}

} // namespace record_lit_merge
