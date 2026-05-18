#define main mlc_user_main
#include "record_lit_merge.hpp"

namespace record_lit_merge {

mlc::HashMap<mlc::String, std::shared_ptr<ast::FieldVal>> field_values_map_from_flat(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values) noexcept{
auto accumulator = mlc::HashMap<mlc::String, std::shared_ptr<ast::FieldVal>>();
auto index = 0;
while ((index < field_values.length())) {
auto row = field_values[index];
accumulator.set(row->name, row);
index = (index + 1);
}
return accumulator;
}
mlc::Array<std::shared_ptr<ast::FieldVal>> merge_explicit_record_literal_with_type_defaults_nonempty(mlc::String nominal_record_name, mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_fields_flat, registry::TypeRegistry registry) noexcept{
auto ordered_target_names = registry::TypeRegistry_record_field_names_ordered_for(registry, nominal_record_name);
auto explicit_by_field_name = field_values_map_from_flat(explicit_fields_flat);
auto merged_ordered_fields = mlc::Array<std::shared_ptr<ast::FieldVal>>{};
auto ordered_walk_index = 0;
while ((ordered_walk_index < ordered_target_names.length())) {
auto slot_field_name = ordered_target_names[ordered_walk_index];
if (explicit_by_field_name.has(slot_field_name)) {
merged_ordered_fields.push_back(explicit_by_field_name.get(slot_field_name));
} else if (registry::TypeRegistry_record_field_has_default_expression(registry, nominal_record_name, slot_field_name)) {
merged_ordered_fields.push_back(std::make_shared<ast::FieldVal>(ast::FieldVal{slot_field_name, registry::TypeRegistry_record_field_default_expression_ast(registry, nominal_record_name, slot_field_name)}));
}

ordered_walk_index = (ordered_walk_index + 1);
}
return merged_ordered_fields;
}
mlc::Array<std::shared_ptr<ast::FieldVal>> merge_explicit_record_literal_with_type_defaults(mlc::String nominal_record_name, mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_fields_flat, registry::TypeRegistry registry) noexcept{
if ((!registry::TypeRegistry_has_fields(registry, nominal_record_name))) {
return explicit_fields_flat;
} else {
return merge_explicit_record_literal_with_type_defaults_nonempty(nominal_record_name, explicit_fields_flat, registry);
}
}
mlc::String record_type_base_name(std::shared_ptr<registry::Type> value_type) noexcept{
return std::visit(overloaded{[&](const registry::TNamed& tNamed) { auto [name] = tNamed; return name; },
[&](const registry::TGeneric& tGeneric) { auto [name, __1] = tGeneric; return name; },
[&](const registry::TShared& tShared) { auto [inner] = tShared; return record_type_base_name(inner); },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*value_type));
}
mlc::HashMap<mlc::String, std::shared_ptr<ast::FieldVal>> explicit_field_map_last_wins(mlc::Array<ast::RecordLitPart> parts) noexcept{
auto accumulator = mlc::HashMap<mlc::String, std::shared_ptr<ast::FieldVal>>();
auto part_index = 0;
while ((part_index < parts.length())) {
std::visit(overloaded{[&](const ast::RecordLitFields& recordLitFields) { auto [field_values] = recordLitFields; return [&]() {
auto field_index = 0;
return [&]() {
while ((field_index < field_values.length())) {
auto field_value = field_values[field_index];
accumulator.set(field_value->name, field_value);
field_index = (field_index + 1);
}
}();
}(); },
[&](const ast::RecordLitSpread& recordLitSpread) { auto [__0] = recordLitSpread; return /* unit */; }
}, parts[part_index]);
part_index = (part_index + 1);
}
return accumulator;
}
mlc::Array<std::shared_ptr<ast::FieldVal>> collect_explicit_field_values_flat(mlc::Array<ast::RecordLitPart> parts) noexcept{
auto collected = mlc::Array<std::shared_ptr<ast::FieldVal>>{};
auto part_index = 0;
while ((part_index < parts.length())) {
std::visit(overloaded{[&](const ast::RecordLitFields& recordLitFields) { auto [field_values] = recordLitFields; return [&]() {
auto field_index = 0;
return [&]() {
while ((field_index < field_values.length())) {
collected.push_back(field_values[field_index]);
field_index = (field_index + 1);
}
}();
}(); },
[&](const ast::RecordLitSpread& recordLitSpread) { auto [__0] = recordLitSpread; return /* unit */; }
}, parts[part_index]);
part_index = (part_index + 1);
}
return collected;
}
bool record_literal_contains_spread(mlc::Array<ast::RecordLitPart> parts) noexcept{
auto part_index = 0;
auto found_spread = false;
while (((part_index < parts.length()) && (!found_spread))) {
std::visit(overloaded{[&](const ast::RecordLitSpread& recordLitSpread) { auto [__0] = recordLitSpread; return [&]() {
found_spread = true;
return /* unit */;
}(); },
[&](const ast::RecordLitFields& recordLitFields) { auto [__0] = recordLitFields; return /* unit */; }
}, parts[part_index]);
part_index = (part_index + 1);
}
return found_spread;
}
mlc::Array<std::shared_ptr<ast::FieldVal>> merge_record_literal_parts_to_field_values(mlc::String type_name, mlc::Array<ast::RecordLitPart> parts, registry::TypeRegistry registry, mlc::Array<std::shared_ptr<registry::Type>> spread_types_in_order) noexcept{
auto explicit_map = explicit_field_map_last_wins(parts);
auto ordered_target_field_names = registry::TypeRegistry_record_field_names_ordered_for(registry, type_name);
auto expanded_member_expressions = mlc::HashMap<mlc::String, std::shared_ptr<ast::Expr>>();
auto spread_type_index = 0;
auto part_walk_index = 0;
while ((part_walk_index < parts.length())) {
std::visit(overloaded{[&](const ast::RecordLitSpread& recordLitSpread) { auto [spread_expression] = recordLitSpread; return [&]() {
auto spread_value_type = spread_types_in_order[spread_type_index];
spread_type_index = (spread_type_index + 1);
auto spread_base_name = record_type_base_name(spread_value_type);
if (((spread_base_name != mlc::String("", 0)) && registry::TypeRegistry_has_fields(registry, spread_base_name))) {
auto spread_field_types = registry::TypeRegistry_fields_for(registry, spread_base_name);
auto ordered_index = 0;
[&]() {
while ((ordered_index < ordered_target_field_names.length())) {
auto candidate_field_name = ordered_target_field_names[ordered_index];
if ((((!explicit_map.has(candidate_field_name)) && (!expanded_member_expressions.has(candidate_field_name))) && spread_field_types.has(candidate_field_name))) {
expanded_member_expressions.set(candidate_field_name, std::make_shared<ast::Expr>(ast::ExprField{spread_expression, candidate_field_name, ast::expr_span(spread_expression)}));
}
ordered_index = (ordered_index + 1);
}
}();
}
return /* unit */;
}(); },
[&](const ast::RecordLitFields& recordLitFields) { auto [__0] = recordLitFields; return /* unit */; }
}, parts[part_walk_index]);
part_walk_index = (part_walk_index + 1);
}
auto merged_field_values = mlc::Array<std::shared_ptr<ast::FieldVal>>{};
auto ordered_merge_index = 0;
while ((ordered_merge_index < ordered_target_field_names.length())) {
auto merge_field_name = ordered_target_field_names[ordered_merge_index];
if (explicit_map.has(merge_field_name)) {
merged_field_values.push_back(explicit_map.get(merge_field_name));
} else if (expanded_member_expressions.has(merge_field_name)) {
merged_field_values.push_back(std::make_shared<ast::FieldVal>(ast::FieldVal{merge_field_name, expanded_member_expressions.get(merge_field_name)}));
}

ordered_merge_index = (ordered_merge_index + 1);
}
return merged_field_values;
}
mlc::Array<ast::Diagnostic> diagnostics_missing_fields_for_ordered_record(mlc::String type_name, registry::TypeRegistry registry, mlc::Array<std::shared_ptr<ast::FieldVal>> merged_field_values, ast::Span span) noexcept{
auto ordered_target_names = registry::TypeRegistry_record_field_names_ordered_for(registry, type_name);
if ((ordered_target_names.length() == 0)) {
auto empty_missing_field_diagnostics = mlc::Array<ast::Diagnostic>{};
return empty_missing_field_diagnostics;
} else {
auto missing_field_diagnostics = mlc::Array<ast::Diagnostic>{};
auto ordered_index = 0;
while ((ordered_index < ordered_target_names.length())) {
auto expected_field_name = ordered_target_names[ordered_index];
auto found_present = false;
auto merged_index = 0;
while (((merged_index < merged_field_values.length()) && (!found_present))) {
if ((merged_field_values[merged_index]->name == expected_field_name)) {
found_present = true;
}
merged_index = (merged_index + 1);
}
if (((!found_present) && (!registry::TypeRegistry_record_field_has_default_expression(registry, type_name, expected_field_name)))) {
missing_field_diagnostics.push_back(ast::diagnostic_error(((mlc::String("missing field \"", 15) + expected_field_name) + mlc::String("\" in record literal", 19)), span));
}
ordered_index = (ordered_index + 1);
}
return missing_field_diagnostics;
}
}

} // namespace record_lit_merge
