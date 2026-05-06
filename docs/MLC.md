# MLC — язык программирования

Апрель 2026.

MLC компилируется в C++20. Цель: self-hosted компилятор, в котором mlcc написан на самом MLC — не хуже по выразительности, чем текущая Ruby-версия.

---

## Принципы

1. **Неизменяемость по умолчанию.** `let` и `const` — неизменяемые. Мутация требует явного `let mut`.
2. **Ошибки через типы.** Нет исключений. `Result<T, E>` — единственный способ выражать сбой.
3. **Нет null.** Отсутствие значения — `Option<T>`.
4. **Явное управление ресурсами.** `Shared<T>` = ref-counted, `Owned<T>` = unique. COW-коллекции.
5. **Нет наследования.** Трейты + `extend` + composition вместо иерархии классов.
6. **Сахар только там, где десугаринг очевиден.** Никакой магии.
7. **Нулевой runtime overhead.** Каждая конструкция транслируется в идиоматичный C++20 без виртуальных вызовов и аллокаций там, где их нет в C++.

---

## Текущее состояние (реализовано)

### Типы и выражения

```mlc
type Point = { x: i32, y: i32 }                          // record
type Shape = Circle(i32) | Rect(i32, i32) | Empty         // sum type
type Result<T, E> = Ok(T) | Err(E)                        // generic sum type
type Option<T>   = Some(T) | None

fn area(s: Shape) -> i32 =
  match s {
    Circle(r)    => r * r * 3,
    Rect(w, h)   => w * h,
    Empty        => 0
  }
```

### Что работает

| Фича | Статус |
|------|--------|
| Records, sum types, generics | ✓ |
| `match` с деструктуризацией | ✓ |
| `match`: `else =>` как catch-all (синоним `_ =>`) | ✓ |
| Exhaustive `let … else … end` (B3) | ✓ |
| `extend Type`, `extend Type : Trait` | ✓ |
| Generic trait bounds (`T: Display`) | ✓ |
| `Result<T,E>` + `?` оператор | ✓ |
| `Shared<T>`, COW Array, HashMap | ✓ |
| Lambdas, `\|>` pipe | ✓ |
| Spread `...` в записях и паттернах (паритет Ruby ⇄ mlcc — § B4) | частично ✓ |
| Модули, `import`/`export` | ✓ |
| Диагностики с позицией (GCC-style) | ✓ |
| Проверка иммутабельности при присвоении | ✓ |
| Triple-bootstrap стабильность | ✓ |

### Что не работает / не реализовано в mlcc

Текущий mlcc имеет 210 `while`-циклов с ручными индексами, 0 pipeline-вызовов, 13 uses of `extend` на 5200 строк. Причина: язык не даёт инструментов для идиоматичного кода.

---

## Фаза A — Критический минимум (HOF + комбинаторы)

*Без этого нельзя писать mlcc в нефункциональном стиле.*

### A1. Array HOF

**Проблема.** 80% `while`-циклов в mlcc — это map/filter/fold вручную.

```mlc
// Сейчас
let mut result: [string] = []; let mut i = 0
while i < decls.length() do result.push(gen_decl(decls[i])); i = i + 1 end

// После A1
decls.map(d => gen_decl(d))
```

**Методы:** `map`, `filter`, `fold`, `flat_map`, `any`, `all`, `find`, `find_index`,
`sort_by`, `group_by`, `zip`, `enumerate`, `flat`, `take`, `drop`, `sum`, `join`.

**C++ трансляция:** `std::transform`, `std::copy_if`, range-for, `std::sort` с компаратором.
Без аллокаций при известном размере — `reserve` по длине источника. Чистое C++20.

**Тип:** `[T].map(T -> U) -> [U]`, `[T].filter(T -> bool) -> [T]`,
`[T].fold<U>(U, (U, T) -> U) -> U`.

**Checker:** новые правила в `builtin_method_return_type` для каждого метода с выводом
параметра типа из типа лямбды.

---

### A2. Result и Option комбинаторы

**Проблема.** Единственный способ цепочки — вложенные `match` или несколько `?`.

```mlc
// Сейчас
let parsed = parse(source)?
let checked = check(parsed)?
let output = codegen(checked)

// После A2 — railway-oriented
parse(source)
  .and_then(check)
  .map(codegen)
  .map_err(format_error)
```

**Result методы:** `map(T -> U) -> Result<U,E>`, `map_err(E -> F) -> Result<T,F>`,
`and_then(T -> Result<U,E>) -> Result<U,E>`, `or_else(E -> Result<T,F>) -> Result<T,F>`,
`unwrap_or(T) -> T`, `unwrap_or_else(E -> T) -> T`, `ok() -> Option<T>`.

**Option методы:** `map(T -> U) -> Option<U>`, `and_then(T -> Option<U>) -> Option<U>`,
`or_else(() -> Option<T>) -> Option<T>`, `unwrap_or(T) -> T`, `filter(T -> bool) -> Option<T>`,
`ok_or(E) -> Result<T,E>`.

**C++ трансляция:** методы на `std::variant<Ok<T>,Err<E>>`. `map` —
`std::visit` с `if holds_alternative<Ok>`. Нет overhead по сравнению с ручным match.

**Checker:** методы типа `and_then` требуют вывода возвращаемого типа из типа лямбды.
Новое правило в `infer_method_from_object_and_arguments` для `TGeneric("Result", ...)` и
`TGeneric("Option", ...)`.

---

### A3. Деструктуризация в `let`

**Проблема.** Нельзя распаковать record или tuple в переменные.

```mlc
let { x, y } = point               // record деструктуризация
let [head, ...tail] = items         // array head/tail
let Ok(value) = result              // sum type (panic если Err — только с exhaustive check)
let (a, b) = pair                   // tuple
```

**C++ трансляция:** `auto& [x, y] = point;` — C++17 structured bindings. Прямое соответствие.

**Checker:** расширение `StmtLet` — вместо имени, паттерн в левой части.

---

### A4. Значения по умолчанию для параметров

**Проблема.** Каждый вариант функции с опциональным аргументом требует отдельной функции.

```mlc
fn format(value: string, width: i32 = 0, pad: string = " ") -> string = ...
fn connect(host: string, port: i32 = 5432, timeout: i32 = 30) -> Result<Conn, Error> = ...
```

**C++ трансляция:** `default` параметры в C++ — прямое соответствие.

**Ограничение:** значение по умолчанию — compile-time константа или вызов без side effects.
Параметры с дефолтами — только в конце списка.

---

### A5. Строковая интерполяция

**Проблема.** `"x = " + x.to_string() + ", y = " + y.to_string()` — нечитаемо.

```mlc
`x = ${x}, y = ${y}`                         // вычисляется через .to_string()
`result: ${if ok then "ok" else "fail" end}`  // произвольные выражения
`многострочный
текст без экранирования`                      // переносы строк — обычные символы
`вложенные ${ f({a: 1}) } скобки`            // баланс {} внутри ${}
```

**Синтаксис:** обратные кавычки `` ` `` с `${expr}` (JS-стиль).  
Escape-последовательности: `` \` `` — буквальная кавычка; `\${` — буквальные `${` без интерполяции.

**C++ трансляция:** цепочка `+`-конкатенации, каждый `${expr}` → `expr.to_string()`.

```mlc
// `a ${x} b` десахарится в:
"a " + x.to_string() + " b"
```

**Ограничение:** тип `${expr}` должен иметь метод `to_string() -> string`.

---

## Фаза B — Производительность разработки (derive, деструктуризация паттернов)

### B1. `derive` — автогенерация трейтов

**Проблема.** Каждый тип-запись требует ручной реализации `to_string`, `==`, comparisons.
В mlcc ~30 таких функций написаны вручную.

```mlc
type Span = { file: string, line: i32, col: i32 } derive { Display, Eq, Ord, Hash }
type Token = Ident(string) | Int(i64) | Str(string) derive { Display, Eq }
```

**Что генерируется:**

| Трейт | Что генерирует |
|-------|----------------|
| `Display` | `to_string` через конкатенацию полей или имён вариантов |
| `Eq` | `==` по всем полям структурно |
| `Ord` | лексикографическое сравнение полей |
| `Hash` | хеш через поля (для HashMap ключей) |
| `Clone` | глубокое копирование (Shared<T> — инкремент ref-count) |

**C++ трансляция:** генерация `operator==`, **`operator<`** для `Ord`, специализации **`std::hash<T>`** для `Hash`,
метода `to_string`. Всё — compile-time, zero overhead. Для `Ord` не используется `operator<=>` до отдельного решения.

**Реализация:** codegen-правило в `compiler/codegen/decl.mlc` для `DeclType` с `derive`.

---

### B2. `or` паттерны в `match` расширенные

Сейчас: `A | B => expr` работает только для unit-вариантов.
После B2: `Circle(r) | Square(r) => r * r * PI` — биндинг одинакового имени.

---

### B3. Exhaustive `let` для sum types

**Статус:** реализовано в Ruby-компиляторе и mlcc (`let Pat = expr else … end`).

```mlc
// Вместо match с одним arm
let Some(value) = optional else return Err("not found") end
let Ok(data) = result else |e| return Err(e) end
```

`else` — обязательная ветка для non-exhaustive деструктуризации. Без `else` — compile error.

**C++ трансляция:** `if (!holds_alternative<Some>(optional)) { return Err("not found"); } auto& value = get<Some>(optional)._0;`

---

### B4. Оператор spread (`...`): семантика, расширение и проверка паритета

**Зачем.** В TypeScript и JavaScript **object spread** `{ ...a, x: 1 }` задаёт поверхностное слияние: берётся объект `a`, затем явные поля перекрывают одноимённые ключи (правило «позже в синтаксисе — сильнее»). В Ruby для хешей обычно `merge` / `**`; в MLC один токен `...` держит ту же **идею композиции без мутации**, но для типизированных записей и C++-трансляции.

**Лексер.** Последовательность `...` — отдельный токен spread; он распознаётся **до** оператора диапазона `..`, иначе возникала бы неоднозначность (как в тестах лексера).

**Где уже есть в MLC**

| Контекст | Ruby (`lib/mlc/`) | mlcc (`compiler/`) | Замечание |
|----------|-------------------|--------------------|-----------|
| Запись: несколько spread и позиция между полями (`{ a: 1, ...base, b: 2 }`) | ✓ AST `RecordLit` + `spreads` с позицией | преимущественно **именованное** обновление `TypeName { ...expr, field: value }` → `ExprRecordUpdate` | Паритет литералов со spread — цель расширения парсера/IR mlcc |
| Только spread (`{ ...base }`) | ✓ | через форму обновления именованной записи | Сверить с эталонной семантикой Ruby |
| Паттерн массива: хвост `[head, ...tail]` | по дорожной карте / A3 | ✓ парсер паттернов (`...` + имя rest) | Checker/codegen должны оставаться согласованы с деструктуризацией в `let` |

**Семантика (ориентир на TS / JS object spread).**

1. **Порядок:** слева направо — каждый следующий spread или блок полей накладывается поверх уже собранного значения.
2. **Конфликт имён полей:** как в TypeScript — **перекрывает то, что объявлено правее** в том же литерале (явное поле после spread побеждает поле из spread).
3. **Типы:** базовое выражение под spread должно быть совместимо с типом записи (либо тот же номинальный тип записи, либо структурно совместимое подмножество полей — правило задаёт checker, не постфактум в codegen).
4. **Неизменяемость:** spread не мутирует операнд; результат — новое значение записи (в C++ — конструктор / пошаговое копирование полей без общего «магического» runtime).

**Что расширять в mlcc (по приоритету).**

1. **Паритет с Ruby-AST:** поддержка в выражениях тех же форм, что уже разбирает Ruby-парсер (`RecordLit` с несколькими spread и позициями), чтобы один и тот же фрагмент MLC давал одинаковое дерево до Semantic IR.
2. **Checker:** проверка дубликатов ключей, порядка слияния, типов операндов spread; сообщения об ошибках с позицией на конкретном `...` или поле.
3. **Codegen:** детерминированное lowering в C++ (копирование полей из каждого spread-выражения затем применение оверрайдов); без лишних аллокаций там, где тип известен как POD/struct.
4. **Опционально позже:** spread в **литералах массивов** `[...xs, y]` — только если появится ясная семантика копирования COW-массивов и паритет с ожиданиями в духе ES (отдельное решение по языку).

**Как проверять паритет (образец процесса).**

- **Эталон разбора и семантики:** конвейер Ruby — `MLC.parse` / SemanticIR / `MLC.to_cpp` для коротких эталонных программ из `test/mlc/spread_operator_test.rb` и `test/mlc/record_update_test.rb`.
- **Self-hosted:** после изменений парсера/checker/codegen — `bundle exec rake test_compiler_mlc` и точечные unit-тесты на литералы записей и обновления (`compiler/tests/test_pipe_and_record_update.mlc`, lexer-тест на токен `...`).
- **Регресс лексера:** `...` против `..` (spread против range) — отдельные случаи в тестах лексера Ruby и mlcc.
- **Интеграция:** при появлении move/borrow правил — сценарии «spread копирует поля, не отнимая владение» в духе `test/integration/move_semantics_e2e_test.rb` (если применимо к записям).

Пока Ruby-путь богаче по формам record-spread, новые фичи языка в этом месте **сначала фиксируются тестами на Ruby**, затем переносятся в mlcc с тем же набором примеров — так сохраняется привычная для проекта дисциплина «Ruby — эталон семантики».

---

### Синтаксис `match`: `else =>` как catch-all

В ветке `match` ключевое слово `else` в позиции **паттерна** — синоним `_` (wildcard): семантика и проверка exhaustiveness те же.

```mlc
match x {
  0 => "zero",
  else => "other"
}
```

---

## Фаза C — Типобезопасность и архитектурные паттерны

### C1. Phantom types

**Проблема.** Нельзя запретить вызов функции с AST нужной фазы на уровне типов.

```mlc
type Unvalidated
type Validated
type Compiled

type Ast<Phase> = Ast { decls: [Shared<Decl>] }       // Phase — phantom, не в полях

fn parse(source: string) -> Result<Ast<Unvalidated>, Error> = ...
fn check(ast: Ast<Unvalidated>) -> Result<Ast<Validated>, Error> = ...
fn codegen(ast: Ast<Validated>) -> Ast<Compiled> = ...

// Ошибка компиляции:
codegen(parse(source)?)     // Ast<Unvalidated> передан в Ast<Validated>
```

**C++ трансляция:** `template<typename Phase> struct Ast { ... };` — Phase не используется в
полях, только в типовых параметрах. C++ поддерживает phantom type parameters нативно.

**Checker:** phantom type parameters — параметры типа, не упомянутые в полях. Сейчас
checker требует, чтобы все type params встречались в полях. Нужно снять это ограничение.

---

### C2. Smart constructors — `private` варианты конструктора

```mlc
type Email = private Email { raw: string }

extend Email {
  fn parse(input: string) -> Result<Email, string> =
    if input.contains("@") then Ok(Email { raw: input })
    else Err("invalid email: " + input)
    end

  fn value(self: Email) -> string = self.raw
}

// За пределами модуля — только Email.parse(), прямой Email { raw: ... } запрещён.
```

**C++ трансляция:** приватный конструктор в struct + `friend` функция или factory.

---

### C3. Именованные аргументы (keyword arguments)

```mlc
fn connect(host: string, port: i32, timeout: i32) -> Result<Conn, Error> = ...

// Вызов:
connect(host: "localhost", port: 5432, timeout: 30)
connect("localhost", port: 5432, timeout: 30)  // можно смешивать с позиционными
```

**C++ трансляция:** именованные аргументы нет в C++, но при трансляции порядок аргументов
фиксирован — это просто синтаксическое соответствие позиционным параметрам. Нет overhead.

---

### C4. Числовая башня

Только `i32` — блокирует реальный ввод/вывод, работу с файлами, размеры буферов.

| Тип | Назначение | C++ |
|-----|-----------|-----|
| `i32` | стандартный (текущий) | `int32_t` |
| `i64` | timestamp, размеры файлов | `int64_t` |
| `u8` | байты, протоколы | `uint8_t` |
| `usize` | индексы, смещения | `size_t` |
| `f64` | вещественная арифметика | `double` |
| `char` | Unicode codepoint | `char32_t` |

Литеральные суффиксы: `42i64`, `3.14f64`, `0xFFu8`. Без суффикса — вывод из контекста или `i32`.

---

### C5. `with` — resource management / RAII

```mlc
type Drop {
  fn drop(self: mut Self) -> unit
}

with File.open(path)? as file do
  file.write(header)?
  file.write(body)?
end   // file.drop() вызывается автоматически, в том числе при раннем return/?
```

**C++ трансляция:** RAII wrapper — деструктор вызывает `drop`. Идиоматичный C++.
`with` — синтаксический сахар над RAII scope-guard.

---

## Фаза D — Продвинутая типовая система

### D1. Ассоциированные типы в трейтах

Необходимы для `Iterator` без type-explosion.

```mlc
trait Iterator {
  type Item
  fn next(self: mut Self) -> Option<Self.Item>
}

trait Collect {
  type Output<T>
  fn collect<I: Iterator>(iter: I) -> Self.Output<I.Item>
}

// Использование:
fn map<I: Iterator, B>(iter: I, f: I.Item -> B) -> MapIter<I, B> = ...
```

**C++ трансляция:** `using Item = T;` в struct, доступ через `typename I::Item`.
C++ идиоматично поддерживает это через type members.

---

### D2. Трейты операторов (оператор-перегрузка через трейты)

```mlc
trait Add<Rhs> {
  type Output
  fn add(self: Self, rhs: Rhs) -> Self.Output
}

type Vec2 = { x: f64, y: f64 }
extend Vec2 : Add<Vec2> {
  type Output = Vec2
  fn add(self: Vec2, rhs: Vec2) -> Vec2 = Vec2 { x: self.x + rhs.x, y: self.y + rhs.y }
}

let v3 = v1 + v2   // десугарируется в Add_add(v1, v2)
```

**C++ трансляция:** `operator+` в struct. Прямое соответствие.

---

### D3. Трейт как тип параметра (structural typing lite)

Вместо нового ключевого слова `impl` — имя трейта используется напрямую как тип параметра.
Компилятор распознаёт объявленный трейт и десугарит в анонимный type-параметр с тем же бо́ундом
(как явная запись `<T: Trait>`).

**MVP:** только «голое» имя объявленного трейта без собственных type-параметров у трейта (`x: Display`,
`[Display]`, вложенность через массив/Shared/аргументы дженерика как у обычного типа). Имя, объявленное и как
`trait`, и как `type` в одной программе — ошибка компиляции.

`TyNamed("T")` из явных type-параметров функции не трогается. Два параметра с одним трейтом дают два
независимых синтетических параметра (два набора `requires`).

Запись `handler: Fn(Request, Response)` — это встроенный тип функции (`Fn`), не D3.

```mlc
fn print_all(items: [Display]) -> unit =
  items.map(x => println(x.to_string())).fold((), (_, _) => ())
```

Каждый параметр-трейт становится отдельным анонимным type param (`__trait_param_0`, …).
Statically dispatched, нет vtable.

**C++ трансляция:** объявление концепта (`concept Display` с `requires`) и параметры функции, тип которых задаётся через этот концепт (аналог `template<typename T> requires Display<T>`). Имена синтетических type-параметров в IR (`__trait_param_0`, …) в целевом C++ могут не совпадать дословно.

---

### D4. Multiline `where` для сложных bounds

**Статус:** реализовано в mlcc (парсер мержит bounds с `<T: Trait>`) и в Ruby (мердж в `TypeParam` до codegen); неизвестное имя в `where` — ошибка.

```mlc
fn serialize<T>(value: T) -> string
  where T: Display + Eq + Clone
= ...

fn merge<K, V>(maps: [Map<K, V>]) -> Map<K, V>
  where K: Eq + Hash + Clone, V: Clone
= ...
```

**C++ трансляция:** `requires (Display<T> && Eq<T> && Clone<T>)` — C++20 concepts.

---

### E1. `derive` — автогенерация трейт-имплементаций

```mlc
type Point = { x: i32, y: i32 }
  derive { Eq, Hash, Display }

// автогенерируется:
// extend Point : Eq   { fn eq(self: Point, other: Point) -> bool = self.x == other.x && self.y == other.y }
// extend Point : Hash { ... }
// extend Point : Display { fn to_string(self: Point) -> string = "Point { x: " + self.x.to_string() + ", y: " + self.y.to_string() + " }" }
```

**Ограничения текущей версии:** `derive { Hash }` только для **мономорфных** типов (без type-параметров).
Для полей записи и полезной нагрузки sum-вариантов в хеше допускаются **`i32`**, **`bool`**, **`string`**;
иначе — ошибка checker. Дубликаты и неизвестные имена в списке `derive { … }` — ошибка.

**C++ трансляция:** `operator==`, для `Ord` — **`operator<`** (лексикографически по полям),
специализация **`std::hash<T>`** в `namespace std` для `Hash`.
Оператор **`operator<=>`** при derive пока **не** генерируется — отдельный этап.

Нулевой overhead — тот же код что написал бы руками (в пределах этих правил).

---

### E2. Гарды в `match`

```mlc
match score {
  n if n >= 90 => "A",
  n if n >= 75 => "B",
  n if n >= 60 => "C",
  _            => "F"
}
```

**C++ трансляция:** вложенные `if` внутри `case` ветки. Чисто синтаксический сахар.

---

### E3. Деструктурирование в параметрах функции

```mlc
fn magnitude({ x, y }: Vec2) -> f64 = sqrt(x * x + y * y)

fn swap_pair((left, right): Pair<i32, string>) -> Pair<string, i32> =
  Pair { first: right, second: left }
```

**Формы:** `{ поля }: ИмяТипа`, кортеж **`(a, b, …): (T, U, …)`** (при необходимости один слот **`(x,): (T,)`**). Один синтетический параметр (**`__mlc_dsN`**) и prelude (`let`-подобная деструктуризация тела).

**Ruby-компилятор:** перечисленные формы; дженерики и вызов с литералом покрыты тестами (`parameter_destructuring_test`, `destructuring_e2e`). Для **`extern`** и дефолта у параметра с паттерном действуют ограничения (диагностики). **`…rest`** в записи-параметре: разбор в AST есть; полный codegen для rest ограничен сценариями.

**Self-hosted (`mlcc`):** паритет по **`{…}`** и **`(…)`** (parse → expand prelude → checker/codegen); запрет деструктуризации на **`extern`**.

**C++ трансляция:** C++17 structured bindings или доступ через `.field` / временная **`__lt`**.

---

### E4. Default значения полей

```mlc
type Config = {
  host:    string = "localhost",
  port:    i32    = 8080,
  timeout: i32    = 30
}

let cfg = Config {}                   // все дефолтные
let cfg = Config { port: 9090 }       // только port переопределён
```

**C++ трансляция:** default member initializers в struct. Нативная C++ фича.

**Self-hosted (`mlcc`):** в типах записи те же дефолты в объявлении; литералы дополняются дефолтами до кодогена. Выражение дефолта должно сводиться к допустимому инициализатору поля в C++ (литералы, простые идентификаторы, цепочки `+ - * / %` и унарные `+` / `-` на них); более общие выражения — в эталонном Ruby-пайплайне.

---

### E5. Пайплайн оператор `|>`

```mlc
let result = items
  |> filter(x => x > 0)
  |> map(x => x * 2)
  |> fold(0, (acc, x) => acc + x)
```

**C++ трансляция:** вложенные вызовы функций — `fold(map(filter(items, ...), ...), ...)`.
Нулевой overhead, компилятор инлайнит.

---

### E6. Частичное применение

```mlc
let double  = mul(2, _)
let add5    = add(5, _)
let greet   = format("Hello, {}!", _)
```

Сахар над лямбдами: `mul(2, _)` → `x => mul(2, x)`.

**C++ трансляция:** C++ лямбда с захватом. Статически известный тип — инлайнится.

---

### F1. Строковые литералы как в Ruby (`'` / `` ` ``)

**Цель:** меньше шума от двойных кавычек; интерполяция отделена от обычных строк.

**Конвенция:**
- `'...'` — обычная строка (нет интерполяции), включая `import 'path/to/module'` и сообщения об ошибках.
- `` `...{expr}...` `` — строковый шаблон с интерполяцией (уже поддерживается лексером mlcc).
- `"..."` — оставить допустимым для совместимости и постепенной миграции (или запретить после массового перевода — решить в задаче).

**План работ:**
1. Убедиться, что Ruby-лексер/парсер трактуют `'...'` так же (не только `"..."`).
2. Документировать в синтаксисе языка правило «обычная строка vs шаблон».
3. Массовая миграция `compiler/**/*.mlc`, примеры и тесты: `"..."` → `'...'` там, где нет интерполяции; шаблоны — только через `` `...` ``.
4. Прогон `rake test_mlc`, `rake test_compiler_mlc`, при необходимости `compiler/build.sh`.

**Риски:** escape-последовательности внутри `'...'` (как в Ruby: минимальный escape) — выровнять правила в обоих компиляторах.

---

### F2. `do` … `end` для тел выражений (`match`, при необходимости другие)

**Проблема:** вложенные `{ }` в ветках `match` и подобных конструкциях тяжело читать; для многострочных тел уже принято `do … end` у функций.

**Цель:** разрешить альтернативный синтаксис тела ветки через `do … end` там, где сейчас обязательно или типично `{ … }`.

```mlc
match opt do
  Some(x) =>
    do
      let doubled = x * 2
      doubled + 1
    end
  None => 0
end
```

(Точный синтаксис — зафиксировать в парсере: после `=>` либо одно выражение, либо `do … end`, либо блок в `{ }`.)

**План работ:**
1. Расширить грамматику в Ruby-парсере (expression_parser / match).
2. Mirror в mlcc `compiler/parser`.
3. Checker/codegen без изменений семантики — только другой AST-путь к тому же блоку.
4. Тесты парсера + один интеграционный пример в mlcc.

**Ограничение:** фигурные скобки остаются для record/tuple литералов и типов — только «скобки тела выражения» замещаются опционально.

---

## Что не войдёт в язык

| Конструкция | Причина |
|-------------|---------|
| Наследование классов | Трейты + composition достаточно. Fragile base class problem. |
| Null / nil | `Option<T>`. |
| Исключения | `Result<T,E>`. |
| Implicit conversion | Явное приведение через функции. |
| Reflection / introspection | Нарушает статическую верифицируемость. |
| `any` / `dynamic` | Отдельный unsafe-контекст, не встроенный тип. |
| Higher-kinded types (HKT) | C++ template template parameters — слишком сложная трансляция без гарантированного нулевого overhead. Пересмотреть после D1. |
| Система эффектов | Правильная реализация требует CPS или continuations — нет прямого C++ аналога. Долгосрочно. |
| Monkey patching / открытые классы | `extend` в MLC статический. |
| Varargs | `Array<T>` явный параметр достаточен. |

---

## Таблица C++ трансляции (фичи → C++20)

| Фича MLC | C++20 |
|----------|-------|
| `[T].map(f)` | `std::transform` + `std::vector` |
| `[T].filter(p)` | `std::copy_if` |
| `[T].fold(z, f)` | `std::accumulate` |
| `Result<T,E>.and_then(f)` | метод на `std::variant` |
| `derive { Display }` | генерация `to_string()` |
| `derive { Eq }` | `operator==` |
| `derive { Ord }` | `operator<` (лексикографическое сравнение полей; `<=>` — позже) |
| `derive { Hash }` | `std::hash<T>` специализация |
| Phantom types | `template<typename Phase>` struct |
| `private` constructor | `private:` в struct + factory |
| `with` block | RAII scope guard |
| Associated types | `using Item = T` в struct |
| `impl Trait` param | `template<T> requires Concept<T>` |
| `where` clause | `requires (A<T> && B<T>)` |
| Operator overloading | `operator+`, `operator==`, etc. |
| Named args | позиционные (compile-time соответствие) |
| Default args | C++ default parameter values |
| `` `...{x}...` `` interpolation | цепочка `+` + `.to_string()` |
| Destructuring `let` | C++17 structured bindings |
| Numeric types i64/f64 | `int64_t`, `double`, etc. |

---

## Приоритет реализации

```
A1  Array HOF              — убирает 80% while-циклов. Фундамент.
A2  Result/Option chain    — убирает вложенные match. Фундамент.
A3  let деструктуризация   — нужна для идиоматичного кода.
A4  Default args           — нужны для API mlcc.
A5  Строковая интерполяция — QOL, без неё всё читается плохо.
B1  derive                 — убирает boilerplate из mlcc.
B3  Exhaustive let / else  — реализовано; см. таблицу «Что работает».
C1  Phantom types          — type-safe compiler phases.
C2  Smart constructors     — encapsulation.
C4  Числовая башня         — i64 нужен для реального ввода/вывода.
C5  with / Drop            — resource management.
D1  Associated types       — Iterator, Collect.
D2  Operator overloading   — после D1.
D3  Трейт как тип          — после D1.
D4  where clause           — после D3.
C3  Named args             — низкий приоритет.
E1  derive                 — Eq/Hash/Display автоматически.
E2  Гарды в match          — if guard в ветках.
E3  Деструктурирование     — в параметрах функций.
E4  Default поля           — в struct-определениях.
E5  Пайплайн |>            — f |> g |> h.
E6  Частичное применение   — f(1, _).
// E7 Строк. интерполяция — уже реализована (A5).
F1  Строки `'` + шаблон `` ` `` — миграция mlcc/Ruby.
F2  match: тело через do/end — читаемость вместо `{`.
G1  Обход и трансформация AST — обобщённые visitor/transform без дублирования mlcc/Ruby.
G2  Линтер — диагностики поверх checker/семантики, категории правил, suppressions.
G3  Рефакторинг — переименование, правки импортов, автопочинки с сохранением форматирования (через span-трекинг).
G4  Внешний контракт AST — сериализация/дифф или stableness узлов для плагинов и IDE (после стабилизации формы дерева).
```

**Критерий "не хуже Ruby":** A1 + A2 + A3 + A4 + B1. После них mlcc можно переписать
без процедурных паттернов.

---

## Процесс реализации

### Почему сначала Ruby

`compiler/build.sh` компилирует mlcc через Ruby-компилятор. Если написать
`decls.map(d => gen_decl(d))` в исходниках mlcc, Ruby-компилятор должен уже понимать
`.map` — иначе сборка упадёт. Любая новая фича, которую планируется использовать внутри
mlcc, должна быть в Ruby-компиляторе раньше, чем в mlcc.

### Порядок для каждой фичи

```
1. Runtime (C++)       runtime/include/mlc/
                       Добавить методы / типы в runtime. Не зависит ни от кого.
                       Пропускается если фича не требует новых runtime-операций.

2. Ruby — семантика    lib/mlc/representations/semantic/  +  lib/mlc/registries/
                       Вывод типов, проверка арности, трейт-баунды для новой конструкции.

3. Ruby — codegen      lib/mlc/backends/cpp/rules/
                       Генерация корректного C++ из новой конструкции.

4. Ruby-тесты          test/mlc/
                       rake test_mlc зелёный до перехода к mlcc.

5. mlcc — checker      compiler/checker/
                       Вывод типов, диагностики — аналогично шагу 2.

6. mlcc — codegen      compiler/codegen/
                       Эмит C++ — аналогично шагу 3.

7. mlcc-тесты          compiler/tests/
                       rake test_compiler_mlc зелёный.

8. Пересборка          compiler/build.sh  →  rake test_compiler_mlc  →  e2e
                       Triple-bootstrap проверяется при значимых изменениях.
```

### Особые случаи

| Фича | Шаги | Примечание |
|------|------|-----------|
| A1 Array HOF | 1→8 полностью | Нужен runtime + оба компилятора |
| A2 Result/Option chain | 1→8 полностью | То же — используется внутри mlcc |
| A5 Строковая интерполяция | 2→8, без 1 | Runtime не меняется |
| B1 derive | 2,3 минимальны → 5,6 основная работа | Ruby уже парсит, нужен codegen |
| C1 Phantom types | 2,5 — checker; 1,3 пропускаются | C++ поддерживает нативно |
| C4 Числовые типы | 1 тривиально (C++ native) | Основное — lexer/parser в обоих |
| C5 with / Drop | 1 (RAII wrapper) → 2→8 | RAII в C++ нативный |
| F1 Строковые литералы | 2→4 Ruby, затем 5→7 mlcc + миграция | Лексер уже есть в mlcc для `'` и `` ` `` |
| F2 match do/end | 2→8, семантика без изменений | Только парсер и AST |
| G1–G4 инструменты AST | параллельно Ruby и mlcc | Ниже — общий план |

### Инструменты над AST (рефакторинг и линтинг)

**Цель:** единые точки входа для анализа и точечных правок исходников без повторной имплементации обходов в каждом инструменте.

**Направления работ:**

1. **Контракт дерева.** Зафиксировать узлы AST mlcc (`compiler/ast.mlc`) и эталон Ruby (`lib/mlc/source/ast/nodes.rb`): именование полей, spans (`origin`/диапазоны), узлы объявлений и выражений — чтобы один алгоритм можно было переносить между реализациями или codegenить общий слой из описания.

2. **Traversal.** Выделить явный обход (visitor или функциональный fold по сумме узлов) с возможностью остановки на произвольной глубине; для mlcc — явный экспорт обходчиков из модулей рядом с checker/codegen; для Ruby — слой поверх существующих visitor-проходов, без смешения с SemanticIR там, где достаточно синтаксиса.

3. **Lint-слой.** Правила: только синтаксис (простые предупреждения), синтаксис + ограниченная семантика (имена, arity без полного codegen), полная семантика (повторное использование `checker`/Semantic Gen диагностик с кодами правил). Агрегатор диагностик — один формат (файл, строка, колонка, код, сообщение).

4. **Рефакторинг.** Операции «локально»: переименование символа с учётом областей видимости из checker/registry; правки `import`/`export`; механическая замена выражений при сохранении текстового интервала (text edits или целиком файл после pretty-print — решить политикой). Обязательный минимум — точное позиционирование узлов из парсера, чтобы не ломать комментарии без явного форматера.

5. **CLI и будущее IDE.** Отдельные подкоманды или бинарь (`analyze`, `lint`, позже `refactor rename`): вход — файл или модуль, выход — JSON диагностик или патчи. LSP остаётся надстройкой поверх тех же примитивов.

**Зависимости.** Стабильность checker/registry для переименований; для mlcc — уже есть пайплайн parse → expand → check. Ruby-путь — парсер + Semantic Gen при необходимости символов. Приоритет: **G1 → G2 (минимальные правила) → G3 (одно-две операции) → G4**, когда форма AST перестанет меняться чаще, чем раз в релиз инструментов.

### Правила

- Ruby-эталон не опережается. Если поведение расходится — эталон в Ruby.
- Каждый шаг — отдельный коммит с зелёными тестами.
- Фича не считается реализованной пока нет тестов на обоих уровнях (Ruby + mlcc).
