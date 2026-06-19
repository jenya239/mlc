#include "derive_validation.hpp"

#include "ast.hpp"
#include "diagnostic_codes.hpp"

namespace derive_validation {

using namespace ast;
using namespace diagnostic_codes;
using namespace ast_tokens;

struct Derive_hash_tuple_field_fold_state {mlc::Array<ast::Diagnostic> diagnostics;int field_index;};

struct Derive_clause_trait_scan_fold_state {mlc::Array<ast::Diagnostic> diagnostics;mlc::HashMap<mlc::String, bool> seen_traits;};

bool derive_trait_name_allowed(mlc::String trait_name) noexcept;

bool string_list_contains_derive_trait(mlc::Array<mlc::String> haystack, mlc::String needle) noexcept;

bool type_expr_allowed_for_derive_hash_field(std::shared_ptr<ast::TypeExpr> type_expression) noexcept;

mlc::Array<ast::Diagnostic> derive_hash_field_error(mlc::String variant_label, mlc::String field_label, std::shared_ptr<ast::TypeExpr> type_expression, ast::Span diagnostic_span) noexcept;

derive_validation::Derive_hash_tuple_field_fold_state derive_hash_tuple_field_fold_step(derive_validation::Derive_hash_tuple_field_fold_state state, std::shared_ptr<ast::TypeExpr> field_type, mlc::String variant_name, ast::Span diagnostic_span) noexcept;

mlc::Array<ast::Diagnostic> derive_hash_field_errors_for_variant(std::shared_ptr<ast::TypeVariant> variant, ast::Span diagnostic_span) noexcept;

mlc::Array<ast::Diagnostic> derive_hash_errors_for_variants(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, ast::Span diagnostic_span) noexcept;

derive_validation::Derive_clause_trait_scan_fold_state derive_clause_trait_scan_fold_step(derive_validation::Derive_clause_trait_scan_fold_state state, mlc::String trait_name, ast::Span diagnostic_span) noexcept;

mlc::Array<ast::Diagnostic> derive_clause_diagnostics_after_trait_scan(mlc::Array<mlc::String> type_parameter_names, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_trait_names, mlc::Array<ast::Diagnostic> trait_scan_diagnostics, ast::Span diagnostic_span) noexcept;

mlc::Array<ast::Diagnostic> derive_clause_diagnostics(mlc::Array<mlc::String> type_parameter_names, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_trait_names, ast::Span diagnostic_span) noexcept;

bool derive_trait_name_allowed(mlc::String trait_name) noexcept{return [&]() { if (trait_name == mlc::String("Display") || trait_name == mlc::String("Eq") || trait_name == mlc::String("Ord") || trait_name == mlc::String("Hash")) { return true; } return false; }();}

bool string_list_contains_derive_trait(mlc::Array<mlc::String> haystack, mlc::String needle) noexcept{return haystack.any([needle](mlc::String trait_name) mutable { return trait_name == needle; });}

bool type_expr_allowed_for_derive_hash_field(std::shared_ptr<ast::TypeExpr> type_expression) noexcept{return [&]() { if (std::holds_alternative<ast::TyI32>((*type_expression)) || std::holds_alternative<ast::TyBool>((*type_expression)) || std::holds_alternative<ast::TyString>((*type_expression))) { return true; } return false; }();}

mlc::Array<ast::Diagnostic> derive_hash_field_error(mlc::String variant_label, mlc::String field_label, std::shared_ptr<ast::TypeExpr> type_expression, ast::Span diagnostic_span) noexcept{
return type_expr_allowed_for_derive_hash_field(type_expression) ? mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("derive Hash: unsupported field type for \"") + field_label + mlc::String("\" in ") + variant_label + mlc::String(" (allowed: i32, bool, string)"), diagnostic_span, diagnostic_codes::diagnostic_code_e069())};
}

derive_validation::Derive_hash_tuple_field_fold_state derive_hash_tuple_field_fold_step(derive_validation::Derive_hash_tuple_field_fold_state state, std::shared_ptr<ast::TypeExpr> field_type, mlc::String variant_name, ast::Span diagnostic_span) noexcept{
mlc::String slot_label = mlc::String("_") + mlc::to_string(state.field_index);
mlc::Array<ast::Diagnostic> field_errors = derive_hash_field_error(mlc::String("variant '") + variant_name + mlc::String("'"), slot_label, field_type, diagnostic_span);
return derive_validation::Derive_hash_tuple_field_fold_state{ast::diagnostics_append(state.diagnostics, field_errors), state.field_index + 1};
}

mlc::Array<ast::Diagnostic> derive_hash_field_errors_for_variant(std::shared_ptr<ast::TypeVariant> variant, ast::Span diagnostic_span) noexcept{return std::visit(overloaded{
  [&](const VarUnit& varunit) -> mlc::Array<ast::Diagnostic> { auto [_w0, _w1] = varunit; return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> diagnostics_none = {};
  return diagnostics_none;
 }(); },
  [&](const VarTuple& vartuple) -> mlc::Array<ast::Diagnostic> { auto [variant_name, field_types, _w0] = vartuple; return field_types.fold(derive_validation::Derive_hash_tuple_field_fold_state{[&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty_diagnostics = {};
  return empty_diagnostics;
 }(), 0}, [variant_name, diagnostic_span](derive_validation::Derive_hash_tuple_field_fold_state state, std::shared_ptr<ast::TypeExpr> field_type) mutable { return derive_hash_tuple_field_fold_step(state, field_type, variant_name, diagnostic_span); }).diagnostics; },
  [&](const VarRecord& varrecord) -> mlc::Array<ast::Diagnostic> { auto [variant_name, field_defs, _w0] = varrecord; return mlc::collections::flat_map(field_defs, [variant_name, diagnostic_span](std::shared_ptr<ast::FieldDef> field_definition) mutable { return derive_hash_field_error(mlc::String("variant '") + variant_name + mlc::String("'"), field_definition->name, field_definition->type_value, diagnostic_span); }); }
}, (*variant));}

mlc::Array<ast::Diagnostic> derive_hash_errors_for_variants(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, ast::Span diagnostic_span) noexcept{return mlc::collections::flat_map(variants, [diagnostic_span](std::shared_ptr<ast::TypeVariant> variant) mutable { return derive_hash_field_errors_for_variant(variant, diagnostic_span); });}

derive_validation::Derive_clause_trait_scan_fold_state derive_clause_trait_scan_fold_step(derive_validation::Derive_clause_trait_scan_fold_state state, mlc::String trait_name, ast::Span diagnostic_span) noexcept{
mlc::Array<ast::Diagnostic> diagnostics_after_duplicate = state.seen_traits.has(trait_name) ? state.diagnostics.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("duplicate derive trait \"") + trait_name + mlc::String("\""), diagnostic_span, diagnostic_codes::diagnostic_code_e070())}) : state.diagnostics;
mlc::HashMap<mlc::String, bool> seen_traits = state.seen_traits;
if (!state.seen_traits.has(trait_name)){
{
seen_traits.set(trait_name, true);
}
}
mlc::Array<ast::Diagnostic> diagnostics_after_unknown = !derive_trait_name_allowed(trait_name) ? diagnostics_after_duplicate.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("unknown derive trait '") + trait_name + mlc::String("'"), diagnostic_span, diagnostic_codes::diagnostic_code_e071())}) : diagnostics_after_duplicate;
return derive_validation::Derive_clause_trait_scan_fold_state{diagnostics_after_unknown, seen_traits};
}

mlc::Array<ast::Diagnostic> derive_clause_diagnostics_after_trait_scan(mlc::Array<mlc::String> type_parameter_names, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_trait_names, mlc::Array<ast::Diagnostic> trait_scan_diagnostics, ast::Span diagnostic_span) noexcept{return string_list_contains_derive_trait(derive_trait_names, mlc::String("Hash")) ? type_parameter_names.size() > 0 ? trait_scan_diagnostics.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("derive Hash is not supported for generic types in this version"), diagnostic_span, diagnostic_codes::diagnostic_code_e072())}) : ast::diagnostics_append(trait_scan_diagnostics, derive_hash_errors_for_variants(variants, diagnostic_span)) : trait_scan_diagnostics;}

mlc::Array<ast::Diagnostic> derive_clause_diagnostics(mlc::Array<mlc::String> type_parameter_names, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_trait_names, ast::Span diagnostic_span) noexcept{return derive_trait_names.size() == 0 ? [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty_diagnostics = {};
  return empty_diagnostics;
 }() : [&]() -> mlc::Array<ast::Diagnostic> { 
  derive_validation::Derive_clause_trait_scan_fold_state trait_scan_result = derive_trait_names.fold(derive_validation::Derive_clause_trait_scan_fold_state{[&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty_diagnostics = {};
  return empty_diagnostics;
 }(), mlc::HashMap<mlc::String, bool>()}, [diagnostic_span](derive_validation::Derive_clause_trait_scan_fold_state state, mlc::String trait_name) mutable { return derive_clause_trait_scan_fold_step(state, trait_name, diagnostic_span); });
  return derive_clause_diagnostics_after_trait_scan(type_parameter_names, variants, derive_trait_names, trait_scan_result.diagnostics, diagnostic_span);
 }();}

} // namespace derive_validation
