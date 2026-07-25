# Track: Editor hover→paint wiring gap — scrollbar thumbs never visibly appear

Parent: [../PLAN.md](../PLAN.md) §94. Priority override (2026-07-25, user report:
"очень не хватает нормального ховера... скролбары должны появляться в
компонентах по ховеру").

## Status: **open** — STEP=0 (Decision) next

## Evidence (2026-07-25 interactive session, not a Driver turn)

Built `misc/editor/demo_live.mlc` at current HEAD (`compiler/build_bin.sh`),
ran the real binary against a real X11 display (`MLC_GLFW_VISIBLE=1`), drove
mouse via `xdotool` (`--window`-relative move + click), captured screenshots
via `import -window`. Across 4+ distinct hover positions inside the editor's
`text_rect` (confirmed by matching `xdotool getmouselocation` against
`xdotool getwindowgeometry`), **no scrollbar thumb pixel ever appeared** on
the right edge of the content pane, even though:

- a synthetic click at the same coordinates measurably moved the caret
  (status bar `Ln X, Col Y` changed between screenshots), proving mouse
  events do reach the running binary and do trigger a repaint;
- one incidental tree-row hover tint DID render in one of the captures.

Code read (`demo_live.mlc:2286-2303`, `ux/scroll.mlc:editor_ux_content_scrollbar_thumb`,
`layout/word_wrap.mlc:editor_scrollbar_thumb`) shows no obvious gating bug:
`editor_hovered` is computed from `point_in_rect(mouse, editor_rect)` with no
focus check, `wrapped_content_height_now` should exceed `text_rect.height`
for the multi-hundred-line file used (`README.md`), and `theme.muted_*`
should have visible contrast against Solarized Light. The existing gate
scenario `content_scrollbar_thumb_on_hover.mlc` only calls
`editor_ux_content_scrollbar_thumb` directly with a **hardcoded** `hovered=1`
— it never exercises the real per-frame `demo_live.mlc` wiring from actual
mouse coordinates to that call, matching the exact class of gap the
2026-07-17 Opus review warned about (L1 model correctness ≠ wiring
correctness in the god-file).

**Honest limitation:** the live-click probe above has real confounds
(`xdotool`-relative-vs-absolute coordinate mapping proved inconsistent
between attempts in this sandbox's X session/WM, some absolute-coordinate
attempts produced zero response at all where relative-coordinate attempts
did respond) — this must **not** be treated as a confirmed root cause. Do
not guess further; do not attempt a code fix before a deterministic
headless repro exists.

## STEP=1 (must be first, before any fix)

Build a deterministic **offscreen** repro: hidden GLFW window
(`MLC_GLFW_VISIBLE=0`, already the smoke-test default), drive input via the
existing `glfw_gl_input_test_*` test hooks (same mechanism used by
`run_editor_*_smoke.sh`) to set a synthetic mouse position inside
`text_rect` with no click, run one frame, `glReadPixels` (existing
offscreen-FBO pattern from `msdf_renderer_shim.hpp`/`text_renderer_shim.hpp`)
at the expected thumb rect coordinates, assert the sampled color differs
from the plain background/panel fill. Repeat for the folder-tree thumb and
the horizontal thumb. If this repro already passes headlessly, the bug is
specific to this interactive session's X11/WM setup, not the code — close
this track as "not reproducible headlessly" with the evidence attached, and
do not touch `demo_live.mlc`. If it fails, that is STEP=1's red state; fix
in STEP=2, Critic in STEP=3 (same gate scenario, run twice per
`GUI_UX_TESTING.md` standing discipline point 5).

## Non-goals

Not a rewrite of `demo_live.mlc`'s hover chrome; scope is strictly "does the
already-written scrollbar-thumb draw call actually reach the framebuffer on
real hover input", nothing else.
