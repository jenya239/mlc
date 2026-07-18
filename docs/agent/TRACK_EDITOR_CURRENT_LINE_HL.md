# Track: Editor current-line highlight (draw report)

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#11**.
Caret line has no background band in the draw path — only selection/find
rects exist. Review gate: `current_line_highlight_draws` (L2). Size **S**.

## Status: **active** (2026-07-18) — STEP=0 done; next STEP=1

## Next step

**STEP=1** — L2 scenario first (`current_line_highlight_draws`) red harness.

## Decision (STEP=0) — **frozen** 2026-07-18

| Item | Choice |
|------|--------|
| Problem | No current-line background highlight; caret line is not visually distinct when selection empty |
| Model | Semantic draw report (same pattern as `editor_ux_selection_draw_report` / find highlights): one full text-viewport-width rect for the **caret line** when that line intersects the visible viewport |
| Caret line | `line_index_offset_to_position(..., selection_caret(...)).line` |
| Geometry | `x`/`width` = text viewport full width; `y`/`height` = that row × `line_height` (scroll-aware, same math as selection report). Empty active tab → empty report |
| Always on | Emit even when selection non-empty (band under selection/find; live draw order: current-line → find → selection → glyphs/caret) |
| Color | Live: `theme.panel_*` at alpha ~0.55 (no new Theme fields this track). L2 asserts geometry only, not RGB |
| API | `editor_ux_current_line_draw_report(state, metrics) -> CurrentLineDrawReport` in `ux/` (new small module or next to selection_apply). Wire `demo_live` solid rect from report |
| Scenarios | L2: `current_line_highlight_draws` — multi-line doc; caret mid-line; assert `highlights.length()==1`, matching `line`, `height==line_height`, `width>0` |
| Standing discipline | Scenario-first; name tokens in SESSION `done` |
| Module touch | `misc/editor` ux draw report + `demo_live` (+ scenario/script). No `compiler/` / `lib/mlc/` |
| REG | no `lib/mlc/`; no `compiler/` |

### Gates

| Step | Token |
|------|-------|
| 1–2 | `ux_ok current_line_highlight_draws` (+ run script) |
| every | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog | **done** (2026-07-18) |
| 1 | L2 scenario first (`current_line_highlight_draws`) | pending |
| 2 | draw report + demo_live wire (panel tint) | pending |
| 3 | Critic: gates; archive | pending |

## Out of scope

Trailing-ws viz (#24); Theme palette expansion; multi-cursor (#23); wrap visual-row band (non-wrap caret line only); `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
