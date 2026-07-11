# Track: текстовый рендеринг целиком на MLC + интерактивное окно (фундамент GUI-фреймворка)

Parent: [../PLAN.md](../../PLAN.md), [../TEXT_RENDERING.md](../../TEXT_RENDERING.md),
[../archive/tracks/TRACK_TEXT_RENDERING.md](TRACK_TEXT_RENDERING.md)
(закрыт — headless-пайплайн, шейпинг/раскладка/кэш на MLC, но GL-orchestration
и вся сцена — внутри `text_renderer_shim.cpp`, C++).
Trigger: пользователь 2026-07-10 — нужно реальное окно с разным текстом,
основной код рендеринга на MLC, не в C++-шиме; это заготовка будущего GUI-
фреймворка (нативные быстрые интерфейсы, анимации, эффекты).

## Status: **closed** (2026-07-11)

**Critic 2026-07-11:** `critique-audit` — commits `1b647480`…`07c0ccdb` match
STEP 1–8; PLAN §10a/queue/archive + TEXT_RENDERING §8 synced; re-ran
`run_text_window_demo.sh`, native/gl smokes, headless `run_text_renderer_a8_smoke.sh`
(all OK). **reopen: none.** Residuals: GlRenderer/TextRenderer live as types
inside example `.mlc` (not stdlib modules); `text_window_helpers.cpp` UV/blit
ABI gaps; demo rebuilds atlas twice; interactive demo optional (by Decision).

**Driver 2026-07-11:** STEP=8 — verify-gate: self-host `diff -rq p1 p2` identical;
`regression_gate` 20/0; `run_text_window_demo.sh` + native/gl smokes ok. Track archived.

**Driver 2026-07-11:** STEP=7 — docs: `TEXT_RENDERING.md` §8 (windowed/MLC path);
backlog [TRACK_GUI_FRAMEWORK.md](TRACK_GUI_FRAMEWORK.md); PLAN §10b.

**Driver 2026-07-11:** STEP=6 — `text_window_demo.mlc`: ≥3 strings (Hi/MLC A8 via
HB+FT, Aa MSDF); clear-color anim via `glfw_gl_anim_unit`; Esc/48-frame loop;
FT glyph byte cache + `text_window_helpers`; gate `run_text_window_demo.sh`.

**Planner 2026-07-11:** activated after STDLIB_VALIDATION Critic OK.

**Driver 2026-07-11:** STEP=1 — Decision locked (GLFW; GL list; MLC
GlRenderer/TextRenderer; thin GL dispatch shim; GLFW optional in CI;
headless golden untouched).

**Driver 2026-07-11:** STEP=2 — `glfw_window.hpp`/`glfw_window.cpp`; smoke
`glfw_window_empty_loop_smoke` (hidden 800×600, clear+swap N frames);
`misc/examples/glfw_window_smoke.mlc`; `scripts/run_glfw_window_smoke.sh`;
optional `glfw3` in `build_bin.sh`.

**Driver 2026-07-11:** STEP=3 — `glfw_gl_dispatch.hpp`/`glfw_gl_dispatch.cpp`;
`glfw_gl_load` via `glfwGetProcAddress`; locked GL set as `mlc::gl::gl_*`;
smoke `glfw_gl_dispatch_clear_smoke` (load all + genBuffers + clear path);
`misc/examples/glfw_gl_dispatch_smoke.mlc`; `scripts/run_glfw_gl_dispatch_smoke.sh`.

**Driver 2026-07-11:** STEP=4 — MLC `GlRenderer` (shader literals, compile/link,
VBO triangle draw) on STEP=3 dispatch; context lifecycle helpers;
`misc/examples/gl_renderer_smoke.mlc`; `scripts/run_gl_renderer_smoke.sh`.
MLC ABI: `int32_t` + `String` by value; `-> unit` for extern void (not `void`).

**Driver 2026-07-11:** STEP=5 — MLC `TextRenderer` (GlyphAtlas + GlyphCache +
textured quad batch); synthetic A8 atlas via scratch_u8; smoke
`text_renderer_native_smoke.mlc`; `scripts/run_text_renderer_native_smoke.sh`.

## Проблема (текущее состояние, факт)

`TRACK_TEXT_RENDERING` (closed) дал: FreeType/HarfBuzz `extern fn` биндинги,
`TextShaper`/`GlyphAtlas`/`GlyphCache` **на чистом MLC** (без FFI) — это уже
верно устроено и остаётся эталоном. Но:

1. **GL-orchestration — целиком в C++.** `runtime/src/gl/text_renderer_shim.cpp`
   владеет шейдерами, VBO/VAO, draw calls, batching квадов на глиф.
2. **Нет окна.** `loader_shim.cpp` — headless EGL pbuffer only.
3. **Нет event loop / анимации.**

## Цель

1. Реальное окно (GLFW), несколько строк текста (A8 + MSDF), кадровый цикл,
   выход по Esc/крестику.
2. **Оркестрация рендеринга — на MLC** (GlRenderer + TextRenderer).
3. Простая анимация одного параметра между кадрами.

## Decision (STEP=1, 2026-07-11) — **locked**

### Windowing

| Вариант | Вердикт |
|---------|---------|
| **GLFW** (`libglfw3-dev`) | **locked** |
| Прямой X11/EGL | **rejected** |
| SDL2 | **rejected** |

**GLFW extern surface (v1):**
`glfwInit`, `glfwWindowHint`, `glfwCreateWindow`, `glfwMakeContextCurrent`,
`glfwGetProcAddress`, `glfwSwapBuffers`, `glfwSwapInterval`, `glfwPollEvents`,
`glfwWindowShouldClose`, `glfwSetWindowShouldClose`, `glfwGetKey`,
`glfwGetTime`, `glfwGetFramebufferSize`, `glfwDestroyWindow`, `glfwTerminate`.

- Window handle: opaque `RawPointer` (or project-conventional opaque handle).
- Headless EGL (`loader_shim.cpp`) **kept** for CI golden tests — separate path.
- Windowed path is **additive**, does not replace headless.

### GL orchestration architecture

```
GLFW C ABI (extern lib/fn) + thin GL dispatch shim (glfwGetProcAddress)
  → GlRenderer (MLC): shaders, VBO/VAO, atlas texture upload
  → TextRenderer (MLC): glyph quad batching (GlyphAtlas/GlyphCache/TextShaper)
  → main loop (MLC): poll → update → draw → swap
```

| Layer | Owner |
|-------|-------|
| GLFW window/input/time | MLC via `extern lib "glfw"` / `extern fn` |
| GL entry points | **Thin C++ dispatch** after `glfwMakeContextCurrent` (same pattern as EGL `loader_shim`: resolve pointers once, export plain C ABI `mlc::gl::*`) — **not** TextRenderer logic in C++ |
| Shader compile / buffers / draw | **MLC** `GlRenderer` |
| Glyph batching / layout | **MLC** `TextRenderer` (reuse existing MLC atlas/cache/shaper) |
| FreeType / HarfBuzz / MSDF gen | Existing C++ shims (unchanged) |

**Why thin GL dispatch (not zero-shim):** calling GL through raw
`glfwGetProcAddress` fptrs from MLC needs C function-pointer call story at
every site; EGL path already solved this with a loader shim. Reuse that
pattern with `glfwGetProcAddress` instead of `eglGetProcAddress`. Scene
orchestration stays MLC — that is the track goal.

### Locked GL function set (windowed text path)

From `text_renderer_shim.cpp` / TEXT_RENDERING §3.3, minus FBO/readback
(not required for interactive demo; add later if screenshot needed):

**State:** `glClear`, `glClearColor`, `glViewport`, `glEnable`, `glBlendFunc`  
**Texture:** `glGenTextures`, `glBindTexture`, `glTexImage2D`, `glTexParameteri`,
`glActiveTexture`, `glDeleteTextures`  
**Buffer/attrib:** `glGenBuffers`, `glBindBuffer`, `glBufferData`,
`glDeleteBuffers`, `glEnableVertexAttribArray`, `glVertexAttribPointer`  
**Shader/program:** `glCreateShader`, `glShaderSource`, `glCompileShader`,
`glGetShaderiv`, `glCreateProgram`, `glAttachShader`, `glLinkProgram`,
`glGetProgramiv`, `glUseProgram`, `glDeleteShader`, `glDeleteProgram`,
`glGetAttribLocation`, `glGetUniformLocation`, `glUniform1i`  
**Draw:** `glDrawArrays`

**Deferred (not v1 windowed):** FBO create/bind/status, `glReadPixels`,
`glTexSubImage2D` (unless atlas update needs it — then add in STEP=4/5).

### CI / build

- `libglfw3-dev` / `-lglfw`: **optional** (same class as freetype2/harfbuzz in
  `build_bin.sh`). Missing GLFW → skip windowed smoke with clear message.
- Interactive demo **not** in mandatory CI gate; verify by manual run.
- Headless golden tests from closed `TRACK_TEXT_RENDERING` **must stay green**.

### Pipeline

- MLC modules + Ruby and/or mlcc as needed per step; GLFW/GL via runtime.
- Self-host + `regression_gate` before track close (STEP=8).

### Non-goals (locked)

- Full GUI framework (layout/widgets/easing)
- Forcing Wayland-only or X11-only
- Windows/macOS
- Replacing/removing headless EGL golden path
- Putting TextRenderer/GlRenderer orchestration back into C++ shims

## Scope (maps to Steps 2–8)

1. ~~Decision~~ → **done** STEP=1
2. ~~GLFW window + empty clear/poll/swap loop~~ → **done** STEP=2
3. ~~GL dispatch via `glfwGetProcAddress` + MLC-callable surface~~ → **done** STEP=3
4. ~~`GlRenderer` on MLC~~ → **done** STEP=4
5. ~~`TextRenderer` on MLC~~ → **done** STEP=5
6. ~~Demo `text_window_demo.mlc`~~ → **done** STEP=6
7. ~~Docs~~ → **done** STEP=7
8. ~~Verify-gate + manual window demo~~ → **done** STEP=8

## Out of scope (этот трек)

- Полноценный GUI-фреймворк
- Wayland-only / Windows/macOS
- Изменение headless EGL golden-тестов

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Decision: GLFW; минимальный GL-набор; архитектура MLC-слоя. | **done** |
| 2 | `extern lib "glfw"` окно + пустой render loop (clear color, poll/swap до закрытия). | **done** (2026-07-11: `glfw_window_*`; smoke ok; optional glfw3) |
| 3 | Узкий GL-набор через `glfwGetProcAddress` (thin dispatch) + MLC calls. | **done** (2026-07-11: `glfw_gl_dispatch_*`; locked `gl_*`; smoke ok) |
| 4 | `GlRenderer` (шейдеры/буферы/текстуры) на MLC поверх STEP=3. | **done** (2026-07-11: MLC GlRenderer + triangle smoke ok) |
| 5 | `TextRenderer` на MLC (batching, переиспользует Atlas/Cache/Shaper). | **done** (2026-07-11: TextRenderer + atlas/cache batch smoke ok; synthetic A8; FT/HB → STEP=6) |
| 6 | Демо: окно, разный текст, анимированный параметр, выход по Esc. | **done** (2026-07-11: text_window_demo A8+MSDF+anim; gate ok) |
| 7 | Docs (`TEXT_RENDERING.md`, `PLAN.md`) + ссылка на будущий GUI-фреймворк трек. | **done** (2026-07-11: TEXT_RENDERING §8; TRACK_GUI_FRAMEWORK backlog; PLAN §10b) |
| 8 | Verify-gate: self-host diff, regression_gate, ручная демонстрация окна. | **done** (2026-07-11: self-host identical; regression 20/0; demo+smokes ok) |

<!-- sub-steps STEP=2: 1) optional -lglfw in build; 2) extern fn GLFW list; 3) smoke window 800x600 clear+Esc -->
<!-- sub-steps STEP=3: 1) glfw_gl_loader after MakeContextCurrent; 2) export locked GL set; 3) MLC smoke clear -->
<!-- sub-steps STEP=4: 1) shader string literals; 2) compile/link on MLC; 3) VBO/VAO -->
<!-- sub-steps STEP=6: 1) ≥3 strings; 2) A8+MSDF; 3) color anim via glfwGetTime -->
