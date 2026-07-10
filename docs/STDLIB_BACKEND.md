# Stdlib для серверных backend-приложений — ревью и план (2026-07)

Parent: [PLAN.md](PLAN.md), [FFI_LAYER.md](FFI_LAYER.md), [CONCURRENCY_V2.md](CONCURRENCY_V2.md).
Trigger: анализ пригодности MLC для класса приложений "современный backend"
(REST/WebSocket API, БД, auth, background workers, внешние HTTP-интеграции) —
2026-07-09. Общий обзор пробелов stdlib в одном документе; треки на каждый
компонент создаются по мере старта работы (не все сразу, порядок — §5).

## 1. Что есть сегодня (факт, проверено в коде)

| Компонент | Файл | Состояние |
|-----------|------|-----------|
| HTTP клиент | `runtime/include/mlc/net/http.hpp` | libcurl-обёртка, `fetch`/`fetch_sync`, только исходящие запросы. `fetch()` — псевдо-async (`std::async` + блокирующий `.get()` внутри `Task`, не настоящая интеграция с event loop) |
| JSON | `runtime/include/mlc/json/json.hpp` | `JsonValue` variant-тип, парсинг/сериализация, 253 строки |
| Concurrency | `runtime/include/mlc/concurrency/` | `channel.hpp` (bounded MPMC), `mutex.hpp`, `arc.hpp`, `stop.hpp`, `task_scope.hpp`, `thread_pool.hpp`, `isolate.hpp` — фундамент есть (`CONCURRENCY_V2` closed) |
| HTTP/TCP сервер | `runtime/include/mlc/net/tcp.hpp`, `http_request.hpp`, `http_router.hpp`, `http_server.hpp`; `std/net/tcp.mlc` | **есть** (2026-07-10): blocking TCP + HTTP/1.1 parse/route/`ThreadPool` serve; MLC TCP handles via `Tcp` module. TLS/HTTP/2/WS — out of scope |
| БД-драйверы | — | нет; блокировано `FFI_LAYER` |
| Crypto | — | нет примитивов (hash, HMAC, TLS) |
| WebSocket | — | нет (ни клиент, ни сервер) |
| Validation/schema | — | нет декларативного слоя (только ручные проверки) |
| Типизированная JSON (де)сериализация / OpenAPI | — | см. [API_CLIENT.md](API_CLIENT.md), [TRACK_API_CLIENT](archive/tracks/TRACK_API_CLIENT.md) **closed** 2026-07-09 |
| Structured logging | — | нет (есть `mlc::core::profile`, это профилирование, не логирование) |
| Env/config | — | нет обёртки над `getenv` |

## 2. Категоризация: что нужно для backend-класса приложений

Ориентир — типовой современный backend (REST + WebSocket API, PostgreSQL,
JWT auth, background workers, внешние HTTP-интеграции, structured logging,
env-based config) — категории зависимостей такого приложения, не привязка к
конкретному продукту.

| Категория | Нужно MLC | Приоритет |
|-----------|-----------|-----------|
| HTTP/TCP сервер | listen/accept, HTTP/1.1 parsing, routing | **критический** — без этого backend-класс невозможен вообще |
| БД (Postgres) | FFI-биндинг `libpq`, минимальный query API | **open** — [TRACK_STDLIB_POSTGRES](agent/TRACK_STDLIB_POSTGRES.md) STEP=3 |
| Crypto | hash (SHA-256), HMAC, bcrypt/argon2 для паролей, JWT sign/verify | высокий — auth невозможен без этого |
| WebSocket | upgrade handshake, frame parsing поверх TCP сервера | средний — зависит от TCP сервера |
| Job queue / scheduler | периодические задачи, retry — библиотека поверх `ThreadPool`/`Channel` | средний — примитивы уже есть, нужна только библиотека |
| Env config + logging | `getenv` обёртка, structured log (json lines) | низкий по сложности, высокий по частоте использования |
| Validation | декларативные схемы для входных данных | средний, зависит от философии (см. §3) |

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
  → Job queue / scheduler (чистый MLC, без новых FFI)

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
   **closed** 2026-07-10 (STEP=1–8). Runtime: TCP + HTTP parse/router/`ThreadPool`;
   MLC: `std/net/tcp` echo. Example: `misc/examples/tcp_echo_demo.mlc`.
2. **Postgres driver** — **[TRACK_STDLIB_POSTGRES](agent/TRACK_STDLIB_POSTGRES.md)
   open** STEP=3. Runtime `mlc::db` + smoke done; next `postgres.mlc`.
3. **Crypto** — после `FFI_LAYER` close, биндинг `libsodium` (предпочтительно
   над OpenSSL — проще C API, меньше глобального состояния).
4. **WebSocket** — после (1), поверх TCP-сервера.
5. **Job queue/scheduler** — не зависит от FFI, может идти параллельно
   с (1)-(4) как чистая MLC-библиотека над `ThreadPool`+`Channel`.
6. **Env config + logging** — низкая сложность, можно взять в любой момент
   как "заполнитель" между крупными треками.
7. **Validation** — после определения философии (§3), не начинать раньше
   явного решения compile-time vs runtime schema.

## 6. Место в общей очереди PLAN.md

Ниже `FFI_LAYER` (для Postgres/Crypto-частей), но TCP/HTTP-сервер как
самостоятельный примитив может стартовать раньше — не зависит от FFI_LAYER
STEP=5-6. Приоритет по отношению к `CONCURRENCY_SUPERVISOR`/`MIR_VM_FULL` —
решается отдельно перед стартом первого стдлиб-трека (не фиксируется здесь
жёстко, т.к. нет активного продукта-заказчика, который требует backend
именно сейчас — см. `TEXT_RENDERING.md` для контраста: тот трек имеет явный
триггер "личный проект пользователя").
