# Track: Diagnostics — remaining bare codes (Phase 1 §1 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 1 §1; previous: [TRACK_COVERAGE.md](TRACK_COVERAGE.md) (**closed**, `9bddbb3`), [TRACK_DIAGNOSTICS.md](TRACK_DIAGNOSTICS.md) (**closed**, `f6863fe`)

## Status: **active** (step 3 pending)

**Goal:** replace remaining bare `diagnostic_error(` in checker with `diagnostic_error_with_code`; extend catalog from E035; negative tests assert exact `error[Exxx]:` format where touched.

**Constraints:**
- One concern per sub-step; no `compiler/` + `lib/mlc/` in one commit.
- Self-host gate when `compiler/**` touched: `build_bin.sh` + diff empty.
- Do not rename existing diagnostic messages without updating asserts.

## Verify gate (every step)

```
bundle exec rake test_compiler_mlc   # 702 pass (baseline post-COVERAGE)
compiler/build.sh                    # when compiler/** touched
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | `check.mlc` — default-param + record-field defaults → E036+; catalog smoke | done (`f74e0fa`) |
| 2 | `infer_array_method.mlc` — array HOF diagnostics → codes | pending |
| 3 | `infer_result_option_method.mlc` — Result/Option method diagnostics → codes | pending |
| 4 | `infer.mlc` + `infer_question_expression.mlc` — spread, let-else, private ctor | pending |
| 5 | `derive_validation.mlc` + transform expand modules + audit; catalog count test | pending |

## Step 1 detail

- File: `compiler/checker/check/check.mlc` (+ `diagnostic_codes.mlc` as needed).
- Targets: trailing defaults, generic/extern defaults, destructuring defaults, non-literal defaults, record field default shape errors.
- Each new code: export in catalog; at least one negative test with exact `error[Exxx]:` line if message stable.

## Deferred (not in this track)

- Parser `ref mut` — separate branch.
- E2E program expansion — separate.
- Bootstrap (`MLCC_BOOTSTRAP=1`) — Phase 4 track.
- `lib/mlc/` parity — separate.

## Next step (Driver)

**STEP=2** — infer_array_method.mlc array HOF diagnostics.
