#include "mlc/script_vm/embedding_abi.h"

#include <cstdio>
#include <cstdint>
#include <cstring>

bool panic_saw_double_release = false;

extern "C" void record_panic(const char* message) {
  if (message != nullptr && std::strstr(message, "double release") != nullptr) {
    panic_saw_double_release = true;
    std::fprintf(stderr, "panic %s\n", message);
  }
}

bool code_starts_with_unknown_identifier(const char* code_text) {
  const char prefix[] = "unknown_identifier";
  return std::strncmp(code_text, prefix, sizeof(prefix) - 1) == 0;
}

int main() {
  (void)mlc_repl_submit;
  MlcVmConfig config{};
  config.instruction_limit = 0;
  config.heap_limit_objects = 1000000;
  config.panic_callback = record_panic;

  MlcRepl* session = mlc_repl_create(&config);
  const char text[] = "1 + 2";
  MlcReplAnswer answer{};
  const int submitted = mlc_repl_submit(session, text, sizeof(text) - 1, &answer);
  if (submitted != 0 || answer.status != MLC_REPL_VALUE || answer.value != 3) {
    std::fprintf(stderr, "expression mismatch status=%d value=%d\n", answer.status, answer.value);
    mlc_repl_destroy(session);
    return 1;
  }
  std::printf("case expression ok\n");

  const char identity_declaration[] = "export fn identity(number: i32) -> i32 = number";
  MlcReplAnswer accepted{};
  const int declared = mlc_repl_submit(
      session, identity_declaration, sizeof(identity_declaration) - 1, &accepted);
  if (declared != 0 || accepted.status != MLC_REPL_ACCEPTED) {
    std::fprintf(stderr, "case accumulate status=%d value=%d\n", accepted.status, accepted.value);
    mlc_repl_destroy(session);
    return 1;
  }
  const char identity_call[] = "identity(4)";
  MlcReplAnswer called{};
  const int invoked = mlc_repl_submit(session, identity_call, sizeof(identity_call) - 1, &called);
  if (invoked != 0 || called.status != MLC_REPL_VALUE || called.value != 4) {
    std::fprintf(stderr, "case accumulate status=%d value=%d\n", called.status, called.value);
    mlc_repl_destroy(session);
    return 1;
  }
  std::printf("case accumulate ok\n");

  const char missing_name[] = "missing_name";
  MlcReplAnswer diagnostic{};
  const int diagnosed = mlc_repl_submit(session, missing_name, sizeof(missing_name) - 1, &diagnostic);
  std::printf(
      "case diagnostic status=%d code=%s line=%d column=%d\n",
      diagnostic.status,
      diagnostic.code_text,
      diagnostic.line,
      diagnostic.column);
  if (diagnosed != 0 || diagnostic.status != MLC_REPL_DIAGNOSTIC ||
      diagnostic.line <= 0 || diagnostic.column <= 0 ||
      !code_starts_with_unknown_identifier(diagnostic.code_text)) {
    mlc_repl_destroy(session);
    return 1;
  }
  std::printf("case diagnostic ok\n");

  const int null_submitted = mlc_repl_submit(nullptr, text, sizeof(text) - 1, &answer);
  mlc_repl_destroy(session);
  mlc_repl_destroy(session);
  if (null_submitted != -1 || !panic_saw_double_release) {
    std::fprintf(stderr, "case misuse submit=%d panic=%d\n", null_submitted, panic_saw_double_release ? 1 : 0);
    return 1;
  }
  std::printf("case misuse ok\n");
  return 0;
}
