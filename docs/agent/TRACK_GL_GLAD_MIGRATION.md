# Track: GL-вызовы через GLAD2 — вместо ручного C++ dispatch

Parent: [../FFI_LAYER.md](../FFI_LAYER.md) §8, [../PLAN.md](../PLAN.md).
Trigger: пользователь 2026-07-11 — без ручного C++ для GL loader. Обсуждение
в чате: современный GL (всё новее версии 1.1) не даёт линкуемых символов из
`libGL.so` — адрес функции только через `glXGetProcAddress`/
`glfwGetProcAddress` во время исполнения (факт ABI, не ограничение MLC).
Решение — не писать loader самим (ни в C++, ни на MLC), а положиться на
готовую генерируемую библиотеку. Выбор — **GLAD2**, не `libepoxy`
(эквивалентна по механике, но GLAD2 самодостаточна — не требует системной
`.so` на целевой машине, важно для будущего embed-режима, см.
`archive/tracks/TRACK_FFI_POINTER_CAST.md`/`TRACK_GL_LOADER_TO_MLC.md`
(superseded) — там же почему свой каст/таблица не нужны).

## Status: **active** — STEP=2 next (extern fn bindings)

**Driver 2026-07-11 STEP=1:** Decision locked; vendored
`runtime/third_party/glad/{gl,egl}/` via `glad2==2.0.8` (glad.sh POST
broken that day — see README). GL Core 3.3 covers all dispatch GL symbols;
EGL 1.5 + GLES2 + mesa surfaceless exts for headless.

**Planner 2026-07-11:** activated after STDLIB_LOGIC Critic OK (`e1ffa13e`).
Last «без C++» §20 queue item (§20e). Callers still
`misc/gui/{gl_renderer,input}.mlc` → `glfw_gl_dispatch.hpp` until STEP=4.

## Факт (проверено)

`glad_glDrawArrays` (GLAD2) и `epoxy_glDrawArrays` (libepoxy) — одна и та же
механика: глобальная переменная-указатель-на-функцию + `#define glDrawArrays
glad_glDrawArrays`, ленивый резолв внутри сгенерированного/системного кода.
Уже закрытый `extern fn ... = "glDrawArrays" from "<glad/gl.h>"` (эмитит
`#include` + текстовый вызов `glDrawArrays(...)`) работает прозрачно через
макрос — **новых примитив языка не нужно**.

GLAD не предназначен для генерации на каждой сборке (официальная позиция
автора: «Glad is not supposed to be built/ran everytime you build your
application» — генерируется один раз через `https://glad.sh` без локального
Python, результат вендорится в репозиторий как обычный C-файл).

Текущий объём для замены: `runtime/src/gl/glfw_gl_dispatch.cpp` (812 строк,
~30 GL + context/input wrappers) + `runtime/include/mlc/gl/glfw_gl_dispatch.hpp`
(130 строк) + `runtime/src/gl/loader_shim.cpp` (117 строк, EGL/GLES2
headless) + `loader_shim.hpp`.

## Цель

`extern fn`-биндинги на MLC-уровне к тем же desktop GL + GLES2-функциям
(headless), реализация — сгенерированный `glad.c`/`glad.h` (desktop, Core
профиль, версия по потребности — проверить минимально нужную на STEP=1) +
отдельный сгенерированный EGL/GLES2 loader для headless-пути. Собственный
C++ dispatch удаляется.

## Decision (STEP=1) — **locked 2026-07-11**

| Вопрос | Locked |
|--------|--------|
| Генерация | `glad2==2.0.8` CLI once (`--reproducible`); same engine as glad.sh. Web `POST /generate` failed 302/405 — documented in `runtime/third_party/glad/README.md` |
| API/версия | Desktop **GL Core 3.3** (all used `gl*` from dispatch present). Headless **EGL 1.5 + GLES2 2.0** + `EGL_EXT_platform_base`, `EGL_MESA_platform_surfaceless`, `EGL_KHR_surfaceless_context` |
| Layout | `runtime/third_party/glad/gl/` + `…/egl/` (two trees) |
| MLC-уровень | later STEPs: `extern fn` → glad headers; GLFW context/input stay thin wrappers until replaced/kept |
| Линковка | `gl/src/gl.c` + `egl/src/{egl,gles2}.c` in object list (`build_bin.sh` / GUI) — STEP=3 |

## Scope

1. Decision (таблица) + сгенерировать desktop (GL Core, `glad.sh`) и
   headless (EGL+GLES2, `glad.sh`) наборы, вендорить в `runtime/third_party/glad/`.
2. `extern fn` для всех используемых desktop GL + GLES2/EGL функций
   → `"<glad name>" from "<glad/…>"` (или тонкий MLC wrapper поверх).
3. Линковка сгенерированных `.c` в `build_bin.sh`.
4. Переключить `misc/gui/*.mlc` demos с `glfw_gl_dispatch.hpp` на glad.
5. Удалить `glfw_gl_dispatch.cpp`/`.hpp`, `loader_shim.cpp`/`.hpp`.
6. Regression: существующие text-rendering/GUI smokes; self-host diff;
   `regression_gate.sh`.
7. Docs: `TEXT_RENDERING.md`/`GUI.md` — GLAD2 вместо ручного dispatch; close.

## Out of scope

- Vulkan/D3D11/Metal backends — не нужны, только GL/GLES.
- Обновление до новой версии GL API — паритет с текущим набором функций.
- Переписывание GLFW window/context на MLC (только GL symbol dispatch).

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Decision + сгенерировать оба набора через `glad.sh`, положить в `runtime/third_party/glad/`. | **done** |
| 2 | `extern fn` для всех используемых функций (desktop + headless). | pending |
| 3 | Линковка сгенерированных `.c` в `build_bin.sh`. | pending |
| 4 | Переключить вызывающий MLC-код (те же имена где возможно). | pending |
| 5 | Удалить `glfw_gl_dispatch.*`/`loader_shim.*`. | pending |
| 6 | Regression (existing smokes) + self-host diff + `regression_gate.sh`. | pending |
| 7 | Docs update; close. | pending |

<!-- STEP=1 sub-steps: 1) inventory GL symbols from glfw_gl_dispatch.hpp + loader_shim; pick GL Core version (≥3.3 if all present); 2) generate desktop + EGL/GLES2 via glad.sh; vendor under runtime/third_party/glad/{gl,egl}/ + README; 3) lock Decision table (version, link via build_bin.sh); PLAN→STEP=2 -->
<!-- STEP=2 sub-steps: 1) map each used symbol to extern fn = "…" from glad header; 2) keep context/input wrappers strategy (GLFW still needed — thin remain or rename); 3) compile-check one TU -->
<!-- STEP=3 sub-steps: 1) add glad .c to build_bin.sh / GUI link flags; 2) smoke link text_window or gl_renderer demo; 3) document flags -->
<!-- STEP=4 sub-steps: 1) retarget misc/gui/*.mlc from glfw_gl_dispatch.hpp → glad; 2) fix any name mismatches; 3) run existing GUI/text smokes -->
<!-- STEP=5 sub-steps: 1) delete dispatch+shim sources; 2) purge includes from mlc.hpp/build; 3) confirm no references -->
<!-- STEP=6 sub-steps: 1) regression_gate 20/0; 2) self-host DIFF 0; 3) GUI smokes green -->
<!-- STEP=7 sub-steps: 1) TEXT_RENDERING/GUI/STDLIB docs; 2) PLAN close; 3) archive → Critic -->
