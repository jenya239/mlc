# Track: Fuzz differential — mlcc vs Ruby checker (Phase 1 §3)

Parent: [../PLAN.md](../PLAN.md) §Phase 1 §3; previous: [TRACK_DIAGNOSTICS2.md](TRACK_DIAGNOSTICS2.md) (**closed**, `d055c49`), [TRACK_PERFORMANCE.md](TRACK_PERFORMANCE.md) (deferred differential)

## Status: **active** (step 1 pending)

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
| 1 | Negative corpus exit-code parity — mlcc `--check-only` vs Ruby checker on 16 files | pending |
| 2 | Random program seeds — both backends no-crash; exit 0/1 match (8 seeds) | pending |
| 3 | Error-count parity on checker-negative subset (mlcc vs Ruby) | pending |
| 4 | `run_fuzz_differential.sh` + hook in `build_tests.sh` | pending |
| 5 | In-process differential smoke in `test_fuzz.mlc`; close track | pending |

## Step 1 detail

- Script: `compiler/tests/fuzz/run_fuzz_differential.sh` (skeleton) or Ruby helper under `scripts/`.
- For each file in `negative_corpus/`: mlcc `--check-only` exit ∈ {0,1}; Ruby checker exit ∈ {0,1}; record mismatches.
- No codegen diff yet — exit code only.

## Deferred (not in this track)

- Parser `ref mut` — separate branch.
- Full AST/codegen diff mlcc vs Ruby — out of scope.
- `--emit=mir` artifact — RESEARCH backlog.
- Bootstrap (`MLCC_BOOTSTRAP=1`) — Phase 4 track.

## Next step (Driver)

**STEP=1** — negative corpus exit-code parity mlcc vs Ruby.
