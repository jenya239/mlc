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

## Status: open — STEP=1 next

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
30 функций) + `runtime/include/mlc/gl/glfw_gl_dispatch.hpp` (130 строк) +
`runtime/src/gl/loader_shim.cpp` (232 строки, отдельный EGL/GLES2 headless-путь).

## Цель

`extern fn`-биндинги на MLC-уровне к тем же 30 (desktop GL) + GLES2-функциям
(headless), реализация — сгенерированный `glad.c`/`glad.h` (desktop, Core
профиль, версия по потребности — проверить минимально нужную на STEP=1) +
отдельный сгенерированный EGL/GLES2 loader для headless-пути. Собственный
C++ dispatch удаляется.

## Decision (STEP=1)

| Вопрос | Вариант | Рекомендация |
|--------|---------|--------------|
| Генерация | Через `https://glad.sh` (веб-сервис, без локального Python — не нарушает правило "скрипты только Ruby/JS", мы не пишем/не запускаем генератор) | да |
| API/версия | Desktop: GL Core, версия = максимум используемых функций (проверить, что все 30 есть в GL 3.3 Core — вероятно да) | сверить на STEP=1 |
| Headless | Отдельная генерация под EGL + GLES2 (два разных target у GLAD2) | да, два вендоренных набора файлов, не один universal |
| Где хранится сгенерированный код | `runtime/third_party/glad/` (новая директория, явно помечена как vendored, не наш код) | явное разделение "наш код" vs "вендоренный генератор" |
| MLC-уровень | `extern fn` в `misc/gui/gl_bindings.mlc` (или где сейчас объявлены `gl_draw_arrays` и т.п.) — тот же публичный API имён, что и сейчас, меняется только реализация под капотом | не трогать вызывающий код (`text_window_demo.mlc`, GUI demos) |

## Scope

1. Decision (таблица) + сгенерировать desktop (GL Core, `glad.sh`) и
   headless (EGL+GLES2, `glad.sh`) наборы, вендорить в `runtime/third_party/glad/`.
2. `extern fn` для всех 30 desktop GL-функций + используемых GLES2/EGL
   функций → `"<glad name>" from "<glad/gl.h>"`/`"<glad/egl.h>"`.
3. `extern lib`/линковка: `runtime/third_party/glad/*.c` компилируется как
   обычный C (не через `extern lib`, напрямую в `build_bin.sh`'s object list —
   решить на STEP=1, вероятно проще как дополнительный `.c` в сборке, не
   отдельная либа).
4. Переключить `text_window_demo.mlc`/GUI-демо на новые `extern fn` (те же
   имена — минимальная правка вызывающего кода).
5. Удалить `glfw_gl_dispatch.cpp`/`.hpp`, `loader_shim.cpp`/`.hpp`.
6. Regression: существующие text-rendering/GUI smokes (не новые тесты,
   паритет поведения); self-host diff; `regression_gate.sh`.
7. Docs: `TEXT_RENDERING.md`/`GUI.md` — упомянуть GLAD2 вместо ручного dispatch.

## Out of scope

- Vulkan/D3D11/Metal backends — не нужны, только GL/GLES.
- Обновление до новой версии GL API — паритет с текущим набором функций.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Decision + сгенерировать оба набора через `glad.sh`, положить в `runtime/third_party/glad/`. | pending |
| 2 | `extern fn` для всех используемых функций (desktop + headless). | pending |
| 3 | Линковка сгенерированных `.c` в `build_bin.sh`. | pending |
| 4 | Переключить вызывающий MLC-код (те же имена). | pending |
| 5 | Удалить `glfw_gl_dispatch.*`/`loader_shim.*`. | pending |
| 6 | Regression (existing smokes) + self-host diff + `regression_gate.sh`. | pending |
| 7 | Docs update; close. | pending |
