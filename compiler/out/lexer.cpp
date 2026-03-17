#include "lexer.hpp"

#include "ast_tokens.hpp"

namespace lexer {

using namespace ast_tokens;

struct LexState {mlc::String src;int pos;int line;int col;};

struct ScanResult {lexer::LexState state;ast_tokens::Token token;};

struct ScanStrResult {lexer::LexState state;ast_tokens::Token token;mlc::String error;};

bool LexState_eof(lexer::LexState self) noexcept;

mlc::String LexState_current(lexer::LexState self) noexcept;

mlc::String LexState_peek(lexer::LexState self, int offset) noexcept;

lexer::LexState LexState_advance(lexer::LexState self) noexcept;

lexer::LexState LexState_advance_by(lexer::LexState self, int count) noexcept;

ast_tokens::Token LexState_token(lexer::LexState self, ast_tokens::TKind kind) noexcept;

bool is_alpha(mlc::String character) noexcept;

bool is_digit(mlc::String character) noexcept;

bool is_alnum(mlc::String character) noexcept;

bool is_ws(mlc::String character) noexcept;

ast_tokens::TKind keyword_kind(mlc::String word) noexcept;

lexer::ScanResult scan_ident(lexer::LexState state) noexcept;

lexer::ScanResult scan_int(lexer::LexState state) noexcept;

lexer::ScanStrResult scan_single_string(lexer::LexState state) noexcept;

mlc::String map_escape(mlc::String character) noexcept;

lexer::ScanStrResult scan_string(lexer::LexState state) noexcept;

lexer::LexState skip_whitespace(lexer::LexState state) noexcept;

lexer::ScanResult scan_op(lexer::LexState state) noexcept;

ast_tokens::LexOut tokenize(mlc::String source) noexcept;

bool LexState_eof(lexer::LexState self) noexcept{return self.pos >= self.src.byte_size();}

mlc::String LexState_current(lexer::LexState self) noexcept{return LexState_eof(self) ? mlc::String("\0", 1) : self.src.byte_at(self.pos);}

mlc::String LexState_peek(lexer::LexState self, int offset) noexcept{
int position = self.pos + offset;
return position >= self.src.byte_size() ? mlc::String("\0", 1) : self.src.byte_at(position);
}

lexer::LexState LexState_advance(lexer::LexState self) noexcept{return LexState_current(self) == mlc::String("\n") ? lexer::LexState{self.src, self.pos + 1, self.line + 1, 1} : lexer::LexState{self.src, self.pos + 1, self.line, self.col + 1};}

lexer::LexState LexState_advance_by(lexer::LexState self, int count) noexcept{
lexer::LexState state = std::move(self);
int i = 0;
while (i < count){
{
state = LexState_advance(state);
i = i + 1;
}
}
return state;
}

ast_tokens::Token LexState_token(lexer::LexState self, ast_tokens::TKind kind) noexcept{return ast_tokens::Token{kind, self.line, self.col};}

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
current = LexState_advance(current);
}
}
mlc::String word = source.byte_substring(start, current.pos - start);
return lexer::ScanResult{current, ast_tokens::Token{keyword_kind(word), token_line, token_col}};
}

lexer::ScanResult scan_int(lexer::LexState state) noexcept{
int token_line = state.line;
int token_col = state.col;
lexer::LexState current = std::move(state);
int value = 0;
while (!LexState_eof(current) && is_digit(LexState_current(current))){
{
value = value * 10 + LexState_current(current).to_i();
current = LexState_advance(current);
}
}
return lexer::ScanResult{current, ast_tokens::Token{ast_tokens::LInt(value), token_line, token_col}};
}

lexer::ScanStrResult scan_single_string(lexer::LexState state) noexcept{
int token_line = state.line;
int token_col = state.col;
lexer::LexState current = LexState_advance(state);
mlc::Array<mlc::String> parts = {};
mlc::String error = mlc::String("");
while (!LexState_eof(current) && LexState_current(current) != mlc::String("'")){
{
if (LexState_current(current) == mlc::String("\\")){
{
current = LexState_advance(current);
if (LexState_eof(current)){
error = mlc::String("unterminated escape in single-quoted string");
} else {
if (LexState_current(current) == mlc::String("\\")){
parts.push_back(mlc::String("\\"));
current = LexState_advance(current);
} else {
if (LexState_current(current) == mlc::String("'")){
parts.push_back(mlc::String("'"));
current = LexState_advance(current);
} else {
if (LexState_current(current) == mlc::String("n")){
parts.push_back(mlc::String("\n"));
current = LexState_advance(current);
} else {
if (LexState_current(current) == mlc::String("t")){
parts.push_back(mlc::String("\t"));
current = LexState_advance(current);
} else {
parts.push_back(mlc::String("\\"));
parts.push_back(LexState_current(current));
current = LexState_advance(current);
}
}
}
}
}
}
} else {
{
parts.push_back(LexState_current(current));
current = LexState_advance(current);
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
current = LexState_advance(current);
}
}
return lexer::ScanStrResult{current, ast_tokens::Token{ast_tokens::LStr(parts.join(mlc::String(""))), token_line, token_col}, error};
}

mlc::String map_escape(mlc::String character) noexcept{return character == mlc::String("n") ? mlc::String("\n") : character == mlc::String("t") ? mlc::String("\t") : character == mlc::String("r") ? mlc::String("\r") : character == mlc::String("\"") ? mlc::String("\"") : character == mlc::String("\\") ? mlc::String("\\") : character == mlc::String("0") ? mlc::String("\0", 1) : character == mlc::String("{") ? mlc::String("{") : character == mlc::String("}") ? mlc::String("}") : mlc::String("");}

lexer::ScanStrResult scan_string(lexer::LexState state) noexcept{
int token_line = state.line;
int token_col = state.col;
lexer::LexState current = LexState_advance(state);
mlc::Array<mlc::String> parts = {};
mlc::String error = mlc::String("");
while (!LexState_eof(current) && LexState_current(current) != mlc::String("\"")){
{
if (LexState_current(current) == mlc::String("\\")){
{
current = LexState_advance(current);
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
current = LexState_advance(current);
}
} else {
{
parts.push_back(LexState_current(current));
current = LexState_advance(current);
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
current = LexState_advance(current);
}
}
return lexer::ScanStrResult{current, ast_tokens::Token{ast_tokens::LStr(parts.join(mlc::String(""))), token_line, token_col}, error};
}

lexer::LexState skip_whitespace(lexer::LexState state) noexcept{
lexer::LexState current = std::move(state);
while (!LexState_eof(current)){
{
mlc::String character = LexState_current(current);
if (is_ws(character)){
{
current = LexState_advance(current);
}
} else {
{
if (character == mlc::String("/") && LexState_peek(current, 1) == mlc::String("/")){
current = LexState_advance_by(current, 2);
[&]() { 
  while (!LexState_eof(current) && LexState_current(current) != mlc::String("\n")){
{
current = LexState_advance(current);
}
}
 }();
} else {
if (character == mlc::String("/") && LexState_peek(current, 1) == mlc::String("*")){
current = LexState_advance_by(current, 2);
[&]() { 
  while (!LexState_eof(current)){
{
if (LexState_current(current) == mlc::String("*") && LexState_peek(current, 1) == mlc::String("/")){
{
current = LexState_advance_by(current, 2);
break;
}
}
current = LexState_advance(current);
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
return character == mlc::String("-") && next == mlc::String(">") ? lexer::ScanResult{LexState_advance_by(state, 2), LexState_token(state, (ast_tokens::Arrow{}))} : character == mlc::String("=") && next == mlc::String(">") ? lexer::ScanResult{LexState_advance_by(state, 2), LexState_token(state, (ast_tokens::FatArrow{}))} : character == mlc::String("|") && next == mlc::String(">") ? lexer::ScanResult{LexState_advance_by(state, 2), LexState_token(state, (ast_tokens::Pipe{}))} : character == mlc::String("=") && next == mlc::String("=") ? lexer::ScanResult{LexState_advance_by(state, 2), LexState_token(state, ast_tokens::Op(mlc::String("==")))} : character == mlc::String("!") && next == mlc::String("=") ? lexer::ScanResult{LexState_advance_by(state, 2), LexState_token(state, ast_tokens::Op(mlc::String("!=")))} : character == mlc::String("<") && next == mlc::String("=") ? lexer::ScanResult{LexState_advance_by(state, 2), LexState_token(state, ast_tokens::Op(mlc::String("<=")))} : character == mlc::String(">") && next == mlc::String("=") ? lexer::ScanResult{LexState_advance_by(state, 2), LexState_token(state, ast_tokens::Op(mlc::String(">=")))} : character == mlc::String("&") && next == mlc::String("&") ? lexer::ScanResult{LexState_advance_by(state, 2), LexState_token(state, ast_tokens::Op(mlc::String("&&")))} : character == mlc::String("|") && next == mlc::String("|") ? lexer::ScanResult{LexState_advance_by(state, 2), LexState_token(state, ast_tokens::Op(mlc::String("||")))} : character == mlc::String("=") ? lexer::ScanResult{LexState_advance(state), LexState_token(state, (ast_tokens::Equal{}))} : character == mlc::String("|") ? lexer::ScanResult{LexState_advance(state), LexState_token(state, (ast_tokens::Bar{}))} : character == mlc::String("?") ? lexer::ScanResult{LexState_advance(state), LexState_token(state, (ast_tokens::Question{}))} : character == mlc::String(".") && next == mlc::String(".") && LexState_peek(state, 2) == mlc::String(".") ? lexer::ScanResult{LexState_advance_by(state, 3), LexState_token(state, (ast_tokens::Spread{}))} : character == mlc::String(".") ? lexer::ScanResult{LexState_advance(state), LexState_token(state, (ast_tokens::Dot{}))} : character == mlc::String("(") ? lexer::ScanResult{LexState_advance(state), LexState_token(state, (ast_tokens::LParen{}))} : character == mlc::String(")") ? lexer::ScanResult{LexState_advance(state), LexState_token(state, (ast_tokens::RParen{}))} : character == mlc::String("{") ? lexer::ScanResult{LexState_advance(state), LexState_token(state, (ast_tokens::LBrace{}))} : character == mlc::String("}") ? lexer::ScanResult{LexState_advance(state), LexState_token(state, (ast_tokens::RBrace{}))} : character == mlc::String("[") ? lexer::ScanResult{LexState_advance(state), LexState_token(state, (ast_tokens::LBracket{}))} : character == mlc::String("]") ? lexer::ScanResult{LexState_advance(state), LexState_token(state, (ast_tokens::RBracket{}))} : character == mlc::String(",") ? lexer::ScanResult{LexState_advance(state), LexState_token(state, (ast_tokens::Comma{}))} : character == mlc::String(";") ? lexer::ScanResult{LexState_advance(state), LexState_token(state, (ast_tokens::Semicolon{}))} : character == mlc::String(":") ? lexer::ScanResult{LexState_advance(state), LexState_token(state, (ast_tokens::Colon{}))} : lexer::ScanResult{LexState_advance(state), LexState_token(state, ast_tokens::Op(character))};
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
tokens.push_back(LexState_token(state, (ast_tokens::Eof{})));
return ast_tokens::LexOut{tokens, errors};
}

} // namespace lexer
