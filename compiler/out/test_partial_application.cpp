#include "test_partial_application.hpp"

#include "test_runner.hpp"
#include "lexer.hpp"
#include "decls.hpp"
#include "check.hpp"
#include "codegen_test_helpers.hpp"
#include "module.hpp"

namespace test_partial_application {

using namespace test_runner;
using namespace lexer;
using namespace decls;
using namespace check;
using namespace codegen_test_helpers;
using namespace module;

int check_error_count(mlc::String source) noexcept;

mlc::Array<test_runner::TestResult> partial_application_tests() noexcept;

int check_error_count(mlc::String source) noexcept{return std::visit(overloaded{
  [&](const ast::Ok<check::CheckOut>& ok) -> int { auto [_w0] = ok; return 0; },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> int { auto [errors] = err; return errors.size(); }
}, check::check(decls::parse_program(lexer::tokenize(source).tokens)));}

mlc::Array<test_runner::TestResult> partial_application_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results.push_back(test_runner::assert_eq_int(mlc::String("partial: two-arg thunk call"), check_error_count(mlc::String("fn add(left: i32, right: i32) -> i32 = left + right\nfn main() -> i32 = do\n  let step = add(1, _)\n  step(2)\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("partial: three-arg nested"), check_error_count(mlc::String("fn triple(first: i32, second: i32, third: i32) -> i32 = first + second + third\nfn main() -> i32 = do\n  let one = triple(1, _, _)\n  let two = one(2, _)\n  two(3)\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("partial: pipe into partially applied callee"), check_error_count(mlc::String("fn add(left: i32, right: i32) -> i32 = left + right\nfn main() -> i32 = do\n  let bound = add(10, _)\n  3 |> bound\nend")), 0));
results.push_back(test_runner::assert_eq_int(mlc::String("partial: match arm with partial call"), check_error_count(mlc::String("fn combine(left: i32, right: i32) -> i32 = left + right\nfn main() -> i32 = do\n  let value = 1\n  match value {\n    1 => combine(10, _)(2),\n    _ => 0\n  }\nend")), 0));
results.push_back(test_runner::assert_true(mlc::String("partial: bare underscore is error"), check_error_count(mlc::String("fn main() -> i32 = _")) > 0));
results.push_back(codegen_test_helpers::assert_code_contains(mlc::String("partial: codegen emits lambda for bound call"), module::gen_program(decls::parse_program(lexer::tokenize(mlc::String("fn add(left: i32, right: i32) -> i32 = left + right\nfn main() -> i32 = do\n  let bound = add(1, _)\n  bound(2)\nend")).tokens)), mlc::String("[&]")));
return results;
}

} // namespace test_partial_application
