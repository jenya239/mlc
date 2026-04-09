# Чек-листы разработки MLC

Дата: апрель 2026. Цель: **надёжность** и **устойчивая архитектура** self-hosted (`compiler/`). Команды — из корня реп `mlc/` unless noted.

---

## 1. Минимум перед merge (self-hosted затронут)

- [ ] `bundle exec rake test_mlc` — зелёный (эталон семантики Ruby).
- [ ] `bundle exec rake test_compiler_mlc` — зелёный (MLC unit через `build_tests.sh`).
- [ ] При изменении **Ruby-пайплайна** (`lib/mlc/`, особенно SemanticGen / `header_generator`): убедиться, что не сломана модульная сборка (`compiler/build.sh` хотя бы один полный прогон после правок).
- [ ] При изменении **хедеров / межмодульных вызовов**: собрать `compiler/out/*.cpp` без ошибок g++ (`compiler/build.sh`). В **GitHub Actions** (`ci.yml`) после unit-тестов выполняются `compiler/tests/build_tests.sh` и полная сборка `compiler/build.sh`.
- [ ] **Нет незакоммиченного мусора** в `out/` в корне и случайных артефактов (только то, что принято в политике репозитория).

---

## 2. Расширенная проверка стабильности компилятора

- [ ] `compiler/triple_bootstrap.sh [path/to/mlcc]` — **bs2 и bs3 совпадают** (`diff` пустой). Доказательство, что второй и третий self-host не расходятся.
- [ ] Либо `bundle exec rake triple_bootstrap` — сначала `compiler/build.sh`, затем тот же сценарий (долго).
- [ ] Либо GitHub Actions: workflow **Triple bootstrap** (`triple-bootstrap.yml`), только ручной запуск (`workflow_dispatch`).
- [ ] Опционально: `MLC_TRIPLE_BOOTSTRAP=1 bundle exec rake test_self_hosted_stack` — Ruby + MLC тесты, затем triple-bootstrap.

---

## 3. E2E и интеграция (выборочно)

- [ ] `compiler/tests/e2e/run_e2e.sh [mlcc]` — четыре программы через mlcc.
- [ ] `compiler/tests/build_tests_self.sh [mlcc]` — unit MLC через self-hosted бинарь.
- [ ] `bundle exec rake test_integration` / `test_e2e` — если менялся Ruby-бэкенд и нужна полная картина.

---

## 4. Архитектура `compiler/` (инварианты)

- [ ] **Граф импортов `.mlc` без циклов** — `ModuleGraph` при сборке; новый модуль не должен создавать `infer.mlc` ↔ `X.mlc` ↔ `infer.mlc`.
- [ ] **Checker:** граница — AST/transform/registry/infer/codegen consumer; не смешивать вывод типов с генерацией C++.
- [ ] **Codegen:** на входе трансформированная программа (`SProgram` / `SExpr` и т.д. из `checker/transform`); не тащить checker-логику обратно в eval.
- [ ] **Ruby SemanticGen — области видимости:** во вложенных `do`/`match` именованный `import { f }` может быть невидим; паттерн **`import * as alias`** и **`alias.f(...)`** (см. `docs/SELF_HOSTED_PLAN.md`). Несколько `import { … } from` из разных файлов в одном модуле иногда ломают разрешение имён в телах — после сплита гонять `compiler/build.sh`.
- [ ] **C++ генерация:** параметры вида `context` могут затенять namespace `context` в `.cpp`; межмодульные вызовы в таких функциях — через обёртки в том же модуле или **абсолютная квалификация** `::context::...` (если доработан backend).

---

## 5. Качество изменений (refactor / новые модули)

- [ ] Идентификаторы — **полные слова** (правило репозитория, см. `.cursor/rules/no-abbreviations.mdc`).
- [ ] Новый `.mlc` в `compiler/checker` или `compiler/codegen`: явно перечислить **экспорт** (`export fn` / типы), кто импортирует, почему **нет цикла**.
- [ ] После вынесения кода из `eval.mlc`: краткая заметка в коммите или в этом файле (раздел «Журнал крупных сплитов») — необязательно, но снижает регрессии.
- [ ] **Модульные хедеры:** неэкспортируемые функции, вызываемые из другого `.mlc`, должны иметь прототип в `.hpp` (политика `header_generator.rb`, сейчас узкие правила вроде `InferResult_*` — не ломать lexer private types).

---

## 6. Документация и согласованность

- [ ] Пути: **codegen — каталог** `compiler/codegen/`, не «один `codegen.mlc`» (см. `README.md`, `CLAUDE.md`, `docs/ROADMAP.md`).
- [ ] Число MLC-тестов в таблицах — периодически синхронять с фактическим выводом `rake test_compiler_mlc` (сейчас ориентир: 192).
- [ ] Черновики в `docs/*.md` вне git: не считать источником истины до мержа.

---

## 7. Перед релизом / крупным тегом (если появится)

- [ ] Все пункты разделов 1 и 2.
- [ ] `MLCC_BOOTSTRAP=1 compiler/build.sh` — опционально, см. `docs/SELF_HOSTED_PLAN.md`.
- [ ] Зафиксировать версию/ревизию и хеш коммита в заметке к релизу.

---

## Быстрая шпаргалка команд

```bash
bundle exec rake test_mlc
bundle exec rake test_compiler_mlc
compiler/build.sh
compiler/triple_bootstrap.sh compiler/out/mlcc
MLC_TRIPLE_BOOTSTRAP=1 bundle exec rake test_self_hosted_stack
compiler/tests/e2e/run_e2e.sh compiler/out/mlcc
```
