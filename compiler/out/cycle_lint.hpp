#ifndef CYCLE_LINT_HPP
#define CYCLE_LINT_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "diagnostic_codes.hpp"

namespace cycle_lint {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<ast::Diagnostic> shared_cycle_lint_diagnostics(ast::Program program, registry::TypeRegistry registry) noexcept;

} // namespace cycle_lint

#endif // CYCLE_LINT_HPP
