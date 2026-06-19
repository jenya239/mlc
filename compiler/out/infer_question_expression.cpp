#include "infer_question_expression.hpp"

#include "ast.hpp"
#include "infer_result.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"
#include "diagnostic_codes.hpp"

namespace infer_question_expression {

using namespace ast;
using namespace infer_result;
using namespace registry;
using namespace semantic_type_structure;
using namespace diagnostic_codes;
using namespace ast_tokens;

infer_result::InferResult infer_question_from_inner_result(infer_result::InferResult inner_parsed, ast::Span question_span) noexcept;

infer_result::InferResult infer_question_from_inner_result(infer_result::InferResult inner_parsed, ast::Span question_span) noexcept{
return [&]() -> infer_result::InferResult { if (std::holds_alternative<registry::TGeneric>((*inner_parsed.inferred_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*inner_parsed.inferred_type)); auto [_w0, type_arguments] = _v_tgeneric; return [&]() -> infer_result::InferResult { 
  std::shared_ptr<registry::Type> ok_type = type_arguments.size() > 0 ? type_arguments[0] : std::make_shared<registry::Type>((registry::TUnknown{}));
  return infer_result::InferResult{ok_type, inner_parsed.errors};
 }(); } if (std::holds_alternative<registry::TUnknown>((*inner_parsed.inferred_type))) {  return inner_parsed; } return [&]() -> infer_result::InferResult { 
  mlc::Array<ast::Diagnostic> error_diagnostics = mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("? operator requires a Result type, got ") + semantic_type_structure::type_description(inner_parsed.inferred_type), question_span, diagnostic_codes::diagnostic_code_e068())};
  return infer_result::InferResult{std::make_shared<registry::Type>((registry::TUnknown{})), ast::diagnostics_append(inner_parsed.errors, error_diagnostics)};
 }(); }();
}

} // namespace infer_question_expression
