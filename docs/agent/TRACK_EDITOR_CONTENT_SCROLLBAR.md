# Track: Editor Content Scrollbar

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#33e**
(pulled forward — live-demo audit 2026-07-19). Size **S**.

## Status: **active** (2026-07-19) — STEP=0 done; next STEP=1

## Next step

**STEP=1** — L2 red harness + `run_ux_content_scrollbar_thumb.sh`.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `a040b223` | Decision freeze + open |

## Decision (STEP=0) — **frozen** 2026-07-19

| Item | Choice |
|------|--------|
| Problem | Tree panel draws `folder_scrollbar_thumb` when `tree_hovered` (`demo_live` ~1712). Editor content pane never draws a thumb despite existing unused `editor_scrollbar_thumb` in `layout/word_wrap.mlc`. Wheel scroll already hover-gated on `editor_rect` (~891) — not a bug |
| Geometry | Reuse `editor_scrollbar_thumb(track_x, track_y, track_height, thumb_width, content_height, scroll_y)`. Track = right edge of `text_rect` (width 5, inset 2 like folder). Inputs: `wrapped_content_height`, `text_rect.height`, `scroll_offset_y`. Empty rect when content ≤ viewport |
| Visibility | Draw thumb only when mouse is over `editor_rect` (same hover contract as tree). No always-on chrome; no drag-to-scroll this track |
| Color | Match tree thumb tint for now (`0.45/0.48/0.55`) — not theme tokens this track |
| Dead hover wheel | **Delete** unused `editor_ux_wheel_scroll_hover` + `editor_app_wheel_scroll_hover` (and imports). Live path uses `editor_app_wheel_scroll` + `point_in_rect(editor_rect)`; wiring the hover variant would change caret-snap semantics — out of scope. Do not leave both |
| Token | `ux_ok content_scrollbar_thumb_on_hover` — L2: (1) content > viewport → thumb width>0, height≥12, y in track; (2) content ≤ viewport → empty; (3) optional: helper that returns draw-or-skip given hovered flag — hovered=0 → empty even if content overflows |
| Standing discipline | Scenario-first; name token in SESSION `done` |
| Module touch | `demo_live.mlc` draw; maybe thin wrap in `ux/scroll.mlc` or call `editor_scrollbar_thumb` directly; scenario + scripts; delete dead hover fns in `ux/scroll.mlc` + `app/state.mlc`. No `compiler/`. No `lib/mlc/` |
| REG | no |
| Out of scope | Thumb drag; horizontal scrollbar; minimap (#35); theme token for thumb color; changing wheel/caret-snap behavior |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | L2 red harness + run scripts | pending — `ux_ok content_scrollbar_red` |
| 2 | Wire draw + delete dead hover; green token | pending — `ux_ok content_scrollbar_thumb_on_hover` |
| 3 | `demo_live_fs_compile_ok` | pending |
| 4 | Critic: gates; archive | pending |

<!-- STEP=1: stub fail token; red script -->
<!-- STEP=2: demo_live hover draw + delete wheel_scroll_hover; green L2 -->
<!-- STEP=3: compile gate -->

## Out of scope

Thumb drag; `#34`/`#35`; `compiler/`; `lib/mlc/`.
