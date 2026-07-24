# Track: Editor content scrollbar thumb drag

Parent: [../PLAN.md](../PLAN.md) §61.
Residual of [TRACK_EDITOR_HORIZONTAL_SCROLLBAR](../archive/tracks/TRACK_EDITOR_HORIZONTAL_SCROLLBAR.md)
(§60) / [TRACK_EDITOR_CONTENT_SCROLLBAR](../archive/tracks/TRACK_EDITOR_CONTENT_SCROLLBAR.md)
(§46 #33e). Size **S**.

## Status: **active** (2026-07-24) — STEP=1 done → Driver STEP=2

## Next step

**STEP=2** — Helpers + demo wire; green stable.

## Decision (STEP=0) — **frozen** 2026-07-24

| Item | Choice |
|------|--------|
| Problem | Vertical + horizontal content thumbs draw on hover (§33e/§60), but mouse drag on thumb does not change `scroll_offset_y` / `scroll_offset_x`. Minimap already maps click/drag → scroll; content thumbs do not |
| Strategy (v1) | Hit-test point-in-thumb; on press+drag map pointer position along track → scroll (same geometry inverse as thumb layout). Y thumb → `scroll_offset_y`; X thumb → `scroll_offset_x`. Thin helpers in `ux/scroll.mlc`; wire `demo_live` mouse path. No always-on chrome change |
| Primary gate | Red: no `run_ux_scrollbar_thumb_drag_stable.sh`. Green: `ux_ok scrollbar_thumb_drag_stable` — drag along Y track changes scroll_y; drag along X track changes scroll_x; auto-discovered by `run_ux_gate` |
| Module touch | `ux/scroll.mlc`, `demo_live` input, scripts/scenario |
| REG | no |
| Out of scope | Theme thumb tint; folder-tree thumb drag; ICU; SCRIPT_VM; MIR Epic 5; reopen §60 numbered STEPs |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: no thumb drag | **done** |
| 2 | Helpers + demo wire; green | open |
| 3 | Critic: stable + full `run_ux_gate` | open |

<!-- STEP=1: red proves thumbs drawn; no scroll-from-thumb-drag helper -->
<!-- STEP=2: scroll_at_thumb_y/x + demo mouse; SKIP if font missing -->
<!-- STEP=3: Critic archive -->

## Out of scope

Theme tokens; folder scrollbar drag; SCRIPT_VM; MIR Epic 5.
