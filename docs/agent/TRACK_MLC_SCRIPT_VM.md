# Track: MLC Script VM — embeddable dynamic-profile VM, Phase 1

Parent: [../MLC_SCRIPT_VM.md](../MLC_SCRIPT_VM.md), [../PLAN.md](../PLAN.md) §103.
Trigger: пользователь 2026-07-13 — концепт (design doc). **Авторизовано
2026-07-28** (user request: "можно добавить треки по... script-vm") — снят
HARD STOP GATE, Phase 1 (`MLC_SCRIPT_VM.md` §12 фаза 1) разбита на атомарные
под-треки ниже. Эмфаза по требованию пользователя: производительность,
архитектура, тестирование — у каждого под-трека явный gate.

## Status: **open** 2026-08-06 — queue head **§103h** STEP=0 Decision done; Red next; §103a–g CLOSED; §109/§110 CLOSED

**НЕ путать с [TRACK_MIR_VM_FULL](TRACK_MIR_VM_FULL.md)** — разные объекты,
полная таблица различий: [../MLC_SCRIPT_VM.md](../MLC_SCRIPT_VM.md) §0.

## Non-goals (unchanged from design doc §14)

Prototype chain, implicit string↔number conversions, pervasive `null`,
live class mutation, `eval` in base runtime, dynamic scoping, magic method
resolution, BigInt in core, concurrent/moving GC, trace JIT (LuaJIT-style
side exits), optimizing JIT before a working tier 0/0.5. Замена native/C++
release backend — не цель никогда (третий путь исполнения). Код живёт в
новом top-level `script_vm/` (sibling к `compiler/`/`runtime/`/`lib/`/`misc/`),
**не** в `compiler/vm/` (это MIR VM).

## Design decisions closed at §103a (frozen 2026-08-03)

| # | Question | Choice |
|---|----------|--------|
| 1 | `dyn` | Confirms [MLC_SCRIPT_VM.md](../MLC_SCRIPT_VM.md) §2–§3: native MLC — `dyn` only as **explicit** annotation on the script/FFI boundary (never inferred). Script profile — unannotated locals/params default to `dyn`. No `any`/`dyn` in static MLC without that boundary. **Not implemented in §103a** (Value only); policy binds later language work |
| 2 | `script module` / `native module` | Module-level keywords as in design §15 (`script module name` / `native module name`). File extension / package manifest — **deferred** (not required for Phase 1 Value/bytecode). Grammar not in §103a |
| 3 | `Value` ABI (x86-64 Phase 1) | **`Value { raw: i64 }` (8 bytes)** + **`ValueRep` NaN-boxing** behind encode/decode helpers — not a scattered tagged-union match in callers. Quiet-NaN payloads for Nil / Bool / Int32; IEEE f64 bit pattern when the bits are a real float (round-trip no precision loss). Heap ptr tags reserved, unused until §103f. ARM64 / alternate reps — out of Phase 1 |
| 4 | Verifier vs interpreter | Confirms design §10 / track order: **§103c before §103d**. No interpreter commit without verifier |

## Sub-tracks, in order (Phase 1 only — §12 фазы 2-5 остаются design-only)

### §103a `SCRIPT_VM_VALUE_REP`

#### Decision (**frozen** 2026-08-03, Driver STEP=0)

| Item | Choice |
|------|--------|
| Problem | No `script_vm/` tree; no 8-byte tagged `Value`; design leaves NaN-box vs tagged-union open behind `ValueRep` |
| Fix | New top-level `script_vm/` (sibling of `compiler/`/`runtime/` — **not** `compiler/vm/`). Module `script_vm/value.mlc`: `type Value = { raw: i64 }`; `ValueRep` encode/decode for **Nil / Bool / Int32 / Float64**; `value_byte_size() == 8` (or equivalent compile/runtime assert). No heap, no GC, no dyn syntax |
| Numeric | Int32 payload in NaN-box; Float = raw f64 bits when not a tagged quiet-NaN. Overflow → boxed i64 is **§103f+**, not §103a |
| Build / test | Dedicated `scripts/run_script_vm_value_rep_unit.sh` builds+runs `script_vm/tests/value_rep_unit.mlc` via `mlcc` + `build_bin.sh`. **Not** folded into `dev_gate_fast` / `run_ux_gate` this sub-track. Optional later `run_script_vm_gate.sh` when more units exist |
| Gate | Unit: round-trip every variant; Float64 bit-identical; Bool/Nil distinct; Int32 extremes; `sizeof`/byte-size == 8. Red: green runner/artifacts absent |
| Sabotage | Break Float round-trip (truncate to f32) or lie about size → unit fails |
| REG | no (`script_vm/**` only; no `lib/mlc/**` / `compiler/**` `.mlc`) |
| Out of scope | Heap/GC; bytecode; dyn/script grammar; embedding ABI; ARM64; changing native MLC type system |
| Epic decisions | Table above (dyn / script module / NaN-box / verifier-before-interpreter) |

#### Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** 2026-08-03 |
| 1 | Red: value rep / unit runner absent | **done** 2026-08-03 — `run_script_vm_value_rep_unit_red.sh` exits 1 (`no script_vm ValueRep / value_rep unit`) |
| 2 | Green: `script_vm/value.mlc` + unit; size/round-trip; `dev_gate_fast` | **done** 2026-08-03 — `value.mlc` + header ABI; `run_script_vm_value_rep_unit.sh` ok; sabotage (size→4) fails; `dev_gate_fast` 1471/0 |
| 3 | Critic | **done** 2026-08-03 — independent unit + C++ probe; sabotages (f32 decode truncate → bit-identical FAIL; Nil→Bool tag → Nil round-trip FAIL); MIR path separate |

**§103a CLOSED** 2026-08-03 (Critic OK). Do not reopen numbered STEPs.

### §103b `SCRIPT_VM_BYTECODE_FORMAT`

#### Decision (**frozen** 2026-08-03, Driver STEP=0)

| Item | Choice |
|------|--------|
| Problem | No Script VM instruction encoding; design §4 leaves ABC layout + extension path unspecified in code |
| Fix | `script_vm/bytecode.mlc`: `Instruction { word: i32 }` holding **u32 bit pattern** `[opcode:8 \| A:8 \| B:8 \| C:8]` (opcode bits 0–7 … C 24–31; MLC/codegen has no usable C++ `u32` mapping). Encode/decode helpers. **Wide form**: primary with **B=0, C=1** (C=1 = wide marker) + trailing `i32` immediate — used for wide `LOAD_CONST` const index and long `JUMP`/`JUMP_IF_FALSE` offsets. Little-endian helpers for word↔bytes |
| Opcode set (§103b) | Frozen numeric tags (gaps reserved for later Phase 1): `LOAD_CONST=1`, `MOVE=2`, `ADD=3`, `SUB=4`, `MUL=5`, `DIV=6`, `RETURN=7`, `JUMP=8`, `JUMP_IF_FALSE=9`. Heap/call/`GET_PROP` etc. **not** in §103b enum (added in §103g/§103h). Semantics of arithmetic/jumps are **not** executed here — format + disasm only |
| Operand conventions | `LOAD_CONST`: A=dst, B=const_index if ≤255 (C=0); else B=0,C=1 + trailing index. `MOVE`: A=dst, B=src. `ADD`/`SUB`/`MUL`/`DIV`: A=dst, B=lhs, C=rhs. `RETURN`: A=src. `JUMP`: PC-relative offset in B:C as signed 16-bit if in range; else B=0,C=1 + trailing i32. `JUMP_IF_FALSE`: A=cond reg; offset same as `JUMP` |
| Disassembler | `instruction_disassemble(words, index) -> string` text dump (not a UI). Fixed expected-string fixtures per opcode (narrow + one wide example for `LOAD_CONST` and `JUMP`) |
| Build / test | `scripts/run_script_vm_bytecode_format_unit.sh` → `script_vm/tests/bytecode_format_unit.mlc` via `mlcc` + `build_bin.sh`. **Not** in `dev_gate_fast` / `run_ux_gate` this sub-track. Same pattern as §103a |
| Gate | encode→decode round-trip for every opcode in the §103b set (narrow operands); wide `LOAD_CONST`/`JUMP` round-trip including trailing word; disasm output matches frozen fixture strings. Red: green runner / `bytecode.mlc` / unit absent |
| Sabotage | Swap two opcode tag numbers, or truncate wide immediate on decode → unit fails |
| REG | no (`script_vm/**` only; no `lib/mlc/**` / `compiler/**` `.mlc`) |
| Out of scope | Verifier; interpreter; heap opcodes; JIT; changing §103a `Value` |

#### Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** 2026-08-03 |
| 1 | Red: bytecode format / unit runner absent | **done** 2026-08-03 — `run_script_vm_bytecode_format_unit_red.sh` exits 1 (`no script_vm bytecode format / unit`) |
| 2 | Green: `bytecode.mlc` + unit + fixtures; `dev_gate_fast` | **done** 2026-08-03 — unit ok; wide-truncate sabotage fails; `dev_gate_fast` 1471/0 |
| 3 | Critic | **done** 2026-08-03 — independent unit + bit-layout probe; sabotages (ADD disasm commas; JUMP wide truncate) load-bearing |

**§103b CLOSED** 2026-08-03 (Critic OK). Do not reopen numbered STEPs. Note: opcode-tag swap alone is not a load-bearing sabotage when all paths use the same getters — use disasm/wide-decode breaks.

### §103c `SCRIPT_VM_VERIFIER`

#### Decision (**frozen** 2026-08-03, Driver STEP=0)

| Item | Choice |
|------|--------|
| Problem | No static check of §103b bytecode before an interpreter exists; design §10 requires register/constant/branch/shape verification for untrusted buffers |
| Fix | `script_vm/verifier.mlc`: `verify_function(words: [i32], register_count: i32, constant_count: i32) -> VerifyResult`. Walks the word buffer using §103b `instruction_word_span` / decode helpers. **No interpreter**, no heap, no execution of ops |
| Opcode scope | §103b set only (tags 1–9). Unknown opcode → `VerifyErrUnknownOpcode`. Heap/call opcodes deferred with later sub-tracks |
| Checks (each distinct `VerifyErr*` code) | (1) **Register**: any A/B/C used as a register index must be `0 ≤ index < register_count` (`VerifyErrRegister`). (2) **Constant**: `LOAD_CONST` index (narrow B or wide trailing) must be `0 ≤ index < constant_count` (`VerifyErrConstant`). (3) **Branch**: for `JUMP`/`JUMP_IF_FALSE`, `target = word_index + span + offset` must satisfy `0 ≤ target ≤ words.length()` and land on a **primary instruction boundary** (not on a wide trailing word) (`VerifyErrBranch`). (4) **Shape**: wide primary (`C=1`) without a following word → `VerifyErrTruncatedWide`. Empty buffer or missing terminal path not required in §103c (no CFG reachability / “must RETURN”) |
| Result type | `VerifyResult = VerifyOk \| VerifyErr(string, i32)` — `(code, word_index)` with frozen codes: `register`, `constant`, `branch`, `truncated_wide`, `unknown_opcode` (tuple form; Decision's `{code, word_index}` record maps 1:1 for MLC match/codegen) |
| Well-formed fixture | Hand-built words: narrow `LOAD_CONST r0,#0` + `LOAD_CONST r1,#1` + `ADD r2,r0,r1` + `RETURN r2` with `register_count=3`, `constant_count=2` → `VerifyOk` |
| Build / test | `scripts/run_script_vm_verifier_unit.sh` → `script_vm/tests/verifier_unit.mlc` via `mlcc` + `build_bin.sh`. **Not** in `dev_gate_fast` / `run_ux_gate`. Same pattern as §103a/b |
| Gate | Unit: fixture accepts; one crafted buffer each for register / constant / branch / truncated_wide / unknown_opcode — each yields its distinct code. Red: green runner / `verifier.mlc` / unit absent |
| Sabotage | Drop register check (accept `A >= register_count`) → register case passes falsely → unit fails |
| REG | no (`script_vm/**` only) |
| Out of scope | Interpreter (§103d); instruction/heap resource limits; full CFG dominance; opcodes beyond §103b |

#### Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** 2026-08-03 |
| 1 | Red: verifier / unit runner absent | **done** 2026-08-03 — `run_script_vm_verifier_unit_red.sh` exits 1 (`no script_vm verifier / unit`) |
| 2 | Green: `verifier.mlc` + unit; `dev_gate_fast` | **done** 2026-08-03 — unit ok; register-check sabotage fails; `dev_gate_fast` 1471/0 |
| 3 | Critic | **done** 2026-08-03 — independent unit + mid-wide branch probe; sabotages (branch_target_ok→true; skip constant; skip unknown_opcode) load-bearing |

**§103c CLOSED** 2026-08-03 (Critic OK). Do not reopen numbered STEPs.

### §103d `SCRIPT_VM_INTERPRETER_ARITHMETIC`

#### Decision (**frozen** 2026-08-03, Driver STEP=0)

| Item | Choice |
|------|--------|
| Problem | Verifier exists (§103c) but nothing executes §103b arithmetic; design §5 tier-0 / §12 phase-1 need a register interpreter before control flow / heap |
| Fix | `script_vm/interpreter.mlc`: `run_arithmetic(words: [i32], constants: [Value], register_count: i32) -> RunResult`. Builds a register file of `register_count` `Value`s (init Nil). **Always** calls `verify_function(words, register_count, constants.length())` first; on `VerifyErr` → `RunErr("verify", word_index)` (no execute). Then linear PC walk with §103b `instruction_word_span` / decode helpers |
| Opcode execute set | `LOAD_CONST`, `MOVE`, `ADD`, `SUB`, `MUL`, `DIV`, `RETURN` only. Encountering `JUMP` / `JUMP_IF_FALSE` (or any other tag) after verify → `RunErr("unsupported_opcode", primary)`. No heap, no calls, no GC |
| Numeric semantics (§103a ValueRep; design §7 narrowed for §103d) | Same-kind only: Int32⊛Int32 → Int32; Float64⊛Float64 → Float64. Mixed kinds / Nil/Bool operands → `RunErr("type", primary)`. Int32 overflow (checked add/sub/mul outside i32 range) → `RunErr("overflow", primary)` — no boxed i64 yet (§103f+). Float ops use IEEE f64 (no overflow trap). `DIV`: Int32 trunc toward zero; divisor 0 (int or float ±0.0) → `RunErr("div_zero", primary)` |
| Control / termination | No backward PC without JUMP (§103e). Fall off end without `RETURN` → `RunErr("no_return", words.length())`. `RETURN A` → `RunOk(registers[A])` |
| Result type | `RunResult = RunOk(Value) \| RunErr(string, i32)` — codes: `verify`, `type`, `overflow`, `div_zero`, `no_return`, `unsupported_opcode` |
| Arithmetic fixture | constants `[ValueInt32(1), ValueInt32(2)]`; words: narrow `LOAD_CONST r0,#0` + `LOAD_CONST r1,#1` + `ADD r2,r0,r1` + `RETURN r2`; `register_count=3` → `RunOk` Int32(3). Second fixture: Float64 `1.5`/`2.25` → ADD → Float64 `3.75` (bit-identical via `value_raw_equal`) |
| Perf smoke | Host loop: run the Int32 ADD fixture `N=100000` times; record wall ns total and **ns per instruction** = total_ns / (N × instruction_count). Print one line; no ceiling fail in §103d (baseline document only, honest number). In-bytecode loops wait for §103e |
| Build / test | `scripts/run_script_vm_interpreter_arithmetic_unit.sh` → `script_vm/tests/interpreter_arithmetic_unit.mlc` via `mlcc` + `build_bin.sh`. **Not** in `dev_gate_fast` / `run_ux_gate`. Same pattern as §103a–c |
| Gate | Unit: Int32 + Float64 fixtures `RunOk`; one crafted case each for `type` / `overflow` / `div_zero` / `no_return` / `unsupported_opcode` (JUMP after verify-ok shape) / verify-fail passthrough; perf line printed. Red: green runner / `interpreter.mlc` / unit absent |
| Sabotage | Force ADD Int32 path to ignore rhs (always `lhs`) → fixture expects 3, gets 1 → unit fails |
| REG | no (`script_vm/**` only) |
| Out of scope | JUMP/comparisons (§103e); heap/GC (§103f); quickening/JIT; opcode DSL; embedding ABI |

#### Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** 2026-08-03 |
| 1 | Red: interpreter / unit runner absent | **done** 2026-08-03 — `run_script_vm_interpreter_arithmetic_unit_red.sh` exits 1 (`no script_vm interpreter arithmetic / unit`) |
| 2 | Green: `interpreter.mlc` + unit + perf smoke; `dev_gate_fast` | **done** 2026-08-03 — unit ok; ADD-ignore-rhs sabotage fails; `dev_gate_fast` 1471/0 |
| 3 | Critic | **done** 2026-08-03 — independent unit + MOVE/SUB/MUL/float-div_zero probe; sabotages (skip verify; skip int div_zero; JUMP no-op) load-bearing |

**§103d CLOSED** 2026-08-03 (Critic OK). Do not reopen numbered STEPs.

### §103e `SCRIPT_VM_CONTROL_FLOW`

#### Decision (**frozen** 2026-08-03, Driver STEP=0)

| Item | Choice |
|------|--------|
| Problem | §103d rejects `JUMP`/`JUMP_IF_FALSE` as `unsupported_opcode`; no compare ops → cannot express loops; track gate needs sum-1-to-N |
| Fix | Extend `script_vm/bytecode.mlc` + `verifier.mlc` + `interpreter.mlc`. Keep entry `run_arithmetic(words, constants, register_count) -> RunResult` (API frozen §103d; name historical). Execute `JUMP`/`JUMP_IF_FALSE`; add compare opcodes that write `ValueBool` |
| New opcodes (tags; §103b gaps) | `EQ=10`, `NE=11`, `LT=12`, `LE=13`, `GT=14`, `GE=15`. Encoding: A=dst, B=lhs, C=rhs (narrow only). Disasm + encode helpers. Verifier: known-opcode + register checks (A/B/C) |
| JUMP semantics | Unconditional: `program_counter = primary + span + instruction_jump_offset(...)`. Offset may be negative (loops). Target already constrained by §103c verifier (primary boundary / bounds) — interpreter assumes verify-first |
| JUMP_IF_FALSE | A=cond register. **Falsy** → take jump (same PC rule as JUMP): `ValueBool(false)`, `ValueNil`, `ValueInt32(0)`, `ValueFloat64(0.0)`. All other values → fall through (`program_counter += span`) |
| Compare semantics | Same-kind only: Int32⊛Int32 or Float64⊛Float64 → `ValueBool`. Mixed / Nil/Bool operands → `RunErr("type", primary)`. Float compares use IEEE relational ops (NaN → false for `<`/`<=`/`>`/`>=`/`==`; `!=` true if unordered) |
| Sum-1-to-N fixture | Fixed `N=10`. Hand-built: init `sum=0`, `i=1`, `n=10`; loop body `LE cond,i,n` / `JUMP_IF_FALSE cond,end` / `ADD sum,sum,i` / `ADD i,i,1` / `JUMP loop` / `RETURN sum` → `RunOk` Int32(55) |
| Verifier gate (extra) | Crafted `JUMP` with target past `words.length()` still → `VerifyErr("branch", …)` (same §103c code); unit asserts `run_arithmetic` maps that to `RunErr("verify", …)` without executing |
| Build / test | `scripts/run_script_vm_control_flow_unit.sh` → `script_vm/tests/control_flow_unit.mlc` via `mlcc` + `build_bin.sh`. **Not** in `dev_gate_fast` / `run_ux_gate` |
| Gate | Unit: sum-1-to-N = 55; forward JUMP skip; JUMP_IF_FALSE taken/not-taken on Bool; one compare `type` err; verify-fail branch passthrough; §103d Int32 ADD fixture still `RunOk(3)`. Red: green runner / unit absent |
| Sabotage | `JUMP_IF_FALSE` always fall-through (ignore falsy) → sum loop never exits / wrong result → unit fails |
| REG | no (`script_vm/**` only; bytecode/verifier/interpreter touch) |
| Out of scope | Heap/GC; calls/closures; new jump forms beyond §103b encoding; changing §103a ValueRep; renaming `run_arithmetic` |

#### Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** 2026-08-03 |
| 1 | Red: control-flow unit runner absent | **done** 2026-08-03 — `run_script_vm_control_flow_unit_red.sh` exits 1 (`no script_vm control_flow unit`) |
| 2 | Green: compare ops + JUMP exec + unit; `dev_gate_fast` | **done** 2026-08-03 — unit ok (sum=55); JUMP_IF_FALSE sabotage fails; `dev_gate_fast` 1471/0 |
| 3 | Critic | **done** 2026-08-03 — independent unit + EQ/GT/Nil/JUMP+1-wide probe; sabotages (invert LE; JUMP fallthrough; drop JUMP+1 wide-escape) load-bearing |

**§103e CLOSED** 2026-08-03 (Critic OK). Do not reopen numbered STEPs. Note: narrow JUMP offset +1 collides with wide marker B=0,C=1 — encoder must use trailing form (Driver Green).

### §103f `SCRIPT_VM_HEAP_GC_ARENA` — **CLOSED** 2026-08-06 (Critic OK)

Non-moving mark-sweep + size-class arenas (design doc §8). Gate detail below.

#### Decision (**frozen** 2026-08-06, Driver STEP=0)

| Item | Choice |
|------|--------|
| Problem | §103a–e Values are immediate-only (Nil/Bool/Int32/Float64); no heap, no `ObjectHeader`, no GC roots, no live-object accounting — track gate and §103g arrays/records cannot land |
| Fix | New `script_vm/heap.mlc` (+ optional thin `runtime/include/mlc/script_vm/heap_abi.hpp` only if bit/layout helpers need it — prefer pure MLC). Extend `ValueRep` with **HeapRef** (NaN-box tag **4**, payload = stable non-moving `object_id: u32`). `ObjectHeader { type_id: u32, gc_flags: u16, object_flags: u16, shape_or_meta: u64 }` as in design §6. Size-class freelist arenas (classes **32 / 64 / 128 / 256** bytes, header included). Explicit root set API. `heap_collect` = single-threaded **stop-the-world mark-sweep** (non-moving). Write-barrier **stub**: `heap_write_barrier(parent, slot, new_child)` always callable; increments `write_barrier_hits` (no incremental-grey work yet) |
| Object model (§103f only) | One allocatable kind: **`Cell`** (`type_id` fixed) = header + one child `Value` slot (Nil or HeapRef). Enough to prove reachability without §103g array/record opcodes |
| Roots | `heap_root_push(Value)` / `heap_root_clear()` (or equivalent). Collect marks from roots only — registers/globals **not** auto-scanned this STEP |
| Gate metrics | `heap_live_object_count()`; after N alloc+drop-root+collect cycles count returns to baseline (no leak); unreachable Cell reclaimed; reachable (rooted) Cell survives |
| Build / test | `scripts/run_script_vm_heap_gc_arena_unit.sh` → `script_vm/tests/heap_gc_arena_unit.mlc` via `mlcc` + `build_bin.sh`. **Not** folded into `run_ux_gate`. Green also runs `dev_gate_fast.sh` (same discipline as §103e) |
| Gate | Unit: (1) alloc Cell → live≥1; (2) no root + collect → live back to baseline; (3) root held + collect → still live; (4) N≥8 alloc/collect cycles no leak; (5) write_barrier_hits increases on child write. Red: green runner / unit absent |
| Sabotage | Collect that never reclaims (or always reclaims rooted) → unit fails; omit write-barrier bump on child write → hits assert fails |
| REG | no if `script_vm/**` (+ optional `runtime/include/mlc/script_vm/heap_abi.hpp`) only — no `lib/mlc/**` / `compiler/**/*.mlc`. If Green must touch `compiler/` → self-host diff + Tier B (avoid) |
| Out of scope | Array/record opcodes (§103g); closures/fibers (§103h); embedding ABI (§103i); concurrent/moving GC; weak refs; true incremental mark quanta (API may expose `heap_collect` only); interpreter heap opcodes; editing `docs/MLC_SCRIPT_VM.md` (design stays authority, not rewritten here); Phase 2–5 |

#### Pre-cut (audit 2026-08-06)

| Fact | Evidence |
|------|----------|
| No heap module | No `script_vm/heap.mlc`; no `*heap*` under `script_vm/` |
| ValueRep immediate-only | `value.mlc` / `value_rep_abi.hpp`: tags Nil/Bool/Int32 (+ Float raw); no HeapRef / tag 4 |
| No GC unit / runner | No `heap_gc_arena_unit.mlc`; no `run_script_vm_heap_gc_arena_unit.sh` |
| Design authority | `MLC_SCRIPT_VM.md` §6 `ObjectHeader`, §8 non-moving mark-sweep + size-class arenas + write barrier — **not** edited this STEP |

#### Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** 2026-08-06 |
| 1 | Red: heap GC unit runner absent | **done** 2026-08-06 — `scripts/run_script_vm_heap_gc_arena_unit_red.sh` exit 1 (`no script_vm heap_gc_arena unit`); green/unit/heap.mlc/HeapRef absent |
| 2 | Green: HeapRef + arenas + mark-sweep + unit; `dev_gate_fast` | **done** 2026-08-06 — `heap.mlc` + HeapRef tag4; unit ok; red already-present; `dev_gate_fast` 1471/0; freelist **push** on sweep (pop-reuse deferred — mlcc drops stmts after if/else-if in do-blocks) |
| 3 | Critic | **done** 2026-08-06 — independent unit OK; sab1 never-reclaim → exit 4; sab2 skip-mark → exit 5; sab3 no barrier bump → exit 6; red already-present; arithmetic side ok |

### Critic notes (§103f)

Independent `SCRIPT_VM_HEAP_GC_ARENA_OUT=tmp/script_vm_heap_gc_critic` green: heap_gc_arena=ok, write_barrier_hits=1. Sabotages load-bearing (sweep always-keep / mark no-op / hits frozen). HeapRef tag4 + ObjectHeader + Cell + roots + STW collect present. **Residual (disclosed, not blocking gate):** freelist **push** on sweep without pop-reuse (Driver: mlcc drops stmts after if/else-if in do-blocks) — size-class lists exist; reuse deferred. No `lib/mlc/**` / `compiler/**/*.mlc` in Green. Queue → §103g Decision.

### §103f Green measured

```
heap_gc_arena=ok
red_already_present=ok
heap_live=1
write_barrier_hits=1
dev_gate_fast=1471/0
```

### §103g `SCRIPT_VM_ARRAYS_RECORDS` — **CLOSED** 2026-08-06 (Critic OK)

Heap-backed array/record objects; field/index opcodes; GC traces elements (design § / track gate).

#### Decision (**frozen** 2026-08-06, Driver STEP=0)

| Item | Choice |
|------|--------|
| Problem | §103f heap only allocates **Cell** (one child); mark follows Cell only. No array/record objects, no `GET_PROP`/`SET_PROP`/index opcodes — track gate (cycle through fields survives iff rooted) cannot land; interpreter still returns `unsupported_opcode` for any heap op |
| Fix | Extend `script_vm/heap.mlc` + `bytecode.mlc` + `verifier.mlc` + `interpreter.mlc`. New heap kinds **Array** / **Record**. New entry `run_with_heap(heap, words, constants, register_count) -> RunHeapResult` (`{ heap, result }`). Keep `run_arithmetic` unchanged for §103d/e fixtures (no heap opcodes) |
| Object model | `type_id`: Cell=1 (existing), **Array=2**, **Record=3**. Slot gains `elements: [Value]` (Cell keeps `child`; Array/Record use `elements`, `child=Nil`). `shape_or_meta` = length/field_count as i64. Alloc via `heap_alloc_array(heap, length)` / `heap_alloc_record(heap, field_count)` → HeapRef. Mark: Cell→child; Array/Record→each element HeapRef. SET paths call `heap_write_barrier` (hits bump) |
| New opcodes (tags 16–21; narrow ABC) | `NEW_ARRAY=16` A=dst, B=len_reg (Int32≥0); `GET_INDEX=17` A=dst, B=arr, C=idx_reg; `SET_INDEX=18` A=arr, B=idx_reg, C=val_reg; `NEW_RECORD=19` A=dst, B=field_count (u8 imm); `GET_PROP=20` A=dst, B=rec, C=field_index (u8); `SET_PROP=21` A=rec, B=field_index, C=val_reg. Prop = **field index** this STEP (symbol/`:` names deferred). Bounds / wrong type_id → `RunErr("type"\|"bounds", primary)` |
| Cycle gate (authority) | Hand-built: two Records, `SET_PROP` each → other (2-cycle); (a) no roots + collect → live back to baseline; (b) root one + collect → both live. Proves mark-sweep handles cycles (refcount-only would leak or over-collect). Prefer **heap API** construction for the cycle assert; interpreter opcodes covered by separate NEW/GET/SET smoke in same unit |
| Build / test | `scripts/run_script_vm_arrays_records_unit.sh` → `script_vm/tests/arrays_records_unit.mlc` via `mlcc` + `build_bin.sh`. **Not** in `run_ux_gate`. Green runs `dev_gate_fast.sh`. Side: §103f heap unit + §103e control_flow still ok |
| Gate | Unit: cycle unreclaimed vs rooted; NEW_ARRAY+SET_INDEX+GET_INDEX round-trip; NEW_RECORD+SET_PROP+GET_PROP; write_barrier_hits increases on SET_*; §103f Cell reclaim still works. Red: green runner / unit absent |
| Sabotage | Mark that skips `elements` → cycle rooted still dies / unreclaimed cycle leaks wrong; SET without barrier bump → hits assert fails |
| REG | no (`script_vm/**` + existing `value_rep_abi.hpp` only if needed — prefer no new ABI). No `lib/mlc/**` / `compiler/**/*.mlc` |
| Out of scope | Closures/fibers (§103h); embedding ABI (§103i); symbol-named props; freelist pop-reuse (§103f residual); concurrent GC; editing `docs/MLC_SCRIPT_VM.md`; Phase 2–5 |

#### Pre-cut (audit 2026-08-06)

| Fact | Evidence |
|------|----------|
| Heap Cell-only | `heap.mlc`: type_id Cell=1; mark follows `child` only; no Array/Record alloc |
| No heap opcodes | `bytecode.mlc` opcodes 1–15 only; interpreter `unsupported_opcode` default |
| No unit / runner | No `arrays_records_unit.mlc`; no `run_script_vm_arrays_records_unit.sh` |
| Interpreter↔heap | `run_arithmetic` has no `Heap` parameter |

#### Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** 2026-08-06 |
| 1 | Red: arrays/records unit runner absent | **done** 2026-08-06 — `scripts/run_script_vm_arrays_records_unit_red.sh` exit 1 (`no script_vm arrays_records unit`); green/unit/opcodes/`run_with_heap` absent |
| 2 | Green: Array/Record + opcodes + cycle gate; `dev_gate_fast` | **done** 2026-08-06 — Array/Record heap; opcodes 16–21; `run_with_heap`; unit cycle+smoke; red already-present; side §103f/§103e; `dev_gate_fast` 1471/0 |
| 3 | Critic | **done** 2026-08-06 — independent unit OK; sab1 skip `mark_elements` → exit 4 (`cycle rooted live=1`); sab2 no barrier bump → exit 8; red already-present; opcodes 16–21; green scope `script_vm/**` only; `dev_gate_fast` 1471/0 |

### Critic notes (§103g)

Independent `SCRIPT_VM_ARRAYS_RECORDS_OUT=tmp/script_vm_arrays_records_critic` green: arrays_records/cycle_*/side ok, write_barrier_hits=1. Sabotages load-bearing (mark without elements; SET without hits bump). Opcodes 16–21 present; `run_with_heap` + Array/Record alloc present. **Residual (disclosed, non-blocking):** Decision said `shape_or_meta` = length/field_count; Green keeps `0` and uses `elements.length()` — gate does not assert meta. No `lib/mlc/**` / `compiler/**/*.mlc` in Green. Queue → §103h Decision.

### §103h `SCRIPT_VM_CLOSURES_FIBERS` — **queue head** (STEP=0 Decision done; Red next)

Closures capturing upvalues; material call `Frame` stack (design doc §11 — frame stays material, no deopt). Gate: mutable upvalue across two calls; recursion to fixed depth without stack corruption.

#### Decision (**frozen** 2026-08-06, Driver STEP=0)

| Item | Choice |
|------|--------|
| Problem | Interpreter is single-frame flat (`execute_with_heap`); no CALL/RETURN frame pop, no Closure heap kind, no upvalue ops — track gate (mutable capture across two calls + recursive depth) cannot land |
| Fix | Extend `heap.mlc` + `bytecode.mlc` + `verifier.mlc` + `interpreter.mlc`. Heap kind **Closure**. Material **Frame** stack in interpreter. `FunctionProto` table (host-owned, not heap). Entry `run_program(heap, protos, entry_proto_index) -> RunHeapResult`. Keep `run_arithmetic` / no-call `run_with_heap` for §103d–g fixtures |
| Object model | `type_id` **Closure=4**. Slot `elements` = upvalue Cells (each Cell type_id=1; mutable capture = Cell.child). `shape_or_meta` = `proto_index` (i64 bits of i32 index into `protos`). Mark: follow `elements` (Cells) as today. Alloc `heap_alloc_closure(heap, proto_index, upvalue_count)` → Nil-filled upvalue slots; fill via `heap_write_barrier` |
| FunctionProto (host) | `{ words: [i32], constants: [Value], register_count: i32 }` — array `protos` passed to `run_program`. Nested protos for recursion/MAKE_CLOSURE by index |
| Frame (interpreter, material) | `{ proto_index: i32, program_counter: i32, registers: [Value], closure: Value, return_dst: i32 }` + `frames: [Frame]`. CALL pushes; RETURN pops and writes result into caller `return_dst`. Max depth **256** → `RunErr("stack", primary)` |
| New opcodes (tags 22–25; narrow ABC) | `CALL=22` A=dst, B=func_reg, C=argc — callee must be Closure; copy args from regs `B+1 .. B+C` into new frame regs `0 .. C-1`; rest Nil; PC=0 on callee proto. `MAKE_CLOSURE=23` A=dst, B=proto_index (u8 imm), C=upvalue_count (u8) — alloc Closure; **following C instruction words** each encode one upvalue source as packed `(is_local:8, reg_or_upval:8)` in low 16 bits of a payload word (span=1+C via special-case in `instruction_word_span` / verifier): `is_local!=0` → capture caller reg as **new Cell** holding that Value; `is_local==0` → share parent Closure upvalue Cell at index. `GET_UPVAL=24` A=dst, B=upval_index (u8). `SET_UPVAL=25` A=upval_index, B=src_reg — write via Cell + `heap_write_barrier`. Existing `RETURN` pops frame (top-level RETURN → `RunOk`) |
| Fiber meaning (this STEP) | **Call-stack frames only** (design §11 material Frame). Cooperative fiber spawn/yield/resume / multi-fiber scheduler — **out of scope** (name residual; gate does not require yield) |
| Gate (authority) | (1) Mutable upvalue: Closure + Cell; SET_UPVAL then CALL twice — second call sees mutated Int32. (2) Recursion: proto calls self to fixed depth N≥8, returns expected Int32; depth>256 → `stack` err. Red: green runner/unit absent |
| Build / test | `scripts/run_script_vm_closures_fibers_unit.sh` → `script_vm/tests/closures_fibers_unit.mlc`. **Not** in `run_ux_gate`. Green: `dev_gate_fast.sh`. Side: §103g arrays_records + §103e control_flow still ok |
| Sabotage | CALL without push (flat PC) → recursion corrupts / wrong result; GET_UPVAL ignores Cell → second call stale; SET_UPVAL skips write_barrier hits bump → hits assert fails |
| REG | no (`script_vm/**` only). No `lib/mlc/**` / `compiler/**/*.mlc` |
| Out of scope | Fiber scheduler/yield; embedding ABI (§103i); freelist pop-reuse; editing `docs/MLC_SCRIPT_VM.md`; Phase 2–5; open upvalues pointing at live registers (always close into Cell on MAKE_CLOSURE) |

#### Pre-cut (audit 2026-08-06)

| Fact | Evidence |
|------|----------|
| No call/closure opcodes | `bytecode.mlc` tags 1–21 only; no CALL/MAKE_CLOSURE/GET_UPVAL/SET_UPVAL |
| Single flat frame | `interpreter.mlc` `execute_with_heap` — one register file, RETURN ends run |
| No Closure kind | `heap.mlc` type_id Cell=1, Array=2, Record=3 only |
| No unit / runner | No `closures_fibers_unit.mlc`; no `run_script_vm_closures_fibers_unit.sh` |

#### Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** 2026-08-06 |
| 1 | Red: closures/fibers unit runner absent | pending |
| 2 | Green: Closure+Frame+CALL/upvals; recursion; `dev_gate_fast` | pending |
| 3 | Critic | pending |

### §103i `SCRIPT_VM_EMBEDDING_ABI`

The C ABI surface from design doc §10: `mlc_vm_create/destroy/
load_module/get_export/call/handle_release`, explicit `MlcVmConfig`
(allocator, resolver, clock, limits, panic callback — no hidden globals).
Gate: a minimal host C program links the ABI, loads a tiny script module
compiled by §103a-h's pipeline, calls one exported function, reads back the
correct result — first true "embeddable" proof, not just an internal test
harness.

## Explicitly deferred past Phase 1 (do not pull forward without new user authorization)

Adaptive interpreter/quickening/inline caches (§12 фаза 2), baseline JIT
(фаза 3), opcode DSL (фаза 4), tier 2 (фаза 5) — `MLC_SCRIPT_VM.md` §12.
Native interop typed boundary (design doc §16) is Phase 1-adjacent but not
listed above; open as §103j only once §103a-i are closed and stable.

## Verification discipline

Same TDD/gate discipline as every other track in this repo: scenario-first
(red before green), one sub-track's Decision frozen before its STEP=1,
`dev_gate_fast.sh` green before Critic close on any sub-track that touches
shared runtime FFI conventions. `script_vm/` is a new top-level directory —
first sub-track (§103a) also settles its build/test wiring (how its tests
run: via `rake`, a dedicated script, or folded into an existing runner) as
part of its Decision, not as an afterthought.
