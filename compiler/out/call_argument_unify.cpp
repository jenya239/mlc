#define main mlc_user_main
#include "call_argument_unify.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"
#include "diagnostic_codes.hpp"

namespace call_argument_unify {

using namespace ast;
using namespace registry;
using namespace semantic_type_structure;
using namespace diagnostic_codes;
using namespace ast_tokens;

mlc::Array<ast::Diagnostic> empty_diagnostic_list() noexcept{
  auto empty_list = mlc::Array<ast::Diagnostic>{};
  return empty_list;
}
ast::Diagnostic argument_type_mismatch_diagnostic(std::shared_ptr<registry::Type> expected_type, std::shared_ptr<registry::Type> actual_type, ast::Span diagnostic_span) noexcept{
  return ast::diagnostic_error_with_code((((mlc::String("argument expects ", 17) + semantic_type_structure::type_description(expected_type)) + mlc::String(", got ", 6)) + semantic_type_structure::type_description(actual_type)), diagnostic_span, diagnostic_codes::diagnostic_code_e003());
}
void record_type_parameter_source_for_argument(mlc::HashMap<mlc::String, int>& type_parameter_source_argument_index, mlc::Array<mlc::String> type_parameter_names, mlc::String type_name, int argument_index) noexcept{
  if (list_contains_string(type_parameter_names, type_name))   {
    return type_parameter_source_argument_index.set(type_name, argument_index);
  }
}
bool list_contains_string(mlc::Array<mlc::String> haystack, mlc::String needle) noexcept{
  return haystack.any([=](mlc::String haystack_entry) mutable { return (haystack_entry == needle); });
}
mlc::Array<ast::Diagnostic> unify_expected_and_actual_argument_types(std::shared_ptr<registry::Type> expected_type, std::shared_ptr<registry::Type> actual_type, mlc::Array<mlc::String> type_parameter_names, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>& substitution, ast::Span diagnostic_span, mlc::HashMap<mlc::String, int>& type_parameter_source_argument_index, int argument_index) noexcept{
  if (semantic_type_structure::type_is_unknown(actual_type))   {
    return empty_diagnostic_list();
  } else if (semantic_type_structure::type_is_unknown(expected_type))   {
    return empty_diagnostic_list();
  } else   {
    return [&]() -> mlc::Array<ast::Diagnostic> {
auto __match_subject = expected_type;
if (std::holds_alternative<registry::TArray>((*__match_subject))) {
const registry::TArray& tArray = std::get<registry::TArray>((*__match_subject));
auto [inner_expected] = tArray; return (semantic_type_structure::type_is_array(actual_type) ? (unify_expected_and_actual_argument_types(inner_expected, semantic_type_structure::array_element_type_from_array_type(actual_type), type_parameter_names, substitution, diagnostic_span, type_parameter_source_argument_index, argument_index)) : (mlc::Array<ast::Diagnostic>{argument_type_mismatch_diagnostic(expected_type, actual_type, diagnostic_span)}));
}
if (std::holds_alternative<registry::TShared>((*__match_subject))) {
const registry::TShared& tShared = std::get<registry::TShared>((*__match_subject));
auto [inner_expected] = tShared; return (semantic_type_structure::type_is_shared_pointer(actual_type) ? (unify_expected_and_actual_argument_types(inner_expected, semantic_type_structure::shared_pointer_inner_type(actual_type), type_parameter_names, substitution, diagnostic_span, type_parameter_source_argument_index, argument_index)) : (mlc::Array<ast::Diagnostic>{argument_type_mismatch_diagnostic(expected_type, actual_type, diagnostic_span)}));
}
if (std::holds_alternative<registry::TNamed>((*__match_subject))) {
const registry::TNamed& tNamed = std::get<registry::TNamed>((*__match_subject));
auto [name] = tNamed; return (list_contains_string(type_parameter_names, name) ? ([&]() -> mlc::Array<ast::Diagnostic> {
  if (substitution.has(name))   {
    return unify_expected_and_actual_argument_types(substitution.get(name), actual_type, type_parameter_names, substitution, diagnostic_span, type_parameter_source_argument_index, argument_index);
  } else   {
    return [&]() {
substitution.set(name, actual_type);
record_type_parameter_source_for_argument(type_parameter_source_argument_index, type_parameter_names, name, argument_index);
return empty_diagnostic_list();
}();
  }
}()) : ((semantic_type_structure::types_compatible_for_call_argument(expected_type, actual_type) ? (empty_diagnostic_list()) : ((semantic_type_structure::types_structurally_equal(expected_type, actual_type) ? (empty_diagnostic_list()) : (mlc::Array<ast::Diagnostic>{argument_type_mismatch_diagnostic(expected_type, actual_type, diagnostic_span)}))))));
}
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [expected_name, expected_type_arguments] = tGeneric; return [&]() -> mlc::Array<ast::Diagnostic> {
auto __match_subject = actual_type;
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [actual_name, actual_type_arguments] = tGeneric; return [&]() -> mlc::Array<ast::Diagnostic> {
  if (((expected_name != actual_name) || (expected_type_arguments.length() != actual_type_arguments.length())))   {
    return mlc::Array<ast::Diagnostic>{argument_type_mismatch_diagnostic(expected_type, actual_type, diagnostic_span)};
  } else   {
    return [&]() {
auto generic_unification_diagnostics = empty_diagnostic_list();
auto generic_index = 0;
while ((generic_index < expected_type_arguments.length())) {
  (generic_unification_diagnostics = ast::diagnostics_append(generic_unification_diagnostics, unify_expected_and_actual_argument_types(expected_type_arguments[generic_index], actual_type_arguments[generic_index], type_parameter_names, substitution, diagnostic_span, type_parameter_source_argument_index, argument_index)));
  (generic_index = (generic_index + 1));
}
return generic_unification_diagnostics;
}();
  }
}();
}
return mlc::Array<ast::Diagnostic>{argument_type_mismatch_diagnostic(expected_type, actual_type, diagnostic_span)};
std::abort();
}();
}
if (std::holds_alternative<registry::TTuple>((*__match_subject))) {
const registry::TTuple& tTuple = std::get<registry::TTuple>((*__match_subject));
auto [expected_elements] = tTuple; return [&]() -> mlc::Array<ast::Diagnostic> {
auto __match_subject = actual_type;
if (std::holds_alternative<registry::TTuple>((*__match_subject))) {
const registry::TTuple& tTuple = std::get<registry::TTuple>((*__match_subject));
auto [actual_elements] = tTuple; return [&]() -> mlc::Array<ast::Diagnostic> {
  if ((expected_elements.length() != actual_elements.length()))   {
    return mlc::Array<ast::Diagnostic>{argument_type_mismatch_diagnostic(expected_type, actual_type, diagnostic_span)};
  } else   {
    return [&]() {
auto tuple_output = empty_diagnostic_list();
auto tuple_index = 0;
while ((tuple_index < expected_elements.length())) {
  (tuple_output = ast::diagnostics_append(tuple_output, unify_expected_and_actual_argument_types(expected_elements[tuple_index], actual_elements[tuple_index], type_parameter_names, substitution, diagnostic_span, type_parameter_source_argument_index, argument_index)));
  (tuple_index = (tuple_index + 1));
}
return tuple_output;
}();
  }
}();
}
if (std::holds_alternative<registry::TPair>((*__match_subject))) {
const registry::TPair& tPair = std::get<registry::TPair>((*__match_subject));
auto [actual_pair_a, actual_pair_b] = tPair; return ((expected_elements.length() != 2) ? (mlc::Array<ast::Diagnostic>{argument_type_mismatch_diagnostic(expected_type, actual_type, diagnostic_span)}) : (ast::diagnostics_append(unify_expected_and_actual_argument_types(expected_elements[0], actual_pair_a, type_parameter_names, substitution, diagnostic_span, type_parameter_source_argument_index, argument_index), unify_expected_and_actual_argument_types(expected_elements[1], actual_pair_b, type_parameter_names, substitution, diagnostic_span, type_parameter_source_argument_index, argument_index))));
}
return mlc::Array<ast::Diagnostic>{argument_type_mismatch_diagnostic(expected_type, actual_type, diagnostic_span)};
std::abort();
}();
}
if (std::holds_alternative<registry::TPair>((*__match_subject))) {
const registry::TPair& tPair = std::get<registry::TPair>((*__match_subject));
auto [expected_a, expected_b] = tPair; return [&]() -> mlc::Array<ast::Diagnostic> {
auto __match_subject = actual_type;
if (std::holds_alternative<registry::TPair>((*__match_subject))) {
const registry::TPair& tPair = std::get<registry::TPair>((*__match_subject));
auto [actual_a, actual_b] = tPair; return ast::diagnostics_append(unify_expected_and_actual_argument_types(expected_a, actual_a, type_parameter_names, substitution, diagnostic_span, type_parameter_source_argument_index, argument_index), unify_expected_and_actual_argument_types(expected_b, actual_b, type_parameter_names, substitution, diagnostic_span, type_parameter_source_argument_index, argument_index));
}
if (std::holds_alternative<registry::TTuple>((*__match_subject))) {
const registry::TTuple& tTuple = std::get<registry::TTuple>((*__match_subject));
auto [actual_elements] = tTuple; return ((actual_elements.length() != 2) ? (mlc::Array<ast::Diagnostic>{argument_type_mismatch_diagnostic(expected_type, actual_type, diagnostic_span)}) : (ast::diagnostics_append(unify_expected_and_actual_argument_types(expected_a, actual_elements[0], type_parameter_names, substitution, diagnostic_span, type_parameter_source_argument_index, argument_index), unify_expected_and_actual_argument_types(expected_b, actual_elements[1], type_parameter_names, substitution, diagnostic_span, type_parameter_source_argument_index, argument_index))));
}
return mlc::Array<ast::Diagnostic>{argument_type_mismatch_diagnostic(expected_type, actual_type, diagnostic_span)};
std::abort();
}();
}
if (std::holds_alternative<registry::TFn>((*__match_subject))) {
const registry::TFn& tFn = std::get<registry::TFn>((*__match_subject));
auto [expected_params, expected_ret] = tFn; return [&]() -> mlc::Array<ast::Diagnostic> {
auto __match_subject = actual_type;
if (std::holds_alternative<registry::TFn>((*__match_subject))) {
const registry::TFn& tFn = std::get<registry::TFn>((*__match_subject));
auto [actual_params, actual_ret] = tFn; return [&]() -> mlc::Array<ast::Diagnostic> {
  if ((expected_params.length() != actual_params.length()))   {
    return mlc::Array<ast::Diagnostic>{argument_type_mismatch_diagnostic(expected_type, actual_type, diagnostic_span)};
  } else   {
    return [&]() {
auto function_output = empty_diagnostic_list();
auto parameter_index = 0;
while ((parameter_index < expected_params.length())) {
  (function_output = ast::diagnostics_append(function_output, unify_expected_and_actual_argument_types(expected_params[parameter_index], actual_params[parameter_index], type_parameter_names, substitution, diagnostic_span, type_parameter_source_argument_index, argument_index)));
  (parameter_index = (parameter_index + 1));
}
return ast::diagnostics_append(function_output, unify_expected_and_actual_argument_types(expected_ret, actual_ret, type_parameter_names, substitution, diagnostic_span, type_parameter_source_argument_index, argument_index));
}();
  }
}();
}
return mlc::Array<ast::Diagnostic>{argument_type_mismatch_diagnostic(expected_type, actual_type, diagnostic_span)};
std::abort();
}();
}
return (semantic_type_structure::types_compatible_for_call_argument(expected_type, actual_type) ? (empty_diagnostic_list()) : ((semantic_type_structure::types_structurally_equal(expected_type, actual_type) ? (empty_diagnostic_list()) : (mlc::Array<ast::Diagnostic>{argument_type_mismatch_diagnostic(expected_type, actual_type, diagnostic_span)}))));
std::abort();
}();
  }
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> build_call_type_substitution(mlc::Array<std::shared_ptr<registry::Type>> expected_parameter_types, mlc::Array<std::shared_ptr<registry::Type>> actual_argument_types, mlc::Array<mlc::String> type_parameter_names, mlc::HashMap<mlc::String, int>& type_parameter_source_argument_index, mlc::Array<std::shared_ptr<ast::Expr>> argument_expressions, ast::Span fallback_span) noexcept{
  auto substitution = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
  auto index = 0;
  while (((index < expected_parameter_types.length()) && (index < actual_argument_types.length())))   {
    auto diagnostic_span = ((index < argument_expressions.length()) ? (ast::expr_span(argument_expressions[index])) : (fallback_span));
    unify_expected_and_actual_argument_types(expected_parameter_types[index], actual_argument_types[index], type_parameter_names, substitution, diagnostic_span, type_parameter_source_argument_index, index);
    (index = (index + 1));
  }
  return substitution;
}
mlc::Array<ast::Diagnostic> call_argument_unification_diagnostics(mlc::Array<std::shared_ptr<registry::Type>> expected_parameter_types, mlc::Array<std::shared_ptr<registry::Type>> actual_argument_types, mlc::Array<mlc::String> type_parameter_names, mlc::Array<std::shared_ptr<ast::Expr>> argument_expressions) noexcept{
  auto substitution = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
  auto type_parameter_source_argument_index = mlc::HashMap<mlc::String, int>();
  auto collected = mlc::Array<ast::Diagnostic>{};
  auto index = 0;
  while (((index < expected_parameter_types.length()) && (index < actual_argument_types.length())))   {
    (collected = ast::diagnostics_append(collected, unify_expected_and_actual_argument_types(expected_parameter_types[index], actual_argument_types[index], type_parameter_names, substitution, ast::expr_span(argument_expressions[index]), type_parameter_source_argument_index, index)));
    (index = (index + 1));
  }
  return collected;
}
ast::Span type_parameter_source_argument_span(mlc::String type_parameter_name, mlc::HashMap<mlc::String, int> type_parameter_source_argument_index, mlc::Array<std::shared_ptr<ast::Expr>> argument_expressions, ast::Span fallback_span) noexcept{
  if (type_parameter_source_argument_index.has(type_parameter_name))   {
    auto argument_index = type_parameter_source_argument_index.get(type_parameter_name);
    if (((argument_index >= 0) && (argument_index < argument_expressions.length())))     {
      return ast::expr_span(argument_expressions[argument_index]);
    } else     {
      return fallback_span;
    }
  } else   {
    return fallback_span;
  }
}

} // namespace call_argument_unify
