#define main mlc_user_main
#include "derive_validation.hpp"

namespace derive_validation {

bool derive_trait_name_allowed(mlc::String trait_name) noexcept{
return ((((trait_name == mlc::String("Display", 7)) || (trait_name == mlc::String("Eq", 2))) || (trait_name == mlc::String("Ord", 3))) || (trait_name == mlc::String("Hash", 4)));
}
bool string_list_contains_derive_trait(mlc::Array<mlc::String> haystack, mlc::String needle) noexcept{
auto index = 0;
while ((index < haystack.length())) {
if ((haystack[index] == needle)) {
return true;
}
index = (index + 1);
}
return false;
}
bool type_expr_allowed_for_derive_hash_field(std::shared_ptr<ast::TypeExpr> type_expression) noexcept{
return std::visit(overloaded{[&](const ast::TyI32& tyI32) { return true; },
[&](const ast::TyBool& tyBool) { return true; },
[&](const ast::TyString& tyString) { return true; },
[&](const auto& __v) { return false; }
}, (*type_expression));
}
mlc::Array<ast::Diagnostic> derive_hash_field_error(mlc::String variant_label, mlc::String field_label, std::shared_ptr<ast::TypeExpr> type_expression, ast::Span diagnostic_span) noexcept{
if (type_expr_allowed_for_derive_hash_field(type_expression)) {
return mlc::Array<ast::Diagnostic>{};
} else {
return mlc::Array<ast::Diagnostic>{ast::diagnostic_error(((((mlc::String("derive Hash: unsupported field type for \"", 41) + field_label) + mlc::String("\" in ", 5)) + variant_label) + mlc::String(" (allowed: i32, bool, string)", 29)), diagnostic_span)};
}
}
mlc::Array<ast::Diagnostic> derive_hash_field_errors_for_variant(std::shared_ptr<ast::TypeVariant> variant, ast::Span diagnostic_span) noexcept{
return std::visit(overloaded{[&](const ast::VarUnit& varUnit) { auto [__0, __1] = varUnit; return [&]() {
auto diagnostics_none = mlc::Array<ast::Diagnostic>{};
return diagnostics_none;
}(); },
[&](const ast::VarTuple& varTuple) { auto [variant_name, field_types, __2] = varTuple; return [&]() {
auto collected = mlc::Array<ast::Diagnostic>{};
auto field_index = 0;
while ((field_index < field_types.length())) {
auto slot_label = (mlc::String("_", 1) + mlc::to_string(field_index));
auto chunk = derive_hash_field_error(((mlc::String("variant \"", 9) + variant_name) + mlc::String("\"", 1)), slot_label, field_types[field_index], diagnostic_span);
auto merge_index = 0;
while ((merge_index < chunk.length())) {
collected.push_back(chunk[merge_index]);
merge_index = (merge_index + 1);
}
field_index = (field_index + 1);
}
return collected;
}(); },
[&](const ast::VarRecord& varRecord) { auto [variant_name, field_defs, __2] = varRecord; return [&]() {
auto collected = mlc::Array<ast::Diagnostic>{};
auto field_index = 0;
while ((field_index < field_defs.length())) {
auto field_definition = field_defs[field_index];
auto chunk = derive_hash_field_error(((mlc::String("variant \"", 9) + variant_name) + mlc::String("\"", 1)), field_definition->name, field_definition->typ, diagnostic_span);
auto merge_index = 0;
while ((merge_index < chunk.length())) {
collected.push_back(chunk[merge_index]);
merge_index = (merge_index + 1);
}
field_index = (field_index + 1);
}
return collected;
}(); }
}, (*variant));
}
mlc::Array<ast::Diagnostic> derive_hash_errors_for_variants(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, ast::Span diagnostic_span) noexcept{
auto collected = mlc::Array<ast::Diagnostic>{};
auto variant_index = 0;
while ((variant_index < variants.length())) {
auto chunk = derive_hash_field_errors_for_variant(variants[variant_index], diagnostic_span);
auto merge_index = 0;
while ((merge_index < chunk.length())) {
collected.push_back(chunk[merge_index]);
merge_index = (merge_index + 1);
}
variant_index = (variant_index + 1);
}
return collected;
}
mlc::Array<ast::Diagnostic> derive_clause_diagnostics(mlc::Array<mlc::String> type_parameter_names, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_trait_names, ast::Span diagnostic_span) noexcept{
auto collected = mlc::Array<ast::Diagnostic>{};
if ((derive_trait_names.length() == 0)) {
return collected;
}
auto seen = mlc::HashMap<mlc::String, bool>();
auto trait_index = 0;
while ((trait_index < derive_trait_names.length())) {
auto trait_name = derive_trait_names[trait_index];
if (seen.has(trait_name)) {
collected.push_back(ast::diagnostic_error(((mlc::String("duplicate derive trait \"", 24) + trait_name) + mlc::String("\"", 1)), diagnostic_span));
} else {
seen.set(trait_name, true);
}
if ((!derive_trait_name_allowed(trait_name))) {
collected.push_back(ast::diagnostic_error(((mlc::String("unknown derive trait \"", 22) + trait_name) + mlc::String("\"", 1)), diagnostic_span));
}
trait_index = (trait_index + 1);
}
if (string_list_contains_derive_trait(derive_trait_names, mlc::String("Hash", 4))) {
if ((type_parameter_names.length() > 0)) {
collected.push_back(ast::diagnostic_error(mlc::String("derive Hash is not supported for generic types in this version", 62), diagnostic_span));
} else {
auto hash_field_errors = derive_hash_errors_for_variants(variants, diagnostic_span);
auto merge_index = 0;
[&]() {
while ((merge_index < hash_field_errors.length())) {
collected.push_back(hash_field_errors[merge_index]);
merge_index = (merge_index + 1);
}
}();
}
}
return collected;
}

} // namespace derive_validation
