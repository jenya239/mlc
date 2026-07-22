# Track: Editor idle caret blink

Parent: [../../PLAN.md](../../PLAN.md) §50.
Residual of [TRACK_EDITOR_IDLE_BUSY_LOOP_92PCT_CPU](TRACK_EDITOR_IDLE_BUSY_LOOP_92PCT_CPU.md)
(§46 #38). Size **S**.

## Status: **closed** (2026-07-22) — Critic OK

**Critic 2026-07-22 (STEP=3):** Re-ran stable + idle snapshot stable + `demo_live` compile +
HEAD `run_ux_gate` (65 scenarios). Anti-false-done: `d6168cb4`…`6a9cf56c` (STEP=0–2);
`content_dirty` / `paint_dirty`; `clock_ms` + `caret_blink_paint_dirty`; blink on `clock_ms`
(not `frame_index * 16`); paint-only keeps `frame_snapshot_cache_tick` dirty=0.
`misc/editor/**` only → REG skip; no `compiler/`/`lib/mlc/`. **reopen: none**.

Honest residual: wrap budget still uniform `char_width` (out of scope); live idle CPU
sample optional — **fixed** §51 `TRACK_EDITOR_IDLE_CPU_BUDGET`; Profile scopes only
on dirty snapshot. Historical
`run_ux_idle_caret_blink_red.sh` expects pre-wire demo_live and fails post-green — use
`run_ux_idle_caret_blink_stable.sh` only.

| Gate | Result |
|------|--------|
| `run_ux_idle_caret_blink_stable.sh` | `ux_ok idle_caret_blink_stable` EXIT=0 |
| `run_ux_idle_frame_snapshot_stable.sh` | `ux_ok idle_frame_snapshot_stable` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (65 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor-only) |

## Next step

**closed** — Critic OK. Queue → Planner (authorized queue empty; select next or document idle).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `d6168cb4` | Decision: clock_ms + paint_dirty |
| 1 | `a664e53f` | Red/stable harnesses |
| 2 | `6a9cf56c` | `demo_live` + `caret_blink_paint_dirty` |
| 3 | (this) | Critic close + archive |

## Decision (STEP=0) — **frozen** 2026-07-22

| Item | Choice |
|------|--------|
| Problem | #38 idle early-out never redraws; blink on `frame_index * 16` freezes on idle |
| Strategy (v1) | (1) Accumulate `clock_ms`. (2) Split `content_dirty` from `paint_dirty`. (3) Idle continue only when neither; paint-only uses cached snapshot |
| Primary gate | Green: blink phase change forces draw without extra snapshot; `demo_live_fs_compile_ok`; `run_ux_gate` |
| Module touch | `misc/editor/**` only |
| REG | no |
| Out of scope | wrap budget / HarfBuzz; live idle CPU sample; full-frame Profile; SCRIPT_VM; MIR Epic 5 |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: idle path no blink deadline | **done** |
| 2 | Wire clock_ms + paint_dirty blink redraw | **done** |
| 3 | Critic: compile + full `run_ux_gate` | **done** — Critic OK |

## Out of scope

Reopen #38 numbered STEPs; wrap-budget HarfBuzz rewrite.
