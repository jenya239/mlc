// Validate smoke (TRACK_STDLIB_VALIDATION STEP=2).
// g++ -std=c++20 -I../include -o test_validate test_validate.cpp && ./test_validate

#include "mlc/validate/validate.hpp"

#include <iostream>
#include <variant>

static int passed = 0;
static int failed = 0;

#define CHECK(expression) do { \
    if (expression) { ++passed; } \
    else { ++failed; std::cerr << "FAIL: " #expression " at line " << __LINE__ << "\n"; } \
} while (0)

static bool is_ok(const mlc::validate::ValidateResult& result) {
  return std::holds_alternative<mlc::result::Ok<mlc::validate::Unit>>(result);
}

static bool is_err(const mlc::validate::ValidateResult& result) {
  return std::holds_alternative<mlc::result::Err<mlc::String>>(result);
}

static mlc::String err_message(const mlc::validate::ValidateResult& result) {
  return std::get<mlc::result::Err<mlc::String>>(result)._0;
}

void test_non_empty() {
  CHECK(is_ok(mlc::validate::non_empty(mlc::String("x"))));
  CHECK(is_err(mlc::validate::non_empty(mlc::String(""))));
  CHECK(err_message(mlc::validate::non_empty(mlc::String(""))) == mlc::String("must be non-empty"));
}

void test_min_max_length() {
  CHECK(is_ok(mlc::validate::min_length(mlc::String("ab"), 2)));
  CHECK(is_err(mlc::validate::min_length(mlc::String("a"), 2)));
  CHECK(is_ok(mlc::validate::max_length(mlc::String("ab"), 2)));
  CHECK(is_err(mlc::validate::max_length(mlc::String("abc"), 2)));
}

void test_range_i32() {
  CHECK(is_ok(mlc::validate::range_i32(5, 1, 10)));
  CHECK(is_ok(mlc::validate::range_i32(1, 1, 10)));
  CHECK(is_ok(mlc::validate::range_i32(10, 1, 10)));
  CHECK(is_err(mlc::validate::range_i32(0, 1, 10)));
  CHECK(is_err(mlc::validate::range_i32(11, 1, 10)));
}

int main() {
  test_non_empty();
  test_min_max_length();
  test_range_i32();
  std::cout << "validate smoke: " << passed << " passed, " << failed << " failed\n";
  return failed == 0 ? 0 : 1;
}
