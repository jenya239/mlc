# Track: Concurrency TaskScope / cancellation propagation

Parent: [../PLAN.md](../PLAN.md) Фаза 8; spec:
[../CONCURRENCY_V2.md](../CONCURRENCY_V2.md) phases 5–7 (Sync surface polish,
`TaskScope`, cancellation propagation). Predecessor closed:
[../archive/tracks/TRACK_CONCURRENCY_V2.md](../archive/tracks/TRACK_CONCURRENCY_V2.md)
(Send/Sync predicates, rendezvous + Sender/Receiver, move/E087/E088, StopToken).

## Status: **open** — STEP=1 done; STEP=2 next

**Driver 2026-07-09:** STEP=1 — `ChannelStatus::{Ok,Closed,Cancelled}`;
`send`/`receive(StopToken)` wake via `stop_callback`; tests in `test_channel.cpp`.

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
| 2 | `TaskScope` / structured spawn ownership | **next** |
| 3 | Sync-safe shared capture without `move` (checker) | pending |
| 4 | Self-host + docs; close or hand off to Isolate/ThreadPool track | pending |

## Out of scope

- `Isolate`, `ThreadPool`, `Supervisor`, async I/O — later tracks.
