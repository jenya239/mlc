# Mutable Collections: анализ и рекомендация

Дата: 2026-02-17

## 1. Текущая реализация vs design documents

### Реализация (факт, обновлено 2026-02-17)

| Свойство | Состояние |
|----------|-----------|
| `T[]` lowering | `mlc::Array<T>` — COW wrapper над `std::vector<T>` |
| `Map<K,V>` lowering | `mlc::HashMap<K,V>` — COW wrapper над `std::unordered_map<K,V>` |
| Ownership | Copy type (COW, не move) |
| `.push()/.pop()/.set()/.get()` | Реализовано, мутирующие методы требуют `let mut` |
| RC/COW | Реализовано: `mlc::Array<T>`, `mlc::HashMap<K,V>` |
| `let mut arr` | Позволяет мутировать содержимое через `.push()`, `.set()`, `.pop()` |
| Конкатенация | `arr ++ [elem]` → новый массив; `.push()` → in-place (COW) |
| Mutability check | Компилятор проверяет `let mut` для мутирующих методов |
| impl blocks | Не реализованы |
| borrow checker | Не реализован (COW делает его ненужным для value types) |

Проверено:
- `test/mlc/array_mutation_test.rb`: 13 unit тестов для push/pop/set/get + mutability checks
- `test/integration/array_cow_e2e_test.rb`: 8 E2E тестов COW семантики
- `test/integration/hashmap_e2e_test.rb`: 8 E2E тестов HashMap
- `runtime/include/mlc/core/array.hpp`: COW Array<T> с detach при мутации
- `runtime/include/mlc/core/hashmap.hpp`: COW HashMap<K,V>

### LANGUAGE_STRATEGY.md (design)

```
Правило: Все значения перемещаются (move) по умолчанию
let v = [1, 2, 3]        // Vec<i32>, owned
consume(v)                // v перемещается
// v.length()             // ❌ use after move
```

```
impl Buffer {
  fn push(&mut self, byte: u8) -> () =
    self.data.push(byte)
}
```

### RUNTIME_OWNERSHIP_GUIDE.md (design)

```
String/Array/Map — managed контейнеры со внутренним refcount + copy-on-write
arr2.push(4)   // COW, arr2 теперь со своим буфером
```

### CLAUDE_DIALOG_SUMMARY.md (обсуждения)

Строки 31419-31430: COW как решение для мутабельных структур.
Строки 30781-30795: RC для иммутабельных record с shared ownership.
Строки 42814-42855: Interior mutability пробивает иммутабельность ("дыры").
Строки 30483-30493: "Отсутствие мутирующих замыканий устраняет целый класс проблем."
Строки 50455-50465: "MLC делает выбор в пользу простоты реализации."

**Вывод:** Три документа описывают три разных модели:
1. LANGUAGE_STRATEGY: Rust-like move semantics + impl blocks
2. RUNTIME_OWNERSHIP_GUIDE: Swift-like RC/COW value types
3. Chat logs: "простота, без borrow checker, без мутирующих замыканий"

---

## 2. Три возможных решения

### Решение A: Rust-like (LANGUAGE_STRATEGY.md)

```mlc
let mut buf = [1, 2, 3]
buf.push(4)              // &mut self method
consume(buf)             // move
// buf.push(5)           // ERROR: use after move
```

**Требует:**
- impl blocks (парсер + lowering)
- `&mut self` в методах
- Move semantics для массивов (сломает текущее поведение)
- Borrow checker (хотя бы упрощённый)

**Сложность:** 5-8 сессий
**Риск:** Высокий. Borrow checker — одна из самых сложных частей любого компилятора.

### Решение B: Swift-like COW (RUNTIME_OWNERSHIP_GUIDE.md)

```mlc
let mut arr = [1, 2, 3]
arr.push(4)              // мутация in-place (единственный владелец)

let arr2 = arr           // RC++, разделяют буфер
arr.push(5)              // COW: arr получает свою копию буфера
```

**Требует:**
- `mlc::Array<T>` wrapper в runtime (RC + COW над `std::vector<T>`)
- Method dispatch для `.push()`, `.pop()`, `.get()`, `.set()`
- Проверка: `.push()` только на `let mut` bindings
- НЕ требует borrow checker
- НЕ требует impl blocks (можно через extend)

**Сложность:** 2-3 сессии
**Риск:** Средний. COW реализация нетривиальна но предсказуема.

### Решение C: Простое (прагматичное)

```mlc
let mut arr = [1, 2, 3]
arr.push(4)              // → arr.push_back(4) в C++
```

**Требует:**
- Method dispatch для `.push()` на массивах
- Проверка: `.push()` только на `let mut`
- Lowering: `arr.push(x)` → `arr.push_back(x)` (уже частично есть в call_rule.rb:343)

**Сложность:** 0.5-1 сессия
**Риск:** Низкий. Но семантика копирования при передаче в функцию дорогая (O(n)).

---

## 3. Рекомендация: Решение B (Swift-like COW)

### Обоснование

1. **Согласуется с RUNTIME_OWNERSHIP_GUIDE.md** — документ уже описывает эту модель.

2. **Не требует borrow checker.** Из логов чатов (строка 30493):
   > "Отсутствие мутирующих замыканий устраняет целый класс проблем,
   > позволяя реализовать функции как простые value types...
   > без необходимости в сложных механизмах borrow checking"

3. **Предсказуемая семантика.** Swift доказал, что value types + COW работают.
   - `let` = нельзя мутировать (binding + содержимое)
   - `let mut` = можно мутировать
   - Передача в функцию = логическая копия (COW делает дешёвой)

4. **Не сломает текущий код.** Массивы и сейчас Copy. COW — это оптимизация копирования.

5. **Достаточно для self-hosted.** Компилятор:
   - Строит AST: `let mut nodes = []; nodes.push(node)` — локальная мутация
   - Symbol table: `let mut table = HashMap.new(); table.set(name, ty)` — локальная мутация
   - Передаёт между фазами: по значению (COW делает дешёвым)

6. **Замыкания безопасны.** Принцип "нет мутирующих замыканий" сохраняется:
   ```mlc
   let mut arr = [1, 2, 3]
   let f = |x| arr.push(x)  // ERROR: замыкание захватывает mut binding
   ```
   Замыкания могут захватывать только `let` (иммутабельные) bindings.

### Семантика

```mlc
// Создание
let mut arr = [1, 2, 3]

// Мутация (только для let mut)
arr.push(4)           // OK
arr.set(0, 10)        // OK: arr[0] = 10

let arr2 = [1, 2, 3]
arr2.push(4)          // ERROR: arr2 is immutable

// Копирование (COW)
let mut a = [1, 2, 3]
let b = a             // RC++, разделяют буфер, b иммутабельный
a.push(4)             // COW: a получает свою копию, b не меняется
// a == [1, 2, 3, 4], b == [1, 2, 3]

// Передача в функцию
fn sum(arr: i32[]) -> i32 = arr.fold(0, |a, b| a + b)

let mut data = [1, 2, 3]
let s = sum(data)     // data передаётся как COW-копия
data.push(4)          // OK: data всё ещё валиден

// Замыкания
let arr = [1, 2, 3]
let f = |x| arr.length() + x  // OK: arr захвачен как immutable (COW copy)
```

### Runtime реализация

```cpp
// runtime/include/mlc/core/array.hpp
namespace mlc {

template<typename T>
class Array {
    struct Buffer {
        std::vector<T> data;
        std::atomic<size_t> refcount{1};
    };
    Buffer* buf_;

    void detach() {
        if (buf_->refcount > 1) {
            buf_->refcount--;
            buf_ = new Buffer{buf_->data, 1};  // COW copy
        }
    }

public:
    // Конструкторы
    Array() : buf_(new Buffer{{}, 1}) {}
    Array(std::initializer_list<T> init) : buf_(new Buffer{std::vector<T>(init), 1}) {}
    
    // Copy: RC++
    Array(const Array& other) : buf_(other.buf_) { buf_->refcount++; }
    Array& operator=(const Array& other) {
        if (this != &other) {
            release();
            buf_ = other.buf_;
            buf_->refcount++;
        }
        return *this;
    }
    
    // Move
    Array(Array&& other) noexcept : buf_(other.buf_) { other.buf_ = nullptr; }

    ~Array() { release(); }

    // Immutable operations (no detach)
    size_t size() const { return buf_->data.size(); }
    const T& operator[](size_t i) const { return buf_->data[i]; }
    bool empty() const { return buf_->data.empty(); }

    // Mutable operations (detach = COW)
    void push(const T& value) { detach(); buf_->data.push_back(value); }
    void set(size_t i, const T& value) { detach(); buf_->data[i] = value; }
    T pop() { detach(); T v = buf_->data.back(); buf_->data.pop_back(); return v; }

    // Iteration (immutable, no detach)
    auto begin() const { return buf_->data.begin(); }
    auto end() const { return buf_->data.end(); }

private:
    void release() {
        if (buf_ && --buf_->refcount == 0) delete buf_;
    }
};

} // namespace mlc
```

### Compiler изменения

**1. Type inference для method calls на массивах:**

Файл: `lib/mlc/representations/semantic/gen/services/inference/type_inference_service.rb`

Добавить в section для array methods:
- `push(T) -> unit` — требует mut binding
- `pop() -> T` — требует mut binding
- `set(i32, T) -> unit` — требует mut binding
- `get(i32) -> T` — не требует mut

**2. Mutability check:**

При вызове `.push()` / `.pop()` / `.set()` на массиве — проверить, что binding объявлен как `let mut`.

Файл: `lib/mlc/representations/semantic/gen/services/scope/var_type_registry.rb`
- Уже хранит `mutable` flag для каждой переменной

**3. Codegen:**

Файл: `lib/mlc/backends/cpp/rules/expressions/call_rule.rb`
- Строка 343: уже есть `arr.push(x) -> arr.push_back(x)`
- Добавить: `arr.pop()`, `arr.set(i, v)`, `arr.get(i)`

**4. Runtime:**

- Заменить `std::vector<T>` на `mlc::Array<T>` в type_mapper
- Или: использовать `std::vector<T>` напрямую (без COW) как первый шаг

### План реализации (поэтапный)

**Этап 1: Простая мутация без COW (0.5 сессии)**
- Добавить `.push()`, `.pop()`, `.set()` в type inference
- Проверка mutability (только `let mut`)
- Codegen: `arr.push_back(x)` в C++
- Массивы остаются `std::vector<T>` (копирование при передаче)

**Этап 2: COW wrapper (1 сессия)**
- Создать `mlc::Array<T>` в runtime
- Заменить `std::vector<T>` на `mlc::Array<T>` в type_mapper
- Копирование становится O(1) (RC++)
- Мутация: detach + modify

**Этап 3: HashMap (1 сессия)**
- `mlc::HashMap<K,V>` аналогично Array
- Методы: `get`, `set`, `contains`, `keys`, `values`
- COW wrapper над `std::unordered_map`

---

## 4. Сравнение с другими языками

| Свойство | MLC (предложение) | Swift | Rust | OCaml |
|----------|-------------------|-------|------|-------|
| Array тип | Value + COW | Value + COW | Owned + Move | Immutable list |
| Мутация | `let mut` + `.push()` | `var` + `.append()` | `let mut` + `.push()` | Нет (Array.t — мутабельный) |
| Передача | Copy (COW) | Copy (COW) | Move | Immutable share |
| Borrow checker | Нет | Нет | Да | Нет |
| Замыкания | Capture immutable only | Capture copy | Capture move/borrow | Capture immutable |
| Безопасность | Compile-time mut check | Compile-time let/var | Compile-time borrow | Type system |

**MLC ближе всего к Swift** по семантике коллекций.
Это не случайно — Swift решал ту же задачу: статическая типизация + компиляция в нативный код + без GC + без borrow checker.

---

## 5. Ответы на концептуальные вопросы

**Q: Не будет ли путаницы Array vs Vec?**
A: Не будет. Один тип: `T[]`. Никакого отдельного `Vec<T>`. Это массив с COW.

**Q: Что с alias mutability (`let b = a; a.push(x)`)?**
A: COW решает: `a.push(x)` делает detach, `b` не видит изменений. Это value semantics.

**Q: Что с iterator invalidation?**
A: COW решает: итератор держит ссылку на текущий буфер. `push()` делает detach → новый буфер. Старый буфер (с итератором) не меняется.

**Q: Что с замыканиями?**
A: Замыкания захватывают `let` (immutable) копии. COW делает захват дешёвым. Мутация внутри замыкания запрещена (нет `let mut` capture).

**Q: Что с передачей в функцию?**
A: Value semantics. Функция получает COW-копию. Если не мутирует — буфер разделяется (O(1)). Если мутирует — detach (O(n) один раз).

**Q: Что с производительностью?**
A: Для self-hosted компилятора достаточно. Локальная мутация (push в цикле) — O(1) amortized (единственный владелец, нет detach). Передача между фазами — O(1) если receiver не мутирует.
