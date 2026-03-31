#include "test_checker.hpp"

#include "test_runner.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "check.hpp"

namespace test_checker {

using namespace test_runner;
using namespace lexer;
using namespace decls;
using namespace check;

int check_error_count(mlc::String source) noexcept;

int check_with_context_error_count(mlc::String entry_source, mlc::String full_source) noexcept;

mlc::String first_checker_error_line(mlc::String source) noexcept;

mlc::String first_checker_error_line_with_path(mlc::String source, mlc::String source_path) noexcept;

mlc::Array<test_runner::TestResult> checker_tests() noexcept;

int check_error_count(mlc::String source) noexcept{return std::visit(overloaded{
  [&](const ast::Ok<check::CheckOut>& ok) -> int { auto [_w0] = ok; return 0; },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> int { auto [errs] = err; return errs.size(); }
}, check::check(decls::parse_program(lexer::tokenize(source).tokens)));}

int check_with_context_error_count(mlc::String entry_source, mlc::String full_source) noexcept{return std::visit(overloaded{
  [&](const ast::Ok<check::CheckOut>& ok) -> int { auto [_w0] = ok; return 0; },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> int { auto [error_list] = err; return error_list.size(); }
}, check::check_with_context(decls::parse_program(lexer::tokenize(entry_source).tokens), decls::parse_program(lexer::tokenize(full_source).tokens)));}

mlc::String first_checker_error_line(mlc::String source) noexcept{return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> mlc::String { auto [error_lines] = err; return error_lines.size() > 0 ? error_lines[0] : mlc::String(""); },
  [&](const ast::Ok<check::CheckOut>& ok) -> mlc::String { auto [_w0] = ok; return mlc::String(""); }
}, check::check(decls::parse_program(lexer::tokenize(source).tokens)));}

mlc::String first_checker_error_line_with_path(mlc::String source, mlc::String source_path) noexcept{return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> mlc::String { auto [error_lines] = err; return error_lines.size() > 0 ? error_lines[0] : mlc::String(""); },
  [&](const ast::Ok<check::CheckOut>& ok) -> mlc::String { auto [_w0] = ok; return mlc::String(""); }
}, check::check(decls::parse_program_with_source_path(lexer::tokenize(source).tokens, source_path)));}

mlc::Array<test_runner::TestResult> checker_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results.push_back(test_runner::assert_eq_int(mlc::String("empty program - 0 errors"), check_error_count(mlc::String("")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("fn returning literal - 0 errors"), check_error_count(mlc::String("fn f() -> i32 = 42")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("fn using its own param - 0 errors"), check_error_count(mlc::String("fn f(x: i32) -> i32 = x")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("fn calling another fn - 0 errors"), check_error_count(mlc::String("fn add(x: i32, y: i32) -> i32 = x + y\nfn main() -> i32 = add(1, 2)")), 0));
results.push_back(test_runner::assert_true(mlc::String("undefined name - at least 1 error"), check_error_count(mlc::String("fn f() -> i32 = undefined_name")) > 0));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("undefined ident: line and column (single line body)"), first_checker_error_line(mlc::String("fn f() -> i32 = badident")), 1, 17, mlc::String("undefined: badident")));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("undefined ident: line and column (next line, indented)"), first_checker_error_line(mlc::String("fn f() -> i32 =\n  badident")), 2, 3, mlc::String("undefined: badident")));
results.push_back(test_runner::assert_eq_str(mlc::String("undefined ident: diagnostic includes source path"), first_checker_error_line_with_path(mlc::String("fn f() -> i32 = badident"), mlc::String("unit.mlc")), mlc::String("error: undefined: badident at unit.mlc:1:17")));
results.push_back(test_runner::assert_eq_int(mlc::String("type decl with variants - 0 errors"), check_error_count(mlc::String("type Color = Red | Green | Blue")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("fn returning bool - 0 errors"), check_error_count(mlc::String("fn f() -> bool = true")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("fn with const binding - 0 errors"), check_error_count(mlc::String("fn f() -> i32 = do\n  const x = 1\n  x\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("let const compile-time binding - 0 errors"), check_error_count(mlc::String("fn f() -> i32 = do\n  let const c = 42\n  c\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("nested generic in array type annotation - 0 errors"), check_error_count(mlc::String("type Result<T, E> = Ok(T) | Err(E)\nfn f() -> i32 = do\n  let items: [Result<i32, string>] = []\n  0\nend")), 0));
results.push_back(test_runner::assert_true(mlc::String("two undefined names - at least 1 error"), check_error_count(mlc::String("fn f() -> i32 = foo + bar")) > 0));
results.push_back(test_runner::assert_eq_int(mlc::String("builtin print - 0 errors"), check_error_count(mlc::String("fn f() -> i32 = do\n  print(\"hi\")\n  0\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("fn with type param - 0 errors"), check_error_count(mlc::String("fn id<T>(x: T) -> T = x")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("fn with type param and bound - 0 errors"), check_error_count(mlc::String("type Display { fn to_string(self: Self) -> string }\nfn id<T: Display>(x: T) -> T = x")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("call generic fn - 0 errors"), check_error_count(mlc::String("fn id<T>(x: T) -> T = x\nfn f() -> i32 = id(42)")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("trait decl + extend impl - 0 errors"), check_error_count(mlc::String("type Display { fn to_string(self: Self) -> string }\ntype Color = Red | Green\nextend Color : Display { fn to_string(self: Color) -> string = \"Red\" }")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("method call on param - 0 errors"), check_error_count(mlc::String("fn f(c: string) -> string = c.to_string()")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("lambda expr - 0 errors"), check_error_count(mlc::String("fn f() -> i32 = do\n  const g = x => x + 1\n  g(41)\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("Ok/Err/Result as known globals - 0 errors"), check_error_count(mlc::String("fn f() -> i32 = do\n  const r = Ok(42)\n  0\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("Err constructor - 0 errors"), check_error_count(mlc::String("fn f() -> i32 = do\n  const e = Err(\"oops\")\n  0\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("Result return type annotation - 0 errors"), check_error_count(mlc::String("type Result<T, E> = Ok(T) | Err(E)\nfn f() -> Result<i32, string> = Ok(42)")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("block: let then use binding - 0 errors"), check_error_count(mlc::String("fn f() -> i32 = do\n  let x = 10\n  x + 1\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("for-in: loop variable visible in body - 0 errors"), check_error_count(mlc::String("fn sum(items: [i32]) -> i32 = do\n  let mut total = 0\n  for element in items do\n    total = total + element\n  end\n  total\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("check_with_context: entry sees helper from full program - 0 errors"), check_with_context_error_count(mlc::String("fn main() -> i32 = helper()"), mlc::String("fn helper() -> i32 = 1\nfn main() -> i32 = helper()")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("match: constructor arm binds payload - 0 errors"), check_error_count(mlc::String("type Answer = Yes(i32) | No\nfn value(answer: Answer) -> i32 = match answer | Yes(n) => n | No => 0\n")), 0));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("undefined record field: line and column on field access"), first_checker_error_line(mlc::String("type Point = { x: i32, y: i32 }\nfn f(p: Point) -> i32 = p.z")), 2, 26, mlc::String("undefined field: z on Point")));
results.push_back(test_runner::assert_eq_str(mlc::String("undefined record field: message text"), first_checker_error_line(mlc::String("type R = { a: i32 }\nfn f(x: R) -> i32 = x.missing")), mlc::String("error: undefined field: missing on R at 2:22")));
results.push_back(test_runner::assert_true(mlc::String("infer: string + i32 is an error"), check_error_count(mlc::String("fn f() -> i32 = \"a\" + 1")) > 0));
results.push_back(test_runner::assert_true(mlc::String("infer: i32 + string is an error"), check_error_count(mlc::String("fn f() -> i32 = 1 + \"a\"")) > 0));
results.push_back(test_runner::assert_true(mlc::String("infer: string with - is an error"), check_error_count(mlc::String("fn f() -> string = \"a\" - \"b\"")) > 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: string + string still ok"), check_error_count(mlc::String("fn f() -> string = \"a\" + \"b\"")), 0));
results.push_back(test_runner::assert_true(mlc::String("infer: ! on i32 is an error"), check_error_count(mlc::String("fn f() -> bool = !42")) > 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: ! on bool ok"), check_error_count(mlc::String("fn f() -> bool = !false")), 0));
results.push_back(test_runner::assert_true(mlc::String("infer: i32 == bool is an error"), check_error_count(mlc::String("fn f() -> bool = 1 == true")) > 0));
results.push_back(test_runner::assert_true(mlc::String("infer: i32 < string is an error"), check_error_count(mlc::String("fn f() -> bool = 1 < \"a\"")) > 0));
results.push_back(test_runner::assert_true(mlc::String("infer: bool && i32 is an error"), check_error_count(mlc::String("fn f() -> bool = true && 1")) > 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: i32 ordering ok"), check_error_count(mlc::String("fn f() -> bool = 1 < 2")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: string ordering ok"), check_error_count(mlc::String("fn f() -> bool = \"a\" <= \"b\"")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: bool && ok"), check_error_count(mlc::String("fn f() -> bool = true && false")), 0));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: unary minus on string"), first_checker_error_line(mlc::String("fn f() -> i32 = -\"x\"")), 1, 17, mlc::String("unary - expects i32")));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: to_i on non-string receiver"), first_checker_error_line(mlc::String("fn f() -> i32 = 1.to_i()")), 1, 18, mlc::String("method to_i expects a string receiver")));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: join on non-array receiver"), first_checker_error_line(mlc::String("fn f() -> string = \"a\".join(\",\")")), 1, 23, mlc::String("method join expects an array receiver")));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: push on string receiver"), first_checker_error_line(mlc::String("fn f() -> i32 = \"a\".push(1)")), 1, 20, mlc::String("method push expects an array receiver")));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: set on string receiver"), first_checker_error_line(mlc::String("fn f() -> i32 = \"a\".set(0, 1)")), 1, 20, mlc::String("method set expects an array or Map receiver")));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: get on non-Map receiver"), first_checker_error_line(mlc::String("fn f() -> i32 = 1.get(\"k\")")), 1, 18, mlc::String("method get expects a Map receiver")));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: get and set on Map ok"), check_error_count(mlc::String("fn f(m: Map<string, i32>) -> i32 = do\n  m.set(\"k\", 1)\n  m.get(\"k\")\nend")), 0));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: builtin map get wrong arity"), first_checker_error_line(mlc::String("fn f(m: Map<string, i32>) -> i32 = m.get()")), 1, 37, mlc::String("expected 1 arguments, got 0")));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: builtin map set wrong arity"), first_checker_error_line(mlc::String("fn f(m: Map<string, i32>) -> i32 = m.set(\"k\")")), 1, 37, mlc::String("expected 2 arguments, got 1")));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: builtin array push wrong arity"), first_checker_error_line(mlc::String("fn f(a: [i32]) -> i32 = a.push()")), 1, 26, mlc::String("expected 1 arguments, got 0")));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: has on non-Map receiver (i32)"), first_checker_error_line(mlc::String("fn f() -> bool = 1.has(\"k\")")), 1, 19, mlc::String("method has expects a Map receiver")));
results.push_back(test_runner::assert_diagnostic_at(mlc::String("infer: has on sum type is an error"), first_checker_error_line(mlc::String("type Hue = Red | Green\nfn f(x: Hue) -> bool = x.has(\"k\")")), 2, 25, mlc::String("method has expects a Map receiver")));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: has on Map ok"), check_error_count(mlc::String("fn f(m: Map<string, i32>) -> bool = m.has(\"k\")")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("infer: ref mut Map uses inner type for builtins"), check_error_count(mlc::String("fn f(cache: ref mut Map<string, i32>) -> bool = cache.has(\"k\")")), 0));
return results;
}

} // namespace test_checker
