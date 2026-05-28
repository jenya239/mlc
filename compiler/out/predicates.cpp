#include "predicates.hpp"

#include "ast_tokens.hpp"
#include "ast.hpp"

namespace predicates {

using namespace ast_tokens;
using namespace ast;
using namespace ast_tokens;

predicates::Parser parser_new_with_source_path(mlc::Array<ast_tokens::Token> tokens, mlc::String source_path) noexcept;

predicates::Parser parser_new(mlc::Array<ast_tokens::Token> tokens) noexcept;

bool is_ctor_name(mlc::String name) noexcept;

bool TKind_is_fn(ast_tokens::TKind self) noexcept;

bool TKind_is_type(ast_tokens::TKind self) noexcept;

bool TKind_is_let(ast_tokens::TKind self) noexcept;

bool TKind_is_mut(ast_tokens::TKind self) noexcept;

bool TKind_is_const(ast_tokens::TKind self) noexcept;

bool TKind_is_if(ast_tokens::TKind self) noexcept;

bool TKind_is_unless(ast_tokens::TKind self) noexcept;

bool TKind_is_else(ast_tokens::TKind self) noexcept;

bool TKind_is_end(ast_tokens::TKind self) noexcept;

bool TKind_is_do(ast_tokens::TKind self) noexcept;

bool TKind_is_while(ast_tokens::TKind self) noexcept;

bool TKind_is_for(ast_tokens::TKind self) noexcept;

bool TKind_is_with(ast_tokens::TKind self) noexcept;

bool TKind_is_match(ast_tokens::TKind self) noexcept;

bool TKind_is_return(ast_tokens::TKind self) noexcept;

bool TKind_is_break(ast_tokens::TKind self) noexcept;

bool TKind_is_continue(ast_tokens::TKind self) noexcept;

bool TKind_is_true(ast_tokens::TKind self) noexcept;

bool TKind_is_false(ast_tokens::TKind self) noexcept;

bool TKind_is_extern(ast_tokens::TKind self) noexcept;

bool TKind_is_extend(ast_tokens::TKind self) noexcept;

bool TKind_is_where(ast_tokens::TKind self) noexcept;

bool TKind_is_import(ast_tokens::TKind self) noexcept;

bool TKind_is_as(ast_tokens::TKind self) noexcept;

bool TKind_is_from(ast_tokens::TKind self) noexcept;

bool TKind_is_eof(ast_tokens::TKind self) noexcept;

bool TKind_is_ident(ast_tokens::TKind self) noexcept;

bool TKind_is_int(ast_tokens::TKind self) noexcept;

bool TKind_is_str(ast_tokens::TKind self) noexcept;

bool TKind_is_template(ast_tokens::TKind self) noexcept;

mlc::Array<mlc::String> TKind_template_parts(ast_tokens::TKind self) noexcept;

bool TKind_is_equal(ast_tokens::TKind self) noexcept;

bool TKind_is_bar(ast_tokens::TKind self) noexcept;

bool TKind_is_pipe(ast_tokens::TKind self) noexcept;

bool TKind_is_colon(ast_tokens::TKind self) noexcept;

bool TKind_is_comma(ast_tokens::TKind self) noexcept;

bool TKind_is_dot(ast_tokens::TKind self) noexcept;

bool TKind_is_spread(ast_tokens::TKind self) noexcept;

bool TKind_is_arrow(ast_tokens::TKind self) noexcept;

bool TKind_is_fat_arrow(ast_tokens::TKind self) noexcept;

bool TKind_is_question(ast_tokens::TKind self) noexcept;

bool TKind_is_lparen(ast_tokens::TKind self) noexcept;

bool TKind_is_rparen(ast_tokens::TKind self) noexcept;

bool TKind_is_lbrace(ast_tokens::TKind self) noexcept;

bool TKind_is_rbrace(ast_tokens::TKind self) noexcept;

bool TKind_is_lbracket(ast_tokens::TKind self) noexcept;

bool TKind_is_rbracket(ast_tokens::TKind self) noexcept;

bool TKind_is_op(ast_tokens::TKind self) noexcept;

mlc::String TKind_ident(ast_tokens::TKind self) noexcept;

int TKind_int_val(ast_tokens::TKind self) noexcept;

mlc::String TKind_str_val(ast_tokens::TKind self) noexcept;

mlc::String TKind_op_val(ast_tokens::TKind self) noexcept;

bool TKind_is_float(ast_tokens::TKind self) noexcept;

bool TKind_is_i64(ast_tokens::TKind self) noexcept;

bool TKind_is_u8(ast_tokens::TKind self) noexcept;

bool TKind_is_usize(ast_tokens::TKind self) noexcept;

bool TKind_is_char_lit(ast_tokens::TKind self) noexcept;

mlc::String TKind_float_val(ast_tokens::TKind self) noexcept;

mlc::String TKind_i64_val(ast_tokens::TKind self) noexcept;

mlc::String TKind_u8_val(ast_tokens::TKind self) noexcept;

mlc::String TKind_usize_val(ast_tokens::TKind self) noexcept;

mlc::String TKind_char_val(ast_tokens::TKind self) noexcept;

ast_tokens::TKind Parser_kind(predicates::Parser self) noexcept;

bool Parser_lambda_shorthand_suppression_active(predicates::Parser self) noexcept;

predicates::Parser Parser_with_lambda_shorthand_suppressed(predicates::Parser self, bool value) noexcept;

predicates::Parser Parser_advance(predicates::Parser self) noexcept;

predicates::Parser Parser_advance_by(predicates::Parser self, int count) noexcept;

bool Parser_at_eof(predicates::Parser self) noexcept;

predicates::Parser Parser_skip_semi(predicates::Parser self) noexcept;

ast::Span Parser_span_at_cursor(predicates::Parser self) noexcept;

int Parser_prev_line(predicates::Parser self) noexcept;

predicates::Parser parser_new_with_source_path(mlc::Array<ast_tokens::Token> tokens, mlc::String source_path) noexcept{return predicates::Parser{tokens, 0, source_path, false};}

predicates::Parser parser_new(mlc::Array<ast_tokens::Token> tokens) noexcept{return parser_new_with_source_path(tokens, mlc::String(""));}

bool is_ctor_name(mlc::String name) noexcept{return name.char_at(0) >= mlc::String("A") && name.char_at(0) <= mlc::String("Z");}

bool TKind_is_fn(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KFn>(self)) {  return true; } return false; }();}

bool TKind_is_type(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KType>(self)) {  return true; } return false; }();}

bool TKind_is_let(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KLet>(self)) {  return true; } return false; }();}

bool TKind_is_mut(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KMut>(self)) {  return true; } return false; }();}

bool TKind_is_const(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KConst>(self)) {  return true; } return false; }();}

bool TKind_is_if(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KIf>(self)) {  return true; } return false; }();}

bool TKind_is_unless(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KUnless>(self)) {  return true; } return false; }();}

bool TKind_is_else(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KElse>(self)) {  return true; } return false; }();}

bool TKind_is_end(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KEnd>(self)) {  return true; } return false; }();}

bool TKind_is_do(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KDo>(self)) {  return true; } return false; }();}

bool TKind_is_while(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KWhile>(self)) {  return true; } return false; }();}

bool TKind_is_for(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KFor>(self)) {  return true; } return false; }();}

bool TKind_is_with(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KWith>(self)) {  return true; } return false; }();}

bool TKind_is_match(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KMatch>(self)) {  return true; } return false; }();}

bool TKind_is_return(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KReturn>(self)) {  return true; } return false; }();}

bool TKind_is_break(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KBreak>(self)) {  return true; } return false; }();}

bool TKind_is_continue(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KContinue>(self)) {  return true; } return false; }();}

bool TKind_is_true(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KTrue>(self)) {  return true; } return false; }();}

bool TKind_is_false(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KFalse>(self)) {  return true; } return false; }();}

bool TKind_is_extern(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KExtern>(self)) {  return true; } return false; }();}

bool TKind_is_extend(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KExtend>(self)) {  return true; } return false; }();}

bool TKind_is_where(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KWhere>(self)) {  return true; } return false; }();}

bool TKind_is_import(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KImport>(self)) {  return true; } return false; }();}

bool TKind_is_as(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KAs>(self)) {  return true; } return false; }();}

bool TKind_is_from(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KFrom>(self)) {  return true; } return false; }();}

bool TKind_is_eof(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Eof>(self)) {  return true; } return false; }();}

bool TKind_is_ident(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Ident>(self)) { auto _v_ident = std::get<ast_tokens::Ident>(self); auto [_w0] = _v_ident; return true; } return false; }();}

bool TKind_is_int(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LInt>(self)) { auto _v_lint = std::get<ast_tokens::LInt>(self); auto [_w0] = _v_lint; return true; } return false; }();}

bool TKind_is_str(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LStr>(self)) { auto _v_lstr = std::get<ast_tokens::LStr>(self); auto [_w0] = _v_lstr; return true; } return false; }();}

bool TKind_is_template(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LTemplate>(self)) { auto _v_ltemplate = std::get<ast_tokens::LTemplate>(self); auto [_w0] = _v_ltemplate; return true; } return false; }();}

mlc::Array<mlc::String> TKind_template_parts(ast_tokens::TKind self) noexcept{return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<ast_tokens::LTemplate>(self)) { auto _v_ltemplate = std::get<ast_tokens::LTemplate>(self); auto [parts] = _v_ltemplate; return parts; } return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> r = {};
  return r;
 }(); }();}

bool TKind_is_equal(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Equal>(self)) {  return true; } return false; }();}

bool TKind_is_bar(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Bar>(self)) {  return true; } return false; }();}

bool TKind_is_pipe(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Pipe>(self)) {  return true; } return false; }();}

bool TKind_is_colon(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Colon>(self)) {  return true; } return false; }();}

bool TKind_is_comma(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Comma>(self)) {  return true; } return false; }();}

bool TKind_is_dot(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Dot>(self)) {  return true; } return false; }();}

bool TKind_is_spread(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Spread>(self)) {  return true; } return false; }();}

bool TKind_is_arrow(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Arrow>(self)) {  return true; } return false; }();}

bool TKind_is_fat_arrow(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::FatArrow>(self)) {  return true; } return false; }();}

bool TKind_is_question(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Question>(self)) {  return true; } return false; }();}

bool TKind_is_lparen(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LParen>(self)) {  return true; } return false; }();}

bool TKind_is_rparen(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::RParen>(self)) {  return true; } return false; }();}

bool TKind_is_lbrace(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LBrace>(self)) {  return true; } return false; }();}

bool TKind_is_rbrace(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::RBrace>(self)) {  return true; } return false; }();}

bool TKind_is_lbracket(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LBracket>(self)) {  return true; } return false; }();}

bool TKind_is_rbracket(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::RBracket>(self)) {  return true; } return false; }();}

bool TKind_is_op(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Op>(self)) { auto _v_op = std::get<ast_tokens::Op>(self); auto [_w0] = _v_op; return true; } return false; }();}

mlc::String TKind_ident(ast_tokens::TKind self) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast_tokens::Ident>(self)) { auto _v_ident = std::get<ast_tokens::Ident>(self); auto [name] = _v_ident; return name; } return mlc::String(""); }();}

int TKind_int_val(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LInt>(self)) { auto _v_lint = std::get<ast_tokens::LInt>(self); auto [n] = _v_lint; return n; } return 0; }();}

mlc::String TKind_str_val(ast_tokens::TKind self) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast_tokens::LStr>(self)) { auto _v_lstr = std::get<ast_tokens::LStr>(self); auto [s] = _v_lstr; return s; } return mlc::String(""); }();}

mlc::String TKind_op_val(ast_tokens::TKind self) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast_tokens::Op>(self)) { auto _v_op = std::get<ast_tokens::Op>(self); auto [s] = _v_op; return s; } return mlc::String(""); }();}

bool TKind_is_float(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LFloat>(self)) { auto _v_lfloat = std::get<ast_tokens::LFloat>(self); auto [_w0] = _v_lfloat; return true; } return false; }();}

bool TKind_is_i64(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LI64>(self)) { auto _v_li64 = std::get<ast_tokens::LI64>(self); auto [_w0] = _v_li64; return true; } return false; }();}

bool TKind_is_u8(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LU8>(self)) { auto _v_lu8 = std::get<ast_tokens::LU8>(self); auto [_w0] = _v_lu8; return true; } return false; }();}

bool TKind_is_usize(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LUsize>(self)) { auto _v_lusize = std::get<ast_tokens::LUsize>(self); auto [_w0] = _v_lusize; return true; } return false; }();}

bool TKind_is_char_lit(ast_tokens::TKind self) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LChar>(self)) { auto _v_lchar = std::get<ast_tokens::LChar>(self); auto [_w0] = _v_lchar; return true; } return false; }();}

mlc::String TKind_float_val(ast_tokens::TKind self) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast_tokens::LFloat>(self)) { auto _v_lfloat = std::get<ast_tokens::LFloat>(self); auto [s] = _v_lfloat; return s; } return mlc::String(""); }();}

mlc::String TKind_i64_val(ast_tokens::TKind self) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast_tokens::LI64>(self)) { auto _v_li64 = std::get<ast_tokens::LI64>(self); auto [s] = _v_li64; return s; } return mlc::String(""); }();}

mlc::String TKind_u8_val(ast_tokens::TKind self) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast_tokens::LU8>(self)) { auto _v_lu8 = std::get<ast_tokens::LU8>(self); auto [s] = _v_lu8; return s; } return mlc::String(""); }();}

mlc::String TKind_usize_val(ast_tokens::TKind self) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast_tokens::LUsize>(self)) { auto _v_lusize = std::get<ast_tokens::LUsize>(self); auto [s] = _v_lusize; return s; } return mlc::String(""); }();}

mlc::String TKind_char_val(ast_tokens::TKind self) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast_tokens::LChar>(self)) { auto _v_lchar = std::get<ast_tokens::LChar>(self); auto [s] = _v_lchar; return s; } return mlc::String(""); }();}

ast_tokens::TKind Parser_kind(predicates::Parser self) noexcept{return self.pos >= self.tokens.size() ? (ast_tokens::Eof{}) : [&]() -> ast_tokens::TKind { 
  ast_tokens::Token tok = self.tokens[self.pos];
  return tok.kind;
 }();}

bool Parser_lambda_shorthand_suppression_active(predicates::Parser self) noexcept{return self.suppress_lambda_shorthand;}

predicates::Parser Parser_with_lambda_shorthand_suppressed(predicates::Parser self, bool value) noexcept{return predicates::Parser{self.tokens, self.pos, self.source_path, value};}

predicates::Parser Parser_advance(predicates::Parser self) noexcept{return predicates::Parser{self.tokens, self.pos + 1, self.source_path, self.suppress_lambda_shorthand};}

predicates::Parser Parser_advance_by(predicates::Parser self, int count) noexcept{return predicates::Parser{self.tokens, self.pos + count, self.source_path, self.suppress_lambda_shorthand};}

bool Parser_at_eof(predicates::Parser self) noexcept{return self.pos >= self.tokens.size() ? true : [&]() -> bool { 
  ast_tokens::Token tok = self.tokens[self.pos];
  return TKind_is_eof(tok.kind);
 }();}

predicates::Parser Parser_skip_semi(predicates::Parser self) noexcept{return self.pos >= self.tokens.size() ? self : [&]() -> predicates::Parser { 
  ast_tokens::Token tok = self.tokens[self.pos];
  return [&]() -> predicates::Parser { if (std::holds_alternative<ast_tokens::Semicolon>(tok.kind)) {  return Parser_advance(self); } return self; }();
 }();}

ast::Span Parser_span_at_cursor(predicates::Parser self) noexcept{return self.pos >= self.tokens.size() ? ast::Span{self.source_path, 0, 0} : [&]() -> ast::Span { 
  ast_tokens::Token tok = self.tokens[self.pos];
  return ast::Span{self.source_path, tok.line, tok.col};
 }();}

int Parser_prev_line(predicates::Parser self) noexcept{
return self.pos > 0 ? [&]() -> int { 
  ast_tokens::Token tok = self.tokens[self.pos - 1];
  return tok.line;
 }() : 0;
}

} // namespace predicates
