# Track: Editor shared full-buffer syntax span cache

Parent: [../../PLAN.md](../../PLAN.md) §79.
Residual of [TRACK_EDITOR_MINIMAP_HIGHLIGHT_CACHE](TRACK_EDITOR_MINIMAP_HIGHLIGHT_CACHE.md)
(§78) / [TRACK_EDITOR_SYNTAX_HIGHLIGHT_CACHE](TRACK_EDITOR_SYNTAX_HIGHLIGHT_CACHE.md)
(§77). Size **S**.

## Status: **closed** (2026-07-25) — Critic OK

**Critic 2026-07-25 (STEP=3):** Re-ran `shared_syntax_span_cache_stable` ×2 +
`syntax_highlight_cache_stable` + `minimap_highlight_cache_stable` +
`minimap_syntax_tags_stable` + `editor_minimap_cache_stable` + `demo_live`
compile + HEAD `run_ux_gate`.
Anti-false-done: `77e83af9`…`69e862b8` (STEP=0–2); `shared_span_cache`
for paint+minimap; `misc/editor/**` + scripts → REG skip; no
`compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: `*_red` post-green fails (use stable only); incremental/diff
lexer still OOS.

| Gate | Result |
|------|--------|
| `run_ux_shared_syntax_span_cache_stable.sh` | `ux_ok shared_syntax_span_cache_stable` EXIT=0 (×2) |
| `run_ux_syntax_highlight_cache_stable.sh` | `ux_ok syntax_highlight_cache_stable` EXIT=0 |
| `run_ux_minimap_highlight_cache_stable.sh` | `ux_ok minimap_highlight_cache_stable` EXIT=0 |
| `run_ux_minimap_syntax_tags_stable.sh` | `ux_ok minimap_syntax_tags_stable` EXIT=0 |
| `run_ux_editor_minimap_cache_stable.sh` | `ux_ok editor_minimap_cache_stable` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (94 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `77e83af9` | Decision: one full-buffer span cache for paint + minimap |
| 1 | `d23911cf` | Red harness + stable stub `not implemented` |
| 2 | `69e862b8` | `shared_span_cache` wire + L2 green |
| 3 | (this) | Critic close + archive |

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
| 2 | Shared full-buffer wire; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | **done** |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE.
