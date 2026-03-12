#ifndef LEXER_HPP
#define LEXER_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"

namespace lexer {

struct Expr;
struct Stmt;

ast_tokens::LexOut tokenize(mlc::String source) noexcept;

} // namespace lexer

#endif // LEXER_HPP
