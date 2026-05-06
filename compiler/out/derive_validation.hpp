#ifndef DERIVE_VALIDATION_HPP
#define DERIVE_VALIDATION_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"

namespace derive_validation {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

mlc::Array<ast::Diagnostic> derive_clause_diagnostics(mlc::Array<mlc::String> type_parameter_names, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_trait_names, ast::Span diagnostic_span) noexcept;

} // namespace derive_validation

#endif // DERIVE_VALIDATION_HPP
