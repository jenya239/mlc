#ifndef CPP_LEXER_HPP
#define CPP_LEXER_HPP

#include "mlc.hpp"
#include <variant>
#include "lexer.hpp"
#include "cpp_tokens.hpp"
#include "cpp_tokens.hpp"
namespace cpp_lexer {

struct CppLexOut {
  mlc::Array<cpp_tokens::CppToken> tokens;
  mlc::Array<mlc::String> errors;
};
struct CppScanIdR {
  lexer::LexState state;
  cpp_tokens::CppToken token;
};
struct CppScanIntR {
  lexer::LexState state;
  cpp_tokens::CppToken token;
};
struct CppScanStrR {
  lexer::LexState state;
  cpp_tokens::CppToken token;
  mlc::String error;
};
struct CppScanCharR {
  lexer::LexState state;
  cpp_tokens::CppToken token;
  mlc::String error;
};
struct CppScanNumberR {
  lexer::LexState state;
  cpp_tokens::CppToken token;
};
struct CppScanOpR {
  lexer::LexState state;
  cpp_tokens::CppToken token;
};
struct CppTriviaScan {
  lexer::LexState state;
  mlc::String trivia;
  mlc::String error;
};
bool cpp_is_horizontal_whitespace(mlc::String character) noexcept;
mlc::String cpp_append_range(mlc::String source_text, int start, int end_position, mlc::String accumulated) noexcept;
bool cpp_lexer_is_pragma_after_hash(lexer::LexState state) noexcept;
bool cpp_lexer_pragma_line_starts(lexer::LexState state) noexcept;
CppTriviaScan cpp_scan_line_through_newline(lexer::LexState state, mlc::String trivia) noexcept;
CppTriviaScan cpp_scan_line_comment(lexer::LexState state, mlc::String trivia) noexcept;
CppTriviaScan cpp_scan_block_comment(lexer::LexState state, mlc::String trivia) noexcept;
CppTriviaScan cpp_collect_leading_trivia(lexer::LexState state) noexcept;
CppTriviaScan cpp_collect_trailing_trivia(lexer::LexState state) noexcept;
CppScanIdR cpp_scan_ident(lexer::LexState state) noexcept;
bool cpp_is_hex_digit(mlc::String character) noexcept;
bool cpp_is_number_suffix(mlc::String character) noexcept;
lexer::LexState cpp_scan_exponent(lexer::LexState state) noexcept;
lexer::LexState cpp_scan_number_suffixes(lexer::LexState state) noexcept;
int cpp_parse_decimal_integer(mlc::String source_text, int start, int end_position) noexcept;
CppScanNumberR cpp_scan_number(lexer::LexState state) noexcept;
CppScanCharR cpp_scan_char(lexer::LexState state) noexcept;
CppScanIntR cpp_scan_int(lexer::LexState state) noexcept;
CppScanStrR cpp_scan_string(lexer::LexState state) noexcept;
CppScanOpR cpp_scan_op_single(lexer::LexState state, mlc::String character, int line, int column) noexcept;
CppScanOpR cpp_scan_op_two_character(lexer::LexState state, mlc::String character, mlc::String next_char, int line, int column) noexcept;
CppScanOpR cpp_scan_op(lexer::LexState state) noexcept;
lexer::LexState cpp_push_token_with_trivia(mlc::Array<cpp_tokens::CppToken>& tokens, cpp_tokens::CppToken token, mlc::String leading_trivia, lexer::LexState lexer_state) noexcept;
lexer::LexState push_ident_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens, mlc::String leading_trivia) noexcept;
lexer::LexState push_int_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens, mlc::String leading_trivia) noexcept;
lexer::LexState push_string_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens, mlc::Array<mlc::String>& errors, mlc::String leading_trivia) noexcept;
lexer::LexState push_number_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens, mlc::String leading_trivia) noexcept;
lexer::LexState push_char_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens, mlc::Array<mlc::String>& errors, mlc::String leading_trivia) noexcept;
lexer::LexState push_hash_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens, mlc::String leading_trivia) noexcept;
lexer::LexState push_operator_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens, mlc::String leading_trivia) noexcept;
CppLexOut cpp_tokenize(mlc::String input_text) noexcept;
bool cpp_lex_roundtrip_preserves_kinds(mlc::String source_text) noexcept;
bool cpp_lex_roundtrip_preserves_source(mlc::String source_text) noexcept;

} // namespace cpp_lexer

#endif // CPP_LEXER_HPP
