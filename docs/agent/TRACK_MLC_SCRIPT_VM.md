# Track: MLC Script VM — embeddable dynamic-profile VM, Phase 1

Parent: [../MLC_SCRIPT_VM.md](../MLC_SCRIPT_VM.md), [../PLAN.md](../PLAN.md) §103.
Trigger: пользователь 2026-07-13 — концепт (design doc). **Авторизовано
2026-07-28** (user request: "можно добавить треки по... script-vm") — снят
HARD STOP GATE, Phase 1 (`MLC_SCRIPT_VM.md` §12 фаза 1) разбита на атомарные
под-треки ниже. Эмфаза по требованию пользователя: производительность,
архитектура, тестирование — у каждого под-трека явный gate.

## Status: **open** — §103a Decision next

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

## Design decisions to close at §103a (before any code)

1. `dyn` — новый явный тип на границе native-checker, или только внутри
   `script module`? Черновой ответ в `MLC_SCRIPT_VM.md` §3: явный в native,
   default для неаннотированных в script.
2. `script module`/`native module` — синтаксис уровня модуля или отдельное
   расширение файла/манифест.
3. `Value` ABI: NaN-boxing vs tagged union — какая версия x86-64 первой (ARM64
   — не в scope Phase 1).
4. Verifier обязателен с первого коммита интерпретатора (design doc §10,
   LuaJIT precedent) — §103c идёт **до** §103d, не после.

## Sub-tracks, in order (Phase 1 only — §12 фазы 2-5 остаются design-only)

### §103a `SCRIPT_VM_VALUE_REP`

`struct Value { raw: u64 }` tagged representation — int/float/bool/nil
variants behind a `ValueRep` abstraction (design doc §6), no heap yet. Gate:
round-trip encode/decode unit tests for every variant, no precision loss for
f64, `size_of(Value) == 8` assertion.

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
