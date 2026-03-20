#include "test_runner.hpp"

namespace test_runner {

test_runner::TestResult assert_true(mlc::String test_name, bool condition) noexcept;

test_runner::TestResult assert_eq_int(mlc::String test_name, int actual, int expected) noexcept;

test_runner::TestResult assert_eq_str(mlc::String test_name, mlc::String actual, mlc::String expected) noexcept;

mlc::String run_all(mlc::Array<test_runner::TestResult> results) noexcept;

test_runner::TestResult assert_true(mlc::String test_name, bool condition) noexcept{return condition ? test_runner::TestResult(Pass{test_name}) : test_runner::TestResult(Fail{test_name, mlc::String("expected true, got false")});}

test_runner::TestResult assert_eq_int(mlc::String test_name, int actual, int expected) noexcept{return actual == expected ? test_runner::TestResult(Pass{test_name}) : test_runner::TestResult(Fail{test_name, mlc::String("expected ") + mlc::to_string(expected) + mlc::String(", got ") + mlc::to_string(actual)});}

test_runner::TestResult assert_eq_str(mlc::String test_name, mlc::String actual, mlc::String expected) noexcept{return actual == expected ? test_runner::TestResult(Pass{test_name}) : test_runner::TestResult(Fail{test_name, mlc::String("expected '") + expected + mlc::String("', got '") + actual + mlc::String("'")});}

mlc::String run_all(mlc::Array<test_runner::TestResult> results) noexcept{
int passed = 0;
int failed = 0;
mlc::String output = mlc::String("");
int i = 0;
while (i < results.size()){
{
std::visit(overloaded{
  [&](const Pass& pass) {
auto [_w0] = pass;
{
passed = passed + 1;
}
},
  [&](const Fail& fail) {
auto [name, message] = fail;
{
failed = failed + 1;
output = output + mlc::String("FAIL: ") + name + mlc::String(" - ") + message + mlc::String("\n");
}
}
}, results[i]);
i = i + 1;
}
}
return output + mlc::to_string(passed) + mlc::String(" passed, ") + mlc::to_string(failed) + mlc::String(" failed\n");
}

} // namespace test_runner
