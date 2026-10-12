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
  mlc_repl_destroy(session);
  return 0;
}
