#ifndef CALL_ARGUMENT_UNIFY_HPP
#define CALL_ARGUMENT_UNIFY_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "type_utils.hpp"
#include "semantic_type_structure.hpp"

namespace call_argument_unify {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

mlc::Array<ast::Diagnostic> call_argument_unification_diagnostics(mlc::Array<std::shared_ptr<registry::Type>> expected_parameter_types, mlc::Array<std::shared_ptr<registry::Type>> actual_argument_types, mlc::Array<mlc::String> type_parameter_names, mlc::Array<std::shared_ptr<ast::Expr>> argument_expressions) noexcept;

} // namespace call_argument_unify

#endif // CALL_ARGUMENT_UNIFY_HPP
