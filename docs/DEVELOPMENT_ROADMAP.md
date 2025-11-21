# MLC Compiler - Development Roadmap

**Дата создания**: 2025-11-20
**Статус проекта**: Core Features Complete
**Тесты**: 1524 runs, 4014 assertions, 0 failures, 0 errors ✅

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

**Статус**: Не начато
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

**Статус**: Не начато
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

**Статус**: Не начато
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

**Статус**: Не начато
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

**Статус**: Не начато
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

**Статус**: Частично реализовано (parsing done)
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

**Статус**: Не начато
**Сложность**: Средняя
**Время**: 2 сессии

**Задачи:**
1. Parser support для `| pattern if condition => expr`
2. SemanticIR representation
3. Lowering в C++ if-statements внутри std::visit

**Примеры:**
```mlc
match value
  | Some(x) if x > 0 => x * 2
  | Some(x) => 0
  | None => -1
```

### 32.2. Nested Patterns

**Статус**: Не начато
**Сложность**: Высокая
**Время**: 3 сессии

**Задачи:**
1. Support nested destructuring
2. Recursive pattern matching
3. Lowering в nested std::visit

**Примеры:**
```mlc
match result
  | Ok(Some(value)) => value
  | Ok(None) => 0
  | Err(msg) => -1
```

### 32.3. Or-Patterns

**Статус**: Не начато
**Сложность**: Средняя
**Время**: 2 сессии

**Задачи:**
1. Syntax для `| A | B => expr`
2. Lowering в multiple std::visit cases

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

**Статус**: Частично реализовано
**Сложность**: Средняя
**Время**: 2 сессии

**Задачи:**
1. `pub` keyword для export control
2. Private by default
3. `pub(module)` для internal visibility

**Примеры:**
```mlc
pub type Point = { x: f32, y: f32 }
fn internal_helper() -> i32 = ...  // private
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

### Immediate (Next 1-2 месяца):
1. **Phase 29.1**: Array higher-order functions
2. **Phase 29.2**: String operations
3. **Phase 30.1**: Enhanced error messages

### Short-term (Next 3-6 месяцев):
4. **Phase 30.2**: Type error recovery
5. **Phase 31.1**: Type constraints
6. **Phase 32.1**: Guard clauses

### Medium-term (Next 6-12 месяцев):
7. **Phase 32.2**: Nested patterns
8. **Phase 34.1**: Module visibility
9. **Phase 33.1-33.3**: Basic optimizations

### Long-term (12+ месяцев):
10. **Phase 31.2**: Trait system
11. **Phase 35**: Developer tools

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
- -59% requires eliminated (45 из 76)
- All tests passing
- Документация обновлена

**Следующий шаг**: Phase 29 - Standard Library Expansion
