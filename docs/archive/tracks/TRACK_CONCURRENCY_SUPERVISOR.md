# Track: Concurrency Supervisor

Parent: [../../PLAN.md](../../PLAN.md) §25 / Фаза 8; spec:
[../../CONCURRENCY_V2.md](../../CONCURRENCY_V2.md) §28–29 (Supervisor, restart storm).
Predecessor closed:
[TRACK_CONCURRENCY_ISOLATE.md](TRACK_CONCURRENCY_ISOLATE.md)
(ThreadPool, Isolate, StopToken shutdown).
HTTP hardening closed (Critic OK 2026-07-12):
[TRACK_STDLIB_HTTP_HARDENING.md](TRACK_STDLIB_HTTP_HARDENING.md).

## Status: **closed** (2026-07-12) — Critic OK

**Planner 2026-07-12:** activated after HTTP Critic OK.
**Driver 2026-07-12:** STEP=1–7 (`149899a2`…`44334e2d`); C++-only v1;
smoke + ASan/UBSan/TSan green; self-host N/A.
No `compiler/**` (regression_gate N/A).

**Critic 2026-07-12:** STEP=1–7 vs `149899a2`…`44334e2d`; archive/PLAN/STDLIB/
CONCURRENCY_V2 aligned; `test_supervisor` 26 re-OK; soft-include in `mlc.hpp`;
**reopen: none**. Residuals: MLC module deferred; exponential backoff deferred;
`one_for_all`/`rest_for_one` out of scope.

## Next step

— (closed; Planner activates `TRACK_CONCURRENCY_TEST_HARNESS` T6/T7)

## Goal

Library-first child supervision: `permanent|transient|temporary`, start with
`one_for_one`; restart storm protection (`max`/`within`/`backoff`).

## Decision (STEP=4, 2026-07-12)

### Pipeline

- **C++ runtime** under `mlc::concurrency`: `supervisor.hpp` (header-only), same
  layer as `TaskScope` / `JobQueue` / `Isolate`.
- **MLC module: deferred for v1** — registering children needs
  `std::function<void()>` / `void(StopToken)` closures. MLC has no reliable
  extern-callable closure story for opaque C++ supervisor handles yet (same
  class of gap as [TRACK_STDLIB_JOB_QUEUE.md](TRACK_STDLIB_JOB_QUEUE.md)
  Decision: C++-only, MLC deferred). Pattern = **C++-only**, not Tcp/HttpServer
  handle APIs.
- `INSTRUCTIONS_REV` runtime-stays-cpp reinforces: do not invent a half MLC
  surface that cannot pass real child bodies.
- Follow-up (out of this track): MLC `Supervisor` once closures/`extern type`
  story supports it.

### Implications

| Step | Effect of Decision |
|------|-------------------|
| 5 | Keep expanding **C++** `test_supervisor.cpp` / concurrency smoke only |
| 6 | Mark `CONCURRENCY_V2.md` §28–29 implemented for C++ v1; `STDLIB_BACKEND.md` concurrency/JobQueue-style **C++-only** note (no MLC registry) |
| 7 | Self-host diff **N/A** (no `compiler/` / no MLC stdlib module) |

### Non-goals (locked for this track)

MLC `import … from 'Supervisor'`, `extern fn` child registration, mlcc demos
that construct supervisors in `.mlc`.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | `Supervisor` runtime skeleton in `runtime/include/mlc/concurrency/supervisor.hpp`: register child (a `std::function<void()>`-style spawn closure, matching how `scope.spawn` bodies are already shaped), start all children, stop all children (cooperative via existing `StopToken`) | **done** (2026-07-12) — `supervisor.hpp` add/start/stop; `test_supervisor.cpp` + smoke hook |
| 2 | Restart policies: `permanent` (always restart on exit, including clean exit), `transient` (restart only on abnormal/exception exit), `temporary` (never restart); default child strategy `one_for_one` (only the failed child restarts, siblings untouched) | **done** (2026-07-12) — `RestartPolicy` + per-child loop; tests permanent/transient/temporary + sibling isolation |
| 3 | Restart storm protection: `max` restarts `within` a rolling time window (per `CONCURRENCY_V2.md` §29) — exceeding it stops the whole supervisor (propagate failure up, do not restart forever) | **done** (2026-07-12) — `set_restart_intensity` / `storm_tripped`; rolling window; trips request supervisor stop |
| 4 | MLC-facing API sketch: does this need to be reachable from MLC source (`extern fn`-based, like `Tcp`) or is it C++-runtime-only for now (like `JobQueue`, per `STDLIB_JOB_QUEUE` decision)? Decide before Step 5, follow the same reasoning as that track — do not assume MLC-reachable by default | **done** (2026-07-12) — **C++-only v1; MLC deferred** (see Decision above) |
| 5 | Smoke test: 3 children, one deliberately panics/exits abnormally twice then succeeds — assert only that child restarts, siblings' state (call count) unaffected; one child configured `permanent` + always-fails → assert restart-storm stop after `max`/`within` | **done** (2026-07-12) — `test_three_children_transient_sibling_isolation` + `test_permanent_always_fails_trips_storm`; 26 checks |
| 6 | Docs: `CONCURRENCY_V2.md` §28-29 — mark implemented; `STDLIB_BACKEND.md` — one row if MLC-reachable | **done** (2026-07-12) — §28–29 C++ v1 status; STDLIB §1 Supervisor row + Concurrency list (no MLC registry) |
| 7 | Verify: `runtime/test/run_concurrency_smoke.sh` includes new supervisor test; sanitizer gate (`scripts/concurrency_sanitize_gate.sh`) green; self-host diff only if MLC-reachable (Step 4 decided yes) | **done** (2026-07-12) — smoke ok (26); sanitize ASan/UBSan/TSan ok; self-host N/A |

### Progress

- **Planner** (2026-07-12): activated after HTTP Critic OK; STEP=1 next.
- **STEP=1** (2026-07-12): `Supervisor` add/start/stop + `test_supervisor` (4 checks); wired into `run_concurrency_smoke.sh`.
- **STEP=2** (2026-07-12): `RestartPolicy` permanent/transient/temporary; one_for_one loops; 9 checks.
- **STEP=3** (2026-07-12): restart intensity `max`/`within`; storm stops supervisor + siblings; 17 checks. Backoff deferred.
- **STEP=4** (2026-07-12): Decision — C++-only v1; MLC module deferred (JobQueue pattern).
- **STEP=5** (2026-07-12): 3-child transient isolation + permanent always-fail storm; 26 checks; smoke green.
- **STEP=6** (2026-07-12): CONCURRENCY_V2 §28–29 + STDLIB_BACKEND Supervisor C++-only row.
- **STEP=7** (2026-07-12): `run_concurrency_smoke.sh` ok; `concurrency_sanitize_gate.sh` ok; archived; self-host N/A.

## Out of scope

- `one_for_all` / `rest_for_one` (later).
- async I/O / `IoReactor`.
- Full OTP actor model.
- Exponential backoff (deferred from §29).
- MLC `Supervisor` module.

## Gate (historical)

**Gate satisfied (2026-07-11):** WebSocket stage reached —
[TRACK_STDLIB_WEBSOCKET_TO_MLC.md](TRACK_STDLIB_WEBSOCKET_TO_MLC.md)
closed, `TRACK_CONCURRENCY_SPAWN_DETACH` closed (real parallel
`scope`+`spawn` accept loops verified with curl), HTTP server working
end-to-end in MLC ([TRACK_STDLIB_HTTP_MLC.md](TRACK_STDLIB_HTTP_MLC.md)).
The originally-required "first realistic multi-connection app that exercises
`Isolate`/`TaskScope` per-connection under real concurrent load" now exists
(`misc/examples/http_scope_accept_loop_demo.mlc`,
`misc/examples/http_server_forever_demo.mlc`).
