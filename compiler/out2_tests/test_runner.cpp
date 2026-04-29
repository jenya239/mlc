#define main mlc_user_main
#include "test_runner.hpp"

namespace test_runner {

TestResult assert_true(mlc::String test_name, bool condition) noexcept{
if (condition) {
return Pass{test_name};
} else {
return Fail{test_name, mlc::String("expected true, got false", 24)};
}
}
TestResult assert_eq_int(mlc::String test_name, int actual, int expected) noexcept{
if ((actual == expected)) {
return Pass{test_name};
} else {
return Fail{test_name, (((mlc::String("expected ", 9) + mlc::to_string(expected)) + mlc::String(", got ", 6)) + mlc::to_string(actual))};
}
}
TestResult assert_eq_str(mlc::String test_name, mlc::String actual, mlc::String expected) noexcept{
if ((actual == expected)) {
return Pass{test_name};
} else {
return Fail{test_name, ((((mlc::String("expected '", 10) + expected) + mlc::String("', got '", 8)) + actual) + mlc::String("'", 1))};
}
}
TestResult assert_diagnostic_at(mlc::String test_name, mlc::String formatted_diagnostic, int expected_line, int expected_column, mlc::String message_needle) noexcept{
auto position_part = (((mlc::to_string(expected_line) + mlc::String(":", 1)) + mlc::to_string(expected_column)) + mlc::String(": ", 2));
auto has_message = formatted_diagnostic.contains(message_needle);
auto has_position = formatted_diagnostic.contains(position_part);
if ((has_message && has_position)) {
return Pass{test_name};
} else {
return Fail{test_name, ((((((mlc::String("expected needle '", 17) + message_needle) + mlc::String("' and position '", 16)) + position_part) + mlc::String("', got '", 8)) + formatted_diagnostic) + mlc::String("'", 1))};
}
}
mlc::String run_all(mlc::Array<TestResult> results) noexcept{
auto passed = 0;
auto failed = 0;
auto output = mlc::String("", 0);
auto i = 0;
while ((i < results.length())) {
std::visit(overloaded{[&](const Pass& pass) { auto [__0] = pass; return [&]() {
passed = (passed + 1);
return /* unit */;
}(); },
[&](const Fail& fail) { auto [name, message] = fail; return [&]() {
failed = (failed + 1);
output = (((((output + mlc::String("FAIL: ", 6)) + name) + mlc::String(" - ", 3)) + message) + mlc::String("\n", 1));
return /* unit */;
}(); }
}, results[i]);
i = (i + 1);
}
return ((((output + mlc::to_string(passed)) + mlc::String(" passed, ", 9)) + mlc::to_string(failed)) + mlc::String(" failed\n", 8));
}

} // namespace test_runner
