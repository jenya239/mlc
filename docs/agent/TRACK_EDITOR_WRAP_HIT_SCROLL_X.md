# Track: Editor wrap hit-test respects scroll_x

Parent: [../PLAN.md](../PLAN.md) §58.
Residual of [TRACK_EDITOR_HORIZONTAL_SCROLL](../archive/tracks/TRACK_EDITOR_HORIZONTAL_SCROLL.md)
(§46 #17) — exposed after §53 pixel hit API still omits `scroll_offset_x`.
Size **S**.

## Status: **active** (2026-07-24) — STEP=1 done → Driver STEP=2

## Next step

**STEP=2** — Add scroll_x to pixel hit; green stable.

## Decision (STEP=0) — **frozen** 2026-07-24

| Item | Choice |
|------|--------|
| Problem | Live wrap click uses `wrap_offset_at_point_pixel` with `scroll_offset_y` only; `content_x` ignores horizontal scroll. Non-wrap `nav_offset_at_point` already adds `metrics.scroll_offset_x`. Click/caret wrong when `scroll_offset_x > 0` under wrap |
| Strategy (v1) | Add `scroll_offset_x` to `wrap_offset_at_point_pixel` (`content_x += scroll_offset_x`); wire `demo_live` click; keep mono `wrap_offset_at_point` in sync if still used |
| Primary gate | Red: no `run_ux_wrap_hit_scroll_x_stable.sh`. Green: `ux_ok wrap_hit_scroll_x_stable` — same screen x → different byte when scroll_x differs; auto-discovered by `run_ux_gate` |
| Module touch | `layout/word_wrap.mlc`, `demo_live` click site, `scripts/run_ux_wrap_hit_scroll_x_*.sh`, `ux_scenarios/` |
| REG | no |
| Out of scope | Horizontal scrollbar UI; ICU; SCRIPT_VM; MIR Epic 5; full-frame idle reshape (§57 OOS) |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: hit API ignores scroll_x | **done** |
| 2 | Add scroll_x to pixel hit; green | open |
| 3 | Critic: stable + full `run_ux_gate` | open |

<!-- STEP=1: red proves wrap_offset_at_point_pixel has no scroll_offset_x param / demo omits it -->
<!-- STEP=2: API + demo wire; SKIP if font missing -->
<!-- STEP=3: Critic archive -->

## Out of scope

ICU; SCRIPT_VM; MIR Epic 5; horizontal scrollbar chrome.
