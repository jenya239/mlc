#ifndef DECLS_HPP
#define DECLS_HPP

#include "mlc.hpp"
#include <variant>

#include "predicates.hpp"
#include "ast_tokens.hpp"
#include "types.hpp"
#include "exprs.hpp"
#include "comma_separated.hpp"
#include "expect_close.hpp"
#include "ast.hpp"

namespace decls {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

predicates::DeclResult parse_declaration(predicates::Parser parser) noexcept;

ast::Program parse_program_with_source_path(mlc::Array<ast_tokens::Token> tokens, mlc::String source_path) noexcept;

predicates::ParseProgramResult parse_program_with_errors(mlc::Array<ast_tokens::Token> tokens, mlc::String source_path) noexcept;

ast::Program parse_program(mlc::Array<ast_tokens::Token> tokens) noexcept;

} // namespace decls

#endif // DECLS_HPP
