# Track: текстовый рендеринг целиком на MLC + интерактивное окно (фундамент GUI-фреймворка)

Parent: [../PLAN.md](../PLAN.md), [../TEXT_RENDERING.md](../TEXT_RENDERING.md),
[../archive/tracks/TRACK_TEXT_RENDERING.md](../archive/tracks/TRACK_TEXT_RENDERING.md)
(закрыт — headless-пайплайн, шейпинг/раскладка/кэш на MLC, но GL-orchestration
и вся сцена — внутри `text_renderer_shim.cpp`, C++).
Trigger: пользователь 2026-07-10 — нужно реальное окно с разным текстом,
основной код рендеринга на MLC, не в C++-шиме; это заготовка будущего GUI-
фреймворка (нативные быстрые интерфейсы, анимации, эффекты).

## Status: **open** — STEP=1 next (Decision) — **active**

**Planner 2026-07-11:** activated after STDLIB_VALIDATION Critic OK. STDLIB
§5 chain complete; PLAN queue next is this track (личный проект / GUI
foundation). STEP=1 still pending — Driver locks GLFW/GL/MLC Decision
(draft already in «Decision» section below).

## Проблема (текущее состояние, факт)

`TRACK_TEXT_RENDERING` (closed) дал: FreeType/HarfBuzz `extern fn` биндинги,
`TextShaper`/`GlyphAtlas`/`GlyphCache` **на чистом MLC** (без FFI) — это уже
верно устроено и остаётся эталоном. Но:

1. **GL-orchestration — целиком в C++.** `runtime/src/gl/text_renderer_shim.cpp`
   владеет шейдерами, VBO/VAO, draw calls, batching квадов на глиф. STEP=6
   явно зафиксировал: «optional narrow GL extern surface — **skipped**
   (shim owns GL; §5.2)». MLC-код (`text_renderer_a8_string_smoke.mlc`) —
   один `extern fn` вызов на весь тест, никакой сцены на MLC нет.
2. **Нет окна.** `loader_shim.cpp` создаёт **headless EGL pbuffer**
   (`EGL_PLATFORM_SURFACELESS_MESA`, 1×1) — offscreen FBO + golden readback.
   Ни одна текущая демка не открывает видимое окно; визуализация делалась
   через debug-дамп (`MLC_DUMP_ACTUAL_RGBA` → raw RGBA → PNG вручную).
3. **Нет event loop / анимации.** Всё — одноразовый рендер одного кадра в
   FBO, сравнение и выход. Нет цикла кадров, нет времени, нет ввода.

## Цель

1. Реальное окно (X11/Wayland через GLFW — см. Decision), открывается,
   рисует **несколько разных строк текста** (разные размеры, минимум один
   пример A8-режима и один MSDF-режима, разные позиции), держит кадровый
   цикл (`glfwPollEvents`/`glfwSwapBuffers`), закрывается по Esc/крестику.
2. **Оркестрация рендеринга — на MLC**, не в C++: MLC-код вызывает узкий
   набор `extern fn` (GL-функции + оконные функции), сам собирает
   шейдеры/буферы/draw calls, сам ведёт кадровый цикл. C++ остаётся только
   там, где это либо C ABI сторонней библиотеки (GLFW, GL, FreeType,
   HarfBuzz), либо действительно узкий шим (MSDF-генерация, как сейчас).
3. Простая анимация одного параметра (например цвет текста меняется по
   времени, или один блок текста скроллится) — не полноценный фреймворк
   анимаций, просто доказательство, что кадровый цикл управляется на MLC и
   поддерживает изменение состояния между кадрами.

## Decision (STEP=1, нужно зафиксировать)

### Windowing

| Вариант | Объём | Вердикт |
|---------|-------|---------|
| **GLFW** (`libglfw3-dev`) | ~15-20 функций, чистый C ABI, сам решает X11/Wayland | **рекомендован** — по объёму сопоставим с libpq/HarfBuzz прецедентами |
| Прямой X11/EGL (без GLFW) | значительно больше кода (Xlib события, EGL window surface вручную) | reject — непропорциональный объём для этой задачи |
| SDL2 | шире GLFW (аудио/джойстики и т.п., не нужно) | reject — GLFW уже достаточен |

Минимальный набор: `glfwInit`, `glfwWindowHint`, `glfwCreateWindow`,
`glfwMakeContextCurrent`, `glfwGetProcAddress` (замена `eglGetProcAddress`
для windowed-контекста), `glfwSwapBuffers`, `glfwSwapInterval`,
`glfwPollEvents`, `glfwWindowShouldClose`, `glfwSetWindowShouldClose`,
`glfwGetKey`, `glfwGetTime`, `glfwGetFramebufferSize`, `glfwDestroyWindow`,
`glfwTerminate`. Headless EGL путь (`loader_shim.cpp`) **не удалять** —
остаётся для CI/regression golden-тестов; windowed путь — новый, отдельный.

### Где живёт GL-orchestration

**MLC**, не C++. Структура (эталон — целевая архитектура из
`TEXT_RENDERING.md` §2, которая для GL-слоя не была реализована):

```
mlc::gl::extern fn (GLFW + узкий набор GL, ~30-40 функций суммарно) — FFI граница
  → GlRenderer (MLC): компиляция шейдеров, VBO/VAO setup, upload атласа как текстуры
  → TextRenderer (MLC): batching квадов на глиф (переиспользует GlyphAtlas/GlyphCache/TextShaper — уже MLC)
  → main render loop (MLC): while !glfwWindowShouldClose(window) do poll → update → draw → swap end
```

C++ остаётся только под: `freetype_shim`, `harfbuzz_shim`, `msdf_shim`
(уже так) — сырые C++ библиотеки без чистого C ABI или с сложным
управлением памятью; GLFW и GL — прямой `extern fn`/`extern lib`, без шима
(оба — чистый C ABI, EGL-шим прецедент показал что это не обязательно даже
для GL — было решение §5.2 обернуть в шим ради `eglGetProcAddress`, но
`glfwGetProcAddress` тоже возвращает указатели на функции, тот же паттерн
"runtime-resolved extern fn", уже отработан — переиспользовать, не
переизобретать).

## Scope

1. Decision: GLFW (см. таблицу); зафиксировать минимальный набор GL-функций
   для сцены (переиспользовать список из `TEXT_RENDERING.md` §3.3, ~20-30
   функций — тот же набор, просто теперь вызываются из MLC, не из шима).
2. `extern lib "glfw"` + `extern fn` биндинги окна (список выше);
   MLC-модуль открывает окно, делает context current, крутит пустой цикл
   poll/swap до закрытия — smoke без текста (просто окно с clear color).
3. Экспорт узкого GL-набора как `extern fn`, резолвинг через
   `glfwGetProcAddress` (тот же паттерн, что EGL в §5.2) — вызывается из
   MLC на старте после создания окна.
4. `GlRenderer` на MLC: компиляция/линковка шейдеров (A8 + MSDF median,
   логика шейдеров уже есть в `text_renderer_shim.cpp`/`msdf_renderer_shim.cpp`
   — перенести исходники шейдеров как строки, саму компиляцию/линковку и
   работу с буферами — на MLC поверх extern fn из STEP=3).
5. `TextRenderer` на MLC: batching квадов на глиф поверх `GlyphAtlas`/
   `GlyphCache`/`TextShaper` (уже MLC) — draw calls через `GlRenderer`.
6. Демо `misc/examples/text_window_demo.mlc`: окно, несколько строк текста
   разных размеров (минимум одна A8, одна MSDF), один анимированный
   параметр (цвет или позиция) на кадр, выход по Esc/крестику.
7. Docs: `TEXT_RENDERING.md` (обновить §2/§3.3 — GL-orchestration теперь
   MLC, не C++), `PLAN.md`; явная ссылка на будущий GUI-фреймворк трек
   (layout/widgets/событийная модель — **не** этот трек, см. Out of scope).
8. Verify-gate: self-host (`mlcc`→`mlcc2` diff), `regression_gate.sh` (не
   ломает headless golden-тесты из закрытого TEXT_RENDERING);
   **интерактивная демка не автоматизируется в CI** — верификация ручным
   запуском с логом/скриншотом в этом чате (как раньше делали дамп кадра).

## Out of scope (этот трек)

- Полноценный GUI-фреймворк (layout engine, widget tree, retained/immediate
  mode, событийная модель мыши/клавиатуры сверх raw `glfwGetKey`,
  анимационный тайминг-движок с easing/interpolation) — это следующий,
  отдельный трек после этого; здесь только доказательство, что примитивы
  (окно + текст + кадровый цикл) работают целиком на MLC.
- Wayland-специфичный путь (GLFW сам абстрагирует X11/Wayland — не
  форсировать один backend явно).
- Мультиплатформенность (Windows/macOS) — вне текущего проекта (см. правило
  «кроме macOS/Windows»).
- Изменение headless EGL golden-тестов из закрытого `TRACK_TEXT_RENDERING`
  — остаются как есть для CI.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Decision: GLFW; минимальный GL-набор; архитектура MLC-слоя (см. выше). | pending |
| 2 | `extern lib "glfw"` окно + пустой render loop (clear color, poll/swap до закрытия). | pending |
| 3 | Узкий GL-набор через `glfwGetProcAddress` как `extern fn`, вызываемые из MLC. | pending |
| 4 | `GlRenderer` (шейдеры/буферы/текстуры) на MLC поверх STEP=3. | pending |
| 5 | `TextRenderer` на MLC (batching, переиспользует Atlas/Cache/Shaper). | pending |
| 6 | Демо: окно, разный текст, анимированный параметр, выход по Esc. | pending |
| 7 | Docs (`TEXT_RENDERING.md`, `PLAN.md`) + ссылка на будущий GUI-фреймворк трек. | pending |
| 8 | Verify-gate: self-host diff, regression_gate, ручная демонстрация окна. | pending |

<!-- sub-steps STEP=2: 1) libglfw3-dev в build_bin.sh опционально (как freetype2/harfbuzz); 2) extern fn список; 3) smoke: окно 800x600, clear color, закрытие по Esc -->
<!-- sub-steps STEP=4: 1) шейдер-строки перенести как MLC string literals; 2) glCreateShader/glCompileShader/glLinkProgram обёртки на MLC; 3) VBO/VAO setup на MLC -->
<!-- sub-steps STEP=6: 1) минимум 3 строки текста разных pixel_size; 2) один A8 + один MSDF (threshold 28 уже есть); 3) анимация цвета по glfwGetTime() -->

## Open questions

- Библиотека `libglfw3-dev` доступна в CI-образе? Если нет — STEP=2
  делает её опциональной сборкой (как `freetype2`/`harfbuzz` уже
  опциональны в `build_bin.sh`), демо не входит в обязательный CI gate,
  только в regression на машине с GLFW.
- Будущий GUI-фреймворк трек — не открывать сейчас, только зафиксировать
  ссылку здесь после STEP=8.
