# Track: HTTP-сервер (парсер+роутер) — на MLC, не C++-обёртка

Parent: [../PLAN.md](../PLAN.md), [../STDLIB_BACKEND.md](../STDLIB_BACKEND.md) §1/§5,
[../FFI_LAYER.md](../FFI_LAYER.md) §8,
[../archive/tracks/TRACK_STDLIB_NET_SERVER.md](../archive/tracks/TRACK_STDLIB_NET_SERVER.md),
[../archive/tracks/TRACK_PIPELINE_MERGE_TCP_SPAWN.md](../archive/tracks/TRACK_PIPELINE_MERGE_TCP_SPAWN.md).
Trigger: пользователь 2026-07-11 — по итогам ревью выяснилось, что HTTP-слой
(парсер запроса, роутер, `serve_http_with_thread_pool`) реализован и
протестирован **только в C++**; из MLC он не вызывается вообще, даже после
того как `Tcp`+`spawn` стали работать вместе в `mlcc`.

**Пересмотр стратегии 2026-07-11** (тот же день, после расширенного запроса
пользователя «без C++ вообще, либо автогенерация, либо прямой импорт
заголовков»): `http_request.hpp`/`http_router.hpp` — **не биндинг к внешней
библиотеке** (`grep #include` — только STL, ни одного стороннего заголовка).
Это HTTP/1.1-парсер и роутинг, написанные на C++ вместо MLC. Прямой
`extern fn`-обёртки здесь не решает задачу пользователя — оборачивать
C++-логику в `extern fn` не убирает C++, только даёт MLC доступ к нему.
**Правильное решение — портировать парсер/роутер на чистый MLC**, оперируя
байтами из `Tcp.read` напрямую (как HTTP/1.1 парсеры делаются в Go/Node/Ruby
на уровне пользовательского кода, не через C-биндинг). Ниже Decision/Scope
переписаны под этот подход, старая версия (`extern fn` над `.hpp`) отменена.

## Status: **closed** (2026-07-11) — STEP=1–7 done

**Driver 2026-07-11:** STEP=7 — verify-gate: self-host `mlcc`→`mlcc2`
`DIFF_EXIT=0`; `regression_gate` 20/0; parse+curl gates EXIT 0. Track closed.
Residuals for Critic: `websocket_http.hpp`; demo local if/else (no
`HttpRoute` array); Ruby `registry.rb` bare-name.

**Driver 2026-07-11:** STEP=6 — `STDLIB_BACKEND.md` §1/§2/§5, `MLC.md`
Tcp/HTTP + bare table, `FFI_LAYER.md` §8 row; examples already in place.

**Driver 2026-07-11:** STEP=5 — deleted public C++ `http_request.hpp`/
`http_router.hpp`/`http_server.hpp` + `test_http_*.cpp`; moved parse/
`read_http_message` to `websocket_http.hpp` (WS handshake only until
`TRACK_STDLIB_WEBSOCKET_TO_MLC`); `mlc.hpp` drops HTTP includes;
`serve_http_with_thread_pool` gone (was only used by deleted test).

**Driver 2026-07-11:** STEP=4 — `http_server_curl_demo.mlc` (Tcp+spawn+route
`GET /`/`/health`); `run_http_server_curl_gate.sh` curl ok.

**Driver 2026-07-11:** STEP=3 — `path_normalize`: bare `"HttpServer"` →
`net/http_server.mlc`; `test_driver` assert; smoke imports `'HttpServer'`;
rebuilt `mlcc`.

**Driver 2026-07-11:** STEP=2 — `http_server.mlc`: records + `parse_http_request`/
`format_http_response`; `HttpParseResult` sum; smoke
`http_server_parse_smoke.mlc` + `run_http_server_parse_smoke.sh` (native link).

**Driver 2026-07-11:** STEP=1 — Decision **locked** (pure-MLC `HttpServer`
module; records mirror C++ shapes; byte-scan parse; array router; one module).

**Planner 2026-07-11:** activated after VM_TYPED_COLLECTIONS Critic OK
(`8dfa2e42`). First in «без hand-written C++» HTTP path; blocks
`TRACK_STDLIB_WEBSOCKET_TO_MLC`. Prefer over FFI_SHIM/MSDF/LOGIC/GLAD until
STEP=1 Decision locked.

## Проблема (проверено 2026-07-11)

`STDLIB_BACKEND.md` §1 утверждал «HTTP/TCP сервер — **есть**» — это верно
только для C++: `runtime/include/mlc/net/http_request.hpp`/`http_router.hpp`/
`http_server.hpp` покрыты C++-тестами (`runtime/test/test_http_*.cpp`), но:

```bash
grep -rln "http_request\|http_router\|HttpRequest\|HttpRouter\|serve_http" \
  lib/mlc/common/stdlib/ misc/examples/*.mlc compiler/
# → пусто (кроме .hpp/.cpp/out/)
```

Ни одного `extern fn`, ни одного `.mlc`-модуля, ни одного примера,
вызывающего `HttpRequest.parse`/`HttpRouter` из MLC. `PIPELINE_MERGE_TCP_SPAWN`
дал `Tcp` (bind/accept/read/write_all) + `spawn` вместе в `mlcc` — но это
**сырой байтовый TCP**, не HTTP: `misc/examples/tcp_spawn_echo_mlcc.mlc`
эхо-сервер без парсинга запроса, обрабатывает 2 соединения и завершается,
не долгоживущий сервер.

**Итог: написать `curl`-совместимый HTTP-сервер на MLC сейчас нельзя.**
Единственный путь — писать сырой парсер вручную над `Tcp.read` (никто не
делал), либо C++ без MLC вообще.

## Цель

HTTP/1.1 request-line + headers + body парсинг, роутинг, response-writer —
**чистый MLC-код** над `Tcp.read`/`Tcp.write_all` (байты — `string`/`Array<i32>`).
Ни одного `extern fn` к `http_request.hpp`/`http_router.hpp`. Один `.mlc`-файл:
слушает TCP, принимает соединение, парсит HTTP-запрос, роутит на минимум 2
хендлера, пишет ответ, использует `spawn` на каждое соединение (переиспользуя
`PIPELINE_MERGE_TCP_SPAWN`), обрабатывается через `mlcc`, проверяется `curl`.

## Decision (STEP=1, 2026-07-11) — **locked**

| Вопрос | Locked |
|--------|--------|
| Модуль | `lib/mlc/common/stdlib/net/http_server.mlc`, `module HttpServer` |
| Bare-name | `"HttpServer"` → `"net/http_server.mlc"` in `path_normalize.mlc` (рядом с `"Tcp"`; **не** трогать client-стаб `http.mlc`) |
| Типы | `HttpHeader { name, value: string }`; `HttpRequest { method, path, headers: [HttpHeader], body }`; `HttpResponse { status: i32, headers: [HttpHeader], body }` — как C++ shapes |
| Парсер | `parse_http_request(raw: string) -> HttpParseResult` (`HttpParseOk`/`HttpParseErr` — local sum; std `Option` import segfaults under mlcc); char-scan, **без regex**; header end `\r\n\r\n` или `\n\n`; request-line `METHOD SP path SP HTTP/x.y` (весь `head`, если нет `\n` — empty headers); header `name: value` (trim); body только по `Content-Length` (cap **1 MiB**); header block cap **64 KiB**; no chunked |
| Response | `format_http_response(response) -> string` (+ demo пишет через `Tcp.write_all`); auto `Content-Length` если нет; `Connection: close` |
| Роутер | `[HttpRoute]` где `HttpRoute { method, path, handler }`; exact match method+path; sync `fn(HttpRequest) -> HttpResponse`; default 404 |
| Один vs два файла | **один** модуль (`http_server.mlc`) — request+response+router |
| C++ `.hpp` | удалить на STEP=5 после MLC-демо/гейта; `serve_http_with_thread_pool` — Out of scope / follow-up если ещё жив |

### Rejected

| Вариант | Почему |
|---------|--------|
| `extern fn` над `http_request.hpp` | не убирает C++; отменено пересмотром трека |
| Bare-name `"Http"` → client `http.mlc` | конфликт с fetch-стабом; server — отдельный ключ |
| Regex / chunked / keep-alive / HTTP/2 | нет regex-движка; Out of scope |

### Impact (последующие STEPs)

| File | Change |
|------|--------|
| `lib/mlc/common/stdlib/net/http_server.mlc` | STEP=2 types+parse+format+router |
| `compiler/driver/path_normalize.mlc` | STEP=3 bare `"HttpServer"` |
| `misc/examples/` + `scripts/run_*_gate.sh` | STEP=4 Tcp+spawn+curl |
| `runtime/include/mlc/net/http_*.hpp` + C++ tests | STEP=5 delete |
| `docs/STDLIB_BACKEND.md` | STEP=6 |

## Scope

1. Decision (таблица выше).
2. `lib/mlc/common/stdlib/net/http_server.mlc` — `record HttpRequest`/
   `HttpResponse`, чистые MLC-функции `parse_http_request(raw: string) -> Option<HttpRequest>`,
   `write_http_response(response: HttpResponse) -> string`, роутер
   (`Array` of (method,path,handler)).
3. Bare-name резолвинг в `mlcc` (`path_normalize.mlc:86`) — добавить второй
   ключ в таблицу (сейчас только `"Tcp" => "net/tcp.mlc"`), не ломая `Tcp`.
4. Демо: один `.mlc`-файл — `Tcp.bind`/`accept` + `spawn` (переиспользовать
   `PIPELINE_MERGE_TCP_SPAWN`) на каждое соединение, парсинг новым модулем,
   роутинг на MLC (минимум `GET /` и `GET /health`), ответ. Гейт: реальный
   `curl http://127.0.0.1:<port>/` из скрипта.
5. Удалить `runtime/include/mlc/net/http_request.hpp`/`http_router.hpp`
   (после переключения; `http_server.hpp`'s `serve_http_with_thread_pool` —
   см. Out of scope) + C++-тесты на них.
6. Docs: `STDLIB_BACKEND.md` §1 (исправить строку «HTTP/TCP сервер — есть» —
   MLC-native парсер, не C++-биндинг), `PLAN.md`.
7. Verify-gate: self-host (`mlcc`→`mlcc2` diff), `regression_gate.sh`.

## Out of scope

- HTTP/2, TLS, chunked transfer, keep-alive — как в закрытом `STDLIB_NET_SERVER`.
- `serve_http_with_thread_pool` (C++ ThreadPool-сервер поверх старого
  парсера) — если ничто в MLC-коде на него не ссылается после этого трека,
  удалить в том же коммите как мёртвый код; если ссылается (проверить перед
  STEP=5) — отдельный follow-up, не блокировать закрытие этого трека.
- Async/non-blocking — v1 синхронный хендлер, как решено в `STDLIB_NET_SERVER`.
- WebSocket-интеграция — отдельный трек [TRACK_STDLIB_WEBSOCKET_TO_MLC](TRACK_STDLIB_WEBSOCKET_TO_MLC.md),
  зависит от этого (handshake строится на HTTP request/response).

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Decision: имя модуля/файла; record; grammar парсинга; роутинг. | **done** (2026-07-11: locked above) |
| 2 | `record HttpRequest`/`HttpResponse` + чистый MLC-парсер (byte-scan, без regex). | **done** (2026-07-11: http_server.mlc + parse smoke) |
| 3 | Bare-name резолвинг в `mlcc` для нового модуля (без поломки `Tcp`). | **done** (2026-07-11: path_normalize + smoke bare import) |
| 4 | Демо: `Tcp`+`spawn`+HTTP-парсинг+роутинг в одном `.mlc`; `curl`-гейт. | **done** (2026-07-11: curl gate EXIT 0) |
| 5 | Удалить `http_request.hpp`/`http_router.hpp` (+ проверить `serve_http_with_thread_pool` usage). | **done** (2026-07-11: deleted + http_server.hpp; WS uses `websocket_http.hpp`) |
| 6 | Docs (`STDLIB_BACKEND.md` §1 fix, `PLAN.md`) + example. | **done** (2026-07-11: STDLIB_BACKEND/MLC/FFI_LAYER) |
| 7 | Verify-gate: self-host diff, regression_gate, close. | **done** (2026-07-11: DIFF_EXIT=0; REG 20/0; parse+curl EXIT 0) |

<!-- sub-steps STEP=1: 1) lock module path + bare-name key; 2) lock HttpRequest/HttpResponse fields; 3) lock parse grammar (request-line/headers/CL) + router shape; write Decision section -->
<!-- sub-steps STEP=2: 1) HttpRequest/HttpResponse record types; 2) parse_http_request byte-scan (request-line, headers до пустой строки, body по Content-Length); 3) unit smoke без сети (строковый вход) -->
<!-- sub-steps STEP=4: 1) reuse tcp_spawn_echo_mlcc.mlc accept-loop shape; 2) роутер на 2 хендлера; 3) run_*_gate.sh с curl -->

## Open questions

- None blocking STEP=2. Chunked/`Content-Length` edge cases: v1 = header
  end + optional CL only (caps locked above).
