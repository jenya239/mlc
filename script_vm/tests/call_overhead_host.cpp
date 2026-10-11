#include "mlc/script_vm/embedding_abi.h"
#include "mlc/script_vm/value_rep_abi.hpp"

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

std::vector<std::uint8_t> read_decimal_module(const char* path) {
  std::ifstream input(path);
  std::vector<std::uint8_t> module_bytes;
  int word = 0;
  while (input >> word) {
    if (word < 0 || word > 255) {
      module_bytes.clear();
      return module_bytes;
    }
    module_bytes.push_back(static_cast<std::uint8_t>(word));
  }
  return module_bytes;
}

int fail_measurement(const char* detail) {
  std::printf("measurement failed: %s\n", detail);
  return 1;
}

MlcVm* open_module(const std::vector<std::uint8_t>& module_bytes, const char* export_name, MlcHandle* handle_out) {
  if (module_bytes.empty()) {
    return nullptr;
  }
  MlcVmConfig config{};
  config.instruction_limit = 0;
  config.heap_limit_objects = 1000000;
  config.panic_callback = nullptr;
  MlcVm* virtual_machine = mlc_vm_create(&config);
  if (virtual_machine == nullptr) {
    return nullptr;
  }
  const MlcResult loaded = mlc_vm_load_module(
      virtual_machine, "module", module_bytes.data(), module_bytes.size());
  if (loaded.code != MLC_OK) {
    mlc_vm_destroy(virtual_machine);
    return nullptr;
  }
  const MlcHandle handle = mlc_vm_get_export(virtual_machine, "module", export_name);
  if (handle == 0) {
    mlc_vm_destroy(virtual_machine);
    return nullptr;
  }
  *handle_out = handle;
  return virtual_machine;
}

std::int64_t elapsed_nanoseconds(std::chrono::steady_clock::time_point started, std::chrono::steady_clock::time_point finished) {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(finished - started).count();
}

int main(int argument_count, char** argument_values) {
  if (argument_count != 3) {
    return fail_measurement("expected two module paths");
  }
  const char* trivial_export_name = "add1";
  const std::vector<std::uint8_t> add1_bytes = read_decimal_module(argument_values[1]);
  const std::vector<std::uint8_t> sum_bytes = read_decimal_module(argument_values[2]);
  MlcHandle add1_handle = 0;
  MlcHandle sum_handle = 0;
  MlcVm* add1_machine = open_module(add1_bytes, trivial_export_name, &add1_handle);
  MlcVm* sum_machine = open_module(sum_bytes, "run", &sum_handle);
  if (add1_machine == nullptr || sum_machine == nullptr) {
    mlc_vm_destroy(add1_machine);
    mlc_vm_destroy(sum_machine);
    return fail_measurement("module or export");
  }

  MlcValue argument{};
  argument.raw = mlc::script_vm::encode_int32(41);
  MlcValue result_value{};
  const MlcResult checked = mlc_vm_call(add1_machine, add1_handle, &argument, 1, &result_value);
  if (checked.code != MLC_OK || mlc::script_vm::decode_int32(result_value.raw) != 42) {
    mlc_vm_destroy(add1_machine);
    mlc_vm_destroy(sum_machine);
    return fail_measurement("add1 result");
  }
  MlcValue sum_result{};
  const MlcResult sum_checked = mlc_vm_call(sum_machine, sum_handle, nullptr, 0, &sum_result);
  if (sum_checked.code != MLC_OK || mlc::script_vm::decode_int32(sum_result.raw) != 55) {
    mlc_vm_destroy(add1_machine);
    mlc_vm_destroy(sum_machine);
    return fail_measurement("sum result");
  }

  std::int64_t fastest_average_nanoseconds = 0;
  int batch_index = 0;
  while (batch_index < 5) {
    const auto started = std::chrono::steady_clock::now();
    int call_index = 0;
    while (call_index < 1000) {
      MlcValue ignored{};
      mlc_vm_call(add1_machine, add1_handle, &argument, 1, &ignored);
      call_index = call_index + 1;
    }
    const auto finished = std::chrono::steady_clock::now();
    const std::int64_t average_nanoseconds = elapsed_nanoseconds(started, finished) / 1000;
    if (batch_index == 0 || average_nanoseconds < fastest_average_nanoseconds) {
      fastest_average_nanoseconds = average_nanoseconds;
    }
    batch_index = batch_index + 1;
  }

  std::int64_t fastest_sum_nanoseconds = 0;
  int sample_index = 0;
  while (sample_index < 5) {
    const auto started = std::chrono::steady_clock::now();
    MlcValue ignored{};
    mlc_vm_call(sum_machine, sum_handle, nullptr, 0, &ignored);
    const auto finished = std::chrono::steady_clock::now();
    const std::int64_t sample_nanoseconds = elapsed_nanoseconds(started, finished);
    if (sample_index == 0 || sample_nanoseconds < fastest_sum_nanoseconds) {
      fastest_sum_nanoseconds = sample_nanoseconds;
    }
    sample_index = sample_index + 1;
  }

  mlc_handle_release(add1_machine, add1_handle);
  mlc_handle_release(sum_machine, sum_handle);
  mlc_vm_destroy(add1_machine);
  mlc_vm_destroy(sum_machine);

  if (fastest_average_nanoseconds <= 0 || fastest_sum_nanoseconds <= 0) {
    return fail_measurement("non-positive timing");
  }
  std::printf("trivial_call_nanoseconds %lld\n", static_cast<long long>(fastest_average_nanoseconds));
  std::printf("sum_call_nanoseconds %lld\n", static_cast<long long>(fastest_sum_nanoseconds));
  return 0;
}
