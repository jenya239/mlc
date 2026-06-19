# Track: Self-host bootstrap (remove Ruby from build.sh)

Parent: [../PLAN.md](../PLAN.md) §Phase 4; follows [TRACK_PHASE26_REMAINING.md](TRACK_PHASE26_REMAINING.md)

## Status: **open** (STEP=1 pending)

**Depends on:** TRACK_PHASE26_REMAINING closed.

**Baseline:** `MLCC_BOOTSTRAP=1 compiler/build.sh` uses Ruby ModularCompiler today.

**Goal:** `compiler/build.sh` produces `mlcc` via mlcc-only path; Ruby retained only for `rake test_mlc` reference.

## Verify gate

```
compiler/tests/build_tests.sh
bash compiler/tests/run_mlcc2_checker_parity.sh
MLCC_BOOTSTRAP=1 compiler/build.sh
compiler/out/mlcc_bootstrap -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/bootstrap2
.tmp_selfhost/bootstrap2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | Document current bootstrap path; inventory Ruby-only steps in `build.sh` | pending |
| 2 | `build.sh` — default path uses existing `compiler/out/mlcc` when fresh enough | pending |
| 3 | `MLCC_BOOTSTRAP=1` — chain mlcc → g++ → mlcc_bootstrap; CI smoke | pending |
| 4 | Parity: mlcc_bootstrap output ≡ mlcc (diff self-host) | pending |
| 5 | Remove Ruby from default `build.sh`; gate; close track | pending |

### Out of scope

- Deleting `lib/mlc/` (reference compiler)
- WASM / cross-compile
