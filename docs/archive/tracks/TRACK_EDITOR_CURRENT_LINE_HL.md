# Track: Editor current-line highlight (draw report)

Parent: [../agent/TRACK_EDITOR_UX_BACKLOG.md](../agent/TRACK_EDITOR_UX_BACKLOG.md) §46 **#11**.
Caret line has no background band in the draw path — only selection/find
rects exist. Review gate: `current_line_highlight_draws` (L2). Size **S**.

## Status: **closed** (2026-07-18) — Critic OK

**Critic 2026-07-18 (STEP=3):** Re-ran L2 + compile. Anti-false-done:
`820c6549`…`c1adb524` (STEP=0–2). Wire present:
`editor_ux_current_line_draw_report` (caret line, full text-viewport width,
scroll-aware, visible-only); `demo_live` panel tint 0.55 under find/selection.
**reopen: none**.

Honest residual: no Theme `current_line_*` fields; wrap visual-row band out of
scope (non-wrap logical line); L2 geometry only (not RGB); band may misalign
slightly vs wrap rows in live demo.

| Gate | Result |
|------|--------|
| `run_ux_current_line_highlight_draws.sh` | `ux_ok current_line_highlight_draws` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§46 `#12 EDITOR_CRLF_PRESERVE`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `820c6549` | Decision freeze + open |
| 1 | `d7a12750` | L2 red harness |
| 2 | `c1adb524` | draw report + demo_live panel tint |
| 3 | this Critic | close + archive |

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
| 1 | L2 scenario first (`current_line_highlight_draws`) | **done** (red: `ux_fail current_line highlights count got 0`) |
| 2 | draw report + demo_live wire (panel tint) | **done** (`ux_ok`; `demo_live_fs_compile_ok`) |
| 3 | Critic: gates; archive | **done** (closed) |

## Out of scope

Trailing-ws viz (#24); Theme palette expansion; multi-cursor (#23); wrap visual-row band (non-wrap caret line only); `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
