#define main mlc_user_main
#include "preds.hpp"

namespace preds {

Parser parser_new_with_source_path(mlc::Array<ast_tokens::Token> tokens, mlc::String source_path) noexcept{
return Parser{tokens, 0, source_path};
}
Parser parser_new(mlc::Array<ast_tokens::Token> tokens) noexcept{
return parser_new_with_source_path(tokens, mlc::String("", 0));
}
ast_tokens::TKind parser_kind(Parser parser) noexcept{
auto tok = parser.tokens[parser.pos];
return tok.kind;
}
Parser parser_advance(Parser parser) noexcept{
return Parser{parser.tokens, (parser.pos + 1), parser.source_path};
}
Parser parser_advance_by(Parser parser, int count) noexcept{
return Parser{parser.tokens, (parser.pos + count), parser.source_path};
}
bool parser_at_eof(Parser parser) noexcept{
if ((parser.pos >= parser.tokens.length())) {
return true;
} else {
auto tok = parser.tokens[parser.pos];
return std::visit(overloaded{[&](const ast_tokens::Eof& eof) { return true; },
[&](const auto& __v) { return false; }
}, tok.kind);
}
}
Parser parser_skip_semi(Parser parser) noexcept{
auto tok = parser.tokens[parser.pos];
return std::visit(overloaded{[&](const ast_tokens::Semicolon& semicolon) { return parser_advance(parser); },
[&](const auto& __v) { return parser; }
}, tok.kind);
}
bool is_keyword_fn(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KFn& kFn) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_keyword_type(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KType& kType) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_keyword_let(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KLet& kLet) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_keyword_mut(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KMut& kMut) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_keyword_const(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KConst& kConst) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_keyword_if(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KIf& kIf) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_keyword_unless(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KUnless& kUnless) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_keyword_else(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KElse& kElse) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_keyword_end(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KEnd& kEnd) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_keyword_do(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KDo& kDo) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_keyword_while(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KWhile& kWhile) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_keyword_for(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KFor& kFor) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_keyword_match(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KMatch& kMatch) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_keyword_return(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KReturn& kReturn) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_keyword_break(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KBreak& kBreak) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_keyword_continue(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KContinue& kContinue) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_keyword_true(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KTrue& kTrue) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_keyword_false(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KFalse& kFalse) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_keyword_extern(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KExtern& kExtern) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_keyword_extend(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KExtend& kExtend) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_ident(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::Ident& ident) { auto [__0] = ident; return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_int_literal(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::LInt& lInt) { auto [__0] = lInt; return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_str_literal(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::LStr& lStr) { auto [__0] = lStr; return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_template_literal(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::LTemplate& lTemplate) { auto [__0] = lTemplate; return true; },
[&](const auto& __v) { return false; }
}, kind);
}
mlc::Array<mlc::String> get_template_parts(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::LTemplate& lTemplate) { auto [parts] = lTemplate; return parts; },
[&](const auto& __v) { return [&]() {
auto r = mlc::Array<mlc::String>{};
return r;
}(); }
}, kind);
}
bool is_equal(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::Equal& equal) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_bar(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::Bar& bar) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_pipe(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::Pipe& pipe) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_colon(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::Colon& colon) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_comma(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::Comma& comma) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_dot(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::Dot& dot) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_spread(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::Spread& spread) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_arrow(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::Arrow& arrow) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_fat_arrow(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::FatArrow& fatArrow) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_question(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::Question& question) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_left_paren(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::LParen& lParen) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_right_paren(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::RParen& rParen) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_left_brace(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::LBrace& lBrace) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_right_brace(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::RBrace& rBrace) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_left_bracket(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::LBracket& lBracket) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_right_bracket(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::RBracket& rBracket) { return true; },
[&](const auto& __v) { return false; }
}, kind);
}
bool is_operator(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::Op& op) { auto [__0] = op; return true; },
[&](const auto& __v) { return false; }
}, kind);
}
mlc::String get_ident(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::Ident& ident) { auto [name] = ident; return name; },
[&](const auto& __v) { return mlc::String("", 0); }
}, kind);
}
int get_int_literal(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::LInt& lInt) { auto [n] = lInt; return n; },
[&](const auto& __v) { return 0; }
}, kind);
}
mlc::String get_str_literal(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::LStr& lStr) { auto [s] = lStr; return s; },
[&](const auto& __v) { return mlc::String("", 0); }
}, kind);
}
mlc::String get_operator(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::Op& op) { auto [s] = op; return s; },
[&](const auto& __v) { return mlc::String("", 0); }
}, kind);
}
bool is_ctor_name(mlc::String name) noexcept{
return ((name.char_at(0) >= mlc::String("A", 1)) && (name.char_at(0) <= mlc::String("Z", 1)));
}
bool TKind_is_fn(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KFn& kFn) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_type(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KType& kType) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_let(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KLet& kLet) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_mut(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KMut& kMut) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_const(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KConst& kConst) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_if(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KIf& kIf) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_unless(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KUnless& kUnless) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_else(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KElse& kElse) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_end(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KEnd& kEnd) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_do(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KDo& kDo) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_while(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KWhile& kWhile) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_for(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KFor& kFor) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_match(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KMatch& kMatch) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_return(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KReturn& kReturn) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_break(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KBreak& kBreak) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_continue(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KContinue& kContinue) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_true(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KTrue& kTrue) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_false(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KFalse& kFalse) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_extern(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KExtern& kExtern) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_extend(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KExtend& kExtend) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_import(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KImport& kImport) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_as(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KAs& kAs) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_from(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::KFrom& kFrom) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_eof(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::Eof& eof) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_ident(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::Ident& ident) { auto [__0] = ident; return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_int(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::LInt& lInt) { auto [__0] = lInt; return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_str(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::LStr& lStr) { auto [__0] = lStr; return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_template(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::LTemplate& lTemplate) { auto [__0] = lTemplate; return true; },
[&](const auto& __v) { return false; }
}, self);
}
mlc::Array<mlc::String> TKind_template_parts(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::LTemplate& lTemplate) { auto [parts] = lTemplate; return parts; },
[&](const auto& __v) { return [&]() {
auto r = mlc::Array<mlc::String>{};
return r;
}(); }
}, self);
}
bool TKind_is_equal(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::Equal& equal) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_bar(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::Bar& bar) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_pipe(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::Pipe& pipe) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_colon(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::Colon& colon) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_comma(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::Comma& comma) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_dot(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::Dot& dot) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_spread(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::Spread& spread) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_arrow(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::Arrow& arrow) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_fat_arrow(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::FatArrow& fatArrow) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_question(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::Question& question) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_lparen(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::LParen& lParen) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_rparen(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::RParen& rParen) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_lbrace(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::LBrace& lBrace) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_rbrace(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::RBrace& rBrace) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_lbracket(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::LBracket& lBracket) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_rbracket(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::RBracket& rBracket) { return true; },
[&](const auto& __v) { return false; }
}, self);
}
bool TKind_is_op(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::Op& op) { auto [__0] = op; return true; },
[&](const auto& __v) { return false; }
}, self);
}
mlc::String TKind_ident(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::Ident& ident) { auto [name] = ident; return name; },
[&](const auto& __v) { return mlc::String("", 0); }
}, self);
}
int TKind_int_val(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::LInt& lInt) { auto [n] = lInt; return n; },
[&](const auto& __v) { return 0; }
}, self);
}
mlc::String TKind_str_val(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::LStr& lStr) { auto [s] = lStr; return s; },
[&](const auto& __v) { return mlc::String("", 0); }
}, self);
}
mlc::String TKind_op_val(ast_tokens::TKind self) noexcept{
return std::visit(overloaded{[&](const ast_tokens::Op& op) { auto [s] = op; return s; },
[&](const auto& __v) { return mlc::String("", 0); }
}, self);
}
ast_tokens::TKind Parser_kind(Parser self) noexcept{
auto tok = self.tokens[self.pos];
return tok.kind;
}
Parser Parser_advance(Parser self) noexcept{
return Parser{self.tokens, (self.pos + 1), self.source_path};
}
Parser Parser_advance_by(Parser self, int count) noexcept{
return Parser{self.tokens, (self.pos + count), self.source_path};
}
bool Parser_at_eof(Parser self) noexcept{
if ((self.pos >= self.tokens.length())) {
return true;
} else {
auto tok = self.tokens[self.pos];
return TKind_is_eof(tok.kind);
}
}
Parser Parser_skip_semi(Parser self) noexcept{
auto tok = self.tokens[self.pos];
return std::visit(overloaded{[&](const ast_tokens::Semicolon& semicolon) { return Parser_advance(self); },
[&](const auto& __v) { return self; }
}, tok.kind);
}
ast::Span Parser_span_at_cursor(Parser self) noexcept{
auto tok = self.tokens[self.pos];
return ast::Span{self.source_path, tok.line, tok.col};
}
int Parser_prev_line(Parser self) noexcept{
if ((self.pos > 0)) {
auto tok = self.tokens[(self.pos - 1)];
/* unit */;
return tok.line;
} else {
return 0;
}
}

} // namespace preds
