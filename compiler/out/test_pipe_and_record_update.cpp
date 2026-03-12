#include "test_pipe_and_record_update.hpp"

#include "test_runner.hpp"
#include "codegen.hpp"
#include "exprs.hpp"
#include "lexer.hpp"
#include "ast.hpp"

namespace test_pipe_and_record_update {

using namespace test_runner;
using namespace codegen;
using namespace exprs;
using namespace lexer;
using namespace ast;
using namespace ast_tokens;

mlc::Array<test_runner::TestResult> pipe_and_record_update_tests() noexcept;

mlc::Array<test_runner::TestResult> pipe_and_record_update_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
ast::Program empty_prog = ast::Program{{}};
codegen::CodegenContext ctx = codegen::create_codegen_context(empty_prog);
std::shared_ptr<ast::Expr> left_expr = std::make_shared<ast::Expr>(ast::ExprIdent(mlc::String("x")));
std::shared_ptr<ast::Expr> right_fn = std::make_shared<ast::Expr>(ast::ExprIdent(mlc::String("double")));
std::shared_ptr<ast::Expr> plain_pipe = exprs::pipe_desugar(left_expr, right_fn);
results.push_back(test_runner::assert_eq_str(mlc::String("pipe x |> double → double_(x)"), codegen::gen_expr(plain_pipe, ctx), mlc::String("double_(x)")));
mlc::Array<std::shared_ptr<ast::Expr>> right_call_args = mlc::Array<std::shared_ptr<ast::Expr>>{std::make_shared<ast::Expr>(ast::ExprInt(5))};
std::shared_ptr<ast::Expr> right_call = std::make_shared<ast::Expr>(ast::ExprCall(std::make_shared<ast::Expr>(ast::ExprIdent(mlc::String("add"))), right_call_args));
std::shared_ptr<ast::Expr> partial_pipe = exprs::pipe_desugar(left_expr, right_call);
results.push_back(test_runner::assert_eq_str(mlc::String("pipe x |> add(5) → add(x, 5)"), codegen::gen_expr(partial_pipe, ctx), mlc::String("add(x, 5)")));
mlc::Array<std::shared_ptr<ast::Expr>> right_multi_args = mlc::Array<std::shared_ptr<ast::Expr>>{std::make_shared<ast::Expr>(ast::ExprInt(3)), std::make_shared<ast::Expr>(ast::ExprInt(4))};
std::shared_ptr<ast::Expr> right_multi_call = std::make_shared<ast::Expr>(ast::ExprCall(std::make_shared<ast::Expr>(ast::ExprIdent(mlc::String("clamp"))), right_multi_args));
std::shared_ptr<ast::Expr> multi_partial_pipe = exprs::pipe_desugar(left_expr, right_multi_call);
results.push_back(test_runner::assert_eq_str(mlc::String("pipe x |> clamp(3, 4) → clamp(x, 3, 4)"), codegen::gen_expr(multi_partial_pipe, ctx), mlc::String("clamp(x, 3, 4)")));
std::shared_ptr<ast::Decl> point_decl = std::make_shared<ast::Decl>(ast::DeclType(mlc::String("Point"), mlc::Array<std::shared_ptr<ast::TypeVariant>>{std::make_shared<ast::TypeVariant>(ast::VarRecord(mlc::String("Point"), mlc::Array<std::shared_ptr<ast::FieldDef>>{std::make_shared<ast::FieldDef>(ast::FieldDef{mlc::String("x"), std::make_shared<ast::TypeExpr>((ast::TyI32{}))}), std::make_shared<ast::FieldDef>(ast::FieldDef{mlc::String("y"), std::make_shared<ast::TypeExpr>((ast::TyI32{}))})}))}));
ast::Program point_prog = ast::Program{mlc::Array<std::shared_ptr<ast::Decl>>{point_decl}};
codegen::CodegenContext ctx_point = codegen::create_codegen_context(point_prog);
mlc::Array<std::shared_ptr<ast::FieldVal>> override_fields = mlc::Array<std::shared_ptr<ast::FieldVal>>{std::make_shared<ast::FieldVal>(ast::FieldVal{mlc::String("x"), std::make_shared<ast::Expr>(ast::ExprInt(10))})};
std::shared_ptr<ast::Expr> base_expr = std::make_shared<ast::Expr>(ast::ExprIdent(mlc::String("p")));
std::shared_ptr<ast::Expr> update_expr = std::make_shared<ast::Expr>(ast::ExprRecordUpdate(mlc::String("Point"), base_expr, override_fields));
results.push_back(test_runner::assert_eq_str(mlc::String("Point { ...p, x: 10 } → Point{10, p.y}"), codegen::gen_expr(update_expr, ctx_point), mlc::String("Point{10, p.y}")));
mlc::Array<std::shared_ptr<ast::FieldVal>> two_overrides = mlc::Array<std::shared_ptr<ast::FieldVal>>{std::make_shared<ast::FieldVal>(ast::FieldVal{mlc::String("x"), std::make_shared<ast::Expr>(ast::ExprInt(0))}), std::make_shared<ast::FieldVal>(ast::FieldVal{mlc::String("y"), std::make_shared<ast::Expr>(ast::ExprInt(0))})};
std::shared_ptr<ast::Expr> all_overrides_expr = std::make_shared<ast::Expr>(ast::ExprRecordUpdate(mlc::String("Point"), base_expr, two_overrides));
results.push_back(test_runner::assert_eq_str(mlc::String("Point { ...p, x: 0, y: 0 } → Point{0, 0}"), codegen::gen_expr(all_overrides_expr, ctx_point), mlc::String("Point{0, 0}")));
ast_tokens::LexOut spread_tokens = lexer::tokenize(mlc::String("..."));
results.push_back(test_runner::assert_eq_int(mlc::String("'...' → 2 tokens (Spread + Eof)"), spread_tokens.tokens.size(), 2));
int first_token_is_spread = [&]() { if (std::holds_alternative<ast_tokens::Spread>(spread_tokens.tokens[0].kind)) {  return 1; } return 0; }();
results.push_back(test_runner::assert_eq_int(mlc::String("first token of '...' is Spread"), first_token_is_spread, 1));
return results;
}

} // namespace test_pipe_and_record_update
