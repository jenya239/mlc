# Query System: Подробное объяснение

## TL;DR

**Query System** = компилятор как база данных с автоматическим кэшированием.

**Когда нужно**: Для incremental compilation и быстрых пересборок больших проектов.

**Когда НЕ нужно**: Пока кодовая база маленькая и компиляция быстрая (как сейчас у MLC).

**Статус для MLC**: Отложить до Phase 27+. Сначала доделать архитектуру (Phase 21-23).

---

## Что это?

Query System - это архитектурный паттерн из Rust компилятора (rustc), где компиляция организована как **база данных запросов** вместо последовательного pipeline.

### Традиционная архитектура (Pass-based)

```ruby
# Как работают большинство компиляторов (включая MLC сейчас)
source_code
  → Parser (Pass 1)
    → Type Checker (Pass 2)
      → IR Generator (Pass 3)
        → Optimizer (Pass 4)
          → Code Generator (Pass 5)

# Проблемы:
# 1. Все pass-ы выполняются всегда (даже если результат не нужен)
# 2. При изменении - пересчитываются ВСЕ pass-ы
# 3. Нет кэширования между компиляциями
# 4. Incremental compilation сложно добавить
```

### Query System архитектура

```ruby
# Компилятор = база данных с запросами (queries)
# Каждый query - чистая функция с кэшированием

query parse(file_id) -> AST           # "Распарсить файл"
query type_of(node_id) -> Type        # "Какой тип у node?"
query transform(node_id) -> IR        # "Преобразовать в IR"
query optimize(ir_id) -> OptimizedIR  # "Оптимизировать IR"

# Магия:
# 1. Queries вызывают друг друга
# 2. Результаты автоматически кэшируются
# 3. При изменении - пересчитываются только зависимые queries
```

---

## Как это работает

### Пример 1: Автоматическое кэширование

**Без Query System:**

```ruby
def compile_function(func_ast)
  type = infer_type(func_ast)          # 10ms
  ir = transform(func_ast, type)        # 20ms
  optimized = optimize(ir)              # 30ms
  code = codegen(optimized)             # 40ms
  code                                  # Total: 100ms
end

# Вызов 1
compile_function(foo)  # 100ms - вычислили всё

# Вызов 2 (с тем же foo)
compile_function(foo)  # 100ms - ВЫЧИСЛИЛИ ЗАНОВО! ❌
```

**С Query System:**

```rust
// Определяем queries (чистые функции)
query type_of(func_id: DefId) -> Type {
    infer_type(func_id)  // 10ms первый раз
}

query transform(func_id: DefId) -> IR {
    let ty = type_of(func_id);  // Взято из кэша!
    transform_to_ir(func_id, ty)  // 20ms
}

query optimize(func_id: DefId) -> OptimizedIR {
    let ir = transform(func_id);  // Взято из кэша!
    optimize_ir(ir)  // 30ms
}

// Вызов 1
optimize(foo)  // 100ms - вычислили всё

// Вызов 2 (с тем же foo)
optimize(foo)  // 0ms - ВЗЯТО ИЗ КЭША! ✅
```

### Пример 2: Incremental Compilation

**Ситуация:** Есть 1000 функций, изменили 1 функцию.

**Без Query System:**

```ruby
# Изменили функцию foo()
edit_function(foo)

# Компилятор перекомпилирует ВСЁ:
compile(func_001)  # Пересчитали (хотя не изменилась) ❌
compile(func_002)  # Пересчитали (хотя не изменилась) ❌
compile(func_003)  # Пересчитали (хотя не изменилась) ❌
# ...
compile(foo)       # Пересчитали (изменилась) ✅
# ...
compile(func_999)  # Пересчитали (хотя не изменилась) ❌

# Время: 100 секунд
```

**С Query System:**

```rust
// Изменили функцию foo()
edit_function(foo)
invalidate_query(type_of, foo)  // Инвалидировали только foo

// Компилятор пересчитывает только зависимые:
type_of(func_001)  // Из кэша ✅ (не зависит от foo)
type_of(func_002)  // Из кэша ✅ (не зависит от foo)
type_of(bar)       // Пересчитали ❌ (вызывает foo!)
type_of(foo)       // Пересчитали ❌ (изменилась)
type_of(func_999)  // Из кэша ✅ (не зависит от foo)

// Время: 1 секунда (100x быстрее!)
```

### Пример 3: Ленивые вычисления (Demand-driven)

**Без Query System:**

```ruby
# Компилируем программу с 3 функциями
def foo(): ...
def bar(): ...
def unused(): ...  # Никто не вызывает!

compile_program do
  compile(foo)     # Компилим ✅
  compile(bar)     # Компилим ✅
  compile(unused)  # Компилим ❌ (зря!)
end

# Скомпилировали 3 функции (хотя unused не нужна)
```

**С Query System:**

```rust
// Компилируем только main
query compile_program() -> Code {
    let main_code = compile_fn(main);  // Компилим main
    // main вызывает foo и bar, но НЕ вызывает unused
    main_code
}

query compile_fn(func_id: DefId) -> Code {
    // Вычисляется ТОЛЬКО если кто-то запросил!
}

// Результат:
// ✅ main - скомпилирован
// ✅ foo - скомпилирован (вызывается из main)
// ✅ bar - скомпилирован (вызывается из main)
// ❌ unused - НЕ скомпилирован (никто не запросил!)
```

---

## Граф зависимостей

Query System автоматически строит граф зависимостей между queries.

### Пример

```ruby
# У нас есть 3 файла
# file1.ml: def foo(): 42
# file2.ml: def bar(): foo() + 1
# file3.ml: def baz(): bar() * 2

# Query System строит граф:
type_of(baz)
  ↓ depends on
transform(baz)
  ↓ depends on
type_of(bar)
  ↓ depends on
transform(bar)
  ↓ depends on
type_of(foo)
  ↓ depends on
transform(foo)

# Изменили foo:
invalidate(transform, foo)

# Query System автоматически инвалидирует:
# ❌ transform(foo) - инвалидирован
# ❌ type_of(bar) - инвалидирован (зависит от foo)
# ❌ transform(bar) - инвалидирован (зависит от type_of(bar))
# ❌ type_of(baz) - инвалидирован (зависит от bar)
# ❌ transform(baz) - инвалидирован (зависит от type_of(baz))

# Изменили baz:
invalidate(transform, baz)

# Query System инвалидирует:
# ❌ transform(baz) - инвалидирован
# ✅ type_of(bar) - НЕ инвалидирован (baz не влияет на bar!)
# ✅ transform(bar) - НЕ инвалидирован
# ✅ type_of(foo) - НЕ инвалидирован
# ✅ transform(foo) - НЕ инвалидирован
```

---

## Ruby адаптация для MLC

### Простая имплементация

```ruby
module MLC
  class QuerySystem
    def initialize
      @cache = {}           # Кэш результатов
      @dependencies = {}    # Граф зависимостей
      @query_stack = []     # Стек для отслеживания вызовов
    end

    # Определить query
    def define_query(name, &compute_fn)
      define_singleton_method(name) do |*args|
        key = [name, args]

        # Если в кэше - вернуть
        if @cache.key?(key)
          puts "[CACHE HIT] #{name}(#{args.inspect})"
          return @cache[key]
        end

        # Запоминаем текущий query (для графа зависимостей)
        @query_stack.push(key)

        puts "[COMPUTING] #{name}(#{args.inspect})"
        result = compute_fn.call(*args, self)

        # Сохраняем в кэш
        @cache[key] = result

        # Записываем зависимости
        current = @query_stack.pop
        if !@query_stack.empty?
          parent = @query_stack.last
          @dependencies[parent] ||= []
          @dependencies[parent] << current
        end

        result
      end
    end

    # Инвалидировать query и всё что от него зависит
    def invalidate(query_name, *args)
      key = [query_name, args]
      @cache.delete(key)

      # Рекурсивно инвалидировать dependents
      @dependencies.each do |parent_key, deps|
        if deps.include?(key)
          invalidate(*parent_key)
        end
      end

      @dependencies.delete(key)
    end
  end
end
```

### Использование

```ruby
# Создаём Query System
queries = MLC::QuerySystem.new

# Определяем queries
queries.define_query(:parse) do |filename, qs|
  puts "  → Parsing #{filename}..."
  sleep(0.1)  # Имитация долгой работы
  "AST(#{filename})"
end

queries.define_query(:type_check) do |filename, qs|
  puts "  → Type checking #{filename}..."
  ast = qs.parse(filename)  # Вызов другого query!
  sleep(0.1)
  "Types(#{ast})"
end

queries.define_query(:transform) do |filename, qs|
  puts "  → Transforming #{filename}..."
  types = qs.type_check(filename)  # Вызов другого query!
  sleep(0.1)
  "IR(#{types})"
end

# === ПЕРВАЯ КОМПИЛЯЦИЯ ===
puts "\n=== First compilation ==="
result1 = queries.transform("foo.ml")
# Output:
# [COMPUTING] transform("foo.ml")
#   → Transforming foo.ml...
# [COMPUTING] type_check("foo.ml")
#   → Type checking foo.ml...
# [COMPUTING] parse("foo.ml")
#   → Parsing foo.ml...
# => "IR(Types(AST(foo.ml)))"

# === ВТОРАЯ КОМПИЛЯЦИЯ (БЕЗ ИЗМЕНЕНИЙ) ===
puts "\n=== Second compilation (no changes) ==="
result2 = queries.transform("foo.ml")
# Output:
# [CACHE HIT] transform("foo.ml")
# => "IR(Types(AST(foo.ml)))"  (мгновенно!)

# === ИЗМЕНИЛИ ФАЙЛ ===
puts "\n=== File changed ==="
queries.invalidate(:parse, "foo.ml")

# === ТРЕТЬЯ КОМПИЛЯЦИЯ (ПОСЛЕ ИЗМЕНЕНИЙ) ===
puts "\n=== Third compilation (after changes) ==="
result3 = queries.transform("foo.ml")
# Output:
# [COMPUTING] transform("foo.ml")
#   → Transforming foo.ml...
# [COMPUTING] type_check("foo.ml")
#   → Type checking foo.ml...
# [COMPUTING] parse("foo.ml")
#   → Parsing foo.ml...
# => "IR(Types(AST(foo.ml)))"
```

---

## Когда Query System нужен?

### ✅ Нужен когда:

1. **Большая кодовая база** (1000+ файлов)
   - Incremental compilation критичен
   - Полная перекомпиляция занимает минуты

2. **Медленная компиляция** (>30 секунд)
   - Типизация сложная (generics, type inference)
   - Много проверок (borrow checker, lifetime analysis)

3. **IDE интеграция**
   - Нужна мгновенная обратная связь
   - Пользователь редактирует → компилятор должен ответить за <100ms

4. **Сложные зависимости**
   - Файлы зависят друг от друга
   - Изменение одного файла влияет на многие

### ❌ НЕ нужен когда:

1. **Маленькая кодовая база** (<100 файлов) ← **MLC сейчас!**
   - Полная перекомпиляция быстрая (<5 секунд)
   - Incremental compilation не даст большого выигрыша

2. **Простая типизация**
   - Type checking быстрый
   - Нет сложных generics

3. **Прототип/MVP**
   - Важнее простота и понятность кода
   - Query System добавляет сложность

4. **Редкие изменения**
   - Компиляция раз в день
   - Не критично подождать 30 секунд

---

## Сравнение: Pass-based vs Query-based

| Критерий | Pass-based (MLC сейчас) | Query-based (Rust) |
|----------|------------------------|-------------------|
| **Простота** | ✅ Очень простой | ⚠️ Сложнее |
| **Понятность** | ✅ Линейный pipeline | ⚠️ Граф зависимостей |
| **Скорость (малый проект)** | ✅ Быстро | ≈ Так же |
| **Скорость (большой проект)** | ❌ Медленно | ✅ Очень быстро |
| **Incremental compilation** | ❌ Сложно добавить | ✅ Из коробки |
| **Кэширование** | ❌ Руками | ✅ Автоматически |
| **Ленивые вычисления** | ❌ Нет | ✅ Да |

---

## Roadmap для MLC

### Сейчас (Phase 21-23): Pass-based ← **Правильный выбор!**

**Приоритет**: Чистота архитектуры, не скорость
- Phase 21: Упростить сервисы
- Phase 22: Убрать coupling
- Phase 23: Почистить комментарии

**Почему**: Кодовая база маленькая, компиляция быстрая, Query System - преждевременная оптимизация.

### Позже (Phase 27+): Query System ← **Когда станет нужно**

**Когда внедрять**: Когда появятся проблемы со скоростью
- Кодовая база >1000 файлов
- Компиляция >30 секунд
- IDE интеграция нужна

**Как внедрять**: Постепенно
1. Начать с самых медленных частей (type checking)
2. Обернуть существующие pass-ы в queries
3. Добавить кэширование
4. Добавить граф зависимостей
5. Включить incremental compilation

---

## Реальные примеры

### Rust (rustc)

**До Query System (2016-2017)**:
- Полная компиляция: 5 минут
- Incremental: не было

**После Query System (2018+)**:
- Полная компиляция: 5 минут (так же)
- Incremental: 10 секунд (30x быстрее!)

**Результат**: Разработка Rust стала намного приятнее.

### TypeScript (tsc)

**Архитектура**: Incremental compilation with file watching
- Похоже на Query System (но проще)
- Кэширует type checking каждого файла
- Пересчитывает только изменённые файлы

**Результат**: tsc --watch даёт мгновенную обратную связь.

### Go (gc)

**Архитектура**: Pass-based, но очень быстрый
- Простая типизация → быстрый type checking
- Нет generics (до Go 1.18) → простая компиляция
- Компиляция огромных проектов за секунды

**Вывод**: Query System не всегда нужен, если компилятор быстрый!

---

## Заключение

### Query System - это:

✅ **Мощная оптимизация** для incremental compilation
✅ **Автоматическое кэширование** результатов
✅ **Demand-driven** вычисления
✅ **Граф зависимостей** между queries

❌ **НЕ архитектурная необходимость**
❌ **НЕ нужен для маленьких проектов**
❌ **НЕ приоритет для MLC сейчас**

### Для MLC:

**Сейчас (2024)**: Оставить Pass-based архитектуру
- Простая, понятная, достаточно быстрая
- Фокус на чистоту, не на скорость

**Будущее (Phase 27+)**: Добавить Query System
- Когда кодовая база вырастет
- Когда компиляция станет медленной
- Когда понадобится IDE интеграция

**Pragmatic approach**: Не оптимизировать преждевременно!

---

## Ресурсы

1. **Rust Compiler Dev Guide - Query System**
   https://rustc-dev-guide.rust-lang.org/query.html

2. **Rust Compiler Dev Guide - Incremental Compilation**
   https://rustc-dev-guide.rust-lang.org/queries/incremental-compilation-in-detail.html

3. **Blog: How Rust compiler queries work**
   https://blog.rust-lang.org/inside-rust/2020/10/09/query-system.html

4. **RustConf 2024: Parallelizing rustc**
   https://lwn.net/Articles/995125/
