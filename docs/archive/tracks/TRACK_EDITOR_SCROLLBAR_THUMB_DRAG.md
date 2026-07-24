# Track: Editor content scrollbar thumb drag

Parent: [../../PLAN.md](../../PLAN.md) §61.
Residual of [TRACK_EDITOR_HORIZONTAL_SCROLLBAR](TRACK_EDITOR_HORIZONTAL_SCROLLBAR.md)
(§60) / [TRACK_EDITOR_CONTENT_SCROLLBAR](TRACK_EDITOR_CONTENT_SCROLLBAR.md)
(§46 #33e). Size **S**.

## Status: **closed** (2026-07-24) — Critic OK

**Critic 2026-07-24 (STEP=3):** Re-ran `scrollbar_thumb_drag_stable` ×2 + `content_scrollbar_thumb` +
`horizontal_scrollbar_stable` + `shift_wheel_scrolls_horizontal` + `demo_live` compile +
HEAD `run_ux_gate` (76 scenarios).
Anti-false-done: `30c7388f`…`f24dcdd7` (STEP=0–2); `content_scrollbar_scroll_at_y/x` +
demo press/drag; `misc/editor/**` + scripts → REG skip; no `compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: theme thumb tint still hardcoded; folder-tree thumb drag still OOS;
`*_red` post-green fails (use stable only).

| Gate | Result |
|------|--------|
| `run_ux_scrollbar_thumb_drag_stable.sh` | `ux_ok scrollbar_thumb_drag_stable` EXIT=0 (×2) |
| `run_ux_content_scrollbar_thumb.sh` | `ux_ok content_scrollbar_thumb_on_hover` EXIT=0 |
| `run_ux_horizontal_scrollbar_stable.sh` | `ux_ok horizontal_scrollbar_stable` EXIT=0 |
| `run_ux_shift_wheel_scrolls_horizontal.sh` | `ux_ok shift_wheel_scrolls_horizontal` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (76 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `30c7388f` | Decision: scrollbar thumb drag |
| 1 | `6211cce8` | Red/stable stub |
| 2 | `f24dcdd7` | helpers + demo + green stable |
| 3 | (this) | Critic close + archive |

## Decision (STEP=0) — **frozen** 2026-07-24

| Item | Choice |
|------|--------|
| Problem | Vertical + horizontal content thumbs draw on hover (§33e/§60), but mouse drag on thumb does not change `scroll_offset_y` / `scroll_offset_x`. Minimap already maps click/drag → scroll; content thumbs do not |
| Strategy (v1) | Hit-test point-in-thumb; on press+drag map pointer position along track → scroll (same geometry inverse as thumb layout). Y thumb → `scroll_offset_y`; X thumb → `scroll_offset_x`. Thin helpers in `ux/scroll.mlc`; wire `demo_live` mouse path. No always-on chrome change |
| Primary gate | Red: no `run_ux_scrollbar_thumb_drag_stable.sh`. Green: `ux_ok scrollbar_thumb_drag_stable` — drag along Y track changes scroll_y; drag along X track changes scroll_x; auto-discovered by `run_ux_gate` |
| Module touch | `ux/scroll.mlc`, `demo_live` input, scripts/scenario |
| REG | no |
| Out of scope | Theme thumb tint; folder-tree thumb drag; ICU; SCRIPT_VM; MIR Epic 5; reopen §60 numbered STEPs |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: no thumb drag | **done** |
| 2 | Helpers + demo wire; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | **done** — Critic OK |

## Out of scope

Theme tokens; folder scrollbar drag; SCRIPT_VM; MIR Epic 5.
