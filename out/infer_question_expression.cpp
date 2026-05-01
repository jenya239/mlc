#define main mlc_user_main
#include "infer_question_expression.hpp"

namespace infer_question_expression {

infer_result::InferResult infer_question_from_inner_result(infer_result::InferResult inner_result, ast::Span question_span) noexcept{
return std::visit(overloaded{[&](const registry::TGeneric& tGeneric) { auto [__0, type_arguments] = tGeneric; return [&]() {
auto ok_type = ((type_arguments.length() > 0) ? type_arguments[0] : std::make_shared<registry::Type>(registry::TUnknown{}));
return infer_result::InferResult{ok_type, inner_result.errors};
}(); },
[&](const registry::TUnknown& tUnknown) { return inner_result; },
[&](const auto& __v) { return [&]() {
auto error = mlc::Array{ast::diagnostic_error((mlc::String("? operator requires a Result type, got ", 39) + semantic_type_structure::type_description(inner_result.inferred_type)), question_span)};
return infer_result::InferResult{std::make_shared<registry::Type>(registry::TUnknown{}), ast::diagnostics_append(inner_result.errors, error)};
}(); }
}, (*inner_result.inferred_type));
}

} // namespace infer_question_expression
