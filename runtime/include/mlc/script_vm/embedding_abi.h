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

typedef enum MlcResultCode {
  MLC_OK = 0,
  MLC_UNSUPPORTED = 1,
  MLC_VERIFY = 2,
  MLC_TYPE = 3,
  MLC_BOUNDS = 4,
  MLC_STACK = 5,
  MLC_FORMAT = 6,
  MLC_NOT_FOUND = 7,
  MLC_BAD_HANDLE = 8,
  MLC_INSTRUCTION_LIMIT = 9,
  MLC_HEAP_LIMIT = 10,
  MLC_OVERFLOW = 11,
  MLC_DIV_ZERO = 12
} MlcResultCode;

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

#define MLC_VM_ABI_VERSION_V2 2

typedef struct MlcVmV2 MlcVmV2;

/* struct_size is the caller-visible prefix.
   A smaller struct_size is rejected; fields past that prefix are not read.
   Allocation failure is returned as a status.
   A segmentation fault is not a C++ exception and is not translated. */
typedef struct MlcVmConfigV2 {
  uint32_t struct_size;
  uint32_t abi_version;
  uint64_t flags;
  uint64_t instruction_limit;
  int32_t heap_limit_objects;
  MlcVmPanicCallback panic_callback;
} MlcVmConfigV2;

MlcResult mlc_vm_create_v2(const MlcVmConfigV2* config, MlcVmV2** created_out);
void mlc_vm_destroy_v2(MlcVmV2* virtual_machine);

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

typedef struct MlcRepl MlcRepl;

typedef enum MlcReplStatus {
  MLC_REPL_INCOMPLETE = 0,
  MLC_REPL_ACCEPTED = 1,
  MLC_REPL_VALUE = 2,
  MLC_REPL_DIAGNOSTIC = 3
} MlcReplStatus;

typedef struct MlcReplAnswer {
  int32_t status;
  int32_t value;
  int32_t string_length;
  int32_t line;
  int32_t column;
  char code_text[64];
} MlcReplAnswer;

MlcRepl* mlc_repl_create(const MlcVmConfig* config);
void mlc_repl_destroy(MlcRepl* session);
int mlc_repl_submit(MlcRepl* session, const char* text, size_t size, MlcReplAnswer* answer);

#ifdef __cplusplus
}
#endif
