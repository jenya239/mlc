# Track: Editor Dirty Close Overlay L1

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#21**.
Dirty-close discard/cancel is covered by L0 `tab_close_unit` (API only);
button hit-test + click→outcome lives inline in `demo_live` only. Review
gate: `dirty_close_overlay_click` (L1). Size **S**.

## Status: **active** (2026-07-19) — STEP=1 done; next STEP=2

## Next step

**STEP=2** — click helper + demo_live wire.

## Decision (STEP=0) — **frozen** 2026-07-19

| Item | Choice |
|------|--------|
| Problem | Overlay Discard/Cancel click path untested at L1; geometry duplicated in `demo_live` |
| Geometry | Match current live hit rects: Discard `rect(w/2-120, h/2+20, 100, 28)`; Cancel `rect(w/2+20, h/2+20, 100, 28)` |
| API | `dirty_close_overlay_click(tabs, overlay, mouse_x, mouse_y, window_width, window_height) -> CloseOutcome` (+ thin rect helpers if useful); miss → tabs/overlay unchanged |
| Discard hit | Center of discard rect → same as `dirty_close_discard` |
| Cancel hit | Center of cancel rect → same as `dirty_close_cancel` |
| Live | `demo_live` overlay mouse branch calls the helper (no duplicate rect literals) |
| Scenarios | L1: `dirty_close_overlay_click` — dirty tab → request close → click discard → tab closed + overlay hidden; cancel path → overlay hidden + tab count unchanged; token `ux_ok dirty_close_overlay_click` |
| Standing discipline | Scenario-first; name tokens in SESSION `done` |
| Module touch | `misc/editor/workspace/tab_close.mlc` (or thin `ux/`); scenario/script; thin `demo_live` wire. No `compiler/` / `lib/mlc/` |
| REG | no `lib/mlc/`; no `compiler/` |

### Gates

| Step | Token |
|------|-------|
| 1–2 | `ux_ok dirty_close_overlay_click` (+ run script) |
| every | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |
| regression | `bash scripts/run_ux_session_restore_caret.sh` → `ux_ok session_restore_caret` |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog | **done** (2026-07-19) |
| 1 | L1 scenario first (`dirty_close_overlay_click`) | **done** (2026-07-19) — stub red: `ux_fail dirty_close_overlay_click discard_visible` |
| 2 | click helper + demo_live wire | pending |
| <!-- sub-steps: 1) implement hit+apply in tab_close; 2) demo_live uses helper; 3) L1 green --> |
| 3 | Critic: gates; archive | pending |

## Out of scope

Save-before-close; keyboard Confirm; redraw label alignment audit; multi-dirty queue; `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
