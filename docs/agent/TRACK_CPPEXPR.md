# Track: Phase 2 — CppExpr backend

Parent: [../PLAN.md](../PLAN.md) §Phase 2 (C++ AST backend)

Previous track: [TRACK_PLAN.md](TRACK_PLAN.md) (folder restructure, steps 1–23 — **closed**)

## Status: in progress (step 17 pending)

**Goal:** replace string concatenation in `codegen/` with `CppExpr` / `CppStmt` / `CppDecl` built in `compiler/cpp/`, emitted via `printer.mlc`. Incremental; no regressions.

**Strategy (PLAN §Phase 2):**
1. Extend `compiler/cpp/ast.mlc` to full emission AST.
2. Add `compiler/cpp/printer.mlc` (deterministic pretty-printer).
3. Migrate `codegen/` leaves → expressions → statements → declarations.
4. Dual-backend diff (string vs printer) before cutover.
5. Remove string-based codegen.

**Constraints:**
- One layer per sub-step; do not bundle `compiler/` + `lib/mlc/` in one step.
- Keep unique module basenames (`path_to_module_base` uses filename only).
- Self-host link: **g++** + `runtime/src/core/profile.cpp` (not `build_bin.sh`/clang++ until codegen emits no GCC-only `auto` in template args).

## Verify gate (every step)

```
bundle exec rake test_compiler_mlc   # 521 pass
compiler/build.sh
# self-host (fresh p1 → g++ mlcc2 → p2):
compiler/out/mlcc -o $TMP/mlc_p1 compiler/main.mlc
g++ -std=c++20 -O2 -I $TMP/mlc_p1 -I runtime/include $TMP/mlc_p1/*.cpp \
  runtime/src/io/io.cpp runtime/src/core/string.cpp runtime/src/core/profile.cpp \
  -o $TMP/mlcc2
$TMP/mlcc2 -o $TMP/mlc_p2 compiler/main.mlc
diff -rq $TMP/mlc_p1 $TMP/mlc_p2   # empty until backend cutover (step 20+)
```

Steps 1–8 (ast + printer only): output unchanged → self-host diff must stay empty.

Steps 9–19 (dual path): string backend still default → self-host diff empty; add/extend unit tests.

Steps 20–21 (cutover + cleanup): self-host diff empty on new backend.

---

| Step | Item | Status |
|------|------|--------|
| 1 | `CppType` in `compiler/cpp/ast.mlc` — `CppTypeName`, `CppTypeTemplate`, `CppTypeRef` | done (`f9340f8`) |
| 2 | Extend `CppExpr` — `CppBool`, `CppTernary`, `CppCast`, `CppInitList` | done (`734d6b6`) |
| 3 | Extend `CppExpr` — `CppLambda`, `CppStdVisit`; `CppMember` pointer flag | done (`aeac4f9`) |
| 4 | Extend `CppStmt`/`CppDecl` — `CppConstDecl`, `CppFor`, `CppNamespace`, `CppVariant`, `CppFile` | done (`547724b`) |
| 5 | `compiler/cpp/printer.mlc` scaffold — print literals + ident | done (`3fdf646`) |
| 6 | Printer — all `CppExpr` variants | done |
| 7 | Printer — `CppStmt`, `CppDecl`, `CppFile` | done |
| 8 | `compiler/tests/test_cpp_printer.mlc` — golden / roundtrip tests | done |
| 9 | `compiler/cpp/emit_helpers.mlc` — builders for literal/ident nodes (no codegen wire yet) | done |
| 10 | Dual emit in `codegen/expr/literals.mlc` — CppExpr path + printer (string path unchanged default) | done |
| 11 | Dual emit in `codegen/expr/identifiers.mlc` | done |
| 12 | `ExprVisitor<CppExpr>` trait + `compiler/expr_visitor_cpp.mlc` dispatch stub | done |
| 13 | Visitor batch 1 — literals, ident, bin/unary → CppExpr | done |
| 14 | Visitor batch 2 — call, method, if, block | done |
| 15 | Visitor batch 3 — match, record, array, lambda | done |
| 16 | Migrate `codegen/stmt/` → `CppStmt` (dual path) | done |
| 17 | Migrate `codegen/decl/` → `CppDecl` (dual path) | pending |
| 18 | `codegen/module.mlc` — optional file emission via printer (behind flag) | pending |
| 19 | Dual-backend diff harness — compare string vs printer on `test_codegen` cases | pending |
| 20 | Default backend → CppExpr/printer in `eval`/`module` | pending |
| 21 | Remove string `codegen/expr/expr.mlc` helpers + `expr_visitor_string.mlc` | pending |

## Step details

### Step 1 — CppType

- File: `compiler/cpp/ast.mlc` only (+ imports in tests if needed).
- Add `CppType` per PLAN §C++ AST; no changes to `codegen/`.
- Existing C++ parse subset (`CppFnProto` string types) stays; new types for emission.

### Steps 2–4 — AST completion

- Align node names/fields with PLAN §C++ AST block.
- `CppMember(Shared<CppExpr>, string, bool)` — third field = pointer (`->`).
- `CppFile { header: [CppDecl], source: [CppDecl] }`.

### Steps 5–8 — Printer

- New module `compiler/cpp/printer.mlc`; deterministic formatting (indent 2 spaces).
- Step 8 test: build AST in MLC, print, compare to expected C++ snippet strings.

### Steps 9–11 — Leaf dual emit

- `emit_helpers.mlc` must not import `codegen/` (avoid cycles).
- Literals/identifiers gain `gen_*_cpp` returning `Shared<CppExpr>`; existing `-> string` APIs unchanged.

### Steps 12–15 — ExprVisitor\<CppExpr\>

- Mirror `expr_visitor_string.mlc` structure; reuse `dispatch_expr`.
- One batch per step; gate after each batch.

### Steps 16–18 — stmt / decl / module

- One subdirectory per step (stmt, then decl, then module).
- Module step: flag `use_cpp_printer: bool` default false.

### Steps 19–21 — Cutover

- Step 19: test helper `assert_cpp_backends_equivalent(program_fragment)`.
- Step 20: flip default; self-host diff must be empty.
- Step 21: delete dead string visitor + unused `expr.mlc` string templates; prune imports.

## Deferred

- Parser `ref mut` — separate branch (TRACK_PLAN step 15).
- Pretty-printer formatting polish beyond deterministic baseline — after step 21.
- `lib/mlc/` Ruby backend parity — not in this track.

## Next step (Driver)

**STEP=17** — migrate `codegen/decl/` → `CppDecl` (dual path).
