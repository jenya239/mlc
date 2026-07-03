#define main mlc_user_main
#include "derive_validation.hpp"

#include "ast.hpp"
#include "diagnostic_codes.hpp"

namespace derive_validation {

using namespace ast;
using namespace diagnostic_codes;
using namespace ast_tokens;

bool derive_trait_name_allowed(mlc::String trait_name) noexcept{
  return [&]() -> bool {
auto __match_subject = trait_name;
if ((__match_subject == mlc::String("Display", 7))) {
  return true;
} else if ((__match_subject == mlc::String("Eq", 2))) {
  return true;
} else if ((__match_subject == mlc::String("Ord", 3))) {
  return true;
} else if ((__match_subject == mlc::String("Hash", 4))) {
  return true;
} else {
  return false;
}
}();
}
bool string_list_contains_derive_trait(mlc::Array<mlc::String> haystack, mlc::String needle) noexcept{
  return haystack.any([=](mlc::String trait_name) mutable { return (trait_name == needle); });
}
bool type_expr_allowed_for_derive_hash_field(std::shared_ptr<ast::TypeExpr> type_expression) noexcept{
  return [&]() -> bool {
auto __match_subject = type_expression;
if (std::holds_alternative<ast::TyI32>((*__match_subject))) {
const ast::TyI32& tyI32 = std::get<ast::TyI32>((*__match_subject));
return true;
}
if (std::holds_alternative<ast::TyBool>((*__match_subject))) {
const ast::TyBool& tyBool = std::get<ast::TyBool>((*__match_subject));
return true;
}
if (std::holds_alternative<ast::TyString>((*__match_subject))) {
const ast::TyString& tyString = std::get<ast::TyString>((*__match_subject));
return true;
}
return false;
std::abort();
}();
}
mlc::Array<ast::Diagnostic> derive_hash_field_error(mlc::String variant_label, mlc::String field_label, std::shared_ptr<ast::TypeExpr> type_expression, ast::Span diagnostic_span) noexcept{
  if (type_expr_allowed_for_derive_hash_field(type_expression))   {
    return mlc::Array<ast::Diagnostic>{};
  } else   {
    return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(((((mlc::String("derive Hash: unsupported field type for \"", 41) + field_label) + mlc::String("\" in ", 5)) + variant_label) + mlc::String(" (allowed: i32, bool, string)", 29)), diagnostic_span, diagnostic_codes::diagnostic_code_e069())};
  }
}
Derive_hash_tuple_field_fold_state derive_hash_tuple_field_fold_step(Derive_hash_tuple_field_fold_state state, std::shared_ptr<ast::TypeExpr> field_type, mlc::String variant_name, ast::Span diagnostic_span) noexcept{
  auto slot_label = (mlc::String("_", 1) + mlc::to_string(state.field_index));
  auto field_errors = derive_hash_field_error(((mlc::String("variant '", 9) + mlc::to_string(variant_name)) + mlc::String("'", 1)), slot_label, field_type, diagnostic_span);
  return Derive_hash_tuple_field_fold_state{ast::diagnostics_append(state.diagnostics, field_errors), (state.field_index + 1)};
}
mlc::Array<ast::Diagnostic> derive_hash_field_errors_for_variant(std::shared_ptr<ast::TypeVariant> variant, ast::Span diagnostic_span) noexcept{
  return std::visit(overloaded{[&](const ast::VarUnit& varUnit) { auto [__0, __1] = varUnit; return [&]() {
auto diagnostics_none = mlc::Array<ast::Diagnostic>{};
return diagnostics_none;
}(); },
[&](const ast::VarTuple& varTuple) { auto [variant_name, field_types, __2] = varTuple; return field_types.fold(Derive_hash_tuple_field_fold_state{[&]() {
auto empty_diagnostics = mlc::Array<ast::Diagnostic>{};
return empty_diagnostics;
}(), 0}, [=](Derive_hash_tuple_field_fold_state state, std::shared_ptr<ast::TypeExpr> field_type) mutable { return derive_hash_tuple_field_fold_step(state, field_type, variant_name, diagnostic_span); }).diagnostics; },
[&](const ast::VarRecord& varRecord) { auto [variant_name, field_defs, __2] = varRecord; return field_defs.flat_map([=](std::shared_ptr<ast::FieldDef> field_definition) mutable { return derive_hash_field_error(((mlc::String("variant '", 9) + mlc::to_string(variant_name)) + mlc::String("'", 1)), field_definition->name, field_definition->type_value, diagnostic_span); }); }
}, (*variant));
}
mlc::Array<ast::Diagnostic> derive_hash_errors_for_variants(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, ast::Span diagnostic_span) noexcept{
  return variants.flat_map([=](std::shared_ptr<ast::TypeVariant> variant) mutable { return derive_hash_field_errors_for_variant(variant, diagnostic_span); });
}
Derive_clause_trait_scan_fold_state derive_clause_trait_scan_fold_step(Derive_clause_trait_scan_fold_state state, mlc::String trait_name, ast::Span diagnostic_span) noexcept{
  auto diagnostics_after_duplicate = (state.seen_traits.has(trait_name) ? (state.diagnostics.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(((mlc::String("duplicate derive trait \"", 24) + trait_name) + mlc::String("\"", 1)), diagnostic_span, diagnostic_codes::diagnostic_code_e070())})) : (state.diagnostics));
  auto seen_traits = state.seen_traits;
  if ((!state.seen_traits.has(trait_name)))   {
    seen_traits.set(trait_name, true);
  }
  auto diagnostics_after_unknown = ((!derive_trait_name_allowed(trait_name)) ? (diagnostics_after_duplicate.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(((mlc::String("unknown derive trait '", 22) + mlc::to_string(trait_name)) + mlc::String("'", 1)), diagnostic_span, diagnostic_codes::diagnostic_code_e071())})) : (diagnostics_after_duplicate));
  return Derive_clause_trait_scan_fold_state{diagnostics_after_unknown, seen_traits};
}
mlc::Array<ast::Diagnostic> derive_clause_diagnostics_after_trait_scan(mlc::Array<mlc::String> type_parameter_names, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_trait_names, mlc::Array<ast::Diagnostic> trait_scan_diagnostics, ast::Span diagnostic_span) noexcept{
  if (string_list_contains_derive_trait(derive_trait_names, mlc::String("Hash", 4)))   {
    if ((type_parameter_names.length() > 0))     {
      return trait_scan_diagnostics.concat(mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("derive Hash is not supported for generic types in this version", 62), diagnostic_span, diagnostic_codes::diagnostic_code_e072())});
    } else     {
      return ast::diagnostics_append(trait_scan_diagnostics, derive_hash_errors_for_variants(variants, diagnostic_span));
    }
  } else   {
    return trait_scan_diagnostics;
  }
}
mlc::Array<ast::Diagnostic> derive_clause_diagnostics(mlc::Array<mlc::String> type_parameter_names, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_trait_names, ast::Span diagnostic_span) noexcept{
  if ((derive_trait_names.length() == 0))   {
    auto empty_diagnostics = mlc::Array<ast::Diagnostic>{};
    return empty_diagnostics;
  } else   {
    auto trait_scan_result = derive_trait_names.fold(Derive_clause_trait_scan_fold_state{[&]() {
auto empty_diagnostics = mlc::Array<ast::Diagnostic>{};
return empty_diagnostics;
}(), {}}, [=](Derive_clause_trait_scan_fold_state state, mlc::String trait_name) mutable { return derive_clause_trait_scan_fold_step(state, trait_name, diagnostic_span); });
    return derive_clause_diagnostics_after_trait_scan(type_parameter_names, variants, derive_trait_names, trait_scan_result.diagnostics, diagnostic_span);
  }
}

} // namespace derive_validation
