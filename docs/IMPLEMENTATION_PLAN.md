# MLC Implementation Plan

Дата ревизии: 2026-02-23  
Основа: MEMORY_MODEL.md — value semantics + COW + capture-by-value

---

## Текущее состояние (аудит)

### Работает
- `mlc::Array<T>` — COW с ручным refcount, основные методы
- `mlc::HashMap<K,V>` — COW с ручным refcount
- `mlc::String` — value type
- `Shared<T>` / `Weak<T>` — `shared_ptr` / `weak_ptr`
- `|>` pipe operator
- Record update `{ ...base, field: val }`
- Базовый lambda-синтаксис в Ruby-компиляторе (`x => expr`)
- `let` / `let mut` / `const` — три формы биндингов

### Не соответствует модели
- `let mut` вместо `let` для rebindable — многословно, неинтуитивно
- `let` вместо `const` для non-rebindable — семантика не соответствует имени
- `const` вместо `let const` для compile-time — смешивает два понятия
- Блоки (`[&]` capture) — должны быть `[=]`
- `mlc::Array` использует ручной refcount вместо `shared_ptr<vector<T>>`
- `mlc::HashMap` аналогично
- HOF функции (`map`, `filter`, `fold`) — свободные функции в `collections.hpp`, не методы на Array
- Lambda не поддержана в self-hosted компиляторе (mlcc)

### Отсутствует
- Тип функции `(T -> R)` как first-class тип
- Method chaining на коллекциях
- `each`, `map`, `filter`, `fold` как методы Array
- `sort_by`, `group_by`, `zip`, `flat_map`, `enumerate` как методы
- `Weak<T>` документирован, но не интегрирован в stdlib паттерны

---

## Фаза 1: Переименование биндингов

**Цель:** `let` = rebindable, `const` = non-rebindable, `let const` = constexpr

**Изменения в Ruby-компиляторе:**

- `lib/mlc/source/parser/lexer.rb` — добавить `const` как второй вариант, не только keyword
- `lib/mlc/source/parser/statement_parser.rb`
  - `let x = expr` → `VariableDecl(mutable: true)`
  - `const x = expr` → `VariableDecl(mutable: false, constant: false)`
  - `let const x = expr` → `VariableDecl(mutable: false, constant: true)`
  - убрать `let mut` (или оставить как deprecated alias)
- `lib/mlc/representations/semantic/gen/services/inference/type_inference_service.rb`
  - убрать проверку `let mut` для `.push()`/`.pop()`/`.set()` — мутация значения всегда разрешена
- `lib/mlc/backends/cpp/rules/statements/variable_decl_rule.rb`
  - `constant: true` → `constexpr auto x = expr;`
  - `constant: false` → `auto x = expr;`
- `test/mlc/let_test.rb` — обновить тесты под новый синтаксис

**Изменения в mlcc:**

- `compiler/ast.mlc` — убрать поле `is_mut` из `StmtLet`, добавить `is_const`
- `compiler/parser/stmts.mlc` — парсить `let x`, `const x`, `let const x`
- `compiler/parser/preds.mlc` — `is_keyword_const`
- `compiler/codegen.mlc` — обновить генерацию `StmtLet`
- `compiler/checker/names.mlc`, `compiler/checker/infer.mlc` — обновить паттерны

**Рефакторинг источников mlcc:** все 612 вхождений `let mut` → `let`, все 837 `let` → `const`

**Тесты:**
```
test/mlc/let_test.rb            — новые тесты let/const/let const
compiler/tests/test_parser.mlc  — обновить тесты парсера
```

---

## Фаза 2: Array и HashMap на shared_ptr

**Цель:** exception safety, упростить код, подготовить к методам

**`runtime/include/mlc/core/array.hpp`:**

```cpp
template<typename T>
class Array {
    std::shared_ptr<std::vector<T>> data_;

    void detach() {
        if (data_.use_count() > 1)
            data_ = std::make_shared<std::vector<T>>(*data_);
    }

public:
    Array() : data_(std::make_shared<std::vector<T>>()) {}
    // ... copy/move через shared_ptr copy/move
    ~Array() = default;  // RAII через shared_ptr

    void push(const T& v) { detach(); data_->push_back(v); }
    void pop()            { detach(); data_->pop_back(); }
    void set(size_t i, const T& v) { detach(); (*data_)[i] = v; }
    // ... остальные методы без изменений
};
```

**`runtime/include/mlc/core/hashmap.hpp`** — аналогично.

**Тесты:** все существующие тесты должны пройти без изменений.

---

## Фаза 3: HOF как методы Array

**Цель:** `arr.map(fn x => x * 2)` вместо `mlc::collections::map(arr, fn)`

**`runtime/include/mlc/core/array.hpp`** — добавить методы:

```cpp
// Immutable HOF — возвращают новый Array
template<typename F>
auto map(F&& fn) const -> Array<std::invoke_result_t<F, const T&>>;

template<typename F>
Array<T> filter(F&& pred) const;

template<typename Acc, typename F>
Acc fold(Acc init, F&& fn) const;    // fold(0, fn acc x => acc + x)

template<typename F>
void each(F&& fn) const;             // side effects only

bool any(auto&& pred) const;
bool all(auto&& pred) const;
bool none(auto&& pred) const;

template<typename F>
std::optional<T> find(F&& pred) const;

Array<T> sort() const;
template<typename F>
Array<T> sort_by(F&& key_fn) const;

Array<T> reverse() const;
Array<T> take(int32_t n) const;
Array<T> drop(int32_t n) const;

template<typename F>
auto flat_map(F&& fn) const;

template<typename B>
Array<std::pair<T,B>> zip(const Array<B>& other) const;

Array<std::pair<size_t, T>> enumerate() const;

template<typename F>
auto group_by(F&& key_fn) const;    // -> HashMap<Key, Array<T>>

bool contains(const T& v) const;
int32_t index_of(const T& v) const;
```

**Stdlib MLC** — обновить `lib/mlc/common/stdlib/collections/array.mlc` с новыми методами.

**Удалить** дублирующие свободные функции из `collections.hpp` (или оставить как deprecated).

**Тесты:**
```
test/mlc/array_hof_test.rb     — map, filter, fold
test/mlc/array_extra_methods_test.rb — sort_by, group_by, zip
```

---

## Фаза 4: Lambda в mlcc (self-hosted)

**Цель:** поддержка `fn x => expr` и `fn x y => expr` в mlcc

**`compiler/ast.mlc`** — добавить:

```mlc
type Param = Param { name: string, is_mut: bool, typ: Shared<TypeExpr> }

// Добавить в Expr:
| ExprLambda([string], Shared<Expr>)   // params, body
```

**`compiler/lexer.mlc`** — уже есть все токены (`KFn`, `Arrow`).

**`compiler/parser/exprs.mlc`** — добавить `parse_lambda_expr`:

```mlc
// fn x => expr
// fn x y => expr
// fn (x: i32) => expr
fn parse_lambda_expr(parser: Parser) -> ExprResult = ...
```

Вызов в `parse_primary` при встрече `KFn` без последующего имени перед `(`.

**`compiler/codegen.mlc`** — генерация:

```mlc
ExprLambda(params, body) =>
  "[=](" + gen_lambda_params(params) + ") { return " + gen_expr(body) + "; }"
```

**`compiler/checker/names.mlc`**, **`compiler/checker/infer.mlc`** — добавить случай для `ExprLambda`.

**Тесты:**
```
compiler/tests/test_parser.mlc   — парсинг lambda
compiler/tests/test_codegen.mlc  — генерация lambda
```

---

## Фаза 5: Проверка захвата в замыканиях

**Цель:** убедиться что capture mode соответствует модели в каждом контексте

**Ситуация:**

| Контекст | Текущий capture | Корректность |
|----------|----------------|--------------|
| Блок как IIFE (`[&]()...()`) | `[&]` | ✅ безопасно — вызывается немедленно |
| Lambda (`fn x => expr`), без захватов | `[]` | ✅ без внешних переменных |
| Lambda с захватами | explicit list | ✅ capture_analyzer добавляет |
| Lambda возвращается из функции | explicit value captures | нужна проверка |

**Что проверить:**

- `lib/mlc/representations/semantic/gen/services/features/capture_analyzer.rb` — убедиться что возвращаемые замыкания получают capture mode `:value`, не `:ref`
- Тест: `fn make_adder(n: i32) -> (i32 -> i32) = fn x => x + n` — `n` должен захватываться по значению

**Тесты:**
```
test/mlc/closure_capture_test.rb — returned closures, stored closures
```

---

## Фаза 6: Типы функций как first-class

**Цель:** `(i32 -> i32)` как тип параметра и возвращаемого значения

```mlc
fn apply(f: (i32 -> i32), x: i32) -> i32 = f(x)

fn make_adder(n: i32) -> (i32 -> i32) =
  fn x => x + n
```

**Изменения:**

- `lib/mlc/source/parser/type_parser.rb` — парсинг `(T -> R)` как `FunctionType`
- `lib/mlc/source/ast/nodes.rb` — `FunctionType` AST-узел
- `lib/mlc/backends/cpp/services/utils/type_mapper.rb` — `(T -> R)` → `std::function<R(T)>`
- `compiler/parser/types.mlc` — добавить парсинг function type
- `compiler/codegen.mlc` — `TyFn(params, ret)` → `std::function<R(Params)>`

**Тесты:**
```
test/mlc/lambda_test.rb  — higher-order functions
```

---

## Фаза 7: Method chaining через pipe

Pipe operator `|>` уже реализован. Задача — убедиться что работает с методами:

```mlc
users
  |> filter(fn u => u.active)    // partial application
  |> sort_by(fn u => u.name)
  |> map(fn u => u.email)
  |> join(", ")
```

Это работает автоматически через `|>` + HOF методы из Фазы 3. Отдельных изменений не требует.

---

## Фаза 8: String API

**Цель:** богатый строковый API в стиле Ruby

```mlc
"hello world".split(" ")     // ["hello", "world"]
"hello".chars()              // ["h", "e", "l", "l", "o"]
"  trim  ".trim()            // "trim"
["a", "b"].join(", ")        // "a, b"
"abc".starts_with("ab")      // true
```

Большинство методов уже есть в `mlc::String`. Задача — убедиться что они доступны в MLC через stdlib и что codegen их правильно маппирует.

**Проверить и дополнить:**

- `lib/mlc/common/stdlib/text/string.mlc` — полный список методов
- `lib/mlc/backends/cpp/services/utils/type_mapper.rb` — маппинг MLC-методов → C++ методы
- `compiler/codegen.mlc` — `map_method` для String

---

## Приоритеты

| Фаза | Приоритет | Сложность | Ценность |
|------|-----------|-----------|---------|
| 1. Биндинги | Высокий | Средняя | Чистота языка |
| 2. Array на shared_ptr | Высокий | Низкая | Безопасность |
| 3. HOF методы | Высокий | Средняя | Удобство |
| 4. Lambda в mlcc | Средний | Средняя | Самохостинг |
| 5. [=] в блоках | Высокий | Низкая | Корректность |
| 6. Типы функций | Средний | Высокая | Выразительность |
| 7. Chaining | Низкий | Низкая | Готово через pipe |
| 8. String API | Низкий | Низкая | Доп. удобство |

**Порядок реализации:** 5 → 2 → 1 → 3 → 4 → 6 → 8

Фаза 5 первая потому что это критический баг (UB при dangling reference), остальные — улучшения.
