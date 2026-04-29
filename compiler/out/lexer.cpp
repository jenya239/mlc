#include "lexer.hpp"

#include "ast_tokens.hpp"

namespace lexer {

using namespace ast_tokens;

struct LexState {mlc::String src;int pos;int line;int col;};

struct ScanResult {lexer::LexState state;ast_tokens::Token token;};

struct ScanStrResult {lexer::LexState state;ast_tokens::Token token;mlc::String error;};

struct SuffixScan {mlc::String suffix;lexer::LexState after;};

bool LexState_eof(lexer::LexState self) noexcept;

mlc::String LexState_current(lexer::LexState self) noexcept;

mlc::String LexState_peek(lexer::LexState self, int offset) noexcept;

lexer::LexState LexState_lex_advance(lexer::LexState self) noexcept;

lexer::LexState LexState_lex_advance_by(lexer::LexState self, int count) noexcept;

ast_tokens::Token LexState_token(lexer::LexState self, ast_tokens::TKind tk) noexcept;

bool is_alpha(mlc::String character) noexcept;

bool is_digit(mlc::String character) noexcept;

bool is_alnum(mlc::String character) noexcept;

bool is_ws(mlc::String character) noexcept;

ast_tokens::TKind keyword_kind(mlc::String word) noexcept;

lexer::ScanResult scan_ident(lexer::LexState state) noexcept;

bool is_alpha_lower(mlc::String c) noexcept;

lexer::SuffixScan try_scan_suffix(lexer::LexState state) noexcept;

lexer::ScanResult scan_int(lexer::LexState state) noexcept;

lexer::ScanStrResult scan_single_string(lexer::LexState state) noexcept;

mlc::String map_escape(mlc::String character) noexcept;

lexer::ScanStrResult scan_string(lexer::LexState state) noexcept;

lexer::LexState scan_template_nested_string(lexer::LexState initial, mlc::Array<mlc::String> expr_chars) noexcept;

lexer::ScanStrResult scan_template(lexer::LexState state) noexcept;

lexer::LexState skip_whitespace(lexer::LexState state) noexcept;

lexer::ScanResult scan_op(lexer::LexState state) noexcept;

ast_tokens::LexOut tokenize(mlc::String source) noexcept;

bool LexState_eof(lexer::LexState self) noexcept{return self.pos >= self.src.byte_size();}

mlc::String LexState_current(lexer::LexState self) noexcept{return LexState_eof(self) ? mlc::String("\0", 1) : self.src.byte_at(self.pos);}

mlc::String LexState_peek(lexer::LexState self, int offset) noexcept{
int position = self.pos + offset;
return position >= self.src.byte_size() ? mlc::String("\0", 1) : self.src.byte_at(position);
}

lexer::LexState LexState_lex_advance(lexer::LexState self) noexcept{return LexState_current(self) == mlc::String("\n") ? lexer::LexState{self.src, self.pos + 1, self.line + 1, 1} : lexer::LexState{self.src, self.pos + 1, self.line, self.col + 1};}

lexer::LexState LexState_lex_advance_by(lexer::LexState self, int count) noexcept{
lexer::LexState state = std::move(self);
int i = 0;
while (i < count){
{
state = LexState_lex_advance(state);
i = i + 1;
}
}
return state;
}

ast_tokens::Token LexState_token(lexer::LexState self, ast_tokens::TKind tk) noexcept{return ast_tokens::Token{tk, self.line, self.col};}

bool is_alpha(mlc::String character) noexcept{return character >= mlc::String("a") && character <= mlc::String("z") || character >= mlc::String("A") && character <= mlc::String("Z") || character == mlc::String("_");}

bool is_digit(mlc::String character) noexcept{return character >= mlc::String("0") && character <= mlc::String("9");}

bool is_alnum(mlc::String character) noexcept{return is_alpha(character) || is_digit(character);}

bool is_ws(mlc::String character) noexcept{return character == mlc::String(" ") || character == mlc::String("\t") || character == mlc::String("\r") || character == mlc::String("\n");}

ast_tokens::TKind keyword_kind(mlc::String word) noexcept{return word == mlc::String("fn") ? (ast_tokens::KFn{}) : word == mlc::String("type") ? ast_tokens::TKind((ast_tokens::KType{})) : ast_tokens::TKind(word == mlc::String("let") ? ast_tokens::TKind((ast_tokens::KLet{})) : ast_tokens::TKind(word == mlc::String("mut") ? ast_tokens::TKind((ast_tokens::KMut{})) : ast_tokens::TKind(word == mlc::String("const") ? ast_tokens::TKind((ast_tokens::KConst{})) : ast_tokens::TKind(word == mlc::String("return") ? ast_tokens::TKind((ast_tokens::KReturn{})) : ast_tokens::TKind(word == mlc::String("break") ? ast_tokens::TKind((ast_tokens::KBreak{})) : ast_tokens::TKind(word == mlc::String("continue") ? ast_tokens::TKind((ast_tokens::KContinue{})) : ast_tokens::TKind(word == mlc::String("if") ? ast_tokens::TKind((ast_tokens::KIf{})) : ast_tokens::TKind(word == mlc::String("then") ? ast_tokens::TKind((ast_tokens::KThen{})) : ast_tokens::TKind(word == mlc::String("else") ? ast_tokens::TKind((ast_tokens::KElse{})) : ast_tokens::TKind(word == mlc::String("unless") ? ast_tokens::TKind((ast_tokens::KUnless{})) : ast_tokens::TKind(word == mlc::String("while") ? ast_tokens::TKind((ast_tokens::KWhile{})) : ast_tokens::TKind(word == mlc::String("for") ? ast_tokens::TKind((ast_tokens::KFor{})) : ast_tokens::TKind(word == mlc::String("in") ? ast_tokens::TKind((ast_tokens::KIn{})) : ast_tokens::TKind(word == mlc::String("do") ? ast_tokens::TKind((ast_tokens::KDo{})) : ast_tokens::TKind(word == mlc::String("end") ? ast_tokens::TKind((ast_tokens::KEnd{})) : ast_tokens::TKind(word == mlc::String("match") ? ast_tokens::TKind((ast_tokens::KMatch{})) : ast_tokens::TKind(word == mlc::String("import") ? ast_tokens::TKind((ast_tokens::KImport{})) : ast_tokens::TKind(word == mlc::String("from") ? ast_tokens::TKind((ast_tokens::KFrom{})) : ast_tokens::TKind(word == mlc::String("as") ? ast_tokens::TKind((ast_tokens::KAs{})) : ast_tokens::TKind(word == mlc::String("extern") ? ast_tokens::TKind((ast_tokens::KExtern{})) : ast_tokens::TKind(word == mlc::String("extend") ? ast_tokens::TKind((ast_tokens::KExtend{})) : ast_tokens::TKind(word == mlc::String("where") ? ast_tokens::TKind((ast_tokens::KWhere{})) : ast_tokens::TKind(word == mlc::String("true") ? ast_tokens::TKind((ast_tokens::KTrue{})) : ast_tokens::TKind(word == mlc::String("false") ? ast_tokens::TKind((ast_tokens::KFalse{})) : ast_tokens::TKind(ast_tokens::Ident(word))))))))))))))))))))))))));}

lexer::ScanResult scan_ident(lexer::LexState state) noexcept{
int start = state.pos;
mlc::String source = state.src;
int token_line = state.line;
int token_col = state.col;
lexer::LexState current = std::move(state);
while (!LexState_eof(current) && is_alnum(LexState_current(current))){
{
current = LexState_lex_advance(current);
}
}
mlc::String word = source.byte_substring(start, current.pos - start);
return lexer::ScanResult{current, ast_tokens::Token{keyword_kind(word), token_line, token_col}};
}

bool is_alpha_lower(mlc::String c) noexcept{return c >= mlc::String("a") && c <= mlc::String("z");}

lexer::SuffixScan try_scan_suffix(lexer::LexState state) noexcept{
return !LexState_eof(state) && is_alpha_lower(LexState_current(state)) ? [&]() -> lexer::SuffixScan { 
  int start = state.pos;
  int orig_line = state.line;
  int orig_col = state.col;
  lexer::LexState cur = std::move(state);
  while (!LexState_eof(cur) && is_alpha_lower(LexState_current(cur)) || is_digit(LexState_current(cur))){
{
cur = LexState_lex_advance(cur);
}
}
  mlc::String candidate = cur.src.byte_substring(start, cur.pos - start);
  return candidate == mlc::String("i64") || candidate == mlc::String("u8") || candidate == mlc::String("usize") || candidate == mlc::String("f64") || candidate == mlc::String("f32") ? lexer::SuffixScan{candidate, cur} : [&]() -> lexer::SuffixScan { 
  lexer::LexState back = lexer::LexState{cur.src, start, orig_line, orig_col};
  return lexer::SuffixScan{mlc::String(""), back};
 }();
 }() : lexer::SuffixScan{mlc::String(""), state};
}

lexer::ScanResult scan_int(lexer::LexState state) noexcept{
int token_line = state.line;
int token_col = state.col;
int int_start = state.pos;
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
  mlc::String raw_float = current.src.byte_substring(int_start, current.pos - int_start);
  lexer::SuffixScan scan = try_scan_suffix(current);
  return lexer::ScanResult{scan.after, ast_tokens::Token{ast_tokens::LFloat(raw_float), token_line, token_col}};
 }() : [&]() -> lexer::ScanResult { 
  lexer::SuffixScan scan = try_scan_suffix(current);
  return scan.suffix == mlc::String("i64") ? lexer::ScanResult{scan.after, ast_tokens::Token{ast_tokens::LI64(mlc::to_string(value)), token_line, token_col}} : scan.suffix == mlc::String("u8") ? lexer::ScanResult{scan.after, ast_tokens::Token{ast_tokens::LU8(mlc::to_string(value)), token_line, token_col}} : scan.suffix == mlc::String("usize") ? lexer::ScanResult{scan.after, ast_tokens::Token{ast_tokens::LUsize(mlc::to_string(value)), token_line, token_col}} : lexer::ScanResult{scan.after, ast_tokens::Token{ast_tokens::LInt(value), token_line, token_col}};
 }();
}

lexer::ScanStrResult scan_single_string(lexer::LexState state) noexcept{
int token_line = state.line;
int token_col = state.col;
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
parts.push_back(mlc::String("\\"));
parts.push_back(LexState_current(current));
current = LexState_lex_advance(current);
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
ast_tokens::TKind token_kind = joined.length() == 1 ? ast_tokens::TKind(ast_tokens::LChar(joined)) : ast_tokens::TKind(ast_tokens::LStr(joined));
return lexer::ScanStrResult{current, ast_tokens::Token{token_kind, token_line, token_col}, error};
}

mlc::String map_escape(mlc::String character) noexcept{return character == mlc::String("n") ? mlc::String("\n") : character == mlc::String("t") ? mlc::String("\t") : character == mlc::String("r") ? mlc::String("\r") : character == mlc::String("\"") ? mlc::String("\"") : character == mlc::String("\\") ? mlc::String("\\") : character == mlc::String("0") ? mlc::String("\0", 1) : character == mlc::String("{") ? mlc::String("{") : character == mlc::String("}") ? mlc::String("}") : mlc::String("");}

lexer::ScanStrResult scan_string(lexer::LexState state) noexcept{
int token_line = state.line;
int token_col = state.col;
lexer::LexState current = LexState_lex_advance(state);
mlc::Array<mlc::String> parts = {};
mlc::String error = mlc::String("");
while (!LexState_eof(current) && LexState_current(current) != mlc::String("\"")){
{
if (LexState_current(current) == mlc::String("\\")){
{
current = LexState_lex_advance(current);
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
error = mlc::String("unterminated string");
}
} else {
{
current = LexState_lex_advance(current);
}
}
return lexer::ScanStrResult{current, ast_tokens::Token{ast_tokens::LStr(parts.join(mlc::String(""))), token_line, token_col}, error};
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
int token_col = state.col;
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
current_lit.push_back(LexState_current(current));
current = LexState_lex_advance(current);
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
return lexer::ScanStrResult{current, ast_tokens::Token{ast_tokens::LTemplate(parts), token_line, token_col}, error};
}

lexer::LexState skip_whitespace(lexer::LexState state) noexcept{
lexer::LexState current = std::move(state);
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
[&]() { 
  while (!LexState_eof(current)){
{
if (LexState_current(current) == mlc::String("*") && LexState_peek(current, 1) == mlc::String("/")){
{
current = LexState_lex_advance_by(current, 2);
break;
}
}
current = LexState_lex_advance(current);
}
}
 }();
} else {
break;
}
}
}
}
}
}
return current;
}

lexer::ScanResult scan_op(lexer::LexState state) noexcept{
int token_line = state.line;
int token_col = state.col;
mlc::String character = LexState_current(state);
mlc::String next = LexState_peek(state, 1);
return character == mlc::String("-") && next == mlc::String(">") ? lexer::ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, (ast_tokens::Arrow{}))} : character == mlc::String("=") && next == mlc::String(">") ? lexer::ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, (ast_tokens::FatArrow{}))} : character == mlc::String("|") && next == mlc::String(">") ? lexer::ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, (ast_tokens::Pipe{}))} : character == mlc::String("=") && next == mlc::String("=") ? lexer::ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::Op(mlc::String("==")))} : character == mlc::String("!") && next == mlc::String("=") ? lexer::ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::Op(mlc::String("!=")))} : character == mlc::String("<") && next == mlc::String("=") ? lexer::ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::Op(mlc::String("<=")))} : character == mlc::String(">") && next == mlc::String("=") ? lexer::ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::Op(mlc::String(">=")))} : character == mlc::String("&") && next == mlc::String("&") ? lexer::ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::Op(mlc::String("&&")))} : character == mlc::String("|") && next == mlc::String("|") ? lexer::ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::Op(mlc::String("||")))} : character == mlc::String("=") ? lexer::ScanResult{LexState_lex_advance(state), LexState_token(state, (ast_tokens::Equal{}))} : character == mlc::String("|") ? lexer::ScanResult{LexState_lex_advance(state), LexState_token(state, (ast_tokens::Bar{}))} : character == mlc::String("?") ? lexer::ScanResult{LexState_lex_advance(state), LexState_token(state, (ast_tokens::Question{}))} : character == mlc::String(".") && next == mlc::String(".") && LexState_peek(state, 2) == mlc::String(".") ? lexer::ScanResult{LexState_lex_advance_by(state, 3), LexState_token(state, (ast_tokens::Spread{}))} : character == mlc::String(".") ? lexer::ScanResult{LexState_lex_advance(state), LexState_token(state, (ast_tokens::Dot{}))} : character == mlc::String("(") ? lexer::ScanResult{LexState_lex_advance(state), LexState_token(state, (ast_tokens::LParen{}))} : character == mlc::String(")") ? lexer::ScanResult{LexState_lex_advance(state), LexState_token(state, (ast_tokens::RParen{}))} : character == mlc::String("{") ? lexer::ScanResult{LexState_lex_advance(state), LexState_token(state, (ast_tokens::LBrace{}))} : character == mlc::String("}") ? lexer::ScanResult{LexState_lex_advance(state), LexState_token(state, (ast_tokens::RBrace{}))} : character == mlc::String("[") ? lexer::ScanResult{LexState_lex_advance(state), LexState_token(state, (ast_tokens::LBracket{}))} : character == mlc::String("]") ? lexer::ScanResult{LexState_lex_advance(state), LexState_token(state, (ast_tokens::RBracket{}))} : character == mlc::String(",") ? lexer::ScanResult{LexState_lex_advance(state), LexState_token(state, (ast_tokens::Comma{}))} : character == mlc::String(";") ? lexer::ScanResult{LexState_lex_advance(state), LexState_token(state, (ast_tokens::Semicolon{}))} : character == mlc::String(":") ? lexer::ScanResult{LexState_lex_advance(state), LexState_token(state, (ast_tokens::Colon{}))} : lexer::ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::Op(character))};
}

ast_tokens::LexOut tokenize(mlc::String source) noexcept{
lexer::LexState state = lexer::LexState{source, 0, 1, 1};
mlc::Array<ast_tokens::Token> tokens = {};
mlc::Array<mlc::String> errors = {};
while (!LexState_eof(state)){
{
state = skip_whitespace(state);
if (LexState_eof(state)){
{
break;
}
}
mlc::String character = LexState_current(state);
if (is_alpha(character)){
{
lexer::ScanResult result = scan_ident(state);
tokens.push_back(result.token);
state = result.state;
}
} else {
{
if (is_digit(character)){
lexer::ScanResult result = scan_int(state);
tokens.push_back(result.token);
state = result.state;
} else {
if (character == mlc::String("\"")){
lexer::ScanStrResult result = scan_string(state);
tokens.push_back(result.token);
state = result.state;
if (result.error != mlc::String("")){
errors.push_back(result.error);
}
} else {
if (character == mlc::String("'")){
lexer::ScanStrResult result = scan_single_string(state);
tokens.push_back(result.token);
state = result.state;
if (result.error != mlc::String("")){
errors.push_back(result.error);
}
} else {
if (character == mlc::String("`")){
lexer::ScanStrResult result = scan_template(state);
tokens.push_back(result.token);
state = result.state;
if (result.error != mlc::String("")){
errors.push_back(result.error);
}
} else {
lexer::ScanResult result = scan_op(state);
tokens.push_back(result.token);
state = result.state;
}
}
}
}
}
}
}
}
tokens.push_back(LexState_token(state, (ast_tokens::Eof{})));
return ast_tokens::LexOut{tokens, errors};
}

} // namespace lexer
