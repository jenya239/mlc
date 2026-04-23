#include "exprs.hpp"

#include "preds.hpp"
#include "types.hpp"
#include "ast.hpp"

namespace exprs {

using namespace preds;
using namespace types;
using namespace ast;
using namespace ast_tokens;

preds::StmtsResult parse_stmts_until_end(preds::Parser parser) noexcept;

preds::StmtsResult parse_stmts_until_else_end(preds::Parser parser) noexcept;

preds::StmtResult parse_statement_let_const(preds::Parser parser, ast::Span statement_span) noexcept;

preds::StmtResult parse_statement_let(preds::Parser parser) noexcept;

preds::StmtResult parse_statement_const(preds::Parser parser) noexcept;

preds::StmtResult parse_statement_return(preds::Parser parser) noexcept;

preds::StmtResult parse_statement_expression_or_assign(preds::Parser parser) noexcept;

preds::StmtResult parse_stmt(preds::Parser parser) noexcept;

preds::PatResult parse_pattern_identifier_branch(preds::Parser parser, mlc::String name) noexcept;

preds::PatResult parse_pattern_integer(preds::Parser parser, int value) noexcept;

preds::PatResult parse_pattern_string(preds::Parser parser, mlc::String value) noexcept;

preds::PatResult parse_pattern_boolean(preds::Parser parser, bool value) noexcept;

preds::PatResult parse_pattern_fallback_wildcard(preds::Parser parser) noexcept;

preds::PatResult parse_pat(preds::Parser parser) noexcept;

preds::PatsResult parse_pat_args(preds::Parser parser) noexcept;

preds::PatsResult parse_record_pat_fields(preds::Parser parser) noexcept;

preds::ExprResult parse_expr(preds::Parser parser) noexcept;

std::shared_ptr<ast::Expr> pipe_desugar(std::shared_ptr<ast::Expr> left_expr, std::shared_ptr<ast::Expr> right_expr) noexcept;

preds::ExprResult parse_pipe(preds::Parser parser) noexcept;

preds::ExprResult parse_or(preds::Parser parser) noexcept;

preds::ExprResult parse_and(preds::Parser parser) noexcept;

bool is_cmp_op(mlc::String op) noexcept;

preds::ExprResult parse_cmp(preds::Parser parser) noexcept;

preds::ExprResult parse_add(preds::Parser parser) noexcept;

preds::ExprResult parse_mul(preds::Parser parser) noexcept;

preds::ExprResult parse_unary(preds::Parser parser) noexcept;

preds::ExprResult parse_postfix(preds::Parser parser) noexcept;

preds::ExprsResult parse_arg_list(preds::Parser parser) noexcept;

std::shared_ptr<ast::Expr> fstr_to_string_expr(mlc::String part) noexcept;

std::shared_ptr<ast::Expr> build_fstr_part_expr(mlc::String part, bool is_literal) noexcept;

std::shared_ptr<ast::Expr> build_fstr_expr(mlc::Array<mlc::String> parts) noexcept;

preds::ExprResult parse_primary_integer_literal(preds::Parser parser, int value) noexcept;

preds::ExprResult parse_primary_string_literal(preds::Parser parser, mlc::String value) noexcept;

preds::ExprResult parse_primary_fat_string_literal(preds::Parser parser, mlc::Array<mlc::String> parts) noexcept;

preds::ExprResult parse_primary_boolean_literal(preds::Parser parser, bool value) noexcept;

preds::ExprResult parse_primary_parenthesized(preds::Parser parser) noexcept;

preds::ExprResult parse_primary_if_or_unless(preds::Parser parser) noexcept;

preds::ExprResult parse_primary_do_block(preds::Parser parser) noexcept;

preds::ExprResult parse_primary_while_loop(preds::Parser parser) noexcept;

preds::ExprResult parse_primary_for_loop(preds::Parser parser) noexcept;

preds::ExprResult parse_primary_match(preds::Parser parser) noexcept;

preds::ExprResult parse_primary_return_as_block(preds::Parser parser) noexcept;

preds::ExprResult parse_primary_identifier(preds::Parser parser, mlc::String name) noexcept;

preds::ExprResult parse_primary_unit_fallback(preds::Parser parser) noexcept;

preds::ExprResult parse_primary(preds::Parser parser) noexcept;

bool looks_like_typed_lambda_params(preds::Parser parser) noexcept;

preds::NamesResult parse_typed_lambda_params(preds::Parser parser) noexcept;

bool looks_like_lambda_params(preds::Parser parser) noexcept;

preds::NamesResult parse_lambda_params(preds::Parser parser) noexcept;

preds::ExprResult parse_array_lit(preds::Parser parser, ast::Span header_span) noexcept;

preds::ExprResult parse_if_expr(preds::Parser parser) noexcept;

preds::ExprResult parse_block(preds::Parser parser, ast::Span header_span) noexcept;

bool is_unit_expr(std::shared_ptr<ast::Expr> expr) noexcept;

std::shared_ptr<ast::Expr> block_result(mlc::Array<std::shared_ptr<ast::Stmt>> stmts) noexcept;

mlc::Array<std::shared_ptr<ast::Stmt>> block_body(mlc::Array<std::shared_ptr<ast::Stmt>> stmts) noexcept;

std::shared_ptr<ast::Expr> StmtsResult_to_block_expr(preds::StmtsResult self, ast::Span header_span) noexcept;

preds::ExprResult parse_while_expr(preds::Parser parser, ast::Span header_span) noexcept;

preds::ExprResult parse_for_expr(preds::Parser parser, ast::Span header_span) noexcept;

preds::ExprResult parse_match_expr(preds::Parser parser, ast::Span header_span) noexcept;

preds::ArmsResult parse_arms_brace(preds::Parser parser) noexcept;

preds::ArmsResult parse_arms_pipe(preds::Parser parser) noexcept;

preds::ArmsResult parse_arms(preds::Parser parser) noexcept;

preds::FieldValsResult parse_record_fields(preds::Parser parser) noexcept;

preds::ExprResult parse_record_expr(preds::Parser parser, mlc::String record_name, ast::Span record_span) noexcept;

preds::StmtsResult parse_stmts_until_end(preds::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::Stmt>> stmts = {};
preds::Parser state = std::move(parser);
while (!preds::TKind_is_end(preds::Parser_kind(state)) && !preds::Parser_at_eof(state)){
{
preds::StmtResult result = parse_stmt(state);
stmts.push_back(result.stmt);
state = preds::Parser_skip_semi(result.parser);
}
}
return preds::StmtsResult{stmts, preds::Parser_advance(state)};
}

preds::StmtsResult parse_stmts_until_else_end(preds::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::Stmt>> stmts = {};
preds::Parser state = std::move(parser);
while (!preds::TKind_is_end(preds::Parser_kind(state)) && !preds::TKind_is_else(preds::Parser_kind(state)) && !preds::Parser_at_eof(state)){
{
preds::StmtResult result = parse_stmt(state);
stmts.push_back(result.stmt);
state = result.parser;
}
}
return preds::StmtsResult{stmts, state};
}

preds::StmtResult parse_statement_let_const(preds::Parser parser, ast::Span statement_span) noexcept{
preds::Parser name_pos = preds::Parser_advance(parser);
mlc::String var_name = preds::TKind_ident(preds::Parser_kind(name_pos));
preds::TypeResult type_result = preds::TKind_is_colon(preds::Parser_kind(preds::Parser_advance(name_pos))) ? types::parse_type(preds::Parser_advance_by(name_pos, 2)) : preds::TypeResult{std::make_shared<ast::TypeExpr>((ast::TyUnit{})), preds::Parser_advance(name_pos)};
preds::ExprResult value_result = parse_expr(preds::Parser_advance(type_result.parser));
return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtLetConst(var_name, type_result.type_expr, value_result.expr, statement_span)), value_result.parser};
}

preds::StmtResult parse_statement_let(preds::Parser parser) noexcept{
ast::Span statement_span = preds::Parser_span_at_cursor(parser);
preds::Parser after_let = preds::Parser_advance(parser);
return preds::TKind_is_const(preds::Parser_kind(after_let)) ? parse_statement_let_const(after_let, statement_span) : [&]() -> preds::StmtResult { 
  bool is_mut = preds::TKind_is_mut(preds::Parser_kind(after_let));
  preds::Parser name_pos = is_mut ? preds::Parser_advance(after_let) : after_let;
  mlc::String var_name = preds::TKind_ident(preds::Parser_kind(name_pos));
  preds::TypeResult type_result = preds::TKind_is_colon(preds::Parser_kind(preds::Parser_advance(name_pos))) ? types::parse_type(preds::Parser_advance_by(name_pos, 2)) : preds::TypeResult{std::make_shared<ast::TypeExpr>((ast::TyUnit{})), preds::Parser_advance(name_pos)};
  preds::ExprResult value_result = parse_expr(preds::Parser_advance(type_result.parser));
  return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtLet(var_name, is_mut, type_result.type_expr, value_result.expr, statement_span)), value_result.parser};
 }();
}

preds::StmtResult parse_statement_const(preds::Parser parser) noexcept{
ast::Span statement_span = preds::Parser_span_at_cursor(parser);
preds::Parser name_pos = preds::Parser_advance(parser);
mlc::String var_name = preds::TKind_ident(preds::Parser_kind(name_pos));
preds::TypeResult type_result = preds::TKind_is_colon(preds::Parser_kind(preds::Parser_advance(name_pos))) ? types::parse_type(preds::Parser_advance_by(name_pos, 2)) : preds::TypeResult{std::make_shared<ast::TypeExpr>((ast::TyUnit{})), preds::Parser_advance(name_pos)};
preds::ExprResult value_result = parse_expr(preds::Parser_advance(type_result.parser));
return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtLet(var_name, false, type_result.type_expr, value_result.expr, statement_span)), value_result.parser};
}

preds::StmtResult parse_statement_return(preds::Parser parser) noexcept{
ast::Span statement_span = preds::Parser_span_at_cursor(parser);
preds::ExprResult value_result = parse_expr(preds::Parser_advance(parser));
return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtReturn(value_result.expr, statement_span)), value_result.parser};
}

preds::StmtResult parse_statement_expression_or_assign(preds::Parser parser) noexcept{
ast::Span statement_span = preds::Parser_span_at_cursor(parser);
preds::ExprResult expr_result = parse_expr(parser);
return preds::TKind_is_equal(preds::Parser_kind(expr_result.parser)) ? [&]() -> preds::StmtResult { 
  ast::Span assign_span = preds::Parser_span_at_cursor(expr_result.parser);
  preds::ExprResult right_hand = parse_expr(preds::Parser_advance(expr_result.parser));
  return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtExpr(std::make_shared<ast::Expr>(ast::ExprBin(mlc::String("="), expr_result.expr, right_hand.expr, assign_span)), statement_span)), right_hand.parser};
 }() : preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtExpr(expr_result.expr, statement_span)), expr_result.parser};
}

preds::StmtResult parse_stmt(preds::Parser parser) noexcept{
ast_tokens::TKind kind = preds::Parser_kind(parser);
return preds::TKind_is_let(kind) ? parse_statement_let(parser) : preds::TKind_is_const(kind) ? parse_statement_const(parser) : preds::TKind_is_break(kind) ? preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtBreak(preds::Parser_span_at_cursor(parser))), preds::Parser_advance(parser)} : preds::TKind_is_continue(kind) ? preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtContinue(preds::Parser_span_at_cursor(parser))), preds::Parser_advance(parser)} : preds::TKind_is_return(kind) ? parse_statement_return(parser) : preds::TKind_is_while(kind) ? [&]() -> preds::StmtResult { 
  ast::Span header_span = preds::Parser_span_at_cursor(parser);
  preds::ExprResult while_result = parse_while_expr(preds::Parser_advance(parser), header_span);
  return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtExpr(while_result.expr, header_span)), while_result.parser};
 }() : preds::TKind_is_for(kind) ? [&]() -> preds::StmtResult { 
  ast::Span header_span = preds::Parser_span_at_cursor(parser);
  preds::ExprResult for_result = parse_for_expr(preds::Parser_advance(parser), header_span);
  return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtExpr(for_result.expr, header_span)), for_result.parser};
 }() : preds::TKind_is_do(kind) ? [&]() -> preds::StmtResult { 
  ast::Span header_span = preds::Parser_span_at_cursor(parser);
  preds::ExprResult block_result = parse_block(preds::Parser_advance(parser), header_span);
  return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtExpr(block_result.expr, header_span)), block_result.parser};
 }() : parse_statement_expression_or_assign(parser);
}

preds::PatResult parse_pattern_identifier_branch(preds::Parser parser, mlc::String name) noexcept{
ast::Span pattern_span = preds::Parser_span_at_cursor(parser);
return name == mlc::String("_") ? preds::PatResult{std::make_shared<ast::Pat>(ast::PatWild(pattern_span)), preds::Parser_advance(parser)} : preds::is_ctor_name(name) ? [&]() -> preds::PatResult { 
  preds::Parser after = preds::Parser_advance(parser);
  return preds::TKind_is_lparen(preds::Parser_kind(after)) ? [&]() -> preds::PatResult { 
  preds::PatsResult sub_patterns = parse_pat_args(preds::Parser_advance(after));
  return preds::PatResult{std::make_shared<ast::Pat>(ast::PatCtor(name, sub_patterns.pats, pattern_span)), sub_patterns.parser};
 }() : preds::TKind_is_lbrace(preds::Parser_kind(after)) ? [&]() -> preds::PatResult { 
  preds::PatsResult field_result = parse_record_pat_fields(preds::Parser_advance(after));
  return preds::PatResult{std::make_shared<ast::Pat>(ast::PatRecord(name, field_result.pats, pattern_span)), field_result.parser};
 }() : preds::PatResult{std::make_shared<ast::Pat>(ast::PatCtor(name, {}, pattern_span)), after};
 }() : preds::PatResult{std::make_shared<ast::Pat>(ast::PatIdent(name, pattern_span)), preds::Parser_advance(parser)};
}

preds::PatResult parse_pattern_integer(preds::Parser parser, int value) noexcept{return preds::PatResult{std::make_shared<ast::Pat>(ast::PatInt(value, preds::Parser_span_at_cursor(parser))), preds::Parser_advance(parser)};}

preds::PatResult parse_pattern_string(preds::Parser parser, mlc::String value) noexcept{return preds::PatResult{std::make_shared<ast::Pat>(ast::PatStr(value, preds::Parser_span_at_cursor(parser))), preds::Parser_advance(parser)};}

preds::PatResult parse_pattern_boolean(preds::Parser parser, bool value) noexcept{return preds::PatResult{std::make_shared<ast::Pat>(ast::PatBool(value, preds::Parser_span_at_cursor(parser))), preds::Parser_advance(parser)};}

preds::PatResult parse_pattern_fallback_wildcard(preds::Parser parser) noexcept{return preds::PatResult{std::make_shared<ast::Pat>(ast::PatWild(preds::Parser_span_at_cursor(parser))), preds::Parser_advance(parser)};}

preds::PatResult parse_pat(preds::Parser parser) noexcept{
ast_tokens::TKind kind = preds::Parser_kind(parser);
return preds::TKind_is_ident(kind) ? parse_pattern_identifier_branch(parser, preds::TKind_ident(kind)) : preds::TKind_is_int(kind) ? parse_pattern_integer(parser, preds::TKind_int_val(kind)) : preds::TKind_is_str(kind) ? parse_pattern_string(parser, preds::TKind_str_val(kind)) : preds::TKind_is_true(kind) ? parse_pattern_boolean(parser, true) : preds::TKind_is_false(kind) ? parse_pattern_boolean(parser, false) : parse_pattern_fallback_wildcard(parser);
}

preds::PatsResult parse_pat_args(preds::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::Pat>> pats = {};
return preds::TKind_is_rparen(preds::Parser_kind(parser)) ? preds::PatsResult{pats, preds::Parser_advance(parser)} : [&]() -> preds::PatsResult { 
  preds::PatResult first = parse_pat(parser);
  pats.push_back(first.pat);
  preds::Parser state = first.parser;
  while (preds::TKind_is_comma(preds::Parser_kind(state))){
{
preds::PatResult next = parse_pat(preds::Parser_advance(state));
pats.push_back(next.pat);
state = next.parser;
}
}
  return preds::PatsResult{pats, preds::Parser_advance(state)};
 }();
}

preds::PatsResult parse_record_pat_fields(preds::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::Pat>> pats = {};
preds::Parser state = std::move(parser);
while (!preds::TKind_is_rbrace(preds::Parser_kind(state)) && !preds::Parser_at_eof(state)){
{
if (preds::TKind_is_ident(preds::Parser_kind(state))){
{
pats.push_back(std::make_shared<ast::Pat>(ast::PatIdent(preds::TKind_ident(preds::Parser_kind(state)), preds::Parser_span_at_cursor(state))));
}
}
state = preds::Parser_advance(state);
if (preds::TKind_is_comma(preds::Parser_kind(state))){
{
state = preds::Parser_advance(state);
}
}
}
}
return preds::PatsResult{pats, preds::Parser_advance(state)};
}

preds::ExprResult parse_expr(preds::Parser parser) noexcept{return parse_pipe(parser);}

std::shared_ptr<ast::Expr> pipe_desugar(std::shared_ptr<ast::Expr> left_expr, std::shared_ptr<ast::Expr> right_expr) noexcept{return [&]() -> std::shared_ptr<ast::Expr> { if (std::holds_alternative<ast::ExprCall>((*right_expr)._)) { auto _v_exprcall = std::get<ast::ExprCall>((*right_expr)._); auto [callee, existing_args, _w0] = _v_exprcall; return [&]() -> std::shared_ptr<ast::Expr> { 
  mlc::Array<std::shared_ptr<ast::Expr>> new_args = mlc::Array<std::shared_ptr<ast::Expr>>{left_expr};
  int i = 0;
  while (i < existing_args.size()){
{
new_args.push_back(existing_args[i]);
i = i + 1;
}
}
  return std::make_shared<ast::Expr>(ast::ExprCall(callee, new_args, ast::span_unknown()));
 }(); } return [&]() -> std::shared_ptr<ast::Expr> { 
  mlc::Array<std::shared_ptr<ast::Expr>> call_args = mlc::Array<std::shared_ptr<ast::Expr>>{left_expr};
  return std::make_shared<ast::Expr>(ast::ExprCall(right_expr, call_args, ast::span_unknown()));
 }(); }();}

preds::ExprResult parse_pipe(preds::Parser parser) noexcept{
preds::ExprResult left = parse_or(parser);
std::shared_ptr<ast::Expr> expr = left.expr;
preds::Parser state = left.parser;
while (preds::TKind_is_pipe(preds::Parser_kind(state))){
{
preds::ExprResult right = parse_or(preds::Parser_advance(state));
expr = pipe_desugar(expr, right.expr);
state = right.parser;
}
}
return preds::ExprResult{expr, state};
}

preds::ExprResult parse_or(preds::Parser parser) noexcept{
preds::ExprResult left = parse_and(parser);
std::shared_ptr<ast::Expr> expr = left.expr;
preds::Parser state = left.parser;
while (preds::TKind_is_op(preds::Parser_kind(state)) && preds::TKind_op_val(preds::Parser_kind(state)) == mlc::String("||")){
{
ast::Span operator_span = preds::Parser_span_at_cursor(state);
preds::ExprResult right = parse_and(preds::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprBin(mlc::String("||"), expr, right.expr, operator_span));
state = right.parser;
}
}
return preds::ExprResult{expr, state};
}

preds::ExprResult parse_and(preds::Parser parser) noexcept{
preds::ExprResult left = parse_cmp(parser);
std::shared_ptr<ast::Expr> expr = left.expr;
preds::Parser state = left.parser;
while (preds::TKind_is_op(preds::Parser_kind(state)) && preds::TKind_op_val(preds::Parser_kind(state)) == mlc::String("&&")){
{
ast::Span operator_span = preds::Parser_span_at_cursor(state);
preds::ExprResult right = parse_cmp(preds::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprBin(mlc::String("&&"), expr, right.expr, operator_span));
state = right.parser;
}
}
return preds::ExprResult{expr, state};
}

bool is_cmp_op(mlc::String op) noexcept{return op == mlc::String("==") || op == mlc::String("!=") || op == mlc::String("<") || op == mlc::String(">") || op == mlc::String("<=") || op == mlc::String(">=");}

preds::ExprResult parse_cmp(preds::Parser parser) noexcept{
preds::ExprResult left = parse_add(parser);
std::shared_ptr<ast::Expr> expr = left.expr;
preds::Parser state = left.parser;
ast_tokens::TKind kind = preds::Parser_kind(state);
if (preds::TKind_is_op(kind) && is_cmp_op(preds::TKind_op_val(kind))){
{
ast::Span operator_span = preds::Parser_span_at_cursor(state);
preds::ExprResult right = parse_add(preds::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprBin(preds::TKind_op_val(kind), expr, right.expr, operator_span));
state = right.parser;
}
}
return preds::ExprResult{expr, state};
}

preds::ExprResult parse_add(preds::Parser parser) noexcept{
preds::ExprResult left = parse_mul(parser);
std::shared_ptr<ast::Expr> expr = left.expr;
preds::Parser state = left.parser;
bool go = true;
while (go){
{
ast_tokens::TKind kind = preds::Parser_kind(state);
if (preds::TKind_is_op(kind) && preds::TKind_op_val(kind) == mlc::String("+") || preds::TKind_op_val(kind) == mlc::String("-")){
{
ast::Span operator_span = preds::Parser_span_at_cursor(state);
preds::ExprResult right = parse_mul(preds::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprBin(preds::TKind_op_val(kind), expr, right.expr, operator_span));
state = right.parser;
}
} else {
{
go = false;
}
}
}
}
return preds::ExprResult{expr, state};
}

preds::ExprResult parse_mul(preds::Parser parser) noexcept{
preds::ExprResult left = parse_unary(parser);
std::shared_ptr<ast::Expr> expr = left.expr;
preds::Parser state = left.parser;
bool go = true;
while (go){
{
ast_tokens::TKind kind = preds::Parser_kind(state);
if (preds::TKind_is_op(kind) && preds::TKind_op_val(kind) == mlc::String("*") || preds::TKind_op_val(kind) == mlc::String("/") || preds::TKind_op_val(kind) == mlc::String("%")){
{
ast::Span operator_span = preds::Parser_span_at_cursor(state);
preds::ExprResult right = parse_unary(preds::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprBin(preds::TKind_op_val(kind), expr, right.expr, operator_span));
state = right.parser;
}
} else {
{
go = false;
}
}
}
}
return preds::ExprResult{expr, state};
}

preds::ExprResult parse_unary(preds::Parser parser) noexcept{
ast_tokens::TKind kind = preds::Parser_kind(parser);
return preds::TKind_is_op(kind) && preds::TKind_op_val(kind) == mlc::String("!") || preds::TKind_op_val(kind) == mlc::String("-") ? [&]() -> preds::ExprResult { 
  ast::Span operator_span = preds::Parser_span_at_cursor(parser);
  preds::ExprResult inner = parse_unary(preds::Parser_advance(parser));
  return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprUn(preds::TKind_op_val(kind), inner.expr, operator_span)), inner.parser};
 }() : parse_postfix(parser);
}

preds::ExprResult parse_postfix(preds::Parser parser) noexcept{
preds::ExprResult base = parse_primary(parser);
std::shared_ptr<ast::Expr> expr = base.expr;
preds::Parser state = base.parser;
bool go = true;
while (go){
{
ast_tokens::TKind kind = preds::Parser_kind(state);
if (preds::TKind_is_dot(kind)){
{
ast::Span dot_span = preds::Parser_span_at_cursor(state);
mlc::String field_name = preds::TKind_ident(preds::Parser_kind(preds::Parser_advance(state)));
if (preds::TKind_is_lparen(preds::Parser_kind(preds::Parser_advance_by(state, 2))) && preds::Parser_span_at_cursor(preds::Parser_advance_by(state, 2)).line == preds::Parser_span_at_cursor(preds::Parser_advance_by(state, 1)).line){
preds::ExprsResult margs = parse_arg_list(preds::Parser_advance_by(state, 3));
expr = std::make_shared<ast::Expr>(ast::ExprMethod(expr, field_name, margs.exprs, dot_span));
state = margs.parser;
} else {
expr = std::make_shared<ast::Expr>(ast::ExprField(expr, field_name, dot_span));
state = preds::Parser_advance_by(state, 2);
}
}
} else {
{
if (preds::TKind_is_lparen(kind) && preds::Parser_span_at_cursor(state).line == preds::Parser_prev_line(state)){
ast::Span call_span = preds::Parser_span_at_cursor(state);
preds::ExprsResult call_args = parse_arg_list(preds::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprCall(expr, call_args.exprs, call_span));
state = call_args.parser;
} else {
if (preds::TKind_is_lbracket(kind)){
ast::Span bracket_span = preds::Parser_span_at_cursor(state);
preds::ExprResult index = parse_expr(preds::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprIndex(expr, index.expr, bracket_span));
state = preds::Parser_advance(index.parser);
} else {
if (preds::TKind_is_question(kind)){
ast::Span question_span = preds::Parser_span_at_cursor(state);
expr = std::make_shared<ast::Expr>(ast::ExprQuestion(expr, question_span));
state = preds::Parser_advance(state);
} else {
go = false;
}
}
}
}
}
}
}
return preds::ExprResult{expr, state};
}

preds::ExprsResult parse_arg_list(preds::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::Expr>> exprs = {};
return preds::TKind_is_rparen(preds::Parser_kind(parser)) ? preds::ExprsResult{exprs, preds::Parser_advance(parser)} : [&]() -> preds::ExprsResult { 
  preds::ExprResult first = parse_expr(parser);
  exprs.push_back(first.expr);
  preds::Parser state = first.parser;
  while (preds::TKind_is_comma(preds::Parser_kind(state))){
{
preds::ExprResult next = parse_expr(preds::Parser_advance(state));
exprs.push_back(next.expr);
state = next.parser;
}
}
  return preds::ExprsResult{exprs, preds::Parser_advance(state)};
 }();
}

std::shared_ptr<ast::Expr> fstr_to_string_expr(mlc::String part) noexcept{return std::make_shared<ast::Expr>(ast::ExprMethod(std::make_shared<ast::Expr>(ast::ExprIdent(part, ast::span_unknown())), mlc::String("to_string"), {}, ast::span_unknown()));}

std::shared_ptr<ast::Expr> build_fstr_part_expr(mlc::String part, bool is_literal) noexcept{return is_literal ? std::make_shared<ast::Expr>(ast::ExprStr(part, ast::span_unknown())) : part.length() == 0 ? std::make_shared<ast::Expr>(ast::ExprStr(part, ast::span_unknown())) : fstr_to_string_expr(part);}

std::shared_ptr<ast::Expr> build_fstr_expr(mlc::Array<mlc::String> parts) noexcept{
std::shared_ptr<ast::Expr> result = std::make_shared<ast::Expr>(ast::ExprStr(mlc::String(""), ast::span_unknown()));
int pi = 0;
while (pi < parts.size()){
{
std::shared_ptr<ast::Expr> part_expr = build_fstr_part_expr(parts[pi], pi % 2 == 0);
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

preds::ExprResult parse_primary_integer_literal(preds::Parser parser, int value) noexcept{
ast::Span source_span = preds::Parser_span_at_cursor(parser);
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprInt(value, source_span)), preds::Parser_advance(parser)};
}

preds::ExprResult parse_primary_string_literal(preds::Parser parser, mlc::String value) noexcept{
ast::Span source_span = preds::Parser_span_at_cursor(parser);
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprStr(value, source_span)), preds::Parser_advance(parser)};
}

preds::ExprResult parse_primary_fat_string_literal(preds::Parser parser, mlc::Array<mlc::String> parts) noexcept{return preds::ExprResult{build_fstr_expr(parts), preds::Parser_advance(parser)};}

preds::ExprResult parse_primary_boolean_literal(preds::Parser parser, bool value) noexcept{
ast::Span source_span = preds::Parser_span_at_cursor(parser);
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprBool(value, source_span)), preds::Parser_advance(parser)};
}

preds::ExprResult parse_primary_parenthesized(preds::Parser parser) noexcept{
ast::Span open_paren_span = preds::Parser_span_at_cursor(parser);
return preds::TKind_is_rparen(preds::Parser_kind(preds::Parser_advance(parser))) ? preds::TKind_is_fat_arrow(preds::Parser_kind(preds::Parser_advance_by(parser, 2))) ? [&]() -> preds::ExprResult { 
  mlc::Array<mlc::String> params = {};
  preds::ExprResult body = parse_expr(preds::Parser_advance_by(parser, 3));
  return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprLambda(params, body.expr, open_paren_span)), body.parser};
 }() : preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprUnit(open_paren_span)), preds::Parser_advance_by(parser, 2)} : looks_like_typed_lambda_params(preds::Parser_advance(parser)) ? [&]() -> preds::ExprResult { 
  preds::NamesResult param_result = parse_typed_lambda_params(preds::Parser_advance(parser));
  preds::ExprResult body = parse_expr(preds::Parser_advance(param_result.parser));
  return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprLambda(param_result.exprs, body.expr, open_paren_span)), body.parser};
 }() : looks_like_lambda_params(preds::Parser_advance(parser)) ? [&]() -> preds::ExprResult { 
  preds::NamesResult param_result = parse_lambda_params(preds::Parser_advance(parser));
  preds::ExprResult body = parse_expr(preds::Parser_advance(param_result.parser));
  return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprLambda(param_result.exprs, body.expr, open_paren_span)), body.parser};
 }() : [&]() -> preds::ExprResult { 
  preds::ExprResult inner = parse_expr(preds::Parser_advance(parser));
  return preds::ExprResult{inner.expr, preds::Parser_advance(inner.parser)};
 }();
}

preds::ExprResult parse_primary_if_or_unless(preds::Parser parser) noexcept{return parse_if_expr(parser);}

preds::ExprResult parse_primary_do_block(preds::Parser parser) noexcept{
ast::Span header_span = preds::Parser_span_at_cursor(parser);
return parse_block(preds::Parser_advance(parser), header_span);
}

preds::ExprResult parse_primary_while_loop(preds::Parser parser) noexcept{
ast::Span header_span = preds::Parser_span_at_cursor(parser);
return parse_while_expr(preds::Parser_advance(parser), header_span);
}

preds::ExprResult parse_primary_for_loop(preds::Parser parser) noexcept{
ast::Span header_span = preds::Parser_span_at_cursor(parser);
return parse_for_expr(preds::Parser_advance(parser), header_span);
}

preds::ExprResult parse_primary_match(preds::Parser parser) noexcept{
ast::Span header_span = preds::Parser_span_at_cursor(parser);
return parse_match_expr(preds::Parser_advance(parser), header_span);
}

preds::ExprResult parse_primary_return_as_block(preds::Parser parser) noexcept{
ast::Span header_span = preds::Parser_span_at_cursor(parser);
preds::ExprResult value = parse_expr(preds::Parser_advance(parser));
mlc::Array<std::shared_ptr<ast::Stmt>> statements = {};
statements.push_back(std::make_shared<ast::Stmt>(ast::StmtReturn(value.expr, header_span)));
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprBlock(statements, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())), header_span)), value.parser};
}

preds::ExprResult parse_primary_identifier(preds::Parser parser, mlc::String name) noexcept{
ast::Span source_span = preds::Parser_span_at_cursor(parser);
preds::Parser after_name = preds::Parser_advance(parser);
return preds::TKind_is_fat_arrow(preds::Parser_kind(after_name)) ? [&]() -> preds::ExprResult { 
  mlc::Array<mlc::String> parameters = mlc::Array<mlc::String>{name};
  preds::ExprResult body = parse_expr(preds::Parser_advance(after_name));
  return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprLambda(parameters, body.expr, source_span)), body.parser};
 }() : preds::is_ctor_name(name) && preds::TKind_is_lbrace(preds::Parser_kind(after_name)) ? [&]() -> preds::ExprResult { 
  ast::Span brace_span = preds::Parser_span_at_cursor(after_name);
  return parse_record_expr(preds::Parser_advance(after_name), name, brace_span);
 }() : preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprIdent(name, source_span)), after_name};
}

preds::ExprResult parse_primary_unit_fallback(preds::Parser parser) noexcept{
ast::Span source_span = preds::Parser_span_at_cursor(parser);
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprUnit(source_span)), preds::Parser_advance(parser)};
}

preds::ExprResult parse_primary(preds::Parser parser) noexcept{
ast_tokens::TKind kind = preds::Parser_kind(parser);
return preds::TKind_is_int(kind) ? parse_primary_integer_literal(parser, preds::TKind_int_val(kind)) : preds::TKind_is_str(kind) ? parse_primary_string_literal(parser, preds::TKind_str_val(kind)) : preds::TKind_is_fstr(kind) ? parse_primary_fat_string_literal(parser, preds::TKind_fstr_parts(kind)) : preds::TKind_is_true(kind) ? parse_primary_boolean_literal(parser, true) : preds::TKind_is_false(kind) ? parse_primary_boolean_literal(parser, false) : preds::TKind_is_lparen(kind) ? parse_primary_parenthesized(parser) : preds::TKind_is_lbracket(kind) ? [&]() -> preds::ExprResult { 
  ast::Span bracket_span = preds::Parser_span_at_cursor(parser);
  return parse_array_lit(preds::Parser_advance(parser), bracket_span);
 }() : preds::TKind_is_if(kind) || preds::TKind_is_unless(kind) ? parse_primary_if_or_unless(parser) : preds::TKind_is_do(kind) ? parse_primary_do_block(parser) : preds::TKind_is_while(kind) ? parse_primary_while_loop(parser) : preds::TKind_is_for(kind) ? parse_primary_for_loop(parser) : preds::TKind_is_match(kind) ? parse_primary_match(parser) : preds::TKind_is_return(kind) ? parse_primary_return_as_block(parser) : preds::TKind_is_ident(kind) ? parse_primary_identifier(parser, preds::TKind_ident(kind)) : parse_primary_unit_fallback(parser);
}

bool looks_like_typed_lambda_params(preds::Parser parser) noexcept{return preds::TKind_is_ident(preds::Parser_kind(parser)) && preds::TKind_is_colon(preds::Parser_kind(preds::Parser_advance(parser)));}

preds::NamesResult parse_typed_lambda_params(preds::Parser parser) noexcept{
mlc::Array<mlc::String> names = {};
preds::Parser state = std::move(parser);
names.push_back(preds::TKind_ident(preds::Parser_kind(state)));
state = types::parse_type(preds::Parser_advance_by(state, 2)).parser;
while (preds::TKind_is_comma(preds::Parser_kind(state))){
{
state = preds::Parser_advance(state);
names.push_back(preds::TKind_ident(preds::Parser_kind(state)));
state = types::parse_type(preds::Parser_advance_by(state, 2)).parser;
}
}
return preds::NamesResult{names, preds::Parser_advance(state)};
}

bool looks_like_lambda_params(preds::Parser parser) noexcept{
preds::Parser state = std::move(parser);
bool ok = true;
if (!preds::TKind_is_ident(preds::Parser_kind(state))){
{
ok = false;
}
} else {
{
state = preds::Parser_advance(state);
while (preds::TKind_is_comma(preds::Parser_kind(state))){
{
state = preds::Parser_advance(state);
if (!preds::TKind_is_ident(preds::Parser_kind(state))){
{
ok = false;
state = preds::Parser_advance(state);
}
} else {
{
state = preds::Parser_advance(state);
}
}
}
}
if (!preds::TKind_is_rparen(preds::Parser_kind(state))){
ok = false;
} else {
ok = preds::TKind_is_fat_arrow(preds::Parser_kind(preds::Parser_advance(state)));
}
}
}
return ok;
}

preds::NamesResult parse_lambda_params(preds::Parser parser) noexcept{
mlc::Array<mlc::String> names = {};
preds::Parser state = std::move(parser);
names.push_back(preds::TKind_ident(preds::Parser_kind(state)));
state = preds::Parser_advance(state);
while (preds::TKind_is_comma(preds::Parser_kind(state))){
{
state = preds::Parser_advance(state);
names.push_back(preds::TKind_ident(preds::Parser_kind(state)));
state = preds::Parser_advance(state);
}
}
return preds::NamesResult{names, preds::Parser_advance(state)};
}

preds::ExprResult parse_array_lit(preds::Parser parser, ast::Span header_span) noexcept{
mlc::Array<std::shared_ptr<ast::Expr>> exprs = {};
return preds::TKind_is_rbracket(preds::Parser_kind(parser)) ? preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprArray(exprs, header_span)), preds::Parser_advance(parser)} : [&]() -> preds::ExprResult { 
  preds::ExprResult first = parse_expr(parser);
  exprs.push_back(first.expr);
  preds::Parser state = first.parser;
  while (preds::TKind_is_comma(preds::Parser_kind(state))){
{
preds::ExprResult next = parse_expr(preds::Parser_advance(state));
exprs.push_back(next.expr);
state = next.parser;
}
}
  return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprArray(exprs, header_span)), preds::Parser_advance(state)};
 }();
}

preds::ExprResult parse_if_expr(preds::Parser parser) noexcept{
ast::Span header_span = preds::Parser_span_at_cursor(parser);
bool is_negated = preds::TKind_is_unless(preds::Parser_kind(parser));
preds::ExprResult cond_result = parse_expr(preds::Parser_advance(parser));
std::shared_ptr<ast::Expr> condition = is_negated ? std::make_shared<ast::Expr>(ast::ExprUn(mlc::String("!"), cond_result.expr, ast::span_unknown())) : cond_result.expr;
preds::StmtsResult then_result = parse_stmts_until_else_end(preds::Parser_advance(cond_result.parser));
std::shared_ptr<ast::Expr> then_expr = StmtsResult_to_block_expr(then_result, header_span);
return preds::TKind_is_else(preds::Parser_kind(then_result.parser)) ? [&]() -> preds::ExprResult { 
  preds::Parser after_else = preds::Parser_advance(then_result.parser);
  return preds::TKind_is_if(preds::Parser_kind(after_else)) || preds::TKind_is_unless(preds::Parser_kind(after_else)) ? [&]() -> preds::ExprResult { 
  preds::ExprResult else_result = parse_if_expr(after_else);
  return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprIf(condition, then_expr, else_result.expr, header_span)), else_result.parser};
 }() : preds::TKind_is_do(preds::Parser_kind(after_else)) ? [&]() -> preds::ExprResult { 
  preds::ExprResult else_block = parse_block(preds::Parser_advance(after_else), header_span);
  preds::Parser after_block = preds::TKind_is_end(preds::Parser_kind(else_block.parser)) ? preds::Parser_advance(else_block.parser) : else_block.parser;
  return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprIf(condition, then_expr, else_block.expr, header_span)), after_block};
 }() : [&]() -> preds::ExprResult { 
  ast::Span else_span = preds::Parser_span_at_cursor(after_else);
  preds::StmtsResult else_stmts = parse_stmts_until_end(after_else);
  return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprIf(condition, then_expr, StmtsResult_to_block_expr(else_stmts, else_span), header_span)), else_stmts.parser};
 }();
 }() : preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprIf(condition, then_expr, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())), header_span)), preds::Parser_advance(then_result.parser)};
}

preds::ExprResult parse_block(preds::Parser parser, ast::Span header_span) noexcept{
preds::StmtsResult result = parse_stmts_until_end(parser);
return preds::ExprResult{StmtsResult_to_block_expr(result, header_span), result.parser};
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

std::shared_ptr<ast::Expr> StmtsResult_to_block_expr(preds::StmtsResult self, ast::Span header_span) noexcept{return std::make_shared<ast::Expr>(ast::ExprBlock(block_body(self.stmts), block_result(self.stmts), header_span));}

preds::ExprResult parse_while_expr(preds::Parser parser, ast::Span header_span) noexcept{
preds::ExprResult cond = parse_expr(parser);
preds::StmtsResult body = parse_stmts_until_end(preds::Parser_advance(cond.parser));
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprWhile(cond.expr, body.stmts, header_span)), body.parser};
}

preds::ExprResult parse_for_expr(preds::Parser parser, ast::Span header_span) noexcept{
mlc::String var_name = preds::TKind_ident(preds::Parser_kind(parser));
preds::ExprResult iter = parse_expr(preds::Parser_advance_by(parser, 2));
preds::StmtsResult body = parse_stmts_until_end(preds::Parser_advance(iter.parser));
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprFor(var_name, iter.expr, body.stmts, header_span)), body.parser};
}

preds::ExprResult parse_match_expr(preds::Parser parser, ast::Span header_span) noexcept{
preds::ExprResult subject = parse_expr(parser);
ast_tokens::TKind next = preds::Parser_kind(subject.parser);
return preds::TKind_is_lbrace(next) ? [&]() -> preds::ExprResult { 
  preds::ArmsResult arms = parse_arms_brace(preds::Parser_advance(subject.parser));
  return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprMatch(subject.expr, arms.arms, header_span)), arms.parser};
 }() : [&]() -> preds::ExprResult { 
  preds::ArmsResult arms = parse_arms_pipe(subject.parser);
  return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprMatch(subject.expr, arms.arms, header_span)), arms.parser};
 }();
}

preds::ArmsResult parse_arms_brace(preds::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::MatchArm>> arms = {};
preds::Parser state = std::move(parser);
while (!preds::TKind_is_rbrace(preds::Parser_kind(state)) && !preds::Parser_at_eof(state)){
{
if (preds::TKind_is_bar(preds::Parser_kind(state))){
{
state = preds::Parser_advance(state);
}
}
preds::PatResult pat_result = parse_pat(state);
preds::ExprResult body_result = parse_expr(preds::Parser_advance(pat_result.parser));
arms.push_back(std::make_shared<ast::MatchArm>(ast::MatchArm{pat_result.pat, body_result.expr}));
state = body_result.parser;
if (preds::TKind_is_comma(preds::Parser_kind(state))){
{
state = preds::Parser_advance(state);
}
}
}
}
return preds::ArmsResult{arms, preds::Parser_advance(state)};
}

preds::ArmsResult parse_arms_pipe(preds::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::MatchArm>> arms = {};
preds::Parser state = std::move(parser);
while (preds::TKind_is_bar(preds::Parser_kind(state))){
{
preds::PatResult pat_result = parse_pat(preds::Parser_advance(state));
preds::ExprResult body_result = parse_expr(preds::Parser_advance(pat_result.parser));
arms.push_back(std::make_shared<ast::MatchArm>(ast::MatchArm{pat_result.pat, body_result.expr}));
state = body_result.parser;
}
}
if (preds::TKind_is_end(preds::Parser_kind(state))){
{
state = preds::Parser_advance(state);
}
}
return preds::ArmsResult{arms, state};
}

preds::ArmsResult parse_arms(preds::Parser parser) noexcept{return parse_arms_brace(parser);}

preds::FieldValsResult parse_record_fields(preds::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals = {};
preds::Parser state = std::move(parser);
while (!preds::TKind_is_rbrace(preds::Parser_kind(state)) && !preds::Parser_at_eof(state)){
{
ast::Span field_span = preds::Parser_span_at_cursor(state);
mlc::String field_name = preds::TKind_ident(preds::Parser_kind(state));
preds::Parser after_name = preds::Parser_advance(state);
if (preds::TKind_is_colon(preds::Parser_kind(after_name))){
{
preds::ExprResult value_result = parse_expr(preds::Parser_advance(after_name));
field_vals.push_back(std::make_shared<ast::FieldVal>(ast::FieldVal{field_name, value_result.expr}));
state = value_result.parser;
}
} else {
{
field_vals.push_back(std::make_shared<ast::FieldVal>(ast::FieldVal{field_name, std::make_shared<ast::Expr>(ast::ExprIdent(field_name, field_span))}));
state = after_name;
}
}
if (preds::TKind_is_comma(preds::Parser_kind(state))){
{
state = preds::Parser_advance(state);
}
}
}
}
return preds::FieldValsResult{field_vals, preds::Parser_advance(state)};
}

preds::ExprResult parse_record_expr(preds::Parser parser, mlc::String record_name, ast::Span record_span) noexcept{return preds::TKind_is_spread(preds::Parser_kind(parser)) ? [&]() -> preds::ExprResult { 
  preds::ExprResult base_result = parse_expr(preds::Parser_advance(parser));
  preds::Parser after_base = base_result.parser;
  if (preds::TKind_is_comma(preds::Parser_kind(after_base))){
{
after_base = preds::Parser_advance(after_base);
}
}
  preds::FieldValsResult fields_result = parse_record_fields(after_base);
  return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprRecordUpdate(record_name, base_result.expr, fields_result.field_vals, record_span)), fields_result.parser};
 }() : [&]() -> preds::ExprResult { 
  preds::FieldValsResult fields_result = parse_record_fields(parser);
  return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprRecord(record_name, fields_result.field_vals, record_span)), fields_result.parser};
 }();}

} // namespace exprs
