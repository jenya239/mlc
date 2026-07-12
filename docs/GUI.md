# GUI framework v0 (immediate-mode on MLC)

Parent: [PLAN.md](PLAN.md) §10b, [TEXT_RENDERING.md](TEXT_RENDERING.md) §8,
[archive/tracks/TRACK_GUI_FRAMEWORK.md](archive/tracks/TRACK_GUI_FRAMEWORK.md).
Input hardening: [agent/TRACK_GUI_INPUT_ROBUSTNESS.md](agent/TRACK_GUI_INPUT_ROBUSTNESS.md).

Built on closed [TRACK_TEXT_RENDERING_NATIVE](archive/tracks/TRACK_TEXT_RENDERING_NATIVE.md)
+ [TRACK_GL_GLAD_MIGRATION](archive/tracks/TRACK_GL_GLAD_MIGRATION.md)
(GLFW window + **GLAD2** GL entry points).

## Model

- **Immediate-mode** layout each frame (`poll → update → layout/draw → swap`).
- Shared MLC modules under `misc/gui/` (not stdlib/compiler).
- Thin C++ only for GLFW window/input (`glfw_window_gl`); GL symbols via
  vendored GLAD2 (`glad_gl_abi` / `glad_gl.mlc`), not a hand GetProcAddress table.

## Modules

| Module | Role |
|--------|------|
| `misc/gui/gl_renderer.mlc` | Shader/VBO triangle helper (glad) |
| `misc/gui/text_renderer.mlc` | Atlas/cache + textured quads (glad); `text_window_width`/`height`/`set_size` |
| `misc/gui/layout.mlc` | `Rect`/`Point`, row/column cursor, `point_in_rect` |
| `misc/gui/input.mlc` | `GuiInput` poll (mouse + `text_buffer`/`backspace`/`enter`), hover, click edge |
| `misc/gui/button.mlc` | `gui_button`, `ease_in_out_unit` |
| `lib/mlc/common/stdlib/gl/gl_window.mlc` | GLFW context, input inject, keyboard text, live window size |

## Input robustness (2026-07-12)

### Click edge / “debounce”

`gui_click_edge` is a one-frame down-edge while hovered (previous frame up).
Controlled repro (`scripts/run_gui_click_edge_repro.sh`, pure +
`gui_input_test_set`) fires **exactly once per down-edge** across
down→hold→up→down. The earlier `text_dashboard_demo` observation of **9**
clicks from one `xdotool` mousedown/mouseup pair is treated as an
**xdotool/X11 synthetic-event artifact**, not a `gui_click_edge` bug — no
extra debounce latch was added.

### Keyboard text

- Runtime: `glfwSetCharCallback` accumulates UTF-8 (ASCII/BMP Latin);
  `glfw_gl_take_text()` returns and clears the frame buffer.
- Keys: `glfw_gl_key_backspace_down` / `glfw_gl_key_enter_down` (level).
- Inject: `glfw_gl_text_test_push`, `glfw_gl_keys_test_set`.
- `GuiInput`: `text_buffer`, `backspace`, `enter` filled by `gui_input_poll`.
- Demo: `scripts/run_gui_text_field_demo.sh` (box + TextRenderer glyphs).

### Window size

- `glfw_gl_window_width` / `glfw_gl_window_height` — live client size
  (cache + `glfwSetWindowSizeCallback`; `set_size` forces cache after poll
  because invisible GLFW may ignore resize).
- `glfw_gl_window_set_size` for tests/demos.
- Callers should read size each frame for `gl_viewport` / draw
  (`gui_button_demo`, `gui_text_field_demo`), not freeze `const width/height`
  after `begin`.

## Gates

```bash
scripts/run_gui_layout_smoke.sh         # no GLFW
scripts/run_gui_input_smoke.sh          # needs glfw3
scripts/run_gui_click_edge_repro.sh     # controlled click-edge
scripts/run_gui_text_input_smoke.sh     # text_buffer + keys inject
scripts/run_gui_text_field_demo.sh      # needs glfw3 + freetype/harfbuzz/font
scripts/run_gui_window_size_smoke.sh    # width/height + set_size
scripts/run_gui_button_demo.sh          # needs glfw3; inject click + easing
scripts/run_gl_renderer_smoke.sh
scripts/run_text_renderer_native_smoke.sh
scripts/run_glad_link_smoke.sh
compiler/tests/run_gl_loader_smoke.sh
```

Interactive: `MLC_GLFW_VISIBLE=1 scripts/run_gui_button_demo.sh`.

## Residuals (not v0)

- Button label text draw (color-only demo)
- `text_window_demo.mlc` still self-contained (FT/MSDF), not on `misc/gui/`
- Full widget set / retained tree / CSS
- IME / CJK composed input
- Retained affine-transform scene graph + vector primitives — единый
  фундамент для classic/game/Flash-rich/Figma-canvas (один tree+matrix,
  разный контент). v0 (screen-space `Rect`, чистый IM) — прототип одной
  кнопки, не основа. Design-only заметка:
  [TRACK_GUI_CANVAS_GRAPH](agent/TRACK_GUI_CANVAS_GRAPH.md)
