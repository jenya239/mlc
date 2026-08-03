# Track: MLC Script VM — embeddable dynamic-profile VM, Phase 1

Parent: [../MLC_SCRIPT_VM.md](../MLC_SCRIPT_VM.md), [../PLAN.md](../PLAN.md) §103.
Trigger: пользователь 2026-07-13 — концепт (design doc). **Авторизовано
2026-07-28** (user request: "можно добавить треки по... script-vm") — снят
HARD STOP GATE, Phase 1 (`MLC_SCRIPT_VM.md` §12 фаза 1) разбита на атомарные
под-треки ниже. Эмфаза по требованию пользователя: производительность,
архитектура, тестирование — у каждого под-трека явный gate.

## Status: **open** 2026-08-03 — queue head **§103a `SCRIPT_VM_VALUE_REP`** (Decision frozen; STEP=1 Red next)

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
| 1 | Red: value rep / unit runner absent | **open** — `run_script_vm_value_rep_unit_red.sh` must fail |
| 2 | Green: `script_vm/value.mlc` + unit; size/round-trip; `dev_gate_fast` | **open** |
| 3 | Critic | **open** |

### §103b `SCRIPT_VM_BYTECODE_FORMAT`

Opcode enum + `[opcode:8|A:8|B:8|C:8]` instruction encode/decode +
extension-instruction path for wide constants/long jumps; minimal
disassembler (text dump, not a UI). Gate: encode→decode round-trip test per
opcode; disassembler output matches a fixed expected-string fixture per
opcode.

### §103c `SCRIPT_VM_VERIFIER`

Validate register/constant indices, branch targets, instruction shapes on a
raw bytecode buffer — runs **before** any interpreter exists. Gate: rejects
out-of-range register index, rejects out-of-range constant index, rejects
branch target outside function body, each with a distinct, testable error;
accepts a well-formed fixture program.

### §103d `SCRIPT_VM_INTERPRETER_ARITHMETIC`

Dispatch loop for `LOAD_CONST`/arithmetic (`ADD`/`SUB`/`MUL`/`DIV` on
int/float)/`RETURN` only — no heap, no calls, no control flow yet, runs only
verifier-accepted bytecode (§103c). Gate: hand-assembled program for a fixed
arithmetic expression produces the correct `Value`; perf smoke measuring
dispatch overhead per instruction on a fixed-iteration-count tight loop —
this is the Phase 1 performance baseline, document the honest measured
number the same way §97a/§101 did, no unverified claims.

### §103e `SCRIPT_VM_CONTROL_FLOW`

Conditional/unconditional jumps, comparison ops, backward branches (loops).
Gate: a hand-assembled sum-1-to-N loop program produces the correct result
for a fixed N; verifier (§103c) rejects a jump target outside bounds
introduced by this opcode set specifically.

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
