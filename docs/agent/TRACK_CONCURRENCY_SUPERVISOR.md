# Track: Concurrency Supervisor

Parent: [../PLAN.md](../PLAN.md) Фаза 8; spec:
[../CONCURRENCY_V2.md](../CONCURRENCY_V2.md) §28–29 (Supervisor, restart storm).
Predecessor closed:
[../archive/tracks/TRACK_CONCURRENCY_ISOLATE.md](../archive/tracks/TRACK_CONCURRENCY_ISOLATE.md)
(ThreadPool, Isolate, StopToken shutdown).

## Status: **deferred** — do not start until Planner reorders after chat-server gate

**Driver 2026-07-09:** Created on ISOLATE close. `TRACK_FFI_LAYER` closed;
queue head is [TRACK_API_CLIENT](TRACK_API_CLIENT.md). Supervisor needs Isolate/TaskScope
in production use (CONCURRENCY_V2 phase-9 chat-server gate) before heavy work.

## Goal

Library-first child supervision: `permanent|transient|temporary`, start with
`one_for_one`; restart storm protection (`max`/`within`/`backoff`).

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | `Supervisor` runtime skeleton (register child, start, stop) | **deferred** |
| 2 | Restart policies + one_for_one | deferred |
| 3 | Restart storm protection | deferred |
| 4 | Docs + smoke / close | deferred |

## Out of scope

- `one_for_all` / `rest_for_one` (later).
- async I/O / `IoReactor`.
- Full OTP actor model.
