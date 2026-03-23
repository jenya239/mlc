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

## Следующие шаги

### 1. Error handling

```mlc
type Result<T, E> = Ok(T) | Err(E)

fn parse(src: string) -> Result<AST, string> = ...

fn compile(src: string) -> Result<string, string> = do
  const ast = parse(src)?      // propagate Err
  const cpp = codegen(ast)?
  Ok(cpp)
end
```

**Что нужно:**
- `Result<T,E>` как встроенный тип (или stdlib)
- `?` оператор: в Ruby-пайплайне — `TryExpr` → SemanticIR → C++ (`try_rule` / `variable_decl_rule` с early return где разобрано)
- self-hosted (`compiler/`): `ExprQuestion` разобран в `let`/`return`/`expr`-stmt и в теле `do` перед `return`; в произвольном подвыражении `gen_expr` пока снимает `?` без unwrap (ограничение)

### 2. Generics

```mlc
fn identity<T>(x: T) -> T = x
fn map<T, U>(arr: [T], f: fn(T) -> U) -> [U] = ...
```

**Варианты реализации:**
- Мономорфизация (как Rust) — просто, но раздувает код
- C++ templates (передать as-is) — проще реализовать

### 3. Traits

```mlc
trait Display {
  fn to_string(self: Self) -> string
}

extend i32 : Display {
  fn to_string(self: i32) -> string = extern
}
```

Требует изменений во всех слоях: SemanticIR, codegen, stdlib.

## Критический путь к full self-hosting

```
mlcc компилирует compiler/ → работает
         ↓
добавить Result<T,E> + ?   (error handling в compiler)
         ↓
generics                   (обобщённые алгоритмы)
         ↓
traits                     (visitor pattern, Display)
         ↓
mlcc полностью заменяет Ruby-компилятор
```

## Команды

```bash
# Unit-тесты
bundle exec ruby -Ilib:test -e "Dir['test/mlc/**/*_test.rb'].each { |f| require_relative f }"

# Собрать mlcc
compiler/build.sh

# Bootstrap-тест
compiler/out/mlcc compiler/main.mlc && \
  cd out && g++ -std=c++20 -I ../runtime/include \
    ast.cpp ast_tokens.cpp codegen.cpp decls.cpp exprs.cpp \
    infer.cpp lexer.cpp main.cpp names.cpp preds.cpp registry.cpp types.cpp \
    ../runtime/src/io/io.cpp ../runtime/src/core/string.cpp \
    -o /tmp/mlcc2
```
