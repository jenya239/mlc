#include "exprs.hpp"

#include "predicates.hpp"
#include "types.hpp"
#include "ast.hpp"
#include "ast_tokens.hpp"
#include "lexer.hpp"

namespace exprs {

using namespace predicates;
using namespace types;
using namespace ast;
using namespace ast_tokens;
using namespace lexer;
using namespace ast_tokens;

struct MatchArmParseOutcome {std::shared_ptr<ast::MatchArm> arm;predicates::Parser parser;};

predicates::StmtsResult parse_stmts_until_end(predicates::Parser parser) noexcept;

predicates::StmtsResult parse_stmts_until_else_end(predicates::Parser parser) noexcept;

predicates::StmtResult parse_statement_let_const(predicates::Parser parser, ast::Span statement_span) noexcept;

ast_tokens::TKind parser_next_kind(predicates::Parser p) noexcept;

predicates::TypeResult parse_after_let_pattern(predicates::Parser parser) noexcept;

predicates::PatResult parse_array_pattern(predicates::Parser parser, ast::Span bracket_span) noexcept;

predicates::StmtResult parse_statement_let(predicates::Parser parser) noexcept;

predicates::StmtResult parse_statement_const(predicates::Parser parser) noexcept;

predicates::StmtResult parse_statement_return(predicates::Parser parser) noexcept;

predicates::StmtResult parse_statement_expression_or_assign(predicates::Parser parser) noexcept;

predicates::StmtResult parse_stmt(predicates::Parser parser) noexcept;

predicates::PatResult parse_pattern_identifier_branch(predicates::Parser parser, mlc::String name) noexcept;

predicates::PatResult parse_pattern_integer(predicates::Parser parser, int value) noexcept;

predicates::PatResult parse_pattern_string(predicates::Parser parser, mlc::String value) noexcept;

predicates::PatResult parse_pattern_boolean(predicates::Parser parser, bool value) noexcept;

predicates::PatResult parse_pattern_fallback_wildcard(predicates::Parser parser) noexcept;

predicates::PatResult parse_or_pat(predicates::Parser parser) noexcept;

predicates::PatResult parse_pat(predicates::Parser parser) noexcept;

predicates::PatsResult comma_separated_pat_suffix_from_first(predicates::PatResult first) noexcept;

predicates::PatsResult parse_pat_args(predicates::Parser parser) noexcept;

predicates::PatsResult parse_record_pat_fields(predicates::Parser parser) noexcept;

predicates::ExprResult parse_expr(predicates::Parser parser) noexcept;

std::shared_ptr<ast::Expr> pipe_desugar(std::shared_ptr<ast::Expr> left_expr, std::shared_ptr<ast::Expr> right_expr) noexcept;

predicates::ExprResult parse_pipe(predicates::Parser parser) noexcept;

predicates::ExprsResult comma_separated_expr_suffix_from_first(predicates::ExprResult first) noexcept;

predicates::ExprResult parse_or(predicates::Parser parser) noexcept;

predicates::ExprResult parse_and(predicates::Parser parser) noexcept;

predicates::ExprResult parse_equality(predicates::Parser parser) noexcept;

bool is_relational_comparison_operator(mlc::String operator_) noexcept;

predicates::ExprResult parse_comparison_relational(predicates::Parser parser) noexcept;

predicates::ExprResult parse_add(predicates::Parser parser) noexcept;

predicates::ExprResult parse_mul(predicates::Parser parser) noexcept;

predicates::ExprResult parse_unary(predicates::Parser parser) noexcept;

predicates::ExprResult parse_postfix(predicates::Parser parser) noexcept;

predicates::ExprsResult parse_arg_list(predicates::Parser parser) noexcept;

std::shared_ptr<ast::Expr> build_template_expr(mlc::Array<mlc::String> parts) noexcept;

predicates::ExprResult parse_primary_integer_literal(predicates::Parser parser, int value) noexcept;

predicates::ExprResult parse_primary_string_literal(predicates::Parser parser, mlc::String value) noexcept;

predicates::ExprResult parse_primary_template_literal(predicates::Parser parser, mlc::Array<mlc::String> parts) noexcept;

predicates::ExprResult parse_primary_boolean_literal(predicates::Parser parser, bool value) noexcept;

predicates::ExprResult parse_primary_parenthesized(predicates::Parser parser) noexcept;

predicates::ExprResult parse_primary_if_or_unless(predicates::Parser parser) noexcept;

predicates::ExprResult parse_primary_do_block(predicates::Parser parser) noexcept;

predicates::ExprResult parse_primary_while_loop(predicates::Parser parser) noexcept;

predicates::ExprResult parse_primary_for_loop(predicates::Parser parser) noexcept;

predicates::ExprResult parse_primary_match(predicates::Parser parser) noexcept;

predicates::ExprResult parse_primary_return_as_block(predicates::Parser parser) noexcept;

predicates::ExprResult parse_primary_identifier(predicates::Parser parser, mlc::String name) noexcept;

predicates::ExprResult parse_primary_unit_fallback(predicates::Parser parser) noexcept;

predicates::ExprResult parse_primary(predicates::Parser parser) noexcept;

bool looks_like_typed_lambda_params(predicates::Parser parser) noexcept;

predicates::NamesResult parse_typed_lambda_params(predicates::Parser parser) noexcept;

bool looks_like_lambda_params(predicates::Parser parser) noexcept;

predicates::NamesResult parse_lambda_params(predicates::Parser parser) noexcept;

predicates::ExprResult parse_array_lit(predicates::Parser parser, ast::Span header_span) noexcept;

predicates::ExprResult parse_if_expr(predicates::Parser parser) noexcept;

predicates::ExprResult parse_block(predicates::Parser parser, ast::Span header_span) noexcept;

bool is_unit_expr(std::shared_ptr<ast::Expr> expr) noexcept;

std::shared_ptr<ast::Expr> block_result(mlc::Array<std::shared_ptr<ast::Stmt>> stmts) noexcept;

mlc::Array<std::shared_ptr<ast::Stmt>> block_body(mlc::Array<std::shared_ptr<ast::Stmt>> stmts) noexcept;

std::shared_ptr<ast::Expr> StmtsResult_to_block_expr(predicates::StmtsResult self, ast::Span header_span) noexcept;

predicates::ExprResult parse_while_expr(predicates::Parser parser, ast::Span header_span) noexcept;

predicates::ExprResult parse_for_expr(predicates::Parser parser, ast::Span header_span) noexcept;

predicates::ExprResult parse_with_expr(predicates::Parser parser, ast::Span header_span) noexcept;

predicates::ExprResult parse_match_expr(predicates::Parser parser, ast::Span header_span) noexcept;

exprs::MatchArmParseOutcome parse_match_arm(predicates::PatResult pat_result) noexcept;

predicates::ArmsResult parse_arms_brace(predicates::Parser parser) noexcept;

predicates::ArmsResult parse_arms_do_delimited(predicates::Parser parser) noexcept;

predicates::ArmsResult parse_arms_pipe(predicates::Parser parser) noexcept;

predicates::ArmsResult parse_arms(predicates::Parser parser) noexcept;

predicates::RecordLitPartsResult parse_record_lit_parts(predicates::Parser parser) noexcept;

predicates::ExprResult parse_record_expr(predicates::Parser parser, mlc::String record_name, ast::Span record_span) noexcept;

predicates::StmtsResult parse_stmts_until_end(predicates::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::Stmt>> stmts = {};
predicates::Parser state = std::move(parser);
while (!predicates::TKind_is_end(predicates::Parser_kind(state)) && !predicates::Parser_at_eof(state)){
{
predicates::StmtResult result = parse_stmt(state);
stmts.push_back(result.stmt);
state = predicates::Parser_skip_semi(result.parser);
}
}
return predicates::StmtsResult{stmts, predicates::Parser_advance(state)};
}

predicates::StmtsResult parse_stmts_until_else_end(predicates::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::Stmt>> stmts = {};
predicates::Parser state = std::move(parser);
while (!predicates::TKind_is_end(predicates::Parser_kind(state)) && !predicates::TKind_is_else(predicates::Parser_kind(state)) && !predicates::Parser_at_eof(state)){
{
predicates::StmtResult result = parse_stmt(state);
stmts.push_back(result.stmt);
state = result.parser;
}
}
return predicates::StmtsResult{stmts, state};
}

predicates::StmtResult parse_statement_let_const(predicates::Parser parser, ast::Span statement_span) noexcept{
predicates::Parser name_pos = predicates::Parser_advance(parser);
mlc::String var_name = predicates::TKind_ident(predicates::Parser_kind(name_pos));
predicates::TypeResult type_result = predicates::TKind_is_colon(predicates::Parser_kind(predicates::Parser_advance(name_pos))) ? types::parse_type(predicates::Parser_advance_by(name_pos, 2)) : predicates::TypeResult{std::make_shared<ast::TypeExpr>((ast::TyUnit{})), predicates::Parser_advance(name_pos)};
predicates::ExprResult value_result = parse_expr(predicates::Parser_advance(type_result.parser));
return predicates::StmtResult{std::make_shared<ast::Stmt>(ast::StmtLetConst(var_name, type_result.type_expr, value_result.expr, statement_span)), value_result.parser};
}

ast_tokens::TKind parser_next_kind(predicates::Parser p) noexcept{
return p.pos + 1 >= p.tokens.size() ? ast_tokens::TKind((ast_tokens::Eof{})) : ast_tokens::TKind(p.tokens[p.pos + 1].kind);
}

predicates::TypeResult parse_after_let_pattern(predicates::Parser parser) noexcept{
return predicates::TKind_is_colon(predicates::Parser_kind(parser)) ? [&]() -> predicates::TypeResult { 
  predicates::TypeResult tr = types::parse_type(predicates::Parser_advance(parser));
  return predicates::TKind_is_equal(predicates::Parser_kind(tr.parser)) ? predicates::TypeResult{tr.type_expr, predicates::Parser_advance(tr.parser)} : predicates::TypeResult{tr.type_expr, tr.parser};
 }() : predicates::TKind_is_equal(predicates::Parser_kind(parser)) ? predicates::TypeResult{std::make_shared<ast::TypeExpr>((ast::TyUnit{})), predicates::Parser_advance(parser)} : predicates::TypeResult{std::make_shared<ast::TypeExpr>((ast::TyUnit{})), parser};
}

predicates::PatResult parse_array_pattern(predicates::Parser parser, ast::Span bracket_span) noexcept{
return predicates::TKind_is_rbracket(predicates::Parser_kind(parser)) ? predicates::PatResult{std::make_shared<ast::Pat>(ast::PatArray({}, mlc::String(""), bracket_span)), predicates::Parser_advance(parser)} : [&]() -> predicates::PatResult { 
  predicates::PatResult first = parse_pat(parser);
  mlc::Array<std::shared_ptr<ast::Pat>> pats = {};
  pats.push_back(first.pat);
  predicates::Parser st = first.parser;
  mlc::String rest = mlc::String("");
  while (predicates::TKind_is_comma(predicates::Parser_kind(st))){
{
st = predicates::Parser_advance(st);
if (predicates::TKind_is_spread(predicates::Parser_kind(st))){
{
st = predicates::Parser_advance(st);
if (predicates::TKind_is_ident(predicates::Parser_kind(st))){
{
rest = predicates::TKind_ident(predicates::Parser_kind(st));
st = predicates::Parser_advance(st);
}
}
break;
}
} else {
{
predicates::PatResult nxt = parse_pat(st);
pats.push_back(nxt.pat);
st = nxt.parser;
}
}
}
}
  return predicates::TKind_is_rbracket(predicates::Parser_kind(st)) ? predicates::PatResult{std::make_shared<ast::Pat>(ast::PatArray(pats, rest, bracket_span)), predicates::Parser_advance(st)} : predicates::PatResult{std::make_shared<ast::Pat>(ast::PatArray(pats, rest, bracket_span)), st};
 }();
}

predicates::StmtResult parse_statement_let(predicates::Parser parser) noexcept{
ast::Span statement_span = predicates::Parser_span_at_cursor(parser);
predicates::Parser after_let = predicates::Parser_advance(parser);
return predicates::TKind_is_const(predicates::Parser_kind(after_let)) ? parse_statement_let_const(after_let, statement_span) : [&]() -> predicates::StmtResult { 
  bool is_mut = predicates::TKind_is_mut(predicates::Parser_kind(after_let));
  predicates::Parser name_pos = is_mut ? predicates::Parser_advance(after_let) : after_let;
  ast_tokens::TKind k0 = predicates::Parser_kind(name_pos);
  return predicates::TKind_is_lparen(k0) ? [&]() -> predicates::StmtResult { 
  ast::Span paren_span = predicates::Parser_span_at_cursor(name_pos);
  predicates::PatsResult inner = parse_pat_args(predicates::Parser_advance(name_pos));
  std::shared_ptr<ast::Pat> pat = std::make_shared<ast::Pat>(ast::PatTuple(inner.pats, paren_span));
  predicates::TypeResult type_result = parse_after_let_pattern(inner.parser);
  predicates::ExprResult value_result = parse_expr(type_result.parser);
  bool has_else = predicates::TKind_is_else(predicates::Parser_kind(value_result.parser));
  predicates::StmtsResult else_result = has_else ? parse_stmts_until_end(predicates::Parser_advance(value_result.parser)) : predicates::StmtsResult{{}, value_result.parser};
  std::shared_ptr<ast::Expr> else_body = std::make_shared<ast::Expr>(ast::ExprBlock(else_result.stmts, std::make_shared<ast::Expr>(ast::ExprUnit(statement_span)), statement_span));
  return predicates::StmtResult{std::make_shared<ast::Stmt>(ast::StmtLetPat(pat, is_mut, type_result.type_expr, value_result.expr, has_else, else_body, statement_span)), else_result.parser};
 }() : predicates::TKind_is_lbracket(k0) ? [&]() -> predicates::StmtResult { 
  ast::Span br_span = predicates::Parser_span_at_cursor(name_pos);
  predicates::PatResult ar = parse_array_pattern(predicates::Parser_advance(name_pos), br_span);
  predicates::TypeResult type_result = parse_after_let_pattern(ar.parser);
  predicates::ExprResult value_result = parse_expr(type_result.parser);
  bool has_else = predicates::TKind_is_else(predicates::Parser_kind(value_result.parser));
  predicates::StmtsResult else_result = has_else ? parse_stmts_until_end(predicates::Parser_advance(value_result.parser)) : predicates::StmtsResult{{}, value_result.parser};
  std::shared_ptr<ast::Expr> else_body = std::make_shared<ast::Expr>(ast::ExprBlock(else_result.stmts, std::make_shared<ast::Expr>(ast::ExprUnit(statement_span)), statement_span));
  return predicates::StmtResult{std::make_shared<ast::Stmt>(ast::StmtLetPat(ar.pat, is_mut, type_result.type_expr, value_result.expr, has_else, else_body, statement_span)), else_result.parser};
 }() : predicates::TKind_is_lbrace(k0) ? [&]() -> predicates::StmtResult { 
  ast::Span br_span = predicates::Parser_span_at_cursor(name_pos);
  predicates::PatsResult fr = parse_record_pat_fields(predicates::Parser_advance(name_pos));
  std::shared_ptr<ast::Pat> pat = std::make_shared<ast::Pat>(ast::PatRecord(mlc::String(""), fr.pats, br_span));
  predicates::TypeResult type_result = parse_after_let_pattern(fr.parser);
  predicates::ExprResult value_result = parse_expr(type_result.parser);
  bool has_else = predicates::TKind_is_else(predicates::Parser_kind(value_result.parser));
  predicates::StmtsResult else_result = has_else ? parse_stmts_until_end(predicates::Parser_advance(value_result.parser)) : predicates::StmtsResult{{}, value_result.parser};
  std::shared_ptr<ast::Expr> else_body = std::make_shared<ast::Expr>(ast::ExprBlock(else_result.stmts, std::make_shared<ast::Expr>(ast::ExprUnit(statement_span)), statement_span));
  return predicates::StmtResult{std::make_shared<ast::Stmt>(ast::StmtLetPat(pat, is_mut, type_result.type_expr, value_result.expr, has_else, else_body, statement_span)), else_result.parser};
 }() : predicates::TKind_is_ident(k0) && predicates::is_ctor_name(predicates::TKind_ident(k0)) && predicates::TKind_is_lparen(parser_next_kind(name_pos)) ? [&]() -> predicates::StmtResult { 
  predicates::PatResult pr = parse_pattern_identifier_branch(name_pos, predicates::TKind_ident(k0));
  predicates::TypeResult type_result = parse_after_let_pattern(pr.parser);
  predicates::ExprResult value_result = parse_expr(type_result.parser);
  bool has_else = predicates::TKind_is_else(predicates::Parser_kind(value_result.parser));
  predicates::StmtsResult else_result = has_else ? parse_stmts_until_end(predicates::Parser_advance(value_result.parser)) : predicates::StmtsResult{{}, value_result.parser};
  std::shared_ptr<ast::Expr> else_body = std::make_shared<ast::Expr>(ast::ExprBlock(else_result.stmts, std::make_shared<ast::Expr>(ast::ExprUnit(statement_span)), statement_span));
  return predicates::StmtResult{std::make_shared<ast::Stmt>(ast::StmtLetPat(pr.pat, is_mut, type_result.type_expr, value_result.expr, has_else, else_body, statement_span)), else_result.parser};
 }() : predicates::TKind_is_ident(k0) ? [&]() -> predicates::StmtResult { 
  mlc::String var_name = predicates::TKind_ident(k0);
  predicates::TypeResult type_result = predicates::TKind_is_colon(predicates::Parser_kind(predicates::Parser_advance(name_pos))) ? types::parse_type(predicates::Parser_advance_by(name_pos, 2)) : predicates::TypeResult{std::make_shared<ast::TypeExpr>((ast::TyUnit{})), predicates::Parser_advance(name_pos)};
  predicates::ExprResult value_result = parse_expr(predicates::Parser_advance(type_result.parser));
  return predicates::StmtResult{std::make_shared<ast::Stmt>(ast::StmtLet(var_name, is_mut, type_result.type_expr, value_result.expr, statement_span)), value_result.parser};
 }() : predicates::StmtResult{std::make_shared<ast::Stmt>(ast::StmtExpr(std::make_shared<ast::Expr>(ast::ExprUnit(predicates::Parser_span_at_cursor(name_pos))), statement_span)), name_pos};
 }();
}

predicates::StmtResult parse_statement_const(predicates::Parser parser) noexcept{
ast::Span statement_span = predicates::Parser_span_at_cursor(parser);
predicates::Parser name_pos = predicates::Parser_advance(parser);
mlc::String var_name = predicates::TKind_ident(predicates::Parser_kind(name_pos));
predicates::TypeResult type_result = predicates::TKind_is_colon(predicates::Parser_kind(predicates::Parser_advance(name_pos))) ? types::parse_type(predicates::Parser_advance_by(name_pos, 2)) : predicates::TypeResult{std::make_shared<ast::TypeExpr>((ast::TyUnit{})), predicates::Parser_advance(name_pos)};
predicates::ExprResult value_result = parse_expr(predicates::Parser_advance(type_result.parser));
return predicates::StmtResult{std::make_shared<ast::Stmt>(ast::StmtLet(var_name, false, type_result.type_expr, value_result.expr, statement_span)), value_result.parser};
}

predicates::StmtResult parse_statement_return(predicates::Parser parser) noexcept{
ast::Span statement_span = predicates::Parser_span_at_cursor(parser);
predicates::ExprResult value_result = parse_expr(predicates::Parser_advance(parser));
return predicates::StmtResult{std::make_shared<ast::Stmt>(ast::StmtReturn(value_result.expr, statement_span)), value_result.parser};
}

predicates::StmtResult parse_statement_expression_or_assign(predicates::Parser parser) noexcept{
ast::Span statement_span = predicates::Parser_span_at_cursor(parser);
predicates::ExprResult expr_result = parse_expr(parser);
return predicates::TKind_is_equal(predicates::Parser_kind(expr_result.parser)) ? [&]() -> predicates::StmtResult { 
  ast::Span assign_span = predicates::Parser_span_at_cursor(expr_result.parser);
  predicates::ExprResult right_hand = parse_expr(predicates::Parser_advance(expr_result.parser));
  return predicates::StmtResult{std::make_shared<ast::Stmt>(ast::StmtExpr(std::make_shared<ast::Expr>(ast::ExprBin(mlc::String("="), expr_result.expr, right_hand.expr, assign_span)), statement_span)), right_hand.parser};
 }() : predicates::StmtResult{std::make_shared<ast::Stmt>(ast::StmtExpr(expr_result.expr, statement_span)), expr_result.parser};
}

predicates::StmtResult parse_stmt(predicates::Parser parser) noexcept{
ast_tokens::TKind kind = predicates::Parser_kind(parser);
return predicates::TKind_is_let(kind) ? parse_statement_let(parser) : predicates::TKind_is_const(kind) ? parse_statement_const(parser) : predicates::TKind_is_break(kind) ? predicates::StmtResult{std::make_shared<ast::Stmt>(ast::StmtBreak(predicates::Parser_span_at_cursor(parser))), predicates::Parser_advance(parser)} : predicates::TKind_is_continue(kind) ? predicates::StmtResult{std::make_shared<ast::Stmt>(ast::StmtContinue(predicates::Parser_span_at_cursor(parser))), predicates::Parser_advance(parser)} : predicates::TKind_is_return(kind) ? parse_statement_return(parser) : predicates::TKind_is_while(kind) ? [&]() -> predicates::StmtResult { 
  ast::Span header_span = predicates::Parser_span_at_cursor(parser);
  predicates::ExprResult while_result = parse_while_expr(predicates::Parser_advance(parser), header_span);
  return predicates::StmtResult{std::make_shared<ast::Stmt>(ast::StmtExpr(while_result.expr, header_span)), while_result.parser};
 }() : predicates::TKind_is_for(kind) ? [&]() -> predicates::StmtResult { 
  ast::Span header_span = predicates::Parser_span_at_cursor(parser);
  predicates::ExprResult for_result = parse_for_expr(predicates::Parser_advance(parser), header_span);
  return predicates::StmtResult{std::make_shared<ast::Stmt>(ast::StmtExpr(for_result.expr, header_span)), for_result.parser};
 }() : predicates::TKind_is_do(kind) ? [&]() -> predicates::StmtResult { 
  ast::Span header_span = predicates::Parser_span_at_cursor(parser);
  predicates::ExprResult block_result = parse_block(predicates::Parser_advance(parser), header_span);
  return predicates::StmtResult{std::make_shared<ast::Stmt>(ast::StmtExpr(block_result.expr, header_span)), block_result.parser};
 }() : predicates::TKind_is_with(kind) ? [&]() -> predicates::StmtResult { 
  ast::Span header_span = predicates::Parser_span_at_cursor(parser);
  predicates::ExprResult with_result = parse_with_expr(predicates::Parser_advance(parser), header_span);
  return predicates::StmtResult{std::make_shared<ast::Stmt>(ast::StmtExpr(with_result.expr, header_span)), with_result.parser};
 }() : parse_statement_expression_or_assign(parser);
}

predicates::PatResult parse_pattern_identifier_branch(predicates::Parser parser, mlc::String name) noexcept{
ast::Span pattern_span = predicates::Parser_span_at_cursor(parser);
return name == mlc::String("_") ? predicates::PatResult{std::make_shared<ast::Pat>(ast::PatWild(pattern_span)), predicates::Parser_advance(parser)} : predicates::is_ctor_name(name) ? [&]() -> predicates::PatResult { 
  predicates::Parser after = predicates::Parser_advance(parser);
  return predicates::TKind_is_lparen(predicates::Parser_kind(after)) ? [&]() -> predicates::PatResult { 
  predicates::PatsResult sub_patterns = parse_pat_args(predicates::Parser_advance(after));
  return predicates::PatResult{std::make_shared<ast::Pat>(ast::PatCtor(name, sub_patterns.pats, pattern_span)), sub_patterns.parser};
 }() : predicates::TKind_is_lbrace(predicates::Parser_kind(after)) ? [&]() -> predicates::PatResult { 
  predicates::PatsResult field_result = parse_record_pat_fields(predicates::Parser_advance(after));
  return predicates::PatResult{std::make_shared<ast::Pat>(ast::PatRecord(name, field_result.pats, pattern_span)), field_result.parser};
 }() : predicates::PatResult{std::make_shared<ast::Pat>(ast::PatCtor(name, {}, pattern_span)), after};
 }() : predicates::PatResult{std::make_shared<ast::Pat>(ast::PatIdent(name, pattern_span)), predicates::Parser_advance(parser)};
}

predicates::PatResult parse_pattern_integer(predicates::Parser parser, int value) noexcept{return predicates::PatResult{std::make_shared<ast::Pat>(ast::PatInt(value, predicates::Parser_span_at_cursor(parser))), predicates::Parser_advance(parser)};}

predicates::PatResult parse_pattern_string(predicates::Parser parser, mlc::String value) noexcept{return predicates::PatResult{std::make_shared<ast::Pat>(ast::PatStr(value, predicates::Parser_span_at_cursor(parser))), predicates::Parser_advance(parser)};}

predicates::PatResult parse_pattern_boolean(predicates::Parser parser, bool value) noexcept{return predicates::PatResult{std::make_shared<ast::Pat>(ast::PatBool(value, predicates::Parser_span_at_cursor(parser))), predicates::Parser_advance(parser)};}

predicates::PatResult parse_pattern_fallback_wildcard(predicates::Parser parser) noexcept{return predicates::PatResult{std::make_shared<ast::Pat>(ast::PatWild(predicates::Parser_span_at_cursor(parser))), predicates::Parser_advance(parser)};}

predicates::PatResult parse_or_pat(predicates::Parser parser) noexcept{
predicates::PatResult first = parse_pat(parser);
ast::Span span = predicates::Parser_span_at_cursor(parser);
mlc::Array<std::shared_ptr<ast::Pat>> alts = mlc::Array<std::shared_ptr<ast::Pat>>{first.pat};
predicates::Parser state = first.parser;
while (predicates::TKind_is_bar(predicates::Parser_kind(state)) && !predicates::Parser_at_eof(predicates::Parser_advance(state)) && !predicates::TKind_is_fat_arrow(predicates::Parser_kind(predicates::Parser_advance(state)))){
{
predicates::PatResult next = parse_pat(predicates::Parser_advance(state));
alts.push_back(next.pat);
state = next.parser;
}
}
return alts.size() == 1 ? first : predicates::PatResult{std::make_shared<ast::Pat>(ast::PatOr(alts, span)), state};
}

predicates::PatResult parse_pat(predicates::Parser parser) noexcept{
ast_tokens::TKind kind = predicates::Parser_kind(parser);
return predicates::TKind_is_ident(kind) ? parse_pattern_identifier_branch(parser, predicates::TKind_ident(kind)) : predicates::TKind_is_int(kind) ? parse_pattern_integer(parser, predicates::TKind_int_val(kind)) : predicates::TKind_is_str(kind) ? parse_pattern_string(parser, predicates::TKind_str_val(kind)) : predicates::TKind_is_true(kind) ? parse_pattern_boolean(parser, true) : predicates::TKind_is_false(kind) ? parse_pattern_boolean(parser, false) : predicates::TKind_is_else(kind) ? predicates::PatResult{std::make_shared<ast::Pat>(ast::PatWild(predicates::Parser_span_at_cursor(parser))), predicates::Parser_advance(parser)} : parse_pattern_fallback_wildcard(parser);
}

predicates::PatsResult comma_separated_pat_suffix_from_first(predicates::PatResult first) noexcept{
mlc::Array<std::shared_ptr<ast::Pat>> pats = {};
pats.push_back(first.pat);
predicates::Parser state = first.parser;
while (predicates::TKind_is_comma(predicates::Parser_kind(state))){
{
predicates::PatResult next = parse_pat(predicates::Parser_advance(state));
pats.push_back(next.pat);
state = next.parser;
}
}
return predicates::PatsResult{pats, state};
}

predicates::PatsResult parse_pat_args(predicates::Parser parser) noexcept{return predicates::TKind_is_rparen(predicates::Parser_kind(parser)) ? predicates::PatsResult{{}, predicates::Parser_advance(parser)} : [&]() -> predicates::PatsResult { 
  predicates::PatsResult suffix = comma_separated_pat_suffix_from_first(parse_pat(parser));
  return predicates::PatsResult{suffix.pats, predicates::Parser_advance(suffix.parser)};
 }();}

predicates::PatsResult parse_record_pat_fields(predicates::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::Pat>> pats = {};
predicates::Parser state = std::move(parser);
while (!predicates::TKind_is_rbrace(predicates::Parser_kind(state)) && !predicates::Parser_at_eof(state)){
{
if (predicates::TKind_is_ident(predicates::Parser_kind(state))){
{
pats.push_back(std::make_shared<ast::Pat>(ast::PatIdent(predicates::TKind_ident(predicates::Parser_kind(state)), predicates::Parser_span_at_cursor(state))));
}
}
state = predicates::Parser_advance(state);
if (predicates::TKind_is_comma(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
}
}
}
}
return predicates::PatsResult{pats, predicates::Parser_advance(state)};
}

predicates::ExprResult parse_expr(predicates::Parser parser) noexcept{return parse_or(parser);}

std::shared_ptr<ast::Expr> pipe_desugar(std::shared_ptr<ast::Expr> left_expr, std::shared_ptr<ast::Expr> right_expr) noexcept{return [&]() -> std::shared_ptr<ast::Expr> { if (std::holds_alternative<ast::ExprCall>((*right_expr)._)) { auto _v_exprcall = std::get<ast::ExprCall>((*right_expr)._); auto [callee, existing_args, _w0] = _v_exprcall; return std::make_shared<ast::Expr>(ast::ExprCall(callee, mlc::Array<std::shared_ptr<ast::Expr>>{left_expr}.concat(existing_args), ast::span_unknown())); } return [&]() -> std::shared_ptr<ast::Expr> { 
  mlc::Array<std::shared_ptr<ast::Expr>> call_args = mlc::Array<std::shared_ptr<ast::Expr>>{left_expr};
  return std::make_shared<ast::Expr>(ast::ExprCall(right_expr, call_args, ast::span_unknown()));
 }(); }();}

predicates::ExprResult parse_pipe(predicates::Parser parser) noexcept{
predicates::ExprResult left = parse_comparison_relational(parser);
std::shared_ptr<ast::Expr> expr = left.expr;
predicates::Parser state = left.parser;
while (predicates::TKind_is_pipe(predicates::Parser_kind(state))){
{
predicates::ExprResult right = parse_comparison_relational(predicates::Parser_advance(state));
expr = pipe_desugar(expr, right.expr);
state = right.parser;
}
}
return predicates::ExprResult{expr, state};
}

predicates::ExprsResult comma_separated_expr_suffix_from_first(predicates::ExprResult first) noexcept{
mlc::Array<std::shared_ptr<ast::Expr>> exprs = {};
exprs.push_back(first.expr);
predicates::Parser state = first.parser;
while (predicates::TKind_is_comma(predicates::Parser_kind(state))){
{
predicates::ExprResult next = parse_expr(predicates::Parser_advance(state));
exprs.push_back(next.expr);
state = next.parser;
}
}
return predicates::ExprsResult{exprs, state};
}

predicates::ExprResult parse_or(predicates::Parser parser) noexcept{
predicates::ExprResult left = parse_and(parser);
std::shared_ptr<ast::Expr> expr = left.expr;
predicates::Parser state = left.parser;
while (predicates::TKind_is_op(predicates::Parser_kind(state)) && predicates::TKind_op_val(predicates::Parser_kind(state)) == mlc::String("||")){
{
ast::Span operator_span = predicates::Parser_span_at_cursor(state);
predicates::ExprResult right = parse_and(predicates::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprBin(mlc::String("||"), expr, right.expr, operator_span));
state = right.parser;
}
}
return predicates::ExprResult{expr, state};
}

predicates::ExprResult parse_and(predicates::Parser parser) noexcept{
predicates::ExprResult left = parse_equality(parser);
std::shared_ptr<ast::Expr> expr = left.expr;
predicates::Parser state = left.parser;
while (predicates::TKind_is_op(predicates::Parser_kind(state)) && predicates::TKind_op_val(predicates::Parser_kind(state)) == mlc::String("&&")){
{
ast::Span operator_span = predicates::Parser_span_at_cursor(state);
predicates::ExprResult right = parse_equality(predicates::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprBin(mlc::String("&&"), expr, right.expr, operator_span));
state = right.parser;
}
}
return predicates::ExprResult{expr, state};
}

predicates::ExprResult parse_equality(predicates::Parser parser) noexcept{
predicates::ExprResult left = parse_pipe(parser);
std::shared_ptr<ast::Expr> expr = left.expr;
predicates::Parser state = left.parser;
bool go = true;
while (go){
{
ast_tokens::TKind kind = predicates::Parser_kind(state);
if (predicates::TKind_is_op(kind) && predicates::TKind_op_val(kind) == mlc::String("==") || predicates::TKind_op_val(kind) == mlc::String("!=")){
{
ast::Span operator_span = predicates::Parser_span_at_cursor(state);
mlc::String operator_value = predicates::TKind_op_val(kind);
predicates::ExprResult right = parse_pipe(predicates::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprBin(operator_value, expr, right.expr, operator_span));
state = right.parser;
}
} else {
{
go = false;
}
}
}
}
return predicates::ExprResult{expr, state};
}

bool is_relational_comparison_operator(mlc::String operator_) noexcept{return operator_ == mlc::String("<") || operator_ == mlc::String(">") || operator_ == mlc::String("<=") || operator_ == mlc::String(">=");}

predicates::ExprResult parse_comparison_relational(predicates::Parser parser) noexcept{
predicates::ExprResult left = parse_add(parser);
std::shared_ptr<ast::Expr> expr = left.expr;
predicates::Parser state = left.parser;
bool go = true;
while (go){
{
ast_tokens::TKind kind = predicates::Parser_kind(state);
if (predicates::TKind_is_op(kind) && is_relational_comparison_operator(predicates::TKind_op_val(kind))){
{
ast::Span operator_span = predicates::Parser_span_at_cursor(state);
mlc::String operator_value = predicates::TKind_op_val(kind);
predicates::ExprResult right = parse_add(predicates::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprBin(operator_value, expr, right.expr, operator_span));
state = right.parser;
}
} else {
{
go = false;
}
}
}
}
return predicates::ExprResult{expr, state};
}

predicates::ExprResult parse_add(predicates::Parser parser) noexcept{
predicates::ExprResult left = parse_mul(parser);
std::shared_ptr<ast::Expr> expr = left.expr;
predicates::Parser state = left.parser;
bool go = true;
while (go){
{
ast_tokens::TKind kind = predicates::Parser_kind(state);
if (predicates::TKind_is_op(kind) && predicates::TKind_op_val(kind) == mlc::String("+") || predicates::TKind_op_val(kind) == mlc::String("-")){
{
ast::Span operator_span = predicates::Parser_span_at_cursor(state);
predicates::ExprResult right = parse_mul(predicates::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprBin(predicates::TKind_op_val(kind), expr, right.expr, operator_span));
state = right.parser;
}
} else {
{
go = false;
}
}
}
}
return predicates::ExprResult{expr, state};
}

predicates::ExprResult parse_mul(predicates::Parser parser) noexcept{
predicates::ExprResult left = parse_unary(parser);
std::shared_ptr<ast::Expr> expr = left.expr;
predicates::Parser state = left.parser;
bool go = true;
while (go){
{
ast_tokens::TKind kind = predicates::Parser_kind(state);
if (predicates::TKind_is_op(kind) && predicates::TKind_op_val(kind) == mlc::String("*") || predicates::TKind_op_val(kind) == mlc::String("/") || predicates::TKind_op_val(kind) == mlc::String("%")){
{
ast::Span operator_span = predicates::Parser_span_at_cursor(state);
predicates::ExprResult right = parse_unary(predicates::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprBin(predicates::TKind_op_val(kind), expr, right.expr, operator_span));
state = right.parser;
}
} else {
{
go = false;
}
}
}
}
return predicates::ExprResult{expr, state};
}

predicates::ExprResult parse_unary(predicates::Parser parser) noexcept{
ast_tokens::TKind kind = predicates::Parser_kind(parser);
return predicates::TKind_is_op(kind) && predicates::TKind_op_val(kind) == mlc::String("!") || predicates::TKind_op_val(kind) == mlc::String("-") ? [&]() -> predicates::ExprResult { 
  ast::Span operator_span = predicates::Parser_span_at_cursor(parser);
  predicates::ExprResult inner = parse_unary(predicates::Parser_advance(parser));
  return predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprUn(predicates::TKind_op_val(kind), inner.expr, operator_span)), inner.parser};
 }() : parse_postfix(parser);
}

predicates::ExprResult parse_postfix(predicates::Parser parser) noexcept{
predicates::ExprResult base = parse_primary(parser);
std::shared_ptr<ast::Expr> expr = base.expr;
predicates::Parser state = base.parser;
bool go = true;
while (go){
{
ast_tokens::TKind kind = predicates::Parser_kind(state);
if (predicates::TKind_is_dot(kind)){
{
ast::Span dot_span = predicates::Parser_span_at_cursor(state);
mlc::String field_name = predicates::TKind_ident(predicates::Parser_kind(predicates::Parser_advance(state)));
if (predicates::TKind_is_lparen(predicates::Parser_kind(predicates::Parser_advance_by(state, 2))) && predicates::Parser_span_at_cursor(predicates::Parser_advance_by(state, 2)).line == predicates::Parser_span_at_cursor(predicates::Parser_advance_by(state, 1)).line){
predicates::ExprsResult margs = parse_arg_list(predicates::Parser_advance_by(state, 3));
expr = std::make_shared<ast::Expr>(ast::ExprMethod(expr, field_name, margs.exprs, dot_span));
state = margs.parser;
} else {
expr = std::make_shared<ast::Expr>(ast::ExprField(expr, field_name, dot_span));
state = predicates::Parser_advance_by(state, 2);
}
}
} else {
{
if (predicates::TKind_is_lparen(kind) && predicates::Parser_span_at_cursor(state).line == predicates::Parser_prev_line(state)){
ast::Span call_span = predicates::Parser_span_at_cursor(state);
predicates::ExprsResult call_args = parse_arg_list(predicates::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprCall(expr, call_args.exprs, call_span));
state = call_args.parser;
} else {
if (predicates::TKind_is_lbracket(kind)){
ast::Span bracket_span = predicates::Parser_span_at_cursor(state);
predicates::ExprResult index = parse_expr(predicates::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprIndex(expr, index.expr, bracket_span));
state = predicates::Parser_advance(index.parser);
} else {
if (predicates::TKind_is_question(kind)){
ast::Span question_span = predicates::Parser_span_at_cursor(state);
expr = std::make_shared<ast::Expr>(ast::ExprQuestion(expr, question_span));
state = predicates::Parser_advance(state);
} else {
go = false;
}
}
}
}
}
}
}
return predicates::ExprResult{expr, state};
}

predicates::ExprsResult parse_arg_list(predicates::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::Expr>> exprs = {};
return predicates::TKind_is_rparen(predicates::Parser_kind(parser)) ? predicates::ExprsResult{exprs, predicates::Parser_advance(parser)} : [&]() -> predicates::ExprsResult { 
  predicates::Parser state = std::move(parser);
  while (!predicates::TKind_is_rparen(predicates::Parser_kind(state)) && !predicates::Parser_at_eof(state)){
{
ast::Span arg_span = predicates::Parser_span_at_cursor(state);
std::shared_ptr<ast::Expr> arg = predicates::TKind_is_ident(predicates::Parser_kind(state)) && predicates::TKind_is_colon(parser_next_kind(state)) ? [&]() -> std::shared_ptr<ast::Expr> { 
  mlc::String name = predicates::TKind_ident(predicates::Parser_kind(state));
  predicates::ExprResult value = parse_expr(predicates::Parser_advance_by(state, 2));
  state = value.parser;
  return std::make_shared<ast::Expr>(ast::ExprNamedArg(name, value.expr, arg_span));
 }() : [&]() -> std::shared_ptr<ast::Expr> { 
  predicates::ExprResult r = parse_expr(state);
  state = r.parser;
  return r.expr;
 }();
exprs.push_back(arg);
if (predicates::TKind_is_comma(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
}
}
}
}
  return predicates::ExprsResult{exprs, predicates::Parser_advance(state)};
 }();
}

std::shared_ptr<ast::Expr> build_template_expr(mlc::Array<mlc::String> parts) noexcept{
std::shared_ptr<ast::Expr> result = std::make_shared<ast::Expr>(ast::ExprStr(mlc::String(""), ast::span_unknown()));
int pi = 0;
while (pi < parts.size()){
{
std::shared_ptr<ast::Expr> part_expr = pi % 2 == 0 ? std::make_shared<ast::Expr>(ast::ExprStr(parts[pi], ast::span_unknown())) : parts[pi].length() == 0 ? std::make_shared<ast::Expr>(ast::ExprStr(mlc::String(""), ast::span_unknown())) : [&]() -> std::shared_ptr<ast::Expr> { 
  ast_tokens::LexOut sub_lex = lexer::tokenize(parts[pi]);
  predicates::ExprResult sub_parsed = parse_expr(predicates::parser_new(sub_lex.tokens));
  return std::make_shared<ast::Expr>(ast::ExprMethod(sub_parsed.expr, mlc::String("to_string"), {}, ast::span_unknown()));
 }();
if (pi == 0){
{
result = part_expr;
}
} else {
{
result = std::make_shared<ast::Expr>(ast::ExprBin(mlc::String("+"), result, part_expr, ast::span_unknown()));
}
}
pi = pi + 1;
}
}
return result;
}

predicates::ExprResult parse_primary_integer_literal(predicates::Parser parser, int value) noexcept{
ast::Span source_span = predicates::Parser_span_at_cursor(parser);
return predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprInt(value, source_span)), predicates::Parser_advance(parser)};
}

predicates::ExprResult parse_primary_string_literal(predicates::Parser parser, mlc::String value) noexcept{
ast::Span source_span = predicates::Parser_span_at_cursor(parser);
return predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprStr(value, source_span)), predicates::Parser_advance(parser)};
}

predicates::ExprResult parse_primary_template_literal(predicates::Parser parser, mlc::Array<mlc::String> parts) noexcept{return predicates::ExprResult{build_template_expr(parts), predicates::Parser_advance(parser)};}

predicates::ExprResult parse_primary_boolean_literal(predicates::Parser parser, bool value) noexcept{
ast::Span source_span = predicates::Parser_span_at_cursor(parser);
return predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprBool(value, source_span)), predicates::Parser_advance(parser)};
}

predicates::ExprResult parse_primary_parenthesized(predicates::Parser parser) noexcept{
ast::Span open_paren_span = predicates::Parser_span_at_cursor(parser);
return predicates::TKind_is_rparen(predicates::Parser_kind(predicates::Parser_advance(parser))) ? [&]() -> predicates::ExprResult { 
  predicates::Parser after_close = predicates::Parser_advance_by(parser, 2);
  return predicates::TKind_is_fat_arrow(predicates::Parser_kind(after_close)) && !predicates::Parser_lambda_shorthand_suppression_active(parser) ? [&]() -> predicates::ExprResult { 
  mlc::Array<mlc::String> params = {};
  predicates::ExprResult body = parse_expr(predicates::Parser_advance_by(parser, 3));
  return predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprLambda(params, body.expr, open_paren_span)), body.parser};
 }() : predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprUnit(open_paren_span)), after_close};
 }() : !predicates::Parser_lambda_shorthand_suppression_active(parser) && looks_like_typed_lambda_params(predicates::Parser_advance(parser)) ? [&]() -> predicates::ExprResult { 
  predicates::NamesResult param_result = parse_typed_lambda_params(predicates::Parser_advance(parser));
  predicates::ExprResult body = parse_expr(predicates::Parser_advance(param_result.parser));
  return predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprLambda(param_result.exprs, body.expr, open_paren_span)), body.parser};
 }() : !predicates::Parser_lambda_shorthand_suppression_active(parser) && looks_like_lambda_params(predicates::Parser_advance(parser)) ? [&]() -> predicates::ExprResult { 
  predicates::NamesResult param_result = parse_lambda_params(predicates::Parser_advance(parser));
  predicates::ExprResult body = parse_expr(predicates::Parser_advance(param_result.parser));
  return predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprLambda(param_result.exprs, body.expr, open_paren_span)), body.parser};
 }() : [&]() -> predicates::ExprResult { 
  predicates::ExprResult e0 = parse_expr(predicates::Parser_advance(parser));
  return predicates::TKind_is_comma(predicates::Parser_kind(e0.parser)) ? [&]() -> predicates::ExprResult { 
  predicates::ExprsResult suffix = comma_separated_expr_suffix_from_first(e0);
  return predicates::TKind_is_rparen(predicates::Parser_kind(suffix.parser)) ? predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprTuple(suffix.exprs, open_paren_span)), predicates::Parser_advance(suffix.parser)} : predicates::ExprResult{e0.expr, e0.parser};
 }() : predicates::ExprResult{e0.expr, predicates::Parser_advance(e0.parser)};
 }();
}

predicates::ExprResult parse_primary_if_or_unless(predicates::Parser parser) noexcept{return parse_if_expr(parser);}

predicates::ExprResult parse_primary_do_block(predicates::Parser parser) noexcept{
ast::Span header_span = predicates::Parser_span_at_cursor(parser);
return parse_block(predicates::Parser_advance(parser), header_span);
}

predicates::ExprResult parse_primary_while_loop(predicates::Parser parser) noexcept{
ast::Span header_span = predicates::Parser_span_at_cursor(parser);
return parse_while_expr(predicates::Parser_advance(parser), header_span);
}

predicates::ExprResult parse_primary_for_loop(predicates::Parser parser) noexcept{
ast::Span header_span = predicates::Parser_span_at_cursor(parser);
return parse_for_expr(predicates::Parser_advance(parser), header_span);
}

predicates::ExprResult parse_primary_match(predicates::Parser parser) noexcept{
ast::Span header_span = predicates::Parser_span_at_cursor(parser);
return parse_match_expr(predicates::Parser_advance(parser), header_span);
}

predicates::ExprResult parse_primary_return_as_block(predicates::Parser parser) noexcept{
ast::Span header_span = predicates::Parser_span_at_cursor(parser);
predicates::ExprResult value = parse_expr(predicates::Parser_advance(parser));
mlc::Array<std::shared_ptr<ast::Stmt>> statements = {};
statements.push_back(std::make_shared<ast::Stmt>(ast::StmtReturn(value.expr, header_span)));
return predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprBlock(statements, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())), header_span)), value.parser};
}

predicates::ExprResult parse_primary_identifier(predicates::Parser parser, mlc::String name) noexcept{
ast::Span source_span = predicates::Parser_span_at_cursor(parser);
predicates::Parser after_name = predicates::Parser_advance(parser);
return predicates::TKind_is_fat_arrow(predicates::Parser_kind(after_name)) && !predicates::Parser_lambda_shorthand_suppression_active(parser) ? [&]() -> predicates::ExprResult { 
  mlc::Array<mlc::String> parameters = mlc::Array<mlc::String>{name};
  predicates::ExprResult body = parse_expr(predicates::Parser_advance(after_name));
  return predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprLambda(parameters, body.expr, source_span)), body.parser};
 }() : predicates::is_ctor_name(name) && predicates::TKind_is_lbrace(predicates::Parser_kind(after_name)) ? [&]() -> predicates::ExprResult { 
  ast::Span brace_span = predicates::Parser_span_at_cursor(after_name);
  return parse_record_expr(predicates::Parser_advance(after_name), name, brace_span);
 }() : predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprIdent(name, source_span)), after_name};
}

predicates::ExprResult parse_primary_unit_fallback(predicates::Parser parser) noexcept{
ast::Span source_span = predicates::Parser_span_at_cursor(parser);
return predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprUnit(source_span)), predicates::Parser_advance(parser)};
}

predicates::ExprResult parse_primary(predicates::Parser parser) noexcept{
ast_tokens::TKind kind = predicates::Parser_kind(parser);
return predicates::TKind_is_float(kind) ? [&]() -> predicates::ExprResult { 
  ast::Span source_span = predicates::Parser_span_at_cursor(parser);
  return predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprFloat(predicates::TKind_float_val(kind), source_span)), predicates::Parser_advance(parser)};
 }() : predicates::TKind_is_i64(kind) ? [&]() -> predicates::ExprResult { 
  ast::Span source_span = predicates::Parser_span_at_cursor(parser);
  return predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprI64(predicates::TKind_i64_val(kind), source_span)), predicates::Parser_advance(parser)};
 }() : predicates::TKind_is_u8(kind) ? [&]() -> predicates::ExprResult { 
  ast::Span source_span = predicates::Parser_span_at_cursor(parser);
  return predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprU8(predicates::TKind_u8_val(kind), source_span)), predicates::Parser_advance(parser)};
 }() : predicates::TKind_is_usize(kind) ? [&]() -> predicates::ExprResult { 
  ast::Span source_span = predicates::Parser_span_at_cursor(parser);
  return predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprUsize(predicates::TKind_usize_val(kind), source_span)), predicates::Parser_advance(parser)};
 }() : predicates::TKind_is_char_lit(kind) ? [&]() -> predicates::ExprResult { 
  ast::Span source_span = predicates::Parser_span_at_cursor(parser);
  return predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprChar(predicates::TKind_char_val(kind), source_span)), predicates::Parser_advance(parser)};
 }() : predicates::TKind_is_int(kind) ? parse_primary_integer_literal(parser, predicates::TKind_int_val(kind)) : predicates::TKind_is_str(kind) ? parse_primary_string_literal(parser, predicates::TKind_str_val(kind)) : predicates::TKind_is_template(kind) ? parse_primary_template_literal(parser, predicates::TKind_template_parts(kind)) : predicates::TKind_is_true(kind) ? parse_primary_boolean_literal(parser, true) : predicates::TKind_is_false(kind) ? parse_primary_boolean_literal(parser, false) : predicates::TKind_is_lparen(kind) ? parse_primary_parenthesized(parser) : predicates::TKind_is_lbracket(kind) ? [&]() -> predicates::ExprResult { 
  ast::Span bracket_span = predicates::Parser_span_at_cursor(parser);
  return parse_array_lit(predicates::Parser_advance(parser), bracket_span);
 }() : predicates::TKind_is_if(kind) || predicates::TKind_is_unless(kind) ? parse_primary_if_or_unless(parser) : predicates::TKind_is_do(kind) ? parse_primary_do_block(parser) : predicates::TKind_is_while(kind) ? parse_primary_while_loop(parser) : predicates::TKind_is_for(kind) ? parse_primary_for_loop(parser) : predicates::TKind_is_with(kind) ? [&]() -> predicates::ExprResult { 
  ast::Span header_span = predicates::Parser_span_at_cursor(parser);
  return parse_with_expr(predicates::Parser_advance(parser), header_span);
 }() : predicates::TKind_is_match(kind) ? parse_primary_match(parser) : predicates::TKind_is_return(kind) ? parse_primary_return_as_block(parser) : predicates::TKind_is_ident(kind) ? parse_primary_identifier(parser, predicates::TKind_ident(kind)) : parse_primary_unit_fallback(parser);
}

bool looks_like_typed_lambda_params(predicates::Parser parser) noexcept{return predicates::TKind_is_ident(predicates::Parser_kind(parser)) && predicates::TKind_is_colon(predicates::Parser_kind(predicates::Parser_advance(parser)));}

predicates::NamesResult parse_typed_lambda_params(predicates::Parser parser) noexcept{
mlc::Array<mlc::String> names = {};
predicates::Parser state = std::move(parser);
names.push_back(predicates::TKind_ident(predicates::Parser_kind(state)));
state = types::parse_type(predicates::Parser_advance_by(state, 2)).parser;
while (predicates::TKind_is_comma(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
names.push_back(predicates::TKind_ident(predicates::Parser_kind(state)));
state = types::parse_type(predicates::Parser_advance_by(state, 2)).parser;
}
}
return predicates::NamesResult{names, predicates::Parser_advance(state)};
}

bool looks_like_lambda_params(predicates::Parser parser) noexcept{
predicates::Parser state = std::move(parser);
bool ok = true;
if (!predicates::TKind_is_ident(predicates::Parser_kind(state))){
{
ok = false;
}
} else {
{
state = predicates::Parser_advance(state);
while (predicates::TKind_is_comma(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
if (!predicates::TKind_is_ident(predicates::Parser_kind(state))){
{
ok = false;
state = predicates::Parser_advance(state);
}
} else {
{
state = predicates::Parser_advance(state);
}
}
}
}
if (!predicates::TKind_is_rparen(predicates::Parser_kind(state))){
ok = false;
} else {
ok = predicates::TKind_is_fat_arrow(predicates::Parser_kind(predicates::Parser_advance(state)));
}
}
}
return ok;
}

predicates::NamesResult parse_lambda_params(predicates::Parser parser) noexcept{
mlc::Array<mlc::String> names = {};
predicates::Parser state = std::move(parser);
names.push_back(predicates::TKind_ident(predicates::Parser_kind(state)));
state = predicates::Parser_advance(state);
while (predicates::TKind_is_comma(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
names.push_back(predicates::TKind_ident(predicates::Parser_kind(state)));
state = predicates::Parser_advance(state);
}
}
return predicates::NamesResult{names, predicates::Parser_advance(state)};
}

predicates::ExprResult parse_array_lit(predicates::Parser parser, ast::Span header_span) noexcept{return predicates::TKind_is_rbracket(predicates::Parser_kind(parser)) ? predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprArray({}, header_span)), predicates::Parser_advance(parser)} : [&]() -> predicates::ExprResult { 
  predicates::ExprsResult suffix = comma_separated_expr_suffix_from_first(parse_expr(parser));
  return predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprArray(suffix.exprs, header_span)), predicates::Parser_advance(suffix.parser)};
 }();}

predicates::ExprResult parse_if_expr(predicates::Parser parser) noexcept{
ast::Span header_span = predicates::Parser_span_at_cursor(parser);
bool is_negated = predicates::TKind_is_unless(predicates::Parser_kind(parser));
predicates::ExprResult cond_result = parse_expr(predicates::Parser_advance(parser));
std::shared_ptr<ast::Expr> condition = is_negated ? std::make_shared<ast::Expr>(ast::ExprUn(mlc::String("!"), cond_result.expr, ast::span_unknown())) : cond_result.expr;
predicates::StmtsResult then_result = parse_stmts_until_else_end(predicates::Parser_advance(cond_result.parser));
std::shared_ptr<ast::Expr> then_expr = StmtsResult_to_block_expr(then_result, header_span);
return predicates::TKind_is_else(predicates::Parser_kind(then_result.parser)) ? [&]() -> predicates::ExprResult { 
  predicates::Parser after_else = predicates::Parser_advance(then_result.parser);
  return predicates::TKind_is_if(predicates::Parser_kind(after_else)) || predicates::TKind_is_unless(predicates::Parser_kind(after_else)) ? [&]() -> predicates::ExprResult { 
  predicates::ExprResult else_result = parse_if_expr(after_else);
  return predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprIf(condition, then_expr, else_result.expr, header_span)), else_result.parser};
 }() : predicates::TKind_is_do(predicates::Parser_kind(after_else)) ? [&]() -> predicates::ExprResult { 
  predicates::ExprResult else_block = parse_block(predicates::Parser_advance(after_else), header_span);
  predicates::Parser after_block = predicates::TKind_is_end(predicates::Parser_kind(else_block.parser)) ? predicates::Parser_advance(else_block.parser) : else_block.parser;
  return predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprIf(condition, then_expr, else_block.expr, header_span)), after_block};
 }() : [&]() -> predicates::ExprResult { 
  ast::Span else_span = predicates::Parser_span_at_cursor(after_else);
  predicates::StmtsResult else_stmts = parse_stmts_until_end(after_else);
  return predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprIf(condition, then_expr, StmtsResult_to_block_expr(else_stmts, else_span), header_span)), else_stmts.parser};
 }();
 }() : predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprIf(condition, then_expr, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())), header_span)), predicates::Parser_advance(then_result.parser)};
}

predicates::ExprResult parse_block(predicates::Parser parser, ast::Span header_span) noexcept{
predicates::StmtsResult result = parse_stmts_until_end(parser);
return predicates::ExprResult{StmtsResult_to_block_expr(result, header_span), result.parser};
}

bool is_unit_expr(std::shared_ptr<ast::Expr> expr) noexcept{return [&]() { if (std::holds_alternative<ast::ExprUnit>((*expr)._)) { auto _v_exprunit = std::get<ast::ExprUnit>((*expr)._); auto [_w0] = _v_exprunit; return true; } if (std::holds_alternative<ast::ExprBlock>((*expr)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expr)._); auto [_w0, result, _w1] = _v_exprblock; return is_unit_expr(result); } if (std::holds_alternative<ast::ExprIf>((*expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expr)._); auto [_w0, _w1, else_expr, _w2] = _v_exprif; return is_unit_expr(else_expr); } if (std::holds_alternative<ast::ExprBin>((*expr)._)) { auto _v_exprbin = std::get<ast::ExprBin>((*expr)._); auto [op, _w0, _w1, _w2] = _v_exprbin; return op == mlc::String("="); } return false; }();}

std::shared_ptr<ast::Expr> block_result(mlc::Array<std::shared_ptr<ast::Stmt>> stmts) noexcept{return stmts.size() == 0 ? std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())) : [&]() -> std::shared_ptr<ast::Expr> { if (std::holds_alternative<ast::StmtExpr>((*stmts[stmts.size() - 1])._)) { auto _v_stmtexpr = std::get<ast::StmtExpr>((*stmts[stmts.size() - 1])._); auto [expr, _w0] = _v_stmtexpr; return is_unit_expr(expr) ? std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())) : expr; } return std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())); }();}

mlc::Array<std::shared_ptr<ast::Stmt>> block_body(mlc::Array<std::shared_ptr<ast::Stmt>> stmts) noexcept{
return stmts.size() == 0 ? stmts : [&]() -> mlc::Array<std::shared_ptr<ast::Stmt>> { if (std::holds_alternative<ast::StmtExpr>((*stmts[stmts.size() - 1])._)) { auto _v_stmtexpr = std::get<ast::StmtExpr>((*stmts[stmts.size() - 1])._); auto [expr, _w0] = _v_stmtexpr; return is_unit_expr(expr) ? stmts : [&]() -> mlc::Array<std::shared_ptr<ast::Stmt>> { 
  mlc::Array<std::shared_ptr<ast::Stmt>> result = {};
  int i = 0;
  while (i < stmts.size() - 1){
{
result.push_back(stmts[i]);
i = i + 1;
}
}
  return result;
 }(); } return stmts; }();
}

std::shared_ptr<ast::Expr> StmtsResult_to_block_expr(predicates::StmtsResult self, ast::Span header_span) noexcept{return std::make_shared<ast::Expr>(ast::ExprBlock(block_body(self.stmts), block_result(self.stmts), header_span));}

predicates::ExprResult parse_while_expr(predicates::Parser parser, ast::Span header_span) noexcept{
predicates::ExprResult cond = parse_expr(parser);
predicates::StmtsResult body = parse_stmts_until_end(predicates::Parser_advance(cond.parser));
return predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprWhile(cond.expr, body.stmts, header_span)), body.parser};
}

predicates::ExprResult parse_for_expr(predicates::Parser parser, ast::Span header_span) noexcept{
mlc::String var_name = predicates::TKind_ident(predicates::Parser_kind(parser));
predicates::ExprResult iter = parse_expr(predicates::Parser_advance_by(parser, 2));
predicates::StmtsResult body = parse_stmts_until_end(predicates::Parser_advance(iter.parser));
return predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprFor(var_name, iter.expr, body.stmts, header_span)), body.parser};
}

predicates::ExprResult parse_with_expr(predicates::Parser parser, ast::Span header_span) noexcept{
predicates::ExprResult resource = parse_expr(parser);
predicates::Parser after_as = predicates::Parser_advance(resource.parser);
mlc::String binder = predicates::TKind_ident(predicates::Parser_kind(after_as));
predicates::StmtsResult body = parse_stmts_until_end(predicates::Parser_advance(predicates::Parser_advance(after_as)));
return predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprWith(resource.expr, binder, body.stmts, header_span)), body.parser};
}

predicates::ExprResult parse_match_expr(predicates::Parser parser, ast::Span header_span) noexcept{
predicates::ExprResult subject = parse_expr(parser);
ast_tokens::TKind next = predicates::Parser_kind(subject.parser);
return predicates::TKind_is_lbrace(next) ? [&]() -> predicates::ExprResult { 
  predicates::ArmsResult arms = parse_arms_brace(predicates::Parser_advance(subject.parser));
  return predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprMatch(subject.expr, arms.arms, header_span)), arms.parser};
 }() : predicates::TKind_is_do(next) ? [&]() -> predicates::ExprResult { 
  predicates::ArmsResult arms = parse_arms_do_delimited(predicates::Parser_advance(subject.parser));
  return predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprMatch(subject.expr, arms.arms, header_span)), arms.parser};
 }() : [&]() -> predicates::ExprResult { 
  predicates::ArmsResult arms = parse_arms_pipe(subject.parser);
  return predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprMatch(subject.expr, arms.arms, header_span)), arms.parser};
 }();
}

exprs::MatchArmParseOutcome parse_match_arm(predicates::PatResult pat_result) noexcept{
predicates::Parser after_pattern = pat_result.parser;
bool has_guard = false;
std::shared_ptr<ast::Expr> guard_expression = std::make_shared<ast::Expr>(ast::ExprBool(true, ast::span_unknown()));
if (predicates::TKind_is_if(predicates::Parser_kind(after_pattern))){
{
predicates::ExprResult guard_expression_result = parse_expr(predicates::Parser_with_lambda_shorthand_suppressed(predicates::Parser_advance(after_pattern), true));
guard_expression = guard_expression_result.expr;
has_guard = true;
after_pattern = guard_expression_result.parser;
}
}
predicates::Parser after_fat_arrow = predicates::Parser_advance(after_pattern);
predicates::ExprResult body_expression_result = parse_expr(after_fat_arrow);
return exprs::MatchArmParseOutcome{std::make_shared<ast::MatchArm>(ast::MatchArm{pat_result.pat, has_guard, guard_expression, body_expression_result.expr}), body_expression_result.parser};
}

predicates::ArmsResult parse_arms_brace(predicates::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::MatchArm>> arms = {};
predicates::Parser state = std::move(parser);
while (!predicates::TKind_is_rbrace(predicates::Parser_kind(state)) && !predicates::Parser_at_eof(state)){
{
if (predicates::TKind_is_bar(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
}
}
predicates::PatResult pat_result = parse_or_pat(state);
exprs::MatchArmParseOutcome finished_match_arm = parse_match_arm(pat_result);
arms.push_back(finished_match_arm.arm);
state = finished_match_arm.parser;
if (predicates::TKind_is_comma(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
}
}
}
}
return predicates::ArmsResult{arms, predicates::Parser_advance(state)};
}

predicates::ArmsResult parse_arms_do_delimited(predicates::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::MatchArm>> arms = {};
predicates::Parser state = predicates::Parser_skip_semi(parser);
while (!predicates::TKind_is_end(predicates::Parser_kind(state)) && !predicates::Parser_at_eof(state)){
{
if (predicates::TKind_is_bar(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
}
}
predicates::PatResult pat_result = parse_or_pat(state);
exprs::MatchArmParseOutcome finished_match_arm = parse_match_arm(pat_result);
arms.push_back(finished_match_arm.arm);
state = predicates::Parser_skip_semi(finished_match_arm.parser);
}
}
predicates::Parser after_delimiter = predicates::TKind_is_end(predicates::Parser_kind(state)) ? predicates::Parser_advance(state) : state;
return predicates::ArmsResult{arms, after_delimiter};
}

predicates::ArmsResult parse_arms_pipe(predicates::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::MatchArm>> arms = {};
predicates::Parser state = std::move(parser);
while (predicates::TKind_is_bar(predicates::Parser_kind(state))){
{
predicates::PatResult pat_result = parse_or_pat(predicates::Parser_advance(state));
exprs::MatchArmParseOutcome finished_match_arm = parse_match_arm(pat_result);
arms.push_back(finished_match_arm.arm);
state = finished_match_arm.parser;
}
}
if (predicates::TKind_is_end(predicates::Parser_kind(state))){
{
state = predicates::Parser_advance(state);
}
}
return predicates::ArmsResult{arms, state};
}

predicates::ArmsResult parse_arms(predicates::Parser parser) noexcept{return parse_arms_brace(parser);}

predicates::RecordLitPartsResult parse_record_lit_parts(predicates::Parser parser) noexcept{
mlc::Array<ast::RecordLitPart> lit_parts = {};
mlc::Array<std::shared_ptr<ast::FieldVal>> current_field_vals = {};
predicates::Parser state = std::move(parser);
while (!predicates::TKind_is_rbrace(predicates::Parser_kind(state)) && !predicates::Parser_at_eof(state)){
{
if (predicates::TKind_is_spread(predicates::Parser_kind(state))){
{
if (current_field_vals.size() > 0){
{
lit_parts.push_back(ast::RecordLitPart(ast::RecordLitFields(current_field_vals)));
current_field_vals = [&]() -> mlc::Array<std::shared_ptr<ast::FieldVal>> { 
  mlc::Array<std::shared_ptr<ast::FieldVal>> cleared_segment = {};
  return cleared_segment;
 }();
}
}
predicates::ExprResult spread_value_result = parse_expr(predicates::Parser_advance(state));
lit_parts.push_back(ast::RecordLitPart(ast::RecordLitSpread(spread_value_result.expr)));
state = spread_value_result.parser;
if (predicates::TKind_is_comma(predicates::Parser_kind(state))){
state = predicates::Parser_advance(state);
}
}
} else {
{
ast::Span field_span = predicates::Parser_span_at_cursor(state);
mlc::String field_name = predicates::TKind_ident(predicates::Parser_kind(state));
predicates::Parser after_field_name = predicates::Parser_advance(state);
if (predicates::TKind_is_colon(predicates::Parser_kind(after_field_name))){
{
predicates::ExprResult field_value_result = parse_expr(predicates::Parser_advance(after_field_name));
current_field_vals.push_back(std::make_shared<ast::FieldVal>(ast::FieldVal{field_name, field_value_result.expr}));
state = field_value_result.parser;
}
} else {
{
current_field_vals.push_back(std::make_shared<ast::FieldVal>(ast::FieldVal{field_name, std::make_shared<ast::Expr>(ast::ExprIdent(field_name, field_span))}));
state = after_field_name;
}
}
if (predicates::TKind_is_comma(predicates::Parser_kind(state))){
state = predicates::Parser_advance(state);
}
}
}
}
}
if (current_field_vals.size() > 0){
{
lit_parts.push_back(ast::RecordLitPart(ast::RecordLitFields(current_field_vals)));
}
}
return predicates::RecordLitPartsResult{lit_parts, predicates::Parser_advance(state)};
}

predicates::ExprResult parse_record_expr(predicates::Parser parser, mlc::String record_name, ast::Span record_span) noexcept{
predicates::RecordLitPartsResult parsed_lit_parts = parse_record_lit_parts(parser);
mlc::Array<ast::RecordLitPart> lit_parts = parsed_lit_parts.lit_parts;
predicates::Parser after_record_body = parsed_lit_parts.parser;
return lit_parts.size() == 2 ? std::visit(overloaded{
  [&](const RecordLitSpread& recordlitspread) -> predicates::ExprResult { auto [base_expression] = recordlitspread; return std::visit(overloaded{
  [&](const RecordLitFields& recordlitfields) -> predicates::ExprResult { auto [override_fields] = recordlitfields; return predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprRecordUpdate(record_name, base_expression, override_fields, record_span)), after_record_body}; },
  [&](const RecordLitSpread& recordlitspread) -> predicates::ExprResult { auto [_w0] = recordlitspread; return predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprRecord(record_name, lit_parts, record_span)), after_record_body}; }
}, lit_parts[1]._); },
  [&](const RecordLitFields& recordlitfields) -> predicates::ExprResult { auto [_w0] = recordlitfields; return predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprRecord(record_name, lit_parts, record_span)), after_record_body}; }
}, lit_parts[0]._) : predicates::ExprResult{std::make_shared<ast::Expr>(ast::ExprRecord(record_name, lit_parts, record_span)), after_record_body};
}

} // namespace exprs
