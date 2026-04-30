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
preds::StmtResult parse_statement_let_const(preds::Parser parser, ast::Span statement_span) noexcept{
auto name_pos = preds::Parser_advance(parser);
auto var_name = preds::TKind_ident(preds::Parser_kind(name_pos));
auto type_result = (preds::TKind_is_colon(preds::Parser_kind(preds::Parser_advance(name_pos))) ? types::parse_type(preds::Parser_advance_by(name_pos, 2)) : preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyUnit{}), preds::Parser_advance(name_pos)});
auto value_result = parse_expr(preds::Parser_advance(type_result.parser));
return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtLetConst{var_name, type_result.type_expr, value_result.expr, statement_span}), value_result.parser};
}
ast_tokens::TKind parser_next_kind(preds::Parser p) noexcept{
if (((p.pos + 1) >= p.tokens.length())) {
return ast_tokens::Eof{};
} else {
return p.tokens[(p.pos + 1)].kind;
}
}
preds::TypeResult parse_after_let_pattern(preds::Parser parser) noexcept{
if (preds::TKind_is_colon(preds::Parser_kind(parser))) {
auto tr = types::parse_type(preds::Parser_advance(parser));
if (preds::TKind_is_equal(preds::Parser_kind(tr.parser))) {
return preds::TypeResult{tr.type_expr, preds::Parser_advance(tr.parser)};
} else {
return preds::TypeResult{tr.type_expr, tr.parser};
}
} else if (preds::TKind_is_equal(preds::Parser_kind(parser))) {
return preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyUnit{}), preds::Parser_advance(parser)};
} else {
return preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyUnit{}), parser};
}

}
void __skip__() noexcept{
return /* unit */;
}
preds::PatResult parse_array_pattern(preds::Parser parser, ast::Span bracket_span) noexcept{
if (preds::TKind_is_rbracket(preds::Parser_kind(parser))) {
return preds::PatResult{std::make_shared<ast::Pat>(ast::PatArray{{}, mlc::String("", 0), bracket_span}), preds::Parser_advance(parser)};
} else {
auto first = parse_pat(parser);
auto pats = mlc::Array<std::shared_ptr<ast::Pat>>{};
pats.push_back(first.pat);
auto st = first.parser;
auto rest = mlc::String("", 0);
while (preds::TKind_is_comma(preds::Parser_kind(st))) {
st = preds::Parser_advance(st);
if (preds::TKind_is_spread(preds::Parser_kind(st))) {
st = preds::Parser_advance(st);
if (preds::TKind_is_ident(preds::Parser_kind(st))) {
rest = preds::TKind_ident(preds::Parser_kind(st));
st = preds::Parser_advance(st);
}
break;
} else {
auto nxt = parse_pat(st);
pats.push_back(nxt.pat);
st = nxt.parser;
}
}
if (preds::TKind_is_rbracket(preds::Parser_kind(st))) {
return preds::PatResult{std::make_shared<ast::Pat>(ast::PatArray{pats, rest, bracket_span}), preds::Parser_advance(st)};
} else {
return preds::PatResult{std::make_shared<ast::Pat>(ast::PatArray{pats, rest, bracket_span}), st};
}
}
}
preds::StmtResult parse_statement_let(preds::Parser parser) noexcept{
auto statement_span = preds::Parser_span_at_cursor(parser);
auto after_let = preds::Parser_advance(parser);
if (preds::TKind_is_const(preds::Parser_kind(after_let))) {
return parse_statement_let_const(after_let, statement_span);
} else {
auto is_mut = preds::TKind_is_mut(preds::Parser_kind(after_let));
auto name_pos = (is_mut ? preds::Parser_advance(after_let) : after_let);
auto k0 = preds::Parser_kind(name_pos);
if (preds::TKind_is_lparen(k0)) {
auto paren_span = preds::Parser_span_at_cursor(name_pos);
auto inner = parse_pat_args(preds::Parser_advance(name_pos));
auto pat = std::make_shared<ast::Pat>(ast::PatTuple{inner.pats, paren_span});
auto type_result = parse_after_let_pattern(inner.parser);
auto value_result = parse_expr(type_result.parser);
auto has_else = preds::TKind_is_else(preds::Parser_kind(value_result.parser));
auto else_result = (has_else ? parse_stmts_until_end(preds::Parser_advance(value_result.parser)) : preds::StmtsResult{{}, value_result.parser});
auto else_body = std::make_shared<ast::Expr>(ast::ExprBlock{else_result.stmts, std::make_shared<ast::Expr>(ast::ExprUnit{statement_span}), statement_span});
return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtLetPat{pat, is_mut, type_result.type_expr, value_result.expr, has_else, else_body, statement_span}), else_result.parser};
} else if (preds::TKind_is_lbracket(k0)) {
auto br_span = preds::Parser_span_at_cursor(name_pos);
auto ar = parse_array_pattern(preds::Parser_advance(name_pos), br_span);
auto type_result = parse_after_let_pattern(ar.parser);
auto value_result = parse_expr(type_result.parser);
auto has_else = preds::TKind_is_else(preds::Parser_kind(value_result.parser));
auto else_result = (has_else ? parse_stmts_until_end(preds::Parser_advance(value_result.parser)) : preds::StmtsResult{{}, value_result.parser});
auto else_body = std::make_shared<ast::Expr>(ast::ExprBlock{else_result.stmts, std::make_shared<ast::Expr>(ast::ExprUnit{statement_span}), statement_span});
return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtLetPat{ar.pat, is_mut, type_result.type_expr, value_result.expr, has_else, else_body, statement_span}), else_result.parser};
} else if (preds::TKind_is_lbrace(k0)) {
auto br_span = preds::Parser_span_at_cursor(name_pos);
auto fr = parse_record_pat_fields(preds::Parser_advance(name_pos));
auto pat = std::make_shared<ast::Pat>(ast::PatRecord{mlc::String("", 0), fr.pats, br_span});
auto type_result = parse_after_let_pattern(fr.parser);
auto value_result = parse_expr(type_result.parser);
auto has_else = preds::TKind_is_else(preds::Parser_kind(value_result.parser));
auto else_result = (has_else ? parse_stmts_until_end(preds::Parser_advance(value_result.parser)) : preds::StmtsResult{{}, value_result.parser});
auto else_body = std::make_shared<ast::Expr>(ast::ExprBlock{else_result.stmts, std::make_shared<ast::Expr>(ast::ExprUnit{statement_span}), statement_span});
return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtLetPat{pat, is_mut, type_result.type_expr, value_result.expr, has_else, else_body, statement_span}), else_result.parser};
} else if (((preds::TKind_is_ident(k0) && preds::is_ctor_name(preds::TKind_ident(k0))) && preds::TKind_is_lparen(parser_next_kind(name_pos)))) {
auto pr = parse_pattern_identifier_branch(name_pos, preds::TKind_ident(k0));
auto type_result = parse_after_let_pattern(pr.parser);
auto value_result = parse_expr(type_result.parser);
auto has_else = preds::TKind_is_else(preds::Parser_kind(value_result.parser));
auto else_result = (has_else ? parse_stmts_until_end(preds::Parser_advance(value_result.parser)) : preds::StmtsResult{{}, value_result.parser});
auto else_body = std::make_shared<ast::Expr>(ast::ExprBlock{else_result.stmts, std::make_shared<ast::Expr>(ast::ExprUnit{statement_span}), statement_span});
return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtLetPat{pr.pat, is_mut, type_result.type_expr, value_result.expr, has_else, else_body, statement_span}), else_result.parser};
} else if (preds::TKind_is_ident(k0)) {
auto var_name = preds::TKind_ident(k0);
auto type_result = (preds::TKind_is_colon(preds::Parser_kind(preds::Parser_advance(name_pos))) ? types::parse_type(preds::Parser_advance_by(name_pos, 2)) : preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyUnit{}), preds::Parser_advance(name_pos)});
auto value_result = parse_expr(preds::Parser_advance(type_result.parser));
return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtLet{var_name, is_mut, type_result.type_expr, value_result.expr, statement_span}), value_result.parser};
} else {
return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtExpr{std::make_shared<ast::Expr>(ast::ExprUnit{preds::Parser_span_at_cursor(name_pos)}), statement_span}), name_pos};
}




}
}
preds::StmtResult parse_statement_const(preds::Parser parser) noexcept{
auto statement_span = preds::Parser_span_at_cursor(parser);
auto name_pos = preds::Parser_advance(parser);
auto var_name = preds::TKind_ident(preds::Parser_kind(name_pos));
auto type_result = (preds::TKind_is_colon(preds::Parser_kind(preds::Parser_advance(name_pos))) ? types::parse_type(preds::Parser_advance_by(name_pos, 2)) : preds::TypeResult{std::make_shared<ast::TypeExpr>(ast::TyUnit{}), preds::Parser_advance(name_pos)});
auto value_result = parse_expr(preds::Parser_advance(type_result.parser));
return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtLet{var_name, false, type_result.type_expr, value_result.expr, statement_span}), value_result.parser};
}
preds::StmtResult parse_statement_return(preds::Parser parser) noexcept{
auto statement_span = preds::Parser_span_at_cursor(parser);
auto value_result = parse_expr(preds::Parser_advance(parser));
return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtReturn{value_result.expr, statement_span}), value_result.parser};
}
preds::StmtResult parse_statement_expression_or_assign(preds::Parser parser) noexcept{
auto statement_span = preds::Parser_span_at_cursor(parser);
auto expr_result = parse_expr(parser);
if (preds::TKind_is_equal(preds::Parser_kind(expr_result.parser))) {
auto assign_span = preds::Parser_span_at_cursor(expr_result.parser);
auto right_hand = parse_expr(preds::Parser_advance(expr_result.parser));
return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtExpr{std::make_shared<ast::Expr>(ast::ExprBin{mlc::String("=", 1), expr_result.expr, right_hand.expr, assign_span}), statement_span}), right_hand.parser};
} else {
return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtExpr{expr_result.expr, statement_span}), expr_result.parser};
}
}
preds::StmtResult parse_stmt(preds::Parser parser) noexcept{
auto kind = preds::Parser_kind(parser);
if (preds::TKind_is_let(kind)) {
return parse_statement_let(parser);
} else if (preds::TKind_is_const(kind)) {
return parse_statement_const(parser);
} else if (preds::TKind_is_break(kind)) {
return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtBreak{preds::Parser_span_at_cursor(parser)}), preds::Parser_advance(parser)};
} else if (preds::TKind_is_continue(kind)) {
return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtContinue{preds::Parser_span_at_cursor(parser)}), preds::Parser_advance(parser)};
} else if (preds::TKind_is_return(kind)) {
return parse_statement_return(parser);
} else if (preds::TKind_is_while(kind)) {
auto header_span = preds::Parser_span_at_cursor(parser);
auto while_result = parse_while_expr(preds::Parser_advance(parser), header_span);
return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtExpr{while_result.expr, header_span}), while_result.parser};
} else if (preds::TKind_is_for(kind)) {
auto header_span = preds::Parser_span_at_cursor(parser);
auto for_result = parse_for_expr(preds::Parser_advance(parser), header_span);
return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtExpr{for_result.expr, header_span}), for_result.parser};
} else if (preds::TKind_is_do(kind)) {
auto header_span = preds::Parser_span_at_cursor(parser);
auto block_result = parse_block(preds::Parser_advance(parser), header_span);
return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtExpr{block_result.expr, header_span}), block_result.parser};
} else if (preds::TKind_is_with(kind)) {
auto header_span = preds::Parser_span_at_cursor(parser);
auto with_result = parse_with_expr(preds::Parser_advance(parser), header_span);
return preds::StmtResult{std::make_shared<ast::Stmt>(ast::StmtExpr{with_result.expr, header_span}), with_result.parser};
} else {
return parse_statement_expression_or_assign(parser);
}








}
preds::PatResult parse_pattern_identifier_branch(preds::Parser parser, mlc::String name) noexcept{
auto pattern_span = preds::Parser_span_at_cursor(parser);
if ((name == mlc::String("_", 1))) {
return preds::PatResult{std::make_shared<ast::Pat>(ast::PatWild{pattern_span}), preds::Parser_advance(parser)};
} else if (preds::is_ctor_name(name)) {
auto after = preds::Parser_advance(parser);
if (preds::TKind_is_lparen(preds::Parser_kind(after))) {
auto sub_patterns = parse_pat_args(preds::Parser_advance(after));
return preds::PatResult{std::make_shared<ast::Pat>(ast::PatCtor{name, sub_patterns.pats, pattern_span}), sub_patterns.parser};
} else if (preds::TKind_is_lbrace(preds::Parser_kind(after))) {
auto field_result = parse_record_pat_fields(preds::Parser_advance(after));
return preds::PatResult{std::make_shared<ast::Pat>(ast::PatRecord{name, field_result.pats, pattern_span}), field_result.parser};
} else {
return preds::PatResult{std::make_shared<ast::Pat>(ast::PatCtor{name, {}, pattern_span}), after};
}

} else {
return preds::PatResult{std::make_shared<ast::Pat>(ast::PatIdent{name, pattern_span}), preds::Parser_advance(parser)};
}

}
preds::PatResult parse_pattern_integer(preds::Parser parser, int value) noexcept{
return preds::PatResult{std::make_shared<ast::Pat>(ast::PatInt{value, preds::Parser_span_at_cursor(parser)}), preds::Parser_advance(parser)};
}
preds::PatResult parse_pattern_string(preds::Parser parser, mlc::String value) noexcept{
return preds::PatResult{std::make_shared<ast::Pat>(ast::PatStr{value, preds::Parser_span_at_cursor(parser)}), preds::Parser_advance(parser)};
}
preds::PatResult parse_pattern_boolean(preds::Parser parser, bool value) noexcept{
return preds::PatResult{std::make_shared<ast::Pat>(ast::PatBool{value, preds::Parser_span_at_cursor(parser)}), preds::Parser_advance(parser)};
}
preds::PatResult parse_pattern_fallback_wildcard(preds::Parser parser) noexcept{
return preds::PatResult{std::make_shared<ast::Pat>(ast::PatWild{preds::Parser_span_at_cursor(parser)}), preds::Parser_advance(parser)};
}
preds::PatResult parse_or_pat(preds::Parser parser) noexcept{
auto first = parse_pat(parser);
auto span = preds::Parser_span_at_cursor(parser);
auto alts = mlc::Array{first.pat};
auto state = first.parser;
while (((preds::TKind_is_bar(preds::Parser_kind(state)) && (!preds::Parser_at_eof(preds::Parser_advance(state)))) && (!preds::TKind_is_fat_arrow(preds::Parser_kind(preds::Parser_advance(state)))))) {
auto next = parse_pat(preds::Parser_advance(state));
alts.push_back(next.pat);
state = next.parser;
}
if ((alts.length() == 1)) {
return first;
} else {
return preds::PatResult{std::make_shared<ast::Pat>(ast::PatOr{alts, span}), state};
}
}
preds::PatResult parse_pat(preds::Parser parser) noexcept{
auto kind = preds::Parser_kind(parser);
if (preds::TKind_is_ident(kind)) {
return parse_pattern_identifier_branch(parser, preds::TKind_ident(kind));
} else if (preds::TKind_is_int(kind)) {
return parse_pattern_integer(parser, preds::TKind_int_val(kind));
} else if (preds::TKind_is_str(kind)) {
return parse_pattern_string(parser, preds::TKind_str_val(kind));
} else if (preds::TKind_is_true(kind)) {
return parse_pattern_boolean(parser, true);
} else if (preds::TKind_is_false(kind)) {
return parse_pattern_boolean(parser, false);
} else if (preds::TKind_is_else(kind)) {
return preds::PatResult{std::make_shared<ast::Pat>(ast::PatWild{preds::Parser_span_at_cursor(parser)}), preds::Parser_advance(parser)};
} else {
return parse_pattern_fallback_wildcard(parser);
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
pats.push_back(std::make_shared<ast::Pat>(ast::PatIdent{preds::TKind_ident(preds::Parser_kind(state)), preds::Parser_span_at_cursor(state)}));
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
return std::visit(overloaded{[&](const ast::ExprCall& exprCall) { auto [callee, existing_args, __2] = exprCall; return [&]() {
auto new_args = mlc::Array{left_expr};
auto i = 0;
while ((i < existing_args.length())) {
new_args.push_back(existing_args[i]);
i = (i + 1);
}
return std::make_shared<ast::Expr>(ast::ExprCall{callee, new_args, ast::span_unknown()});
}(); },
[&](const auto& __v) { return [&]() {
auto call_args = mlc::Array{left_expr};
return std::make_shared<ast::Expr>(ast::ExprCall{right_expr, call_args, ast::span_unknown()});
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
auto operator_span = preds::Parser_span_at_cursor(state);
auto right = parse_and(preds::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprBin{mlc::String("||", 2), expr, right.expr, operator_span});
state = right.parser;
}
return preds::ExprResult{expr, state};
}
preds::ExprResult parse_and(preds::Parser parser) noexcept{
auto left = parse_cmp(parser);
auto expr = left.expr;
auto state = left.parser;
while ((preds::TKind_is_op(preds::Parser_kind(state)) && (preds::TKind_op_val(preds::Parser_kind(state)) == mlc::String("&&", 2)))) {
auto operator_span = preds::Parser_span_at_cursor(state);
auto right = parse_cmp(preds::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprBin{mlc::String("&&", 2), expr, right.expr, operator_span});
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
auto operator_span = preds::Parser_span_at_cursor(state);
auto right = parse_add(preds::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprBin{preds::TKind_op_val(kind), expr, right.expr, operator_span});
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
auto operator_span = preds::Parser_span_at_cursor(state);
auto right = parse_mul(preds::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprBin{preds::TKind_op_val(kind), expr, right.expr, operator_span});
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
auto operator_span = preds::Parser_span_at_cursor(state);
auto right = parse_unary(preds::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprBin{preds::TKind_op_val(kind), expr, right.expr, operator_span});
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
auto operator_span = preds::Parser_span_at_cursor(parser);
auto inner = parse_unary(preds::Parser_advance(parser));
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprUn{preds::TKind_op_val(kind), inner.expr, operator_span}), inner.parser};
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
auto dot_span = preds::Parser_span_at_cursor(state);
auto field_name = preds::TKind_ident(preds::Parser_kind(preds::Parser_advance(state)));
if ((preds::TKind_is_lparen(preds::Parser_kind(preds::Parser_advance_by(state, 2))) && (preds::Parser_span_at_cursor(preds::Parser_advance_by(state, 2)).line == preds::Parser_span_at_cursor(preds::Parser_advance_by(state, 1)).line))) {
auto margs = parse_arg_list(preds::Parser_advance_by(state, 3));
expr = std::make_shared<ast::Expr>(ast::ExprMethod{expr, field_name, margs.exprs, dot_span});
state = margs.parser;
} else {
expr = std::make_shared<ast::Expr>(ast::ExprField{expr, field_name, dot_span});
state = preds::Parser_advance_by(state, 2);
}
} else if ((preds::TKind_is_lparen(kind) && (preds::Parser_span_at_cursor(state).line == preds::Parser_prev_line(state)))) {
auto call_span = preds::Parser_span_at_cursor(state);
auto call_args = parse_arg_list(preds::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprCall{expr, call_args.exprs, call_span});
state = call_args.parser;
} else if (preds::TKind_is_lbracket(kind)) {
auto bracket_span = preds::Parser_span_at_cursor(state);
auto index = parse_expr(preds::Parser_advance(state));
expr = std::make_shared<ast::Expr>(ast::ExprIndex{expr, index.expr, bracket_span});
state = preds::Parser_advance(index.parser);
} else if (preds::TKind_is_question(kind)) {
auto question_span = preds::Parser_span_at_cursor(state);
expr = std::make_shared<ast::Expr>(ast::ExprQuestion{expr, question_span});
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
auto state = parser;
while (((!preds::TKind_is_rparen(preds::Parser_kind(state))) && (!preds::Parser_at_eof(state)))) {
auto arg_span = preds::Parser_span_at_cursor(state);
auto arg = ((preds::TKind_is_ident(preds::Parser_kind(state)) && preds::TKind_is_colon(parser_next_kind(state))) ? [&]() {
auto name = preds::TKind_ident(preds::Parser_kind(state));
auto value = parse_expr(preds::Parser_advance_by(state, 2));
state = value.parser;
return std::make_shared<ast::Expr>(ast::ExprNamedArg{name, value.expr, arg_span});
}() : [&]() {
auto r = parse_expr(state);
state = r.parser;
return r.expr;
}());
exprs.push_back(arg);
if (preds::TKind_is_comma(preds::Parser_kind(state))) {
state = preds::Parser_advance(state);
}
}
return preds::ExprsResult{exprs, preds::Parser_advance(state)};
}
}
std::shared_ptr<ast::Expr> build_template_expr(mlc::Array<mlc::String> parts) noexcept{
auto result = std::make_shared<ast::Expr>(ast::ExprStr{mlc::String("", 0), ast::span_unknown()});
auto pi = 0;
while ((pi < parts.length())) {
auto part_expr = (((pi % 2) == 0) ? std::make_shared<ast::Expr>(ast::ExprStr{parts[pi], ast::span_unknown()}) : ((parts[pi].length() == 0) ? std::make_shared<ast::Expr>(ast::ExprStr{mlc::String("", 0), ast::span_unknown()}) : [&]() {
auto sub_lex = lexer::tokenize(parts[pi]);
auto sub_parsed = parse_expr(preds::parser_new(sub_lex.tokens));
return std::make_shared<ast::Expr>(ast::ExprMethod{sub_parsed.expr, mlc::String("to_string", 9), {}, ast::span_unknown()});
}()));
if ((pi == 0)) {
result = part_expr;
} else {
result = std::make_shared<ast::Expr>(ast::ExprBin{mlc::String("+", 1), result, part_expr, ast::span_unknown()});
}
pi = (pi + 1);
}
return result;
}
preds::ExprResult parse_primary_integer_literal(preds::Parser parser, int value) noexcept{
auto source_span = preds::Parser_span_at_cursor(parser);
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprInt{value, source_span}), preds::Parser_advance(parser)};
}
preds::ExprResult parse_primary_string_literal(preds::Parser parser, mlc::String value) noexcept{
auto source_span = preds::Parser_span_at_cursor(parser);
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprStr{value, source_span}), preds::Parser_advance(parser)};
}
preds::ExprResult parse_primary_template_literal(preds::Parser parser, mlc::Array<mlc::String> parts) noexcept{
return preds::ExprResult{build_template_expr(parts), preds::Parser_advance(parser)};
}
preds::ExprResult parse_primary_boolean_literal(preds::Parser parser, bool value) noexcept{
auto source_span = preds::Parser_span_at_cursor(parser);
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprBool{value, source_span}), preds::Parser_advance(parser)};
}
preds::ExprResult parse_primary_parenthesized(preds::Parser parser) noexcept{
auto open_paren_span = preds::Parser_span_at_cursor(parser);
if (preds::TKind_is_rparen(preds::Parser_kind(preds::Parser_advance(parser)))) {
if (preds::TKind_is_fat_arrow(preds::Parser_kind(preds::Parser_advance_by(parser, 2)))) {
auto params = mlc::Array<mlc::String>{};
auto body = parse_expr(preds::Parser_advance_by(parser, 3));
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprLambda{params, body.expr, open_paren_span}), body.parser};
} else {
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprUnit{open_paren_span}), preds::Parser_advance_by(parser, 2)};
}
} else if (looks_like_typed_lambda_params(preds::Parser_advance(parser))) {
auto param_result = parse_typed_lambda_params(preds::Parser_advance(parser));
auto body = parse_expr(preds::Parser_advance(param_result.parser));
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprLambda{param_result.exprs, body.expr, open_paren_span}), body.parser};
} else if (looks_like_lambda_params(preds::Parser_advance(parser))) {
auto param_result = parse_lambda_params(preds::Parser_advance(parser));
auto body = parse_expr(preds::Parser_advance(param_result.parser));
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprLambda{param_result.exprs, body.expr, open_paren_span}), body.parser};
} else {
auto e0 = parse_expr(preds::Parser_advance(parser));
if (preds::TKind_is_comma(preds::Parser_kind(e0.parser))) {
auto elts = mlc::Array<std::shared_ptr<ast::Expr>>{};
elts.push_back(e0.expr);
auto st = e0.parser;
while (preds::TKind_is_comma(preds::Parser_kind(st))) {
auto en = parse_expr(preds::Parser_advance(st));
elts.push_back(en.expr);
st = en.parser;
}
if (preds::TKind_is_rparen(preds::Parser_kind(st))) {
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprTuple{elts, open_paren_span}), preds::Parser_advance(st)};
} else {
return preds::ExprResult{e0.expr, e0.parser};
}
} else {
return preds::ExprResult{e0.expr, preds::Parser_advance(e0.parser)};
}
}


}
preds::ExprResult parse_primary_if_or_unless(preds::Parser parser) noexcept{
return parse_if_expr(parser);
}
preds::ExprResult parse_primary_do_block(preds::Parser parser) noexcept{
auto header_span = preds::Parser_span_at_cursor(parser);
return parse_block(preds::Parser_advance(parser), header_span);
}
preds::ExprResult parse_primary_while_loop(preds::Parser parser) noexcept{
auto header_span = preds::Parser_span_at_cursor(parser);
return parse_while_expr(preds::Parser_advance(parser), header_span);
}
preds::ExprResult parse_primary_for_loop(preds::Parser parser) noexcept{
auto header_span = preds::Parser_span_at_cursor(parser);
return parse_for_expr(preds::Parser_advance(parser), header_span);
}
preds::ExprResult parse_primary_match(preds::Parser parser) noexcept{
auto header_span = preds::Parser_span_at_cursor(parser);
return parse_match_expr(preds::Parser_advance(parser), header_span);
}
preds::ExprResult parse_primary_return_as_block(preds::Parser parser) noexcept{
auto header_span = preds::Parser_span_at_cursor(parser);
auto value = parse_expr(preds::Parser_advance(parser));
auto statements = mlc::Array<std::shared_ptr<ast::Stmt>>{};
statements.push_back(std::make_shared<ast::Stmt>(ast::StmtReturn{value.expr, header_span}));
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprBlock{statements, std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()}), header_span}), value.parser};
}
preds::ExprResult parse_primary_identifier(preds::Parser parser, mlc::String name) noexcept{
auto source_span = preds::Parser_span_at_cursor(parser);
auto after_name = preds::Parser_advance(parser);
if (preds::TKind_is_fat_arrow(preds::Parser_kind(after_name))) {
auto parameters = mlc::Array{name};
auto body = parse_expr(preds::Parser_advance(after_name));
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprLambda{parameters, body.expr, source_span}), body.parser};
} else if ((preds::is_ctor_name(name) && preds::TKind_is_lbrace(preds::Parser_kind(after_name)))) {
auto brace_span = preds::Parser_span_at_cursor(after_name);
return parse_record_expr(preds::Parser_advance(after_name), name, brace_span);
} else {
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprIdent{name, source_span}), after_name};
}

}
preds::ExprResult parse_primary_unit_fallback(preds::Parser parser) noexcept{
auto source_span = preds::Parser_span_at_cursor(parser);
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprUnit{source_span}), preds::Parser_advance(parser)};
}
preds::ExprResult parse_primary(preds::Parser parser) noexcept{
auto kind = preds::Parser_kind(parser);
if (preds::TKind_is_float(kind)) {
auto source_span = preds::Parser_span_at_cursor(parser);
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprFloat{preds::TKind_float_val(kind), source_span}), preds::Parser_advance(parser)};
} else if (preds::TKind_is_i64(kind)) {
auto source_span = preds::Parser_span_at_cursor(parser);
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprI64{preds::TKind_i64_val(kind), source_span}), preds::Parser_advance(parser)};
} else if (preds::TKind_is_u8(kind)) {
auto source_span = preds::Parser_span_at_cursor(parser);
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprU8{preds::TKind_u8_val(kind), source_span}), preds::Parser_advance(parser)};
} else if (preds::TKind_is_usize(kind)) {
auto source_span = preds::Parser_span_at_cursor(parser);
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprUsize{preds::TKind_usize_val(kind), source_span}), preds::Parser_advance(parser)};
} else if (preds::TKind_is_char_lit(kind)) {
auto source_span = preds::Parser_span_at_cursor(parser);
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprChar{preds::TKind_char_val(kind), source_span}), preds::Parser_advance(parser)};
} else if (preds::TKind_is_int(kind)) {
return parse_primary_integer_literal(parser, preds::TKind_int_val(kind));
} else if (preds::TKind_is_str(kind)) {
return parse_primary_string_literal(parser, preds::TKind_str_val(kind));
} else if (preds::TKind_is_template(kind)) {
return parse_primary_template_literal(parser, preds::TKind_template_parts(kind));
} else if (preds::TKind_is_true(kind)) {
return parse_primary_boolean_literal(parser, true);
} else if (preds::TKind_is_false(kind)) {
return parse_primary_boolean_literal(parser, false);
} else if (preds::TKind_is_lparen(kind)) {
return parse_primary_parenthesized(parser);
} else if (preds::TKind_is_lbracket(kind)) {
auto bracket_span = preds::Parser_span_at_cursor(parser);
return parse_array_lit(preds::Parser_advance(parser), bracket_span);
} else if ((preds::TKind_is_if(kind) || preds::TKind_is_unless(kind))) {
return parse_primary_if_or_unless(parser);
} else if (preds::TKind_is_do(kind)) {
return parse_primary_do_block(parser);
} else if (preds::TKind_is_while(kind)) {
return parse_primary_while_loop(parser);
} else if (preds::TKind_is_for(kind)) {
return parse_primary_for_loop(parser);
} else if (preds::TKind_is_with(kind)) {
auto header_span = preds::Parser_span_at_cursor(parser);
return parse_with_expr(preds::Parser_advance(parser), header_span);
} else if (preds::TKind_is_match(kind)) {
return parse_primary_match(parser);
} else if (preds::TKind_is_return(kind)) {
return parse_primary_return_as_block(parser);
} else if (preds::TKind_is_ident(kind)) {
return parse_primary_identifier(parser, preds::TKind_ident(kind));
} else {
return parse_primary_unit_fallback(parser);
}



















}
bool looks_like_typed_lambda_params(preds::Parser parser) noexcept{
return (preds::TKind_is_ident(preds::Parser_kind(parser)) && preds::TKind_is_colon(preds::Parser_kind(preds::Parser_advance(parser))));
}
preds::NamesResult parse_typed_lambda_params(preds::Parser parser) noexcept{
auto names = mlc::Array<mlc::String>{};
auto state = parser;
names.push_back(preds::TKind_ident(preds::Parser_kind(state)));
state = types::parse_type(preds::Parser_advance_by(state, 2)).parser;
while (preds::TKind_is_comma(preds::Parser_kind(state))) {
state = preds::Parser_advance(state);
names.push_back(preds::TKind_ident(preds::Parser_kind(state)));
state = types::parse_type(preds::Parser_advance_by(state, 2)).parser;
}
return preds::NamesResult{names, preds::Parser_advance(state)};
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
preds::ExprResult parse_array_lit(preds::Parser parser, ast::Span header_span) noexcept{
auto exprs = mlc::Array<std::shared_ptr<ast::Expr>>{};
if (preds::TKind_is_rbracket(preds::Parser_kind(parser))) {
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprArray{exprs, header_span}), preds::Parser_advance(parser)};
} else {
auto first = parse_expr(parser);
exprs.push_back(first.expr);
auto state = first.parser;
while (preds::TKind_is_comma(preds::Parser_kind(state))) {
auto next = parse_expr(preds::Parser_advance(state));
exprs.push_back(next.expr);
state = next.parser;
}
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprArray{exprs, header_span}), preds::Parser_advance(state)};
}
}
preds::ExprResult parse_if_expr(preds::Parser parser) noexcept{
auto header_span = preds::Parser_span_at_cursor(parser);
auto is_negated = preds::TKind_is_unless(preds::Parser_kind(parser));
auto cond_result = parse_expr(preds::Parser_advance(parser));
auto condition = (is_negated ? std::make_shared<ast::Expr>(ast::ExprUn{mlc::String("!", 1), cond_result.expr, ast::span_unknown()}) : cond_result.expr);
auto then_result = parse_stmts_until_else_end(preds::Parser_advance(cond_result.parser));
auto then_expr = StmtsResult_to_block_expr(then_result, header_span);
if (preds::TKind_is_else(preds::Parser_kind(then_result.parser))) {
auto after_else = preds::Parser_advance(then_result.parser);
if ((preds::TKind_is_if(preds::Parser_kind(after_else)) || preds::TKind_is_unless(preds::Parser_kind(after_else)))) {
auto else_result = parse_if_expr(after_else);
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprIf{condition, then_expr, else_result.expr, header_span}), else_result.parser};
} else if (preds::TKind_is_do(preds::Parser_kind(after_else))) {
auto else_block = parse_block(preds::Parser_advance(after_else), header_span);
auto after_block = (preds::TKind_is_end(preds::Parser_kind(else_block.parser)) ? preds::Parser_advance(else_block.parser) : else_block.parser);
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprIf{condition, then_expr, else_block.expr, header_span}), after_block};
} else {
auto else_span = preds::Parser_span_at_cursor(after_else);
auto else_stmts = parse_stmts_until_end(after_else);
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprIf{condition, then_expr, StmtsResult_to_block_expr(else_stmts, else_span), header_span}), else_stmts.parser};
}

} else {
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprIf{condition, then_expr, std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()}), header_span}), preds::Parser_advance(then_result.parser)};
}
}
preds::ExprResult parse_block(preds::Parser parser, ast::Span header_span) noexcept{
auto result = parse_stmts_until_end(parser);
return preds::ExprResult{StmtsResult_to_block_expr(result, header_span), result.parser};
}
bool is_unit_expr(std::shared_ptr<ast::Expr> expr) noexcept{
return std::visit(overloaded{[&](const ast::ExprUnit& exprUnit) { auto [__0] = exprUnit; return true; },
[&](const ast::ExprBlock& exprBlock) { auto [__0, result, __2] = exprBlock; return is_unit_expr(result); },
[&](const ast::ExprIf& exprIf) { auto [__0, __1, else_expr, __3] = exprIf; return is_unit_expr(else_expr); },
[&](const ast::ExprBin& exprBin) { auto [op, __1, __2, __3] = exprBin; return (op == mlc::String("=", 1)); },
[&](const auto& __v) { return false; }
}, (*expr));
}
std::shared_ptr<ast::Expr> block_result(mlc::Array<std::shared_ptr<ast::Stmt>> stmts) noexcept{
if ((stmts.length() == 0)) {
return std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()});
} else {
return std::visit(overloaded{[&](const ast::StmtExpr& stmtExpr) { auto [expr, __1] = stmtExpr; return (is_unit_expr(expr) ? std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()}) : expr); },
[&](const auto& __v) { return std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()}); }
}, (*stmts[(stmts.length() - 1)]));
}
}
mlc::Array<std::shared_ptr<ast::Stmt>> block_body(mlc::Array<std::shared_ptr<ast::Stmt>> stmts) noexcept{
if ((stmts.length() == 0)) {
return stmts;
} else {
return std::visit(overloaded{[&](const ast::StmtExpr& stmtExpr) { auto [expr, __1] = stmtExpr; return (is_unit_expr(expr) ? stmts : [&]() {
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
std::shared_ptr<ast::Expr> StmtsResult_to_block_expr(preds::StmtsResult self, ast::Span header_span) noexcept{
return std::make_shared<ast::Expr>(ast::ExprBlock{block_body(self.stmts), block_result(self.stmts), header_span});
}
preds::ExprResult parse_while_expr(preds::Parser parser, ast::Span header_span) noexcept{
auto cond = parse_expr(parser);
auto body = parse_stmts_until_end(preds::Parser_advance(cond.parser));
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprWhile{cond.expr, body.stmts, header_span}), body.parser};
}
preds::ExprResult parse_for_expr(preds::Parser parser, ast::Span header_span) noexcept{
auto var_name = preds::TKind_ident(preds::Parser_kind(parser));
auto iter = parse_expr(preds::Parser_advance_by(parser, 2));
auto body = parse_stmts_until_end(preds::Parser_advance(iter.parser));
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprFor{var_name, iter.expr, body.stmts, header_span}), body.parser};
}
preds::ExprResult parse_with_expr(preds::Parser parser, ast::Span header_span) noexcept{
auto resource = parse_expr(parser);
auto after_as = preds::Parser_advance(resource.parser);
auto binder = preds::TKind_ident(preds::Parser_kind(after_as));
auto body = parse_stmts_until_end(preds::Parser_advance(preds::Parser_advance(after_as)));
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprWith{resource.expr, binder, body.stmts, header_span}), body.parser};
}
preds::ExprResult parse_match_expr(preds::Parser parser, ast::Span header_span) noexcept{
auto subject = parse_expr(parser);
auto next = preds::Parser_kind(subject.parser);
if (preds::TKind_is_lbrace(next)) {
auto arms = parse_arms_brace(preds::Parser_advance(subject.parser));
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprMatch{subject.expr, arms.arms, header_span}), arms.parser};
} else {
auto arms = parse_arms_pipe(subject.parser);
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprMatch{subject.expr, arms.arms, header_span}), arms.parser};
}
}
preds::ArmsResult parse_arms_brace(preds::Parser parser) noexcept{
auto arms = mlc::Array<std::shared_ptr<ast::MatchArm>>{};
auto state = parser;
while (((!preds::TKind_is_rbrace(preds::Parser_kind(state))) && (!preds::Parser_at_eof(state)))) {
if (preds::TKind_is_bar(preds::Parser_kind(state))) {
state = preds::Parser_advance(state);
}
auto pat_result = parse_or_pat(state);
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
auto pat_result = parse_or_pat(preds::Parser_advance(state));
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
auto field_span = preds::Parser_span_at_cursor(state);
auto field_name = preds::TKind_ident(preds::Parser_kind(state));
auto after_name = preds::Parser_advance(state);
if (preds::TKind_is_colon(preds::Parser_kind(after_name))) {
auto value_result = parse_expr(preds::Parser_advance(after_name));
field_vals.push_back(std::make_shared<ast::FieldVal>(ast::FieldVal{field_name, value_result.expr}));
state = value_result.parser;
} else {
field_vals.push_back(std::make_shared<ast::FieldVal>(ast::FieldVal{field_name, std::make_shared<ast::Expr>(ast::ExprIdent{field_name, field_span})}));
state = after_name;
}
if (preds::TKind_is_comma(preds::Parser_kind(state))) {
state = preds::Parser_advance(state);
}
}
return preds::FieldValsResult{field_vals, preds::Parser_advance(state)};
}
preds::ExprResult parse_record_expr(preds::Parser parser, mlc::String record_name, ast::Span record_span) noexcept{
if (preds::TKind_is_spread(preds::Parser_kind(parser))) {
auto base_result = parse_expr(preds::Parser_advance(parser));
auto after_base = base_result.parser;
if (preds::TKind_is_comma(preds::Parser_kind(after_base))) {
after_base = preds::Parser_advance(after_base);
}
auto fields_result = parse_record_fields(after_base);
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprRecordUpdate{record_name, base_result.expr, fields_result.field_vals, record_span}), fields_result.parser};
} else {
auto fields_result = parse_record_fields(parser);
return preds::ExprResult{std::make_shared<ast::Expr>(ast::ExprRecord{record_name, fields_result.field_vals, record_span}), fields_result.parser};
}
}

} // namespace exprs
