# Track: Editor text draw respects scroll_x

Parent: [../PLAN.md](../PLAN.md) §59.
Residual of [TRACK_EDITOR_WRAP_HIT_SCROLL_X](../archive/tracks/TRACK_EDITOR_WRAP_HIT_SCROLL_X.md)
(§58) / [TRACK_EDITOR_HORIZONTAL_SCROLL](../archive/tracks/TRACK_EDITOR_HORIZONTAL_SCROLL.md)
(§46 #17). Size **S**.

## Status: **active** (2026-07-24) — STEP=0 frozen → Driver STEP=1

## Next step

**STEP=1** — Red: prove glyph/caret/selection draw ignore `scroll_offset_x`.

## Decision (STEP=0) — **frozen** 2026-07-24

| Item | Choice |
|------|--------|
| Problem | After §58, wrap hit adds `scroll_offset_x`, but live draw still places syntax glyphs / selection fills / carets at `text_rect.x + 4` with no `- scroll_offset_x`. Visual text does not shift; click maps as if it did |
| Strategy (v1) | Subtract `app.ux.scroll_offset_x` from text pen_x, selection fill x, and caret x in `demo_live` (same origin as hit); thin helper optional |
| Primary gate | Red: no `run_ux_text_draw_scroll_x_stable.sh`. Green: `ux_ok text_draw_scroll_x_stable` — draw origin with scroll_x differs from origin without; auto-discovered by `run_ux_gate` |
| Module touch | `demo_live` draw sites (+ thin helper if needed), `scripts/run_ux_text_draw_scroll_x_*.sh`, `ux_scenarios/` |
| REG | no |
| Out of scope | Horizontal scrollbar UI; ICU; SCRIPT_VM; MIR Epic 5; reopen §58 numbered STEPs |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: draw ignores scroll_x | open |
| 2 | Subtract scroll_x on draw; green | open |
| 3 | Critic: stable + full `run_ux_gate` | open |

<!-- STEP=1: red proves syntax/caret/fill use text_rect.x+4 without scroll_offset_x -->
<!-- STEP=2: subtract scroll on pen/caret/fill; SKIP if font missing -->
<!-- STEP=3: Critic archive -->

## Out of scope

ICU; SCRIPT_VM; MIR Epic 5; horizontal scrollbar chrome.
