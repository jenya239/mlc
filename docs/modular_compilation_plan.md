# План перехода на модульную компиляцию

## Цель

Вместо inline (подстановка исходников в один файл) — генерация отдельного `.cpp` и `.hpp` для каждого `.mlc` модуля. Импорты превращаются в `#include`. Сборка: `g++ main.cpp foo.cpp bar.cpp ...`.

---

## 1. Текущее состояние

### 1.1 Pipeline (Ruby backend)

```
source → parse → AST (Program) → transform → SemanticIR (Module) → lower → C++ AST → to_source → .cpp
```

- (removed) inline_source_imports — use ModularCompiler for multi-file
- Один Program → один SemanticIR Module → один C++ output
- `to_hpp_cpp` уже есть (HeaderGenerator), но работает с одним модулем и не учитывает файловую структуру

### 1.2 Импорты

- Синтаксис: `import { X, Y } from "./path"` или `import * as Alias from "./path"`
- AST: `ImportDecl` с `path`, `items`, `import_all`, `alias`
- ImportService: stdlib — из SignatureRegistry; user — из `.mlcmeta` (metadata, не исходники)

### 1.3 mlcc (self-hosted)

- Отдельный pipeline: lexer → parser → checker → codegen
- Свой AST (Decl, Expr, Program)
- Сборка через ModularCompiler

---

## 2. Целевая архитектура (Nim-style)

### 2.1 Выход

```
foo.mlc  →  foo.cpp  +  foo.hpp
bar.mlc  →  bar.cpp  +  bar.hpp
main.mlc →  main.cpp +  main.hpp
```

### 2.2 Маппинг импортов

```
import { add, Vec } from "./math"
```

→ в `main.cpp`:
```cpp
#include "math.hpp"
// использование: add(...), Vec{...}
```

→ в `main.hpp` (если main экспортирует типы, использующие Vec):
```cpp
#include "math.hpp"
```

### 2.3 Сборка

```
g++ -I out/ -c out/main.cpp -o out/main.o
g++ -I out/ -c out/math.cpp -o out/math.o
g++ -I out/ -c out/foo.cpp  -o out/foo.o
g++ out/main.o out/math.o out/foo.o -o app
```

Или через build system (Makefile, CMake, ninja).

---

## 3. Фазы компиляции

### 3.1 Фаза 1: Парсинг и граф зависимостей

**Вход:** список .mlc файлов (entry point + транзитивные импорты)

**Действия:**
1. Для каждого .mlc: parse без inline → AST (Program)
2. Из AST извлечь ImportDecl → построить граф: `file → [imported files]`
3. Топологическая сортировка (или проверка на циклы)
4. Результат: упорядоченный список модулей `[M1, M2, ..., Mn]`, где M1 не зависит ни от кого, Mn — entry

**Важно:** парсим каждый файл отдельно. Путь импорта `"./foo"` резолвится относительно директории текущего файла.

### 3.2 Фаза 2: Семантический анализ (общий контекст)

**Вход:** упорядоченный список модулей

**Действия:**
1. Создать глобальные TypeRegistry, FunctionRegistry
2. Для каждого модуля по порядку:
   - transform AST → SemanticIR
   - ModuleImportPass: для каждого ImportDecl загрузить импорт
   - Для user import: нужно загрузить SemanticIR импортируемого модуля (он уже обработан — мы в топологическом порядке)
   - Зарегистрировать типы/функции текущего модуля в registry
3. Результат: полные registry + SemanticIR для каждого модуля

**Проблема:** сейчас ImportService для user modules загружает `.mlcmeta`, а не SemanticIR. Нужно либо:
- A) Генерировать .mlcmeta при компиляции модуля и использовать для следующих
- B) Хранить SemanticIR модулей в памяти и отдавать по запросу

Вариант B проще для монолитной компиляции: сначала parse+transform всех модулей в правильном порядке, потом lower каждый.

### 3.3 Фаза 3: Lowering по модулям

**Вход:** SemanticIR для каждого модуля, полные registry

**Действия:**
1. Для каждого модуля M:
   - Создать Codegen с shared registry
   - Lower M → C++ AST
   - HeaderGenerator: разделить на .hpp (декларации, экспорты) и .cpp (определения)
   - Вставить `#include "dep.hpp"` для каждого импорта M
   - Записать M.hpp и M.cpp

**Экспорт:** только `export` типы и функции попадают в .hpp. Остальное — только в .cpp.

### 3.4 Фаза 4: Сборка

Вызвать g++/clang для каждого .cpp, затем линковку. Или выдать Makefile/CMakeLists.txt.

---

## 4. Изменения по компонентам

### 4.1 compiler_api.rb

| Метод | Сейчас | После |
|-------|--------|-------|
| `compile` | parse → transform → lower | `build_project` для multi-file |
| `to_cpp` | compile → to_source | Опционально: `to_cpp(source, filename, modular: true)` → запись в out_dir |
| `inline_source_imports` | — | Удалено |

### 4.2 Новый API

```ruby
# Компиляция проекта в модульном режиме
MLC.compile_project(
  entry_path: "src/main.mlc",
  out_dir: "build",
  emit_build_file: true  # генерировать Makefile/compile_commands.json
)
# => { cpp_files: [...], hpp_files: [...], build_file: "build/Makefile" }
```

### 4.3 ModuleGraph / CompilationUnit

Новый класс для управления модулями:

```ruby
class ModuleGraph
  def add_file(path)          # parse, extract imports
  def dependency_order        # topo sort
  def resolve_import(path, from_file)  # path → absolute path
end

class CompilationUnit
  attr_reader :path, :ast, :semantic_ir, :imports
  def compile_to(out_dir)     # emit .cpp + .hpp
end
```

### 4.4 ImportService

Сейчас для user import: `metadata_loader.load(metadata_path)` — загружает .mlcmeta.

Для модульной компиляции: импортируемый модуль уже обработан в этой же сессии. Нужен `ImportService.register_from_ir(semantic_ir)` — зарегистрировать типы/функции из уже преобразованного модуля.

### 4.5 HeaderGenerator

Уже умеет:
- Разделять header/implementation
- `#include` из imports
- Forward declarations

Нужно:
- Маппинг `"./foo"` → `"foo.hpp"` (относительно out_dir)
- Учитывать, что импорты — это файловые пути, а не имена модулей

### 4.6 Name mangling

Сейчас: `add` → `add`, `Math::add` → `Math_add` или в namespace.

При модульности: каждый модуль — отдельная единица. Имена в C++ должны быть уникальны. Варианты:
- Namespace по имени файла: `namespace main { ... }`
- Prefix: `main_add`, `math_add`
- Оставить как есть, если имена не коллидируют (разные .cpp)

---

## 5. mlcc (self-hosted compiler)

mlcc имеет свой pipeline и не использует SemanticIR. План для mlcc:

### 5.1 Текущий codegen.mlc

- Принимает Program (ModuleGraph для multi-file)
- Генерирует один .cpp

### 5.2 Целевой codegen для mlcc

1. **Вход:** Program одного модуля + список импортов (path → что импортировано)
2. **Выход:** строки для .cpp и .hpp
3. **Импорты:** в начале .cpp — `#include "foo.hpp"` для каждого `import from "./foo"`
4. **Главный цикл:** main.mlc вызывает компиляцию каждого импортируемого файла рекурсивно, записывает результаты

### 5.3 Структура mlcc при модульности

```
compile_project(entry_path) ->
  1. load_module(entry_path) — как сейчас, но без merge decls
  2. Для каждого уникального path: 
     - parse
     - check (нужен ли? или check на merged?)
     - codegen → .cpp, .hpp
  3. Записать файлы в out_dir
```

Проблема: checker (checker/infer.mlc) работает с Program. При модульности каждый модуль — отдельный Program. Checker должен видеть типы из импортов. Значит, либо:
- Checker получает registry извне (типы импортов уже зарегистрированы)
- Либо check делается на merged program (как сейчас), а codegen — по модулям

Упрощение: оставить check на merged (для корректности), codegen — по отдельным модулям. То есть: merge только для проверки, для codegen — разбить обратно по файлам. Это требует сохранения маппинга decl → source_file.

---

## 6. Порядок внедрения

### Этап 1: Ruby backend, без смены дефолтного поведения

1. Реализовать `ModuleGraph` — парсинг без inline, граф зависимостей
2. Реализовать `compile_modular` — полный pipeline без inline
3. Интегрировать с `HeaderGenerator` для per-module output
4. Тесты: компиляция двух модулей (main imports foo), проверка что получаются main.cpp, main.hpp, foo.cpp, foo.hpp

### Этап 2: Сборка

1. Генерация Makefile или вызов g++ из Ruby
2. CLI: `mlc --modular -o build src/main.mlc`

### Этап 3: Переключение дефолта

1. Сделать modular режим по умолчанию
2. `inline_source_imports` удалён

### Этап 4: mlcc

1. Адаптировать codegen.mlc для per-file output
2. Сохранять маппинг decl → file при merge (для codegen)
3. Или: отдельный проход codegen до merge, по каждому файлу

---

## 7. Минимальные независимые шаги (TDD)

Каждый шаг — отдельный тест, можно мержить по одному.

### 7.1 Граф зависимостей

| # | Шаг | Тест | Зависимости |
|---|-----|------|-------------|
| 1 | `resolve_import_path("./foo", from: "src/main.mlc")` → `"src/foo.mlc"` | unit | — |
| 2 | `resolve_import_path("../lib/bar", from: "src/sub/main.mlc")` → `"src/lib/bar.mlc"` | unit | — |
| 3 | `ModuleGraph.from_entry("main.mlc")` — парсит main, извлекает imports, возвращает `{main => [foo]}` | unit | 1, 2 |
| 4 | Транзитивно: main→foo→bar → граф из 3 узлов | unit | 3 |
| 5 | `dependency_order` — топологическая сортировка, bar перед foo перед main | unit | 4 |
| 6 | Цикл main→foo→main → ошибка "circular import" | unit | 5 |

### 7.2 Парсинг без inline

| # | Шаг | Тест | Зависимости |
|---|-----|------|-------------|
| 7 | `parse_file("foo.mlc")` без inline — AST с ImportDecl, но без содержимого foo | unit | — |
| 8 | Файл с `import { x } from "./bar"` — парсится, bar не подставляется | unit | 7 |
| 9 | Ошибка в bar.mlc не ломает parse main.mlc (разные вызовы parse) | unit | 7 |

### 7.3 Transform по порядку

| # | Шаг | Тест | Зависимости |
|---|-----|------|-------------|
| 10 | `register_from_ir(semantic_ir)` — положить типы/функции модуля в registry | unit | — |
| 11 | Transform bar (без импортов) → SemanticIR, register | unit | 10 |
| 12 | Transform foo (import bar) — ImportService берёт bar из in-memory registry | unit | 11 |
| 13 | Transform main (import foo) — registry уже содержит foo, bar | integration | 12 |

### 7.4 Lowering по модулям

| # | Шаг | Тест | Зависимости |
|---|-----|------|-------------|
| 14 | Lower один модуль (без импортов) → C++ AST | unit | — |
| 15 | Lower модуль с `import { T } from "./foo"` — в C++ используется T, lowerer находит в registry | unit | 14 |
| 16 | HeaderGenerator для модуля: типы в .hpp, функции: decl в .hpp, def в .cpp | unit | — |
| 17 | HeaderGenerator: `#include "foo.hpp"` для каждого import | unit | 16 |
| 18 | Путь `"./foo"` → `"foo.hpp"` в include | unit | 17 |

### 7.5 End-to-end (минимальный)

| # | Шаг | Тест | Зависимости |
|---|-----|------|-------------|
| 19 | Два файла: bar.mlc (type T), foo.mlc (import T, fn f), main.mlc (import f) | integration | 5, 13, 18 |
| 20 | Сгенерировать foo.cpp, foo.hpp, bar.cpp, bar.hpp, main.cpp, main.hpp | integration | 19 |
| 21 | g++ компилирует и линкует → исполняемый файл | integration | 20 |

### 7.6 Граничные случаи

| # | Шаг | Тест | Зависимости |
|---|-----|------|-------------|
| 22 | Selective import: `import { a } from "./m"` — в scope только a, не b | unit | 12 |
| 23 | Export: только export типы/функции в .hpp | unit | 16 |
| 24 | Цикл типов A→B→A: forward declaration в .hpp | unit | 17 |
| 25 | Один модуль (без импортов) — один .cpp + .hpp | unit | 19 |

### 7.7 Порядок реализации

```
Фаза A (граф):     1 → 2 → 3 → 4 → 5 → 6
Фаза B (парсинг):  7 → 8 → 9
Фаза C (registry): 10 → 11 → 12 → 13
Фаза D (lower):    14 → 15
Фаза E (header):   16 → 17 → 18
Фаза F (e2e):      19 → 20 → 21
Фаза G (edge):     22, 23, 24, 25 — по необходимости
```

Фазы A и B независимы. C зависит от A,B. D зависит от C. E можно параллельно с D. F — финальная сборка.

---

## 8. Риски и открытые вопросы

1. **Циклические импорты:** шаг 6 — запрет; шаг 24 — forward declarations при цикле типов.
2. **Stdlib:** как импортируется? Сейчас — через SignatureRegistry. При модульности stdlib — отдельные .hpp в runtime?
3. **.mlcmeta:** в модульном режиме генерировать ли .mlcmeta для каждого модуля (для инкрементальной компиляции)?
4. **Обратная совместимость:** проекты без явного entry point (один файл) — как определять что компилировать?
