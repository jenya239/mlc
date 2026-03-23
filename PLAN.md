# План развития MLC

Дата: март 2026 (актуализация плана и привязок)

## Общая цель

**Self-hosted компилятор**: весь пайплайн MLC → C++ реализован на MLC (`compiler/*.mlc`), бинарь `mlcc` собирается из сгенерированного C++ и может пересобирать себя (bootstrap). До полной замены Ruby остаётся закрыть generics/traits/Result в self-hosted слое и перенести оставшуюся семантику.

---

## Что сделано

### Ruby-компилятор (`lib/mlc/`) — основной продакшен-пайплайн

| Область | Состояние |
|---------|-----------|
| Lexer/parser, AST | ✅ |
| SemanticIR, type check, inference | ✅ |
| Lowering → C++ AST, codegen, модульные `.hpp`/`.cpp` | ✅ |
| `const` / `let` / `let mut` / `let const`, присваивание только в `mut` | ✅ |
| `TryExpr` (`?`), частично: IR + C++ (`try_rule`, `variable_decl_rule`) | ⚠️ не везде идеальный unwrap |
| Generics/traits в языке (парсинг, часть семантики) | ⚠️ по фичам; E2E частично |
| Тесты `test/mlc/`, `test/integration/` | ✅ |

### Self-hosted `compiler/*.mlc` (mlcc)

| Область | Состояние |
|---------|-----------|
| Лексер, парсер модулей, AST, импорты | ✅ |
| Имена + базовый infer + registry типов | ✅ |
| Codegen в C++20, модульная сборка (`compiler/build.sh`) | ✅ |
| Pattern matching, records, `extend`, f-строки | ✅ |
| `Shared` / массивы / map, COW в рантайме | ✅ |
| Семантика `let`/`const`/`let mut` (перепривязка) | ✅ приведено к правилам Ruby |
| `ExprQuestion` в stmt/`return`/теле блока (`gen_try_unwrap` и т.д.) | ✅ |
| `ExprQuestion` в `gen_expr` (вложенный `?`) | ✅ GNU statement expr `({ ... })` |
| `Ok`/`Err`/`Result` как builtin globals в checker | ✅ |
| Generic fn call без ложных ошибок checker | ✅ |
| Lambda как аргумент функции | ✅ |
| Trait + extend impl — 0 ложных ошибок checker | ✅ |
| `Result`/`?` end-to-end в mlcc (compiler/ использует Result) | ❌ |
| Generic type codegen (`template<T,E>` на каждую variant struct) | ❌ |
| Unit-тесты `compiler/tests/` (81 passed) | ✅ |

---

## Что не сделано (приоритеты)

| Фича | Слой | Приоритет | Оценка |
|------|------|-----------|--------|
| `Result<T,E>` + `?` полностью (в т.ч. вложенные выражения в mlcc) | Ruby + mlcc | Высокий | ~20% |
| Пользовательские generics `fn foo<T>` | в основном mlcc | Высокий | ~60% |
| Traits `extend T : Foo` в mlcc | mlcc | Высокий | ~80% |
| Lambdas в mlcc без дыр | mlcc | Средний | ~15% |
| File I/O stdlib (`read_file`, …) | stdlib | Средний | ~10% |
| Инкрементальная сборка (кэш `.hpp`) | tooling | Низкий | ~10% |
| Стандартная библиотека (итераторы, Display, …) | stdlib | Низкий | долго |

## Ближайшие шаги (итерации)

### Итерация 1–4 — ЗАВЕРШЕНО

- `ExprQuestion` в `gen_expr`: GNU statement expr `({ auto __q = inner; if (holds<Err>) return get<Err>; get<Ok>.field0; })` — корректная семантика `?` во вложенных выражениях.
- `Ok`/`Err`/`Result` зарегистрированы в checker как built-in globals и constructors.
- Generic fn call — checker не даёт ложных ошибок (type params в locals/globals).
- Trait + extend impl — checker не даёт ложных ошибок.
- Lambda как аргумент — codegen корректен.
- Тестов: 81 passed.

---

### Итерации 5–6 — ЗАВЕРШЕНО

- Generic type codegen: `gen_adt_fwd`/`gen_adt_defs` теперь применяют `template_prefix` к каждой variant-struct отдельно; `using R = std::variant<Ok<T,E>, Err<T,E>>`.
- `gen_try_unwrap` и `ExprQuestion` в `gen_expr` переведены на индексный доступ (`std::get_if<1>`, `std::get<0>`) — работают для любого generic `Result<T,E>`.
- `type Result<T, E> = Ok(T) | Err(E)` добавлен в `compiler/ast.mlc` — доступен всем модулям.
- Тестов: 83 passed.

---

### Итерация 7 — Использование `Result` в compiler/*.mlc (приоритет: средний)

**Цель:** переписать хотя бы один parser/checker на `Result<X, [string]>` вместо отдельного списка ошибок.

**Что делать:**
1. В `compiler/checker/infer.mlc::check` возвращать `Result<CheckOut, [string]>` вместо `CheckOut { errors, registry }` с проверкой `errors.length() > 0`.
2. В `compiler/main.mlc` использовать `?` для chain-а check → codegen.
3. Добавить E2E тест компиляции через mlcc программы с `Result`.

---

### После (низкий приоритет)

- **File I/O в stdlib** (`read_file`, `write_file`) — уже `File.read` / `File.write` есть в mlcc; для Ruby — в `stdlib/io/`.
- **Инкрементальная сборка** — кэшировать `.hpp` если AST не изменился.
- **Iterator / Display** — после traits.

---

## Критический путь к full self-hosting

```
mlcc компилирует compiler/ → работает  ✅
         ↓
Итерация 1: Result<T,E> + ? в gen_expr → mlcc использует Result для ошибок
         ↓
Итерация 2: Generics — проверка вызовов fn<T> без ложных ошибок
         ↓
Итерация 3: Traits — concept + impl в mlcc без ложных ошибок checker'а
         ↓
Убрать Ruby-пайплайн как обязательный (mlcc достаточен для компиляции mlc)
```

## Команды

```bash
# Unit-тесты Ruby-пайплайна
bundle exec rake test_mlc

# Собрать mlcc (Ruby → C++ → g++)
compiler/build.sh

# Self-hosted тесты (требует g++)
compiler/tests/build_tests.sh

# Один тест Ruby
bundle exec ruby -Ilib:test test/mlc/some_test.rb
```
