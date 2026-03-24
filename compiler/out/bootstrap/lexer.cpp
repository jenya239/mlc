#define main mlc_user_main
#include "lexer.hpp"

namespace lexer {

bool LexState_eof(LexState self) noexcept{
return (self.pos >= self.src.byte_size());
}
mlc::String LexState_current(LexState self) noexcept{
if (LexState_eof(self)) {
return mlc::String("\0", 1);
} else {
return self.src.byte_at(self.pos);
}
}
mlc::String LexState_peek(LexState self, int offset) noexcept{
auto position = (self.pos + offset);
return ((position >= self.src.byte_size()) ? mlc::String("\0", 1) : self.src.byte_at(position));
}
LexState LexState_lex_advance(LexState self) noexcept{
if ((LexState_current(self) == mlc::String("\n", 1))) {
return LexState{self.src, (self.pos + 1), (self.line + 1), 1};
} else {
return LexState{self.src, (self.pos + 1), self.line, (self.col + 1)};
}
}
LexState LexState_lex_advance_by(LexState self, int count) noexcept{
auto state = self;
auto i = 0;
while ((i < count)) {
state = LexState_lex_advance(state);
i = (i + 1);
}
return state;
}
ast_tokens::Token LexState_token(LexState self, ast_tokens::TKind tk) noexcept{
return ast_tokens::Token{tk, self.line, self.col};
}
bool is_alpha(mlc::String character) noexcept{
return ((((character >= mlc::String("a", 1)) && (character <= mlc::String("z", 1))) || ((character >= mlc::String("A", 1)) && (character <= mlc::String("Z", 1)))) || (character == mlc::String("_", 1)));
}
bool is_digit(mlc::String character) noexcept{
return ((character >= mlc::String("0", 1)) && (character <= mlc::String("9", 1)));
}
bool is_alnum(mlc::String character) noexcept{
return (is_alpha(character) || is_digit(character));
}
bool is_ws(mlc::String character) noexcept{
return ((((character == mlc::String(" ", 1)) || (character == mlc::String("\t", 1))) || (character == mlc::String("\r", 1))) || (character == mlc::String("\n", 1)));
}
ast_tokens::TKind keyword_kind(mlc::String word) noexcept{
if ((word == mlc::String("fn", 2))) {
return ast_tokens::KFn{};
} else if ((word == mlc::String("type", 4))) {
return ast_tokens::KType{};
} else if ((word == mlc::String("let", 3))) {
return ast_tokens::KLet{};
} else if ((word == mlc::String("mut", 3))) {
return ast_tokens::KMut{};
} else if ((word == mlc::String("const", 5))) {
return ast_tokens::KConst{};
} else if ((word == mlc::String("return", 6))) {
return ast_tokens::KReturn{};
} else if ((word == mlc::String("break", 5))) {
return ast_tokens::KBreak{};
} else if ((word == mlc::String("continue", 8))) {
return ast_tokens::KContinue{};
} else if ((word == mlc::String("if", 2))) {
return ast_tokens::KIf{};
} else if ((word == mlc::String("then", 4))) {
return ast_tokens::KThen{};
} else if ((word == mlc::String("else", 4))) {
return ast_tokens::KElse{};
} else if ((word == mlc::String("unless", 6))) {
return ast_tokens::KUnless{};
} else if ((word == mlc::String("while", 5))) {
return ast_tokens::KWhile{};
} else if ((word == mlc::String("for", 3))) {
return ast_tokens::KFor{};
} else if ((word == mlc::String("in", 2))) {
return ast_tokens::KIn{};
} else if ((word == mlc::String("do", 2))) {
return ast_tokens::KDo{};
} else if ((word == mlc::String("end", 3))) {
return ast_tokens::KEnd{};
} else if ((word == mlc::String("match", 5))) {
return ast_tokens::KMatch{};
} else if ((word == mlc::String("import", 6))) {
return ast_tokens::KImport{};
} else if ((word == mlc::String("from", 4))) {
return ast_tokens::KFrom{};
} else if ((word == mlc::String("as", 2))) {
return ast_tokens::KAs{};
} else if ((word == mlc::String("extern", 6))) {
return ast_tokens::KExtern{};
} else if ((word == mlc::String("extend", 6))) {
return ast_tokens::KExtend{};
} else if ((word == mlc::String("where", 5))) {
return ast_tokens::KWhere{};
} else if ((word == mlc::String("true", 4))) {
return ast_tokens::KTrue{};
} else if ((word == mlc::String("false", 5))) {
return ast_tokens::KFalse{};
} else {
return ast_tokens::Ident(word);
}

























}
ScanResult scan_ident(LexState state) noexcept{
auto start = state.pos;
auto source = state.src;
auto token_line = state.line;
auto token_col = state.col;
auto current = state;
while (((!LexState_eof(current)) && is_alnum(LexState_current(current)))) {
current = LexState_lex_advance(current);
}
auto word = source.byte_substring(start, (current.pos - start));
return ScanResult{current, ast_tokens::Token{keyword_kind(word), token_line, token_col}};
}
ScanResult scan_int(LexState state) noexcept{
auto token_line = state.line;
auto token_col = state.col;
auto current = state;
auto value = 0;
while (((!LexState_eof(current)) && is_digit(LexState_current(current)))) {
value = ((value * 10) + LexState_current(current).to_i());
current = LexState_lex_advance(current);
}
return ScanResult{current, ast_tokens::Token{ast_tokens::LInt(value), token_line, token_col}};
}
ScanStrResult scan_single_string(LexState state) noexcept{
auto token_line = state.line;
auto token_col = state.col;
auto current = LexState_lex_advance(state);
auto parts = mlc::Array<mlc::String>{};
auto error = mlc::String("", 0);
while (((!LexState_eof(current)) && (LexState_current(current) != mlc::String("'", 1)))) {
if ((LexState_current(current) == mlc::String("\\", 1))) {
current = LexState_lex_advance(current);
if (LexState_eof(current)) {
error = mlc::String("unterminated escape in single-quoted string", 43);
} else if ((LexState_current(current) == mlc::String("\\", 1))) {
parts.push_back(mlc::String("\\", 1));
/* unit */;
current = LexState_lex_advance(current);
} else if ((LexState_current(current) == mlc::String("'", 1))) {
parts.push_back(mlc::String("'", 1));
/* unit */;
current = LexState_lex_advance(current);
} else if ((LexState_current(current) == mlc::String("n", 1))) {
parts.push_back(mlc::String("\n", 1));
/* unit */;
current = LexState_lex_advance(current);
} else if ((LexState_current(current) == mlc::String("t", 1))) {
parts.push_back(mlc::String("\t", 1));
/* unit */;
current = LexState_lex_advance(current);
} else {
parts.push_back(mlc::String("\\", 1));
parts.push_back(LexState_current(current));
current = LexState_lex_advance(current);
}




} else {
parts.push_back(LexState_current(current));
current = LexState_lex_advance(current);
}
}
if (LexState_eof(current)) {
error = mlc::String("unterminated single-quoted string", 33);
} else {
current = LexState_lex_advance(current);
}
return ScanStrResult{current, ast_tokens::Token{ast_tokens::LStr(parts.join(mlc::String("", 0))), token_line, token_col}, error};
}
mlc::String map_escape(mlc::String character) noexcept{
if ((character == mlc::String("n", 1))) {
return mlc::String("\n", 1);
} else if ((character == mlc::String("t", 1))) {
return mlc::String("\t", 1);
} else if ((character == mlc::String("r", 1))) {
return mlc::String("\r", 1);
} else if ((character == mlc::String("\"", 1))) {
return mlc::String("\"", 1);
} else if ((character == mlc::String("\\", 1))) {
return mlc::String("\\", 1);
} else if ((character == mlc::String("0", 1))) {
return mlc::String("\0", 1);
} else if ((character == mlc::String("{", 1))) {
return mlc::String("{", 1);
} else if ((character == mlc::String("}", 1))) {
return mlc::String("}", 1);
} else {
return mlc::String("", 0);
}







}
ScanStrResult scan_string(LexState state) noexcept{
auto token_line = state.line;
auto token_col = state.col;
auto current = LexState_lex_advance(state);
auto parts = mlc::Array<mlc::String>{};
auto error = mlc::String("", 0);
while (((!LexState_eof(current)) && (LexState_current(current) != mlc::String("\"", 1)))) {
if ((LexState_current(current) == mlc::String("\\", 1))) {
current = LexState_lex_advance(current);
auto mapped = map_escape(LexState_current(current));
if ((mapped != mlc::String("", 0))) {
parts.push_back(mapped);
} else {
parts.push_back(mlc::String("\\", 1));
parts.push_back(LexState_current(current));
}
current = LexState_lex_advance(current);
} else {
parts.push_back(LexState_current(current));
current = LexState_lex_advance(current);
}
}
if (LexState_eof(current)) {
error = mlc::String("unterminated string", 19);
} else {
current = LexState_lex_advance(current);
}
return ScanStrResult{current, ast_tokens::Token{ast_tokens::LStr(parts.join(mlc::String("", 0))), token_line, token_col}, error};
}
ScanStrResult scan_fstring(LexState state) noexcept{
auto token_line = state.line;
auto token_col = state.col;
auto current = LexState_lex_advance(LexState_lex_advance(state));
auto parts = mlc::Array<mlc::String>{};
auto current_lit = mlc::Array<mlc::String>{};
auto error = mlc::String("", 0);
while (((!LexState_eof(current)) && (LexState_current(current) != mlc::String("\"", 1)))) {
if ((LexState_current(current) == mlc::String("\\", 1))) {
current = LexState_lex_advance(current);
auto mapped = map_escape(LexState_current(current));
if ((mapped != mlc::String("", 0))) {
current_lit.push_back(mapped);
} else {
current_lit.push_back(mlc::String("\\", 1));
current_lit.push_back(LexState_current(current));
}
current = LexState_lex_advance(current);
} else if ((LexState_current(current) == mlc::String("{", 1))) {
parts.push_back(current_lit.join(mlc::String("", 0)));
current_lit = {};
current = LexState_lex_advance(current);
auto expr_chars = mlc::Array<mlc::String>{};
while (((!LexState_eof(current)) && (LexState_current(current) != mlc::String("}", 1)))) {
expr_chars.push_back(LexState_current(current));
current = LexState_lex_advance(current);
}
if ((!LexState_eof(current))) {
current = LexState_lex_advance(current);
}
parts.push_back(expr_chars.join(mlc::String("", 0)));
} else {
current_lit.push_back(LexState_current(current));
current = LexState_lex_advance(current);
}

}
if (LexState_eof(current)) {
error = mlc::String("unterminated f-string", 21);
} else {
current = LexState_lex_advance(current);
}
parts.push_back(current_lit.join(mlc::String("", 0)));
return ScanStrResult{current, ast_tokens::Token{ast_tokens::LFStr(parts), token_line, token_col}, error};
}
LexState skip_whitespace(LexState state) noexcept{
auto current = state;
while ((!LexState_eof(current))) {
auto character = LexState_current(current);
if (is_ws(character)) {
current = LexState_lex_advance(current);
} else if (((character == mlc::String("/", 1)) && (LexState_peek(current, 1) == mlc::String("/", 1)))) {
current = LexState_lex_advance_by(current, 2);
[&]() {
while (((!LexState_eof(current)) && (LexState_current(current) != mlc::String("\n", 1)))) {
current = LexState_lex_advance(current);
}
}();
} else if (((character == mlc::String("/", 1)) && (LexState_peek(current, 1) == mlc::String("*", 1)))) {
current = LexState_lex_advance_by(current, 2);
[&]() {
while ((!LexState_eof(current))) {
if (((LexState_current(current) == mlc::String("*", 1)) && (LexState_peek(current, 1) == mlc::String("/", 1)))) {
current = LexState_lex_advance_by(current, 2);
/* unit */;
break;
}
current = LexState_lex_advance(current);
}
}();
} else {
break;
}


}
return current;
}
ScanResult scan_op(LexState state) noexcept{
auto token_line = state.line;
auto token_col = state.col;
auto character = LexState_current(state);
auto next = LexState_peek(state, 1);
return (((character == mlc::String("-", 1)) && (next == mlc::String(">", 1))) ? ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::Arrow{})} : (((character == mlc::String("=", 1)) && (next == mlc::String(">", 1))) ? ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::FatArrow{})} : (((character == mlc::String("|", 1)) && (next == mlc::String(">", 1))) ? ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::Pipe{})} : (((character == mlc::String("=", 1)) && (next == mlc::String("=", 1))) ? ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::Op(mlc::String("==", 2)))} : (((character == mlc::String("!", 1)) && (next == mlc::String("=", 1))) ? ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::Op(mlc::String("!=", 2)))} : (((character == mlc::String("<", 1)) && (next == mlc::String("=", 1))) ? ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::Op(mlc::String("<=", 2)))} : (((character == mlc::String(">", 1)) && (next == mlc::String("=", 1))) ? ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::Op(mlc::String(">=", 2)))} : (((character == mlc::String("&", 1)) && (next == mlc::String("&", 1))) ? ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::Op(mlc::String("&&", 2)))} : (((character == mlc::String("|", 1)) && (next == mlc::String("|", 1))) ? ScanResult{LexState_lex_advance_by(state, 2), LexState_token(state, ast_tokens::Op(mlc::String("||", 2)))} : ((character == mlc::String("=", 1)) ? ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::Equal{})} : ((character == mlc::String("|", 1)) ? ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::Bar{})} : ((character == mlc::String("?", 1)) ? ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::Question{})} : ((((character == mlc::String(".", 1)) && (next == mlc::String(".", 1))) && (LexState_peek(state, 2) == mlc::String(".", 1))) ? ScanResult{LexState_lex_advance_by(state, 3), LexState_token(state, ast_tokens::Spread{})} : ((character == mlc::String(".", 1)) ? ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::Dot{})} : ((character == mlc::String("(", 1)) ? ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::LParen{})} : ((character == mlc::String(")", 1)) ? ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::RParen{})} : ((character == mlc::String("{", 1)) ? ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::LBrace{})} : ((character == mlc::String("}", 1)) ? ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::RBrace{})} : ((character == mlc::String("[", 1)) ? ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::LBracket{})} : ((character == mlc::String("]", 1)) ? ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::RBracket{})} : ((character == mlc::String(",", 1)) ? ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::Comma{})} : ((character == mlc::String(";", 1)) ? ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::Semicolon{})} : ((character == mlc::String(":", 1)) ? ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::Colon{})} : ScanResult{LexState_lex_advance(state), LexState_token(state, ast_tokens::Op(character))})))))))))))))))))))))));
}
ast_tokens::LexOut tokenize(mlc::String source) noexcept{
auto state = LexState{source, 0, 1, 1};
auto tokens = mlc::Array<ast_tokens::Token>{};
auto errors = mlc::Array<mlc::String>{};
while ((!LexState_eof(state))) {
state = skip_whitespace(state);
if (LexState_eof(state)) {
break;
}
auto character = LexState_current(state);
if (is_alpha(character)) {
if ((((character == mlc::String("f", 1)) && (!LexState_eof(LexState_lex_advance(state)))) && (LexState_current(LexState_lex_advance(state)) == mlc::String("\"", 1)))) {
auto result = scan_fstring(state);
/* unit */;
tokens.push_back(result.token);
/* unit */;
state = result.state;
if ((result.error != mlc::String("", 0))) {
errors.push_back(result.error);
}
} else {
auto result = scan_ident(state);
tokens.push_back(result.token);
state = result.state;
}
} else if (is_digit(character)) {
auto result = scan_int(state);
/* unit */;
tokens.push_back(result.token);
/* unit */;
state = result.state;
} else if ((character == mlc::String("\"", 1))) {
auto result = scan_string(state);
/* unit */;
tokens.push_back(result.token);
/* unit */;
state = result.state;
if ((result.error != mlc::String("", 0))) {
errors.push_back(result.error);
}
} else if ((character == mlc::String("'", 1))) {
auto result = scan_single_string(state);
/* unit */;
tokens.push_back(result.token);
/* unit */;
state = result.state;
if ((result.error != mlc::String("", 0))) {
errors.push_back(result.error);
}
} else {
auto result = scan_op(state);
tokens.push_back(result.token);
state = result.state;
}



}
tokens.push_back(LexState_token(state, ast_tokens::Eof{}));
return ast_tokens::LexOut{tokens, errors};
}

} // namespace lexer
