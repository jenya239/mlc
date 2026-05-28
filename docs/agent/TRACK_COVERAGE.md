# Track: Test coverage — checker and codegen (Phase 1 §2)

Parent: [../PLAN.md](../PLAN.md) §Phase 1 §2; previous: [TRACK_DIAGNOSTICS.md](TRACK_DIAGNOSTICS.md) (**closed**, `f6863fe`)

## Status: **active** (step 5 pending)

**Goal:** extend `test_checker.mlc` and `test_codegen.mlc` so major `SExpr`/`SStmt` variants have at least one positive test; add targeted negative tests where gaps exist.

**Constraints:**
- One concern per sub-step; no `compiler/` + `lib/mlc/` in one commit.
- Self-host gate when `compiler/**` touched: `build_bin.sh` + diff empty.
- Do not rename existing diagnostic messages without updating asserts.

## Verify gate (every step)

```
bundle exec rake test_compiler_mlc   # 692 pass (baseline post-DIAGNOSTICS)
compiler/build.sh                    # when compiler/** touched
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | Lambda + array literal — positive tests in `test_checker.mlc` (typed/untyped lambda, array lit) | done (`9bf51aa`) |
| 2 | Tuple + record update — positive tests in `test_checker.mlc` | done (`c414120`) |
| 3 | Match + for-in edge — positive tests (guard ok path, nested for) | done (`cbca038`) |
| 4 | Codegen smoke — extend `test_codegen.mlc` for one uncovered expr arm | done (`d15c1a2`) |
| 5 | Codegen stmt — extend `test_codegen.mlc` for one stmt category; document test count | pending |

## Step 1 detail

- File: `compiler/tests/test_checker.mlc` only.
- Add: typed lambda two-param (may exist — verify); untyped lambda closure; array literal `[1, 2]` in fn body.
- Each new case: `assert_eq_int(..., 0)` error count.
- No compiler source changes unless test reveals bug.

## Deferred (not in this track)

- Remaining bare `diagnostic_error(` — future diagnostics track.
- E2E program expansion — separate.
- Parser `ref mut` — separate branch.
- Bootstrap (`MLCC_BOOTSTRAP=1`) — Phase 4 track.

## Next step (Driver)

**STEP=5** — codegen stmt + baseline count.
