# Track: Editor Column Pixel Model Fix

Parent: [TRACK_EDITOR_UX_BACKLOG.md](../agent/TRACK_EDITOR_UX_BACKLOG.md) §46 **#31a**
(pulled forward — coordinate-model correctness). Size **L**.

## Status: **closed** (2026-07-19) — Critic OK

**Critic 2026-07-19 (STEP=4):** Re-ran both L1 + font zoom + demo. Anti-false-done:
`7d52550b`…`f1ac7186` (STEP=0–3); no `compiler/` / `lib/mlc/` → REG skip.
Wire: `editor_measure_mono_char_width` (shape `"M"`); `editor_font_metrics_from_size`
uses advance; `line_display_column_end` / `_at` in `selection_apply` + `demo_live`;
unused `editor_app_char_width` removed. **reopen: none**.

Honest residual: uniform mono cell model only (no per-glyph proportional hit-test);
horizontal layout still `column * char_width`.

| Gate | Result |
|------|--------|
| `run_ux_selection_highlight_matches_glyph_x.sh` | `ux_ok selection_highlight_matches_glyph_x` EXIT=0 |
| `run_ux_selection_full_line_uses_display_columns.sh` | `ux_ok selection_full_line_uses_display_columns` EXIT=0 |
| `run_ux_ctrl_wheel_font_zoom.sh` | `ux_ok ctrl_wheel_font_zoom` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§46 `#31 EDITOR_TAB_STRIP_CLASSIC_LAYOUT`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `7d52550b` | Decision freeze + open |
| 1 | `3b211fe8` | Two red stubs + run scripts + `run_ux_column_pixel_model_red.sh` |
| 2 | `4c75c268` | `editor_measure_mono_char_width` + metrics from advance; green glyph-x |
| 3 | `f1ac7186` | selection/demo `col_end` display columns; green display-columns L1 |
| 4 | this Critic | close + archive |

## Decision (STEP=0) — **frozen** 2026-07-19

| Item | Choice |
|------|--------|
| Problem | Selection/caret/hit-test `column * metrics.char_width` drifts from real glyph x; multi-line full-line highlight uses `byte_size()` as column end |
| Root cause #1 | `editor_font_metrics_from_size` used ratio `font_size * 7 / 11`; unused `editor_app_char_width()=7` same class of guess |
| Root cause #2 | `demo_live` / `selection_apply` full-line `col_end` used byte length; `TextPosition.column` is display columns (#28) |
| Char width source | Shape `"M"` at load/zoom; `char_width = max(1, advance/64)`; uniform mono cell |
| Tokens | `ux_ok selection_highlight_matches_glyph_x`; `ux_ok selection_full_line_uses_display_columns` |
| Module touch | `misc/editor` only. No `compiler/`. No `lib/mlc/` |
| REG | no |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Two red L1/L2 harnesses | **done** — `ux_ok column_pixel_model_red` |
| 2 | Measure `char_width` from font advance | **done** — `ux_ok selection_highlight_matches_glyph_x` |
| 3 | Full-line `col_end` → display columns | **done** — `ux_ok selection_full_line_uses_display_columns` |
| 4 | Critic: gates; archive | **done** (closed) |

## Out of scope

Proportional (non-mono) layout; grapheme/ZWJ; `compiler/`; `lib/mlc/`; tab-strip (#31).
