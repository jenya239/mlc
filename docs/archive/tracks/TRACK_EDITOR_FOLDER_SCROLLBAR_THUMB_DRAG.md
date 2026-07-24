# Track: Editor folder scrollbar thumb drag

Parent: [../../PLAN.md](../../PLAN.md) §63.
Residual of [TRACK_EDITOR_SCROLLBAR_THEME_TINT](TRACK_EDITOR_SCROLLBAR_THEME_TINT.md)
(§62) / [TRACK_EDITOR_SCROLLBAR_THUMB_DRAG](TRACK_EDITOR_SCROLLBAR_THUMB_DRAG.md)
(§61). Size **S**.

## Status: **closed** (2026-07-24) — Critic OK

**Critic 2026-07-24 (STEP=3):** Re-ran `folder_scrollbar_thumb_drag_stable` ×2 +
`scrollbar_thumb_drag_stable` + `scrollbar_theme_tint_stable` + `demo_live` compile +
HEAD `run_ux_gate` (78 scenarios).
Anti-false-done: `4afce0fe`…`84253492` (STEP=0–2); `folder_scrollbar_track` /
`folder_scrollbar_scroll_at_y` + demo `scrollbar_drag=3`; `misc/editor/**` + scripts →
REG skip; no `compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: `*_red` post-green fails (use stable only).

| Gate | Result |
|------|--------|
| `run_ux_folder_scrollbar_thumb_drag_stable.sh` | `ux_ok folder_scrollbar_thumb_drag_stable` EXIT=0 (×2) |
| `run_ux_scrollbar_thumb_drag_stable.sh` | `ux_ok scrollbar_thumb_drag_stable` EXIT=0 |
| `run_ux_scrollbar_theme_tint_stable.sh` | `ux_ok scrollbar_theme_tint_stable` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (78 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `4afce0fe` | Decision: folder thumb drag |
| 1 | `6657dfee` | Red/stable stub |
| 2 | `84253492` | scroll_at_y + demo drag + L0 green |
| 3 | (this) | Critic close + archive |

## Decision (STEP=0) — **frozen** 2026-07-24

| Item | Choice |
|------|--------|
| Problem | Folder tree draws `folder_scrollbar_thumb` and wheel updates `tree_scroll_y`, but mouse drag on the thumb/track does not scroll. Content pane already has thumb drag (§61) |
| Strategy (v1) | Mirror content: track hit + `folder_scrollbar_scroll_at_y` (map pointer along track → scroll, reuse `folder_clamp_scroll` / `folder_max_scroll`). Wire `demo_live` press/drag on tree list track. Helpers in `ux/folder_panel.mlc` |
| Primary gate | Red: no `run_ux_folder_scrollbar_thumb_drag_stable.sh`. Green: `ux_ok folder_scrollbar_thumb_drag_stable` — pointer low vs high on track → different `tree_scroll_y`; auto-discovered by `run_ux_gate` |
| Module touch | `ux/folder_panel.mlc`, `demo_live` input, scripts/scenario |
| REG | no |
| Out of scope | Content thumb changes; theme; ICU; SCRIPT_VM; MIR Epic 5; reopen §62 numbered STEPs |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: no folder thumb drag | **done** |
| 2 | Helper + demo wire; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | **done** — Critic OK |

## Out of scope

SCRIPT_VM; MIR Epic 5; content scrollbar changes.
