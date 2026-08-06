#pragma once

// TRACK_MLC_SCRIPT_VM §103i — C embedding ABI (design doc §10).
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MlcVm MlcVm;

typedef struct MlcValue {
  int64_t raw;
} MlcValue;

typedef uint64_t MlcHandle;

typedef struct MlcResult {
  int32_t code;
} MlcResult;

typedef void (*MlcVmPanicCallback)(const char* message);

typedef struct MlcVmConfig {
  uint64_t instruction_limit;
  int32_t heap_limit_objects;
  MlcVmPanicCallback panic_callback;
  void* reserved_allocator;
  void* reserved_module_resolver;
  void* reserved_clock;
  void* reserved_random;
  void* reserved_logger;
  void* reserved_native_resolver;
  uint64_t reserved_jit_code_cache_limit;
} MlcVmConfig;

MlcVm* mlc_vm_create(const MlcVmConfig* config);
void mlc_vm_destroy(MlcVm* vm);
MlcResult mlc_vm_load_module(
  MlcVm* vm,
  const char* name,
  const void* source_or_bytecode,
  size_t size
);
MlcHandle mlc_vm_get_export(MlcVm* vm, const char* module_name, const char* export_name);
MlcResult mlc_vm_call(
  MlcVm* vm,
  MlcHandle callable,
  const MlcValue* args,
  size_t argc,
  MlcValue* result
);
void mlc_handle_release(MlcVm* vm, MlcHandle handle);

/* Test helper: fill buffer with frozen add1 module blob; returns byte size. */
size_t mlc_script_vm_fixture_add1(uint8_t* out, size_t capacity);

#ifdef __cplusplus
}
#endif
