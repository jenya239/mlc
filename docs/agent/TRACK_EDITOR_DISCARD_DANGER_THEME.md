# Track: Editor discard danger button from Theme

Parent: [../PLAN.md](../PLAN.md) §67.
Residual of [TRACK_EDITOR_CHROME_HOVER_DEFAULT_THEME](../archive/tracks/TRACK_EDITOR_CHROME_HOVER_DEFAULT_THEME.md)
(§66) / overlay theme chain. Size **S**.

## Status: **open** — STEP=2 done; next Critic STEP=3

## Next step

**STEP=3** — Critic: stable ×2 + related overlay + demo_live compile + full `run_ux_gate`.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `8ce50d63` | Decision: Theme.danger_* + discard wire |
| 1 | `d7249641` | Red/stable stub |
| 2 | (this) | Theme.danger_* + discard wire + L0 green |

## Decision (STEP=0) — **frozen** 2026-07-24

| Item | Choice |
|------|--------|
| Problem | Dirty-close discard button in `demo_live` still draws bare `0.55, 0.22, 0.22` while panel/accent/overlays follow `Theme`. Last honest residual of the chrome theme-tint chain |
| Strategy (v1) | Add `danger_red`/`danger_green`/`danger_blue` to `Theme`; set Solarized Light to current discard RGB; set dark palette to a matching danger red; draw discard rect with `theme.danger_*` |
| Primary gate | Red: no green `run_ux_discard_danger_theme_stable.sh`. Green: `ux_ok discard_danger_theme_stable` — Theme has `danger_*`; discard draw uses `theme.danger_*`; no bare `0.55, 0.22, 0.22` in `demo_live`; auto-discovered by `run_ux_gate` |
| Module touch | `ui/theme.mlc`, `demo_live` discard draw, scripts/scenario |
| REG | no |
| Out of scope | New chrome widgets; ICU; SCRIPT_VM; MIR Epic 5; reopen §64–§66 numbered STEPs |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: bare discard RGB + stable stub | **done** |
| 2 | Theme.danger_* + wire discard; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | pending |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE.
