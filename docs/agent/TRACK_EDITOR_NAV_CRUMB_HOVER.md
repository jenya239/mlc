# Track: Editor folder nav/breadcrumb hover

Parent: [../PLAN.md](../PLAN.md) §69.
Residual of [TRACK_EDITOR_CHROME_HOVER_STATE](../archive/tracks/TRACK_EDITOR_CHROME_HOVER_STATE.md)
(§46 #32). Size **S**.

## Status: **open** — STEP=2 done; next Critic STEP=3

## Next step

**STEP=3** — Critic: stable ×2 + overlay + demo_live compile + full `run_ux_gate`.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `0d14ad1c` | Decision: nav/crumb hover via chrome_hover |
| 1 | `d4f6cb7c` | Red/stable stub |
| 2 | (this) | chrome_hover wire + L0 green |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | After §46 #32, toolbar/tabs/tree-rows use `editor_ux_chrome_hover_draw_entry`, but folder nav back/forward and breadcrumb bar still draw flat `theme.panel_*` rects with no hover lift. Honest residual of #32 |
| Strategy (v1) | Pass `nav_back_rect` / `nav_forward_rect` / `breadcrumb_rect` through `editor_ux_chrome_hover_draw_entry` (panel base) before solid fill, same pattern as toolbar buttons. No new Theme fields |
| Primary gate | Red: no green `run_ux_nav_crumb_hover_stable.sh`. Green: `ux_ok nav_crumb_hover_stable` — nav back/forward + breadcrumb draw sites call `editor_ux_chrome_hover_draw_entry`; auto-discovered by `run_ux_gate` |
| Module touch | `demo_live` nav/crumb draw, scripts/scenario |
| REG | no |
| Out of scope | Remove `[title]` brackets; scrollbar thumb hover fill; ICU; SCRIPT_VM; MIR Epic 5; reopen #32 / §68 numbered STEPs |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: flat nav/crumb fills | **done** |
| 2 | Wire chrome_hover; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | pending |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE.
