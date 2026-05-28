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

ast::Diagnostic argument_type_mismatch_diagnostic(std::shared_ptr<registry::Type> expected_type, std::shared_ptr<registry::Type> actual_type, ast::Span diagnostic_span) noexcept;

void record_type_parameter_source_for_argument(mlc::HashMap<mlc::String, int>& type_parameter_source_argument_index, mlc::Array<mlc::String> type_parameter_names, mlc::String type_name, int argument_index) noexcept;

bool list_contains_string(mlc::Array<mlc::String> haystack, mlc::String needle) noexcept;

mlc::Array<ast::Diagnostic> unify_expected_and_actual_argument_types(std::shared_ptr<registry::Type> expected_type, std::shared_ptr<registry::Type> actual_type, mlc::Array<mlc::String> type_parameter_names, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>& substitution, ast::Span diagnostic_span, mlc::HashMap<mlc::String, int>& type_parameter_source_argument_index, int argument_index) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> build_call_type_substitution(mlc::Array<std::shared_ptr<registry::Type>> expected_parameter_types, mlc::Array<std::shared_ptr<registry::Type>> actual_argument_types, mlc::Array<mlc::String> type_parameter_names, mlc::HashMap<mlc::String, int>& type_parameter_source_argument_index) noexcept;

mlc::Array<ast::Diagnostic> call_argument_unification_diagnostics(mlc::Array<std::shared_ptr<registry::Type>> expected_parameter_types, mlc::Array<std::shared_ptr<registry::Type>> actual_argument_types, mlc::Array<mlc::String> type_parameter_names, mlc::Array<std::shared_ptr<ast::Expr>> argument_expressions) noexcept;

ast::Span type_parameter_source_argument_span(mlc::String type_parameter_name, mlc::HashMap<mlc::String, int> type_parameter_source_argument_index, mlc::Array<std::shared_ptr<ast::Expr>> argument_expressions, ast::Span fallback_span) noexcept;

mlc::Array<ast::Diagnostic> empty_diagnostic_list() noexcept{
mlc::Array<ast::Diagnostic> empty_list = {};
return empty_list;
}

ast::Diagnostic argument_type_mismatch_diagnostic(std::shared_ptr<registry::Type> expected_type, std::shared_ptr<registry::Type> actual_type, ast::Span diagnostic_span) noexcept{return ast::diagnostic_error_with_code(mlc::String("argument expects ") + semantic_type_structure::type_description(expected_type) + mlc::String(", got ") + semantic_type_structure::type_description(actual_type), diagnostic_span, mlc::String("E003"));}

void record_type_parameter_source_for_argument(mlc::HashMap<mlc::String, int>& type_parameter_source_argument_index, mlc::Array<mlc::String> type_parameter_names, mlc::String type_name, int argument_index) noexcept{
if (list_contains_string(type_parameter_names, type_name)){
{
type_parameter_source_argument_index.set(type_name, argument_index);
}
}
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

mlc::Array<ast::Diagnostic> unify_expected_and_actual_argument_types(std::shared_ptr<registry::Type> expected_type, std::shared_ptr<registry::Type> actual_type, mlc::Array<mlc::String> type_parameter_names, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>& substitution, ast::Span diagnostic_span, mlc::HashMap<mlc::String, int>& type_parameter_source_argument_index, int argument_index) noexcept{
return semantic_type_structure::type_is_unknown(actual_type) ? empty_diagnostic_list() : semantic_type_structure::type_is_unknown(expected_type) ? empty_diagnostic_list() : [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<registry::TArray>((*expected_type))) { auto _v_tarray = std::get<registry::TArray>((*expected_type)); auto [inner_expected] = _v_tarray; return semantic_type_structure::type_is_array(actual_type) ? unify_expected_and_actual_argument_types(inner_expected, semantic_type_structure::array_element_type_from_array_type(actual_type), type_parameter_names, substitution, diagnostic_span, type_parameter_source_argument_index, argument_index) : mlc::Array<ast::Diagnostic>{argument_type_mismatch_diagnostic(expected_type, actual_type, diagnostic_span)}; } if (std::holds_alternative<registry::TShared>((*expected_type))) { auto _v_tshared = std::get<registry::TShared>((*expected_type)); auto [inner_expected] = _v_tshared; return semantic_type_structure::type_is_shared_pointer(actual_type) ? unify_expected_and_actual_argument_types(inner_expected, semantic_type_structure::shared_pointer_inner_type(actual_type), type_parameter_names, substitution, diagnostic_span, type_parameter_source_argument_index, argument_index) : mlc::Array<ast::Diagnostic>{argument_type_mismatch_diagnostic(expected_type, actual_type, diagnostic_span)}; } if (std::holds_alternative<registry::TNamed>((*expected_type))) { auto _v_tnamed = std::get<registry::TNamed>((*expected_type)); auto [name] = _v_tnamed; return list_contains_string(type_parameter_names, name) ? substitution.has(name) ? unify_expected_and_actual_argument_types(substitution.get(name), actual_type, type_parameter_names, substitution, diagnostic_span, type_parameter_source_argument_index, argument_index) : [&]() -> mlc::Array<ast::Diagnostic> { 
  substitution.set(name, actual_type);
  record_type_parameter_source_for_argument(type_parameter_source_argument_index, type_parameter_names, name, argument_index);
  return empty_diagnostic_list();
 }() : semantic_type_structure::types_structurally_equal(expected_type, actual_type) ? empty_diagnostic_list() : mlc::Array<ast::Diagnostic>{argument_type_mismatch_diagnostic(expected_type, actual_type, diagnostic_span)}; } if (std::holds_alternative<registry::TGeneric>((*expected_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*expected_type)); auto [expected_name, expected_args] = _v_tgeneric; return [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<registry::TGeneric>((*actual_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*actual_type)); auto [actual_name, actual_args] = _v_tgeneric; return expected_name != actual_name || expected_args.size() != actual_args.size() ? mlc::Array<ast::Diagnostic>{argument_type_mismatch_diagnostic(expected_type, actual_type, diagnostic_span)} : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> out = empty_diagnostic_list();
  int generic_index = 0;
  while (generic_index < expected_args.size()){
{
out = ast::diagnostics_append(out, unify_expected_and_actual_argument_types(expected_args[generic_index], actual_args[generic_index], type_parameter_names, substitution, diagnostic_span, type_parameter_source_argument_index, argument_index));
generic_index = generic_index + 1;
}
}
  return out;
 }(); } return mlc::Array<ast::Diagnostic>{argument_type_mismatch_diagnostic(expected_type, actual_type, diagnostic_span)}; }(); } if (std::holds_alternative<registry::TTuple>((*expected_type))) { auto _v_ttuple = std::get<registry::TTuple>((*expected_type)); auto [expected_elts] = _v_ttuple; return [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<registry::TTuple>((*actual_type))) { auto _v_ttuple = std::get<registry::TTuple>((*actual_type)); auto [actual_elts] = _v_ttuple; return expected_elts.size() != actual_elts.size() ? mlc::Array<ast::Diagnostic>{argument_type_mismatch_diagnostic(expected_type, actual_type, diagnostic_span)} : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> tuple_output = empty_diagnostic_list();
  int tuple_index = 0;
  while (tuple_index < expected_elts.size()){
{
tuple_output = ast::diagnostics_append(tuple_output, unify_expected_and_actual_argument_types(expected_elts[tuple_index], actual_elts[tuple_index], type_parameter_names, substitution, diagnostic_span, type_parameter_source_argument_index, argument_index));
tuple_index = tuple_index + 1;
}
}
  return tuple_output;
 }(); } if (std::holds_alternative<registry::TPair>((*actual_type))) { auto _v_tpair = std::get<registry::TPair>((*actual_type)); auto [a2, b2] = _v_tpair; return expected_elts.size() != 2 ? mlc::Array<ast::Diagnostic>{argument_type_mismatch_diagnostic(expected_type, actual_type, diagnostic_span)} : ast::diagnostics_append(unify_expected_and_actual_argument_types(expected_elts[0], a2, type_parameter_names, substitution, diagnostic_span, type_parameter_source_argument_index, argument_index), unify_expected_and_actual_argument_types(expected_elts[1], b2, type_parameter_names, substitution, diagnostic_span, type_parameter_source_argument_index, argument_index)); } return mlc::Array<ast::Diagnostic>{argument_type_mismatch_diagnostic(expected_type, actual_type, diagnostic_span)}; }(); } if (std::holds_alternative<registry::TPair>((*expected_type))) { auto _v_tpair = std::get<registry::TPair>((*expected_type)); auto [expected_a, expected_b] = _v_tpair; return [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<registry::TPair>((*actual_type))) { auto _v_tpair = std::get<registry::TPair>((*actual_type)); auto [actual_a, actual_b] = _v_tpair; return ast::diagnostics_append(unify_expected_and_actual_argument_types(expected_a, actual_a, type_parameter_names, substitution, diagnostic_span, type_parameter_source_argument_index, argument_index), unify_expected_and_actual_argument_types(expected_b, actual_b, type_parameter_names, substitution, diagnostic_span, type_parameter_source_argument_index, argument_index)); } if (std::holds_alternative<registry::TTuple>((*actual_type))) { auto _v_ttuple = std::get<registry::TTuple>((*actual_type)); auto [actual_elts] = _v_ttuple; return actual_elts.size() != 2 ? mlc::Array<ast::Diagnostic>{argument_type_mismatch_diagnostic(expected_type, actual_type, diagnostic_span)} : ast::diagnostics_append(unify_expected_and_actual_argument_types(expected_a, actual_elts[0], type_parameter_names, substitution, diagnostic_span, type_parameter_source_argument_index, argument_index), unify_expected_and_actual_argument_types(expected_b, actual_elts[1], type_parameter_names, substitution, diagnostic_span, type_parameter_source_argument_index, argument_index)); } return mlc::Array<ast::Diagnostic>{argument_type_mismatch_diagnostic(expected_type, actual_type, diagnostic_span)}; }(); } if (std::holds_alternative<registry::TFn>((*expected_type))) { auto _v_tfn = std::get<registry::TFn>((*expected_type)); auto [expected_params, expected_ret] = _v_tfn; return [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<registry::TFn>((*actual_type))) { auto _v_tfn = std::get<registry::TFn>((*actual_type)); auto [actual_params, actual_ret] = _v_tfn; return expected_params.size() != actual_params.size() ? mlc::Array<ast::Diagnostic>{argument_type_mismatch_diagnostic(expected_type, actual_type, diagnostic_span)} : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> function_output = empty_diagnostic_list();
  int parameter_index = 0;
  while (parameter_index < expected_params.size()){
{
function_output = ast::diagnostics_append(function_output, unify_expected_and_actual_argument_types(expected_params[parameter_index], actual_params[parameter_index], type_parameter_names, substitution, diagnostic_span, type_parameter_source_argument_index, argument_index));
parameter_index = parameter_index + 1;
}
}
  return ast::diagnostics_append(function_output, unify_expected_and_actual_argument_types(expected_ret, actual_ret, type_parameter_names, substitution, diagnostic_span, type_parameter_source_argument_index, argument_index));
 }(); } return mlc::Array<ast::Diagnostic>{argument_type_mismatch_diagnostic(expected_type, actual_type, diagnostic_span)}; }(); } return semantic_type_structure::types_structurally_equal(expected_type, actual_type) ? empty_diagnostic_list() : mlc::Array<ast::Diagnostic>{argument_type_mismatch_diagnostic(expected_type, actual_type, diagnostic_span)}; }();
}

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> build_call_type_substitution(mlc::Array<std::shared_ptr<registry::Type>> expected_parameter_types, mlc::Array<std::shared_ptr<registry::Type>> actual_argument_types, mlc::Array<mlc::String> type_parameter_names, mlc::HashMap<mlc::String, int>& type_parameter_source_argument_index) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
int index = 0;
while (index < expected_parameter_types.size() && index < actual_argument_types.size()){
{
unify_expected_and_actual_argument_types(expected_parameter_types[index], actual_argument_types[index], type_parameter_names, substitution, ast::span_unknown(), type_parameter_source_argument_index, index);
index = index + 1;
}
}
return substitution;
}

mlc::Array<ast::Diagnostic> call_argument_unification_diagnostics(mlc::Array<std::shared_ptr<registry::Type>> expected_parameter_types, mlc::Array<std::shared_ptr<registry::Type>> actual_argument_types, mlc::Array<mlc::String> type_parameter_names, mlc::Array<std::shared_ptr<ast::Expr>> argument_expressions) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
mlc::HashMap<mlc::String, int> type_parameter_source_argument_index = mlc::HashMap<mlc::String, int>();
mlc::Array<ast::Diagnostic> collected = {};
int index = 0;
while (index < expected_parameter_types.size() && index < actual_argument_types.size()){
{
collected = ast::diagnostics_append(collected, unify_expected_and_actual_argument_types(expected_parameter_types[index], actual_argument_types[index], type_parameter_names, substitution, ast::expr_span(argument_expressions[index]), type_parameter_source_argument_index, index));
index = index + 1;
}
}
return collected;
}

ast::Span type_parameter_source_argument_span(mlc::String type_parameter_name, mlc::HashMap<mlc::String, int> type_parameter_source_argument_index, mlc::Array<std::shared_ptr<ast::Expr>> argument_expressions, ast::Span fallback_span) noexcept{return type_parameter_source_argument_index.has(type_parameter_name) ? [&]() -> ast::Span { 
  int argument_index = type_parameter_source_argument_index.get(type_parameter_name);
  return argument_index >= 0 && argument_index < argument_expressions.size() ? ast::expr_span(argument_expressions[argument_index]) : fallback_span;
 }() : fallback_span;}

} // namespace call_argument_unify
