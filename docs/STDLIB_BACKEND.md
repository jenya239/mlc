# Stdlib для серверных backend-приложений — ревью и план (2026-07)

Parent: [PLAN.md](PLAN.md), [FFI_LAYER.md](FFI_LAYER.md) §8, [CONCURRENCY_V2.md](CONCURRENCY_V2.md).

**2026-07-11:** таблица §1 ниже обновлена под инициативу
«без hand-written C++» ([FFI_LAYER.md](FFI_LAYER.md) §8) — HTTP server +
WebSocket protocol на MLC; thin bridges only
([TRACK_STDLIB_WEBSOCKET_TO_MLC](archive/tracks/TRACK_STDLIB_WEBSOCKET_TO_MLC.md) **closed**).
Trigger: анализ пригодности MLC для класса приложений "современный backend"
(REST/WebSocket API, БД, auth, background workers, внешние HTTP-интеграции) —
2026-07-09. Общий обзор пробелов stdlib в одном документе; треки на каждый
компонент создаются по мере старта работы (не все сразу, порядок — §5).

## 1. Что есть сегодня (факт, проверено в коде)

| Компонент | Файл | Состояние |
|-----------|------|-----------|
| HTTP клиент | `runtime/include/mlc/net/http.hpp` | libcurl-обёртка, `fetch`/`fetch_sync`, только исходящие запросы. `fetch()` — псевдо-async (`std::async` + блокирующий `.get()` внутри `Task`, не настоящая интеграция с event loop) |
| JSON | `runtime/include/mlc/json/json.hpp` | `JsonValue` variant-тип, парсинг/сериализация, 253 строки |
| Concurrency | `runtime/include/mlc/concurrency/` | `channel.hpp`, `mutex.hpp`, `arc.hpp`, `stop.hpp`, `task_scope.hpp`, `thread_pool.hpp`, `job_queue.hpp`, `isolate.hpp` — фундамент есть (`CONCURRENCY_V2` closed); JobQueue **closed** ([TRACK_STDLIB_JOB_QUEUE](archive/tracks/TRACK_STDLIB_JOB_QUEUE.md)) |
| TCP сервер | `runtime/include/mlc/net/tcp.hpp`; `std/net/tcp.mlc` | **есть, MLC-reachable**: blocking TCP; `Tcp` в Ruby **и** mlcc (`import … from 'Tcp'`); `Tcp`+`spawn` под mlcc — `misc/examples/tcp_spawn_echo_mlcc.mlc` ([TRACK_PIPELINE_MERGE_TCP_SPAWN](archive/tracks/TRACK_PIPELINE_MERGE_TCP_SPAWN.md) **closed**) |
| HTTP-парсер/роутер (server) | `lib/mlc/common/stdlib/net/http_server.mlc` (`HttpServer`) | **есть, MLC-reachable** (2026-07-11): pure-MLC parse/format; bare `import … from 'HttpServer'`; demo `misc/examples/http_server_curl_demo.mlc`; gates `run_http_server_parse_smoke.sh` / `run_http_server_curl_gate.sh` ([TRACK_STDLIB_HTTP_MLC](archive/tracks/TRACK_STDLIB_HTTP_MLC.md) **closed**, Critic OK). Public C++ `http_*.hpp` **удалены**; WS upgrade всё ещё использует private `websocket_http.hpp`. Residual: no `[HttpRoute]` table API (demo if/else). TLS/HTTP/2 — out of scope |
| БД-драйверы | `runtime/include/mlc/db/postgres.hpp`; `std/db/postgres.mlc` | **есть** (2026-07-10, PoC): libpq handle-based connect/exec/select; только Postgres, только blocking |
| Crypto | `runtime/include/mlc/crypto/sodium.hpp`; `std/crypto/crypto.mlc` | **есть** (2026-07-10): SHA-256/HMAC/random/`pwhash`; JWT/TLS — out |
| WebSocket | `std/net/websocket.mlc`; thin `websocket_bridge.hpp` | **есть, MLC** (2026-07-11): upgrade/frames/handshake in MLC; gate `run_websocket_gate.sh` (MLC echo + Ruby client); [TRACK_STDLIB_WEBSOCKET_TO_MLC](archive/tracks/TRACK_STDLIB_WEBSOCKET_TO_MLC.md) **closed**. Residual: Ruby `:extern` stubs in bridge |

| Job queue | `runtime/include/mlc/concurrency/job_queue.hpp` | **есть** (2026-07-11): C++ `JobQueue` on `ThreadPool`; gate `run_job_queue_gate.sh`; example `job_queue_demo.cpp`; no MLC module ([TRACK_STDLIB_JOB_QUEUE](archive/tracks/TRACK_STDLIB_JOB_QUEUE.md) **closed**) |
| Validation/schema | `runtime/include/mlc/validate/validate.hpp`; `std/validate/validate.mlc` | **есть** (2026-07-11): explicit helpers; MLC `Validate`; gate `run_validate_gate.sh` ([TRACK_STDLIB_VALIDATION](archive/tracks/TRACK_STDLIB_VALIDATION.md) **closed**) |
| Типизированная JSON (де)сериализация / OpenAPI | — | см. [API_CLIENT.md](API_CLIENT.md), [TRACK_API_CLIENT](archive/tracks/TRACK_API_CLIENT.md) **closed** 2026-07-09 |
| Structured logging | `std/log/logger.mlc` + thin `log_abi.hpp` (fwrite only; `log.hpp` deleted) | **есть** (2026-07-11 STEP=3): JSON escape/format in MLC; mlcc `log_mlc_smoke.mlc` |
| Env/config | `runtime/include/mlc/env/env_abi.hpp`; `std/env/env.mlc` | **есть** (2026-07-11 STEP=2): thin getenv `get`/`get_or`/`has`; MLC `Env`; `env.hpp` deleted |

## 2. Категоризация: что нужно для backend-класса приложений

Ориентир — типовой современный backend (REST + WebSocket API, PostgreSQL,
JWT auth, background workers, внешние HTTP-интеграции, structured logging,
env-based config) — категории зависимостей такого приложения, не привязка к
конкретному продукту.

| Категория | Нужно MLC | Приоритет |
|-----------|-----------|-----------|
| HTTP/TCP сервер | listen/accept, HTTP/1.1 parsing, routing | TCP: **closed** MLC-reachable ([TRACK_STDLIB_NET_SERVER](archive/tracks/TRACK_STDLIB_NET_SERVER.md)/[TRACK_PIPELINE_MERGE_TCP_SPAWN](archive/tracks/TRACK_PIPELINE_MERGE_TCP_SPAWN.md)). HTTP parse/route: **MLC `HttpServer`** — [TRACK_STDLIB_HTTP_MLC](archive/tracks/TRACK_STDLIB_HTTP_MLC.md) **closed** |
| БД (Postgres) | FFI-биндинг `libpq`, минимальный query API | **closed** — [TRACK_STDLIB_POSTGRES](archive/tracks/TRACK_STDLIB_POSTGRES.md) |
| Crypto | hash (SHA-256), HMAC, bcrypt/argon2 для паролей, JWT sign/verify | **closed** — [TRACK_STDLIB_CRYPTO](archive/tracks/TRACK_STDLIB_CRYPTO.md) (JWT follow-up) |
| WebSocket | upgrade handshake, frame parsing поверх TCP сервера | **closed** — [TRACK_STDLIB_WEBSOCKET](archive/tracks/TRACK_STDLIB_WEBSOCKET.md) |
| Job queue / scheduler | периодические задачи, retry — библиотека поверх `ThreadPool`/`Channel` | **closed** — [TRACK_STDLIB_JOB_QUEUE](archive/tracks/TRACK_STDLIB_JOB_QUEUE.md) |
| Env config + logging | `getenv` обёртка, structured log (json lines) | **closed** — [TRACK_STDLIB_ENV_LOGGING](archive/tracks/TRACK_STDLIB_ENV_LOGGING.md) |
| Validation | декларативные схемы для входных данных | **closed** — [TRACK_STDLIB_VALIDATION](archive/tracks/TRACK_STDLIB_VALIDATION.md) |

## 3. Философия: что не копировать из динамических экосистем

MLC — статический, compile-time checked, без рефлексии в рантайме. Прямой
перенос паттернов из Node.js/Ruby-класса фреймворков противоречит этой базе:

- **Reflection-based DI контейнеры** (автоматическое связывание по типам в
  рантайме) — MLC должен использовать явную compile-time сборку зависимостей
  (конструкторы/фабрики, без магии).
- **Dynamic ORM query builder** (строим SQL через цепочки методов в рантайме)
  — предпочтительно compile-time проверяемый SQL (аналог Rust `sqlx`:
  запрос известен на этапе компиляции, типы результата выводятся статически).
- **Декораторы для роутинга/middleware** (`@Get('/users')`-стиль) — MLC не
  имеет декораторов и не должен имитировать рефлексию через них; explicit
  таблица роутов или codegen из декларативной структуры.
- **Hot-reload dev-server** — противоречит модели компилируемого языка;
  компенсируется быстрой инкрементальной компиляцией (см. `TRACK_BUILD_SPEED3`).

## 4. Зависимости между компонентами

```
FFI_LAYER (STEP=1-6)
  → Postgres driver (libpq биндинг)
  → Crypto (libsodium/openssl биндинг)

CONCURRENCY_V2 (closed) + ThreadPool/Channel (есть)
  → Job queue / scheduler (C++ `JobQueue` on ThreadPool; MLC module deferred)

TCP/HTTP сервер (новый примитив, не зависит от FFI_LAYER — сокеты через
  syscalls напрямую или через существующий extern-механизм)
  → WebSocket (upgrade поверх HTTP сервера)
  → Routing/middleware слой
```

**TCP/HTTP сервер не блокирован `FFI_LAYER`** — BSD sockets (`socket`,
`bind`, `listen`, `accept`, `read`, `write`) — это уже покрываемый `extern
fn`-паттерн (POSIX C ABI, без callback/struct-сложностей уровня OpenGL),
может стартовать сразу после `FFI_LAYER` STEP=3 (`extern fn` codegen) без
ожидания STEP=5-6 (`extern type`/C function pointer не обязательны для
базового sockets API — `int` file descriptor, не opaque handle).

## 5. Порядок реализации (треки создаются по мере старта, не все сразу)

1. **TCP/HTTP-сервер** — [TRACK_STDLIB_NET_SERVER](archive/tracks/TRACK_STDLIB_NET_SERVER.md)
   **closed** 2026-07-10 (STEP=1–8). Runtime: TCP; MLC: `std/net/tcp` echo.
   Example: `misc/examples/tcp_echo_demo.mlc` (Ruby).
   **mlcc merge** — [TRACK_PIPELINE_MERGE_TCP_SPAWN](archive/tracks/TRACK_PIPELINE_MERGE_TCP_SPAWN.md)
   **closed** 2026-07-10 (Decision A): bare `Tcp` + `spawn` e2e; examples `tcp_echo_mlcc.mlc` /
   `tcp_spawn_echo_mlcc.mlc`; gates `run_mlcc_tcp_echo_gate.sh` /
   `run_mlcc_tcp_spawn_echo_gate.sh`.
   **HTTP parse/route on MLC** — [TRACK_STDLIB_HTTP_MLC](archive/tracks/TRACK_STDLIB_HTTP_MLC.md)
   **closed** (2026-07-11): `HttpServer` module + curl demo; public C++ HTTP headers removed.
2. **Postgres driver** — [TRACK_STDLIB_POSTGRES](archive/tracks/TRACK_STDLIB_POSTGRES.md)
   **closed** 2026-07-10. Runtime `mlc::db`; MLC `std/db/postgres`; gate
   `scripts/run_postgres_gate.sh`; example `misc/examples/postgres_select_demo.mlc`.
3. **Crypto** — [TRACK_STDLIB_CRYPTO](archive/tracks/TRACK_STDLIB_CRYPTO.md)
   **closed** 2026-07-10. Runtime `mlc::crypto`; MLC `std/crypto/crypto`; gate
   `scripts/run_crypto_gate.sh`; example `misc/examples/crypto_sha256_demo.mlc`.
   JWT — follow-up TRACK.
4. **WebSocket** — [TRACK_STDLIB_WEBSOCKET](archive/tracks/TRACK_STDLIB_WEBSOCKET.md)
   **closed** 2026-07-11. Runtime + `std/net/websocket`; gate
   `scripts/run_websocket_gate.sh`; example `misc/examples/websocket_echo_demo.mlc`.
   WSS/client — out of scope.
5. **Job queue/scheduler** — [TRACK_STDLIB_JOB_QUEUE](archive/tracks/TRACK_STDLIB_JOB_QUEUE.md)
   **closed** 2026-07-11. C++ `job_queue.hpp`; gate `run_job_queue_gate.sh`;
   example `misc/examples/job_queue_demo.cpp`. MLC module — out of scope v1.
6. **Env config + logging** — [TRACK_STDLIB_ENV_LOGGING](archive/tracks/TRACK_STDLIB_ENV_LOGGING.md)
   **closed** 2026-07-11. Runtime `env.hpp`/`log.hpp`; MLC `Env`/`Log`; gate
   `run_env_log_gate.sh`; example `misc/examples/env_log_demo.mlc`. No OTel/dotenv.
7. **Validation** — [TRACK_STDLIB_VALIDATION](archive/tracks/TRACK_STDLIB_VALIDATION.md)
   **closed** 2026-07-11. Runtime `validate.hpp`; MLC `Validate`; gate
   `run_validate_gate.sh`; example `misc/examples/validate_demo.mlc`. No derive/JSON Schema.

## 6. Место в общей очереди PLAN.md

Ниже `FFI_LAYER` (для Postgres/Crypto-частей), но TCP/HTTP-сервер как
самостоятельный примитив может стартовать раньше — не зависит от FFI_LAYER
STEP=5-6. Приоритет по отношению к `CONCURRENCY_SUPERVISOR`/`MIR_VM_FULL` —
решается отдельно перед стартом первого стдлиб-трека (не фиксируется здесь
жёстко, т.к. нет активного продукта-заказчика, который требует backend
именно сейчас — см. `TEXT_RENDERING.md` для контраста: тот трек имеет явный
триггер "личный проект пользователя").
