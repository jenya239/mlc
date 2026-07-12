# Track: Concurrency Supervisor

Parent: [../PLAN.md](../PLAN.md) §25 / Фаза 8; spec:
[../CONCURRENCY_V2.md](../CONCURRENCY_V2.md) §28–29 (Supervisor, restart storm).
Predecessor closed:
[../archive/tracks/TRACK_CONCURRENCY_ISOLATE.md](../archive/tracks/TRACK_CONCURRENCY_ISOLATE.md)
(ThreadPool, Isolate, StopToken shutdown).
HTTP hardening closed (Critic OK 2026-07-12):
[../archive/tracks/TRACK_STDLIB_HTTP_HARDENING.md](../archive/tracks/TRACK_STDLIB_HTTP_HARDENING.md).

## Status: **active** (Planner 2026-07-12) — очередь §25

## Next step

**STEP=6** — docs: `CONCURRENCY_V2.md` §28–29 implemented (C++ v1); `STDLIB_BACKEND.md` C++-only note (JobQueue-style, no MLC row).

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
  class of gap as [TRACK_STDLIB_JOB_QUEUE](../archive/tracks/TRACK_STDLIB_JOB_QUEUE.md)
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
| 6 | Docs: `CONCURRENCY_V2.md` §28-29 — mark implemented; `STDLIB_BACKEND.md` — one row if MLC-reachable | pending — C++-only docs (no MLC row); align wording with JobQueue |
| 7 | Verify: `runtime/test/run_concurrency_smoke.sh` includes new supervisor test; sanitizer gate (`scripts/concurrency_sanitize_gate.sh`) green; self-host diff only if MLC-reachable (Step 4 decided yes) | pending — self-host N/A per Decision |

### Progress

- **Planner** (2026-07-12): activated after HTTP Critic OK; STEP=1 next.
- **STEP=1** (2026-07-12): `Supervisor` add/start/stop + `test_supervisor` (4 checks); wired into `run_concurrency_smoke.sh`.
- **STEP=2** (2026-07-12): `RestartPolicy` permanent/transient/temporary; one_for_one loops; 9 checks.
- **STEP=3** (2026-07-12): restart intensity `max`/`within`; storm stops supervisor + siblings; 17 checks. Backoff deferred.
- **STEP=4** (2026-07-12): Decision — C++-only v1; MLC module deferred (JobQueue pattern).
- **STEP=5** (2026-07-12): 3-child transient isolation + permanent always-fail storm; 26 checks; smoke green.

## Out of scope

- `one_for_all` / `rest_for_one` (later).
- async I/O / `IoReactor`.
- Full OTP actor model.

## Gate (historical)

**Gate satisfied (2026-07-11):** WebSocket stage reached —
[TRACK_STDLIB_WEBSOCKET_TO_MLC](../archive/tracks/TRACK_STDLIB_WEBSOCKET_TO_MLC.md)
closed, `TRACK_CONCURRENCY_SPAWN_DETACH` closed (real parallel
`scope`+`spawn` accept loops verified with curl), HTTP server working
end-to-end in MLC ([TRACK_STDLIB_HTTP_MLC](../archive/tracks/TRACK_STDLIB_HTTP_MLC.md)).
The originally-required "first realistic multi-connection app that exercises
`Isolate`/`TaskScope` per-connection under real concurrent load" now exists
(`misc/examples/http_scope_accept_loop_demo.mlc`,
`misc/examples/http_server_forever_demo.mlc`).
