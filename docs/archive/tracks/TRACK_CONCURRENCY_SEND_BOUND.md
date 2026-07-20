# Track: Concurrency Send Bound

Parent: [TRACK_MLC_CONCURRENCY_REFINEMENT.md](../../agent/TRACK_MLC_CONCURRENCY_REFINEMENT.md) §47 **#2**.
Source: [CONCURRENCY_V2.md](../CONCURRENCY_V2.md) §2/§44 phase 3. Size **M**.

## Status: **closed** (2026-07-20) — Critic OK

**Critic 2026-07-20 (STEP=4):** Re-ran E092 gate + `--check-only main`. Anti-false-done:
`a1d68295`…`9691e847` (STEP=0–3); no `lib/mlc/` → REG skip.
Wire: `spawn_capture.mlc` `free_send_capture_*` + Shared.new type hint; Channel/Arc
Send diag → **E092**; MEMORY_MODEL cites E092 on spawn/`TaskScope.spawn` + Channel/Arc.
**reopen: none**.

Honest residual: Send detection uses spawn type-environment hints (Unknown skipped);
`Array` still always `!Send` in `type_is_send` (CONCURRENCY_V2 “Send iff T” not
retuned); `.spawn` still `method_name == "spawn"` not type-aware TaskScope.

| Gate | Result |
|------|--------|
| `run_spawn_shared_capture_not_send.sh` | `ok spawn_shared_capture_not_send_e092` EXIT=0 |
| `mlcc --check-only compiler/main.mlc` | EXIT=0 |
| Tier B / self-host | STEP=2: `build_tests.sh` EXIT=0; p1≡p2 DIFF_EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§47 `#3 CONCURRENCY_MOVE_TRACKING`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `a1d68295` | Decision freeze + open |
| 1 | `d7d0bf5a` | Red harness (gap no E092) |
| 2 | `dcae89d3` | free_send_capture + E092; Tier B + self-host |
| 3 | `9691e847` | MEMORY_MODEL sync |
| 4 | this Critic | close + archive |

## Decision (STEP=0) — **frozen** 2026-07-20

| Item | Choice |
|------|--------|
| Problem | `type_is_send` existed but only on Channel.send / Arc.new (E082). Gap closed: bare `spawn` + `TaskScope.spawn` free `!Send` → E092 |
| Boundaries in scope | bare spawn + TaskScope.spawn; Channel/Arc diag migrate E082→E092 |
| Boundaries out | Array Send retune; Sync trait → §47 **#4**; move-tracking → **#3** |
| Fixture | `Shared.new(7)` (no MLC RawPtr) |
| Diagnostic | **E092** |
| REG | no |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red test: spawn Shared capture → E092 | **done** |
| 2 | Wire Send + E092; green; Tier B; self-host | **done** |
| 3 | MEMORY_MODEL sync | **done** |
| 4 | Critic: gates; archive | **done** (closed) |

## Out of scope

§47 #3–#11; Array Send predicate retune; Sync trait; ThreadPool/Isolate MLC; `lib/mlc/`.
