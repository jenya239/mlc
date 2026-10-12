#include "mlc/script_vm/embedding_abi.h"

#include <cstdio>
#include <cstdint>

int main() {
  (void)mlc_repl_submit;
  MlcVmConfig config{};
  config.instruction_limit = 0;
  config.heap_limit_objects = 1000000;
  config.panic_callback = nullptr;

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
  mlc_repl_destroy(session);
  return 0;
}
