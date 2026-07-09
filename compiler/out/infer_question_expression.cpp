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

std::shared_ptr<registry::Type> ok_type_from_type_arguments(mlc::Array<std::shared_ptr<registry::Type>> type_arguments) noexcept;

infer_result::InferResult infer_result_for_non_result_type(infer_result::InferResult inner_parsed, ast::Span question_span) noexcept;

mlc::String named_or_described_type_name(std::shared_ptr<registry::Type> type_value) noexcept;

mlc::String from_trait_name_for_error_type(std::shared_ptr<registry::Type> inner_error_type) noexcept;

bool expected_error_implements_from(registry::TypeRegistry registry, std::shared_ptr<registry::Type> expected_error_type, std::shared_ptr<registry::Type> inner_error_type) noexcept;

mlc::Array<ast::Diagnostic> question_error_type_mismatch_diagnostics(registry::TypeRegistry registry, std::shared_ptr<registry::Type> inner_result_type, std::shared_ptr<registry::Type> expected_return_type, ast::Span question_span) noexcept;

infer_result::InferResult infer_question_from_inner_result(infer_result::InferResult inner_parsed, ast::Span question_span, std::shared_ptr<registry::Type> expected_return_type, registry::TypeRegistry registry) noexcept;

std::shared_ptr<registry::Type> ok_type_from_type_arguments(mlc::Array<std::shared_ptr<registry::Type>> type_arguments) noexcept{return type_arguments.size() > 0 ? type_arguments[0] : std::make_shared<registry::Type>((registry::TUnknown{}));}

infer_result::InferResult infer_result_for_non_result_type(infer_result::InferResult inner_parsed, ast::Span question_span) noexcept{
mlc::Array<ast::Diagnostic> error_diagnostics = mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("? operator requires a Result type, got ") + semantic_type_structure::type_description(inner_parsed.inferred_type), question_span, diagnostic_codes::diagnostic_code_e068())};
return infer_result::InferResult{std::make_shared<registry::Type>((registry::TUnknown{})), ast::diagnostics_append(inner_parsed.errors, error_diagnostics)};
}

mlc::String named_or_described_type_name(std::shared_ptr<registry::Type> type_value) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*type_value))) { auto _v_tnamed = std::get<registry::TNamed>((*type_value)); auto [name] = _v_tnamed; return name; } return semantic_type_structure::type_description(type_value); }();}

mlc::String from_trait_name_for_error_type(std::shared_ptr<registry::Type> inner_error_type) noexcept{return mlc::String("From<") + named_or_described_type_name(inner_error_type) + mlc::String(">");}

bool expected_error_implements_from(registry::TypeRegistry registry, std::shared_ptr<registry::Type> expected_error_type, std::shared_ptr<registry::Type> inner_error_type) noexcept{return registry::TypeRegistry_type_implements_trait(registry, named_or_described_type_name(expected_error_type), from_trait_name_for_error_type(inner_error_type));}

mlc::Array<ast::Diagnostic> question_error_type_mismatch_diagnostics(registry::TypeRegistry registry, std::shared_ptr<registry::Type> inner_result_type, std::shared_ptr<registry::Type> expected_return_type, ast::Span question_span) noexcept{
if (!result_option_method_types::is_result_generic(inner_result_type)){
{
return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty = {};
  return empty;
 }();
}
}
if (!result_option_method_types::is_result_generic(expected_return_type)){
{
return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty = {};
  return empty;
 }();
}
}
std::shared_ptr<registry::Type> inner_error_type = result_option_method_types::result_err_type(inner_result_type);
std::shared_ptr<registry::Type> expected_error_type = result_option_method_types::result_err_type(expected_return_type);
if (semantic_type_structure::type_is_unknown(inner_error_type) || semantic_type_structure::type_is_unknown(expected_error_type)){
{
return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty = {};
  return empty;
 }();
}
}
if (semantic_type_structure::types_structurally_equal(inner_error_type, expected_error_type)){
{
return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty = {};
  return empty;
 }();
}
}
if (expected_error_implements_from(registry, expected_error_type, inner_error_type)){
{
return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty = {};
  return empty;
 }();
}
}
return mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("? operator error type mismatch: expected ") + semantic_type_structure::type_description(expected_error_type) + mlc::String(", got ") + semantic_type_structure::type_description(inner_error_type), question_span, diagnostic_codes::diagnostic_code_e085())};
}

infer_result::InferResult infer_question_from_inner_result(infer_result::InferResult inner_parsed, ast::Span question_span, std::shared_ptr<registry::Type> expected_return_type, registry::TypeRegistry registry) noexcept{return [&]() -> infer_result::InferResult { if (std::holds_alternative<registry::TGeneric>((*inner_parsed.inferred_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*inner_parsed.inferred_type)); auto [generic_name, type_arguments] = _v_tgeneric; return [&]() -> infer_result::InferResult { 
  if (generic_name != mlc::String("Result")){
{
return infer_result_for_non_result_type(inner_parsed, question_span);
}
}
  mlc::Array<ast::Diagnostic> mismatch_diagnostics = question_error_type_mismatch_diagnostics(registry, inner_parsed.inferred_type, expected_return_type, question_span);
  return infer_result::InferResult{ok_type_from_type_arguments(type_arguments), ast::diagnostics_append(inner_parsed.errors, mismatch_diagnostics)};
 }(); } if (std::holds_alternative<registry::TUnknown>((*inner_parsed.inferred_type))) {  return inner_parsed; } return infer_result_for_non_result_type(inner_parsed, question_span); }();}

} // namespace infer_question_expression
