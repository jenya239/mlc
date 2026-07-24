# Track: Editor scrollbar thumb theme tint

Parent: [../PLAN.md](../PLAN.md) §62.
Residual of [TRACK_EDITOR_SCROLLBAR_THUMB_DRAG](../archive/tracks/TRACK_EDITOR_SCROLLBAR_THUMB_DRAG.md)
(§61) / [TRACK_EDITOR_CONTENT_SCROLLBAR](../archive/tracks/TRACK_EDITOR_CONTENT_SCROLLBAR.md)
(§46 #33e). Size **S**.

## Status: **active** (2026-07-24) — STEP=0 done → Driver STEP=1

## Next step

**STEP=1** — Red: content/folder thumbs still hardcoded `0.45/0.48/0.55`; failing stable stub.

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
| 1 | Red: hardcoded thumb tint | open |
| 2 | Wire theme.muted_*; green | open |
| 3 | Critic: stable + full `run_ux_gate` | open |

<!-- STEP=1: red proves 0.45/0.48/0.55 at thumb draw sites -->
<!-- STEP=2: replace with theme.muted_*; SKIP if font missing -->
<!-- STEP=3: Critic archive -->

## Out of scope

Folder thumb drag; SCRIPT_VM; MIR Epic 5.
