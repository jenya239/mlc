# Track: один компилятор для `Tcp` + `spawn` (многопоточный HTTP-сервер целиком на MLC)

Parent: [../../PLAN.md](../../PLAN.md), [../../CONCURRENCY_V2.md](../../CONCURRENCY_V2.md),
[TRACK_CONCURRENCY_RUBY_PARITY.md](TRACK_CONCURRENCY_RUBY_PARITY.md)
(Decision C — **Option B "port `common/stdlib` в `mlcc`" отложена туда же**,
этот трек её реализует).
Trigger: пользователь 2026-07-10, **максимальный приоритет** — снять
единственный оставшийся блокер многопоточного HTTP-сервера целиком на MLC
(сейчас STDLIB_NET_SERVER STEP=7 обходит проблему C++-обёрткой
`serve_http_with_thread_pool`, а не языковым `spawn`).

## Status: **closed** (2026-07-10) — STEP=1–6 **done**

**Driver 2026-07-10:** STEP=1 — Decision **A** (Tcp into mlcc).
**Driver 2026-07-10:** STEP=2 — bare-name `Tcp` resolve in `path_normalize`.
**Driver 2026-07-10:** STEP=3 — Tcp echo via mlcc (`*_mlc` FFI + gate).
**Driver 2026-07-10:** STEP=4 — Tcp+spawn e2e (`[=]` capture; overlap gate).
**Driver 2026-07-10:** STEP=5 — docs `MLC.md` / `STDLIB_BACKEND.md`.
**Driver 2026-07-10:** STEP=6 — self-host p1≡p2; `regression_gate` 20/0; archive.

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

## Decision (STEP=1, 2026-07-10)

**Chosen: A** — port `Tcp` into self-hosted `mlcc` via named-module resolution.
**B rejected** — duplicates concurrency into Ruby; fights self-host direction
(same as RUBY_PARITY Decision C / deferred Option B).

### Minimal stdlib for demo

| Module | Path | Notes |
|--------|------|-------|
| `Tcp` | `lib/mlc/common/stdlib/net/tcp.mlc` | No MLC imports; only `export extern fn` → `mlc::net::*`. |

No other `common/stdlib` modules required for STEP=3–4 smoke (echo uses only
Tcp; `spawn` is language builtin in mlcc).

### Named resolution (lock for STEP=2)

- Bare import name (no `/`, no `.`, no `./`) → resolve under
  `lib/mlc/common/stdlib/` via a **name→relative path table**.
- v1 table: at least `"Tcp" => "net/tcp.mlc"` (hardcoded or built once from
  known registry). Full directory scan of every `.mlc` for `module X` is
  **allowed later** but not required for this track’s acceptance.
- Relative imports (`./foo`, `../bar`, paths with `/`) unchanged.
- Stdlib root (**STEP=2 rule**): from the importing file’s directory, walk
  parents (max 48) looking for `lib/mlc/common/stdlib/net/tcp.mlc` via
  `File.exists`; first hit wins. Repo-relative paths work when cwd is the
  repo root (e.g. `tmp/probe.mlc` → `lib/mlc/common/stdlib`). No
  `--stdlib-root` / env yet.

### Success criteria (unchanged)

One `.mlc` with `import Tcp::{…}` + `spawn` compiles under **`mlcc`**, links,
runs multi-client echo without `serve_http_with_thread_pool`.

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
| 1 | Decision: A vs B (см. таблицу выше); список минимальных stdlib-модулей для демо. | **done** (2026-07-10: **A**; Tcp only; bare-name table) |
| 2 | `mlcc`: именованный резолвинг модулей в `compiler/driver/module_loader.mlc`/`path_normalize.mlc` для голых имён (`Tcp`, не `./tcp`); не ломать относительные импорты (regression 20/0 после). | **done** (2026-07-10: bare `Tcp`→`net/tcp.mlc`; parent walk; unit+check-only; regression 20/0) |
| 3 | Смоук: `Tcp` standalone через `mlcc` (без `spawn`) — bind/accept/echo. | **done** (2026-07-10: FFI `*_mlc` + `tcp_echo_mlcc.mlc`; `run_mlcc_tcp_echo_gate.sh` OK) |
| 4 | Совмещение: `Tcp` + `spawn` в одном файле через `mlcc`; e2e gate 2+ параллельных клиента. | **done** (2026-07-10: `[=]` spawn capture; `tcp_spawn_echo_mlcc.mlc`; gate overlap OK) |
| 5 | Docs (`STDLIB_BACKEND.md`, `MLC.md`, `PLAN.md`) + пример `misc/examples/`. | **done** (2026-07-10: MLC §pipelines + Tcp; STDLIB_BACKEND §1/§5; examples linked) |
| 6 | Verify-gate: self-host `mlcc`→`mlcc2` diff identical; `regression_gate.sh`; close. | **done** (2026-07-10: p1≡p2; regression 20/0; archived) |

<!-- sub-steps STEP=2: 1) добавить ветку "bare name" в resolve_import_path; 2) таблица имя→путь под lib/mlc/common/stdlib/ (или сканирование); 3) unit + regression 20/0 -->
<!-- sub-steps STEP=4: 1) mt_server-подобный репро; 2) e2e script с двумя клиентами; 3) assert оба ответа корректны и параллельны (не сериализованы) -->

## Open questions

- Full stdlib directory scan vs hardcoded `Tcp` table — **resolved for v1**:
  hardcoded `"Tcp" => "net/tcp.mlc"`; directory scan deferred.
