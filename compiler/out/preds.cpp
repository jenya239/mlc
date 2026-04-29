#include "preds.hpp"

#include "ast_tokens.hpp"
#include "ast.hpp"

namespace preds {

using namespace ast_tokens;
using namespace ast;
using namespace ast_tokens;

preds::Parser parser_new_with_source_path(mlc::Array<ast_tokens::Token> tokens, mlc::String source_path) noexcept;

preds::Parser parser_new(mlc::Array<ast_tokens::Token> tokens) noexcept;

ast_tokens::TKind parser_kind(preds::Parser parser) noexcept;

preds::Parser parser_advance(preds::Parser parser) noexcept;

preds::Parser parser_advance_by(preds::Parser parser, int count) noexcept;

bool parser_at_eof(preds::Parser parser) noexcept;

preds::Parser parser_skip_semi(preds::Parser parser) noexcept;

bool is_keyword_fn(ast_tokens::TKind kind) noexcept;

bool is_keyword_type(ast_tokens::TKind kind) noexcept;

bool is_keyword_let(ast_tokens::TKind kind) noexcept;

bool is_keyword_mut(ast_tokens::TKind kind) noexcept;

bool is_keyword_const(ast_tokens::TKind kind) noexcept;

bool is_keyword_if(ast_tokens::TKind kind) noexcept;

bool is_keyword_unless(ast_tokens::TKind kind) noexcept;

bool is_keyword_else(ast_tokens::TKind kind) noexcept;

bool is_keyword_end(ast_tokens::TKind kind) noexcept;

bool is_keyword_do(ast_tokens::TKind kind) noexcept;

bool is_keyword_while(ast_tokens::TKind kind) noexcept;

bool is_keyword_for(ast_tokens::TKind kind) noexcept;

bool is_keyword_match(ast_tokens::TKind kind) noexcept;

bool is_keyword_return(ast_tokens::TKind kind) noexcept;

bool is_keyword_break(ast_tokens::TKind kind) noexcept;

bool is_keyword_continue(ast_tokens::TKind kind) noexcept;

bool is_keyword_true(ast_tokens::TKind kind) noexcept;

bool is_keyword_false(ast_tokens::TKind kind) noexcept;

bool is_keyword_extern(ast_tokens::TKind kind) noexcept;

bool is_keyword_extend(ast_tokens::TKind kind) noexcept;

bool is_ident(ast_tokens::TKind kind) noexcept;

bool is_int_literal(ast_tokens::TKind kind) noexcept;

bool is_str_literal(ast_tokens::TKind kind) noexcept;

bool is_template_literal(ast_tokens::TKind kind) noexcept;

mlc::Array<mlc::String> get_template_parts(ast_tokens::TKind kind) noexcept;

bool is_equal(ast_tokens::TKind kind) noexcept;

bool is_bar(ast_tokens::TKind kind) noexcept;

bool is_pipe(ast_tokens::TKind kind) noexcept;

bool is_colon(ast_tokens::TKind kind) noexcept;

bool is_comma(ast_tokens::TKind kind) noexcept;

bool is_dot(ast_tokens::TKind kind) noexcept;

bool is_spread(ast_tokens::TKind kind) noexcept;

bool is_arrow(ast_tokens::TKind kind) noexcept;

bool is_fat_arrow(ast_tokens::TKind kind) noexcept;

bool is_question(ast_tokens::TKind kind) noexcept;

bool is_left_paren(ast_tokens::TKind kind) noexcept;

bool is_right_paren(ast_tokens::TKind kind) noexcept;

bool is_left_brace(ast_tokens::TKind kind) noexcept;

bool is_right_brace(ast_tokens::TKind kind) noexcept;

bool is_left_bracket(ast_tokens::TKind kind) noexcept;

bool is_right_bracket(ast_tokens::TKind kind) noexcept;

bool is_operator(ast_tokens::TKind kind) noexcept;

mlc::String get_ident(ast_tokens::TKind kind) noexcept;

int get_int_literal(ast_tokens::TKind kind) noexcept;

mlc::String get_str_literal(ast_tokens::TKind kind) noexcept;

mlc::String get_operator(ast_tokens::TKind kind) noexcept;

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

ast_tokens::TKind Parser_kind(preds::Parser self) noexcept;

preds::Parser Parser_advance(preds::Parser self) noexcept;

preds::Parser Parser_advance_by(preds::Parser self, int count) noexcept;

bool Parser_at_eof(preds::Parser self) noexcept;

preds::Parser Parser_skip_semi(preds::Parser self) noexcept;

ast::Span Parser_span_at_cursor(preds::Parser self) noexcept;

int Parser_prev_line(preds::Parser self) noexcept;

preds::Parser parser_new_with_source_path(mlc::Array<ast_tokens::Token> tokens, mlc::String source_path) noexcept{return preds::Parser{tokens, 0, source_path};}

preds::Parser parser_new(mlc::Array<ast_tokens::Token> tokens) noexcept{return parser_new_with_source_path(tokens, mlc::String(""));}

ast_tokens::TKind parser_kind(preds::Parser parser) noexcept{
ast_tokens::Token tok = parser.tokens[parser.pos];
return tok.kind;
}

preds::Parser parser_advance(preds::Parser parser) noexcept{return preds::Parser{parser.tokens, parser.pos + 1, parser.source_path};}

preds::Parser parser_advance_by(preds::Parser parser, int count) noexcept{return preds::Parser{parser.tokens, parser.pos + count, parser.source_path};}

bool parser_at_eof(preds::Parser parser) noexcept{
return parser.pos >= parser.tokens.size() ? true : [&]() -> bool { 
  ast_tokens::Token tok = parser.tokens[parser.pos];
  return [&]() { if (std::holds_alternative<ast_tokens::Eof>(tok.kind)) {  return true; } return false; }();
 }();
}

preds::Parser parser_skip_semi(preds::Parser parser) noexcept{
ast_tokens::Token tok = parser.tokens[parser.pos];
return [&]() -> preds::Parser { if (std::holds_alternative<ast_tokens::Semicolon>(tok.kind)) {  return parser_advance(parser); } return parser; }();
}

bool is_keyword_fn(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KFn>(kind)) {  return true; } return false; }();}

bool is_keyword_type(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KType>(kind)) {  return true; } return false; }();}

bool is_keyword_let(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KLet>(kind)) {  return true; } return false; }();}

bool is_keyword_mut(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KMut>(kind)) {  return true; } return false; }();}

bool is_keyword_const(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KConst>(kind)) {  return true; } return false; }();}

bool is_keyword_if(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KIf>(kind)) {  return true; } return false; }();}

bool is_keyword_unless(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KUnless>(kind)) {  return true; } return false; }();}

bool is_keyword_else(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KElse>(kind)) {  return true; } return false; }();}

bool is_keyword_end(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KEnd>(kind)) {  return true; } return false; }();}

bool is_keyword_do(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KDo>(kind)) {  return true; } return false; }();}

bool is_keyword_while(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KWhile>(kind)) {  return true; } return false; }();}

bool is_keyword_for(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KFor>(kind)) {  return true; } return false; }();}

bool is_keyword_match(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KMatch>(kind)) {  return true; } return false; }();}

bool is_keyword_return(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KReturn>(kind)) {  return true; } return false; }();}

bool is_keyword_break(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KBreak>(kind)) {  return true; } return false; }();}

bool is_keyword_continue(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KContinue>(kind)) {  return true; } return false; }();}

bool is_keyword_true(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KTrue>(kind)) {  return true; } return false; }();}

bool is_keyword_false(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KFalse>(kind)) {  return true; } return false; }();}

bool is_keyword_extern(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KExtern>(kind)) {  return true; } return false; }();}

bool is_keyword_extend(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::KExtend>(kind)) {  return true; } return false; }();}

bool is_ident(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Ident>(kind)) { auto _v_ident = std::get<ast_tokens::Ident>(kind); auto [_w0] = _v_ident; return true; } return false; }();}

bool is_int_literal(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LInt>(kind)) { auto _v_lint = std::get<ast_tokens::LInt>(kind); auto [_w0] = _v_lint; return true; } return false; }();}

bool is_str_literal(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LStr>(kind)) { auto _v_lstr = std::get<ast_tokens::LStr>(kind); auto [_w0] = _v_lstr; return true; } return false; }();}

bool is_template_literal(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LTemplate>(kind)) { auto _v_ltemplate = std::get<ast_tokens::LTemplate>(kind); auto [_w0] = _v_ltemplate; return true; } return false; }();}

mlc::Array<mlc::String> get_template_parts(ast_tokens::TKind kind) noexcept{return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<ast_tokens::LTemplate>(kind)) { auto _v_ltemplate = std::get<ast_tokens::LTemplate>(kind); auto [parts] = _v_ltemplate; return parts; } return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> r = {};
  return r;
 }(); }();}

bool is_equal(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Equal>(kind)) {  return true; } return false; }();}

bool is_bar(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Bar>(kind)) {  return true; } return false; }();}

bool is_pipe(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Pipe>(kind)) {  return true; } return false; }();}

bool is_colon(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Colon>(kind)) {  return true; } return false; }();}

bool is_comma(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Comma>(kind)) {  return true; } return false; }();}

bool is_dot(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Dot>(kind)) {  return true; } return false; }();}

bool is_spread(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Spread>(kind)) {  return true; } return false; }();}

bool is_arrow(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Arrow>(kind)) {  return true; } return false; }();}

bool is_fat_arrow(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::FatArrow>(kind)) {  return true; } return false; }();}

bool is_question(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Question>(kind)) {  return true; } return false; }();}

bool is_left_paren(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LParen>(kind)) {  return true; } return false; }();}

bool is_right_paren(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::RParen>(kind)) {  return true; } return false; }();}

bool is_left_brace(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LBrace>(kind)) {  return true; } return false; }();}

bool is_right_brace(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::RBrace>(kind)) {  return true; } return false; }();}

bool is_left_bracket(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LBracket>(kind)) {  return true; } return false; }();}

bool is_right_bracket(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::RBracket>(kind)) {  return true; } return false; }();}

bool is_operator(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::Op>(kind)) { auto _v_op = std::get<ast_tokens::Op>(kind); auto [_w0] = _v_op; return true; } return false; }();}

mlc::String get_ident(ast_tokens::TKind kind) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast_tokens::Ident>(kind)) { auto _v_ident = std::get<ast_tokens::Ident>(kind); auto [name] = _v_ident; return name; } return mlc::String(""); }();}

int get_int_literal(ast_tokens::TKind kind) noexcept{return [&]() { if (std::holds_alternative<ast_tokens::LInt>(kind)) { auto _v_lint = std::get<ast_tokens::LInt>(kind); auto [n] = _v_lint; return n; } return 0; }();}

mlc::String get_str_literal(ast_tokens::TKind kind) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast_tokens::LStr>(kind)) { auto _v_lstr = std::get<ast_tokens::LStr>(kind); auto [s] = _v_lstr; return s; } return mlc::String(""); }();}

mlc::String get_operator(ast_tokens::TKind kind) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast_tokens::Op>(kind)) { auto _v_op = std::get<ast_tokens::Op>(kind); auto [s] = _v_op; return s; } return mlc::String(""); }();}

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

ast_tokens::TKind Parser_kind(preds::Parser self) noexcept{
ast_tokens::Token tok = self.tokens[self.pos];
return tok.kind;
}

preds::Parser Parser_advance(preds::Parser self) noexcept{return preds::Parser{self.tokens, self.pos + 1, self.source_path};}

preds::Parser Parser_advance_by(preds::Parser self, int count) noexcept{return preds::Parser{self.tokens, self.pos + count, self.source_path};}

bool Parser_at_eof(preds::Parser self) noexcept{return self.pos >= self.tokens.size() ? true : [&]() -> bool { 
  ast_tokens::Token tok = self.tokens[self.pos];
  return TKind_is_eof(tok.kind);
 }();}

preds::Parser Parser_skip_semi(preds::Parser self) noexcept{
ast_tokens::Token tok = self.tokens[self.pos];
return [&]() -> preds::Parser { if (std::holds_alternative<ast_tokens::Semicolon>(tok.kind)) {  return Parser_advance(self); } return self; }();
}

ast::Span Parser_span_at_cursor(preds::Parser self) noexcept{
ast_tokens::Token tok = self.tokens[self.pos];
return ast::Span{self.source_path, tok.line, tok.col};
}

int Parser_prev_line(preds::Parser self) noexcept{
return self.pos > 0 ? [&]() -> int { 
  ast_tokens::Token tok = self.tokens[self.pos - 1];
  return tok.line;
 }() : 0;
}

} // namespace preds
