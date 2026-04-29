#include "test_codegen.hpp"

#include "test_runner.hpp"
#include "eval.hpp"
#include "context.hpp"
#include "type_gen.hpp"
#include "module.hpp"
#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"

namespace test_codegen {

using namespace test_runner;
using namespace eval;
using namespace context;
using namespace type_gen;
using namespace module;
using namespace ast;
using namespace semantic_ir;
using namespace registry;
using namespace ast_tokens;

std::shared_ptr<registry::Type> i32_t() noexcept;

std::shared_ptr<registry::Type> str_t() noexcept;

std::shared_ptr<registry::Type> bool_t() noexcept;

std::shared_ptr<registry::Type> unit_t() noexcept;

std::shared_ptr<registry::Type> unk_t() noexcept;

std::shared_ptr<semantic_ir::SExpr> si(int v) noexcept;

std::shared_ptr<semantic_ir::SExpr> ss(mlc::String v) noexcept;

std::shared_ptr<semantic_ir::SExpr> sb(bool v) noexcept;

std::shared_ptr<semantic_ir::SExpr> sid(mlc::String name) noexcept;

std::shared_ptr<semantic_ir::SExpr> su() noexcept;

mlc::Array<test_runner::TestResult> codegen_tests() noexcept;

std::shared_ptr<registry::Type> i32_t() noexcept{return std::make_shared<registry::Type>((registry::TI32{}));}

std::shared_ptr<registry::Type> str_t() noexcept{return std::make_shared<registry::Type>((registry::TString{}));}

std::shared_ptr<registry::Type> bool_t() noexcept{return std::make_shared<registry::Type>((registry::TBool{}));}

std::shared_ptr<registry::Type> unit_t() noexcept{return std::make_shared<registry::Type>((registry::TUnit{}));}

std::shared_ptr<registry::Type> unk_t() noexcept{return std::make_shared<registry::Type>((registry::TUnknown{}));}

std::shared_ptr<semantic_ir::SExpr> si(int v) noexcept{return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprInt(v, i32_t(), ast::span_unknown()));}

std::shared_ptr<semantic_ir::SExpr> ss(mlc::String v) noexcept{return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprStr(v, str_t(), ast::span_unknown()));}

std::shared_ptr<semantic_ir::SExpr> sb(bool v) noexcept{return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBool(v, bool_t(), ast::span_unknown()));}

std::shared_ptr<semantic_ir::SExpr> sid(mlc::String name) noexcept{return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprIdent(name, unk_t(), ast::span_unknown()));}

std::shared_ptr<semantic_ir::SExpr> su() noexcept{return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUnit(unit_t(), ast::span_unknown()));}

mlc::Array<test_runner::TestResult> codegen_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
ast::Program empty_prog = ast::Program{{}};
context::CodegenContext ctx = context::create_codegen_context(empty_prog);
results.push_back(test_runner::assert_eq_str(mlc::String("create_codegen_context: gen_expr with ctx"), eval::gen_expr(si(42), ctx), mlc::String("42")));
results.push_back(test_runner::assert_eq_str(mlc::String("TyI32 - 'int'"), type_gen::type_to_cpp(ctx, std::make_shared<ast::TypeExpr>((ast::TyI32{}))), mlc::String("int")));
results.push_back(test_runner::assert_eq_str(mlc::String("TyString - 'mlc::String'"), type_gen::type_to_cpp(ctx, std::make_shared<ast::TypeExpr>((ast::TyString{}))), mlc::String("mlc::String")));
results.push_back(test_runner::assert_eq_str(mlc::String("TyBool - 'bool'"), type_gen::type_to_cpp(ctx, std::make_shared<ast::TypeExpr>((ast::TyBool{}))), mlc::String("bool")));
results.push_back(test_runner::assert_eq_str(mlc::String("TyUnit - 'void'"), type_gen::type_to_cpp(ctx, std::make_shared<ast::TypeExpr>((ast::TyUnit{}))), mlc::String("void")));
results.push_back(test_runner::assert_eq_str(mlc::String("TyArray(TyI32) - 'mlc::Array<int>'"), type_gen::type_to_cpp(ctx, std::make_shared<ast::TypeExpr>(ast::TyArray(std::make_shared<ast::TypeExpr>((ast::TyI32{}))))), mlc::String("mlc::Array<int>")));
results.push_back(test_runner::assert_eq_str(mlc::String("TyShared(TyI32) - 'std::shared_ptr<int>'"), type_gen::type_to_cpp(ctx, std::make_shared<ast::TypeExpr>(ast::TyShared(std::make_shared<ast::TypeExpr>((ast::TyI32{}))))), mlc::String("std::shared_ptr<int>")));
results.push_back(test_runner::assert_eq_str(mlc::String("sem_type TPair(i32, string)"), type_gen::sem_type_to_cpp(ctx, std::make_shared<registry::Type>(registry::TPair(std::make_shared<registry::Type>((registry::TI32{})), std::make_shared<registry::Type>((registry::TString{}))))), mlc::String("std::pair<int, mlc::String>")));
results.push_back(test_runner::assert_eq_str(mlc::String("sem_type TTuple(i32, string, bool)"), type_gen::sem_type_to_cpp(ctx, std::make_shared<registry::Type>(registry::TTuple(mlc::Array<std::shared_ptr<registry::Type>>{std::make_shared<registry::Type>((registry::TI32{})), std::make_shared<registry::Type>((registry::TString{})), std::make_shared<registry::Type>((registry::TBool{}))}))), mlc::String("std::tuple<int, mlc::String, bool>")));
results.push_back(test_runner::assert_eq_str(mlc::String("sem_type Option<i32> -> std::optional"), type_gen::sem_type_to_cpp(ctx, std::make_shared<registry::Type>(registry::TGeneric(mlc::String("Option"), mlc::Array<std::shared_ptr<registry::Type>>{std::make_shared<registry::Type>((registry::TI32{}))}))), mlc::String("std::optional<int>")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprInt(42) - '42'"), eval::gen_expr(si(42), ctx), mlc::String("42")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprQuestion: gen_expr index-based unwrap"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprQuestion(si(7), unk_t(), ast::span_unknown())), ctx), mlc::String("({ auto __q = 7; if (std::get_if<1>(&__q)) return *std::get_if<1>(&__q); std::get<0>(__q).field0; })")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprBool(true) - 'true'"), eval::gen_expr(sb(true), ctx), mlc::String("true")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprStr('hi') - 'mlc::String(\"hi\", 2)'"), eval::gen_expr(ss(mlc::String("hi")), ctx), mlc::String("mlc::String(\"hi\", 2)")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprUnit - '/* unit */'"), eval::gen_expr(su(), ctx), mlc::String("/* unit */")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprIdent('x') - 'x'"), eval::gen_expr(sid(mlc::String("x")), ctx), mlc::String("x")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprIdent('print') - 'mlc::io::print'"), eval::gen_expr(sid(mlc::String("print")), ctx), mlc::String("mlc::io::print")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprIdent('println') - 'mlc::io::println'"), eval::gen_expr(sid(mlc::String("println")), ctx), mlc::String("mlc::io::println")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprBin add - '(1 + 2)'"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBin(mlc::String("+"), si(1), si(2), i32_t(), ast::span_unknown())), ctx), mlc::String("(1 + 2)")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprUn neg - '(-42)'"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUn(mlc::String("-"), si(42), i32_t(), ast::span_unknown())), ctx), mlc::String("(-42)")));
results.push_back(test_runner::assert_eq_str(mlc::String("map_builtin passthrough - 'foo'"), cpp_naming::map_builtin(mlc::String("foo")), mlc::String("foo")));
mlc::Array<mlc::String> single_params = mlc::Array<mlc::String>{mlc::String("x")};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprLambda single param - '[=](auto x) { return ... }'"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprLambda(single_params, std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBin(mlc::String("+"), sid(mlc::String("x")), si(1), i32_t(), ast::span_unknown())), unk_t(), ast::span_unknown())), ctx), mlc::String("[=](auto x) { return (x + 1); }")));
mlc::Array<mlc::String> two_params = mlc::Array<mlc::String>{mlc::String("a"), mlc::String("b")};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprLambda two params - '[=](auto a, auto b) { return ... }'"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprLambda(two_params, std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBin(mlc::String("+"), sid(mlc::String("a")), sid(mlc::String("b")), i32_t(), ast::span_unknown())), unk_t(), ast::span_unknown())), ctx), mlc::String("[=](auto a, auto b) { return (a + b); }")));
mlc::Array<mlc::String> zero_params = {};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprLambda zero params - '[]() { return ... }'"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprLambda(zero_params, si(42), unk_t(), ast::span_unknown())), ctx), mlc::String("[]() { return 42; }")));
results.push_back(test_runner::assert_eq_str(mlc::String("map_method: length - length"), cpp_naming::map_method(mlc::String("length")), mlc::String("length")));
results.push_back(test_runner::assert_eq_str(mlc::String("map_method: len - length"), cpp_naming::map_method(mlc::String("len")), mlc::String("length")));
results.push_back(test_runner::assert_eq_str(mlc::String("map_method: push - push_back"), cpp_naming::map_method(mlc::String("push")), mlc::String("push_back")));
results.push_back(test_runner::assert_eq_str(mlc::String("map_method: upper - upper"), cpp_naming::map_method(mlc::String("upper")), mlc::String("upper")));
results.push_back(test_runner::assert_eq_str(mlc::String("map_method: to_upper - upper"), cpp_naming::map_method(mlc::String("to_upper")), mlc::String("upper")));
results.push_back(test_runner::assert_eq_str(mlc::String("map_method: to_lower - lower"), cpp_naming::map_method(mlc::String("to_lower")), mlc::String("lower")));
results.push_back(test_runner::assert_eq_str(mlc::String("map_method: trim passthrough"), cpp_naming::map_method(mlc::String("trim")), mlc::String("trim")));
results.push_back(test_runner::assert_eq_str(mlc::String("map_method: split passthrough"), cpp_naming::map_method(mlc::String("split")), mlc::String("split")));
results.push_back(test_runner::assert_eq_str(mlc::String("map_method: chars passthrough"), cpp_naming::map_method(mlc::String("chars")), mlc::String("chars")));
results.push_back(test_runner::assert_eq_str(mlc::String("map_method: lines passthrough"), cpp_naming::map_method(mlc::String("lines")), mlc::String("lines")));
mlc::Array<std::shared_ptr<semantic_ir::SExpr>> call_lambda_args = mlc::Array<std::shared_ptr<semantic_ir::SExpr>>{si(5), std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprLambda(single_params, std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBin(mlc::String("+"), sid(mlc::String("x")), si(1), i32_t(), ast::span_unknown())), unk_t(), ast::span_unknown()))};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprCall with lambda arg"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprCall(sid(mlc::String("apply")), call_lambda_args, unk_t(), ast::span_unknown())), ctx), mlc::String("apply(5, [=](auto x) { return (x + 1); })")));
mlc::Array<std::shared_ptr<semantic_ir::SExpr>> ok_args = mlc::Array<std::shared_ptr<semantic_ir::SExpr>>{si(42)};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprCall ctor Ok(42) uses brace init"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprCall(sid(mlc::String("Ok")), ok_args, unk_t(), ast::span_unknown())), ctx), mlc::String("Ok{42}")));
mlc::Array<std::shared_ptr<semantic_ir::SExpr>> err_args = mlc::Array<std::shared_ptr<semantic_ir::SExpr>>{ss(mlc::String("oops"))};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprCall ctor Err(str) uses brace init"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprCall(sid(mlc::String("Err")), err_args, unk_t(), ast::span_unknown())), ctx), mlc::String("Err{mlc::String(\"oops\", 4)}")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprIf"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprIf(sb(true), si(1), si(2), i32_t(), ast::span_unknown())), ctx), mlc::String("(true ? 1 : 2)")));
mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arr_elems = mlc::Array<std::shared_ptr<semantic_ir::SExpr>>{si(1), si(2), si(3)};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprArray"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprArray(arr_elems, std::make_shared<registry::Type>(registry::TArray(i32_t())), ast::span_unknown())), ctx), mlc::String("mlc::Array{1, 2, 3}")));
mlc::Array<std::shared_ptr<semantic_ir::SExpr>> empty_arr = {};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprArray empty"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprArray(empty_arr, std::make_shared<registry::Type>(registry::TArray(i32_t())), ast::span_unknown())), ctx), mlc::String("{}")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprIndex"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprIndex(sid(mlc::String("arr")), si(0), i32_t(), ast::span_unknown())), ctx), mlc::String("arr[0]")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprField on ident"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprField(sid(mlc::String("point")), mlc::String("x"), unk_t(), ast::span_unknown())), ctx), mlc::String("point.x")));
mlc::Array<std::shared_ptr<semantic_ir::SExpr>> method_args = mlc::Array<std::shared_ptr<semantic_ir::SExpr>>{si(1)};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprMethod push"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMethod(sid(mlc::String("arr")), mlc::String("push"), method_args, unit_t(), ast::span_unknown())), ctx), mlc::String("arr.push_back(1)")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprMethod length"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMethod(sid(mlc::String("arr")), mlc::String("length"), empty_arr, i32_t(), ast::span_unknown())), ctx), mlc::String("arr.length()")));
std::shared_ptr<registry::Type> result_r_t = std::make_shared<registry::Type>(registry::TGeneric(mlc::String("Result"), mlc::Array<std::shared_ptr<registry::Type>>{i32_t(), str_t()}));
std::shared_ptr<semantic_ir::SExpr> s_result = std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprIdent(mlc::String("r"), result_r_t, ast::span_unknown()));
mlc::Array<std::shared_ptr<semantic_ir::SExpr>> map_one = mlc::Array<std::shared_ptr<semantic_ir::SExpr>>{std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprIdent(mlc::String("f"), unk_t(), ast::span_unknown()))};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprMethod Result map"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMethod(s_result, mlc::String("map"), map_one, unk_t(), ast::span_unknown())), ctx), mlc::String("mlc::result::map(r, f)")));
mlc::HashMap<mlc::String, mlc::String> namespace_alias_prefixes = mlc::HashMap<mlc::String, mlc::String>();
namespace_alias_prefixes.set(mlc::String("helpers"), mlc::String("infer_literals::"));
context::CodegenContext context_with_aliases = context::context_with_namespace_alias_prefixes(ctx, namespace_alias_prefixes);
results.push_back(test_runner::assert_eq_str(mlc::String("ExprMethod on import-as alias emits namespace::fn()"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMethod(sid(mlc::String("helpers")), mlc::String("infer_expr_integer_literal"), empty_arr, unk_t(), ast::span_unknown())), context_with_aliases), mlc::String("infer_literals::infer_expr_integer_literal()")));
mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> pat_arms = mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>>{std::make_shared<semantic_ir::SMatchArm>(semantic_ir::SMatchArm{std::make_shared<ast::Pat>(ast::PatInt(1, ast::span_unknown())), ss(mlc::String("one"))}), std::make_shared<semantic_ir::SMatchArm>(semantic_ir::SMatchArm{std::make_shared<ast::Pat>(ast::PatWild(ast::span_unknown())), ss(mlc::String("other"))})};
mlc::String match_out = eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMatch(sid(mlc::String("n")), pat_arms, str_t(), ast::span_unknown())), ctx);
results.push_back(test_runner::assert_eq_str(mlc::String("ExprMatch contains std::visit"), match_out.contains(mlc::String("std::visit")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprMatch contains one"), match_out.contains(mlc::String("\"one\"")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprMatch contains other"), match_out.contains(mlc::String("\"other\"")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> or_arms = mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>>{std::make_shared<semantic_ir::SMatchArm>(semantic_ir::SMatchArm{std::make_shared<ast::Pat>(ast::PatOr(mlc::Array<std::shared_ptr<ast::Pat>>{std::make_shared<ast::Pat>(ast::PatCtor(mlc::String("Circle"), mlc::Array<std::shared_ptr<ast::Pat>>{std::make_shared<ast::Pat>(ast::PatWild(ast::span_unknown()))}, ast::span_unknown())), std::make_shared<ast::Pat>(ast::PatCtor(mlc::String("Square"), mlc::Array<std::shared_ptr<ast::Pat>>{std::make_shared<ast::Pat>(ast::PatWild(ast::span_unknown()))}, ast::span_unknown()))}, ast::span_unknown())), ss(mlc::String("r"))})};
mlc::String or_match_out = eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMatch(sid(mlc::String("s")), or_arms, str_t(), ast::span_unknown())), ctx);
results.push_back(test_runner::assert_eq_str(mlc::String("PatOr expanded: Circle lambda present"), or_match_out.contains(mlc::String("Circle")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
results.push_back(test_runner::assert_eq_str(mlc::String("PatOr expanded: Square lambda present"), or_match_out.contains(mlc::String("Square")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
mlc::Array<std::shared_ptr<semantic_ir::SStmt>> while_body = mlc::Array<std::shared_ptr<semantic_ir::SStmt>>{std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtExpr(si(0), ast::span_unknown()))};
mlc::String while_out = eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprWhile(sb(true), while_body, unit_t(), ast::span_unknown())), ctx);
results.push_back(test_runner::assert_eq_str(mlc::String("ExprWhile contains while (true)"), while_out.contains(mlc::String("while (true)")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
mlc::Array<std::shared_ptr<semantic_ir::SStmt>> block_stmts = mlc::Array<std::shared_ptr<semantic_ir::SStmt>>{std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtLet(mlc::String("x"), false, si(1), i32_t(), ast::span_unknown()))};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprBlock"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBlock(block_stmts, sid(mlc::String("x")), i32_t(), ast::span_unknown())), ctx), mlc::String("[&]() {\nauto x = 1;\nreturn x;\n}()")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_stmts_str: let const emits constexpr"), eval::gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>{std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtLetConst(mlc::String("limit"), si(42), i32_t(), ast::span_unknown()))}, ctx), mlc::String("constexpr auto limit = 42;\n")));
mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> fv_a = mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>>{std::make_shared<semantic_ir::SFieldVal>(semantic_ir::SFieldVal{mlc::String("a"), si(1)}), std::make_shared<semantic_ir::SFieldVal>(semantic_ir::SFieldVal{mlc::String("b"), si(2)})};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprRecord positional"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprRecord(mlc::String("Point"), fv_a, std::make_shared<registry::Type>(registry::TNamed(mlc::String("Point"))), ast::span_unknown())), ctx), mlc::String("Point{1, 2}")));
mlc::Array<std::shared_ptr<ast::Param>> fn_param = mlc::Array<std::shared_ptr<ast::Param>>{std::make_shared<ast::Param>(ast::Param{mlc::String("n"), false, std::make_shared<ast::TypeExpr>((ast::TyI32{})), false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown()))})};
std::shared_ptr<ast::Expr> fn_body = std::make_shared<ast::Expr>(ast::ExprBin(mlc::String("*"), std::make_shared<ast::Expr>(ast::ExprIdent(mlc::String("n"), ast::span_unknown())), std::make_shared<ast::Expr>(ast::ExprInt(2, ast::span_unknown())), ast::span_unknown()));
ast::Decl fn_decl = ast::DeclFn(mlc::String("double_it"), {}, {}, fn_param, std::make_shared<ast::TypeExpr>((ast::TyI32{})), fn_body);
ast::Program fn_prog = ast::Program{mlc::Array<std::shared_ptr<ast::Decl>>{std::make_shared<ast::Decl>(ast::DeclExported(std::make_shared<ast::Decl>(fn_decl)))}};
mlc::String fn_prog_out = module::gen_program(fn_prog);
results.push_back(test_runner::assert_eq_str(mlc::String("DeclFn: signature in output"), fn_prog_out.contains(mlc::String("double_it")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
results.push_back(test_runner::assert_eq_str(mlc::String("DeclFn: param type in output"), fn_prog_out.contains(mlc::String("int n")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
results.push_back(test_runner::assert_eq_str(mlc::String("DeclFn: body in output"), fn_prog_out.contains(mlc::String("return (n * 2)")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
mlc::Array<mlc::String> result_type_params = mlc::Array<mlc::String>{mlc::String("T"), mlc::String("E")};
mlc::Array<std::shared_ptr<ast::TypeExpr>> ok_fields = mlc::Array<std::shared_ptr<ast::TypeExpr>>{std::make_shared<ast::TypeExpr>(ast::TyNamed(mlc::String("T")))};
mlc::Array<std::shared_ptr<ast::TypeExpr>> err_fields = mlc::Array<std::shared_ptr<ast::TypeExpr>>{std::make_shared<ast::TypeExpr>(ast::TyNamed(mlc::String("E")))};
mlc::Array<std::shared_ptr<ast::TypeVariant>> result_variants = mlc::Array<std::shared_ptr<ast::TypeVariant>>{std::make_shared<ast::TypeVariant>(ast::VarTuple(mlc::String("Ok"), ok_fields)), std::make_shared<ast::TypeVariant>(ast::VarTuple(mlc::String("Err"), err_fields))};
mlc::String expected_fwd = mlc::String("template<typename T>\nstruct Ok;\n") + mlc::String("template<typename E>\nstruct Err;\n") + mlc::String("template<typename T, typename E>\nusing Result = std::variant<Ok<T>, Err<E>>;\n");
mlc::String expected_body = mlc::String("template<typename T>\nstruct Ok {T field0;};\n") + mlc::String("template<typename E>\nstruct Err {E field0;};\n");
results.push_back(test_runner::assert_eq_str(mlc::String("generic type Result<T,E> = Ok(T) | Err(E) codegen"), type_gen::gen_type_decl(ctx, mlc::String("Result"), result_type_params, result_variants), expected_fwd + expected_body));
results.push_back(test_runner::assert_eq_str(mlc::String("string literal with embedded newline - escapes to \\n in C++"), eval::gen_expr(ss(mlc::String("hello\nworld")), ctx), mlc::String("mlc::String(\"hello\\nworld\", 11)")));
results.push_back(test_runner::assert_eq_str(mlc::String("string literal with tab - escapes to \\t"), eval::gen_expr(ss(mlc::String("a\tb")), ctx), mlc::String("mlc::String(\"a\\tb\", 3)")));
results.push_back(test_runner::assert_eq_str(mlc::String("string literal with backslash - escapes to \\\\"), eval::gen_expr(ss(mlc::String("a\\b")), ctx), mlc::String("mlc::String(\"a\\\\b\", 3)")));
results.push_back(test_runner::assert_eq_str(mlc::String("string literal with double quote - escapes to \\\""), eval::gen_expr(ss(mlc::String("say \"hi\"")), ctx), mlc::String("mlc::String(\"say \\\"hi\\\"\", 8)")));
std::shared_ptr<ast::Pat> letpat_ctor_pat = std::make_shared<ast::Pat>(ast::PatCtor(mlc::String("Some"), mlc::Array<std::shared_ptr<ast::Pat>>{std::make_shared<ast::Pat>(ast::PatIdent(mlc::String("v"), ast::span_unknown()))}, ast::span_unknown()));
std::shared_ptr<semantic_ir::SExpr> letpat_else_body = std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprInt(0, i32_t(), ast::span_unknown()));
std::shared_ptr<semantic_ir::SStmt> letpat_with_else = std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtLetPat(letpat_ctor_pat, false, sid(mlc::String("opt")), std::make_shared<registry::Type>(registry::TNamed(mlc::String("Opt"))), true, letpat_else_body, ast::span_unknown()));
std::shared_ptr<semantic_ir::SStmt> letpat_no_else = std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtLetPat(letpat_ctor_pat, false, sid(mlc::String("opt")), std::make_shared<registry::Type>(registry::TNamed(mlc::String("Opt"))), false, letpat_else_body, ast::span_unknown()));
mlc::String letpat_with_else_out = eval::gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>{letpat_with_else}, ctx);
mlc::String letpat_no_else_out = eval::gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>{letpat_no_else}, ctx);
results.push_back(test_runner::assert_eq_str(mlc::String("let-else: with else no abort"), letpat_with_else_out.contains(mlc::String("abort")) ? mlc::String("abort") : mlc::String("no-abort"), mlc::String("no-abort")));
results.push_back(test_runner::assert_eq_str(mlc::String("let-else: without else uses abort"), letpat_no_else_out.contains(mlc::String("abort")) ? mlc::String("abort") : mlc::String("no-abort"), mlc::String("abort")));
return results;
}

} // namespace test_codegen
