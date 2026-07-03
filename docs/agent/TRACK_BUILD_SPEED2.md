# Track: Incremental build correctness (dev-loop speed, phase 2)

Parent: [../PLAN.md](../PLAN.md) §Phase 2.9; previous: [TRACK_BUILD_SPEED.md](TRACK_BUILD_SPEED.md) (**closed**, STEP=7) — this track fixes gaps found in that closed track's incremental cache.

Related rule: [.cursor/rules/mlcc-self-host-verification.mdc](../../.cursor/rules/mlcc-self-host-verification.mdc) — "всё зелёное, а mlcc устарел" недопустим. This track is triggered by a **live reproduction** of exactly that scenario.

## Status: **open** STEP=5 (1 done modified, 2 done, 3 done, 4 declined)

**Priority class: stability** (not pure performance) — steps 1–2 fix silent staleness, not just speed. Steps 3–4 are performance. Order matters: 1 → 2 → 3 → 4 → 5 → 6 → 7.

## Baseline (measured 2026-07-01, 8-core, clang+ccache+lld present, no mold)

- Cold `build_bin.sh` (147 generated TU + 3 runtime TU, `MLCC_OBJ_CLEAN=1`, default `MLCC_PCH=1`, clang+ccache+`-O2`): **564.9s**.
- Same, `MLCC_PCH=0`: **546.8s**; `ccache -s` showed `147/147` misses (first-time content, not a PCH artifact — cache had never seen this exact non-PCH invocation before).
- `ccache -s` delta across a full `MLCC_PCH=1` cold build: **+151 total calls, +0 Cacheable calls** (all uncacheable). `ccache` + clang `-include-pch` does not cache the 147 hot TUs at all in the default config.
- `compute_module_fingerprint()` (`compiler/build.sh:40-42`) = `sha256` of **sorted list of `.mlc` filenames only**. Recomputing it live matched the stored `.mlcc_module_stamp` despite uncommitted content edits in `compiler/codegen/expr/match_gen.mlc` (mtime newer than `compiler/out/mlcc`).
- Regenerating `match_gen.cpp` directly via the current `compiler/out/mlcc -o tmp compiler/main.mlc` produced a file differing from the checked-in `compiler/out/match_gen.cpp` (672 vs 586 lines) — proof the currently built `mlcc` binary does not reflect `match_gen.mlc`'s current content. Running `compiler/build.sh` with no env vars afterward printed `mlcc up to date (skip)` — **no error, no warning**.
- `-ftime-trace` on `transform.cpp` (140K, largest generated TU): `-O2`+PCH = 17.3s wall (`InstantiateFunction` 32.9s cumulative / 5148 instantiations, `Backend` 10.6s, `Optimizer` 7.0s, `Frontend` 6.5s). `-O0 -g`+matching PCH = **9.4s** (−46%; Backend/Optimizer collapse).
- Generated header fan-out: avg 5.75 direct local `#include` per `.hpp`, max 27, across 144 files (828 edges) — moderate, not the dominant cost per the trace above.

## Goal

1. Incremental cache must never silently produce a stale `mlcc` binary (correctness).
2. Incremental cache must never force a full 147-TU rebuild when only 1–5 files actually changed (speed).
3. Dev-loop default should use `-O0` (`MLCC_DEV=1`), not `-O2`, unless explicitly building for release/CI/self-host bootstrap check.

## Verify gate (every step)

```bash
bundle exec rake test_compiler_mlc   # 447 baseline, must stay green
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # must be empty
```

Plus a **staleness repro check** after steps 1–2 (must now correctly detect the change):

```bash
# append a harmless comment to any existing compiler/*.mlc file (no new/removed files)
touch compiler/codegen/expr/match_gen.mlc   # or real content edit
rm -f compiler/out/mlcc   # force cold to isolate the fingerprint logic, or use MLCC_FORCE_RUBY=1 once
MLCC_BUILD_VERBOSE=1 compiler/build.sh
# expected: codegen actually re-runs (not "skip"); compiler/out/*.cpp mtimes bump
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | `compute_module_fingerprint()` — hash file **content** | **done, but NOT wired to skip-decision** — see "STEP=1 finding" below. `should_skip_codegen()` still uses the file-list-only fingerprint (unchanged behavior); content fingerprint is computed and compared only to emit a loud warning (`warn_if_content_drifted`), replacing the previous silent lie. |
| 2 | `build_bin.sh` — `-MMD -MF obj/$TAG/$name.d` per compile; staleness check consults depfiles | **done** — verified live: touched a shared header (`ast.hpp`, ~131 dependent TUs) with no `.cpp` mtime change → 114/147 objects correctly recompiled, rest skipped. Fully-warm no-op rebuild: 4.5s (unchanged). `regression_gate.sh`: 14/14 pass after rebuild. |
| 3 | ccache+PCH experiment: try `CCACHE_SLOPPINESS=pch_defines,time_macros` (+ `CCACHE_DEPEND=1`) with clang `-include-pch`; measure `ccache -s` delta on 2 consecutive `MLCC_OBJ_CLEAN=1` runs of identical content. If still 0 hits — drop PCH for the ccache-enabled dev path, or switch PCH strategy (`g++` `.gch` `-include`, not `-include-pch`) and re-measure | **done** — root cause was exactly the missing sloppiness (ccache debug log: `"You have to specify time_macros sloppiness when using precompiled headers to get direct hits"`). Wired `export CCACHE_SLOPPINESS=pch_defines,time_macros` in `build_bin.sh` whenever `ccache` is in `CXX_CMD` (2026-07-03). Measured on `compiler/out` (147 TU): before fix, `ccache -s` showed 97.4% "Uncacheable calls", 0% hit rate, cold `MLCC_OBJ_CLEAN=1` rebuild 416s. After fix: run1 (cold, populates cache) 416s, 99.3% cacheable, 0 hits (expected); run2 (`MLCC_OBJ_CLEAN=1` again, unchanged content, same dir) **2.4s**, 148/148 direct hits, binaries byte-identical to run1. **Scope of the win**: only helps repeated builds of the *same* output directory with unchanged content (repeated `compiler/build.sh`/gate re-runs without source edits, or CI with a persisted `~/.cache/ccache` across pushes) — it does *not* collapse the one-shot cross-directory chain (`compiler/out` vs `compiler/out/bootstrap` vs `.tmp_selfhost/p1`/`p2`), since those are legitimately independent `-I` paths (by design, to keep the self-hosting identity check honest); confirmed via a `CCACHE_BASEDIR` experiment that cross-directory reuse does not happen (different `-I` value ⇒ different cache key, expected/correct). CI (`ci.yml`) currently has **no `actions/cache` step for `~/.cache/ccache`**, so this fix currently has zero effect there until that's added (separate follow-up, not yet a track step). |
| 4 | Flip dev-loop default to `MLCC_DEV=1` (`-O0 -g`) in `build.sh`/`build_bin.sh`; require explicit `MLCC_OPT=2` for CI / self-host bootstrap verification / release builds. Update README + `mlcc-self-host-verification.mdc` example commands | **declined (2026-07-03)** — blast radius too wide for the gain: would silently change the optimization level of the *committed* `compiler/out/mlcc` binary (used as the bootstrap seed, downloaded by CI) unless every one of `ci.yml`/`build-mlcc-once.yml`/`triple-bootstrap.yml` is updated in lockstep to pin `MLCC_OPT=2`, and the `-O2` figure in `mlcc-self-host-verification.mdc`'s perf ориентир would silently stop meaning what it says. `MLCC_DEV=1` already exists as an explicit opt-in for anyone who wants a faster local dev-loop build; that is sufficient without touching the default. Not revisiting unless a fresh measurement shows the opt-in isn't being used / isn't enough. |
| 5 | (optional, bigger) Replace the hand-rolled bash parallel-compile loop in `build_bin.sh` with generated `build.ninja` (native depfile support, correct scheduling). mlcc's plain `.cpp`/`.hpp` emission stays unchanged — only compile orchestration changes | planned |
| 6 | Re-run `-ftime-trace` on `transform.cpp` (or current largest TU) after steps 1–5. Only if `InstantiateFunction`/`Backend` still dominates on a typical single-file-edit incremental build (not full rebuild) — open a follow-up track for `extern template` explicit instantiation of the hottest `Shared<T>`/`Array<T>`/`std::function<...>` combos, backed by an actual instantiation-count table, not guesswork | planned |
| 7 | Audit + update `TRACK_BUILD_SPEED.md` (correction note: STEP=5 fingerprint was content-blind), README, `mlcc-self-host-verification.mdc`; close track | planned |

## STEP=1 finding (2026-07-01): content-based skip is blocked by TRACK_BOOTSTRAP_LINK

Implemented `compute_module_content_fingerprint()` and wired it as the **sole** skip
condition first (as originally planned). Live test: with uncommitted WIP in
`match_gen.mlc`/`expr_visitor_cpp.mlc`, `compiler/build.sh` correctly stopped
skipping and ran real codegen — but `build_bin.sh` then failed with real `g++`
compile errors (`transform_stmts.cpp`: `std::visit` no matching overload).

Then tested against **clean HEAD** (`git stash`, no WIP at all): fresh
`compiler/out/mlcc -o /tmp/head_check compiler/main.mlc` produces **290 files**
differing from the committed `compiler/out/*` (33K+/27K- lines) — the checked-in
snapshot has been drifting from `compiler/*.mlc` HEAD for a while, hidden by the
old filename-only fingerprint. Building `/tmp/head_check` via `build_bin.sh`
**fails**: `value.cpp` `std::visit` non-void-lambda-returns-void error,
`type_index.cpp` warning. This is not a new bug — it is
[TRACK_BOOTSTRAP_LINK.md](TRACK_BOOTSTRAP_LINK.md), **open**, STEP=8-P8e
("incremental sweep 144/0 done; bootstrap build pending"). A truly fresh
whole-program regen still does not compile; only the curated incremental state
(never fully wiped) does.

**Conclusion:** switching `should_skip_codegen()` to content-based right now
would make **every** `compiler/build.sh` call attempt a full regen and fail on
files unrelated to whatever was actually edited — a availability regression far
worse than the staleness bug it fixes. Reverted the skip condition back to
file-list-only (original, safe behavior); kept the content fingerprint purely as
a **non-blocking warning** (`warn_if_content_drifted`, fires on both the
mtime-fresh fast path and the module-stamp fast path). Verified: default
`compiler/build.sh` now prints
`WARNING: .mlc content changed but codegen skipped ... mlcc may be stale`
instead of silently claiming "up to date", while still not blocking normal
builds.

**Do not** flip `should_skip_codegen()` to content-based until
TRACK_BOOTSTRAP_LINK STEP=8-P8e is green (fresh `mlcc -o tmp compiler/main.mlc`
+ `build_bin.sh` on the result, zero `g++` errors). At that point STEP=1 here
can be finished (few-line change: swap `module_file_list_matches` for
`module_content_matches` in `should_skip_codegen()`).

Secondary finding: `mlcc_binary_is_fresh()` (`compiler/build.sh`, outer gate) is
mtime-based and therefore unreliable across `git checkout`/`stash`/branch-switch
(git does not preserve relative mtimes). `warn_if_content_drifted` is now also
called from that outer "up to date (skip)" branch for this reason, but the
underlying mtime check itself is unchanged — a future step could replace it with
the content fingerprint too, gated behind the same TRACK_BOOTSTRAP_LINK
precondition.

## 2026-07-03 full-chain measurement (8-core, contended: load avg 11–12 from other tenants)

Measured "Ruby rebuild from scratch → regenerate everything → full verification chain" end to end, as run by hand (not yet a committed script):

| phase | naive (as commonly run by hand) | with safe fixes applied | fix |
|---|---|---|---|
| `MLCC_FORCE_RUBY=1 MLCC_INCREMENTAL=0 build.sh` | 817s | 817s (unchanged; single-threaded Ruby codegen, rare step) | — |
| self-regen `mlcc -o tmp main.mlc` | 10s | 10s | — |
| clang `-fsyntax-only` audit, 144 TU | 2094s (sequential `for` loop) | 405s | `ls *.cpp \| xargs -P8 clang++ -fsyntax-only ...` — same 144/144 result, no coverage loss |
| regen `compiler/out` via self-hosted `mlcc` + `build_bin.sh` | 414s | 414s cold / **30s** if identical content to a just-built dir (STEP=3 fix) | — |
| `test_compiler_mlc` | 99s | 99s | — |
| `regression_gate.sh` (14 programs) | 262s (strictly sequential, ~19s/program: Ruby startup + uncached g++ link each) | not yet fixed, est. ~80–100s if parallelized | not implemented — candidate addition to `TRACK_CLANG_MIGRATION` step 2 |
| mlcc2 g++ cross-check (self-host verification rule) | 750–1283s (single non-parallel `g++ *.cpp -o mlcc2`, no PCH) | **328s** | use `MLC_CXX=g++ compiler/build_bin.sh <dir> mlcc2` instead of a hand-rolled `g++` invocation — parallel + PCH + mold link, verified byte-identical self-regen output vs the naive path |
| `MLCC_BOOTSTRAP=1 build.sh` | 377s | 377s (new dir each time, cold) | — |
| `run_mlcc_bootstrap_parity.sh` | 374s | 374s (new dirs, cold) | — |
| **total** | **~5447s (≈91 min)** | **~3086s (≈51 min)** | audit parallelization + build_bin.sh-based g++ cross-check alone, no coverage/reliability loss |

Findings not yet acted on:

- `.cursor/rules/mlcc-self-host-verification.mdc`'s example `g++ ... *.cpp ... -o mlcc2` command should be replaced with the `MLC_CXX=g++ build_bin.sh` form above (rule text, not code — separate from this track).
- `.github/workflows/ci.yml` sets `MLC_CXX: ccache clang++` but has **no `actions/cache` step for `~/.cache/ccache`** — every CI run starts with a cold cache, so STEP=3's fix currently has zero effect in CI even though CI recompiles the same 147 TU on every push where most files are usually unchanged. Adding a persisted cache (keyed e.g. on `compiler/**` content hash, restore-keys fallback to any prior cache) is the highest-ROI next step for CI wall-clock, and needs no server rental.
- No committed script for the parallel clang audit exists (it has only ever been run ad hoc). Should live as e.g. `compiler/tests/clang_audit.sh` if the project wants to keep using it as a recurring gate.
- Renting a bigger machine: only helps the compile-bound half of the chain (144-TU compiles + audit + links, ≈1900–2900s of the total depending on which fixes are applied) — that part is embarrassingly parallel across TUs and scales close to linearly with core count. It does **not** help the Ruby rebuild (817s, single-threaded, and a rare step — only needed after `lib/mlc/backends/cpp/**` edits) or `regression_gate.sh`'s per-program Ruby-startup floor. Cheapest actual lever, given CI already exists and is currently idle-cost when not running: move the *full* 9-phase gate to CI (with the ccache-persistence fix above) and keep only the fast local loop (`test_compiler_mlc` + `regression_gate.sh`, ~6 min) for interactive dev-loop use, per the existing CLAUDE.md guidance to run E2E selectively. A rented dedicated/cloud box is only worth it if this full chain must run interactively, synchronously, many times a day — current usage pattern does not show that need.
- This machine was measured under real contention (load avg 11–12 on 8 cores from other tenants during the run) — absolute numbers above are ~1.5–2x inflated vs an uncontended run on the same hardware; relative speedups (5x audit, 2.3–4x g++ cross-check, 173x repeated-build ccache hit) are contention-independent.

## Out of scope (do not do without a fresh measurement showing steps 1–6 are insufficient)

- `#include` hygiene / forward-declaration pass across `compiler/**` and `runtime/include/**` — PCH already captures most of the available win (STEP=4 of closed `TRACK_BUILD_SPEED`); `-ftime-trace` shows `Frontend` (header parse) is not the dominant cost.
- Pimpl for runtime types.
- Unity/jumbo build.
- C++20 modules.
- Second codegen backend (`cpp_fastbuild`: tagged unions instead of `std::variant`/`std::visit`) — speculative, high effort, not supported by current `-ftime-trace` data (template instantiation volume is broad — many types — not concentrated in variant/visit specifically; would need its own profiling pass to justify).

If a future agent is asked to apply the C++ "minimize includes / pimpl / unity build / modules" advice wholesale: **do not**. Point back to this baseline first; re-run step 6's trace; only act on what the trace actually shows.

## Per-turn template

```
| step | <1-7> |
| done | <one line> |
| verify | test_compiler_mlc N/0; diff_exit=0; staleness repro: <pass/fail> |
| next | STEP=<n+1> |
```
