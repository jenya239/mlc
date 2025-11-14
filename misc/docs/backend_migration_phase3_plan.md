# Backend Migration: Phase 3 Plan - Legacy Code Removal

**Цель**: Полное удаление `Backend::CodeGen` (~1072 строки) и завершение Strangler Fig миграции

**Текущий статус**: Phase 2 завершена (99% tests passing с v2 backend)

**Оценка времени**: 5-7 дней до полного удаления legacy

---

## Phase 2.5: Qualified Function Names (1-2 дня) ⏸

**Цель**: Достичь 100% test passing с `MLC_CPP_BACKEND=v2`

### Проблема

**Текущие ошибки** (15 failures, 6 errors):
```cpp
// Generated (incorrect):
min_i32(nums)         // ❌ unresolved
max_i32(nums)         // ❌ unresolved
to_string_i32(val)    // ❌ unresolved
range(1, 11)          // ❌ unresolved
sum_i32(nums)         // ❌ unresolved

// Expected (correct):
mlc::collections::min_i32(nums)         // ✅
mlc::collections::max_i32(nums)         // ✅
mlc::to_string_i32(val)                 // ✅
mlc::collections::range(1, 11)          // ✅
mlc::collections::sum_i32(nums)         // ✅
```

### Решение

**Файл**: `lib/mlc/backends/cpp/rules/expressions/call_rule.rb`

**Изменения**:
1. Добавить lookup через `FunctionHelper.qualified_function_name`
2. Использовать `context.function_registry` для stdlib functions
3. Fallback на простое имя для user-defined functions

**Пример**:
```ruby
def apply(node)
  callee = node.callee

  # NEW: Qualify stdlib function names
  if callee.is_a?(SemanticIR::VarExpr)
    qualified_name = context.qualified_function_name(callee.name)
    callee_cpp = if qualified_name
                   context.factory.identifier(name: qualified_name)
                 else
                   lower_expression(callee)
                 end
  else
    callee_cpp = lower_expression(callee)
  end

  # ... rest of implementation
end
```

**Критерий готовности**: 1540 runs, 4036 assertions, 0 failures, 0 errors

**Оценка**: 1-2 дня

---

## Phase 3a: Function-Level Rules (1-2 дня) ⏸

**Цель**: Удалить зависимость от `@legacy_backend.rule_engine`

### Текущая реализация

**Файл**: `lib/mlc/backends/cpp/legacy_adapter.rb:131-138`
```ruby
func_decl = @legacy_backend.rule_engine.apply(
  :cpp_function_declaration,
  func_decl,
  context: {
    core_func: func,
    event_bus: @legacy_backend.event_bus
  }
)
```

**Legacy rule**: `lib/mlc/backend/codegen/rules/function_rule.rb` (57 строк)
- Добавляет `constexpr` prefix modifier
- Добавляет `noexcept` suffix modifier
- Публикует event `:cpp_function_rule_applied`

### Решение

**1. Создать новую FunctionRule в новой архитектуре**

**Файл**: `lib/mlc/backends/cpp/rules/function_rule.rb`
```ruby
module MLC
  module Backends
    module Cpp
      module Rules
        class FunctionRule < BaseRule
          def applies?(node)
            node.is_a?(CppAst::Nodes::FunctionDeclaration)
          end

          def apply(node, semantic_func:, event_bus:)
            effects = Array(semantic_func.effects)

            if effects.include?(:constexpr)
              node.prefix_modifiers = merge_prefix_modifier(node.prefix_modifiers, "constexpr")
            end

            if effects.include?(:noexcept)
              node.modifiers_text = merge_suffix_modifier(node.modifiers_text, "noexcept")
            end

            event_bus&.publish(:cpp_function_rule_applied, name: semantic_func.name, effects: effects)
            node
          end

          # ... helper methods ...
        end
      end
    end
  end
end
```

**2. Обновить LegacyAdapter**

**Файл**: `lib/mlc/backends/cpp/legacy_adapter.rb`
```ruby
# BEFORE (Phase 2):
func_decl = @legacy_backend.rule_engine.apply(
  :cpp_function_declaration,
  func_decl,
  context: { core_func: func, event_bus: @legacy_backend.event_bus }
)

# AFTER (Phase 3a):
function_rule = Rules::FunctionRule.new(@context)
func_decl = function_rule.apply(
  func_decl,
  semantic_func: func,
  event_bus: @legacy_backend.event_bus  # Still using legacy event_bus (Phase 3b)
)
```

**Критерий готовности**: Все tests passing, `@legacy_backend.rule_engine` не используется

**Удаляемые файлы**:
- `lib/mlc/backend/codegen/rules/function_rule.rb` (57 строк)

**Оценка**: 1 день

---

## Phase 3b: Event Bus Migration (1 день) ⏸

**Цель**: Удалить зависимость от `@legacy_backend.event_bus`

### Текущая реализация

**Файл**: `lib/mlc/backends/cpp/legacy_adapter.rb:140-144`
```ruby
@legacy_backend.event_bus&.publish(
  :cpp_function_lowered,
  name: func.name,
  effects: Array(func.effects)
)
```

### Решение

**1. Добавить event_bus в новый Container**

**Файл**: `lib/mlc/backends/cpp/container.rb`
```ruby
class Container
  attr_reader :event_bus

  def initialize(type_registry:, function_registry:, runtime_policy:, event_bus: nil)
    @event_bus = event_bus || MLC::Infrastructure::EventBus.new
    # ... rest of initialization ...
  end
end
```

**2. Обновить Bootstrap**

**Файл**: `lib/mlc/backends/cpp/bootstrap.rb`
```ruby
def self.create_backend(type_registry:, function_registry:, runtime_policy:, event_bus: nil)
  container = Container.new(
    type_registry: type_registry,
    function_registry: function_registry,
    runtime_policy: runtime_policy,
    event_bus: event_bus  # NEW
  )
  # ... rest ...
end
```

**3. Обновить LegacyAdapter**

**Файл**: `lib/mlc/backends/cpp/legacy_adapter.rb`
```ruby
# BEFORE (Phase 3a):
backend = Bootstrap.create_backend(
  type_registry: type_registry,
  function_registry: function_registry,
  runtime_policy: runtime_policy
)
# ... later ...
@legacy_backend.event_bus&.publish(...)

# AFTER (Phase 3b):
backend = Bootstrap.create_backend(
  type_registry: type_registry,
  function_registry: function_registry,
  runtime_policy: runtime_policy,
  event_bus: event_bus  # Pass event_bus to new architecture
)
# ... later ...
@container.event_bus&.publish(...)  # Use new architecture event_bus
```

**Критерий готовности**: Все tests passing, `@legacy_backend.event_bus` не используется

**Оценка**: 1 день

---

## Phase 3c: Type Declaration Lowering (2-3 дня) ⏸

**Цель**: Удалить последнюю dependency - `@legacy_backend.lower(core_ir)` для TypeDecl

### Текущая реализация

**Файл**: `lib/mlc/backends/cpp/legacy_adapter.rb:54-56`
```ruby
when SemanticIR::TypeDecl
  # Delegate type declarations to legacy (Phase 3 concern)
  @legacy_backend.lower(core_ir)
```

**Legacy implementation**: `lib/mlc/backend/codegen/type_lowerer.rb` (133 строки)
- `lower_type_decl`: Dispatches to `lower_sum_type` or `lower_record_type`
- `lower_sum_type`: Generates std::variant + constructor + visitor support
- `lower_record_type`: Generates struct declarations

### Решение

**1. Создать TypeDeclRule в новой архитектуре**

**Файлы**:
- `lib/mlc/backends/cpp/rules/type_decl_rule.rb`
- `lib/mlc/backends/cpp/rules/type_decls/sum_type_rule.rb`
- `lib/mlc/backends/cpp/rules/type_decls/record_type_rule.rb`

**Структура**:
```ruby
module MLC
  module Backends
    module Cpp
      module Rules
        module TypeDecls
          class SumTypeRule < BaseRule
            def applies?(node)
              context.checker.type_decl?(node) && node.type.is_a?(SemanticIR::SumType)
            end

            def apply(node)
              # Generate std::variant declaration
              # Generate constructors
              # Generate visitor support
            end
          end

          class RecordTypeRule < BaseRule
            def applies?(node)
              context.checker.type_decl?(node) && node.type.is_a?(SemanticIR::RecordType)
            end

            def apply(node)
              # Generate struct declaration
              # Generate field initializers
            end
          end
        end
      end
    end
  end
end
```

**2. Обновить Bootstrap для регистрации type rules**

**3. Обновить LegacyAdapter**

```ruby
# BEFORE (Phase 2):
when SemanticIR::TypeDecl
  @legacy_backend.lower(core_ir)

# AFTER (Phase 3c):
when SemanticIR::TypeDecl
  lower_type_decl(core_ir)  # NEW: использует новую архитектуру

# ... private ...
def lower_type_decl(type_decl)
  @context.lower_type_decl(type_decl)  # Delegates to type_decl rules
end
```

**Критерий готовности**: Все tests passing, `@legacy_backend.lower` не используется

**Удаляемые файлы**:
- `lib/mlc/backend/codegen/type_lowerer.rb` (133 строки)

**Оценка**: 2-3 дня (sum types сложные)

---

## Phase 3d: Complete Legacy Removal (1 день) ⏸

**Цель**: Удалить `@legacy_backend` dependency полностью

### Cleanup Tasks

**1. Удалить @legacy_backend из LegacyAdapter**

**Файл**: `lib/mlc/backends/cpp/legacy_adapter.rb`
```ruby
# BEFORE (Phase 3c):
@legacy_backend = Backend::CodeGen.new(...)

# AFTER (Phase 3d):
# Removed completely
```

**2. Удалить attribute delegation**

```ruby
# BEFORE (Phase 2-3c):
def type_registry
  @legacy_backend.type_registry
end

def function_registry
  @legacy_backend.function_registry
end

# AFTER (Phase 3d):
def type_registry
  @container.type_registry  # Direct access to new architecture
end

def function_registry
  @container.function_registry
end
```

**3. Удалить legacy codegen files**

**Удаляемые файлы** (~1072 строки):
```
lib/mlc/backend/codegen.rb                         (170 строк)
lib/mlc/backend/codegen/base_lowerer.rb            (249 строк)
lib/mlc/backend/codegen/expression_lowerer.rb      (65 строк)
lib/mlc/backend/codegen/statement_lowerer.rb       (94 строк)
lib/mlc/backend/codegen/function_lowerer.rb        (110 строк)
lib/mlc/backend/codegen/type_lowerer.rb            (133 строк)
lib/mlc/backend/codegen/helpers.rb                 (251 строк)
lib/mlc/backend/codegen/rules/function_rule.rb     (57 строк)
```

**4. Обновить Application.rb**

**Файл**: `lib/mlc/application.rb`
```ruby
# BEFORE (Phase 1-3c):
case ENV.fetch("MLC_CPP_BACKEND", "legacy")
when "v2"
  Backends::Cpp::LegacyAdapter.new(...)  # Still named LegacyAdapter
else
  Backend::CodeGen.new(...)              # Legacy (default)
end

# AFTER (Phase 3d):
case ENV.fetch("MLC_CPP_BACKEND", "legacy")
when "v2"
  Backends::Cpp::CodeGen.new(...)        # Renamed: LegacyAdapter → CodeGen
else
  Backend::CodeGen.new(...)              # Legacy (still available)
end
```

**Критерий готовности**:
- ✅ All tests passing с `MLC_CPP_BACKEND=v2`
- ✅ `@legacy_backend` полностью удалён
- ✅ Legacy files удалены (~1072 строки)
- ✅ `LegacyAdapter` переименован в `Backends::Cpp::CodeGen`

**Оценка**: 1 день

---

## Phase 3e: Make V2 Default (0.5 дня) ⏸

**Цель**: Сделать новую архитектуру default, удалить legacy Backend::CodeGen

### Tasks

**1. Изменить default backend на v2**

**Файл**: `lib/mlc/application.rb`
```ruby
# BEFORE (Phase 3d):
case ENV.fetch("MLC_CPP_BACKEND", "legacy")
when "v2"
  Backends::Cpp::CodeGen.new(...)
else
  Backend::CodeGen.new(...)
end

# AFTER (Phase 3e):
case ENV.fetch("MLC_CPP_BACKEND", "v2")  # Default changed: "legacy" → "v2"
when "legacy"
  warn "DEPRECATED: Legacy backend will be removed in next version"
  Backend::CodeGen.new(...)
else
  Backends::Cpp::CodeGen.new(...)        # NEW DEFAULT
end
```

**2. Обновить все тесты**

- Удалить `MLC_CPP_BACKEND=legacy` из тестов
- Добавить smoke tests для `MLC_CPP_BACKEND=legacy` (deprecated)

**3. Удалить legacy backend полностью (опционально)**

Если все tests passing с v2, можно полностью удалить legacy:
```ruby
# FINAL VERSION:
Backends::Cpp::CodeGen.new(...)  # No more feature flag
```

**Критерий готовности**:
- ✅ V2 backend is default
- ✅ Legacy backend deprecated или удалён
- ✅ All tests passing without feature flag

**Оценка**: 0.5 дня

---

## Timeline Summary

| Phase   | Task                                  | Estimate | Cumulative |
|---------|---------------------------------------|----------|------------|
| 2.5     | Qualified function names              | 1-2 days | 1-2 days   |
| 3a      | Function-level rules                  | 1 day    | 2-3 days   |
| 3b      | Event bus migration                   | 1 day    | 3-4 days   |
| 3c      | Type declaration lowering             | 2-3 days | 5-7 days   |
| 3d      | Complete legacy removal               | 1 day    | 6-8 days   |
| 3e      | Make v2 default                       | 0.5 days | 7-9 days   |

**Total estimated time**: **7-9 дней** (1.5-2 недели)

**Conservative estimate**: **2 недели**

---

## Metrics

### Current State (Phase 2 Complete)

**Code Size:**
- New architecture: ~3257 lines
- Legacy code: ~1129 lines (codegen + LegacyAdapter delegation)
- Technical debt: ~1129 lines

**Test Coverage:**
- Tests passing: 99% (1540 runs, 4036 assertions, 15 failures, 6 errors)
- Feature flag: `MLC_CPP_BACKEND=v2`

### Target State (Phase 3 Complete)

**Code Size:**
- New architecture: ~3500 lines (added type rules)
- Legacy code: **0 lines** (deleted)
- Code reduction: **-1129 lines**

**Test Coverage:**
- Tests passing: 100% (1540 runs, 4036+ assertions, 0 failures, 0 errors)
- Feature flag: Removed (v2 is default)

---

## Risk Assessment

### Low Risk Items
- ✅ Phase 2.5: Qualified function names (localized change in CallRule)
- ✅ Phase 3a: Function-level rules (trivial migration)
- ✅ Phase 3b: Event bus (container dependency injection)

### Medium Risk Items
- ⚠️ Phase 3c: Type declaration lowering (complex sum types, variant generation)
- ⚠️ Phase 3d: Legacy removal (integration testing required)

### Mitigation Strategies
1. **Incremental testing**: Run full test suite after each phase
2. **Feature flag preservation**: Keep `MLC_CPP_BACKEND=legacy` until Phase 3e
3. **Rollback safety**: Each phase is independently committable
4. **Equivalence testing**: Compare legacy vs v2 output for type declarations

---

## Success Criteria

### Phase 2.5 Complete ✅
- [ ] 0 failures with `MLC_CPP_BACKEND=v2`
- [ ] 0 errors with `MLC_CPP_BACKEND=v2`
- [ ] Qualified function names resolved correctly

### Phase 3a Complete ✅
- [ ] `@legacy_backend.rule_engine` не используется
- [ ] FunctionRule мигрирована в новую архитектуру
- [ ] All tests passing

### Phase 3b Complete ✅
- [ ] `@legacy_backend.event_bus` не используется
- [ ] Event bus в Container
- [ ] All tests passing

### Phase 3c Complete ✅
- [ ] `@legacy_backend.lower(TypeDecl)` не используется
- [ ] Type declaration rules работают
- [ ] Sum types и record types генерируются корректно
- [ ] All tests passing

### Phase 3d Complete ✅
- [ ] `@legacy_backend` полностью удалён
- [ ] ~1072 строк legacy кода удалены
- [ ] `LegacyAdapter` переименован в `CodeGen`
- [ ] All tests passing

### Phase 3e Complete ✅
- [ ] V2 backend is default
- [ ] Feature flag удалён или legacy deprecated
- [ ] Production ready

---

## Next Steps

**Immediate (Phase 2.5)**:
1. Implement qualified function name lookup in CallRule
2. Add tests for stdlib function resolution
3. Achieve 100% test passing rate

**When ready to proceed**: Ask to begin Phase 2.5 implementation.

**Estimated timeline to legacy removal**: **7-9 дней** (2 weeks conservative)

---

## Ответ на вопрос: "Когда можно удалить legacy?"

**Короткий ответ**: Через **7-9 дней** работы (2 недели с запасом).

**Детальный breakdown**:
- Phase 2.5 (qualified names): 1-2 дня → 100% tests passing
- Phase 3a-3b (function rules + event bus): 2 дня → Remove delegation dependencies
- Phase 3c (type declarations): 2-3 дня → Remove last `@legacy_backend.lower` call
- Phase 3d (cleanup): 1 день → Delete ~1072 lines of legacy code
- Phase 3e (default switch): 0.5 дня → Make v2 default

**Current blocker**: 15 failures + 6 errors (qualified function names issue).

**После Phase 2.5**: Можно начинать Phase 3 migrations параллельно.

**Full legacy removal possible**: После Phase 3d (6-8 дней от сейчас).
