# Track: Editor horizontal content scrollbar

Parent: [../../PLAN.md](../../PLAN.md) §60.
Residual of [TRACK_EDITOR_TEXT_DRAW_SCROLL_X](TRACK_EDITOR_TEXT_DRAW_SCROLL_X.md)
(§59) / [TRACK_EDITOR_CONTENT_SCROLLBAR](TRACK_EDITOR_CONTENT_SCROLLBAR.md)
(§46 #33e). Size **S**.

## Status: **closed** (2026-07-24) — Critic OK

**Critic 2026-07-24 (STEP=3):** Re-ran `horizontal_scrollbar_stable` ×2 + `content_scrollbar_thumb` +
`shift_wheel_scrolls_horizontal` + `text_draw_scroll_x_stable` + `demo_live` compile +
HEAD `run_ux_gate` (75 scenarios).
Anti-false-done: `9d3287e3`…`8f6e230e` (STEP=0–2); `editor_ux_content_scrollbar_thumb_x` +
demo hover draw; `misc/editor/**` + scripts → REG skip; no `compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: thumb drag still OOS; theme thumb tint still hardcoded; `*_red` post-green fails (use stable only).

| Gate | Result |
|------|--------|
| `run_ux_horizontal_scrollbar_stable.sh` | `ux_ok horizontal_scrollbar_stable` EXIT=0 (×2) |
| `run_ux_content_scrollbar_thumb.sh` | `ux_ok content_scrollbar_thumb_on_hover` EXIT=0 |
| `run_ux_shift_wheel_scrolls_horizontal.sh` | `ux_ok shift_wheel_scrolls_horizontal` EXIT=0 |
| `run_ux_text_draw_scroll_x_stable.sh` | `ux_ok text_draw_scroll_x_stable` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (75 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `9d3287e3` | Decision: horizontal scrollbar |
| 1 | `18bb9a98` | Red/stable stub |
| 2 | `8f6e230e` | helper + demo + green stable |
| 3 | (this) | Critic close + archive |

## Decision (STEP=0) — **frozen** 2026-07-24

| Item | Choice |
|------|--------|
| Problem | Vertical content thumb exists (`editor_ux_content_scrollbar_thumb`); Shift+wheel updates `scroll_offset_x`, but demo never draws a bottom-edge thumb when `content_width > viewport`. No visual affordance for horizontal scroll |
| Strategy (v1) | Mirror vertical: hover-gated horizontal thumb along bottom of `text_rect` (height 5, inset 2). Inputs: `content_width` (same as Shift+wheel: max line columns × `char_width`), `text_rect.width`, `scroll_offset_x`. Empty when content ≤ viewport or not hovered. Thin helper in `ux/scroll.mlc`; wire `demo_live` draw near vertical thumb |
| Primary gate | Red: no `run_ux_horizontal_scrollbar_stable.sh`. Green: `ux_ok horizontal_scrollbar_stable` — overflow → thumb width≥12 in track; fit → empty; hover=0 → empty; auto-discovered by `run_ux_gate` |
| Module touch | `ux/scroll.mlc`, `demo_live` draw, `scripts/run_ux_horizontal_scrollbar_*.sh`, `ux_scenarios/` |
| REG | no |
| Out of scope | Thumb drag; theme tokens; ICU; SCRIPT_VM; MIR Epic 5; reopen §59 numbered STEPs |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: no horizontal thumb | **done** |
| 2 | Helper + demo draw; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | **done** — Critic OK |

## Out of scope

Thumb drag; theme thumb color; SCRIPT_VM; MIR Epic 5.
