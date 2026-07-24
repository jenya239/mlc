# Track: Editor wrap budget by glyph advance

Parent: [../../PLAN.md](../../PLAN.md) §52.
Residual of [TRACK_EDITOR_WRAPPED_TEXT_BLEEDS_INTO_MINIMAP](TRACK_EDITOR_WRAPPED_TEXT_BLEEDS_INTO_MINIMAP.md)
(§46 #37) / deferred from §49–§51. Size **M**.

## Status: **closed** (2026-07-24) — Critic OK

**Critic 2026-07-24 (STEP=3):** Re-ran `wrap_budget_advance_stable` ×2 + `wrap_count_cache_stable` +
`demo_live` compile + HEAD `run_ux_gate` (67 scenarios). Anti-false-done: `717a690d`…`af121425`
(STEP=0–2); `visual_rows_for_line_pixel_budget` sums shaped advances; demo_live + wrap_cache
pixel tick; `misc/editor/**` + scripts → REG skip; no `compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: proportional caret/selection/hit-test still mono cell (Decision OOS);
`*_red` post-green fails (use stable only). Full-document pixel reshape cost on large files
cached via wrap_count_cache_tick_pixel (invalidate on text/viewport/font).

| Gate | Result |
|------|--------|
| `run_ux_wrap_budget_advance_stable.sh` | `ux_ok wrap_budget_advance_stable` EXIT=0 (×2) |
| `run_ux_wrap_count_cache_stable.sh` | `ux_ok wrap_count_cache_stable` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (67 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Queue → Planner (authorized queue empty; select next or document idle).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `717a690d` | Decision: pixel-budget wrap |
| 1 | `82796260` | Red/stable stub |
| 2 | `af121425` | Advance-sum wrap + demo wire |
| 3 | (this) | Critic close + archive |

## Decision (STEP=0) — **frozen** 2026-07-24

| Item | Choice |
|------|--------|
| Problem | Soft wrap budgets by display-column × mono `char_width`; drawn glyphs use per-glyph advances |
| Strategy (v1) | Pixel-budget wrap via shaped advances; soft word-break; column fallback if shape fails |
| Primary gate | `ux_ok wrap_budget_advance_stable`; full `run_ux_gate` |
| Module touch | `word_wrap.mlc` + `wrap_cache.mlc` + `demo_live` + scripts/scenario |
| REG | no |
| Out of scope | Proportional hit-test; ICU grapheme; SCRIPT_VM; MIR Epic 5 |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: no wrap-budget advance script | **done** |
| 2 | Implement advance-sum wrap; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | **done** — Critic OK |

## Out of scope

Reopen #37/#49/#50/#51 numbered STEPs; proportional hit-test rewrite.
