# План self-hosted компилятора MLC

Документ фиксирует дорожную карту, критерии готовности и политику по Ruby.  
Исходники self-hosted: `compiler/*.mlc`. Эталон и первая ступень сборки: `lib/mlc/` (Ruby).

---

## Политика Ruby: резерв и bootstrap

- **`lib/mlc/` не удалять и не выводить из репозитория** — полнофункциональный компилятор, эталон семантики и codegen, резерв при регрессиях self-hosted.
- **Сборка `mlcc` по умолчанию** остаётся через `compiler/build.sh` (Ruby `ModularCompiler`).
- Любое расхождение Ruby vs `compiler/out/mlcc` — баг self-hosted до доказательства обратного; Ruby правит только осознанно (спека/тест).
- Долгосрочная цель «сборка без Ruby» — **дополнительная** цепочка, а не замена удалённого кода.

---

## 1. Цель и критерии «готово»

**Цель:** компилятор MLC на MLC с минимальной зависимостью от Ruby (только первая ступень или вынесенный артефакт).

**Критерии:**

1. Поддерживаемое подмножество языка даёт согласованный результат Ruby-пайплайна и self-hosted `mlcc` на общем наборе тестов.
2. Воспроизводимая цепочка: `compiler/*.mlc` → C++ → g++ → бинарь; опционально второй круг (bootstrap closure).
3. Регрессии ловятся: Ruby unit-тесты + `compiler/tests` + выборочно E2E.

---

## 2. Текущая архитектура (факты)

| Компонент | Путь |
|-----------|------|
| Bootstrap (полный стек) | `lib/mlc/` |
| Self-hosted | `compiler/` (lexer, parser, checker, codegen) |
| Runtime | `runtime/include`, `runtime/src` |
| Сборка `mlcc` | `compiler/build.sh` |
| Bootstrap C++ бинарь | `MLCC_BOOTSTRAP=1 compiler/build.sh` → `compiler/out/bootstrap/mlcc_bootstrap` |
| Тесты Ruby (MLC) | `test/mlc/**/*_test.rb`, `rake test_mlc` |
| Тесты Ruby (шире) | `rake test_unit` — весь `test/` кроме `integration` |
| Тесты компилятора MLC | `compiler/tests/`, `compiler/tests/build_tests.sh`, `rake test_compiler_mlc` |

---

## 3. Фазы работ

### Фаза A — Паритет self-hosted и Ruby

- **A.1** Семантика/типы: `compiler/checker/` vs `lib/mlc/representations/semantic/gen/`.
- **A.2** Codegen: `compiler/codegen.mlc` vs `lib/mlc/backends/cpp/`; уточнение `subject_needs_deref` / match + `Shared` по образцу Ruby (`MatchScrutineeDeref`, вызовы вроде `decl_inner`).
- **A.3** Парсер/лексер: граничные случаи, синхронизация с `lib/mlc/source/parser/`.

**Итог:** таблица паритета (фича → тест Ruby → тест MLC) + зафиксированные фикстуры.

### Фаза B — Тесты и CI

- CI (`.github/workflows/ci.yml`): `rake test_unit` + `compiler/tests/build_tests.sh`. Локально быстрее по MLC-only: `rake test_mlc`. Комбо: `rake test_self_hosted_stack`.
- Периодически / отдельным job: `MLCC_BOOTSTRAP=1 compiler/build.sh`.
- Следить, чтобы `test/mlc/mlcc_modules_test.rb` не маскировал падения условным `skip` при `check:` в выводе.

### Фаза C — Сборка без Ruby (долгосрочно)

- Предсобранный проверенный `mlcc` как артефакт.
- Скрипт сборки только из C++ + runtime.
- Доказательство self-bootstrap: `mlcc_N` → те же исходники → `mlcc_N+1` эквивалентен по тестам/хешу.

### Фаза D — Качество

- Диагностика ошибок (файл, строка).
- Профилирование self-hosted на полном `compiler/`.
- Новые фичи языка: сначала Ruby + тесты, затем перенос в `compiler/` + тесты в `compiler/tests`.

### Фаза E — Документация

- Обновлять `README.md`, `CLAUDE.md`, этот файл при смене команд и статуса bootstrap.

---

## 4. Зависимости фаз

```
A (паритет) → B (CI держит паритет)
A + B стабильны → C (опционально без Ruby)
D, E — параллельно с A/B
```

---

## 5. Риски

| Риск | Митигация |
|------|-----------|
| Тихий skip тестов | CI: явная проверка успешного прогона `mlcc` на `compiler/main.mlc` |
| Регрессии `std::visit` / `Shared` | Тесты в `compiler/tests` на match, record update, `decl_inner` |
| Дублирование логики | Общие `.mlc` фикстуры и golden-фрагменты C++ |

---

## 6. Чеклист действий

1. Эталон входа: полный `compiler/` как регрессионный набор.
2. После крупных изменений: `rake test_self_hosted_stack` (или по отдельности `test_mlc`, `test_compiler_mlc`).
3. Периодически: `MLCC_BOOTSTRAP=1 compiler/build.sh`.
4. Таблица паритета фича ↔ тесты.
5. Уточнить `subject_needs_deref` при появлении кейсов.
6. Артефакт bootstrap-бинаря и документация «сборка без Ruby» (фаза C).

---

## 7. Команды (кратко)

```bash
bundle exec rake test_mlc           # только test/mlc/** (CI)
bundle exec rake test_compiler_mlc  # compiler/tests → run_tests
bundle exec rake test_self_hosted_stack  # test_mlc + test_compiler_mlc
bundle exec rake test_unit          # весь test/ кроме integration (может отличаться от CI)
MLCC_BOOTSTRAP=1 compiler/build.sh  # mlcc + g++ → mlcc_bootstrap
```
