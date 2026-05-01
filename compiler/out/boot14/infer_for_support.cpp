#define main mlc_user_main
#include "infer_for_support.hpp"

namespace infer_for_support {

infer_result::InferResult infer_for_iterator_with_range_rules(infer_result::InferResult iterator_base, std::shared_ptr<ast::Expr> iterator_expression) noexcept{
return infer_result::InferResult{iterator_base.inferred_type, ast::diagnostics_append(iterator_base.errors, type_diagnostics::for_loop_range_diagnostic(iterator_base.inferred_type, iterator_expression))};
}
std::shared_ptr<registry::Type> element_type_for_for_iterator(std::shared_ptr<registry::Type> iterator_type) noexcept{
return std::visit(overloaded{[&](const registry::TArray& tArray) { auto [inner] = tArray; return inner; },
[&](const auto& __v) { return std::make_shared<registry::Type>(registry::TUnknown{}); }
}, (*iterator_type));
}

} // namespace infer_for_support
