# План развития MLC

Дата: март 2026

## Текущее состояние

### Готово (self-hosted mlcc)

| Фича | Состояние |
|------|-----------|
| Лексер, парсер | ✅ |
| AST: record/sum типы, функции, блоки | ✅ |
| Name resolution | ✅ |
| Type inference (базовый) | ✅ |
| Codegen → C++20 | ✅ |
| Модульная компиляция (import/export → hpp/cpp) | ✅ |
| Pattern matching: PatCtor, PatRecord, PatIdent, PatWild | ✅ |
| `Shared<T>` / `[T]` / `[K:V]` | ✅ |
| `let mut` / `const` | ✅ |
| `extend Type { fn method(self) }` | ✅ |
| F-string `f"hello {name}"` | ✅ |
| Bootstrap convergence (mlcc→mlcc→mlcc) | ✅ |

### Отсутствует

| Фича | Приоритет | Оценка трудоёмкости |
|------|-----------|---------------------|
| `Result<T,E>` + `?` operator | Высокий | ~20% от сделанного |
| Пользовательские generics (`fn foo<T>`) | Высокий | ~60% от сделанного |
| Traits (`trait Foo + extend T : Foo`) | Высокий | ~80% от сделанного |
| Lambdas в mlcc (полная поддержка) | Средний | ~15% |
| File I/O в stdlib (read_file, write_file) | Средний | ~10% |
| Инкрементальная сборка (кэш .hpp) | Низкий | ~10% |
| Стандартная библиотека (Iterator, Display) | Низкий | бесконечно |

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
- `?` оператор в парсере и codegen: `ExprQuestion(inner)` → `try { ... } / early return`
- Сейчас `ExprQuestion` в AST есть, codegen его игнорирует

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
