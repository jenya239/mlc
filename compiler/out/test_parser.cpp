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

bool expr_call_one_ident_argument(std::shared_ptr<ast::Expr> expression, mlc::String expected_function, mlc::String expected_argument) noexcept;

bool pipe_case_logical_or_over_pipeline(std::shared_ptr<ast::Expr> expression) noexcept;

bool pipe_case_logical_and_over_pipeline(std::shared_ptr<ast::Expr> expression) noexcept;

bool pipe_case_equality_right_operand_contains_pipeline(std::shared_ptr<ast::Expr> expression) noexcept;

bool pipe_pipeline_left_equals_shape(std::shared_ptr<ast::Expr> left_equals, std::shared_ptr<ast::Expr> right_equals) noexcept;

bool pipe_case_pipeline_binds_tighter_than_equality_on_left(std::shared_ptr<ast::Expr> expression) noexcept;

int expr_match_arm_count(std::shared_ptr<ast::Expr> parsed_expression) noexcept;

bool expr_match_arm_has_guard(std::shared_ptr<ast::Expr> parsed_expression, int arm_index) noexcept;

mlc::String expr_guard_binary_operation(std::shared_ptr<ast::Expr> candidate_guard) noexcept;

mlc::String expr_pat_ident_binding_name(std::shared_ptr<ast::Pat> pattern) noexcept;

int expr_pat_or_alt_count(std::shared_ptr<ast::Pat> pattern) noexcept;

mlc::String expr_match_arm_guard_binary_operation(std::shared_ptr<ast::Expr> parsed_expression, int arm_index) noexcept;

mlc::String expr_match_arm_pat_ident_name(std::shared_ptr<ast::Expr> parsed_expression, int arm_index) noexcept;

int expr_match_arm_pat_or_alt_count(std::shared_ptr<ast::Expr> parsed_expression, int arm_index) noexcept;

std::shared_ptr<ast::Expr> sample_parse_match_guard_brace() noexcept;

std::shared_ptr<ast::Expr> sample_parse_match_guard_pipe() noexcept;

std::shared_ptr<ast::Expr> sample_parse_match_guard_or_pattern() noexcept;

int decl_count(mlc::String source) noexcept;

std::shared_ptr<ast::Decl> first_decl(mlc::String source) noexcept;

std::shared_ptr<ast::Decl> second_decl(mlc::String source) noexcept;

bool trait_display_decl(ast::Program program) noexcept;

mlc::Array<test_runner::TestResult> parser_tests() noexcept;

ast::Program parse_source(mlc::String source) noexcept{return decls::parse_program(lexer::tokenize(source).tokens);}

std::shared_ptr<ast::Expr> parse_expr_source(mlc::String source) noexcept{return exprs::parse_expr(preds::parser_new(lexer::tokenize(source).tokens)).expr;}

bool expr_call_one_ident_argument(std::shared_ptr<ast::Expr> expression, mlc::String expected_function, mlc::String expected_argument) noexcept{return [&]() { if (std::holds_alternative<ast::ExprCall>((*expression)._)) { auto _v_exprcall = std::get<ast::ExprCall>((*expression)._); auto [callee, arguments, _w0] = _v_exprcall; return [&]() { if (std::holds_alternative<ast::ExprIdent>((*callee)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*callee)._); auto [function_name, _w0] = _v_exprident; return function_name != expected_function || arguments.size() != 1 ? false : [&]() { if (std::holds_alternative<ast::ExprIdent>((*arguments[0])._)) { auto _v_exprident = std::get<ast::ExprIdent>((*arguments[0])._); auto [argument_name, _w0] = _v_exprident; return argument_name == expected_argument; } return false; }(); } return false; }(); } return false; }();}

bool pipe_case_logical_or_over_pipeline(std::shared_ptr<ast::Expr> expression) noexcept{return [&]() { if (std::holds_alternative<ast::ExprBin>((*expression)._)) { auto _v_exprbin = std::get<ast::ExprBin>((*expression)._); auto [binary_operator, left_logical, right_logical, _w0] = _v_exprbin; return binary_operator != mlc::String("||") ? false : [&]() { if (std::holds_alternative<ast::ExprIdent>((*left_logical)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*left_logical)._); auto [left_name, _w0] = _v_exprident; return left_name == mlc::String("a") && expr_call_one_ident_argument(right_logical, mlc::String("c"), mlc::String("b")); } return false; }(); } return false; }();}

bool pipe_case_logical_and_over_pipeline(std::shared_ptr<ast::Expr> expression) noexcept{return [&]() { if (std::holds_alternative<ast::ExprBin>((*expression)._)) { auto _v_exprbin = std::get<ast::ExprBin>((*expression)._); auto [binary_operator, left_conjunct, right_conjunct, _w0] = _v_exprbin; return binary_operator != mlc::String("&&") ? false : [&]() { if (std::holds_alternative<ast::ExprIdent>((*left_conjunct)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*left_conjunct)._); auto [left_name, _w0] = _v_exprident; return left_name == mlc::String("a") && expr_call_one_ident_argument(right_conjunct, mlc::String("c"), mlc::String("b")); } return false; }(); } return false; }();}

bool pipe_case_equality_right_operand_contains_pipeline(std::shared_ptr<ast::Expr> expression) noexcept{return [&]() { if (std::holds_alternative<ast::ExprBin>((*expression)._)) { auto _v_exprbin = std::get<ast::ExprBin>((*expression)._); auto [binary_operator, left_equals, right_equals, _w0] = _v_exprbin; return binary_operator != mlc::String("==") ? false : [&]() { if (std::holds_alternative<ast::ExprIdent>((*left_equals)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*left_equals)._); auto [left_name, _w0] = _v_exprident; return left_name == mlc::String("a") && expr_call_one_ident_argument(right_equals, mlc::String("c"), mlc::String("b")); } return false; }(); } return false; }();}

bool pipe_pipeline_left_equals_shape(std::shared_ptr<ast::Expr> left_equals, std::shared_ptr<ast::Expr> right_equals) noexcept{return [&]() { if (std::holds_alternative<ast::ExprCall>((*left_equals)._)) { auto _v_exprcall = std::get<ast::ExprCall>((*left_equals)._); auto [callee, arguments, _w0] = _v_exprcall; return [&]() { if (std::holds_alternative<ast::ExprIdent>((*callee)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*callee)._); auto [function_name, _w0] = _v_exprident; return [&]() { if (std::holds_alternative<ast::ExprIdent>((*right_equals)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*right_equals)._); auto [right_name, _w0] = _v_exprident; return function_name != mlc::String("b") || right_name != mlc::String("c") || arguments.size() != 1 ? false : [&]() { if (std::holds_alternative<ast::ExprIdent>((*arguments[0])._)) { auto _v_exprident = std::get<ast::ExprIdent>((*arguments[0])._); auto [argument_name, _w0] = _v_exprident; return argument_name == mlc::String("a"); } return false; }(); } return false; }(); } return false; }(); } return false; }();}

bool pipe_case_pipeline_binds_tighter_than_equality_on_left(std::shared_ptr<ast::Expr> expression) noexcept{return [&]() { if (std::holds_alternative<ast::ExprBin>((*expression)._)) { auto _v_exprbin = std::get<ast::ExprBin>((*expression)._); auto [binary_operator, left_equals, right_equals, _w0] = _v_exprbin; return binary_operator != mlc::String("==") ? false : pipe_pipeline_left_equals_shape(left_equals, right_equals); } return false; }();}

int expr_match_arm_count(std::shared_ptr<ast::Expr> parsed_expression) noexcept{return [&]() { if (std::holds_alternative<ast::ExprMatch>((*parsed_expression)._)) { auto _v_exprmatch = std::get<ast::ExprMatch>((*parsed_expression)._); auto [_w0, arms, _w1] = _v_exprmatch; return [&]() -> int { 
  int count = 0;
  while (count < arms.size()){
{
count = count + 1;
}
}
  return count;
 }(); } return -1; }();}

bool expr_match_arm_has_guard(std::shared_ptr<ast::Expr> parsed_expression, int arm_index) noexcept{return [&]() { if (std::holds_alternative<ast::ExprMatch>((*parsed_expression)._)) { auto _v_exprmatch = std::get<ast::ExprMatch>((*parsed_expression)._); auto [_w0, arms, _w1] = _v_exprmatch; return arm_index < 0 || arm_index >= arms.size() ? false : arms[arm_index]->has_guard; } return false; }();}

mlc::String expr_guard_binary_operation(std::shared_ptr<ast::Expr> candidate_guard) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprBin>((*candidate_guard)._)) { auto _v_exprbin = std::get<ast::ExprBin>((*candidate_guard)._); auto [binary_operation, _w0, _w1, _w2] = _v_exprbin; return binary_operation; } return mlc::String(""); }();}

mlc::String expr_pat_ident_binding_name(std::shared_ptr<ast::Pat> pattern) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::PatIdent>((*pattern))) { auto _v_patident = std::get<ast::PatIdent>((*pattern)); auto [binding_name, _w0] = _v_patident; return binding_name; } return mlc::String(""); }();}

int expr_pat_or_alt_count(std::shared_ptr<ast::Pat> pattern) noexcept{return [&]() { if (std::holds_alternative<ast::PatOr>((*pattern))) { auto _v_pator = std::get<ast::PatOr>((*pattern)); auto [alts, _w0] = _v_pator; return [&]() -> int { 
  int count = 0;
  while (count < alts.size()){
{
count = count + 1;
}
}
  return count;
 }(); } return -1; }();}

mlc::String expr_match_arm_guard_binary_operation(std::shared_ptr<ast::Expr> parsed_expression, int arm_index) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprMatch>((*parsed_expression)._)) { auto _v_exprmatch = std::get<ast::ExprMatch>((*parsed_expression)._); auto [_w0, arms, _w1] = _v_exprmatch; return arm_index < 0 || arm_index >= arms.size() ? mlc::String("") : expr_guard_binary_operation(arms[arm_index]->when_condition); } return mlc::String(""); }();}

mlc::String expr_match_arm_pat_ident_name(std::shared_ptr<ast::Expr> parsed_expression, int arm_index) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprMatch>((*parsed_expression)._)) { auto _v_exprmatch = std::get<ast::ExprMatch>((*parsed_expression)._); auto [_w0, arms, _w1] = _v_exprmatch; return arm_index < 0 || arm_index >= arms.size() ? mlc::String("") : expr_pat_ident_binding_name(arms[arm_index]->pat); } return mlc::String(""); }();}

int expr_match_arm_pat_or_alt_count(std::shared_ptr<ast::Expr> parsed_expression, int arm_index) noexcept{return [&]() { if (std::holds_alternative<ast::ExprMatch>((*parsed_expression)._)) { auto _v_exprmatch = std::get<ast::ExprMatch>((*parsed_expression)._); auto [_w0, arms, _w1] = _v_exprmatch; return arm_index < 0 || arm_index >= arms.size() ? -1 : expr_pat_or_alt_count(arms[arm_index]->pat); } return -1; }();}

std::shared_ptr<ast::Expr> sample_parse_match_guard_brace() noexcept{return parse_expr_source(mlc::String("match score { n if n >= 90 => 1 }"));}

std::shared_ptr<ast::Expr> sample_parse_match_guard_pipe() noexcept{return parse_expr_source(mlc::String("match x | n if n > 0 => 1 end"));}

std::shared_ptr<ast::Expr> sample_parse_match_guard_or_pattern() noexcept{return parse_expr_source(mlc::String("match x { A(r) | B(r) if r > 0 => r }"));}

int decl_count(mlc::String source) noexcept{return parse_source(source).decls.size();}

std::shared_ptr<ast::Decl> first_decl(mlc::String source) noexcept{return parse_source(source).decls[0];}

std::shared_ptr<ast::Decl> second_decl(mlc::String source) noexcept{return parse_source(source).decls[1];}

bool trait_display_decl(ast::Program program) noexcept{return [&]() { if (std::holds_alternative<ast::DeclTrait>((*program.decls[0]))) { auto _v_decltrait = std::get<ast::DeclTrait>((*program.decls[0])); auto [name, _w0, methods] = _v_decltrait; return name == mlc::String("Display") && methods.size() == 1; } return false; }();}

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
results.push_back(test_runner::assert_eq_int(mlc::String("record type field default expression — parse decl count"), decl_count(mlc::String("type Line = { offset: i32 = 10 + 20 }\nfn main() -> i32 = 0")), 2));
results.push_back(test_runner::assert_true(mlc::String("fn decl is DeclFn"), [&]() { if (std::holds_alternative<ast::DeclFn>((*first_decl(mlc::String("fn f() -> i32 = 42"))))) { auto _v_declfn = std::get<ast::DeclFn>((*first_decl(mlc::String("fn f() -> i32 = 42")))); auto [_w0, _w1, _w2, _w3, _w4, _w5, _w6] = _v_declfn; return true; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("type decl is DeclType"), [&]() { if (std::holds_alternative<ast::DeclType>((*first_decl(mlc::String("type Color = Red | Green"))))) { auto _v_decltype = std::get<ast::DeclType>((*first_decl(mlc::String("type Color = Red | Green")))); auto [_w0, _w1, _w2, _w3] = _v_decltype; return true; } return false; }()));
results.push_back(test_runner::assert_eq_int(mlc::String("fn with if expr - 1 decl"), decl_count(mlc::String("fn f(x: i32) -> i32 = if x > 0 then 1 else 0 end")), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("fn with for-in - 1 decl"), decl_count(mlc::String("fn f(items: [i32]) -> i32 = do\n  let mut s = 0\n  for x in items do\n    s = s + x\n  end\n  s\nend")), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("type + fn with match pipe arms - 2 decls"), decl_count(mlc::String("type T = A(i32) | B\nfn f(x: T) -> i32 = match x | A(n) => n | B => 0\n")), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("trait plus fn with where clause — 2 decls"), decl_count(mlc::String("type Display { fn to_string(self: Self) -> string }\nfn f<T>(x: T) -> unit where T: Display = ()")), 2));
results.push_back(test_runner::assert_eq_int(mlc::String("fn with parenthesized lambda - 1 decl"), decl_count(mlc::String("fn f() -> i32 = (() => 7)()")), 1));
results.push_back(test_runner::assert_true(mlc::String("extend method: self with type and extra param"), [&]() { if (std::holds_alternative<ast::DeclExtend>((*second_decl(mlc::String("type R = { a: i32 }\nextend R {\n  fn m(self: R, x: i32) -> i32 = x\n}"))))) { auto _v_declextend = std::get<ast::DeclExtend>((*second_decl(mlc::String("type R = { a: i32 }\nextend R {\n  fn m(self: R, x: i32) -> i32 = x\n}")))); auto [_w0, _w1, methods] = _v_declextend; return [&]() { if (std::holds_alternative<ast::DeclFn>((*methods[0]))) { auto _v_declfn = std::get<ast::DeclFn>((*methods[0])); auto [_w0, _w1, _w2, parameters, _w3, _w4, _w5] = _v_declfn; return parameters.size() == 2; } return false; }(); } return false; }()));
results.push_back(test_runner::assert_eq_int(mlc::String("import star-as - 1 decl"), decl_count(mlc::String("import * as helpers from \"./infer_literals.mlc\"")), 1));
results.push_back(test_runner::assert_true(mlc::String("import star-as: path and symbols"), [&]() { if (std::holds_alternative<ast::DeclImport>((*first_decl(mlc::String("import * as helpers from \"./infer_literals.mlc\""))))) { auto _v_declimport = std::get<ast::DeclImport>((*first_decl(mlc::String("import * as helpers from \"./infer_literals.mlc\"")))); auto [path, symbols] = _v_declimport; return path == mlc::String("./infer_literals.mlc") && symbols.size() == 2 && symbols[0] == mlc::String("*") && symbols[1] == mlc::String("helpers"); } return false; }()));
results.push_back(test_runner::assert_eq_int(mlc::String("import named list - 1 decl"), decl_count(mlc::String("import { foo, bar } from \"./mod.mlc\"")), 1));
results.push_back(test_runner::assert_true(mlc::String("import named list: path and symbols order"), [&]() { if (std::holds_alternative<ast::DeclImport>((*first_decl(mlc::String("import { foo, bar } from \"./mod.mlc\""))))) { auto _v_declimport = std::get<ast::DeclImport>((*first_decl(mlc::String("import { foo, bar } from \"./mod.mlc\"")))); auto [path, symbols] = _v_declimport; return path == mlc::String("./mod.mlc") && symbols.size() == 2 && symbols[0] == mlc::String("foo") && symbols[1] == mlc::String("bar"); } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("import single name in braces"), [&]() { if (std::holds_alternative<ast::DeclImport>((*first_decl(mlc::String("import { only } from \"./one.mlc\""))))) { auto _v_declimport = std::get<ast::DeclImport>((*first_decl(mlc::String("import { only } from \"./one.mlc\"")))); auto [path, symbols] = _v_declimport; return path == mlc::String("./one.mlc") && symbols.size() == 1 && symbols[0] == mlc::String("only"); } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("or-pattern brace-style: PatOr produced"), [&]() { if (std::holds_alternative<ast::ExprMatch>((*parse_expr_source(mlc::String("match x { A(r) | B(r) => r }")))._)) { auto _v_exprmatch = std::get<ast::ExprMatch>((*parse_expr_source(mlc::String("match x { A(r) | B(r) => r }")))._); auto [_w0, arms, _w1] = _v_exprmatch; return arms.size() == 1 ? [&]() { if (std::holds_alternative<ast::PatOr>((*arms[0]->pat))) { auto _v_pator = std::get<ast::PatOr>((*arms[0]->pat)); auto [alts, _w0] = _v_pator; return alts.size() == 2; } return false; }() : false; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("or-pattern pipe-style: PatOr produced"), [&]() { if (std::holds_alternative<ast::ExprMatch>((*parse_expr_source(mlc::String("match x | A(r) | B(r) => r end")))._)) { auto _v_exprmatch = std::get<ast::ExprMatch>((*parse_expr_source(mlc::String("match x | A(r) | B(r) => r end")))._); auto [_w0, arms, _w1] = _v_exprmatch; return arms.size() == 1 ? [&]() { if (std::holds_alternative<ast::PatOr>((*arms[0]->pat))) { auto _v_pator = std::get<ast::PatOr>((*arms[0]->pat)); auto [alts, _w0] = _v_pator; return alts.size() == 2; } return false; }() : false; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("unit or-pattern: PatOr produced"), [&]() { if (std::holds_alternative<ast::ExprMatch>((*parse_expr_source(mlc::String("match x { Red | Green => 1, Blue => 2 }")))._)) { auto _v_exprmatch = std::get<ast::ExprMatch>((*parse_expr_source(mlc::String("match x { Red | Green => 1, Blue => 2 }")))._); auto [_w0, arms, _w1] = _v_exprmatch; return arms.size() == 2 ? [&]() { if (std::holds_alternative<ast::PatOr>((*arms[0]->pat))) { auto _v_pator = std::get<ast::PatOr>((*arms[0]->pat)); auto [alts, _w0] = _v_pator; return alts.size() == 2; } return false; }() : false; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("single pattern: no PatOr, no guard"), [&]() { if (std::holds_alternative<ast::ExprMatch>((*parse_expr_source(mlc::String("match x { A(r) => r }")))._)) { auto _v_exprmatch = std::get<ast::ExprMatch>((*parse_expr_source(mlc::String("match x { A(r) => r }")))._); auto [_w0, arms, _w1] = _v_exprmatch; return arms.size() == 1 ? arms[0]->has_guard ? false : [&]() { if (std::holds_alternative<ast::PatCtor>((*arms[0]->pat))) { auto _v_patctor = std::get<ast::PatCtor>((*arms[0]->pat)); auto [name, _w0, _w1] = _v_patctor; return name == mlc::String("A"); } return false; }() : false; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("match arm guard brace-style"), expr_match_arm_count(sample_parse_match_guard_brace()) == 1 && expr_match_arm_has_guard(sample_parse_match_guard_brace(), 0) && expr_match_arm_pat_ident_name(sample_parse_match_guard_brace(), 0) == mlc::String("n") && expr_match_arm_guard_binary_operation(sample_parse_match_guard_brace(), 0) == mlc::String(">=")));
results.push_back(test_runner::assert_true(mlc::String("match arm guard pipe-style"), expr_match_arm_count(sample_parse_match_guard_pipe()) == 1 && expr_match_arm_has_guard(sample_parse_match_guard_pipe(), 0) && expr_match_arm_guard_binary_operation(sample_parse_match_guard_pipe(), 0) == mlc::String(">")));
results.push_back(test_runner::assert_true(mlc::String("or-pattern with shared guard after alternatives"), expr_match_arm_count(sample_parse_match_guard_or_pattern()) == 1 && expr_match_arm_has_guard(sample_parse_match_guard_or_pattern(), 0) && expr_match_arm_pat_or_alt_count(sample_parse_match_guard_or_pattern(), 0) == 2 && expr_match_arm_guard_binary_operation(sample_parse_match_guard_or_pattern(), 0) == mlc::String(">")));
results.push_back(test_runner::assert_true(mlc::String("match do delimiter with inner do arm body"), [&]() { if (std::holds_alternative<ast::ExprMatch>((*parse_expr_source(mlc::String("match x do\n  0 =>\n    do\n      2\n    end\n  _ => 3\nend")))._)) { auto _v_exprmatch = std::get<ast::ExprMatch>((*parse_expr_source(mlc::String("match x do\n  0 =>\n    do\n      2\n    end\n  _ => 3\nend")))._); auto [_w0, arms, _w1] = _v_exprmatch; return arms.size() == 2 ? [&]() { if (std::holds_alternative<ast::ExprBlock>((*arms[0]->body)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*arms[0]->body)._); auto [_w0, _w1, _w2] = _v_exprblock; return true; } return false; }() : false; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("match catch-all: else => synonyms wildcard PatWild"), [&]() { if (std::holds_alternative<ast::ExprMatch>((*parse_expr_source(mlc::String("match x { 1 => 0, else => 1 }")))._)) { auto _v_exprmatch = std::get<ast::ExprMatch>((*parse_expr_source(mlc::String("match x { 1 => 0, else => 1 }")))._); auto [_w0, arms, _w1] = _v_exprmatch; return arms.size() == 2 ? [&]() { if (std::holds_alternative<ast::PatWild>((*arms[1]->pat))) { auto _v_patwild = std::get<ast::PatWild>((*arms[1]->pat)); auto [_w0] = _v_patwild; return true; } return false; }() : false; } return false; }()));
results.push_back(test_runner::assert_eq_int(mlc::String("plain template literal - 1 decl"), decl_count(mlc::String("fn f() -> string = `hello`")), 1));
results.push_back(test_runner::assert_eq_int(mlc::String("template with interpolation - 1 decl"), decl_count(mlc::String("fn f(x: i32) -> string = `a ${x} b`")), 1));
results.push_back(test_runner::assert_true(mlc::String("plain template parses to ExprStr"), [&]() { if (std::holds_alternative<ast::ExprStr>((*parse_expr_source(mlc::String("`hello`")))._)) { auto _v_exprstr = std::get<ast::ExprStr>((*parse_expr_source(mlc::String("`hello`")))._); auto [s, _w0] = _v_exprstr; return s == mlc::String("hello"); } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("template with interp parses to ExprBin"), [&]() { if (std::holds_alternative<ast::ExprBin>((*parse_expr_source(mlc::String("`a ${x}`")))._)) { auto _v_exprbin = std::get<ast::ExprBin>((*parse_expr_source(mlc::String("`a ${x}`")))._); auto [op, _w0, _w1, _w2] = _v_exprbin; return op == mlc::String("+"); } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("template with only interp parses to ExprBin"), [&]() { if (std::holds_alternative<ast::ExprBin>((*parse_expr_source(mlc::String("`${x}`")))._)) { auto _v_exprbin = std::get<ast::ExprBin>((*parse_expr_source(mlc::String("`${x}`")))._); auto [op, _w0, _w1, _w2] = _v_exprbin; return op == mlc::String("+"); } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("let-else: has_else=true when else present"), [&]() { if (std::holds_alternative<ast::ExprBlock>((*parse_expr_source(mlc::String("do\n  let Some(v) = opt else return 0 end\n  0\nend")))._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*parse_expr_source(mlc::String("do\n  let Some(v) = opt else return 0 end\n  0\nend")))._); auto [stmts, _w0, _w1] = _v_exprblock; return stmts.size() >= 1 ? [&]() { if (std::holds_alternative<ast::StmtLetPat>((*stmts[0])._)) { auto _v_stmtletpat = std::get<ast::StmtLetPat>((*stmts[0])._); auto [_w0, _w1, _w2, _w3, has_else, _w4, _w5] = _v_stmtletpat; return has_else; } return false; }() : false; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("let-else: has_else=false without else"), [&]() { if (std::holds_alternative<ast::ExprBlock>((*parse_expr_source(mlc::String("do\n  let {x} = pt\n  0\nend")))._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*parse_expr_source(mlc::String("do\n  let {x} = pt\n  0\nend")))._); auto [stmts, _w0, _w1] = _v_exprblock; return stmts.size() >= 1 ? [&]() { if (std::holds_alternative<ast::StmtLetPat>((*stmts[0])._)) { auto _v_stmtletpat = std::get<ast::StmtLetPat>((*stmts[0])._); auto [_w0, _w1, _w2, _w3, has_else, _w4, _w5] = _v_stmtletpat; return !has_else; } return false; }() : false; } return false; }()));
results.push_back(test_runner::assert_true(mlc::String("pipe precedence: logical or binds looser than pipeline"), pipe_case_logical_or_over_pipeline(parse_expr_source(mlc::String("a || b |> c")))));
results.push_back(test_runner::assert_true(mlc::String("pipe precedence: logical and binds looser than pipeline"), pipe_case_logical_and_over_pipeline(parse_expr_source(mlc::String("a && b |> c")))));
results.push_back(test_runner::assert_true(mlc::String("pipe precedence: equality binds tighter than pipeline on right"), pipe_case_equality_right_operand_contains_pipeline(parse_expr_source(mlc::String("a == b |> c")))));
results.push_back(test_runner::assert_true(mlc::String("pipe precedence: pipeline binds tighter than equality on left"), pipe_case_pipeline_binds_tighter_than_equality_on_left(parse_expr_source(mlc::String("a |> b == c")))));
results.push_back(test_runner::assert_true(mlc::String("parse trait: DeclTrait with one method"), trait_display_decl(parse_source(mlc::String("trait Display { fn to_string(self: Display) -> string }")))));
results.push_back(test_runner::assert_eq_int(mlc::String("parse export trait: single top-level decl"), parse_source(mlc::String("export trait Display { fn to_string(self: Display) -> string }")).decls.size(), 1));
return results;
}

} // namespace test_parser
