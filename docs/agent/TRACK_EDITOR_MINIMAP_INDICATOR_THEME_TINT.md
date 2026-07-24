# Track: Editor minimap viewport indicator theme tint

Parent: [../PLAN.md](../PLAN.md) §65.
Residual of [TRACK_EDITOR_OVERLAY_THEME_TINT](../archive/tracks/TRACK_EDITOR_OVERLAY_THEME_TINT.md)
(§64) / [TRACK_EDITOR_MINIMAP](../archive/tracks/TRACK_EDITOR_MINIMAP.md)
(§46 #35). Size **S**.

## Status: **active** (2026-07-24) — STEP=2 done → Critic STEP=3

## Next step

**STEP=3** — Critic: stable×2 + full `run_ux_gate`.

## Decision (STEP=0) — **frozen** 2026-07-24

| Item | Choice |
|------|--------|
| Problem | Minimap viewport indicator in `demo_live` draws with hardcoded `0.35, 0.40, 0.45` while chrome/scrollbars/overlays now follow `Theme`. Tint does not follow light/dark theme |
| Strategy (v1) | Draw indicator with `theme.muted_*` (same pattern as scrollbar thumbs §62). No new Theme fields. Touch `demo_live` draw site only |
| Primary gate | Red: no `run_ux_minimap_indicator_theme_tint_stable.sh`. Green: `ux_ok minimap_indicator_theme_tint_stable` — indicator draw uses `theme.muted_*`, no bare `0.35, 0.40, 0.45`; auto-discovered by `run_ux_gate` |
| Module touch | `demo_live` draw, scripts/scenario (grep/L0 ok) |
| REG | no |
| Out of scope | Minimap layout/cache; discard danger red; ICU; SCRIPT_VM; MIR Epic 5; reopen §64 / #35 numbered STEPs |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: hardcoded indicator tint | **done** |
| 2 | Wire theme.muted_*; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | open |

<!-- STEP=1: red proves 0.35/0.40/0.45 at indicator; stable stub not implemented -->
<!-- STEP=2: theme.muted_* at indicator site -->
<!-- STEP=3: Critic archive -->

## Out of scope

Minimap geometry; SCRIPT_VM; MIR Epic 5.
