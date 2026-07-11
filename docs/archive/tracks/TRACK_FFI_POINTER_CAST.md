> **Superseded 2026-07-11, тем же чатом, ~30 минут после создания.** Причина:
> факт-чек показал, что `libepoxy` (наш собственный C++-референс
> `gtk-gl-cpp-2025` использует именно её) объявляет GL-функции как макрос →
> глобальная переменная-указатель-на-функцию, резолвится ленивo **внутри**
> `libepoxy.so`. Уже закрытый `extern fn ... = "glDrawArrays" from
> "<epoxy/gl.h>"` эмитит текстовый вызов `glDrawArrays(...)`, который проходит
> через макрос прозрачно — каст `RawPointer→fn-ptr` не нужен вообще. См.
> [TRACK_GL_EPOXY_MIGRATION](../../agent/TRACK_GL_EPOXY_MIGRATION.md).

# Track: RawPointer[T] → extern fn(...) cast — примитив языка (superseded, не нужен)

Parent: [../FFI_LAYER.md](../FFI_LAYER.md) §2.4/§8, [../PLAN.md](../PLAN.md).
Trigger: пользователь 2026-07-11 — предпосылка для [TRACK_GL_LOADER_TO_MLC](TRACK_GL_LOADER_TO_MLC.md)
(GL function-pointer loader на MLC вместо `glfw_gl_dispatch.cpp`).

## Status: open — STEP=1 next

## Проблема

`extern fn(Args) -> Return` как тип (C function pointer, `FFI_LAYER.md` §2.4)
закрыт для **объявления** (тип существует, codegen → `Ret(*)(Args...)`,
запрет на лямбды/захват). Нет способа **получить** значение этого типа из
`RawPointer[Unit]`, который возвращает `glfwGetProcAddress`/`dlsym`/
`eglGetProcAddress`. В C++ это `reinterpret_cast<FunctionPointer>(void*)` —
единственное легитимное использование `reinterpret_cast` в этом домене
(ABI-required, не UB при соответствии реальной сигнатуры символа).

## Цель

MLC-выражение, конвертирующее `RawPointer[Unit]` (или `RawPointer[Byte]`) в
конкретный `extern fn(...)` тип, без общего unsafe `reinterpret_cast` для
всех типов (узкий примитив, не дыра в системе типов).

## Decision (STEP=1)

| Вопрос | Вариант | Рекомендация |
|--------|---------|--------------|
| Синтаксис | `raw_pointer.as_function[extern fn(i32) -> i32]()` (метод с явным type argument) vs новый keyword | метод — не требует нового keyword, паттерн похож на существующие generic static methods |
| Codegen | `reinterpret_cast<Ret(*)(Args...)>(pointer)` — единственное разрешённое место в сгенерированном коде для `reinterpret_cast` в этом контексте | да, узко зафиксировать в codegen, не выставлять общий `reinterpret_cast` пользователю |
| Safety | Помечается как unsafe-операция (см. [TRACK_FFI_SAFETY](TRACK_FFI_SAFETY.md)) — соответствие сигнатуры символу — ответственность автора, как весь остальной extern-слой | да, тот же контракт, что `RawPointer`/`extern fn` в целом |

## Scope

1. Decision (синтаксис).
2. Парсер/checker: новый built-in метод на `RawPointer[T]`, type argument —
   `extern fn(...)` тип.
3. Codegen: `reinterpret_cast<Ret(*)(Args...)>(...)`.
4. Тест: `dlsym`/`glfwGetProcAddress`-подобный smoke (получить указатель на
   известную C-функцию, вызвать через каст, сравнить результат с прямым
   вызовом).
5. Self-host diff + `regression_gate.sh`.

## Out of scope

- Общий `reinterpret_cast`/`unsafe cast<T>` для произвольных типов — не
  открывать, узкий кейс только function pointer из raw pointer.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Decision: синтаксис метода + type argument. | pending |
| 2 | Parser/checker: built-in метод, типовая проверка target type = `extern fn(...)`. | pending |
| 3 | Codegen: `reinterpret_cast`. | pending |
| 4 | Smoke-тест (dlsym-подобный). | pending |
| 5 | Self-host diff + regression_gate; close. | pending |
