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
| `ExprQuestion` внутри произвольного `gen_expr` | ❌ passthrough (без unwrap) |
| `Result`/`?` как в PLAN-примере end-to-end в mlcc | ❌ |
| Пользовательские `fn foo<T>` в mlcc | ❌ |
| Traits в mlcc как у Ruby | ❌ |
| Unit-тесты `compiler/tests/` (сборка через `build_tests.sh`) | ✅ |

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

### Итерация 1 — `Result<T,E>` + `?` в mlcc (приоритет: высокий)

**Цель:** self-hosted компилятор использует `Result` для ошибок вместо `[string]`.

**Текущее состояние:**
- Ruby: `Result<T,E>` как stdlib (`lib/mlc/common/stdlib/core/result.mlc`), `TryExpr` + `try_rule` + `variable_decl_rule` — работают.
- mlcc: `ExprQuestion` в `let`/`return`/stmt (`gen_try_unwrap`) — готово. В `gen_expr` (вложенный `?`) — passthrough без unwrap.

**Что делать:**
1. В `compiler/codegen.mlc::gen_expr` ветка `ExprQuestion` — вместо passthrough сгенерировать `__try_N = inner; if (Err) return Err; std::get<Ok>(__try_N).field0`. Это требует `try_counter` внутри `gen_expr` → передавать его как параметр или использовать статический счётчик строк.
2. Добавить `Result` в `compiler/ast.mlc` и `compiler/checker/registry.mlc` как известный generic-тип.
3. Написать тест в `compiler/tests/test_codegen.mlc` на `? expr` внутри выражения.

---

### Итерация 2 — Generics в mlcc (приоритет: высокий)

**Текущее состояние:**
- Ruby: generics разобраны в парсере и SemanticIR, C++ templates генерируются.
- mlcc: `fn foo<T>` в парсере `decls.mlc` разбирается (через `parse_type_params_opt`), type_params передаются в `DeclFn`. В `gen_fn_decl` → `template_prefix(type_params)` и `requires_clause(type_params, type_bounds)` — уже работают.
- Что **не** работает: `check_names_expr` не знает о `T` как имени типа для пользовательских параметров в body; `registry.mlc` не регистрирует generic-функции; codegen не выводит concrete instantiation.

**Что делать:**
1. `compiler/checker/names.mlc::collect_globals` — добавить регистрацию имён параметров типа в locals для тела функции.
2. Тест в `compiler/tests/test_checker.mlc` — `fn id<T>(x: T) -> T = x` уже есть (0 errors). Добавить тест на вызов `id(42)`.
3. `compiler/checker/infer.mlc` — при вызове `fn id<T>` инстанцировать тип (простейший вариант — не проверять, возвращать TUnknown).
4. `compiler/codegen.mlc` — убедиться что `template<typename T>` эмитируется (уже есть через `template_prefix`).

---

### Итерация 3 — Traits в mlcc (приоритет: высокий)

**Текущее состояние:**
- Ruby: полный пайплайн traits через `DeclTrait` / `extend T : Foo` / `requires`-concept в C++.
- mlcc: `compiler/parser/decls.mlc` разбирает `trait`-блок (`parse_trait_body`), `compiler/checker/registry.mlc` имеет `build_registry` с полями. `gen_trait_decl` в `codegen.mlc` генерирует `concept`. Impl через `extend T : Foo` — генерирует `static_assert`.

**Что делать:**
1. Добавить тест `compiler/tests/` на компиляцию trait + impl + вызов через trait bound.
2. Проверить, что checker в mlcc регистрирует методы trait и не даёт ложных ошибок при вызове через `obj.method()`.
3. Trait inheritance (`trait B : A`) — пока не нужно.

---

### Итерация 4 — Лямбды без дыр в mlcc (приоритет: средний)

**Текущее состояние:**
- Ruby: `Lambda` AST, `x => body` и `(x, y) => body` — парсинг и codegen работают.
- mlcc: парсер разбирает `() => expr`, `(x, y) => expr`. `gen_expr::ExprLambda` генерирует `[=](auto x) { return ...; }`. Тесты в `compiler/tests/test_codegen.mlc` — есть.
- Не проверено: лямбда передаётся как аргумент в вызове (`arr.filter(x => x > 0)`).

**Что делать:**
1. Тест в `compiler/tests/test_codegen.mlc` — `ExprCall` с `ExprLambda` как аргументом.
2. Проверить что `checker/names.mlc` не ругается на имена параметров лямбды.

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
