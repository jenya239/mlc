# Track: TypeRegistry split + HOF helpers (Phase 2.6 �6 + cleanup)

Parent: [../PLAN.md](../PLAN.md) �Phase 2.6; previous: [TRACK_VISITOR_PATTERN.md](TRACK_VISITOR_PATTERN.md)

## Status: **closed** (`20f9d45` — steps 1–10; step 11 audit 2026-06-04)

**STEP=11 note (2026-06-04 Driver):** Audit steps 1–10 in HEAD; committed parser steps 9–10 (`20f9d45`). Gate: **940**/0; build.sh OK. Track closed.

**STEP=10 note (2026-06-04 Driver):** `parser/expect_close.mlc` — `parser_expect_rbrace/rparen/rbracket`; deduped exprs.mlc + decls.mlc. Gate: **940**/0; build.sh OK.

**STEP=9 note (2026-06-04 Driver):** `parser/comma_separated.mlc` — shared suffix helpers with `parse_one` callbacks; migrated types/decls/exprs (4 duplicate loops removed). Gate: **940**/0; build.sh OK.

**STEP=8 note (2026-06-04 Driver):** `checker/check/` — `.any`/`.all`/`.flat_map` in `call_argument_unify`, `derive_validation`, `check.mlc` (`type_is_checkable`). Gate: **940**/0; build.sh OK; commit `8aee4a9`.

**STEP=7 note (2026-06-04 Driver):** `checker/hof_method_spec.mlc` — `is_result_option_hof_method`, `is_array_hof_method_on_receiver`, `hof_call_result_type`; infer + transform `ExprMethod`. Gate: **940**/0; build.sh OK; commit `9220227`.

**STEP=6 note (2026-05-19 Driver):** `checker/substitution.mlc` — `substitution_from_generic`; infer_match, transform match, let_pattern_infer. Gate: **940**/0; build.sh OK; commit `ad4cdf7`.

**STEP=5 note (2026-05-19 Driver):** `checker/named_args.mlc` — shared reorder; infer E034/E035, transform silent. Gate: **940**/0; build.sh OK; commit `e0652e8`.

**STEP=4 note (2026-05-19 Driver):** `codegen/decl/` + `codegen/stmt/` — map/filter/flat_map; helpers in `decl_extend`/`decl.mlc`. Gate: build_tests **940**/0; build.sh OK.

**STEP=3 note (2026-05-19 Driver):** `cpp_printer.mlc` — `print_comma_separated_*` + 9 join sites → `.map`/`.flat_map` + `.join`; helpers `print_capture_entry`, `print_parameter_entry`, `variant_arm_type_strings`. Gate: build_tests **940**/0; build.sh OK.

**STEP=2 note (2026-05-19 Driver):** `checker/` uses `TypeRegistry` extend API only; **0** flat-field (`fn_types`/`ctor_types`/…) access outside `registry.mlc`. Gate: build_tests **940**/0; build.sh OK.

**STEP=1b note (2026-05-19 Driver):** committed `registry.mlc` (`01a04df`).

**STEP=1 note (2026-06-04 Driver):** `FunctionIndex`, `AdtIndex`, `RecordIndex` extracted in `registry.mlc`; `TypeRegistry` composes three indices; extend API unchanged; registration uses `function_index`/`adt_index`/`record_index`. Gate: build_tests **937**/0; build.sh OK; self-host N/A (pre-existing `build_bin`).

**Planner (2026-05-19):** MODULE_TU closed (`df744e5`). Active track; Driver **STEP=1** = commit `registry.mlc` before step 2 migrate.

**Critic audit (2026-06-04):** Step 1 **OK on worktree**; `registry.mlc` **not in HEAD** (`df744e5` has MODULE_TU only). Hold step 2 until step 1 committed.

**Goal:** ??????? god-object `TypeRegistry` (15+ map ? ????? ?????????) ?? ???????????? ? ????????. ?????? ????????????? while-push-join ???????? ????? `.map()/.filter()/.fold()`. ????????? ?????: `named-args reorder`, `generic substitution`, `HOF method spec`.

## Verify gate (every step)

```
compiler/tests/build_tests.sh
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | `checker/registry.mlc` � ???????? `FunctionIndex`, `AdtIndex`, `RecordIndex` ??? ???????????? ? ???????? | done |
| 1b | Commit step 1 WIP (`registry.mlc`) | done (`01a04df`) |
| 2 | Migrate imports/usage in `checker/` | done |
| 3 | `cpp_printer.mlc` — while-push-join → `.map().join` | done |
| 4 | `codegen/decl/` + `codegen/stmt/` � while-push ? `.map()` ??? ???????? | done (`a604a1b`) |
| 5 | `named_args.mlc` — shared reorder for infer and transform | done (`e0652e8`) |
| 6 | `substitution.mlc` — `substitution_from_generic` | done (`ad4cdf7`) |
| 7 | `hof_method_spec` — shared HOF dispatcher for infer + transform | done (`9220227`) |
| 8 | `checker/check/` — `.all()` / `.any()` / `flat_map` | done (`8aee4a9`) |
| 9 | `parser/` � ????? `comma_separated` helper, ?????? 4 ????? | done |
| 10 | `parser/` � ????? `expect_rbrace/rparen/rbracket`, ?????? ????? ? exprs/decls | done |
| 11 | Audit + ??????? ???? | pending |

### Step 7 scope (Driver)

- Add `compiler/checker/hof_method_spec.mlc` (or extend `check/method_types/`): classify method (`array_hof` vs `result_option_hof`), arity, result type — single API for infer and transform.
- Infer: dispatcher entry from `infer.mlc` `ExprMethod`; keep diagnostics in `infer_array_method` / `infer_result_option_method` or fold shared checks.
- Transform: unify `transform_array_hof_method_arguments` and result-option `ExprMethod` branch via same spec.
- Do not bundle parser steps 9–10 in this step.
