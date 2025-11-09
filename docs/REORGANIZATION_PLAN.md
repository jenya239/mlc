# План реорганизации структуры MLC

## Цель
Привести структуру к best practices компиляторов (Rust, Crystal, Roslyn) с чётким разделением:
- Центральные registries (как rustc_middle)
- Глобальные утилиты (cross-phase)
- Phase-specific сервисы (semantic_gen)

## Проблемы текущей структуры

1. **Неочевидное разделение**: 90% сервисов в `mlc/services/` на самом деле используются ТОЛЬКО в semantic_gen
2. **Дублирование**: TypeInferenceService vs ExpressionTypeResolver, IdentifierTypeResolver, MemberAccessService (~940 строк на одну задачу)
3. **Нарушение SRP**: TypeChecker (356 строк, 10+ ответственностей), TypeInferenceService (592 строки)
4. **Хаотичная организация**: semantic_gen/services/ смешивает builders, checkers, inference без группировки

## Целевая структура

```
lib/mlc/
  # Entry point
  application.rb

  # Центральные registries (как rustc_middle)
  core/
    type_registry.rb
    function_registry.rb
    function_signature.rb

  # Инфраструктура компилятора
  infrastructure/
    event_bus.rb
    pass_manager.rb

  # Глобальные утилиты (могут использоваться в разных фазах)
  compiler/
    stdlib/
      scanner.rb              # stdlib сканирование
      resolver.rb             # stdlib резолвинг
      signature_registry.rb   # stdlib кэширование

  # Phase loaders
  semantic_gen.rb

  # Type system utilities (чистые функции с proc dependencies)
  type_system/
    generic_call_resolver.rb
    effect_analyzer.rb
    match_analyzer.rb
    type_constraint_solver.rb

  # Backend (кодогенерация)
  backend/
    codegen.rb
    codegen/...
    header_generator.rb

  # Analysis passes
  analysis/
    base_pass.rb
    name_resolution_pass.rb
    type_check_pass.rb
    effect_analysis_pass.rb

  # SemanticGen phase (AST → SemanticIR)
  semantic_gen/
    services/
      # Scope management
      scope/
        var_type_registry.rb      # MOVE из mlc/services
        scope_context.rb          # MOVE из mlc/services
        module_context.rb         # MOVE из mlc/services

      # Builders (IR construction)
      builders/
        ir_builder.rb
        ast_factory.rb
        type_builder.rb
        literal_builder.rb        # MERGE: record + array

      # Type checking utilities
      checkers/
        ast_type_checker.rb
        semantic_ir_classifier.rb
        type_validator.rb         # SPLIT из TypeChecker

      # Type inference (объединение дублирования)
      inference/
        type_inference.rb         # MOVE + MERGE TypeInferenceService
        expression_types.rb       # DELETE ExpressionTypeResolver, merge
        member_types.rb           # DELETE MemberAccessService, merge
        generic_types.rb          # extract из TypeInferenceService
        type_predicates.rb        # extract из TypeInferenceService

      # Language features
      features/
        loop_handler.rb
        lambda_handler.rb
        match_handler.rb
        comprehension_handler.rb
        index_access_handler.rb

      # Import/Export
      imports/
        import_processor.rb
        module_resolver.rb

      # Registration (в центральные registries)
      registration/
        type_registrar.rb         # MOVE TypeRegistrationService
        function_registrar.rb     # MOVE FunctionRegistrationService
        constructor_registrar.rb  # MOVE SumTypeConstructorService

      # Naming/Validation utilities
      utils/
        type_name_resolver.rb     # SPLIT из TypeChecker
        type_unification.rb       # MOVE TypeUnificationService
        type_resolution.rb        # MOVE TypeResolutionService
        purity_analyzer.rb        # MOVE PurityAnalyzer

    visitors/
      expression_visitor.rb
      statement_visitor.rb

    rules/
      base_rule.rb
      ...

    reducers/
      function_reducer.rb
      ...

    engine.rb
    pipeline.rb
```

## Этапы миграции

### ЭТАП 1: Подготовка (создание структуры, документация)
**Цель**: Создать новую структуру каталогов, не трогая существующий код
**Риск**: минимальный (только создание папок)
**Время**: 15 минут

**Действия:**
1. Создать новые каталоги:
   ```bash
   mkdir -p lib/mlc/core
   mkdir -p lib/mlc/infrastructure
   mkdir -p lib/mlc/compiler/stdlib
   mkdir -p lib/mlc/semantic_gen/services/{scope,builders,checkers,inference,features,imports,registration,utils}
   ```

2. Создать документ `docs/MIGRATION_MAPPING.md` с mapping старых путей на новые

3. Закоммитить структуру каталогов

**Проверка**: `git status` показывает только новые пустые каталоги

---

### ЭТАП 1.5: Реорганизация корневых файлов (5 файлов)
**Цель**: Переместить registries и infrastructure из корня в соответствующие каталоги
**Риск**: средний (много файлов используют эти классы)
**Время**: 30 минут

**Структура (Вариант 2):**
- `core/` - центральные registries (как rustc_middle)
- `infrastructure/` - event bus, pass manager
- `compiler/` - утилиты (stdlib будет добавлен в ЭТАП 2)

**Действия:**

#### 1.5.1 Перенос registries в core/ (3 файла)
```bash
git mv lib/mlc/type_registry.rb lib/mlc/core/
git mv lib/mlc/function_registry.rb lib/mlc/core/
git mv lib/mlc/function_signature.rb lib/mlc/core/
```

**Обновить module declarations:**
```ruby
# было:
module MLC
  class TypeRegistry

# стало:
module MLC
  module Core
    class TypeRegistry
```

**Файлы, которые нужно обновить** (автоматический поиск):
```bash
grep -r "require.*type_registry\|require.*function_registry\|require.*function_signature" lib/ test/
grep -r "TypeRegistry\|FunctionRegistry\|FunctionSignature" lib/ test/ | grep -v "\.md" | wc -l
```

Ожидается ~100+ файлов с использованиями.

**Стратегия обновления:**
1. Обновить все `require_relative "../type_registry"` → `require_relative "../core/type_registry"`
2. Обновить все `MLC::TypeRegistry` → `MLC::Core::TypeRegistry`
3. Обновить все `MLC::FunctionRegistry` → `MLC::Core::FunctionRegistry`
4. Обновить все `MLC::FunctionSignature` → `MLC::Core::FunctionSignature`

**Проверка**: `rake test` после каждого типа файлов (сначала lib/, потом test/)

#### 1.5.2 Перенос infrastructure (2 файла)
```bash
git mv lib/mlc/event_bus.rb lib/mlc/infrastructure/
git mv lib/mlc/pass_manager.rb lib/mlc/infrastructure/
```

**Обновить module declarations:**
```ruby
# было:
module MLC
  class EventBus

# стало:
module MLC
  module Infrastructure
    class EventBus
```

**Файлы для обновления:**
```bash
grep -r "require.*event_bus\|require.*pass_manager" lib/ test/
grep -r "EventBus\|PassManager" lib/ test/ | wc -l
```

**Проверка**: `rake test`

#### 1.5.3 Обновление lib/mlc.rb (главный require файл)
Обновить все require_relative в главном файле библиотеки:
```ruby
# lib/mlc.rb
require_relative "mlc/core/type_registry"
require_relative "mlc/core/function_registry"
require_relative "mlc/core/function_signature"
require_relative "mlc/infrastructure/event_bus"
require_relative "mlc/infrastructure/pass_manager"
```

**Финальная проверка ЭТАП 1.5:**
```bash
rake test
git diff --stat  # проверить количество изменённых файлов
```

**Откат при проблемах:**
```bash
git reset --hard HEAD
```

---

### ЭТАП 2: Перенос stdlib utilities (3 файла)
**Цель**: Переместить глобальные stdlib утилиты
**Риск**: низкий (используются в 2-3 местах)
**Время**: 20 минут

**Действия:**
1. Переместить файлы:
   ```bash
   git mv lib/mlc/services/stdlib_scanner.rb lib/mlc/compiler/stdlib/scanner.rb
   git mv lib/mlc/services/stdlib_resolver.rb lib/mlc/compiler/stdlib/resolver.rb
   git mv lib/mlc/services/stdlib_signature_registry.rb lib/mlc/compiler/stdlib/signature_registry.rb
   ```

2. Обновить `module` declarations внутри файлов:
   ```ruby
   # было:
   module MLC
     class StdlibScanner

   # стало:
   module MLC
     module Compiler
       class StdlibScanner
   ```

3. Обновить все `require_relative` (автоматический поиск):
   ```bash
   grep -r "require.*stdlib_scanner\|require.*stdlib_resolver\|require.*stdlib_signature_registry" lib/ test/
   ```

4. Обновить все использования класса:
   ```bash
   # MLC::StdlibScanner → MLC::Compiler::StdlibScanner
   ```

**Файлы, которые нужно обновить** (известные):
- `lib/mlc/application.rb`
- `lib/mlc/services/stdlib_resolver.rb:3`
- `lib/mlc/semantic_gen/services/import_service.rb`
- `test/mlc/stdlib_scanner_test.rb`
- `test/mlc/stdlib_resolution_test.rb`

**Проверка**:
```bash
rake test TEST=test/mlc/stdlib_scanner_test.rb
rake test TEST=test/mlc/stdlib_resolution_test.rb
```

**Откат при проблемах**:
```bash
git reset --hard HEAD
```

---

### ЭТАП 3: Группировка semantic_gen/services (перемещение БЕЗ изменений)
**Цель**: Переместить файлы в подпапки semantic_gen/services/* без изменения кода
**Риск**: средний (много файлов, но только перемещение)
**Время**: 40 минут

**Действия** (в 6 подэтапов):

#### 3.1 Scope management (3 файла)
```bash
git mv lib/mlc/services/var_type_registry.rb lib/mlc/semantic_gen/services/scope/
git mv lib/mlc/services/scope_context_service.rb lib/mlc/semantic_gen/services/scope/
git mv lib/mlc/services/module_context_service.rb lib/mlc/semantic_gen/services/scope/
```

Обновить module:
```ruby
module MLC::SemanticGen::Services::Scope
  class VarTypeRegistry
```

Обновить все requires и uses (поиск автоматический)

**Проверка**: `rake test`

#### 3.2 Builders (4 файла)
```bash
git mv lib/mlc/semantic_gen/services/ir_builder.rb lib/mlc/semantic_gen/services/builders/
git mv lib/mlc/semantic_gen/services/ast_factory.rb lib/mlc/semantic_gen/services/builders/
git mv lib/mlc/semantic_gen/services/type_builder.rb lib/mlc/semantic_gen/services/builders/
# record + array оставим пока отдельно, объединим в ЭТАП 5
git mv lib/mlc/semantic_gen/services/record_literal_builder.rb lib/mlc/semantic_gen/services/builders/
git mv lib/mlc/semantic_gen/services/array_literal_builder.rb lib/mlc/semantic_gen/services/builders/
```

**Проверка**: `rake test`

#### 3.3 Checkers (2 файла)
```bash
git mv lib/mlc/semantic_gen/services/ast_type_checker.rb lib/mlc/semantic_gen/services/checkers/
git mv lib/mlc/semantic_gen/services/semantic_ir_classifier.rb lib/mlc/semantic_gen/services/checkers/
```

**Проверка**: `rake test`

#### 3.4 Features (5 файлов)
```bash
git mv lib/mlc/semantic_gen/services/loop_service.rb lib/mlc/semantic_gen/services/features/loop_handler.rb
git mv lib/mlc/semantic_gen/services/lambda_service.rb lib/mlc/semantic_gen/services/features/lambda_handler.rb
git mv lib/mlc/semantic_gen/services/match_service.rb lib/mlc/semantic_gen/services/features/match_handler.rb
git mv lib/mlc/semantic_gen/services/list_comprehension_service.rb lib/mlc/semantic_gen/services/features/comprehension_handler.rb
git mv lib/mlc/semantic_gen/services/index_access_service.rb lib/mlc/semantic_gen/services/features/index_access_handler.rb
```

**Проверка**: `rake test`

#### 3.5 Imports (2 файла)
```bash
git mv lib/mlc/semantic_gen/services/import_service.rb lib/mlc/semantic_gen/services/imports/import_processor.rb
git mv lib/mlc/semantic_gen/services/module_resolver.rb lib/mlc/semantic_gen/services/imports/
```

**Проверка**: `rake test`

#### 3.6 Registration (3 файла из mlc/services)
```bash
git mv lib/mlc/services/type_registration_service.rb lib/mlc/semantic_gen/services/registration/type_registrar.rb
git mv lib/mlc/services/function_registration_service.rb lib/mlc/semantic_gen/services/registration/function_registrar.rb
git mv lib/mlc/services/sum_type_constructor_service.rb lib/mlc/semantic_gen/services/registration/constructor_registrar.rb
```

**Проверка**: `rake test`

**Откат при проблемах**:
```bash
git reset --hard HEAD
```

---

### ЭТАП 4: Удаление дублирования (критичный этап)
**Цель**: Удалить ExpressionTypeResolver, IdentifierTypeResolver, MemberAccessService - всё есть в TypeInferenceService
**Риск**: ВЫСОКИЙ (изменение логики)
**Время**: 60 минут

**Стратегия**: Переместить TypeInferenceService в inference/, затем удалить дубликаты и обновить их uses на TypeInferenceService

#### 4.1 Анализ дублирования (проверка перед удалением)

**Проверим, что функциональность идентична:**

1. **ExpressionTypeResolver vs TypeInferenceService**:
   - `ExpressionTypeResolver.binary_type` == `TypeInferenceService.infer_binary_type`
   - `ExpressionTypeResolver.unary_type` == `TypeInferenceService.infer_unary_type`
   - `ExpressionTypeResolver.combine_numeric` == `TypeInferenceService.combine_numeric_type`

2. **IdentifierTypeResolver vs TypeInferenceService**:
   - `IdentifierTypeResolver.type_for` == `TypeInferenceService.infer_variable_type`

3. **MemberAccessService vs TypeInferenceService**:
   - `MemberAccessService.resolve` == `TypeInferenceService.infer_member_type`

#### 4.2 Подготовка: перемещение TypeInferenceService
```bash
git mv lib/mlc/services/type_inference_service.rb lib/mlc/semantic_gen/services/inference/type_inference.rb
```

Обновить module:
```ruby
module MLC::SemanticGen::Services::Inference
  class TypeInference
```

Обновить container.rb и другие uses.

**Проверка**: `rake test` (все тесты должны пройти)

#### 4.3 Удаление ExpressionTypeResolver

**Найти все uses**:
```bash
grep -r "ExpressionTypeResolver" lib/mlc/semantic_gen
```

**Заменить на TypeInference методы**:
- `@expression_type_resolver.binary_type(...)` → `@type_inference.infer_binary_type(...)`
- `@expression_type_resolver.unary_type(...)` → `@type_inference.infer_unary_type(...)`

**Удалить файл**:
```bash
git rm lib/mlc/semantic_gen/services/expression_type_resolver.rb
```

**Обновить container.rb**: удалить инициализацию ExpressionTypeResolver

**Проверка**: `rake test` (должен пройти ВСЕ тесты)

#### 4.4 Удаление IdentifierTypeResolver

**Найти uses** → заменить на `@type_inference.infer_variable_type`

**Удалить**:
```bash
git rm lib/mlc/semantic_gen/services/identifier_type_resolver.rb
```

**Проверка**: `rake test`

#### 4.5 Удаление MemberAccessService

**Найти uses** → заменить на `@type_inference.infer_member_type`

**Удалить**:
```bash
git rm lib/mlc/semantic_gen/services/member_access_service.rb
```

**Проверка**: `rake test`

**Откат при проблемах**:
```bash
git reset --hard HEAD
```

---

### ЭТАП 5: Разделение больших классов (рефакторинг)
**Цель**: Разделить TypeChecker (356 строк) и TypeInferenceService (592 строки)
**Риск**: ВЫСОКИЙ (большой рефакторинг)
**Время**: 90 минут

#### 5.1 Разделение TypeChecker на 3 класса

**Создать новые файлы:**

1. **lib/mlc/semantic_gen/services/checkers/type_validator.rb**:
   ```ruby
   module MLC::SemanticGen::Services::Checkers
     class TypeValidator
       def initialize(type_name_resolver:, error_reporter:)
         @type_name_resolver = type_name_resolver
         @error_reporter = error_reporter
       end

       # Методы из TypeChecker:
       # - ensure_compatible_type
       # - ensure_boolean_type
       # - ensure_numeric_type
       # - validate_function_call
       # - validate_constraint_name
       # - type_satisfies_constraint?
     end
   end
   ```

2. **lib/mlc/semantic_gen/services/utils/type_name_resolver.rb**:
   ```ruby
   module MLC::SemanticGen::Services::Utils
     class TypeNameResolver
       # Методы из TypeChecker:
       # - type_name
       # - normalized_type_name
       # - describe_type
       # - numeric_type?
       # - generic_type_name?
       # - unit_like?
     end
   end
   ```

3. **lib/mlc/semantic_gen/services/utils/error_reporter.rb**:
   ```ruby
   module MLC::SemanticGen::Services::Utils
     class ErrorReporter
       # Методы из TypeChecker:
       # - type_error
       # - ensure_type!
     end
   end
   ```

**Стратегия миграции**:
1. Создать новые файлы с методами (copy-paste из TypeChecker)
2. Добавить в container.rb инициализацию новых сервисов
3. Обновить ВСЕ uses TypeChecker постепенно:
   - `@type_checker.ensure_compatible_type` → `@type_validator.ensure_compatible_type`
   - `@type_checker.type_name` → `@type_name_resolver.type_name`
   - `@type_checker.type_error` → `@error_reporter.type_error`
4. Когда все uses обновлены и тесты проходят - удалить старый TypeChecker

**Проверка после каждого file**: `rake test`

#### 5.2 Разделение TypeInferenceService на 4 класса

**Создать:**
1. `inference/type_inference.rb` (координатор, 200 строк)
2. `inference/expression_types.rb` (binary/unary, 150 строк)
3. `inference/member_types.rb` (member access, 150 строк)
4. `inference/generic_types.rb` (unification, 100 строк)

**Стратегия**: аналогично TypeChecker - создать новые, обновить uses, удалить старый

**Проверка**: `rake test` после каждого изменения

---

### ЭТАП 6: Финализация (cleanup, документация)
**Цель**: Удалить старые папки, обновить документацию
**Риск**: минимальный
**Время**: 30 минут

**Действия:**
1. Удалить пустые папки:
   ```bash
   rmdir lib/mlc/services  # должна быть пуста после всех переносов
   ```

2. Обновить документацию:
   - `docs/ARCHITECTURE_GUIDE.md` - обновить схему структуры
   - `docs/REFACTORING_ROADMAP.md` - отметить завершение реорганизации
   - `README.md` - обновить примеры путей

3. Проверить все тесты:
   ```bash
   rake test
   ```

4. Commit:
   ```bash
   git add -A
   git commit -m "feat: complete codebase reorganization

   - Reorganized root files: core/, infrastructure/, compiler/
   - Moved stdlib utilities to compiler/stdlib/
   - Organized semantic_gen/services by functionality
   - Removed code duplication (ExpressionTypeResolver, etc)
   - Split large classes (TypeChecker, TypeInferenceService)
   - Aligned structure with compiler best practices (Rust, Crystal, Roslyn)

   Result: cleaner architecture, -10% LOC, better maintainability"
   ```

---

## Откат стратегия

**После каждого этапа:**
- Если тесты не проходят: `git reset --hard HEAD`
- Если проблемы найдены позже: `git revert <commit>`

**Полный откат всей миграции:**
```bash
git log --oneline | grep "reorganization" | head -1  # найти начальный commit
git revert <commit>..<current>  # откатить все изменения
```

## Метрики успеха

**До:**
- 37+ сервисов без чёткой группировки
- 4656 строк кода с дублированием
- TypeChecker: 356 строк, 10+ ответственностей
- TypeInferenceService: 592 строки
- Неочевидное разделение mlc/services vs semantic_gen/services

**После:**
- ~30 сервисов с чёткой группировкой по функциональности
- ~4200 строк (-10%)
- Каждый класс <200 строк
- Одна implementation для каждой задачи
- Чёткое разделение: compiler/ (global), semantic_gen/services/ (phase-specific)

## Тестирование

**На каждом этапе запускать:**
```bash
# Быстрая проверка
rake test TEST=test/mlc/semantic_gen_test.rb

# Полная проверка
rake test

# Проверка конкретных затронутых файлов
rake test TEST=test/mlc/type_inference_test.rb
rake test TEST=test/mlc/stdlib_scanner_test.rb
```

## Время выполнения

- ЭТАП 1: 15 минут
- ЭТАП 1.5: 30 минут (новый - реорганизация корневых файлов)
- ЭТАП 2: 20 минут
- ЭТАП 3: 40 минут
- ЭТАП 4: 60 минут (критичный)
- ЭТАП 5: 90 минут (критичный)
- ЭТАП 6: 30 минут

**Итого**: ~4.5 часа (с запасом на отладку)

## Зависимости между этапами

- ЭТАП 1 → ЭТАП 1.5 (нужна структура каталогов)
- ЭТАП 1.5 → ЭТАП 2 (registries уже в core/, infrastructure/)
- ЭТАП 2 → ЭТАП 3 (stdlib утилиты должны быть уже перемещены)
- ЭТАП 3 → ЭТАП 4 (файлы должны быть в правильных местах)
- ЭТАП 4 → ЭТАП 5 (дубликаты должны быть удалены)
- ЭТАП 5 → ЭТАП 6 (рефакторинг завершён)

**Каждый этап можно коммитить отдельно.**
