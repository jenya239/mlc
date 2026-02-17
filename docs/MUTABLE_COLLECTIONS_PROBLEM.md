# Проблема: Mutable Collections в MLC

Дата: 2026-02-17

## Контекст вопроса

Для self-hosted компилятора критично нужны mutable dynamic arrays (Vec<T>) для symbol tables, AST builders, и т.д. Но есть **концептуальные проблемы** с добавлением мутабельных коллекций в MLC.

## Текущее состояние массивов в MLC

### Реализация

**Runtime:**
- `T[]` → `std::vector<T>` напрямую (без обёртки)
- См. `runtime/include/mlc/core/collections.hpp`

**Семантика:**
- Массивы **НЕ** имеют move semantics
- Передача в функцию: копирование (как Copy type)
- Тест: `test_array_arg_not_moved_on_pass` (test/mlc/move_semantics_test.rb:144)

```mlc
fn foo() -> i32 = do
  let a = [1, 2, 3]
  let x = len(a)   // a не перемещается
  let y = len(a)   // можно использовать снова
  x + y
end
```

### Операции

**Работает:**
- Литералы: `[1, 2, 3]`
- Индексация: `arr[i]`
- Slicing: `arr[1:3]`
- HOF: `arr.map(f)`, `arr.filter(p)`, `arr.fold(acc, f)`
- Конкатенация: `arr ++ [elem]` (O(n) - создаёт новый массив)

**НЕ работает:**
- Мутабельные операции: `arr.push(elem)`, `arr.pop()`
- In-place модификация: `arr[i] = value`

### Документация vs реальность

**RUNTIME_OWNERSHIP_GUIDE.md утверждает:**
> String/Array/Map — managed контейнеры со внутренним refcount + copy-on-write (RC/COW)

**Реальность:**
- Это **design document**, не реальная реализация
- `std::vector<T>` не имеет RC/COW
- Копирование массива = полное копирование O(n)

---

## Концептуальные проблемы с Vec<T>

### Проблема 1: Array vs Vec - семантическая путаница

**Вопрос:** В чём разница между `T[]` и `Vec<T>`?

**Варианты ответа:**

#### Вариант A: Immutable vs Mutable (Haskell style)
```mlc
// Array - immutable, функциональный
let arr: i32[] = [1, 2, 3]
let arr2 = arr ++ [4]  // новый массив

// Vec - mutable, императивный
let mut vec: Vec<i32> = Vec.new()
vec.push(4)  // модификация in-place
```

**Проблема:** 
- Зачем тогда нужен `let mut arr: i32[] = ...`?
- Два типа для одной сущности - сложность для пользователя

#### Вариант B: Fixed vs Dynamic (C++ style)
```mlc
// Array - fixed size (stack/inline)
let arr: [i32; 3] = [1, 2, 3]  // размер в типе

// Vec - dynamic size (heap)
let vec: Vec<i32> = Vec.new()
vec.push(4)
```

**Проблема:**
- MLC не поддерживает fixed-size arrays
- Текущий `T[]` уже dynamic (std::vector)

#### Вариант C: Unified (Ruby/Python style)
```mlc
// Только один тип - Array (mutable)
let mut arr = [1, 2, 3]
arr.push(4)  // модификация in-place
```

**Проблема:**
- Что делать с `let arr = ...` (immutable binding)?
- Как запретить мутацию для immutable?

---

### Проблема 2: Ownership и мутабельность

**Текущая модель:**
- `T[]` - Copy type (как i32)
- Передача в функцию = копирование
- Нет move semantics для массивов

**Если добавить Vec<T> с мутацией:**

```mlc
let mut vec = Vec.new()
vec.push(1)

fn modify(v: Vec<i32>) = v.push(2)  // OK?

modify(vec)  // vec перемещается или копируется?
vec.push(3)  // ERROR: use after move?
```

**Варианты:**

#### A) Vec<T> - Move type
- `Vec<T>` имеет move semantics (как `Owned<T>`)
- Передача в функцию = перемещение
- После передачи нельзя использовать

```mlc
let mut vec = Vec.new()
vec.push(1)

fn consume(v: Vec<i32>) -> i32 = v.len()

consume(vec)
vec.push(2)  // ERROR: use after move
```

**Проблема:** Неудобно для компилятора. Symbol table нужно передавать между функциями.

#### B) Vec<T> - Ref-counted (Shared)
- `Vec<T>` внутри использует `Shared<std::vector<T>>`
- Копирование дешёвое (RC++)
- Мутация видна везде

```mlc
let mut vec = Vec.new()
vec.push(1)

let vec2 = vec  // RC++, оба указывают на один буфер
vec2.push(2)

vec.len()  // 2 - видна мутация vec2
```

**Проблема:** Неожиданное поведение. Alias mutable state - источник багов.

#### C) Vec<T> - Unique ownership с borrowing
- `Vec<T>` - owned, но можно брать `&mut Vec<T>`
- Мутабельные ссылки эксклюзивны (Rust borrow checker)

```mlc
let mut vec = Vec.new()
vec.push(1)

fn modify(v: &mut Vec<i32>) = v.push(2)

modify(&mut vec)  // временный доступ
vec.push(3)  // OK - ownership вернулся
```

**Проблема:** Требует полноценный borrow checker (сложность).

---

### Проблема 3: Interior mutability и `let` vs `let mut`

**Текущая семантика:**
- `let x = ...` - immutable binding (x нельзя переприсвоить)
- `let mut x = ...` - mutable binding (x можно переприсвоить)

**Вопрос:** Что такое "мутация" для коллекции?

```mlc
let vec = Vec.new()  // immutable binding
vec.push(1)  // OK или ERROR?
```

**Варианты:**

#### A) Binding mutability = data mutability
```mlc
let vec = Vec.new()
vec.push(1)  // ERROR: vec immutable

let mut vec2 = Vec.new()
vec2.push(1)  // OK: vec2 mutable
```

**Проблема:** В Rust `let vec = Vec::new()` позволяет `vec.push()`, если `&mut self` доступен.

#### B) Interior mutability (RefCell style)
```mlc
let vec = Vec.new()  // внутри RefCell/Shared
vec.push(1)  // OK - interior mutability

// Мутация через runtime checks
```

**Проблема:** Runtime overhead, может паниковать.

#### C) Явный mutability в типе
```mlc
let vec: MutVec<i32> = Vec.new()  // мутабельный тип
vec.push(1)  // OK - тип позволяет

let arr: i32[] = [1,2,3]  // иммутабельный тип
arr.push(1)  // ERROR - тип не поддерживает
```

**Проблема:** Размножение типов (Vec, MutVec, ImmutVec?).

---

### Проблема 4: C++ backend mapping

**Варианты lowering для Vec<T>:**

#### A) `Vec<T>` → `std::vector<T>` (by value)
```cpp
std::vector<int> vec;
vec.push_back(1);

foo(vec);  // копирование!
```
**Проблема:** Копирование дорогое, нужно std::move явно.

#### B) `Vec<T>` → `std::shared_ptr<std::vector<T>>`
```cpp
auto vec = std::make_shared<std::vector<int>>();
vec->push_back(1);

foo(vec);  // дешёвое копирование Shared ptr
```
**Проблема:** Всегда heap allocation, overhead RC.

#### C) `Vec<T>` → `std::vector<T>` с move semantics
```cpp
std::vector<int> vec;
vec.push_back(1);

foo(std::move(vec));  // перемещение
// vec теперь empty
```
**Проблема:** Требует borrow checker в MLC.

---

## Предыдущие обсуждения (из логов)

Проверка `misc/docs/chat_logs/` показывает:

1. **Обсуждение push() в closure capture:**
   - Проблема: `data.items.push(x)` в замыкании
   - Решение: capturing closure с ограничениями

2. **Обсуждение iterator invalidation:**
   - `items.push(4)` во время итерации запрещено
   - Требует borrow checking

3. **Обсуждение reference storage:**
   - `registry.items.push(ref item)` запрещено
   - `registry.items.push(item)` OK если ownership передан

**Вывод:** Мутабельные коллекции обсуждались, но не реализованы из-за сложности ownership.

---

## Решения из других языков

### Rust
- `Vec<T>` - owned, move by default
- `&mut Vec<T>` - mutable borrow (эксклюзивный)
- Borrow checker проверяет lifetime

**Для MLC:** Слишком сложно для первой версии.

### OCaml
- `list` - immutable, O(1) cons
- `Array.t` - mutable, но редко используется
- Компиляторы используют mutable state локально

**Для MLC:** Можно позаимствовать локальную мутабельность.

### Ruby
- `Array` - mutable, reference semantics
- GC управляет lifetime
- Нет статических проверок

**Для MLC:** Не подходит (статическая типизация).

### TypeScript
- `Array<T>` - mutable
- По ссылке в runtime
- Проверки типов, но не ownership

**Для MLC:** Можно позаимствовать синтаксис, но нужна ownership модель.

---

## Рекомендуемое решение

### Подход: Простой Vec<T> с ограничениями

**Идея:**
1. `Vec<T>` - новый тип (отдельно от `T[]`)
2. Внутри: `std::shared_ptr<std::vector<T>>` (RC)
3. Методы: `push`, `pop`, `get`, `set`, `len`
4. **Локальная мутабельность** - без передачи между потоками
5. Без полноценного borrow checker (пока)

### Семантика

```mlc
// Создание
let mut vec = Vec.new()  // внутри Shared<vector>

// Мутация (без mut binding - ERROR)
vec.push(42)  // OK

let vec2 = vec  // RC++, оба указывают на один буфер
vec2.push(100)  // оба видят изменения

// Передача в функцию
fn process(v: Vec<i32>) -> i32 = v.len()

process(vec)  // дешёвое копирование Shared ptr
vec.push(1)  // OK - vec всё ещё валиден
```

### Ограничения (для безопасности)

1. **Нет iterator invalidation проверок** (пока)
   - Документация: "не мутируйте во время итерации"
   - Undefined behavior если нарушено

2. **Нет проверки alias mutability** (пока)
   - `vec` и `vec2` могут мутировать общий буфер
   - Ответственность на программисте

3. **Только для локального использования**
   - Не отправлять между потоками (будущее: Send trait)

### Lowering в C++

```mlc
let mut vec = Vec.new()
vec.push(42)
let x = vec.get(0)
```

```cpp
auto vec = mlc::Vec<int>::make();  // std::shared_ptr<std::vector<int>>
vec->push_back(42);
auto x = vec->at(0);
```

### Реализация

**Runtime:**
```cpp
// runtime/include/mlc/collections/vec.hpp
namespace mlc {
  template<typename T>
  class Vec {
    std::shared_ptr<std::vector<T>> data_;
  public:
    static Vec<T> make() { return Vec{std::make_shared<std::vector<T>>()}; }
    void push(const T& value) { data_->push_back(value); }
    T get(size_t index) const { return data_->at(index); }
    size_t len() const { return data_->size(); }
  };
}
```

**Stdlib:**
```mlc
// lib/mlc/stdlib/collections/vec.mlc
type Vec<T> = extern

extend Vec<T> {
  extern fn new() -> Vec<T>
  extern fn push(self, value: T) -> unit
  extern fn get(self, index: i32) -> T
  extern fn len(self) -> i32
}
```

---

## Альтернатива: Persistent data structures

Если мутабельность слишком проблемна, можно использовать **persistent vectors** (Clojure style):

```mlc
let vec = PVec.empty()
let vec2 = vec.push(1)  // O(log n), structural sharing
let vec3 = vec2.push(2)
// vec, vec2, vec3 - все валидны
```

**Плюсы:**
- Нет alias mutability
- Безопасно для многопоточности
- Нет iterator invalidation

**Минусы:**
- O(log n) вместо O(1) для push
- Сложнее реализовать
- Не все операции эффективны (pop from middle)

---

## Выводы

### Концептуальные проблемы с Vec<T>:

1. **Семантическая путаница:** Array vs Vec
2. **Ownership:** Move vs Copy vs Shared
3. **Interior mutability:** `let` vs `let mut`
4. **Alias mutability:** Два binding на один буфер
5. **Iterator invalidation:** Мутация во время итерации
6. **Borrow checking:** Нужен для безопасности, но сложен

### Рекомендация:

**Для self-hosted (Tier 1):**
- Реализовать простой `Vec<T>` с RC (Shared<vector>)
- Документировать ограничения (нет borrow checker)
- Использовать **только локально** (symbol tables, builders)
- Не передавать через границы API

**Для будущего (Tier 2):**
- Добавить borrow checker (постепенно)
- Улучшить диагностику (iterator invalidation)
- Рассмотреть persistent structures как альтернативу

**Сложность реализации:**
- Tier 1 (простой Vec): ~1 сессия (runtime wrapper + stdlib)
- Tier 2 (borrow checker): ~5-10 сессий (очень сложно)

**Стратегия:**
1. Начать с простого Vec<T> (RC)
2. Написать mini-lexer/parser на MLC как валидацию
3. Зафиксировать найденные проблемы
4. Решить, нужен ли полноценный borrow checker
