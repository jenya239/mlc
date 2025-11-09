# Критерии разделения: compiler/ vs semantic_gen/services/

## Простое правило

### lib/mlc/compiler/ - ГЛОБАЛЬНЫЕ утилиты
**Критерий**: Используется (или МОЖЕТ использоваться) в **≥2 фазах** компилятора

**Фазы компилятора:**
1. `semantic_gen/` - AST → SemanticIR трансформация
2. `backend/` - SemanticIR → C++ кодогенерация
3. `analysis/` - Analysis passes (name resolution, type checking)

**Примеры:**
- ✅ `StdlibScanner` - может использоваться и в semantic_gen (для imports), и в analysis (для проверки stdlib)
- ✅ `TypeRegistry` - используется в semantic_gen, backend, analysis
- ✅ `FunctionRegistry` - используется в semantic_gen, backend, analysis

### lib/mlc/semantic_gen/services/ - PHASE-SPECIFIC
**Критерий**: Используется **ТОЛЬКО** в semantic_gen фазе

**Примеры:**
- ✅ `VarTypeRegistry` - runtime state для отслеживания типов переменных ТОЛЬКО во время semantic_gen
- ✅ `TypeInferenceService` - выведение типов ТОЛЬКО в semantic_gen
- ✅ `IRBuilder` - построение SemanticIR ТОЛЬКО в semantic_gen
- ✅ `ASTFactory` - построение AST nodes ТОЛЬКО в semantic_gen

## Проверка на реальном коде

### TypeChecker - куда отнести?

**Анализ использования:**
```bash
grep -r "TypeChecker" lib/mlc/{semantic_gen,backend,analysis}
```

**Результат:**
- semantic_gen: ✅ используется
- backend: ❌ НЕ используется
- analysis: ❌ НЕ используется

**Вывод:** TypeChecker → `semantic_gen/services/` (phase-specific)

### StdlibScanner - куда отнести?

**Анализ:**
- semantic_gen: ✅ используется (import processing)
- backend: ❌ НЕ используется
- analysis: ❌ НЕ используется (но МОЖЕТ использоваться в будущем для stdlib validation)

**Вывод:** StdlibScanner → `compiler/stdlib/` (потенциально cross-phase)

### TypeRegistry - куда отнести?

**Анализ:**
```
lib/mlc/backend/codegen.rb:6: require_relative "../type_registry"
lib/mlc/backend/header_generator.rb: используется
lib/mlc/semantic_gen/: используется
lib/mlc/analysis/: используется
```

**Вывод:** TypeRegistry → `lib/mlc/` (корень, центральный registry)

## Результаты анализа текущих сервисов

### ИЗ mlc/services/ → compiler/stdlib/ (3 файла)
- `stdlib_scanner.rb` → `compiler/stdlib/scanner.rb`
- `stdlib_resolver.rb` → `compiler/stdlib/resolver.rb`
- `stdlib_signature_registry.rb` → `compiler/stdlib/signature_registry.rb`

### ИЗ mlc/services/ → semantic_gen/services/ (10 файлов)
- `type_checker.rb` → `semantic_gen/services/checkers/type_validator.rb` (split)
- `type_inference_service.rb` → `semantic_gen/services/inference/type_inference.rb`
- `var_type_registry.rb` → `semantic_gen/services/scope/var_type_registry.rb`
- `scope_context_service.rb` → `semantic_gen/services/scope/scope_context.rb`
- `module_context_service.rb` → `semantic_gen/services/scope/module_context.rb`
- `purity_analyzer.rb` → `semantic_gen/services/utils/purity_analyzer.rb`
- `type_unification_service.rb` → `semantic_gen/services/utils/type_unification.rb`
- `function_registration_service.rb` → `semantic_gen/services/registration/function_registrar.rb`
- `type_resolution_service.rb` → `semantic_gen/services/utils/type_resolution.rb`
- `sum_type_constructor_service.rb` → `semantic_gen/services/registration/constructor_registrar.rb`

### ОСТАЮТСЯ в lib/mlc/ (корень) - центральные registries
- `type_registry.rb`
- `function_registry.rb`
- `function_signature.rb`

## Почему НЕ использовать "общее название" services/?

**Проблема**: Неочевидно, какие сервисы глобальные, а какие phase-specific

**Решение**: Использовать говорящие названия:
- `compiler/` - сразу понятно, что это cross-phase utilities
- `semantic_gen/services/` - сразу понятно, что это semantic_gen-specific
- `backend/codegen/` - понятно, что это backend-specific

## Аналоги в других компиляторах

### Rust (rustc)
```
compiler/
  rustc_middle/          # Центральные данные (как наш type_registry, function_registry)
  rustc_data_structures/ # Общие структуры (как наш compiler/)
  rustc_hir/             # HIR phase-specific (как наш semantic_gen/)
  rustc_codegen_llvm/    # Codegen phase-specific (как наш backend/)
```

### Crystal
```
src/compiler/crystal/
  syntax/      # Parser (phase-specific)
  semantic/    # Semantic analysis (phase-specific)
  codegen/     # Code generation (phase-specific)
```

### Roslyn (C#)
```
Roslyn/
  Compilers/
    Core/              # Общие утилиты (как наш compiler/)
    CSharp/
      Symbols/         # Центральные данные (как наш registries)
      Binder/          # Binding phase (как наш semantic_gen/)
      CodeGen/         # Codegen phase (как наш backend/)
```

## Итоговая структура MLC (Вариант 2)

```
lib/mlc/
  # Entry point
  application.rb

  # Центральные registries (как rustc_middle)
  core/
    type_registry.rb
    function_registry.rb
    function_signature.rb

  # Infrastructure компилятора
  infrastructure/
    event_bus.rb
    pass_manager.rb

  # Cross-phase utilities (как rustc_data_structures)
  compiler/
    stdlib/
      scanner.rb
      resolver.rb
      signature_registry.rb

  # Phase loaders
  semantic_gen.rb

  # Phase-specific (как rustc_hir)
  semantic_gen/
    services/
      scope/
      builders/
      inference/
      ...

  # Phase-specific (как rustc_codegen_llvm)
  backend/
    codegen/
    ...

  # Analysis passes
  analysis/
    ...

  # Type system utilities (чистые функции)
  type_system/
    ...
```

## FAQ

**Q: Почему не всё в semantic_gen/utils/ или semantic_gen/services/?**
A: Потому что "services" и "utils" - слишком общие термины. Группировка по **функциональности** (scope/, builders/, inference/) более понятна.

**Q: Почему stdlib в compiler/, а не в semantic_gen/?**
A: Потому что stdlib scanning МОЖЕТ использоваться и в других фазах (например, analysis pass для проверки stdlib). Это cross-phase utility.

**Q: Почему TypeRegistry в core/, а не в корне?**
A: Вариант 2 выбран для более чёткого разделения:
- `core/` - центральные данные (registries)
- `infrastructure/` - механизмы (event bus, pass manager)
- `compiler/` - утилиты
Это соответствует Rust (rustc_middle), где центральные данные выделены отдельно.

**Q: Зачем разделять core/ и infrastructure/?**
A: Разные роли:
- `core/` - **данные** (state, registries) - "что знает компилятор"
- `infrastructure/` - **механизмы** (event bus, pass orchestration) - "как работает компилятор"
Аналог: в базах данных - schema (данные) vs engine (механизмы).

**Q: Как решить, куда отнести новый сервис?**
A: Спросить себя: "Будет ли этот сервис использоваться в других фазах (backend, analysis)?"
- Да или возможно → `compiler/`
- Нет, только semantic_gen → `semantic_gen/services/`
