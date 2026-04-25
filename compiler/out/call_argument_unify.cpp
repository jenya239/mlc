#include "call_argument_unify.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"

namespace call_argument_unify {

using namespace ast;
using namespace registry;
using namespace semantic_type_structure;
using namespace ast_tokens;

mlc::Array<ast::Diagnostic> empty_diagnostic_list() noexcept;

bool list_contains_string(mlc::Array<mlc::String> haystack, mlc::String needle) noexcept;

mlc::Array<ast::Diagnostic> unify_expected_and_actual_argument_types(std::shared_ptr<registry::Type> expected_type, std::shared_ptr<registry::Type> actual_type, mlc::Array<mlc::String> type_parameter_names, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>& substitution, ast::Span diagnostic_span) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> build_call_type_substitution(mlc::Array<std::shared_ptr<registry::Type>> expected_parameter_types, mlc::Array<std::shared_ptr<registry::Type>> actual_argument_types, mlc::Array<mlc::String> type_parameter_names) noexcept;

mlc::Array<ast::Diagnostic> call_argument_unification_diagnostics(mlc::Array<std::shared_ptr<registry::Type>> expected_parameter_types, mlc::Array<std::shared_ptr<registry::Type>> actual_argument_types, mlc::Array<mlc::String> type_parameter_names, mlc::Array<std::shared_ptr<ast::Expr>> argument_expressions) noexcept;

mlc::Array<ast::Diagnostic> empty_diagnostic_list() noexcept{
mlc::Array<ast::Diagnostic> empty_list = {};
return empty_list;
}

bool list_contains_string(mlc::Array<mlc::String> haystack, mlc::String needle) noexcept{
int index = 0;
bool found = false;
while (index < haystack.size() && !found){
{
if (haystack[index] == needle){
{
found = true;
}
}
index = index + 1;
}
}
return found;
}

mlc::Array<ast::Diagnostic> unify_expected_and_actual_argument_types(std::shared_ptr<registry::Type> expected_type, std::shared_ptr<registry::Type> actual_type, mlc::Array<mlc::String> type_parameter_names, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>& substitution, ast::Span diagnostic_span) noexcept{
return semantic_type_structure::type_is_unknown(actual_type) ? empty_diagnostic_list() : semantic_type_structure::type_is_unknown(expected_type) ? empty_diagnostic_list() : [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<registry::TArray>((*expected_type))) { auto _v_tarray = std::get<registry::TArray>((*expected_type)); auto [inner_expected] = _v_tarray; return semantic_type_structure::type_is_array(actual_type) ? unify_expected_and_actual_argument_types(inner_expected, semantic_type_structure::array_element_type_from_array_type(actual_type), type_parameter_names, substitution, diagnostic_span) : mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("argument expects ") + semantic_type_structure::type_description(expected_type) + mlc::String(", got ") + semantic_type_structure::type_description(actual_type), diagnostic_span)}; } if (std::holds_alternative<registry::TShared>((*expected_type))) { auto _v_tshared = std::get<registry::TShared>((*expected_type)); auto [inner_expected] = _v_tshared; return semantic_type_structure::type_is_shared_pointer(actual_type) ? unify_expected_and_actual_argument_types(inner_expected, semantic_type_structure::shared_pointer_inner_type(actual_type), type_parameter_names, substitution, diagnostic_span) : mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("argument expects ") + semantic_type_structure::type_description(expected_type) + mlc::String(", got ") + semantic_type_structure::type_description(actual_type), diagnostic_span)}; } if (std::holds_alternative<registry::TNamed>((*expected_type))) { auto _v_tnamed = std::get<registry::TNamed>((*expected_type)); auto [name] = _v_tnamed; return list_contains_string(type_parameter_names, name) ? substitution.has(name) ? unify_expected_and_actual_argument_types(substitution.get(name), actual_type, type_parameter_names, substitution, diagnostic_span) : [&]() -> mlc::Array<ast::Diagnostic> { 
  substitution.set(name, actual_type);
  return empty_diagnostic_list();
 }() : semantic_type_structure::types_structurally_equal(expected_type, actual_type) ? empty_diagnostic_list() : mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("argument expects ") + semantic_type_structure::type_description(expected_type) + mlc::String(", got ") + semantic_type_structure::type_description(actual_type), diagnostic_span)}; } if (std::holds_alternative<registry::TGeneric>((*expected_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*expected_type)); auto [expected_name, expected_args] = _v_tgeneric; return [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<registry::TGeneric>((*actual_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*actual_type)); auto [actual_name, actual_args] = _v_tgeneric; return expected_name != actual_name || expected_args.size() != actual_args.size() ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("argument expects ") + semantic_type_structure::type_description(expected_type) + mlc::String(", got ") + semantic_type_structure::type_description(actual_type), diagnostic_span)} : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> out = empty_diagnostic_list();
  int gi = 0;
  while (gi < expected_args.size()){
{
out = ast::diagnostics_append(out, unify_expected_and_actual_argument_types(expected_args[gi], actual_args[gi], type_parameter_names, substitution, diagnostic_span));
gi = gi + 1;
}
}
  return out;
 }(); } return mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("argument expects ") + semantic_type_structure::type_description(expected_type) + mlc::String(", got ") + semantic_type_structure::type_description(actual_type), diagnostic_span)}; }(); } if (std::holds_alternative<registry::TTuple>((*expected_type))) { auto _v_ttuple = std::get<registry::TTuple>((*expected_type)); auto [expected_elts] = _v_ttuple; return [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<registry::TTuple>((*actual_type))) { auto _v_ttuple = std::get<registry::TTuple>((*actual_type)); auto [actual_elts] = _v_ttuple; return expected_elts.size() != actual_elts.size() ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("argument expects ") + semantic_type_structure::type_description(expected_type) + mlc::String(", got ") + semantic_type_structure::type_description(actual_type), diagnostic_span)} : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> out_t = empty_diagnostic_list();
  int ti = 0;
  while (ti < expected_elts.size()){
{
out_t = ast::diagnostics_append(out_t, unify_expected_and_actual_argument_types(expected_elts[ti], actual_elts[ti], type_parameter_names, substitution, diagnostic_span));
ti = ti + 1;
}
}
  return out_t;
 }(); } if (std::holds_alternative<registry::TPair>((*actual_type))) { auto _v_tpair = std::get<registry::TPair>((*actual_type)); auto [a2, b2] = _v_tpair; return expected_elts.size() != 2 ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("argument expects ") + semantic_type_structure::type_description(expected_type) + mlc::String(", got ") + semantic_type_structure::type_description(actual_type), diagnostic_span)} : ast::diagnostics_append(unify_expected_and_actual_argument_types(expected_elts[0], a2, type_parameter_names, substitution, diagnostic_span), unify_expected_and_actual_argument_types(expected_elts[1], b2, type_parameter_names, substitution, diagnostic_span)); } return mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("argument expects ") + semantic_type_structure::type_description(expected_type) + mlc::String(", got ") + semantic_type_structure::type_description(actual_type), diagnostic_span)}; }(); } if (std::holds_alternative<registry::TPair>((*expected_type))) { auto _v_tpair = std::get<registry::TPair>((*expected_type)); auto [expected_a, expected_b] = _v_tpair; return [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<registry::TPair>((*actual_type))) { auto _v_tpair = std::get<registry::TPair>((*actual_type)); auto [actual_a, actual_b] = _v_tpair; return ast::diagnostics_append(unify_expected_and_actual_argument_types(expected_a, actual_a, type_parameter_names, substitution, diagnostic_span), unify_expected_and_actual_argument_types(expected_b, actual_b, type_parameter_names, substitution, diagnostic_span)); } if (std::holds_alternative<registry::TTuple>((*actual_type))) { auto _v_ttuple = std::get<registry::TTuple>((*actual_type)); auto [ats] = _v_ttuple; return ats.size() != 2 ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("argument expects ") + semantic_type_structure::type_description(expected_type) + mlc::String(", got ") + semantic_type_structure::type_description(actual_type), diagnostic_span)} : ast::diagnostics_append(unify_expected_and_actual_argument_types(expected_a, ats[0], type_parameter_names, substitution, diagnostic_span), unify_expected_and_actual_argument_types(expected_b, ats[1], type_parameter_names, substitution, diagnostic_span)); } return mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("argument expects ") + semantic_type_structure::type_description(expected_type) + mlc::String(", got ") + semantic_type_structure::type_description(actual_type), diagnostic_span)}; }(); } if (std::holds_alternative<registry::TFn>((*expected_type))) { auto _v_tfn = std::get<registry::TFn>((*expected_type)); auto [expected_params, expected_ret] = _v_tfn; return [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<registry::TFn>((*actual_type))) { auto _v_tfn = std::get<registry::TFn>((*actual_type)); auto [actual_params, actual_ret] = _v_tfn; return expected_params.size() != actual_params.size() ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("argument expects ") + semantic_type_structure::type_description(expected_type) + mlc::String(", got ") + semantic_type_structure::type_description(actual_type), diagnostic_span)} : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> out_fn = empty_diagnostic_list();
  int pi = 0;
  while (pi < expected_params.size()){
{
out_fn = ast::diagnostics_append(out_fn, unify_expected_and_actual_argument_types(expected_params[pi], actual_params[pi], type_parameter_names, substitution, diagnostic_span));
pi = pi + 1;
}
}
  return ast::diagnostics_append(out_fn, unify_expected_and_actual_argument_types(expected_ret, actual_ret, type_parameter_names, substitution, diagnostic_span));
 }(); } return mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("argument expects ") + semantic_type_structure::type_description(expected_type) + mlc::String(", got ") + semantic_type_structure::type_description(actual_type), diagnostic_span)}; }(); } return semantic_type_structure::types_structurally_equal(expected_type, actual_type) ? empty_diagnostic_list() : mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("argument expects ") + semantic_type_structure::type_description(expected_type) + mlc::String(", got ") + semantic_type_structure::type_description(actual_type), diagnostic_span)}; }();
}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> build_call_type_substitution(mlc::Array<std::shared_ptr<registry::Type>> expected_parameter_types, mlc::Array<std::shared_ptr<registry::Type>> actual_argument_types, mlc::Array<mlc::String> type_parameter_names) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
int index = 0;
while (index < expected_parameter_types.size() && index < actual_argument_types.size()){
{
unify_expected_and_actual_argument_types(expected_parameter_types[index], actual_argument_types[index], type_parameter_names, substitution, ast::span_unknown());
index = index + 1;
}
}
return substitution;
}

mlc::Array<ast::Diagnostic> call_argument_unification_diagnostics(mlc::Array<std::shared_ptr<registry::Type>> expected_parameter_types, mlc::Array<std::shared_ptr<registry::Type>> actual_argument_types, mlc::Array<mlc::String> type_parameter_names, mlc::Array<std::shared_ptr<ast::Expr>> argument_expressions) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
mlc::Array<ast::Diagnostic> collected = {};
int index = 0;
while (index < expected_parameter_types.size() && index < actual_argument_types.size()){
{
collected = ast::diagnostics_append(collected, unify_expected_and_actual_argument_types(expected_parameter_types[index], actual_argument_types[index], type_parameter_names, substitution, ast::expr_span(argument_expressions[index])));
index = index + 1;
}
}
return collected;
}

} // namespace call_argument_unify
