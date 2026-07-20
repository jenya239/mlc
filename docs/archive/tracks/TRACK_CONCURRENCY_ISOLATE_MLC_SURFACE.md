# Track: Concurrency Isolate MLC Surface

Parent: [TRACK_MLC_CONCURRENCY_REFINEMENT.md](../../agent/TRACK_MLC_CONCURRENCY_REFINEMENT.md) §47 **#9**.
Source: [CONCURRENCY_V2.md](../CONCURRENCY_V2.md) §16/§17/§44 phase 8. Size **L**.

## Status: **closed** (2026-07-21) — Critic OK

**Critic 2026-07-21 (STEP=4):** Re-ran `run_isolate_mlc.sh` + fixture/`main.mlc`
`--check-only` + `test_isolate`. Anti-false-done: `07ee30cd`…`659b268a` (STEP=0–3);
no `lib/mlc/` → REG skip. Wire: MLC `Isolate.start`/`send`/`shutdown`/
`state_after_shutdown`; Block overflow; Msg Send (**E092**); Isolate !Send/!Sync;
MEMORY_MODEL + CONCURRENCY_V2 §0/§16/§17/§43/§44 synced. **reopen: none**.

Honest residual: DropNewest/DropOldest/Reject deferred; MLC multi-thread concurrent
senders need shareable handle (Isolate !copy/!Send — concurrent serial gate remains
`runtime/test/test_isolate.cpp`); named typed handler preferred (lambda Msg may be Unknown).

| Gate | Result |
|------|--------|
| `run_isolate_mlc.sh` | `ok isolate_mlc` EXIT=0 (check-only + link + serial + test_isolate) |
| fixture `--check-only` | EXIT=0 |
| `mlcc --check-only compiler/main.mlc` | EXIT=0 |
| `test_isolate` | ALL 879 checks PASSED |
| Tier B / self-host | STEP=2: Tier B EXIT=0; p1≡p2 DIFF_EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§47 `#10 CONCURRENCY_SUPERVISOR_MLC_SURFACE`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `07ee30cd` | Decision freeze + open |
| 1 | `2a0cf9bc` | Red harness (E001 gap) |
| 2 | `eb51a99e` | MLC wire; Tier B + self-host |
| 3 | `2fd1bd9c` (+ `659b268a`) | MEMORY_MODEL + CONCURRENCY_V2 sync |
| 4 | this Critic | close + archive |

## Decision (STEP=0) — **frozen** 2026-07-21

| Item | Choice |
|------|--------|
| Problem | CONCURRENCY_V2 §16/§17: `Isolate[State, Msg]` with bounded mailbox + serial handler. **Already present (C++):** `runtime/include/mlc/concurrency/isolate.hpp` — owner thread, Block overflow, `send`/`shutdown`/`token`, smoke `runtime/test/test_isolate.cpp` (serial under concurrent senders). MEMORY_MODEL row is C++-only. **Real gap:** no MLC name/`Isolate` in checker/codegen (probed 2026-07-21: `Isolate` → **E001**; design `Isolate[S,M].start(...) \|state, msg\| {…}` parse-fails). Same historical “closure gap” as JobQueue/Supervisor MLC deferral — TaskScope/`spawn` already prove trailing/`do` closures are shippable |
| Boundaries in scope | (1) MLC surface: `Isolate[State, Msg]` + `start` (initial state, capacity, handler) + `send` + `shutdown` (and/or dtor). (2) Overflow **Block only** (match C++ v1). (3) Gate: MLC e2e concurrent senders → serial handler (exact sum / no re-entry). (4) Checker: Send on Msg; State owned by isolate (no free concurrent mut). (5) Docs MEMORY_MODEL / CONCURRENCY_V2 §16–§17 |
| Boundaries out | DropNewest/DropOldest/Reject overflow (C++ deferred; not required for backlog gate); Supervisor (#10); TestRuntime (#11); ThreadPool MLC; `lib/mlc/`; rewriting C++ Isolate API; full actor model / ask-pattern replies |
| Wire location | `compiler/` registry + infer/codegen for Isolate methods; possibly thin MLC prelude; reuse `isolate.hpp`; tests under `compiler/tests/` + optional runtime smoke already green |
| Token / gate | (a) red: Isolate MLC undefined / parse fail; (b) green: `--check-only` + linked e2e concurrent senders serial; (c) `mlcc --check-only`; Tier B + self-host on every `compiler/` STEP |
| Docs | MEMORY_MODEL Isolate row → MLC surface; CONCURRENCY_V2 §16/§17/§44 status; cite track |
| REG | yes if `lib/mlc/` touched (expect **no**); else skip |
| Out of scope | #10–#11; Drop*/Reject policies; JobQueue MLC |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: Isolate MLC missing / not constructible | **done** — `ok isolate_mlc_red` (E001; `isolate.hpp` present) |
| 2 | Wire MLC Isolate start/send/shutdown; green e2e serial; Tier B; self-host | **done** — `ok isolate_mlc`; Tier B EXIT=0; self-host DIFF=0 |
| 3 | MEMORY_MODEL + CONCURRENCY_V2 §16/§17 sync | **done** — MLC Isolate.start/send/shutdown; Block-only; !Send/!Sync |
| 4 | Critic: gates; archive | **done** — closed |

## Out of scope

§47 #10–#11; Drop*/Reject; `lib/mlc/`.
