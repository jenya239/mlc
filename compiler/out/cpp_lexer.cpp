#include "cpp_lexer.hpp"

#include "lexer.hpp"
#include "cpp_tokens.hpp"
#include "cpp_tokens.hpp"

namespace cpp_lexer {

using namespace lexer;
using namespace cpp_tokens;
using namespace cpp_tokens;

struct CppScanIdR {lexer::LexState state;cpp_tokens::CppToken token;};

struct CppScanIntR {lexer::LexState state;cpp_tokens::CppToken token;};

struct CppScanStrR {lexer::LexState state;cpp_tokens::CppToken token;mlc::String error;};

struct CppScanOpR {lexer::LexState state;cpp_tokens::CppToken token;};

cpp_lexer::CppScanIdR cpp_scan_ident(lexer::LexState state) noexcept;

cpp_lexer::CppScanIntR cpp_scan_int(lexer::LexState state) noexcept;

cpp_lexer::CppScanStrR cpp_scan_string(lexer::LexState state) noexcept;

cpp_lexer::CppScanOpR cpp_scan_op_single(lexer::LexState state, mlc::String character, int line, int column) noexcept;

cpp_lexer::CppScanOpR cpp_scan_op_two_character(lexer::LexState state, mlc::String character, mlc::String next_char, int line, int column) noexcept;

cpp_lexer::CppScanOpR cpp_scan_op(lexer::LexState state) noexcept;

lexer::LexState push_ident_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens) noexcept;

lexer::LexState push_int_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens) noexcept;

lexer::LexState push_string_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens, mlc::Array<mlc::String>& errors) noexcept;

lexer::LexState push_hash_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens) noexcept;

lexer::LexState push_operator_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens) noexcept;

cpp_lexer::CppLexOut cpp_tokenize(mlc::String input_text) noexcept;

bool cpp_lex_roundtrip_preserves_kinds(mlc::String source_text) noexcept;

cpp_lexer::CppScanIdR cpp_scan_ident(lexer::LexState state) noexcept{
int start = state.position;
mlc::String input_text = state.source;
int line = state.line;
int token_column = state.column;
lexer::LexState current = std::move(state);
while (!lexer::LexState_eof(current) && lexer::is_alnum(lexer::LexState_current(current))){
{
current = lexer::LexState_lex_advance(current);
}
}
mlc::String word = input_text.substring(start, current.position - start);
return cpp_lexer::CppScanIdR{current, cpp_tokens::CppToken{cpp_tokens::cpp_keyword_kind(word), line, token_column}};
}

cpp_lexer::CppScanIntR cpp_scan_int(lexer::LexState state) noexcept{
int line = state.line;
int token_column = state.column;
lexer::LexState current = std::move(state);
int value = 0;
while (!lexer::LexState_eof(current) && lexer::is_digit(lexer::LexState_current(current))){
{
value = value * 10 + lexer::LexState_current(current).to_i();
current = lexer::LexState_lex_advance(current);
}
}
return cpp_lexer::CppScanIntR{current, cpp_tokens::CppToken{cpp_tokens::CLInt(value), line, token_column}};
}

cpp_lexer::CppScanStrR cpp_scan_string(lexer::LexState state) noexcept{
int line = state.line;
int token_column = state.column;
lexer::LexState current = lexer::LexState_lex_advance(state);
mlc::Array<mlc::String> parts = {};
mlc::String error = mlc::String("");
while (!lexer::LexState_eof(current) && lexer::LexState_current(current) != mlc::String("\"")){
{
if (lexer::LexState_current(current) == mlc::String("\\")){
{
current = lexer::LexState_lex_advance(current);
mlc::String mapped = lexer::map_escape(lexer::LexState_current(current));
if (mapped != mlc::String("")){
{
parts.push_back(mapped);
}
} else {
{
parts.push_back(mlc::String("\\"));
parts.push_back(lexer::LexState_current(current));
}
}
current = lexer::LexState_lex_advance(current);
}
} else {
{
parts.push_back(lexer::LexState_current(current));
current = lexer::LexState_lex_advance(current);
}
}
}
}
if (lexer::LexState_eof(current)){
{
error = mlc::String("unterminated string");
}
} else {
{
current = lexer::LexState_lex_advance(current);
}
}
return cpp_lexer::CppScanStrR{current, cpp_tokens::CppToken{cpp_tokens::CLStr(parts.join(mlc::String(""))), line, token_column}, error};
}

cpp_lexer::CppScanOpR cpp_scan_op_single(lexer::LexState state, mlc::String character, int line, int column) noexcept{return [&]() -> cpp_lexer::CppScanOpR { if (character == mlc::String("(")) { return cpp_lexer::CppScanOpR{lexer::LexState_lex_advance(state), cpp_tokens::CppToken{(cpp_tokens::CLParen{}), line, column}}; } if (character == mlc::String(")")) { return cpp_lexer::CppScanOpR{lexer::LexState_lex_advance(state), cpp_tokens::CppToken{(cpp_tokens::CRParen{}), line, column}}; } if (character == mlc::String("{")) { return cpp_lexer::CppScanOpR{lexer::LexState_lex_advance(state), cpp_tokens::CppToken{(cpp_tokens::CLBrace{}), line, column}}; } if (character == mlc::String("}")) { return cpp_lexer::CppScanOpR{lexer::LexState_lex_advance(state), cpp_tokens::CppToken{(cpp_tokens::CRBrace{}), line, column}}; } if (character == mlc::String("[")) { return cpp_lexer::CppScanOpR{lexer::LexState_lex_advance(state), cpp_tokens::CppToken{(cpp_tokens::CLBracket{}), line, column}}; } if (character == mlc::String("]")) { return cpp_lexer::CppScanOpR{lexer::LexState_lex_advance(state), cpp_tokens::CppToken{(cpp_tokens::CRBracket{}), line, column}}; } return cpp_lexer::CppScanOpR{lexer::LexState_lex_advance(state), cpp_tokens::CppToken{cpp_tokens::COp(character), line, column}}; }();}

cpp_lexer::CppScanOpR cpp_scan_op_two_character(lexer::LexState state, mlc::String character, mlc::String next_char, int line, int column) noexcept{return [&]() -> cpp_lexer::CppScanOpR { if (character == mlc::String("-")) { return [&]() -> cpp_lexer::CppScanOpR { if (next_char == mlc::String(">")) { return cpp_lexer::CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::CppToken{(cpp_tokens::CArrow{}), line, column}}; } if (next_char == mlc::String("-")) { return cpp_lexer::CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::CppToken{(cpp_tokens::CDec{}), line, column}}; } return cpp_scan_op_single(state, character, line, column); }(); } if (character == mlc::String(":")) { return [&]() -> cpp_lexer::CppScanOpR { if (next_char == mlc::String(":")) { return cpp_lexer::CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::CppToken{(cpp_tokens::CScope{}), line, column}}; } return cpp_scan_op_single(state, character, line, column); }(); } if (character == mlc::String("=")) { return [&]() -> cpp_lexer::CppScanOpR { if (next_char == mlc::String("=")) { return cpp_lexer::CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::CppToken{(cpp_tokens::CEq{}), line, column}}; } return cpp_scan_op_single(state, character, line, column); }(); } if (character == mlc::String("!")) { return [&]() -> cpp_lexer::CppScanOpR { if (next_char == mlc::String("=")) { return cpp_lexer::CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::CppToken{(cpp_tokens::CNe{}), line, column}}; } return cpp_scan_op_single(state, character, line, column); }(); } if (character == mlc::String("<")) { return [&]() -> cpp_lexer::CppScanOpR { if (next_char == mlc::String("=")) { return cpp_lexer::CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::CppToken{(cpp_tokens::CLe{}), line, column}}; } if (next_char == mlc::String("<")) { return cpp_lexer::CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::CppToken{(cpp_tokens::CLShift{}), line, column}}; } return cpp_lexer::CppScanOpR{lexer::LexState_lex_advance(state), cpp_tokens::CppToken{(cpp_tokens::CLAngle{}), line, column}}; }(); } if (character == mlc::String(">")) { return [&]() -> cpp_lexer::CppScanOpR { if (next_char == mlc::String("=")) { return cpp_lexer::CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::CppToken{(cpp_tokens::CGe{}), line, column}}; } if (next_char == mlc::String(">")) { return cpp_lexer::CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::CppToken{(cpp_tokens::CRShift{}), line, column}}; } return cpp_lexer::CppScanOpR{lexer::LexState_lex_advance(state), cpp_tokens::CppToken{(cpp_tokens::CRAngle{}), line, column}}; }(); } if (character == mlc::String("&")) { return [&]() -> cpp_lexer::CppScanOpR { if (next_char == mlc::String("&")) { return cpp_lexer::CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::CppToken{(cpp_tokens::CAnd{}), line, column}}; } return cpp_scan_op_single(state, character, line, column); }(); } if (character == mlc::String("|")) { return [&]() -> cpp_lexer::CppScanOpR { if (next_char == mlc::String("|")) { return cpp_lexer::CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::CppToken{(cpp_tokens::COr{}), line, column}}; } return cpp_scan_op_single(state, character, line, column); }(); } if (character == mlc::String("+")) { return [&]() -> cpp_lexer::CppScanOpR { if (next_char == mlc::String("+")) { return cpp_lexer::CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::CppToken{(cpp_tokens::CInc{}), line, column}}; } return cpp_scan_op_single(state, character, line, column); }(); } return cpp_scan_op_single(state, character, line, column); }();}

cpp_lexer::CppScanOpR cpp_scan_op(lexer::LexState state) noexcept{
int line = state.line;
int token_column = state.column;
mlc::String character = lexer::LexState_current(state);
mlc::String next_char = lexer::LexState_peek(state, 1);
return cpp_scan_op_two_character(state, character, next_char, line, token_column);
}

lexer::LexState push_ident_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens) noexcept{
cpp_lexer::CppScanIdR scan_result = cpp_scan_ident(state);
tokens.push_back(scan_result.token);
return scan_result.state;
}

lexer::LexState push_int_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens) noexcept{
cpp_lexer::CppScanIntR scan_result = cpp_scan_int(state);
tokens.push_back(scan_result.token);
return scan_result.state;
}

lexer::LexState push_string_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens, mlc::Array<mlc::String>& errors) noexcept{
cpp_lexer::CppScanStrR scan_result = cpp_scan_string(state);
tokens.push_back(scan_result.token);
if (scan_result.error != mlc::String("")){
{
errors.push_back(scan_result.error);
}
}
return scan_result.state;
}

lexer::LexState push_hash_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens) noexcept{
tokens.push_back(cpp_tokens::CppToken{(cpp_tokens::CHash{}), state.line, state.column});
return lexer::LexState_lex_advance(state);
}

lexer::LexState push_operator_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens) noexcept{
cpp_lexer::CppScanOpR scan_result = cpp_scan_op(state);
tokens.push_back(scan_result.token);
return scan_result.state;
}

cpp_lexer::CppLexOut cpp_tokenize(mlc::String input_text) noexcept{
lexer::LexState state = lexer::LexState{input_text, 0, 1, 1};
mlc::Array<cpp_tokens::CppToken> tokens = {};
mlc::Array<mlc::String> errors = {};
lexer::LexState lexer_state = std::move(state);
while (!lexer::LexState_eof(lexer_state)){
{
auto __tmp_11 = lexer::skip_whitespace(lexer_state);
auto after = __tmp_11.after;
lexer_state = after;
if (lexer::LexState_eof(lexer_state)){
{
break;
}
}
mlc::String character = lexer::LexState_current(lexer_state);
if (lexer::is_alpha(character)){
{
lexer_state = push_ident_scan(lexer_state, tokens);
}
} else {
{
if (lexer::is_digit(character)){
lexer_state = push_int_scan(lexer_state, tokens);
} else {
if (character == mlc::String("\"")){
lexer_state = push_string_scan(lexer_state, tokens, errors);
} else {
if (character == mlc::String("#")){
lexer_state = push_hash_scan(lexer_state, tokens);
} else {
lexer_state = push_operator_scan(lexer_state, tokens);
}
}
}
}
}
}
}
tokens.push_back(cpp_tokens::CppToken{(cpp_tokens::CEof{}), lexer_state.line, lexer_state.column});
return cpp_lexer::CppLexOut{tokens, errors};
}

bool cpp_lex_roundtrip_preserves_kinds(mlc::String source_text) noexcept{
mlc::Array<cpp_tokens::CppToken> first_tokens = cpp_tokenize(source_text).tokens;
mlc::String rebuilt_source = cpp_tokens::cpp_tokens_to_source(first_tokens);
mlc::Array<cpp_tokens::CppToken> second_tokens = cpp_tokenize(rebuilt_source).tokens;
return cpp_tokens::cpp_token_kinds_equal(first_tokens, second_tokens);
}

} // namespace cpp_lexer
