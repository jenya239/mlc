# Track: COW fold audit (mlcc2 parity)

Parent: [../PLAN.md](../PLAN.md) §Phase 1 stabilization; follows [TRACK_MLCC2_CHECKER_PARITY.md](TRACK_MLCC2_CHECKER_PARITY.md) (**closed**)

## Status: **closed** (2026-05-19 Driver STEP=5 — verify gate green)

**STEP=5 note (2026-05-19 Driver):** Idempotent gate: build_tests ok; parity 2/2; main=0; **diff_exit=0**. P0 fold+`.push` sites from inventory fixed (steps 2–4). **P1 deferred** (while/closure `.push`: `transform_stmts`, `transform.mlc` call-args, `named_args`, `collect_pattern_bindings`, `infer_match`, `derive_validation`) — separate track if mlcc2 repro.

**STEP=4 note (2026-05-19 Driver):** `infer.mlc` tuple+spread fold → `concat`; `transform.mlc` lambda param fold → `concat`; `check.mlc` diagnostic folds (`param_default`, literal record, generic record, `where`) → `concat`; `trait_param_expand.mlc` conflict walk → `Trait_name_conflict_fold_state`+fold. Parse: nested `else do` in standalone fn broke Ruby parser — inline fold `do/end` like HEAD; closing quote via `` `"` `` not `'\"'`. Gate: build_tests ok; parity 2/2; main=0; **diff_exit=0**.

**STEP=3 note (2026-06-19 Driver):** `infer_call.mlc` `accumulate_inference_for_one_call_argument` — `inferred_argument_types_list.concat` not `.push`. Gate: parity 2/2; main=0; **diff_exit=0**.

**STEP=2 note (2026-06-19 Driver):** `partial_application_desugar.mlc` — `statements_block`/`match_arms_list` fold+`concat`; `partial_allocate_parameter_names` recursive loop (`do/end` required — bare `if/else` tail-call broke mlcc codegen). Gate: parity 2/2; main=0; **diff_exit=0**.

**Проблема:** mlcc (Ruby-codegen `compiler/out`) и mlcc2 (self-hosted p1) расходятся, когда аккумулятор в `.fold` мутируется через `.push` или `let mut` + reassignment на COW `[T]` / `Map`. Ruby codegen иногда спасает через `[&]` capture; mlcc codegen — нет.

**Pattern (fix):** thread via fold state + `.concat` / new record; no `.push` on binding captured from outer `const` in fold callback.

## Inventory (STEP=1, 2026-06-19)

### Already fixed (MLCC2_CHECKER_PARITY + record_lit_merge)

| File | Site | Fix |
|------|------|-----|
| `names.mlc` | `check_names_statements` fold scope | `scope.concat` / `scope_with_bindings` |
| `check_mutations.mlc` | mutable scope in stmt fold | `mutable_scope_after_*` + `concat` |
| `record_lit_merge.mlc` | while+reassign loops | fold+`concat` helpers |
| `partial_application_desugar.mlc` | record lit parts, field/expr sequences | `RecordLitPartsFoldState` + `concat` |

### P0 — `fold` callback `.push` then return same array in state (mlcc2-high)

| File | Function / lines | Field pushed |
|------|------------------|--------------|
| `partial_application_desugar.mlc` | `partial_application_desugar_statements_block` ~289 | `accumulated_statements` |
| `partial_application_desugar.mlc` | `partial_application_desugar_match_arms_list` ~351 | `match_arms` |
| `infer_call.mlc` | `accumulate_inference_for_one_call_argument` ~27 | `inferred_argument_types_list` |
| `infer/infer.mlc` | `infer_tuple_literal_element_fold_step` ~281 | `member_types` |
| `infer/infer.mlc` | `accumulate_record_literal_spread_inference_for_literal_part` ~560 | `spread_inferred_types_in_visit_order` |
| `transform/transform.mlc` | `transform_lambda_parameter_types_environment_fold_step` ~345 | `parameter_type_vector` |
| `check/check.mlc` | `accumulate_diagnostics_when_record_fields_have_defaults...` fold ~56 | diagnostics list |
| `check/check.mlc` | `accumulate_diagnostics_after_literal_record_default_checks` fold ~84,99 | `diagnostics_after_field` |
| `check/check.mlc` | `parameters.fold` default checks ~177–200 | `diagnostics_after_parameter` |
| `check/check.mlc` | `where_entries.fold` ~228 | `diagnostics_accumulated_so_far` |

### P1 — `const`/`let mut` array + `.push` in `while` or closure (not fold; same COW class)

| File | Function / lines | Notes |
|------|------------------|-------|
| `partial_application_desugar.mlc` | `partial_allocate_parameter_names` ~61 | while+`names.push` |
| `transform/transform_stmts.mlc` | `transform_stmts` ~16–64 | while+`typed_statements.push` |
| `transform/transform.mlc` | `transform_call_arguments_with_optional_types` ~231–247 | while+`transformed_arguments.push` |
| `transform/trait_param_expand.mlc` | `trait_and_type_name_conflict_diagnostics` ~31–63 | `const conflicts` + `.push` in decl walk |
| `named_args.mlc` | `reorder_named_arguments_to_positional` ~39–85 | while+slot/`errors.push` |
| `names.mlc` | `collect_pattern_bindings` ~18–52 | recursive `mut accumulator` + `.push` |
| `infer/infer_match.mlc` | string-match diagnostic helpers ~97–156 | while+`errors.push` |
| `check/derive_validation.mlc` | merge loops ~56–107 | while+`collected.push` |

### P2 — fold OK (Map/set, concat, absorb) or low traffic

| File | Notes |
|------|-------|
| `pattern_env.mlc`, `let_pattern_infer.mlc` | fold state returns new `Map` via `.set` |
| `check/check.mlc` | `global_names_*` folds use `Map.set` |
| `infer/infer.mlc` | stmt-sequence fold uses `type_environment` Map updates |
| `param_destructure_expand.mlc`, `transform_decl.mlc` | while-index loops; audit in STEP=4 if needed |

### Out of scope (STEP=1)

`compiler/frontend/parser/**` — separate `ref mut` branch (TRACK_PLAN step 15). `infer_array_method.mlc` / `infer_result_option_method.mlc` — local `argument_types` in branches, not fold accumulators.

### Fix order (steps 2–4)

1. **STEP=2:** `partial_application_desugar` P0+P1 survivors (block, match arms, allocate names)
2. **STEP=3:** `infer_call.mlc` P0
3. **STEP=4:** `infer.mlc` tuple+spread, `transform.mlc` lambda params, `check.mlc` diagnostic folds, `trait_param_expand`; corpus if repro found

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
| 1 | Inventory: `fold` + `.push` / `let mut` reassignment in `compiler/checker/**`; list in track note | done |
| 2 | `partial_application_desugar.mlc`: `statements_block`, `match_arms_list`, `partial_allocate_parameter_names` → fold+`concat` | done |
| 3 | `infer_call.mlc` argument accumulator → `concat` | done |
| 4 | Remaining high-traffic checker folds from step 1; extend `mlcc2_parity_corpus/` if needed | done |
| 5 | Full verify gate; close track | done |
