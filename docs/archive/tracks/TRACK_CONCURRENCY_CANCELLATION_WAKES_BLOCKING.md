# Track: Concurrency Cancellation Wakes Blocking

Parent: [TRACK_MLC_CONCURRENCY_REFINEMENT.md](../../agent/TRACK_MLC_CONCURRENCY_REFINEMENT.md) §47 **#5**.
Source: [CONCURRENCY_V2.md](../CONCURRENCY_V2.md) §8/§9/§44. Size **L**.

## Status: **closed** (2026-07-20) — Critic OK

**Critic 2026-07-20 (STEP=4):** Re-ran `run_channel_recv_cancel.sh` + smoke +
`--check-only main`. Anti-false-done: `7b8a33c7`…`692d1489` (STEP=0–3); no `lib/mlc/`
→ REG skip. Wire: `StopSource.new`/`.token`/`.request`; `recv(token)` →
`ChannelReceiveResult` + `.cancelled()`; MEMORY_MODEL cites MLC cancel surface.
**reopen: none**.

Honest residual: sleep/socket cancel deferred; TaskScope.token/request_cancel MLC
not primary (StopSource chosen); concurrent blocked-recv wake covered by C++
stress + pre-requested smoke.

| Gate | Result |
|------|--------|
| `run_channel_recv_cancel.sh` | `ok channel_recv_cancel` EXIT=0 |
| `run_channel_recv_cancel_smoke.sh` | `ok channel_recv_cancel_smoke` EXIT=0 |
| `mlcc --check-only compiler/main.mlc` | EXIT=0 |
| Tier B / self-host | STEP=2: arch lint allowlist method_gen; p1≡p2 DIFF_EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§47 `#6 CONCURRENCY_CHANNEL_RENDEZVOUS_UNBOUNDED`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `7b8a33c7` | Decision freeze + open |
| 1 | `caa22873` | Red harness (E001/E014 gap) |
| 2 | `dcc9f2ea` | StopSource + recv(token); smoke; Tier B + self-host |
| 3 | `692d1489` | MEMORY_MODEL sync |
| 4 | this Critic | close + archive |

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
| 3 | MEMORY_MODEL sync (MLC Cancelled / StopToken) | **done** |
| 4 | Critic: gates; archive | **done** — closed |

## Out of scope

§47 #6–#11; sleep/socket cancel; Isolate/ThreadPool MLC; `lib/mlc/`.
