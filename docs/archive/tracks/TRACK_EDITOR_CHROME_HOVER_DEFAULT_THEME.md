# Track: Editor chrome hover default from Theme

Parent: [../../PLAN.md](../../PLAN.md) §66.
Residual of [TRACK_EDITOR_CHROME_THEME_DRIFT](TRACK_EDITOR_CHROME_THEME_DRIFT.md)
(§46 #33c) / post-§65 idle. Size **S**.

## Status: **closed** (2026-07-24) — Critic OK

**Critic 2026-07-24 (STEP=3):** Re-ran `chrome_hover_default_theme_stable` ×2 +
`chrome_hover_tint_differs` + `chrome_panel_rgb_matches_theme` + `demo_live` compile +
HEAD `run_ux_gate` (81 scenarios).
Anti-false-done: `8f177065`…`ae896b1a` (STEP=0–2); default/stub →
`theme_solarized_light().panel_*`; `misc/editor/**` + scripts → REG skip; no
`compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: discard danger RGB still intentional OOS; `*_red` post-green fails.

| Gate | Result |
|------|--------|
| `run_ux_chrome_hover_default_theme_stable.sh` | `ux_ok chrome_hover_default_theme_stable` EXIT=0 (×2) |
| `run_ux_chrome_hover_tint_differs.sh` | `ux_ok chrome_hover_tint_differs` EXIT=0 |
| `run_ux_chrome_panel_rgb_matches_theme.sh` | `ux_ok chrome_panel_rgb_matches_theme` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (81 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `8f177065` | Decision: chrome hover default from Theme |
| 1 | `141f31ae` | Red/stable stub |
| 2 | `ae896b1a` | theme_solarized_light + L0 green |
| 3 | (this) | Critic close + archive |

## Decision (STEP=0) — **frozen** 2026-07-24

| Item | Choice |
|------|--------|
| Problem | After §33c/`from_panel`, `editor_ux_chrome_hover_rgb` and `editor_ux_chrome_hover_draw_report_stub` still hardcode Solarized Light `0.933, 0.910, 0.835` instead of reading `theme_solarized_light().panel_*`. Magic numbers drift from Theme |
| Strategy (v1) | Import `theme_solarized_light` in `ux/chrome_hover.mlc`; default hover helper + stub pass `theme.panel_*`. Update `chrome_hover_tint_differs` if it relies on bare literals. No new Theme fields |
| Primary gate | Red: no `run_ux_chrome_hover_default_theme_stable.sh`. Green: `ux_ok chrome_hover_default_theme_stable` — `chrome_hover.mlc` has no bare `0.933, 0.910, 0.835`; default path uses `theme_solarized_light` / `theme.panel_*`; auto-discovered by `run_ux_gate` |
| Module touch | `ux/chrome_hover.mlc`, scenario(s), scripts |
| REG | no |
| Out of scope | Discard danger RGB; new Theme fields; ICU; SCRIPT_VM; MIR Epic 5; reopen §33c / §65 numbered STEPs |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: bare Light RGB in chrome_hover | **done** |
| 2 | Wire theme_solarized_light; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | **done** — Critic OK |

## Out of scope

Discard button; SCRIPT_VM; MIR Epic 5.
