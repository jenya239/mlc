# MLC Roadmap

Дата: апрель 2026.

## Текущее состояние

| Компонент | Путь | Состояние |
|-----------|------|-----------|
| Bootstrap-компилятор | `lib/mlc/` | Полнофункциональный, эталон семантики |
| Self-hosted компилятор | `compiler/` | Компилирует весь `compiler/`, `rake test_compiler_mlc` (227 тестов) |
| Triple-bootstrap | `compiler/triple_bootstrap.sh` | При актуальном `compiler/out/mlcc`: `diff bs2 bs3` пустой (стабильность self-host) |
| Runtime | `runtime/include/`, `runtime/src/` | C++20, стабильный |
| Unit-тесты Ruby | `test/mlc/` | 1106 runs, 0 failures |
| Unit-тесты MLC | `compiler/tests/` | `rake test_compiler_mlc`; опционально `rake triple_bootstrap` или `MLC_TRIPLE_BOOTSTRAP=1 rake test_self_hosted_stack` |
| E2E тесты mlcc | `compiler/tests/e2e/` | 5 программ (hello, fibonacci, result, sum_types, record_update), все проходят |

---

## Цели

1. **Надёжность**: доказанный self-bootstrap, покрытие крайних случаев, внятные диагностики
2. **Архитектура mlcc**: паттерны и сервисы вместо процедурного кода — по образцу `lib/mlc/`
3. **Паритет**: любая фича сначала в Ruby + тест, потом в `compiler/` + тест

---

## Фаза 1 — Triple-bootstrap (стабильность) ✓ ВЫПОЛНЕНО

**Критерий:** `diff out2/ out3/` пуст → компилятор стабилен.

Шаги:
1. Скрипт `compiler/triple_bootstrap.sh`:
   - `mlcc → /tmp/bs1/` → компиляция → `mlcc2`
   - `mlcc2 → /tmp/bs2/` → компиляция → `mlcc3`
   - `mlcc3 → /tmp/bs3/` → diff `bs2/ bs3/`
2. Интеграция: `rake triple_bootstrap`; при `MLC_TRIPLE_BOOTSTRAP=1` — после `test_self_hosted_stack`
3. Продвинутый контроль: `compiler/build.sh` в `.github/workflows/ci.yml`; triple-bootstrap по желанию локально или в отдельном джобе (долго).

**Метрика:** `bs2/*.cpp == bs3/*.cpp` для всех модулей `compiler/`.

---

## Фаза 2 — Архитектура codegen ✓ ВЫПОЛНЕНО

### Проблема (обновление)

Монолит `compiler/codegen.mlc` снят: код в каталоге `compiler/codegen/`. Взаимная рекурсия `gen_expr` ↔ `gen_stmts_str` остаётся в **`eval.mlc`** (граф модулей не допускает циклов). Слой **`expr.mlc`** — только сборка C++-строк из уже сгенерированных подвыражений (`import * as …` из `eval.mlc`).

### Цель

Структура по образцу `lib/mlc/backends/cpp/rules/`:

```
compiler/codegen/
  context.mlc          # CodegenContext, PrecomputedCtx, builder-функции
  expr.mlc             # фрагменты C++ (тернарный оператор, IIFE, std::visit, вызовы); обход SExpr в eval.mlc
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

## Фаза 3 — Архитектура checker ✓ ВЫПОЛНЕНО

### Проблема

`compiler/checker/infer.mlc` — dispatch по выражениям, вывод типов и диагностики; дальнейшее разбиение см. дерево ниже.

### Цель

```
compiler/checker/
  semantic_type_structure.mlc # предикаты и проекции формы Type (TArray/TShared и др.); далее unify/substitute над TypeExpr
  call_argument_unify.mlc   # согласование фактических аргументов с ожидаемыми типами и параметрами типа вызываемой fn / ctor
  expr_checker.mlc          # check_expr разбит на fn per-ExprXxx
  decl_checker.mlc          # check_decl, collect_globals
  context.mlc               # CheckContext (env, return_type, errors)
  infer_expr_ident.mlc      # разрешение ExprIdent (env, fn, ctor); без вызова infer_expr
  infer.mlc                 # infer_expr, infer_statements — диспетчер и остальные правила
```

---

## Фаза 4 — Диагностики ✓ ВЫПОЛНЕНО

В self-hosted цепочке: `Span` и `Diagnostic` в `compiler/ast.mlc`; checker накапливает `[Diagnostic]` с позициями из AST.

Реализовано: арность вызова для `TFn` и конструктора; согласованность аргументов с параметрами типа (`call_argument_unify.mlc`); «called value is not a function»; поля записи и доступ к не-записи / массиву; индексация; расхождение типов веток `if`; типы рук `match`; диапазон `for`; тип поля в record-конструкторе; тип return. Trait bounds при вызове generic fn.

Тесты: `assert_diagnostic_at` в `compiler/tests/test_checker.mlc` (224 unit-теста).

Добавлено: GCC-style формат `file:line:col: severity: message`; `?` на не-Result типе; проверка присвоения иммутабельным биндингам (`check_mutations.mlc`).

---

## Фаза 5 — Покрытие языка ✓ ВЫПОЛНЕНО

Фичи, реализованные в `lib/mlc/` но не покрытые тестами в `compiler/tests/`:

| Фича | Ruby-тест | MLC-тест | Статус |
|------|-----------|----------|--------|
| Generics с type bounds (`T: Display`) | есть | есть (`test_checker`, checker validates at call site) | ✓ |
| Trait dispatch (`extend Type : Trait`) | есть | есть (`test_checker`, `record_update` E2E) | ✓ |
| Trait bound check at call site | есть | есть (`test_checker`, `trait_bound_diagnostics`) | ✓ |
| Lambdas (`x => x + 1`) | есть | есть (`test_checker`) | расширять по мере фич |
| Record update (`Point { ...p, x: 10 }`) | есть | есть (`test_checker`, E2E `record_update`) | ✓ |
| User-defined `to_string` (не shadowed builtin) | нет | E2E `record_update` | ✓ исправлен баг |
| `let const` (constexpr) | есть | есть (`test_checker`, `test_codegen`) | ✓ |
| Вложенные generics (`Array<Result<T,E>>`) | есть | есть (`test_checker`) | ✓ |
| Multiline string literals (реальный `\n` в строке) | нет | есть (`test_checker`, `test_codegen`) | ✓ |
| Return type mismatch diagnostic | нет | есть (`test_checker`) | ✓ |

Порядок: сначала тест в `compiler/tests/`, затем фикс в `compiler/`.

---

## Зависимости фаз

```
Фаза 1 (triple-bootstrap)  — ✓ ВЫПОЛНЕНО
Фаза 2 (codegen arch)      — ✓ ВЫПОЛНЕНО
Фаза 3 (checker arch)      — ✓ ВЫПОЛНЕНО
Фаза 4 (диагностики)       — ✓ ВЫПОЛНЕНО
Фаза 5 (покрытие)          — ✓ ВЫПОЛНЕНО
```

---

## Принципы

- Каждое изменение в `compiler/` — зелёные тесты до и после
- Новая фича языка: Ruby-реализация + Ruby-тест первичны; затем MLC-реализация + MLC-тест
- `compiler/out/` синхронизируется после каждой серии изменений в `compiler/*.mlc`
- Монолит дробится механически (перенос, не переписывание); переписывание только с полным тестовым покрытием
