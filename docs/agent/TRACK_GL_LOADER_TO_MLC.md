# Track: GL function-pointer loader — на MLC, не C++

Parent: [../FFI_LAYER.md](../FFI_LAYER.md) §8, [TRACK_FFI_POINTER_CAST](TRACK_FFI_POINTER_CAST.md) (hard dependency), [../PLAN.md](../PLAN.md).
Trigger: пользователь 2026-07-11 — без ручного C++. Обсуждение в чате:
современный OpenGL (всё новее версии 1.1) не экспортирует линкуемые символы
из `libGL.so` — адрес функции отдаёт только `glXGetProcAddress`/
`glfwGetProcAddress` во время выполнения (так работает GL везде, не
ограничение MLC — отсюда GLEW/GLAD/epoxy в C++-экосистеме). Заголовок
(`<GL/gl.h>`/`glcorearb.h`) даёт только типы/константы, не сами вызовы.

## Status: blocked — depends on TRACK_FFI_POINTER_CAST (not started)

## Проблема

`runtime/src/gl/glfw_gl_dispatch.cpp` (812 строк) + `.hpp` (130 строк) —
таблица ~40-70 GL function pointers, каждый заполняется через
`reinterpret_cast<FunctionPointer>(glfwGetProcAddress(name))`. Это не
бизнес-логика и не биндинг к конкретной функции — это ABI-необходимый
loader. Порт на MLC требует примитива каста `RawPointer[T]` → `extern fn(...)`
(нет сегодня, см. `TRACK_FFI_POINTER_CAST`).

## Цель

Таблица GL-указателей — MLC `record` из полей типа `extern fn(...)`,
заполняемая вызовами `glfwGetProcAddress` (уже легитимный `extern fn`,
реальный символ GLFW) + новым cast-примитивом. `glfw_gl_dispatch.cpp`
удаляется.

## Decision (STEP=1, после закрытия TRACK_FFI_POINTER_CAST)

| Вопрос | Вариант | Рекомендация |
|--------|---------|--------------|
| Список функций | 1:1 с текущим C++ (не расширять, не сокращать) — извлечь список имён из `glfw_gl_dispatch.cpp` | да, паритет прежде всего |
| Хранение | `record GlFunctionTable { gl_draw_arrays: extern fn(i32, i32, i32) -> unit, ... }`, один instance, заполняется один раз при инициализации контекста | соответствует текущему usage-паттерну (init once, вызовы через таблицу) |
| Обратная совместимость вызывающего кода (`text_window_demo.mlc`, `gui_button_demo.mlc` и т.п.) | Сохранить те же имена MLC-level функций (`gl_draw_arrays(...)` и т.п.), поменять только реализацию под капотом | да, не трогать вызывающий код за пределами loader-модуля |

## Scope

1. Дождаться закрытия `TRACK_FFI_POINTER_CAST`.
2. Decision + извлечь полный список GL-функций из текущего C++ (не
   пропустить ни одной — regression risk).
3. `record GlFunctionTable` + init-функция на MLC (`glfwGetProcAddress` +
   cast для каждой функции).
4. Переключить существующие вызовы (`text_window_demo.mlc`, GUI-демо) на
   новую таблицу — тот же публичный API имён, что раньше.
5. Удалить `glfw_gl_dispatch.cpp`/`.hpp`.
6. Regression: text-rendering/GUI smokes (уже существующие, не новые);
   self-host diff.

## Out of scope

- EGL/GLES2 loader (`runtime/src/gl/loader_shim.cpp`) — отдельно: GLES
  функции в основном линкуются статически из `libGLESv2.so` (не требуют
  `eglGetProcAddress` для core-функций так же массово, как desktop GL) —
  проверить на отдельном STEP перед тем как включать в этот трек или
  открывать новый; не блокировать закрытие этого трека этой проверкой.
- Автогенерация таблицы из Khronos `gl.xml` registry — не нужна, список
  функций — фиксированные ~40-70 имён, ручное перечисление на MLC
  (1:1 копия текущего C++ списка) достаточно, генератор — overkill для
  этого масштаба (см. `FFI_LAYER.md` §3, §7 — тот же принцип: не строить
  генератор без объёма, который его оправдывает).

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Ждать `TRACK_FFI_POINTER_CAST` closed. | pending |
| 2 | Decision + извлечь полный список функций 1:1. | pending |
| 3 | `GlFunctionTable` record + init на MLC. | pending |
| 4 | Переключить вызывающий код (text/GUI demos). | pending |
| 5 | Удалить `glfw_gl_dispatch.*`. | pending |
| 6 | Regression (existing smokes) + self-host diff; close. | pending |
