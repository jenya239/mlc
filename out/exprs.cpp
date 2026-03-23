#define main mlc_user_main
#include "exprs.hpp"

namespace exprs {

preds::StmtsResult parse_stmts_until_end(preds::Parser parser) noexcept{
auto stmts = mlc::Array<std::shared_ptr<ast::Stmt>>{};
auto state = parser;
while (((!preds::TKind_is_end(preds::Parser_kind(state))) && (!preds::Parser_at_eof(state)))) {
auto result = parse_stmt(state);
stmts.push_back(result.stmt);
state = preds::Parser_skip_semi(result.parser);
}
return preds::StmtsResult{stmts, preds::Parser_advance(state)};
}
preds::StmtsResult parse_stmts_until_else_end(preds::Parser parser) noexcept{
auto stmts = mlc::Array<std::shared_ptr<ast::Stmt>>{};
auto state = parser;
while ((((!preds::TKind_is_end(preds::Parser_kind(state))) && (!preds::TKind_is_else(preds::Parser_kind(state)))) && (!preds::Parser_at_eof(state)))) {
auto result = parse_stmt(state);
stmts.push_back(result.stmt);
state = result.parser;
}
return preds::StmtsResult{stmts, state};
}
preds::StmtResult parse_stmt(preds::Parser parser) noexcept{
auto kind = preds::Parser_kind(parser);
if (preds::TKind_is_let(kind)) {
auto after_let = preds::Parser_advance(parser);
auto is_mut = preds::TKind_is_mut(preds::Parser_kind(after_let));
auto name_pos = (is_mut ? preds::Parser_advance(after_let) : after_let);
auto var_name = preds::TKind_ident(preds::Parser_kind(name_pos));
auto type_result = (preds::TKind_is_colon(preds::Parser_kind(preds::Parser_advance(name_pos))) ? types::parse_type(preds::Parser_advance_by(name_pos, 2)) : preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyUnit{}), preds::Parser_advance(name_pos)});
auto value_result = parse_expr(preds::Parser_advance(type_result.parser));
return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtLet{var_name, is_mut, type_result.type_expr, value_result.expr}), value_result.parser};
} else if (preds::TKind_is_const(kind)) {
auto name_pos = preds::Parser_advance(parser);
auto var_name = preds::TKind_ident(preds::Parser_kind(name_pos));
auto type_result = (preds::TKind_is_colon(preds::Parser_kind(preds::Parser_advance(name_pos))) ? types::parse_type(preds::Parser_advance_by(name_pos, 2)) : preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyUnit{}), preds::Parser_advance(name_pos)});
auto value_result = parse_expr(preds::Parser_advance(type_result.parser));
return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtLet{var_name, false, type_result.type_expr, value_result.expr}), value_result.parser};
} else if (preds::TKind_is_break(kind)) {
return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtBreak{}), preds::Parser_advance(parser)};
} else if (preds::TKind_is_continue(kind)) {
return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtContinue{}), preds::Parser_advance(parser)};
} else if (preds::TKind_is_return(kind)) {
auto value_result = parse_expr(preds::Parser_advance(parser));
return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtReturn{value_result.expr}), value_result.parser};
} else if (preds::TKind_is_while(kind)) {
auto while_result = parse_while_expr(preds::Parser_advance(parser));
return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtExpr{while_result.expr}), while_result.parser};
} else if (preds::TKind_is_for(kind)) {
auto for_result = parse_for_expr(preds::Parser_advance(parser));
return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtExpr{for_result.expr}), for_result.parser};
} else if (preds::TKind_is_do(kind)) {
auto block_result = parse_block(preds::Parser_advance(parser));
return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtExpr{block_result.expr}), block_result.parser};
} else {
auto expr_result = parse_expr(parser);
if (preds::TKind_is_equal(preds::Parser_kind(expr_result.parser))) {
auto rhs = parse_expr(preds::Parser_advance(expr_result.parser));
return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtExpr{std::make_shared<ast::Expr>(ast::ExprBin{mlc::String("=", 1), expr_result.expr, rhs.expr})}), rhs.parser};
} else {
return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtExpr{expr_result.expr}), expr_result.parser};
}
}







}
preds::PatResult parse_pat(preds::Parser parser) noexcept{
auto kind = preds::Parser_kind(parser);
if (preds::TKind_is_ident(kind)) {
auto name = preds::TKind_ident(kind);
if ((name == mlc::String("_", 1))) {
return preds::PatResult{std::make_shared<ast::Pat>(ast::PatWild{}), preds::Parser_advance(parser)};
} else if (preds::is_ctor_name(name)) {
auto after = preds::Parser_advance(parser);
if (preds::TKind_is_lparen(preds::Parser_kind(after))) {
auto sub_pats = parse_pat_args(preds::Parser_advance(after));
return preds::PatResult{std::make_shared<ast::Pat>(ast::PatCtor{name, sub_pats.pats}), sub_pats.parser};
} else if (preds::TKind_is_lbrace(preds::Parser_kind(after))) {
auto field_result = parse_record_pat_fields(preds::Parser_advance(after));
return preds::PatResult{std::make_shared<ast::Pat>(ast::PatRecord{name, field_result.pats}), field_result.parser};
} else {
return preds::PatResult{std::make_shared<ast::Pat>(ast::PatCtor{name, {}}), after};
}

} else {
return preds::PatResult{std::make_shared<ast::Pat>(ast::PatIdent{name}), preds::Parser_advance(parser)};
}

} else if (preds::TKind_is_int(kind)) {
return preds::PatResult{std::make_shared<ast::Pat>(ast::PatInt{preds::TKind_int_val(kind)}), preds::Parser_advance(parser)};
} else if (preds::TKind_is_str(kind)) {
return preds::PatResult{std::make_shared<ast::Pat>(ast::PatStr{preds::TKind_str_val(kind)}), preds::Parser_advance(parser)};
} else if (preds::TKind_is_true(kind)) {
return preds::PatResult{std::make_shared<ast::Pat>(ast::PatBool{true}), preds::Parser_advance(parser)};
} else if (preds::TKind_is_false(kind)) {
return preds::PatResult{std::make_shared<ast::Pat>(ast::PatBool{false}), preds::Parser_advance(parser)};
} else {
return preds::PatResult{std::make_shared<ast::Pat>(ast::PatWild{}), preds::Parser_advance(parser)};
}




}
preds::PatsResult parse_pat_args(preds::Parser parser) noexcept{
auto pats = mlc::Array<std::shared_ptr<ast::Pat>>{};
if (preds::TKind_is_rparen(preds::Parser_kind(parser))) {
return preds::PatsResult{pats, preds::Parser_advance(parser)};
} else {
auto first = parse_pat(parser);
pats.push_back(first.pat);
auto state = first.parser;
while (preds::TKind_is_comma(preds::Parser_kind(state))) {
auto next = parse_pat(preds::Parser_advance(state));
pats.push_back(next.pat);
state = next.parser;
}
return preds::PatsResult{pats, preds::Parser_advance(state)};
}
}
preds::PatsResult parse_record_pat_fields(preds::Parser parser) noexcept{
auto pats = mlc::Array<std::shared_ptr<ast::Pat>>{};
auto state = parser;
while (((!preds::TKind_is_rbrace(preds::Parser_kind(state))) && (!preds::Parser_at_eof(state)))) {
if (preds::TKind_is_ident(preds::Parser_kind(state))) {
pats.push_back(std::make_shared<ast::Pat>(ast::PatIdent{preds::TKind_ident(preds::Parser_kind(state))}));
}
state = preds::Parser_advance(state);
if (preds::TKind_is_comma(preds::Parser_kind(state))) {
state = preds::Parser_advance(state);
}
}
return preds::PatsResult{pats, preds::Parser_advance(state)};
}
preds::ExprResult parse_expr(preds::Parser parser) noexcept{
return parse_pipe(parser);
}
std::shared_ptr<ast::Expr> pipe_desugar(std::shared_ptr<ast::Expr> left_expr, std::shared_ptr<ast::Expr> right_expr) noexcept{
return std::visit(overloaded{[&](const ast::ExprCall& exprCall) { auto [callee, existing_args] = exprCall; return [&]() {
auto new_args = mlc::Array{left_expr};
auto i = 0;
while ((i < existing_args.length())) {
new_args.push_back(existing_args[i]);
i = (i + 1);
}
return std::make_shared<ast::Expr>(ast::ExprCall{callee, new_args});
}(); },
[&](const auto& __v) { return [&]() {
auto call_args = mlc::Array{left_expr};
return std::make_shared<ast::Expr>(ast::ExprCall{right_expr, call_args});
}(); }
}, (*right_expr));
}
preds::ExprResult parse_pipe(preds::Parser parser) noexcept{
auto left = parse_or(parser);
auto expr = left.expr;
auto state = left.parser;
while (preds::TKind_is_pipe(preds::Parser_kind(state))) {
auto right = parse_or(preds::Parser_advance(state));
expr = pipe_desugar(expr, right.expr);
state = right.parser;
}
return preds::ExprResult{expr, state};
}
preds::ExprResult parse_or(preds::Parser parser) noexcept{
auto left = parse_and(parser);
auto expr = left.expr;
auto state = left.parser;
while ((preds::TKind_is_op(preds::Parser_kind(state)) && (preds::TKind_op_val(preds::Parser_kind(state)) == mlc::String("||", 2)))) {
auto right = parse_and(preds::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprBin{mlc::String("||", 2), expr, right.expr});
state = right.parser;
}
return preds::ExprResult{expr, state};
}
preds::ExprResult parse_and(preds::Parser parser) noexcept{
auto left = parse_cmp(parser);
auto expr = left.expr;
auto state = left.parser;
while ((preds::TKind_is_op(preds::Parser_kind(state)) && (preds::TKind_op_val(preds::Parser_kind(state)) == mlc::String("&&", 2)))) {
auto right = parse_cmp(preds::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprBin{mlc::String("&&", 2), expr, right.expr});
state = right.parser;
}
return preds::ExprResult{expr, state};
}
bool is_cmp_op(mlc::String op) noexcept{
return ((((((op == mlc::String("==", 2)) || (op == mlc::String("!=", 2))) || (op == mlc::String("<", 1))) || (op == mlc::String(">", 1))) || (op == mlc::String("<=", 2))) || (op == mlc::String(">=", 2)));
}
preds::ExprResult parse_cmp(preds::Parser parser) noexcept{
auto left = parse_add(parser);
auto expr = left.expr;
auto state = left.parser;
auto kind = preds::Parser_kind(state);
if ((preds::TKind_is_op(kind) && is_cmp_op(preds::TKind_op_val(kind)))) {
auto right = parse_add(preds::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprBin{preds::TKind_op_val(kind), expr, right.expr});
state = right.parser;
}
return preds::ExprResult{expr, state};
}
preds::ExprResult parse_add(preds::Parser parser) noexcept{
auto left = parse_mul(parser);
auto expr = left.expr;
auto state = left.parser;
auto go = true;
while (go) {
auto kind = preds::Parser_kind(state);
if ((preds::TKind_is_op(kind) && ((preds::TKind_op_val(kind) == mlc::String("+", 1)) || (preds::TKind_op_val(kind) == mlc::String("-", 1))))) {
auto right = parse_mul(preds::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprBin{preds::TKind_op_val(kind), expr, right.expr});
state = right.parser;
} else {
go = false;
}
}
return preds::ExprResult{expr, state};
}
preds::ExprResult parse_mul(preds::Parser parser) noexcept{
auto left = parse_unary(parser);
auto expr = left.expr;
auto state = left.parser;
auto go = true;
while (go) {
auto kind = preds::Parser_kind(state);
if ((preds::TKind_is_op(kind) && (((preds::TKind_op_val(kind) == mlc::String("*", 1)) || (preds::TKind_op_val(kind) == mlc::String("/", 1))) || (preds::TKind_op_val(kind) == mlc::String("%", 1))))) {
auto right = parse_unary(preds::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprBin{preds::TKind_op_val(kind), expr, right.expr});
state = right.parser;
} else {
go = false;
}
}
return preds::ExprResult{expr, state};
}
preds::ExprResult parse_unary(preds::Parser parser) noexcept{
auto kind = preds::Parser_kind(parser);
if ((preds::TKind_is_op(kind) && ((preds::TKind_op_val(kind) == mlc::String("!", 1)) || (preds::TKind_op_val(kind) == mlc::String("-", 1))))) {
auto inner = parse_unary(preds::Parser_advance(parser));
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprUn{preds::TKind_op_val(kind), inner.expr}), inner.parser};
} else {
return parse_postfix(parser);
}
}
preds::ExprResult parse_postfix(preds::Parser parser) noexcept{
auto base = parse_primary(parser);
auto expr = base.expr;
auto state = base.parser;
auto go = true;
while (go) {
auto kind = preds::Parser_kind(state);
if (preds::TKind_is_dot(kind)) {
auto field_name = preds::TKind_ident(preds::Parser_kind(preds::Parser_advance(state)));
if (preds::TKind_is_lparen(preds::Parser_kind(preds::Parser_advance_by(state, 2)))) {
auto margs = parse_arg_list(preds::Parser_advance_by(state, 3));
expr = std::make_shared<ast::Expr>(ast::ExprMethod{expr, field_name, margs.exprs});
state = margs.parser;
} else {
expr = std::make_shared<ast::Expr>(ast::ExprField{expr, field_name});
state = preds::Parser_advance_by(state, 2);
}
} else if (preds::TKind_is_lparen(kind)) {
auto call_args = parse_arg_list(preds::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprCall{expr, call_args.exprs});
state = call_args.parser;
} else if (preds::TKind_is_lbracket(kind)) {
auto index = parse_expr(preds::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprIndex{expr, index.expr});
state = preds::Parser_advance(index.parser);
} else if (preds::TKind_is_question(kind)) {
expr = std::make_shared<ast::Expr>(ast::ExprQuestion{expr});
state = preds::Parser_advance(state);
} else {
go = false;
}



}
return preds::ExprResult{expr, state};
}
preds::ExprsResult parse_arg_list(preds::Parser parser) noexcept{
auto exprs = mlc::Array<std::shared_ptr<ast::Expr>>{};
if (preds::TKind_is_rparen(preds::Parser_kind(parser))) {
return preds::ExprsResult{exprs, preds::Parser_advance(parser)};
} else {
auto first = parse_expr(parser);
exprs.push_back(first.expr);
auto state = first.parser;
while (preds::TKind_is_comma(preds::Parser_kind(state))) {
auto next = parse_expr(preds::Parser_advance(state));
exprs.push_back(next.expr);
state = next.parser;
}
return preds::ExprsResult{exprs, preds::Parser_advance(state)};
}
}
std::shared_ptr<ast::Expr> fstr_to_string_expr(mlc::String part) noexcept{
return std::make_shared<ast::Expr>(ast::ExprMethod{std::make_shared<ast::Expr>(ast::ExprIdent{part}), mlc::String("to_string", 9), {}});
}
std::shared_ptr<ast::Expr> build_fstr_part_expr(mlc::String part, bool is_literal) noexcept{
if (is_literal) {
return std::make_shared<ast::Expr>(ast::ExprStr{part});
} else if ((part.length() == 0)) {
return std::make_shared<ast::Expr>(ast::ExprStr{part});
} else {
return fstr_to_string_expr(part);
}

}
std::shared_ptr<ast::Expr> build_fstr_expr(mlc::Array<mlc::String> parts) noexcept{
auto result = std::make_shared<ast::Expr>(ast::ExprStr{mlc::String("", 0)});
auto pi = 0;
while ((pi < parts.length())) {
auto part_expr = build_fstr_part_expr(parts[pi], ((pi % 2) == 0));
if ((pi == 0)) {
result = part_expr;
} else {
result = std::make_shared<ast::Expr>(ast::ExprBin{mlc::String("+", 1), result, part_expr});
}
pi = (pi + 1);
}
return result;
}
preds::ExprResult parse_primary(preds::Parser parser) noexcept{
auto kind = preds::Parser_kind(parser);
if (preds::TKind_is_int(kind)) {
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprInt{preds::TKind_int_val(kind)}), preds::Parser_advance(parser)};
} else if (preds::TKind_is_str(kind)) {
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprStr{preds::TKind_str_val(kind)}), preds::Parser_advance(parser)};
} else if (preds::TKind_is_fstr(kind)) {
return preds::ExprResult{build_fstr_expr(preds::TKind_fstr_parts(kind)), preds::Parser_advance(parser)};
} else if (preds::TKind_is_true(kind)) {
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprBool{true}), preds::Parser_advance(parser)};
} else if (preds::TKind_is_false(kind)) {
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprBool{false}), preds::Parser_advance(parser)};
} else if (preds::TKind_is_lparen(kind)) {
if (preds::TKind_is_rparen(preds::Parser_kind(preds::Parser_advance(parser)))) {
if (preds::TKind_is_fat_arrow(preds::Parser_kind(preds::Parser_advance_by(parser, 2)))) {
auto params = mlc::Array<mlc::String>{};
auto body = parse_expr(preds::Parser_advance_by(parser, 3));
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprLambda{params, body.expr}), body.parser};
} else {
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprUnit{}), preds::Parser_advance_by(parser, 2)};
}
} else if (looks_like_lambda_params(preds::Parser_advance(parser))) {
auto param_result = parse_lambda_params(preds::Parser_advance(parser));
auto body = parse_expr(preds::Parser_advance(param_result.parser));
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprLambda{param_result.exprs, body.expr}), body.parser};
} else {
auto inner = parse_expr(preds::Parser_advance(parser));
return preds::ExprResult{inner.expr, preds::Parser_advance(inner.parser)};
}

} else if (preds::TKind_is_lbracket(kind)) {
return parse_array_lit(preds::Parser_advance(parser));
} else if ((preds::TKind_is_if(kind) || preds::TKind_is_unless(kind))) {
return parse_if_expr(parser);
} else if (preds::TKind_is_do(kind)) {
return parse_block(preds::Parser_advance(parser));
} else if (preds::TKind_is_while(kind)) {
return parse_while_expr(preds::Parser_advance(parser));
} else if (preds::TKind_is_for(kind)) {
return parse_for_expr(preds::Parser_advance(parser));
} else if (preds::TKind_is_match(kind)) {
return parse_match_expr(preds::Parser_advance(parser));
} else if (preds::TKind_is_return(kind)) {
auto value = parse_expr(preds::Parser_advance(parser));
auto stmts = mlc::Array<std::shared_ptr<ast::Stmt>>{};
stmts.push_back(std::make_shared<ast::Stmt>(ast::StmtReturn{value.expr}));
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprBlock{stmts, std::make_shared<ast::Expr>(ast::ExprUnit{})}), value.parser};
} else if (preds::TKind_is_ident(kind)) {
auto name = preds::TKind_ident(kind);
if (preds::TKind_is_fat_arrow(preds::Parser_kind(preds::Parser_advance(parser)))) {
auto params = mlc::Array{name};
auto body = parse_expr(preds::Parser_advance_by(parser, 2));
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprLambda{params, body.expr}), body.parser};
} else if ((preds::is_ctor_name(name) && preds::TKind_is_lbrace(preds::Parser_kind(preds::Parser_advance(parser))))) {
return parse_record_expr(preds::Parser_advance_by(parser, 2), name);
} else {
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprIdent{name}), preds::Parser_advance(parser)};
}

} else {
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprUnit{}), preds::Parser_advance(parser)};
}













}
bool looks_like_lambda_params(preds::Parser parser) noexcept{
auto state = parser;
auto ok = true;
if ((!preds::TKind_is_ident(preds::Parser_kind(state)))) {
ok = false;
} else {
state = preds::Parser_advance(state);
while (preds::TKind_is_comma(preds::Parser_kind(state))) {
state = preds::Parser_advance(state);
if ((!preds::TKind_is_ident(preds::Parser_kind(state)))) {
ok = false;
state = preds::Parser_advance(state);
} else {
state = preds::Parser_advance(state);
}
}
if ((!preds::TKind_is_rparen(preds::Parser_kind(state)))) {
ok = false;
} else {
ok = preds::TKind_is_fat_arrow(preds::Parser_kind(preds::Parser_advance(state)));
}
}
return ok;
}
preds::NamesResult parse_lambda_params(preds::Parser parser) noexcept{
auto names = mlc::Array<mlc::String>{};
auto state = parser;
names.push_back(preds::TKind_ident(preds::Parser_kind(state)));
state = preds::Parser_advance(state);
while (preds::TKind_is_comma(preds::Parser_kind(state))) {
state = preds::Parser_advance(state);
names.push_back(preds::TKind_ident(preds::Parser_kind(state)));
state = preds::Parser_advance(state);
}
return preds::NamesResult{names, preds::Parser_advance(state)};
}
preds::ExprResult parse_array_lit(preds::Parser parser) noexcept{
auto exprs = mlc::Array<std::shared_ptr<ast::Expr>>{};
if (preds::TKind_is_rbracket(preds::Parser_kind(parser))) {
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprArray{exprs}), preds::Parser_advance(parser)};
} else {
auto first = parse_expr(parser);
exprs.push_back(first.expr);
auto state = first.parser;
while (preds::TKind_is_comma(preds::Parser_kind(state))) {
auto next = parse_expr(preds::Parser_advance(state));
exprs.push_back(next.expr);
state = next.parser;
}
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprArray{exprs}), preds::Parser_advance(state)};
}
}
preds::ExprResult parse_if_expr(preds::Parser parser) noexcept{
auto is_negated = preds::TKind_is_unless(preds::Parser_kind(parser));
auto cond_result = parse_expr(preds::Parser_advance(parser));
auto condition = (is_negated ? std::make_shared<ast::Expr>(ast::ExprUn{mlc::String("!", 1), cond_result.expr}) : cond_result.expr);
auto then_result = parse_stmts_until_else_end(preds::Parser_advance(cond_result.parser));
auto then_expr = StmtsResult_to_block_expr(then_result);
if (preds::TKind_is_else(preds::Parser_kind(then_result.parser))) {
auto after_else = preds::Parser_advance(then_result.parser);
if ((preds::TKind_is_if(preds::Parser_kind(after_else)) || preds::TKind_is_unless(preds::Parser_kind(after_else)))) {
auto else_result = parse_if_expr(after_else);
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprIf{condition, then_expr, else_result.expr}), else_result.parser};
} else {
auto else_stmts = parse_stmts_until_end(after_else);
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprIf{condition, then_expr, StmtsResult_to_block_expr(else_stmts)}), else_stmts.parser};
}
} else {
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprIf{condition, then_expr, std::make_shared<ast::Expr>(ast::ExprUnit{})}), preds::Parser_advance(then_result.parser)};
}
}
preds::ExprResult parse_block(preds::Parser parser) noexcept{
auto result = parse_stmts_until_end(parser);
return preds::ExprResult{StmtsResult_to_block_expr(result), result.parser};
}
bool is_unit_expr(std::shared_ptr<ast::Expr> expr) noexcept{
return std::visit(overloaded{[&](const ast::ExprUnit& exprUnit) { return true; },
[&](const ast::ExprBlock& exprBlock) { auto [__0, result] = exprBlock; return is_unit_expr(result); },
[&](const ast::ExprIf& exprIf) { auto [__0, __1, else_expr] = exprIf; return is_unit_expr(else_expr); },
[&](const ast::ExprBin& exprBin) { auto [op, __1, __2] = exprBin; return (op == mlc::String("=", 1)); },
[&](const auto& __v) { return false; }
}, (*expr));
}
std::shared_ptr<ast::Expr> block_result(mlc::Array<std::shared_ptr<ast::Stmt>> stmts) noexcept{
if ((stmts.length() == 0)) {
return std::make_shared<ast::Expr>(ast::ExprUnit{});
} else {
return std::visit(overloaded{[&](const ast::StmtExpr& stmtExpr) { auto [expr] = stmtExpr; return (is_unit_expr(expr) ? std::make_shared<ast::Expr>(ast::ExprUnit{}) : expr); },
[&](const auto& __v) { return std::make_shared<ast::Expr>(ast::ExprUnit{}); }
}, (*stmts[(stmts.length() - 1)]));
}
}
mlc::Array<std::shared_ptr<ast::Stmt>> block_body(mlc::Array<std::shared_ptr<ast::Stmt>> stmts) noexcept{
if ((stmts.length() == 0)) {
return stmts;
} else {
return std::visit(overloaded{[&](const ast::StmtExpr& stmtExpr) { auto [expr] = stmtExpr; return (is_unit_expr(expr) ? stmts : [&]() {
auto result = mlc::Array<std::shared_ptr<ast::Stmt>>{};
auto i = 0;
while ((i < (stmts.length() - 1))) {
result.push_back(stmts[i]);
i = (i + 1);
}
return result;
}()); },
[&](const auto& __v) { return stmts; }
}, (*stmts[(stmts.length() - 1)]));
}
}
std::shared_ptr<ast::Expr> StmtsResult_to_block_expr(preds::StmtsResult self) noexcept{
return std::make_shared<ast::Expr>(ast::ExprBlock{block_body(self.stmts), block_result(self.stmts)});
}
preds::ExprResult parse_while_expr(preds::Parser parser) noexcept{
auto cond = parse_expr(parser);
auto body = parse_stmts_until_end(preds::Parser_advance(cond.parser));
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprWhile{cond.expr, body.stmts}), body.parser};
}
preds::ExprResult parse_for_expr(preds::Parser parser) noexcept{
auto var_name = preds::TKind_ident(preds::Parser_kind(parser));
auto iter = parse_expr(preds::Parser_advance_by(parser, 2));
auto body = parse_stmts_until_end(preds::Parser_advance(iter.parser));
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprFor{var_name, iter.expr, body.stmts}), body.parser};
}
preds::ExprResult parse_match_expr(preds::Parser parser) noexcept{
auto subject = parse_expr(parser);
auto next = preds::Parser_kind(subject.parser);
if (preds::TKind_is_lbrace(next)) {
auto arms = parse_arms_brace(preds::Parser_advance(subject.parser));
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprMatch{subject.expr, arms.arms}), arms.parser};
} else {
auto arms = parse_arms_pipe(subject.parser);
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprMatch{subject.expr, arms.arms}), arms.parser};
}
}
preds::ArmsResult parse_arms_brace(preds::Parser parser) noexcept{
auto arms = mlc::Array<std::shared_ptr<ast::MatchArm>>{};
auto state = parser;
while (((!preds::TKind_is_rbrace(preds::Parser_kind(state))) && (!preds::Parser_at_eof(state)))) {
if (preds::TKind_is_bar(preds::Parser_kind(state))) {
state = preds::Parser_advance(state);
}
auto pat_result = parse_pat(state);
auto body_result = parse_expr(preds::Parser_advance(pat_result.parser));
arms.push_back(std::make_shared<ast::MatchArm>(ast::MatchArm{pat_result.pat, body_result.expr}));
state = body_result.parser;
if (preds::TKind_is_comma(preds::Parser_kind(state))) {
state = preds::Parser_advance(state);
}
}
return preds::ArmsResult{arms, preds::Parser_advance(state)};
}
preds::ArmsResult parse_arms_pipe(preds::Parser parser) noexcept{
auto arms = mlc::Array<std::shared_ptr<ast::MatchArm>>{};
auto state = parser;
while (preds::TKind_is_bar(preds::Parser_kind(state))) {
auto pat_result = parse_pat(preds::Parser_advance(state));
auto body_result = parse_expr(preds::Parser_advance(pat_result.parser));
arms.push_back(std::make_shared<ast::MatchArm>(ast::MatchArm{pat_result.pat, body_result.expr}));
state = body_result.parser;
}
if (preds::TKind_is_end(preds::Parser_kind(state))) {
state = preds::Parser_advance(state);
}
return preds::ArmsResult{arms, state};
}
preds::ArmsResult parse_arms(preds::Parser parser) noexcept{
return parse_arms_brace(parser);
}
preds::FieldValsResult parse_record_fields(preds::Parser parser) noexcept{
auto field_vals = mlc::Array<std::shared_ptr<ast::FieldVal>>{};
auto state = parser;
while (((!preds::TKind_is_rbrace(preds::Parser_kind(state))) && (!preds::Parser_at_eof(state)))) {
auto field_name = preds::TKind_ident(preds::Parser_kind(state));
auto after_name = preds::Parser_advance(state);
if (preds::TKind_is_colon(preds::Parser_kind(after_name))) {
auto value_result = parse_expr(preds::Parser_advance(after_name));
field_vals.push_back(std::make_shared<ast::FieldVal>(ast::FieldVal{field_name, value_result.expr}));
state = value_result.parser;
} else {
field_vals.push_back(std::make_shared<ast::FieldVal>(ast::FieldVal{field_name, std::make_shared<ast::Expr>(ast::ExprIdent{field_name})}));
state = after_name;
}
if (preds::TKind_is_comma(preds::Parser_kind(state))) {
state = preds::Parser_advance(state);
}
}
return preds::FieldValsResult{field_vals, preds::Parser_advance(state)};
}
preds::ExprResult parse_record_expr(preds::Parser parser, mlc::String record_name) noexcept{
if (preds::TKind_is_spread(preds::Parser_kind(parser))) {
auto base_result = parse_expr(preds::Parser_advance(parser));
auto after_base = base_result.parser;
if (preds::TKind_is_comma(preds::Parser_kind(after_base))) {
after_base = preds::Parser_advance(after_base);
}
auto fields_result = parse_record_fields(after_base);
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprRecordUpdate{record_name, base_result.expr, fields_result.field_vals}), fields_result.parser};
} else {
auto fields_result = parse_record_fields(parser);
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprRecord{record_name, fields_result.field_vals}), fields_result.parser};
}
}

} // namespace exprs
