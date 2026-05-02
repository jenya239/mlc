#include "derive_validation.hpp"

#include "ast.hpp"

namespace derive_validation {

using namespace ast;
using namespace ast_tokens;

bool derive_trait_name_allowed(mlc::String trait_name) noexcept;

bool string_list_contains_derive_trait(mlc::Array<mlc::String> haystack, mlc::String needle) noexcept;

bool type_expr_allowed_for_derive_hash_field(std::shared_ptr<ast::TypeExpr> type_expression) noexcept;

mlc::Array<ast::Diagnostic> derive_hash_field_error(mlc::String variant_label, mlc::String field_label, std::shared_ptr<ast::TypeExpr> type_expression, ast::Span diagnostic_span) noexcept;

mlc::Array<ast::Diagnostic> derive_hash_field_errors_for_variant(std::shared_ptr<ast::TypeVariant> variant, ast::Span diagnostic_span) noexcept;

mlc::Array<ast::Diagnostic> derive_hash_errors_for_variants(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, ast::Span diagnostic_span) noexcept;

mlc::Array<ast::Diagnostic> derive_clause_diagnostics(mlc::Array<mlc::String> type_parameter_names, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_trait_names, ast::Span diagnostic_span) noexcept;

bool derive_trait_name_allowed(mlc::String trait_name) noexcept{return trait_name == mlc::String("Display") || trait_name == mlc::String("Eq") || trait_name == mlc::String("Ord") || trait_name == mlc::String("Hash");}

bool string_list_contains_derive_trait(mlc::Array<mlc::String> haystack, mlc::String needle) noexcept{
int index = 0;
while (index < haystack.size()){
{
if (haystack[index] == needle){
{
return true;
}
}
index = index + 1;
}
}
return false;
}

bool type_expr_allowed_for_derive_hash_field(std::shared_ptr<ast::TypeExpr> type_expression) noexcept{return [&]() { if (std::holds_alternative<ast::TyI32>((*type_expression))) {  return true; } if (std::holds_alternative<ast::TyBool>((*type_expression))) {  return true; } if (std::holds_alternative<ast::TyString>((*type_expression))) {  return true; } return false; }();}

mlc::Array<ast::Diagnostic> derive_hash_field_error(mlc::String variant_label, mlc::String field_label, std::shared_ptr<ast::TypeExpr> type_expression, ast::Span diagnostic_span) noexcept{
return type_expr_allowed_for_derive_hash_field(type_expression) ? mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("derive Hash: unsupported field type for \"") + field_label + mlc::String("\" in ") + variant_label + mlc::String(" (allowed: i32, bool, string)"), diagnostic_span)};
}

mlc::Array<ast::Diagnostic> derive_hash_field_errors_for_variant(std::shared_ptr<ast::TypeVariant> variant, ast::Span diagnostic_span) noexcept{return std::visit(overloaded{
  [&](const VarUnit& varunit) -> mlc::Array<ast::Diagnostic> { auto [_w0, _w1] = varunit; return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> diagnostics_none = {};
  return diagnostics_none;
 }(); },
  [&](const VarTuple& vartuple) -> mlc::Array<ast::Diagnostic> { auto [variant_name, field_types, _w0] = vartuple; return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> collected = {};
  int field_index = 0;
  while (field_index < field_types.size()){
{
mlc::String slot_label = mlc::String("_") + mlc::to_string(field_index);
mlc::Array<ast::Diagnostic> chunk = derive_hash_field_error(mlc::String("variant \"") + variant_name + mlc::String("\""), slot_label, field_types[field_index], diagnostic_span);
int merge_index = 0;
while (merge_index < chunk.size()){
{
collected.push_back(chunk[merge_index]);
merge_index = merge_index + 1;
}
}
field_index = field_index + 1;
}
}
  return collected;
 }(); },
  [&](const VarRecord& varrecord) -> mlc::Array<ast::Diagnostic> { auto [variant_name, field_defs, _w0] = varrecord; return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> collected = {};
  int field_index = 0;
  while (field_index < field_defs.size()){
{
std::shared_ptr<ast::FieldDef> field_definition = field_defs[field_index];
mlc::Array<ast::Diagnostic> chunk = derive_hash_field_error(mlc::String("variant \"") + variant_name + mlc::String("\""), field_definition->name, field_definition->typ, diagnostic_span);
int merge_index = 0;
while (merge_index < chunk.size()){
{
collected.push_back(chunk[merge_index]);
merge_index = merge_index + 1;
}
}
field_index = field_index + 1;
}
}
  return collected;
 }(); }
}, (*variant));}

mlc::Array<ast::Diagnostic> derive_hash_errors_for_variants(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, ast::Span diagnostic_span) noexcept{
mlc::Array<ast::Diagnostic> collected = {};
int variant_index = 0;
while (variant_index < variants.size()){
{
mlc::Array<ast::Diagnostic> chunk = derive_hash_field_errors_for_variant(variants[variant_index], diagnostic_span);
int merge_index = 0;
while (merge_index < chunk.size()){
{
collected.push_back(chunk[merge_index]);
merge_index = merge_index + 1;
}
}
variant_index = variant_index + 1;
}
}
return collected;
}

mlc::Array<ast::Diagnostic> derive_clause_diagnostics(mlc::Array<mlc::String> type_parameter_names, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_trait_names, ast::Span diagnostic_span) noexcept{
mlc::Array<ast::Diagnostic> collected = {};
if (derive_trait_names.size() == 0){
{
return collected;
}
}
mlc::HashMap<mlc::String, bool> seen = mlc::HashMap<mlc::String, bool>();
int trait_index = 0;
while (trait_index < derive_trait_names.size()){
{
mlc::String trait_name = derive_trait_names[trait_index];
if (seen.has(trait_name)){
{
collected.push_back(ast::diagnostic_error(mlc::String("duplicate derive trait \"") + trait_name + mlc::String("\""), diagnostic_span));
}
} else {
{
seen.set(trait_name, true);
}
}
if (!derive_trait_name_allowed(trait_name)){
{
collected.push_back(ast::diagnostic_error(mlc::String("unknown derive trait \"") + trait_name + mlc::String("\""), diagnostic_span));
}
}
trait_index = trait_index + 1;
}
}
if (string_list_contains_derive_trait(derive_trait_names, mlc::String("Hash"))){
{
if (type_parameter_names.size() > 0){
collected.push_back(ast::diagnostic_error(mlc::String("derive Hash is not supported for generic types in this version"), diagnostic_span));
} else {
mlc::Array<ast::Diagnostic> hash_field_errors = derive_hash_errors_for_variants(variants, diagnostic_span);
int merge_index = 0;
[&]() { 
  while (merge_index < hash_field_errors.size()){
{
collected.push_back(hash_field_errors[merge_index]);
merge_index = merge_index + 1;
}
}
 }();
}
}
}
return collected;
}

} // namespace derive_validation
