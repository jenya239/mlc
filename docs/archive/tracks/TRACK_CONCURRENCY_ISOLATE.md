# Track: Concurrency Isolate / ThreadPool

Parent: [../../PLAN.md](../../PLAN.md) Фаза 8; spec:
[../../CONCURRENCY_V2.md](../../CONCURRENCY_V2.md) phases 8+ (`Isolate`, `ThreadPool`,
Supervisor later). Predecessor closed:
[TRACK_CONCURRENCY_TASKSCOPE.md](TRACK_CONCURRENCY_TASKSCOPE.md)
(cancel wake, TaskScope C++, Sync-safe spawn capture).

## Status: **closed** — STEP=1–4 done (2026-07-09)

**Closed note:** `ThreadPool` (bounded queue, submit/shutdown); `Isolate`
(serial owner + mailbox, Block overflow); StopToken cancel on both; MEMORY_MODEL
+ PLAN updated; smoke green. Handoff Supervisor:
[TRACK_CONCURRENCY_SUPERVISOR.md](TRACK_CONCURRENCY_SUPERVISOR.md)
(deferred). Queue next was FFI (now closed): [TRACK_FFI_LAYER](TRACK_FFI_LAYER.md).

### STEP=4 acceptance (Driver)

**Layer:** docs + verify (no new feature code).

- MEMORY_MODEL: ThreadPool + Isolate rows; runtime file list.
- PLAN §8 / queue: ISOLATE closed; FFI STEP=1 next; Supervisor deferred stub.
- Archive this track; `mlcc --check-only main` + smoke green.

### STEP=3 acceptance (Driver)

**Layer:** `runtime/` only.

- StopSource on ThreadPool/Isolate; `token` / `request_cancel`.
- Cancellable submit/send; `submit_with_token`; shutdown cancels then joins.
- Unit tests: cancel unblocks blocked submit/send; token observed.
- Verify: `runtime/test/run_concurrency_smoke.sh` exit 0.

### STEP=2 acceptance (Driver)

**Layer:** `runtime/` only.

- `Isolate<State,Message>` in `isolate.hpp`.
- Bounded mailbox, single owner thread, serial handler, Block overflow.
- Unit + smoke; include from `mlc.hpp`.
- Verify: smoke exit 0.

### STEP=1 acceptance (Driver)

**Layer:** `runtime/` only.

- `ThreadPool` in `thread_pool.hpp`: workers + bounded Channel queue.
- `submit`, `shutdown` / dtor drain+join; full queue blocks.
- Unit + smoke; include from `mlc.hpp`.
- Verify: smoke exit 0.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | `ThreadPool` runtime (workers + bounded queue); unit + smoke | **done** |
| 2 | `Isolate` mailbox (bounded channel + single owner thread) | **done** |
| 3 | Wire cancel/`StopToken` into pool/isolate shutdown | **done** |
| 4 | Docs + self-host / close or hand off to Supervisor | **done** |

## Out of scope

- `Supervisor` / restart policies (CONCURRENCY_V2 §28–29).
- async I/O / `IoReactor`.
- MLC `scope |s|` surface (still deferred from TASKSCOPE).
- DropNewest / DropOldest / Reject mailbox policies.
