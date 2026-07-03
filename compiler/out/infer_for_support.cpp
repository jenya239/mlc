#define main mlc_user_main
#include "infer_for_support.hpp"

#include "ast.hpp"
#include "infer_result.hpp"
#include "registry.hpp"
#include "type_diagnostics.hpp"

namespace infer_for_support {

using namespace ast;
using namespace infer_result;
using namespace registry;
using namespace type_diagnostics;
using namespace ast_tokens;

infer_result::InferResult infer_for_iterator_with_range_rules(infer_result::InferResult iterator_base, std::shared_ptr<ast::Expr> iterator_expression) noexcept{
  return infer_result::InferResult{iterator_base.inferred_type, ast::diagnostics_append(iterator_base.errors, type_diagnostics::for_loop_range_diagnostic(iterator_base.inferred_type, iterator_expression))};
}
std::shared_ptr<registry::Type> element_type_for_for_iterator(std::shared_ptr<registry::Type> iterator_type) noexcept{
  return [&]() -> std::shared_ptr<registry::Type> {
auto __match_subject = iterator_type;
if (std::holds_alternative<registry::TArray>((*__match_subject))) {
const registry::TArray& tArray = std::get<registry::TArray>((*__match_subject));
auto [element_type] = tArray; return element_type;
}
return std::make_shared<registry::Type>(registry::TUnknown{});
std::abort();
}();
}

} // namespace infer_for_support
