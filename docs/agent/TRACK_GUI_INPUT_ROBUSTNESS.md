# Track: GUI input robustness (click debounce, keyboard text, resize)

Parent: [../GUI.md](../GUI.md), [../archive/tracks/TRACK_GUI_FRAMEWORK.md](../archive/tracks/TRACK_GUI_FRAMEWORK.md).
Trigger: 2026-07-11 (несвязанная задача — демо для пользователя).
`misc/examples/text_dashboard_demo.mlc` manual test: one `xdotool
mousedown`/`mouseup` pair (held ~200ms) registered as **9** separate clicks
in the on-screen counter, not 1. Not confirmed as a real bug yet — could be
`xdotool` sending synthetic button-repeat events GLFW sees as multiple
press/release pairs, or a genuine debounce gap in `gui_click_edge`
(`misc/gui/input.mlc`). STEP=1 below must reproduce with a controlled input
source before fixing anything.

## Status: **active** (Planner 2026-07-12) — очередь §23

## Next step

**STEP=6** — docs: `GUI.md` debounce finding + keyboard/resize primitives.

## Goal

Hardened real-input path for the GUI framework (`misc/gui/`) beyond the one
button smoke that shipped with `TRACK_GUI_FRAMEWORK`: reliable click
counting under rapid real input, basic keyboard text entry (needed for any
future text-input widget), and window resize handled without corrupting
layout/hit-testing (`layout.mlc` rects are computed once at frame start
against a hardcoded `width`/`height`, not the live framebuffer size).

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Repro: minimal MLC program, no `xdotool`, uses `gui_input_test_set` to simulate a controlled down→up→down sequence at known frame boundaries; assert `gui_click_edge` fires exactly once per down-edge. If this passes cleanly, the 9-vs-1 observation was an `xdotool`/X11 synthetic-event artifact, not a real bug — close this step as "not reproducible under controlled input", downgrade rest of track priority accordingly, still keep Steps 3-5 (keyboard/resize are real gaps regardless) | **done** (2026-07-12) — not reproducible under controlled input; `misc/examples/gui_click_edge_repro.mlc` + `scripts/run_gui_click_edge_repro.sh` (pure+glfw: exactly 2 edges on down→hold→up→down) |
| 2 | If Step 1 reproduces a real double-fire: fix `gui_click_edge` in `misc/gui/input.mlc` (likely needs a minimum-hold-frames debounce or edge-latch that survives a single `glfwPollEvents` call returning multiple queued button events) | **N/A** (STEP=1 passed) |
| 3 | Keyboard text input: extend `glfw_gl_*` bindings (`runtime/src/gl/glfw_window_gl.cpp`) with a UTF-8 text-input callback (`glfwSetCharCallback`) + backspace/enter key polling; expose via `gl_window.mlc`/`gui/input.mlc` as `GuiInput.text_buffer: string` (accumulated since last frame) | **done** (2026-07-12) — `take_text`/`text_test_push`/`keys_test_set`; `GuiInput.text_buffer`/`backspace`/`enter`; `misc/examples/gui_text_input_smoke.mlc` + `scripts/run_gui_text_input_smoke.sh` |
| 4 | One smoke demo: single-line text field (box + live typed text via A8 `TextRenderer`, reusing `text_dashboard_demo.mlc` patterns) — proves the keyboard path end-to-end | **done** (2026-07-12) — `misc/examples/gui_text_field_demo.mlc` + `scripts/run_gui_text_field_demo.sh` (inject H+ix+BS→Hi, enter; TextRenderer box+glyphs) |
| 5 | Window resize: `glfw_gl_context_should_close`/`swap_poll` family gains a `glfw_gl_window_width()`/`glfw_gl_window_height()` pair (or a single `glfw_gl_framebuffer_size()` returning both via out-params/scratch) so demos stop hardcoding `800x600`; update `misc/gui/layout.mlc` callers to read live size instead of a `const width/height` at top of `main()` | pending |
| 6 | Docs: `GUI.md` — note debounce finding (or its absence) + new keyboard/resize primitives | pending |
| 7 | Verify: existing `run_gui_button_demo.sh` + new smokes green; self-host diff identical (touches `runtime/` C++ + `misc/gui/*.mlc`, not `compiler/`, so self-host diff only needed if `compiler/checker` extern signatures change) | pending |

### STEP=1 sub-steps (Driver)

1. Read `misc/gui/input.mlc` (`gui_click_edge`, `gui_input_test_set`) + existing `scripts/run_gui_input_smoke.sh`.
2. Add fixture/smoke under `misc/examples/` or `compiler/tests/` that drives `gui_input_test_set` across frames and counts edges (expect 1 per down-edge).
3. Record pass/fail in TRACK; if pass → mark STEP=1 done as "not reproducible under controlled input", skip STEP=2 (or mark N/A), proceed STEP=3.
4. No debounce fix in STEP=1.

### Progress

- **STEP=1** (2026-07-12): controlled repro green. 9-vs-1 under xdotool treated as synthetic-event artifact, not `gui_click_edge` bug. STEP=2 skipped.
- **STEP=3** (2026-07-12): `glfwSetCharCallback` + `glfw_gl_take_text` / keys; `GuiInput.text_buffer`/`backspace`/`enter`; `scripts/run_gui_text_input_smoke.sh` ok.
- **STEP=4** (2026-07-12): text field demo + TextRenderer; inject → `Hi` + enter; `run_gui_text_field_demo.sh` ok.

## Out of scope

- IME / composed input (CJK) — plain ASCII/Latin-1 typed text only for now.
- Multi-touch / gamepad — not requested, no driving use case yet.
- The retained scene-graph rewrite ([TRACK_GUI_CANVAS_GRAPH](TRACK_GUI_CANVAS_GRAPH.md),
  still design-only, not activated) — this track works within the existing
  v0 immediate-mode `misc/gui/` architecture, does not migrate it.
