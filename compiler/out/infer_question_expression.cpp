#define main mlc_user_main
#include "infer_question_expression.hpp"

#include "ast.hpp"
#include "infer_result.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"
#include "diagnostic_codes.hpp"
#include "result_option_method_types.hpp"

namespace infer_question_expression {

using namespace ast;
using namespace infer_result;
using namespace registry;
using namespace semantic_type_structure;
using namespace diagnostic_codes;
using namespace result_option_method_types;
using namespace ast_tokens;

std::shared_ptr<registry::Type> ok_type_from_type_arguments(mlc::Array<std::shared_ptr<registry::Type>> type_arguments) noexcept{
  if ((type_arguments.length() > 0))   {
    return type_arguments[0];
  } else   {
    return std::make_shared<registry::Type>(registry::TUnknown{});
  }
}
infer_result::InferResult infer_result_for_non_result_type(infer_result::InferResult inner_parsed, ast::Span question_span) noexcept{
  auto error_diagnostics = mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code((mlc::String("? operator requires a Result type, got ", 39) + semantic_type_structure::type_description(inner_parsed.inferred_type)), question_span, diagnostic_codes::diagnostic_code_e068())};
  return infer_result::InferResult{std::make_shared<registry::Type>(registry::TUnknown{}), ast::diagnostics_append(inner_parsed.errors, error_diagnostics)};
}
mlc::String named_or_described_type_name(std::shared_ptr<registry::Type> type_value) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = type_value;
if (std::holds_alternative<registry::TNamed>((*__match_subject))) {
const registry::TNamed& tNamed = std::get<registry::TNamed>((*__match_subject));
auto [name] = tNamed; return name;
}
return semantic_type_structure::type_description(type_value);
std::abort();
}();
}
mlc::String from_trait_name_for_error_type(std::shared_ptr<registry::Type> inner_error_type) noexcept{
  return ((mlc::String("From<", 5) + named_or_described_type_name(inner_error_type)) + mlc::String(">", 1));
}
bool expected_error_implements_from(registry::TypeRegistry registry, std::shared_ptr<registry::Type> expected_error_type, std::shared_ptr<registry::Type> inner_error_type) noexcept{
  return registry::TypeRegistry_type_implements_trait(registry, named_or_described_type_name(expected_error_type), from_trait_name_for_error_type(inner_error_type));
}
mlc::Array<ast::Diagnostic> empty_diagnostic_list() noexcept{
  auto empty = mlc::Array<ast::Diagnostic>{};
  return empty;
}
mlc::Array<ast::Diagnostic> question_error_type_mismatch_diagnostics(registry::TypeRegistry registry, std::shared_ptr<registry::Type> inner_result_type, std::shared_ptr<registry::Type> expected_return_type, ast::Span question_span) noexcept{
  if ((!result_option_method_types::is_result_generic(inner_result_type)))   {
    return empty_diagnostic_list();
  } else if ((!result_option_method_types::is_result_generic(expected_return_type)))   {
    return empty_diagnostic_list();
  } else if ((semantic_type_structure::type_is_unknown(result_option_method_types::result_err_type(inner_result_type)) || semantic_type_structure::type_is_unknown(result_option_method_types::result_err_type(expected_return_type))))   {
    return empty_diagnostic_list();
  } else if (semantic_type_structure::types_structurally_equal(result_option_method_types::result_err_type(inner_result_type), result_option_method_types::result_err_type(expected_return_type)))   {
    return empty_diagnostic_list();
  } else if (expected_error_implements_from(registry, result_option_method_types::result_err_type(expected_return_type), result_option_method_types::result_err_type(inner_result_type)))   {
    return empty_diagnostic_list();
  } else   {
    return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code((((mlc::String("? operator error type mismatch: expected ", 41) + semantic_type_structure::type_description(result_option_method_types::result_err_type(expected_return_type))) + mlc::String(", got ", 6)) + semantic_type_structure::type_description(result_option_method_types::result_err_type(inner_result_type))), question_span, diagnostic_codes::diagnostic_code_e085())};
  }
}
infer_result::InferResult infer_question_from_inner_result(infer_result::InferResult inner_parsed, ast::Span question_span, std::shared_ptr<registry::Type> expected_return_type, registry::TypeRegistry registry) noexcept{
  return [&]() -> infer_result::InferResult {
auto __match_subject = inner_parsed.inferred_type;
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [generic_name, type_arguments] = tGeneric; return [&]() {
if ((generic_name != mlc::String("Result", 6))) {
  return infer_result_for_non_result_type(inner_parsed, question_span);
}
auto mismatch_diagnostics = question_error_type_mismatch_diagnostics(registry, inner_parsed.inferred_type, expected_return_type, question_span);
return infer_result::InferResult{ok_type_from_type_arguments(type_arguments), ast::diagnostics_append(inner_parsed.errors, mismatch_diagnostics)};
}();
}
if (std::holds_alternative<registry::TUnknown>((*__match_subject))) {
const registry::TUnknown& tUnknown = std::get<registry::TUnknown>((*__match_subject));
return inner_parsed;
}
return infer_result_for_non_result_type(inner_parsed, question_span);
std::abort();
}();
}

} // namespace infer_question_expression
