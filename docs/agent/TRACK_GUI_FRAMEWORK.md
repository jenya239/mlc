# Track: GUI framework (native UI on MLC)

Parent: [../PLAN.md](../PLAN.md), [../TEXT_RENDERING.md](../TEXT_RENDERING.md) §8,
[TRACK_TEXT_RENDERING_NATIVE.md](../archive/tracks/TRACK_TEXT_RENDERING_NATIVE.md).

## Status: **open** — STEP=6 next (docs + verify-gate) — **active**

**Driver 2026-07-11:** STEP=5 — `misc/gui/button.mlc` + `ease_in_out_unit`;
`gui_button_demo.mlc` (layout button, inject click, clear-color easing);
`run_gui_button_demo.sh` ok.

**Driver 2026-07-11:** STEP=4 — `misc/gui/input.mlc` (GuiInput poll/hover/click_edge);
GLFW `glfw_gl_mouse_*` + test inject; `run_gui_input_smoke.sh` ok.

**Driver 2026-07-11:** STEP=3 — `misc/gui/layout.mlc` (Rect/Point, row/column IM,
`point_in_rect`); smoke `gui_layout_smoke` (no GLFW) EXIT 0.

**Driver 2026-07-11:** STEP=2 — `misc/gui/gl_renderer.mlc` + `text_renderer.mlc`;
smokes rewired to `import` from `../gui/*`; both smokes EXIT 0.
`text_window_demo.mlc` still local (FT/MSDF) — residual, not required for v0 close.

**Driver 2026-07-11:** STEP=1 — Decision **locked** (IM A; `misc/gui/`; Button v0).

**Planner 2026-07-11:** promoted after NATIVE Critic OK (`05186e8c`). Queue next
after closed `TRACK_TEXT_RENDERING_NATIVE`. v0 = extract reusable MLC render
modules (Critic residual) + minimal layout/input/widget on GLFW path — not a
full toolkit.

## Problem

`TRACK_TEXT_RENDERING_NATIVE` delivers a GLFW window + MLC text/GL orchestration
(foundation). Critic residuals: `GlRenderer`/`TextRenderer`/`GlyphAtlas` live
only inside example `.mlc` files; `text_window_helpers.cpp` covers UV/blit ABI
gaps. A full GUI stack (layout, widgets, input routing, easing, hit-testing)
was out of scope there.

## Goal (v0)

1. Shared MLC modules for GlRenderer + TextRenderer (dedupe examples).
2. Minimal immediate-mode layout (locked).
3. Input routing from GLFW (mouse + key) to one hit-tested widget.
4. One animated parameter (easing) on that widget.
5. Smoke/demo gate; headless text golden untouched.

## Decision (STEP=1, 2026-07-11) — **locked**

### Layout model

| Вариант | Вердикт |
|---------|---------|
| **A. Immediate-mode** (Dear ImGui-like) | **locked** |
| B. Retained tree | **rejected** (v0) |

One frame = one layout/draw pass inside existing
`poll → update → layout/draw → swap` loop. No widget tree persistence beyond
frame-local IDs / hover-click edge state held by the app.

### Event loop / windowing

- Reuse NATIVE: `glfw_window_*` / `glfw_gl_*` (no new windowing lib).
- Headless EGL golden path **untouched**.

### Module path

| Вариант | Вердикт |
|---------|---------|
| **`misc/gui/*.mlc`** (examples-adjacent shared modules) | **locked** |
| `lib/mlc/` / stdlib / `compiler/` | **rejected** (v0 — avoid compiler/stdlib churn) |

**v0 files (STEP=2 target names):**
- `misc/gui/gl_renderer.mlc` — shaders/VBO/texture helpers (`GlRenderer`)
- `misc/gui/text_renderer.mlc` — atlas/cache/quad batch (`TextRenderer` + deps)
- Optional later: `misc/gui/layout.mlc`, `misc/gui/button.mlc`

Smokes/demos under `misc/examples/` **import** these modules; no duplicated
type bodies. C++ `text_window_helpers` stays thin ABI only.

### Widget v0

| Вариант | Вердикт |
|---------|---------|
| **`Button`** (label text + rect; hover/click) | **locked** |
| Label-only | **rejected** as sole v0 widget (no click surface) |

**API sketch (non-binding names; Driver STEP=3–5 refine):**
```
type Rect = { x: i32, y: i32, width: i32, height: i32 }
type GuiInput = { mouse_x: i32, mouse_y: i32, mouse_down: i32, escape: i32 }
fn gui_button(label: string, rect: Rect, input: GuiInput) -> i32
  # returns 1 on click edge; draws via TextRenderer + solid/quad clear
```
Easing: `glfw_gl_anim_unit` and/or small MLC `ease_in_out` on button color.

### Non-goals (v0) — locked

- Full widget set, CSS-like styling, focus rings, Wayland-only, Win/macOS
- Replacing headless EGL golden path
- Scene orchestration in `text_renderer_shim.cpp`
- Retained UI tree / dirty flags

## Steps

| Step | Item | Status |
|------|------|--------|
| 0 | Activate after NATIVE Critic OK. | **done** (2026-07-11 Planner) |
| 1 | Decision: layout model (A/B); module path; widget v0 surface. | **done** (2026-07-11: IM A; `misc/gui/`; Button) |
| 2 | Extract shared MLC `GlRenderer` + `TextRenderer` modules; rewire smokes. | **done** (2026-07-11: `misc/gui/*`; smokes import; demo deferred STEP=5) |
| 3 | Layout pass v0 (IM row/column) + screen hit-test helper. | **done** (2026-07-11: `layout.mlc`; `run_gui_layout_smoke.sh` ok) |
| 4 | Input: mouse position/button + key via GLFW → widget hover/click. | **done** (2026-07-11: `input.mlc` + GLFW poll/inject; smoke ok) |
| 5 | Demo: one Button + easing anim; gate script (hidden window OK). | **done** (2026-07-11: `button.mlc` + `run_gui_button_demo.sh` ok) |
| 6 | Docs (`TEXT_RENDERING` or short `GUI.md`) + PLAN; verify-gate. | pending |

<!-- sub-steps STEP=2: 1) misc/gui/gl_renderer.mlc + text_renderer.mlc; 2) rewire gl_renderer_smoke + text_renderer_native_smoke; 3) text_window_demo import or thin wrapper; 4) smokes EXIT 0 -->
<!-- sub-steps STEP=3: 1) Rect/Point in misc/gui; 2) IM row/column; 3) point-in-rect -->
<!-- sub-steps STEP=4: 1) glfw cursor/button externs if missing; 2) hover state; 3) click edge -->
<!-- sub-steps STEP=5: 1) gui_button_demo.mlc; 2) run_gui_button_demo.sh; 3) Esc/N-frame -->
<!-- sub-steps STEP=6: 1) docs; 2) smokes (+ self-host if compiler touched); 3) close -->
