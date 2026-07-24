# Track: Editor chrome hover default from Theme

Parent: [../PLAN.md](../PLAN.md) §66.
Residual of [TRACK_EDITOR_CHROME_THEME_DRIFT](../archive/tracks/TRACK_EDITOR_CHROME_THEME_DRIFT.md)
(§46 #33c) / post-§65 idle. Size **S**.

## Status: **active** (2026-07-24) — STEP=0 done → Driver STEP=1

## Next step

**STEP=1** — Red: chrome_hover still embeds bare Light panel RGB; failing stable stub.

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
| 1 | Red: bare Light RGB in chrome_hover | open |
| 2 | Wire theme_solarized_light; green | open |
| 3 | Critic: stable + full `run_ux_gate` | open |

<!-- STEP=1: red proves 0.933/0.910/0.835 in chrome_hover; stable stub -->
<!-- STEP=2: theme_solarized_light().panel_*; green L0 -->
<!-- STEP=3: Critic archive -->

## Out of scope

Discard button; SCRIPT_VM; MIR Epic 5.
