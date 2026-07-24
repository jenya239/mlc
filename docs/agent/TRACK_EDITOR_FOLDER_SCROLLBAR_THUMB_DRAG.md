# Track: Editor folder scrollbar thumb drag

Parent: [../PLAN.md](../PLAN.md) §63.
Residual of [TRACK_EDITOR_SCROLLBAR_THEME_TINT](../archive/tracks/TRACK_EDITOR_SCROLLBAR_THEME_TINT.md)
(§62) / [TRACK_EDITOR_SCROLLBAR_THUMB_DRAG](../archive/tracks/TRACK_EDITOR_SCROLLBAR_THUMB_DRAG.md)
(§61). Size **S**.

## Status: **active** (2026-07-24) — STEP=0 done → Driver STEP=1

## Next step

**STEP=1** — Red: folder thumb draws; no drag→`tree_scroll_y`; failing stable stub.

## Decision (STEP=0) — **frozen** 2026-07-24

| Item | Choice |
|------|--------|
| Problem | Folder tree draws `folder_scrollbar_thumb` and wheel updates `tree_scroll_y`, but mouse drag on the thumb/track does not scroll. Content pane already has thumb drag (§61) |
| Strategy (v1) | Mirror content: track hit + `folder_scrollbar_scroll_at_y` (map pointer along track → scroll, reuse `folder_clamp_scroll` / `folder_max_scroll`). Wire `demo_live` press/drag on tree list track. Helpers in `ux/folder_panel.mlc` |
| Primary gate | Red: no `run_ux_folder_scrollbar_thumb_drag_stable.sh`. Green: `ux_ok folder_scrollbar_thumb_drag_stable` — pointer low vs high on track → different `tree_scroll_y`; auto-discovered by `run_ux_gate` |
| Module touch | `ux/folder_panel.mlc`, `demo_live` input, scripts/scenario |
| REG | no |
| Out of scope | Content thumb changes; theme; ICU; SCRIPT_VM; MIR Epic 5; reopen §62 numbered STEPs |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: no folder thumb drag | open |
| 2 | Helper + demo wire; green | open |
| 3 | Critic: stable + full `run_ux_gate` | open |

<!-- STEP=1: red proves folder thumb draw; no scroll_at helper -->
<!-- STEP=2: folder_scrollbar_scroll_at_y + demo drag; SKIP if font missing -->
<!-- STEP=3: Critic archive -->

## Out of scope

SCRIPT_VM; MIR Epic 5; content scrollbar changes.
