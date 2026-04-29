#define main mlc_user_main
#include "infer_result.hpp"

namespace infer_result {

InferResult InferResult_with_type(InferResult self, std::shared_ptr<registry::Type> new_type) noexcept{
return InferResult{new_type, self.errors};
}
InferResult InferResult_absorb(InferResult self, InferResult other) noexcept{
return InferResult{self.inferred_type, ast::diagnostics_append(self.errors, other.errors)};
}
InferResult InferResult_absorb_stmt(InferResult self, StmtInferResult statement_result) noexcept{
return InferResult{self.inferred_type, ast::diagnostics_append(statement_result.errors, self.errors)};
}
InferResult infer_ok(std::shared_ptr<registry::Type> type_value) noexcept{
return InferResult{type_value, {}};
}

} // namespace infer_result
