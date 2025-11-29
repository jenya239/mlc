# Сессия 2025-11-25: Синтаксис умных указателей

## Контекст
Продолжение работы над системой типов указателей после завершения array pattern matching.

## Проблема
Документация (pointer_types_demo.mlc) использовала синтаксис `::` для статических методов:
- `Shared::new()`, `Shared::null()`, `Shared::downgrade()`
- `Owned::new()`
- `ref T`, `ref mut T` для ссылок

## Решение

### 1. Удален синтаксис `ref` (keyword)
**Файлы:**
- `lib/mlc/source/parser/lexer.rb:30-34` - удален `ref` из KEYWORDS
- `lib/mlc/source/parser/type_parser.rb` - удален метод `parse_reference_type`
- `test/mlc/safety_model_test.rb` - обновлены все тесты

**Заменен на:**
- `Ref<T>` - иммутабельная ссылка → `const T&`
- `RefMut<T>` - мутабельная ссылка → `T&`

### 2. Добавлена поддержка в TypeMapper
**Файл:** `lib/mlc/backends/cpp/services/utils/type_mapper.rb`

```ruby
REFERENCE_TYPES = {
  "Ref" => :immutable,
  "RefMut" => :mutable
}.freeze
```

Маппинг:
- `Ref<T>` → `const T&`
- `RefMut<T>` → `T&`

### 3. Удален синтаксис `::`
**Файл:** `misc/examples/pointer_types_demo.mlc`

Заменено:
- `Shared::new()` → `Shared.new()`
- `Shared::null()` → `Shared.null()`
- `Shared::downgrade()` → `Shared.downgrade()`
- `Owned::new()` → `Owned.new()`
- `Option::map()` → `.map()` (метод экземпляра)
- `Option::unwrap_or()` → `.unwrap_or()`

## Текущее состояние

### Реализовано:
- ✅ Generic типы: `Shared<T>`, `Owned<T>`, `Weak<T>`, `Ref<T>`, `RefMut<T>`
- ✅ Маппинг в C++ типы через TypeMapper
- ✅ Документация обновлена
- ✅ Все тесты проходят (1829 runs, 4725 assertions, 0 failures)

### НЕ реализовано:
- ❌ Синтаксис `Type.method()` - компилятор не поддерживает
- Ошибка: "Unknown identifier 'Shared'" при попытке `Shared.new(42)`

## Необходимо решить

**Вариант 1:** Использовать обычные функции в stdlib
- `shared_new()`, `shared_null()`, `shared_downgrade()`
- `owned_new()`

**Вариант 2:** Реализовать `Type.method()` синтаксис
- Добавить member rule для типов как "namespace"
- Преобразовывать `Shared.new(x)` → `shared_new(x)` на этапе semantic analysis

## Изменённые файлы
- `lib/mlc/backends/cpp/services/utils/type_mapper.rb`
- `lib/mlc/source/parser/lexer.rb`
- `lib/mlc/source/parser/type_parser.rb`
- `test/mlc/safety_model_test.rb`
- `misc/examples/pointer_types_demo.mlc`

---

## Решение: Трейты для статических методов

### Обоснование

Для поддержки синтаксиса `Type.method()` (например `Shared.new(42)`) необходима система трейтов с ассоциированными функциями. Это даёт:
- Полиморфизм без наследования
- Статические методы для типов (фабрики, утилиты)
- Хороший маппинг на C++ шаблоны/концепты

### Дизайн-решения

1. **Новые ключевые слова**: `trait`, `extend`
2. **Синтаксис**:
   ```mlc
   trait Memory<T> {
     fn new(value: T) -> Self      // ассоциированная функция (без @)
     fn null() -> Self
   }

   extend Shared<T> : Memory<T> {
     fn new(value: T) -> Shared<T> = ...
     fn null() -> Shared<T> = ...
   }
   ```
3. **Вызов через точку**: `Type.func()` - ассоциированная, `value.func()` - экземплярная
4. **Резолюция**: парсер различает Type (заглавная буква) vs переменная

### Архитектура реализации

**Уровни изменений:**

1. **Парсер/AST**:
   - `trait ... { fn ... }` - TraitDecl
   - `extend Type : Trait { fn ... }` - ExtendDecl
   - Различать ассоциированные (fn без @) и методы (@)

2. **Регистры**:
   - TraitRegistry: trait → методы/ассоц.функции
   - ImplRegistry: (Type, Trait) → реализации

3. **Типовая система**:
   - Bounds: `T: Trait`
   - Проверка полноты реализации
   - Резолюция `Type.method()` через ImplRegistry

4. **IR/Semantic**:
   - TraitMethodCall узел
   - Привязка к конкретной реализации

5. **C++ Backend**:
   - Статический dispatch через шаблоны
   - Ассоц. функции → namespace/static методы

### План реализации

**Фаза 1: Парсинг**
- [ ] Добавить `trait`, `extend` в KEYWORDS (lexer.rb)
- [ ] Создать TraitDecl, ExtendDecl AST nodes
- [ ] Реализовать parse_trait_decl(), parse_extend_decl()

**Фаза 2: Регистрация**
- [ ] TraitRegistry для хранения trait определений
- [ ] ImplRegistry для хранения реализаций

**Фаза 3: Типовая проверка**
- [ ] Проверка полноты реализации trait
- [ ] Резолюция Type.method() через registry
- [ ] Bounds checking для generic параметров

**Фаза 4: IR генерация**
- [ ] TraitMethodCall → конкретный вызов
- [ ] Мономорфизация при известном типе

**Фаза 5: C++ кодген**
- [ ] Ассоц. функции как static/namespace
- [ ] Template specialization для bounds

---

## Реализовано (2025-11-25)

### Фаза 1: Парсинг ✅
- Добавлены `trait`, `extend` в KEYWORDS (`lexer.rb:33`)
- Созданы `TraitDecl`, `TraitMethod`, `ExtendDecl` AST nodes (`nodes.rb:712-754`)
- Реализованы `parse_trait_decl()`, `parse_extend_decl()` (`declaration_parser.rb:312-447`)

### Фаза 2: Регистрация ✅
- Создан `TraitRegistry` (`services/traits/trait_registry.rb`)
- Добавлен в Container (`container.rb:63`)
- Регистрация трейтов и реализаций в `pass_preregister_traits` (`program_reducer.rb:81-139`)

### Фаза 3: Резолюция Type.method() ✅
- Создан `StaticMethodCallRule` (`rules/static_method_call_rule.rb`)
- Добавлена проверка `static_method_call?` в `expression_visitor.rb:123-142`
- `Type.method(args)` → `Type_method(args)` mangling

### Тестирование

**Пример кода:**
```mlc
type Point = { x: i32, y: i32 }

extend Point {
  fn origin() -> Point = Point { x: 0, y: 0 }
}

fn test() -> Point = Point.origin()
```

**Сгенерированный C++:**
```cpp
struct Point {int x;int y;};
constexpr Point Point_origin() noexcept{return Point{0, 0};}
constexpr Point test() noexcept{return Point_origin();}
```

### Результаты
- Все тесты проходят: 1829 runs, 4725 assertions, 0 failures
- Синтаксис `Type.method()` работает для extend-блоков

### Фаза 4: Дополнительные проверки ✅

**Проверка полноты реализации:**
- Добавлена `validate_trait_implementation` в `program_reducer.rb:146-168`
- При неполной реализации выдаётся ошибка с указанием недостающих методов

**Пример ошибки:**
```
Incomplete trait implementation: Point does not implement
required method(s) null from trait Memory at <input>:8:1
```

**Дефолтные реализации:**
- Методы с телом в trait пропускаются при проверке полноты
- Работает из коробки через `trait_method[:body]` check

**Trait bounds (`T: Trait`):**
- Парсинг уже поддерживается в `type_parser.rb:229-237`
- TypeParam хранит constraint: `{ name: "T", constraint: "Display" }`
