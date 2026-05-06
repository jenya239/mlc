#include "record_lit_merge.hpp"

#include "ast.hpp"
#include "registry.hpp"

namespace record_lit_merge {

using namespace ast;
using namespace registry;
using namespace ast_tokens;

mlc::HashMap<mlc::String, std::shared_ptr<ast::FieldVal>> field_values_map_from_flat(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values) noexcept;

mlc::Array<std::shared_ptr<ast::FieldVal>> merge_explicit_record_literal_with_type_defaults_nonempty(mlc::String nominal_record_name, mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_fields_flat, registry::TypeRegistry registry) noexcept;

mlc::Array<std::shared_ptr<ast::FieldVal>> merge_explicit_record_literal_with_type_defaults(mlc::String nominal_record_name, mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_fields_flat, registry::TypeRegistry registry) noexcept;

mlc::String record_type_base_name(std::shared_ptr<registry::Type> value_type) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<ast::FieldVal>> explicit_field_map_last_wins(mlc::Array<ast::RecordLitPart> parts) noexcept;

mlc::Array<std::shared_ptr<ast::FieldVal>> collect_explicit_field_values_flat(mlc::Array<ast::RecordLitPart> parts) noexcept;

bool record_literal_contains_spread(mlc::Array<ast::RecordLitPart> parts) noexcept;

mlc::Array<std::shared_ptr<ast::FieldVal>> merge_record_literal_parts_to_field_values(mlc::String type_name, mlc::Array<ast::RecordLitPart> parts, registry::TypeRegistry registry, mlc::Array<std::shared_ptr<registry::Type>> spread_types_in_order) noexcept;

mlc::Array<ast::Diagnostic> diagnostics_missing_fields_for_ordered_record(mlc::String type_name, registry::TypeRegistry registry, mlc::Array<std::shared_ptr<ast::FieldVal>> merged_field_values, ast::Span span) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<ast::FieldVal>> field_values_map_from_flat(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<ast::FieldVal>> accumulator = mlc::HashMap<mlc::String, std::shared_ptr<ast::FieldVal>>();
int index = 0;
while (index < field_values.size()){
{
std::shared_ptr<ast::FieldVal> row = field_values[index];
accumulator.set(row->name, row);
index = index + 1;
}
}
return accumulator;
}

mlc::Array<std::shared_ptr<ast::FieldVal>> merge_explicit_record_literal_with_type_defaults_nonempty(mlc::String nominal_record_name, mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_fields_flat, registry::TypeRegistry registry) noexcept{
mlc::Array<mlc::String> ordered_target_names = registry::TypeRegistry_record_field_names_ordered_for(registry, nominal_record_name);
mlc::HashMap<mlc::String, std::shared_ptr<ast::FieldVal>> explicit_by_field_name = field_values_map_from_flat(explicit_fields_flat);
mlc::Array<std::shared_ptr<ast::FieldVal>> merged_ordered_fields = {};
int ordered_walk_index = 0;
while (ordered_walk_index < ordered_target_names.size()){
{
mlc::String slot_field_name = ordered_target_names[ordered_walk_index];
if (explicit_by_field_name.has(slot_field_name)){
{
merged_ordered_fields.push_back(explicit_by_field_name.get(slot_field_name));
}
} else {
{
if (registry::TypeRegistry_record_field_has_default_expression(registry, nominal_record_name, slot_field_name)){
merged_ordered_fields.push_back(std::make_shared<ast::FieldVal>(ast::FieldVal{slot_field_name, registry::TypeRegistry_record_field_default_expression_ast(registry, nominal_record_name, slot_field_name)}));
}
}
}
ordered_walk_index = ordered_walk_index + 1;
}
}
return merged_ordered_fields;
}

mlc::Array<std::shared_ptr<ast::FieldVal>> merge_explicit_record_literal_with_type_defaults(mlc::String nominal_record_name, mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_fields_flat, registry::TypeRegistry registry) noexcept{
return !registry::TypeRegistry_has_fields(registry, nominal_record_name) ? explicit_fields_flat : merge_explicit_record_literal_with_type_defaults_nonempty(nominal_record_name, explicit_fields_flat, registry);
}

mlc::String record_type_base_name(std::shared_ptr<registry::Type> value_type) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*value_type))) { auto _v_tnamed = std::get<registry::TNamed>((*value_type)); auto [name] = _v_tnamed; return name; } if (std::holds_alternative<registry::TGeneric>((*value_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*value_type)); auto [name, _w0] = _v_tgeneric; return name; } if (std::holds_alternative<registry::TShared>((*value_type))) { auto _v_tshared = std::get<registry::TShared>((*value_type)); auto [inner] = _v_tshared; return record_type_base_name(inner); } return mlc::String(""); }();}

mlc::HashMap<mlc::String, std::shared_ptr<ast::FieldVal>> explicit_field_map_last_wins(mlc::Array<ast::RecordLitPart> parts) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<ast::FieldVal>> accumulator = mlc::HashMap<mlc::String, std::shared_ptr<ast::FieldVal>>();
int part_index = 0;
while (part_index < parts.size()){
{
std::visit(overloaded{
  [&](const RecordLitFields& recordlitfields) -> void { auto [field_values] = recordlitfields; [&]() { 
  int field_index = 0;
  return [&]() { 
  while (field_index < field_values.size()){
{
std::shared_ptr<ast::FieldVal> field_value = field_values[field_index];
accumulator.set(field_value->name, field_value);
field_index = field_index + 1;
}
}
 }();
 }(); },
  [&](const RecordLitSpread& recordlitspread) -> void { auto [_w0] = recordlitspread; std::make_tuple(); }
}, parts[part_index]._);
part_index = part_index + 1;
}
}
return accumulator;
}

mlc::Array<std::shared_ptr<ast::FieldVal>> collect_explicit_field_values_flat(mlc::Array<ast::RecordLitPart> parts) noexcept{
mlc::Array<std::shared_ptr<ast::FieldVal>> collected = {};
int part_index = 0;
while (part_index < parts.size()){
{
std::visit(overloaded{
  [&](const RecordLitFields& recordlitfields) -> void { auto [field_values] = recordlitfields; [&]() { 
  int field_index = 0;
  return [&]() { 
  while (field_index < field_values.size()){
{
collected.push_back(field_values[field_index]);
field_index = field_index + 1;
}
}
 }();
 }(); },
  [&](const RecordLitSpread& recordlitspread) -> void { auto [_w0] = recordlitspread; std::make_tuple(); }
}, parts[part_index]._);
part_index = part_index + 1;
}
}
return collected;
}

bool record_literal_contains_spread(mlc::Array<ast::RecordLitPart> parts) noexcept{
int part_index = 0;
bool found_spread = false;
while (part_index < parts.size() && !found_spread){
{
std::visit(overloaded{
  [&](const RecordLitSpread& recordlitspread) -> void { auto [_w0] = recordlitspread; [&]() { 
  found_spread = true;
 }(); },
  [&](const RecordLitFields& recordlitfields) -> void { auto [_w0] = recordlitfields; std::make_tuple(); }
}, parts[part_index]._);
part_index = part_index + 1;
}
}
return found_spread;
}

mlc::Array<std::shared_ptr<ast::FieldVal>> merge_record_literal_parts_to_field_values(mlc::String type_name, mlc::Array<ast::RecordLitPart> parts, registry::TypeRegistry registry, mlc::Array<std::shared_ptr<registry::Type>> spread_types_in_order) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<ast::FieldVal>> explicit_map = explicit_field_map_last_wins(parts);
mlc::Array<mlc::String> ordered_target_field_names = registry::TypeRegistry_record_field_names_ordered_for(registry, type_name);
mlc::HashMap<mlc::String, std::shared_ptr<ast::Expr>> expanded_member_expressions = mlc::HashMap<mlc::String, std::shared_ptr<ast::Expr>>();
int spread_type_index = 0;
int part_walk_index = 0;
while (part_walk_index < parts.size()){
{
std::visit(overloaded{
  [&](const RecordLitSpread& recordlitspread) -> void { auto [spread_expression] = recordlitspread; [&]() { 
  std::shared_ptr<registry::Type> spread_value_type = spread_types_in_order[spread_type_index];
  spread_type_index = spread_type_index + 1;
  mlc::String spread_base_name = record_type_base_name(spread_value_type);
  if (spread_base_name != mlc::String("") && registry::TypeRegistry_has_fields(registry, spread_base_name)){
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> spread_field_types = registry::TypeRegistry_fields_for(registry, spread_base_name);
int ordered_index = 0;
[&]() { 
  while (ordered_index < ordered_target_field_names.size()){
{
mlc::String candidate_field_name = ordered_target_field_names[ordered_index];
if (!explicit_map.has(candidate_field_name) && !expanded_member_expressions.has(candidate_field_name) && spread_field_types.has(candidate_field_name)){
{
expanded_member_expressions.set(candidate_field_name, std::make_shared<ast::Expr>(ast::ExprField(spread_expression, candidate_field_name, ast::expr_span(spread_expression))));
}
}
ordered_index = ordered_index + 1;
}
}
 }();
}
 }(); },
  [&](const RecordLitFields& recordlitfields) -> void { auto [_w0] = recordlitfields; std::make_tuple(); }
}, parts[part_walk_index]._);
part_walk_index = part_walk_index + 1;
}
}
mlc::Array<std::shared_ptr<ast::FieldVal>> merged_field_values = {};
int ordered_merge_index = 0;
while (ordered_merge_index < ordered_target_field_names.size()){
{
mlc::String merge_field_name = ordered_target_field_names[ordered_merge_index];
if (explicit_map.has(merge_field_name)){
{
merged_field_values.push_back(explicit_map.get(merge_field_name));
}
} else {
{
if (expanded_member_expressions.has(merge_field_name)){
merged_field_values.push_back(std::make_shared<ast::FieldVal>(ast::FieldVal{merge_field_name, expanded_member_expressions.get(merge_field_name)}));
}
}
}
ordered_merge_index = ordered_merge_index + 1;
}
}
return merged_field_values;
}

mlc::Array<ast::Diagnostic> diagnostics_missing_fields_for_ordered_record(mlc::String type_name, registry::TypeRegistry registry, mlc::Array<std::shared_ptr<ast::FieldVal>> merged_field_values, ast::Span span) noexcept{
mlc::Array<mlc::String> ordered_target_names = registry::TypeRegistry_record_field_names_ordered_for(registry, type_name);
return ordered_target_names.size() == 0 ? [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty_missing_field_diagnostics = {};
  return empty_missing_field_diagnostics;
 }() : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> missing_field_diagnostics = {};
  int ordered_index = 0;
  while (ordered_index < ordered_target_names.size()){
{
mlc::String expected_field_name = ordered_target_names[ordered_index];
bool found_present = false;
int merged_index = 0;
while (merged_index < merged_field_values.size() && !found_present){
{
if (merged_field_values[merged_index]->name == expected_field_name){
{
found_present = true;
}
}
merged_index = merged_index + 1;
}
}
if (!found_present && !registry::TypeRegistry_record_field_has_default_expression(registry, type_name, expected_field_name)){
{
missing_field_diagnostics.push_back(ast::diagnostic_error(mlc::String("missing field \"") + expected_field_name + mlc::String("\" in record literal"), span));
}
}
ordered_index = ordered_index + 1;
}
}
  return missing_field_diagnostics;
 }();
}

} // namespace record_lit_merge
