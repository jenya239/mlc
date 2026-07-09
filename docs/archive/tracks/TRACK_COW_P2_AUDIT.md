# Track: COW P2 audit (mlcc2 parity)

Parent: [../PLAN.md](../PLAN.md) §Phase 1 stabilization; follows [TRACK_COW_WHILE_AUDIT.md](TRACK_COW_WHILE_AUDIT.md) (**closed**)

## Status: **closed** (STEP=5, 2026-05-19)

**STEP=5 note (2026-05-19 Driver):** `check.mlc` → `type_alias_cycle_diagnostic_fold_step`+decl fold; `Check_fn_locals_fold_state` for type params + parameters. Inventory 0 `.push` in 6 scope files. Gate: build_tests ok; parity 2/2; main=0; **diff_exit=0**; mlcc p1 **7.01s** / mlcc2 p2 **2.57s**. `registry.mlc` (10) remains out of scope.

**STEP=4 note (2026-05-19 Driver):** `param_destructure_expand.mlc` → `Expand_fn_parameter_destructure_fold_state`+fold; `prelude.concat(inner)`; diagnostic `existing.concat(extra)`; extern scan fold. `transform_decl.mlc` → `transform_decls`/`to_semantic_namespace_aliases`/`transform_load_items` fold+`concat`. Gate: build_tests ok; parity 2/2; main=0; **diff_exit=0**; mlcc p1 **5.38s** / mlcc2 p2 **4.10s**.

**STEP=3 note (2026-05-19 Driver):** `infer_result_option_method.mlc` → `append_argument_type`+`concat` (12 sites). Gate: build_tests 996/3 fail (pre-existing ctor brace +1); parity 2/2; main=0; **diff_exit=0**; mlcc p1 **4.36s** / mlcc2 p2 **3.09s**.

**STEP=2 note (2026-05-19 Driver):** `infer_lambda_context.mlc` → `infer_lambda_parameter_type_fold_step`+fold+`concat`; `infer_array_method.mlc` → `append_argument_type`+`concat` (no `.push`). Gate: build_tests ok; parity 2/2; main=0; **diff_exit=0**; mlcc p1 **2.38s** / mlcc2 p2 **1.79s**.

**STEP=1 note (2026-05-19 Driver):** Grep 6 files; **33** `.push` sites confirmed (counts match draft). Fn refs: `infer_lambda_with_param_types`, `infer_array_hof_method_call`, `infer_result_option_hof_method_call`, `wrap_expression_with_statement_prelude` / `expand_decl_fn_parameter_destructuring_inner` / `expand_extend_methods_parameter_destructuring` / `expand_parameter_destructuring_in_program` / `concatenate_diagnostic_arrays` / `extern_parameter_destructure_errors_for_decl_shared`, `transform_decls` / `to_semantic_namespace_aliases` / `transform_load_items`, `type_alias_cycle_diagnostics` / `check_program_against_full`. Gate skipped (docs only).

**Проблема:** после COW_FOLD (P0) и COW_WHILE (P1) остаются `.push` на `let mut`/`const` `[T]` в ветках и while-index циклах — тот же класс COW/mlcc2 parity. P2 в COW_FOLD: «local branches / low traffic»; после закрытия P1 — следующий stability-приоритет.

**Pattern (fix):** `.concat([item])`, fold state + new record, recursive helper; избегать `.push` на binding из outer scope в callback/while-body.

## Inventory (STEP=1 refresh, 2026-05-19)

| File | Function | Lines | Count | Pattern |
|------|----------|-------|-------|---------|
| `infer/infer_lambda_context.mlc` | `infer_lambda_with_param_types` | — | 0 | **fixed STEP=2** (fold+`concat`) |
| `infer/infer_array_method.mlc` | `infer_array_hof_method_call` | — | 0 | **fixed STEP=2** (`append_argument_type`) |
| `infer/infer_result_option_method.mlc` | `infer_result_option_hof_method_call` | — | 0 | **fixed STEP=3** (`append_argument_type`) |
| `transform/param_destructure_expand.mlc` | all listed fns | — | 0 | **fixed STEP=4** (fold+`concat`) |
| `transform/transform_decl.mlc` | `transform_decls` / aliases / load_items | — | 0 | **fixed STEP=4** (fold+`concat`) |
| `check/check.mlc` | `type_alias_cycle_diagnostics` / `check_program_against_full` | — | 0 | **fixed STEP=5** (fold+`concat`) |

**Total remaining in scope:** 0 `.push` call sites (track inventory complete).

### Out of scope (this track)

- `registry.mlc` (10) — build-time / perf track (COW_WHILE deferral)
- `compiler/frontend/parser/**` — `ref mut` branch (TRACK_PLAN step 15)
- `test_codegen.mlc` ctor brace (2 fails) — separate fix if gate blocks; not COW class

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
| 1 | Refresh inventory: grep P2 files; confirm counts; update table | done |
| 2 | `infer_lambda_context.mlc` + `infer_array_method.mlc` → `concat`/fold | done |
| 3 | `infer_result_option_method.mlc` → `concat`/fold | done |
| 4 | `param_destructure_expand.mlc` + `transform_decl.mlc` → `concat`/fold | done |
| 5 | `check.mlc` survivors + full verify gate; close track | done |

### Step 1 sub-steps

1. `rg '\.push\(' compiler/checker/infer/infer_lambda_context.mlc compiler/checker/infer/infer_array_method.mlc compiler/checker/infer/infer_result_option_method.mlc compiler/checker/transform/param_destructure_expand.mlc compiler/checker/transform/transform_decl.mlc compiler/checker/check/check.mlc`
2. Update inventory table in this file if counts shifted
3. No compiler changes; gate skip ok

### Step 2 sub-steps

1. `infer_lambda_context.mlc`: while+`built_parameter_types.push` → fold+`concat`
2. `infer_array_method.mlc`: branch `argument_types` accumulation without mutating outer `const` arrays
3. Run verify gate

### Step 3 sub-steps

1. `infer_result_option_method.mlc`: all `argument_types.push` branches
2. Run verify gate

### Step 4 sub-steps

1. `param_destructure_expand.mlc`: prelude/combined while loops
2. `transform_decl.mlc`: while-index decl transforms
3. Run verify gate

### Step 5 sub-steps

1. `check.mlc`: `locals.push` / `diagnostics.push` in decl walk
2. Full verify gate; close track
