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

infer::CheckOut check_source(mlc::String source) noexcept;

int check_error_count(mlc::String source) noexcept;

mlc::Array<test_runner::TestResult> checker_tests() noexcept;

infer::CheckOut check_source(mlc::String source) noexcept{return infer::check(decls::parse_program(lexer::tokenize(source).tokens));}

int check_error_count(mlc::String source) noexcept{return check_source(source).errors.size();}

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
return results;
}

} // namespace test_checker
