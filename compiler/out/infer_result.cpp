#include "infer_result.hpp"

#include "registry.hpp"
#include "ast.hpp"

namespace infer_result {

using namespace registry;
using namespace ast;
using namespace ast_tokens;

infer_result::InferResult InferResult_with_type(infer_result::InferResult self, std::shared_ptr<registry::Type> new_type) noexcept;

infer_result::InferResult InferResult_absorb(infer_result::InferResult self, infer_result::InferResult other) noexcept;

infer_result::InferResult InferResult_absorb_stmt(infer_result::InferResult self, infer_result::StmtInferResult statement_result) noexcept;

infer_result::InferResult infer_ok(std::shared_ptr<registry::Type> type_value) noexcept;

infer_result::InferResult InferResult_with_type(infer_result::InferResult self, std::shared_ptr<registry::Type> new_type) noexcept{return infer_result::InferResult{new_type, self.errors};}

infer_result::InferResult InferResult_absorb(infer_result::InferResult self, infer_result::InferResult other) noexcept{return infer_result::InferResult{self.inferred_type, ast::diagnostics_append(self.errors, other.errors)};}

infer_result::InferResult InferResult_absorb_stmt(infer_result::InferResult self, infer_result::StmtInferResult statement_result) noexcept{return infer_result::InferResult{self.inferred_type, ast::diagnostics_append(statement_result.errors, self.errors)};}

infer_result::InferResult infer_ok(std::shared_ptr<registry::Type> type_value) noexcept{return infer_result::InferResult{type_value, {}};}

} // namespace infer_result
