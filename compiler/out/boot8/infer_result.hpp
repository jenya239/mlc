#ifndef INFER_RESULT_HPP
#define INFER_RESULT_HPP

#include "mlc.hpp"
#include <variant>

#include "registry.hpp"
#include "ast.hpp"

namespace infer_result {

struct InferResult {std::shared_ptr<registry::Type> inferred_type;mlc::Array<ast::Diagnostic> errors;};
struct StmtInferResult {mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env;mlc::Array<ast::Diagnostic> errors;};
InferResult InferResult_with_type(InferResult self, std::shared_ptr<registry::Type> new_type) noexcept;
InferResult InferResult_absorb(InferResult self, InferResult other) noexcept;
InferResult InferResult_absorb_stmt(InferResult self, StmtInferResult statement_result) noexcept;
InferResult infer_ok(std::shared_ptr<registry::Type> type_value) noexcept;

} // namespace infer_result

#endif // INFER_RESULT_HPP
