# Track: Editor Dirty Close Overlay L1

Parent: [TRACK_EDITOR_UX_BACKLOG.md](../agent/TRACK_EDITOR_UX_BACKLOG.md) §46 **#21**.
Dirty-close discard/cancel is covered by L0 `tab_close_unit` (API only);
button hit-test + click→outcome lives inline in `demo_live` only. Review
gate: `dirty_close_overlay_click` (L1). Size **S**.

## Status: **closed** (2026-07-19) — Critic OK

**Critic 2026-07-19 (STEP=3):** Re-ran L1 + session regression + demo compile.
Anti-false-done: `310b2435`…`8fbd83ac` (STEP=0–2). Wire present:
`dirty_close_overlay_discard_rect` / `cancel_rect`;
`dirty_close_overlay_click` → discard/cancel; demo_live overlay mouse
uses helper. **reopen: none**.

Honest residual: L1 covers discard+cancel centers only (not miss/outside);
label draw positions still slightly offset from hit rects (pre-existing);
no keyboard Confirm / save-before-close.

| Gate | Result |
|------|--------|
| `run_ux_dirty_close_overlay_click.sh` | `ux_ok dirty_close_overlay_click` EXIT=0 |
| `run_ux_session_restore_caret.sh` | `ux_ok session_restore_caret` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§46 `#23 EDITOR_MULTI_CURSOR`; `#22` superseded).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `310b2435` | Decision freeze + open |
| 1 | `0d949598` | L1 red harness + stub miss |
| 2 | `8fbd83ac` | hit-test + demo_live wire |
| 3 | this Critic | close + archive |

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
| 2 | click helper + demo_live wire | **done** (2026-07-19) — L1 green `ux_ok dirty_close_overlay_click` |
| 3 | Critic: gates; archive | **done** (closed) |

## Out of scope

Save-before-close; keyboard Confirm; redraw label alignment audit; multi-dirty queue; `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
