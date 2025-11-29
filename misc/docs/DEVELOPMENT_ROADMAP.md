# MLC Compiler - Development Roadmap

**Дата создания**: 2025-11-20
**Обновлено**: 2025-11-28
**Статус проекта**: Feature Complete
**Тесты**: 2336 runs, 5995 assertions, 0 failures, 0 errors ✅

## Текущее состояние

### Завершённые компоненты

**Язык MLC (Core Features):**
- ✅ Функции, let-биндинги, if-expressions
- ✅ Product types (records)
- ✅ Sum types (алгебраические типы данных)
- ✅ Pattern matching с std::visit
- ✅ Generics (параметрический полиморфизм)
- ✅ Module system (18/18 тестов)
- ✅ Lambda expressions (базовые)
- ✅ Pipe operator
- ✅ For loops
- ✅ List comprehensions
- ✅ Array literals и indexing
- ✅ Array methods (length, push, pop)

**Инфраструктура:**
- ✅ C++ AST DSL (production ready, 1000+ тестов)
- ✅ Zeitwerk autoloading (Phase 28 complete, -59% requires)
- ✅ TypeRegistry (unified type system)
- ✅ Container/Context pattern for DI
- ✅ Rule-based backend architecture

---

## Phase 29: Standard Library Expansion

**Цель**: Расширение стандартной библиотеки MLC

**Приоритет**: **Высокий**

**Зависимости**: Нет

### 29.1. Array Operations (Higher-Order Functions)

**Статус**: ✅ Завершено (2025-11-23)
**Сложность**: Средняя
**Время**: 2-3 сессии

**Задачи:**
1. Реализовать `arr.map(f)` - применить функцию к каждому элементу
2. Реализовать `arr.filter(pred)` - отфильтровать элементы по предикату
3. Реализовать `arr.reduce(init, f)` - свернуть массив в значение
4. Реализовать `arr.find(pred)` - найти первый элемент, удовлетворяющий предикату
5. Реализовать `arr.all(pred)` / `arr.any(pred)` - проверки

**Технические детали:**
- Передача lambda как аргумента функции
- Type inference для generic lambda parameters
- Lowering в C++ range algorithms (std::ranges)

**Тесты:**
- Unit тесты для каждой операции
- Integration тесты с композицией операций
- Performance бенчмарки

**Примеры:**
```mlc
let numbers = [1, 2, 3, 4, 5]
let doubled = numbers.map(x => x * 2)
let evens = numbers.filter(x => x % 2 == 0)
let sum = numbers.reduce(0, (acc, x) => acc + x)
```

### 29.2. String Operations

**Статус**: ✅ Завершено (2025-11-23)
**Сложность**: Средняя
**Время**: 2-3 сессии

**Задачи:**
1. String concatenation (`str1 + str2`)
2. String interpolation (`"Hello, {name}!"`)
3. String methods: `length()`, `substring()`, `split()`, `join()`
4. String comparison операторы

**Технические детали:**
- Lowering в mlc::String runtime type
- UTF-8 support через runtime
- Efficient string building для interpolation

**Тесты:**
- Базовые операции
- Unicode handling
- Edge cases (пустые строки, большие строки)

**Примеры:**
```mlc
let greeting = "Hello, " + name + "!"
let message = "User {name} has {count} items"
let words = text.split(" ")
```

### 29.3. Option/Result Standard Types

**Статус**: ✅ Завершено (уже реализовано в stdlib)
**Сложность**: Низкая
**Время**: 1 сессия

**Задачи:**
1. Определить стандартные типы `Option<T>` и `Result<T, E>` в stdlib
2. Реализовать helper functions: `unwrap()`, `unwrap_or()`, `map()`, `and_then()`
3. Интегрировать в type system

**Технические детали:**
- Добавить в `lib/mlc/common/stdlib/`
- Auto-import в каждый модуль
- Consistent lowering в C++

**Примеры:**
```mlc
fn divide(a: i32, b: i32) -> Result<i32, str> = ...
let result = divide(10, 2).unwrap_or(0)
let mapped = result.map(x => x * 2)
```

---

## Phase 30: Error Handling & Diagnostics

**Цель**: Улучшение диагностики ошибок и user experience

**Приоритет**: **Высокий**

**Зависимости**: Phase 29 (опционально)

### 30.1. Enhanced Error Messages

**Статус**: ✅ Завершено (2025-11-23)
**Сложность**: Высокая
**Время**: 3-4 сессии

**Задачи:**
1. Source location tracking во всех AST/IR узлах
2. Colored error output с подсветкой синтаксиса
3. Helpful suggestions для типичных ошибок
4. Multi-line error context с указателями

**Технические детали:**
- Добавить `source_location: {file, line, column}` в каждый AST/IR node
- Реализовать `ErrorReporter` с форматированием
- Интегрировать в lexer, parser, semantic analyzer

**Примеры вывода:**
```
Error: Type mismatch in function call
  --> program.mlc:15:10
   |
15 |   let x = add("hello", 5)
   |           ^^^ expected i32, found str
   |
Help: The function 'add' expects two integers
```

### 30.2. Type Error Recovery

**Статус**: ✅ Завершено (2025-11-23)
**Сложность**: Высокая
**Время**: 2-3 сессии

**Задачи:**
1. Continue type checking после первой ошибки
2. Collect multiple errors перед выводом
3. Avoid cascading errors

**Технические детали:**
- Redesign type checker для error recovery
- Использовать "error type" для неизвестных типов
- Batch error reporting

---

## Phase 31: Type System Enhancements

**Цель**: Расширение системы типов

**Приоритет**: **Средний**

**Зависимости**: Phase 30 (частично)

### 31.1. Type Constraints for Generics

**Статус**: ✅ Завершено (2025-11-23)
**Сложность**: Высокая
**Время**: 3-4 сессии

**Задачи:**
1. Реализовать checking constraints в type inference
2. Lowering constraints в C++ concepts
3. Стандартные constraints: `Numeric`, `Comparable`, `Eq`

**Технические детали:**
- Constraint resolution в type checker
- Mapping MLC constraints → C++20 concepts
- Error messages для constraint violations

**Примеры:**
```mlc
fn sum<T: Numeric>(arr: Array<T>) -> T = ...
fn max<T: Comparable>(a: T, b: T) -> T = ...
```

### 31.2. Trait System (Basic)

**Статус**: Не начато
**Сложность**: Очень высокая
**Время**: 5-7 сессий

**Задачи:**
1. Trait declarations синтаксис
2. Trait implementations для types
3. Trait constraints в generics
4. Lowering в C++ concepts + template specialization

**Отложено**: Low priority, требует значительного redesign

**Примеры:**
```mlc
trait Show {
  fn show(self) -> str
}

impl Show for i32 {
  fn show(self) -> str = "{self}"
}
```

---

## Phase 32: Advanced Pattern Matching

**Цель**: Расширение возможностей pattern matching

**Приоритет**: **Средний**

**Зависимости**: Phase 30

### 32.1. Guard Clauses

**Статус**: ✅ Завершено (2025-11-23)
**Сложность**: Средняя
**Время**: 1 сессия

**Задачи:**
1. ✅ Parser support для `| pattern if condition => expr` (уже существовал)
2. ✅ SemanticIR representation (уже существовало)
3. ✅ Lowering в C++ if-else chain (добавлен `lower_match_with_guards`)

**Примеры:**
```mlc
match value
  | Some(x) if x > 0 => x * 2
  | Some(x) => 0
  | None => -1
```

### 32.2. Nested Patterns

**Статус**: ✅ Завершено (2025-11-23)
**Сложность**: Высокая
**Время**: 1 сессия

**Задачи:**
1. ✅ Parser support для nested patterns (рекурсивный parse_pattern)
2. ✅ SemanticIR обработка вложенных Pattern объектов
3. ✅ Lowering в if-else chain с nested std::holds_alternative/std::get

**Примеры:**
```mlc
match result
  | Ok(Some(value)) => value
  | Ok(None) => 0
  | Err(msg) => -1
```

### 32.3. Or-Patterns

**Статус**: ✅ Завершено (2025-11-23)
**Сложность**: Средняя
**Время**: 1 сессия

**Задачи:**
1. ✅ Parser support для `| A | B => expr` (parse_or_pattern)
2. ✅ SemanticIR transformation с kind :or
3. ✅ Lowering в if-else chain с || conditions

**Примеры:**
```mlc
match value
  | Some(1) | Some(2) => "one or two"
  | Some(x) => "other"
  | None => "none"
```

---

## Phase 33: Optimization & Performance

**Цель**: Оптимизация сгенерированного кода

**Приоритет**: **Низкий**

**Зависимости**: Phase 29-32 (опционально)

### 33.1. Dead Code Elimination

**Статус**: Не начато
**Сложность**: Средняя
**Время**: 2-3 сессии

**Задачи:**
1. Unused function detection
2. Unused type detection
3. Unused variable detection

### 33.2. Constant Folding

**Статус**: Не начато
**Сложность**: Средняя
**Время**: 2 сессии

**Задачи:**
1. Compile-time evaluation простых выражений
2. Constant propagation

### 33.3. Inline Optimization Hints

**Статус**: Не начато
**Сложность**: Низкая
**Время**: 1 сессия

**Задачи:**
1. Генерация `inline` hints для малых функций
2. Генерация `constexpr` где возможно

---

## Phase 34: Module System Improvements

**Цель**: Улучшение модульной системы

**Приоритет**: **Средний**

**Зависимости**: Phase 30

### 34.1. Module Visibility Control

**Статус**: ✅ Завершено (2025-11-24)
**Сложность**: Средняя
**Время**: 2 сессии

**Задачи:**
1. ✅ `export` keyword для export control (реализован)
2. ✅ Private by default (non-exported = private)
3. ⏳ `pub(module)` для internal visibility (отложено - advanced feature)

**Реализация:**
- `export fn`, `export type` - публичный экспорт
- Без `export` - приватный (не попадает в metadata)
- Валидация через metadata (private символы не видны при импорте)

**Примеры:**
```mlc
export type Point = { x: f32, y: f32 }  // public
fn internal_helper() -> i32 = ...        // private (implicit)
```

### 34.2. Re-exports

**Статус**: Не начато
**Сложность**: Средняя
**Время**: 2 сессии

**Задачи:**
1. `pub use Module::Type` синтаксис
2. Transitive exports

---

## Phase 35: Developer Experience

**Цель**: Инструменты для разработчиков

**Приоритет**: **Низкий**

### 35.1. Language Server Protocol (LSP)

**Статус**: Не начато
**Сложность**: Очень высокая
**Время**: 10+ сессий

**Задачи:**
1. Autocomplete
2. Go-to-definition
3. Find references
4. Diagnostics in editor

**Отложено**: Очень low priority

### 35.2. REPL

**Статус**: Не начато
**Сложность**: Высокая
**Время**: 4-5 сессий

**Задачи:**
1. Interactive expression evaluation
2. State management
3. Multi-line input

---

## Priority Matrix

### Завершено (2025-11):
- ✅ **Phase 29.1**: Array higher-order functions
- ✅ **Phase 29.2**: String operations
- ✅ **Phase 29.3**: Option/Result types
- ✅ **Phase 30.1**: Enhanced error messages
- ✅ **Phase 30.2**: Type error recovery
- ✅ **Phase 31.1**: Type constraints
- ✅ **Phase 32.1**: Guard clauses
- ✅ **Phase 32.2**: Nested patterns
- ✅ **Phase 32.3**: Or-patterns
- ✅ **Phase 34.1**: Module visibility

### Immediate (Next 1-2 месяца):
1. **Phase 33.1-33.3**: Basic optimizations (Dead code, constant folding)
2. **Phase 34.2**: Re-exports

### Long-term (Deferred):
3. **Phase 31.2**: Trait system
4. **Phase 35**: Developer tools (LSP, REPL)

---

## Success Metrics

**Для каждой phase:**
- ✅ Все тесты проходят (0 failures)
- ✅ Новая функциональность покрыта тестами (>90%)
- ✅ Документация обновлена
- ✅ Примеры работают

**Для проекта в целом:**
- Поддержка всех основных языковых фич
- Понятные сообщения об ошибках
- Production-ready stdlib
- Стабильная кодовая база

---

## Open Questions

1. **Ownership system**: Нужен ли Rust-like ownership для MLC?
2. **Async/await**: Приоритет для асинхронности?
3. **FFI**: Интеграция с C++ библиотеками?
4. **Package manager**: Нужен ли package manager для MLC?

---

## Maintenance Tasks

**Регулярные:**
- Обновление зависимостей (Ruby gems)
- Рефакторинг устаревшего кода
- Улучшение test coverage
- Документация изменений

**По мере необходимости:**
- Performance profiling
- Memory usage optimization
- Code review сгенерированного C++

---

## Завершение Phase 28

**Дата**: 2025-11-20
**Результат**: ✅ Success

**Достигнуто:**
- Zeitwerk autoloading infrastructure
- -61% requires eliminated (72 из 118)
- All tests passing
- Документация обновлена

---

## Phase 28.5: Pure Functions Extraction (2025-11-24)

**Цель**: Извлечение pure functions в отдельные модули для улучшения тестируемости

**Результат**: ✅ Success (+124 tests)

**Созданные модули:**

1. **MLC::Common::Typing::Predicates** (`lib/mlc/common/typing/predicates.rb`)
   - 7 pure functions: `type_name`, `normalized_type_name`, `describe_type`, `numeric_type?`, `error_type?`, `generic_type_name?`, `unit_like?`
   - 28 тестов

2. **MLC::Backends::Cpp::Services::Utils::ComplexityAnalysis** (`lib/mlc/backends/cpp/services/utils/complexity_analysis.rb`)
   - 11 pure functions для анализа сложности блоков и выражений
   - 27 тестов

3. **MLC::Representations::Semantic::Gen::Services::Utils::PurityAnalysis** (`lib/mlc/representations/semantic/gen/services/utils/purity_analysis.rb`)
   - 5 pure functions: `pure_expression?`, `pure_call?`, `non_literal_type?`, `pure_block?`, `pure_statement?`
   - 20 тестов

**Обновлённые сервисы (делегируют к pure modules):**
- TypeChecker → Predicates
- BlockComplexityAnalyzer → ComplexityAnalysis
- IfComplexityAnalyzer → ComplexityAnalysis
- MatchComplexityAnalyzer → ComplexityAnalysis
- PurityAnalyzer → PurityAnalysis

**Паттерн**: Сервисы остаются тонкими обёртками, pure logic вынесена в `module_function` модули

---

## Текущий статус (2025-11-28)

**Тесты**: 2336 runs, 5995 assertions, 0 failures, 0 errors ✅

### Test Commands

```bash
rake test_unit      # Fast (~50 sec) - use by default
rake test_fast      # Without E2E (~2-3 min)
rake test           # Full suite (~20 min)
rake test_e2e       # E2E only (compile + run)
```

### Recent Features (2025-11-28)

- ✅ **Spread operator**: `Point { ...base, z: 3 }`
- ✅ **Block syntax**: `fn foo() -> i32 = { let x = 1; x }`
- ✅ **Test infrastructure**: Fast/full test separation

**Следующий шаг**: Phase 36 - Low-level Primitives (см. FEATURE_ROADMAP.md)

---

## Новые фазы (2025-11-28)

> Полный roadmap с детализацией: [FEATURE_ROADMAP.md](./FEATURE_ROADMAP.md)

### Phase 36: Low-level Primitives (КРИТИЧЕСКИЙ)

**Цель**: Фичи для ELF parser, VM, бинарной работы
**C++ mapping**: Прямая трансляция

| Фича | MLC | C++ | Статус |
|------|-----|-----|--------|
| Bitwise AND | `a & b` | `a & b` | ❌ |
| Bitwise OR | `a \| b` | `a \| b` | ❌ |
| Bitwise XOR | `a ^ b` | `a ^ b` | ❌ |
| Bitwise NOT | `~a` | `~a` | ❌ |
| Left shift | `a << n` | `a << n` | ❌ |
| Right shift | `a >> n` | `a >> n` | ❌ |
| Binary literals | `0b1010` | `0b1010` | ❌ |
| Hex literals | `0xFF` | `0xFF` | ✅ (проверить) |
| u8/u16/u32/u64 | `u8`, `u16`... | `uint8_t`... | ❌ |
| Char literals | `'a'` | `'a'` | ❌ |

### Phase 37: Traits & Concepts (ВЫСОКИЙ)

**Цель**: OOP-like абстракции для компилятора, backend
**C++ mapping**: C++20 Concepts

```mlc
trait Show {
  fn show(self) -> str
}

impl Show for i32 {
  fn show(self) -> str = to_string(self)
}

fn print_all<T: Show>(items: T[]) -> void
```

### Phase 38: Operator Overloading (ВЫСОКИЙ)

**Цель**: Математические типы для 3D, GUI
**C++ mapping**: operator+, operator-, etc.

```mlc
impl Add for Vec3 {
  fn add(self, other: Vec3) -> Vec3 = ...
}

let v = v1 + v2  // uses Add trait
```

### Phase 39: Type System Extensions (СРЕДНИЙ)

- Tuple types `(i32, str)` → `std::tuple`
- Type aliases `type UserId = i32` → `using`
- Newtype pattern

### Phase 40: Null Safety (СРЕДНИЙ)

- Null coalescing `??`
- Safe navigation `?.`

---

## Целевые проекты

| Проект | Критические фичи |
|--------|------------------|
| ELF Parser | Bitwise ops, u8[], endianness |
| C++ Parser | Strings, char literals, recursive types |
| Self-hosted Compiler | Traits, visitor pattern |
| Virtual Machine | Stack ops, computed goto, FFI |
| GUI Framework | Events, callbacks, RAII |
| 3D Engine | SIMD, matrices, operator overloading |
| Enterprise Backend | DI, decorators, async |

---

## Приоритеты (C++ органичность)

Все фичи оцениваются по критерию: насколько прямо они транслируются в C++20.

| Приоритет | Фичи | C++ mapping quality |
|-----------|------|---------------------|
| 🔴 Критический | Bitwise, int types, literals | 5/5 прямая трансляция |
| 🟠 Высокий | Traits, operators | 4/5 через concepts |
| 🟡 Средний | Tuples, aliases, null-safety | 4/5 |
| 🟢 Низкий | Decorators, reflection | 2-3/5 |
