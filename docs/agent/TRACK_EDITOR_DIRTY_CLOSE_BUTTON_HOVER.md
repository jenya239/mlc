# Track: Editor dirty-close button hover

Parent: [../PLAN.md](../PLAN.md) §73.
Residual of [TRACK_EDITOR_CONTEXT_MENU_ITEM_HOVER](../archive/tracks/TRACK_EDITOR_CONTEXT_MENU_ITEM_HOVER.md)
(§72) / [TRACK_EDITOR_CHROME_HOVER_STATE](../archive/tracks/TRACK_EDITOR_CHROME_HOVER_STATE.md)
(§46 #32) / [TRACK_EDITOR_DISCARD_DANGER_THEME](../archive/tracks/TRACK_EDITOR_DISCARD_DANGER_THEME.md)
(§67). Size **S**.

## Status: **open** — STEP=2 done; next Critic STEP=3

## Next step

**STEP=3** — Critic: stable×2 + `run_ux_gate`; close.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | e6d88360 | Decision: dirty-close buttons via chrome_hover |
| 1 | 99a7e297 | Red harness + stable stub `not implemented` |
| 2 | (this) | Wire chrome_hover on discard/cancel; green stable |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | Dirty-close discard/cancel buttons in `demo_live` paint flat `theme.danger_*` / `theme.accent_*` with no hover lift, while tabs/toolbar/tree/nav/thumbs/menu items already use `editor_ux_chrome_hover_draw_entry`. Honest residual after §72 |
| Strategy (v1) | Pass discard rect through `editor_ux_chrome_hover_draw_entry` with danger base; cancel rect with accent base. Keep overlay panel fill flat `theme.panel_*`. Prefer `dirty_close_overlay_discard_rect` / `cancel_rect` if already imported; else match existing hardcoded geometry. No new Theme fields |
| Primary gate | Red: no green `run_ux_dirty_close_button_hover_stable.sh`. Green: `ux_ok dirty_close_button_hover_stable` — discard + cancel draw sites call `editor_ux_chrome_hover_draw_entry`; auto-discovered by `run_ux_gate` |
| Module touch | `demo_live` dirty-close draw, scripts/scenario |
| REG | no |
| Out of scope | Keyboard overlay nav; minimap syntax tags; ICU; SCRIPT_VM; MIR Epic 5; reopen #32 / §67 / §72 numbered STEPs |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: flat danger/accent button fills | **done** |
| 2 | Wire chrome_hover; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | pending |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE.
