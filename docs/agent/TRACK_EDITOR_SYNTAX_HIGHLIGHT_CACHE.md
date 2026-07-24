# Track: Editor syntax highlight span cache

Parent: [../PLAN.md](../PLAN.md) §77.
Residual of [TRACK_EDITOR_MINIMAP_WIDTH_ZOOM](../archive/tracks/TRACK_EDITOR_MINIMAP_WIDTH_ZOOM.md)
(§76) / [TRACK_EDITOR_MINIMAP_SYNTAX_TAGS](../archive/tracks/TRACK_EDITOR_MINIMAP_SYNTAX_TAGS.md)
(§74) / idle CPU chain (§51). Size **S**.

## Status: **open** — STEP=2 done; next Critic STEP=3

## Next step

**STEP=3** — Critic: stable×2 + `run_ux_gate`; close.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | 8fc5f83f | Decision: cache visible-range highlight spans across paints |
| 1 | 4e250b30 | Red harness + stable stub `not implemented` |
| 2 | (this) | `syntax_span_cache` + demo wire + L2 green |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | `demo_live` calls `highlight_range(draw_text, range_start, range_end)` on every paint when visible lines exist (`demo_live.mlc` ~2093–2100), even when text and visible byte range are unchanged (scroll-idle blink paints, caret blink). Honest residual of §76 («per-frame full rehighlight still OOS»); minimap already gates `highlight_mlc` on cache rebuild |
| Strategy (v1) | Add small cache (e.g. `editor_ux_syntax_span_cache_*` or local in demo): key = text identity + `range_start`/`range_end` (or first/last visible line). Hit → reuse `[HighlightSpan]`; miss → `highlight_range` and store. Invalidate on text edit or visible-range change. Keep minimap path unchanged |
| Primary gate | Red: no green `run_ux_syntax_highlight_cache_stable.sh`. Green: `ux_ok syntax_highlight_cache_stable` — same text+range → no recompute (rebuild/tick counter stays); text or range change → recompute; auto-discovered by `run_ux_gate` |
| Module touch | New thin helper under `misc/editor/ux/` or `syntax/` + `demo_live` wire; scripts/scenario |
| REG | no |
| Out of scope | Incremental/diff lexer; ICU; SCRIPT_VM; MIR Epic 5; reopen #35/#76 numbered STEPs |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: no span cache | **done** |
| 2 | Cache helper + demo wire; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | pending |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE.
