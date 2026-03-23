#include "test_codegen.hpp"

#include "test_runner.hpp"
#include "codegen.hpp"
#include "ast.hpp"

namespace test_codegen {

using namespace test_runner;
using namespace codegen;
using namespace ast;
using namespace ast_tokens;

mlc::Array<test_runner::TestResult> codegen_tests() noexcept;

mlc::Array<test_runner::TestResult> codegen_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
ast::Program empty_prog = ast::Program{{}};
codegen::CodegenContext ctx = codegen::create_codegen_context(empty_prog);
results.push_back(test_runner::assert_eq_str(mlc::String("create_codegen_context: gen_expr with ctx"), codegen::gen_expr(std::make_shared<ast::Expr>(ast::ExprInt(42)), ctx), mlc::String("42")));
results.push_back(test_runner::assert_eq_str(mlc::String("TyI32 - 'int'"), codegen::type_to_cpp(ctx, std::make_shared<ast::TypeExpr>((ast::TyI32{}))), mlc::String("int")));
results.push_back(test_runner::assert_eq_str(mlc::String("TyString - 'mlc::String'"), codegen::type_to_cpp(ctx, std::make_shared<ast::TypeExpr>((ast::TyString{}))), mlc::String("mlc::String")));
results.push_back(test_runner::assert_eq_str(mlc::String("TyBool - 'bool'"), codegen::type_to_cpp(ctx, std::make_shared<ast::TypeExpr>((ast::TyBool{}))), mlc::String("bool")));
results.push_back(test_runner::assert_eq_str(mlc::String("TyUnit - 'void'"), codegen::type_to_cpp(ctx, std::make_shared<ast::TypeExpr>((ast::TyUnit{}))), mlc::String("void")));
results.push_back(test_runner::assert_eq_str(mlc::String("TyArray(TyI32) - 'mlc::Array<int>'"), codegen::type_to_cpp(ctx, std::make_shared<ast::TypeExpr>(ast::TyArray(std::make_shared<ast::TypeExpr>((ast::TyI32{}))))), mlc::String("mlc::Array<int>")));
results.push_back(test_runner::assert_eq_str(mlc::String("TyShared(TyI32) - 'std::shared_ptr<int>'"), codegen::type_to_cpp(ctx, std::make_shared<ast::TypeExpr>(ast::TyShared(std::make_shared<ast::TypeExpr>((ast::TyI32{}))))), mlc::String("std::shared_ptr<int>")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprInt(42) - '42'"), codegen::gen_expr(std::make_shared<ast::Expr>(ast::ExprInt(42)), ctx), mlc::String("42")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprQuestion: gen_expr index-based unwrap"), codegen::gen_expr(std::make_shared<ast::Expr>(ast::ExprQuestion(std::make_shared<ast::Expr>(ast::ExprInt(7)))), ctx), mlc::String("({ auto __q = 7; if (std::get_if<1>(&__q)) return *std::get_if<1>(&__q); std::get<0>(__q).field0; })")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprBool(true) - 'true'"), codegen::gen_expr(std::make_shared<ast::Expr>(ast::ExprBool(true)), ctx), mlc::String("true")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprStr('hi') - 'mlc::String(\"hi\", 2)'"), codegen::gen_expr(std::make_shared<ast::Expr>(ast::ExprStr(mlc::String("hi"))), ctx), mlc::String("mlc::String(\"hi\", 2)")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprUnit - '/* unit */'"), codegen::gen_expr(std::make_shared<ast::Expr>((ast::ExprUnit{})), ctx), mlc::String("/* unit */")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprIdent('x') - 'x'"), codegen::gen_expr(std::make_shared<ast::Expr>(ast::ExprIdent(mlc::String("x"))), ctx), mlc::String("x")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprIdent('print') - 'mlc::io::print'"), codegen::gen_expr(std::make_shared<ast::Expr>(ast::ExprIdent(mlc::String("print"))), ctx), mlc::String("mlc::io::print")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprIdent('println') - 'mlc::io::println'"), codegen::gen_expr(std::make_shared<ast::Expr>(ast::ExprIdent(mlc::String("println"))), ctx), mlc::String("mlc::io::println")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprBin add - '(1 + 2)'"), codegen::gen_expr(std::make_shared<ast::Expr>(ast::ExprBin(mlc::String("+"), std::make_shared<ast::Expr>(ast::ExprInt(1)), std::make_shared<ast::Expr>(ast::ExprInt(2)))), ctx), mlc::String("(1 + 2)")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprUn neg - '(-42)'"), codegen::gen_expr(std::make_shared<ast::Expr>(ast::ExprUn(mlc::String("-"), std::make_shared<ast::Expr>(ast::ExprInt(42)))), ctx), mlc::String("(-42)")));
results.push_back(test_runner::assert_eq_str(mlc::String("map_builtin passthrough - 'foo'"), codegen::map_builtin(mlc::String("foo")), mlc::String("foo")));
mlc::Array<mlc::String> single_params = mlc::Array<mlc::String>{mlc::String("x")};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprLambda single param - '[=](auto x) { return ... }'"), codegen::gen_expr(std::make_shared<ast::Expr>(ast::ExprLambda(single_params, std::make_shared<ast::Expr>(ast::ExprBin(mlc::String("+"), std::make_shared<ast::Expr>(ast::ExprIdent(mlc::String("x"))), std::make_shared<ast::Expr>(ast::ExprInt(1)))))), ctx), mlc::String("[=](auto x) { return (x + 1); }")));
mlc::Array<mlc::String> two_params = mlc::Array<mlc::String>{mlc::String("a"), mlc::String("b")};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprLambda two params - '[=](auto a, auto b) { return ... }'"), codegen::gen_expr(std::make_shared<ast::Expr>(ast::ExprLambda(two_params, std::make_shared<ast::Expr>(ast::ExprBin(mlc::String("+"), std::make_shared<ast::Expr>(ast::ExprIdent(mlc::String("a"))), std::make_shared<ast::Expr>(ast::ExprIdent(mlc::String("b"))))))), ctx), mlc::String("[=](auto a, auto b) { return (a + b); }")));
mlc::Array<mlc::String> zero_params = {};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprLambda zero params - '[]() { return ... }'"), codegen::gen_expr(std::make_shared<ast::Expr>(ast::ExprLambda(zero_params, std::make_shared<ast::Expr>(ast::ExprInt(42)))), ctx), mlc::String("[]() { return 42; }")));
results.push_back(test_runner::assert_eq_str(mlc::String("map_method: length - length"), codegen::map_method(mlc::String("length")), mlc::String("length")));
results.push_back(test_runner::assert_eq_str(mlc::String("map_method: len - length"), codegen::map_method(mlc::String("len")), mlc::String("length")));
results.push_back(test_runner::assert_eq_str(mlc::String("map_method: push - push_back"), codegen::map_method(mlc::String("push")), mlc::String("push_back")));
results.push_back(test_runner::assert_eq_str(mlc::String("map_method: upper - upper"), codegen::map_method(mlc::String("upper")), mlc::String("upper")));
results.push_back(test_runner::assert_eq_str(mlc::String("map_method: to_upper - upper"), codegen::map_method(mlc::String("to_upper")), mlc::String("upper")));
results.push_back(test_runner::assert_eq_str(mlc::String("map_method: to_lower - lower"), codegen::map_method(mlc::String("to_lower")), mlc::String("lower")));
results.push_back(test_runner::assert_eq_str(mlc::String("map_method: trim passthrough"), codegen::map_method(mlc::String("trim")), mlc::String("trim")));
results.push_back(test_runner::assert_eq_str(mlc::String("map_method: split passthrough"), codegen::map_method(mlc::String("split")), mlc::String("split")));
results.push_back(test_runner::assert_eq_str(mlc::String("map_method: chars passthrough"), codegen::map_method(mlc::String("chars")), mlc::String("chars")));
results.push_back(test_runner::assert_eq_str(mlc::String("map_method: lines passthrough"), codegen::map_method(mlc::String("lines")), mlc::String("lines")));
mlc::Array<std::shared_ptr<ast::Expr>> call_lambda_args = mlc::Array<std::shared_ptr<ast::Expr>>{std::make_shared<ast::Expr>(ast::ExprInt(5)), std::make_shared<ast::Expr>(ast::ExprLambda(single_params, std::make_shared<ast::Expr>(ast::ExprBin(mlc::String("+"), std::make_shared<ast::Expr>(ast::ExprIdent(mlc::String("x"))), std::make_shared<ast::Expr>(ast::ExprInt(1))))))};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprCall with lambda arg"), codegen::gen_expr(std::make_shared<ast::Expr>(ast::ExprCall(std::make_shared<ast::Expr>(ast::ExprIdent(mlc::String("apply"))), call_lambda_args)), ctx), mlc::String("apply(5, [=](auto x) { return (x + 1); })")));
mlc::Array<std::shared_ptr<ast::Expr>> ok_args = mlc::Array<std::shared_ptr<ast::Expr>>{std::make_shared<ast::Expr>(ast::ExprInt(42))};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprCall ctor Ok(42) uses brace init"), codegen::gen_expr(std::make_shared<ast::Expr>(ast::ExprCall(std::make_shared<ast::Expr>(ast::ExprIdent(mlc::String("Ok"))), ok_args)), ctx), mlc::String("Ok{42}")));
mlc::Array<std::shared_ptr<ast::Expr>> err_args = mlc::Array<std::shared_ptr<ast::Expr>>{std::make_shared<ast::Expr>(ast::ExprStr(mlc::String("oops")))};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprCall ctor Err(str) uses brace init"), codegen::gen_expr(std::make_shared<ast::Expr>(ast::ExprCall(std::make_shared<ast::Expr>(ast::ExprIdent(mlc::String("Err"))), err_args)), ctx), mlc::String("Err{mlc::String(\"oops\", 4)}")));
mlc::Array<mlc::String> result_type_params = mlc::Array<mlc::String>{mlc::String("T"), mlc::String("E")};
mlc::Array<std::shared_ptr<ast::TypeExpr>> ok_fields = mlc::Array<std::shared_ptr<ast::TypeExpr>>{std::make_shared<ast::TypeExpr>(ast::TyNamed(mlc::String("T")))};
mlc::Array<std::shared_ptr<ast::TypeExpr>> err_fields = mlc::Array<std::shared_ptr<ast::TypeExpr>>{std::make_shared<ast::TypeExpr>(ast::TyNamed(mlc::String("E")))};
mlc::Array<std::shared_ptr<ast::TypeVariant>> result_variants = mlc::Array<std::shared_ptr<ast::TypeVariant>>{std::make_shared<ast::TypeVariant>(ast::VarTuple(mlc::String("Ok"), ok_fields)), std::make_shared<ast::TypeVariant>(ast::VarTuple(mlc::String("Err"), err_fields))};
mlc::String expected_fwd = mlc::String("template<typename T>\nstruct Ok;\n") + mlc::String("template<typename E>\nstruct Err;\n") + mlc::String("template<typename T, typename E>\nusing Result = std::variant<Ok<T>, Err<E>>;\n");
mlc::String expected_body = mlc::String("template<typename T>\nstruct Ok {T field0;};\n") + mlc::String("template<typename E>\nstruct Err {E field0;};\n");
results.push_back(test_runner::assert_eq_str(mlc::String("generic type Result<T,E> = Ok(T) | Err(E) codegen"), codegen::gen_type_decl(ctx, mlc::String("Result"), result_type_params, result_variants), expected_fwd + expected_body));
return results;
}

} // namespace test_codegen
