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

struct CppScanCharR {lexer::LexState state;cpp_tokens::CppToken token;mlc::String error;};

struct CppScanNumberR {lexer::LexState state;cpp_tokens::CppToken token;};

struct CppScanOpR {lexer::LexState state;cpp_tokens::CppToken token;};

struct CppTriviaScan {lexer::LexState state;mlc::String trivia;mlc::String error;};

bool cpp_is_horizontal_whitespace(mlc::String character) noexcept;

mlc::String cpp_append_range(mlc::String source_text, int start, int end_position, mlc::String accumulated) noexcept;

bool cpp_lexer_is_pragma_after_hash(lexer::LexState state) noexcept;

bool cpp_lexer_pragma_line_starts(lexer::LexState state) noexcept;

cpp_lexer::CppTriviaScan cpp_scan_line_through_newline(lexer::LexState state, mlc::String trivia) noexcept;

cpp_lexer::CppTriviaScan cpp_scan_line_comment(lexer::LexState state, mlc::String trivia) noexcept;

cpp_lexer::CppTriviaScan cpp_scan_block_comment(lexer::LexState state, mlc::String trivia) noexcept;

cpp_lexer::CppTriviaScan cpp_collect_leading_trivia(lexer::LexState state) noexcept;

cpp_lexer::CppTriviaScan cpp_collect_trailing_trivia(lexer::LexState state) noexcept;

cpp_lexer::CppScanIdR cpp_scan_ident(lexer::LexState state) noexcept;

bool cpp_is_hex_digit(mlc::String character) noexcept;

bool cpp_is_number_suffix(mlc::String character) noexcept;

lexer::LexState cpp_scan_exponent(lexer::LexState state) noexcept;

lexer::LexState cpp_scan_number_suffixes(lexer::LexState state) noexcept;

int cpp_parse_decimal_integer(mlc::String source_text, int start, int end_position) noexcept;

cpp_lexer::CppScanNumberR cpp_scan_number(lexer::LexState state) noexcept;

cpp_lexer::CppScanCharR cpp_scan_char(lexer::LexState state) noexcept;

cpp_lexer::CppScanIntR cpp_scan_int(lexer::LexState state) noexcept;

cpp_lexer::CppScanStrR cpp_scan_string(lexer::LexState state) noexcept;

cpp_lexer::CppScanOpR cpp_scan_op_single(lexer::LexState state, mlc::String character, int line, int column) noexcept;

cpp_lexer::CppScanOpR cpp_scan_op_two_character(lexer::LexState state, mlc::String character, mlc::String next_char, int line, int column) noexcept;

cpp_lexer::CppScanOpR cpp_scan_op(lexer::LexState state) noexcept;

lexer::LexState cpp_push_token_with_trivia(mlc::Array<cpp_tokens::CppToken>& tokens, cpp_tokens::CppToken token, mlc::String leading_trivia, lexer::LexState lexer_state) noexcept;

lexer::LexState push_ident_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens, mlc::String leading_trivia) noexcept;

lexer::LexState push_int_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens, mlc::String leading_trivia) noexcept;

lexer::LexState push_string_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens, mlc::Array<mlc::String>& errors, mlc::String leading_trivia) noexcept;

lexer::LexState push_number_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens, mlc::String leading_trivia) noexcept;

lexer::LexState push_char_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens, mlc::Array<mlc::String>& errors, mlc::String leading_trivia) noexcept;

lexer::LexState push_hash_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens, mlc::String leading_trivia) noexcept;

lexer::LexState push_operator_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens, mlc::String leading_trivia) noexcept;

cpp_lexer::CppLexOut cpp_tokenize(mlc::String input_text) noexcept;

bool cpp_lex_roundtrip_preserves_kinds(mlc::String source_text) noexcept;

bool cpp_lex_roundtrip_preserves_source(mlc::String source_text) noexcept;

bool cpp_is_horizontal_whitespace(mlc::String character) noexcept{return character == mlc::String(" ") || character == mlc::String("\t");}

mlc::String cpp_append_range(mlc::String source_text, int start, int end_position, mlc::String accumulated) noexcept{return end_position > start ? accumulated + source_text.substring(start, end_position - start) : accumulated;}

bool cpp_lexer_is_pragma_after_hash(lexer::LexState state) noexcept{
mlc::String source_text = state.source;
lexer::LexState scan = lexer::LexState_lex_advance(state);
while (!lexer::LexState_eof(scan) && cpp_is_horizontal_whitespace(lexer::LexState_current(scan))){
{
scan = lexer::LexState_lex_advance(scan);
}
}
int word_start = scan.position;
while (!lexer::LexState_eof(scan) && lexer::is_alnum(lexer::LexState_current(scan))){
{
scan = lexer::LexState_lex_advance(scan);
}
}
mlc::String word = source_text.substring(word_start, scan.position - word_start);
return word == mlc::String("pragma");
}

bool cpp_lexer_pragma_line_starts(lexer::LexState state) noexcept{return lexer::LexState_current(state) != mlc::String("#") ? false : cpp_lexer_is_pragma_after_hash(state);}

cpp_lexer::CppTriviaScan cpp_scan_line_through_newline(lexer::LexState state, mlc::String trivia) noexcept{
mlc::String source_text = state.source;
lexer::LexState current = std::move(state);
int start = current.position;
while (!lexer::LexState_eof(current) && lexer::LexState_current(current) != mlc::String("\n")){
{
current = lexer::LexState_lex_advance(current);
}
}
if (!lexer::LexState_eof(current)){
{
current = lexer::LexState_lex_advance(current);
}
}
return cpp_lexer::CppTriviaScan{current, cpp_append_range(source_text, start, current.position, trivia), mlc::String("")};
}

cpp_lexer::CppTriviaScan cpp_scan_line_comment(lexer::LexState state, mlc::String trivia) noexcept{
mlc::String source_text = state.source;
int start = state.position;
lexer::LexState current = lexer::LexState_lex_advance_by(state, 2);
while (!lexer::LexState_eof(current) && lexer::LexState_current(current) != mlc::String("\n")){
{
current = lexer::LexState_lex_advance(current);
}
}
if (!lexer::LexState_eof(current)){
{
current = lexer::LexState_lex_advance(current);
}
}
return cpp_lexer::CppTriviaScan{current, cpp_append_range(source_text, start, current.position, trivia), mlc::String("")};
}

cpp_lexer::CppTriviaScan cpp_scan_block_comment(lexer::LexState state, mlc::String trivia) noexcept{
mlc::String source_text = state.source;
int start = state.position;
lexer::LexState current = lexer::LexState_lex_advance_by(state, 2);
mlc::String error = mlc::String("");
bool closed = false;
while (!lexer::LexState_eof(current)){
{
if (lexer::LexState_current(current) == mlc::String("*") && lexer::LexState_peek(current, 1) == mlc::String("/")){
{
current = lexer::LexState_lex_advance_by(current, 2);
closed = true;
break;
}
}
current = lexer::LexState_lex_advance(current);
}
}
if (!closed){
{
error = mlc::String("unterminated block comment");
}
}
return cpp_lexer::CppTriviaScan{current, cpp_append_range(source_text, start, current.position, trivia), error};
}

cpp_lexer::CppTriviaScan cpp_collect_leading_trivia(lexer::LexState state) noexcept{
mlc::String source_text = state.source;
lexer::LexState current = std::move(state);
mlc::String trivia = mlc::String("");
mlc::String error = mlc::String("");
while (!lexer::LexState_eof(current)){
{
mlc::String character = lexer::LexState_current(current);
if (character == mlc::String("\n")){
{
trivia = trivia + mlc::String("\n");
current = lexer::LexState_lex_advance(current);
}
} else {
{
if (cpp_is_horizontal_whitespace(character)){
int start = current.position;
while (!lexer::LexState_eof(current) && cpp_is_horizontal_whitespace(lexer::LexState_current(current))){
{
current = lexer::LexState_lex_advance(current);
}
}
trivia = cpp_append_range(source_text, start, current.position, trivia);
} else {
if (character == mlc::String("/") && lexer::LexState_peek(current, 1) == mlc::String("/")){
cpp_lexer::CppTriviaScan scan = cpp_scan_line_comment(current, trivia);
current = scan.state;
trivia = scan.trivia;
if (scan.error != mlc::String("")){
error = scan.error;
}
} else {
if (character == mlc::String("/") && lexer::LexState_peek(current, 1) == mlc::String("*")){
cpp_lexer::CppTriviaScan scan = cpp_scan_block_comment(current, trivia);
current = scan.state;
trivia = scan.trivia;
if (scan.error != mlc::String("")){
error = scan.error;
}
} else {
if (character == mlc::String("#") && cpp_lexer_pragma_line_starts(current)){
cpp_lexer::CppTriviaScan scan = cpp_scan_line_through_newline(current, trivia);
current = scan.state;
trivia = scan.trivia;
} else {
break;
}
}
}
}
}
}
}
}
return cpp_lexer::CppTriviaScan{current, trivia, error};
}

cpp_lexer::CppTriviaScan cpp_collect_trailing_trivia(lexer::LexState state) noexcept{
lexer::LexState current = std::move(state);
mlc::String trivia = mlc::String("");
while (!lexer::LexState_eof(current) && cpp_is_horizontal_whitespace(lexer::LexState_current(current))){
{
trivia = trivia + lexer::LexState_current(current);
current = lexer::LexState_lex_advance(current);
}
}
return cpp_lexer::CppTriviaScan{current, trivia, mlc::String("")};
}

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
return cpp_lexer::CppScanIdR{current, cpp_tokens::cpp_token_make(cpp_tokens::cpp_keyword_kind(word), line, token_column)};
}

bool cpp_is_hex_digit(mlc::String character) noexcept{return lexer::is_digit(character) || character == mlc::String("a") || character == mlc::String("A") || character == mlc::String("b") || character == mlc::String("B") || character == mlc::String("c") || character == mlc::String("C") || character == mlc::String("d") || character == mlc::String("D") || character == mlc::String("e") || character == mlc::String("E") || character == mlc::String("f") || character == mlc::String("F");}

bool cpp_is_number_suffix(mlc::String character) noexcept{return character == mlc::String("u") || character == mlc::String("U") || character == mlc::String("l") || character == mlc::String("L") || character == mlc::String("f") || character == mlc::String("F");}

lexer::LexState cpp_scan_exponent(lexer::LexState state) noexcept{
lexer::LexState current = std::move(state);
if (lexer::LexState_current(current) == mlc::String("e") || lexer::LexState_current(current) == mlc::String("E")){
{
current = lexer::LexState_lex_advance(current);
if (lexer::LexState_current(current) == mlc::String("+") || lexer::LexState_current(current) == mlc::String("-")){
{
current = lexer::LexState_lex_advance(current);
}
}
[&]() { 
  while (!lexer::LexState_eof(current) && lexer::is_digit(lexer::LexState_current(current))){
{
current = lexer::LexState_lex_advance(current);
}
}
 }();
}
}
return current;
}

lexer::LexState cpp_scan_number_suffixes(lexer::LexState state) noexcept{
lexer::LexState current = std::move(state);
while (!lexer::LexState_eof(current) && cpp_is_number_suffix(lexer::LexState_current(current))){
{
current = lexer::LexState_lex_advance(current);
}
}
return current;
}

int cpp_parse_decimal_integer(mlc::String source_text, int start, int end_position) noexcept{
int value = 0;
int index = start;
while (index < end_position){
{
value = value * 10 + source_text.char_at(index).to_i();
index = index + 1;
}
}
return value;
}

cpp_lexer::CppScanNumberR cpp_scan_number(lexer::LexState state) noexcept{
mlc::String source_text = state.source;
int start = state.position;
int line = state.line;
int token_column = state.column;
lexer::LexState current = std::move(state);
return lexer::LexState_current(current) == mlc::String(".") && lexer::is_digit(lexer::LexState_peek(current, 1)) ? [&]() -> cpp_lexer::CppScanNumberR { 
  current = lexer::LexState_lex_advance(current);
  while (!lexer::LexState_eof(current) && lexer::is_digit(lexer::LexState_current(current))){
{
current = lexer::LexState_lex_advance(current);
}
}
  current = cpp_scan_exponent(current);
  current = cpp_scan_number_suffixes(current);
  mlc::String lexeme = source_text.substring(start, current.position - start);
  return cpp_lexer::CppScanNumberR{current, cpp_tokens::cpp_token_make(cpp_tokens::CLFloat(lexeme), line, token_column)};
 }() : [&]() -> cpp_lexer::CppScanNumberR { 
  while (!lexer::LexState_eof(current) && lexer::is_digit(lexer::LexState_current(current))){
{
current = lexer::LexState_lex_advance(current);
}
}
  return source_text.char_at(start) == mlc::String("0") && !lexer::LexState_eof(current) && (lexer::LexState_current(current) == mlc::String("x") || lexer::LexState_current(current) == mlc::String("X")) ? [&]() -> cpp_lexer::CppScanNumberR { 
  current = lexer::LexState_lex_advance(current);
  while (!lexer::LexState_eof(current) && cpp_is_hex_digit(lexer::LexState_current(current))){
{
current = lexer::LexState_lex_advance(current);
}
}
  current = cpp_scan_number_suffixes(current);
  mlc::String hex_lexeme = source_text.substring(start, current.position - start);
  return cpp_lexer::CppScanNumberR{current, cpp_tokens::cpp_token_make(cpp_tokens::CLHex(hex_lexeme), line, token_column)};
 }() : !lexer::LexState_eof(current) && lexer::LexState_current(current) == mlc::String(".") ? [&]() -> cpp_lexer::CppScanNumberR { 
  current = lexer::LexState_lex_advance(current);
  while (!lexer::LexState_eof(current) && lexer::is_digit(lexer::LexState_current(current))){
{
current = lexer::LexState_lex_advance(current);
}
}
  current = cpp_scan_exponent(current);
  current = cpp_scan_number_suffixes(current);
  mlc::String float_lexeme = source_text.substring(start, current.position - start);
  return cpp_lexer::CppScanNumberR{current, cpp_tokens::cpp_token_make(cpp_tokens::CLFloat(float_lexeme), line, token_column)};
 }() : !lexer::LexState_eof(current) && (lexer::LexState_current(current) == mlc::String("e") || lexer::LexState_current(current) == mlc::String("E")) ? [&]() -> cpp_lexer::CppScanNumberR { 
  current = cpp_scan_exponent(current);
  current = cpp_scan_number_suffixes(current);
  mlc::String exponent_lexeme = source_text.substring(start, current.position - start);
  return cpp_lexer::CppScanNumberR{current, cpp_tokens::cpp_token_make(cpp_tokens::CLFloat(exponent_lexeme), line, token_column)};
 }() : [&]() -> cpp_lexer::CppScanNumberR { 
  current = cpp_scan_number_suffixes(current);
  int integer_value = cpp_parse_decimal_integer(source_text, start, current.position);
  return cpp_lexer::CppScanNumberR{current, cpp_tokens::cpp_token_make(cpp_tokens::CLInt(integer_value), line, token_column)};
 }();
 }();
}

cpp_lexer::CppScanCharR cpp_scan_char(lexer::LexState state) noexcept{
int line = state.line;
int token_column = state.column;
lexer::LexState current = lexer::LexState_lex_advance(state);
mlc::String character_value = mlc::String("");
mlc::String error = mlc::String("");
if (lexer::LexState_eof(current)){
{
error = mlc::String("unterminated character literal");
}
} else {
{
if (lexer::LexState_current(current) == mlc::String("'")){
error = mlc::String("empty character literal");
current = lexer::LexState_lex_advance(current);
} else {
if (lexer::LexState_current(current) == mlc::String("\\")){
current = lexer::LexState_lex_advance(current);
if (lexer::LexState_eof(current)){
error = mlc::String("unterminated character literal");
} else {
mlc::String mapped = lexer::map_escape(lexer::LexState_current(current));
character_value = mapped != mlc::String("") ? mapped : lexer::LexState_current(current);
current = lexer::LexState_lex_advance(current);
}
} else {
character_value = lexer::LexState_current(current);
current = lexer::LexState_lex_advance(current);
}
}
}
}
if (error == mlc::String("") && !lexer::LexState_eof(current) && lexer::LexState_current(current) == mlc::String("'")){
{
current = lexer::LexState_lex_advance(current);
}
} else {
{
if (error == mlc::String("")){
error = mlc::String("unterminated character literal");
}
}
}
return cpp_lexer::CppScanCharR{current, cpp_tokens::cpp_token_make(cpp_tokens::CLChar(character_value), line, token_column), error};
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
return cpp_lexer::CppScanIntR{current, cpp_tokens::cpp_token_make(cpp_tokens::CLInt(value), line, token_column)};
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
}
if (!lexer::LexState_eof(current)){
{
current = lexer::LexState_lex_advance(current);
}
}
return cpp_lexer::CppScanStrR{current, cpp_tokens::cpp_token_make(cpp_tokens::CLStr(parts.join(mlc::String(""))), line, token_column), error};
}

cpp_lexer::CppScanOpR cpp_scan_op_single(lexer::LexState state, mlc::String character, int line, int column) noexcept{return character == mlc::String("(") ? cpp_lexer::CppScanOpR{lexer::LexState_lex_advance(state), cpp_tokens::cpp_token_make((cpp_tokens::CLParen{}), line, column)} : character == mlc::String(")") ? cpp_lexer::CppScanOpR{lexer::LexState_lex_advance(state), cpp_tokens::cpp_token_make((cpp_tokens::CRParen{}), line, column)} : character == mlc::String("{") ? cpp_lexer::CppScanOpR{lexer::LexState_lex_advance(state), cpp_tokens::cpp_token_make((cpp_tokens::CLBrace{}), line, column)} : character == mlc::String("}") ? cpp_lexer::CppScanOpR{lexer::LexState_lex_advance(state), cpp_tokens::cpp_token_make((cpp_tokens::CRBrace{}), line, column)} : character == mlc::String("[") ? cpp_lexer::CppScanOpR{lexer::LexState_lex_advance(state), cpp_tokens::cpp_token_make((cpp_tokens::CLBracket{}), line, column)} : character == mlc::String("]") ? cpp_lexer::CppScanOpR{lexer::LexState_lex_advance(state), cpp_tokens::cpp_token_make((cpp_tokens::CRBracket{}), line, column)} : cpp_lexer::CppScanOpR{lexer::LexState_lex_advance(state), cpp_tokens::cpp_token_make(cpp_tokens::COp(character), line, column)};}

cpp_lexer::CppScanOpR cpp_scan_op_two_character(lexer::LexState state, mlc::String character, mlc::String next_char, int line, int column) noexcept{return character == mlc::String("-") ? next_char == mlc::String(">") ? cpp_lexer::CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::cpp_token_make((cpp_tokens::CArrow{}), line, column)} : next_char == mlc::String("-") ? cpp_lexer::CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::cpp_token_make((cpp_tokens::CDec{}), line, column)} : cpp_scan_op_single(state, character, line, column) : character == mlc::String(":") ? next_char == mlc::String(":") ? cpp_lexer::CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::cpp_token_make((cpp_tokens::CScope{}), line, column)} : cpp_scan_op_single(state, character, line, column) : character == mlc::String("=") ? next_char == mlc::String("=") ? cpp_lexer::CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::cpp_token_make((cpp_tokens::CEq{}), line, column)} : cpp_scan_op_single(state, character, line, column) : character == mlc::String("!") ? next_char == mlc::String("=") ? cpp_lexer::CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::cpp_token_make((cpp_tokens::CNe{}), line, column)} : cpp_scan_op_single(state, character, line, column) : character == mlc::String("<") ? next_char == mlc::String("=") ? cpp_lexer::CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::cpp_token_make((cpp_tokens::CLe{}), line, column)} : next_char == mlc::String("<") ? cpp_lexer::CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::cpp_token_make((cpp_tokens::CLShift{}), line, column)} : cpp_lexer::CppScanOpR{lexer::LexState_lex_advance(state), cpp_tokens::cpp_token_make((cpp_tokens::CLAngle{}), line, column)} : character == mlc::String(">") ? next_char == mlc::String("=") ? cpp_lexer::CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::cpp_token_make((cpp_tokens::CGe{}), line, column)} : next_char == mlc::String(">") ? cpp_lexer::CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::cpp_token_make((cpp_tokens::CRShift{}), line, column)} : cpp_lexer::CppScanOpR{lexer::LexState_lex_advance(state), cpp_tokens::cpp_token_make((cpp_tokens::CRAngle{}), line, column)} : character == mlc::String("&") ? next_char == mlc::String("&") ? cpp_lexer::CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::cpp_token_make((cpp_tokens::CAnd{}), line, column)} : cpp_scan_op_single(state, character, line, column) : character == mlc::String("|") ? next_char == mlc::String("|") ? cpp_lexer::CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::cpp_token_make((cpp_tokens::COr{}), line, column)} : cpp_scan_op_single(state, character, line, column) : character == mlc::String("+") ? next_char == mlc::String("+") ? cpp_lexer::CppScanOpR{lexer::LexState_lex_advance_by(state, 2), cpp_tokens::cpp_token_make((cpp_tokens::CInc{}), line, column)} : cpp_scan_op_single(state, character, line, column) : cpp_scan_op_single(state, character, line, column);}

cpp_lexer::CppScanOpR cpp_scan_op(lexer::LexState state) noexcept{
int line = state.line;
int token_column = state.column;
mlc::String character = lexer::LexState_current(state);
mlc::String next_char = lexer::LexState_peek(state, 1);
return cpp_scan_op_two_character(state, character, next_char, line, token_column);
}

lexer::LexState cpp_push_token_with_trivia(mlc::Array<cpp_tokens::CppToken>& tokens, cpp_tokens::CppToken token, mlc::String leading_trivia, lexer::LexState lexer_state) noexcept{
cpp_lexer::CppTriviaScan trailing_scan = cpp_collect_trailing_trivia(lexer_state);
tokens.push_back(cpp_tokens::cpp_token_with_trivia(token, leading_trivia, trailing_scan.trivia));
return trailing_scan.state;
}

lexer::LexState push_ident_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens, mlc::String leading_trivia) noexcept{
cpp_lexer::CppScanIdR scan_result = cpp_scan_ident(state);
return cpp_push_token_with_trivia(tokens, scan_result.token, leading_trivia, scan_result.state);
}

lexer::LexState push_int_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens, mlc::String leading_trivia) noexcept{
cpp_lexer::CppScanIntR scan_result = cpp_scan_int(state);
return cpp_push_token_with_trivia(tokens, scan_result.token, leading_trivia, scan_result.state);
}

lexer::LexState push_string_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens, mlc::Array<mlc::String>& errors, mlc::String leading_trivia) noexcept{
cpp_lexer::CppScanStrR scan_result = cpp_scan_string(state);
if (scan_result.error != mlc::String("")){
{
errors.push_back(scan_result.error);
}
}
return cpp_push_token_with_trivia(tokens, scan_result.token, leading_trivia, scan_result.state);
}

lexer::LexState push_number_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens, mlc::String leading_trivia) noexcept{
cpp_lexer::CppScanNumberR scan_result = cpp_scan_number(state);
return cpp_push_token_with_trivia(tokens, scan_result.token, leading_trivia, scan_result.state);
}

lexer::LexState push_char_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens, mlc::Array<mlc::String>& errors, mlc::String leading_trivia) noexcept{
cpp_lexer::CppScanCharR scan_result = cpp_scan_char(state);
if (scan_result.error != mlc::String("")){
{
errors.push_back(scan_result.error);
}
}
return cpp_push_token_with_trivia(tokens, scan_result.token, leading_trivia, scan_result.state);
}

lexer::LexState push_hash_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens, mlc::String leading_trivia) noexcept{
cpp_tokens::CppToken token = cpp_tokens::cpp_token_make((cpp_tokens::CHash{}), state.line, state.column);
return cpp_push_token_with_trivia(tokens, token, leading_trivia, lexer::LexState_lex_advance(state));
}

lexer::LexState push_operator_scan(lexer::LexState state, mlc::Array<cpp_tokens::CppToken>& tokens, mlc::String leading_trivia) noexcept{
cpp_lexer::CppScanOpR scan_result = cpp_scan_op(state);
return cpp_push_token_with_trivia(tokens, scan_result.token, leading_trivia, scan_result.state);
}

cpp_lexer::CppLexOut cpp_tokenize(mlc::String input_text) noexcept{
lexer::LexState state = lexer::LexState{input_text, 0, 1, 1};
mlc::Array<cpp_tokens::CppToken> tokens = {};
mlc::Array<mlc::String> errors = {};
lexer::LexState lexer_state = std::move(state);
mlc::String pending_leading = mlc::String("");
while (!lexer::LexState_eof(lexer_state)){
{
cpp_lexer::CppTriviaScan leading_scan = cpp_collect_leading_trivia(lexer_state);
lexer_state = leading_scan.state;
pending_leading = pending_leading + leading_scan.trivia;
if (leading_scan.error != mlc::String("")){
{
errors.push_back(leading_scan.error);
}
}
if (lexer::LexState_eof(lexer_state)){
{
break;
}
}
mlc::String character = lexer::LexState_current(lexer_state);
mlc::String leading_trivia = pending_leading;
pending_leading = mlc::String("");
if (lexer::is_alpha(character)){
{
lexer_state = push_ident_scan(lexer_state, tokens, leading_trivia);
}
} else {
{
if (lexer::is_digit(character)){
lexer_state = push_number_scan(lexer_state, tokens, leading_trivia);
} else {
if (character == mlc::String(".") && lexer::is_digit(lexer::LexState_peek(lexer_state, 1))){
lexer_state = push_number_scan(lexer_state, tokens, leading_trivia);
} else {
if (character == mlc::String("\"")){
lexer_state = push_string_scan(lexer_state, tokens, errors, leading_trivia);
} else {
if (character == mlc::String("'")){
lexer_state = push_char_scan(lexer_state, tokens, errors, leading_trivia);
} else {
if (character == mlc::String("#")){
lexer_state = push_hash_scan(lexer_state, tokens, leading_trivia);
} else {
lexer_state = push_operator_scan(lexer_state, tokens, leading_trivia);
}
}
}
}
}
}
}
}
}
tokens.push_back(cpp_tokens::cpp_token_with_trivia(cpp_tokens::cpp_token_make((cpp_tokens::CEof{}), lexer_state.line, lexer_state.column), pending_leading, mlc::String("")));
return cpp_lexer::CppLexOut{tokens, errors};
}

bool cpp_lex_roundtrip_preserves_kinds(mlc::String source_text) noexcept{
mlc::Array<cpp_tokens::CppToken> first_tokens = cpp_tokenize(source_text).tokens;
mlc::String rebuilt_source = cpp_tokens::cpp_tokens_to_source(first_tokens);
mlc::Array<cpp_tokens::CppToken> second_tokens = cpp_tokenize(rebuilt_source).tokens;
return cpp_tokens::cpp_token_kinds_equal(first_tokens, second_tokens);
}

bool cpp_lex_roundtrip_preserves_source(mlc::String source_text) noexcept{return cpp_tokens::cpp_tokens_to_source(cpp_tokenize(source_text).tokens) == source_text;}

} // namespace cpp_lexer
