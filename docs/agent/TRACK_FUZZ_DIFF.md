# Track: Fuzz differential — mlcc vs Ruby checker (Phase 1 §3)

Parent: [../PLAN.md](../PLAN.md) §Phase 1 §3; previous: [TRACK_DIAGNOSTICS2.md](TRACK_DIAGNOSTICS2.md) (**closed**, `d055c49`), [TRACK_PERFORMANCE.md](TRACK_PERFORMANCE.md) (deferred differential)

## Status: **active** (step 4 pending)

**Goal:** same `.mlc` input through mlcc `--check-only` and Ruby ModularCompiler checker path; no crash; exit-code parity on corpus + random seeds.

**Constraints:**
- One concern per sub-step; no `compiler/` + `lib/mlc/` in one commit.
- Self-host gate when `compiler/**` touched: `build_bin.sh` + diff empty.
- Shell differential gate must stay fast (<60s in `build_tests.sh`).

## Verify gate (every step)

```
bundle exec rake test_compiler_mlc   # 706 pass (baseline post-DIAGNOSTICS2)
compiler/build.sh                    # when compiler/** touched
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

Differential gate (when `compiler/tests/fuzz/**` touched):

```
compiler/tests/fuzz/run_fuzz_differential.sh compiler/out/mlcc
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | Negative corpus exit-code parity — mlcc `--check-only` vs Ruby checker on 16 files | done (`0654dda`) |
| 2 | Random program seeds — both backends no-crash; exit 0/1 match (8 seeds) | done (`a23ba55`) |
| 3 | Error-count parity on checker-negative subset (mlcc vs Ruby) | pending |
| 4 | `run_fuzz_differential.sh` + hook in `build_tests.sh` | pending |
| 5 | In-process differential smoke in `test_fuzz.mlc`; close track | pending |

## Step 2 detail (done)

- `run_fuzz_differential.sh`: seeds 0–7 via same generator as `run_fuzz_smoke.sh`; strict exit parity, no crash.
- All 8 seeds exit 0 on both backends.

## Step 1 detail (done)

- `compiler/tests/fuzz/run_fuzz_differential.sh` + `scripts/fuzz_ruby_checker_exit.rb` (parse + transform, no codegen).
- 16 files: 10 strict parity, 6 known parser/lexer divergences in `negative_corpus_known_divergences.txt`.
- No codegen diff — exit code only.

## Deferred (not in this track)

- Parser `ref mut` — separate branch.
- Full AST/codegen diff mlcc vs Ruby — out of scope.
- `--emit=mir` artifact — RESEARCH backlog.
- Bootstrap (`MLCC_BOOTSTRAP=1`) — Phase 4 track.

## Next step (Driver)

**STEP=3** — error-count parity on checker-negative subset (mlcc vs Ruby).
