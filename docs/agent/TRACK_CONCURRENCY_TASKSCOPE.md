# Track: Concurrency TaskScope / cancellation propagation

Parent: [../PLAN.md](../PLAN.md) Фаза 8; spec:
[../CONCURRENCY_V2.md](../CONCURRENCY_V2.md) phases 5–7 (Sync surface polish,
`TaskScope`, cancellation propagation). Predecessor closed:
[../archive/tracks/TRACK_CONCURRENCY_V2.md](../archive/tracks/TRACK_CONCURRENCY_V2.md)
(Send/Sync predicates, rendezvous + Sender/Receiver, move/E087/E088, StopToken).

## Status: **open** — STEP=1 next (Planner may refine)

Handoff from CONCURRENCY_V2 STEP=6 (2026-07-09): phases 1–4 of the V2 roadmap
are done in the predecessor track. This track owns structured concurrency and
cancel wake-up.

### STEP=1 acceptance (draft)

**Layer:** `runtime/` first (or Planner splits).

- Document / implement `TaskScope` API shape per CONCURRENCY_V2 §6.
- Wire `StopToken` into blocked `Channel` send/recv wake (Cancelled outcome) —
  unblocks HARNESS T5.
- Verify: concurrency smoke + targeted cancel tests.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Cancel wake for Channel send/recv + `Cancelled` outcome; unlock HARNESS T5 | **next** |
| 2 | `TaskScope` / structured spawn ownership | pending |
| 3 | Sync-safe shared capture without `move` (checker) | pending |
| 4 | Self-host + docs; close or hand off to Isolate/ThreadPool track | pending |

## Out of scope

- `Isolate`, `ThreadPool`, `Supervisor`, async I/O — later tracks.
