#ifndef METHOD_RECEIVER_DIAGNOSTICS_HPP
#define METHOD_RECEIVER_DIAGNOSTICS_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"

namespace method_receiver_diagnostics {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

mlc::Array<ast::Diagnostic> infer_builtin_method_receiver_diagnostics(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name, ast::Span method_span) noexcept;

} // namespace method_receiver_diagnostics

#endif // METHOD_RECEIVER_DIAGNOSTICS_HPP
