#ifndef LEXER_HPP
#define LEXER_HPP

#include "mlc.hpp"
#include <variant>

#include "ast_tokens.hpp"

namespace lexer {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

ast_tokens::LexOut tokenize(mlc::String input_text) noexcept;

} // namespace lexer

#endif // LEXER_HPP
