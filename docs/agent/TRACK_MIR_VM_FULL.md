# Track: MIR VM � full plan (self-host path)

Parent: [TRACK_MIR_VM.md](../archive/tracks/TRACK_MIR_VM.md) (MVP closed); [TRACK_MIR.md](../archive/tracks/TRACK_MIR.md)

**Status:** open, Epic 0–4 **done** (STEP=12 closed Epic 4); Epic 5 **NOT authorized** — next turn is Planner, not STEP=13

**Epic 4 stability correction (2026-07-10):** Epic 4's "single/multi-file
`--run` stable" claim (STEP=10–12) was **false for non-toy programs** until
[TRACK_VM_TRAMPOLINE](../archive/tracks/TRACK_VM_TRAMPOLINE.md) closed —
`vm_run_frames` recursed once per MIR step in host C++ and segfaulted past
~1500–2000 steps. Corpus fixtures were too small to catch it. **Fixed**
2026-07-10 (trampoline host loop; depth gate 100k; self-host diff identical;
`regression_gate` 20/0). Epic 5 remains **NOT authorized**.

**Second correctness bug (2026-07-10, closed):**
[TRACK_VM_BLOCK_ID_COLLISION](../archive/tracks/TRACK_VM_BLOCK_ID_COLLISION.md)
— nested `if`/elif in `mir_lower_return_expression` reused MIR block ids
(`then_block_step.state` instead of `else_block_step.state`); VM hung on the
second elif arm. **Fixed** 2026-07-10 (one-line; classify/deep-nesting gates;
corpus green; self-host diff identical; `regression_gate` 20/0).

**Lowering coverage gaps (2026-07-10, closed):**
[TRACK_VM_LOWERING_GAPS](../archive/tracks/TRACK_VM_LOWERING_GAPS.md) —
unary `!`/`-`/`~`/`+` (`MirRvalueUnary`); mid-block `if` as statement;
`if` as rvalue via shared local + continue. **Fixed** 2026-07-10 (self-host
DIFF identical; `regression_gate` 20/0; VM corpus green).

**HARD STOP GATE (2026-07-09, user decision):** Epic 4 (STEP 10–12) is
authorized to run to completion. **Epic 5 (STEP 13+) is NOT authorized** —
do not start STEP 13 without an explicit new user command in chat. When
STEP=12 is done, next turn is **not** `ROLE=Driver STEP=13 TRACK_MIR_VM_FULL`
— instead `ROLE=Planner` picks the next track from `PLAN.md` priority queue
(this track's own scope estimate for Epic 5 alone is ~80–150 agent-hours,
explicitly out of proportion to its priority — this is not build-speed work,
see `PLAN.md` §2.9 note).

| Step | Item | Status |
|------|------|--------|
| A | Layer split (`eval` / `execute` / `runtime` / `outcome`) | done |
| B | `build_mir_program_from_semantic_items_checked` on `--run` | done |
| C | Variant ctor via MIR metadata | **done** |

**Goal (north star):** `mlcc --run compiler/main.mlc -o /tmp/out` compiles a small program end-to-end without `g++`, with parity vs C++ backend on a growing corpus.

**Explicitly out of scope:** bytecode file format, JIT, GC, replacing C++ release backend.

---

## 1. Target architecture (fix before scale)

MVP mixed eval + dispatch in one file. Full track starts with layer split:

```text
compiler/vm/
  value.mlc          # VmValue, truthiness, as_i32, print repr
  heap.mlc           # optional: boxed variant fields, Shared simulation (later)
  native.mlc         # __mir_* + println; pure fns, no frames
  runtime.mlc        # call dispatch: native | variant_ctor | user_fn
  mir_eval.mlc       # MirOperand / MirRvalue eval (no control flow)
  execute.mlc        # block walk, terminators, calls, trace
  interpreter.mlc    # public: run_mir_program_from_semantic_items
  lower_report.mlc   # surfacing lowering failures (no silent skip)
  error.mlc          # VmRunFailed messages
```

**Rules (unchanged, enforced in review):**

1. VM executes **MIR only** � no SemanticIR shapes in interpreter.
2. Missing MIR for a Semantic construct ? **lower_fn / lower_program**, not VM hack.
3. Variant ctors lowered to `MirCallAssign` with tag metadata � drop `A-Z` heuristic in interpreter.
4. Every lowering skip is logged / fails verify when `--run` requested.

**Acceptance for architecture refactor (STEP A):** same smoke + diff as MVP; `interpreter.mlc` < 80 lines glue.

---

## 2. Fractal breakdown

Each epic ? steps ? gate. Agent picks **one leaf step** per session; no parallel epics.

### Epic 0 � Foundation (STEP A�C)

| Step | Deliverable | Gate |
|------|-------------|------|
| A | Layer split per �1 | dev_gate_fast + vm smoke |
| B | `lower_report`: failed `lower_semantic_function` ? diagnostic, not silent drop | test_lower_fn |
| C | Variant ctor via MIR metadata (`MirCallAssign` + const tag operand or `__mir_variant_new`) | **done** (vm `--run` + dump `__mir_variant_new`; C++ sum visit gap deferred) |

### Epic 1 � Values & ADTs (STEP 1�4)

| Step | Deliverable | Gate |
|------|-------------|------|
| 1 | `VmVariant` typed fields: i32, bool, string (flat, no recursive VmValue in MLC type) | **done** |
| 2 | Full variant match: guards, `PatternOr` expand (done in lower), nested ctor patterns | **done** |
| 3 | Records: literal, field access, record patterns in match | **done** |
| 4 | `Option`/`Result`/`Shared` as variants in VM corpus | **done** |

### Epic 2 � Control & functions (STEP 5�7)

| Step | Deliverable | Gate |
|------|-------------|------|
| 5 | Closures / lambda calls (already partial) — formalize calling convention | **done** |
| 6 | Mutual recursion, default params desugared | **done** |
| 7 | `?` / Result propagation in MIR + VM | **done** |


### Lambda calling convention (STEP=5)

- Immediate call `(params) => body(args)`: inline at call site (bind params → locals, evaluate body).
- Bound `const name = (params) => body` then `name(args)`: store `MirLambdaBinding` in lower state (not a `VmValue`); call site inlines the same way.
- Captures: free names resolve through `name_environment` at inline time (lexical, no closure heap object).
- Out of scope this step: first-class lambda values, returning/storing lambdas, higher-order without inline.


### Default params / mutual recursion (STEP=6)

- Mutual recursion: already works via `MirCallAssign` + user frames (no forward-decl needed in MIR).
- Defaults: `MirParam.default_value` (literal MVP); `vm_bind_call_arguments` fills omitted trailing args. C++ still uses header defaults.


### Question operator / Result propagation (STEP=7)

- `expr?` in rvalue/let: CondJump on `__mir_result_is_err`; Err → `MirReturn` whole Result; Ok → `__mir_result_ok_value` into local.
- Natives: `__mir_result_is_err`, `__mir_result_ok_value` (tag `Err`/`Ok` on `VmVariant`).
- Diff harness: `vm_question*.mlc` are VM-only (C++ emit of `Result<>` lacks `mlc::result::` qualify — pre-existing).

### Epic 3 � Collections & strings (STEP 8�9)

| Step | Deliverable | Gate |
|------|-------------|------|
| 8 | Array/map/string parity with C++ on corpus file | **done** |
| 9 | Iteration, methods beyond push/get/set/length | **done** |


### Collections corpus (STEP=8)

- Diff corpus: `vm_collections_corpus` (array index/push + typed `Map` get + string concat/length), plus `vm_map` / `vm_string` / `vm_array_index`.
- C++ gaps deferred: untyped `Map.new()` → `auto table = {}`; `Map.length()` not on `mlc::HashMap` — use typed Map + get only in diff fixtures.


### Iteration / extra methods (STEP=9)

- `for x in array`: index walk via `__mir_length` / `__mir_array_get`.
- `array.pop()`: return last element + `__mir_array_pop` (VM-only in diff; C++ uses `pop_back`).
- `string.contains(needle)`: `__mir_string_contains`.

### Epic 4 � Module graph (STEP 10�12)

| Step | Deliverable | Gate |
|------|-------------|------|
| 10 | Single-file `--run` (current) stable | **done** (`run_single_file_vm_gate.sh`: 18 `--run` + diff 18/0; Tier B self-host p1≡p2) |
| 11 | Multi-module `--run`: import merge → one MirProgram | **done** (`vm_multi` + `run_multi_module_vm_gate.sh`; unwrap `SemanticDeclarationExported` in MIR lower) |
| 12 | `--run` on `misc/examples/*` subset (automated list) | **done** (`run_examples_vm_gate.sh` 28 programs; Epic 4 closed — **STOP, do not open Epic 5**) |

### Single-file `--run` stable (STEP=10)

- Gate: `compiler/tests/run_single_file_vm_gate.sh` over `vm_cpp_diff_programs.txt`.
- Verify: 18 programs `--run` (no `error:`); `run_vm_cpp_exit_diff` 18/0; `mlcc`→`mlcc2`→`diff -rq` empty.
- Note: Ruby `dev_gate_fast` / `build_tests` still hit pre-existing `MATCH(match)` parse fail — not a STEP=10 blocker.

### Multi-module `--run` (STEP=11)

- Fixture: `misc/examples/vm_multi/{lib,main}.mlc` (`export fn` + import).
- Fix: `mir_lower_append_declaration` lowers `sdecl_inner(...)` so `SemanticDeclarationExported` reaches `lower_semantic_function`.
- Gate: `compiler/tests/run_multi_module_vm_gate.sh` (exit 7).

### Examples subset `--run` (STEP=12)

- Manifest: `compiler/tests/vm_examples_run_programs.txt` (28: all green `vm_*` + `minimal` + `vm_multi`).
- Gate: `compiler/tests/run_examples_vm_gate.sh` (checks `error:` on stdout **and** stderr — `--run` diagnostics go to stdout).
- Excluded: legacy demos (`to_string`, unit `main`, broken imports) — not VM blockers.

### Epic 5 � Compiler-shaped workload (STEP 13�20)

Incremental: run `--run` on increasing **slices** of `compiler/`, not whole tree at once.

| Step | Slice | Gate |
|------|-------|------|
| 13 | `frontend/lexer.mlc` only | parse self |
| 14 | `frontend/parser/*` | parse sample |
| 15 | `checker/registry.mlc` + small infer | check sample |
| 16 | `mir/*` | lower sample |
| 17 | `vm/*` (dogfood) | `--run` vm_hello via vm |
| 18 | `driver/cli.mlc` + pipeline hook | `--check-only` path |
| 19 | Modular compile Ruby path unchanged; VM path for merged single module | integration |
| 20 | `compiler/main.mlc` full `--run` | self-host smoke (no g++) |

**Estimate (agent-hours, cumulative):** Epic 0�3 ~40�60h; Epic 4 ~20�30h; Epic 5 ~80�150h ? **~150�250h** total to STEP 20 if no major design dead-ends.

---

## 3. Agent workflow (autonomous, no rush)

```text
1. Read TRACK_MIR_VM_FULL.md ? pick lowest open STEP with clear gate
2. Implement ONE step; architecture lint + dev_gate_fast
3. Append TURNLOG; update STEP status in this file
4. Stop; next session continues
```

Do not skip Epic 0 even if tempting to add features in monolithic interpreter.

---

## 4. Performance: VM mlcc vs native mlcc

### Measured tight-loop (TRACK_VM_TRAMPOLINE STEP=4, 2026-07-10)

`while i < N; i = i + 1` via `mlcc --run` after trampoline (`ulimit -s=8192`):

| N | VM `--run` | Native run |
|---|------------|------------|
| 1_000 | 0.01 s | ~0.00 s |
| 2_000_000 | 10.72 s | ~0.00 s |

Pre-trampoline: 2e6 did not finish in 15s even with unlimited stack (host recursion). Post-fix: completes; ~1.87e5 iters/s on this microbench.

### Order-of-magnitude (planning)**Model:** direct MIR struct interpreter (no bytecode), same algorithmic complexity as emitted C++.

| Workload | Native `mlcc` (O2) | `--run` VM (expect) |
|----------|-------------------|---------------------|
| `minimal.mlc` | ~1�5 ms | ~20�100 ms |
| Single file ~500 LOC | ~50�200 ms | ~2�15 s |
| Full compile `compiler/main.mlc` module graph | ~1�5 min (incremental C++) | **~30 min � 3 h** (rough) |

**Slowdown factor (conceptual):** **~20�80�** typical, **up to ~100�200�** on dispatch-heavy tight loops (visitor-style match in interpreter).

**Why not faster without JIT/bytecode:**

- Every `MirStmt`/`MirTerminator` ? match dispatch + bounds checks
- `VmValue` as variant ? branch mispredict on every value op
- No inlining, no LLVM opts, no PCH for VM path
- Compiler is allocation- and pointer-chasing-heavy ? interpreter amplifies cache misses

**Practical expectation after full success:**

- **Daily dev:** still use native `mlcc` / `build.sh`
- **`--run`:** CI parity checks, bootstrap experiments, environments without g++
- **Self-host VM mlcc:** proof-of-concept, not performance-competitive

Order-of-magnitude: if native self-host compile is **8 min**, VM self-host might be **~2�8 hours** (very rough; measure at STEP 17�20).

---

## 5. Main conceptual problems

### 5.1 Two backends, one language

CppAST and MIR interpreter must stay semantically aligned. Drift shows up as diff harness failures. **Mitigation:** expand `vm_cpp_diff_programs.txt`; staged lowering tests per construct.

### 5.2 Runtime value model vs MLC ? C++ codegen

Full `VmVariant { fields: [VmValue] }` breaks C++ codegen (recursive variant). MVP uses `integer_fields: [i32]`. **Options:** flat tagged union per field slot; heap indirection module; or lower variants to MIR records only. Must decide in Epic 1 before scaling.

### 5.3 Lowering surface area

Compiler uses most of SemanticIR. Each construct needs MIR lowering before VM sees it. **This is the long pole**, not interpreter loop speed.

### 5.4 Silent lowering failure

`mir_lower_append_function` currently drops failed fns. Unacceptable for self-host. Epic 0 STEP B.

### 5.5 Module system

Native build: modular compile, many `.cpp`. VM: one MirProgram or multi-module loader. Epic 4 is architectural fork � needs design before Epic 5.

### 5.6 Memory & `Shared<T>`

Interpreter copies `[VmValue]` and maps; no refcount/GC. Compiler relies on `Shared` heavily. Either simulate refcount in VM or restrict `--run` to value-safe subset until Epic 1+ proves otherwise.

### 5.7 Concurrency

spawn/channel/mutex in language; runtime C++ only today. VM path: **stub or exclude** for self-host (compile flags / checker gate for `--run` mode).

### 5.8 Verification chicken-and-egg

Proving VM self-host correct requires trusted native mlcc to build test oracle. VM self-host is consistency check, not sole root of trust.

### 5.9 Maintenance cost

Every new language feature ? MIR lower + VM native + often C++ codegen. Three places unless MIR?Cpp subsumes release path only.

---

## 6. Success criteria (STEP 20)

```bash
# No g++ on PATH
mlcc --run compiler/main.mlc -o /tmp/vm_out --check-only some_fixture.mlc
echo $?  # 0
```

Plus: diff corpus green; lowering never silent on `--run`; architecture layers per �1.

---

## 7. Per-turn template

```text
ROLE=Driver STEP=<letter|number> TRACK_MIR_VM_FULL
epic: <0�5>
done: <files>
verify: <gate from table>
next: <following leaf step>
```
