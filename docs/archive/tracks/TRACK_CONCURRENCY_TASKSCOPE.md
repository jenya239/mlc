# Track: Concurrency TaskScope / cancellation propagation

Parent: [../../PLAN.md](../../PLAN.md) Фаза 8; spec:
[../../CONCURRENCY_V2.md](../../CONCURRENCY_V2.md) phases 5–7 (Sync surface polish,
`TaskScope`, cancellation propagation). Predecessor closed:
[TRACK_CONCURRENCY_V2.md](TRACK_CONCURRENCY_V2.md)
(Send/Sync predicates, rendezvous + Sender/Receiver, move/E087/E088, StopToken).

## Status: **closed** — STEP=1–4 done (2026-07-09)

**Closed note:** Channel cancel wake + `Cancelled`; C++ `TaskScope`; Sync-safe
Mutex/Arc spawn capture (`type_is_sync_safe_shared`); MEMORY_MODEL + PLAN updated;
self-host p1≡p2. Deferred: MLC `scope |s|` syntax, child-error policy, Isolate/
ThreadPool → [../../agent/TRACK_CONCURRENCY_ISOLATE.md](../../agent/TRACK_CONCURRENCY_ISOLATE.md).
HARNESS T5 (cancel stress matrix) unblocked.

### STEP=4 acceptance (Driver)

**Layer:** docs + verify (no new `compiler/`/`runtime/` feature code).

- Self-host: `mlcc` → p1 → `mlcc2` → p2; `diff -rq` exclude obj empty.
- MEMORY_MODEL: Sync-safe spawn capture + wake-on-cancel + TaskScope runtime.
- PLAN §8 / queue: TASKSCOPE closed; next HARNESS T5 or Isolate handoff.
- Archive this track; smoke green.

### STEP=3 acceptance (Driver)

**Layer:** `compiler/` only (not `lib/mlc/` / not `runtime/` in the same turn).

- `spawn_capture.mlc`: `type_is_sync_safe_shared` — Mutex always; Arc iff T Sync.
- Typed/heuristic binding types for untyped `let mut x = Mutex.new(...)`.
- `let mut` i32 / Array still E087; Sync-safe Mutex/Arc capture OK.
- Tests in `test_spawn.mlc`; verify via `mlcc --check-only` probes + `main.mlc`.
- Out of scope: full Sync trait surface; immutable Sync-only table expansion.

### STEP=2 acceptance (Driver)

**Layer:** `runtime/` only (not `compiler/` / not `lib/mlc/` in the same turn).

- `TaskScope` in `runtime/include/mlc/concurrency/task_scope.hpp`.
- `spawn(callable)`, `spawn_with_token(callable(StopToken))`, `join()`,
  `request_cancel()` / `token()`.
- Destructor: `request_cancel()` then `join()` (no orphan children).
- Unit test + smoke wire; include from `mlc.hpp`.
- Out of scope: MLC `scope |s|` syntax, error propagation policy, typed Task handles.
- Verify: `runtime/test/run_concurrency_smoke.sh` exit 0.

### STEP=1 acceptance (Driver)

**Layer:** `runtime/` only (not `compiler/` / not `lib/mlc/` in the same turn).

- `ChannelStatus` + `ChannelReceiveResult<T>` in `channel.hpp`.
- Overloads: `send(value, StopToken)` → status; `receive(StopToken)` → result.
- Blocked wait wakes on `StopSource::request()` (`std::stop_callback` → `wake_all`).
- Legacy `bool`/`optional` APIs unchanged.
- Tests: cancel-unblocks send/recv + ok path; smoke green.
- Out of scope: TestChannel cancel, MLC surface, TaskScope construct.
- Verify: `runtime/test/run_concurrency_smoke.sh` exit 0.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Cancel wake for Channel send/recv + `Cancelled` outcome; unlock HARNESS T5 | **done** |
| 2 | `TaskScope` / structured spawn ownership | **done** |
| 3 | Sync-safe shared capture without `move` (checker) | **done** |
| 4 | Self-host + docs; close or hand off to Isolate/ThreadPool track | **done** |

## Out of scope

- `Isolate`, `ThreadPool`, `Supervisor`, async I/O — later tracks.
- MLC `scope |s|` syntax + TaskScope error propagation policy.
