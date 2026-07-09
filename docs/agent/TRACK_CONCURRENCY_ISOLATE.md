# Track: Concurrency Isolate / ThreadPool

Parent: [../PLAN.md](../PLAN.md) Фаза 8; spec:
[../CONCURRENCY_V2.md](../CONCURRENCY_V2.md) phases 8+ (`Isolate`, `ThreadPool`,
Supervisor later). Predecessor closed:
[../archive/tracks/TRACK_CONCURRENCY_TASKSCOPE.md](../archive/tracks/TRACK_CONCURRENCY_TASKSCOPE.md)
(cancel wake, TaskScope C++, Sync-safe spawn capture).

## Status: **open** — STEP=1 done; STEP=2 next (Isolate mailbox)

**Driver 2026-07-09:** STEP=1 — `ThreadPool` (workers + bounded Channel queue,
blocking submit, shutdown/dtor drain+join).

## Goal

Library-first `Isolate[State, Message]` (bounded mailbox) and `ThreadPool`
per CONCURRENCY_V2 §16–19. No MLC actor sugar in v1.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | `ThreadPool` runtime (workers + bounded queue); unit + smoke | **done** |
| 2 | `Isolate` mailbox (bounded channel + single owner thread) | **next** |
| 3 | Wire cancel/`StopToken` into pool/isolate shutdown | pending |
| 4 | Docs + self-host / close or hand off to Supervisor | pending |

## Out of scope

- `Supervisor` / restart policies (CONCURRENCY_V2 §28–29).
- async I/O / `IoReactor`.
- MLC `scope |s|` surface (still deferred from TASKSCOPE).
