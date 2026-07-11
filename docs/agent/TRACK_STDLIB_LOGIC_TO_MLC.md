# Track: Env/Log/Validation — mlcc-пайплайн + минимум C++

Parent: [../FFI_LAYER.md](../FFI_LAYER.md) §8, [../PLAN.md](../PLAN.md).
Trigger: пользователь 2026-07-11 — всё должно собираться через `mlcc` без
Ruby. `Env`/`Log`/`Validation` сегодня протестированы **только через Ruby**
(`bundle exec ruby test/mlc/{env_log,validate}_stdlib_test.rb` — проверено
`grep` по gate-скриптам), нет `mlcc`-пути вообще, в отличие от `Tcp`.

## Status: open — STEP=1 next

## Проблема (проверено)

`scripts/run_env_log_gate.sh`/`run_validate_gate.sh` вызывают только
`bundle exec ruby -Ilib:test test/mlc/*_stdlib_test.rb` — ни один из этих
модулей не резолвится через self-hosted `mlcc` (не в bare-name таблице
`path_normalize.mlc`, по прецеденту `TRACK_PIPELINE_MERGE_TCP_SPAWN` для `Tcp`).

Содержимое C++ по каждому:

| Модуль | `.hpp` | Что внутри |
|--------|--------|------------|
| `env/env.hpp` | 35 строк | `getenv`/`setenv` тонкая обёртка — легитимный FFI к libc, не бизнес-логика |
| `log/log.hpp` | 85 строк | JSON lines writer в stderr — **логика, не биндинг**, использует уже существующий `json.hpp` |
| `validate/validate.hpp` | 47 строк | explicit-стиль валидаторы (не reflection) — **чистая логика**, без внешней зависимости |

`Log`/`Validation` — кандидаты на полный порт в MLC (как HTTP/websocket/msdf).
`Env` — легитимный тонкий FFI (`extern fn getenv/setenv`), остаётся как есть,
но нужен на `mlcc`.

## Цель

Все три модуля резолвятся через `mlcc` (bare-name или explicit import),
`Log`/`Validation` — полностью MLC-логика (не C++ JSON-writer/валидатор),
`Env` — тонкий `extern fn` к `getenv`/`setenv` (не отдельный `.hpp`).

## Decision (STEP=1)

| Вопрос | Вариант | Рекомендация |
|--------|---------|--------------|
| Bare-name резолвинг | Добавить `"Env"`/`"Log"`/`"Validate"` в ту же таблицу `path_normalize.mlc`, что и `Tcp` | да, прямое расширение прецедента |
| `Log` JSON lines | Переиспользовать существующий MLC `Json`-модуль (из `API_CLIENT`/`json.hpp` biндинг уже есть) для сериализации записи лога, `println`/`eprintln` для вывода в stderr | да — не переизобретать JSON, только формат записи (timestamp/level/message/fields) на MLC |
| `Validate` | Явные MLC-функции (`Validate.not_empty(s)`, `Validate.range(n, min, max)` и т.п.) без промежуточного C++ | прямой порт текущих правил 1:1 |
| `Env` | `extern fn getenv(name: RawPointer[Byte]) -> RawPointer[Byte]` напрямую к `<cstdlib>`, без `.hpp` | тонкий FFI, легитимный, не переписывать на MLC (сам `getenv` — не логика) |

## Scope

1. Decision.
2. `Env`: заменить `env.hpp` на `extern fn` в `std/env/env.mlc` напрямую.
3. `Log`: портировать JSON-lines форматирование на MLC поверх `Json`-модуля;
   удалить `log.hpp`.
4. `Validate`: портировать правила на чистый MLC; удалить `validate.hpp`.
5. Bare-name резолвинг для всех трёх в `mlcc`.
6. Regression: `run_env_log_gate.sh`/`run_validate_gate.sh` — переключить на
   `mlcc`-based test (по прецеденту, как ушёл Ruby-only гейт `Tcp` в
   `PIPELINE_MERGE_TCP_SPAWN`), не только Ruby.
7. Self-host diff + `regression_gate.sh`; docs (`STDLIB_BACKEND.md`).

## Out of scope

- `JobQueue` — намеренно C++-only по решению закрытого `TRACK_STDLIB_JOB_QUEUE`
  (это часть concurrency runtime, не FFI-адаптер к сторонней библиотеке —
  см. границу в `FFI_LAYER.md` §8 таблица). Не открывать заново без отдельной
  команды пользователя.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Decision (таблица) + bare-name план. | pending |
| 2 | `Env` → прямой `extern fn`, удалить `env.hpp`. | pending |
| 3 | `Log` → MLC JSON-lines поверх `Json`, удалить `log.hpp`. | pending |
| 4 | `Validate` → чистый MLC, удалить `validate.hpp`. | pending |
| 5 | Bare-name резолвинг в `mlcc` (`path_normalize.mlc`). | pending |
| 6 | Regression gates на `mlcc`; self-host diff; close. | pending |
