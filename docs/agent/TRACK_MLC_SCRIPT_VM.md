# Track: MLC Script VM — embeddable dynamic-profile VM, Phase 1

Parent: [../MLC_SCRIPT_VM.md](../MLC_SCRIPT_VM.md), [../PLAN.md](../PLAN.md) §103.
Trigger: пользователь 2026-07-13 — концепт (design doc). **Авторизовано
2026-07-28** (user request: "можно добавить треки по... script-vm") — снят
HARD STOP GATE, Phase 1 (`MLC_SCRIPT_VM.md` §12 фаза 1) разбита на атомарные
под-треки ниже. Эмфаза по требованию пользователя: производительность,
архитектура, тестирование — у каждого под-трека явный gate.

## Status: **open** 2026-08-03 — queue head **§103e `SCRIPT_VM_CONTROL_FLOW`** (Red done; STEP=2 Green next)

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
| 2 | Green: compare ops + JUMP exec + unit; `dev_gate_fast` | **open** |
| 3 | Critic | **open** |

### §103f `SCRIPT_VM_HEAP_GC_ARENA`

Non-moving incremental mark-sweep, size-class arenas, `ObjectHeader{type_id,
gc_flags, object_flags, shape_or_meta}`, write barrier stub (design doc §8).
Gate: alloc/free cycle test with an explicit live-object counter — no leaks
across N alloc/collect cycles; a collect call reclaims an object proven
unreachable from roots, does not reclaim one that is reachable.

### §103g `SCRIPT_VM_ARRAYS_RECORDS`

Heap-backed array/record objects; `GET_PROP`/`SET_PROP`/array-index opcodes
tracing correctly through §103f's GC (objects referenced only via array/record
fields survive a collect). Gate: build a record graph with a field cycle,
collect, assert cycle is collected iff unreachable from roots (mark-sweep
handles cycles; this is the test that would catch a refcount-only
regression by mistake).

### §103h `SCRIPT_VM_CLOSURES_FIBERS`

Closures capturing upvalues; `Frame{function, instruction, registers,
caller}` call stack (design doc §11 — frame stays material, no deopt
machinery yet since there is no JIT in Phase 1). Gate: a closure capturing a
mutable upvalue across two calls returns the mutated value on the second
call; a recursive function to a fixed depth returns the correct result
without stack corruption.

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
