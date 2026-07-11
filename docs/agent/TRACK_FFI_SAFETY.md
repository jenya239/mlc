# Track: FFI safety contract

Parent: [../PLAN.md](../PLAN.md) §14, [../FFI_LAYER.md](../FFI_LAYER.md) §9
(инфраструктура уже реализована, `TRACK_FFI_LAYER` closed 2026-07-09 — этот
трек только про безопасность использования, без нового codegen).

## Status: **active** — STEP=1 next (Design: marker vs diagnostics-only)

**Planner 2026-07-11:** Activated after GLAD Critic OK. §20a–e all closed;
§20 parent → done. Next work = low-pri §14 FFI_SAFETY (queue order before
ERROR_UNION / DEBUG_SOURCE_MAP).

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
| 4 | Документация: раздел "Safety contract" в `FFI_LAYER.md` §9 — что компилятор проверяет, что нет, что на совести автора биндинга (lifetime, null, реальная ABI-сверка при отсутствии header import). | pending |
| 5 | Verify-gate + close: self-host diff identical, `regression_gate.sh`. | pending |

<!-- STEP=1 sub-steps: 1) inventory current `extern fn` sites without concurrency attrs (stdlib/runtime bindings); 2) lock Decision table: (A) new `unsafe` syntax vs (B) `extern` already marker + warnings only; prefer B unless A has clear win without mass migration; 3) lock STEP=2 severity (warning vs error) + default attr when absent; PLAN→STEP=2 -->
<!-- STEP=2 sub-steps: 1) find checker hook for extern decls; 2) emit warning code + message; 3) one positive + one negative test -->
<!-- STEP=3 sub-steps: 1) only when header_import already loaded the header; 2) compare arity; 3) skip if header not imported -->
<!-- STEP=4 sub-steps: 1) rewrite FFI_LAYER §9 from «не реализовано» to contract table; 2) cross-link TRACK; 3) note lifetime/null remain author duty -->
<!-- STEP=5 sub-steps: 1) regression_gate 20/0; 2) self-host DIFF 0; 3) archive → Critic -->

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
