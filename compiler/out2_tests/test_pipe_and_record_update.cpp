#define main mlc_user_main
#include "test_pipe_and_record_update.hpp"

namespace test_pipe_and_record_update {

std::shared_ptr<semantic_ir::SExpr> sid(mlc::String name) noexcept{
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprIdent{name, std::make_shared<registry::Type>(registry::TUnknown{}), ast::span_unknown()});
}
std::shared_ptr<semantic_ir::SExpr> si(int v) noexcept{
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprInt{v, std::make_shared<registry::Type>(registry::TI32{}), ast::span_unknown()});
}
mlc::Array<test_runner::TestResult> pipe_and_record_update_tests() noexcept{
auto results = mlc::Array<test_runner::TestResult>{};
auto empty_prog = ast::Program{{}};
auto ctx = context::create_codegen_context(empty_prog);
auto tctx = transform::empty_transform_context();
auto left_expr = std::make_shared<ast::Expr>(ast::ExprIdent{mlc::String("x", 1), ast::span_unknown()});
auto right_fn = std::make_shared<ast::Expr>(ast::ExprIdent{mlc::String("double", 6), ast::span_unknown()});
auto plain_pipe = exprs::pipe_desugar(left_expr, right_fn);
results.push_back(test_runner::assert_eq_str(mlc::String("pipe x |> double - double_(x)", 29), eval::gen_expr(transform::transform_expr(plain_pipe, tctx, [=](auto stmts, auto ctx2) { return transform_stmts::transform_stmts(stmts, ctx2); }), ctx), mlc::String("double_(x)", 10)));
auto right_call_args = mlc::Array{std::make_shared<ast::Expr>(ast::ExprInt{5, ast::span_unknown()})};
auto right_call = std::make_shared<ast::Expr>(ast::ExprCall{std::make_shared<ast::Expr>(ast::ExprIdent{mlc::String("add", 3), ast::span_unknown()}), right_call_args, ast::span_unknown()});
auto partial_pipe = exprs::pipe_desugar(left_expr, right_call);
results.push_back(test_runner::assert_eq_str(mlc::String("pipe x |> add(5) - add(x, 5)", 28), eval::gen_expr(transform::transform_expr(partial_pipe, tctx, [=](auto stmts, auto ctx2) { return transform_stmts::transform_stmts(stmts, ctx2); }), ctx), mlc::String("add(x, 5)", 9)));
auto right_multi_args = mlc::Array{std::make_shared<ast::Expr>(ast::ExprInt{3, ast::span_unknown()}), std::make_shared<ast::Expr>(ast::ExprInt{4, ast::span_unknown()})};
auto right_multi_call = std::make_shared<ast::Expr>(ast::ExprCall{std::make_shared<ast::Expr>(ast::ExprIdent{mlc::String("clamp", 5), ast::span_unknown()}), right_multi_args, ast::span_unknown()});
auto multi_partial_pipe = exprs::pipe_desugar(left_expr, right_multi_call);
results.push_back(test_runner::assert_eq_str(mlc::String("pipe x |> clamp(3, 4) - clamp(x, 3, 4)", 38), eval::gen_expr(transform::transform_expr(multi_partial_pipe, tctx, [=](auto stmts, auto ctx2) { return transform_stmts::transform_stmts(stmts, ctx2); }), ctx), mlc::String("clamp(x, 3, 4)", 14)));
auto point_decl = std::make_shared<ast::Decl>(ast::DeclType{mlc::String("Point", 5), {}, mlc::Array{std::make_shared<ast::TypeVariant>(ast::VarRecord{mlc::String("Point", 5), mlc::Array{std::make_shared<ast::FieldDef>(ast::FieldDef{mlc::String("x", 1), std::make_shared<ast::TypeExpr>(ast::TyI32{})}), std::make_shared<ast::FieldDef>(ast::FieldDef{mlc::String("y", 1), std::make_shared<ast::TypeExpr>(ast::TyI32{})})}})}, {}});
auto point_prog = ast::Program{mlc::Array{point_decl}};
auto ctx_point = context::create_codegen_context(point_prog);
auto point_type = std::make_shared<registry::Type>(registry::TNamed{mlc::String("Point", 5)});
auto override_fields = mlc::Array{std::make_shared<semantic_ir::SFieldVal>(semantic_ir::SFieldVal{mlc::String("x", 1), si(10)})};
auto base_expr = sid(mlc::String("p", 1));
auto update_expr = std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprRecordUpdate{mlc::String("Point", 5), base_expr, override_fields, point_type, ast::span_unknown()});
results.push_back(test_runner::assert_eq_str(mlc::String("Point { ...p, x: 10 } - Point{10, p.y}", 38), eval::gen_expr(update_expr, ctx_point), mlc::String("Point{10, p.y}", 14)));
auto two_overrides = mlc::Array{std::make_shared<semantic_ir::SFieldVal>(semantic_ir::SFieldVal{mlc::String("x", 1), si(0)}), std::make_shared<semantic_ir::SFieldVal>(semantic_ir::SFieldVal{mlc::String("y", 1), si(0)})};
auto all_overrides_expr = std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprRecordUpdate{mlc::String("Point", 5), base_expr, two_overrides, point_type, ast::span_unknown()});
results.push_back(test_runner::assert_eq_str(mlc::String("Point { ...p, x: 0, y: 0 } - Point{0, 0}", 40), eval::gen_expr(all_overrides_expr, ctx_point), mlc::String("Point{0, 0}", 11)));
auto lazy_update = std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprRecordUpdate{mlc::String("Point", 5), base_expr, override_fields, point_type, ast::span_unknown()});
results.push_back(test_runner::assert_true(mlc::String("record update without field order registry uses lazy block", 58), eval::gen_expr(lazy_update, ctx).contains(mlc::String("__upd", 5))));
auto spread_tokens = lexer::tokenize(mlc::String("...", 3));
results.push_back(test_runner::assert_eq_int(mlc::String("'...' - 2 tokens (Spread + Eof)", 31), spread_tokens.tokens.length(), 2));
auto first_token_is_spread = std::visit(overloaded{[&](const ast_tokens::Spread& spread) { return 1; },
[&](const auto& __v) { return 0; }
}, spread_tokens.tokens[0].kind);
results.push_back(test_runner::assert_eq_int(mlc::String("first token of '...' is Spread", 30), first_token_is_spread, 1));
return results;
}

} // namespace test_pipe_and_record_update
