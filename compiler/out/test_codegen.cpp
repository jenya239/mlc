#include "test_codegen.hpp"

#include "test_runner.hpp"
#include "codegen_test_helpers.hpp"
#include "eval.hpp"
#include "expression_support.hpp"
#include "context.hpp"
#include "type_gen.hpp"
#include "module.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "ast.hpp"
#include "semantic_ir.hpp"
#include "semantic_ir.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "registry.hpp"
#include "ctor_info.hpp"

namespace test_codegen {

using namespace test_runner;
using namespace codegen_test_helpers;
using namespace eval;
using namespace expression_support;
using namespace context;
using namespace type_gen;
using namespace module;
using namespace lexer;
using namespace decls;
using namespace ast;
using namespace semantic_ir;
using namespace semantic_ir;
using namespace semantic_ir;
using namespace registry;
using namespace registry;
using namespace ctor_info;
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

std::shared_ptr<semantic_ir::SMatchArm> smatch_arm(std::shared_ptr<ast::Pat> pattern, std::shared_ptr<semantic_ir::SExpr> body_expression) noexcept;

std::shared_ptr<semantic_ir::SMatchArm> smatch_arm_guarded(std::shared_ptr<ast::Pat> pattern, std::shared_ptr<semantic_ir::SExpr> when_condition_expression, std::shared_ptr<semantic_ir::SExpr> body_expression) noexcept;

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

std::shared_ptr<semantic_ir::SMatchArm> smatch_arm(std::shared_ptr<ast::Pat> pattern, std::shared_ptr<semantic_ir::SExpr> body_expression) noexcept{return std::make_shared<semantic_ir::SMatchArm>(semantic_ir::SMatchArm{pattern, false, std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBool(true, bool_t(), ast::span_unknown())), body_expression});}

std::shared_ptr<semantic_ir::SMatchArm> smatch_arm_guarded(std::shared_ptr<ast::Pat> pattern, std::shared_ptr<semantic_ir::SExpr> when_condition_expression, std::shared_ptr<semantic_ir::SExpr> body_expression) noexcept{return std::make_shared<semantic_ir::SMatchArm>(semantic_ir::SMatchArm{pattern, true, when_condition_expression, body_expression});}

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
results.push_back(test_runner::assert_eq_str(mlc::String("ExprLambda single param - '[=](auto x) mutable { return ... }'"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprLambda(single_params, std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBin(mlc::String("+"), sid(mlc::String("x")), si(1), i32_t(), ast::span_unknown())), unk_t(), ast::span_unknown())), ctx), mlc::String("[=](auto x) mutable { return (x + 1); }")));
std::shared_ptr<registry::Type> lambda_typed_with_single_i32_parameter = std::make_shared<registry::Type>(registry::TFn(mlc::Array<std::shared_ptr<registry::Type>>{std::make_shared<registry::Type>((registry::TI32{}))}, std::make_shared<registry::Type>((registry::TI32{}))));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprLambda annotated TFn([i32], i32) uses explicit int parameter"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprLambda(single_params, std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBin(mlc::String("+"), sid(mlc::String("x")), si(1), i32_t(), ast::span_unknown())), lambda_typed_with_single_i32_parameter, ast::span_unknown())), ctx), mlc::String("[=](int x) mutable { return (x + 1); }")));
mlc::Array<mlc::String> two_params = mlc::Array<mlc::String>{mlc::String("a"), mlc::String("b")};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprLambda two params - '[=](auto a, auto b) mutable { return ... }'"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprLambda(two_params, std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBin(mlc::String("+"), sid(mlc::String("a")), sid(mlc::String("b")), i32_t(), ast::span_unknown())), unk_t(), ast::span_unknown())), ctx), mlc::String("[=](auto a, auto b) mutable { return (a + b); }")));
mlc::Array<mlc::String> zero_params = {};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprLambda zero params - '[]() mutable { return ... }'"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprLambda(zero_params, si(42), unk_t(), ast::span_unknown())), ctx), mlc::String("[]() mutable { return 42; }")));
mlc::Array<mlc::String> three_lambda_parameters = mlc::Array<mlc::String>{mlc::String("first_parameter"), mlc::String("second_parameter"), mlc::String("third_parameter")};
results.push_back(test_runner::assert_eq_str(mlc::String("cpp_lambda_header_prefix — three identifiers"), expression_support::cpp_lambda_header_prefix(three_lambda_parameters), mlc::String("[=](auto first_parameter, auto second_parameter, auto third_parameter) mutable")));
mlc::Array<mlc::String> escaped_cpp_keyword_parameter = mlc::Array<mlc::String>{mlc::String("class")};
results.push_back(test_runner::assert_eq_str(mlc::String("cpp_lambda_header_prefix — cpp keyword parameter uses cpp_safe"), expression_support::cpp_lambda_header_prefix(escaped_cpp_keyword_parameter), mlc::String("[=](auto class_) mutable")));
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
results.push_back(test_runner::assert_eq_str(mlc::String("ExprCall with lambda arg"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprCall(sid(mlc::String("apply")), call_lambda_args, unk_t(), ast::span_unknown())), ctx), mlc::String("apply(5, [=](auto x) mutable { return (x + 1); })")));
mlc::Array<std::shared_ptr<semantic_ir::SExpr>> ok_args = mlc::Array<std::shared_ptr<semantic_ir::SExpr>>{si(42)};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprCall ctor Ok(42) uses brace init"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprCall(sid(mlc::String("Ok")), ok_args, unk_t(), ast::span_unknown())), ctx), mlc::String("Ok{42}")));
mlc::Array<std::shared_ptr<semantic_ir::SExpr>> err_args = mlc::Array<std::shared_ptr<semantic_ir::SExpr>>{ss(mlc::String("oops"))};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprCall ctor Err(str) uses brace init"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprCall(sid(mlc::String("Err")), err_args, unk_t(), ast::span_unknown())), ctx), mlc::String("Err{mlc::String(\"oops\", 4)}")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprIf"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprIf(sb(true), si(1), si(2), i32_t(), ast::span_unknown())), ctx), mlc::String("(true ? (1) : (2))")));
mlc::Array<std::shared_ptr<semantic_ir::SExpr>> arr_elems = mlc::Array<std::shared_ptr<semantic_ir::SExpr>>{si(1), si(2), si(3)};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprArray"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprArray(arr_elems, std::make_shared<registry::Type>(registry::TArray(i32_t())), ast::span_unknown())), ctx), mlc::String("mlc::Array<int>{1, 2, 3}")));
mlc::Array<std::shared_ptr<semantic_ir::SExpr>> empty_arr = {};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprArray empty"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprArray(empty_arr, std::make_shared<registry::Type>(registry::TArray(i32_t())), ast::span_unknown())), ctx), mlc::String("mlc::Array<int>{}")));
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
context::CodegenContext context_with_aliases = context::CodegenContext_with_namespace_alias_prefixes(ctx, namespace_alias_prefixes);
results.push_back(test_runner::assert_eq_str(mlc::String("ExprMethod on import-as alias emits namespace::fn()"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMethod(sid(mlc::String("helpers")), mlc::String("infer_expr_integer_literal"), empty_arr, unk_t(), ast::span_unknown())), context_with_aliases), mlc::String("infer_literals::infer_expr_integer_literal()")));
mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> pat_arms = mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>>{smatch_arm(std::make_shared<ast::Pat>(ast::PatInt(1, ast::span_unknown())), ss(mlc::String("one"))), smatch_arm(std::make_shared<ast::Pat>(ast::PatWild(ast::span_unknown())), ss(mlc::String("other")))};
mlc::String match_out = eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMatch(sid(mlc::String("n")), pat_arms, str_t(), ast::span_unknown())), ctx);
results.push_back(test_runner::assert_eq_str(mlc::String("ExprMatch contains std::visit"), match_out.contains(mlc::String("std::visit")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprMatch contains one"), match_out.contains(mlc::String("\"one\"")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprMatch contains other"), match_out.contains(mlc::String("\"other\"")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>> or_arms = mlc::Array<std::shared_ptr<semantic_ir::SMatchArm>>{smatch_arm(std::make_shared<ast::Pat>(ast::PatOr(mlc::Array<std::shared_ptr<ast::Pat>>{std::make_shared<ast::Pat>(ast::PatCtor(mlc::String("Circle"), mlc::Array<std::shared_ptr<ast::Pat>>{std::make_shared<ast::Pat>(ast::PatWild(ast::span_unknown()))}, ast::span_unknown())), std::make_shared<ast::Pat>(ast::PatCtor(mlc::String("Square"), mlc::Array<std::shared_ptr<ast::Pat>>{std::make_shared<ast::Pat>(ast::PatWild(ast::span_unknown()))}, ast::span_unknown()))}, ast::span_unknown())), ss(mlc::String("r")))};
mlc::String or_match_out = eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMatch(sid(mlc::String("s")), or_arms, str_t(), ast::span_unknown())), ctx);
results.push_back(test_runner::assert_eq_str(mlc::String("PatOr expanded: Circle lambda present"), or_match_out.contains(mlc::String("Circle")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
results.push_back(test_runner::assert_eq_str(mlc::String("PatOr expanded: Square lambda present"), or_match_out.contains(mlc::String("Square")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
mlc::String guarded_primitive_program = mlc::String("fn main() -> i32 = match 5 { n if n > 0 => 1, else => 0 }\n");
mlc::String guarded_primitive_cpp = module::gen_program(decls::parse_program(lexer::tokenize(guarded_primitive_program).tokens));
results.push_back(test_runner::assert_eq_str(mlc::String("match guard on primitive avoids std::visit"), guarded_primitive_cpp.contains(mlc::String("std::visit")) ? mlc::String("has_visit") : mlc::String("no_visit"), mlc::String("no_visit")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("match guard emits typed IIFE"), guarded_primitive_cpp, mlc::String("[&]() -> int")));
mlc::String plain_sum_program = mlc::String("type T = A(i32) | B\nfn f(x: T) -> i32 = match x | A(n) => n | B => 0 end\nfn main() -> i32 = f(A(1))\n");
mlc::String plain_sum_cpp = module::gen_program(decls::parse_program(lexer::tokenize(plain_sum_program).tokens));
results.push_back(test_runner::assert_eq_str(mlc::String("match sum without guard keeps std::visit"), plain_sum_cpp.contains(mlc::String("std::visit")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
mlc::String guarded_sum_program = mlc::String("type U = A(i32) | B\nfn g(x: U) -> i32 = match x | A(n) if n > 0 => n | A(_) => 0 | B => -1 end\nfn main() -> i32 = g(A(3))\n");
mlc::String guarded_sum_cpp = module::gen_program(decls::parse_program(lexer::tokenize(guarded_sum_program).tokens));
results.push_back(test_runner::assert_eq_str(mlc::String("match sum with guard avoids std::visit"), guarded_sum_cpp.contains(mlc::String("std::visit")) ? mlc::String("has_visit") : mlc::String("no_visit"), mlc::String("no_visit")));
mlc::String mixed_guard_program = mlc::String("type V = A(i32) | B\nfn h(x: V) -> i32 = match x | A(n) => n | B if true => 0 end\nfn main() -> i32 = h(B)\n");
mlc::String mixed_guard_cpp = module::gen_program(decls::parse_program(lexer::tokenize(mixed_guard_program).tokens));
results.push_back(test_runner::assert_eq_str(mlc::String("match mixed arms use guarded lowering when any guard present"), mixed_guard_cpp.contains(mlc::String("std::visit")) ? mlc::String("has_visit") : mlc::String("no_visit"), mlc::String("no_visit")));
mlc::Array<std::shared_ptr<semantic_ir::SStmt>> while_body = mlc::Array<std::shared_ptr<semantic_ir::SStmt>>{std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtExpr(si(0), ast::span_unknown()))};
mlc::String while_out = eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprWhile(sb(true), while_body, unit_t(), ast::span_unknown())), ctx);
results.push_back(test_runner::assert_eq_str(mlc::String("ExprWhile contains while (true)"), while_out.contains(mlc::String("while (true)")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
mlc::Array<std::shared_ptr<semantic_ir::SStmt>> block_stmts = mlc::Array<std::shared_ptr<semantic_ir::SStmt>>{std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtLet(mlc::String("x"), false, si(1), i32_t(), ast::span_unknown()))};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprBlock"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBlock(block_stmts, sid(mlc::String("x")), i32_t(), ast::span_unknown())), ctx), mlc::String("[&]() {\nauto x = 1;\nreturn x;\n}()")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_stmts_str: let const emits constexpr"), eval::gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>{std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtLetConst(mlc::String("limit"), si(42), i32_t(), ast::span_unknown()))}, ctx), mlc::String("constexpr auto limit = 42;\n")));
mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> fv_a = mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>>{std::make_shared<semantic_ir::SFieldVal>(semantic_ir::SFieldVal{mlc::String("a"), si(1)}), std::make_shared<semantic_ir::SFieldVal>(semantic_ir::SFieldVal{mlc::String("b"), si(2)})};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprRecord positional"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprRecord(mlc::String("Point"), fv_a, std::make_shared<registry::Type>(registry::TNamed(mlc::String("Point"))), ast::span_unknown())), ctx), mlc::String("Point{1, 2}")));
mlc::Array<std::shared_ptr<ast::Param>> fn_param = mlc::Array<std::shared_ptr<ast::Param>>{std::make_shared<ast::Param>(ast::Param{mlc::String("n"), false, std::make_shared<ast::TypeExpr>((ast::TyI32{})), false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())), std::make_shared<ast::Pat>(ast::PatUnit(ast::span_unknown()))})};
std::shared_ptr<ast::Expr> fn_body = std::make_shared<ast::Expr>(ast::ExprBin(mlc::String("*"), std::make_shared<ast::Expr>(ast::ExprIdent(mlc::String("n"), ast::span_unknown())), std::make_shared<ast::Expr>(ast::ExprInt(2, ast::span_unknown())), ast::span_unknown()));
ast::Decl fn_decl = ast::DeclFn(mlc::String("double_it"), {}, {}, fn_param, std::make_shared<ast::TypeExpr>((ast::TyI32{})), fn_body, {});
ast::Program fn_prog = ast::Program{mlc::Array<std::shared_ptr<ast::Decl>>{std::make_shared<ast::Decl>(ast::DeclExported(std::make_shared<ast::Decl>(fn_decl)))}};
mlc::String fn_prog_out = module::gen_program(fn_prog);
results.push_back(test_runner::assert_eq_str(mlc::String("DeclFn: signature in output"), fn_prog_out.contains(mlc::String("double_it")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
results.push_back(test_runner::assert_eq_str(mlc::String("DeclFn: param type in output"), fn_prog_out.contains(mlc::String("int n")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
results.push_back(test_runner::assert_eq_str(mlc::String("DeclFn: body in output"), fn_prog_out.contains(mlc::String("return (n * 2)")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
mlc::String d3_trait_parameter_source = mlc::String("type Display { fn to_string(self: Self) -> string }\nfn f(x: Display) -> unit = ()");
mlc::String d3_trait_parameter_output = module::gen_program(decls::parse_program(lexer::tokenize(d3_trait_parameter_source).tokens));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("D3 gen_program: concept Display"), d3_trait_parameter_output, mlc::String("concept Display")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("D3 gen_program: requires in trait concept"), d3_trait_parameter_output, mlc::String("requires")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("D3 gen_program: parameter uses Display constraint"), d3_trait_parameter_output, mlc::String("f(Display")));
mlc::String d3_two_trait_parameter_source = mlc::String("type Display { fn to_string(self: Self) -> string }\nfn g(first: Display, second: Display) -> unit = ()");
mlc::String d3_two_trait_parameter_output = module::gen_program(decls::parse_program(lexer::tokenize(d3_two_trait_parameter_source).tokens));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("D3 gen_program: first Display parameter"), d3_two_trait_parameter_output, mlc::String("Display first")));
mlc::String d4_where_trait_bounds_source = mlc::String("type Display { fn to_string(self: Self) -> string }\ntype EqCompare { fn same(self: Self, other: Self) -> bool }\nfn f<T>(x: T) -> unit where T: Display + EqCompare = ()");
mlc::String d4_where_trait_bounds_output = module::gen_program(decls::parse_program(lexer::tokenize(d4_where_trait_bounds_source).tokens));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("D4 gen_program: requires Display"), d4_where_trait_bounds_output, mlc::String("Display")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("D4 gen_program: requires EqCompare"), d4_where_trait_bounds_output, mlc::String("EqCompare")));
mlc::Array<mlc::String> result_type_params = mlc::Array<mlc::String>{mlc::String("T"), mlc::String("E")};
mlc::Array<std::shared_ptr<ast::TypeExpr>> ok_fields = mlc::Array<std::shared_ptr<ast::TypeExpr>>{std::make_shared<ast::TypeExpr>(ast::TyNamed(mlc::String("T")))};
mlc::Array<std::shared_ptr<ast::TypeExpr>> err_fields = mlc::Array<std::shared_ptr<ast::TypeExpr>>{std::make_shared<ast::TypeExpr>(ast::TyNamed(mlc::String("E")))};
mlc::Array<std::shared_ptr<ast::TypeVariant>> result_variants = mlc::Array<std::shared_ptr<ast::TypeVariant>>{std::make_shared<ast::TypeVariant>(ast::VarTuple(mlc::String("Ok"), ok_fields, false)), std::make_shared<ast::TypeVariant>(ast::VarTuple(mlc::String("Err"), err_fields, false))};
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
mlc::Array<std::shared_ptr<ast::FieldDef>> phantom_fds = mlc::Array<std::shared_ptr<ast::FieldDef>>{std::make_shared<ast::FieldDef>(ast::FieldDef{mlc::String("value"), std::make_shared<ast::TypeExpr>((ast::TyI32{})), false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown()))})};
mlc::Array<std::shared_ptr<ast::TypeVariant>> phantom_variants_1 = mlc::Array<std::shared_ptr<ast::TypeVariant>>{std::make_shared<ast::TypeVariant>(ast::VarRecord(mlc::String("Ast"), phantom_fds, false))};
mlc::String phantom_type_out = type_gen::gen_type_decl(ctx, mlc::String("Ast"), mlc::Array<mlc::String>{mlc::String("Phase")}, phantom_variants_1);
results.push_back(test_runner::assert_eq_str(mlc::String("phantom single-variant: template<typename Phase> in output"), phantom_type_out.contains(mlc::String("typename Phase")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
mlc::Array<std::shared_ptr<ast::TypeVariant>> tagged_variants = mlc::Array<std::shared_ptr<ast::TypeVariant>>{std::make_shared<ast::TypeVariant>(ast::VarTuple(mlc::String("Active"), mlc::Array<std::shared_ptr<ast::TypeExpr>>{std::make_shared<ast::TypeExpr>((ast::TyI32{}))}, false)), std::make_shared<ast::TypeVariant>(ast::VarUnit(mlc::String("Inactive"), false))};
mlc::String tagged_out = type_gen::gen_type_decl(ctx, mlc::String("Tagged"), mlc::Array<mlc::String>{mlc::String("Tag")}, tagged_variants);
results.push_back(test_runner::assert_eq_str(mlc::String("phantom multi-variant: Active<Tag> in alias"), tagged_out.contains(mlc::String("Active<Tag>")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
results.push_back(test_runner::assert_eq_str(mlc::String("phantom multi-variant: template<typename Tag> on Active struct"), tagged_out.contains(mlc::String("template<typename Tag>\nstruct Active")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
std::shared_ptr<registry::Type> phantom_rec_type = std::make_shared<registry::Type>(registry::TGeneric(mlc::String("Ast"), mlc::Array<std::shared_ptr<registry::Type>>{std::make_shared<registry::Type>(registry::TNamed(mlc::String("Unvalidated")))}));
mlc::Array<std::shared_ptr<semantic_ir::SFieldVal>> phantom_rec_fvals = {};
std::shared_ptr<semantic_ir::SExpr> phantom_rec_expr = std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprRecord(mlc::String("Ast"), phantom_rec_fvals, phantom_rec_type, ast::span_unknown()));
results.push_back(test_runner::assert_eq_str(mlc::String("phantom record expr: Ast<Unvalidated>{}"), eval::gen_expr(phantom_rec_expr, ctx), mlc::String("Ast<Unvalidated>{}")));
auto f64_t = std::make_shared<registry::Type>((registry::TF64{}));
auto i64_t = std::make_shared<registry::Type>((registry::TI64{}));
auto u8_t = std::make_shared<registry::Type>((registry::TU8{}));
auto usize_t = std::make_shared<registry::Type>((registry::TUsize{}));
auto char_t = std::make_shared<registry::Type>((registry::TChar{}));
results.push_back(test_runner::assert_eq_str(mlc::String("c4: f64 literal codegen"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprFloat(mlc::String("3.14"), f64_t, ast::span_unknown())), ctx), mlc::String("3.14")));
results.push_back(test_runner::assert_eq_str(mlc::String("c4: i64 literal codegen"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprI64(mlc::String("42"), i64_t, ast::span_unknown())), ctx), mlc::String("42")));
results.push_back(test_runner::assert_eq_str(mlc::String("c4: u8 literal codegen"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprU8(mlc::String("255"), u8_t, ast::span_unknown())), ctx), mlc::String("static_cast<uint8_t>(255)")));
results.push_back(test_runner::assert_eq_str(mlc::String("c4: usize literal codegen"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUsize(mlc::String("100"), usize_t, ast::span_unknown())), ctx), mlc::String("static_cast<size_t>(100)")));
results.push_back(test_runner::assert_eq_str(mlc::String("c4: char literal codegen"), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprChar(mlc::String("a"), char_t, ast::span_unknown())), ctx), mlc::String("static_cast<char32_t>(a)")));
results.push_back(test_runner::assert_eq_str(mlc::String("c4: sem_type_to_cpp TI64"), type_gen::sem_type_to_cpp(ctx, i64_t), mlc::String("int64_t")));
results.push_back(test_runner::assert_eq_str(mlc::String("c4: sem_type_to_cpp TF64"), type_gen::sem_type_to_cpp(ctx, f64_t), mlc::String("double")));
results.push_back(test_runner::assert_eq_str(mlc::String("c4: sem_type_to_cpp TU8"), type_gen::sem_type_to_cpp(ctx, u8_t), mlc::String("uint8_t")));
results.push_back(test_runner::assert_eq_str(mlc::String("c4: sem_type_to_cpp TUsize"), type_gen::sem_type_to_cpp(ctx, usize_t), mlc::String("size_t")));
results.push_back(test_runner::assert_eq_str(mlc::String("c4: sem_type_to_cpp TChar"), type_gen::sem_type_to_cpp(ctx, char_t), mlc::String("char32_t")));
mlc::Array<std::shared_ptr<semantic_ir::SStmt>> with_body = mlc::Array<std::shared_ptr<semantic_ir::SStmt>>{std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtExpr(si(0), ast::span_unknown()))};
mlc::String with_out = eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprWith(si(1), mlc::String("res"), with_body, unit_t(), ast::span_unknown())), ctx);
results.push_back(test_runner::assert_eq_str(mlc::String("c5: SExprWith contains auto res ="), with_out.contains(mlc::String("auto res =")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
results.push_back(test_runner::assert_eq_str(mlc::String("c5: SExprWith contains res.drop()"), with_out.contains(mlc::String("res.drop()")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
std::shared_ptr<registry::Type> assoc_t = std::make_shared<registry::Type>(registry::TAssoc(mlc::String("I"), mlc::String("Item")));
results.push_back(test_runner::assert_eq_str(mlc::String("d1: sem_type_to_cpp TAssoc"), type_gen::sem_type_to_cpp(ctx, assoc_t), mlc::String("typename I::Item")));
mlc::String assoc_prog_src = mlc::String("type Iter { type Item } type Box<T> = { val: T } extend Box<T>: Iter { type Item = T fn Iter_next(self: Box<T>) -> i32 = 0 }");
mlc::String assoc_prog_out = module::gen_program(decls::parse_program(lexer::tokenize(assoc_prog_src).tokens));
results.push_back(test_runner::assert_eq_str(mlc::String("d1: gen_program struct contains using Item ="), assoc_prog_out.contains(mlc::String("using Item =")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
mlc::String op_prog_src = mlc::String("type Vec2 = { x: i32, y: i32 } extend Vec2 : Add<Vec2> { type Output = Vec2 fn add(self: Vec2, rhs: Vec2) -> Vec2 = Vec2 { x: self.x + rhs.x, y: self.y + rhs.y } } fn test(a: Vec2, b: Vec2) -> Vec2 = a + b fn main() -> i32 = 0");
mlc::String op_prog_out = module::gen_program(decls::parse_program(lexer::tokenize(op_prog_src).tokens));
results.push_back(test_runner::assert_eq_str(mlc::String("d2: gen_program Vec2 + Vec2 desugars to Vec2_add"), op_prog_out.contains(mlc::String("Vec2_add(")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
mlc::String codegen_invariant_tail_after_else_if_chain = mlc::String("export fn tail_after_chain(kind: string, extra: i32) -> i32 = do\n  let mut accumulator = 0\n  if kind == \"a\" then\n    accumulator = 1\n  else if kind == \"b\" then\n    accumulator = 2\n  else\n    accumulator = 3\n  end\n  accumulator + extra\nend\n\nexport fn main() -> i32 = tail_after_chain(\"a\", 100)");
mlc::String codegen_tail_chain_cpp = module::gen_program(decls::parse_program(lexer::tokenize(codegen_invariant_tail_after_else_if_chain).tokens));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_program: else-if chain keeps trailing tail expr"), codegen_tail_chain_cpp, mlc::String("accumulator + extra")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_program: else-if chain emits return of tail"), codegen_tail_chain_cpp, mlc::String("return")));
results.push_back(codegen_test_helpers::assert_code_not_contains(mlc::String("gen_program: else-if chain has no stray export marker"), codegen_tail_chain_cpp, mlc::String("export_;")));
mlc::String codegen_invariant_twin_if_zip_join_style = mlc::String("export fn twin_branch(kind: string, has_arg: bool) -> i32 = do\n  let mut diagnostics_weight = 0\n  if has_arg then\n    if kind == \"zip\" then\n      diagnostics_weight = 1\n    end\n    if kind == \"join\" then\n      diagnostics_weight = diagnostics_weight + 2\n    end\n  end\n  diagnostics_weight + 50\nend\n\nexport fn main() -> i32 = twin_branch(\"zip\", true)");
mlc::String codegen_twin_if_cpp = module::gen_program(decls::parse_program(lexer::tokenize(codegen_invariant_twin_if_zip_join_style).tokens));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_program: twin if emits final accumulator tail"), codegen_twin_if_cpp, mlc::String("diagnostics_weight + 50")));
results.push_back(codegen_test_helpers::assert_code_not_contains(mlc::String("gen_program: twin if has no stray export marker"), codegen_twin_if_cpp, mlc::String("export_;")));
mlc::String codegen_invariant_if_else_flattened = mlc::String("fn merge_unknown_inner(then_inner: i32, else_errors: i32) -> i32 = do\n  if then_inner == 0 then else_errors + 10\n  else then_inner + else_errors\n  end\nend\n\nexport fn conditional_flat(use_merge: bool, then_inner: i32, else_errors: i32) -> i32 = do\n  if !use_merge then else_errors\n  else merge_unknown_inner(then_inner, else_errors)\n  end\nend\n\nexport fn main() -> i32 = conditional_flat(true, 0, 5)");
mlc::String codegen_if_else_flat_cpp = module::gen_program(decls::parse_program(lexer::tokenize(codegen_invariant_if_else_flattened).tokens));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_program: flattened else branch references helper"), codegen_if_else_flat_cpp, mlc::String("merge_unknown_inner")));
results.push_back(codegen_test_helpers::assert_code_not_contains(mlc::String("gen_program: flattened if/else has no stray export marker"), codegen_if_else_flat_cpp, mlc::String("export_;")));
ast::Program point_type = decls::parse_program(lexer::tokenize(mlc::String("type Point = Point(i32, Shared<i32>)")).tokens);
mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> point_infos = ctor_info::build_ctor_type_infos_from_decls(point_type.decls);
results.push_back(test_runner::assert_eq_int(mlc::String("ctor_info: build from sum type — 1 info"), point_infos.size(), 1));
results.push_back(test_runner::assert_eq_str(mlc::String("ctor_info: build — correct name"), point_infos[0]->name, mlc::String("Point")));
results.push_back(test_runner::assert_eq_int(mlc::String("ctor_info: build — shared_pos has 1 entry"), point_infos[0]->shared_pos.size(), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("ctor_info: build — shared_pos[0] == 1"), point_infos[0]->shared_pos[0], 1));
results.push_back(test_runner::assert_eq_int(mlc::String("ctor_info: build — shared_arr_pos empty"), point_infos[0]->shared_arr_pos.size(), 0));
std::shared_ptr<ctor_info::CtorTypeInfo> found_info = ctor_info::lookup_ctor_type_info(point_infos, mlc::String("Point"));
results.push_back(test_runner::assert_eq_str(mlc::String("ctor_info: lookup — finds by name"), found_info->name, mlc::String("Point")));
std::shared_ptr<ctor_info::CtorTypeInfo> missing_info = ctor_info::lookup_ctor_type_info(point_infos, mlc::String("Missing"));
results.push_back(test_runner::assert_eq_str(mlc::String("ctor_info: lookup — missing returns empty name"), missing_info->name, mlc::String("")));
ast::Program multi_type = decls::parse_program(lexer::tokenize(mlc::String("type Tree = Leaf | Node(Shared<i32>, [Shared<i32>])")).tokens);
mlc::Array<std::shared_ptr<ctor_info::CtorTypeInfo>> multi_infos = ctor_info::build_ctor_type_infos_from_decls(multi_type.decls);
results.push_back(test_runner::assert_eq_int(mlc::String("ctor_info: multi-variant — 1 info (VarUnit excluded)"), multi_infos.size(), 1));
std::shared_ptr<ctor_info::CtorTypeInfo> node_info = ctor_info::lookup_ctor_type_info(multi_infos, mlc::String("Node"));
results.push_back(test_runner::assert_eq_int(mlc::String("ctor_info: Node shared_pos — 1"), node_info->shared_pos.size(), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("ctor_info: Node shared_arr_pos — 1"), node_info->shared_arr_pos.size(), 1));
return results;
}

} // namespace test_codegen
