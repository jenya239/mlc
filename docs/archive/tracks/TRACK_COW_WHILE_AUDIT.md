# Track: COW while/closure audit (mlcc2 parity)

Parent: [../PLAN.md](../PLAN.md) §Phase 1 stabilization; follows [TRACK_COW_FOLD_AUDIT.md](TRACK_COW_FOLD_AUDIT.md) (**closed**)

## Status: **closed** (STEP=5, 2026-05-19)

**STEP=5 note (2026-05-19 Driver):** Final verify gate; inventory 0 `.push` in 6 scope files. Gate: build_tests 997/2 fail (pre-existing ctor brace); parity 2/2; main=0; **diff_exit=0**; mlcc p1 **3.31s** / mlcc2 p2 **2.09s**. P2 survivors (`infer_array_method`, `infer_result_option_method`, `transform_decl`, `registry`) remain out of scope.

**STEP=4 note (2026-05-19 Driver):** `infer_match.mlc` → `pattern_string_literals_flat` fold+`concat`; string-match diagnostics → fold states + `diagnostics_append`; wildcard `last_arm_index` via `while` (avoid `length()` in struct init / C++ narrowing). `derive_validation.mlc` → VarTuple fold + `derive_clause_trait_scan_fold_state`; `derive_clause_diagnostics_after_trait_scan` helper (no nested `else do`). Gate: build_tests 997/2 fail (pre-existing ctor brace); parity 2/2; main=0; **diff_exit=0**; mlcc p1 **1.91s** / mlcc2 p2 **1.71s**.

**STEP=3 note (2026-05-19 Driver):** `named_args.mlc` → `Named_argument_reorder_state`+argument fold+`errors.concat`; slot init via `fold`+`concat`; `names.mlc` `collect_pattern_bindings` → `fold`+`concat` (no `mut accumulator`). Gate: build_tests ok; parity 2/2; main=0; **diff_exit=0**.

**STEP=2 note (2026-05-19 Driver):** `transform_stmts.mlc` → `Transform_stmts_fold_state`+fold+`concat`; `transform.mlc` `transform_call_arguments_using_callee_semantic_type` → fold state; `transform_array_hof_method_arguments` → `semantic_expression_list_singleton`+`concat` (no `[expr]` literal — parses as index). Gate: build_tests ok; parity 2/2; main=0; **diff_exit=0**.

**STEP=1 note (2026-05-19 Driver):** Inventory refresh (grep 6 files). 28 `.push` sites in scope; 1 new site: `infer_match.mlc` `pattern_string_literals_flat` (recursive `mut accumulator`). `transform.mlc` callee-args fn = `transform_call_arguments_using_callee_semantic_type` (not old name). Gate skipped (docs only).

**Проблема:** тот же класс, что COW_FOLD P0: `const`/`let mut` `[T]` + `.push` в `while` или closure на пути, который mlcc2 компилирует иначе чем Ruby-bootstrap mlcc. P0 fold-сайты закрыты; P1 survivors — ниже.

**Pattern (fix):** `.concat([item])`, fold state + new record, или recursive helper; избегать `.push` на binding из outer `const` в callback/while-body.

## Inventory (STEP=1 refresh, 2026-05-19)

| File | Function | Lines | Count | Pattern |
|------|----------|-------|-------|---------|
| `transform/transform_stmts.mlc` | `transform_stmts` | — | 0 | **fixed STEP=2** (fold+`concat`) |
| `transform/transform.mlc` | `transform_call_arguments_using_callee_semantic_type` | — | 0 | **fixed STEP=2** (fold state) |
| `transform/transform.mlc` | `transform_array_hof_method_arguments` | — | 0 | **fixed STEP=2** (`semantic_expression_list_singleton`) |
| `named_args.mlc` | `reorder_named_arguments_to_positional` | — | 0 | **fixed STEP=3** (fold state + `errors.concat`) |
| `names.mlc` | `collect_pattern_bindings` | — | 0 | **fixed STEP=3** (fold + `concat`) |
| `infer/infer_match.mlc` | `pattern_string_literals_flat`, `string_match_*_diagnostic` | — | 0 | **fixed STEP=4** (fold+`concat`/`diagnostics_append`) |
| `check/derive_validation.mlc` | `derive_hash_field_errors_for_variant`, `derive_clause_diagnostics` | — | 0 | **fixed STEP=4** (fold state + helper) |

**Total remaining in scope:** 0 `.push` call sites (track inventory complete).

### Out of scope (this track)

- `infer_array_method.mlc` / `infer_result_option_method.mlc` — local `argument_types` in branches (inventory P2)
- `transform_decl.mlc` while-index — low traffic; audit if gate regresses
- `registry.mlc` build-time only — separate perf track
- `compiler/frontend/parser/**` — `ref mut` branch (TRACK_PLAN step 15)

## Verify gate

```
compiler/tests/build_tests.sh
bash compiler/tests/run_mlcc2_checker_parity.sh
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 --check-only compiler/main.mlc
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | Refresh inventory: grep `.push` in P1 files; confirm line refs; note any new sites | done |
| 2 | `transform_stmts.mlc` + `transform.mlc` call-args/method-args while loops → `concat`/fold | done |
| 3 | `named_args.mlc` + `names.mlc` `collect_pattern_bindings` → `concat`/fold | done |
| 4 | `infer_match.mlc` + `derive_validation.mlc` merge loops → `concat`/fold | done |
| 5 | Full verify gate; close track | done |

### Step 1 sub-steps

1. `rg '\.push\(' compiler/checker/transform/transform_stmts.mlc compiler/checker/transform/transform.mlc compiler/checker/named_args.mlc compiler/checker/names.mlc compiler/checker/infer/infer_match.mlc compiler/checker/check/derive_validation.mlc`
2. Update inventory table in this file if counts shifted
3. No compiler changes; gate skip ok

### Step 2 sub-steps

1. `transform_stmts.mlc`: replace while+`typed_statements.push` with fold+`concat` or indexed concat helper
2. `transform.mlc`: `transform_call_arguments_using_callee_semantic_type` + `transform_array_hof_method_arguments`
3. Run verify gate

### Step 3 sub-steps

1. `named_args.mlc`: slot/errors accumulation without mutating `const` arrays in while
2. `names.mlc`: `collect_pattern_bindings` recursive accumulator → `concat`
3. Run verify gate

### Step 4 sub-steps

1. `infer_match.mlc`: string-match diagnostic while+`errors.push`
2. `derive_validation.mlc`: merge while+`collected.push`
3. Run verify gate
