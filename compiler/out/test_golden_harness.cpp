#include "test_golden_harness.hpp"

#include "test_runner.hpp"
#include "golden_harness.hpp"
#include "path_from_root.hpp"

namespace test_golden_harness {

using namespace test_runner;
using namespace golden_harness;
using namespace path_from_root;

mlc::Array<test_runner::TestResult> golden_harness_tests() noexcept;

mlc::Array<test_runner::TestResult> golden_harness_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results.push_back(test_runner::assert_true(mlc::String("test_relative_path_is_safe accepts fixtures/golden/sample_expected.txt"), path_from_root::test_relative_path_is_safe(mlc::String("fixtures/golden/sample_expected.txt"))));
results.push_back(test_runner::assert_true(mlc::String("golden_relative_path_is_safe rejects parent segment"), !golden_harness::golden_relative_path_is_safe(mlc::String("fixtures/golden/../evil.txt"))));
results.push_back(test_runner::assert_true(mlc::String("golden_relative_path_is_safe rejects outside golden prefix"), !golden_harness::golden_relative_path_is_safe(mlc::String("fixtures/cpp_parser/sample_header.h"))));
ast::Result<mlc::String, mlc::String> read_result = golden_harness::read_golden_fixture(mlc::String("fixtures/golden/sample_expected.txt"));
results.push_back(test_runner::assert_true(mlc::String("read_golden_fixture loads sample"), std::visit(overloaded{
  [&](const ast::Ok<mlc::String>& ok) -> bool { auto [content] = ok; return content.contains(mlc::String("hello golden")); },
  [&](const ast::Err<mlc::String>& err) -> bool { auto [_w0] = err; return false; }
}, read_result)));
ast::Result<mlc::String, mlc::String> unsafe_read = golden_harness::read_golden_fixture(mlc::String("fixtures/golden/../../main.mlc"));
results.push_back(test_runner::assert_true(mlc::String("read_golden_fixture rejects unsafe path"), std::visit(overloaded{
  [&](const ast::Err<mlc::String>& err) -> bool { auto [_w0] = err; return true; },
  [&](const ast::Ok<mlc::String>& ok) -> bool { auto [_w0] = ok; return false; }
}, unsafe_read)));
mlc::String sample_content = std::visit(overloaded{
  [&](const ast::Ok<mlc::String>& ok) -> mlc::String { auto [content] = ok; return content; },
  [&](const ast::Err<mlc::String>& err) -> mlc::String { auto [_w0] = err; return mlc::String(""); }
}, read_result);
results.push_back(golden_harness::assert_golden_string(mlc::String("assert_golden_string sample"), sample_content, mlc::String("fixtures/golden/sample_expected.txt")));
return results;
}

} // namespace test_golden_harness
