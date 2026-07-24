# Track: Editor minimap viewport indicator theme tint

Parent: [../../PLAN.md](../../PLAN.md) §65.
Residual of [TRACK_EDITOR_OVERLAY_THEME_TINT](TRACK_EDITOR_OVERLAY_THEME_TINT.md)
(§64) / [TRACK_EDITOR_MINIMAP](TRACK_EDITOR_MINIMAP.md)
(§46 #35). Size **S**.

## Status: **closed** (2026-07-24) — Critic OK

**Critic 2026-07-24 (STEP=3):** Re-ran `minimap_indicator_theme_tint_stable` ×2 +
`scrollbar_theme_tint_stable` + `overlay_theme_tint_stable` + `demo_live` compile +
HEAD `run_ux_gate` (80 scenarios).
Anti-false-done: `e3e34e2c`…`a21da97b` (STEP=0–2); indicator → `theme.muted_*`;
`misc/editor/**` + scripts → REG skip; no `compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: `*_red` post-green fails (use stable only); discard danger red intentional.

| Gate | Result |
|------|--------|
| `run_ux_minimap_indicator_theme_tint_stable.sh` | `ux_ok minimap_indicator_theme_tint_stable` EXIT=0 (×2) |
| `run_ux_scrollbar_theme_tint_stable.sh` | `ux_ok scrollbar_theme_tint_stable` EXIT=0 |
| `run_ux_overlay_theme_tint_stable.sh` | `ux_ok overlay_theme_tint_stable` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (80 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `e3e34e2c` | Decision: minimap indicator tint |
| 1 | `b914dd1f` | Red/stable stub + SESSION archive |
| 2 | `a21da97b` | theme.muted_* + L0 green |
| 3 | (this) | Critic close + archive |

## Decision (STEP=0) — **frozen** 2026-07-24

| Item | Choice |
|------|--------|
| Problem | Minimap viewport indicator in `demo_live` draws with hardcoded `0.35, 0.40, 0.45` while chrome/scrollbars/overlays now follow `Theme`. Tint does not follow light/dark theme |
| Strategy (v1) | Draw indicator with `theme.muted_*` (same pattern as scrollbar thumbs §62). No new Theme fields. Touch `demo_live` draw site only |
| Primary gate | Red: no `run_ux_minimap_indicator_theme_tint_stable.sh`. Green: `ux_ok minimap_indicator_theme_tint_stable` — indicator draw uses `theme.muted_*`, no bare `0.35, 0.40, 0.45`; auto-discovered by `run_ux_gate` |
| Module touch | `demo_live` draw, scripts/scenario (grep/L0 ok) |
| REG | no |
| Out of scope | Minimap layout/cache; discard danger red; ICU; SCRIPT_VM; MIR Epic 5; reopen §64 / #35 numbered STEPs |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: hardcoded indicator tint | **done** |
| 2 | Wire theme.muted_*; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | **done** — Critic OK |

## Out of scope

Minimap geometry; SCRIPT_VM; MIR Epic 5.
