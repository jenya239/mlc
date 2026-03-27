# MLC Roadmap

Дата: март 2026.

## Текущее состояние

| Компонент | Путь | Состояние |
|-----------|------|-----------|
| Bootstrap-компилятор | `lib/mlc/` | Полнофункциональный, эталон семантики |
| Self-hosted компилятор | `compiler/` | Компилирует весь `compiler/`, 85 тестов проходят |
| Triple-bootstrap | `compiler/triple_bootstrap.sh` | Скрипт есть; `mlcc` генерирует C++, но сборка `g++ bs1 → mlcc2` пока падает на ошибках self-hosted codegen (невалидный C++: см. лог скрипта) |
| Runtime | `runtime/include/`, `runtime/src/` | C++20, стабильный |
| Unit-тесты Ruby | `test/mlc/` | 1106 runs, 0 failures |
| Unit-тесты MLC | `compiler/tests/` | 85 passed, 0 failed |
| E2E тесты mlcc | `compiler/tests/e2e/` | 4 программы, все проходят через mlcc |

---

## Цели

1. **Надёжность**: доказанный self-bootstrap, покрытие крайних случаев, внятные диагностики
2. **Архитектура mlcc**: паттерны и сервисы вместо процедурного кода — по образцу `lib/mlc/`
3. **Паритет**: любая фича сначала в Ruby + тест, потом в `compiler/` + тест

---

## Фаза 1 — Triple-bootstrap (стабильность)

**Критерий:** `diff out2/ out3/` пуст → компилятор стабилен.

Шаги:
1. Скрипт `compiler/triple_bootstrap.sh`:
   - `mlcc → /tmp/bs1/` → компиляция → `mlcc2`
   - `mlcc2 → /tmp/bs2/` → компиляция → `mlcc3`
   - `mlcc3 → /tmp/bs3/` → diff `bs2/ bs3/`
2. Интеграция в `rake test_self_hosted_stack` опциональным флагом
3. Результат зафиксировать в артефакте CI

**Метрика:** `bs2/*.cpp == bs3/*.cpp` для всех модулей `compiler/`.

---

## Фаза 2 — Архитектура codegen

### Проблема

`compiler/codegen.mlc` — 1655 строк, один файл.  
`gen_expr` — 500+ строк, 30+ веток `match`.  
Нет изоляции логики по AST-узлам.

### Цель

Структура по образцу `lib/mlc/backends/cpp/rules/`:

```
compiler/codegen/
  context.mlc          # CodegenContext, PrecomputedCtx, builder-функции
  expr.mlc             # gen_expr разбит на fn per-ExprXxx
  decl.mlc             # gen_decl, gen_proto, collect_decl_parts
  type.mlc             # gen_type, gen_adt_fwd, gen_adt_defs
  pattern.mlc          # gen_arm, gen_pattern
  module.mlc           # gen_module, decls_have_main
```

Каждый модуль:
- Экспортирует публичный интерфейс (`export fn`)
- Принимает `CodegenContext` явно, не через globals
- Покрыт тестами в `compiler/tests/test_codegen.mlc`

### Стратегия разбивки

Не переписывать — механически выносить функции по одной с сохранением поведения.  
Каждый перенос функции → отдельный коммит + зелёные тесты.

---

## Фаза 3 — Архитектура checker

### Проблема

`compiler/checker/infer.mlc` — dispatch по выражениям, вывод типов и диагностики; дальнейшее разбиение см. дерево ниже.

### Цель

```
compiler/checker/
  types.mlc            # операции над TypeExpr: unify, substitute, occurs_check
  expr_checker.mlc     # check_expr разбит на fn per-ExprXxx
  decl_checker.mlc     # check_decl, collect_globals
  context.mlc          # CheckContext (env, return_type, errors)
  infer.mlc            # точка входа: check(), check_with_context()
```

---

## Фаза 4 — Диагностики

В self-hosted цепочке: `Span` и `Diagnostic` в `compiler/ast.mlc`; checker накапливает `[Diagnostic]` с позициями из AST (например `expr_span`, спаны вызова / поля / индекса).

В `compiler/checker/infer.mlc` среди прочего: арность вызова для `TFn` и конструктора; «called value is not a function»; поля записи и доступ к не-записи / массиву; индексация (не массив; индекс не `i32`); расхождение типов веток `if`; согласованность типов рук `match`; диапазон `for` не массив.

Тесты позиций и текста: `assert_diagnostic_at` в `compiler/tests/test_checker.mlc`.

Дальше: больше узлов и сообщений; при желании единый формат вывода как у Ruby-компилятора.

---

## Фаза 5 — Покрытие языка

Фичи, реализованные в `lib/mlc/` но не покрытые тестами в `compiler/tests/`:

| Фича | Ruby-тест | MLC-тест | Статус |
|------|-----------|----------|--------|
| Generics с type bounds (`T: Display`) | есть | нет | нужен |
| Trait dispatch (`extend Type : Trait`) | есть | нет | нужен |
| Lambdas (`x => x + 1`) | есть | есть (`test_checker`) | расширять по мере фич |
| Record update (`{ old \| field: val }`) | есть | частично | расширить |
| `let const` (constexpr) | есть | нет | нужен |
| Вложенные generics (`Array<Result<T,E>>`) | есть | нет | нужен |
| Multiline string literals | нет | нет | зафиксировать |

Порядок: сначала тест в `compiler/tests/`, затем фикс в `compiler/`.

---

## Зависимости фаз

```
Фаза 1 (triple-bootstrap)  — скрипт + критерий diff; блокер: починить emit mlcc так, чтобы g++ собирал bs1 без ошибок
Фаза 2 (codegen arch)      — в работе (снижает риск блокера Фазы 1)
Фаза 3 (checker arch)      — параллельно с Фазой 2
Фаза 4 (диагностики)       — после Фаз 2+3
Фаза 5 (покрытие)          — параллельно с любой фазой
```

---

## Принципы

- Каждое изменение в `compiler/` — зелёные тесты до и после
- Новая фича языка: Ruby-реализация + Ruby-тест первичны; затем MLC-реализация + MLC-тест
- `compiler/out/` синхронизируется после каждой серии изменений в `compiler/*.mlc`
- Монолит дробится механически (перенос, не переписывание); переписывание только с полным тестовым покрытием
