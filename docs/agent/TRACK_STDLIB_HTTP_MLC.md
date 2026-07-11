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

## Status: **open** — STEP=1 next (Decision)

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

## Decision (STEP=1, нужно зафиксировать)

| Вопрос | Варианты | Рекомендация |
|--------|----------|--------------|
| Где живёт парсер | (a) `lib/mlc/common/stdlib/net/http_server.mlc` (новый, не трогать client-стаб `http.mlc`) | (a) — отдельный модуль, добавить второй bare-name ключ в `mlcc` |
| Представление запроса | Честный MLC `record HttpRequest { method: string, path: string, headers: Array<...>, body: string }`, собранный парсером один раз на запрос | record — не hot-path per-byte, разовая аллокация на соединение |
| Парсинг request-line/headers | Ручной byte/char-scan на `string` (`.char_at`/`.substring`/`.index_of`, как в `points_mem.mlc`/subnet-calculator примерах) — без regex (в MLC нет regex-движка) | построчный сплит по `\r\n`, `substring` до первого `:`/пробела — стандартный подход HTTP-парсеров без библиотеки |
| Роутинг | Явный `Array`/`Map` (method,path)→handler на MLC (как задумано в закрытом `STDLIB_NET_SERVER`) | без изменений |
| Хендлер | `fn(HttpRequest) -> HttpResponse` синхронный | без изменений — v1 остаётся sync |
| C++ `http_request.hpp`/`http_router.hpp`/`http_server.hpp` | Удалить после переключения (не оставлять параллельный C++-путь — иначе два парсера, расхождение поведения) | удалить; `serve_http_with_thread_pool` (C++-only ThreadPool-сервер) — отдельный вопрос, не блокирует этот трек, но зафиксировать в Out of scope явно |

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
| 1 | Decision: имя модуля/файла; record; grammar парсинга; роутинг. | pending |
| 2 | `record HttpRequest`/`HttpResponse` + чистый MLC-парсер (byte-scan, без regex). | pending |
| 3 | Bare-name резолвинг в `mlcc` для нового модуля (без поломки `Tcp`). | pending |
| 4 | Демо: `Tcp`+`spawn`+HTTP-парсинг+роутинг в одном `.mlc`; `curl`-гейт. | pending |
| 5 | Удалить `http_request.hpp`/`http_router.hpp` (+ проверить `serve_http_with_thread_pool` usage). | pending |
| 6 | Docs (`STDLIB_BACKEND.md` §1 fix, `PLAN.md`) + example. | pending |
| 7 | Verify-gate: self-host diff, regression_gate, close. | pending |

<!-- sub-steps STEP=2: 1) HttpRequest/HttpResponse record types; 2) parse_http_request byte-scan (request-line, headers до пустой строки, body по Content-Length); 3) unit smoke без сети (строковый вход) -->
<!-- sub-steps STEP=4: 1) reuse tcp_spawn_echo_mlcc.mlc accept-loop shape; 2) роутер на 2 хендлера; 3) run_*_gate.sh с curl -->

## Open questions

- `Content-Length`/chunked edge cases при byte-scan без библиотеки — v1
  ограничить фиксированным чтением до `\r\n\r\n` + `Content-Length` заголовок
  (если есть), без chunked (уже в Out of scope).
- Один module или два (`http_request`/`http_router` отдельно) — решить на
  STEP=1, не критично для v1.
