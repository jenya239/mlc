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

**STEP=3** — restart storm protection (`max` / `within`).

## Goal

Library-first child supervision: `permanent|transient|temporary`, start with
`one_for_one`; restart storm protection (`max`/`within`/`backoff`).

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | `Supervisor` runtime skeleton in `runtime/include/mlc/concurrency/supervisor.hpp`: register child (a `std::function<void()>`-style spawn closure, matching how `scope.spawn` bodies are already shaped), start all children, stop all children (cooperative via existing `StopToken`) | **done** (2026-07-12) — `supervisor.hpp` add/start/stop; `test_supervisor.cpp` + smoke hook |
| 2 | Restart policies: `permanent` (always restart on exit, including clean exit), `transient` (restart only on abnormal/exception exit), `temporary` (never restart); default child strategy `one_for_one` (only the failed child restarts, siblings untouched) | **done** (2026-07-12) — `RestartPolicy` + per-child loop; tests permanent/transient/temporary + sibling isolation |
| 3 | Restart storm protection: `max` restarts `within` a rolling time window (per `CONCURRENCY_V2.md` §29) — exceeding it stops the whole supervisor (propagate failure up, do not restart forever) | pending |
| 4 | MLC-facing API sketch: does this need to be reachable from MLC source (`extern fn`-based, like `Tcp`) or is it C++-runtime-only for now (like `JobQueue`, per `STDLIB_JOB_QUEUE` decision)? Decide before Step 5, follow the same reasoning as that track — do not assume MLC-reachable by default | pending |
| 5 | Smoke test: 3 children, one deliberately panics/exits abnormally twice then succeeds — assert only that child restarts, siblings' state (call count) unaffected; one child configured `permanent` + always-fails → assert restart-storm stop after `max`/`within` | pending |
| 6 | Docs: `CONCURRENCY_V2.md` §28-29 — mark implemented; `STDLIB_BACKEND.md` — one row if MLC-reachable | pending |
| 7 | Verify: `runtime/test/run_concurrency_smoke.sh` includes new supervisor test; sanitizer gate (`scripts/concurrency_sanitize_gate.sh`) green; self-host diff only if MLC-reachable (Step 4 decided yes) | pending |

### STEP=1 sub-steps (Driver)

1. Read `runtime/include/mlc/concurrency/task_scope.hpp` + `stop.hpp` — how spawn closures and `StopToken` cooperative stop work today.
2. Add `runtime/include/mlc/concurrency/supervisor.hpp` (header-only if peers are): child registry (`id`/name + `std::function<void()>` body), `start()` launches children on `ThreadPool`/`std::thread` (match Isolate/TaskScope pattern — prefer existing pool), `stop()` requests stop via `StopToken` and joins.
3. Minimal C++ unit under `runtime/test/` (or extend `run_concurrency_smoke.sh`) that registers 1–2 no-op children, start+stop without leak/hang.
4. Do **not** implement restart policies here (STEP=2) or storm limits (STEP=3).

### Progress

- **Planner** (2026-07-12): activated after HTTP Critic OK; STEP=1 next.
- **STEP=1** (2026-07-12): `Supervisor` add/start/stop + `test_supervisor` (4 checks); wired into `run_concurrency_smoke.sh`.
- **STEP=2** (2026-07-12): `RestartPolicy` permanent/transient/temporary; one_for_one loops; 9 checks.

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
