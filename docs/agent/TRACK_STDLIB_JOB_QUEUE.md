# Track: Job queue / scheduler (stdlib)

Parent: [../PLAN.md](../PLAN.md), [../STDLIB_BACKEND.md](../STDLIB_BACKEND.md) §5.5,
[../archive/tracks/TRACK_STDLIB_WEBSOCKET.md](../archive/tracks/TRACK_STDLIB_WEBSOCKET.md),
[../CONCURRENCY_V2.md](../CONCURRENCY_V2.md).
Trigger: WEBSOCKET **closed**; STDLIB_BACKEND §5 next is job queue/scheduler
over existing `ThreadPool`/`Channel` (no new FFI).

## Status: **open** — STEP=5 next (docs + example + close)

**Planner 2026-07-11:** opened after closed STDLIB_WEBSOCKET Critic. Chose
job-queue over env/config+logging (§5 order: item 5 before filler item 6).
Primitives exist (`mlc::concurrency::ThreadPool`, `Channel`, language `spawn`
under mlcc). Ruby pipeline still has no `spawn`/`Channel` builtins.

**Driver 2026-07-11:** STEP=1 — Decision locked (see below).
**Driver 2026-07-11:** STEP=2 — `job_queue.hpp` + smoke (enqueue/delay/retry).
**Driver 2026-07-11:** STEP=3 — C++-only documented; no MLC registry.
**Driver 2026-07-11:** STEP=4 — `run_job_queue_gate.sh` (smoke 50/0).

## Decision (STEP=1, 2026-07-11)

### Pipeline

- **C++ runtime** under `mlc::concurrency`: `job_queue.hpp` built on existing
  `ThreadPool` (no new socket/FFI).
- **MLC module: deferred for v1** — enqueue needs `std::function` / closures;
  MLC has no reliable extern-callable closure story yet (same class of gap as
  Ruby lacking `spawn`). Pattern = **C++-only like early HTTP router**, not
  Crypto/WebSocket handles.
- Soft include from `mlc.hpp` / concurrency umbrella when implemented.
- Pure-mlcc `spawn` queue library = **follow-up**, not this track.

### API (C++ v1)

```
JobQueue(worker_count, queue_capacity)
bool enqueue(Callable)                    // fire-and-forget; false if shut down
bool schedule_after(delay_ms, Callable)   // timer thread → enqueue when due
void wait_idle()                          // block until in-flight+queued done
                                          // (for tests/shutdown coordination)
void shutdown()                           // cancel accept + join (ThreadPool)
std::size_t pending_count() const         // queued + running (best-effort)
```

Callable = `void()` (same as `ThreadPool::submit`). Exceptions in jobs: catch
in worker wrapper, count as failure for retry; do **not** `std::terminate`.

### Retry

- **Fixed max_attempts** (constructor arg, default **1** = run once).
- On exception or explicit failure callback: re-enqueue until attempts exhausted.
- No exponential backoff in v1 (constant immediate retry).

### Delay

- **`schedule_after(delay_ms, callable)` in v1** (track name includes scheduler).
- Implementation: **timer thread** + min-heap of due times (STEP=2); does not
  sleep on pool workers.

### Completion / error model

- Fire-and-forget enqueue; **`wait_idle()`** for gates (not per-job futures).
- No `last_error` string table required for C++-only v1; `enqueue`/`schedule_after`
  return `bool` (false after shutdown).
- No panic on job failure after retries exhausted (drop / count metric optional).

### Non-goals (locked)

Broker, persistence, cron expressions, MLC `JobQueue` module, mlcc-only spawn
queue, per-job Result handles.

### Open questions — resolved

1. **Pipeline?** C++ on `ThreadPool`; MLC deferred.
2. **Completion?** Fire-and-forget + `wait_idle`.
3. **Retry?** Fixed `max_attempts` (default 1).
4. **Delay?** Yes — `schedule_after` in v1.

## Goal

Minimal **in-process job queue**: enqueue work, run on a worker pool, optional
delay/retry for failed jobs. Enough for backend “background workers” without
a distributed broker (Redis/Sidekiq out of scope).

## Constraints (STDLIB_BACKEND §3)

- Prefer library over new socket/FFI stack — build on `ThreadPool` / `Channel`
  already shipped.
- C++-only v1 (Decision); bool returns; no broker.
- No distributed queue, no persistence, no multi-process broker in v1.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design: API (`enqueue` / `schedule_after` / `shutdown`); sync vs async completion; retry policy; MLC module vs C++-only; which pipeline (Ruby+C++ runtime vs mlcc `spawn`). Document in «Decision». | **done** (2026-07-11: C++ JobQueue on ThreadPool; MLC deferred; wait_idle; fixed retry; schedule_after) |
| 2 | Runtime: `job_queue.hpp` + C++ smoke (N jobs complete; delay; shutdown). | **done** (2026-07-11: timer thread; retry; `run_job_queue_runtime_smoke.sh` 50/0) |
| 3 | Stdlib: **skip MLC** per Decision — document C++-only in STDLIB/MLC notes (or no-op if docs wait for STEP=5). | **done** (2026-07-11: STDLIB §1/§2/§4/§5 + MLC.md JobQueue; no registry) |
| 4 | Gate: script — enqueue N jobs, assert all run; `schedule_after` smoke; shutdown clean. | pending |
| 5 | Docs (`STDLIB_BACKEND.md` / `MLC.md`) + example; close (regression_gate if `compiler/**`). | pending |

<!-- sub-steps STEP=2: 1) JobQueue wrap ThreadPool; 2) schedule_after; 3) test_job_queue.cpp -->
<!-- sub-steps STEP=3: 1) mark C++-only in track/STDLIB; 2) no registry -->
<!-- sub-steps STEP=4: 1) run_job_queue_gate.sh; 2) asserts; 3) SESSION -->
<!-- sub-steps STEP=5: 1) docs+example; 2) archive; 3) Critic -->

## Out of scope (this track)

- Redis / Sidekiq / RabbitMQ / any network broker
- Persistent job store / crash recovery
- Cron expression parser
- Env config + structured logging (separate §5.6 track)
- Validation schemas (§5.7)
- MLC `JobQueue` module / mlcc bare-name resolve (follow-up)

## Open questions

None for v1 — resolved in Decision above.
