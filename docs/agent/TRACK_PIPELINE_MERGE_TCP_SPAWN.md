# Track: один компилятор для `Tcp` + `spawn` (многопоточный HTTP-сервер целиком на MLC)

Parent: [../PLAN.md](../PLAN.md), [../CONCURRENCY_V2.md](../CONCURRENCY_V2.md),
[../archive/tracks/TRACK_CONCURRENCY_RUBY_PARITY.md](../archive/tracks/TRACK_CONCURRENCY_RUBY_PARITY.md)
(Decision C — **Option B "port `common/stdlib` в `mlcc`" отложена туда же**,
этот трек её реализует).
Trigger: пользователь 2026-07-10, **максимальный приоритет** — снять
единственный оставшийся блокер многопоточного HTTP-сервера целиком на MLC
(сейчас STDLIB_NET_SERVER STEP=7 обходит проблему C++-обёрткой
`serve_http_with_thread_pool`, а не языковым `spawn`).

## Status: **open** — STEP=1 next (Decision)

## Проблема (воспроизведено 2026-07-10)

Два фиче-набора реализованы в двух непересекающихся компиляторах:

1. **self-hosted `mlcc`**: `import Tcp::{bind, accept}` →
   `error: file not found: /tmp/.mlc`. Причина —
   `compiler/driver/module_loader.mlc` + `path_normalize.mlc`
   (`resolve_import_path`) резолвят импорт **только** как относительный путь
   к файлу (`dirname(base_path) + import_path + ".mlc"`); никакого
   name→stdlib-path резолвинга нет вообще. `spawn`/`Mutex`/`Channel`/`Task`
   — в checker+codegen `mlcc`, работают.

2. **Ruby bootstrap**: `spawn do ... end` →
   `Pass lower_declarations failed: Unknown identifier 'spawn'`. `rg spawn
   lib/mlc` → 0 совпадений. `Tcp` (`lib/mlc/common/stdlib/net/tcp.mlc`)
   работает.

Итог: ни один компилятор не принимает **оба** набора в одной программе.
Проверено прямым запуском обоих компиляторов на одном репро-файле — не
гипотеза.

## Цель

Одна MLC-программа, один компилятор (**целевой — self-hosted `mlcc`**, это
направление проекта, Ruby — bootstrap/эталон), которая открывает
`TcpListener`, принимает соединения и обрабатывает каждое в `spawn`-таске —
целиком на MLC, без C++ `ThreadPool`-обёртки.

## Decision (STEP=1, нужно зафиксировать)

| Вариант | Объём | Вердикт |
|---------|-------|---------|
| **A** Портировать `Tcp` (+ минимальные транзитивные зависимости) в self-hosted `mlcc`: добавить именованный резолвинг модулей (`import Tcp::{...}` → фиксированный stdlib-путь), стек `extern fn` уже есть (FFI_LAYER closed) | средний | **рекомендован** — self-hosted это целевой компилятор |
| **B** Портировать `spawn`/`Mutex`/`Channel`/`Task` семантику в Ruby checker+codegen | большой, дублирует self-hosted реализацию во втором backend, противоречит направлению на self-hosting | reject по умолчанию, зафиксировать явно на STEP=1 |

Ожидаемое решение: **A**. Если Driver на STEP=1 выберет B — обязательно
явно аргументировать (иначе Planner должен вернуть на пересмотр).

## Scope (Вариант A)

1. Decision — зафиксировать A/B; минимальный stdlib-модуль для демо (`Tcp`
   + то, что он реально импортирует — проверить `lib/mlc/common/stdlib/net/tcp.mlc`
   на транзитивные зависимости).
2. `mlcc`: именованный резолвинг модулей — `import Tcp::{...}` (без `./`,
   без `.mlc`, без `/`) ищется в фиксированном stdlib-корне
   (`lib/mlc/common/stdlib/`) по таблице имя-модуля → путь (или по
   соглашению `module Tcp` внутри файла + сканирование директории один раз
   при старте компиляции). **Не ломать** существующий относительный
   резолвинг (`./foo`, `../bar`) — только добавить отдельную ветку для
   «голых» имён без `/`.
3. Смоук: `Tcp` через `mlcc` в изоляции (`bind`/`accept`/`read`/`write_all`
   в одном файле без `spawn`) — компилируется, линкуется, echo-раунд-трип
   проходит.
4. Совмещение: `import Tcp::{...}` + `spawn do ... end` в одном `.mlc`,
   компилируется `mlcc`; e2e gate — 2+ параллельных клиента, каждый
   обработан в своём `spawn`-таске, оба ответа корректны (не через
   `serve_http_with_thread_pool`, а через язык).
5. Docs: `STDLIB_BACKEND.md`, `MLC.md` § «Два пайплайна» (описан в
   RUBY_PARITY) — обновить, что для `Tcp`+`spawn` разрыв снят; `PLAN.md`.
6. Self-host gate (`mlcc`→`mlcc2`→`diff -rq`), `scripts/regression_gate.sh`.

## Out of scope

- Общий package-manager / module-registry для произвольных зависимостей
  ([TRACK_PACKAGE_MANAGER](TRACK_PACKAGE_MANAGER.md) — design-only, не
  трогать).
- Порт всех модулей `common/stdlib/` — только то, что нужно для `Tcp`.
- Порт `WebSocket` в self-hosted (отдельно, если вообще понадобится после
  этого трека — может стать тривиальным, но не мандат этого трека).
- Замена C++ `serve_http_with_thread_pool` в `STDLIB_NET_SERVER` — тот путь
  остаётся как есть; этот трек добавляет **альтернативный** MLC-путь, не
  обязывает переписывать закрытый трек.
- Вариант B (порт concurrency в Ruby) — если STEP=1 всё же выбирает B,
  переписать это поле, но по умолчанию не в scope.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Decision: A vs B (см. таблицу выше); список минимальных stdlib-модулей для демо. | pending |
| 2 | `mlcc`: именованный резолвинг модулей в `compiler/driver/module_loader.mlc`/`path_normalize.mlc` для голых имён (`Tcp`, не `./tcp`); не ломать относительные импорты (regression 20/0 после). | pending |
| 3 | Смоук: `Tcp` standalone через `mlcc` (без `spawn`) — bind/accept/echo. | pending |
| 4 | Совмещение: `Tcp` + `spawn` в одном файле через `mlcc`; e2e gate 2+ параллельных клиента. | pending |
| 5 | Docs (`STDLIB_BACKEND.md`, `MLC.md`, `PLAN.md`) + пример `misc/examples/`. | pending |
| 6 | Verify-gate: self-host `mlcc`→`mlcc2` diff identical; `regression_gate.sh`; close. | pending |

<!-- sub-steps STEP=2: 1) добавить ветку "bare name" в resolve_import_path; 2) таблица имя→путь под lib/mlc/common/stdlib/ (или сканирование); 3) unit + regression 20/0 -->
<!-- sub-steps STEP=4: 1) mt_server-подобный репро; 2) e2e script с двумя клиентами; 3) assert оба ответа корректны и параллельны (не сериализованы) -->

## Open questions

- Именованный резолвинг — таблица (hardcode `Tcp` → путь) или сканирование
  директории `common/stdlib/` при старте компиляции (ищет `module X` в
  каждом файле)? Решить на STEP=1/2 — сканирование дороже, но не требует
  ручного мейнтенанса таблицы.
