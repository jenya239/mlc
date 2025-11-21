# MLC Language Strategy & Safety Model

**Дата**: 2025-11-20
**Версия**: 1.0

## Основополагающие Принципы

### 1. Близость к C++

**Принцип**: Язык MLC не должен далеко уходить от C++

**Обоснование**:
- Простота интеграции с существующими C++ библиотеками
- Предсказуемое поведение и производительность
- Низкий порог входа для C++ разработчиков
- Прозрачная компиляция в читаемый C++ код

**Следствия**:
- Совместимость ABI с C++
- Похожие идиомы и паттерны
- Аналогичная модель памяти (stack/heap)
- Zero-cost abstractions

### 2. Строгая Модель Безопасности

**Принцип**: При близости к C++ обеспечить строгие гарантии безопасности

**Обоснование**:
- Современные языки должны предотвращать целые классы ошибок
- Безопасность по умолчанию, unsafe - явно
- Compile-time проверки вместо runtime overhead

**Цели безопасности**:
- ✅ Нет dangling pointers
- ✅ Нет data races
- ✅ Нет use-after-free
- ✅ Нет null pointer dereference (через Option<T>)
- ✅ Bounds checking для массивов

---

## Ownership & Memory Safety Model

### Базовые Правила

**1. Неявный Move Semantics**

**Правило**: Все значения перемещаются (move) по умолчанию

```mlc
fn consume(data: Vec<i32>) -> i32 =
  data.length()

fn main() -> i32 =
  let v = [1, 2, 3]
  let len = consume(v)  // v перемещается, больше недоступен
  // v.length()  // ❌ ОШИБКА: use after move
  len
```

**Lowering в C++**:
```cpp
int consume(std::vector<int> data) {  // Принимает by-value (move)
  return data.size();
}

int main() {
  auto v = std::vector{1, 2, 3};
  int len = consume(std::move(v));  // Явный move в C++
  return len;
}
```

**Преимущества**:
- Нет неявного копирования больших объектов
- Явная семантика владения
- Zero-cost: move в C++ дешевый

**2. Явные Shared Указатели**

**Правило**: Разделяемое владение через `Shared<T>`

```mlc
fn share_data() -> i32 =
  let data = Shared::new([1, 2, 3])
  let ref1 = data.clone()  // Увеличивает reference count
  let ref2 = data.clone()
  ref1.deref().length() + ref2.deref().length()
```

**Lowering в C++**:
```cpp
int share_data() {
  auto data = std::make_shared<std::vector<int>>(std::vector{1, 2, 3});
  auto ref1 = data;  // shared_ptr copy
  auto ref2 = data;
  return ref1->size() + ref2->size();
}
```

**Ключевые моменты**:
- `Shared<T>` = `std::shared_ptr<T>` в C++
- `.clone()` увеличивает refcount (cheap)
- `.deref()` для доступа к данным
- Явная семантика: видно где разделяемое владение

**3. Явные Weak Указатели**

**Правило**: Слабые ссылки через `Weak<T>` для разрыва циклов

```mlc
type Node = {
  value: i32,
  parent: Weak<Node>  // Не владеет, может стать invalid
}

fn access_parent(node: Shared<Node>) -> Option<i32> =
  match node.deref().parent.upgrade()  // Weak -> Option<Shared>
    | Some(parent) => Some(parent.deref().value)
    | None => None
```

**Lowering в C++**:
```cpp
struct Node {
  int value;
  std::weak_ptr<Node> parent;
};

std::optional<int> access_parent(std::shared_ptr<Node> node) {
  if (auto parent = node->parent.lock()) {  // weak_ptr::lock()
    return parent->value;
  }
  return std::nullopt;
}
```

**Использование**:
- Для parent pointers в деревьях
- Для cache/observer patterns
- Для разрыва циклических ссылок

**4. Ссылки Только в Unsafe Блоках**

**Правило**: Raw references (`&T`, `&mut T`) запрещены в safe коде

**Safe код (ТОЛЬКО значения и смарт-поинтеры)**:
```mlc
fn safe_processing(data: Vec<i32>) -> i32 =  // Передача by-move
  data.length()

fn shared_processing(data: Shared<Vec<i32>>) -> i32 =  // Shared ownership
  data.deref().length()
```

**Unsafe блоки (можно использовать ссылки)**:
```mlc
fn unsafe_ref_access(data: Vec<i32>) -> i32 =
  unsafe {
    let ref: &Vec<i32> = &data  // Явное создание ссылки
    ref.length()
  }

fn unsafe_mut_ref(mut data: Vec<i32>) -> () =
  unsafe {
    let mut_ref: &mut Vec<i32> = &mut data
    mut_ref.push(42)
  }
```

**Обоснование**:
- References в C++ - источник dangling pointers
- В safe коде проще рассуждать о lifetimes
- Unsafe явно маркирует опасные участки
- 90% кода может быть safe

**Исключение: Методы**

Методы могут принимать `self`, `&self`, `&mut self` неявно:

```mlc
type Counter = { count: i32 }

impl Counter {
  fn increment(&mut self) -> () =  // &mut self - неявная ссылка
    self.count = self.count + 1

  fn get(&self) -> i32 =  // &self - неявная ссылка
    self.count
}
```

Это безопасно потому что:
- Методы вызываются на владельце или ссылке
- Lifetime привязан к receiver
- Компилятор проверяет корректность

---

## Типы Владения

### Value Types (Owned)

**Описание**: Единственный владелец, move по умолчанию

**Примеры**:
```mlc
let v = [1, 2, 3]           // Vec<i32>, owned
let s = "hello"             // String, owned
let p = Point { x: 1, y: 2 } // Record, owned
```

**Операции**:
- ✅ Move (неявно)
- ✅ Explicit clone (через `.clone()` если реализован Clone trait)
- ❌ Implicit copy (только для Copy types: i32, f32, bool)

### Shared<T> (Shared Ownership)

**Описание**: Множественное владение, reference counting

**Создание**:
```mlc
let shared = Shared::new(data)
let another = shared.clone()  // Увеличивает refcount
```

**Доступ**:
```mlc
let value = shared.deref()  // Получить &T (в unsafe) или копию
```

**Lifetime**: Данные живут пока есть хотя бы один Shared<T>

### Weak<T> (Non-Owning Reference)

**Описание**: Не владеет, может стать invalid

**Создание**:
```mlc
let shared = Shared::new(data)
let weak = shared.downgrade()  // Shared<T> -> Weak<T>
```

**Доступ**:
```mlc
match weak.upgrade()  // Weak<T> -> Option<Shared<T>>
  | Some(shared) => // Данные живы
  | None => // Данные уже удалены
```

**Lifetime**: Не продлевает жизнь объекта

---

## Borrowing Rules (для unsafe блоков)

Когда используются ссылки в unsafe блоках, применяются Rust-like правила:

### Правило 1: Одна Mutable ИЛИ Много Immutable

```mlc
unsafe {
  let data = [1, 2, 3]
  let ref1 = &data  // ✅ Immutable borrow
  let ref2 = &data  // ✅ Ещё одна immutable

  // let mut_ref = &mut data  // ❌ ОШИБКА: уже есть immutable borrows
}
```

```mlc
unsafe {
  let mut data = [1, 2, 3]
  let mut_ref = &mut data  // ✅ Mutable borrow

  // let ref = &data  // ❌ ОШИБКА: уже есть mutable borrow
}
```

### Правило 2: Borrows Не Переживают Владельца

```mlc
fn dangling_ref() -> &Vec<i32> =  // ❌ ОШИБКА: возвращает dangling ref
  unsafe {
    let data = [1, 2, 3]
    &data  // data удаляется при выходе из функции
  }
```

### Правило 3: No Aliasing для Mutable

```mlc
unsafe {
  let mut data = [1, 2, 3]
  let ref1 = &mut data
  // let ref2 = &mut data  // ❌ ОШИБКА: только одна &mut
}
```

---

## Интеграция с C++

### FFI Boundary

**C++ функции в MLC** (через extern):

```mlc
extern "C++" {
  fn cpp_process(data: Shared<Vec<i32>>) -> i32
}

fn call_cpp() -> i32 =
  let data = Shared::new([1, 2, 3])
  cpp_process(data)  // Передаём shared_ptr
```

**Lowering**:
```cpp
extern "C" int cpp_process(std::shared_ptr<std::vector<int>> data);

int call_cpp() {
  auto data = std::make_shared<std::vector<int>>(std::vector{1, 2, 3});
  return cpp_process(data);
}
```

### Raw Pointers для C API

Для интеграции с C используем raw pointers (только в unsafe):

```mlc
extern "C" {
  fn c_function(ptr: *const i32) -> i32
}

fn call_c(data: Vec<i32>) -> i32 =
  unsafe {
    let ptr = data.as_ptr()  // Vec<T> -> *const T
    c_function(ptr)
  }
```

---

## Copy vs Move Types

### Copy Types (Implicit Copy)

Типы реализующие `Copy` копируются неявно:

```mlc
let x: i32 = 42
let y = x  // Копируется (i32 implements Copy)
let z = x  // x всё ещё валиден
```

**Built-in Copy types**:
- Все примитивы: `i8`, `i16`, `i32`, `i64`, `f32`, `f64`, `bool`
- Указатели: `*const T`, `*mut T`
- References: `&T` (если в unsafe)

### Move Types (Explicit Clone)

Все остальные типы перемещаются:

```mlc
let v = [1, 2, 3]  // Vec<i32>
let v2 = v.move()  // Явный move (можно просто v)
// v недоступен

let v3 = v2.clone()  // Явное копирование (если Clone реализован)
```

---

## Implementation Timeline

### Phase 0: Current State (Implicit Shared)

**Сейчас**:
- Всё передаётся by-value в C++
- Нет явной модели владения
- Нет проверок lifetime

### Phase 1: Explicit Shared (Priority: HIGH)

**Цель**: Добавить `Shared<T>` и `Weak<T>`

**Задачи**:
1. Определить stdlib types `Shared<T>`, `Weak<T>`
2. Синтаксис создания: `Shared::new(value)`
3. Методы: `.clone()`, `.deref()`, `.downgrade()`, `.upgrade()`
4. Lowering в `std::shared_ptr` и `std::weak_ptr`
5. Тесты и примеры

**Время**: 2-3 сессии

### Phase 2: Move Semantics Analysis (Priority: HIGH)

**Цель**: Track moved values и запретить use-after-move

**Задачи**:
1. Move tracking в semantic analyzer
2. Обнаружение use-after-move ошибок
3. `.clone()` для explicit copy
4. Error messages для moved values

**Время**: 3-4 сессии

### Phase 3: Unsafe Blocks (Priority: MEDIUM)

**Цель**: Ограничить raw references

**Задачи**:
1. `unsafe { }` блоки синтаксис
2. Запретить `&T`, `&mut T` вне unsafe
3. Разрешить в методах (`&self`, `&mut self`)
4. Borrowing rules checker для unsafe блоков

**Время**: 4-5 сессий

### Phase 4: Lifetime Annotations (Priority: LOW)

**Цель**: Явные lifetimes для сложных случаев

**Задачи**:
1. Lifetime syntax: `fn foo<'a>(x: &'a T) -> &'a T`
2. Lifetime inference где возможно
3. Lifetime checking в borrowing rules

**Время**: 5-7 сессий

---

## Design Decisions

### Q: Почему неявный move вместо явного?

**A**:
- Move дешевле copy для большинства типов
- Принуждает думать о владении
- Явное `.clone()` показывает где дорогие операции
- Соответствует Rust best practices

### Q: Почему ссылки только в unsafe?

**A**:
- References - главная причина UB в C++
- Safe код проще анализировать
- Большинство кода может быть safe
- Явная маркировка опасных участков

### Q: Зачем явные Shared/Weak если есть move?

**A**:
- Некоторые структуры данных требуют shared ownership (графы, деревья с parent pointers)
- Явное разделение single vs shared ownership
- Видно где потенциальные refcount overhead

### Q: Совместимость с C++ кодом?

**A**:
- MLC Shared<T> = C++ shared_ptr<T> (ABI compatible)
- Можно передавать через FFI
- Zero-cost abstraction
- Читаемый сгенерированный код

---

## Examples

### Example 1: Ownership Transfer

```mlc
fn process(data: Vec<i32>) -> i32 =
  data.length()

fn main() -> i32 =
  let v = [1, 2, 3, 4, 5]
  process(v)  // v moved into process
  // v.length()  // ❌ ОШИБКА: v moved
```

### Example 2: Shared Ownership

```mlc
fn share_between_threads() -> i32 =
  let data = Shared::new([1, 2, 3])
  let copy1 = data.clone()  // Refcount++
  let copy2 = data.clone()  // Refcount++

  // Можем использовать все три
  data.deref().length() + copy1.deref().length()
```

### Example 3: Weak References (Tree)

```mlc
type Node = {
  value: i32,
  children: Vec<Shared<Node>>,
  parent: Weak<Node>
}

fn make_tree() -> Shared<Node> =
  let root = Shared::new(Node {
    value: 0,
    children: [],
    parent: Weak::new()
  })

  let child = Shared::new(Node {
    value: 1,
    children: [],
    parent: root.downgrade()  // Weak reference к parent
  })

  root  // Children держат Shared, parent держит Weak - нет цикла
```

### Example 4: Unsafe References

```mlc
fn efficient_scan(data: Vec<i32>, target: i32) -> bool =
  unsafe {
    let ref = &data  // Borrow вместо move
    for item in ref do
      if item == target then return true
    false
  }
```

### Example 5: Method Syntax

```mlc
type Buffer = {
  data: Vec<u8>
}

impl Buffer {
  fn push(&mut self, byte: u8) -> () =  // &mut self - safe
    self.data.push(byte)

  fn len(&self) -> i32 =  // &self - safe
    self.data.length()
}

fn use_buffer() -> i32 =
  let mut buf = Buffer { data: [] }
  buf.push(42)  // Вызов метода с &mut self
  buf.len()     // Вызов метода с &self
```

---

## Benefits

### Для Разработчиков

1. **Безопасность**: No dangling pointers, no use-after-free
2. **Производительность**: Zero-cost abstractions, явные дорогие операции
3. **Понятность**: Явная семантика владения
4. **Совместимость**: Прямое соответствие C++ концептам

### Для Проекта

1. **Unique Selling Point**: "Safe C++ without GC"
2. **Простота интеграции**: Shared<T> = shared_ptr<T>
3. **Читаемый вывод**: Понятный сгенерированный C++ код
4. **Постепенная миграция**: Можно начать без unsafe, добавить позже

---

## References

**Inspiration**:
- Rust ownership model
- C++ smart pointers
- Herb Sutter's C++ lifetime safety proposals

**Key Differences from Rust**:
- Ссылки по умолчанию запрещены (вместо разрешённых с проверкой)
- Явный Shared<T> (вместо неявного через lifetime)
- Ближе к C++ идиомам

**Key Differences from C++**:
- Move по умолчанию (вместо copy)
- Явные smart pointers always
- Compile-time проверки владения
