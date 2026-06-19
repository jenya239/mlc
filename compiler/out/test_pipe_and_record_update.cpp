#include "test_pipe_and_record_update.hpp"

#include "test_runner.hpp"
#include "eval.hpp"
#include "context.hpp"
#include "exprs.hpp"
#include "lexer.hpp"
#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "transform.hpp"
#include "transform_stmts.hpp"

namespace test_pipe_and_record_update {

using namespace test_runner;
using namespace eval;
using namespace context;
using namespace exprs;
using namespace lexer;
using namespace ast;
using namespace semantic_ir;
using namespace registry;
using namespace transform;
using namespace transform_stmts;
using namespace ast_tokens;

std::shared_ptr<semantic_ir::SemanticExpression> sid(mlc::String name) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> si(int v) noexcept;

mlc::Array<test_runner::TestResult> pipe_and_record_update_tests() noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> sid(mlc::String name) noexcept{return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionIdent(name, std::make_shared<registry::Type>((registry::TUnknown{})), ast::span_unknown()));}

std::shared_ptr<semantic_ir::SemanticExpression> si(int v) noexcept{return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionInt(v, std::make_shared<registry::Type>((registry::TI32{})), ast::span_unknown()));}

mlc::Array<test_runner::TestResult> pipe_and_record_update_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
ast::Program empty_program = ast::Program{{}};
context::CodegenContext context = context::create_codegen_context(empty_program);
transform::TransformContext transform_context = transform::empty_transform_context();
std::shared_ptr<ast::Expr> left_expr = std::make_shared<ast::Expr>(ast::ExprIdent(mlc::String("x"), ast::span_unknown()));
std::shared_ptr<ast::Expr> right_fn = std::make_shared<ast::Expr>(ast::ExprIdent(mlc::String("double"), ast::span_unknown()));
std::shared_ptr<ast::Expr> plain_pipe = exprs::pipe_desugar(left_expr, right_fn);
results.push_back(test_runner::assert_eq_str(mlc::String("pipe x |> double - double_(x)"), eval::gen_expr(transform::transform_expr(plain_pipe, transform_context, [](mlc::Array<std::shared_ptr<ast::Stmt>> statements, transform::TransformContext nested_transform_context) mutable { return transform_stmts::transform_stmts(statements, nested_transform_context); }), context), mlc::String("double_(x)")));
mlc::Array<std::shared_ptr<ast::Expr>> right_call_args = mlc::Array<std::shared_ptr<ast::Expr>>{std::make_shared<ast::Expr>(ast::ExprInt(5, ast::span_unknown()))};
std::shared_ptr<ast::Expr> right_call = std::make_shared<ast::Expr>(ast::ExprCall(std::make_shared<ast::Expr>(ast::ExprIdent(mlc::String("add"), ast::span_unknown())), right_call_args, ast::span_unknown()));
std::shared_ptr<ast::Expr> partial_pipe = exprs::pipe_desugar(left_expr, right_call);
results.push_back(test_runner::assert_eq_str(mlc::String("pipe x |> add(5) - add(x, 5)"), eval::gen_expr(transform::transform_expr(partial_pipe, transform_context, [](mlc::Array<std::shared_ptr<ast::Stmt>> statements, transform::TransformContext nested_transform_context) mutable { return transform_stmts::transform_stmts(statements, nested_transform_context); }), context), mlc::String("add(x, 5)")));
mlc::Array<std::shared_ptr<ast::Expr>> right_multi_args = mlc::Array<std::shared_ptr<ast::Expr>>{std::make_shared<ast::Expr>(ast::ExprInt(3, ast::span_unknown())), std::make_shared<ast::Expr>(ast::ExprInt(4, ast::span_unknown()))};
std::shared_ptr<ast::Expr> right_multi_call = std::make_shared<ast::Expr>(ast::ExprCall(std::make_shared<ast::Expr>(ast::ExprIdent(mlc::String("clamp"), ast::span_unknown())), right_multi_args, ast::span_unknown()));
std::shared_ptr<ast::Expr> multi_partial_pipe = exprs::pipe_desugar(left_expr, right_multi_call);
results.push_back(test_runner::assert_eq_str(mlc::String("pipe x |> clamp(3, 4) - clamp(x, 3, 4)"), eval::gen_expr(transform::transform_expr(multi_partial_pipe, transform_context, [](mlc::Array<std::shared_ptr<ast::Stmt>> statements, transform::TransformContext nested_transform_context) mutable { return transform_stmts::transform_stmts(statements, nested_transform_context); }), context), mlc::String("clamp(x, 3, 4)")));
std::shared_ptr<ast::Decl> point_decl = std::make_shared<ast::Decl>(ast::DeclType(mlc::String("Point"), {}, mlc::Array<std::shared_ptr<ast::TypeVariant>>{std::make_shared<ast::TypeVariant>(ast::VarRecord(mlc::String("Point"), mlc::Array<std::shared_ptr<ast::FieldDef>>{std::make_shared<ast::FieldDef>(ast::FieldDef{mlc::String("x"), std::make_shared<ast::TypeExpr>((ast::TyI32{})), false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown()))}), std::make_shared<ast::FieldDef>(ast::FieldDef{mlc::String("y"), std::make_shared<ast::TypeExpr>((ast::TyI32{})), false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown()))})}, false))}, {}, ast::span_unknown()));
ast::Program point_program = ast::Program{mlc::Array<std::shared_ptr<ast::Decl>>{point_decl}};
context::CodegenContext point_context = context::create_codegen_context(point_program);
std::shared_ptr<registry::Type> point_type = std::make_shared<registry::Type>(registry::TNamed(mlc::String("Point")));
mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> override_fields = mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>>{std::make_shared<semantic_ir::SemanticFieldVal>(semantic_ir::SemanticFieldVal{mlc::String("x"), si(10)})};
std::shared_ptr<semantic_ir::SemanticExpression> base_expr = sid(mlc::String("p"));
std::shared_ptr<semantic_ir::SemanticExpression> update_expr = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionRecordUpdate(mlc::String("Point"), base_expr, override_fields, point_type, ast::span_unknown()));
results.push_back(test_runner::assert_eq_str(mlc::String("Point { ...p, x: 10 } - Point{10, p.y}"), eval::gen_expr(update_expr, point_context), mlc::String("Point{10, p.y}")));
mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> two_overrides = mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>>{std::make_shared<semantic_ir::SemanticFieldVal>(semantic_ir::SemanticFieldVal{mlc::String("x"), si(0)}), std::make_shared<semantic_ir::SemanticFieldVal>(semantic_ir::SemanticFieldVal{mlc::String("y"), si(0)})};
std::shared_ptr<semantic_ir::SemanticExpression> all_overrides_expr = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionRecordUpdate(mlc::String("Point"), base_expr, two_overrides, point_type, ast::span_unknown()));
results.push_back(test_runner::assert_eq_str(mlc::String("Point { ...p, x: 0, y: 0 } - Point{0, 0}"), eval::gen_expr(all_overrides_expr, point_context), mlc::String("Point{0, 0}")));
std::shared_ptr<semantic_ir::SemanticExpression> lazy_update = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionRecordUpdate(mlc::String("Point"), base_expr, override_fields, point_type, ast::span_unknown()));
results.push_back(test_runner::assert_true(mlc::String("record update without field order registry uses lazy block"), eval::gen_expr(lazy_update, context).contains(mlc::String("__upd"))));
ast_tokens::LexOut spread_tokens = lexer::tokenize(mlc::String("..."));
results.push_back(test_runner::assert_eq_int(mlc::String("'...' - 2 tokens (Spread + Eof)"), spread_tokens.tokens.size(), 2));
int first_token_is_spread = [&]() { if (std::holds_alternative<ast_tokens::Spread>(spread_tokens.tokens[0].kind)) {  return 1; } return 0; }();
results.push_back(test_runner::assert_eq_int(mlc::String("first token of '...' is Spread"), first_token_is_spread, 1));
return results;
}

} // namespace test_pipe_and_record_update
