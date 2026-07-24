# Track: Editor dirty-close button hover

Parent: [../../PLAN.md](../../PLAN.md) §73.
Residual of [TRACK_EDITOR_CONTEXT_MENU_ITEM_HOVER](TRACK_EDITOR_CONTEXT_MENU_ITEM_HOVER.md)
(§72) / [TRACK_EDITOR_CHROME_HOVER_STATE](TRACK_EDITOR_CHROME_HOVER_STATE.md)
(§46 #32) / [TRACK_EDITOR_DISCARD_DANGER_THEME](TRACK_EDITOR_DISCARD_DANGER_THEME.md)
(§67). Size **S**.

## Status: **closed** (2026-07-25) — Critic OK

**Critic 2026-07-25 (STEP=3):** Re-ran `dirty_close_button_hover_stable` ×2 +
`discard_danger_theme_stable` + `overlay_theme_tint_stable` + `chrome_hover_tint_differs` +
`demo_live` compile + HEAD `run_ux_gate`.
Anti-false-done: `e6d88360`…`b58523b3` (STEP=0–2); discard/cancel →
`chrome_hover_draw_entry` (danger/accent bases) via rect helpers; `misc/editor/**` +
scripts → REG skip; no `compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: `*_red` post-green fails (use stable only); minimap glyphs theme-text
only (no syntax tags); #32 chrome-hover chain done for tabs/toolbar/tree/nav/thumbs/
menu items/dirty-close buttons.

| Gate | Result |
|------|--------|
| `run_ux_dirty_close_button_hover_stable.sh` | `ux_ok dirty_close_button_hover_stable` EXIT=0 (×2) |
| `run_ux_discard_danger_theme_stable.sh` | `ux_ok discard_danger_theme_stable` EXIT=0 |
| `run_ux_overlay_theme_tint_stable.sh` | `ux_ok overlay_theme_tint_stable` EXIT=0 |
| `run_ux_chrome_hover_tint_differs.sh` | `ux_ok chrome_hover_tint_differs` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (88 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `e6d88360` | Decision: dirty-close buttons via chrome_hover |
| 1 | `99a7e297` | Red/stable stub |
| 2 | `b58523b3` | chrome_hover wire + L0 green |
| 3 | (this) | Critic close + archive |

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
| 3 | Critic: stable + full `run_ux_gate` | **done** — Critic OK |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE.
