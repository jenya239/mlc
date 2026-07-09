# Track: FFI-слой (RawPointer, extern codegen, линковка, C function pointer)

Parent: [../PLAN.md](../PLAN.md) §10, [../FFI_LAYER.md](../FFI_LAYER.md)
(полная спецификация — читать перед началом работы, там же факт-таблица
текущего состояния с точными файлами/строками).

Приоритет: **средний** — concurrency Isolate/TaskScope closed.

## Status: **closed** 2026-07-09 — STEP=1–8 done

**Driver 2026-07-09:** STEP=8 — self-host `mlcc`→`mlcc2`→`diff` identical;
`regression_gate.sh` 20/20; `FFI_LAYER.md` §6 acceptance filled.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | `RawPointer[T]` в Ruby-бутстрапе: новый builtin generic type, `lib/mlc/registries/type_registry.rb` + `type_mapper.rb` (codegen → `T*`). Методы: `is_null`, `==`/`!=` на null. Без refcounting, без `Shared<T>`-обёртки. Тесты в `test/mlc/`. | **done** |
| 2 | `RawPointer[T]` в self-hosted (`compiler/`) — checker (registry entry, `compiler/checker/registry.mlc`) + codegen (`compiler/codegen/decl/type_gen.mlc`). Self-host verify gate. | **done** |
| 3 | `extern fn name(...) -> Ret = "c_name" from "<header>"` — синтаксис уже намечен в `PLAN.md:564-567`, реализовать реальный codegen: `#include "<header>"` в `.cpp` того модуля, где объявление, плюс настоящий C++ вызов вместо `empty_cpp_declaration()` (`compiler/codegen/decl_cpp.mlc:517,538,1145,1154`). Сначала Ruby, потом self-hosted. Smoke: `extern fn sqrt(x: f64) -> f64 = "sqrt" from "<math.h>"`, вызов, сравнение с ожидаемым значением. | **done** |
| 4 | `extern lib "name"` — декларация на уровне модуля; `build_bin.sh` собирает список по всем импортированным модулям финальной программы, добавляет `-l<name>` в командную строку линковки (не в компиляцию `.o`). Проверить на `-lm` (везде есть, не требует внешней инсталляции) + STEP=3 smoke. | **done** |
| 5 | `extern type Name = "CName" from "<header>"` в self-hosted парсере (сегодня только Ruby: `lib/mlc/source/parser/declaration_parser.rb:534-560`) — `compiler/frontend/parser/decls.mlc` (сейчас `type Name` без `=` уходит в `parse_variants`, `:699-733` — не opaque). Плюс `drop "c_function"` — генерация RAII-обёртки (`std::unique_ptr` + custom deleter) для функций, возвращающих `-> owned RawPointer[T]`. | **done** (typedef+deleter; `owned` return marker deferred) |
| 6 | `extern fn(Args) -> Return` как тип (C function pointer, не closure) — codegen `Ret(*)(Args...)`; checker запрещает передачу не-top-level/захватывающей функции в такую позицию (переиспользовать `compiler/checker/escape_analysis.mlc`, новая диагностика вместо тихого несоответствия типов). | **done** |
| 7 | Concurrency-метаданные на `extern fn`/`extern type`: `blocking`/`thread_safe`/`thread_affine` (спецификация `CONCURRENCY_V2.md:369-380`). Send/Sync predicates ready (CONCURRENCY_V2 closed). | **done** |
| 8 | Verify-gate + close: полный self-host (`mlcc`→`mlcc2`→`diff`), `regression_gate.sh`, обновить `FFI_LAYER.md` критерий приёмки (§6) с фактическими результатами. | **done** |

## Verify gate (STEP=8, 2026-07-09)

```
compiler/build.sh → ok
mlcc -o p1 compiler/main.mlc → 0
build_bin.sh p1 mlcc2 → 0
mlcc2 -o p2 compiler/main.mlc → 0
diff -rq p1 p2 --exclude=obj → identical (exit 0)
scripts/regression_gate.sh → 20 passed, 0 failed
```

Known deferrals (not blockers for close): `owned` return-marker wiring;
ASan/valgrind smoke on drop RAII; `rake test_compiler_mlc` blocked by
pre-existing Ruby parse fail on `spawn_capture.mlc` (MATCH).

## Out of scope (этот трек — см. `FFI_LAYER.md` §3)

- Генератор биндингов (bindgen-аналог).
- Variadic C-функции.
- Layout-совместимый "raw struct" режим (ffmpeg-класс структур).
- Сами биндинги (libpq/OpenGL/GTK/ffmpeg) — следующие треки, не создавать
  раньше закрытия этого.
