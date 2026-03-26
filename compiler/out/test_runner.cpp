#include "test_runner.hpp"

namespace test_runner {

test_runner::TestResult assert_true(mlc::String test_name, bool condition) noexcept;

test_runner::TestResult assert_eq_int(mlc::String test_name, int actual, int expected) noexcept;

test_runner::TestResult assert_eq_str(mlc::String test_name, mlc::String actual, mlc::String expected) noexcept;

test_runner::TestResult assert_diagnostic_at(mlc::String test_name, mlc::String formatted_diagnostic, int expected_line, int expected_column, mlc::String message_needle) noexcept;

mlc::String run_all(mlc::Array<test_runner::TestResult> results) noexcept;

test_runner::TestResult assert_true(mlc::String test_name, bool condition) noexcept{return condition ? test_runner::TestResult(Pass{test_name}) : test_runner::TestResult(Fail{test_name, mlc::String("expected true, got false")});}

test_runner::TestResult assert_eq_int(mlc::String test_name, int actual, int expected) noexcept{return actual == expected ? test_runner::TestResult(Pass{test_name}) : test_runner::TestResult(Fail{test_name, mlc::String("expected ") + mlc::to_string(expected) + mlc::String(", got ") + mlc::to_string(actual)});}

test_runner::TestResult assert_eq_str(mlc::String test_name, mlc::String actual, mlc::String expected) noexcept{return actual == expected ? test_runner::TestResult(Pass{test_name}) : test_runner::TestResult(Fail{test_name, mlc::String("expected '") + expected + mlc::String("', got '") + actual + mlc::String("'")});}

test_runner::TestResult assert_diagnostic_at(mlc::String test_name, mlc::String formatted_diagnostic, int expected_line, int expected_column, mlc::String message_needle) noexcept{
mlc::String position_suffix = mlc::String(" at ") + mlc::to_string(expected_line) + mlc::String(":") + mlc::to_string(expected_column);
bool has_message = formatted_diagnostic.contains(message_needle);
bool has_position = formatted_diagnostic.length() >= position_suffix.length() && formatted_diagnostic.substring(formatted_diagnostic.length() - position_suffix.length(), position_suffix.length()) == position_suffix;
return has_message && has_position ? test_runner::TestResult(Pass{test_name}) : test_runner::TestResult(Fail{test_name, mlc::String("expected needle '") + message_needle + mlc::String("' and suffix '") + position_suffix + mlc::String("', got '") + formatted_diagnostic + mlc::String("'")});
}

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
