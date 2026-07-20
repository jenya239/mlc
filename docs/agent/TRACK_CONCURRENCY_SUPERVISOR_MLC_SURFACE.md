# Track: Concurrency Supervisor MLC Surface

Parent: [TRACK_MLC_CONCURRENCY_REFINEMENT.md](TRACK_MLC_CONCURRENCY_REFINEMENT.md) §47 **#10**.
Source: [CONCURRENCY_V2.md](../CONCURRENCY_V2.md) §28/§29/§44 phase 10.
C++ predecessor: [archive/tracks/TRACK_CONCURRENCY_SUPERVISOR](../archive/tracks/TRACK_CONCURRENCY_SUPERVISOR.md). Size **M**.

## Status: **active** (2026-07-21) — STEP=1 done → Driver STEP=2

## Next step

**STEP=2** — wire MLC `Supervisor.new` / `.add` / `.start` / `.stop`; green
`run_supervisor_mlc.sh` + restart e2e; Tier B + self-host.

## Decision (STEP=0) — **frozen** 2026-07-21

| Item | Choice |
|------|--------|
| Problem | CONCURRENCY_V2 §28/§29: Supervisor with restart policies + storm intensity. **Already present (C++):** `runtime/include/mlc/concurrency/supervisor.hpp` — `add`/`start`/`stop`/`set_restart_intensity`; `RestartPolicy` permanent/transient/temporary; one_for_one; smoke `runtime/test/test_supervisor.cpp`. Prior track closed **C++-only** (JobQueue-style closure deferral). **Real gap:** no MLC name (probed 2026-07-21: `Supervisor` → **E001**). Isolate MLC (#9) proved named handlers + method API shippable — same path here |
| Boundaries in scope | (1) MLC method API (not `supervisor { }` sugar): `Supervisor.new` / `.add(name, policy, handler)` / `.start` / `.stop` / `.set_restart_intensity` (and/or `.storm_tripped`). (2) `RestartPolicy` surface matching C++ enum. (3) Handler: named fn or lambda taking `StopToken` (Isolate-style). (4) Gate: MLC e2e — permanent child that fails then succeeds is restarted; siblings unaffected (mirror C++ smoke). (5) Docs MEMORY_MODEL / CONCURRENCY_V2 §28–§29 |
| Boundaries out | `supervisor { child(...) {…} }` block syntax (backlog sketch — defer); `one_for_all`/`rest_for_one`; exponential backoff; TestRuntime (#11); JobQueue MLC; rewriting C++ Supervisor API; `lib/mlc/` |
| Wire location | `compiler/` registry + infer/codegen; reuse `supervisor.hpp`; tests under `compiler/tests/` + existing `test_supervisor` |
| Token / gate | (a) red: Supervisor MLC undefined; (b) green: `--check-only` + linked restart e2e; (c) `mlcc --check-only`; Tier B + self-host on every `compiler/` STEP |
| Docs | MEMORY_MODEL Supervisor row; CONCURRENCY_V2 §28/§29 MLC status; cite track |
| REG | yes if `lib/mlc/` touched (expect **no**); else skip |
| Out of scope | #11; block sugar; multi-strategy; backoff |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: Supervisor MLC missing | **done** — `ok supervisor_mlc_red` (E001; `supervisor.hpp` present) |
| 2 | Wire MLC Supervisor add/start/stop (+ intensity); green e2e restart; Tier B; self-host | open |
| 3 | MEMORY_MODEL + CONCURRENCY_V2 §28/§29 sync | open |
| 4 | Critic: gates; archive | open |

## Out of scope

§47 #11; `supervisor { }` sugar; `one_for_all`/`rest_for_one`; backoff; `lib/mlc/`.
