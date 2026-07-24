# Track: Editor active-tab idle tint

Parent: [../../PLAN.md](../../PLAN.md) §68.
Residual of [TRACK_EDITOR_CHROME_HOVER_STATE](TRACK_EDITOR_CHROME_HOVER_STATE.md)
(§46 #32). Size **S**.

## Status: **closed** (2026-07-25) — Critic OK

**Critic 2026-07-25 (STEP=3):** Re-ran `active_tab_tint_stable` ×2 +
`chrome_hover_default_theme_stable` + `chrome_hover_tint_differs` + `demo_live` compile +
HEAD `run_ux_gate` (83 scenarios).
Anti-false-done: `7058b240`…`c786e5ec` (STEP=0–2); active → `theme.selection_*`;
inactive → `theme.panel_*`; `[title]` kept; `misc/editor/**` + scripts → REG skip; no
`compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: `*_red` post-green fails (use stable only); `[title]` brackets kept by Decision.

| Gate | Result |
|------|--------|
| `run_ux_active_tab_tint_stable.sh` | `ux_ok active_tab_tint_stable` EXIT=0 (×2) |
| `run_ux_chrome_hover_default_theme_stable.sh` | `ux_ok chrome_hover_default_theme_stable` EXIT=0 |
| `run_ux_chrome_hover_tint_differs.sh` | `ux_ok chrome_hover_tint_differs` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (83 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `7058b240` | Decision: active-tab idle from Theme.selection_* |
| 1 | `c2907e19` | Red/stable stub |
| 2 | `c786e5ec` | selection_* for active + L0 green |
| 3 | (this) | Critic close + archive |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | Active tab is marked only by text brackets `[title]`; tab fill still uses the same `theme.panel_*` idle path as inactive tabs (`demo_live` hover loop). Honest residual of §46 #32 |
| Strategy (v1) | For the active tab index, pass `theme.selection_*` (not `theme.panel_*`) into `editor_ux_chrome_hover_draw_entry` so idle fill differs; hover lift still via existing helper. Keep `[title]` text mark. No new Theme fields |
| Primary gate | Red: no green `run_ux_active_tab_tint_stable.sh`. Green: `ux_ok active_tab_tint_stable` — active-tab draw path uses `theme.selection_*`; inactive keeps `theme.panel_*`; auto-discovered by `run_ux_gate` |
| Module touch | `demo_live` tab fill loop, scripts/scenario |
| REG | no |
| Out of scope | Remove `[title]` brackets; toolbar/tree active states; ICU; SCRIPT_VM; MIR Epic 5; reopen #32 numbered STEPs |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: active==inactive panel idle | **done** |
| 2 | Wire selection_* for active; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | **done** — Critic OK |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE.
