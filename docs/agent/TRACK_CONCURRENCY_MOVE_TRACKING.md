# Track: Concurrency Move Tracking

Parent: [TRACK_MLC_CONCURRENCY_REFINEMENT.md](TRACK_MLC_CONCURRENCY_REFINEMENT.md) §47 **#3**.
Source: [CONCURRENCY_V2.md](../CONCURRENCY_V2.md) §4/§44 phase 4. Size **M**.

## Status: **active** (2026-07-20) — STEP=2 done → Driver STEP=3

## Next step

**STEP=3** — MEMORY_MODEL sync (E088 / TaskScope.spawn move).

## Decision (STEP=0) — **frozen** 2026-07-20

| Item | Choice |
|------|--------|
| Problem | `move_check.mlc` already marks `move x` and emits **E088** on later free use. **Bare `ExprSpawn`** walks the body and **propagates** moved names to the outer scope (`test_spawn.mlc`: `use after move into spawn E088` green). **Gap:** `TaskScope.spawn` lowers to `ExprMethod(..., "spawn", [ExprLambda])`. `ExprLambda` runs `move_check_expr(body, empty_names())` and **returns outer `moved` unchanged** — so `move job` inside `.spawn` does not mark `job` moved for code after the call. Verified 2026-07-20: `scope \|task_scope\| do let mut job = 7; task_scope.spawn do move job end; job end` → `--check-only` no `error[E088]` (EXIT 0). Same pattern with immutable `let job` |
| Boundaries in scope | (1) keep bare `ExprSpawn` move propagation; (2) **`TaskScope.spawn` / `receiver.spawn`**: when `method_name == "spawn"`, walk spawn-body args so outer free bindings moved inside the body are added to the caller's `moved` set (same E088 after the call). Prefer mirroring ExprSpawn semantics, not changing general `ExprLambda` (lambdas that are not concurrency handoff must not leak moves) |
| Boundaries out | Full borrow checker; Sync trait → §47 **#4**; Channel.send / Isolate move surface; changing E088 message; `lib/mlc/` |
| Diagnostic | Reuse **E088** (existing message: value was moved and cannot be used here). Do not invent a new code |
| Detecting `.spawn` | Same as §47 #1/#2: `method_name == "spawn"` (parser keyword). Document residual: not type-aware TaskScope |
| Token / gate | Checker: (a) `TaskScope.spawn` + `move job` + later `job` → ≥1 `error[E088]`; (b) bare `spawn` + move + use still E088; (c) `.spawn` without `move` + later use of immutable Send value → 0 E088 (may still hit E087 if mut); (d) `mlcc --check-only` probes. Tier B: `compiler/tests/build_tests.sh`. Self-host: `build.sh` + mlcc2 diff |
| Docs | MEMORY_MODEL: E088 covers move into bare `spawn` **and** `TaskScope.spawn` |
| REG | yes if `lib/mlc/` touched (expect **no**); else skip |
| Out of scope | New diagnostic codes; general lambda move export; Sync; Array Send retune |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red test: TaskScope.spawn move then use → expect E088 (fails today) | **done** — `ok task_scope_spawn_move_then_use_red` |
| 2 | Wire move_check for ExprMethod spawn; green tests; Tier B; self-host | **done** — `ok task_scope_spawn_move_then_use_e088`; Tier B 1471/0; p1≡p2 |
| 3 | MEMORY_MODEL sync (E088 / TaskScope.spawn move) | open |
| 4 | Critic: gates; archive | open |

## Out of scope

§47 #4–#11; general ExprLambda move export; Sync trait; ThreadPool/Isolate MLC; `lib/mlc/`.
