#include "golden_harness.hpp"

#include "ast.hpp"
#include "test_runner.hpp"
#include "path_from_root.hpp"

namespace golden_harness {

using namespace ast;
using namespace test_runner;
using namespace path_from_root;
using namespace ast_tokens;

bool golden_relative_path_is_safe(mlc::String relative_path) noexcept;

ast::Result<mlc::String, mlc::String> read_golden_fixture(mlc::String relative_path) noexcept;

test_runner::TestResult assert_golden_string(mlc::String test_name, mlc::String actual, mlc::String golden_relative_path) noexcept;

bool golden_relative_path_is_safe(mlc::String relative_path) noexcept{
return !path_from_root::test_relative_path_is_safe(relative_path) ? false : relative_path.length() < 16 ? false : relative_path.substring(0, 16) != mlc::String("fixtures/golden/") ? false : true;
}

ast::Result<mlc::String, mlc::String> read_golden_fixture(mlc::String relative_path) noexcept{
return !golden_relative_path_is_safe(relative_path) ? ast::Result<mlc::String, mlc::String>(ast::Err<mlc::String>(mlc::String("golden harness: unsafe fixture path"))) : ast::Result<mlc::String, mlc::String>([&]() -> ast::Result<mlc::String, mlc::String> { 
  mlc::String absolute_path = path_from_root::resolve_test_fixture_path(relative_path);
  return !mlc::file::exists(absolute_path) ? ast::Result<mlc::String, mlc::String>(ast::Err<mlc::String>(mlc::String("golden harness: missing fixture ") + relative_path)) : ast::Result<mlc::String, mlc::String>(ast::Ok<mlc::String>(mlc::file::read_to_string(absolute_path)));
 }());
}

test_runner::TestResult assert_golden_string(mlc::String test_name, mlc::String actual, mlc::String golden_relative_path) noexcept{return std::visit(overloaded{
  [&](const ast::Ok<mlc::String>& ok) -> test_runner::TestResult { auto [expected] = ok; return test_runner::assert_eq_str(test_name, actual, expected); },
  [&](const ast::Err<mlc::String>& err) -> test_runner::TestResult { auto [message] = err; return Fail{test_name, message}; }
}, read_golden_fixture(golden_relative_path));}

} // namespace golden_harness
