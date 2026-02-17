# План развития MLC — ближайшие сессии

Дата: 2026-02-17
Контекст: Ruby 4.0.1, 2132+ unit tests pass, 0 failures.

## Что сделано в этой сессии

1. **Move Semantics (Tier 1)** — use-after-move detection для non-Copy типов, std::move() в codegen
2. **Tuples** — (a, b) → std::make_tuple, t.0 → std::get<0>, (i32, str) → std::tuple<int, mlc::String>
3. **Type Aliases** — type Name = Type → using Name = CppType;
4. **Docs cleanup** — убрано "Feature Complete", добавлены честные "NOT Implemented" секции
5. **Ruby 4.0.1** — обновлён через rbenv, ostruct/benchmark добавлены в Gemfile

## Сессия 1: const/constexpr + destructuring

### 1A. Const declarations

**Цель**: `const PI: f64 = 3.14` → `constexpr double PI = 3.14`

**Файлы для изменения:**

1. `lib/mlc/source/parser/lexer.rb` — добавить `const` в keywords (если ещё нет)
2. `lib/mlc/source/parser/declaration_parser.rb` — парсинг `const name: Type = expr` на уровне модуля
3. `lib/mlc/source/parser/statement_parser.rb` — парсинг `const name = expr` внутри блоков
4. `lib/mlc/source/ast/nodes.rb` — проверить, есть ли уже ConstDecl. Если нет — добавить (или использовать VariableDecl с флагом constant: true)
5. `lib/mlc/representations/semantic/gen/rules/statements/variable_decl_rule.rb` — пробросить флаг constant
6. `lib/mlc/representations/semantic/nodes/nodes.rb` — VariableDeclStmt: добавить attr_reader :constant (если нет)
7. `lib/mlc/backends/cpp/rules/statements/variable_decl_rule.rb` — если constant → генерировать `constexpr` вместо `auto`/type

**Проверка:**
```
echo 'const MAX: i32 = 100
fn main() -> i32 = MAX' | bundle exec bin/mlc --emit-cpp -
```
Ожидание: `constexpr int MAX = 100;`

**Тесты:**
- `test/mlc/const_test.rb` — unit тесты парсинга и codegen
- `test/integration/const_e2e_test.rb` — E2E: скомпилировать и запустить

### 1B. Destructuring bindings

**Цель**: `let (a, b) = expr` → `auto [a, b] = expr;` (C++17 structured bindings)

**Текущее состояние:**
- Парсер: `DestructuringDecl` AST node уже существует
- SemanticIR: `DestructuringDeclStmt` уже существует
- C++ backend: `destructuring_decl_rule.rb` уже существует в `lib/mlc/backends/cpp/rules/statements/`

**Нужно проверить:**
1. Работает ли pipeline от парсера до codegen. Попробовать:
```
echo 'fn main() -> i32 = do
  let (a, b) = (10, 20)
  a + b
end' | bundle exec bin/mlc --emit-cpp -
```
2. Если работает — написать тесты. Если нет — найти и исправить разрыв.

**Файлы для проверки:**
- `lib/mlc/backends/cpp/rules/statements/destructuring_decl_rule.rb` — прочитать
- `lib/mlc/representations/semantic/gen/rules/statements/destructuring_decl_rule.rb` — прочитать
- `test/mlc/destructuring_test.rb` — существующие тесты

**Тесты:**
- Добавить E2E тест в `test/integration/tuples_e2e_test.rb`

### 1C. Async — пометить как experimental

**Цель**: добавить ошибку компиляции при попытке lowering async функции.

**Файл:**
- `lib/mlc/representations/semantic/gen/reducers/function_reducer.rb` — в `reduce` или `build_function`, если функция async → `raise CompileError, "async functions are not yet supported (no C++ coroutine lowering)"`

**Проверка:**
```
echo 'async fn foo() -> i32 = 42' | bundle exec bin/mlc --emit-cpp -
```
Ожидание: ошибка компиляции с понятным сообщением.

---

## Сессия 2: Валидационная программа — калькулятор на MLC

### Цель
Написать работающую программу на MLC (100-200 строк), которая:
- Читает арифметическое выражение из stdin (или аргумента)
- Лексит его (числа, +, -, *, /, скобки)
- Парсит (recursive descent, AST как sum type)
- Вычисляет результат
- Выводит в stdout

### Зачем
Ни одна нетривиальная программа на MLC ещё не написана. Калькулятор — минимальная программа, которая использует:
- Sum types (AST: `type Expr = Num(i32) | Add(Shared<Expr>, Shared<Expr>) | Mul(...)`)
- Pattern matching (для eval)
- Рекурсию
- String manipulation (для лексера)
- While/for loops
- Mutable variables
- println для вывода

### Вероятные проблемы
Это самая ценная сессия — она вскроет реальные баги:
- Рекурсивные sum types через Shared<T> — может не работать
- String char access — может не быть
- Вложенный pattern matching — может быть баг
- Runtime: println для чисел — может не работать

### Порядок работы
1. Начать с простейшего: `type Expr = Num(i32) | Add(i32, i32)` + eval через match
2. Постепенно усложнять: добавить рекурсию через Shared<Expr>
3. Добавить парсер выражений
4. Каждый шаг — скомпилировать и запустить через `bin/mlc`

### Файл
`misc/examples/calculator.mlc` — программа
`test/integration/calculator_e2e_test.rb` — E2E тест

---

## Сессия 3: Исправление багов из сессии 2

Калькулятор неизбежно вскроет проблемы. Эта сессия — на исправление найденных багов.

Вероятные кандидаты:
- Рекурсивные типы
- String → char conversion
- Вложенный match
- Stdlib функции (to_string, println для i32)
- Move semantics: false positives в сложных программах

---

## Сессия 4: Мини-транслятор на MLC

### Цель
Написать транслятор подмножества MLC → C++ на самом MLC (300-500 строк).

### Подмножество
Транслируемый язык (назовём "MLC-tiny"):
- `fn name(params) -> Type = expr`
- `let name = expr`
- `if/else`
- Арифметика: `+, -, *, /`
- Литералы: i32, string
- Вызовы функций

### Структура
```
module Lexer    — tokenization (while loop, char matching)
module Parser   — recursive descent → AST (sum types)
module Codegen  — AST → C++ string (pattern matching)
module Main     — read file, pipeline, write output
```

### Зависимости
- Рабочий калькулятор из сессии 2 (доказывает что sum types + recursion + IO работают)
- File I/O (read_file / write_file)
- Возможно HashMap для keywords

---

## Приоритеты фич (если потребуются для калькулятора/транслятора)

| Фича | Нужна для | Сложность |
|------|-----------|-----------|
| String char indexing `s[i]` | Лексер | Низкая |
| `to_string(i32)` → stdlib | Вывод | Проверить |
| Recursive sum types через Shared | AST | Проверить |
| HashMap<str, T> | Keywords | Средняя, можно обойтись if-else |
| File read/write | Транслятор | Проверить stdlib |
| while с break/continue | Лексер | Уже есть |
| Mutable variable update | Парсер state | Уже есть |

---

## Команды для работы

```bash
# Быстрые тесты (по умолчанию)
bundle exec rake test_unit

# Один файл тестов
bundle exec ruby -Ilib:test test/mlc/some_test.rb

# Проверить генерацию C++
echo 'fn main() -> i32 = 42' | bundle exec bin/mlc --emit-cpp -

# Скомпилировать и запустить
echo 'fn main() -> i32 = 42' | bundle exec bin/mlc -

# Полный набор (долго, ~20 мин)
bundle exec rake test
```

## Структура проекта (ключевые файлы)

```
lib/mlc/source/parser/          — Лексер, парсер (recursive descent)
lib/mlc/source/ast/nodes.rb     — AST node определения
lib/mlc/representations/semantic/gen/
  pipeline.rb                   — SemanticIR pipeline orchestration
  services/builders/type_builder.rb — AST Type → SemanticIR Type
  services/scope/var_type_registry.rb — Типы переменных + move tracking
  rules/                        — Правила трансформации AST → SemanticIR
  visitors/expression_visitor.rb — Dispatch AST выражений в rules
lib/mlc/backends/cpp/
  codegen.rb                    — Главный lowering SemanticIR → C++ AST
  rules/expressions/            — Правила lowering выражений
  rules/statements/             — Правила lowering statements
  services/utils/type_mapper.rb — SemanticIR Type → C++ type string
lib/mlc/registries/             — TypeRegistry, FunctionRegistry
test/mlc/                       — Unit тесты
test/integration/               — E2E тесты (компиляция + запуск)
```
