# Track: MIR VM � full plan (self-host path)

Parent: [TRACK_MIR_VM.md](../archive/tracks/TRACK_MIR_VM.md) (MVP closed); [TRACK_MIR.md](../archive/tracks/TRACK_MIR.md)

**Status:** open, Epic 0 A–C **done**; Epic 1 STEP=1–4 **done**; Epic 2 STEP=5 **done**; STEP=6 next

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
| 6 | Mutual recursion, default params desugared | smoke |
| 7 | `?` / Result propagation in MIR + VM | smoke |


### Lambda calling convention (STEP=5)

- Immediate call `(params) => body(args)`: inline at call site (bind params → locals, evaluate body).
- Bound `const name = (params) => body` then `name(args)`: store `MirLambdaBinding` in lower state (not a `VmValue`); call site inlines the same way.
- Captures: free names resolve through `name_environment` at inline time (lexical, no closure heap object).
- Out of scope this step: first-class lambda values, returning/storing lambdas, higher-order without inline.

### Epic 3 � Collections & strings (STEP 8�9)

| Step | Deliverable | Gate |
|------|-------------|------|
| 8 | Array/map/string parity with C++ on corpus file | diff |
| 9 | Iteration, methods beyond push/get/set/length | smoke |

### Epic 4 � Module graph (STEP 10�12)

| Step | Deliverable | Gate |
|------|-------------|------|
| 10 | Single-file `--run` (current) stable | Tier B |
| 11 | Multi-module `--run`: import merge ? one MirProgram | vm on 2-file fixture |
| 12 | `--run` on `misc/examples/*` subset (automated list) | script gate |

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

**Model:** direct MIR struct interpreter (no bytecode), same algorithmic complexity as emitted C++.

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
