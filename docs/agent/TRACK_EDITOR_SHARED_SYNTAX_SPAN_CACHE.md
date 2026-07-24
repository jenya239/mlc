# Track: Editor shared full-buffer syntax span cache

Parent: [../PLAN.md](../PLAN.md) §79.
Residual of [TRACK_EDITOR_MINIMAP_HIGHLIGHT_CACHE](../archive/tracks/TRACK_EDITOR_MINIMAP_HIGHLIGHT_CACHE.md)
(§78) / [TRACK_EDITOR_SYNTAX_HIGHLIGHT_CACHE](../archive/tracks/TRACK_EDITOR_SYNTAX_HIGHLIGHT_CACHE.md)
(§77). Size **S**.

## Status: **open** — STEP=1 done; next Driver STEP=2

## Next step

**STEP=2** — Shared full-buffer wire; green stable.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | 77e83af9 | Decision: one full-buffer span cache for paint + minimap |
| 1 | (this) | Red harness + stable stub `not implemented` |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | After §77/§78, `demo_live` keeps two caches: visible-range `syntax_span_cache` (re-lex when scroll changes `range_start`/`range_end`) and full-buffer `minimap_span_cache`. Unchanged text + scroll → main paint calls `highlight_range` again; text edit → both caches re-lex. Honest residual of §78 |
| Strategy (v1) | Single shared full-buffer cache (`0..byte_size`), tick once per text change. Main paint and minimap both read `.spans` (`append_syntax_colored_row` already takes full spans + row byte range). Drop separate visible-range tick (or make it a thin alias). Keep layout/click unchanged |
| Primary gate | Red: no green `run_ux_shared_syntax_span_cache_stable.sh`. Green: `ux_ok shared_syntax_span_cache_stable` — demo uses one shared full-buffer cache for paint+minimap (not dual visible-range + minimap caches); scroll/range change without text edit does not bump rebuild; auto-discovered by `run_ux_gate` |
| Module touch | `demo_live` paint + minimap; scripts/scenario |
| REG | no |
| Out of scope | Incremental/diff lexer; ICU; SCRIPT_VM; MIR Epic 5; reopen #77/#78 numbered STEPs |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: dual / visible-range cache | **done** |
| 2 | Shared full-buffer wire; green | pending |
| 3 | Critic: stable + full `run_ux_gate` | pending |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE.
