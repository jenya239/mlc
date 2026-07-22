# Track: MLC concurrency refinement — atomic items from CONCURRENCY_V2.md gap audit

Parent: [../PLAN.md](../PLAN.md) §47. Source: [../CONCURRENCY_V2.md](../CONCURRENCY_V2.md)
(design doc, §0 "Что уже есть" + §44 "Порядок реализации" phase table) — cross-checked
against current code 2026-07-19 (`compiler/checker/send_safe.mlc`,
`runtime/include/mlc/concurrency/*.hpp`). Authorized 2026-07-19 (user command: "поставь
в треки именно доработку mlc по многопоточности") — Driver opens STEP=1 of the next
**pending** item in order without further permission, same rule as §46.

## Status: **done** (2026-07-22) — `#1`–`#11` closed (Critic OK on `#11` TESTRUNTIME)

## How to use this backlog

Same discipline as [TRACK_EDITOR_UX_BACKLOG](TRACK_EDITOR_UX_BACKLOG.md): each item gets
its own Decision→Driver→Driver→Critic cycle and its own `TRACK_CONCURRENCY_<NAME>.md`
file created when picked up (do not pre-create all of them now). This backlog is scoped
to **language/checker/runtime primitives**, not applications built on top of them — items
like the video-player or terminal-component explorations discussed earlier are out of
scope here.

Order below follows CONCURRENCY_V2.md §20's own stated dependency chain (`Thread → Send/
Sync → Mutex → Atomics → BoundedChannel → TaskScope → Cancellation → ThreadPool →
Isolate`, async/await explicitly deferred past all of this) plus §44's phase table.
Planner picks the next **pending** item in order, does not jump ahead unless the current
one is blocked.

| # | Track name | Scope (one line) | Gate scenario | Size | Status |
|---|-----------|-------------------|----------------|------|--------|
| 1 | `CONCURRENCY_MUTABLE_CAPTURE_CHECK` | Checker rejects mutable-value capture (no `move`/`Arc`/`Channel`) across a concurrency boundary (`spawn`, `TaskScope.spawn`, `Channel.send`) as a compile error, per CONCURRENCY_V2.md §1/§4. Acceptance bar: the already-documented known limitation (`MEMORY_MODEL.md` §Известные ограничения п.2 — COW detach race via `spawn { array.push(x) }` on a shared variable) must become **unreachable from safe code**, not just documented | new checker diagnostic test: `spawn` capturing a non-moved mutable local is a compile error; existing race repro from `MEMORY_MODEL.md` now fails to compile | L | **done** — [archive/tracks/TRACK_CONCURRENCY_MUTABLE_CAPTURE_CHECK](../archive/tracks/TRACK_CONCURRENCY_MUTABLE_CAPTURE_CHECK.md) |
| 2 | `CONCURRENCY_SEND_BOUND` | Generalize `type_is_send_safe` (`compiler/checker/send_safe.mlc`) from a `Channel.send`-only helper into a real `Send` bound enforced at every concurrency boundary (`spawn`, `TaskScope.spawn`, `Channel.send`), per §2/§44 phase 3 | new checker test: free capture of a `!Send` type (e.g. `Shared[i32]`; no MLC `RawPtr` surface) in `spawn`/`TaskScope.spawn` is a compile error (**E092**), matching existing `Channel.send` behavior | M | **done** — [archive/tracks/TRACK_CONCURRENCY_SEND_BOUND](../archive/tracks/TRACK_CONCURRENCY_SEND_BOUND.md) |
| 3 | `CONCURRENCY_MOVE_TRACKING` | Simple move-state tracking (not full borrow checker) so `spawn(move job) { ... }` forbids `use(job)` after the move, per §4/§44 phase 4 | new checker test: `use` of a moved-into-`TaskScope.spawn` binding after the `.spawn` call is **E088** (bare `spawn` already green) | M | **done** — [archive/tracks/TRACK_CONCURRENCY_MOVE_TRACKING](../archive/tracks/TRACK_CONCURRENCY_MOVE_TRACKING.md) |
| 4 | `CONCURRENCY_SYNC_TRAIT` | Introduce `Sync` trait (name per CONCURRENCY_V2.md §⚠️ recommendation 1 — avoids collision with the existing `Shared<T>`/`std::shared_ptr` type), compositional inference per §3 (`Atomic[T]`/`Mutex[T]`/`Arc[ImmutableConfig]` → `Sync`; `HashMap`/mutable `Array` → `!Sync`) | new checker test: `Arc[Config]` usable without explicit keyword across a boundary that requires `Sync`; mutable `Array` rejected | M | **done** — [archive/tracks/TRACK_CONCURRENCY_SYNC_TRAIT](../archive/tracks/TRACK_CONCURRENCY_SYNC_TRAIT.md) |
| 5 | `CONCURRENCY_CANCELLATION_WAKES_BLOCKING` | MLC surface for cancel-wake on blocked `Channel.send`/`recv` (C++ `StopToken` path already green) → `Cancelled`, per §8/§9; sleep/socket deferred | MLC smoke: blocked empty-channel `recv(token)` observes `Cancelled` after `stop.request()` / `TaskScope` cancel | L | **done** — [archive/tracks/TRACK_CONCURRENCY_CANCELLATION_WAKES_BLOCKING](../archive/tracks/TRACK_CONCURRENCY_CANCELLATION_WAKES_BLOCKING.md) |
| 6 | `CONCURRENCY_CHANNEL_RENDEZVOUS_UNBOUNDED` | Rendezvous `capacity 0` already C++ green (docs stale); add explicit **unbounded** channel + MLC surface, per §10 | runtime/MLC smoke: unbounded accepts sends past any fixed capacity; rendezvous handoff remains green | M | **done** — [archive/tracks/TRACK_CONCURRENCY_CHANNEL_RENDEZVOUS_UNBOUNDED](../archive/tracks/TRACK_CONCURRENCY_CHANNEL_RENDEZVOUS_UNBOUNDED.md) |
| 7 | `CONCURRENCY_ATOMICS` | `Atomic[Bool/Int32/Int64/UInt64]` with `load/store/exchange/compare_exchange/fetch_add/fetch_sub`, sequentially-consistent only (no `.acquire`/`.release` in v1), per §14 | e2e: concurrent `fetch_add` from N threads yields exact expected sum, TSan-clean | M | **done** — [archive/tracks/TRACK_CONCURRENCY_ATOMICS](../archive/tracks/TRACK_CONCURRENCY_ATOMICS.md) |
| 8 | `CONCURRENCY_FFI_METADATA` | `extern c` type/fn concurrency attributes (`thread_safe`/`sendable`/`blocking`/`callback_concurrent`/`thread_affine(MainThread)`) + checker enforcement of thread affinity at call sites, per §26/§27 | checker test: calling a `thread_affine(MainThread)`-tagged extern fn from inside `spawn`/`TaskScope.spawn` is a compile error | M | **done** — [archive/tracks/TRACK_CONCURRENCY_FFI_METADATA](../archive/tracks/TRACK_CONCURRENCY_FFI_METADATA.md) |
| 9 | `CONCURRENCY_ISOLATE_MLC_SURFACE` | Expose `Isolate[State, Msg]` at the MLC level (today C++-only, `runtime/include/mlc/concurrency/` — MLC syntax deferred per §16 "same closure gap as JobQueue") with a bounded mailbox and explicit overflow policy (`Block \| DropNewest \| DropOldest \| Reject`), per §16/§17/§44 phase 8 | e2e: `Isolate.start(...)` from MLC source, `room.send(...)` observed serialized (no concurrent handler re-entry) under concurrent senders | L | **done** — [archive/tracks/TRACK_CONCURRENCY_ISOLATE_MLC_SURFACE](../archive/tracks/TRACK_CONCURRENCY_ISOLATE_MLC_SURFACE.md) |
| 10 | `CONCURRENCY_SUPERVISOR_MLC_SURFACE` | Expose `Supervisor` (C++ v1 done, [archive/tracks/TRACK_CONCURRENCY_SUPERVISOR](../archive/tracks/TRACK_CONCURRENCY_SUPERVISOR.md)) at the MLC level — same closure-capture gap noted in §28 | e2e: permanent restart + Temporary sibling unaffected (method API; block sugar deferred) | M | **done** — [archive/tracks/TRACK_CONCURRENCY_SUPERVISOR_MLC_SURFACE](../archive/tracks/TRACK_CONCURRENCY_SUPERVISOR_MLC_SURFACE.md) |
| 11 | `CONCURRENCY_TESTRUNTIME_MLC_SURFACE` | Expose `TestRuntime.new(seed:)` deterministic scheduler (C++ v1 done, `TestScheduler`/`TestMutex`/`TestChannel`) at the MLC level, per §34 | e2e: same `seed` reproduces the same interleaving across two runs, from MLC source | M | **done** — [archive/tracks/TRACK_CONCURRENCY_TESTRUNTIME_MLC_SURFACE](../archive/tracks/TRACK_CONCURRENCY_TESTRUNTIME_MLC_SURFACE.md) |

## Non-goals (frozen, per CONCURRENCY_V2.md §20/§41)

- `Future`/`async`/`await`/`IoReactor`/`AsyncSocket`/`select` — explicitly ordered
  **after** everything above in the source design doc; do not pull forward.
- `parallel_map` (§42) — sugar on top of `Send` (#2) once that lands; not its own item.
- Lock-free channel internals — doc §12 explicitly says mutex+condvar first, no
  profile-driven need to change yet.
- Full Rust-style borrow checker — §2/§4 explicitly scope to compositional `Send`/`Sync`
  bounds + simple move-state, not general lifetimes.
- Clang `-Wthread-safety` annotation generation (§37) — "second echelon", not MVP.
- Applications on top of these primitives (video player, terminal component, chat
  server reference scenario) — those are consumers, not part of this refinement.
