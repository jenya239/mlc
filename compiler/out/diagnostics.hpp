#ifndef DIAGNOSTICS_HPP
#define DIAGNOSTICS_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "check.hpp"

namespace diagnostics {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<ast::Diagnostic> collect_diagnostics_in_source(mlc::String source_text, mlc::String source_file) noexcept;

} // namespace diagnostics

#endif // DIAGNOSTICS_HPP
