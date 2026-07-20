# Track: Concurrency Channel Rendezvous / Unbounded

Parent: [TRACK_MLC_CONCURRENCY_REFINEMENT.md](TRACK_MLC_CONCURRENCY_REFINEMENT.md) §47 **#6**.
Source: [CONCURRENCY_V2.md](../CONCURRENCY_V2.md) §10/§44. Size **M**.

## Status: **active** (2026-07-20) — STEP=2 done → Driver STEP=3

## Next step

**STEP=3** — MEMORY_MODEL + CONCURRENCY_V2 status sync.

## Decision (STEP=0) — **frozen** 2026-07-20

| Item | Choice |
|------|--------|
| Problem | Backlog/CONCURRENCY_V2 §0/§10 say `capacity == 0` throws — **stale**. C++ rendezvous already green (`channel.hpp` capacity 0 handoff; `test_channel.cpp`; TRACK_CONCURRENCY_V2 STEP=2). MLC `make_channel(0usize)` `--check-only` EXIT=0. **Real gap:** no `unbounded` variant (no `make_channel_unbounded` / `Channel.unbounded` in runtime; producer can block forever only on bounded full). Docs still claim rendezvous missing |
| Boundaries in scope | (1) C++ `make_unbounded_channel` (never blocks on full; grow queue; keep close/cancel wake). (2) MLC surface for `make_unbounded_channel`. (3) Runtime smoke: unbounded accepts sends past any fixed capacity without blocking; rendezvous `make_channel(0)` remains green. (4) Docs: MEMORY_MODEL + CONCURRENCY_V2 status rows (rendezvous done; unbounded added) |
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
| 1 | Red: unbounded API missing | **done** — `ok channel_unbounded_red` |
| 2 | Wire unbounded (+ MLC); green smokes; Tier B; self-host if compiler/ | **done** — `ok channel_unbounded` + smoke; C++ test; p1≡p2 |
| 3 | MEMORY_MODEL + CONCURRENCY_V2 status sync | open |
| 4 | Critic: gates; archive | open |

## Out of scope

§47 #7–#11; lock-free; Isolate overflow policies; `lib/mlc/`.
