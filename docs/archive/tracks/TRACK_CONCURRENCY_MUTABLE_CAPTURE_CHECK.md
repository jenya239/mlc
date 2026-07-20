# Track: Concurrency Mutable Capture Check

Parent: [TRACK_MLC_CONCURRENCY_REFINEMENT.md](../../agent/TRACK_MLC_CONCURRENCY_REFINEMENT.md) §47 **#1**.
Source: [CONCURRENCY_V2.md](../CONCURRENCY_V2.md) §1/§4. Size **L**.

## Status: **closed** (2026-07-20) — Critic OK

**Critic 2026-07-20 (STEP=4):** Re-ran E087 gate + `--check-only main`. Anti-false-done:
`eaeeb3d9`…`317e0d56` (STEP=0–3); no `lib/mlc/` → REG skip.
Wire: `spawn_capture.mlc` `ExprMethod` name=`spawn` → `free_mutable_capture_in_expr`
on args; MEMORY_MODEL §2 covers bare `spawn` + `TaskScope.spawn`.
**reopen: none**.

Honest residual: detection is `method_name == "spawn"` (parser keyword), not
type-aware `TaskScope`; Channel.send Send → §47 #2; nested bare `spawn` inside
`.spawn` lambda still sees emptied mutable scope from `ExprLambda` walk.

| Gate | Result |
|------|--------|
| `run_task_scope_spawn_mutable_capture.sh` | `ok task_scope_spawn_mutable_capture_e087` EXIT=0 |
| `mlcc --check-only compiler/main.mlc` | EXIT=0 |
| Tier B / self-host | STEP=2: `build_tests.sh` EXIT=0; p1≡p2 DIFF_EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§47 `#2 CONCURRENCY_SEND_BOUND`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `eaeeb3d9` | Decision freeze + open |
| 1 | `a0446bce` | Red harness (gap no E087) |
| 2 | `21931cf6` | ExprMethod spawn body E087; Tier B + self-host |
| 3 | `317e0d56` | MEMORY_MODEL sync |
| 4 | this Critic | close + archive |

## Decision (STEP=0) — **frozen** 2026-07-20

| Item | Choice |
|------|--------|
| Problem | Bare `ExprSpawn` already emits `error[E087]` for free enclosing `let mut`. **Gap closed:** `TaskScope.spawn` (`ExprMethod` `"spawn"`) now runs the same free-mut capture walk |
| Boundaries in scope | bare `spawn` + `TaskScope.spawn` / `.spawn do…end` |
| Boundaries out | `Channel.send` Send → §47 **#2**; ThreadPool/Isolate; move-tracking → **#3** |
| Diagnostic | Reuse **E087** |
| Detecting `.spawn` | `method_name == "spawn"` (parser keyword) |
| REG | no |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red test: `TaskScope.spawn` mut capture | **done** |
| 2 | Wire `.spawn` body; green tests; Tier B; self-host | **done** |
| 3 | MEMORY_MODEL sync | **done** |
| 4 | Critic: gates; archive | **done** (closed) |

## Out of scope

§47 #2–#11; Channel.send Send bound; ThreadPool/Isolate MLC; `lib/mlc/`.
