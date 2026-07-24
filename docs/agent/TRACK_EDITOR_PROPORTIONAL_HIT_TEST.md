# Track: Editor proportional hit-test / caret / selection

Parent: [../PLAN.md](../PLAN.md) §53.
Residual of [TRACK_EDITOR_WRAP_BUDGET_ADVANCE](../archive/tracks/TRACK_EDITOR_WRAP_BUDGET_ADVANCE.md)
(§52 Decision OOS). Size **M**.

## Status: **active** (2026-07-24) — STEP=0 done → Driver STEP=1

## Next step

**STEP=1** — Red: no proportional hit-test gate (gap).

## Decision (STEP=0) — **frozen** 2026-07-24

| Item | Choice |
|------|--------|
| Problem | Wrap rows use pixel-budget shaped advances (§52), but click hit-test (`wrap_offset_at_point`), caret x, and selection fills still use mono `char_width` display columns → click/caret drift on mixed-script / variable-width glyphs |
| Strategy (v1) | Within a visual wrap row, map pixel↔byte via cumulative shaped advances (same shaping path as `visual_rows_for_line_pixel_budget`); wire `demo_live` caret draw + selection highlight + click; keep column APIs as fallback when shape fails |
| Primary gate | Red: no `run_ux_proportional_hit_test_stable.sh`. Green: `ux_ok proportional_hit_test_stable` — click/caret x align with shaped glyph edges on a mixed-script wrapped line; auto-discovered by `run_ux_gate` |
| Module touch | `word_wrap.mlc` (`wrap_offset_at_point` + caret/selection pixel helpers), `demo_live`, `scripts/run_ux_proportional_hit_test_*.sh`, `ux_scenarios/` |
| REG | no |
| Out of scope | ICU grapheme/ZWJ; rewrite all column APIs globally; SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; reopen §52 numbered STEPs |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: no proportional hit-test script | open |
| 2 | Implement advance-sum hit/caret/selection; green | open |
| 3 | Critic: stable + full `run_ux_gate` | open |

<!-- STEP=1: red proves stable script missing or documents gap -->
<!-- STEP=2: pixel hit-test + demo wire; SKIP if font/shape missing -->
<!-- STEP=3: Critic archive -->

## Out of scope

ICU; full column-model delete; SCRIPT_VM; MIR Epic 5.
