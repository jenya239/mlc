#include "mlc/script_vm/embedding_abi.h"
#include "mlc/script_vm/value_rep_abi.hpp"

#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

int panic_invocation_count = 0;
std::string panic_message;

extern "C" void record_panic(const char* message) {
  panic_invocation_count = panic_invocation_count + 1;
  if (message == nullptr) {
    panic_message = "";
  } else {
    panic_message = message;
  }
}

void reset_panic() {
  panic_invocation_count = 0;
  panic_message.clear();
}

std::vector<std::uint8_t> add1_module_bytes() {
  std::vector<std::uint8_t> module_bytes(4096);
  const size_t needed = mlc_script_vm_fixture_add1(module_bytes.data(), module_bytes.size());
  if (needed == 0 || needed > module_bytes.size()) {
    module_bytes.clear();
    return module_bytes;
  }
  module_bytes.resize(needed);
  return module_bytes;
}

MlcVmConfig standard_config() {
  MlcVmConfig config{};
  config.instruction_limit = 0;
  config.heap_limit_objects = 1000000;
  config.panic_callback = record_panic;
  return config;
}

int fail_case(const char* case_name, const char* detail) {
  std::printf("case %s %s\n", case_name, detail);
  return 1;
}

int load_demo(MlcVm* virtual_machine, const std::vector<std::uint8_t>& module_bytes, const char* case_name) {
  if (module_bytes.empty()) {
    return fail_case(case_name, "empty module");
  }
  const MlcResult loaded = mlc_vm_load_module(
      virtual_machine, "demo", module_bytes.data(), module_bytes.size());
  if (loaded.code != MLC_OK) {
    std::printf("case %s load code=%d\n", case_name, loaded.code);
    return 1;
  }
  return 0;
}

int case_missing_export(const std::vector<std::uint8_t>& module_bytes) {
  const char* case_name = "get_export_missing_export";
  MlcVmConfig config = standard_config();
  MlcVm* virtual_machine = mlc_vm_create(&config);
  if (load_demo(virtual_machine, module_bytes, case_name) != 0) {
    mlc_vm_destroy(virtual_machine);
    return 1;
  }
  const MlcHandle handle = mlc_vm_get_export(virtual_machine, "demo", "missing");
  mlc_vm_destroy(virtual_machine);
  if (handle != 0) {
    std::printf("case %s got=%llu expected=0\n", case_name, static_cast<unsigned long long>(handle));
    return 1;
  }
  std::printf("case %s ok\n", case_name);
  return 0;
}

int case_unknown_module() {
  const char* case_name = "get_export_unknown_module";
  MlcVmConfig config = standard_config();
  MlcVm* virtual_machine = mlc_vm_create(&config);
  const MlcHandle handle = mlc_vm_get_export(virtual_machine, "unknown", "add1");
  mlc_vm_destroy(virtual_machine);
  if (handle != 0) {
    std::printf("case %s got=%llu expected=0\n", case_name, static_cast<unsigned long long>(handle));
    return 1;
  }
  std::printf("case %s ok\n", case_name);
  return 0;
}

int case_zero_handle() {
  const char* case_name = "call_zero_handle";
  MlcVmConfig config = standard_config();
  MlcVm* virtual_machine = mlc_vm_create(&config);
  MlcValue result_value{};
  const MlcResult called = mlc_vm_call(virtual_machine, 0, nullptr, 0, &result_value);
  mlc_vm_destroy(virtual_machine);
  if (called.code != MLC_BAD_HANDLE) {
    std::printf("case %s got=%d expected=%d\n", case_name, called.code, MLC_BAD_HANDLE);
    return 1;
  }
  std::printf("case %s ok\n", case_name);
  return 0;
}

int case_call_after_release(const std::vector<std::uint8_t>& module_bytes) {
  const char* case_name = "call_after_release";
  MlcVmConfig config = standard_config();
  MlcVm* virtual_machine = mlc_vm_create(&config);
  if (load_demo(virtual_machine, module_bytes, case_name) != 0) {
    mlc_vm_destroy(virtual_machine);
    return 1;
  }
  const MlcHandle handle = mlc_vm_get_export(virtual_machine, "demo", "add1");
  if (handle == 0) {
    mlc_vm_destroy(virtual_machine);
    return fail_case(case_name, "export missing");
  }
  mlc_handle_release(virtual_machine, handle);
  MlcValue argument{};
  argument.raw = mlc::script_vm::encode_int32(41);
  MlcValue result_value{};
  const MlcResult called = mlc_vm_call(virtual_machine, handle, &argument, 1, &result_value);
  mlc_vm_destroy(virtual_machine);
  if (called.code != MLC_BAD_HANDLE) {
    std::printf("case %s got=%d expected=%d\n", case_name, called.code, MLC_BAD_HANDLE);
    return 1;
  }
  std::printf("case %s ok\n", case_name);
  return 0;
}

int case_double_release(const std::vector<std::uint8_t>& module_bytes) {
  const char* case_name = "double_release_panics";
  reset_panic();
  MlcVmConfig config = standard_config();
  MlcVm* virtual_machine = mlc_vm_create(&config);
  if (load_demo(virtual_machine, module_bytes, case_name) != 0) {
    mlc_vm_destroy(virtual_machine);
    return 1;
  }
  const MlcHandle handle = mlc_vm_get_export(virtual_machine, "demo", "add1");
  if (handle == 0) {
    mlc_vm_destroy(virtual_machine);
    return fail_case(case_name, "export missing");
  }
  mlc_handle_release(virtual_machine, handle);
  mlc_handle_release(virtual_machine, handle);
  mlc_vm_destroy(virtual_machine);
  if (panic_invocation_count != 1 || panic_message.find("double release") == std::string::npos) {
    std::printf("case %s panics=%d message=%s\n", case_name, panic_invocation_count, panic_message.c_str());
    return 1;
  }
  std::printf("case %s ok\n", case_name);
  return 0;
}

int case_unknown_handle() {
  const char* case_name = "release_unknown_handle_panics";
  reset_panic();
  MlcVmConfig config = standard_config();
  MlcVm* virtual_machine = mlc_vm_create(&config);
  mlc_handle_release(virtual_machine, 9999);
  mlc_vm_destroy(virtual_machine);
  if (panic_invocation_count != 1 || panic_message.find("bad handle") == std::string::npos) {
    std::printf("case %s panics=%d message=%s\n", case_name, panic_invocation_count, panic_message.c_str());
    return 1;
  }
  std::printf("case %s ok\n", case_name);
  return 0;
}

int case_null_arguments(const std::vector<std::uint8_t>& module_bytes) {
  const char* case_name = "null_arguments";
  MlcValue argument{};
  argument.raw = mlc::script_vm::encode_int32(41);
  MlcValue result_value{};
  const MlcResult null_virtual_machine = mlc_vm_call(nullptr, 1, &argument, 1, &result_value);
  if (null_virtual_machine.code != MLC_TYPE) {
    std::printf("case %s call-null got=%d\n", case_name, null_virtual_machine.code);
    return 1;
  }
  MlcVmConfig config = standard_config();
  MlcVm* virtual_machine = mlc_vm_create(&config);
  const MlcResult null_name = mlc_vm_load_module(
      virtual_machine, nullptr, module_bytes.data(), module_bytes.size());
  if (null_name.code != MLC_TYPE) {
    std::printf("case %s load-null got=%d\n", case_name, null_name.code);
    mlc_vm_destroy(virtual_machine);
    return 1;
  }
  if (load_demo(virtual_machine, module_bytes, case_name) != 0) {
    mlc_vm_destroy(virtual_machine);
    return 1;
  }
  const MlcHandle handle = mlc_vm_get_export(virtual_machine, "demo", "add1");
  const MlcResult null_result = mlc_vm_call(virtual_machine, handle, &argument, 1, nullptr);
  if (null_result.code != MLC_TYPE) {
    std::printf("case %s result-null got=%d\n", case_name, null_result.code);
    mlc_vm_destroy(virtual_machine);
    return 1;
  }
  mlc_vm_destroy(nullptr);
  const MlcHandle null_export = mlc_vm_get_export(nullptr, "demo", "add1");
  mlc_vm_destroy(virtual_machine);
  if (null_export != 0) {
    return fail_case(case_name, "null get_export");
  }
  std::printf("case %s ok\n", case_name);
  return 0;
}

int case_garbage_blob() {
  const char* case_name = "garbage_blob";
  const char garbage[] = "hello";
  MlcVmConfig config = standard_config();
  MlcVm* virtual_machine = mlc_vm_create(&config);
  const MlcResult loaded = mlc_vm_load_module(virtual_machine, "demo", garbage, 5);
  mlc_vm_destroy(virtual_machine);
  if (loaded.code != MLC_FORMAT) {
    std::printf("case %s got=%d expected=%d\n", case_name, loaded.code, MLC_FORMAT);
    return 1;
  }
  std::printf("case %s ok\n", case_name);
  return 0;
}

int case_failed_load_keeps_module(const std::vector<std::uint8_t>& module_bytes) {
  const char* case_name = "failed_load_keeps_module";
  const char garbage[] = "hello";
  MlcVmConfig config = standard_config();
  MlcVm* virtual_machine = mlc_vm_create(&config);
  if (load_demo(virtual_machine, module_bytes, case_name) != 0) {
    mlc_vm_destroy(virtual_machine);
    return 1;
  }
  const MlcResult reloaded = mlc_vm_load_module(virtual_machine, "demo", garbage, 5);
  if (reloaded.code != MLC_FORMAT) {
    std::printf("case %s reload got=%d\n", case_name, reloaded.code);
    mlc_vm_destroy(virtual_machine);
    return 1;
  }
  const MlcHandle handle = mlc_vm_get_export(virtual_machine, "demo", "add1");
  MlcValue argument{};
  argument.raw = mlc::script_vm::encode_int32(41);
  MlcValue result_value{};
  const MlcResult called = mlc_vm_call(virtual_machine, handle, &argument, 1, &result_value);
  const std::int32_t decoded = mlc::script_vm::decode_int32(result_value.raw);
  mlc_vm_destroy(virtual_machine);
  if (called.code != MLC_OK || mlc::script_vm::decode_kind(result_value.raw) != 3 || decoded != 42) {
    std::printf("case %s code=%d value=%d\n", case_name, called.code, decoded);
    return 1;
  }
  std::printf("case %s ok\n", case_name);
  return 0;
}

int case_argument_count_without_arguments(const std::vector<std::uint8_t>& module_bytes) {
  const char* case_name = "argc_without_args";
  MlcVmConfig config = standard_config();
  MlcVm* virtual_machine = mlc_vm_create(&config);
  if (load_demo(virtual_machine, module_bytes, case_name) != 0) {
    mlc_vm_destroy(virtual_machine);
    return 1;
  }
  const MlcHandle handle = mlc_vm_get_export(virtual_machine, "demo", "add1");
  MlcValue result_value{};
  const MlcResult called = mlc_vm_call(virtual_machine, handle, nullptr, 1, &result_value);
  mlc_vm_destroy(virtual_machine);
  if (called.code != MLC_BOUNDS) {
    std::printf("case %s got=%d expected=%d\n", case_name, called.code, MLC_BOUNDS);
    return 1;
  }
  std::printf("case %s ok\n", case_name);
  return 0;
}

int main() {
  const std::vector<std::uint8_t> module_bytes = add1_module_bytes();
  if (case_missing_export(module_bytes) != 0) {
    return 1;
  }
  if (case_unknown_module() != 0) {
    return 1;
  }
  if (case_zero_handle() != 0) {
    return 1;
  }
  if (case_call_after_release(module_bytes) != 0) {
    return 1;
  }
  if (case_double_release(module_bytes) != 0) {
    return 1;
  }
  if (case_unknown_handle() != 0) {
    return 1;
  }
  if (case_null_arguments(module_bytes) != 0) {
    return 1;
  }
  if (case_garbage_blob() != 0) {
    return 1;
  }
  if (case_failed_load_keeps_module(module_bytes) != 0) {
    return 1;
  }
  if (case_argument_count_without_arguments(module_bytes) != 0) {
    return 1;
  }
  return 0;
}
