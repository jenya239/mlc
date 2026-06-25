#ifndef VERIFY_AST_HPP
#define VERIFY_AST_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"

namespace verify_ast {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::Array<mlc::String> verify_ast_program(ast::Program program) noexcept;

} // namespace verify_ast

#endif // VERIFY_AST_HPP
