// Env runtime smoke (TRACK_STDLIB_LOGIC_TO_MLC STEP=3).
// Log JSON-lines moved to MLC — see misc/examples/log_mlc_smoke.mlc.
// g++ -std=c++20 -I../include -o test_env_log test_env_log.cpp && ./test_env_log

#include "mlc/env/env_abi.hpp"

#include <cstdlib>
#include <iostream>

static int passed = 0;
static int failed = 0;

#define CHECK(expression) do { \
    if (expression) { ++passed; } \
    else { ++failed; std::cerr << "FAIL: " #expression " at line " << __LINE__ << "\n"; } \
} while (0)

void test_env_get_has_get_or() {
  setenv("MLC_ENV_SMOKE_KEY", "smoke_value", 1);
  setenv("MLC_ENV_SMOKE_EMPTY", "", 1);
  unsetenv("MLC_ENV_SMOKE_MISSING");

  CHECK(mlc::env::has(mlc::String("MLC_ENV_SMOKE_KEY")));
  CHECK(!mlc::env::has(mlc::String("MLC_ENV_SMOKE_MISSING")));

  auto present = mlc::env::get(mlc::String("MLC_ENV_SMOKE_KEY"));
  CHECK(present.has_value());
  CHECK(*present == mlc::String("smoke_value"));

  auto empty = mlc::env::get(mlc::String("MLC_ENV_SMOKE_EMPTY"));
  CHECK(empty.has_value());
  CHECK(empty->is_empty());

  auto missing = mlc::env::get(mlc::String("MLC_ENV_SMOKE_MISSING"));
  CHECK(!missing.has_value());

  CHECK(mlc::env::get_or(mlc::String("MLC_ENV_SMOKE_KEY"), mlc::String("default"))
        == mlc::String("smoke_value"));
  CHECK(mlc::env::get_or(mlc::String("MLC_ENV_SMOKE_MISSING"), mlc::String("default"))
        == mlc::String("default"));
}

int main() {
  test_env_get_has_get_or();
  std::cout << "env_log smoke: " << passed << " passed, " << failed << " failed\n";
  return failed == 0 ? 0 : 1;
}
