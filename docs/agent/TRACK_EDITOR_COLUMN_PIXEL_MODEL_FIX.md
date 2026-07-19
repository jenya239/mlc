# Track: Editor Column Pixel Model Fix

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#31a**
(pulled forward — coordinate-model correctness). Size **L**.

## Status: **active** (2026-07-19) — STEP=0 done; next STEP=1

## Next step

**STEP=1** — Red L1/L2 harnesses for both root causes.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | *(this turn)* | Decision freeze + open |

## Decision (STEP=0) — **frozen** 2026-07-19

| Item | Choice |
|------|--------|
| Problem | Selection/caret/hit-test `column * metrics.char_width` drifts from real glyph x; multi-line full-line highlight uses `byte_size()` as column end |
| Root cause #1 | `editor_font_metrics_from_size` uses ratio `font_size * 7 / 11`; unused `editor_app_char_width()=7` is the same class of guess. Draw path uses HarfBuzz advances (`text_shaping_glyph_advance_at` / 64) |
| Root cause #2 | `demo_live` full-line `col_end = line_text_at(...).byte_size()`; `selection_apply` uses `line_content_end - line_start` (also bytes). `TextPosition.column` is **display columns** (#28) |
| Char width source | At load/zoom: shape reference `"M"` with the editor font at current `pixel_size`; `char_width = max(1, advance_0 / 64)`. Keep **uniform cell** model (mono); do not per-glyph proportional layout this track |
| API | Prefer `editor_font_metrics_from_size` (or thin helper it calls) to measure via `text_shaping_shape` + advance; delete or redirect unused `editor_app_char_width`. Zoom path must remeasure |
| Selection fix | Full-line `col_end` = display column at line content end (`line_index_offset_to_position` / display helpers), not byte length — both `selection_apply` and `demo_live` draw path |
| Tokens | `ux_ok selection_highlight_matches_glyph_x` (ASCII line, N>40 cols: highlight right edge ≡ shaped glyph x); `ux_ok selection_full_line_uses_display_columns` (multi-line sel, non-ASCII middle line: width ≡ display cols × char_width) |
| Standing discipline | Scenario-first; name tokens in SESSION `done` |
| Module touch | `misc/editor` (`ux/font_zoom`, `ux/selection_apply`, `demo_live`, scenarios/scripts); may touch `misc/gui/text_shaping` only if thin measure helper needed. No `compiler/`. No `lib/mlc/` |
| REG | no |
| Out of scope | Per-glyph proportional hit-test; changing #28 display-column rules; tab soft-width; `#31` tab-strip layout |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Two red L1/L2 harnesses + `run_ux_*.sh` | pending — `ux_ok …_red` or stub fails named tokens |
| 2 | Measure `char_width` from font advance; wire load/zoom | pending — `ux_ok selection_highlight_matches_glyph_x` |
| 3 | Full-line selection `col_end` → display columns (`selection_apply` + `demo_live`) | pending — `ux_ok selection_full_line_uses_display_columns` + demo |
| 4 | Critic: both tokens + zoom + demo; archive | pending |

<!-- STEP=1 sub-steps: 1) `ux_scenarios/selection_highlight_matches_glyph_x.mlc` stub + script; 2) `ux_scenarios/selection_full_line_uses_display_columns.mlc` stub + script; 3) fail tokens until STEP=2/3 -->
<!-- STEP=2 sub-steps: 1) helper: shape `"M"` → advance/64; 2) `editor_font_metrics_from_size` uses it (font path resolve); 3) update `ctrl_wheel_font_zoom` asserts if ratios change; 4) green glyph-x scenario -->
<!-- STEP=3 sub-steps: 1) fix `selection_apply` mid-line `col_end`; 2) fix `demo_live` highlight loop; 3) green display-columns scenario + `demo_live_fs_compile_ok` -->

## Out of scope

Proportional (non-mono) layout; grapheme/ZWJ; `compiler/`; `lib/mlc/`; tab-strip (#31).
