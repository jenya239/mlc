# Track: Editor minimap syntax tags

Parent: [../PLAN.md](../PLAN.md) §74.
Residual of [TRACK_EDITOR_MINIMAP](../archive/tracks/TRACK_EDITOR_MINIMAP.md)
(§46 #35) / [TRACK_EDITOR_SYNTAX_HIGHLIGHT_MLC_RICHER](../archive/tracks/TRACK_EDITOR_SYNTAX_HIGHLIGHT_MLC_RICHER.md)
(§46 #34). Size **S**.

## Status: **open** — STEP=2 done; next Critic STEP=3

## Next step

**STEP=3** — Critic: stable×2 + `run_ux_gate`; close.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | 8a1837d9 | Decision: minimap glyphs via syntax tags |
| 1 | e9229c1b | Red harness + stable stub `not implemented` |
| 2 | (this) | Wire append_syntax + highlight_mlc; green stable |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | Minimap strip rebuild in `demo_live` builds plain `push_line` rows and draws them with uniform `theme.text_*`. Main editor already uses `editor_ux_append_syntax_colored_row` + highlight spans. Honest residual of #35 after §73 |
| Strategy (v1) | On minimap cache rebuild, for each doc line append colored segments via `editor_ux_append_syntax_colored_row` (reuse helper; scaled `minimap_pixel` size; line byte range from `line_index`). Spans: `highlight_mlc(draw_text)` (or equivalent full-buffer highlight) at rebuild time so strip is not limited to visible-range spans. Draw still via `static_text_draw_lines_colored` with text fallback. Keep layout/cache/indicator/click unchanged |
| Primary gate | Red: no green `run_ux_minimap_syntax_tags_stable.sh`. Green: `ux_ok minimap_syntax_tags_stable` — minimap rebuild calls `editor_ux_append_syntax_colored_row` (not bare `push_line` for map text); auto-discovered by `run_ux_gate` |
| Module touch | `demo_live` minimap rebuild draw, scripts/scenario |
| REG | no |
| Out of scope | Zoom-linked scale; per-frame full rehighlight without cache; ICU; SCRIPT_VM; MIR Epic 5; reopen #34/#35 numbered STEPs |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: uniform text minimap glyphs | **done** |
| 2 | Wire syntax-colored rebuild; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | pending |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE.
