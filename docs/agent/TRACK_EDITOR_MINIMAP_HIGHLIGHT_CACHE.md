# Track: Editor minimap highlight span cache

Parent: [../PLAN.md](../PLAN.md) §78.
Residual of [TRACK_EDITOR_SYNTAX_HIGHLIGHT_CACHE](../archive/tracks/TRACK_EDITOR_SYNTAX_HIGHLIGHT_CACHE.md)
(§77) / [TRACK_EDITOR_MINIMAP_SYNTAX_TAGS](../archive/tracks/TRACK_EDITOR_MINIMAP_SYNTAX_TAGS.md)
(§74). Size **S**.

## Status: **open** — STEP=2 done; next Critic STEP=3

## Next step

**STEP=3** — Critic: stable×2 + `run_ux_gate`; close.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | bbff3983 | Decision: cache full-buffer spans for minimap rebuild |
| 1 | 60ee67b4 | Red harness + stable stub `not implemented` |
| 2 | (this) | `minimap_span_cache` wire + L2 green |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | After §77, main paint caches visible-range spans, but minimap rebuild still does `highlight_mlc(draw_text)` whenever strip height or `zoom_font_size` changes — even when document text is unchanged (`demo_live.mlc` ~2563–2568). Honest residual of §77 (minimap path) / speed ceiling |
| Strategy (v1) | Reuse `editor_ux_syntax_span_cache_*` (or thin wrapper) keyed by full-buffer text (`range_start=0`, `range_end=byte_size`). On glyph rebuild for height/font only → cache hit, no re-lex. Text edit → miss → `highlight_mlc`/`highlight_range` once. Keep layout/click/indicator unchanged |
| Primary gate | Red: no green `run_ux_minimap_highlight_cache_stable.sh`. Green: `ux_ok minimap_highlight_cache_stable` — demo minimap rebuild uses span cache (not bare `highlight_mlc(draw_text)`); same text → no recompute; auto-discovered by `run_ux_gate` |
| Module touch | `demo_live` minimap rebuild; optionally thin helper; scripts/scenario |
| REG | no |
| Out of scope | Incremental/diff lexer; ICU; SCRIPT_VM; MIR Epic 5; reopen #77 numbered STEPs |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: bare `highlight_mlc` on minimap rebuild | **done** |
| 2 | Cache wire; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | pending |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE.
