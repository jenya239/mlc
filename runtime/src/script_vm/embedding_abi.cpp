// TRACK_MLC_SCRIPT_VM §103i — C ABI bridge (calls MLC embedding::* — no opcode twin).
#include "mlc/script_vm/embedding_abi.h"

#include "embedding.hpp"
#include "embedding_codes.hpp"

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <limits>
#include <new>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace {

struct HandleSlot {
  bool live = false;
  std::uint32_t generation = 1;
  std::string module_name;
  std::string export_name;
  std::vector<std::uint8_t> blob;
};

std::uint32_t handle_index(MlcHandle handle) {
  return static_cast<std::uint32_t>(handle & 0xffffffffu);
}

std::uint32_t handle_generation(MlcHandle handle) {
  return static_cast<std::uint32_t>(handle >> 32);
}

MlcHandle pack_handle(std::uint32_t index, std::uint32_t generation) {
  return (static_cast<MlcHandle>(generation) << 32) | index;
}

struct EmbeddingVmState {
  std::uint64_t instruction_limit = 0;
  std::int32_t heap_limit_objects = 1000000;
  MlcVmPanicCallback panic_callback = nullptr;
  bool owner_thread_bound = false;
  std::thread::id owner_thread{};
  std::unordered_map<std::string, std::vector<std::uint8_t>> modules;
  std::vector<HandleSlot> handles;
};

// Load, call, and release run only on the thread that created the instance.
// A foreign thread is not attached. Debug builds assert; they do not continue into the heap.
void bind_owner_thread(EmbeddingVmState& state) {
  state.owner_thread = std::this_thread::get_id();
  state.owner_thread_bound = true;
}

void require_owner_thread(const EmbeddingVmState& state) {
  assert(state.owner_thread_bound);
  assert(state.owner_thread == std::this_thread::get_id());
}

void maybe_panic(EmbeddingVmState* vm, const char* message) {
  if (vm != nullptr && vm->panic_callback != nullptr) {
    vm->panic_callback(message);
  }
}

std::int64_t clamped_instruction_limit(std::uint64_t instruction_limit) {
  const auto maximum = static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max());
  if (instruction_limit > maximum) {
    return std::numeric_limits<std::int64_t>::max();
  }
  return static_cast<std::int64_t>(instruction_limit);
}

} // namespace

struct MlcVm {
  EmbeddingVmState state;
};

struct MlcVmV2 {
  EmbeddingVmState state;
};

// std::bad_alloc becomes a status. A segmentation fault is not caught.
template <typename Function>
MlcResult status_from_native(Function function) {
  try {
    return function();
  } catch (const std::bad_alloc&) {
    MlcResult result{};
    result.code = MLC_HEAP_LIMIT;
    return result;
  } catch (const std::exception&) {
    MlcResult result{};
    result.code = MLC_UNSUPPORTED;
    return result;
  }
}

extern "C" {

MlcVm* mlc_vm_create(const MlcVmConfig* config) try {
  auto* vm = new MlcVm();
  if (config != nullptr) {
    vm->state.instruction_limit = config->instruction_limit;
    if (config->heap_limit_objects > 0) {
      vm->state.heap_limit_objects = config->heap_limit_objects;
    }
    vm->state.panic_callback = config->panic_callback;
  }
  bind_owner_thread(vm->state);
  return vm;
} catch (const std::bad_alloc&) {
  return nullptr;
} catch (const std::exception&) {
  return nullptr;
}

void mlc_vm_destroy(MlcVm* vm) try {
  if (vm != nullptr) {
    require_owner_thread(vm->state);
  }
  delete vm;
} catch (const std::bad_alloc&) {
} catch (const std::exception&) {
}

MlcResult mlc_vm_load_module(
  MlcVm* vm,
  const char* name,
  const void* source_or_bytecode,
  size_t size
) try {
  MlcResult result{};
  if (vm == nullptr || name == nullptr || source_or_bytecode == nullptr) {
    result.code = embedding_codes::emb_type();
    return result;
  }
  require_owner_thread(vm->state);
  if (size > static_cast<size_t>(0x7fffffff)) {
    result.code = embedding_codes::emb_format();
    return result;
  }
  const auto size_i32 = static_cast<std::int32_t>(size);
  const auto* source_bytes = static_cast<const std::uint8_t*>(source_or_bytecode);
  std::vector<std::uint8_t> owned(source_bytes, source_bytes + size);
  const auto owned_pointer = reinterpret_cast<std::int64_t>(owned.data());
  const auto code = embedding::emb_verify_module_ptr(owned_pointer, size_i32);
  if (code != embedding_codes::emb_ok()) {
    result.code = code;
    return result;
  }
  vm->state.modules[std::string(name)] = std::move(owned);
  result.code = embedding_codes::emb_ok();
  return result;
} catch (const std::bad_alloc&) {
  MlcResult result{};
  result.code = MLC_HEAP_LIMIT;
  return result;
} catch (const std::exception&) {
  MlcResult result{};
  result.code = MLC_UNSUPPORTED;
  return result;
}

MlcHandle mlc_vm_get_export(MlcVm* vm, const char* module_name, const char* export_name) try {
  if (vm == nullptr || module_name == nullptr || export_name == nullptr) {
    return 0;
  }
  require_owner_thread(vm->state);
  const auto module_it = vm->state.modules.find(std::string(module_name));
  if (module_it == vm->state.modules.end()) {
    return 0;
  }
  const auto data_pointer = reinterpret_cast<std::int64_t>(module_it->second.data());
  const auto export_present = embedding::emb_has_export_ptr(
      data_pointer,
      static_cast<std::int32_t>(module_it->second.size()),
      mlc::String(export_name));
  if (export_present == 0) {
    return 0;
  }
  std::size_t reused_index = vm->state.handles.size();
  for (std::size_t index = 0; index < vm->state.handles.size(); ++index) {
    if (!vm->state.handles[index].live) {
      reused_index = index;
      break;
    }
  }
  if (reused_index == vm->state.handles.size()) {
    HandleSlot slot;
    slot.live = true;
    slot.generation = 1;
    slot.module_name = module_name;
    slot.export_name = export_name;
    slot.blob = module_it->second;
    vm->state.handles.push_back(std::move(slot));
    return pack_handle(static_cast<std::uint32_t>(vm->state.handles.size()), 1);
  }
  HandleSlot& slot = vm->state.handles[reused_index];
  slot.live = true;
  slot.module_name = module_name;
  slot.export_name = export_name;
  slot.blob = module_it->second;
  return pack_handle(static_cast<std::uint32_t>(reused_index + 1), slot.generation);
} catch (const std::bad_alloc&) {
  return 0;
} catch (const std::exception&) {
  return 0;
}

MlcResult mlc_vm_call(
  MlcVm* vm,
  MlcHandle callable,
  const MlcValue* args,
  size_t argc,
  MlcValue* result_value
) try {
  MlcResult result{};
  if (vm == nullptr || result_value == nullptr) {
    result.code = embedding_codes::emb_type();
    return result;
  }
  require_owner_thread(vm->state);
  const std::uint32_t callable_index = handle_index(callable);
  const std::uint32_t callable_generation = handle_generation(callable);
  if (callable_index == 0 || callable_index > static_cast<std::uint32_t>(vm->state.handles.size())) {
    result.code = embedding_codes::emb_bad_handle();
    return result;
  }
  HandleSlot& slot = vm->state.handles[static_cast<std::size_t>(callable_index - 1)];
  if (!slot.live || slot.generation != callable_generation) {
    result.code = embedding_codes::emb_bad_handle();
    return result;
  }
  if (argc > static_cast<size_t>(0x7fffffff) || (argc > 0 && args == nullptr)) {
    result.code = embedding_codes::emb_bounds();
    return result;
  }
  std::int64_t result_raw = 0;
  const auto data_pointer = reinterpret_cast<std::int64_t>(slot.blob.data());
  const auto args_pointer = reinterpret_cast<std::int64_t>(args);
  const auto result_pointer = reinterpret_cast<std::int64_t>(&result_raw);
  const auto code = embedding::emb_call_export_ptr_limited(
      data_pointer,
      static_cast<std::int32_t>(slot.blob.size()),
      mlc::String(slot.export_name.c_str()),
      args_pointer,
      static_cast<std::int32_t>(argc),
      result_pointer,
      clamped_instruction_limit(vm->state.instruction_limit),
      vm->state.heap_limit_objects
  );
  result.code = code;
  if (code == embedding_codes::emb_ok()) {
    result_value->raw = result_raw;
  }
  return result;
} catch (const std::bad_alloc&) {
  MlcResult result{};
  result.code = MLC_HEAP_LIMIT;
  return result;
} catch (const std::exception&) {
  MlcResult result{};
  result.code = MLC_UNSUPPORTED;
  return result;
}

void mlc_handle_release(MlcVm* vm, MlcHandle handle) try {
  const std::uint32_t released_index = handle_index(handle);
  const std::uint32_t released_generation = handle_generation(handle);
  if (vm == nullptr || released_index == 0 ||
      released_index > static_cast<std::uint32_t>(vm->state.handles.size())) {
    maybe_panic(vm == nullptr ? nullptr : &vm->state, "mlc_handle_release: bad handle");
    return;
  }
  require_owner_thread(vm->state);
  HandleSlot& slot = vm->state.handles[static_cast<std::size_t>(released_index - 1)];
  if (!slot.live || slot.generation != released_generation) {
    maybe_panic(&vm->state, "mlc_handle_release: double release");
    return;
  }
  slot.live = false;
  slot.generation = slot.generation + 1;
  slot.blob.clear();
} catch (const std::bad_alloc&) {
} catch (const std::exception&) {
}

size_t mlc_script_vm_fixture_add1(uint8_t* out, size_t capacity) try {
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
} catch (const std::bad_alloc&) {
  return 0;
} catch (const std::exception&) {
  return 0;
}

MlcResult mlc_vm_create_v2(const MlcVmConfigV2* config, MlcVmV2** created_out) {
  return status_from_native([&]() -> MlcResult {
    MlcResult result{};
    if (created_out == nullptr) {
      result.code = MLC_TYPE;
      return result;
    }
    *created_out = nullptr;
    if (config == nullptr) {
      result.code = MLC_TYPE;
      return result;
    }
    const auto required_size = static_cast<std::uint32_t>(sizeof(MlcVmConfigV2));
    if (config->struct_size < required_size) {
      result.code = MLC_UNSUPPORTED;
      return result;
    }
    if (config->abi_version != MLC_VM_ABI_VERSION_V2) {
      result.code = MLC_UNSUPPORTED;
      return result;
    }
    auto* virtual_machine = new MlcVmV2();
    virtual_machine->state.instruction_limit = config->instruction_limit;
    if (config->heap_limit_objects > 0) {
      virtual_machine->state.heap_limit_objects = config->heap_limit_objects;
    }
    virtual_machine->state.panic_callback = config->panic_callback;
    bind_owner_thread(virtual_machine->state);
    *created_out = virtual_machine;
    result.code = MLC_OK;
    return result;
  });
}

void mlc_vm_destroy_v2(MlcVmV2* virtual_machine) try {
  if (virtual_machine != nullptr) {
    require_owner_thread(virtual_machine->state);
  }
  delete virtual_machine;
} catch (const std::bad_alloc&) {
} catch (const std::exception&) {
}

} // extern "C"

struct MlcRepl {
  bool released = false;
  std::uint64_t instruction_limit = 0;
  std::int32_t heap_limit_objects = 1000000;
  MlcVmPanicCallback panic_callback = nullptr;
  std::string accepted_source;
  std::string pending;
};

static void repl_panic(MlcRepl* session, const char* message) {
  if (session != nullptr && session->panic_callback != nullptr) {
    session->panic_callback(message);
  }
}

static void copy_code_text(char* destination, const mlc::String& code) {
  const auto code_view = code.view();
  std::size_t code_length = code_view.size();
  if (code_length > 63) {
    code_length = 63;
  }
  if (code_length > 0) {
    std::memcpy(destination, code_view.data(), code_length);
  }
  destination[code_length] = '\0';
}

extern "C" {

MlcRepl* mlc_repl_create(const MlcVmConfig* config) try {
  auto* session = new MlcRepl();
  if (config != nullptr) {
    session->instruction_limit = config->instruction_limit;
    if (config->heap_limit_objects > 0) {
      session->heap_limit_objects = config->heap_limit_objects;
    }
    session->panic_callback = config->panic_callback;
  }
  return session;
} catch (const std::bad_alloc&) {
  return nullptr;
} catch (const std::exception&) {
  return nullptr;
}

void mlc_repl_destroy(MlcRepl* session) try {
  if (session == nullptr) {
    return;
  }
  if (session->released) {
    repl_panic(session, "mlc_repl_destroy: double release");
    return;
  }
  session->released = true;
} catch (const std::bad_alloc&) {
} catch (const std::exception&) {
}

int mlc_repl_submit(MlcRepl* session, const char* text, size_t size, MlcReplAnswer* answer) try {
  if (session == nullptr || answer == nullptr || session->released) {
    return -1;
  }
  const char* chunk_text = text == nullptr ? "" : text;
  const std::size_t chunk_size = text == nullptr ? 0 : size;
  const std::string chunk(chunk_text, chunk_size);
  const auto result = embedding::emb_repl_submit(
      mlc::String(session->accepted_source),
      mlc::String(session->pending),
      mlc::String(chunk),
      clamped_instruction_limit(session->instruction_limit),
      session->heap_limit_objects);
  session->accepted_source.assign(result.accepted_source.view());
  session->pending.assign(result.pending.view());
  *answer = MlcReplAnswer{};
  answer->status = result.status;
  answer->value = result.value;
  answer->string_length = result.string_length;
  answer->line = result.line;
  answer->column = result.column;
  copy_code_text(answer->code_text, result.code);
  return 0;
} catch (const std::bad_alloc&) {
  return -1;
} catch (const std::exception&) {
  return -1;
}

} // extern "C"

namespace {

MlcResult native_allocation_status() {
  return status_from_native([]() -> MlcResult {
    throw std::bad_alloc();
  });
}

bool short_configuration_rejected() {
  alignas(MlcVmConfigV2) unsigned char storage[sizeof(MlcVmConfigV2)];
  std::memset(storage, 0x5A, sizeof(storage));
  auto* configuration = reinterpret_cast<MlcVmConfigV2*>(storage);
  configuration->struct_size =
      static_cast<std::uint32_t>(sizeof(std::uint32_t) + sizeof(std::uint32_t));
  configuration->abi_version = MLC_VM_ABI_VERSION_V2;
  MlcVmV2* created = nullptr;
  const MlcResult result = mlc_vm_create_v2(configuration, &created);
  if (created != nullptr) {
    mlc_vm_destroy_v2(created);
    return false;
  }
  return result.code == MLC_UNSUPPORTED;
}

bool current_configuration_accepted() {
  MlcVmConfigV2 configuration{};
  configuration.struct_size = static_cast<std::uint32_t>(sizeof(MlcVmConfigV2));
  configuration.abi_version = MLC_VM_ABI_VERSION_V2;
  configuration.heap_limit_objects = 1;
  MlcVmV2* created = nullptr;
  const MlcResult result = mlc_vm_create_v2(&configuration, &created);
  if (result.code != MLC_OK || created == nullptr) {
    return false;
  }
  const bool owner_matches =
      created->state.owner_thread_bound &&
      created->state.owner_thread == std::this_thread::get_id();
  mlc_vm_destroy_v2(created);
  return owner_matches;
}

struct BoundaryProbe {
  BoundaryProbe() {
    if (!short_configuration_rejected()) {
      std::fprintf(stderr, "[script_vm embedding_v2] FAIL: short config accepted\n");
      std::exit(1);
    }
    if (!current_configuration_accepted()) {
      std::fprintf(stderr, "[script_vm embedding_v2] FAIL: current config rejected\n");
      std::exit(1);
    }
    const MlcResult allocation = native_allocation_status();
    if (allocation.code != MLC_HEAP_LIMIT) {
      std::fprintf(
          stderr,
          "[script_vm embedding_v2] FAIL: allocation exception was not a status code=%d\n",
          allocation.code);
      std::exit(1);
    }
  }
};

BoundaryProbe boundary_probe;

} // namespace
