// TRACK_MLC_SCRIPT_VM §103i — host C++ links embedding ABI (call add1 → 42).
#include "mlc/script_vm/embedding_abi.h"
#include "mlc/script_vm/value_rep_abi.hpp"

#include <cstdio>
#include <cstdint>
#include <vector>

int main() {
  std::vector<std::uint8_t> blob(4096);
  const size_t needed = mlc_script_vm_fixture_add1(blob.data(), blob.size());
  if (needed == 0 || needed > blob.size()) {
    std::fprintf(stderr, "[embedding_abi_host] FAIL: fixture size=%zu\n", needed);
    return 2;
  }
  blob.resize(needed);

  MlcVmConfig config{};
  config.instruction_limit = 0;
  config.heap_limit_objects = 1000000;
  config.panic_callback = nullptr;

  MlcVm* vm = mlc_vm_create(&config);
  if (vm == nullptr) {
    std::fprintf(stderr, "[embedding_abi_host] FAIL: create\n");
    return 3;
  }

  const MlcResult loaded = mlc_vm_load_module(vm, "demo", blob.data(), blob.size());
  if (loaded.code != 0) {
    std::fprintf(stderr, "[embedding_abi_host] FAIL: load code=%d\n", loaded.code);
    mlc_vm_destroy(vm);
    return 4;
  }

  const MlcHandle handle = mlc_vm_get_export(vm, "demo", "add1");
  if (handle == 0) {
    std::fprintf(stderr, "[embedding_abi_host] FAIL: get_export\n");
    mlc_vm_destroy(vm);
    return 5;
  }

  MlcValue arg{};
  arg.raw = mlc::script_vm::encode_int32(41);
  MlcValue result{};
  const MlcResult called = mlc_vm_call(vm, handle, &arg, 1, &result);
  if (called.code != 0) {
    std::fprintf(stderr, "[embedding_abi_host] FAIL: call code=%d\n", called.code);
    mlc_handle_release(vm, handle);
    mlc_vm_destroy(vm);
    return 6;
  }
  if (mlc::script_vm::decode_kind(result.raw) != 3 ||
      mlc::script_vm::decode_int32(result.raw) != 42) {
    std::fprintf(stderr, "[embedding_abi_host] FAIL: result raw=%lld\n",
                 static_cast<long long>(result.raw));
    mlc_handle_release(vm, handle);
    mlc_vm_destroy(vm);
    return 7;
  }

  mlc_handle_release(vm, handle);
  mlc_vm_destroy(vm);

  std::printf("host_call=ok\n");
  std::printf("add1=ok\n");
  std::printf("[script_vm] embedding_abi_host ok\n");
  return 0;
}
