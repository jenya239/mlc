#ifndef LEXER_HPP
#define LEXER_HPP

#include "mlc.hpp"
#include <variant>

#include "ast_tokens.hpp"

namespace lexer {

struct CppStatement;
struct CppExpression;

struct LexState {mlc::String source;int position;int line;int column;};

struct SkipResult {lexer::LexState after;mlc::String error;};

bool is_alpha(mlc::String character) noexcept;

bool is_digit(mlc::String character) noexcept;

bool is_alnum(mlc::String character) noexcept;

mlc::String map_escape(mlc::String character) noexcept;

lexer::SkipResult skip_whitespace(lexer::LexState state) noexcept;

ast_tokens::LexOut tokenize(mlc::String input_text) noexcept;

bool LexState_eof(lexer::LexState self) noexcept;

mlc::String LexState_current(lexer::LexState self) noexcept;

mlc::String LexState_peek(lexer::LexState self, int offset) noexcept;

lexer::LexState LexState_lex_advance(lexer::LexState self) noexcept;

lexer::LexState LexState_lex_advance_by(lexer::LexState self, int advance_count) noexcept;

ast_tokens::Token LexState_token(lexer::LexState self, ast_tokens::TokenKind token_kind) noexcept;

} // namespace lexer

#endif // LEXER_HPP
