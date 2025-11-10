# ЭТАП 5: Безопасный план рефакторинга TypeChecker

## Анализ текущего состояния

**TypeChecker:** 355 строк, 28+ публичных методов

**Статистика использования методов (в semantic_gen/):**
- type_error: 12 использований
- ensure_argument_count: 10
- ensure_type!: 9
- ensure_compatible_type: 9
- type_name: 8
- ensure_numeric_type: 8
- ensure_boolean_type: 7
- normalize_type_params: 6
- normalized_type_name: 4
- infer_type_kind: 2
- assign_expression_type: 2
- validate_function_call: 2
- io_return_type: 2
- describe_type: 2
- module_member_info: 1

**Методы с 0 external usage (используются только внутри TypeChecker):**
- unit_like? → используется в ensure_compatible_type (2x)
- generic_type_name? → используется в ensure_compatible/boolean/numeric (3x)
- numeric_type? → используется в ensure_numeric_type (1x)
- record_field_type_map → используется в propagate_literal_types (1x)
- validate_constraint_name → используется в normalize_type_params (1x)
- extract_actual_type_name → используется в validate_type_constraints (1x)
- type_satisfies_constraint? → используется в validate_type_constraints (1x)
- validate_type_constraints → (не вызывается, возможно legacy)

## Проблема агрессивного разбиения

**Circular dependencies:**
- ensure_compatible_type зависит от: generic_type_name?, unit_like?, type_error, type_name
- ensure_numeric_type зависит от: numeric_type?, type_error, describe_type
- describe_type зависит от: type_name, normalized_type_name

Разбиение на TypeValidator + TypeNameResolver + ErrorReporter создаст:
1. TypeValidator нужен ErrorReporter и TypeNameResolver
2. TypeNameResolver нужен ErrorReporter (для type_error в некоторых случаях)
3. Сложная инициализация в container.rb
4. Высокий риск регрессий

## БЕЗОПАСНОЕ РЕШЕНИЕ: Минимальное извлечение

### Вариант 1 (РЕКОМЕНДУЕМЫЙ): Не разбивать TypeChecker

**Обоснование:**
- 355 строк - приемлемый размер для сервиса
- Методы тесно связаны (high cohesion)
- Все методы связаны с валидацией типов (single responsibility)
- Разбиение создаст больше проблем, чем решит

**Действия:**
1. Добавить комментарии-разделители в TypeChecker для группировки методов
2. Улучшить документацию методов
3. ЭТАП 5 завершается без разбиения TypeChecker

### Вариант 2 (АЛЬТЕРНАТИВНЫЙ): Минимальное извлечение ErrorReporter

**Извлечь только изолированные методы:**

**ErrorReporter (2 метода, полностью изолированные):**
- type_error (строки 67-71)
- ensure_type! (строки 73-75) - зависит только от type_error

**Остальное остаётся в TypeChecker.**

**Плюсы:**
- Минимальный риск
- ErrorReporter полностью изолирован
- Простая миграция (~12 файлов с type_error)

**Минусы:**
- Малая польза (извлекаем только 2 метода)

### Вариант 3: Проверить TypeInferenceService

**TypeInferenceService:** 592 строки - более крупный кандидат

Сначала проанализировать TypeInferenceService, возможно там разбиение более очевидно.

## Рекомендация

**ВАРИАНТ 1** - не разбивать TypeChecker, перейти к анализу TypeInferenceService.

**Причины:**
1. TypeChecker имеет хорошую структуру (high cohesion, single responsibility)
2. Circular dependencies усложнят код без реальной пользы
3. 355 строк - приемлемый размер
4. Время лучше потратить на анализ TypeInferenceService (592 строки)

## Следующие шаги

1. ✅ Аудит TypeChecker завершён
2. ⏭️ Аудит TypeInferenceService
3. ⏭️ Решение о разбиении TypeInferenceService
4. ⏭️ ЭТАП 6 (финализация)
