#include "test_pipe_and_record_update.hpp"

#include "test_runner.hpp"
#include "eval.hpp"
#include "context.hpp"
#include "exprs.hpp"
#include "lexer.hpp"
#include "ast.hpp"

namespace test_pipe_and_record_update {

using namespace test_runner;
using namespace eval;
using namespace context;
using namespace exprs;
using namespace lexer;
using namespace ast;
using namespace ast_tokens;

mlc::Array<test_runner::TestResult> pipe_and_record_update_tests() noexcept;

mlc::Array<test_runner::TestResult> pipe_and_record_update_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
ast::Program empty_prog = ast::Program{{}};
context::CodegenContext ctx = context::create_codegen_context(empty_prog);
std::shared_ptr<ast::Expr> left_expr = std::make_shared<ast::Expr>(ast::ExprIdent(mlc::String("x"), ast::span_unknown()));
std::shared_ptr<ast::Expr> right_fn = std::make_shared<ast::Expr>(ast::ExprIdent(mlc::String("double"), ast::span_unknown()));
std::shared_ptr<ast::Expr> plain_pipe = exprs::pipe_desugar(left_expr, right_fn);
results.push_back(test_runner::assert_eq_str(mlc::String("pipe x |> double - double_(x)"), eval::gen_expr(plain_pipe, ctx), mlc::String("double_(x)")));
mlc::Array<std::shared_ptr<ast::Expr>> right_call_args = mlc::Array<std::shared_ptr<ast::Expr>>{std::make_shared<ast::Expr>(ast::ExprInt(5, ast::span_unknown()))};
std::shared_ptr<ast::Expr> right_call = std::make_shared<ast::Expr>(ast::ExprCall(std::make_shared<ast::Expr>(ast::ExprIdent(mlc::String("add"), ast::span_unknown())), right_call_args, ast::span_unknown()));
std::shared_ptr<ast::Expr> partial_pipe = exprs::pipe_desugar(left_expr, right_call);
results.push_back(test_runner::assert_eq_str(mlc::String("pipe x |> add(5) - add(x, 5)"), eval::gen_expr(partial_pipe, ctx), mlc::String("add(x, 5)")));
mlc::Array<std::shared_ptr<ast::Expr>> right_multi_args = mlc::Array<std::shared_ptr<ast::Expr>>{std::make_shared<ast::Expr>(ast::ExprInt(3, ast::span_unknown())), std::make_shared<ast::Expr>(ast::ExprInt(4, ast::span_unknown()))};
std::shared_ptr<ast::Expr> right_multi_call = std::make_shared<ast::Expr>(ast::ExprCall(std::make_shared<ast::Expr>(ast::ExprIdent(mlc::String("clamp"), ast::span_unknown())), right_multi_args, ast::span_unknown()));
std::shared_ptr<ast::Expr> multi_partial_pipe = exprs::pipe_desugar(left_expr, right_multi_call);
results.push_back(test_runner::assert_eq_str(mlc::String("pipe x |> clamp(3, 4) - clamp(x, 3, 4)"), eval::gen_expr(multi_partial_pipe, ctx), mlc::String("clamp(x, 3, 4)")));
std::shared_ptr<ast::Decl> point_decl = std::make_shared<ast::Decl>(ast::DeclType(mlc::String("Point"), {}, mlc::Array<std::shared_ptr<ast::TypeVariant>>{std::make_shared<ast::TypeVariant>(ast::VarRecord(mlc::String("Point"), mlc::Array<std::shared_ptr<ast::FieldDef>>{std::make_shared<ast::FieldDef>(ast::FieldDef{mlc::String("x"), std::make_shared<ast::TypeExpr>((ast::TyI32{}))}), std::make_shared<ast::FieldDef>(ast::FieldDef{mlc::String("y"), std::make_shared<ast::TypeExpr>((ast::TyI32{}))})}))}));
ast::Program point_prog = ast::Program{mlc::Array<std::shared_ptr<ast::Decl>>{point_decl}};
context::CodegenContext ctx_point = context::create_codegen_context(point_prog);
mlc::Array<std::shared_ptr<ast::FieldVal>> override_fields = mlc::Array<std::shared_ptr<ast::FieldVal>>{std::make_shared<ast::FieldVal>(ast::FieldVal{mlc::String("x"), std::make_shared<ast::Expr>(ast::ExprInt(10, ast::span_unknown()))})};
std::shared_ptr<ast::Expr> base_expr = std::make_shared<ast::Expr>(ast::ExprIdent(mlc::String("p"), ast::span_unknown()));
std::shared_ptr<ast::Expr> update_expr = std::make_shared<ast::Expr>(ast::ExprRecordUpdate(mlc::String("Point"), base_expr, override_fields, ast::span_unknown()));
results.push_back(test_runner::assert_eq_str(mlc::String("Point { ...p, x: 10 } - Point{10, p.y}"), eval::gen_expr(update_expr, ctx_point), mlc::String("Point{10, p.y}")));
mlc::Array<std::shared_ptr<ast::FieldVal>> two_overrides = mlc::Array<std::shared_ptr<ast::FieldVal>>{std::make_shared<ast::FieldVal>(ast::FieldVal{mlc::String("x"), std::make_shared<ast::Expr>(ast::ExprInt(0, ast::span_unknown()))}), std::make_shared<ast::FieldVal>(ast::FieldVal{mlc::String("y"), std::make_shared<ast::Expr>(ast::ExprInt(0, ast::span_unknown()))})};
std::shared_ptr<ast::Expr> all_overrides_expr = std::make_shared<ast::Expr>(ast::ExprRecordUpdate(mlc::String("Point"), base_expr, two_overrides, ast::span_unknown()));
results.push_back(test_runner::assert_eq_str(mlc::String("Point { ...p, x: 0, y: 0 } - Point{0, 0}"), eval::gen_expr(all_overrides_expr, ctx_point), mlc::String("Point{0, 0}")));
ast_tokens::LexOut spread_tokens = lexer::tokenize(mlc::String("..."));
results.push_back(test_runner::assert_eq_int(mlc::String("'...' - 2 tokens (Spread + Eof)"), spread_tokens.tokens.size(), 2));
int first_token_is_spread = [&]() { if (std::holds_alternative<ast_tokens::Spread>(spread_tokens.tokens[0].kind)) {  return 1; } return 0; }();
results.push_back(test_runner::assert_eq_int(mlc::String("first token of '...' is Spread"), first_token_is_spread, 1));
return results;
}

} // namespace test_pipe_and_record_update
