# MLC Language Design

Дата: март 2026.

Цель документа — зафиксировать принципы и решения по дизайну языка. Документ описывает как то, что уже реализовано, так и запланированные фичи с обоснованием. Порядок разделов соответствует приоритету реализации.

---

## Принципы

1. **Безопасность прежде удобства.** Конструкция недопустима если она вводит неопределённое поведение или позволяет обойти систему типов.
2. **Лаконичность без магии.** Синтаксический сахар разрешён только если сахар однозначно десугарируется и это десугарирование понятно читающему.
3. **Прогрессивный ООП.** Язык функциональный по умолчанию. ООП — слой поверх алгебраических типов и трейтов, без иерархии наследования.
4. **Неизменяемость по умолчанию.** `let` и `const` — неизменяемые. Мутация требует явного `let mut`.
5. **Явное управление ресурсами.** Нет GC. Нет неявного копирования. `Shared<T>` = ref-counted, `Owned<T>` = unique. COW для коллекций.
6. **Ошибки через типы.** Нет исключений. `Result<T, E>` и `Option<T>` — единственный способ выражать отказ.
7. **Нет null.** Отсутствие значения выражается через `Option<T>`, а не через специальное пустое значение.

---

## Что уже реализовано

### Типы данных

```mlc
// Record type (именованная структура)
type Point = Point { x: i32, y: i32 }

// Sum type (алгебраический тип / sealed class)
type Shape =
  | Circle(i32)
  | Rect(i32, i32)
  | Point

// Generic sum type
type Result<T, E> = Ok(T) | Err(E)
type Option<T>   = Some(T) | None
```

Record-типы — единственный способ хранить именованные поля. Sum-типы — единственный способ моделировать "одно из". Нет классов, нет наследования.

### Функции

```mlc
fn add(x: i32, y: i32) -> i32 = x + y

fn clamp(value: i32, low: i32, high: i32) -> i32 =
  if value < low then low
  else if value > high then high
  else value
  end

// Многострочная форма
fn process(items: [i32]) -> [i32] = do
  let mut result: [i32] = []
  for item in items do
    result.push(item * 2)
  end
  result
end
```

Тело функции — одно выражение. `do ... end` — блок-выражение с последовательностью операторов и возвращаемым значением (последнее выражение или явный `return`).

### Трейты и extend

```mlc
type Display {
  fn to_string(self: Self) -> string
}

type Color = Red | Green | Blue

extend Color : Display {
  fn to_string(self: Color) -> string =
    match self { Red => "Red", Green => "Green", Blue => "Blue" }
}

// Методы без трейта
extend Color {
  fn is_primary(self: Color) -> bool = true
}

// Статический вызов
Color.is_primary(Red)

// Вызов метода (синтаксический сахар над функцией)
Red.to_string()
```

`extend` — единственный способ добавить методы к типу. Нет классов с методами внутри. Нет наследования. Трейты — pure interfaces, не abstract classes.

### Pattern matching

```mlc
match shape {
  Circle(r)    => area_circle(r),
  Rect(w, h)   => w * h,
  Point        => 0
}

// Pipe-синтаксис (без фигурных скобок)
match result
| Ok(v)  => process(v)
| Err(e) => handle(e)
```

Match обязан быть исчерпывающим (enforced компилятором). Ветки `| PatCtor(binding) => expr` образуют именованные привязки.

### Pipe-оператор

```mlc
items
  |> filter(is_positive)
  |> map(double)
  |> take(10)
```

Десугарируется: `take(map(filter(items, is_positive), double), 10)`. Функция справа получает левый операнд первым аргументом; если справа уже вызов с аргументами, левый операнд вставляется перед ними.

### Оператор ?

```mlc
fn read_config(path: string) -> Result<Config, string> = do
  const text = File.read(path)
  const data = parse_json(text)?      // возвращает Err если ошибка
  Ok(Config.from_json(data)?)
end
```

`expr?` десугарируется в: если `Err(e)` — ранний возврат `Err(e)` из текущей функции, иначе unwrap значения. Функция должна возвращать `Result<_, E>`.

### Управление памятью

```mlc
// Shared<T> = std::shared_ptr — ref-counted, может быть shared между binding'ами
let node: Shared<Node> = Shared.new(Node { ... })

// Owned<T> = std::unique_ptr — единственный владелец
let buffer: Owned<Buffer> = Owned.new(Buffer.new(1024))

// Mutable reference — не копирует, не продлевает lifetime
fn fill(buf: ref mut Buffer, value: u8) -> unit
```

Коллекции (`Array<T>`, `HashMap<K,V>`) используют COW (copy-on-write): при мутации делается копия только если есть несколько владельцев.

### Диагностики с позицией

```mlc
type Span       = Span       { line: i32, column: i32 }
type Diagnostic = Diagnostic { message: string, span: Span }
```

Лексер хранит `line`/`col` в каждом токене. Парсер создаёт `Span` из позиции токена (`parser.span_at_cursor()`). Checker использует `Span` из `ExprIdent` для диагностики "undefined: name at line:column".

---

## Запланировано: приоритет 1

### Значения по умолчанию и именованные аргументы

**Проблема:** сейчас нужно или перегружать функцию, или передавать константы на вызывающей стороне.

**Решение:**

```mlc
fn connect(host: string, port: i32 = 443, secure: bool = true) -> Connection

// вызов — только нужные аргументы, явные имена
connect(host: "example.com")
connect(host: "dev.local", secure: false)
```

Правила:
- Аргументы с умолчанием — только в конце списка параметров (без позиционной неоднозначности).
- Значения вычисляются в точке **объявления**, не в точке вызова.
- При передаче по имени порядок не важен; позиционные аргументы идут перед именованными.
- Именованные аргументы не обязательны если порядок очевиден — это соглашение, не ограничение.

**Десугаривание:** на уровне codegen — несколько перегрузок или передача struct с флагами, прозрачно для пользователя.

### Деструктуризация в let

**Проблема:** доступ к полям через `value.field` делает простые трансформации многословными.

**Решение:**

```mlc
// Деструктуризация record
let { name, age } = person
let { x, y } = point

// Переименование
let { name: display_name, age: years } = person

// Деструктуризация в параметрах функции
fn distance({ x: x1, y: y1 }: Point, { x: x2, y: y2 }: Point) -> f64

// Guard let (позитивный путь без вложенного match)
let Ok(value) = result else return Err("failed")
let Some(item) = collection.first() else return
```

`guard let` — из Swift. Устраняет пирамиду `match Ok(x) => match Some(y) => ...`.

### Option<T> как встроенный тип

**Проблема:** "значение может отсутствовать" — не то же самое что "операция завершилась с ошибкой". Сейчас нет различия.

**Решение:**

```mlc
type Option<T> = Some(T) | None

fn find<T>(xs: [T], pred: T -> bool) -> Option<T>
fn first<T>(xs: [T]) -> Option<T>
fn get<K, V>(map: Map<K, V>, key: K) -> Option<V>
```

Операторы и методы:

```mlc
option?              // ранний возврат None из Option-функции
option ?: default    // Elvis: Some(x) -> x, None -> default (синтаксис обсуждается)
option.map(f)        // Some(x) -> Some(f(x)), None -> None
option.and_then(f)   // flatMap
option.unwrap_or(v)  // значение или default
```

`?` работает одинаково для `Result` и `Option` — если функция возвращает `Option<T>`, ранний возврат будет `None`; если `Result<T,E>` — `Err(...)`.

---

## Запланировано: приоритет 2

### Операторные трейты

**Проблема:** `+`, `==`, `<`, `[]` работают только для встроенных типов. Пользовательские типы не могут их перегрузить.

**Решение:** операторы — синтаксический сахар над трейтами.

```mlc
type Add<Rhs = Self> {
  type Output
  fn add(self: Self, rhs: Rhs) -> Self.Output
}

type Eq {
  fn eq(self: Self, other: Self) -> bool
  fn ne(self: Self, other: Self) -> bool = !self.eq(other)
}

type Ord : Eq {
  fn compare(self: Self, other: Self) -> Ordering
  fn lt(self: Self, other: Self) -> bool = self.compare(other) == Less
  fn gt(self: Self, other: Self) -> bool = self.compare(other) == Greater
}

type Index<Idx> {
  type Output
  fn index(self: Self, idx: Idx) -> Self.Output
}
```

Десугарирование: `a + b` → `Add_add(a, b)`, `a == b` → `Eq_eq(a, b)`, `xs[i]` → `Index_index(xs, i)`.

Преимущество: нет специальных случаев в компиляторе. Встроенные числовые операции — такие же impl трейта как пользовательские.

### Ассоциированные типы в трейтах

**Проблема:** без ассоциированных типов Iterator приходится параметризовать трейт, что создаёт type explosion при композиции.

**Решение:**

```mlc
// Без ассоциированных типов (плохо)
type Iterator<Item> {
  fn next(self: mut Self) -> Option<Item>
}
// extend Vec<T> : Iterator<T>    — OK
// fn map<A, B, I: Iterator<A>>(iter: I, f: A -> B) -> MapIter<I, A, B>  — громоздко

// С ассоциированными типами (хорошо)
type Iterator {
  type Item
  fn next(self: mut Self) -> Option<Self.Item>
}
// fn map<I: Iterator>(iter: I, f: I.Item -> B) -> MapIter<I, B>
```

### Числовая башня

**Проблема:** только `i32`. Реальные программы (работа с файлами, временными метками, сетью) требуют других числовых типов.

| Тип | Назначение |
|-----|-----------|
| `i32` | стандартный целый (текущий) |
| `i64` | большие числа, timestamp, размеры файлов |
| `u8`  | байты, протоколы, изображения |
| `usize` | индексы, смещения |
| `f64` | вещественная арифметика |

Литеральные суффиксы: `42i64`, `3.14f64`, `0x1Fu8`.

Без суффиксов — вывод типа из контекста. Если контекст не позволяет вывести — `i32` по умолчанию.

Числовые трейты: `Numeric`, `Integral`, `Float` — для обобщённой арифметики.

---

## Запланировано: приоритет 3

### Структурная типизация для record-типов

**Проблема:** сейчас `extend Type : Trait` требует явного объявления для каждого типа. Если тип структурно совместим с трейтом, это избыточно.

**Решение (ограниченное):** structural subtyping только для record-типов, nominal для sum-типов.

```mlc
type Named { fn name(self: Self) -> string }

// Автоматически совместимо, если у типа есть метод name -> string
fn greet(entity: impl Named) -> string = "Hello, " + entity.name()
```

Ключевое слово `impl Trait` в позиции параметра — принимает любой тип с нужными методами без явного `extend`. Компилятор проверяет совместимость статически.

### Varargs

```mlc
fn println(format: string, args: ..string) -> unit

// Вызов
println("x = {}, y = {}", x.to_string(), y.to_string())

// Внутри функции args — Array<string>
```

Varargs — только последний параметр. Десугарируется в передачу `Array`.

### Scope-контекст `with`

**Проблема:** resource management (файлы, соединения) требует явного освобождения или вложения в блок.

**Решение:**

```mlc
with File.open(path) as file do
  file.write(header)
  file.write(body)
end   // file.close() вызывается автоматически при выходе из блока
```

Требует трейт `Closeable` / `Drop`:

```mlc
type Drop {
  fn drop(self: mut Self) -> unit
}
```

При выходе из `with`-блока (в том числе при раннем `return`) вызывается `drop`.

---

## Запланировано: долгосрочно

### Система эффектов

Позволяет декларировать на уровне типов какие side effects разрешены функции.

```mlc
fn read_config(path: string) -> Config with IO
fn transform(x: T) -> T                      // чистая, без эффектов
fn random_choice<T>(xs: [T]) -> T with Random
```

Преимущества:
- Тестировать чистые функции без mock-объектов.
- Запрещать IO в checker/parser на уровне типов.
- `async` — частный случай эффекта `Async`, а не отдельный синтаксис.

Реализация: effect handlers (как в Koka/Effekt). Нет runtime overhead если эффект не используется.

### Higher-kinded types

Необходимы для `Functor`, `Monad`, `Applicative` и обобщённых комбинаторов коллекций.

```mlc
type Functor<F<_>> {
  fn map<A, B>(fa: F<A>, f: A -> B) -> F<B>
}
```

Приоритет низкий — без них язык уже выразителен для большинства задач; они нужны для библиотечного кода общего назначения.

---

## Что не войдёт в язык

| Конструкция | Причина отказа |
|-------------|----------------|
| Наследование классов | Трейты + composition достаточно. Иерархия наследования порождает fragile base class problem и усложняет анализ. |
| Null / nil | Заменяется `Option<T>`. Null-reference — источник ~30% ошибок в языках без null-safety. |
| Implicit conversion | Источник трудноуловимых ошибок. Явное приведение через функцию (`x.as_i64()`, `x.to_string()`). |
| Исключения | Заменяются `Result<T, E>`. Исключения нарушают локальность обработки ошибок и усложняют анализ control flow. |
| Monkey patching в runtime | `extend` в MLC статический. Открытые классы — источник конфликтов в больших кодовых базах. |
| Reflection / introspection | Нарушает статическую верифицируемость. Вместо этого — макросы и codegen (долгосрочно). |
| `any` / `dynamic` тип | Даже для interop — отдельный unsafe-контекст, не встроенный тип. |

---

## Сравнение с другими языками

| Фича | Ruby | Kotlin | TypeScript | Rust | MLC |
|------|------|--------|------------|------|-----|
| Null safety | нет | `T?` | `T \| null` | нет null | `Option<T>` |
| Ошибки | exception | `Result` / exception | `throws` | `Result<T,E>` | `Result<T,E>` |
| Управление памятью | GC | GC | GC | ownership | shared_ptr + COW |
| Трейты/интерфейсы | mixin | interface | interface | trait | trait + extend |
| Наследование | да | да | да | нет | нет |
| Pattern matching | ограниченно | when (ограниченно) | нет | полное | полное |
| Pipe operator | нет | нет | нет | нет | `\|>` |
| Default args | да | да | да | нет | планируется |
| Деструктуризация | ограниченно | да | да | да | планируется |
| Эффекты | нет | `suspend` | нет | `unsafe` | планируется |

---

## Связь с реализацией

Каждая новая языковая конструкция проходит через:

1. **Спецификация** в этом документе.
2. **Ruby-реализация** в `lib/mlc/` + тест в `test/mlc/`.
3. **MLC-реализация** в `compiler/` + тест в `compiler/tests/`.
4. **Синхронизация** `compiler/out/` через `compiler/build.sh`.

Реализация в `compiler/` не опережает Ruby-эталон. Если поведение расходится — эталон в Ruby.

Статус реализованных фич: `compiler/tests/` — 192 теста (`rake test_compiler_mlc`), `test/mlc/` — 1106 runs.
