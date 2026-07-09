#ifndef LEXER_HPP
#define LEXER_HPP

#include "mlc.hpp"
#include <variant>
#include "ast_tokens.hpp"
namespace lexer {

struct LexState {
  mlc::String source;
  int position;
  int line;
  int column;
};
struct SkipResult {
  LexState after;
  mlc::String error;
};
struct ScanResult {
  LexState after;
  ast_tokens::Token token;
};
struct ScanStrResult {
  LexState after;
  ast_tokens::Token token;
  mlc::String error;
};
struct SuffixScan {
  mlc::String suffix;
  LexState after;
};
void lexer_internal_panic(mlc::String message) noexcept;
bool LexState_eof(LexState self) noexcept;
mlc::String LexState_current(LexState self) noexcept;
mlc::String LexState_peek(LexState self, int offset) noexcept;
LexState LexState_lex_advance(LexState self) noexcept;
LexState LexState_lex_advance_by(LexState self, int advance_count) noexcept;
ast_tokens::Token LexState_token(LexState self, ast_tokens::TokenKind token_kind) noexcept;
bool is_alpha(mlc::String character) noexcept;
bool is_digit(mlc::String character) noexcept;
bool is_alnum(mlc::String character) noexcept;
bool is_ws(mlc::String character) noexcept;
mlc::Array<mlc::String> mlc_reserved_keywords() noexcept;
bool is_reserved_keyword(mlc::String word) noexcept;
ast_tokens::TokenKind reserved_keyword_kind(mlc::String word) noexcept;
ast_tokens::TokenKind keyword_kind(mlc::String word) noexcept;
ScanResult scan_ident(LexState state) noexcept;
bool is_alpha_lower(mlc::String character) noexcept;
SuffixScan try_scan_suffix(LexState state) noexcept;
ScanResult scan_int(LexState state) noexcept;
ScanStrResult scan_single_string(LexState state) noexcept;
mlc::String map_escape(mlc::String character) noexcept;
LexState lex_advance_past_unescaped_string_run(LexState state) noexcept;
LexState lex_advance_past_template_literal_run(LexState state) noexcept;
ScanStrResult scan_string(LexState state) noexcept;
LexState scan_template_nested_string(LexState initial, mlc::Array<mlc::String> expr_chars) noexcept;
ScanStrResult scan_template(LexState state) noexcept;
SkipResult skip_whitespace(LexState state) noexcept;
ScanResult scan_op_two_character(LexState state, mlc::String character, mlc::String next) noexcept;
ScanResult scan_op_single(mlc::String character, LexState state) noexcept;
ScanResult scan_op(LexState state) noexcept;
void accumulate_nonempty_scan_message_into_errors(mlc::Array<mlc::String>& error_messages, mlc::String message_under_scan) noexcept;
LexState apply_skip_whitespace(LexState state, mlc::Array<mlc::String>& error_messages) noexcept;
LexState push_ident_scan(LexState state, mlc::Array<ast_tokens::Token>& tokens) noexcept;
LexState push_int_scan(LexState state, mlc::Array<ast_tokens::Token>& tokens) noexcept;
LexState push_string_scan(LexState state, mlc::Array<ast_tokens::Token>& tokens, mlc::Array<mlc::String>& error_messages) noexcept;
LexState push_single_string_scan(LexState state, mlc::Array<ast_tokens::Token>& tokens, mlc::Array<mlc::String>& error_messages) noexcept;
LexState push_template_scan(LexState state, mlc::Array<ast_tokens::Token>& tokens, mlc::Array<mlc::String>& error_messages) noexcept;
LexState push_operator_scan(LexState state, mlc::Array<ast_tokens::Token>& tokens) noexcept;
ast_tokens::LexOut tokenize(mlc::String input_text) noexcept;
bool LexState_eof(LexState self) noexcept;
mlc::String LexState_current(LexState self) noexcept;
mlc::String LexState_peek(LexState self, int offset) noexcept;
LexState LexState_lex_advance(LexState self) noexcept;
LexState LexState_lex_advance_by(LexState self, int advance_count) noexcept;
ast_tokens::Token LexState_token(LexState self, ast_tokens::TokenKind token_kind) noexcept;

} // namespace lexer

#endif // LEXER_HPP
