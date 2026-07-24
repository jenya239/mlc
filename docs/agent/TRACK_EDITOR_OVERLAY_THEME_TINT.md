# Track: Editor overlay / menu / nav theme tint

Parent: [../PLAN.md](../PLAN.md) §64.
Residual of [TRACK_EDITOR_CHROME_THEME_DRIFT](../archive/tracks/TRACK_EDITOR_CHROME_THEME_DRIFT.md)
(§46 #33c) / post-§63 idle. Size **S**.

## Status: **active** (2026-07-24) — STEP=1 done → Driver STEP=2

## Next step

**STEP=2** — Wire theme.*; green stable.

## Decision (STEP=0) — **frozen** 2026-07-24

| Item | Choice |
|------|--------|
| Problem | Tab/toolbar/breadcrumb use `theme.panel_*` (Solarized Light), but dirty-close overlay panel, context-menu panel/items, and folder nav back/forward button fills still use hardcoded dark RGB (`0.16/0.18/0.22`, `0.18/0.20/0.24`, `0.22/0.24/0.28`, `0.22/0.24/0.30`). Light chrome + dark popups/nav reads broken |
| Strategy (v1) | Route those fills through existing Theme: panel/menu/nav → `theme.panel_*` (menu item rows may use `theme.muted_*` for slight lift). Keep discard button danger red as-is (`0.55/0.22/0.22`) and cancel already on `theme.accent_*`. No new Theme fields. Touch `demo_live` draw sites only |
| Primary gate | Red: no `run_ux_overlay_theme_tint_stable.sh`. Green: `ux_ok overlay_theme_tint_stable` — demo draw sites for dirty-close panel, context-menu panel/items, nav back/forward use `theme.panel_*` and/or `theme.muted_*`, not the bare dark triples above; auto-discovered by `run_ux_gate` |
| Module touch | `demo_live` draw, scripts/scenario (grep/L0 ok) |
| REG | no |
| Out of scope | Minimap indicator tint; changing Solarized values; new Theme fields; ICU; SCRIPT_VM; MIR Epic 5; reopen §33c / §63 numbered STEPs |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: hardcoded overlay/menu/nav | **done** |
| 2 | Wire theme.*; green | open |
| 3 | Critic: stable + full `run_ux_gate` | open |

<!-- STEP=1: red proves dark constants at overlay/menu/nav; stable stub not implemented -->
<!-- STEP=2: theme.panel_*/muted_* at those sites; SKIP if font missing -->
<!-- STEP=3: Critic archive -->

## Out of scope

Minimap indicator; SCRIPT_VM; MIR Epic 5; theme palette redesign.
