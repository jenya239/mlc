#ifndef PARTIAL_APPLICATION_DESUGAR_HPP
#define PARTIAL_APPLICATION_DESUGAR_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"

namespace partial_application_desugar {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

std::shared_ptr<ast::Expr> partial_application_desugar_expr(std::shared_ptr<ast::Expr> expression) noexcept;

} // namespace partial_application_desugar

#endif // PARTIAL_APPLICATION_DESUGAR_HPP
