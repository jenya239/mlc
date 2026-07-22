# Track: Concurrency TestRuntime MLC Surface

Parent: [TRACK_MLC_CONCURRENCY_REFINEMENT.md](TRACK_MLC_CONCURRENCY_REFINEMENT.md) §47 **#11**.
Source: [CONCURRENCY_V2.md](../CONCURRENCY_V2.md) §34.
C++ predecessor: [archive/tracks/TRACK_CONCURRENCY_TEST_HARNESS](../archive/tracks/TRACK_CONCURRENCY_TEST_HARNESS.md) (T7 deferred MLC). Size **M**.

## Status: **active** (2026-07-21) — STEP=3 done → Critic

## Next step

**STEP=4** — Critic: gates; archive.

## Decision (STEP=0) — **frozen** 2026-07-21

| Item | Choice |
|------|--------|
| Problem | CONCURRENCY_V2 §34: deterministic `TestRuntime.new(seed:)` for reproducible interleavings. **Already present (C++):** `runtime/include/mlc/concurrency/testing/scheduler.hpp` — `TestScheduler(seed)` / `spawn` / `join` / `yield` / `park` / `unpark` / `log_event` / `events`; `TestMutex`/`TestChannel`; smoke `runtime/test/test_scheduler.cpp` (9 checks, green in `run_concurrency_smoke.sh`). Prior TEST_HARNESS T7 closed **C++-only** (JobQueue/Supervisor-style closure deferral). **Real gap:** no MLC name (probed 2026-07-21: `TestRuntime` → **E001** status=1; `TestScheduler` → **E001**). Supervisor/Isolate MLC (#9–#10) proved handler wrap shippable — same path |
| Boundaries in scope | (1) MLC method API: `TestRuntime` as facade over C++ `TestScheduler` — `TestRuntime.new(seed)` / `.spawn do … end` (parser requires `do`; body = `() -> unit` lambda) / `.join()` / `.seed()`. (2) Optional MVP extras if needed for gate: `.yield()` / `.log_event(string)` (or events via C++ smoke). (3) Gate: MLC e2e — same `seed` → identical event/order snapshot across two runs (mirror `test_scheduler` determinism). (4) Ensure generated TU sees `testing/scheduler.hpp` (not in `mlc.hpp` today — add include or emit). (5) Docs MEMORY_MODEL / CONCURRENCY_V2 §34 |
| Boundaries out | Full MLC `TestMutex`/`TestChannel` surface (defer unless trivial); rewriting C++ scheduler; fuzz/sanitize CI (already T6); `rt.run()` sugar (C++ is `join`); model checker; `lib/mlc/`; JobQueue MLC |
| Wire location | `compiler/` registry + infer/codegen; reuse `testing/scheduler.hpp`; tests under `compiler/tests/` + existing `test_scheduler` |
| Token / gate | (a) red: TestRuntime MLC undefined; (b) green: `--check-only` + linked same-seed determinism e2e; (c) `mlcc --check-only`; Tier B + self-host on every `compiler/` STEP |
| Docs | MEMORY_MODEL TestRuntime/TestScheduler row; CONCURRENCY_V2 §34 MLC status; cite track |
| REG | yes if `lib/mlc/` touched (expect **no**); else skip |
| Out of scope | TestMutex/TestChannel MLC; fuzz; Future/async |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: TestRuntime MLC missing | **done** — `ok testruntime_mlc_red` (E001; `scheduler.hpp` present) |
| 2 | Wire MLC TestRuntime new/spawn/join; green same-seed e2e; Tier B; self-host | **done** — `ok testruntime_mlc`; Tier B EXIT=0; p1≡p2 DIFF=0 |
| 3 | MEMORY_MODEL + CONCURRENCY_V2 §34 sync | **done** — TestRuntime row/notes; §0/§34/§43 |
| 4 | Critic: gates; archive | open |

<!-- STEP=1 sub-steps: 1) fixture `testruntime_new_spawn.mlc` using TestRuntime; 2) `run_testruntime_mlc_red.sh` expect E001 + status≠0; 3) assert `scheduler.hpp` exists; green stub `run_testruntime_mlc.sh` EXIT=1 -->
<!-- STEP=2 sub-steps: 1) infer+codegen TestRuntime→TestScheduler; spawn handler wrap; include testing header; 2) green harness check-only+link+same-seed; 3) Tier B + self-host -->

## Out of scope

TestMutex/TestChannel MLC; fuzz/sanitize; `lib/mlc/`; `Future`/`async`.
