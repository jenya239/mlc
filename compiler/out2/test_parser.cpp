#include "test_parser.hpp"

#include "test_runner.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "ast.hpp"
#include "exprs.hpp"
#include "preds.hpp"

namespace test_parser {

using namespace test_runner;
using namespace lexer;
using namespace decls;
using namespace ast;
using namespace exprs;
using namespace preds;
using namespace ast_tokens;

ast::Program parse_source(mlc::String source) noexcept;

std::shared_ptr<ast::Expr> parse_expr_source(mlc::String source) noexcept;

int decl_count(mlc::String source) noexcept;

std::shared_ptr<ast::Decl> first_decl(mlc::String source) noexcept;

std::shared_ptr<ast::Decl> second_decl(mlc::String source) noexcept;

mlc::Array<test_runner::TestResult> parser_tests() noexcept;

ast::Program parse_source(mlc::String source) noexcept{return decls::parse_program(lexer::tokenize(source).tokens);}

std::shared_ptr<ast::Expr> parse_expr_source(mlc::String source) noexcept{return exprs::parse_expr(preds::parser_new(lexer::tokenize(source).tokens)).expr;}

int decl_count(mlc::String source) noexcept{return parse_source(source).decls.size();}

std::shared_ptr<ast::Decl> first_decl(mlc::String source) noexcept{return parse_source(source).decls[0];}

std::shared_ptr<ast::Decl> second_decl(mlc::String source) noexcept{return parse_source(source).decls[1];}

mlc::Array<test_runner::TestResult> parser_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results.push_back(test_runner::assert_eq_int(mlc::String("empty source - 0 decls"), decl_count(mlc::String("")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("single fn decl - 1 decl"), decl_count(mlc::String("fn f() -> i32 = 42")), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("two fn decls - 2 decls"), decl_count(mlc::String("fn f() -> i32 = 1\nfn g() -> i32 = 2")), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("type decl - 1 decl"), decl_count(mlc::String("type Color = Red | Green | Blue")), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("fn with params - 1 decl"), decl_count(mlc::String("fn add(x: i32, y: i32) -> i32 = x + y")), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("fn with default param - 1 decl"), decl_count(mlc::String("fn f(a: i32 = 1) -> i32 = a")), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("fn with do block - 1 decl"), decl_count(mlc::String("fn f() -> i32 = do\n  const x = 1\n  x\nend")), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("type + fn - 2 decls"), decl_count(mlc::String("type Color = Red\nfn f() -> i32 = 1")), 2));
results.push_back(test_runner::assert_true(mlc::String("fn decl is DeclFn"), [&]() { if (std::holds_alternative<ast::DeclFn>((*first_decl(mlc::String("fn f() -> i32 = 42"))))) { auto _v_declfn = std::get<ast::DeclFn>((*first_decl(mlc::String("fn f() -> i32 = 42")))); auto [_w0, _w1, _w2, _w3, _w4, _w5] = _v_declfn; return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("type decl is DeclType"), [&]() { if (std::holds_alternative<ast::DeclType>((*first_decl(mlc::String("type Color = Red | Green"))))) { auto _v_decltype = std::get<ast::DeclType>((*first_decl(mlc::String("type Color = Red | Green")))); auto [_w0, _w1, _w2, _w3] = _v_decltype; return true; } return false; }()));
results.push_back(test_runner::assert_eq_int(mlc::String("fn with if expr - 1 decl"), decl_count(mlc::String("fn f(x: i32) -> i32 = if x > 0 then 1 else 0 end")), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("fn with for-in - 1 decl"), decl_count(mlc::String("fn f(items: [i32]) -> i32 = do\n  let mut s = 0\n  for x in items do\n    s = s + x\n  end\n  s\nend")), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("type + fn with match pipe arms - 2 decls"), decl_count(mlc::String("type T = A(i32) | B\nfn f(x: T) -> i32 = match x | A(n) => n | B => 0\n")), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("fn with parenthesized lambda - 1 decl"), decl_count(mlc::String("fn f() -> i32 = (() => 7)()")), 1));
results.push_back(test_runner::assert_true(mlc::String("extend method: self with type and extra param"), [&]() { if (std::holds_alternative<ast::DeclExtend>((*second_decl(mlc::String("type R = { a: i32 }\nextend R {\n  fn m(self: R, x: i32) -> i32 = x\n}"))))) { auto _v_declextend = std::get<ast::DeclExtend>((*second_decl(mlc::String("type R = { a: i32 }\nextend R {\n  fn m(self: R, x: i32) -> i32 = x\n}")))); auto [_w0, _w1, methods] = _v_declextend; return [&]() { if (std::holds_alternative<ast::DeclFn>((*methods[0]))) { auto _v_declfn = std::get<ast::DeclFn>((*methods[0])); auto [_w0, _w1, _w2, parameters, _w3, _w4] = _v_declfn; return parameters.size() == 2; } return false; }(); } return false; }()));
results.push_back(test_runner::assert_eq_int(mlc::String("import star-as - 1 decl"), decl_count(mlc::String("import * as helpers from \"./infer_literals.mlc\"")), 1));
results.push_back(test_runner::assert_true(mlc::String("import star-as: path and symbols"), [&]() { if (std::holds_alternative<ast::DeclImport>((*first_decl(mlc::String("import * as helpers from \"./infer_literals.mlc\""))))) { auto _v_declimport = std::get<ast::DeclImport>((*first_decl(mlc::String("import * as helpers from \"./infer_literals.mlc\"")))); auto [path, symbols] = _v_declimport; return path == mlc::String("./infer_literals.mlc") && symbols.size() == 2 && symbols[0] == mlc::String("*") && symbols[1] == mlc::String("helpers"); } return false; }()));
results.push_back(test_runner::assert_eq_int(mlc::String("import named list - 1 decl"), decl_count(mlc::String("import { foo, bar } from \"./mod.mlc\"")), 1));
results.push_back(test_runner::assert_true(mlc::String("import named list: path and symbols order"), [&]() { if (std::holds_alternative<ast::DeclImport>((*first_decl(mlc::String("import { foo, bar } from \"./mod.mlc\""))))) { auto _v_declimport = std::get<ast::DeclImport>((*first_decl(mlc::String("import { foo, bar } from \"./mod.mlc\"")))); auto [path, symbols] = _v_declimport; return path == mlc::String("./mod.mlc") && symbols.size() == 2 && symbols[0] == mlc::String("foo") && symbols[1] == mlc::String("bar"); } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("import single name in braces"), [&]() { if (std::holds_alternative<ast::DeclImport>((*first_decl(mlc::String("import { only } from \"./one.mlc\""))))) { auto _v_declimport = std::get<ast::DeclImport>((*first_decl(mlc::String("import { only } from \"./one.mlc\"")))); auto [path, symbols] = _v_declimport; return path == mlc::String("./one.mlc") && symbols.size() == 1 && symbols[0] == mlc::String("only"); } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("or-pattern brace-style: PatOr produced"), [&]() { if (std::holds_alternative<ast::ExprMatch>((*parse_expr_source(mlc::String("match x { A(r) | B(r) => r }")))._)) { auto _v_exprmatch = std::get<ast::ExprMatch>((*parse_expr_source(mlc::String("match x { A(r) | B(r) => r }")))._); auto [_w0, arms, _w1] = _v_exprmatch; return arms.size() == 1 ? [&]() { if (std::holds_alternative<ast::PatOr>((*arms[0]->pat))) { auto _v_pator = std::get<ast::PatOr>((*arms[0]->pat)); auto [alts, _w0] = _v_pator; return alts.size() == 2; } return false; }() : false; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("or-pattern pipe-style: PatOr produced"), [&]() { if (std::holds_alternative<ast::ExprMatch>((*parse_expr_source(mlc::String("match x | A(r) | B(r) => r end")))._)) { auto _v_exprmatch = std::get<ast::ExprMatch>((*parse_expr_source(mlc::String("match x | A(r) | B(r) => r end")))._); auto [_w0, arms, _w1] = _v_exprmatch; return arms.size() == 1 ? [&]() { if (std::holds_alternative<ast::PatOr>((*arms[0]->pat))) { auto _v_pator = std::get<ast::PatOr>((*arms[0]->pat)); auto [alts, _w0] = _v_pator; return alts.size() == 2; } return false; }() : false; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("unit or-pattern: PatOr produced"), [&]() { if (std::holds_alternative<ast::ExprMatch>((*parse_expr_source(mlc::String("match x { Red | Green => 1, Blue => 2 }")))._)) { auto _v_exprmatch = std::get<ast::ExprMatch>((*parse_expr_source(mlc::String("match x { Red | Green => 1, Blue => 2 }")))._); auto [_w0, arms, _w1] = _v_exprmatch; return arms.size() == 2 ? [&]() { if (std::holds_alternative<ast::PatOr>((*arms[0]->pat))) { auto _v_pator = std::get<ast::PatOr>((*arms[0]->pat)); auto [alts, _w0] = _v_pator; return alts.size() == 2; } return false; }() : false; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("single pattern: no PatOr"), [&]() { if (std::holds_alternative<ast::ExprMatch>((*parse_expr_source(mlc::String("match x { A(r) => r }")))._)) { auto _v_exprmatch = std::get<ast::ExprMatch>((*parse_expr_source(mlc::String("match x { A(r) => r }")))._); auto [_w0, arms, _w1] = _v_exprmatch; return arms.size() == 1 ? [&]() { if (std::holds_alternative<ast::PatCtor>((*arms[0]->pat))) { auto _v_patctor = std::get<ast::PatCtor>((*arms[0]->pat)); auto [name, _w0, _w1] = _v_patctor; return name == mlc::String("A"); } return false; }() : false; } return false; }()));
results.push_back(test_runner::assert_eq_int(mlc::String("plain template literal - 1 decl"), decl_count(mlc::String("fn f() -> string = `hello`")), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("template with interpolation - 1 decl"), decl_count(mlc::String("fn f(x: i32) -> string = `a ${x} b`")), 1));
results.push_back(test_runner::assert_true(mlc::String("plain template parses to ExprStr"), [&]() { if (std::holds_alternative<ast::ExprStr>((*parse_expr_source(mlc::String("`hello`")))._)) { auto _v_exprstr = std::get<ast::ExprStr>((*parse_expr_source(mlc::String("`hello`")))._); auto [s, _w0] = _v_exprstr; return s == mlc::String("hello"); } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("template with interp parses to ExprBin"), [&]() { if (std::holds_alternative<ast::ExprBin>((*parse_expr_source(mlc::String("`a ${x}`")))._)) { auto _v_exprbin = std::get<ast::ExprBin>((*parse_expr_source(mlc::String("`a ${x}`")))._); auto [op, _w0, _w1, _w2] = _v_exprbin; return op == mlc::String("+"); } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("template with only interp parses to ExprBin"), [&]() { if (std::holds_alternative<ast::ExprBin>((*parse_expr_source(mlc::String("`${x}`")))._)) { auto _v_exprbin = std::get<ast::ExprBin>((*parse_expr_source(mlc::String("`${x}`")))._); auto [op, _w0, _w1, _w2] = _v_exprbin; return op == mlc::String("+"); } return false; }()));
return results;
}

} // namespace test_parser
