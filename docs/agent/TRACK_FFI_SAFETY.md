# Track: FFI safety contract

Parent: [../PLAN.md](../PLAN.md), [../FFI_LAYER.md](../FFI_LAYER.md) §2
(инфраструктура уже реализована, `TRACK_FFI_LAYER` closed 2026-07-09 — этот
трек только про безопасность использования, без нового codegen).

## Status: **open, низкий приоритет** — доки/checker-диагностики, не блокирует
`TRACK_TEXT_RENDERING`/`STDLIB_BACKEND` (те продолжают использовать `extern`
как есть).

## Проблема

`extern fn`/`RawPointer[T]`/`extern type` — полный unsafe escape hatch без
единого маркера в языке. В отличие от Rust (`unsafe fn`, `unsafe {}`), в MLC
вызов `extern fn` синтаксически неотличим от обычного вызова — нет сигнала
читателю кода "здесь ответственность за lifetime/null/ABI на тебе, не на
компиляторе". Риски, которые сегодня не проверяются и не документированы:

1. **Lifetime.** `RawPointer[T]`, возвращённый extern-функцией, не привязан
   ни к какому scope — use-after-free после `drop`-деструктора обёртывающего
   `owned` значения не детектируется.
2. **Null.** `is_null()` есть, но ничего не заставляет его вызывать перед
   разыменованием/передачей далее.
3. **ABI-совпадение сигнатуры.** `extern fn foo(x: i32) -> i32 = "foo" from
   "<h.h>"` — если реальная C-сигнатура другая, никакой проверки на этапе
   компиляции (заголовок не парсится для сверки, см. `FFI_LAYER.md` §1) —
   молчаливый UB.
4. **Thread-safety.** Concurrency-атрибуты (`blocking`/`thread_safe`/
   `thread_affine`, `FFI_LAYER.md` §2.7) реализованы (STEP=7 closed), но
   ничего не мешает написать `extern fn` без атрибута вообще — дефолт не
   зафиксирован явно ни в одном документе.

## Задача трека

Не менять codegen — сделать риск видимым: явный маркер в синтаксисе +
диагностики checker там, где это дёшево.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design-решение: нужен ли синтаксический маркер `unsafe extern fn`/блок `unsafe { ... }`, или достаточно диагностики checker без нового синтаксиса (см. открытые вопросы ниже). | pending |
| 2 | Checker: диагностика (warning, не error) на `extern fn`/`extern type` без явного concurrency-атрибута — заставить автора выбрать `blocking`/`thread_safe`/`thread_affine` осознанно, а не молчаливым дефолтом. | pending |
| 3 | Checker: базовая ABI sanity-проверка для `RawPointer` C function pointer (§2.4 `FFI_LAYER.md`) — количество параметров extern-объявления совпадает с количеством в `from "<header>"`, если заголовок уже импортирован через `header_import.mlc` (best-effort, не полная верификация). | pending |
| 4 | Документация: раздел "Safety contract" в `FFI_LAYER.md` §8 — что компилятор проверяет, что нет, что на совести автора биндинга (lifetime, null, реальная ABI-сверка при отсутствии header import). | pending |
| 5 | Verify-gate + close: self-host diff identical, `regression_gate.sh`. | pending |

## Открытые вопросы (design, STEP=1)

1. Синтаксический маркер (`unsafe extern fn`) — плюс: видимость в коде;
   минус: ломает уже написанный код STEP=1-8 `TRACK_FFI_LAYER` (миграция).
   Альтернатива: маркер не нужен, `extern` сам по себе уже является
   маркером — тогда трек ограничивается диагностиками (STEP=2-3) и
   документацией (STEP=4) без изменения синтаксиса.
2. Уровень диагностики STEP=2 — warning (не блокирует компиляцию) или error
   (требует явного атрибута всегда)? Склон к warning: `blocking` дефолт
   безопасен консервативно (хуже performance, не хуже correctness), делать
   error — ломает эргономику для тривиальных биндингов (`sqrt`).

## Out of scope

- Полный borrow checker для `RawPointer` — не в roadmap проекта.
- Автоматическая генерация ABI-описания из `.h` без явного `header_import`
  (см. `FFI_LAYER.md` §3 — bindgen-аналог отдельно, не здесь).
