# Track: Editor scrollbar thumb hover lift

Parent: [../../PLAN.md](../../PLAN.md) §71.
Residual of [TRACK_EDITOR_NAV_CRUMB_HOVER](TRACK_EDITOR_NAV_CRUMB_HOVER.md)
(§69) / [TRACK_EDITOR_CHROME_HOVER_STATE](TRACK_EDITOR_CHROME_HOVER_STATE.md)
(§46 #32). Size **S**.

## Status: **closed** (2026-07-25) — Critic OK

**Critic 2026-07-25 (STEP=3):** Re-ran `scrollbar_thumb_hover_stable` ×2 +
`scrollbar_theme_tint_stable` + `chrome_hover_tint_differs` + `demo_live` compile +
HEAD `run_ux_gate`.
Anti-false-done: `c6edcfc2`…`1e49413a` (STEP=0–2); content Y/X + folder thumbs →
`chrome_hover_draw_entry` (muted base); `misc/editor/**` + scripts → REG skip; no
`compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: `*_red` post-green fails (use stable only); #32 chrome-hover chain largely done.

| Gate | Result |
|------|--------|
| `run_ux_scrollbar_thumb_hover_stable.sh` | `ux_ok scrollbar_thumb_hover_stable` EXIT=0 (×2) |
| `run_ux_scrollbar_theme_tint_stable.sh` | `ux_ok scrollbar_theme_tint_stable` EXIT=0 |
| `run_ux_chrome_hover_tint_differs.sh` | `ux_ok chrome_hover_tint_differs` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (86 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `c6edcfc2` | Decision: thumb hover via chrome_hover |
| 1 | `d3353b35` | Red/stable stub |
| 2 | `1e49413a` | chrome_hover wire + L0 green |
| 3 | (this) | Critic close + archive |

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
| 1 | Red: flat muted thumbs | **done** |
| 2 | Wire chrome_hover; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | **done** — Critic OK |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE.
