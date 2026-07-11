# Track: Env/Log/Validation — mlcc-пайплайн + минимум C++

Parent: [../FFI_LAYER.md](../FFI_LAYER.md) §8, [../PLAN.md](../PLAN.md).
Trigger: пользователь 2026-07-11 — всё должно собираться через `mlcc` без
Ruby. `Env`/`Log`/`Validation` сегодня протестированы **только через Ruby**
(`bundle exec ruby test/mlc/{env_log,validate}_stdlib_test.rb` — проверено
`grep` по gate-скриптам), нет `mlcc`-пути вообще, в отличие от `Tcp`.

## Status: **active** — STEP=2 next (Env thin extern)

**Driver 2026-07-11:** STEP=1 — Decision **locked**. Verified APIs:
`env.hpp` get/get_or/has; `log.hpp` JSON-line error/warn/info/debug (manual
escape, not json.hpp); `validate.hpp` four Result checks. Bare names
Env/Log/Validate → path_normalize (STEP=5).

**Planner 2026-07-11:** activated after TEXT_MSDF Critic OK (`f29b39ec`).
Next «без C++» queue item (§20d before §20e). Verified on disk:
`env.hpp` 35 / `log.hpp` 85 / `validate.hpp` 47; gates Ruby-only;
`path_normalize` has Tcp/HttpServer only (no Env/Log/Validate).

## Проблема (проверено)

`scripts/run_env_log_gate.sh`/`run_validate_gate.sh` вызывают только
`bundle exec ruby -Ilib:test test/mlc/*_stdlib_test.rb` — ни один из этих
модулей не резолвится через self-hosted `mlcc` (не в bare-name таблице
`path_normalize.mlc`, по прецеденту `TRACK_PIPELINE_MERGE_TCP_SPAWN` для `Tcp`).

Содержимое C++ по каждому:

| Модуль | `.hpp` | Что внутри |
|--------|--------|------------|
| `env/env.hpp` | 35 строк | `getenv`/`setenv` тонкая обёртка — легитимный FFI к libc, не бизнес-логика |
| `log/log.hpp` | 85 строк | JSON lines writer в stderr — **логика, не биндинг**, ручной escape (не `json.hpp`) |
| `validate/validate.hpp` | 47 строк | explicit-стиль валидаторы (не reflection) — **чистая логика**, без внешней зависимости |

`Log`/`Validation` — кандидаты на полный порт в MLC (как HTTP/websocket/msdf).
`Env` — легитимный тонкий FFI (`extern fn getenv/setenv`), остаётся как есть,
но нужен на `mlcc`.

## Цель

Все три модуля резолвятся через `mlcc` (bare-name или explicit import),
`Log`/`Validation` — полностью MLC-логика (не C++ JSON-writer/валидатор),
`Env` — тонкий `extern fn` к `getenv`/`setenv` (не отдельный `.hpp`).

## Decision (STEP=1, 2026-07-11) — **locked**

Verified by reading `env.hpp` / `log.hpp` / `validate.hpp` and
`lib/mlc/common/stdlib/{env,log,validate}/*.mlc`.

### Locked choices

| Вопрос | Locked |
|--------|--------|
| Public Env API | Keep `get` / `get_or` / `has` on `string` (not RawPointer surface) |
| Env impl | Thin FFI over `std::getenv` (tiny `env_abi` or `from "<cstdlib>"` + MLC Option wrap); **delete** `env.hpp` |
| Public Log API | Keep `error` / `warn` / `info` / `debug(message: string)`; wire format `{"level":"<level>","msg":"<escaped>"}\n` on stderr |
| Log impl | Pure MLC: JSON-escape + format + `eprintln` (or fwrite stderr extern if needed); **delete** `log.hpp`. Prefer MLC escape 1:1 with current `append_json_escaped`; Json module optional if it already escapes strings identically |
| Public Validate API | Keep `non_empty` / `min_length` / `max_length` / `range_i32` → `Result<(), string>` messages unchanged |
| Validate impl | Pure MLC bodies; **delete** `validate.hpp` |
| Bare names (STEP=5) | `"Env"` → `env/env.mlc`, `"Log"` → `log/log.mlc`, `"Validate"` → `validate/validate.mlc` in `path_normalize.mlc` (same table as Tcp/HttpServer) |
| Gates (STEP=6) | `run_env_log_gate.sh` / `run_validate_gate.sh` must exercise **mlcc** path (not Ruby-only) |
| Rejected | Keeping business logic in `.hpp`; RawPointer-only Env public API; expanding Log with timestamp/fields in this track; JobQueue |

### STEP=2–4 delete targets

| File | Step |
|------|------|
| `runtime/include/mlc/env/env.hpp` | 2 |
| `runtime/include/mlc/log/log.hpp` | 3 |
| `runtime/include/mlc/validate/validate.hpp` | 4 |

## Scope

1. Decision.
2. `Env`: заменить `env.hpp` на thin extern + MLC wrappers.
3. `Log`: MLC JSON-lines (escape+format); удалить `log.hpp`.
4. `Validate`: чистый MLC; удалить `validate.hpp`.
5. Bare-name резолвинг для всех трёх в `mlcc`.
6. Regression gates на `mlcc`; self-host; docs (`STDLIB_BACKEND.md`).

## Out of scope

- `JobQueue` — намеренно C++-only по решению закрытого `TRACK_STDLIB_JOB_QUEUE`
  (это часть concurrency runtime, не FFI-адаптер к сторонней библиотеке —
  см. границу в `FFI_LAYER.md` §8 таблица). Не открывать заново без отдельной
  команды пользователя.
- Расширение Log schema (timestamp/fields) — текущий wire format only.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Decision (таблица) + bare-name план. | **done** (2026-07-11: locked APIs + delete targets) |
| 2 | `Env` → прямой `extern fn`, удалить `env.hpp`. | pending |
| 3 | `Log` → MLC JSON-lines поверх escape/`eprintln`, удалить `log.hpp`. | pending |
| 4 | `Validate` → чистый MLC, удалить `validate.hpp`. | pending |
| 5 | Bare-name резолвинг в `mlcc` (`path_normalize.mlc`). | pending |
| 6 | Regression gates на `mlcc`; self-host diff; close. | pending |

<!-- STEP=1 sub-steps: 1) lock Decision (bare names, Json for Log, Env libc extern); 2) cite hpp sizes + gate Ruby-only; 3) list STEP=2–4 delete targets; PLAN→STEP=2 -->
<!-- STEP=2 sub-steps: 1) env.mlc → extern getenv/setenv; 2) delete env.hpp; 3) smoke via mlcc -->
<!-- STEP=3 sub-steps: 1) port log format to MLC+escape; 2) delete log.hpp; 3) gate stderr JSON lines -->
<!-- STEP=4 sub-steps: 1) port validate rules to MLC; 2) delete validate.hpp; 3) gate valid/invalid -->
<!-- STEP=5 sub-steps: 1) path_normalize Env/Log/Validate; 2) bare import smoke; 3) test_driver assert if any -->
<!-- STEP=6 sub-steps: 1) switch env_log/validate gates to mlcc; 2) regression_gate + self-host; 3) close → Critic -->
