# Track: Editor context-menu item hover

Parent: [../../PLAN.md](../../PLAN.md) §72.
Residual of [TRACK_EDITOR_CHROME_HOVER_STATE](TRACK_EDITOR_CHROME_HOVER_STATE.md)
(§46 #32) / [TRACK_EDITOR_CONTEXT_MENU](TRACK_EDITOR_CONTEXT_MENU.md)
(§46 #25). Size **S**.

## Status: **closed** (2026-07-25) — Critic OK

**Critic 2026-07-25 (STEP=3):** Re-ran `context_menu_item_hover_stable` ×2 +
`overlay_theme_tint_stable` + `chrome_hover_tint_differs` + `demo_live` compile +
HEAD `run_ux_gate`.
Anti-false-done: `598bba3d`…`74fb0662` (STEP=0–2); menu item loop →
`chrome_hover_draw_entry` (muted base); `misc/editor/**` + scripts → REG skip; no
`compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: `*_red` post-green fails (use stable only); dirty-close button hover
still OOS; minimap glyphs theme-text only (no syntax tags); #32 chrome-hover chain
otherwise done for tabs/toolbar/tree/nav/thumbs/menu items.

| Gate | Result |
|------|--------|
| `run_ux_context_menu_item_hover_stable.sh` | `ux_ok context_menu_item_hover_stable` EXIT=0 (×2) |
| `run_ux_overlay_theme_tint_stable.sh` | `ux_ok overlay_theme_tint_stable` EXIT=0 |
| `run_ux_chrome_hover_tint_differs.sh` | `ux_ok chrome_hover_tint_differs` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (87 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `598bba3d` | Decision: menu item hover via chrome_hover |
| 1 | `16a5ef4f` | Red/stable stub |
| 2 | `74fb0662` | chrome_hover wire + L0 green |
| 3 | (this) | Critic close + archive |

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
| 1 | Red: flat muted item fills | **done** |
| 2 | Wire chrome_hover; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | **done** — Critic OK |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE.
