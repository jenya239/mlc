# Self-Hosted Compiler: Language Requirements Analysis

Дата: 2026-02-17

## Цель

Определить минимальный набор языковых конструкций, необходимых для написания компилятора MLC на самом MLC.

## Анализ текущих возможностей MLC

### Уже реализовано (работает)

**Система типов:**
- Sum types (ADT): `type Expr = Add(i32, i32) | Var(str)`
- Record types: `type Token = { kind: str, value: str }`
- Generics: `type Option<T> = Some(T) | None`
- Type aliases: `type Name = i32`
- Tuples: `(i32, str)`, destructuring
- Smart pointers: `Shared<T>`, `Weak<T>`, `Owned<T>`

**Управление потоком:**
- Pattern matching с guards, nested patterns, or-patterns
- if/else expressions
- while/for loops с break/continue
- do-end блоки

**Структуры данных:**
- Arrays: `i32[]`, indexing `arr[i]`, slicing `arr[1:3]`
- String indexing: `s[i]` → `char`
- HOF на массивах: map, filter, fold

**Функции:**
- Первый класс: передача как аргумент
- Lambdas с closure capture
- Обобщённые функции

**Модули:**
- import/export
- extern declarations

**Ownership:**
- Move semantics (use-after-move detection)
- Copy types (i32, bool, f32)
- Reference counting (Shared<T>)

**Оптимизации:**
- Constant folding
- Dead code elimination

---

## Критичные недостающие конструкции для self-hosted

### Tier 1: Критично (без этого компилятор невозможен)

#### 1. Mutable data structures

**Проблема:** Компилятор требует изменяемых коллекций (symbol tables, type registries, AST builders).

**Текущая реализация MLC:**
```mlc
let mut vec = []
vec = vec ++ [elem]  // O(n) - копирование всего массива
```

**Что нужно:**
```mlc
// Вариант 1: Imperative mutability (Ruby-style)
let mut vec = Vec.new()
vec.push(elem)  // O(1) amortized

// Вариант 2: Functional persistent structures (OCaml-style)
let vec = Vec.empty()
let vec2 = Vec.push(vec, elem)  // O(1) with structural sharing
```

**Решение для MLC:**
- Stdlib модуль `Vec<T>` с методами `push`, `pop`, `get`, `set`, `length`
- Внутри: wrapper над `std::vector<T>`
- Runtime: `runtime/include/mlc/collections/vec.hpp`

**Приоритет:** КРИТИЧНО

---

#### 2. Hash maps (mutable)

**Проблема:** Symbol tables, type registries, scopes.

**Ruby подход:**
```ruby
symbols = {}
symbols[name] = type
```

**OCaml подход:**
```ocaml
let symbols = Hashtbl.create 16
Hashtbl.add symbols name type_info
```

**TypeScript подход:**
```typescript
const symbols = new Map<string, Type>()
symbols.set(name, typeInfo)
```

**Для MLC:**
```mlc
type HashMap<K, V> = ...  // extern wrapper

let mut table = HashMap.new()
table.set("main", IntType)
let ty = table.get("main")  // Option<V>
```

**Реализация:**
- Stdlib: `lib/mlc/stdlib/collections/hashmap.mlc`
- Runtime: `std::unordered_map<K, V>` wrapper
- Methods: `new()`, `get(key) -> Option<V>`, `set(key, value)`, `contains(key) -> bool`, `keys() -> K[]`, `values() -> V[]`

**Приоритет:** КРИТИЧНО

---

#### 3. String manipulation

**Проблема:** Lexer требует String slicing, charAt, substring, split.

**Текущее:**
- `s[i]` → `char` ✓
- `s.length()` ✓
- `s.contains(substr)` ✓

**Недостающее:**
```mlc
s.substring(start, end) -> str
s.char_at(i) -> str  // single-char string
s.split(delim) -> str[]
s.trim() -> str
s.starts_with(prefix) -> bool
s.ends_with(suffix) -> bool
```

**Реализация:** уже есть в `runtime/include/mlc/core/string.hpp`, нужно пробросить в SemanticIR type inference.

**Приоритет:** ВЫСОКИЙ

---

#### 4. File I/O

**Проблема:** Читать исходные файлы, писать сгенерированный C++.

**Минимальный API:**
```mlc
// Read entire file
fn read_file(path: str) -> Result<str, str>

// Write entire file
fn write_file(path: str, content: str) -> Result<unit, str>
```

**Расширенный API (опционально):**
```mlc
type File = ...  // handle
fn open(path: str, mode: str) -> Result<File, str>
fn read_line(f: File) -> Option<str>
fn write_line(f: File, line: str) -> Result<unit, str>
fn close(f: File) -> unit
```

**Реализация:**
- Stdlib: `lib/mlc/stdlib/io/file.mlc`
- Runtime: `std::ifstream`, `std::ofstream` wrappers

**Приоритет:** КРИТИЧНО

---

#### 5. Error handling ergonomics

**Проблема:** Компилятор генерирует тысячи проверок типов. Нужен удобный propagation.

**Текущее (вербозно):**
```mlc
let result = parse(input)
match result
  | Ok(ast) => process(ast)
  | Err(e) => return Err(e)
end
```

**Ruby подход:** exceptions (неприменимо для статической типизации)

**OCaml подход:** монадический bind (слишком вербозно без синтаксического сахара)

**Rust подход:** `?` operator
```rust
let ast = parse(input)?;
let ir = lower(ast)?;
let cpp = codegen(ir)?;
```

**Для MLC:**
```mlc
// Вариант 1: Monadic do-notation (Haskell/OCaml style)
do {
  ast <- parse(input);
  ir <- lower(ast);
  cpp <- codegen(ir);
  return cpp
}

// Вариант 2: try! macro (Ruby-inspired)
let ast = parse(input).unwrap_or_return()
let ir = lower(ast).unwrap_or_return()
```

**Реализация:** требует синтаксического расширения парсера.

**Приоритет:** СРЕДНИЙ (можно обойтись без этого, но код будет вербозным)

---

### Tier 2: Очень желательно (резко повысит удобство)

#### 6. Method call syntax (UFCS)

**Проблема:** Цепочки вызовов функций трудночитаемы.

**Текущее:**
```mlc
let result = map(filter(parse(input), is_valid), transform)
```

**Ruby/TypeScript стиль:**
```mlc
let result = input
  .parse()
  .filter(is_valid)
  .map(transform)
```

**Реализация:**
- `receiver.method(args)` → `method(receiver, args)` (синтаксический сахар)
- Требует расширения парсера и resolution логики

**Приоритет:** ВЫСОКИЙ (читаемость кода компилятора)

---

#### 7. Impl blocks (Rust/TypeScript style)

**Проблема:** Группировка методов вокруг типа.

**Текущее:**
```mlc
fn token_to_string(t: Token) -> str = ...
fn token_is_keyword(t: Token) -> bool = ...
```

**С impl blocks:**
```mlc
type Token = { kind: str, value: str }

impl Token {
  fn to_string(self) -> str = self.kind + ": " + self.value
  fn is_keyword(self) -> bool = keywords.contains(self.value)
}

// Usage:
let t = Token { kind: "ID", value: "foo" }
t.to_string()
```

**Реализация:**
- Парсинг `impl TypeName { ... }`
- Lowering: метод → функция с первым аргументом `self`
- Требует namespace resolution

**Приоритет:** СРЕДНИЙ

---

#### 8. Enums with methods (OCaml GADT style)

**Проблема:** Sum types часто требуют вспомогательных функций.

**Текущее:**
```mlc
type Expr = Add(i32, i32) | Var(str)

fn eval_expr(e: Expr) -> i32 = match e
  | Add(a, b) => a + b
  | Var(name) => lookup(name)
end
```

**С методами на enum:**
```mlc
type Expr = Add(i32, i32) | Var(str)

impl Expr {
  fn eval(self) -> i32 = match self
    | Add(a, b) => a + b
    | Var(name) => lookup(name)
  end
}

// Usage:
let e = Add(1, 2)
e.eval()
```

**Реализация:** аналогично impl blocks для record types.

**Приоритет:** СРЕДНИЙ

---

#### 9. Nested modules

**Проблема:** Организация кода компилятора по модулям.

**Ruby подход:**
```ruby
module MLC
  module Source
    module Parser
      class Lexer; end
    end
  end
end
```

**OCaml подход:**
```ocaml
module MLC = struct
  module Source = struct
    module Parser = struct
      module Lexer = struct ... end
    end
  end
end
```

**Для MLC:**
```mlc
module mlc.source.parser {
  export type Token = ...
  export fn lex(input: str) -> Token[]
}

// Usage:
import mlc.source.parser.{Token, lex}
```

**Реализация:**
- Парсинг nested module declarations
- Namespace resolution с точками
- Codegen: C++ namespaces

**Приоритет:** СРЕДНИЙ

---

#### 10. Pattern guards + where clauses

**Проблема:** Сложные условия в pattern matching.

**Текущее (guards работают):**
```mlc
match x
  | Some(n) if n > 0 => "positive"
  | _ => "other"
end
```

**Расширение (where clause, OCaml style):**
```mlc
match expr
  | BinOp("+", a, b) where is_constant(a) && is_constant(b) =>
      fold_constant(a, b)
  | _ => expr
end
```

**Реализация:** уже частично есть (guards), требует рефакторинга парсера.

**Приоритет:** НИЗКИЙ

---

### Tier 3: Качество жизни (можно обойтись без них)

#### 11. String interpolation (улучшение)

**Текущее:**
```mlc
"Error at line " + line.to_string() + ": " + message
```

**С интерполяцией:**
```mlc
"Error at line #{line}: #{message}"
```

**Реализация:** уже есть, но требует проверки корректности работы в сложных случаях.

---

#### 12. List comprehensions (улучшение)

**Текущее:**
```mlc
tokens.filter(|t| t.kind == "ID").map(|t| t.value)
```

**С comprehensions:**
```mlc
[t.value for t in tokens if t.kind == "ID"]
```

**Реализация:** уже есть, требует проверки.

---

#### 13. Default arguments

**Проблема:** Builder patterns требуют множества параметров.

**Текущее:**
```mlc
fn make_token(kind: str, value: str, line: i32, col: i32) -> Token
```

**С default args:**
```mlc
fn make_token(kind: str, value: str = "", line: i32 = 0, col: i32 = 0) -> Token
```

**Реализация:** требует расширения парсера и codegen (C++ default args).

**Приоритет:** НИЗКИЙ

---

#### 14. Named arguments

**Проблема:** Вызовы с множеством параметров трудночитаемы.

**Ruby/Python style:**
```mlc
make_token(kind: "ID", value: "foo", line: 10, col: 5)
```

**Реализация:** требует расширения парсера и call resolution.

**Приоритет:** НИЗКИЙ

---

## Итоговый приоритетный список для self-hosted

### Критично (Tier 1)
1. **Vec<T>** — mutable dynamic array (stdlib + runtime wrapper)
2. **HashMap<K, V>** — mutable hash table (stdlib + runtime wrapper)
3. **String methods** — substring, split, char_at (проброс из runtime)
4. **File I/O** — read_file, write_file (stdlib + runtime wrapper)

### Очень желательно (Tier 2)
5. **Method call syntax** — `x.method()` → `method(x)` (parser sugar)
6. **Impl blocks** — группировка методов вокруг типов (parser + lowering)

### Качество жизни (Tier 3)
7. **Error propagation sugar** — `?` operator или `unwrap_or_return()` (macro)
8. **Nested modules** — `module a.b.c { ... }` (parser + namespace resolution)

---

## Сравнение с Ruby/OCaml/TypeScript

### Ruby (сильные стороны для компилятора)
- **Mutable collections:** `[]`, `{}` — удобно для symbol tables
- **String API:** `split`, `strip`, `chars` — критично для lexer
- **Blocks + iterators:** `tokens.select { |t| ... }` — краткость
- **Open classes:** можно добавлять методы к типам (но в статике — проблемно)

**Для MLC:** взять mutable collections API, String methods, HOF на массивах.

### OCaml (сильные стороны)
- **Pattern matching:** exhaustiveness checking, nested patterns ✓ (уже есть)
- **Algebraic types:** Sum + Record ✓ (уже есть)
- **Immutable by default:** functional style (но для компилятора нужна мутабельность)
- **Modules + functors:** сильная модульная система (слишком сложно для первой версии)

**Для MLC:** взять только базовые модули (без functors).

### TypeScript (сильные стороны)
- **Structural typing:** duck typing в статической системе (сложно для C++ backend)
- **Union types:** `string | number` ✓ (аналог Sum types, есть)
- **Method call syntax:** `obj.method()` — критично для читаемости
- **Generics:** `Array<T>`, `Map<K, V>` ✓ (уже есть)

**Для MLC:** взять method call syntax, impl blocks.

---

## Рекомендации для ближайших шагов

### Сессия 4: Stdlib foundations
1. `Vec<T>` — runtime wrapper + stdlib
2. `HashMap<K, V>` — runtime wrapper + stdlib
3. String methods — проброс в type inference
4. File I/O — minimal API

### Сессия 5: Syntax sugar
1. Method call syntax — parser extension
2. Impl blocks — parser + lowering

### Сессия 6: Validation
1. Написать mini-lexer на MLC (токенизация простых выражений)
2. Написать mini-parser на MLC (recursive descent для арифметики)
3. Проверить, что Vec/HashMap/String/File работают в реальных условиях

---

## Выводы

**Минимальный набор для self-hosted:**
- Vec<T>, HashMap<K, V> (критично для AST, symbol tables)
- String API (критично для lexer)
- File I/O (критично для чтения исходников)
- Method call syntax (читаемость)

**Не критично (можно отложить):**
- Error propagation sugar (можно писать вербозно)
- Impl blocks (можно использовать свободные функции)
- Nested modules (можно использовать плоские модули)

**Оценка объёма работы:**
- Tier 1 (Vec/HashMap/String/File): ~2-3 сессии
- Tier 2 (method syntax/impl): ~1-2 сессии
- Tier 3: можно отложить до реальной потребности

**Стратегия:** начать с Tier 1, затем написать mini-lexer/parser как валидацию, затем Tier 2 по необходимости.
