# Track: Concurrency Channel Rendezvous / Unbounded

Parent: [TRACK_MLC_CONCURRENCY_REFINEMENT.md](TRACK_MLC_CONCURRENCY_REFINEMENT.md) §47 **#6**.
Source: [CONCURRENCY_V2.md](../CONCURRENCY_V2.md) §10/§44. Size **M**.

## Status: **active** (2026-07-20) — STEP=0 done → Driver STEP=1

## Next step

**STEP=1** — red harness: no MLC/C++ `unbounded` channel API (rendezvous `make_channel(0)` already check-only green).

## Decision (STEP=0) — **frozen** 2026-07-20

| Item | Choice |
|------|--------|
| Problem | Backlog/CONCURRENCY_V2 §0/§10 say `capacity == 0` throws — **stale**. C++ rendezvous already green (`channel.hpp` capacity 0 handoff; `test_channel.cpp`; TRACK_CONCURRENCY_V2 STEP=2). MLC `make_channel(0usize)` `--check-only` EXIT=0. **Real gap:** no `unbounded` variant (no `make_channel_unbounded` / `Channel.unbounded` in runtime; producer can block forever only on bounded full). Docs still claim rendezvous missing |
| Boundaries in scope | (1) C++ `unbounded` channel: never blocks on full (grow queue; keep close/cancel wake). Pick API name in STEP=2: `make_unbounded_channel` **or** `Channel.unbounded()` / capacity sentinel — one primary. (2) MLC surface wired to that API. (3) Runtime smoke: unbounded accepts sends past any fixed capacity without blocking; rendezvous `make_channel(0)` handoff smoke from MLC (or C++ already covered — prefer MLC if cheap). (4) Docs: MEMORY_MODEL + CONCURRENCY_V2 status rows (rendezvous done; unbounded added) |
| Boundaries out | Lock-free channel; DropNewest/Oldest/Reject overflow policies; Isolate mailbox (#9); changing existing rendezvous/buffered semantics; `lib/mlc/`; Atomics (#7) |
| Wire location | `runtime/include/mlc/concurrency/channel.hpp` (+ tests); MLC registry/codegen for unbounded ctor; smoke under `compiler/tests/` or `runtime/test` |
| Token / gate | (a) red: unbounded API absent / fails today; (b) green: unbounded send past capacity OK; rendezvous `make_channel(0)` no throw + handoff; (c) `mlcc --check-only`; Tier B + self-host if `compiler/` touched |
| Docs | MEMORY_MODEL: capacity 0 rendezvous (cite); unbounded API; CONCURRENCY_V2 §0/§10 status sync |
| REG | yes if `lib/mlc/` touched (expect **no**); else skip |
| Out of scope | lock-free; overflow policies other than Block/unbounded grow |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: unbounded API missing | open |
| 2 | Wire unbounded (+ MLC); green smokes; Tier B; self-host if compiler/ | open |
| 3 | MEMORY_MODEL + CONCURRENCY_V2 status sync | open |
| 4 | Critic: gates; archive | open |

## Out of scope

§47 #7–#11; lock-free; Isolate overflow policies; `lib/mlc/`.
