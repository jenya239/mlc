#ifndef LEXER_HPP
#define LEXER_HPP

#include "mlc.hpp"
#include <variant>

#include "ast_tokens.hpp"

namespace lexer {

struct LexState {mlc::String src;int pos;int line;int col;};
struct ScanResult {LexState state;ast_tokens::Token token;};
struct ScanStrResult {LexState state;ast_tokens::Token token;mlc::String error;};
struct SuffixScan {mlc::String suffix;LexState after;};
bool LexState_eof(LexState self) noexcept;
mlc::String LexState_current(LexState self) noexcept;
mlc::String LexState_peek(LexState self, int offset) noexcept;
LexState LexState_lex_advance(LexState self) noexcept;
LexState LexState_lex_advance_by(LexState self, int count) noexcept;
ast_tokens::Token LexState_token(LexState self, ast_tokens::TKind tk) noexcept;
bool is_alpha(mlc::String character) noexcept;
bool is_digit(mlc::String character) noexcept;
bool is_alnum(mlc::String character) noexcept;
bool is_ws(mlc::String character) noexcept;
ast_tokens::TKind keyword_kind(mlc::String word) noexcept;
ScanResult scan_ident(LexState state) noexcept;
bool is_alpha_lower(mlc::String c) noexcept;
SuffixScan try_scan_suffix(LexState state) noexcept;
ScanResult scan_int(LexState state) noexcept;
ScanStrResult scan_single_string(LexState state) noexcept;
mlc::String map_escape(mlc::String character) noexcept;
ScanStrResult scan_string(LexState state) noexcept;
LexState scan_template_nested_string(LexState initial, mlc::Array<mlc::String> expr_chars) noexcept;
ScanStrResult scan_template(LexState state) noexcept;
LexState skip_whitespace(LexState state) noexcept;
ScanResult scan_op(LexState state) noexcept;
ast_tokens::LexOut tokenize(mlc::String source) noexcept;

} // namespace lexer

#endif // LEXER_HPP
