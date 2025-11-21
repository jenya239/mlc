# MLC - Полная Техническая Спецификация

## Оглавление

1. [Обзор проекта](#1-обзор-проекта)
2. [Структура проекта](#2-структура-проекта)
3. [Архитектура компилятора](#3-архитектура-компилятора)
4. [Язык MLC - синтаксис и семантика](#4-язык-mlc---синтаксис-и-семантика)
5. [Система типов](#5-система-типов)
6. [Парсер](#6-парсер)
7. [Семантический анализ](#7-семантический-анализ)
8. [C++ бэкенд](#8-c-бэкенд)
9. [Runtime библиотека](#9-runtime-библиотека)
10. [Стандартная библиотека](#10-стандартная-библиотека)
11. [C++ AST DSL](#11-c-ast-dsl)
12. [Модульная система](#12-модульная-система)
13. [CLI интерфейс](#13-cli-интерфейс)
14. [Тестирование](#14-тестирование)

---

## 1. Обзор проекта

### 1.1 Назначение

MLC - экспериментальный компилятор, преобразующий исходный код на статически-типизированном функциональном языке высокого уровня в исходный код на C++20.

### 1.2 Цели проекта

- Исследование возможностей AI-assisted разработки компиляторов
- Образовательные задачи по архитектуре компиляторов
- Эксперименты с дизайном языков программирования

### 1.3 Технологический стек

| Компонент | Технология |
|-----------|------------|
| Язык реализации | Ruby 3.x |
| Целевой язык | C++20 |
| Автозагрузка | Zeitwerk |
| Тестирование | Minitest |
| JSON parsing | nlohmann/json |
| Графика | XCB + Cairo |

### 1.4 Текущий статус

- Версия: 2.0.0
- Тесты: 1524 runs, 4014 assertions, 0 failures
- Фаза разработки: Phase 28 завершена (Zeitwerk refactoring)

---

## 2. Структура проекта

```
mlc/
├── bin/                    # Исполняемые файлы
│   └── mlc                 # CLI точка входа
├── lib/                    # Исходный код
│   ├── cpp_ast/            # C++ AST DSL библиотека
│   │   ├── builder/        # DSL генераторы
│   │   ├── lexer/          # C++ лексер
│   │   ├── nodes/          # AST узлы C++
│   │   └── parsers/        # C++ парсеры
│   └── mlc/                # Компилятор MLC
│       ├── backends/       # Бэкенды генерации кода
│       │   └── cpp/        # C++ бэкенд
│       ├── common/         # Общие компоненты
│       │   ├── analysis/   # Passes анализа
│       │   ├── diagnostics/# Диагностика
│       │   ├── stdlib/     # Стандартная библиотека
│       │   └── typing/     # Типизация
│       ├── registries/     # Реестры типов/функций
│       ├── representations/# IR представления
│       │   └── semantic/   # SemanticIR
│       └── source/         # Frontend
│           ├── ast/        # Source AST
│           └── parser/     # Парсер
├── runtime/                # C++ Runtime
│   ├── include/mlc/        # Заголовочные файлы
│   │   ├── core/           # Core (string, collections, match)
│   │   ├── io/             # I/O (file, buffer)
│   │   ├── json/           # JSON поддержка
│   │   ├── math/           # Математика
│   │   ├── text/           # Текст (regex)
│   │   └── graphics/       # Графика (XCB + Cairo)
│   └── src/                # Реализации
├── test/                   # Тесты
│   ├── builder/            # Тесты C++ AST DSL
│   ├── mlc/                # Тесты компилятора
│   ├── integration/        # Интеграционные тесты
│   └── performance/        # Бенчмарки
├── misc/                   # Примеры и утилиты
│   ├── examples/           # Примеры программ
│   └── tools/              # Инструменты
├── docs/                   # Документация
└── scripts/                # Вспомогательные скрипты
```

---

## 3. Архитектура компилятора

### 3.1 Конвейер компиляции

```
MLC Source Code
       ↓
[1] ЛЕКСИЧЕСКИЙ АНАЛИЗ (Lexer)
       ↓ Token Stream
[2] СИНТАКСИЧЕСКИЙ АНАЛИЗ (Parser)
       ↓ AST (Abstract Syntax Tree)
[3] СЕМАНТИЧЕСКИЙ АНАЛИЗ (Semantic IR Pipeline)
       ↓ SemanticIR (Type-checked)
[4] ГЕНЕРАЦИЯ C++ AST
       ↓ C++ AST
[5] ГЕНЕРАЦИЯ C++ КОДА
       ↓ C++20 Source Code
[6] КОМПИЛЯЦИЯ (g++/clang++)
       ↓ Binary Executable
```

### 3.2 Точка входа

**Файл:** `lib/mlc/common/compiler_api.rb`

```ruby
module MLC
  class << self
    def compile(source, filename: nil, runtime_policy: nil)
      # 1. Парсинг → AST
      # 2. Трансформация → SemanticIR
      # 3. Lowering → C++ AST
    end

    def to_cpp(source, filename: nil, runtime_policy: nil)
      # Полный pipeline с генерацией C++ текста
    end

    def to_hpp_cpp(source, filename: nil, runtime_policy: nil)
      # Генерация header + implementation + metadata
    end
  end
end
```

### 3.3 Архитектурные паттерны

1. **Visitor Pattern** - обход AST и SemanticIR
2. **Strategy Pattern** - выбор стратегии генерации кода
3. **Builder Pattern** - построение C++ AST через fluent API
4. **Container Pattern** - dependency injection
5. **Chain of Responsibility** - Rule-based трансформации

---

## 4. Язык MLC - синтаксис и семантика

### 4.1 Объявление функций

```mlc
// Простая функция
fn add(a: i32, b: i32) -> i32 = a + b

// Функция с блоком
fn complex(x: i32) -> i32 = do
  let y = x * 2
  let z = y + 1
  z
end

// Generic функция
fn identity<T>(x: T) -> T = x

// Функция с несколькими generic параметрами
fn first<T>(a: T, b: T) -> T = a
```

### 4.2 Объявление типов

```mlc
// Record type (структура)
type Point = { x: f32, y: f32 }
type Person = { name: str, age: i32 }

// Sum type (алгебраический тип)
type Option<T> = Some(T) | None
type Result<T, E> = Ok(T) | Err(E)
type Shape = Circle(i32) | Rectangle(i32, i32) | Square(i32)

// Тип массива
type Numbers = i32[]
type Matrix = f32[][]
```

### 4.3 Литералы

```mlc
// Целочисленные
42, 0, -100, 1_000_000

// С плавающей точкой
3.14, 0.0, -2.5

// Строковые
"hello", "world", ""

// Булевы
true, false

// Массивы
[1, 2, 3, 4, 5]
["a", "b", "c"]

// Records
Point { x: 1.0, y: 2.0 }
Person { name: "Alice", age: 30 }

// Regex
r"pattern"
r"[a-z]+" as captures
```

### 4.4 Операторы

```mlc
// Арифметические
a + b, a - b, a * b, a / b, a % b

// Сравнения
a == b, a != b, a < b, a > b, a <= b, a >= b

// Логические
a && b, a || b, !a

// Унарные
-x, !flag

// Доступ к полям
point.x, person.name

// Индексация
arr[0], matrix[i][j]

// Pipe оператор
value |> transform |> process
```

### 4.5 Управление потоком

```mlc
// If-else выражение
let result = if x > 0 then "positive" else "non-positive"

// Цепочка if-else
let grade = if score >= 90 then "A"
           else if score >= 80 then "B"
           else if score >= 70 then "C"
           else "F"

// For цикл
for item in items do
  println(item)
end

// While цикл
while count < 10 do
  count = count + 1
end

// Pattern matching
match shape {
  Circle(r) => r * r * 3,
  Rectangle(w, h) => w * h,
  Square(s) => s * s
}
```

### 4.6 Let биндинги и присваивание

```mlc
// Immutable
let x = 10
let name = "Alice"

// Mutable
let mut counter = 0
counter = counter + 1

// С типом
let numbers: i32[] = [1, 2, 3]
```

### 4.7 Lambda выражения

```mlc
// Однопараметровые
x => x * 2
item => item.length()

// Многопараметровые
(a, b) => a + b
(acc, x) => acc + x

// С типами параметров
(line: str) => parse(line)
(acc: Totals, status: str) => bump(acc, status)
```

### 4.8 List comprehensions

```mlc
// Простой маппинг
[x * 2 for x in numbers]

// С фильтрацией
[x for x in numbers if x > 0]

// Комбинация
[x * x for x in range(1, 10) if x % 2 == 0]
```

### 4.9 Методы массивов (HOF)

```mlc
// Map - преобразование элементов
numbers.map(x => x * 2)

// Filter - фильтрация
numbers.filter(x => x % 2 == 0)

// Fold - свёртка
numbers.fold(0, (acc, x) => acc + x)

// Цепочка
numbers
  .filter(x => x > 0)
  .map(x => x * 2)
  .fold(0, (acc, x) => acc + x)
```

### 4.10 Методы строк

```mlc
text.upper()              // Верхний регистр
text.lower()              // Нижний регистр
text.trim()               // Удаление пробелов
text.split(",")           // Разбиение
text.contains("word")     // Поиск
text.starts_with("prefix")// Проверка префикса
text.ends_with("suffix")  // Проверка суффикса
text.length()             // Длина
text.is_empty()           // Проверка пустоты
```

---

## 5. Система типов

### 5.1 Примитивные типы

| Тип | Описание | C++ эквивалент |
|-----|----------|----------------|
| `i32` | 32-bit signed integer | `int32_t` |
| `i64` | 64-bit signed integer | `int64_t` |
| `f32` | 32-bit float | `float` |
| `f64` | 64-bit float | `double` |
| `bool` | Boolean | `bool` |
| `str` | String | `mlc::String` |
| `void` | Void | `void` |

### 5.2 Составные типы

```mlc
// Array types
i32[]           // Массив целых
str[]           // Массив строк
T[]             // Generic массив

// Function types
(i32, i32) -> i32      // Функция двух аргументов
(T) -> T               // Generic функция
() -> void             // Функция без аргументов и результата

// Record types
{ x: f32, y: f32 }     // Анонимный record
Point                   // Именованный record

// Sum types
Option<T>              // Optional значение
Result<T, E>           // Результат с ошибкой
```

### 5.3 Параметрический полиморфизм

```mlc
// Generic функция
fn map<T, U>(arr: T[], f: (T) -> U) -> U[]

// Generic тип
type Pair<A, B> = { first: A, second: B }

// Вложенные generics
fn flatten<T>(arr: T[][]) -> T[]
```

### 5.4 Type Inference

Компилятор выводит типы локальных переменных:

```mlc
let x = 42              // i32
let y = 3.14            // f32
let s = "hello"         // str
let arr = [1, 2, 3]     // i32[]
let doubled = arr.map(x => x * 2)  // i32[]
```

### 5.5 Маппинг типов в C++

| MLC тип | C++ тип |
|---------|---------|
| `i32` | `int32_t` |
| `i64` | `int64_t` |
| `f32` | `float` |
| `f64` | `double` |
| `bool` | `bool` |
| `str` | `mlc::String` |
| `void` | `void` |
| `T[]` | `std::vector<T>` |
| Sum type | `std::variant<...>` |
| Record | `struct { ... }` |
| Lambda | `std::function<...>` |
| Generic | C++20 template |

---

## 6. Парсер

### 6.1 Структура

**Директория:** `lib/mlc/source/parser/`

```
parser/
├── parser.rb              # Главный класс Parser
├── lexer.rb               # Токенизатор
├── base_parser.rb         # Базовые утилиты
├── expression_parser.rb   # Парсинг выражений
├── statement_parser.rb    # Парсинг операторов
├── declaration_parser.rb  # Парсинг объявлений
├── type_parser.rb         # Парсинг типов
├── pattern_parser.rb      # Парсинг паттернов
├── optimized_parser.rb    # Оптимизации
└── error_recovery_parser.rb # Error recovery
```

### 6.2 Лексер

**Файл:** `lib/mlc/source/parser/lexer.rb`

**Ключевые слова:**
```
fn, type, let, mut, return, break, continue,
if, then, else, while, for, in, do, end,
match, module, export, import, enum, extern,
true, false, i32, i64, f32, f64, bool, str, void
```

**Операторы:**
```
+, -, *, /, %, ==, !=, <, >, <=, >=, &&, ||, !,
=, =>, ->, |>, |, &, ::, ., ,, :, ;, (, ), {, }, [, ]
```

**Token структура:**
```ruby
Token = Struct.new(:type, :value, :line, :column, :file, :end_line, :end_column)
```

### 6.3 AST узлы

**Файл:** `lib/mlc/source/ast/nodes.rb`

**Иерархия:**
```
Node (base)
├── Program
├── ModuleDecl
├── ImportDecl
├── TypeDecl
├── FunctionDecl
├── VarDecl
├── ExternDecl
├── Literal
├── VarRef
├── Binary
├── Unary
├── Call
├── Index
├── Member
├── ArrayLiteral
├── RecordLiteral
├── IfExpression
├── Block
├── Lambda
├── Match
├── ForLoop
├── WhileLoop
├── DoBlock
├── ListComprehension
├── Assignment
├── Return
├── Break
└── Continue
```

### 6.4 Source Origin

Каждый AST узел содержит информацию о позиции в исходном коде:

```ruby
SourceOrigin = Struct.new(
  :file,
  :line,
  :column,
  :end_line,
  :end_column,
  :line_text
)
```

---

## 7. Семантический анализ

### 7.1 Структура

**Директория:** `lib/mlc/representations/semantic/gen/`

```
gen/
├── pipeline.rb           # Главный pipeline
├── engine.rb             # Оркестратор
├── services/             # Сервисы
│   ├── container.rb      # DI контейнер
│   ├── builders/         # Построители IR
│   ├── checkers/         # Проверки типов
│   ├── features/         # Обработка конструкций
│   ├── imports/          # Разрешение импортов
│   ├── inference/        # Type inference
│   ├── registration/     # Регистрация символов
│   ├── scope/            # Управление scope
│   └── utils/            # Утилиты
├── rules/                # Правила трансформации
│   ├── expressions/      # Expression rules
│   └── statements/       # Statement rules
├── reducers/             # Редукторы
└── passes/               # Passes анализа
```

### 7.2 SemanticIR узлы

**Файл:** `lib/mlc/representations/semantic/nodes/nodes.rb`

**Типы:**
```ruby
Type              # Базовый тип
├── kind          # :prim, :record, :func, :unit, :opaque, :generic, :type_var, :array, :sum

RecordType        # { name, fields: [{name, type}] }
SumType           # { name, variants: [{name, fields}] }
FunctionType      # { params, ret_type }
GenericType       # { base_type, type_args }
ArrayType         # { element_type }
TypeVariable      # { name, bounds }
OpaqueType        # { name }
```

**Выражения:**
```ruby
Expr              # Базовое выражение
├── Literal       # Скалярные значения
├── VarRef        # Ссылка на переменную
├── Binary        # Бинарные операции
├── Unary         # Унарные операции
├── Call          # Вызов функции
├── Lambda        # Lambda выражение
├── Match         # Pattern matching
├── If            # Условное выражение
├── Block         # Блок кода
├── RecordConstruction  # Создание record
├── ArrayConstruction   # Создание массива
└── IndexAccess   # Индексный доступ
```

### 7.3 Type Inference Service

**Файл:** `lib/mlc/representations/semantic/gen/services/inference/type_inference_service.rb`

```ruby
class TypeInferenceService
  def infer_variable_type(name)      # Тип переменной/функции
  def infer_call(callee, args)       # Тип вызова (с generic resolution)
  def infer_binary(op, left, right)  # Тип бинарной операции
  def infer_unary(op, operand)       # Тип унарной операции
end
```

### 7.4 Реестры

**TypeRegistry** (`lib/mlc/registries/type_registry.rb`):
```ruby
class TypeRegistry
  def register(name, type_info)
  def fetch(name)
  def has_type?(name)
  def cpp_name(name)
  def qualified_name(name)
end

TypeInfo = Struct.new(
  :name, :ast_node, :core_ir_type, :cpp_name,
  :namespace, :kind, :exported, :module_name
)
```

**FunctionRegistry** (`lib/mlc/registries/function_registry.rb`):
```ruby
class FunctionRegistry
  def register(name, entry)
  def fetch(name)
  def exported
  def by_namespace(ns)
  def by_module(mod)
end

Entry = Struct.new(
  :name, :info, :canonical_name, :namespace,
  :module_name, :exported, :external, :ast_node,
  :origin, :effects, :aliases
)
```

---

## 8. C++ бэкенд

### 8.1 Структура

**Директория:** `lib/mlc/backends/cpp/`

```
cpp/
├── codegen.rb            # Главный генератор
├── bootstrap.rb          # Инициализация архитектуры
├── container.rb          # DI контейнер
├── context.rb            # High-level API
├── rules/                # Правила преобразования
│   ├── base_rule.rb      # Базовый класс
│   ├── expression_rule.rb
│   ├── statement_rule.rb
│   ├── function_rule.rb
│   ├── expressions/      # 17+ expression rules
│   └── statements/       # 10+ statement rules
├── services/             # Сервисы
│   ├── rule_engine.rb
│   ├── builders/
│   ├── helpers/
│   ├── policies/
│   └── utils/
├── block_complexity_analyzer.rb
├── header_generator.rb
└── metadata_generator.rb
```

### 8.2 Bootstrap процесс

**Файл:** `lib/mlc/backends/cpp/bootstrap.rb`

```ruby
def create_backend(type_registry:, function_registry:, ...)
  container = Container.new(...)
  context = Context.new(container)
  rule_engine = RuleEngine.new(context)

  register_expression_rules(rule_engine)
  register_statement_rules(rule_engine)

  { container:, context:, rule_engine: }
end
```

### 8.3 Expression Rules

| Rule | Входной узел | Выходной C++ |
|------|--------------|--------------|
| `LiteralRule` | `Literal` | Литералы C++ |
| `VarRefRule` | `VarRef` | Идентификаторы |
| `BinaryRule` | `Binary` | Бинарные операции |
| `UnaryRule` | `Unary` | Унарные операции |
| `CallRule` | `Call` | Вызовы функций |
| `IndexRule` | `Index` | Индексация |
| `MemberRule` | `Member` | Доступ к членам |
| `ArrayLiteralRule` | `ArrayLiteral` | `std::vector` |
| `RecordRule` | `RecordLiteral` | Struct initialization |
| `LambdaRule` | `Lambda` | `std::function` |
| `IfRule` | `If` | Ternary / if |
| `BlockRule` | `Block` | Compound statement |
| `MatchRule` | `Match` | `std::visit` |
| `ListCompRule` | `ListComprehension` | For loop |
| `RegexRule` | Regex literal | `mlc::Regex` |

### 8.4 Преобразование типов

**Файл:** `lib/mlc/backends/cpp/services/utils/type_mapper.rb`

```ruby
module TypeMapper
  def map_type(type, type_map, type_registry)
    case type
    when TypeVariable    -> type.name
    when GenericType     -> "#{base}<#{args.join(', ')}>"
    when ArrayType       -> "std::vector<#{element_type}>"
    when FunctionType    -> "std::function<#{ret}(#{params})>"
    when OpaqueType      -> type.name (as-is)
    when RecordType      -> struct name
    when SumType         -> "std::variant<...>"
    else                 -> primitive lookup
    end
  end
end
```

### 8.5 Pattern Matching → std::visit

MLC:
```mlc
match shape {
  Circle(r) => r * r * 3,
  Rectangle(w, h) => w * h,
  Square(s) => s * s
}
```

C++:
```cpp
std::visit(overloaded{
  [](const Circle& __v) {
    auto r = __v.field0;
    return r * r * 3;
  },
  [](const Rectangle& __v) {
    auto w = __v.field0;
    auto h = __v.field1;
    return w * h;
  },
  [](const Square& __v) {
    auto s = __v.field0;
    return s * s;
  }
}, shape);
```

---

## 9. Runtime библиотека

### 9.1 Структура

**Директория:** `runtime/include/mlc/`

```
mlc/
├── mlc.hpp               # Main include
├── core/
│   ├── string.hpp        # String class
│   ├── collections.hpp   # Vector operations
│   ├── match.hpp         # Pattern matching helper
│   └── types.hpp         # Type definitions
├── io/
│   ├── io.hpp            # Console I/O
│   ├── file.hpp          # File operations
│   └── buffer.hpp        # Binary buffer
├── math/
│   └── math.hpp          # Math functions
├── text/
│   └── regex.hpp         # Regex support
├── json/
│   └── json.hpp          # JSON support
└── graphics/
    └── graphics.hpp      # XCB + Cairo graphics
```

### 9.2 Core модули

**String (`core/string.hpp`):**
```cpp
class String {
public:
  // Конструкторы
  String();
  String(const char* s);
  String(const std::string& s);

  // Методы
  size_t length() const;
  bool is_empty() const;
  String upper() const;
  String lower() const;
  String trim() const;
  bool contains(const String& s) const;
  bool starts_with(const String& prefix) const;
  bool ends_with(const String& suffix) const;
  std::vector<String> split(const String& delim) const;

  // Операторы
  String operator+(const String& other) const;
  bool operator==(const String& other) const;
};

// Конверсия
String to_string(int32_t value);
String to_string(float value);
String to_string(bool value);
int32_t parse_i32(const String& s);
float parse_f32(const String& s);
```

**Collections (`core/collections.hpp`):**
```cpp
// HOF
template<typename T, typename F>
auto map(const std::vector<T>& vec, F f);

template<typename T, typename F>
std::vector<T> filter(const std::vector<T>& vec, F pred);

template<typename T, typename R, typename F>
R fold(const std::vector<T>& vec, R init, F f);

// Специализированные функции
std::vector<int32_t> reverse_i32(const std::vector<int32_t>& vec);
int32_t sum_i32(const std::vector<int32_t>& vec);
std::vector<int32_t> range(int32_t start, int32_t end);
```

**Match helper (`core/match.hpp`):**
```cpp
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;
```

### 9.3 I/O модули

**Console I/O (`io/io.hpp`):**
```cpp
void print(const String& s);
void println(const String& s);
void eprint(const String& s);
void eprintln(const String& s);
String read_line();
String read_all();
std::vector<String> args();
```

**File I/O (`io/file.hpp`):**
```cpp
class File {
public:
  File(const std::string& path);
  void open_read();
  void open_write();
  String read_all();
  void write(const String& content);
  void close();
};

// Удобные функции
String read_to_string(const String& path);
void write_string(const String& path, const String& content);
bool exists(const String& path);
```

**Binary Buffer (`io/buffer.hpp`):**
```cpp
class Buffer {
  size_t size() const;
  void append(const uint8_t* data, size_t len);
  Bytes to_bytes() const;
};

class BinaryReader {
  int32_t read_i32();
  float read_f32();
  String read_string(size_t len);
};

class BinaryWriter {
  void write_i32(int32_t value);
  void write_f32(float value);
  void write_string(const String& s);
};
```

### 9.4 JSON модуль

**Файл:** `json/json.hpp`

```cpp
using JsonValue = std::variant<
  std::nullptr_t,
  bool,
  double,
  mlc::String,
  std::vector<JsonValue>,
  nlohmann::json
>;

JsonValue parse_json(const String& s);
String stringify_json(const JsonValue& v);
String stringify_json_pretty(const JsonValue& v);

JsonValue json_get(const JsonValue& obj, const String& key);
void json_set(JsonValue& obj, const String& key, const JsonValue& value);
bool json_has_key(const JsonValue& obj, const String& key);
```

### 9.5 Graphics модуль

**Файл:** `graphics/graphics.hpp`

```cpp
struct Color {
  double r, g, b, a;
};

struct Window {
  xcb_connection_t* connection;
  xcb_window_t window;
  int32_t width, height;
};

struct DrawContext {
  cairo_surface_t* surface;
  cairo_t* cr;
};

enum class EventType {
  None, Expose, KeyPress, ButtonPress,
  ButtonRelease, MotionNotify, Quit
};

struct Event {
  EventType type;
  int32_t x, y, button, keycode;
};

// Управление окном
Window* create_window(int32_t w, int32_t h, const String& title);
DrawContext* create_draw_context(Window* win);
Event poll_event(Window* win);
void flush_window(Window* win);

// Рисование
void clear(DrawContext* ctx, const Color& c);
void set_color(DrawContext* ctx, const Color& c);
void draw_rect(DrawContext* ctx, int32_t x, int32_t y, int32_t w, int32_t h);
void draw_circle(DrawContext* ctx, int32_t x, int32_t y, int32_t r);
void draw_line(DrawContext* ctx, int32_t x1, int32_t y1, int32_t x2, int32_t y2);
void draw_text(DrawContext* ctx, int32_t x, int32_t y, const String& text);

// Утилиты
Color rgb(double r, double g, double b);
Color rgba(double r, double g, double b, double a);
void sleep_ms(int32_t ms);
```

---

## 10. Стандартная библиотека

### 10.1 Структура

**Директория:** `lib/mlc/common/stdlib/`

```
stdlib/
├── registry.rb           # Реестр модулей
├── scanner.rb            # Сканер метаданных
├── resolver.rb           # Разрешение путей
├── signature_registry.rb # Кэш сигнатур
├── metadata_loader.rb    # Загрузчик .mlcmeta
├── core/
│   ├── option.mlc        # Option<T>
│   └── result.mlc        # Result<T, E>
├── collections/
│   └── array.mlc         # Array операции
├── text/
│   ├── string.mlc        # String методы
│   └── conv.mlc          # Конверсия типов
├── io/
│   ├── io.mlc            # Console I/O
│   └── file.mlc          # File I/O
├── math/
│   └── math.mlc          # Математика
├── data/
│   └── json.mlc          # JSON
└── graphics/
    └── graphics.mlc      # Graphics
```

### 10.2 Core модули

**Option (`core/option.mlc`):**
```mlc
type Option<T> = Some(T) | None

fn is_some<T>(opt: Option<T>) -> bool
fn is_none<T>(opt: Option<T>) -> bool
fn unwrap<T>(opt: Option<T>) -> T
fn unwrap_or<T>(opt: Option<T>, default: T) -> T
fn map<T, U>(opt: Option<T>, f: (T) -> U) -> Option<U>
fn and_then<T, U>(opt: Option<T>, f: (T) -> Option<U>) -> Option<U>
```

**Result (`core/result.mlc`):**
```mlc
type Result<T, E> = Ok(T) | Err(E)

fn is_ok<T, E>(res: Result<T, E>) -> bool
fn is_err<T, E>(res: Result<T, E>) -> bool
fn unwrap<T, E>(res: Result<T, E>) -> T
fn unwrap_or<T, E>(res: Result<T, E>, default: T) -> T
fn unwrap_err<T, E>(res: Result<T, E>) -> E
fn map<T, U, E>(res: Result<T, E>, f: (T) -> U) -> Result<U, E>
fn map_err<T, E, F>(res: Result<T, E>, f: (E) -> F) -> Result<T, F>
```

### 10.3 Collections

**Array (`collections/array.mlc`):**
```mlc
// Информация
fn length<T>(arr: T[]) -> i32
fn is_empty<T>(arr: T[]) -> bool
fn first<T>(arr: T[]) -> T
fn last<T>(arr: T[]) -> T

// Поиск (специализированные)
extern fn contains_i32(arr: i32[], value: i32) -> bool
extern fn contains_str(arr: str[], value: str) -> bool

// Трансформации (специализированные)
extern fn reverse_i32(arr: i32[]) -> i32[]
extern fn take_i32(arr: i32[], n: i32) -> i32[]
extern fn drop_i32(arr: i32[], n: i32) -> i32[]
extern fn slice_i32(arr: i32[], start: i32, end: i32) -> i32[]

// Агрегация
extern fn sum_i32(arr: i32[]) -> i32
extern fn min_i32(arr: i32[]) -> i32
extern fn max_i32(arr: i32[]) -> i32

// Генерация
extern fn range(start: i32, end: i32) -> i32[]

// Строки
extern fn join_strings(arr: str[], sep: str) -> str
```

### 10.4 Text модули

**String (`text/string.mlc`):**
```mlc
fn upper(s: str) -> str
fn lower(s: str) -> str
fn trim(s: str) -> str
fn split(s: str, delim: str) -> str[]
fn contains(s: str, sub: str) -> bool
fn starts_with(s: str, prefix: str) -> bool
fn ends_with(s: str, suffix: str) -> bool
fn length(s: str) -> i32
fn is_empty(s: str) -> bool

// Утилиты
fn lines(s: str) -> str[] = s.split("\n")
fn words(s: str) -> str[] = s.split(" ")
fn join(arr: str[], sep: str) -> str
fn repeat(s: str, n: i32) -> str
```

**Conversion (`text/conv.mlc`):**
```mlc
// Конверсия в строку
extern fn to_string_i32(value: i32) -> str
extern fn to_string_f32(value: f32) -> str
extern fn to_string_bool(value: bool) -> str

// Парсинг из строки
extern fn parse_i32(s: str) -> i32
extern fn parse_f32(s: str) -> f32
extern fn parse_bool(s: str) -> bool

// Алиасы
fn int_to_str(v: i32) -> str = to_string_i32(v)
fn str_to_int(s: str) -> i32 = parse_i32(s)
```

### 10.5 I/O модули

**Console (`io/io.mlc`):**
```mlc
extern fn print(s: str) -> void
extern fn println(s: str) -> void
extern fn eprint(s: str) -> void
extern fn eprintln(s: str) -> void
extern fn read_line() -> str
extern fn read_all() -> str
extern fn exit(code: i32) -> void

// Утилиты
fn panic(msg: str) -> void = do
  eprintln("PANIC: " + msg)
  exit(1)
end

fn debug_print(msg: str) -> void = println("[DEBUG] " + msg)
fn error_print(msg: str) -> void = eprintln("[ERROR] " + msg)
```

**File (`io/file.mlc`):**
```mlc
// Чтение
extern fn read_to_string(path: str) -> str
extern fn read_lines(path: str) -> str[]

// Запись
extern fn write_string(path: str, content: str) -> void
extern fn write_lines(path: str, lines: str[]) -> void
extern fn append_string(path: str, content: str) -> void
extern fn append_line(path: str, line: str) -> void

// Файловая система
extern fn exists(path: str) -> bool
extern fn remove_file(path: str) -> void
extern fn rename_file(old: str, new: str) -> void

// Safe версии (возвращают Result)
fn safe_read_to_string(path: str) -> Result<str, str>
fn safe_write_string(path: str, content: str) -> Result<void, str>
```

### 10.6 Math модуль

**Math (`math/math.mlc`):**
```mlc
extern fn abs(x: i32) -> i32
extern fn abs_f(x: f32) -> f32
extern fn min(a: i32, b: i32) -> i32
extern fn max(a: i32, b: i32) -> i32
extern fn min_f(a: f32, b: f32) -> f32
extern fn max_f(a: f32, b: f32) -> f32
extern fn pow_f(base: f32, exp: f32) -> f32
extern fn sqrt_f(x: f32) -> f32
extern fn sin_f(x: f32) -> f32
extern fn cos_f(x: f32) -> f32
extern fn tan_f(x: f32) -> f32

// Утилиты
fn hypotenuse(a: f32, b: f32) -> f32 = sqrt_f(a * a + b * b)
fn radians(deg: f32) -> f32 = deg * 3.14159 / 180.0
fn degrees(rad: f32) -> f32 = rad * 180.0 / 3.14159
```

### 10.7 Канонические имена модулей

| Каноническое имя | Namespace | Тип |
|------------------|-----------|-----|
| `std/core/option` | - | `:pure` |
| `std/core/result` | - | `:pure` |
| `std/collections/array` | `mlc::collections` | `:mixed` |
| `std/text/string` | `mlc` | `:extern` |
| `std/text/conv` | `mlc` | `:extern` |
| `std/io/io` | `mlc::io` | `:extern` |
| `std/io/file` | `mlc::io` | `:extern` |
| `std/math/math` | `mlc::math` | `:extern` |
| `std/data/json` | `mlc::json` | `:extern` |
| `std/graphics/graphics` | `mlc::graphics` | `:extern` |

---

## 11. C++ AST DSL

### 11.1 Структура

**Директория:** `lib/cpp_ast/`

```
cpp_ast/
├── builder/
│   ├── dsl.rb            # DSL функции
│   ├── dsl_generator.rb  # DSL генератор
│   └── optimized_generator.rb
├── nodes/
│   ├── declarations.rb   # Объявления
│   ├── statements.rb     # Операторы
│   ├── expressions.rb    # Выражения
│   └── types.rb          # Типы
├── lexer/                # C++ лексер
└── parsers/              # C++ парсеры
```

### 11.2 DSL примеры

```ruby
include CppAst::Builder::DSL

# Создание функции
func = function_declaration(
  name: "add",
  return_type: type_ref("int32_t"),
  parameters: [
    parameter("a", type_ref("int32_t")),
    parameter("b", type_ref("int32_t"))
  ],
  body: return_stmt(binary("+", var_ref("a"), var_ref("b")))
)

# Создание структуры
struct = struct_declaration(
  name: "Point",
  members: [
    field("x", type_ref("float")),
    field("y", type_ref("float"))
  ]
)

# Создание template
template = template_declaration(
  parameters: [template_param("T")],
  declaration: function_declaration(...)
)
```

### 11.3 AST узлы

**Declarations:**
- `FunctionDeclaration`
- `StructDeclaration`
- `VariableDeclaration`
- `TemplateDeclaration`
- `UsingDeclaration`
- `NamespaceDeclaration`

**Statements:**
- `ReturnStatement`
- `IfStatement`
- `ForStatement`
- `WhileStatement`
- `ExpressionStatement`
- `CompoundStatement`

**Expressions:**
- `Literal`
- `Identifier`
- `BinaryExpression`
- `UnaryExpression`
- `CallExpression`
- `MemberExpression`
- `LambdaExpression`
- `ConditionalExpression`

---

## 12. Модульная система

### 12.1 Объявление модуля

```mlc
module Math

fn add(a: i32, b: i32) -> i32 = a + b
fn multiply(a: i32, b: i32) -> i32 = a * b
```

### 12.2 Импорты

**Традиционный стиль:**
```mlc
import Math
import Geometry::Shapes
```

**ESM стиль:**
```mlc
import { add, multiply } from "./math"
import * as Math from "./math"
```

### 12.3 Экспорты

```mlc
export fn public_function(x: i32) -> i32 = x * 2

fn private_function(x: i32) -> i32 = x + 1  // Не экспортируется
```

### 12.4 Namespace в C++

MLC модули преобразуются в C++ namespaces:

```mlc
module Geometry::Shapes
fn area(w: i32, h: i32) -> i32 = w * h
```

```cpp
namespace Geometry {
namespace Shapes {
int32_t area(int32_t w, int32_t h) {
  return w * h;
}
}
}
```

---

## 13. CLI интерфейс

### 13.1 Использование

```bash
# Компиляция и запуск
bin/mlc program.mlc

# Только вывод C++ кода
bin/mlc --emit-cpp program.mlc

# Сохранение временных файлов
bin/mlc --keep-tmp program.mlc

# Использование другого компилятора
bin/mlc --compiler clang++ program.mlc

# Чтение из stdin
echo 'fn main() -> i32 = 42' | bin/mlc -

# С аргументами программы
bin/mlc program.mlc -- arg1 arg2
```

### 13.2 Опции

| Опция | Описание |
|-------|----------|
| `--emit-cpp` | Вывести сгенерированный C++ код |
| `--keep-tmp` | Сохранить временные файлы |
| `--compiler <name>` | Использовать указанный C++ компилятор |
| `--verbose` | Подробный вывод |
| `-` | Читать из stdin |

### 13.3 Процесс компиляции

1. Парсинг MLC → AST
2. Семантический анализ → SemanticIR
3. Генерация C++ кода
4. Запись во временный файл
5. Компиляция: `g++ -std=c++20 -O2 -I<runtime> <tmp.cpp> -o <tmp>`
6. Запуск бинарника
7. Удаление временных файлов (если не `--keep-tmp`)

---

## 14. Тестирование

### 14.1 Структура тестов

```
test/
├── builder/              # C++ AST DSL тесты (~1000)
├── mlc/                  # Компилятор тесты (~400)
│   ├── parser/           # Парсер
│   ├── semantic/         # Семантика
│   ├── backend/          # Бэкенд
│   └── stdlib/           # Stdlib
├── integration/          # Интеграционные тесты (~128)
└── performance/          # Бенчмарки
```

### 14.2 Запуск тестов

```bash
# Все тесты
rake test

# Конкретный файл
ruby -Ilib:test test/mlc/parser/parser_test.rb

# С verbose
rake test TESTOPTS="--verbose"
```

### 14.3 Текущий статус

```
1524 runs, 4014 assertions, 0 failures, 0 errors
```

---

## Приложения

### A. Грамматика (упрощённая)

```
program     = (declaration)*
declaration = fn_decl | type_decl | module_decl | import_decl

fn_decl     = "fn" IDENT generic_params? "(" params ")" "->" type "=" expr
type_decl   = "type" IDENT generic_params? "=" type_body

type_body   = record_type | sum_type
record_type = "{" (field ("," field)*)? "}"
sum_type    = variant ("|" variant)*
variant     = IDENT ("(" types ")")?

expr        = if_expr | match_expr | lambda | binary
if_expr     = "if" expr "then" expr "else" expr
match_expr  = "match" expr "{" match_arms "}"
lambda      = params "=>" expr
binary      = unary (BINOP unary)*
unary       = UNOP? primary
primary     = literal | call | member | index | "(" expr ")"

stmt        = let_stmt | assignment | for_loop | while_loop | return_stmt
let_stmt    = "let" "mut"? IDENT (":" type)? "=" expr
for_loop    = "for" IDENT "in" expr "do" stmts "end"
while_loop  = "while" expr "do" stmts "end"

type        = primitive | IDENT | type "[" "]" | "(" types ")" "->" type
primitive   = "i32" | "i64" | "f32" | "f64" | "bool" | "str" | "void"
```

### B. Маппинг конструкций MLC → C++

| MLC | C++ |
|-----|-----|
| `fn f(x: i32) -> i32 = x * 2` | `int32_t f(int32_t x) { return x * 2; }` |
| `let x = 10` | `auto x = 10;` |
| `let mut y = 0` | `auto y = 0;` |
| `[1, 2, 3]` | `std::vector<int32_t>{1, 2, 3}` |
| `Point { x: 1.0, y: 2.0 }` | `Point{.x = 1.0, .y = 2.0}` |
| `type A = B \| C` | `using A = std::variant<B, C>;` |
| `x => x * 2` | `[](auto x) { return x * 2; }` |
| `match e { ... }` | `std::visit(overloaded{...}, e)` |
| `for x in arr do ... end` | `for (auto& x : arr) { ... }` |
| `if c then a else b` | `c ? a : b` |

### C. Зависимости

**Ruby:**
- Ruby >= 3.0
- Zeitwerk (автозагрузка)
- Minitest (тестирование)

**C++ Runtime:**
- C++20 компилятор (g++ >= 10, clang++ >= 11)
- nlohmann/json (JSON парсинг)
- XCB + Cairo (графика, опционально)

---

*Документ сгенерирован: 2025-11-21*
*Версия компилятора: 2.0.0*
