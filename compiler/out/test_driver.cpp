#include "test_driver.hpp"

#include "test_runner.hpp"
#include "path_normalize.hpp"
#include "compile_driver.hpp"

namespace test_driver {

using namespace test_runner;
using namespace path_normalize;
using namespace compile_driver;

mlc::Array<test_runner::TestResult> driver_tests() noexcept;

mlc::Array<test_runner::TestResult> driver_tests() noexcept{
mlc::Array<test_runner::TestResult> results = {};
results.push_back(test_runner::assert_true(mlc::String("driver_source_path_is_safe accepts entry.mlc"), path_normalize::driver_source_path_is_safe(mlc::String("entry.mlc"))));
results.push_back(test_runner::assert_true(mlc::String("driver_source_path_is_safe rejects parent segment"), !path_normalize::driver_source_path_is_safe(mlc::String("../entry.mlc"))));
results.push_back(test_runner::assert_true(mlc::String("driver_source_path_is_safe rejects semicolon"), !path_normalize::driver_source_path_is_safe(mlc::String("foo;bar.mlc"))));
ast::Result<mlc::String, mlc::Array<mlc::String>> unsafe_compile = compile_driver::compile_modular(mlc::String("../bad.mlc"), mlc::String(""), false, true, false, false, false, false, false);
results.push_back(test_runner::assert_true(mlc::String("compile_modular rejects unsafe entry path"), std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> bool { auto [_w0] = err; return true; },
  [&](const ast::Ok<mlc::String>& ok) -> bool { auto [_w0] = ok; return false; }
}, unsafe_compile)));
return results;
}

} // namespace test_driver
