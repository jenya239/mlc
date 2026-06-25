#include "test_codegen.hpp"

#include "test_runner.hpp"
#include "codegen_test_helpers.hpp"
#include "eval.hpp"
#include "expression_support.hpp"
#include "expr.hpp"
#include "context.hpp"
#include "type_gen.hpp"
#include "codegen_harness.hpp"
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
using namespace expr;
using namespace context;
using namespace type_gen;
using namespace codegen_harness;
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

std::shared_ptr<semantic_ir::SemanticExpression> si(int v) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> ss(mlc::String v) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> sb(bool v) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> sid(mlc::String name) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> su() noexcept;

mlc::Array<int> empty_parameter_mutability_pattern() noexcept;

std::shared_ptr<semantic_ir::SemanticMatchArm> smatch_arm(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<semantic_ir::SemanticExpression> body_expression) noexcept;

std::shared_ptr<semantic_ir::SemanticMatchArm> smatch_arm_guarded(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<semantic_ir::SemanticExpression> when_condition_expression, std::shared_ptr<semantic_ir::SemanticExpression> body_expression) noexcept;

mlc::Array<test_runner::TestResult> codegen_tests() noexcept;

std::shared_ptr<registry::Type> i32_t() noexcept{return std::make_shared<registry::Type>((registry::TI32{}));}

std::shared_ptr<registry::Type> str_t() noexcept{return std::make_shared<registry::Type>((registry::TString{}));}

std::shared_ptr<registry::Type> bool_t() noexcept{return std::make_shared<registry::Type>((registry::TBool{}));}

std::shared_ptr<registry::Type> unit_t() noexcept{return std::make_shared<registry::Type>((registry::TUnit{}));}

std::shared_ptr<registry::Type> unk_t() noexcept{return std::make_shared<registry::Type>((registry::TUnknown{}));}

std::shared_ptr<semantic_ir::SemanticExpression> si(int v) noexcept{return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionInt(v, i32_t(), ast::span_unknown()));}

std::shared_ptr<semantic_ir::SemanticExpression> ss(mlc::String v) noexcept{return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionStr(v, str_t(), ast::span_unknown()));}

std::shared_ptr<semantic_ir::SemanticExpression> sb(bool v) noexcept{return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBool(v, bool_t(), ast::span_unknown()));}

std::shared_ptr<semantic_ir::SemanticExpression> sid(mlc::String name) noexcept{return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionIdent(name, unk_t(), ast::span_unknown()));}

std::shared_ptr<semantic_ir::SemanticExpression> su() noexcept{return std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionUnit(unit_t(), ast::span_unknown()));}

mlc::Array<int> empty_parameter_mutability_pattern() noexcept{
mlc::Array<int> pattern = {};
return pattern;
}

std::shared_ptr<semantic_ir::SemanticMatchArm> smatch_arm(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<semantic_ir::SemanticExpression> body_expression) noexcept{return std::make_shared<semantic_ir::SemanticMatchArm>(semantic_ir::SemanticMatchArm{pattern, false, std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBool(true, bool_t(), ast::span_unknown())), body_expression});}

std::shared_ptr<semantic_ir::SemanticMatchArm> smatch_arm_guarded(std::shared_ptr<ast::Pattern> pattern, std::shared_ptr<semantic_ir::SemanticExpression> when_condition_expression, std::shared_ptr<semantic_ir::SemanticExpression> body_expression) noexcept{return std::make_shared<semantic_ir::SemanticMatchArm>(semantic_ir::SemanticMatchArm{pattern, true, when_condition_expression, body_expression});}

mlc::Array<test_runner::TestResult> codegen_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
ast::Program empty_program = ast::Program{{}};
context::CodegenContext context = context::create_codegen_context(empty_program);
results.push_back(test_runner::assert_eq_str(mlc::String("create_codegen_context: gen_expr with ctx"), eval::gen_expr(si(42), context), mlc::String("42")));
results.push_back(test_runner::assert_eq_str(mlc::String("TyI32 - 'int'"), type_gen::type_to_cpp(context, std::make_shared<ast::TypeExpr>((ast::TyI32{}))), mlc::String("int")));
results.push_back(test_runner::assert_eq_str(mlc::String("TyString - 'mlc::String'"), type_gen::type_to_cpp(context, std::make_shared<ast::TypeExpr>((ast::TyString{}))), mlc::String("mlc::String")));
results.push_back(test_runner::assert_eq_str(mlc::String("TyBool - 'bool'"), type_gen::type_to_cpp(context, std::make_shared<ast::TypeExpr>((ast::TyBool{}))), mlc::String("bool")));
results.push_back(test_runner::assert_eq_str(mlc::String("TyUnit - 'void'"), type_gen::type_to_cpp(context, std::make_shared<ast::TypeExpr>((ast::TyUnit{}))), mlc::String("void")));
results.push_back(test_runner::assert_eq_str(mlc::String("TyArray(TyI32) - 'mlc::Array<int>'"), type_gen::type_to_cpp(context, std::make_shared<ast::TypeExpr>(ast::TyArray(std::make_shared<ast::TypeExpr>((ast::TyI32{}))))), mlc::String("mlc::Array<int>")));
results.push_back(test_runner::assert_eq_str(mlc::String("TyShared(TyI32) - 'std::shared_ptr<int>'"), type_gen::type_to_cpp(context, std::make_shared<ast::TypeExpr>(ast::TyShared(std::make_shared<ast::TypeExpr>((ast::TyI32{}))))), mlc::String("std::shared_ptr<int>")));
results.push_back(test_runner::assert_eq_str(mlc::String("sem_type TPair(i32, string)"), type_gen::sem_type_to_cpp(context, std::make_shared<registry::Type>(registry::TPair(std::make_shared<registry::Type>((registry::TI32{})), std::make_shared<registry::Type>((registry::TString{}))))), mlc::String("std::pair<int, mlc::String>")));
results.push_back(test_runner::assert_eq_str(mlc::String("sem_type TTuple(i32, string, bool)"), type_gen::sem_type_to_cpp(context, std::make_shared<registry::Type>(registry::TTuple(mlc::Array<std::shared_ptr<registry::Type>>{std::make_shared<registry::Type>((registry::TI32{})), std::make_shared<registry::Type>((registry::TString{})), std::make_shared<registry::Type>((registry::TBool{}))}))), mlc::String("std::tuple<int, mlc::String, bool>")));
results.push_back(test_runner::assert_eq_str(mlc::String("sem_type Option<i32> -> std::optional"), type_gen::sem_type_to_cpp(context, std::make_shared<registry::Type>(registry::TGeneric(mlc::String("Option"), mlc::Array<std::shared_ptr<registry::Type>>{std::make_shared<registry::Type>((registry::TI32{}))}))), mlc::String("std::optional<int>")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprInt(42) - '42'"), eval::gen_expr(si(42), context), mlc::String("42")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprQuestion: gen_expr index-based unwrap"), eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionQuestion(si(7), unk_t(), ast::span_unknown())), context), mlc::String("({ auto __q = 7; if (std::get_if<1>(&__q)) return *std::get_if<1>(&__q); std::get<0>(__q).field0; })")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprBool(true) - 'true'"), eval::gen_expr(sb(true), context), mlc::String("true")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprStr('hi') - 'mlc::String(\"hi\", 2)'"), eval::gen_expr(ss(mlc::String("hi")), context), mlc::String("mlc::String(\"hi\", 2)")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprUnit - 'std::make_tuple()'"), eval::gen_expr(su(), context), mlc::String("std::make_tuple()")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprIdent('x') - 'x'"), eval::gen_expr(sid(mlc::String("x")), context), mlc::String("x")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprIdent('print') - 'mlc::io::print'"), eval::gen_expr(sid(mlc::String("print")), context), mlc::String("mlc::io::print")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprIdent('println') - 'mlc::io::println'"), eval::gen_expr(sid(mlc::String("println")), context), mlc::String("mlc::io::println")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprBin add - '(1 + 2)'"), eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBin(mlc::String("+"), si(1), si(2), i32_t(), ast::span_unknown())), context), mlc::String("(1 + 2)")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprUn neg - '(-42)'"), eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionUn(mlc::String("-"), si(42), i32_t(), ast::span_unknown())), context), mlc::String("(-42)")));
results.push_back(test_runner::assert_eq_str(mlc::String("map_builtin passthrough - 'foo'"), cpp_naming::map_builtin(mlc::String("foo")), mlc::String("foo")));
mlc::Array<mlc::String> single_params = mlc::Array<mlc::String>{mlc::String("x")};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprLambda single param - '[=](auto x) mutable { return ... }'"), eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionLambda(single_params, std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBin(mlc::String("+"), sid(mlc::String("x")), si(1), i32_t(), ast::span_unknown())), unk_t(), ast::span_unknown())), context), mlc::String("[=](auto x) mutable { return (x + 1); }")));
std::shared_ptr<registry::Type> lambda_typed_with_single_i32_parameter = std::make_shared<registry::Type>(registry::TFn(mlc::Array<std::shared_ptr<registry::Type>>{std::make_shared<registry::Type>((registry::TI32{}))}, std::make_shared<registry::Type>((registry::TI32{}))));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprLambda annotated TFn([i32], i32) uses explicit int parameter"), eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionLambda(single_params, std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBin(mlc::String("+"), sid(mlc::String("x")), si(1), i32_t(), ast::span_unknown())), lambda_typed_with_single_i32_parameter, ast::span_unknown())), context), mlc::String("[=](int x) mutable { return (x + 1); }")));
mlc::Array<mlc::String> two_params = mlc::Array<mlc::String>{mlc::String("a"), mlc::String("b")};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprLambda two params - '[=](auto a, auto b) mutable { return ... }'"), eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionLambda(two_params, std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBin(mlc::String("+"), sid(mlc::String("a")), sid(mlc::String("b")), i32_t(), ast::span_unknown())), unk_t(), ast::span_unknown())), context), mlc::String("[=](auto a, auto b) mutable { return (a + b); }")));
mlc::Array<mlc::String> zero_params = {};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprLambda zero params - '[]() mutable { return ... }'"), eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionLambda(zero_params, si(42), unk_t(), ast::span_unknown())), context), mlc::String("[]() mutable { return 42; }")));
mlc::String stored_lambda_program = mlc::String("fn make_checker(allowed: [string]) -> i32 = do\n  const checker = fn word => if allowed.contains(word) then 1 else 0 end\n  checker(\"fn\")\nend\nfn main() -> i32 = make_checker([\"fn\"])\n");
mlc::String stored_lambda_cpp = codegen_harness::gen_program(decls::parse_program(lexer::tokenize(stored_lambda_program).tokens));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("stored lambda make_checker uses value capture"), stored_lambda_cpp, mlc::String("[=](auto word)")));
results.push_back(codegen_test_helpers::assert_code_not_contains(mlc::String("stored lambda make_checker avoids reference capture"), stored_lambda_cpp, mlc::String("[&](auto word)")));
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
mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> call_lambda_args = mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>>{si(5), std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionLambda(single_params, std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBin(mlc::String("+"), sid(mlc::String("x")), si(1), i32_t(), ast::span_unknown())), unk_t(), ast::span_unknown()))};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprCall with lambda arg"), eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionCall(sid(mlc::String("apply")), call_lambda_args, empty_parameter_mutability_pattern(), unk_t(), ast::span_unknown())), context), mlc::String("apply(5, [=](auto x) mutable { return (x + 1); })")));
mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> ok_args = mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>>{si(42)};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprCall ctor Ok(42) uses brace init"), eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionCall(sid(mlc::String("Ok")), ok_args, empty_parameter_mutability_pattern(), unk_t(), ast::span_unknown())), context), mlc::String("Ok{42}")));
mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> err_args = mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>>{ss(mlc::String("oops"))};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprCall ctor Err(str) uses brace init"), eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionCall(sid(mlc::String("Err")), err_args, empty_parameter_mutability_pattern(), unk_t(), ast::span_unknown())), context), mlc::String("Err{mlc::String(\"oops\", 4)}")));
mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> empty_arr = {};
mlc::Array<int> mut_actual_first_parameter = {};
mut_actual_first_parameter.push_back(1);
std::shared_ptr<semantic_ir::SemanticExpression> inner_call_under_mut_actual_test = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionCall(sid(mlc::String("spawn_inner")), empty_arr, empty_parameter_mutability_pattern(), unk_t(), ast::span_unknown()));
std::shared_ptr<semantic_ir::SemanticExpression> outer_under_mut_actual_test_expression = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionCall(sid(mlc::String("consume_mut_first")), mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>>{inner_call_under_mut_actual_test}, mut_actual_first_parameter, unk_t(), ast::span_unknown()));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("mutable first positional actual argument lowers with holder IIFE"), eval::gen_expr(outer_under_mut_actual_test_expression, context), mlc::String("__mut_actual_argument_holder_0")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("mutable first positional actual argument lowers with return prelude"), eval::gen_expr(outer_under_mut_actual_test_expression, context), mlc::String("[&]()")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("mutable first positional actual argument holder auto decl"), eval::gen_expr(outer_under_mut_actual_test_expression, context), mlc::String("auto __mut_actual_argument_holder_0")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprIf"), eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionIf(sb(true), si(1), si(2), i32_t(), ast::span_unknown())), context), mlc::String("(true ? (1) : (2))")));
mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arr_elems = mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>>{si(1), si(2), si(3)};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprArray"), eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionArray(arr_elems, std::make_shared<registry::Type>(registry::TArray(i32_t())), ast::span_unknown())), context), mlc::String("mlc::Array<int>{1, 2, 3}")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprArray empty"), eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionArray(empty_arr, std::make_shared<registry::Type>(registry::TArray(i32_t())), ast::span_unknown())), context), mlc::String("mlc::Array<int>{}")));
mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> tuple_elements = mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>>{si(1), si(2)};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprTuple - std::make_tuple(1, 2)"), eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionTuple(tuple_elements, std::make_shared<registry::Type>(registry::TTuple(mlc::Array<std::shared_ptr<registry::Type>>{std::make_shared<registry::Type>((registry::TI32{})), std::make_shared<registry::Type>((registry::TI32{}))})), ast::span_unknown())), context), mlc::String("std::make_tuple(1, 2)")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprIndex"), eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionIndex(sid(mlc::String("arr")), si(0), i32_t(), ast::span_unknown())), context), mlc::String("arr[0]")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprField on ident"), eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionField(sid(mlc::String("point")), mlc::String("x"), unk_t(), ast::span_unknown())), context), mlc::String("point.x")));
mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> method_args = mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>>{si(1)};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprMethod push"), eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionMethod(sid(mlc::String("arr")), mlc::String("push"), method_args, empty_parameter_mutability_pattern(), unit_t(), ast::span_unknown())), context), mlc::String("arr.push_back(1)")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprMethod length"), eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionMethod(sid(mlc::String("arr")), mlc::String("length"), empty_arr, empty_parameter_mutability_pattern(), i32_t(), ast::span_unknown())), context), mlc::String("arr.length()")));
std::shared_ptr<registry::Type> result_r_t = std::make_shared<registry::Type>(registry::TGeneric(mlc::String("Result"), mlc::Array<std::shared_ptr<registry::Type>>{i32_t(), str_t()}));
std::shared_ptr<semantic_ir::SemanticExpression> s_result = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionIdent(mlc::String("r"), result_r_t, ast::span_unknown()));
mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> map_one = mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>>{std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionIdent(mlc::String("f"), unk_t(), ast::span_unknown()))};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprMethod Result map"), eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionMethod(s_result, mlc::String("map"), map_one, empty_parameter_mutability_pattern(), unk_t(), ast::span_unknown())), context), mlc::String("mlc::result::map(r, f)")));
mlc::HashMap<mlc::String, mlc::String> namespace_alias_prefixes = mlc::HashMap<mlc::String, mlc::String>();
namespace_alias_prefixes.set(mlc::String("helpers"), mlc::String("infer_literals::"));
context::CodegenContext context_with_aliases = context::CodegenContext_with_namespace_alias_prefixes(context, namespace_alias_prefixes);
results.push_back(test_runner::assert_eq_str(mlc::String("ExprMethod on import-as alias emits namespace::fn()"), eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionMethod(sid(mlc::String("helpers")), mlc::String("infer_expr_integer_literal"), empty_arr, empty_parameter_mutability_pattern(), unk_t(), ast::span_unknown())), context_with_aliases), mlc::String("infer_literals::infer_expr_integer_literal()")));
mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> pattern_arms = mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>>{smatch_arm(std::make_shared<ast::Pattern>(ast::PatternInt(1, ast::span_unknown())), ss(mlc::String("one"))), smatch_arm(std::make_shared<ast::Pattern>(ast::PatternWild(ast::span_unknown())), ss(mlc::String("other")))};
mlc::String match_out = eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionMatch(sid(mlc::String("n")), pattern_arms, str_t(), ast::span_unknown())), context);
results.push_back(test_runner::assert_eq_str(mlc::String("ExprMatch contains std::visit"), match_out.contains(mlc::String("std::visit")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprMatch contains one"), match_out.contains(mlc::String("\"one\"")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprMatch contains other"), match_out.contains(mlc::String("\"other\"")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
results.push_back(test_runner::assert_eq_str(mlc::String("match_arm_wild_or_unit_return uses value capture"), expr::match_arm_wild_or_unit_return(mlc::String("1")), mlc::String("[=](const auto& __v) { return 1; }")));
results.push_back(test_runner::assert_eq_str(mlc::String("match_arm_binding_identifier uses value capture"), expr::match_arm_binding_identifier(mlc::String("n"), mlc::String("n")), mlc::String("[=](const auto& n) { return n; }")));
results.push_back(test_runner::assert_eq_str(mlc::String("match_arm_constructed_value uses value capture"), expr::match_arm_constructed_value(mlc::String("const Point& p"), mlc::String(""), mlc::String("p.x")), mlc::String("[=](const Point& p) { return p.x; }")));
results.push_back(test_runner::assert_eq_str(mlc::String("match_arm_constructed_value_generic uses value capture"), expr::match_arm_constructed_value_generic(mlc::String("Shape"), mlc::String("shape"), mlc::String(""), mlc::String("1")), mlc::String("[=]<typename __mlcT>(const Shape<__mlcT>& shape) { return 1; }")));
mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> string_match_arms = mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>>{smatch_arm(std::make_shared<ast::Pattern>(ast::PatternStringLit(mlc::String("fn"), ast::span_unknown())), si(1)), smatch_arm(std::make_shared<ast::Pattern>(ast::PatternStringLit(mlc::String("type"), ast::span_unknown())), si(2)), smatch_arm(std::make_shared<ast::Pattern>(ast::PatternWild(ast::span_unknown())), si(0))};
std::shared_ptr<semantic_ir::SemanticExpression> word_subject = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionIdent(mlc::String("word"), str_t(), ast::span_unknown()));
mlc::String string_match_out = eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionMatch(word_subject, string_match_arms, i32_t(), ast::span_unknown())), context);
results.push_back(test_runner::assert_eq_str(mlc::String("string match avoids std::visit"), string_match_out.contains(mlc::String("std::visit")) ? mlc::String("has_visit") : mlc::String("no_visit"), mlc::String("no_visit")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("string match emits if/else if chain"), string_match_out, mlc::String("else if")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("string match compares subject to literal"), string_match_out, mlc::String("__match_subject ==")));
mlc::String string_match_program = mlc::String("fn f(word: string) -> i32 = match word | \"fn\" => 1 | \"type\" => 2 | _ => 0 end\nfn main() -> i32 = f(\"fn\")\n");
mlc::String string_match_program_cpp = codegen_harness::gen_program(decls::parse_program(lexer::tokenize(string_match_program).tokens));
results.push_back(test_runner::assert_eq_str(mlc::String("program string match avoids std::visit"), string_match_program_cpp.contains(mlc::String("std::visit")) ? mlc::String("has_visit") : mlc::String("no_visit"), mlc::String("no_visit")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("program string match has else if"), string_match_program_cpp, mlc::String("else if")));
mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> or_match_arms = mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>>{smatch_arm(std::make_shared<ast::Pattern>(ast::PatternOr(mlc::Array<std::shared_ptr<ast::Pattern>>{std::make_shared<ast::Pattern>(ast::PatternCtor(mlc::String("Circle"), mlc::Array<std::shared_ptr<ast::Pattern>>{std::make_shared<ast::Pattern>(ast::PatternWild(ast::span_unknown()))}, ast::span_unknown())), std::make_shared<ast::Pattern>(ast::PatternCtor(mlc::String("Square"), mlc::Array<std::shared_ptr<ast::Pattern>>{std::make_shared<ast::Pattern>(ast::PatternWild(ast::span_unknown()))}, ast::span_unknown()))}, ast::span_unknown())), ss(mlc::String("r")))};
mlc::String or_match_out = eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionMatch(sid(mlc::String("s")), or_match_arms, str_t(), ast::span_unknown())), context);
results.push_back(test_runner::assert_eq_str(mlc::String("PatternOr expanded: Circle lambda present"), or_match_out.contains(mlc::String("Circle")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
results.push_back(test_runner::assert_eq_str(mlc::String("PatternOr expanded: Square lambda present"), or_match_out.contains(mlc::String("Square")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
mlc::String guarded_primitive_program = mlc::String("fn main() -> i32 = match 5 { n if n > 0 => 1, else => 0 }\n");
mlc::String guarded_primitive_cpp = codegen_harness::gen_program(decls::parse_program(lexer::tokenize(guarded_primitive_program).tokens));
results.push_back(test_runner::assert_eq_str(mlc::String("match guard on primitive avoids std::visit"), guarded_primitive_cpp.contains(mlc::String("std::visit")) ? mlc::String("has_visit") : mlc::String("no_visit"), mlc::String("no_visit")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("match guard emits typed IIFE"), guarded_primitive_cpp, mlc::String("[&]() -> int")));
mlc::String plain_sum_program = mlc::String("type T = A(i32) | B\nfn f(x: T) -> i32 = match x | A(n) => n | B => 0 end\nfn main() -> i32 = f(A(1))\n");
mlc::String plain_sum_cpp = codegen_harness::gen_program(decls::parse_program(lexer::tokenize(plain_sum_program).tokens));
results.push_back(test_runner::assert_eq_str(mlc::String("match sum without guard keeps std::visit"), plain_sum_cpp.contains(mlc::String("std::visit")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
mlc::String guarded_sum_program = mlc::String("type U = A(i32) | B\nfn g(x: U) -> i32 = match x | A(n) if n > 0 => n | A(_) => 0 | B => -1 end\nfn main() -> i32 = g(A(3))\n");
mlc::String guarded_sum_cpp = codegen_harness::gen_program(decls::parse_program(lexer::tokenize(guarded_sum_program).tokens));
results.push_back(test_runner::assert_eq_str(mlc::String("match sum with guard avoids std::visit"), guarded_sum_cpp.contains(mlc::String("std::visit")) ? mlc::String("has_visit") : mlc::String("no_visit"), mlc::String("no_visit")));
mlc::String mixed_guard_program = mlc::String("type V = A(i32) | B\nfn h(x: V) -> i32 = match x | A(n) => n | B if true => 0 end\nfn main() -> i32 = h(B)\n");
mlc::String mixed_guard_cpp = codegen_harness::gen_program(decls::parse_program(lexer::tokenize(mixed_guard_program).tokens));
results.push_back(test_runner::assert_eq_str(mlc::String("match mixed arms use guarded lowering when any guard present"), mixed_guard_cpp.contains(mlc::String("std::visit")) ? mlc::String("has_visit") : mlc::String("no_visit"), mlc::String("no_visit")));
mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> while_body = mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementExpr(si(0), ast::span_unknown()))};
mlc::String while_out = eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionWhile(sb(true), while_body, unit_t(), ast::span_unknown())), context);
results.push_back(test_runner::assert_eq_str(mlc::String("ExprWhile contains while (true)"), while_out.contains(mlc::String("while (true)")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> block_statements = mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementLet(mlc::String("x"), false, si(1), i32_t(), ast::span_unknown()))};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprBlock"), eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBlock(block_statements, sid(mlc::String("x")), i32_t(), ast::span_unknown())), context), mlc::String("[&]() {\nauto x = 1;\nreturn x;\n}()")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_stmts_str: let const emits constexpr"), eval::gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementLetConst(mlc::String("limit"), si(42), i32_t(), ast::span_unknown()))}, context), mlc::String("constexpr auto limit = 42;\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_stmts_cpp_as_string: break"), eval::gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementBreak(ast::span_unknown()))}, context), mlc::String("break;\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_stmts_cpp_as_string: continue"), eval::gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementContinue(ast::span_unknown()))}, context), mlc::String("continue;\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_stmts_cpp_as_string: let"), eval::gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementLet(mlc::String("counter"), false, si(1), i32_t(), ast::span_unknown()))}, context), mlc::String("auto counter = 1;\n")));
std::shared_ptr<semantic_ir::SemanticExpression> map_new_value = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionMethod(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionIdent(mlc::String("Map"), unk_t(), ast::span_unknown())), mlc::String("new"), {}, empty_parameter_mutability_pattern(), unk_t(), ast::span_unknown()));
std::shared_ptr<registry::Type> map_new_type = std::make_shared<registry::Type>(registry::TGeneric(mlc::String("Map"), mlc::Array<std::shared_ptr<registry::Type>>{std::make_shared<registry::Type>((registry::TString{})), std::make_shared<registry::Type>((registry::TString{}))}));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_stmts_cpp_as_string: let Map.new typed"), eval::gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementLet(mlc::String("table"), false, map_new_value, map_new_type, ast::span_unknown()))}, context), mlc::String("auto table = mlc::HashMap<mlc::String, mlc::String>();\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_stmts_cpp_as_string: let question"), eval::gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementLet(mlc::String("value"), false, std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionQuestion(si(7), unk_t(), ast::span_unknown())), unk_t(), ast::span_unknown()))}, context), mlc::String("auto value = ({ auto __q = 7; if (std::get_if<1>(&__q)) return *std::get_if<1>(&__q); std::get<0>(__q).field0; });\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_stmts_cpp_as_string: let const if"), eval::gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementLetConst(mlc::String("picked"), std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionIf(sb(true), si(1), si(2), i32_t(), ast::span_unknown())), i32_t(), ast::span_unknown()))}, context), mlc::String("constexpr auto picked = (true ? (1) : (2));\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_stmts_cpp_as_string: let const block"), eval::gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementLetConst(mlc::String("limit"), std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBlock({}, si(42), i32_t(), ast::span_unknown())), i32_t(), ast::span_unknown()))}, context), mlc::String("constexpr auto limit = 42;\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_stmts_cpp_as_string: return"), eval::gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementReturn(si(0), ast::span_unknown()))}, context), mlc::String("return 0;\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_stmts_cpp_as_string: return question"), eval::gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementReturn(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionQuestion(si(7), unk_t(), ast::span_unknown())), ast::span_unknown()))}, context), mlc::String("return ({ auto __q = 7; if (std::get_if<1>(&__q)) return *std::get_if<1>(&__q); std::get<0>(__q).field0; });\n")));
std::shared_ptr<semantic_ir::SemanticExpression> assign_expr = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBin(mlc::String("="), sid(mlc::String("counter")), std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionBin(mlc::String("+"), sid(mlc::String("counter")), si(1), i32_t(), ast::span_unknown())), i32_t(), ast::span_unknown()));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_stmts_cpp_as_string: expr assign"), eval::gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementExpr(assign_expr, ast::span_unknown()))}, context), mlc::String("(counter = (counter + 1));\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_stmts_cpp_as_string: expr question"), eval::gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementExpr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionQuestion(si(7), unk_t(), ast::span_unknown())), ast::span_unknown()))}, context), mlc::String("({ auto __q = 7; if (std::get_if<1>(&__q)) return *std::get_if<1>(&__q); std::get<0>(__q).field0; });\n")));
mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> if_stmt_body = mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementExpr(si(0), ast::span_unknown()))};
results.push_back(test_runner::assert_eq_str(mlc::String("gen_stmts_cpp_as_string: expr while"), eval::gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementExpr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionWhile(sb(true), if_stmt_body, unit_t(), ast::span_unknown())), ast::span_unknown()))}, context), mlc::String("while (true) {\n  0;\n}\n")));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_stmts_cpp_as_string: let const matches string"), eval::gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementLetConst(mlc::String("limit"), si(42), i32_t(), ast::span_unknown()))}, context), eval::gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementLetConst(mlc::String("limit"), si(42), i32_t(), ast::span_unknown()))}, context)));
mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> fv_a = mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>>{std::make_shared<semantic_ir::SemanticFieldVal>(semantic_ir::SemanticFieldVal{mlc::String("a"), si(1)}), std::make_shared<semantic_ir::SemanticFieldVal>(semantic_ir::SemanticFieldVal{mlc::String("b"), si(2)})};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprRecord positional"), eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionRecord(mlc::String("Point"), fv_a, std::make_shared<registry::Type>(registry::TNamed(mlc::String("Point"))), ast::span_unknown())), context), mlc::String("Point{1, 2}")));
mlc::Array<std::shared_ptr<ast::Param>> fn_param = mlc::Array<std::shared_ptr<ast::Param>>{std::make_shared<ast::Param>(ast::Param{mlc::String("n"), false, std::make_shared<ast::TypeExpr>((ast::TyI32{})), false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())), std::make_shared<ast::Pattern>(ast::PatternUnit(ast::span_unknown()))})};
std::shared_ptr<ast::Expr> fn_body = std::make_shared<ast::Expr>(ast::ExprBin(mlc::String("*"), std::make_shared<ast::Expr>(ast::ExprIdent(mlc::String("n"), ast::span_unknown())), std::make_shared<ast::Expr>(ast::ExprInt(2, ast::span_unknown())), ast::span_unknown()));
ast::Decl fn_decl = ast::DeclFn(mlc::String("double_it"), {}, {}, fn_param, std::make_shared<ast::TypeExpr>((ast::TyI32{})), fn_body, {});
ast::Program fn_prog = ast::Program{mlc::Array<std::shared_ptr<ast::Decl>>{std::make_shared<ast::Decl>(ast::DeclExported(std::make_shared<ast::Decl>(fn_decl)))}};
mlc::String fn_prog_out = codegen_harness::gen_program(fn_prog);
results.push_back(test_runner::assert_eq_str(mlc::String("DeclFn: signature in output"), fn_prog_out.contains(mlc::String("double_it")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
results.push_back(test_runner::assert_eq_str(mlc::String("DeclFn: param type in output"), fn_prog_out.contains(mlc::String("int n")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
results.push_back(test_runner::assert_eq_str(mlc::String("DeclFn: body in output"), fn_prog_out.contains(mlc::String("return (n * 2)")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
mlc::String type_alias_return_source = mlc::String("type Words = [string]\nexport fn main() -> Words = []");
mlc::String type_alias_return_output = codegen_harness::gen_program(decls::parse_program(lexer::tokenize(type_alias_return_source).tokens));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("type alias: return uses underlying array type"), type_alias_return_output, mlc::String("mlc::Array<mlc::String>")));
results.push_back(codegen_test_helpers::assert_code_not_contains(mlc::String("type alias: no struct decl for alias name"), type_alias_return_output, mlc::String("struct Words")));
results.push_back(codegen_test_helpers::assert_code_not_contains(mlc::String("type alias: no using decl for alias name"), type_alias_return_output, mlc::String("using Words")));
mlc::String type_alias_parameter_source = mlc::String("type Count = i32\nexport fn double_count(value: Count) -> Count = value * 2");
mlc::String type_alias_parameter_output = codegen_harness::gen_program(decls::parse_program(lexer::tokenize(type_alias_parameter_source).tokens));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("type alias: parameter uses underlying int type"), type_alias_parameter_output, mlc::String("int value")));
results.push_back(codegen_test_helpers::assert_code_not_contains(mlc::String("type alias parameter: no struct for alias name"), type_alias_parameter_output, mlc::String("struct Count")));
mlc::String d3_trait_parameter_source = mlc::String("type Display { fn to_string(self: Self) -> string }\nfn f(x: Display) -> unit = ()");
mlc::String d3_trait_parameter_output = codegen_harness::gen_program(decls::parse_program(lexer::tokenize(d3_trait_parameter_source).tokens));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("D3 gen_program: concept Display"), d3_trait_parameter_output, mlc::String("concept Display")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("D3 gen_program: requires in trait concept"), d3_trait_parameter_output, mlc::String("requires")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("D3 gen_program: parameter uses Display constraint"), d3_trait_parameter_output, mlc::String("requires Display<")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("D3 gen_program: expanded trait param type"), d3_trait_parameter_output, mlc::String("__trait_param_0")));
mlc::String d3_two_trait_parameter_source = mlc::String("type Display { fn to_string(self: Self) -> string }\nfn g(first: Display, second: Display) -> unit = ()");
mlc::String d3_two_trait_parameter_output = codegen_harness::gen_program(decls::parse_program(lexer::tokenize(d3_two_trait_parameter_source).tokens));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("D3 gen_program: first Display parameter"), d3_two_trait_parameter_output, mlc::String("Display<__trait_param_0>")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("D3 gen_program: second Display parameter"), d3_two_trait_parameter_output, mlc::String("Display<__trait_param_1>")));
mlc::String d4_where_trait_bounds_source = mlc::String("type Display { fn to_string(self: Self) -> string }\ntype EqCompare { fn same(self: Self, other: Self) -> bool }\nfn f<T>(x: T) -> unit where T: Display + EqCompare = ()");
mlc::String d4_where_trait_bounds_output = codegen_harness::gen_program(decls::parse_program(lexer::tokenize(d4_where_trait_bounds_source).tokens));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("D4 gen_program: requires Display"), d4_where_trait_bounds_output, mlc::String("Display")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("D4 gen_program: requires EqCompare"), d4_where_trait_bounds_output, mlc::String("EqCompare")));
mlc::Array<mlc::String> result_type_params = mlc::Array<mlc::String>{mlc::String("T"), mlc::String("E")};
mlc::Array<std::shared_ptr<ast::TypeExpr>> ok_fields = mlc::Array<std::shared_ptr<ast::TypeExpr>>{std::make_shared<ast::TypeExpr>(ast::TyNamed(mlc::String("T")))};
mlc::Array<std::shared_ptr<ast::TypeExpr>> err_fields = mlc::Array<std::shared_ptr<ast::TypeExpr>>{std::make_shared<ast::TypeExpr>(ast::TyNamed(mlc::String("E")))};
mlc::Array<std::shared_ptr<ast::TypeVariant>> result_variants = mlc::Array<std::shared_ptr<ast::TypeVariant>>{std::make_shared<ast::TypeVariant>(ast::VarTuple(mlc::String("Ok"), ok_fields, false)), std::make_shared<ast::TypeVariant>(ast::VarTuple(mlc::String("Err"), err_fields, false))};
mlc::String expected_fwd = mlc::String("template<typename T>\nstruct Ok;\n") + mlc::String("template<typename E>\nstruct Err;\n") + mlc::String("template<typename T, typename E>\nusing Result = std::variant<Ok<T>, Err<E>>;\n");
mlc::String expected_body = mlc::String("template<typename T>\nstruct Ok {T field0;};\n") + mlc::String("template<typename E>\nstruct Err {E field0;};\n");
results.push_back(test_runner::assert_eq_str(mlc::String("generic type Result<T,E> = Ok(T) | Err(E) codegen"), type_gen::gen_type_decl(context, mlc::String("Result"), result_type_params, result_variants), expected_fwd + expected_body));
results.push_back(test_runner::assert_eq_str(mlc::String("string literal with embedded newline - escapes to \\n in C++"), eval::gen_expr(ss(mlc::String("hello\nworld")), context), mlc::String("mlc::String(\"hello\\nworld\", 11)")));
results.push_back(test_runner::assert_eq_str(mlc::String("string literal with tab - escapes to \\t"), eval::gen_expr(ss(mlc::String("a\tb")), context), mlc::String("mlc::String(\"a\\tb\", 3)")));
results.push_back(test_runner::assert_eq_str(mlc::String("string literal with backslash - escapes to \\\\"), eval::gen_expr(ss(mlc::String("a\\b")), context), mlc::String("mlc::String(\"a\\\\b\", 3)")));
results.push_back(test_runner::assert_eq_str(mlc::String("string literal with double quote - escapes to \\\""), eval::gen_expr(ss(mlc::String("say \"hi\"")), context), mlc::String("mlc::String(\"say \\\"hi\\\"\", 8)")));
std::shared_ptr<semantic_ir::SemanticExpression> letpat_else_body = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionInt(0, i32_t(), ast::span_unknown()));
std::shared_ptr<semantic_ir::SemanticStatement> letpat_ident = std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementLetPattern(std::make_shared<ast::Pattern>(ast::PatternIdent(mlc::String("value"), ast::span_unknown())), false, si(7), i32_t(), false, letpat_else_body, ast::span_unknown()));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_stmts_cpp_as_string: let pat ident"), eval::gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{letpat_ident}, context), mlc::String("auto value = 7;\n")));
std::shared_ptr<ast::Pattern> tuple_pat = std::make_shared<ast::Pattern>(ast::PatternTuple(mlc::Array<std::shared_ptr<ast::Pattern>>{std::make_shared<ast::Pattern>(ast::PatternIdent(mlc::String("left"), ast::span_unknown())), std::make_shared<ast::Pattern>(ast::PatternIdent(mlc::String("right"), ast::span_unknown()))}, ast::span_unknown()));
std::shared_ptr<registry::Type> pair_type = std::make_shared<registry::Type>(registry::TPair(std::make_shared<registry::Type>((registry::TI32{})), std::make_shared<registry::Type>((registry::TI32{}))));
std::shared_ptr<semantic_ir::SemanticStatement> let_tuple_pair = std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementLetPattern(tuple_pat, false, sid(mlc::String("pair")), pair_type, false, letpat_else_body, ast::span_unknown()));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_stmts_cpp_as_string: let tuple pair native std::get"), eval::gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{let_tuple_pair}, context), mlc::String("auto __tmp_0 = pair;\nauto left = std::get<0>(__tmp_0);\nauto right = std::get<1>(__tmp_0);\n")));
results.push_back(codegen_test_helpers::assert_code_not_contains(mlc::String("gen_stmts_cpp_as_string: let tuple pair no structured binding fragment"), eval::gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{let_tuple_pair}, context), mlc::String("auto [")));
std::shared_ptr<ast::Pattern> tuple_wild_pat = std::make_shared<ast::Pattern>(ast::PatternTuple(mlc::Array<std::shared_ptr<ast::Pattern>>{std::make_shared<ast::Pattern>(ast::PatternIdent(mlc::String("left"), ast::span_unknown())), std::make_shared<ast::Pattern>(ast::PatternWild(ast::span_unknown()))}, ast::span_unknown()));
std::shared_ptr<semantic_ir::SemanticStatement> let_tuple_wild = std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementLetPattern(tuple_wild_pat, false, sid(mlc::String("pair")), pair_type, false, letpat_else_body, ast::span_unknown()));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_stmts_cpp: let tuple pair wild native structured binding"), eval::gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{let_tuple_wild}, context), mlc::String("auto __tmp_0 = pair;\nauto [left, __1] = __tmp_0;\n")));
std::shared_ptr<ast::Pattern> record_tuple_pat = std::make_shared<ast::Pattern>(ast::PatternTuple(mlc::Array<std::shared_ptr<ast::Pattern>>{std::make_shared<ast::Pattern>(ast::PatternIdent(mlc::String("first"), ast::span_unknown())), std::make_shared<ast::Pattern>(ast::PatternIdent(mlc::String("second"), ast::span_unknown()))}, ast::span_unknown()));
std::shared_ptr<semantic_ir::SemanticStatement> let_record_tuple_fallback = std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementLetPattern(record_tuple_pat, false, sid(mlc::String("point")), std::make_shared<registry::Type>(registry::TNamed(mlc::String("Point"))), false, letpat_else_body, ast::span_unknown()));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_stmts_cpp: let record tuple fallback native structured binding"), eval::gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{let_record_tuple_fallback}, context), mlc::String("auto __tmp_0 = point;\nauto [first, second] = __tmp_0;\n")));
std::shared_ptr<ast::Pattern> unknown_tuple_pat = std::make_shared<ast::Pattern>(ast::PatternTuple(mlc::Array<std::shared_ptr<ast::Pattern>>{std::make_shared<ast::Pattern>(ast::PatternIdent(mlc::String("alpha"), ast::span_unknown())), std::make_shared<ast::Pattern>(ast::PatternWild(ast::span_unknown()))}, ast::span_unknown()));
std::shared_ptr<semantic_ir::SemanticStatement> let_unknown_tuple_fallback = std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementLetPattern(unknown_tuple_pat, false, sid(mlc::String("value")), unk_t(), false, letpat_else_body, ast::span_unknown()));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_stmts_cpp: let unknown type tuple fallback native structured binding"), eval::gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{let_unknown_tuple_fallback}, context), mlc::String("auto __tmp_0 = value;\nauto [alpha, __1] = __tmp_0;\n")));
std::shared_ptr<ast::Pattern> letpat_ctor_pat = std::make_shared<ast::Pattern>(ast::PatternCtor(mlc::String("Some"), mlc::Array<std::shared_ptr<ast::Pattern>>{std::make_shared<ast::Pattern>(ast::PatternIdent(mlc::String("v"), ast::span_unknown()))}, ast::span_unknown()));
std::shared_ptr<semantic_ir::SemanticStatement> letpat_with_else = std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementLetPattern(letpat_ctor_pat, false, sid(mlc::String("opt")), std::make_shared<registry::Type>(registry::TNamed(mlc::String("Opt"))), true, letpat_else_body, ast::span_unknown()));
std::shared_ptr<semantic_ir::SemanticStatement> letpat_no_else = std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementLetPattern(letpat_ctor_pat, false, sid(mlc::String("opt")), std::make_shared<registry::Type>(registry::TNamed(mlc::String("Opt"))), false, letpat_else_body, ast::span_unknown()));
mlc::String letpat_with_else_out = eval::gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{letpat_with_else}, context);
mlc::String letpat_no_else_out = eval::gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{letpat_no_else}, context);
results.push_back(test_runner::assert_eq_str(mlc::String("let-else: with else no abort"), letpat_with_else_out.contains(mlc::String("abort")) ? mlc::String("abort") : mlc::String("no-abort"), mlc::String("no-abort")));
results.push_back(test_runner::assert_eq_str(mlc::String("let-else: without else uses abort"), letpat_no_else_out.contains(mlc::String("abort")) ? mlc::String("abort") : mlc::String("no-abort"), mlc::String("abort")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_stmts_cpp: let ctor native holds_alternative"), letpat_no_else_out, mlc::String("std::holds_alternative")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_stmts_cpp: let ctor native std::get bind"), letpat_no_else_out, mlc::String("std::get<0>(__tmp_1)")));
std::shared_ptr<ast::Pattern> letpat_ctor_wild_pat = std::make_shared<ast::Pattern>(ast::PatternCtor(mlc::String("Some"), mlc::Array<std::shared_ptr<ast::Pattern>>{std::make_shared<ast::Pattern>(ast::PatternIdent(mlc::String("head"), ast::span_unknown())), std::make_shared<ast::Pattern>(ast::PatternWild(ast::span_unknown()))}, ast::span_unknown()));
std::shared_ptr<semantic_ir::SemanticStatement> letpat_ctor_wild = std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementLetPattern(letpat_ctor_wild_pat, false, sid(mlc::String("opt")), std::make_shared<registry::Type>(registry::TNamed(mlc::String("Opt"))), false, letpat_else_body, ast::span_unknown()));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_stmts_cpp: let ctor wild native structured binding"), eval::gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{letpat_ctor_wild}, context), mlc::String("auto [head, __1] = __tmp_1")));
std::shared_ptr<ast::Pattern> array_pat = std::make_shared<ast::Pattern>(ast::PatternArray(mlc::Array<std::shared_ptr<ast::Pattern>>{std::make_shared<ast::Pattern>(ast::PatternIdent(mlc::String("head"), ast::span_unknown()))}, mlc::String("tail"), ast::span_unknown()));
std::shared_ptr<semantic_ir::SemanticStatement> let_array_rest = std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementLetPattern(array_pat, false, sid(mlc::String("items")), std::make_shared<registry::Type>(registry::TArray(std::make_shared<registry::Type>((registry::TI32{})))), false, letpat_else_body, ast::span_unknown()));
mlc::String let_array_out = eval::gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{let_array_rest}, context);
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_stmts_cpp: let array rest native slice"), let_array_out, mlc::String("mlc::Array<int>")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_stmts_cpp: let array rest cbegin offset"), let_array_out, mlc::String("cbegin() + 1")));
std::shared_ptr<ast::Pattern> record_pat = std::make_shared<ast::Pattern>(ast::PatternRecord(mlc::String(""), mlc::Array<std::shared_ptr<ast::Pattern>>{std::make_shared<ast::Pattern>(ast::PatternIdent(mlc::String("value"), ast::span_unknown()))}, ast::span_unknown()));
std::shared_ptr<semantic_ir::SemanticStatement> let_record = std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementLetPattern(record_pat, false, sid(mlc::String("point")), std::make_shared<registry::Type>(registry::TNamed(mlc::String("Point"))), false, letpat_else_body, ast::span_unknown()));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_stmts_cpp: let record field native"), eval::gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{let_record}, context), mlc::String("__tmp_0.value")));
mlc::Array<std::shared_ptr<ast::FieldDef>> phantom_fds = mlc::Array<std::shared_ptr<ast::FieldDef>>{std::make_shared<ast::FieldDef>(ast::FieldDef{mlc::String("value"), std::make_shared<ast::TypeExpr>((ast::TyI32{})), false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown()))})};
mlc::Array<std::shared_ptr<ast::TypeVariant>> phantom_variants_1 = mlc::Array<std::shared_ptr<ast::TypeVariant>>{std::make_shared<ast::TypeVariant>(ast::VarRecord(mlc::String("Ast"), phantom_fds, false))};
mlc::String phantom_type_out = type_gen::gen_type_decl(context, mlc::String("Ast"), mlc::Array<mlc::String>{mlc::String("Phase")}, phantom_variants_1);
results.push_back(test_runner::assert_eq_str(mlc::String("phantom single-variant: template<typename Phase> in output"), phantom_type_out.contains(mlc::String("typename Phase")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
mlc::Array<std::shared_ptr<ast::TypeVariant>> tagged_variants = mlc::Array<std::shared_ptr<ast::TypeVariant>>{std::make_shared<ast::TypeVariant>(ast::VarTuple(mlc::String("Active"), mlc::Array<std::shared_ptr<ast::TypeExpr>>{std::make_shared<ast::TypeExpr>((ast::TyI32{}))}, false)), std::make_shared<ast::TypeVariant>(ast::VarUnit(mlc::String("Inactive"), false))};
mlc::String tagged_out = type_gen::gen_type_decl(context, mlc::String("Tagged"), mlc::Array<mlc::String>{mlc::String("Tag")}, tagged_variants);
results.push_back(test_runner::assert_eq_str(mlc::String("phantom multi-variant: Active<Tag> in alias"), tagged_out.contains(mlc::String("Active<Tag>")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
results.push_back(test_runner::assert_eq_str(mlc::String("phantom multi-variant: template<typename Tag> on Active struct"), tagged_out.contains(mlc::String("template<typename Tag>\nstruct Active")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
std::shared_ptr<registry::Type> phantom_rec_type = std::make_shared<registry::Type>(registry::TGeneric(mlc::String("Ast"), mlc::Array<std::shared_ptr<registry::Type>>{std::make_shared<registry::Type>(registry::TNamed(mlc::String("Unvalidated")))}));
mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> phantom_rec_fvals = {};
std::shared_ptr<semantic_ir::SemanticExpression> phantom_rec_expr = std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionRecord(mlc::String("Ast"), phantom_rec_fvals, phantom_rec_type, ast::span_unknown()));
results.push_back(test_runner::assert_eq_str(mlc::String("phantom record expr: Ast<Unvalidated>{}"), eval::gen_expr(phantom_rec_expr, context), mlc::String("Ast<Unvalidated>{}")));
auto f64_t = std::make_shared<registry::Type>((registry::TF64{}));
auto i64_t = std::make_shared<registry::Type>((registry::TI64{}));
auto u8_t = std::make_shared<registry::Type>((registry::TU8{}));
auto usize_t = std::make_shared<registry::Type>((registry::TUsize{}));
auto char_t = std::make_shared<registry::Type>((registry::TChar{}));
results.push_back(test_runner::assert_eq_str(mlc::String("c4: f64 literal codegen"), eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionFloat(mlc::String("3.14"), f64_t, ast::span_unknown())), context), mlc::String("3.14")));
results.push_back(test_runner::assert_eq_str(mlc::String("c4: i64 literal codegen"), eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionI64(mlc::String("42"), i64_t, ast::span_unknown())), context), mlc::String("42")));
results.push_back(test_runner::assert_eq_str(mlc::String("c4: u8 literal codegen"), eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionU8(mlc::String("255"), u8_t, ast::span_unknown())), context), mlc::String("static_cast<uint8_t>(255)")));
results.push_back(test_runner::assert_eq_str(mlc::String("c4: usize literal codegen"), eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionUsize(mlc::String("100"), usize_t, ast::span_unknown())), context), mlc::String("static_cast<size_t>(100)")));
results.push_back(test_runner::assert_eq_str(mlc::String("c4: char literal codegen"), eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionChar(mlc::String("a"), char_t, ast::span_unknown())), context), mlc::String("static_cast<char32_t>(a)")));
results.push_back(test_runner::assert_eq_str(mlc::String("c4: sem_type_to_cpp TI64"), type_gen::sem_type_to_cpp(context, i64_t), mlc::String("int64_t")));
results.push_back(test_runner::assert_eq_str(mlc::String("c4: sem_type_to_cpp TF64"), type_gen::sem_type_to_cpp(context, f64_t), mlc::String("double")));
results.push_back(test_runner::assert_eq_str(mlc::String("c4: sem_type_to_cpp TU8"), type_gen::sem_type_to_cpp(context, u8_t), mlc::String("uint8_t")));
results.push_back(test_runner::assert_eq_str(mlc::String("c4: sem_type_to_cpp TUsize"), type_gen::sem_type_to_cpp(context, usize_t), mlc::String("size_t")));
results.push_back(test_runner::assert_eq_str(mlc::String("c4: sem_type_to_cpp TChar"), type_gen::sem_type_to_cpp(context, char_t), mlc::String("char32_t")));
mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> with_body = mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementExpr(si(0), ast::span_unknown()))};
mlc::String with_out = eval::gen_expr(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionWith(si(1), mlc::String("res"), with_body, unit_t(), ast::span_unknown())), context);
results.push_back(test_runner::assert_eq_str(mlc::String("c5: SemanticExpressionWith contains auto res ="), with_out.contains(mlc::String("auto res =")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
results.push_back(test_runner::assert_eq_str(mlc::String("c5: SemanticExpressionWith contains res.drop()"), with_out.contains(mlc::String("res.drop()")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
std::shared_ptr<registry::Type> assoc_t = std::make_shared<registry::Type>(registry::TAssoc(mlc::String("I"), mlc::String("Item")));
results.push_back(test_runner::assert_eq_str(mlc::String("d1: sem_type_to_cpp TAssoc"), type_gen::sem_type_to_cpp(context, assoc_t), mlc::String("typename I::Item")));
mlc::String assoc_prog_src = mlc::String("type Iter { type Item } type Box<T> = { val: T } extend Box<T>: Iter { type Item = T fn Iter_next(self: Box<T>) -> i32 = 0 }");
mlc::String assoc_prog_out = codegen_harness::gen_program(decls::parse_program(lexer::tokenize(assoc_prog_src).tokens));
results.push_back(test_runner::assert_eq_str(mlc::String("d1: gen_program struct contains using Item ="), assoc_prog_out.contains(mlc::String("using Item =")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
mlc::String op_prog_src = mlc::String("type Vec2 = { x: i32, y: i32 } extend Vec2 : Add<Vec2> { type Output = Vec2 fn add(self: Vec2, rhs: Vec2) -> Vec2 = Vec2 { x: self.x + rhs.x, y: self.y + rhs.y } } fn test(a: Vec2, b: Vec2) -> Vec2 = a + b fn main() -> i32 = 0");
mlc::String op_prog_out = codegen_harness::gen_program(decls::parse_program(lexer::tokenize(op_prog_src).tokens));
results.push_back(test_runner::assert_eq_str(mlc::String("d2: gen_program Vec2 + Vec2 desugars to Vec2_add"), op_prog_out.contains(mlc::String("Vec2_add(")) ? mlc::String("yes") : mlc::String("no"), mlc::String("yes")));
mlc::String codegen_invariant_tail_after_else_if_chain = mlc::String("export fn tail_after_chain(kind: string, extra: i32) -> i32 = do\n  let mut accumulator = 0\n  if kind == \"a\" then\n    accumulator = 1\n  else if kind == \"b\" then\n    accumulator = 2\n  else\n    accumulator = 3\n  end\n  accumulator + extra\nend\n\nexport fn main() -> i32 = tail_after_chain(\"a\", 100)");
mlc::String codegen_tail_chain_cpp = codegen_harness::gen_program(decls::parse_program(lexer::tokenize(codegen_invariant_tail_after_else_if_chain).tokens));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_program: else-if chain keeps trailing tail expr"), codegen_tail_chain_cpp, mlc::String("accumulator + extra")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_program: else-if chain emits return of tail"), codegen_tail_chain_cpp, mlc::String("return")));
results.push_back(codegen_test_helpers::assert_code_not_contains(mlc::String("gen_program: else-if chain has no stray export marker"), codegen_tail_chain_cpp, mlc::String("export_;")));
mlc::String codegen_invariant_twin_if_zip_join_style = mlc::String("export fn twin_branch(kind: string, has_arg: bool) -> i32 = do\n  let mut diagnostics_weight = 0\n  if has_arg then\n    if kind == \"zip\" then\n      diagnostics_weight = 1\n    end\n    if kind == \"join\" then\n      diagnostics_weight = diagnostics_weight + 2\n    end\n  end\n  diagnostics_weight + 50\nend\n\nexport fn main() -> i32 = twin_branch(\"zip\", true)");
mlc::String codegen_twin_if_cpp = codegen_harness::gen_program(decls::parse_program(lexer::tokenize(codegen_invariant_twin_if_zip_join_style).tokens));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_program: twin if emits final accumulator tail"), codegen_twin_if_cpp, mlc::String("diagnostics_weight + 50")));
results.push_back(codegen_test_helpers::assert_code_not_contains(mlc::String("gen_program: twin if has no stray export marker"), codegen_twin_if_cpp, mlc::String("export_;")));
mlc::String codegen_invariant_if_else_flattened = mlc::String("fn merge_unknown_inner(then_inner: i32, else_errors: i32) -> i32 = do\n  if then_inner == 0 then else_errors + 10\n  else then_inner + else_errors\n  end\nend\n\nexport fn conditional_flat(use_merge: bool, then_inner: i32, else_errors: i32) -> i32 = do\n  if !use_merge then else_errors\n  else merge_unknown_inner(then_inner, else_errors)\n  end\nend\n\nexport fn main() -> i32 = conditional_flat(true, 0, 5)");
mlc::String codegen_if_else_flat_cpp = codegen_harness::gen_program(decls::parse_program(lexer::tokenize(codegen_invariant_if_else_flattened).tokens));
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
mlc::String codegen_large_function_body = mlc::String("export fn large_body(seed: i32) -> i32 = do\n  let mut accumulator = seed\n  accumulator = accumulator + 1\n  accumulator = accumulator + 2\n  accumulator = accumulator + 3\n  if accumulator > 10 then\n    accumulator = accumulator - 1\n  end\n  if accumulator > 20 then\n    accumulator = accumulator - 2\n  else\n    accumulator = accumulator + 4\n  end\n  let mut counter = 0\n  while counter < 5 do\n    accumulator = accumulator + counter\n    counter = counter + 1\n  end\n  match accumulator {\n    0 => 0,\n    _ => accumulator\n  }\nend\n\nexport fn main() -> i32 = large_body(0)");
mlc::String codegen_large_body_cpp = codegen_harness::gen_program(decls::parse_program(lexer::tokenize(codegen_large_function_body).tokens));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_program: large fn body keeps accumulator updates"), codegen_large_body_cpp, mlc::String("accumulator")));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_program: large fn body emits while loop"), codegen_large_body_cpp, mlc::String("while")));
mlc::String codegen_extend_method = mlc::String("type Widget = { value: i32 }\nextend Widget { fn doubled(self: Widget) -> i32 = self.value * 2 }\nexport fn main() -> i32 = do\n  let widget = Widget { value: 3 }\n  widget.doubled()\nend");
mlc::String codegen_extend_method_cpp = codegen_harness::gen_program(decls::parse_program(lexer::tokenize(codegen_extend_method).tokens));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("gen_program: extend method call mangles owner"), codegen_extend_method_cpp, mlc::String("Widget_doubled")));
return results;
}

} // namespace test_codegen
