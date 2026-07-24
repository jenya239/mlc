# Track: Editor scrollbar thumb theme tint

Parent: [../../PLAN.md](../../PLAN.md) §62.
Residual of [TRACK_EDITOR_SCROLLBAR_THUMB_DRAG](TRACK_EDITOR_SCROLLBAR_THUMB_DRAG.md)
(§61) / [TRACK_EDITOR_CONTENT_SCROLLBAR](TRACK_EDITOR_CONTENT_SCROLLBAR.md)
(§46 #33e). Size **S**.

## Status: **closed** (2026-07-24) — Critic OK

**Critic 2026-07-24 (STEP=3):** Re-ran `scrollbar_theme_tint_stable` ×2 + `content_scrollbar_thumb` +
`horizontal_scrollbar_stable` + `chrome_panel_rgb_matches_theme` + `demo_live` compile +
HEAD `run_ux_gate` (77 scenarios).
Anti-false-done: `8cf6c637`…`d4835288` (STEP=0–2); folder + content Y/X thumbs use
`theme.muted_*`; `misc/editor/**` + scripts → REG skip; no `compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: folder-tree thumb drag still OOS; `*_red` post-green fails (use stable only).

| Gate | Result |
|------|--------|
| `run_ux_scrollbar_theme_tint_stable.sh` | `ux_ok scrollbar_theme_tint_stable` EXIT=0 (×2) |
| `run_ux_content_scrollbar_thumb.sh` | `ux_ok content_scrollbar_thumb_on_hover` EXIT=0 |
| `run_ux_horizontal_scrollbar_stable.sh` | `ux_ok horizontal_scrollbar_stable` EXIT=0 |
| `run_ux_chrome_panel_rgb_matches_theme.sh` | `ux_ok chrome_panel_rgb_matches_theme` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (77 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `8cf6c637` | Decision: scrollbar theme tint |
| 1 | `ca1e31de` | Red/stable stub |
| 2 | `d4835288` | theme.muted_* + L0 green |
| 3 | (this) | Critic close + archive |

## Decision (STEP=0) — **frozen** 2026-07-24

| Item | Choice |
|------|--------|
| Problem | Content Y/X thumbs (and folder thumb) draw with hardcoded `0.45/0.48/0.55` while chrome elsewhere uses `Theme`. Tint does not follow light/dark theme |
| Strategy (v1) | Draw thumbs with `theme.muted_red/green/blue` (existing Theme field; dark already ~0.55/0.58/0.62). No new Theme fields. Touch `demo_live` draw sites only |
| Primary gate | Red: no `run_ux_scrollbar_theme_tint_stable.sh`. Green: `ux_ok scrollbar_theme_tint_stable` — demo draw sites use `theme.muted_*` for content/folder thumbs, no bare `0.45, 0.48, 0.55` at those sites; auto-discovered by `run_ux_gate` |
| Module touch | `demo_live` draw, scripts/scenario (grep/L0 ok) |
| REG | no |
| Out of scope | New Theme fields; folder thumb drag; ICU; SCRIPT_VM; MIR Epic 5; reopen §61 numbered STEPs |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: hardcoded thumb tint | **done** |
| 2 | Wire theme.muted_*; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | **done** — Critic OK |

## Out of scope

Folder thumb drag; SCRIPT_VM; MIR Epic 5.
