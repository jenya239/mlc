// TRACK_MLC_SCRIPT_VM §103i — C ABI bridge (calls MLC embedding::* — no opcode twin).
#include "mlc/script_vm/embedding_abi.h"

#include "embedding.hpp"

#include <cstdint>
#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

struct HandleSlot {
  bool live = false;
  std::string module_name;
  std::string export_name;
  std::vector<std::uint8_t> blob;
};

struct EmbeddingVmState {
  std::uint64_t instruction_limit = 0;
  std::int32_t heap_limit_objects = 1000000;
  MlcVmPanicCallback panic_callback = nullptr;
  std::unordered_map<std::string, std::vector<std::uint8_t>> modules;
  std::vector<HandleSlot> handles;
};

void maybe_panic(EmbeddingVmState* vm, const char* message) {
  if (vm != nullptr && vm->panic_callback != nullptr) {
    vm->panic_callback(message);
  }
}

} // namespace

struct MlcVm {
  EmbeddingVmState state;
};

extern "C" {

MlcVm* mlc_vm_create(const MlcVmConfig* config) {
  auto* vm = new MlcVm();
  if (config != nullptr) {
    vm->state.instruction_limit = config->instruction_limit;
    if (config->heap_limit_objects > 0) {
      vm->state.heap_limit_objects = config->heap_limit_objects;
    }
    vm->state.panic_callback = config->panic_callback;
  }
  return vm;
}

void mlc_vm_destroy(MlcVm* vm) {
  delete vm;
}

MlcResult mlc_vm_load_module(
  MlcVm* vm,
  const char* name,
  const void* source_or_bytecode,
  size_t size
) {
  MlcResult result{};
  if (vm == nullptr || name == nullptr || source_or_bytecode == nullptr) {
    result.code = embedding::emb_type();
    return result;
  }
  if (size > static_cast<size_t>(0x7fffffff)) {
    result.code = embedding::emb_format();
    return result;
  }
  const auto data_pointer = reinterpret_cast<std::int64_t>(source_or_bytecode);
  const auto size_i32 = static_cast<std::int32_t>(size);
  const auto code = embedding::emb_verify_module_ptr(data_pointer, size_i32);
  if (code != embedding::emb_ok()) {
    result.code = code;
    return result;
  }
  const auto* bytes = static_cast<const std::uint8_t*>(source_or_bytecode);
  vm->state.modules[std::string(name)] = std::vector<std::uint8_t>(bytes, bytes + size);
  result.code = embedding::emb_ok();
  return result;
}

MlcHandle mlc_vm_get_export(MlcVm* vm, const char* module_name, const char* export_name) {
  if (vm == nullptr || module_name == nullptr || export_name == nullptr) {
    return 0;
  }
  const auto module_it = vm->state.modules.find(std::string(module_name));
  if (module_it == vm->state.modules.end()) {
    return 0;
  }
  HandleSlot slot;
  slot.live = true;
  slot.module_name = module_name;
  slot.export_name = export_name;
  slot.blob = module_it->second;
  vm->state.handles.push_back(std::move(slot));
  return static_cast<MlcHandle>(vm->state.handles.size());
}

MlcResult mlc_vm_call(
  MlcVm* vm,
  MlcHandle callable,
  const MlcValue* args,
  size_t argc,
  MlcValue* result_value
) {
  MlcResult result{};
  if (vm == nullptr || result_value == nullptr) {
    result.code = embedding::emb_type();
    return result;
  }
  if (callable == 0 || callable > static_cast<MlcHandle>(vm->state.handles.size())) {
    result.code = embedding::emb_bad_handle();
    return result;
  }
  HandleSlot& slot = vm->state.handles[static_cast<std::size_t>(callable - 1)];
  if (!slot.live) {
    result.code = embedding::emb_bad_handle();
    return result;
  }
  if (argc > static_cast<size_t>(0x7fffffff)) {
    result.code = embedding::emb_bounds();
    return result;
  }
  std::int64_t result_raw = 0;
  const auto data_pointer = reinterpret_cast<std::int64_t>(slot.blob.data());
  const auto args_pointer = reinterpret_cast<std::int64_t>(args);
  const auto result_pointer = reinterpret_cast<std::int64_t>(&result_raw);
  const auto code = embedding::emb_call_export_ptr(
      data_pointer,
      static_cast<std::int32_t>(slot.blob.size()),
      mlc::String(slot.export_name.c_str()),
      args_pointer,
      static_cast<std::int32_t>(argc),
      result_pointer
  );
  result.code = code;
  if (code == embedding::emb_ok()) {
    result_value->raw = result_raw;
  }
  return result;
}

void mlc_handle_release(MlcVm* vm, MlcHandle handle) {
  if (vm == nullptr || handle == 0 ||
      handle > static_cast<MlcHandle>(vm->state.handles.size())) {
    maybe_panic(vm == nullptr ? nullptr : &vm->state, "mlc_handle_release: bad handle");
    return;
  }
  HandleSlot& slot = vm->state.handles[static_cast<std::size_t>(handle - 1)];
  if (!slot.live) {
    maybe_panic(&vm->state, "mlc_handle_release: double release");
    return;
  }
  slot.live = false;
  slot.blob.clear();
}

size_t mlc_script_vm_fixture_add1(uint8_t* out, size_t capacity) {
  const auto size = embedding::emb_add1_fixture_size();
  if (size < 0) {
    return 0;
  }
  const auto needed = static_cast<size_t>(size);
  if (out == nullptr || capacity < needed) {
    return needed;
  }
  for (std::int32_t index = 0; index < size; ++index) {
    out[static_cast<std::size_t>(index)] =
        static_cast<uint8_t>(embedding::emb_add1_fixture_byte_at(index) & 255);
  }
  return needed;
}

} // extern "C"
