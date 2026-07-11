// Env + Log smoke (TRACK_STDLIB_ENV_LOGGING STEP=2).
// g++ -std=c++20 -I../include -o test_env_log test_env_log.cpp && ./test_env_log

#include "mlc/env/env_abi.hpp"
#include "mlc/log/log.hpp"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unistd.h>

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

std::string capture_stderr_log(void (*emit)()) {
  int pipe_fds[2];
  if (pipe(pipe_fds) != 0) {
    return {};
  }
  int saved_stderr = dup(STDERR_FILENO);
  dup2(pipe_fds[1], STDERR_FILENO);
  close(pipe_fds[1]);
  emit();
  fflush(stderr);
  dup2(saved_stderr, STDERR_FILENO);
  close(saved_stderr);

  std::string captured;
  char buffer[512];
  ssize_t bytes_read = 0;
  while ((bytes_read = read(pipe_fds[0], buffer, sizeof(buffer))) > 0) {
    captured.append(buffer, static_cast<size_t>(bytes_read));
  }
  close(pipe_fds[0]);
  return captured;
}

void test_log_json_lines() {
  std::string info_line = capture_stderr_log([] {
    mlc::log::info(mlc::String("hello"));
  });
  CHECK(info_line == "{\"level\":\"info\",\"msg\":\"hello\"}\n");

  std::string escaped_line = capture_stderr_log([] {
    mlc::log::warn(mlc::String("a\"b\\c\nd"));
  });
  CHECK(escaped_line == "{\"level\":\"warn\",\"msg\":\"a\\\"b\\\\c\\nd\"}\n");

  std::string error_line = capture_stderr_log([] {
    mlc::log::error(mlc::String("boom"));
  });
  CHECK(error_line.find("\"level\":\"error\"") != std::string::npos);
  CHECK(error_line.find("\"msg\":\"boom\"") != std::string::npos);

  std::string debug_line = capture_stderr_log([] {
    mlc::log::debug(mlc::String("trace"));
  });
  CHECK(debug_line.find("\"level\":\"debug\"") != std::string::npos);
}

int main() {
  test_env_get_has_get_or();
  test_log_json_lines();
  std::cout << "env_log smoke: " << passed << " passed, " << failed << " failed\n";
  return failed == 0 ? 0 : 1;
}
