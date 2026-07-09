# Track: Concurrency Isolate / ThreadPool

Parent: [../PLAN.md](../PLAN.md) Фаза 8; spec:
[../CONCURRENCY_V2.md](../CONCURRENCY_V2.md) phases 8+ (`Isolate`, `ThreadPool`,
Supervisor later). Predecessor closed:
[../archive/tracks/TRACK_CONCURRENCY_TASKSCOPE.md](../archive/tracks/TRACK_CONCURRENCY_TASKSCOPE.md)
(cancel wake, TaskScope C++, Sync-safe spawn capture).

## Status: **open** — STEP=3 done; STEP=4 next (docs / close)

**Driver 2026-07-09:** STEP=3 — StopToken on ThreadPool/Isolate: `token` /
`request_cancel`; cancellable submit/send; `submit_with_token`; shutdown cancels.

## Goal

Library-first `Isolate[State, Message]` (bounded mailbox) and `ThreadPool`
per CONCURRENCY_V2 §16–19. No MLC actor sugar in v1.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | `ThreadPool` runtime (workers + bounded queue); unit + smoke | **done** |
| 2 | `Isolate` mailbox (bounded channel + single owner thread) | **done** |
| 3 | Wire cancel/`StopToken` into pool/isolate shutdown | **done** |
| 4 | Docs + self-host / close or hand off to Supervisor | **next** |

## Out of scope

- `Supervisor` / restart policies (CONCURRENCY_V2 §28–29).
- async I/O / `IoReactor`.
- MLC `scope |s|` surface (still deferred from TASKSCOPE).
