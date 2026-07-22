# Track: Concurrency TestRuntime MLC Surface

Parent: [TRACK_MLC_CONCURRENCY_REFINEMENT.md](../../agent/TRACK_MLC_CONCURRENCY_REFINEMENT.md) §47 **#11**.
Source: [CONCURRENCY_V2.md](../CONCURRENCY_V2.md) §34.
C++ predecessor: [TRACK_CONCURRENCY_TEST_HARNESS](TRACK_CONCURRENCY_TEST_HARNESS.md) (T7 deferred MLC). Size **M**.

## Status: **closed** (2026-07-22) — Critic OK

**Critic 2026-07-22 (STEP=4):** Re-ran `run_testruntime_mlc.sh` + fixture/`main.mlc`
`--check-only` + `test_scheduler`. Anti-false-done: `4550e1cf`…`a6dce083` (STEP=0–3);
no `lib/mlc/` → REG skip. Wire: MLC `TestRuntime.new`/`spawn`/`join`/`log_event`/
`events_joined`/`seed` → `TestScheduler`; TestRuntime !Send/!Sync; MEMORY_MODEL +
CONCURRENCY_V2 §0/§34/§43 synced. **reopen: none**.

Honest residual: `TestMutex`/`TestChannel` MLC out; receiver `TNamed(TestRuntime)`
often missing at codegen — gate `spawn`≠`TaskScope` + unique method names.

| Gate | Result |
|------|--------|
| `run_testruntime_mlc.sh` | `ok testruntime_mlc` EXIT=0 (check-only + link + same-seed + test_scheduler) |
| fixture `--check-only` | EXIT=0 (new_spawn + same_seed) |
| `mlcc --check-only compiler/main.mlc` | EXIT=0 |
| `test_scheduler` | ALL 9 checks PASSED |
| Tier B / self-host | STEP=2: Tier B EXIT=0; p1≡p2 DIFF_EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§46 `#38 EDITOR_IDLE_BUSY_LOOP_92PCT_CPU`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `4550e1cf` | Decision freeze + open |
| 1 | `2b6ee46a` | Red harness (E001 gap) |
| 2 | `381121a4` | MLC wire; Tier B + self-host |
| 3 | `a6dce083` | MEMORY_MODEL + CONCURRENCY_V2 sync |
| 4 | this Critic | close + archive |

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
| 4 | Critic: gates; archive | **done** — closed |

## Out of scope

TestMutex/TestChannel MLC; fuzz/sanitize; `lib/mlc/`; `Future`/`async`.
