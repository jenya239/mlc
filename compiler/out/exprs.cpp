#include "exprs.hpp"

#include "preds.hpp"
#include "types.hpp"

namespace exprs {

using namespace preds;
using namespace types;

preds::StmtsResult parse_stmts_until_end(preds::Parser parser) noexcept;

preds::StmtsResult parse_stmts_until_else_end(preds::Parser parser) noexcept;

preds::StmtResult parse_stmt(preds::Parser parser) noexcept;

preds::PatResult parse_pat(preds::Parser parser) noexcept;

preds::PatsResult parse_pat_args(preds::Parser parser) noexcept;

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

preds::ExprResult parse_primary(preds::Parser parser) noexcept;

bool looks_like_lambda_params(preds::Parser parser) noexcept;

preds::NamesResult parse_lambda_params(preds::Parser parser) noexcept;

preds::ExprResult parse_array_lit(preds::Parser parser) noexcept;

preds::ExprResult parse_if_expr(preds::Parser parser) noexcept;

preds::ExprResult parse_block(preds::Parser parser) noexcept;

bool is_unit_expr(std::shared_ptr<ast::Expr> expr) noexcept;

std::shared_ptr<ast::Expr> block_result(mlc::Array<std::shared_ptr<ast::Stmt>> stmts) noexcept;

mlc::Array<std::shared_ptr<ast::Stmt>> block_body(mlc::Array<std::shared_ptr<ast::Stmt>> stmts) noexcept;

std::shared_ptr<ast::Expr> StmtsResult_to_block_expr(preds::StmtsResult self) noexcept;

preds::ExprResult parse_while_expr(preds::Parser parser) noexcept;

preds::ExprResult parse_for_expr(preds::Parser parser) noexcept;

preds::ExprResult parse_match_expr(preds::Parser parser) noexcept;

preds::ArmsResult parse_arms_brace(preds::Parser parser) noexcept;

preds::ArmsResult parse_arms_pipe(preds::Parser parser) noexcept;

preds::ArmsResult parse_arms(preds::Parser parser) noexcept;

preds::FieldValsResult parse_record_fields(preds::Parser parser) noexcept;

preds::ExprResult parse_record_expr(preds::Parser parser, mlc::String record_name) noexcept;

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

preds::StmtResult parse_stmt(preds::Parser parser) noexcept{
ast_tokens::TKind kind = preds::Parser_kind(parser);
return preds::TKind_is_let(kind) ? [&]() -> preds::StmtResult { 
  preds::Parser after_let = preds::Parser_advance(parser);
  bool is_mut = preds::TKind_is_mut(preds::Parser_kind(after_let));
  preds::Parser name_pos = is_mut ? preds::Parser_advance(after_let) : after_let;
  mlc::String var_name = preds::TKind_ident(preds::Parser_kind(name_pos));
  preds::TypeResult type_result = preds::TKind_is_colon(preds::Parser_kind(preds::Parser_advance(name_pos))) ? types::parse_type(preds::Parser_advance_by(name_pos, 2)) : preds::TypeResult{std::make_shared<ast::TypeExpr>((ast::TyUnit{})), preds::Parser_advance(name_pos)};
  preds::ExprResult value_result = parse_expr(preds::Parser_advance(type_result.parser));
  return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtLet(var_name, is_mut, type_result.type_expr, value_result.expr)), value_result.parser};
 }() : preds::TKind_is_const(kind) ? [&]() -> preds::StmtResult { 
  preds::Parser name_pos = preds::Parser_advance(parser);
  mlc::String var_name = preds::TKind_ident(preds::Parser_kind(name_pos));
  preds::TypeResult type_result = preds::TKind_is_colon(preds::Parser_kind(preds::Parser_advance(name_pos))) ? types::parse_type(preds::Parser_advance_by(name_pos, 2)) : preds::TypeResult{std::make_shared<ast::TypeExpr>((ast::TyUnit{})), preds::Parser_advance(name_pos)};
  preds::ExprResult value_result = parse_expr(preds::Parser_advance(type_result.parser));
  return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtLet(var_name, false, type_result.type_expr, value_result.expr)), value_result.parser};
 }() : preds::TKind_is_break(kind) ? preds::StmtResult{std::make_shared<ast::Stmt>((ast::StmtBreak{})), preds::Parser_advance(parser)} : preds::TKind_is_continue(kind) ? preds::StmtResult{std::make_shared<ast::Stmt>((ast::StmtContinue{})), preds::Parser_advance(parser)} : preds::TKind_is_return(kind) ? [&]() -> preds::StmtResult { 
  preds::ExprResult value_result = parse_expr(preds::Parser_advance(parser));
  return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtReturn(value_result.expr)), value_result.parser};
 }() : preds::TKind_is_while(kind) ? [&]() -> preds::StmtResult { 
  preds::ExprResult while_result = parse_while_expr(preds::Parser_advance(parser));
  return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtExpr(while_result.expr)), while_result.parser};
 }() : preds::TKind_is_for(kind) ? [&]() -> preds::StmtResult { 
  preds::ExprResult for_result = parse_for_expr(preds::Parser_advance(parser));
  return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtExpr(for_result.expr)), for_result.parser};
 }() : preds::TKind_is_do(kind) ? [&]() -> preds::StmtResult { 
  preds::ExprResult block_result = parse_block(preds::Parser_advance(parser));
  return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtExpr(block_result.expr)), block_result.parser};
 }() : [&]() -> preds::StmtResult { 
  preds::ExprResult expr_result = parse_expr(parser);
  return preds::TKind_is_equal(preds::Parser_kind(expr_result.parser)) ? [&]() -> preds::StmtResult { 
  preds::ExprResult rhs = parse_expr(preds::Parser_advance(expr_result.parser));
  return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtExpr(std::make_shared<ast::Expr>(ast::ExprBin(mlc::String("="), expr_result.expr, rhs.expr)))), rhs.parser};
 }() : preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtExpr(expr_result.expr)), expr_result.parser};
 }();
}

preds::PatResult parse_pat(preds::Parser parser) noexcept{
ast_tokens::TKind kind = preds::Parser_kind(parser);
return preds::TKind_is_ident(kind) ? [&]() -> preds::PatResult { 
  mlc::String name = preds::TKind_ident(kind);
  return name == mlc::String("_") ? preds::PatResult{std::make_shared<ast::Pat>((ast::PatWild{})), preds::Parser_advance(parser)} : preds::is_ctor_name(name) ? [&]() -> preds::PatResult { 
  preds::Parser after = preds::Parser_advance(parser);
  return preds::TKind_is_lparen(preds::Parser_kind(after)) ? [&]() -> preds::PatResult { 
  preds::PatsResult sub_pats = parse_pat_args(preds::Parser_advance(after));
  return preds::PatResult{std::make_shared<ast::Pat>(ast::PatCtor(name, sub_pats.pats)), sub_pats.parser};
 }() : preds::PatResult{std::make_shared<ast::Pat>(ast::PatCtor(name, {})), after};
 }() : preds::PatResult{std::make_shared<ast::Pat>(ast::PatIdent(name)), preds::Parser_advance(parser)};
 }() : preds::TKind_is_int(kind) ? preds::PatResult{std::make_shared<ast::Pat>(ast::PatInt(preds::TKind_int_val(kind))), preds::Parser_advance(parser)} : preds::TKind_is_str(kind) ? preds::PatResult{std::make_shared<ast::Pat>(ast::PatStr(preds::TKind_str_val(kind))), preds::Parser_advance(parser)} : preds::TKind_is_true(kind) ? preds::PatResult{std::make_shared<ast::Pat>(ast::PatBool(true)), preds::Parser_advance(parser)} : preds::TKind_is_false(kind) ? preds::PatResult{std::make_shared<ast::Pat>(ast::PatBool(false)), preds::Parser_advance(parser)} : preds::PatResult{std::make_shared<ast::Pat>((ast::PatWild{})), preds::Parser_advance(parser)};
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

preds::ExprResult parse_expr(preds::Parser parser) noexcept{return parse_pipe(parser);}

std::shared_ptr<ast::Expr> pipe_desugar(std::shared_ptr<ast::Expr> left_expr, std::shared_ptr<ast::Expr> right_expr) noexcept{return [&]() -> std::shared_ptr<ast::Expr> { if (std::holds_alternative<ast::ExprCall>((*right_expr)._)) { auto _v_exprcall = std::get<ast::ExprCall>((*right_expr)._); auto [callee, existing_args] = _v_exprcall; return [&]() -> std::shared_ptr<ast::Expr> { 
  mlc::Array<std::shared_ptr<ast::Expr>> new_args = mlc::Array<std::shared_ptr<ast::Expr>>{left_expr};
  int i = 0;
  while (i < existing_args.size()){
{
new_args.push_back(existing_args[i]);
i = i + 1;
}
}
  return std::make_shared<ast::Expr>(ast::ExprCall(callee, new_args));
 }(); } return [&]() -> std::shared_ptr<ast::Expr> { 
  mlc::Array<std::shared_ptr<ast::Expr>> call_args = mlc::Array<std::shared_ptr<ast::Expr>>{left_expr};
  return std::make_shared<ast::Expr>(ast::ExprCall(right_expr, call_args));
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
preds::ExprResult right = parse_and(preds::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprBin(mlc::String("||"), expr, right.expr));
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
preds::ExprResult right = parse_cmp(preds::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprBin(mlc::String("&&"), expr, right.expr));
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
preds::ExprResult right = parse_add(preds::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprBin(preds::TKind_op_val(kind), expr, right.expr));
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
preds::ExprResult right = parse_mul(preds::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprBin(preds::TKind_op_val(kind), expr, right.expr));
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
preds::ExprResult right = parse_unary(preds::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprBin(preds::TKind_op_val(kind), expr, right.expr));
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
  preds::ExprResult inner = parse_unary(preds::Parser_advance(parser));
  return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprUn(preds::TKind_op_val(kind), inner.expr)), inner.parser};
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
mlc::String field_name = preds::TKind_ident(preds::Parser_kind(preds::Parser_advance(state)));
if (preds::TKind_is_lparen(preds::Parser_kind(preds::Parser_advance_by(state, 2)))){
preds::ExprsResult margs = parse_arg_list(preds::Parser_advance_by(state, 3));
expr = std::make_shared<ast::Expr>(ast::ExprMethod(expr, field_name, margs.exprs));
state = margs.parser;
} else {
expr = std::make_shared<ast::Expr>(ast::ExprField(expr, field_name));
state = preds::Parser_advance_by(state, 2);
}
}
} else {
{
if (preds::TKind_is_lparen(kind)){
preds::ExprsResult call_args = parse_arg_list(preds::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprCall(expr, call_args.exprs));
state = call_args.parser;
} else {
if (preds::TKind_is_lbracket(kind)){
preds::ExprResult index = parse_expr(preds::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprIndex(expr, index.expr));
state = preds::Parser_advance(index.parser);
} else {
if (preds::TKind_is_question(kind)){
expr = std::make_shared<ast::Expr>(ast::ExprQuestion(expr));
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

preds::ExprResult parse_primary(preds::Parser parser) noexcept{
ast_tokens::TKind kind = preds::Parser_kind(parser);
return preds::TKind_is_int(kind) ? preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprInt(preds::TKind_int_val(kind))), preds::Parser_advance(parser)} : preds::TKind_is_str(kind) ? preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprStr(preds::TKind_str_val(kind))), preds::Parser_advance(parser)} : preds::TKind_is_true(kind) ? preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprBool(true)), preds::Parser_advance(parser)} : preds::TKind_is_false(kind) ? preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprBool(false)), preds::Parser_advance(parser)} : preds::TKind_is_lparen(kind) ? preds::TKind_is_rparen(preds::Parser_kind(preds::Parser_advance(parser))) ? preds::TKind_is_fat_arrow(preds::Parser_kind(preds::Parser_advance_by(parser, 2))) ? [&]() -> preds::ExprResult { 
  mlc::Array<mlc::String> params = {};
  preds::ExprResult body = parse_expr(preds::Parser_advance_by(parser, 3));
  return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprLambda(params, body.expr)), body.parser};
 }() : preds::ExprResult{std::make_shared<ast::Expr>((ast::ExprUnit{})), preds::Parser_advance_by(parser, 2)} : looks_like_lambda_params(preds::Parser_advance(parser)) ? [&]() -> preds::ExprResult { 
  preds::NamesResult param_result = parse_lambda_params(preds::Parser_advance(parser));
  preds::ExprResult body = parse_expr(preds::Parser_advance(param_result.parser));
  return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprLambda(param_result.exprs, body.expr)), body.parser};
 }() : [&]() -> preds::ExprResult { 
  preds::ExprResult inner = parse_expr(preds::Parser_advance(parser));
  return preds::ExprResult{inner.expr, preds::Parser_advance(inner.parser)};
 }() : preds::TKind_is_lbracket(kind) ? parse_array_lit(preds::Parser_advance(parser)) : preds::TKind_is_if(kind) || preds::TKind_is_unless(kind) ? parse_if_expr(parser) : preds::TKind_is_do(kind) ? parse_block(preds::Parser_advance(parser)) : preds::TKind_is_while(kind) ? parse_while_expr(preds::Parser_advance(parser)) : preds::TKind_is_for(kind) ? parse_for_expr(preds::Parser_advance(parser)) : preds::TKind_is_match(kind) ? parse_match_expr(preds::Parser_advance(parser)) : preds::TKind_is_return(kind) ? [&]() -> preds::ExprResult { 
  preds::ExprResult value = parse_expr(preds::Parser_advance(parser));
  mlc::Array<std::shared_ptr<ast::Stmt>> stmts = {};
  stmts.push_back(std::make_shared<ast::Stmt>(ast::StmtReturn(value.expr)));
  return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprBlock(stmts, std::make_shared<ast::Expr>((ast::ExprUnit{})))), value.parser};
 }() : preds::TKind_is_ident(kind) ? [&]() -> preds::ExprResult { 
  mlc::String name = preds::TKind_ident(kind);
  return preds::TKind_is_fat_arrow(preds::Parser_kind(preds::Parser_advance(parser))) ? [&]() -> preds::ExprResult { 
  mlc::Array<mlc::String> params = mlc::Array<mlc::String>{name};
  preds::ExprResult body = parse_expr(preds::Parser_advance_by(parser, 2));
  return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprLambda(params, body.expr)), body.parser};
 }() : preds::is_ctor_name(name) && preds::TKind_is_lbrace(preds::Parser_kind(preds::Parser_advance(parser))) ? parse_record_expr(preds::Parser_advance_by(parser, 2), name) : preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprIdent(name)), preds::Parser_advance(parser)};
 }() : preds::ExprResult{std::make_shared<ast::Expr>((ast::ExprUnit{})), preds::Parser_advance(parser)};
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

preds::ExprResult parse_array_lit(preds::Parser parser) noexcept{
mlc::Array<std::shared_ptr<ast::Expr>> exprs = {};
return preds::TKind_is_rbracket(preds::Parser_kind(parser)) ? preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprArray(exprs)), preds::Parser_advance(parser)} : [&]() -> preds::ExprResult { 
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
  return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprArray(exprs)), preds::Parser_advance(state)};
 }();
}

preds::ExprResult parse_if_expr(preds::Parser parser) noexcept{
bool is_negated = preds::TKind_is_unless(preds::Parser_kind(parser));
preds::ExprResult cond_result = parse_expr(preds::Parser_advance(parser));
std::shared_ptr<ast::Expr> condition = is_negated ? std::make_shared<ast::Expr>(ast::ExprUn(mlc::String("!"), cond_result.expr)) : cond_result.expr;
preds::StmtsResult then_result = parse_stmts_until_else_end(preds::Parser_advance(cond_result.parser));
std::shared_ptr<ast::Expr> then_expr = StmtsResult_to_block_expr(then_result);
return preds::TKind_is_else(preds::Parser_kind(then_result.parser)) ? [&]() -> preds::ExprResult { 
  preds::Parser after_else = preds::Parser_advance(then_result.parser);
  return preds::TKind_is_if(preds::Parser_kind(after_else)) || preds::TKind_is_unless(preds::Parser_kind(after_else)) ? [&]() -> preds::ExprResult { 
  preds::ExprResult else_result = parse_if_expr(after_else);
  return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprIf(condition, then_expr, else_result.expr)), else_result.parser};
 }() : [&]() -> preds::ExprResult { 
  preds::StmtsResult else_stmts = parse_stmts_until_end(after_else);
  return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprIf(condition, then_expr, StmtsResult_to_block_expr(else_stmts))), else_stmts.parser};
 }();
 }() : preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprIf(condition, then_expr, std::make_shared<ast::Expr>((ast::ExprUnit{})))), preds::Parser_advance(then_result.parser)};
}

preds::ExprResult parse_block(preds::Parser parser) noexcept{
preds::StmtsResult result = parse_stmts_until_end(parser);
return preds::ExprResult{StmtsResult_to_block_expr(result), result.parser};
}

bool is_unit_expr(std::shared_ptr<ast::Expr> expr) noexcept{return [&]() { if (std::holds_alternative<ast::ExprUnit>((*expr)._)) {  return true; } if (std::holds_alternative<ast::ExprBlock>((*expr)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expr)._); auto [_w0, result] = _v_exprblock; return is_unit_expr(result); } if (std::holds_alternative<ast::ExprIf>((*expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expr)._); auto [_w0, _w1, else_expr] = _v_exprif; return is_unit_expr(else_expr); } return false; }();}

std::shared_ptr<ast::Expr> block_result(mlc::Array<std::shared_ptr<ast::Stmt>> stmts) noexcept{return stmts.size() == 0 ? std::make_shared<ast::Expr>((ast::ExprUnit{})) : [&]() { if (std::holds_alternative<ast::StmtExpr>((*stmts[stmts.size() - 1])._)) { auto _v_stmtexpr = std::get<ast::StmtExpr>((*stmts[stmts.size() - 1])._); auto [expr] = _v_stmtexpr; return is_unit_expr(expr) ? std::make_shared<ast::Expr>((ast::ExprUnit{})) : expr; } return std::make_shared<ast::Expr>((ast::ExprUnit{})); }();}

mlc::Array<std::shared_ptr<ast::Stmt>> block_body(mlc::Array<std::shared_ptr<ast::Stmt>> stmts) noexcept{
return stmts.size() == 0 ? stmts : [&]() -> mlc::Array<std::shared_ptr<ast::Stmt>> { if (std::holds_alternative<ast::StmtExpr>((*stmts[stmts.size() - 1])._)) { auto _v_stmtexpr = std::get<ast::StmtExpr>((*stmts[stmts.size() - 1])._); auto [expr] = _v_stmtexpr; return is_unit_expr(expr) ? stmts : [&]() -> mlc::Array<std::shared_ptr<ast::Stmt>> { 
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

std::shared_ptr<ast::Expr> StmtsResult_to_block_expr(preds::StmtsResult self) noexcept{return std::make_shared<ast::Expr>(ast::ExprBlock(block_body(self.stmts), block_result(self.stmts)));}

preds::ExprResult parse_while_expr(preds::Parser parser) noexcept{
preds::ExprResult cond = parse_expr(parser);
preds::StmtsResult body = parse_stmts_until_end(preds::Parser_advance(cond.parser));
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprWhile(cond.expr, body.stmts)), body.parser};
}

preds::ExprResult parse_for_expr(preds::Parser parser) noexcept{
mlc::String var_name = preds::TKind_ident(preds::Parser_kind(parser));
preds::ExprResult iter = parse_expr(preds::Parser_advance_by(parser, 2));
preds::StmtsResult body = parse_stmts_until_end(preds::Parser_advance(iter.parser));
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprFor(var_name, iter.expr, body.stmts)), body.parser};
}

preds::ExprResult parse_match_expr(preds::Parser parser) noexcept{
preds::ExprResult subject = parse_expr(parser);
ast_tokens::TKind next = preds::Parser_kind(subject.parser);
return preds::TKind_is_lbrace(next) ? [&]() -> preds::ExprResult { 
  preds::ArmsResult arms = parse_arms_brace(preds::Parser_advance(subject.parser));
  return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprMatch(subject.expr, arms.arms)), arms.parser};
 }() : [&]() -> preds::ExprResult { 
  preds::ArmsResult arms = parse_arms_pipe(subject.parser);
  return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprMatch(subject.expr, arms.arms)), arms.parser};
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
field_vals.push_back(std::make_shared<ast::FieldVal>(ast::FieldVal{field_name, std::make_shared<ast::Expr>(ast::ExprIdent(field_name))}));
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

preds::ExprResult parse_record_expr(preds::Parser parser, mlc::String record_name) noexcept{return preds::TKind_is_spread(preds::Parser_kind(parser)) ? [&]() -> preds::ExprResult { 
  preds::ExprResult base_result = parse_expr(preds::Parser_advance(parser));
  preds::Parser after_base = base_result.parser;
  if (preds::TKind_is_comma(preds::Parser_kind(after_base))){
{
after_base = preds::Parser_advance(after_base);
}
}
  preds::FieldValsResult fields_result = parse_record_fields(after_base);
  return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprRecordUpdate(record_name, base_result.expr, fields_result.field_vals)), fields_result.parser};
 }() : [&]() -> preds::ExprResult { 
  preds::FieldValsResult fields_result = parse_record_fields(parser);
  return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprRecord(record_name, fields_result.field_vals)), fields_result.parser};
 }();}

} // namespace exprs
