#include "mlc/script_vm/embedding_abi.h"
#include "mlc/script_vm/value_rep_abi.hpp"

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <limits>
#include <string>
#include <vector>

constexpr std::int32_t result_ok = 0;
constexpr std::int32_t result_instruction_limit = 9;
constexpr std::int32_t result_heap_limit = 10;
constexpr std::int32_t result_overflow = 11;
constexpr std::int32_t result_div_zero = 12;

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

int run_case(
    const char* case_name,
    const std::vector<std::uint8_t>& module_bytes,
    const char* export_name,
    const std::vector<std::int32_t>& integer_arguments,
    std::uint64_t instruction_limit,
    std::int32_t heap_limit_objects,
    std::int32_t expected_code,
    bool expect_integer,
    std::int32_t expected_integer) {
  if (module_bytes.empty()) {
    std::printf("case %s empty module\n", case_name);
    return 1;
  }
  MlcVmConfig config{};
  config.instruction_limit = instruction_limit;
  config.heap_limit_objects = heap_limit_objects;
  config.panic_callback = nullptr;
  MlcVm* virtual_machine = mlc_vm_create(&config);
  if (virtual_machine == nullptr) {
    std::printf("case %s create failed\n", case_name);
    return 1;
  }
  const MlcResult loaded = mlc_vm_load_module(
      virtual_machine, "module", module_bytes.data(), module_bytes.size());
  if (loaded.code != result_ok) {
    std::printf("case %s load code=%d\n", case_name, loaded.code);
    mlc_vm_destroy(virtual_machine);
    return 1;
  }
  const MlcHandle handle = mlc_vm_get_export(virtual_machine, "module", export_name);
  if (handle == 0) {
    std::printf("case %s export missing\n", case_name);
    mlc_vm_destroy(virtual_machine);
    return 1;
  }
  std::vector<MlcValue> arguments;
  for (std::int32_t integer_argument : integer_arguments) {
    MlcValue argument{};
    argument.raw = mlc::script_vm::encode_int32(integer_argument);
    arguments.push_back(argument);
  }
  MlcValue result_value{};
  const MlcValue* argument_pointer = arguments.empty() ? nullptr : arguments.data();
  const MlcResult called = mlc_vm_call(
      virtual_machine, handle, argument_pointer, arguments.size(), &result_value);
  mlc_handle_release(virtual_machine, handle);
  mlc_vm_destroy(virtual_machine);
  if (called.code != expected_code) {
    std::printf("case %s got=%d expected=%d\n", case_name, called.code, expected_code);
    return 1;
  }
  if (expect_integer) {
    const std::int32_t decoded = mlc::script_vm::decode_int32(result_value.raw);
    if (mlc::script_vm::decode_kind(result_value.raw) != 3 || decoded != expected_integer) {
      std::printf("case %s value=%d expected=%d\n", case_name, decoded, expected_integer);
      return 1;
    }
  }
  std::printf("case %s ok\n", case_name);
  return 0;
}

int main(int argument_count, char** argument_values) {
  if (argument_count != 6) {
    std::printf("case usage expected 5 module paths\n");
    return 2;
  }
  const std::vector<std::uint8_t> sum_loop_bytes = read_decimal_module(argument_values[1]);
  const std::vector<std::uint8_t> fibonacci_bytes = read_decimal_module(argument_values[2]);
  const std::vector<std::uint8_t> divide_bytes = read_decimal_module(argument_values[3]);
  const std::vector<std::uint8_t> two_records_bytes = read_decimal_module(argument_values[4]);
  const std::vector<std::uint8_t> spin_bytes = read_decimal_module(argument_values[5]);
  const std::int32_t integer_minimum = std::numeric_limits<std::int32_t>::min();
  const std::int32_t negative_one = static_cast<std::int32_t>(0) - 1;
  const std::vector<std::int32_t> no_arguments;
  if (run_case("sum_loop", sum_loop_bytes, "run", no_arguments, 0, 0, result_ok, true, 55) != 0) {
    return 1;
  }
  if (run_case("fib", fibonacci_bytes, "run", no_arguments, 0, 0, result_ok, true, 55) != 0) {
    return 1;
  }
  const std::vector<std::int32_t> zero_divisor = {1, 0};
  if (run_case("divide_zero", divide_bytes, "divide", zero_divisor, 0, 0, result_div_zero, false, 0) != 0) {
    return 1;
  }
  const std::vector<std::int32_t> overflow_arguments = {integer_minimum, negative_one};
  if (run_case("overflow", divide_bytes, "divide", overflow_arguments, 0, 0, result_overflow, false, 0) != 0) {
    return 1;
  }
  if (run_case("heap_limit", two_records_bytes, "run", no_arguments, 0, 1, result_heap_limit, false, 0) != 0) {
    return 1;
  }
  if (run_case("instruction_limit", spin_bytes, "spin", no_arguments, 1000, 0, result_instruction_limit, false, 0) != 0) {
    return 1;
  }
  return 0;
}
