#ifndef LEXER_HPP
#define LEXER_HPP

#include "mlc.hpp"
#include <variant>

#include "ast_tokens.hpp"

namespace lexer {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

ast_tokens::LexOut tokenize(mlc::String source) noexcept;

} // namespace lexer

#endif // LEXER_HPP
