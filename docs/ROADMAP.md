# MLC Roadmap

Дата: март 2026.

## Текущее состояние

| Компонент | Путь | Состояние |
|-----------|------|-----------|
| Bootstrap-компилятор | `lib/mlc/` | Полнофункциональный, эталон семантики |
| Self-hosted компилятор | `compiler/` | Компилирует весь `compiler/`, 85 тестов проходят |
| Triple-bootstrap | `mlcc → mlcc2 → mlcc3` | Не проверялась идентичность `out2 == out3` |
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

`compiler/checker/infer.mlc` — 309 строк, смешаны: unification, constraint solving, dispatch по AST-узлам.

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

Сейчас ошибки: `[string]`. Нет позиций.

```mlc
type Span = { file: string, line: i32, column: i32 }
type Diagnostic = { message: string, span: Span, severity: string }
```

- Lexer пробрасывает `Span` в токены (уже есть поля `line`/`col` в AST-токенах — проверить)
- Parser прикрепляет `Span` к каждому AST-узлу
- Checker использует `Span` из AST для `Diagnostic`
- Codegen: ошибки уже не генерирует, но паникует — заменить на `Result<string, Diagnostic>`

Тесты: `assert_diagnostic_at(source, line, col, "message")`.

---

## Фаза 5 — Покрытие языка

Фичи, реализованные в `lib/mlc/` но не покрытые тестами в `compiler/tests/`:

| Фича | Ruby-тест | MLC-тест | Статус |
|------|-----------|----------|--------|
| Generics с type bounds (`T: Display`) | есть | нет | нужен |
| Trait dispatch (`extend Type : Trait`) | есть | нет | нужен |
| Lambdas (`x => x + 1`) | есть | нет | нужен |
| Record update (`{ old \| field: val }`) | есть | частично | расширить |
| `let const` (constexpr) | есть | нет | нужен |
| Вложенные generics (`Array<Result<T,E>>`) | есть | нет | нужен |
| Multiline string literals | нет | нет | зафиксировать |

Порядок: сначала тест в `compiler/tests/`, затем фикс в `compiler/`.

---

## Фаза 6 — CI

Текущее состояние: нет CI-конфига (`.github/workflows/`).

Минимальный пайплайн:
```yaml
jobs:
  test_ruby:
    - bundle exec rake test_mlc
  test_self_hosted:
    - bundle exec rake test_compiler_mlc   # build_tests.sh → 85 тестов
  test_e2e:
    - compiler/tests/e2e/run_e2e.sh        # 4 E2E программы
```

Дополнительно (медленный job):
- `build_tests_self.sh compiler/out/mlcc` — тесты через self-hosted бинарь
- Triple-bootstrap diff

---

## Зависимости фаз

```
Фаза 1 (triple-bootstrap)  — независима, можно начать сейчас
Фаза 2 (codegen arch)      — после Фазы 1 (чтобы рефакторинг был стабилен)
Фаза 3 (checker arch)      — параллельно с Фазой 2
Фаза 4 (диагностики)       — после Фаз 2+3 (нужна стабильная архитектура)
Фаза 5 (покрытие)          — параллельно с любой фазой
Фаза 6 (CI)                — после Фазы 1
```

---

## Принципы

- Каждое изменение в `compiler/` — зелёные тесты до и после
- Новая фича языка: Ruby-реализация + Ruby-тест первичны; затем MLC-реализация + MLC-тест
- `compiler/out/` синхронизируется после каждой серии изменений в `compiler/*.mlc`
- Монолит дробится механически (перенос, не переписывание); переписывание только с полным тестовым покрытием
