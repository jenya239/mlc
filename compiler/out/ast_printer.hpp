#ifndef AST_PRINTER_HPP
#define AST_PRINTER_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"

namespace ast_printer {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::String print_expr(std::shared_ptr<ast::Expr> expression) noexcept;

} // namespace ast_printer

#endif // AST_PRINTER_HPP
