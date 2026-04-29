#define main mlc_user_main
#include "test_codegen.hpp"

namespace test_codegen {

std::shared_ptr<registry::Type> i32_t() noexcept{
return std::make_shared<registry::Type>(registry::TI32{});
}
std::shared_ptr<registry::Type> str_t() noexcept{
return std::make_shared<registry::Type>(registry::TString{});
}
std::shared_ptr<registry::Type> bool_t() noexcept{
return std::make_shared<registry::Type>(registry::TBool{});
}
std::shared_ptr<registry::Type> unit_t() noexcept{
return std::make_shared<registry::Type>(registry::TUnit{});
}
std::shared_ptr<registry::Type> unk_t() noexcept{
return std::make_shared<registry::Type>(registry::TUnknown{});
}
std::shared_ptr<semantic_ir::SExpr> si(int v) noexcept{
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprInt{v, i32_t(), ast::span_unknown()});
}
std::shared_ptr<semantic_ir::SExpr> ss(mlc::String v) noexcept{
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprStr{v, str_t(), ast::span_unknown()});
}
std::shared_ptr<semantic_ir::SExpr> sb(bool v) noexcept{
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBool{v, bool_t(), ast::span_unknown()});
}
std::shared_ptr<semantic_ir::SExpr> sid(mlc::String name) noexcept{
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprIdent{name, unk_t(), ast::span_unknown()});
}
std::shared_ptr<semantic_ir::SExpr> su() noexcept{
return std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUnit{unit_t(), ast::span_unknown()});
}
mlc::Array<test_runner::TestResult> codegen_tests() noexcept{
auto results = mlc::Array<test_runner::TestResult>{};
auto empty_prog = ast::Program{{}};
auto ctx = context::create_codegen_context(empty_prog);
results.push_back(test_runner::assert_eq_str(mlc::String("create_codegen_context: gen_expr with ctx", 41), eval::gen_expr(si(42), ctx), mlc::String("42", 2)));
results.push_back(test_runner::assert_eq_str(mlc::String("TyI32 - 'int'", 13), type_gen::type_to_cpp(ctx, std::make_shared<ast::TypeExpr>(ast::TyI32{})), mlc::String("int", 3)));
results.push_back(test_runner::assert_eq_str(mlc::String("TyString - 'mlc::String'", 24), type_gen::type_to_cpp(ctx, std::make_shared<ast::TypeExpr>(ast::TyString{})), mlc::String("mlc::String", 11)));
results.push_back(test_runner::assert_eq_str(mlc::String("TyBool - 'bool'", 15), type_gen::type_to_cpp(ctx, std::make_shared<ast::TypeExpr>(ast::TyBool{})), mlc::String("bool", 4)));
results.push_back(test_runner::assert_eq_str(mlc::String("TyUnit - 'void'", 15), type_gen::type_to_cpp(ctx, std::make_shared<ast::TypeExpr>(ast::TyUnit{})), mlc::String("void", 4)));
results.push_back(test_runner::assert_eq_str(mlc::String("TyArray(TyI32) - 'mlc::Array<int>'", 34), type_gen::type_to_cpp(ctx, std::make_shared<ast::TypeExpr>(ast::TyArray{std::make_shared<ast::TypeExpr>(ast::TyI32{})})), mlc::String("mlc::Array<int>", 15)));
results.push_back(test_runner::assert_eq_str(mlc::String("TyShared(TyI32) - 'std::shared_ptr<int>'", 40), type_gen::type_to_cpp(ctx, std::make_shared<ast::TypeExpr>(ast::TyShared{std::make_shared<ast::TypeExpr>(ast::TyI32{})})), mlc::String("std::shared_ptr<int>", 20)));
results.push_back(test_runner::assert_eq_str(mlc::String("sem_type TPair(i32, string)", 27), type_gen::sem_type_to_cpp(ctx, std::make_shared<registry::Type>(registry::TPair{std::make_shared<registry::Type>(registry::TI32{}), std::make_shared<registry::Type>(registry::TString{})})), mlc::String("std::pair<int, mlc::String>", 27)));
results.push_back(test_runner::assert_eq_str(mlc::String("sem_type TTuple(i32, string, bool)", 34), type_gen::sem_type_to_cpp(ctx, std::make_shared<registry::Type>(registry::TTuple{mlc::Array{std::make_shared<registry::Type>(registry::TI32{}), std::make_shared<registry::Type>(registry::TString{}), std::make_shared<registry::Type>(registry::TBool{})}})), mlc::String("std::tuple<int, mlc::String, bool>", 34)));
results.push_back(test_runner::assert_eq_str(mlc::String("sem_type Option<i32> -> std::optional", 37), type_gen::sem_type_to_cpp(ctx, std::make_shared<registry::Type>(registry::TGeneric{mlc::String("Option", 6), mlc::Array{std::make_shared<registry::Type>(registry::TI32{})}})), mlc::String("std::optional<int>", 18)));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprInt(42) - '42'", 18), eval::gen_expr(si(42), ctx), mlc::String("42", 2)));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprQuestion: gen_expr index-based unwrap", 41), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprQuestion{si(7), unk_t(), ast::span_unknown()}), ctx), mlc::String("({ auto __q = 7; if (std::get_if<1>(&__q)) return *std::get_if<1>(&__q); std::get<0>(__q).field0; })", 100)));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprBool(true) - 'true'", 23), eval::gen_expr(sb(true), ctx), mlc::String("true", 4)));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprStr('hi') - 'mlc::String(\"hi\", 2)'", 38), eval::gen_expr(ss(mlc::String("hi", 2)), ctx), mlc::String("mlc::String(\"hi\", 2)", 20)));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprUnit - '/* unit */'", 23), eval::gen_expr(su(), ctx), mlc::String("/* unit */", 10)));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprIdent('x') - 'x'", 20), eval::gen_expr(sid(mlc::String("x", 1)), ctx), mlc::String("x", 1)));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprIdent('print') - 'mlc::io::print'", 37), eval::gen_expr(sid(mlc::String("print", 5)), ctx), mlc::String("mlc::io::print", 14)));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprIdent('println') - 'mlc::io::println'", 41), eval::gen_expr(sid(mlc::String("println", 7)), ctx), mlc::String("mlc::io::println", 16)));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprBin add - '(1 + 2)'", 23), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBin{mlc::String("+", 1), si(1), si(2), i32_t(), ast::span_unknown()}), ctx), mlc::String("(1 + 2)", 7)));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprUn neg - '(-42)'", 20), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprUn{mlc::String("-", 1), si(42), i32_t(), ast::span_unknown()}), ctx), mlc::String("(-42)", 5)));
results.push_back(test_runner::assert_eq_str(mlc::String("map_builtin passthrough - 'foo'", 31), cpp_naming::map_builtin(mlc::String("foo", 3)), mlc::String("foo", 3)));
auto single_params = mlc::Array{mlc::String("x", 1)};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprLambda single param - '[=](auto x) { return ... }'", 54), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprLambda{single_params, std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBin{mlc::String("+", 1), sid(mlc::String("x", 1)), si(1), i32_t(), ast::span_unknown()}), unk_t(), ast::span_unknown()}), ctx), mlc::String("[=](auto x) { return (x + 1); }", 31)));
auto two_params = mlc::Array{mlc::String("a", 1), mlc::String("b", 1)};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprLambda two params - '[=](auto a, auto b) { return ... }'", 60), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprLambda{two_params, std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBin{mlc::String("+", 1), sid(mlc::String("a", 1)), sid(mlc::String("b", 1)), i32_t(), ast::span_unknown()}), unk_t(), ast::span_unknown()}), ctx), mlc::String("[=](auto a, auto b) { return (a + b); }", 39)));
auto zero_params = mlc::Array<mlc::String>{};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprLambda zero params - '[]() { return ... }'", 46), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprLambda{zero_params, si(42), unk_t(), ast::span_unknown()}), ctx), mlc::String("[]() { return 42; }", 19)));
results.push_back(test_runner::assert_eq_str(mlc::String("map_method: length - length", 27), cpp_naming::map_method(mlc::String("length", 6)), mlc::String("length", 6)));
results.push_back(test_runner::assert_eq_str(mlc::String("map_method: len - length", 24), cpp_naming::map_method(mlc::String("len", 3)), mlc::String("length", 6)));
results.push_back(test_runner::assert_eq_str(mlc::String("map_method: push - push_back", 28), cpp_naming::map_method(mlc::String("push", 4)), mlc::String("push_back", 9)));
results.push_back(test_runner::assert_eq_str(mlc::String("map_method: upper - upper", 25), cpp_naming::map_method(mlc::String("upper", 5)), mlc::String("upper", 5)));
results.push_back(test_runner::assert_eq_str(mlc::String("map_method: to_upper - upper", 28), cpp_naming::map_method(mlc::String("to_upper", 8)), mlc::String("upper", 5)));
results.push_back(test_runner::assert_eq_str(mlc::String("map_method: to_lower - lower", 28), cpp_naming::map_method(mlc::String("to_lower", 8)), mlc::String("lower", 5)));
results.push_back(test_runner::assert_eq_str(mlc::String("map_method: trim passthrough", 28), cpp_naming::map_method(mlc::String("trim", 4)), mlc::String("trim", 4)));
results.push_back(test_runner::assert_eq_str(mlc::String("map_method: split passthrough", 29), cpp_naming::map_method(mlc::String("split", 5)), mlc::String("split", 5)));
results.push_back(test_runner::assert_eq_str(mlc::String("map_method: chars passthrough", 29), cpp_naming::map_method(mlc::String("chars", 5)), mlc::String("chars", 5)));
results.push_back(test_runner::assert_eq_str(mlc::String("map_method: lines passthrough", 29), cpp_naming::map_method(mlc::String("lines", 5)), mlc::String("lines", 5)));
auto call_lambda_args = mlc::Array{si(5), std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprLambda{single_params, std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBin{mlc::String("+", 1), sid(mlc::String("x", 1)), si(1), i32_t(), ast::span_unknown()}), unk_t(), ast::span_unknown()})};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprCall with lambda arg", 24), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprCall{sid(mlc::String("apply", 5)), call_lambda_args, unk_t(), ast::span_unknown()}), ctx), mlc::String("apply(5, [=](auto x) { return (x + 1); })", 41)));
auto ok_args = mlc::Array{si(42)};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprCall ctor Ok(42) uses brace init", 36), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprCall{sid(mlc::String("Ok", 2)), ok_args, unk_t(), ast::span_unknown()}), ctx), mlc::String("Ok{42}", 6)));
auto err_args = mlc::Array{ss(mlc::String("oops", 4))};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprCall ctor Err(str) uses brace init", 38), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprCall{sid(mlc::String("Err", 3)), err_args, unk_t(), ast::span_unknown()}), ctx), mlc::String("Err{mlc::String(\"oops\", 4)}", 27)));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprIf", 6), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprIf{sb(true), si(1), si(2), i32_t(), ast::span_unknown()}), ctx), mlc::String("(true ? 1 : 2)", 14)));
auto arr_elems = mlc::Array{si(1), si(2), si(3)};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprArray", 9), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprArray{arr_elems, std::make_shared<registry::Type>(registry::TArray{i32_t()}), ast::span_unknown()}), ctx), mlc::String("mlc::Array{1, 2, 3}", 19)));
auto empty_arr = mlc::Array<std::shared_ptr<semantic_ir::SExpr>>{};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprArray empty", 15), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprArray{empty_arr, std::make_shared<registry::Type>(registry::TArray{i32_t()}), ast::span_unknown()}), ctx), mlc::String("{}", 2)));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprIndex", 9), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprIndex{sid(mlc::String("arr", 3)), si(0), i32_t(), ast::span_unknown()}), ctx), mlc::String("arr[0]", 6)));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprField on ident", 18), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprField{sid(mlc::String("point", 5)), mlc::String("x", 1), unk_t(), ast::span_unknown()}), ctx), mlc::String("point.x", 7)));
auto method_args = mlc::Array{si(1)};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprMethod push", 15), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMethod{sid(mlc::String("arr", 3)), mlc::String("push", 4), method_args, unit_t(), ast::span_unknown()}), ctx), mlc::String("arr.push_back(1)", 16)));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprMethod length", 17), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMethod{sid(mlc::String("arr", 3)), mlc::String("length", 6), empty_arr, i32_t(), ast::span_unknown()}), ctx), mlc::String("arr.length()", 12)));
auto result_r_t = std::make_shared<registry::Type>(registry::TGeneric{mlc::String("Result", 6), mlc::Array{i32_t(), str_t()}});
auto s_result = std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprIdent{mlc::String("r", 1), result_r_t, ast::span_unknown()});
auto map_one = mlc::Array{std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprIdent{mlc::String("f", 1), unk_t(), ast::span_unknown()})};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprMethod Result map", 21), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMethod{s_result, mlc::String("map", 3), map_one, unk_t(), ast::span_unknown()}), ctx), mlc::String("mlc::result::map(r, f)", 22)));
auto namespace_alias_prefixes = mlc::HashMap<mlc::String, mlc::String>();
namespace_alias_prefixes.set(mlc::String("helpers", 7), mlc::String("infer_literals::", 16));
auto context_with_aliases = context::context_with_namespace_alias_prefixes(ctx, namespace_alias_prefixes);
results.push_back(test_runner::assert_eq_str(mlc::String("ExprMethod on import-as alias emits namespace::fn()", 51), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMethod{sid(mlc::String("helpers", 7)), mlc::String("infer_expr_integer_literal", 26), empty_arr, unk_t(), ast::span_unknown()}), context_with_aliases), mlc::String("infer_literals::infer_expr_integer_literal()", 44)));
auto pat_arms = mlc::Array{std::make_shared<semantic_ir::SMatchArm>(semantic_ir::SMatchArm{std::make_shared<ast::Pat>(ast::PatInt{1, ast::span_unknown()}), ss(mlc::String("one", 3))}), std::make_shared<semantic_ir::SMatchArm>(semantic_ir::SMatchArm{std::make_shared<ast::Pat>(ast::PatWild{ast::span_unknown()}), ss(mlc::String("other", 5))})};
auto match_out = eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMatch{sid(mlc::String("n", 1)), pat_arms, str_t(), ast::span_unknown()}), ctx);
results.push_back(test_runner::assert_eq_str(mlc::String("ExprMatch contains std::visit", 29), (match_out.contains(mlc::String("std::visit", 10)) ? mlc::String("yes", 3) : mlc::String("no", 2)), mlc::String("yes", 3)));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprMatch contains one", 22), (match_out.contains(mlc::String("\"one\"", 5)) ? mlc::String("yes", 3) : mlc::String("no", 2)), mlc::String("yes", 3)));
results.push_back(test_runner::assert_eq_str(mlc::String("ExprMatch contains other", 24), (match_out.contains(mlc::String("\"other\"", 7)) ? mlc::String("yes", 3) : mlc::String("no", 2)), mlc::String("yes", 3)));
auto or_arms = mlc::Array{std::make_shared<semantic_ir::SMatchArm>(semantic_ir::SMatchArm{std::make_shared<ast::Pat>(ast::PatOr{mlc::Array{std::make_shared<ast::Pat>(ast::PatCtor{mlc::String("Circle", 6), mlc::Array{std::make_shared<ast::Pat>(ast::PatWild{ast::span_unknown()})}, ast::span_unknown()}), std::make_shared<ast::Pat>(ast::PatCtor{mlc::String("Square", 6), mlc::Array{std::make_shared<ast::Pat>(ast::PatWild{ast::span_unknown()})}, ast::span_unknown()})}, ast::span_unknown()}), ss(mlc::String("r", 1))})};
auto or_match_out = eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprMatch{sid(mlc::String("s", 1)), or_arms, str_t(), ast::span_unknown()}), ctx);
results.push_back(test_runner::assert_eq_str(mlc::String("PatOr expanded: Circle lambda present", 37), (or_match_out.contains(mlc::String("Circle", 6)) ? mlc::String("yes", 3) : mlc::String("no", 2)), mlc::String("yes", 3)));
results.push_back(test_runner::assert_eq_str(mlc::String("PatOr expanded: Square lambda present", 37), (or_match_out.contains(mlc::String("Square", 6)) ? mlc::String("yes", 3) : mlc::String("no", 2)), mlc::String("yes", 3)));
auto while_body = mlc::Array{std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtExpr{si(0), ast::span_unknown()})};
auto while_out = eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprWhile{sb(true), while_body, unit_t(), ast::span_unknown()}), ctx);
results.push_back(test_runner::assert_eq_str(mlc::String("ExprWhile contains while (true)", 31), (while_out.contains(mlc::String("while (true)", 12)) ? mlc::String("yes", 3) : mlc::String("no", 2)), mlc::String("yes", 3)));
auto block_stmts = mlc::Array{std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtLet{mlc::String("x", 1), false, si(1), i32_t(), ast::span_unknown()})};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprBlock", 9), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprBlock{block_stmts, sid(mlc::String("x", 1)), i32_t(), ast::span_unknown()}), ctx), mlc::String("[&]() {\nauto x = 1;\nreturn x;\n}()", 33)));
results.push_back(test_runner::assert_eq_str(mlc::String("gen_stmts_str: let const emits constexpr", 40), eval::gen_stmts_str(mlc::Array{std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtLetConst{mlc::String("limit", 5), si(42), i32_t(), ast::span_unknown()})}, ctx), mlc::String("constexpr auto limit = 42;\n", 27)));
auto fv_a = mlc::Array{std::make_shared<semantic_ir::SFieldVal>(semantic_ir::SFieldVal{mlc::String("a", 1), si(1)}), std::make_shared<semantic_ir::SFieldVal>(semantic_ir::SFieldVal{mlc::String("b", 1), si(2)})};
results.push_back(test_runner::assert_eq_str(mlc::String("ExprRecord positional", 21), eval::gen_expr(std::make_shared<semantic_ir::SExpr>(semantic_ir::SExprRecord{mlc::String("Point", 5), fv_a, std::make_shared<registry::Type>(registry::TNamed{mlc::String("Point", 5)}), ast::span_unknown()}), ctx), mlc::String("Point{1, 2}", 11)));
auto fn_param = mlc::Array{std::make_shared<ast::Param>(ast::Param{mlc::String("n", 1), false, std::make_shared<ast::TypeExpr>(ast::TyI32{}), false, std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()})})};
auto fn_body = std::make_shared<ast::Expr>(ast::ExprBin{mlc::String("*", 1), std::make_shared<ast::Expr>(ast::ExprIdent{mlc::String("n", 1), ast::span_unknown()}), std::make_shared<ast::Expr>(ast::ExprInt{2, ast::span_unknown()}), ast::span_unknown()});
auto fn_decl = ast::DeclFn{mlc::String("double_it", 9), {}, {}, fn_param, std::make_shared<ast::TypeExpr>(ast::TyI32{}), fn_body};
auto fn_prog = ast::Program{mlc::Array{std::make_shared<ast::Decl>(ast::DeclExported{std::make_shared<ast::Decl>(fn_decl)})}};
auto fn_prog_out = module::gen_program(fn_prog);
results.push_back(test_runner::assert_eq_str(mlc::String("DeclFn: signature in output", 27), (fn_prog_out.contains(mlc::String("double_it", 9)) ? mlc::String("yes", 3) : mlc::String("no", 2)), mlc::String("yes", 3)));
results.push_back(test_runner::assert_eq_str(mlc::String("DeclFn: param type in output", 28), (fn_prog_out.contains(mlc::String("int n", 5)) ? mlc::String("yes", 3) : mlc::String("no", 2)), mlc::String("yes", 3)));
results.push_back(test_runner::assert_eq_str(mlc::String("DeclFn: body in output", 22), (fn_prog_out.contains(mlc::String("return (n * 2)", 14)) ? mlc::String("yes", 3) : mlc::String("no", 2)), mlc::String("yes", 3)));
auto result_type_params = mlc::Array{mlc::String("T", 1), mlc::String("E", 1)};
auto ok_fields = mlc::Array{std::make_shared<ast::TypeExpr>(ast::TyNamed{mlc::String("T", 1)})};
auto err_fields = mlc::Array{std::make_shared<ast::TypeExpr>(ast::TyNamed{mlc::String("E", 1)})};
auto result_variants = mlc::Array{std::make_shared<ast::TypeVariant>(ast::VarTuple{mlc::String("Ok", 2), ok_fields}), std::make_shared<ast::TypeVariant>(ast::VarTuple{mlc::String("Err", 3), err_fields})};
auto expected_fwd = ((mlc::String("template<typename T>\nstruct Ok;\n", 32) + mlc::String("template<typename E>\nstruct Err;\n", 33)) + mlc::String("template<typename T, typename E>\nusing Result = std::variant<Ok<T>, Err<E>>;\n", 77));
auto expected_body = (mlc::String("template<typename T>\nstruct Ok {T field0;};\n", 44) + mlc::String("template<typename E>\nstruct Err {E field0;};\n", 45));
results.push_back(test_runner::assert_eq_str(mlc::String("generic type Result<T,E> = Ok(T) | Err(E) codegen", 49), type_gen::gen_type_decl(ctx, mlc::String("Result", 6), result_type_params, result_variants), (expected_fwd + expected_body)));
results.push_back(test_runner::assert_eq_str(mlc::String("string literal with embedded newline - escapes to \\n in C++", 59), eval::gen_expr(ss(mlc::String("hello\nworld", 11)), ctx), mlc::String("mlc::String(\"hello\\nworld\", 11)", 31)));
results.push_back(test_runner::assert_eq_str(mlc::String("string literal with tab - escapes to \\t", 39), eval::gen_expr(ss(mlc::String("a\tb", 3)), ctx), mlc::String("mlc::String(\"a\\tb\", 3)", 22)));
results.push_back(test_runner::assert_eq_str(mlc::String("string literal with backslash - escapes to \\\\", 45), eval::gen_expr(ss(mlc::String("a\\b", 3)), ctx), mlc::String("mlc::String(\"a\\\\b\", 3)", 22)));
results.push_back(test_runner::assert_eq_str(mlc::String("string literal with double quote - escapes to \\\"", 48), eval::gen_expr(ss(mlc::String("say \"hi\"", 8)), ctx), mlc::String("mlc::String(\"say \\\"hi\\\"\", 8)", 28)));
return results;
}

} // namespace test_codegen
