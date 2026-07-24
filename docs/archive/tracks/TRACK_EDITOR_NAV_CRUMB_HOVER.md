# Track: Editor folder nav/breadcrumb hover

Parent: [../../PLAN.md](../../PLAN.md) §69.
Residual of [TRACK_EDITOR_CHROME_HOVER_STATE](TRACK_EDITOR_CHROME_HOVER_STATE.md)
(§46 #32). Size **S**.

## Status: **closed** (2026-07-25) — Critic OK

**Critic 2026-07-25 (STEP=3):** Re-ran `nav_crumb_hover_stable` ×2 +
`overlay_theme_tint_stable` + `chrome_hover_tint_differs` + `demo_live` compile +
HEAD `run_ux_gate` (84 scenarios).
Anti-false-done: `0d14ad1c`…`d0268f6e` (STEP=0–2); crumb/nav →
`chrome_hover_draw_entry`; `misc/editor/**` + scripts → REG skip; no
`compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: `*_red` post-green fails (use stable only); scrollbar thumb hover fill still OOS (#32).

| Gate | Result |
|------|--------|
| `run_ux_nav_crumb_hover_stable.sh` | `ux_ok nav_crumb_hover_stable` EXIT=0 (×2) |
| `run_ux_overlay_theme_tint_stable.sh` | `ux_ok overlay_theme_tint_stable` EXIT=0 |
| `run_ux_chrome_hover_tint_differs.sh` | `ux_ok chrome_hover_tint_differs` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (84 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `0d14ad1c` | Decision: nav/crumb hover via chrome_hover |
| 1 | `d4f6cb7c` | Red/stable stub |
| 2 | `d0268f6e` | chrome_hover wire + L0 green |
| 3 | (this) | Critic close + archive |

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
| 3 | Critic: stable + full `run_ux_gate` | **done** — Critic OK |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE.
