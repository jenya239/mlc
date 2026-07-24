# Track: Editor discard danger button from Theme

Parent: [../../PLAN.md](../../PLAN.md) §67.
Residual of [TRACK_EDITOR_CHROME_HOVER_DEFAULT_THEME](TRACK_EDITOR_CHROME_HOVER_DEFAULT_THEME.md)
(§66) / overlay theme chain. Size **S**.

## Status: **closed** (2026-07-24) — Critic OK

**Critic 2026-07-24 (STEP=3):** Re-ran `discard_danger_theme_stable` ×2 +
`overlay_theme_tint_stable` + `chrome_hover_default_theme_stable` + `demo_live` compile +
HEAD `run_ux_gate` (82 scenarios).
Anti-false-done: `8ce50d63`…`c9190ac0` (STEP=0–2); discard → `theme.danger_*`;
Theme fields on light/dark; `misc/editor/**` + scripts → REG skip; no
`compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: `*_red` post-green fails (use stable only); theme-tint chrome chain largely done.

| Gate | Result |
|------|--------|
| `run_ux_discard_danger_theme_stable.sh` | `ux_ok discard_danger_theme_stable` EXIT=0 (×2) |
| `run_ux_overlay_theme_tint_stable.sh` | `ux_ok overlay_theme_tint_stable` EXIT=0 |
| `run_ux_chrome_hover_default_theme_stable.sh` | `ux_ok chrome_hover_default_theme_stable` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (82 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `8ce50d63` | Decision: Theme.danger_* + discard wire |
| 1 | `d7249641` | Red/stable stub |
| 2 | `c9190ac0` | Theme.danger_* + discard wire + L0 green |
| 3 | (this) | Critic close + archive |

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
| 3 | Critic: stable + full `run_ux_gate` | **done** — Critic OK |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE.
