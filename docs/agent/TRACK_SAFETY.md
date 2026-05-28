# Track: Safety — crash-safety and fuzz hardening

Parent: [../PLAN.md](../PLAN.md) §Phase 1 §4; previous: [TRACK_PHASE1.md](TRACK_PHASE1.md) (**closed**, step 8 `fd42eab`)

## Status: in progress (step 4 pending)

**Goal:** mlcc never core-dumps on bad input; fuzz coverage beyond smoke skeleton.

**Constraints:**
- One concern per sub-step; no `compiler/` + `lib/mlc/` in one commit.
- Self-host gate when `compiler/**` touched: `build_bin.sh` + `runtime/src/core/profile.cpp`.

## Verify gate (every step)

```
bundle exec rake test_compiler_mlc   # 536 pass (baseline post-PHASE1)
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | Negative corpus — static invalid `.mlc` files + shell gate (exit 0/1 only) | done (`65fe570`) |
| 2 | Random-byte smoke — binary garbage inputs via `run_fuzz_smoke.sh` | done (`44da689`) |
| 3 | Expand `fuzz/random_program.mlc` (if/let/match variants; sync shell generator) | done (`HASH`) |
| 4 | Repro + fix in-process pipeline heap crash (`free(): invalid size` in run_tests loop) | pending |

## Step 1 detail

- Dir: `compiler/tests/fuzz/negative_corpus/*.mlc` — lex/parse/check failures, no valid programs.
- Extend or add script under `compiler/tests/fuzz/`; hook from `build_tests.sh` after existing fuzz smoke.
- Accept mlcc exit 0 or 1; fail on signal or exit > 1.

## Step 4 detail (from PHASE1 step 8)

- `test_fuzz.mlc` in-process loop calling `run_modular_compiler_pipeline` ~32× aborted run_tests.
- Shell `run_fuzz_smoke.sh` on same programs is fine — isolate heap bug to embedded pipeline path.

## Deferred (not in this track)

- Differential fuzz mlcc vs Ruby — RESEARCH.md; needs harness.
- `treereduce` ICE minimizer — later.
- Performance track — after SAFETY closed ([CONTINUITY.md](CONTINUITY.md)).

## Next step (Driver)

**STEP=4** — repro + fix in-process pipeline heap crash.
