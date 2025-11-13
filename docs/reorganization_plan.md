# План реорганизации структуры lib/mlc/

## Текущее состояние

**Статистика:**
- 152 файла в lib/mlc/
- 266 require_relative зависимостей
- Хаотичная структура на первом/втором уровне

## Целевая структура

```
lib/mlc/
├── source/              # Исходный синтаксис (AST, Parser)
│   ├── ast/
│   └── parser/
├── representations/     # Промежуточные представления
│   └── semantic/
│       ├── ir/          # semantic_ir → здесь
│       └── gen/         # semantic_gen → здесь
├── backends/            # Бэкенды (без изменений)
│   └── cpp/
├── common/              # Общие компоненты
│   ├── core/            # FunctionSignature, FunctionRegistry, TypeRegistry
│   ├── type_system/     # TypeConstraintSolver, MatchAnalyzer
│   ├── diagnostics/     # EventBus, EventLogger, Formatter, Errors
│   └── analysis/        # PassManager, анализы
├── tools/               # Инструменты
│   └── compiler/        # stdlib scanner, resolver, registry, metadata loader
└── stdlib/              # .mlc исходники (БЕЗ ИЗМЕНЕНИЙ)
```

## Mapping директорий

| Старый путь    | Новый путь                      | Файлов | Зависимостей |
|----------------|---------------------------------|--------|--------------|
| ast            | source/ast                      | 1      | 3            |
| parser         | source/parser                   | 10     | 1            |
| semantic_ir    | representations/semantic/ir     | 2      | 1            |
| semantic_gen   | representations/semantic/gen    | 68     | 51           |
| core           | common/core                     | 3      | 8            |
| type_system    | common/type_system              | 4      | 8            |
| diagnostics    | common/diagnostics              | 5      | 3            |
| analysis       | common/analysis                 | 5      | 1            |
| compiler       | tools/compiler                  | 4      | 4            |

## Порядок миграции

**Принцип:** от независимых к зависимым (по количеству внешних зависимостей)

1. **parser** → source/parser (10 файлов, 1 зависимость)
2. **semantic_ir** → representations/semantic/ir (2 файла, 1 зависимость)
3. **analysis** → common/analysis (5 файлов, 1 зависимость)
4. **ast** → source/ast (1 файл, 3 зависимости)
5. **diagnostics** → common/diagnostics (5 файлов, 3 зависимости)
6. **compiler** → tools/compiler (4 файла, 4 зависимости)
7. **core** → common/core (3 файла, 8 зависимостей)
8. **type_system** → common/type_system (4 файла, 8 зависимостей)
9. **semantic_gen** → representations/semantic/gen (68 файлов, 51 зависимость)

## Процесс миграции для каждого шага

1. **Создание директорий** (если ещё не созданы)
2. **Перемещение файлов** через `git mv` (сохраняет историю)
3. **Обновление require_relative** через regex замены:
   - `require_relative 'old_path/...'` → `require_relative 'new_path/...'`
   - `require_relative "../old_path/..."` → `require_relative "../new_path/..."`
   - `require_relative "mlc/old_path/..."` → `require_relative "mlc/new_path/..."`
4. **Запуск тестов** - проверка на регрессии
5. **Коммит** с описанием изменений
6. **Откат при ошибке** - `git reset --hard HEAD`

## Файлы с наибольшими зависимостями

Эти файлы потребуют особого внимания при обновлении:

1. **semantic_gen.rb** - 49 requires
2. **semantic_gen/engine.rb** - 34 requires
3. **semantic_gen/services/container.rb** - 30 requires
4. **backends/cpp/bootstrap.rb** - 29 requires
5. **semantic_gen/pipeline.rb** - 10 requires

## Автоматизация

Созданы скрипты:

1. **scripts/analyze_reorganization.rb** - анализ текущей структуры
2. **scripts/migrate_structure.rb** - автоматическая миграция

### Использование

```bash
# Анализ структуры
ruby scripts/analyze_reorganization.rb

# Проверка плана (dry-run)
ruby scripts/migrate_structure.rb --dry-run

# Выполнение миграции с тестами
ruby scripts/migrate_structure.rb

# Выполнение миграции без тестов (быстрее, но рискованнее)
ruby scripts/migrate_structure.rb --no-tests
```

## Проверка после миграции

1. Запуск полного набора тестов: `MLC_CPP_BACKEND=v2 rake test`
2. Проверка отсутствия старых путей: `grep -r "require_relative.*semantic_gen" lib/`
3. Проверка структуры: `tree -L 3 lib/mlc/`

## Откат при проблемах

Если миграция пошла не так:

```bash
# Откат последнего коммита
git reset --hard HEAD~1

# Откат всех изменений до начала миграции
git reset --hard <commit-hash-before-migration>
```

## Ожидаемые результаты

1. ✅ Чёткая логическая структура директорий
2. ✅ Разделение по уровням абстракции (source → representations → backends)
3. ✅ Общие компоненты в common/
4. ✅ Инструменты в tools/
5. ✅ Все тесты проходят
6. ✅ История файлов сохранена через git mv

## Риски и митигация

| Риск | Вероятность | Митигация |
|------|-------------|-----------|
| Пропущены require_relative | Средняя | Автоматические regex замены + тесты |
| Циклические зависимости | Низкая | Порядок миграции от независимых к зависимым |
| Конфликты слияния | Средняя | Выполнять миграцию в отдельной ветке |
| Регрессии в тестах | Средняя | Запуск тестов после каждого шага + откат при ошибках |

## Время выполнения

**Оценка:**
- С тестами после каждого шага: ~20-30 минут
- Без тестов (только финальная проверка): ~5 минут

**Рекомендация:** выполнять с тестами для безопасности.
