#define main mlc_user_main
#include "preds.hpp"

namespace preds {

Parser parser_new(mlc::Array<ast_tokens::Token> tokens) noexcept{
return Parser{tokens, 0};
}
ast_tokens::TKind parser_kind(Parser parser) noexcept{
auto tok = parser.tokens[parser.pos];
return tok.kind;
}
Parser parser_advance(Parser parser) noexcept{
return Parser{parser.tokens, (parser.pos + 1)};
}
Parser parser_advance_by(Parser parser, int count) noexcept{
return Parser{parser.tokens, (parser.pos + count)};
}
bool parser_at_eof(Parser parser) noexcept{
return ((parser.pos >= parser.tokens.length()) ? true : [&]() {
auto tok = parser.tokens[parser.pos];
return std::visit(overloaded{[&](const ast_tokens::Eof& eof) { return true; },
[&](const auto& __v) { return false; }
}, tok.kind);
}());
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
bool is_fstr_literal(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::LFStr& lFStr) { auto [__0] = lFStr; return true; },
[&](const auto& __v) { return false; }
}, kind);
}
mlc::Array<mlc::String> get_fstr_parts(ast_tokens::TKind kind) noexcept{
return std::visit(overloaded{[&](const ast_tokens::LFStr& lFStr) { auto [parts] = lFStr; return parts; },
[&](const auto& __v) { return {}; }
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
self TKind_is_fn(ast_tokens::TKind self) noexcept{
return [=](auto KFn) { return true; };
}
self is_type(void self) noexcept{
return [=](auto KType) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_let(void self) noexcept{
return [=](auto KLet) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_mut(void self) noexcept{
return [=](auto KMut) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_const(void self) noexcept{
return [=](auto KConst) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_if(void self) noexcept{
return [=](auto KIf) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_unless(void self) noexcept{
return [=](auto KUnless) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_else(void self) noexcept{
return [=](auto KElse) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_end(void self) noexcept{
return [=](auto KEnd) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_do(void self) noexcept{
return [=](auto KDo) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_while(void self) noexcept{
return [=](auto KWhile) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_for(void self) noexcept{
return [=](auto KFor) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_match(void self) noexcept{
return [=](auto KMatch) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_return(void self) noexcept{
return [=](auto KReturn) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_break(void self) noexcept{
return [=](auto KBreak) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_continue(void self) noexcept{
return [=](auto KContinue) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_true(void self) noexcept{
return [=](auto KTrue) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_false(void self) noexcept{
return [=](auto KFalse) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_extern(void self) noexcept{
return [=](auto KExtern) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_extend(void self) noexcept{
return [=](auto KExtend) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_import(void self) noexcept{
return [=](auto KImport) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_from(void self) noexcept{
return [=](auto KFrom) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_eof(void self) noexcept{
return [=](auto Eof) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_ident(void self) noexcept{
return ast_tokens::Ident(_);
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_int(void self) noexcept{
return ast_tokens::LInt(_);
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_str(void self) noexcept{
return ast_tokens::LStr(_);
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_fstr(void self) noexcept{
return ast_tokens::LFStr(_);
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void fstr_parts(void self) noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_equal(void self) noexcept{
return [=](auto Equal) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_bar(void self) noexcept{
return [=](auto Bar) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_pipe(void self) noexcept{
return [=](auto Pipe) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_colon(void self) noexcept{
return [=](auto Colon) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_comma(void self) noexcept{
return [=](auto Comma) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_dot(void self) noexcept{
return [=](auto Dot) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_spread(void self) noexcept{
return [=](auto Spread) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_arrow(void self) noexcept{
return [=](auto Arrow) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_fat_arrow(void self) noexcept{
return [=](auto FatArrow) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_question(void self) noexcept{
return [=](auto Question) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_lparen(void self) noexcept{
return [=](auto LParen) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_rparen(void self) noexcept{
return [=](auto RParen) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_lbrace(void self) noexcept{
return [=](auto LBrace) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_rbrace(void self) noexcept{
return [=](auto RBrace) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_lbracket(void self) noexcept{
return [=](auto LBracket) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_rbracket(void self) noexcept{
return [=](auto RBracket) { return true; };
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self is_op(void self) noexcept{
return ast_tokens::Op(_);
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self ident(void self) noexcept{
return ast_tokens::Ident(name);
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self int_val(void self) noexcept{
return ast_tokens::LInt(n);
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self str_val(void self) noexcept{
return ast_tokens::LStr(s);
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
self op_val(void self) noexcept{
return ast_tokens::Op(s);
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void Parser_kind(Parser self) noexcept{
return ast_tokens::Token{};
}
void advance_by(count self) noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
pos at_eof(void self) noexcept{
return tokens.length();
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void skip_semi(void self) noexcept{
return ast_tokens::Token{};
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}

} // namespace preds
