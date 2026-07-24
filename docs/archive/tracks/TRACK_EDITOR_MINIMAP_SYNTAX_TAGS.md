# Track: Editor minimap syntax tags

Parent: [../../PLAN.md](../../PLAN.md) §74.
Residual of [TRACK_EDITOR_MINIMAP](TRACK_EDITOR_MINIMAP.md)
(§46 #35) / [TRACK_EDITOR_SYNTAX_HIGHLIGHT_MLC_RICHER](TRACK_EDITOR_SYNTAX_HIGHLIGHT_MLC_RICHER.md)
(§46 #34). Size **S**.

## Status: **closed** (2026-07-25) — Critic OK

**Critic 2026-07-25 (STEP=3):** Re-ran `minimap_syntax_tags_stable` ×2 +
`editor_minimap_cache_stable` + `syntax_mlc_richer_tags` + `demo_live` compile +
HEAD `run_ux_gate`.
Anti-false-done: `8a1837d9`…`70c91b60` (STEP=0–2); minimap rebuild →
`highlight_mlc` + `append_syntax_colored_row`; `misc/editor/**` + scripts → REG skip;
no `compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: `*_red` post-green fails (use stable only); zoom-linked scale still
OOS; full-buffer highlight only on cache rebuild (not per-frame).

| Gate | Result |
|------|--------|
| `run_ux_minimap_syntax_tags_stable.sh` | `ux_ok minimap_syntax_tags_stable` EXIT=0 (×2) |
| `run_ux_editor_minimap_cache_stable.sh` | `ux_ok editor_minimap_cache_stable` EXIT=0 |
| `run_ux_syntax_mlc_richer_tags.sh` | `ux_ok syntax_mlc_richer_tags` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (89 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `8a1837d9` | Decision: minimap glyphs via syntax tags |
| 1 | `e9229c1b` | Red/stable stub |
| 2 | `70c91b60` | syntax wire + L0 green |
| 3 | (this) | Critic close + archive |

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
| 3 | Critic: stable + full `run_ux_gate` | **done** — Critic OK |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE.
