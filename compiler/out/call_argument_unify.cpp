#include "call_argument_unify.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "type_utils.hpp"

namespace call_argument_unify {

using namespace ast;
using namespace registry;
using namespace type_utils;
using namespace ast_tokens;

mlc::Array<ast::Diagnostic> empty_diagnostic_list() noexcept;

bool list_contains_string(mlc::Array<mlc::String> haystack, mlc::String needle) noexcept;

mlc::Array<ast::Diagnostic> unify_expected_and_actual_argument_types(std::shared_ptr<registry::Type> expected_type, std::shared_ptr<registry::Type> actual_type, mlc::Array<mlc::String> type_parameter_names, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>& substitution, ast::Span diagnostic_span) noexcept;

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
return type_utils::type_is_unknown(actual_type) ? empty_diagnostic_list() : type_utils::type_is_unknown(expected_type) ? empty_diagnostic_list() : [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<registry::TArray>((*expected_type))) { auto _v_tarray = std::get<registry::TArray>((*expected_type)); auto [inner_expected] = _v_tarray; return type_utils::type_is_array(actual_type) ? unify_expected_and_actual_argument_types(inner_expected, type_utils::array_element_type_from_array_type(actual_type), type_parameter_names, substitution, diagnostic_span) : mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("argument expects ") + type_utils::type_description(expected_type) + mlc::String(", got ") + type_utils::type_description(actual_type), diagnostic_span)}; } if (std::holds_alternative<registry::TShared>((*expected_type))) { auto _v_tshared = std::get<registry::TShared>((*expected_type)); auto [inner_expected] = _v_tshared; return type_utils::type_is_shared_pointer(actual_type) ? unify_expected_and_actual_argument_types(inner_expected, type_utils::shared_pointer_inner_type(actual_type), type_parameter_names, substitution, diagnostic_span) : mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("argument expects ") + type_utils::type_description(expected_type) + mlc::String(", got ") + type_utils::type_description(actual_type), diagnostic_span)}; } if (std::holds_alternative<registry::TNamed>((*expected_type))) { auto _v_tnamed = std::get<registry::TNamed>((*expected_type)); auto [name] = _v_tnamed; return list_contains_string(type_parameter_names, name) ? substitution.has(name) ? unify_expected_and_actual_argument_types(substitution.get(name), actual_type, type_parameter_names, substitution, diagnostic_span) : [&]() -> mlc::Array<ast::Diagnostic> { 
  substitution.set(name, actual_type);
  return empty_diagnostic_list();
 }() : type_utils::types_structurally_equal(expected_type, actual_type) ? empty_diagnostic_list() : mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("argument expects ") + type_utils::type_description(expected_type) + mlc::String(", got ") + type_utils::type_description(actual_type), diagnostic_span)}; } return type_utils::types_structurally_equal(expected_type, actual_type) ? empty_diagnostic_list() : mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("argument expects ") + type_utils::type_description(expected_type) + mlc::String(", got ") + type_utils::type_description(actual_type), diagnostic_span)}; }();
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
