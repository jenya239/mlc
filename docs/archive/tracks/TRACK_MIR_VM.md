# Track: MIR interpreter (`mlcc --run`)

Parent: [../PLAN.md](../PLAN.md); [TRACK_MIR.md](TRACK_MIR.md) (lowering closed)

## Status: **closed** (MVP; STEP=7 done 2026-06-26)

**Goal:** dev-run without `g++`. C++ backend unchanged; MIR is the fork:

```text
SemanticIR ? MIR ? MirInterpreter   (--run)
                 ? CppAST ? emit     (release / self-host)
```

**Not in scope (MVP):** JIT, bytecode file format, GC, full self-host via VM.

**Full plan (self-host path, architecture-first):** [TRACK_MIR_VM_FULL.md](TRACK_MIR_VM_FULL.md)

---

## Verify gate

**Tier A (each step):** `bash scripts/dev_gate_fast.sh`

**Tier B (MVP close):** `build_tests.sh` + `mlcc --run misc/examples/minimal.mlc` vs C++ diff stdout

---

| Step | Item | Status |
|------|------|--------|
| 1 | `compiler/vm/value.mlc`, `frame.mlc`, `interpreter.mlc` - walk MIR structs; literals, binops, return | done |
| 2 | `--run` / `--trace-vm` CLI; pipeline hook after `build_mir_program_from_semantic_items` | done |
| 3 | `MirOperandConstStr`, `VmString`, `println`; `misc/examples/vm_hello.mlc` | done |
| 4 | if/while smoke via `test_mir_vm_smoke.mlc` | done |
| 5 | records, variants, match (lowered MIR only); differential vs C++ | done (match via cond-jump MIR; record/variant await MIR operands; `run_vm_cpp_exit_diff.sh` minimal+hello) |
| 6 | arrays/maps/strings natives; e2e subset | done (`VmArray`/`VmMap`, `__mir_*` natives, smoke + `vm_collections.mlc`) |
| 7 | VM vs C++ diff harness in `compiler/tests/` | done (`vm_cpp_diff_programs.txt`, `run_vm_cpp_exit_diff.sh`, `build_tests.sh` phase) |

---

## Layout

```text
compiler/vm/
  value.mlc
  frame.mlc
  interpreter.mlc
  native.mlc
  error.mlc
```

---

## MVP-1 acceptance

```bash
mlcc --run misc/examples/minimal.mlc
```

Supports: `main`, literals, locals, arithmetic, `if`, `while`, calls, `return`, `println`.

---

## Rule

If MIR cannot be interpreted statement-by-statement, fix MIR lowering first - do not patch VM for SemanticIR shapes.

---

## Per-turn template

```text
ROLE=Driver STEP=N TRACK_MIR_VM
done: <files>
verify: dev_gate_fast + --run target
```
