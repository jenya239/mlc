# Self-hosted compiler: политика и текущий статус

Дата: март 2026.  
Детальный roadmap: `docs/ROADMAP.md`.

---

## Политика Ruby: резерв и bootstrap

- `lib/mlc/` не удалять — полнофункциональный компилятор, эталон семантики и codegen, резерв при регрессиях.
- Сборка `mlcc` по умолчанию — через `compiler/build.sh` (Ruby `ModularCompiler`).
- Любое расхождение Ruby vs `compiler/out/mlcc` — баг self-hosted до доказательства обратного.
- Новая фича языка: Ruby-реализация + Ruby-тест первичны; затем перенос в `compiler/` + MLC-тест.

---

## Текущий статус (март 2026)

| Задача | Статус |
|--------|--------|
| `compiler/` компилирует весь `compiler/` | ✓ |
| 85 unit-тестов (`compiler/tests/`) проходят | ✓ |
| 4 E2E программы (`compiler/tests/e2e/`) проходят через mlcc | ✓ |
| Ruby unit-тесты: 1106 runs, 0 failures | ✓ |
| `Result<T,E>` + `?` оператор | ✓ |
| Triple-bootstrap (mlcc3 == mlcc2) | не проверялось |
| Generics с bounds, trait dispatch в `compiler/` | нет |
| Диагностики с позицией (файл:строка:колонка) | нет |
| CI конфиг | нет |

---

## Компоненты

| Компонент | Путь |
|-----------|------|
| Bootstrap (полный стек) | `lib/mlc/` |
| Self-hosted | `compiler/` (lexer, parser, checker, codegen) |
| Runtime | `runtime/include/`, `runtime/src/` |
| Сборка mlcc | `compiler/build.sh` |
| Bootstrap-бинарь | `MLCC_BOOTSTRAP=1 compiler/build.sh` → `compiler/out/bootstrap/mlcc_bootstrap` |
| Тесты Ruby | `test/mlc/**/*_test.rb`, `rake test_mlc` |
| Unit-тесты MLC | `compiler/tests/`, `rake test_compiler_mlc` |
| E2E тесты mlcc | `compiler/tests/e2e/run_e2e.sh` |
| Self-hosted тест-runner | `compiler/tests/build_tests_self.sh [mlcc_binary]` |

---

## Команды

```bash
# Тесты
bundle exec rake test_mlc                               # Ruby unit-тесты (быстро)
bundle exec rake test_compiler_mlc                      # MLC unit-тесты через build_tests.sh
bundle exec rake test_self_hosted_stack                 # оба вместе

compiler/tests/e2e/run_e2e.sh [mlcc_binary]            # E2E тесты
compiler/tests/build_tests_self.sh [mlcc_binary]        # unit-тесты через self-hosted mlcc

# Сборка
compiler/build.sh                                       # Ruby → mlcc
MLCC_BOOTSTRAP=1 compiler/build.sh                     # Ruby → mlcc → mlcc_bootstrap (g++)
```
