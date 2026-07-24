# Track: Editor wrap budget by glyph advance

Parent: [../PLAN.md](../PLAN.md) §52.
Residual of [TRACK_EDITOR_WRAPPED_TEXT_BLEEDS_INTO_MINIMAP](../archive/tracks/TRACK_EDITOR_WRAPPED_TEXT_BLEEDS_INTO_MINIMAP.md)
(§46 #37) / deferred from §49–§51. Size **M**.

## Status: **active** (2026-07-24) — STEP=2 done → Critic STEP=3

## Next step

**STEP=3** — Critic: stable + full `run_ux_gate`.

## Decision (STEP=0) — **frozen** 2026-07-24

| Item | Choice |
|------|--------|
| Problem | Soft wrap (`visual_rows_for_line`) budgets by display-column count × uniform mono `char_width`. Drawn glyphs use HarfBuzz per-glyph advances (`text_shaping_glyph_advance_at`). On mixed Cyrillic/Latin lines advances diverge from the mono cell → wrap break can sit past the content clip (secondary cause of #37; scissors fixed bleed, budget drift remains) |
| Strategy (v1) | Pixel-budget wrap path: sum shaped advances (26.6 → px) against `viewport_width` when deciding breaks; keep soft word-break preference. Fallback to current column wrap if shaping unavailable. Do **not** rewrite selection/hit-test to proportional (still mono cell) |
| Primary gate | Red: no `run_ux_wrap_budget_advance_stable.sh`. Green: `ux_ok wrap_budget_advance_stable` — long mixed-script line wraps so last glyph x of each visual row ≤ content right edge; auto-discovered by `run_ux_gate` |
| Module touch | `misc/editor/layout/word_wrap.mlc` (+ thin shaping helper if needed), `demo_live` call sites, `scripts/run_ux_wrap_budget_advance_*.sh`, scenario under `ux_scenarios/` |
| REG | no |
| Out of scope | Proportional caret/selection/hit-test; ICU grapheme/ZWJ; Profile full-frame; SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: no wrap-budget advance script | **done** |
| 2 | Implement advance-sum wrap; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | open |

<!-- STEP=1: red proves stable script missing or documents gap -->
<!-- STEP=2: pixel-budget wrap + demo wire; SKIP if font/shape missing -->
<!-- STEP=3: Critic archive -->

## Out of scope

Reopen #37/#49/#50/#51 numbered STEPs; proportional hit-test rewrite.
