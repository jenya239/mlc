# Track: Editor Trailing Whitespace Visualization

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#24**.
Trailing spaces/tabs at EOL are invisible in the live draw path. Review
gate: `trailing_ws_visualized` (L2). Size **S**.

## Status: **active** (2026-07-19) — STEP=2 done; next Critic

## Next step

**STEP=3** — Critic: gates; archive

## Decision (STEP=0) — **frozen** 2026-07-19

| Item | Choice |
|------|--------|
| Problem | Trailing whitespace at end of line is invisible; hard to spot accidental spaces/tabs |
| Model | Semantic draw report (same pattern as `editor_ux_current_line_draw_report`): one rect per **trailing run** on each **visible** line |
| Trailing run | On each logical line: maximal suffix of ASCII space (`' '`) and/or tab (`'\t'`) immediately before `\n` or EOF. Empty run → no highlight for that line |
| Geometry | `x` = text origin + column_start×`char_width` (scroll_x-aware); `width` = run_columns×`char_width` (space=1 col, tab=1 col this track); `y`/`height` = line×`line_height` (scroll_y-aware, visible-only). Empty active tab → empty report |
| Always on | Emit whenever document has trailing runs in viewport (no settings toggle this track) |
| Color | Live: muted theme tint (reuse `theme.panel_*` or selection at low alpha). L2 asserts geometry only, not RGB |
| API | `editor_ux_trailing_ws_draw_report(state, metrics) -> TrailingWsDrawReport` in `misc/editor/ux/` (new small module). Wire `demo_live` solid rects from report (under glyphs, above or beside current-line band — order: current-line → trailing-ws → find → selection → glyphs/caret) |
| Scenarios | L2: `trailing_ws_visualized` — doc with trailing spaces on ≥1 line + clean line; assert highlight count, matching line, `width>0`, `height==line_height`; token `ux_ok trailing_ws_visualized` |
| Standing discipline | Scenario-first; name tokens in SESSION `done` |
| Module touch | `misc/editor` ux draw report + `demo_live` (+ scenario/script). No `compiler/` / `lib/mlc/` |
| REG | no `lib/mlc/`; no `compiler/` |

### Gates

| Step | Token |
|------|-------|
| 1–2 | `ux_ok trailing_ws_visualized` (+ run script) |
| every | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |
| regression | `bash scripts/run_ux_current_line_highlight_draws.sh` → `ux_ok current_line_highlight_draws` |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog | **done** (2026-07-19) |
| 1 | L2 scenario first (`trailing_ws_visualized`) | **done** (2026-07-19) — stub red: `ux_fail trailing_ws_visualized count got 0` |
| <!-- sub-steps: 1) scenario + run script; 2) stub draw report → red; 3) demo_live_fs_compile + current_line regression green --> |
| 2 | draw report + demo_live wire | **done** (2026-07-19) — `editor_ux_trailing_ws_draw_report` + demo_live solid rects; L2 green |
| <!-- sub-steps: 1) trailing run detect + visible geometry; 2) demo_live solid rects; 3) L2 green --> |
| 3 | Critic: gates; archive | pending |

## Out of scope

All-whitespace viz (leading/mid-line); settings toggle; tab display width >1; wrap visual-row split of trailing run; strip-on-save; `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
