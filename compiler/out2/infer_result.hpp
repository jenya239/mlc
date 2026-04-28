#ifndef INFER_RESULT_HPP
#define INFER_RESULT_HPP

#include "mlc.hpp"
#include <variant>

#include "registry.hpp"
#include "ast.hpp"

namespace infer_result {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

struct InferResult {std::shared_ptr<registry::Type> inferred_type;mlc::Array<ast::Diagnostic> errors;};

struct StmtInferResult {mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env;mlc::Array<ast::Diagnostic> errors;};

infer_result::InferResult infer_ok(std::shared_ptr<registry::Type> type_value) noexcept;

infer_result::InferResult InferResult_with_type(infer_result::InferResult self, std::shared_ptr<registry::Type> new_type) noexcept;

infer_result::InferResult InferResult_absorb(infer_result::InferResult self, infer_result::InferResult other) noexcept;

infer_result::InferResult InferResult_absorb_stmt(infer_result::InferResult self, infer_result::StmtInferResult statement_result) noexcept;

} // namespace infer_result

#endif // INFER_RESULT_HPP
