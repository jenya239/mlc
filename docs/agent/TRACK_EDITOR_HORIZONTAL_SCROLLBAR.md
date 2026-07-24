# Track: Editor horizontal content scrollbar

Parent: [../PLAN.md](../PLAN.md) §60.
Residual of [TRACK_EDITOR_TEXT_DRAW_SCROLL_X](../archive/tracks/TRACK_EDITOR_TEXT_DRAW_SCROLL_X.md)
(§59) / [TRACK_EDITOR_CONTENT_SCROLLBAR](../archive/tracks/TRACK_EDITOR_CONTENT_SCROLLBAR.md)
(§46 #33e). Size **S**.

## Status: **active** (2026-07-24) — STEP=2 done → Critic STEP=3

## Next step

**STEP=3** — Critic: stable + full `run_ux_gate`.

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
| 3 | Critic: stable + full `run_ux_gate` | open |

<!-- STEP=1: red proves demo has vertical thumb only; no horizontal helper -->
<!-- STEP=2: editor_ux_content_scrollbar_thumb_x + demo wire; SKIP if font missing -->
<!-- STEP=3: Critic archive -->

## Out of scope

Thumb drag; theme thumb color; SCRIPT_VM; MIR Epic 5.
