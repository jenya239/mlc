# Track: Editor syntax highlight span cache

Parent: [../../PLAN.md](../../PLAN.md) §77.
Residual of [TRACK_EDITOR_MINIMAP_WIDTH_ZOOM](TRACK_EDITOR_MINIMAP_WIDTH_ZOOM.md)
(§76) / [TRACK_EDITOR_MINIMAP_SYNTAX_TAGS](TRACK_EDITOR_MINIMAP_SYNTAX_TAGS.md)
(§74) / idle CPU chain (§51). Size **S**.

## Status: **closed** (2026-07-25) — Critic OK

**Critic 2026-07-25 (STEP=3):** Re-ran `syntax_highlight_cache_stable` ×2 +
`editor_minimap_cache_stable` + `minimap_syntax_tags_stable` +
`minimap_width_zoom_stable` + `syntax_mlc_richer_tags` + `demo_live` compile +
HEAD `run_ux_gate`.
Anti-false-done: `8fc5f83f`…`d4805de7` (STEP=0–2); `syntax_span_cache` +
demo tick; `misc/editor/**` + scripts → REG skip; no `compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: `*_red` post-green fails (use stable only); incremental/diff
lexer still OOS.

| Gate | Result |
|------|--------|
| `run_ux_syntax_highlight_cache_stable.sh` | `ux_ok syntax_highlight_cache_stable` EXIT=0 (×2) |
| `run_ux_editor_minimap_cache_stable.sh` | `ux_ok editor_minimap_cache_stable` EXIT=0 |
| `run_ux_minimap_syntax_tags_stable.sh` | `ux_ok minimap_syntax_tags_stable` EXIT=0 |
| `run_ux_minimap_width_zoom_stable.sh` | `ux_ok minimap_width_zoom_stable` EXIT=0 |
| `run_ux_syntax_mlc_richer_tags.sh` | `ux_ok syntax_mlc_richer_tags` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (92 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `8fc5f83f` | Decision: cache visible-range highlight spans across paints |
| 1 | `4e250b30` | Red harness + stable stub `not implemented` |
| 2 | `d4805de7` | `syntax_span_cache` + demo wire + L2 green |
| 3 | (this) | Critic close + archive |

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
| 3 | Critic: stable + full `run_ux_gate` | **done** |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE.
