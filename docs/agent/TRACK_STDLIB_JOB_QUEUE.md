# Track: Job queue / scheduler (stdlib)

Parent: [../PLAN.md](../PLAN.md), [../STDLIB_BACKEND.md](../STDLIB_BACKEND.md) §5.5,
[../archive/tracks/TRACK_STDLIB_WEBSOCKET.md](../archive/tracks/TRACK_STDLIB_WEBSOCKET.md),
[../CONCURRENCY_V2.md](../CONCURRENCY_V2.md).
Trigger: WEBSOCKET **closed**; STDLIB_BACKEND §5 next is job queue/scheduler
over existing `ThreadPool`/`Channel` (no new FFI).

## Status: **open** — STEP=1 next (API Decision)

**Planner 2026-07-11:** opened after closed STDLIB_WEBSOCKET Critic. Chose
job-queue over env/config+logging (§5 order: item 5 before filler item 6).
Primitives exist (`mlc::concurrency::ThreadPool`, `Channel`, language `spawn`
under mlcc). Ruby pipeline still has no `spawn`/`Channel` builtins.

## Goal

Minimal **in-process job queue**: enqueue work, run on a worker pool, optional
delay/retry for failed jobs. Enough for backend “background workers” without
a distributed broker (Redis/Sidekiq out of scope).

## Constraints (STDLIB_BACKEND §3)

- Prefer library over new socket/FFI stack — build on `ThreadPool` / `Channel`
  / `spawn` already shipped.
- Thin surface; errors → `Option`/`bool`+`last_error` **or** Decision-locked
  alternative — **lock in STEP=1**; avoid Decision/ship split.
- No distributed queue, no persistence, no multi-process broker in v1.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design: API (`enqueue` / `schedule_after` / `shutdown`); sync vs async completion; retry policy; MLC module vs C++-only; which pipeline (Ruby+C++ runtime vs mlcc `spawn`). Document in «Decision». | **pending** |
| 2 | Runtime and/or MLC core: queue + workers; C++ or mlcc smoke (N jobs complete; cancel/shutdown). | pending |
| 3 | Stdlib module (if Decision says MLC) + registry/bridge **or** document C++-only. | pending |
| 4 | Gate: script — enqueue N jobs, assert all run once (or retry count); shutdown clean. | pending |
| 5 | Docs (`STDLIB_BACKEND.md` / `MLC.md`) + example; close (regression_gate if `compiler/**`). | pending |

<!-- sub-steps STEP=1: 1) API table; 2) pipeline Ruby/C++/mlcc; 3) retry/delay; 4) non-goals broker -->
<!-- sub-steps STEP=2: 1) queue impl; 2) smoke; 3) shutdown -->
<!-- sub-steps STEP=3: 1) mlc module or skip; 2) registry; 3) codegen -->
<!-- sub-steps STEP=4: 1) gate script; 2) asserts; 3) SESSION -->
<!-- sub-steps STEP=5: 1) docs+example; 2) archive; 3) Critic -->

## Out of scope (this track)

- Redis / Sidekiq / RabbitMQ / any network broker
- Persistent job store / crash recovery
- Cron expression parser (simple `schedule_after(ms)` is enough for v1 if chosen)
- Env config + structured logging (separate §5.6 track)
- Validation schemas (§5.7)

## Open questions (resolve in STEP=1)

1. **Pipeline:** C++ `mlc::concurrency` JobQueue + Ruby `std/` module (Crypto
   pattern) vs pure MLC under **mlcc** (`spawn`/`Channel`) vs both?
2. **Completion:** fire-and-forget only, or wait/join handle / channel of results?
3. **Retry:** none in v1, fixed N retries, or backoff?
4. **Delay:** `enqueue` only, or `schedule_after(delay_ms, …)`?
