# Track: Editor overlay / menu / nav theme tint

Parent: [../../PLAN.md](../../PLAN.md) §64.
Residual of [TRACK_EDITOR_CHROME_THEME_DRIFT](TRACK_EDITOR_CHROME_THEME_DRIFT.md)
(§46 #33c) / post-§63 idle. Size **S**.

## Status: **closed** (2026-07-24) — Critic OK

**Critic 2026-07-24 (STEP=3):** Re-ran `overlay_theme_tint_stable` ×2 +
`chrome_panel_rgb_matches_theme` + `scrollbar_theme_tint_stable` + `demo_live` compile +
HEAD `run_ux_gate` (79 scenarios).
Anti-false-done: `d77fa0a3`…`c72841c5` (STEP=0–2); dirty-close/nav/menu panel →
`theme.panel_*`; menu items → `theme.muted_*`; discard danger kept; `misc/editor/**` +
scripts → REG skip; no `compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: minimap indicator tint still OOS; `*_red` post-green fails (use stable only).

| Gate | Result |
|------|--------|
| `run_ux_overlay_theme_tint_stable.sh` | `ux_ok overlay_theme_tint_stable` EXIT=0 (×2) |
| `run_ux_chrome_panel_rgb_matches_theme.sh` | `ux_ok chrome_panel_rgb_matches_theme` EXIT=0 |
| `run_ux_scrollbar_theme_tint_stable.sh` | `ux_ok scrollbar_theme_tint_stable` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (79 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `d77fa0a3` | Decision: overlay/menu/nav theme |
| 1 | `2b4eb1ee` | Red/stable stub |
| 2 | `c72841c5` | theme.panel_/muted_ + L0 green |
| 3 | (this) | Critic close + archive |

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
| 2 | Wire theme.*; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | **done** — Critic OK |

## Out of scope

Minimap indicator; SCRIPT_VM; MIR Epic 5; theme palette redesign.
