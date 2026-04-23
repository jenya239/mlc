#ifndef BINARY_DIAGNOSTICS_HPP
#define BINARY_DIAGNOSTICS_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"

namespace binary_diagnostics {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

mlc::Array<ast::Diagnostic> infer_binary_operand_diagnostics(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept;

} // namespace binary_diagnostics

#endif // BINARY_DIAGNOSTICS_HPP
