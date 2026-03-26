#ifndef DECLS_HPP
#define DECLS_HPP

#include "mlc.hpp"
#include <variant>

#include "preds.hpp"
#include "types.hpp"
#include "exprs.hpp"
#include "ast.hpp"

namespace decls {

struct Expr;
struct Stmt;

preds::DeclResult parse_decl(preds::Parser parser) noexcept;

ast::Program parse_program_with_source_path(mlc::Array<ast_tokens::Token> tokens, mlc::String source_path) noexcept;

ast::Program parse_program(mlc::Array<ast_tokens::Token> tokens) noexcept;

} // namespace decls

#endif // DECLS_HPP
