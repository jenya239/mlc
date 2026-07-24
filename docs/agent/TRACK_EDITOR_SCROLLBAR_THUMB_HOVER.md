# Track: Editor scrollbar thumb hover lift

Parent: [../PLAN.md](../PLAN.md) §71.
Residual of [TRACK_EDITOR_NAV_CRUMB_HOVER](../archive/tracks/TRACK_EDITOR_NAV_CRUMB_HOVER.md)
(§69) / [TRACK_EDITOR_CHROME_HOVER_STATE](../archive/tracks/TRACK_EDITOR_CHROME_HOVER_STATE.md)
(§46 #32). Size **S**.

## Status: **open** — STEP=0 done; next Driver STEP=1

## Next step

**STEP=1** — Red: assert content/folder thumbs draw flat `theme.muted_*` without `chrome_hover_draw_entry`; stable stub `not implemented`.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | (this) | Decision: thumb hover via chrome_hover |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | Content Y/X and folder scrollbar thumbs draw flat `theme.muted_*` with no hover lift. Last #32 residual called out by §69/#70 Critic |
| Strategy (v1) | Pass each drawn thumb rect through `editor_ux_chrome_hover_draw_entry` with muted base (same pattern as toolbar). Keep visibility gating (editor/tree hovered). No Theme fields |
| Primary gate | Red: no green `run_ux_scrollbar_thumb_hover_stable.sh`. Green: `ux_ok scrollbar_thumb_hover_stable` — content Y/X + folder thumb draw sites call `editor_ux_chrome_hover_draw_entry`; auto-discovered by `run_ux_gate` |
| Module touch | `demo_live` thumb draw, scripts/scenario |
| REG | no |
| Out of scope | Thumb drag math; minimap syntax tags; ICU; SCRIPT_VM; MIR Epic 5; reopen #32 / §69 numbered STEPs |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: flat muted thumbs | pending |
| 2 | Wire chrome_hover; green | pending |
| 3 | Critic: stable + full `run_ux_gate` | pending |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE.
