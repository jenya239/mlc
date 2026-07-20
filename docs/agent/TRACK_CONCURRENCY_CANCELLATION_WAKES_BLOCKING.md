# Track: Concurrency Cancellation Wakes Blocking

Parent: [TRACK_MLC_CONCURRENCY_REFINEMENT.md](TRACK_MLC_CONCURRENCY_REFINEMENT.md) §47 **#5**.
Source: [CONCURRENCY_V2.md](../CONCURRENCY_V2.md) §8/§9/§44. Size **L**.

## Status: **active** (2026-07-20) — STEP=2 done → Driver STEP=3

## Next step

**STEP=3** — MEMORY_MODEL sync (MLC Cancelled / StopToken).

## Decision (STEP=0) — **frozen** 2026-07-20

| Item | Choice |
|------|--------|
| Problem | C++ already wakes `Channel::send`/`receive(StopToken)` → `ChannelStatus::Cancelled` (`channel.hpp`, `stress_channel`, TRACK_CONCURRENCY_TASKSCOPE STEP=1). **Gap:** MLC `c.send` → `bool`, `c.recv` → `Option[T]` only — no token arg, no `Cancelled`. `StopSource`/`StopToken` runtime-only (MEMORY_MODEL). `TaskScope` has C++ `token()`/`request_cancel()` but MLC exposes only `scope` + `.spawn`. Backlog text «no cancellation at all» is **stale** for C++; true for MLC surface |
| Boundaries in scope | (1) MLC surface for `StopSource`/`StopToken` **or** `TaskScope.token` + `request_cancel` (pick one primary cancel driver in STEP=2). (2) MLC `Channel.send`/`recv` overloads that take `StopToken` and distinguish **Ok / Closed / Cancelled** (not `Option` alone). (3) Runtime smoke compiled from MLC: blocked empty-channel `recv(token)` observes `Cancelled` after `request()` / scope cancel, without channel activity. (4) Keep existing non-token `send`/`recv` behavior |
| Boundaries out | `sleep` / `Timer.wait` / `Socket.read`/`accept` cancel wake; `Thread.spawn(stop.token)` full §8 API; Isolate/ThreadPool MLC (#9/#10); rendezvous/unbounded (#6); Atomics (#7); changing C++ cancel path that already works; `lib/mlc/` |
| Wire location | checker channel method types + registry/codegen for Stop*/token methods; thin MLC↔C++ naming; smoke under `compiler/tests/` or `runtime/test` driven by mlcc-built binary |
| Token / gate | (a) red: MLC program with `recv(token)` / cancel API fails today; (b) green: blocked `recv` → `Cancelled` after cancel; (c) non-token `recv` still `Option`; (d) `mlcc --check-only`; Tier B + self-host if `compiler/` touched |
| Docs | MEMORY_MODEL: MLC cancel surface; Channel Ok/Closed/Cancelled; cite this track |
| REG | yes if `lib/mlc/` touched (expect **no**); else skip |
| Out of scope | sleep/socket cancel; user-declared cancel traits; async/await |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: MLC cannot cancel-wake blocked `Channel.recv` | **done** — `ok channel_recv_cancel_red` |
| 2 | Wire MLC Stop*/token + cancellable send/recv; green smoke; Tier B; self-host | **done** — `ok channel_recv_cancel` + smoke; Tier B (arch lint allowlist method_gen); p1≡p2 |
| 3 | MEMORY_MODEL sync (MLC Cancelled / StopToken) | open |
| 4 | Critic: gates; archive | open |

## Out of scope

§47 #6–#11; sleep/socket cancel; Isolate/ThreadPool MLC; `lib/mlc/`.
