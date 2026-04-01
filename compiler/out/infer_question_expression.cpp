#include "infer_question_expression.hpp"

#include "infer_result.hpp"
#include "registry.hpp"

namespace infer_question_expression {

using namespace infer_result;
using namespace registry;

infer_result::InferResult infer_question_from_inner_result(infer_result::InferResult inner_result) noexcept;

infer_result::InferResult infer_question_from_inner_result(infer_result::InferResult inner_result) noexcept{
std::shared_ptr<registry::Type> ok_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TGeneric>((*inner_result.inferred_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*inner_result.inferred_type)); auto [_w0, type_arguments] = _v_tgeneric; return type_arguments.size() > 0 ? type_arguments[0] : std::make_shared<registry::Type>((registry::TUnknown{})); } return std::make_shared<registry::Type>((registry::TUnknown{})); }();
return infer_result::InferResult{ok_type, inner_result.errors};
}

} // namespace infer_question_expression
