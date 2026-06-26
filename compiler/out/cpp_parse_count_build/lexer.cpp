#include "lexer.hpp"

#include "ast_tokens.hpp"

namespace lexer {

using namespace ast_tokens;

struct ScanResult {lexer::LexState after;ast_tokens::Token token;};

struct ScanStrResult {lexer::LexState after;ast_tokens::Token token;mlc::String error;};

struct SuffixScan {mlc::String suffix;lexer::LexState after;};

void lexer_internal_panic(mlc::String message) noexcept;

bool LexState_eof(lexer::LexState self) noexcept;

mlc::String LexState_current(lexer::LexState self) noexcept;

mlc::String LexState_peek(lexer::LexState self, int offset) noexcept;

lexer::LexState LexState_lex_advance(lexer::LexState self) noexcept;

lexer::LexState LexState_lex_advance_by(lexer::LexState self, int advance_count) noexcept;

ast_tokens::Token LexState_token(lexer::LexState self, ast_tokens::TokenKind token_kind) noexcept;

bool is_alpha(mlc::String character) noexcept;

bool is_digit(mlc::String character) noexcept;

bool is_alnum(mlc::String character) noexcept;

bool is_ws(mlc::String character) noexcept;

bool is_reserved_keyword(mlc::String word) noexcept;

ast_tokens::TokenKind reserved_keyword_kind(mlc::String word) noexcept;

ast_tokens::TokenKind keyword_kind(mlc::String word) noexcept;

lexer::ScanResult scan_ident(lexer::LexState state) noexcept;

bool is_alpha_lower(mlc::String character) noexcept;

lexer::SuffixScan try_scan_suffix(lexer::LexState state) noexcept;

lexer::ScanResult scan_int(lexer::LexState state) noexcept;

lexer::ScanStrResult scan_single_string(lexer::LexState state) noexcept;

mlc::String map_escape(mlc::String character) noexcept;

lexer::LexState lex_advance_past_unescaped_string_run(lexer::LexState state) noexcept;

lexer::LexState lex_advance_past_template_literal_run(lexer::LexState state) noexcept;

lexer::ScanStrResult scan_string(lexer::LexState state) noexcept;

lexer::LexState scan_template_nested_string(lexer::LexState initial, mlc::Array<mlc::String> expr_chars) noexcept;

lexer::ScanStrResult scan_template(lexer::LexState state) noexcept;

lexer::SkipResult skip_whitespace(lexer::LexState state) noexcept;

lexer::ScanResult scan_op_two_character(lexer::LexState state, mlc::String character, mlc::String next) noexcept;

lexer::ScanResult scan_op_single(mlc::String character, lexer::LexState state) noexcept;

lexer::ScanResult scan_op(lexer::LexState state) noexcept;

std::tuple<> accumulate_nonempty_scan_message_into_errors(mlc::Array<mlc::String>& error_messages, mlc::String message_under_scan) noexcept;

lexer::LexState apply_skip_whitespace(lexer::LexState state, mlc::Array<mlc::String>& error_messages) noexcept;

lexer::LexState push_ident_scan(lexer::LexState state, mlc::Array<ast_tokens::Token>& tokens) noexcept;

lexer::LexState push_int_scan(lexer::LexState state, mlc::Array<ast_tokens::Token>& tokens) noexcept;

lexer::LexState push_string_scan(lexer::LexState state, mlc::Array<ast_tokens::Token>& tokens, mlc::Array<mlc::String>& error_messages) noexcept;

lexer::LexState push_single_string_scan(lexer::LexState state, mlc::Array<ast_tokens::Token>& tokens, mlc::Array<mlc::String>& error_messages) noexcept;

lexer::LexState push_template_scan(lexer::LexState state, mlc::Array<ast_tokens::Token>& tokens, mlc::Array<mlc::String>& error_messages) noexcept;

lexer::LexState push_operator_scan(lexer::LexState state, mlc::Array<ast_tokens::Token>& tokens) noexcept;

ast_tokens::LexOut tokenize(mlc::String input_text) noexcept;

void lexer_internal_panic(mlc::String message) noexcept{
mlc::io::println(mlc::String("lexer panic: ") + message);
mlc::io::exit(1);
return;
}

bool LexState_eof(lexer::LexState self) noexcept{return self.position >= self.source.byte_size();}

mlc::String LexState_current(lexer::LexState self) noexcept{return LexState_eof(self) ? mlc::String("\0", 1) : self.source.byte_at(self.position);}

mlc::String LexState_peek(lexer::LexState self, int offset) noexcept{
int byte_index = self.position + offset;
return byte_index < 0 ? [&]() -> mlc::String { 
  lexer_internal_panic(mlc::String("lexer peek before start of source"));
  return mlc::String("\0", 1);
 }() : byte_index >= self.source.byte_size() ? mlc::String("\0", 1) : self.source.byte_at(byte_index);
}

lexer::LexState LexState_lex_advance(lexer::LexState self) noexcept{return LexState_current(self) == mlc::String("\n") ? lexer::LexState{self.source, self.position + 1, self.line + 1, 1} : lexer::LexState{self.source, self.position + 1, self.line, self.column + 1};}

lexer::LexState LexState_lex_advance_by(lexer::LexState self, int advance_count) noexcept{return advance_count <= 0 ? self : LexState_lex_advance_by(LexState_lex_advance(self), advance_count - 1);}

ast_tokens::Token LexState_token(lexer::LexState self, ast_tokens::TokenKind token_kind) noexcept{return ast_tokens::Token{token_kind, self.line, self.column};}

bool is_alpha(mlc::String character) noexcept{return character >= mlc::String("a") && character <= mlc::String("z") || character >= mlc::String("A") && character <= mlc::String("Z") || character == mlc::String("_");}

bool is_digit(mlc::String character) noexcept{return character >= mlc::String("0") && character <= mlc::String("9");}

bool is_alnum(mlc::String character) noexcept{return is_alpha(character) || is_digit(character);}

bool is_ws(mlc::String character) noexcept{return character == mlc::String(" ") || character == mlc::String("\t") || character == mlc::String("\r") || character == mlc::String("\n");}

bool is_reserved_keyword(mlc::String word) noexcept{return [&]() { if (word == mlc::String("fn")) { return true; } if (word == mlc::String("type")) { return true; } if (word == mlc::String("let")) { return true; } if (word == mlc::String("mut")) { return true; } if (word == mlc::String("const")) { return true; } if (word == mlc::String("return")) { return true; } if (word == mlc::String("break")) { return true; } if (word == mlc::String("continue")) { return true; } if (word == mlc::String("if")) { return true; } if (word == mlc::String("then")) { return true; } if (word == mlc::String("else")) { return true; } if (word == mlc::String("unless")) { return true; } if (word == mlc::String("while")) { return true; } if (word == mlc::String("for")) { return true; } if (word == mlc::String("in")) { return true; } if (word == mlc::String("do")) { return true; } if (word == mlc::String("end")) { return true; } if (word == mlc::String("match")) { return true; } if (word == mlc::String("with")) { return true; } if (word == mlc::String("import")) { return true; } if (word == mlc::String("from")) { return true; } if (word == mlc::String("as")) { return true; } if (word == mlc::String("extern")) { return true; } if (word == mlc::String("extend")) { return true; } if (word == mlc::String("where")) { return true; } if (word == mlc::String("true")) { return true; } if (word == mlc::String("false")) { return true; } return false; }();}

ast_tokens::TokenKind reserved_keyword_kind(mlc::String word) noexcept{return [&]() -> ast_tokens::TokenKind { if (word == mlc::String("fn")) { return (ast_tokens::KFn{}); } if (word == mlc::String("type")) { return (ast_tokens::KType{}); } if (word == mlc::String("let")) { return (ast_tokens::KLet{}); } if (word == mlc::String("mut")) { return (ast_tokens::KMut{}); } if (word == mlc::String("const")) { return (ast_tokens::KConst{}); } if (word == mlc::String("return")) { return (ast_tokens::KReturn{}); } if (word == mlc::String("break")) { return (ast_tokens::KBreak{}); } if (word == mlc::String("continue")) { return (ast_tokens::KContinue{}); } if (word == mlc::String("if")) { return (ast_tokens::KIf{}); } if (word == mlc::String("then")) { return (ast_tokens::KThen{}); } if (word == mlc::String("else")) { return (ast_tokens::KElse{}); } if (word == mlc::String("unless")) { return (ast_tokens::KUnless{}); } if (word == mlc::String("while")) { return (ast_tokens::KWhile{}); } if (word == mlc::String("for")) { return (ast_tokens::KFor{}); } if (word == mlc::String("in")) { return (ast_tokens::KIn{}); } if (word == mlc::String("do")) { return (ast_tokens::KDo{}); } if (word == mlc::String("end")) { return (ast_tokens::KEnd{}); } if (word == mlc::String("match")) { return (ast_tokens::KMatch{}); } if (word == mlc::String("with")) { return (ast_tokens::KWith{}); } if (word == mlc::String("import")) { return (ast_tokens::KImport{}); } if (word == mlc::String("from")) { return (ast_tokens::KFrom{}); } if (word == mlc::String("as")) { return (ast_tokens::KAs{}); } if (word == mlc::String("extern")) { return (ast_tokens::KExtern{}); } if (word == mlc::String("extend")) { return (ast_tokens::KExtend{}); } if (word == mlc::String("where")) { return (ast_tokens::KWhere{}); } if (word == mlc::String("true")) { return (ast_tokens::KTrue{}); } if (word == mlc::String("false")) { return (ast_tokens::KFalse{}); } }();}

ast_tokens::TokenKind keyword_kind(mlc::String word) noexcept{return is_reserved_keyword(word) ? ast_tokens::TokenKind(reserved_keyword_kind(word)) : ast_tokens::TokenKind(ast_tokens::Ident(word));}

lexer::ScanResult scan_ident(lexer::LexState state) noexcept{
int start = state.position;
mlc::String input_text = state.source;
int token_line = state.line;
int token_column = state.column;
lexer::LexState current = std::move(state);
while (!LexState_eof(current) && is_alnum(LexState_current(current))){
{
current = LexState_lex_advance(current);
}
}
mlc::String word = input_text.byte_substring(start, current.position - start);
return lexer::ScanResult{current, ast_tokens::Token{keyword_kind(word), token_line, token_column}};
}

bool is_alpha_lower(mlc::String character) noexcept{return character >= mlc::String("a") && character <= mlc::String("z");}

lexer::SuffixScan try_scan_suffix(lexer::LexState state) noexcept{
mlc::Array<mlc::String> numeric_type_suffixes = mlc::Array<mlc::String>{mlc::String("i64"), mlc::String("u8"), mlc::String("usize"), mlc::String("f64"), mlc::String("f32")};
return !LexState_eof(state) && is_alpha_lower(LexState_current(state)) ? [&]() -> lexer::SuffixScan { 
  int start = state.position;
  int orig_line = state.line;
  int original_column = state.column;
  lexer::LexState current_state = std::move(state);
  while (!LexState_eof(current_state) && is_alpha_lower(LexState_current(current_state)) || is_digit(LexState_current(current_state))){
{
current_state = LexState_lex_advance(current_state);
}
}
  mlc::String candidate = current_state.source.byte_substring(start, current_state.position - start);
  return numeric_type_suffixes.any([candidate](mlc::String suffix_under_scan) mutable { return suffix_under_scan == candidate; }) ? lexer::SuffixScan{candidate, current_state} : [&]() -> lexer::SuffixScan { 
  lexer::LexState back = lexer::LexState{current_state.source, start, orig_line, original_column};
  return lexer::SuffixScan{mlc::String(""), back};
 }();
 }() : lexer::SuffixScan{mlc::String(""), state};
}

lexer::ScanResult scan_int(lexer::LexState state) noexcept{
int token_line = state.line;
int token_column = state.column;
int int_start = state.position;
lexer::LexState current = std::move(state);
int value = 0;
while (!LexState_eof(current) && is_digit(LexState_current(current))){
{
value = value * 10 + LexState_current(current).to_i();
current = LexState_lex_advance(current);
}
}
return !LexState_eof(current) && LexState_current(current) == mlc::String(".") && !LexState_eof(LexState_lex_advance(current)) && is_digit(LexState_current(LexState_lex_advance(current))) ? [&]() -> lexer::ScanResult { 
  current = LexState_lex_advance(current);
  while (!LexState_eof(current) && is_digit(LexState_current(current))){
{
current = LexState_lex_advance(current);
}
}
  mlc::String raw_float = current.source.byte_substring(int_start, current.position - int_start);
  auto __tmp_0 = try_scan_suffix(current);
  auto after = __tmp_0.after;
  return lexer::ScanResult{after, ast_tokens::Token{ast_tokens::LFloat(raw_float), token_line, token_column}};
 }() : [&]() -> lexer::ScanResult { 
  auto __tmp_1 = try_scan_suffix(current);
  auto suffix = __tmp_1.suffix;
  auto after = __tmp_1.after;
  ast_tokens::TokenKind token_kind = [&]() -> ast_tokens::TokenKind { if (suffix == mlc::String("i64")) { return ast_tokens::LI64(mlc::to_string(value)); } if (suffix == mlc::String("u8")) { return ast_tokens::LU8(mlc::to_string(value)); } if (suffix == mlc::String("usize")) { return ast_tokens::LUsize(mlc::to_string(value)); } return ast_tokens::LInt(value); }();
  return lexer::ScanResult{after, ast_tokens::Token{token_kind, token_line, token_column}};
 }();
}

lexer::ScanStrResult scan_single_string(lexer::LexState state) noexcept{
int token_line = state.line;
int token_column = state.column;
lexer::LexState current = LexState_lex_advance(state);
mlc::Array<mlc::String> parts = {};
mlc::String error = mlc::String("");
while (!LexState_eof(current) && LexState_current(current) != mlc::String("'")){
{
if (LexState_current(current) == mlc::String("\\")){
{
current = LexState_lex_advance(current);
if (LexState_eof(current)){
error = mlc::String("unterminated escape in single-quoted string");
} else {
if (LexState_current(current) == mlc::String("\\")){
parts.push_back(mlc::String("\\"));
current = LexState_lex_advance(current);
} else {
if (LexState_current(current) == mlc::String("'")){
parts.push_back(mlc::String("'"));
current = LexState_lex_advance(current);
} else {
if (LexState_current(current) == mlc::String("n")){
parts.push_back(mlc::String("\n"));
current = LexState_lex_advance(current);
} else {
if (LexState_current(current) == mlc::String("t")){
parts.push_back(mlc::String("\t"));
current = LexState_lex_advance(current);
} else {
if (LexState_current(current) == mlc::String("r")){
parts.push_back(mlc::String("\r"));
current = LexState_lex_advance(current);
} else {
if (LexState_current(current) == mlc::String("0")){
parts.push_back(mlc::String("\0", 1));
current = LexState_lex_advance(current);
} else {
parts.push_back(mlc::String("\\"));
parts.push_back(LexState_current(current));
current = LexState_lex_advance(current);
}
}
}
}
}
}
}
}
} else {
{
parts.push_back(LexState_current(current));
current = LexState_lex_advance(current);
}
}
}
}
if (LexState_eof(current)){
{
error = mlc::String("unterminated single-quoted string");
}
} else {
{
current = LexState_lex_advance(current);
}
}
mlc::String joined = parts.join(mlc::String(""));
ast_tokens::TokenKind token_kind = joined.length() == 1 ? ast_tokens::TokenKind(ast_tokens::LChar(joined)) : ast_tokens::TokenKind(ast_tokens::LStr(joined));
return lexer::ScanStrResult{current, ast_tokens::Token{token_kind, token_line, token_column}, error};
}

mlc::String map_escape(mlc::String character) noexcept{return [&]() -> mlc::String { if (character == mlc::String("n")) { return mlc::String("\n"); } if (character == mlc::String("t")) { return mlc::String("\t"); } if (character == mlc::String("r")) { return mlc::String("\r"); } if (character == mlc::String("\"")) { return mlc::String("\""); } if (character == mlc::String("\\")) { return mlc::String("\\"); } if (character == mlc::String("0")) { return mlc::String("\0", 1); } if (character == mlc::String("{")) { return mlc::String("{"); } if (character == mlc::String("}")) { return mlc::String("}"); } return mlc::String(""); }();}

lexer::LexState lex_advance_past_unescaped_string_run(lexer::LexState state) noexcept{
lexer::LexState current = std::move(state);
while (!LexState_eof(current) && LexState_current(current) != mlc::String("\"") && LexState_current(current) != mlc::String("\\")){
{
current = LexState_lex_advance(current);
}
}
return current;
}

lexer::LexState lex_advance_past_template_literal_run(lexer::LexState state) noexcept{
lexer::LexState current = std::move(state);
bool run_active = true;
while (run_active && !LexState_eof(current) && LexState_current(current) != mlc::String("`")){
{
if (LexState_current(current) == mlc::String("\\")){
{
run_active = false;
}
} else {
{
if (LexState_current(current) == mlc::String("$") && LexState_peek(current, 1) == mlc::String("{")){
run_active = false;
} else {
current = LexState_lex_advance(current);
}
}
}
}
}
return current;
}

lexer::ScanStrResult scan_string(lexer::LexState state) noexcept{
int token_line = state.line;
int token_column = state.column;
lexer::LexState current = LexState_lex_advance(state);
mlc::Array<mlc::String> parts = {};
mlc::String error = mlc::String("");
while (!LexState_eof(current) && LexState_current(current) != mlc::String("\"")){
{
if (LexState_current(current) == mlc::String("\\")){
{
current = LexState_lex_advance(current);
if (LexState_eof(current)){
error = mlc::String("dangling escape at end of string");
} else {
mlc::String mapped = map_escape(LexState_current(current));
if (mapped != mlc::String("")){
{
parts.push_back(mapped);
}
} else {
{
parts.push_back(mlc::String("\\"));
parts.push_back(LexState_current(current));
}
}
current = LexState_lex_advance(current);
}
}
} else {
{
int run_start = current.position;
current = lex_advance_past_unescaped_string_run(current);
if (current.position > run_start){
parts.push_back(current.source.byte_substring(run_start, current.position - run_start));
}
}
}
}
}
if (LexState_eof(current)){
{
error = mlc::String("unterminated string");
}
} else {
{
current = LexState_lex_advance(current);
}
}
return lexer::ScanStrResult{current, ast_tokens::Token{ast_tokens::LStr(parts.join(mlc::String(""))), token_line, token_column}, error};
}

lexer::LexState scan_template_nested_string(lexer::LexState initial, mlc::Array<mlc::String> expr_chars) noexcept{
lexer::LexState current = LexState_lex_advance(initial);
while (!LexState_eof(current) && LexState_current(current) != mlc::String("\"")){
{
if (LexState_current(current) == mlc::String("\\") && !LexState_eof(LexState_lex_advance(current))){
{
expr_chars.push_back(LexState_current(current));
current = LexState_lex_advance(current);
expr_chars.push_back(LexState_current(current));
current = LexState_lex_advance(current);
}
} else {
{
expr_chars.push_back(LexState_current(current));
current = LexState_lex_advance(current);
}
}
}
}
if (!LexState_eof(current)){
{
expr_chars.push_back(LexState_current(current));
current = LexState_lex_advance(current);
}
}
return current;
}

lexer::ScanStrResult scan_template(lexer::LexState state) noexcept{
int token_line = state.line;
int token_column = state.column;
lexer::LexState current = LexState_lex_advance(state);
mlc::Array<mlc::String> parts = {};
mlc::Array<mlc::String> current_lit = {};
mlc::String error = mlc::String("");
while (!LexState_eof(current) && LexState_current(current) != mlc::String("`")){
{
if (LexState_current(current) == mlc::String("\\") && !LexState_eof(LexState_lex_advance(current))){
{
mlc::String next = LexState_current(LexState_lex_advance(current));
current = LexState_lex_advance(current);
if (next == mlc::String("`")){
current_lit.push_back(mlc::String("`"));
current = LexState_lex_advance(current);
} else {
if (next == mlc::String("$") && !LexState_eof(LexState_lex_advance(current)) && LexState_current(LexState_lex_advance(current)) == mlc::String("{")){
current_lit.push_back(mlc::String("$"));
current = LexState_lex_advance(current);
current_lit.push_back(mlc::String("{"));
current = LexState_lex_advance(current);
} else {
if (next == mlc::String("n")){
current_lit.push_back(mlc::String("\n"));
current = LexState_lex_advance(current);
} else {
if (next == mlc::String("t")){
current_lit.push_back(mlc::String("\t"));
current = LexState_lex_advance(current);
} else {
if (next == mlc::String("r")){
current_lit.push_back(mlc::String("\r"));
current = LexState_lex_advance(current);
} else {
if (next == mlc::String("\\")){
current_lit.push_back(mlc::String("\\"));
current = LexState_lex_advance(current);
} else {
current_lit.push_back(mlc::String("\\"));
current_lit.push_back(next);
current = LexState_lex_advance(current);
}
}
}
}
}
}
}
} else {
{
if (LexState_current(current) == mlc::String("$") && !LexState_eof(LexState_lex_advance(current)) && LexState_current(LexState_lex_advance(current)) == mlc::String("{")){
parts.push_back(current_lit.join(mlc::String("")));
current_lit = {};
current = LexState_lex_advance(LexState_lex_advance(current));
mlc::Array<mlc::String> expr_chars = {};
int depth = 1;
while (!LexState_eof(current) && depth > 0){
{
if (LexState_current(current) == mlc::String("{")){
{
depth = depth + 1;
expr_chars.push_back(LexState_current(current));
current = LexState_lex_advance(current);
}
} else {
{
if (LexState_current(current) == mlc::String("}")){
depth = depth - 1;
if (depth > 0){
{
expr_chars.push_back(LexState_current(current));
}
}
current = LexState_lex_advance(current);
} else {
if (LexState_current(current) == mlc::String("\"")){
expr_chars.push_back(LexState_current(current));
current = scan_template_nested_string(current, expr_chars);
} else {
expr_chars.push_back(LexState_current(current));
current = LexState_lex_advance(current);
}
}
}
}
}
}
if (depth > 0){
{
error = mlc::String("unterminated interpolation in template literal");
}
}
parts.push_back(expr_chars.join(mlc::String("")));
} else {
int run_start = current.position;
current = lex_advance_past_template_literal_run(current);
if (current.position > run_start){
current_lit.push_back(current.source.byte_substring(run_start, current.position - run_start));
}
}
}
}
}
}
if (LexState_eof(current)){
{
error = mlc::String("unterminated template literal");
}
} else {
{
current = LexState_lex_advance(current);
}
}
parts.push_back(current_lit.join(mlc::String("")));
return lexer::ScanStrResult{current, ast_tokens::Token{ast_tokens::LTemplate(parts), token_line, token_column}, error};
}

lexer::SkipResult skip_whitespace(lexer::LexState state) noexcept{
lexer::LexState current = std::move(state);
mlc::String error = mlc::String("");
while (!LexState_eof(current)){
{
mlc::String character = LexState_current(current);
if (is_ws(character)){
{
current = LexState_lex_advance(current);
}
} else {
{
if (character == mlc::String("/") && LexState_peek(current, 1) == mlc::String("/")){
current = LexState_lex_advance_by(current, 2);
[&]() { 
  while (!LexState_eof(current) && LexState_current(current) != mlc::String("\n")){
{
current = LexState_lex_advance(current);
}
}
 }();
} else {
if (character == mlc::String("/") && LexState_peek(current, 1) == mlc::String("*")){
current = LexState_lex_advance_by(current, 2);
bool comment_closed = false;
while (!LexState_eof(current)){
{
if (LexState_current(current) == mlc::String("*") && LexState_peek(current, 1) == mlc::String("/")){
{
current = LexState_lex_advance_by(current, 2);
comment_closed = true;
break;
}
}
current = LexState_lex_advance(current);
}
}
if (!comment_closed){
error = mlc::String("unterminated block comment");
}
} else {
break;
}
}
}
}
}
}
return lexer::SkipResult{current, error};
}

lexer::ScanResult scan_op_two_character(lexer::LexState state, mlc::String character, mlc::String next) noexcept{return [&]() -> lexer::ScanResult { if (character == mlc::String("-")) { return [&]() -> lexer::ScanResult { if (next == mlc::String(">")) { return lexer::ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, (ast_tokens::Arrow{}))}; } return scan_op_single(character, state); }(); } if (character == mlc::String("=")) { return [&]() -> lexer::ScanResult { if (next == mlc::String(">")) { return lexer::ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, (ast_tokens::FatArrow{}))}; } if (next == mlc::String("=")) { return lexer::ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::Op(mlc::String("==")))}; } return lexer::ScanResult{LexState_lex_advance(state), LexState_token(state, (ast_tokens::Equal{}))}; }(); } if (character == mlc::String("|")) { return [&]() -> lexer::ScanResult { if (next == mlc::String(">")) { return lexer::ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, (ast_tokens::Pipe{}))}; } if (next == mlc::String("|")) { return lexer::ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::Op(mlc::String("||")))}; } return lexer::ScanResult{LexState_lex_advance(state), LexState_token(state, (ast_tokens::Bar{}))}; }(); } if (character == mlc::String("!")) { return [&]() -> lexer::ScanResult { if (next == mlc::String("=")) { return lexer::ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::Op(mlc::String("!=")))}; } return scan_op_single(character, state); }(); } if (character == mlc::String("<")) { return [&]() -> lexer::ScanResult { if (next == mlc::String("=")) { return lexer::ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::Op(mlc::String("<=")))}; } return scan_op_single(character, state); }(); } if (character == mlc::String(">")) { return [&]() -> lexer::ScanResult { if (next == mlc::String("=")) { return lexer::ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::Op(mlc::String(">=")))}; } return scan_op_single(character, state); }(); } if (character == mlc::String("&")) { return [&]() -> lexer::ScanResult { if (next == mlc::String("&")) { return lexer::ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::Op(mlc::String("&&")))}; } return scan_op_single(character, state); }(); } return scan_op_single(character, state); }();}

lexer::ScanResult scan_op_single(mlc::String character, lexer::LexState state) noexcept{return [&]() -> lexer::ScanResult { if (character == mlc::String("=")) { return lexer::ScanResult{LexState_lex_advance(state), LexState_token(state, (ast_tokens::Equal{}))}; } if (character == mlc::String("|")) { return lexer::ScanResult{LexState_lex_advance(state), LexState_token(state, (ast_tokens::Bar{}))}; } if (character == mlc::String("?")) { return lexer::ScanResult{LexState_lex_advance(state), LexState_token(state, (ast_tokens::Question{}))}; } if (character == mlc::String(".")) { return lexer::ScanResult{LexState_lex_advance(state), LexState_token(state, (ast_tokens::Dot{}))}; } if (character == mlc::String("(")) { return lexer::ScanResult{LexState_lex_advance(state), LexState_token(state, (ast_tokens::LParen{}))}; } if (character == mlc::String(")")) { return lexer::ScanResult{LexState_lex_advance(state), LexState_token(state, (ast_tokens::RParen{}))}; } if (character == mlc::String("{")) { return lexer::ScanResult{LexState_lex_advance(state), LexState_token(state, (ast_tokens::LBrace{}))}; } if (character == mlc::String("}")) { return lexer::ScanResult{LexState_lex_advance(state), LexState_token(state, (ast_tokens::RBrace{}))}; } if (character == mlc::String("[")) { return lexer::ScanResult{LexState_lex_advance(state), LexState_token(state, (ast_tokens::LBracket{}))}; } if (character == mlc::String("]")) { return lexer::ScanResult{LexState_lex_advance(state), LexState_token(state, (ast_tokens::RBracket{}))}; } if (character == mlc::String(",")) { return lexer::ScanResult{LexState_lex_advance(state), LexState_token(state, (ast_tokens::Comma{}))}; } if (character == mlc::String(";")) { return lexer::ScanResult{LexState_lex_advance(state), LexState_token(state, (ast_tokens::Semicolon{}))}; } if (character == mlc::String(":")) { return lexer::ScanResult{LexState_lex_advance(state), LexState_token(state, (ast_tokens::Colon{}))}; } return lexer::ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::Op(character))}; }();}

lexer::ScanResult scan_op(lexer::LexState state) noexcept{
mlc::String character = LexState_current(state);
mlc::String next = LexState_peek(state, 1);
return character == mlc::String(".") && next == mlc::String(".") && LexState_peek(state, 2) == mlc::String(".") ? lexer::ScanResult{LexState_lex_advance_by(state, 3), LexState_token(state, (ast_tokens::Spread{}))} : scan_op_two_character(state, character, next);
}

std::tuple<> accumulate_nonempty_scan_message_into_errors(mlc::Array<mlc::String>& error_messages, mlc::String message_under_scan) noexcept{
if (message_under_scan != mlc::String("")){
{
error_messages.push_back(message_under_scan);
}
} else {
{
std::make_tuple();
}
}
return std::make_tuple();
}

lexer::LexState apply_skip_whitespace(lexer::LexState state, mlc::Array<mlc::String>& error_messages) noexcept{
auto __tmp_2 = skip_whitespace(state);
auto after = __tmp_2.after;
auto error = __tmp_2.error;
accumulate_nonempty_scan_message_into_errors(error_messages, error);
return after;
}

lexer::LexState push_ident_scan(lexer::LexState state, mlc::Array<ast_tokens::Token>& tokens) noexcept{
auto __tmp_3 = scan_ident(state);
auto after = __tmp_3.after;
auto token = __tmp_3.token;
tokens.push_back(token);
return after;
}

lexer::LexState push_int_scan(lexer::LexState state, mlc::Array<ast_tokens::Token>& tokens) noexcept{
auto __tmp_4 = scan_int(state);
auto after = __tmp_4.after;
auto token = __tmp_4.token;
tokens.push_back(token);
return after;
}

lexer::LexState push_string_scan(lexer::LexState state, mlc::Array<ast_tokens::Token>& tokens, mlc::Array<mlc::String>& error_messages) noexcept{
auto __tmp_5 = scan_string(state);
auto after = __tmp_5.after;
auto token = __tmp_5.token;
auto error = __tmp_5.error;
tokens.push_back(token);
accumulate_nonempty_scan_message_into_errors(error_messages, error);
return after;
}

lexer::LexState push_single_string_scan(lexer::LexState state, mlc::Array<ast_tokens::Token>& tokens, mlc::Array<mlc::String>& error_messages) noexcept{
auto __tmp_6 = scan_single_string(state);
auto after = __tmp_6.after;
auto token = __tmp_6.token;
auto error = __tmp_6.error;
tokens.push_back(token);
accumulate_nonempty_scan_message_into_errors(error_messages, error);
return after;
}

lexer::LexState push_template_scan(lexer::LexState state, mlc::Array<ast_tokens::Token>& tokens, mlc::Array<mlc::String>& error_messages) noexcept{
auto __tmp_7 = scan_template(state);
auto after = __tmp_7.after;
auto token = __tmp_7.token;
auto error = __tmp_7.error;
tokens.push_back(token);
accumulate_nonempty_scan_message_into_errors(error_messages, error);
return after;
}

lexer::LexState push_operator_scan(lexer::LexState state, mlc::Array<ast_tokens::Token>& tokens) noexcept{
auto __tmp_8 = scan_op(state);
auto after = __tmp_8.after;
auto token = __tmp_8.token;
tokens.push_back(token);
return after;
}

ast_tokens::LexOut tokenize(mlc::String input_text) noexcept{
lexer::LexState lexer_state = lexer::LexState{input_text, 0, 1, 1};
mlc::Array<ast_tokens::Token> tokens = {};
mlc::Array<mlc::String> errors = {};
while (!LexState_eof(lexer_state)){
{
int position_before_token = lexer_state.position;
lexer_state = apply_skip_whitespace(lexer_state, errors);
if (LexState_eof(lexer_state)){
{
break;
}
}
mlc::String character = LexState_current(lexer_state);
is_alpha(character) ? [&]() -> std::tuple<> { 
  lexer_state = push_ident_scan(lexer_state, tokens);
  return std::make_tuple();
 }() : is_digit(character) ? [&]() -> std::tuple<> { 
  lexer_state = push_int_scan(lexer_state, tokens);
  return std::make_tuple();
 }() : character == mlc::String("\"") ? [&]() -> std::tuple<> { 
  lexer_state = push_string_scan(lexer_state, tokens, errors);
  return std::make_tuple();
 }() : character == mlc::String("'") ? [&]() -> std::tuple<> { 
  lexer_state = push_single_string_scan(lexer_state, tokens, errors);
  return std::make_tuple();
 }() : character == mlc::String("`") ? [&]() -> std::tuple<> { 
  lexer_state = push_template_scan(lexer_state, tokens, errors);
  return std::make_tuple();
 }() : [&]() -> std::tuple<> { 
  lexer_state = push_operator_scan(lexer_state, tokens);
  return std::make_tuple();
 }();
if (lexer_state.position == position_before_token){
{
lexer_internal_panic(mlc::String("tokenize made no progress at byte ") + mlc::to_string(lexer_state.position));
}
}
}
}
tokens.push_back(LexState_token(lexer_state, (ast_tokens::Eof{})));
return ast_tokens::LexOut{tokens, errors};
}

} // namespace lexer
