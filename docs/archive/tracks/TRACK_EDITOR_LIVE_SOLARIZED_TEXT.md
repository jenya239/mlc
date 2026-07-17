# Track: Editor live Solarized glyph colors

Parent: [../agent/TRACK_EDITOR_UX_BACKLOG.md](../agent/TRACK_EDITOR_UX_BACKLOG.md) §46 **#1b**.
Found 2026-07-17 screenshot audit (not a regression from #1).

## Status: **closed** (2026-07-18) — Critic OK

**Critic 2026-07-18 (STEP=3):** Re-ran L2 + compile gates. Anti-false-done:
`94dd3f79`…`08c4dde6` (STEP=0–2). Decision met: glyph rgba via
`editor_ux_syntax_glyph_line` / `editor_ux_append_syntax_colored_row`; syntax
bg tint (`rgb*0.35`) gone; chrome `theme_solarized_light()`; stale white-glyph
comment scrubbed. No `compiler/` / `lib/mlc/`. **reopen: none**.

| Gate | Result |
|------|--------|
| `run_ux_syntax_glyph_color.sh` | `ux_ok syntax_glyph_color_matches_theme` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§46 `#1c EDITOR_STALE_HELP_TEXT`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `94dd3f79` | Decision freeze + open |
| 1 | `3b999c87` | L2 red harness |
| 2 | `08c4dde6` | wire glyph colors + Solarized Light chrome |
| 3 | this Critic | close + archive |

## Delivered

Per-tag `StaticTextLine` colors from `theme_rgb_for_tag`; live demo Solarized
Light chrome; selection rects kept; L2 scenario gate.

## Out of scope (unchanged)

#1c–#1d, tree-sitter, keyboard nav, `compiler/`.
