# Track: Concurrency Supervisor

Parent: [../PLAN.md](../PLAN.md) Фаза 8; spec:
[../CONCURRENCY_V2.md](../CONCURRENCY_V2.md) §28–29 (Supervisor, restart storm).
Predecessor closed:
[../archive/tracks/TRACK_CONCURRENCY_ISOLATE.md](../archive/tracks/TRACK_CONCURRENCY_ISOLATE.md)
(ThreadPool, Isolate, StopToken shutdown).

## Status: **deferred** — gate condition below, not before it

**Driver 2026-07-09:** Created on ISOLATE close. `TRACK_FFI_LAYER` closed;
queue head is [TRACK_MIR_VM_FULL](TRACK_MIR_VM_FULL.md) (API_CLIENT closed). Supervisor needs Isolate/TaskScope
in production use before heavy work.

**Gate — clarified 2026-07-10** (previous "chat-server gate" wording was
undefined, no such phase exists in `CONCURRENCY_V2.md`). Concrete condition:
[STDLIB_BACKEND.md](../STDLIB_BACKEND.md) §5 TCP/HTTP server track (not yet
created, "критический" gap, unblocked) reaching WebSocket stage — that is
the first realistic multi-connection app that exercises `Isolate`/
`TaskScope` per-connection under real concurrent load, which is what
Supervisor needs to restart against. Until that track exists and reaches
WS stage, Supervisor design questions (restart policy semantics under real
failure modes) have no concrete driving use case — do not start STEP=1
without it.

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
