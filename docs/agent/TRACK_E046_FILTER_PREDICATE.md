# Track: E046 array filter predicate diagnostic

Parent: [../PLAN.md](../PLAN.md) §Phase 1 stabilization; follows [TRACK_VARIANT_CTOR_BRACE.md](TRACK_VARIANT_CTOR_BRACE.md) (**closed**)

## Status: **closed** (STEP=5, 2026-05-19)

**STEP=5 note (2026-05-19 Driver):** Track closed. Gate: build_tests **999/0**; parity **2/2**; **diff_exit=0**; mlcc **1.88s** / mlcc2 **1.43s**. Change: `infer_lambda_context.mlc` inline `lambda_environment.set` in fold. Uncommitted.

**After STEP=4:** **diff_exit=0**; main_check=**0**; mlcc **1.88s** / mlcc2 **1.43s** (g++ **91s**).

**Scope:** `infer_array_hof_method_call` / `append_predicate_bool_diag` в `compiler/checker/infer/infer_array_method.mlc`; при необходимости `infer_lambda_context.mlc` / `function_return_type` для unary lambda `x => x`.

### Out of scope

- `registry.mlc` COW `.push` (10) — perf track
- parser `ref mut` — TRACK_PLAN step 15
- E064 Option filter (test exists; verify green, fix only if regressed)

## Verify gate

```
compiler/tests/build_tests.sh          # target: 999/0 or document sole remaining fail
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
| 1 | Reproduce; trace why E046 not emitted for `filter(x => x)` | **done** |
| 2 | Fix predicate return-type check (lambda / HOF path) | **done** |
| 3 | `build_tests` green; parity + build.sh | **done** |
| 4 | Full self-host verify gate | **done** |
| 5 | Close track | **done** |

### Step 1 sub-steps

1. Run failing test; compare E047 (green) vs E046 path in `infer_array_method.mlc`
2. Inspect `infer_expr_maybe_lambda` + `function_return_type` for `x => x` on `[i32]`
3. Document root cause in inventory below; gate skip ok

### Step 2 sub-steps

1. Emit E046 when filter/any/all/find predicate return is known non-bool
2. Run `build_tests` (E046 case)

### Step 3 sub-steps

1. Full `build_tests.sh`; parity + `compiler/build.sh`

### Step 4 sub-steps

1. Self-host diff gate per TRACK

### Step 5 sub-steps

1. Mark track closed; update TRACK_PLAN

---

## Inventory (STEP=1)

### Reproduce

| Check | Result |
|-------|--------|
| `build_tests` | **998/1** — only E046 fail |
| `check('fn f(a: [i32]) -> [i32] = a.filter(x => x)')` | **Ok** (0 errors) |
| `mlcc --check-only` same source | **exit 0** |
| E047 fold `(s,x)=>"bad"` | **pass** |
| E064 Option `filter(x=>x+1)` | **pass** |

### Call chain

```
check → infer_expr → infer_expr_method
  → infer_array_hof_method_call (filter on TArray)
  → infer_expr_maybe_lambda → infer_lambda_with_param_types
  → append_predicate_bool_diag (skipped)
```

### Root cause

| File | Issue |
|------|-------|
| `infer_lambda_context.mlc` L8–18 | `infer_lambda_parameter_type_fold_step` takes `lambda_env` **by value**; `lambda_env.set` mutates copy only |
| `infer_lambda_context.mlc` L29–33 | fold updates `built_parameter_types`, not `lambda_env`; body `check_context_new(lambda_env)` lacks param bindings |
| `infer_array_method.mlc` L67–77 | `append_predicate_bool_diag` guards `!type_is_unknown(return_type)` — identity `x => x` body → `TUnknown` → **no E046** |

**Why E047/E064 pass:** literal `"bad"` / operator `x+1` / `x>0` infer **known** non-bool return without param env (`arithmetic_binary_result_type` defaults unknown→i32; comparisons→bool).

### STEP=2 change

`infer_lambda_context.mlc` — removed by-value `infer_lambda_parameter_type_fold_step`; inline `lambda_environment.set` in fold closure (same pattern as `infer_expr_lambda`). Param types now reach lambda body → E046 fires for `a.filter(x => x)`.
