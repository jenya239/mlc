#include "test_checker.hpp"

#include "test_runner.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "infer.hpp"

namespace test_checker {

using namespace test_runner;
using namespace lexer;
using namespace decls;
using namespace infer;

int check_error_count(mlc::String source) noexcept;

mlc::Array<test_runner::TestResult> checker_tests() noexcept;

int check_error_count(mlc::String source) noexcept{return std::visit(overloaded{
  [&](const ast::Ok<infer::CheckOut>& ok) -> int { auto [_w0] = ok; return 0; },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> int { auto [errs] = err; return errs.size(); }
}, infer::check(decls::parse_program(lexer::tokenize(source).tokens)));}

mlc::Array<test_runner::TestResult> checker_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results.push_back(test_runner::assert_eq_int(mlc::String("empty program - 0 errors"), check_error_count(mlc::String("")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("fn returning literal - 0 errors"), check_error_count(mlc::String("fn f() -> i32 = 42")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("fn using its own param - 0 errors"), check_error_count(mlc::String("fn f(x: i32) -> i32 = x")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("fn calling another fn - 0 errors"), check_error_count(mlc::String("fn add(x: i32, y: i32) -> i32 = x + y\nfn main() -> i32 = add(1, 2)")), 0));
results.push_back(test_runner::assert_true(mlc::String("undefined name - at least 1 error"), check_error_count(mlc::String("fn f() -> i32 = undefined_name")) > 0));
results.push_back(test_runner::assert_eq_int(mlc::String("type decl with variants - 0 errors"), check_error_count(mlc::String("type Color = Red | Green | Blue")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("fn returning bool - 0 errors"), check_error_count(mlc::String("fn f() -> bool = true")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("fn with const binding - 0 errors"), check_error_count(mlc::String("fn f() -> i32 = do\n  const x = 1\n  x\nend")), 0));
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
return results;
}

} // namespace test_checker
