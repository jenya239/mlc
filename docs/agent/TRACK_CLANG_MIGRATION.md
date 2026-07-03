# Track: Full clang migration (drop hardcoded g++ from verification scripts)

Parent: [../PLAN.md](../PLAN.md) §Phase 2.9; related: [TRACK_BUILD_SPEED2.md](TRACK_BUILD_SPEED2.md), [TRACK_CPPEXPR.md](TRACK_CPPEXPR.md) (closed — origin of the g++ pin below).

## Status: **planned** — unblocked (2026-07-03: [TRACK_BOOTSTRAP_LINK](TRACK_BOOTSTRAP_LINK.md) closed, all codegen bugs fixed, 144/144 clean under clang)

## Background

`compiler/build_bin.sh` (the actual mlcc build used in dev loop) already prefers
`ccache clang++` when both are on PATH (`compiler/build_bin.sh:27-37`), and CI
(`ci.yml`, `build-mlcc-once.yml`, `triple-bootstrap.yml`) already sets
`MLC_CXX: ccache clang++`. So the **main build path is already on clang**.

The g++ pin is a leftover from **TRACK_CPPEXPR.md:21** (closed track):
"Self-host link: **g++** ... not `build_bin.sh`/clang++, until codegen emits no
GCC-only `auto` in template args." That reason has not been re-verified since.

**2026-07-01 finding:** ran `compiler/build_bin.sh` (auto-selected `ccache
clang++`) on a fresh full regen of `compiler/main.mlc` — 145/147 generated TUs
compiled clean under clang. The 2 remaining failures (`value.cpp` — fixed same
day; `transform_stmts.cpp` — open, see TRACK_BOOTSTRAP_LINK) are portable
standard-C++ errors (`std::visit` return-type mismatch, `Transform_stmts_fold_state`
type confusion), reproducible on **both** clang and g++ — not a GCC-extension
issue. The original TRACK_CPPEXPR constraint appears stale.

## Goal

1. Confirm (not assume) there is no remaining GCC-only construct anywhere in
   currently-generated output.
2. Make every script/doc/rule that currently hardcodes `g++` follow the same
   `MLC_CXX` chain as `build_bin.sh` (`ccache clang++` → `clang++` → `ccache g++`
   → `g++`), so clang is used everywhere by default and g++ remains an explicit,
   working fallback — not deleted, since environments without clang must still work.
3. Close out the stale TRACK_CPPEXPR g++ pin with a dated note.

## Non-goals

- Do not remove g++ support entirely. Keep it reachable via `MLC_CXX=g++` for
  environments without clang, and as an independent cross-check compiler for
  regression comparisons.
- `benchmarks/profile/run_profile.sh`'s `GPROF=1` path is **intentionally** g++
  (`-pg` + `gprof` is a GNU-toolchain-specific profiling flow; clang's `-pg`
  behavior is not equivalent). Leave as is.

## Verify gate (every step)

```bash
bundle exec rake test_compiler_mlc
scripts/regression_gate.sh
compiler/tests/build_tests.sh
```

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Full per-TU audit: `clang++ -std=c++20 -fsyntax-only -I <dir> -I runtime/include <file>.cpp` on **every** file in a fresh `compiler/out` regen (not just the aggregate `build_bin.sh` run, which interleaves parallel-job errors). Catalog any GCC-only construct found (expected: none beyond the 2 already-tracked bugs) | planned |
| 2 | `scripts/regression_gate.sh:33` — hardcoded `g++` → `${MLC_CXX:-g++}`-style chain (mirror `build_bin.sh`'s selection, factor into a shared shell snippet if a third place needs it) | planned |
| 3 | `.cursor/rules/mlcc-self-host-verification.mdc` — example commands use `g++` → switch to the same `MLC_CXX` chain (or explicitly show both) | planned |
| 4 | `compiler/scripts/bisect_bootstrap_link.sh` — 3 hardcoded `g++` calls → `MLC_CXX` chain | planned |
| 5 | `compiler/tests/build_tests_self.sh`, `compiler/tests/e2e/run_e2e.sh`, `compiler/tests/test_result_e2e.sh`, `compiler/tests/run_vm_cpp_exit_diff.sh` — hardcoded `g++` → `MLC_CXX` chain | planned |
| 6 | `runtime/test/run_concurrency_smoke.sh:10` — already `${MLC_CXX:-g++}`; just flip default to prefer clang when present (same one-liner pattern as `build_bin.sh:27-37`, not a bare env-var default) | planned |
| 7 | `compiler/triple_bootstrap.sh` — 2 stages currently always g++ (comments + `echo` labels) → `MLC_CXX` chain; update comments | planned |
| 8 | Delete `compiler/mlcc_build.sh` (dead code — zero callers found repo-wide, superseded by `build_bin.sh`) | planned |
| 9 | `TRACK_CPPEXPR.md` — append dated closing note: g++ pin verified stale (link to this track's STEP=1 audit result) | planned |
| 10 | `docs/PLAN.md:381` — row is stale ("planned", doesn't mention `TRACK_BUILD_SPEED2`/this track); update | planned |
| 11 | Full regression sweep on the converted scripts (verify gate above) + re-run `compiler/tests/e2e/run_e2e.sh` and `build_tests_self.sh` under clang explicitly (`MLC_CXX=` unset, i.e. default chain) to confirm no behavior change vs the previous all-g++ baseline | planned |

## Out of scope

- `compiler/mlcc_precompiled.hpp` / PCH strategy — unaffected by compiler choice, already branches on `cxx_is_clang()` in `build_bin.sh`.
- Any codegen change. This track is scripts/docs only. If STEP=1 finds a real GCC-only construct in codegen output, that becomes a separate track (codegen bug), not part of this cleanup.
