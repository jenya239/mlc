# Track: Performance — mlcc compile-time regression guard + hotspots

Parent: [../PLAN.md](../PLAN.md) §1 «Производительность»; previous: optimization batch (`419de62`), [TRACK_SAFETY.md](TRACK_SAFETY.md) (**closed**, `32f8335`)

## Status: in progress (step 1 pending)

**Goal:** keep full `compiler/main.mlc` translation within baseline (wall ms + scaling exponent); remove remaining super-linear / copy-heavy paths.

**Constraints:**
- One concern per sub-step; no `compiler/` + `lib/mlc/` in one commit.
- Self-host gate when `compiler/**` touched: `build_bin.sh` + `runtime/src/core/profile.cpp`.
- Measure before/after each optimization step: `benchmarks/profile/run_profile.sh` or `compare_baseline.sh`.

## Verify gate (every step)

```
bundle exec rake test_compiler_mlc   # 607 pass (baseline post-SAFETY)
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

Perf (when `compiler/**` or baseline touched):

```
benchmarks/profile/record_baseline.sh
benchmarks/profile/compare_baseline.sh benchmarks/profile/baseline_reference.txt
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | Post-SAFETY baseline — record profile + scaling; commit `benchmarks/profile/baseline_reference.txt` | pending |
| 2 | CI perf gate — `compare_baseline.sh` fails on regression (not `continue-on-error`) | pending |
| 3 | `build_registry` COW audit — eliminate Map-copy fold on large accumulators (PLAN §1) | pending |
| 4 | CodegenContext hot path — reduce full struct copies in stmt/expr visitor entry | pending |
| 5 | Re-baseline + document wall ms / top phases in this file | pending |

## Step 1 detail

- Fresh mlcc after SAFETY (`32f8335`); `compiler/build.sh` first.
- Run `benchmarks/profile/record_baseline.sh 5`; copy output to `benchmarks/profile/baseline_reference.txt` (committed).
- Note median `total_ms` and top 3 phases in commit message or TRACK step 5.
- No compiler logic changes in step 1.

## Step 2 detail

- File: `.github/workflows/ci.yml` — profile step must fail build on `compare_baseline.sh` failure.
- Default baseline path: `benchmarks/profile/baseline_reference.txt` (update `compare_baseline.sh` if needed).
- Factor: keep default `1.15`; scaling soft exponent `1.35`.

## Step 3 detail

- Start: `compiler/checker/registry.mlc` and grep for `fold` + `Map` copy patterns in checker/codegen.
- Pattern: single `ref mut` accumulator (see PLAN §1 `build_registry` note).
- One module or one call-site cluster per commit if split needed.

## Step 4 detail

- Profile after step 3; target dominant `codegen` sub-phase if still >35% wall.
- Scope: one entry path (`stmt_eval`, `expr_visitor_cpp`, or `context.mlc` update helpers) — no API churn across all codegen modules in one step.

## Deferred (not in this track)

- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15, separate branch.
- MLC IR / LLVM — PLAN §2, post-stabilization.
- Differential fuzz mlcc vs Ruby — [RESEARCH.md](RESEARCH.md).
- `--emit=mir`-style artifact for fuzz — RESEARCH backlog.

## Next step (Driver)

**STEP=1** — post-SAFETY baseline file + verify compare passes.
