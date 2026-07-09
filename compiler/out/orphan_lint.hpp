#ifndef ORPHAN_LINT_HPP
#define ORPHAN_LINT_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "diagnostic_codes.hpp"

namespace orphan_lint {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<ast::Diagnostic> orphan_impl_diagnostics(ast::Program program, registry::TypeRegistry registry) noexcept;

} // namespace orphan_lint

#endif // ORPHAN_LINT_HPP
