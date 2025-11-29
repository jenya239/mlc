# MLC Feature Roadmap

**Дата**: 2025-11-28
**Версия**: 1.0
**Статус**: Активный

## Принцип приоритизации

Приоритет отдаётся фичам, которые:
1. **Органично транслируются в C++20** — прямое соответствие конструкциям
2. **Zero-cost abstractions** — без runtime overhead
3. **Нужны для целевых проектов** — ELF/C++ парсеры, компилятор, VM, GUI, 3D, backend

---

## Целевые проекты

| Проект | Описание | Ключевые требования |
|--------|----------|---------------------|
| **ELF Parser/Generator** | Парсинг и генерация ELF бинарников | Bitwise ops, byte arrays, endianness |
| **C++ Parser/Generator** | Парсинг и генерация C++ кода | Strings, recursive types, большие enum |
| **Self-hosted Compiler** | Компилятор MLC на MLC | Traits, visitor pattern, error handling |
| **Virtual Machine** | Bytecode VM | Stack ops, computed goto, FFI |
| **GUI Framework** | OpenGL-based GUI | Events, callbacks, RAII |
| **3D Engine** | 3D рендеринг | SIMD, matrices, operator overloading |
| **Enterprise Backend** | Nest-like framework | DI, decorators, async |

---

## Текущее состояние (✅ реализовано)

### Язык
- Функции, generics, lambda, closures
- Sum types (ADT) + pattern matching (guards, nested, or-patterns)
- Product types (records) + spread operator `...`
- HOF: map, filter, fold
- Pipe operator `|>`, list comprehensions
- Option/Result типы
- String interpolation `"Hello, {name}"`
- Heredoc строки, raw strings
- Async/await (базовый)
- Module system, imports/exports
- Block syntax `{ ... }`

### Инфраструктура
- C++ AST DSL (production ready)
- Zeitwerk autoloading
- TypeRegistry, FunctionRegistry
- Container/Context DI pattern
- Rule-based backend

---

## Фаза 1: Низкоуровневые примитивы

**Приоритет**: КРИТИЧЕСКИЙ
**Срок**: 2-3 сессии
**Проекты**: ELF, VM, 3D

### 1.1 Bitwise операторы

**C++ mapping**: Прямая трансляция

| MLC | C++ | Описание |
|-----|-----|----------|
| `a & b` | `a & b` | Bitwise AND |
| `a \| b` | `a \| b` | Bitwise OR |
| `a ^ b` | `a ^ b` | Bitwise XOR |
| `~a` | `~a` | Bitwise NOT |
| `a << n` | `a << n` | Left shift |
| `a >> n` | `a >> n` | Right shift |

**Задачи**:
- [ ] Lexer: добавить токены `&`, `|`, `^`, `~`, `<<`, `>>`
- [ ] Parser: приоритеты операторов (ниже сравнений, выше логических)
- [ ] SemanticIR: Binary с :bitwise_* операциями
- [ ] C++ codegen: прямая трансляция
- [ ] Тесты

### 1.2 Числовые литералы

**C++ mapping**: Прямая трансляция

```mlc
let bin = 0b1010_1100   // → 0b10101100
let hex = 0xFF_00       // → 0xFF00
let oct = 0o755         // → 0755
```

**Задачи**:
- [ ] Lexer: распознавание `0b`, `0x`, `0o` префиксов
- [ ] Lexer: поддержка `_` разделителей
- [ ] Parser: Literal с соответствующими типами
- [ ] C++ codegen: прямая трансляция литералов

### 1.3 Расширенные целочисленные типы

**C++ mapping**: Прямая трансляция через `<cstdint>`

| MLC | C++ |
|-----|-----|
| `u8` | `uint8_t` |
| `u16` | `uint16_t` |
| `u32` | `uint32_t` |
| `u64` | `uint64_t` |
| `i8` | `int8_t` |
| `i16` | `int16_t` |

**Задачи**:
- [ ] Lexer: keywords `u8`, `u16`, `u32`, `u64`, `i8`, `i16`
- [ ] TypeRegistry: регистрация новых примитивов
- [ ] TypeMapper: маппинг в `<cstdint>` типы
- [ ] Тесты конверсии и переполнения

### 1.4 Character literals

**C++ mapping**: Прямая трансляция

```mlc
let ch = 'a'      // → 'a'
let newline = '\n' // → '\n'
let tab = '\t'     // → '\t'
```

**Задачи**:
- [ ] Lexer: распознавание `'...'`
- [ ] AST: CharLiteral node
- [ ] TypeChecker: тип `char` или `u8`
- [ ] C++ codegen: прямая трансляция

---

## Фаза 2: Traits и интерфейсы

**Приоритет**: ВЫСОКИЙ
**Срок**: 4-5 сессий
**Проекты**: Компилятор, Backend, GUI

### 2.1 Базовые Traits

**C++ mapping**: C++20 Concepts + template constraints

```mlc
trait Show {
  fn show(self) -> str
}

impl Show for i32 {
  fn show(self) -> str = to_string(self)
}
```

**C++ output**:
```cpp
template<typename T>
concept Show = requires(T t) {
  { t.show() } -> std::convertible_to<mlc::String>;
};

// Specialization
template<>
inline mlc::String show(int32_t self) {
  return std::to_string(self);
}
```

**Задачи**:
- [ ] Parser: `trait` и `impl` синтаксис
- [ ] AST: TraitDecl, ImplDecl nodes
- [ ] SemanticIR: TraitDef, TraitImpl
- [ ] TypeChecker: проверка impl соответствия trait
- [ ] C++ codegen: concepts + template specialization
- [ ] Stdlib traits: Show, Eq, Ord, Clone, Default

### 2.2 Trait bounds

**C++ mapping**: C++20 Concepts в template constraints

```mlc
fn print_all<T: Show>(items: T[]) -> void =
  for item in items do
    println(item.show())
  end
```

**C++ output**:
```cpp
template<Show T>
void print_all(const std::vector<T>& items) {
  for (const auto& item : items) {
    mlc::io::println(item.show());
  }
}
```

---

## Фаза 3: Operator Overloading

**Приоритет**: ВЫСОКИЙ
**Срок**: 2-3 сессии
**Проекты**: 3D, GUI, математика

### 3.1 Арифметические операторы

**C++ mapping**: `operator+`, `operator-`, etc.

```mlc
type Vec3 = { x: f32, y: f32, z: f32 }

impl Add for Vec3 {
  fn add(self, other: Vec3) -> Vec3 =
    Vec3 { x: self.x + other.x, y: self.y + other.y, z: self.z + other.z }
}

// Использование
let v = v1 + v2
```

**C++ output**:
```cpp
struct Vec3 {
  float x, y, z;

  Vec3 operator+(const Vec3& other) const {
    return Vec3{x + other.x, y + other.y, z + other.z};
  }
};
```

**Задачи**:
- [ ] Stdlib traits: Add, Sub, Mul, Div, Neg
- [ ] TypeChecker: разрешение операторов через traits
- [ ] C++ codegen: генерация operator overloads
- [ ] Stdlib: Vec2, Vec3, Vec4, Mat4

### 3.2 Comparison операторы

**C++ mapping**: C++20 `<=>` (spaceship operator)

```mlc
impl Ord for Vec3 {
  fn cmp(self, other: Vec3) -> Ordering = ...
}
```

**C++ output**:
```cpp
auto operator<=>(const Vec3& other) const = default;
// или custom implementation
```

---

## Фаза 4: Расширенные типы данных

**Приоритет**: СРЕДНИЙ
**Срок**: 2-3 сессии
**Проекты**: Все

### 4.1 Tuple types

**C++ mapping**: `std::tuple`

```mlc
fn divide(a: i32, b: i32) -> (i32, i32) =
  (a / b, a % b)

let (quot, rem) = divide(10, 3)
```

**C++ output**:
```cpp
std::tuple<int32_t, int32_t> divide(int32_t a, int32_t b) {
  return {a / b, a % b};
}

auto [quot, rem] = divide(10, 3);  // structured bindings
```

### 4.2 Type aliases

**C++ mapping**: `using`

```mlc
type UserId = i32
type Callback = (i32) -> void
type Matrix4 = f32[4][4]
```

**C++ output**:
```cpp
using UserId = int32_t;
using Callback = std::function<void(int32_t)>;
using Matrix4 = std::array<std::array<float, 4>, 4>;
```

### 4.3 Newtype pattern

**C++ mapping**: Strong typedef через struct

```mlc
newtype UserId = i32
newtype Email = str
```

**C++ output**:
```cpp
struct UserId {
  int32_t value;
  explicit UserId(int32_t v) : value(v) {}
  explicit operator int32_t() const { return value; }
};
```

---

## Фаза 5: Null-safety и Optional chaining

**Приоритет**: СРЕДНИЙ
**Срок**: 2 сессии
**Проекты**: Backend, GUI

### 5.1 Null coalescing

**C++ mapping**: Custom operator или inline expansion

```mlc
let name = user?.name ?? "Anonymous"
```

**C++ output**:
```cpp
auto name = user.has_value()
  ? (user->name.has_value() ? *user->name : mlc::String("Anonymous"))
  : mlc::String("Anonymous");
```

### 5.2 Safe navigation

**C++ mapping**: Monadic operations на `std::optional`

```mlc
let len = user?.profile?.bio?.length() ?? 0
```

**C++ output**:
```cpp
auto len = user
  .and_then([](auto& u) { return u.profile; })
  .and_then([](auto& p) { return p.bio; })
  .transform([](auto& b) { return b.length(); })
  .value_or(0);
```

---

## Фаза 6: Destructuring

**Приоритет**: СРЕДНИЙ
**Срок**: 2 сессии
**Проекты**: Все

### 6.1 Record destructuring

**C++ mapping**: Structured bindings (C++17)

```mlc
let { x, y } = point
let { name, age: userAge } = person
```

**C++ output**:
```cpp
auto [x, y] = point;
// или для переименования:
auto x = point.x;
auto userAge = point.age;
```

### 6.2 Array destructuring

**C++ mapping**: Structured bindings + views

```mlc
let [first, second, ...rest] = items
```

**C++ output**:
```cpp
auto first = items[0];
auto second = items[1];
auto rest = std::vector(items.begin() + 2, items.end());
```

---

## Фаза 7: Константы и compile-time

**Приоритет**: СРЕДНИЙ
**Срок**: 2 сессии
**Проекты**: VM, 3D

### 7.1 Const declarations

**C++ mapping**: `constexpr`

```mlc
const PI: f64 = 3.14159265359
const MAX_SIZE: i32 = 1024
```

**C++ output**:
```cpp
constexpr double PI = 3.14159265359;
constexpr int32_t MAX_SIZE = 1024;
```

### 7.2 Compile-time evaluation

**C++ mapping**: `constexpr` functions

```mlc
const fn factorial(n: i32) -> i32 =
  if n <= 1 then 1 else n * factorial(n - 1)

const FACT_10: i32 = factorial(10)
```

**C++ output**:
```cpp
constexpr int32_t factorial(int32_t n) {
  return n <= 1 ? 1 : n * factorial(n - 1);
}
constexpr int32_t FACT_10 = factorial(10);
```

---

## Фаза 8: Advanced Patterns

**Приоритет**: НИЗКИЙ
**Срок**: 3-4 сессии
**Проекты**: Backend

### 8.1 Decorators/Annotations

**C++ mapping**: Attributes + code generation

```mlc
@route("/users")
@method("GET")
fn get_users() -> Response = ...
```

**C++ output**:
```cpp
// Metadata stored separately, code generation for routing
Response get_users() { ... }
// + registration in router
```

### 8.2 Compile-time reflection

**C++ mapping**: Template metaprogramming

```mlc
fn fields_of<T>() -> str[] = @fields(T)
```

---

## Priority Matrix

### Критический (блокирует проекты)
1. **Bitwise operators** — ELF, VM
2. **u8/u16/u32/u64 types** — ELF, VM
3. **Binary/hex literals** — ELF, VM

### Высокий (значительно упрощает)
4. **Traits** — Компилятор, Backend
5. **Operator overloading** — 3D, GUI
6. **Character literals** — C++ parser

### Средний (улучшает ergonomics)
7. **Tuple types** — Все
8. **Type aliases** — Все
9. **Null coalescing** — Backend
10. **Destructuring** — Все
11. **Const/constexpr** — VM, 3D

### Низкий (advanced features)
12. **Decorators** — Backend
13. **Reflection** — Backend
14. **Macros** — Компилятор

---

## C++ Translation Quality Criteria

Каждая фича оценивается по:

1. **Прямота трансляции** (1-5): насколько прямо MLC → C++
2. **Zero-cost** (да/нет): нет runtime overhead
3. **Читаемость вывода** (1-5): насколько понятен сгенерированный код
4. **C++ идиоматичность** (1-5): соответствует ли best practices

| Фича | Прямота | Zero-cost | Читаемость | Идиоматичность |
|------|---------|-----------|------------|----------------|
| Bitwise ops | 5 | Да | 5 | 5 |
| Int types | 5 | Да | 5 | 5 |
| Traits → Concepts | 4 | Да | 4 | 5 |
| Op overload | 5 | Да | 5 | 5 |
| Tuples | 5 | Да | 4 | 5 |
| Type aliases | 5 | Да | 5 | 5 |
| Null coalescing | 3 | Да | 3 | 4 |
| Destructuring | 4 | Да | 4 | 5 |
| Decorators | 2 | Частично | 3 | 3 |

---

## Implementation Order

```
Фаза 1.1: Bitwise ops          [2 сессии]  ████████████████████
Фаза 1.2: Numeric literals     [1 сессия]  ██████████
Фаза 1.3: Extended int types   [1 сессия]  ██████████
Фаза 1.4: Char literals        [1 сессия]  ██████████
---
Фаза 2.1: Basic traits         [3 сессии]  ██████████████████████████████
Фаза 2.2: Trait bounds         [2 сессии]  ████████████████████
---
Фаза 3.1: Arith op overload    [2 сессии]  ████████████████████
Фаза 3.2: Comparison ops       [1 сессия]  ██████████
---
Фаза 4.1: Tuples               [2 сессии]  ████████████████████
Фаза 4.2: Type aliases         [1 сессия]  ██████████
---
Остальные фазы по необходимости
```

---

## Success Metrics

- [ ] Все тесты проходят после каждой фазы
- [ ] Сгенерированный C++ компилируется без warnings
- [ ] Производительность сгенерированного кода ≥ ручного C++
- [ ] Документация обновлена
- [ ] E2E тесты для каждой фичи

---

*Последнее обновление: 2025-11-28*
