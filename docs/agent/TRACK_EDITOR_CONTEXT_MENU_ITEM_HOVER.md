# Track: Editor context-menu item hover

Parent: [../PLAN.md](../PLAN.md) §72.
Residual of [TRACK_EDITOR_CHROME_HOVER_STATE](../archive/tracks/TRACK_EDITOR_CHROME_HOVER_STATE.md)
(§46 #32) / [TRACK_EDITOR_CONTEXT_MENU](../archive/tracks/TRACK_EDITOR_CONTEXT_MENU.md)
(§46 #25). Size **S**.

## Status: **open** — STEP=0 done; next Driver STEP=1

## Next step

**STEP=1** — Red: assert menu items draw flat `theme.muted_*` without `chrome_hover_draw_entry`; stable stub `not implemented`.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | (this) | Decision: menu item hover via chrome_hover |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | Context-menu item rows in `demo_live` always paint flat `theme.muted_*` with no hover lift, while tabs/toolbar/tree/nav/thumbs already use `editor_ux_chrome_hover_draw_entry`. Residual of #32 after §71 |
| Strategy (v1) | Pass each `editor_ux_context_menu_item_rect` through `editor_ux_chrome_hover_draw_entry` with muted base before solid fill. Keep panel fill for menu chrome. No Theme fields |
| Primary gate | Red: no green `run_ux_context_menu_item_hover_stable.sh`. Green: `ux_ok context_menu_item_hover_stable` — item draw loop calls `editor_ux_chrome_hover_draw_entry`; auto-discovered by `run_ux_gate` |
| Module touch | `demo_live` context-menu draw, scripts/scenario |
| REG | no |
| Out of scope | Keyboard menu nav; OS native menu; dirty-close button hover; ICU; SCRIPT_VM; MIR Epic 5; reopen #25/#32 numbered STEPs |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: flat muted item fills | pending |
| 2 | Wire chrome_hover; green | pending |
| 3 | Critic: stable + full `run_ux_gate` | pending |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE.
