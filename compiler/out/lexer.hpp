#ifndef LEXER_HPP
#define LEXER_HPP

#include "mlc.hpp"
#include <variant>

#include "ast_tokens.hpp"

namespace lexer {

struct Expr;
struct Stmt;

struct LexState {mlc::String src;int pos;int line;int col;};

struct ScanResult {lexer::LexState state;ast_tokens::Token token;};

struct ScanStrResult {lexer::LexState state;ast_tokens::Token token;mlc::String error;};

ast_tokens::LexOut tokenize(mlc::String source) noexcept;

} // namespace lexer

#endif // LEXER_HPP
