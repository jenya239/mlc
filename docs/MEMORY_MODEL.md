# MLC Memory Model

## Философия

MLC компилируется в C++20 без GC и без borrow checker. Безопасность памяти достигается через три принципа:

1. **Value semantics** — всё передаётся по значению
2. **COW (Copy-on-Write)** — копирование коллекций O(1), мутация создаёт копию только при необходимости
3. **Capture by value** — замыкания всегда захватывают переменные по значению

Эта комбинация устраняет классы ошибок, требующие GC или borrow checker, сохраняя при этом удобный синтаксис в стиле Ruby/Kotlin.

---

## Типы данных

### Примитивы

`i32`, `i64`, `f64`, `bool`, `unit` — inline значения, без heap-аллокаций. Копирование тривиально.

### String

```mlc
let name = "hello"
let upper = name.upper()   // новая строка, name не изменилась
```

`mlc::String` — value type над `std::string`. Копирование по значению. Мутирующие методы (`.append()` и т.п.) возвращают новое значение, не изменяют оригинал.

### Array

```mlc
let arr = [1, 2, 3]
let arr2 = arr             // O(1) — shared buffer, refcount++
arr2.push(4)               // COW detach: arr2 получает свой буфер, arr не изменился
```

`mlc::Array<T>` — COW над `shared_ptr<vector<T>>`:
- Копирование: `shared_ptr` copy → O(1)
- Мутация при `use_count > 1` → deep copy buffer (detach)
- Мутация при `use_count == 1` → in-place, без копии
- RAII: буфер освобождается автоматически когда `use_count → 0`

### HashMap

```mlc
let m = HashMap.new()
let m2 = m.set("key", 1)   // m не изменился, m2 — обновлённая копия
```

`mlc::HashMap<K,V>` — COW над `shared_ptr<unordered_map<K,V>>`. Та же семантика что у Array.

### Shared\<T\> и Weak\<T\>

Для рекурсивных типов и явного разделения владения:

```mlc
type Node = { value: i32, children: [Shared<Node>] }

let root = Shared.new(Node { value: 1, children: [] })
```

`Shared<T>` → `std::shared_ptr<T>`. Используется для:
- Рекурсивных AST-узлов (дерево — ациклично, утечек нет)
- Явного разделения одного объекта между несколькими владельцами

`Weak<T>` → `std::weak_ptr<T>`. Используется для разрыва циклов:

```mlc
type Parent = { children: [Shared<Child>] }
type Child  = { parent: Weak<Parent> }   // обратная ссылка через Weak
```

---

## Система биндингов

### Три формы

| Синтаксис | Семантика | C++ |
|-----------|-----------|-----|
| `let x = expr` | rebindable — можно переприсвоить | `auto x = expr;` |
| `const x = expr` | non-rebindable — биндинг фиксирован | `auto x = expr;` (checker блокирует rebind) |
| `let const x = expr` | compile-time constant | `constexpr auto x = expr;` |

### Ключевое различие с Rust

В Rust `let` = immutable, `let mut` = mutable. Это про **мутацию значения**.

В MLC `let` vs `const` — только про **переприсваивание биндинга**. Мутация значения (`.push()` на массиве) разрешена в обоих случаях:

```mlc
const arr = []
arr.push(1)    // OK — мутируем содержимое, не биндинг
arr = []       // Error — нельзя переприсвоить const

let arr2 = []
arr2.push(1)   // OK
arr2 = []      // OK — можно переприсвоить
```

### Почему так

- В MLC нет aliasing mutable references (как в Rust), значит бессмысленно запрещать мутацию через `const`
- `const arr = []` + `arr.push(x)` — стандартный паттерн в JS/TS, хорошо понятен разработчикам
- Разграничение полезно: хочу убедиться что не переприсваиваю → `const`

### let const

```mlc
let const MAX_SIZE = 1024      // constexpr int MAX_SIZE = 1024;
let const PI = 3.14159         // constexpr double PI = 3.14159;
```

Ограничения: инициализатор должен быть compile-time вычислимым. Используется для именованных числовых констант и флагов.

---

## Замыкания

### Синтаксис

```mlc
fn double(x: i32) -> i32 = x * 2

let numbers = [1, 2, 3, 4, 5]
let doubled = numbers.map(fn x => x * 2)
let evens   = numbers.filter(fn x => x % 2 == 0)
let sum     = numbers.fold(0, fn acc x => acc + x)
```

### Правило захвата: всегда по значению

```mlc
let factor = 10
let transform = fn x => x * factor    // factor захвачен by value
// factor можно изменить — на transform не влияет
```

Генерируется C++ lambda с `[=]` capture:

```cpp
auto factor = 10;
auto transform = [=](int x) { return x * factor; };
```

### Почему не [&]

`[&]` (by reference) создаёт dangling reference если lambda переживает scope:

```cpp
// Опасно:
std::function<int(int)> make_adder(int n) {
    return [&n](int x) { return x + n; };  // n — dangling!
}

// Безопасно:
std::function<int(int)> make_adder(int n) {
    return [=](int x) { return x + n; };   // n скопирован
}
```

С COW копирование даже больших объектов дешёво:

```mlc
fn make_checker(allowed: [string]) -> (string -> bool) =
  fn word => allowed.contains(word)
// allowed скопирован в замыкание за O(1) через shared_ptr
```

### Блоки (do...end) — не замыкания

`do...end` блоки используются для последовательности операций в теле функции. Они компилируются в C++ IIFE (`[=]() { ... }()`) только когда используются как выражение (не как последняя строка функции). Захват — `[=]`.

---

## Отсутствие GC: почему достаточно

GC решает проблему неизвестного времени жизни. В MLC времени жизни явны:

| Ситуация | Решение без GC |
|----------|---------------|
| Функция возвращает массив | RAII — shared_ptr автоматически освобождает |
| Lambda захватывает локальную переменную | Capture by value — lambda владеет копией |
| Дерево AST-узлов | Shared<T> — дерево ациклично, нет утечек |
| Граф с обратными рёбрами | Weak<T> для обратных ссылок |
| `arr2 = arr` + `arr2.push(x)` | COW — безопасное разделение буфера |

**Что требует GC но отсутствует в MLC по дизайну:**
- Мутабельные ссылки на один объект из нескольких мест без копирования (заменяется COW)
- Циклические графы без явных `Weak` (документируется как ограничение)

---

## Известные ограничения

1. **Циклические структуры** требуют `Weak<T>`. Забытый `Weak` → утечка. Компилятор не детектирует.

2. **Thread safety**: `shared_ptr` reference counting — атомарный. Но COW detach (read + write Buffer) — не атомарный. Для concurrent код нужна явная синхронизация (не поддерживается пока).

3. **`let const` с runtime-значениями** → C++ compile error. Checker должен проверять.
