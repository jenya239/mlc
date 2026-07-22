# Track: Editor idle caret blink

Parent: [../PLAN.md](../PLAN.md) §50.
Residual of [TRACK_EDITOR_IDLE_BUSY_LOOP_92PCT_CPU](../archive/tracks/TRACK_EDITOR_IDLE_BUSY_LOOP_92PCT_CPU.md)
(§46 #38). Size **S**.

## Status: **active** (2026-07-22) — STEP=0 done → Driver STEP=1

## Next step

**STEP=1** — Red: idle path has no blink deadline (caret clock frozen on `continue`).

## Decision (STEP=0) — **frozen** 2026-07-22

| Item | Choice |
|------|--------|
| Problem | #38 idle early-out (`activity == 0` → `wait_events_timeout` + `continue` in `demo_live.mlc` ~857–860) never redraws. Caret blink uses `frame_index * 16` (`~2038`), which freezes on idle — caret stuck on or off |
| Strategy (v1) | (1) Accumulate `clock_ms` (active frames + idle wait timeout ms). (2) Split **content_dirty** (input/edit/resize → snapshot cache) from **paint_dirty** (blink phase change vs last drawn). (3) Idle `continue` only when neither dirty; paint-only redraw uses cached snapshot (`frame_snapshot_cache_tick` dirty=0). Do **not** reintroduce always-on 60fps loop |
| Primary gate | Red: idle path has no blink/paint deadline (only input-driven `activity`). Green: blink phase change forces a draw without extra `document_frame_snapshot`; `demo_live_fs_compile_ok`; `run_ux_gate` |
| Module touch | `misc/editor/**` only (`ux/caret_blink` helper + `demo_live`) |
| REG | no |
| Out of scope | wrap `char_width` budget / HarfBuzz wrap; live idle CPU sample; full-frame Profile phases; SCRIPT_VM; MIR Epic 5 |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: idle path no blink deadline | open |
| 2 | Wire clock_ms + paint_dirty blink redraw | open |
| 3 | Critic: compile + full `run_ux_gate` | open |

<!-- STEP=1: red script greps idle continue without blink/paint deadline -->
<!-- STEP=2: clock_ms + paint_dirty; snapshot cache still skip on paint-only -->
<!-- STEP=3: Critic archive -->

## Out of scope

Reopen #38 numbered STEPs; wrap-budget HarfBuzz rewrite.
