# Track: Concurrency Mutable Capture Check

Parent: [TRACK_MLC_CONCURRENCY_REFINEMENT.md](TRACK_MLC_CONCURRENCY_REFINEMENT.md) §47 **#1**.
Source: [CONCURRENCY_V2.md](../CONCURRENCY_V2.md) §1/§4. Size **L**.

## Status: **active** (2026-07-20) — STEP=2 done → Driver STEP=3

## Next step

**STEP=3** — MEMORY_MODEL sync (Tier B + self-host already green on STEP=2).

## Decision (STEP=0) — **frozen** 2026-07-20

| Item | Choice |
|------|--------|
| Problem | Bare `ExprSpawn` already emits `error[E087]` for free enclosing `let mut` (`spawn_capture.mlc`, `test_spawn.mlc`). **Gap:** `scope \|s\| do s.spawn do … end` lowers to `ExprMethod(..., "spawn", …)`, and `walk_spawn_sites_in_expr` only recurses into method args — it does **not** run `free_mutable_capture_in_stmts` on the `.spawn` body. Verified 2026-07-20: `let mut value = 7` + `task_scope.spawn do value end` → `--check-only` EXIT 0, no E087. Same for `let mut items` + `.push` inside `.spawn`. COW detach race from CONCURRENCY_V2 §0 / MEMORY_MODEL §2 remains reachable via `TaskScope.spawn` |
| Boundaries in scope | (1) bare `spawn` — keep current E087 + Sync-safe exemptions (`Mutex` always; `Arc<T>` iff `T` Sync); (2) **`TaskScope.spawn` / `receiver.spawn` method body** — apply the same free-mut capture walk as `ExprSpawn` |
| Boundaries out | `Channel.send` value Send bound → §47 **#2**; `ThreadPool.submit` / `Isolate.send` MLC surface → later items; move-tracking E088 expansion → **#3**; general `Send` on spawn args → **#2** |
| Diagnostic | Reuse **E087** (same message family: mutable value crosses concurrency boundary). Do not invent a new code for `.spawn` |
| Detecting `.spawn` | In `walk_spawn_sites_in_expr` `ExprMethod` arm: if `method_name == "spawn"` and the spawn body argument is a block/stmts form, run `free_mutable_capture_in_stmts` on that body with the enclosing mutable scope (same Sync-safe filter as bare spawn). Prefer type-aware `is_task_scope_spawn_method` when receiver type is available in `SpawnCaptureContext`; if type not yet known at walk time, name=`spawn` + existing context is acceptable **only** if tests prove no false positive on unrelated `.spawn` methods — freeze preference: use TaskScope type check when env has it, else conservative name match documented in SESSION if needed |
| Token / gate | Checker tests in `compiler/tests/test_spawn.mlc` (or sibling): (a) `scope` + `let mut` + `.spawn` free capture → ≥1 error containing E087; (b) same with `move` → 0; (c) `Mutex`/`Arc` Sync-safe → 0; (d) Array mut push via `.spawn` → E087. Plus `mlcc --check-only` probes. Tier B: `compiler/tests/build_tests.sh`. Self-host: `build.sh` + mlcc2 diff (compiler/ touched) |
| Docs | Update `MEMORY_MODEL.md` §2 spawn-capture row: E087 covers bare `spawn` **and** `TaskScope.spawn`; remove/narrow wording that COW race is still a live safe-code path for `.spawn` |
| REG | yes if `lib/mlc/` touched (expect **no**); else skip |
| Out of scope | New diagnostic codes; Channel.send Send; ThreadPool/Isolate; rejecting immutable Array capture; full borrow checker |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red test: `TaskScope.spawn` mut capture (expect E087, currently fails) | **done** — `ok task_scope_spawn_mutable_capture_red` |
| 2 | Wire `.spawn` body into `spawn_capture` walk; green tests | **done** — `ok task_scope_spawn_mutable_capture_e087`; Tier B; self-host identical |
| 3 | MEMORY_MODEL sync; `compiler/build.sh` + Tier B + self-host diff | open |
| 4 | Critic: gates; archive | open |

<!-- STEP=1: test_spawn case red -->
<!-- STEP=2: ExprMethod spawn body capture -->
<!-- STEP=3: docs + Tier B + self-host -->

## Out of scope

§47 #2–#11; Channel.send Send bound; ThreadPool/Isolate MLC; `lib/mlc/` unless forced.
