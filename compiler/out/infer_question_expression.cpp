#define main mlc_user_main
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
infer_result::InferResult infer_question_from_inner_result(infer_result::InferResult inner_parsed, ast::Span question_span) noexcept{
  return [&]() -> infer_result::InferResult {
auto __match_subject = inner_parsed.inferred_type;
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [__0, type_arguments] = tGeneric; return infer_result::InferResult{ok_type_from_type_arguments(type_arguments), inner_parsed.errors};
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
