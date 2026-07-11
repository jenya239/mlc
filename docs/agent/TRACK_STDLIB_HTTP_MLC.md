# Track: HTTP-сервер (парсер+роутер) — доступен из MLC, не только C++

Parent: [../PLAN.md](../PLAN.md), [../STDLIB_BACKEND.md](../STDLIB_BACKEND.md) §1/§5,
[../archive/tracks/TRACK_STDLIB_NET_SERVER.md](../archive/tracks/TRACK_STDLIB_NET_SERVER.md),
[../archive/tracks/TRACK_PIPELINE_MERGE_TCP_SPAWN.md](../archive/tracks/TRACK_PIPELINE_MERGE_TCP_SPAWN.md).
Trigger: пользователь 2026-07-11 — по итогам ревью выяснилось, что HTTP-слой
(парсер запроса, роутер, `serve_http_with_thread_pool`) реализован и
протестирован **только в C++**; из MLC он не вызывается вообще, даже после
того как `Tcp`+`spawn` стали работать вместе в `mlcc`.

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

`HttpRequest.parse`/`HttpRouter`/response-writer доступны из MLC как
`extern fn` (тот же паттерн, что `Tcp` — обёртка над уже готовым и
протестированным C++, не переписывание парсера на MLC). Один `.mlc`-файл:
слушает TCP, принимает соединение, парсит HTTP-запрос, роутит на минимум 2
хендлера, пишет ответ, использует `spawn` на каждое соединение (переиспользуя
`PIPELINE_MERGE_TCP_SPAWN`), обрабатывается через `mlcc`, проверяется `curl`.

## Decision (STEP=1, нужно зафиксировать)

| Вопрос | Варианты | Рекомендация |
|--------|----------|--------------|
| Где живёт обёртка | (a) `lib/mlc/common/stdlib/net/http.mlc` (сейчас там только stub `fetch`, переписать под сервер) вместо client-стаба; (b) новый файл `http_server.mlc` рядом | (b) — не трогать client-заглушку, отдельный модуль под именем `HttpServer` или расширить bare-name таблицу `mlcc` вторым именем |
| Владение памятью запроса | `HttpRequest` как opaque handle (`i32`, как `Tcp`-хендлы) vs честный MLC `record` с полями `method`/`path`/`headers`/`body`, собранный один раз при парсинге | record — С++ `parse_http_request` уже возвращает готовую структуру, копия в `Array`/`string` полей один раз на запрос, не hot-path per-byte |
| Роутинг | (a) явный `Array`/`Map` (method,path)→handler на MLC (уже так задумано в закрытом `STDLIB_NET_SERVER` §Decision); (b) обёртка над C++ `HttpRouter` целиком | (a) роутинг на MLC поверх C++ только parse/write — больше контроля, не два уровня диспетчеризации |
| Хендлер | `fn(HttpRequest) -> HttpResponse` синхронный (как в закрытом NET_SERVER) | без изменений — v1 остаётся sync |

## Scope

1. Decision (таблица выше) + минимальный набор `extern fn` (`parse_http_request`,
   `write_http_response`, геттеры полей `HttpRequest` если handle-based).
2. `lib/mlc/common/stdlib/net/http_server.mlc` (или выбранное на STEP=1 имя) —
   record `HttpRequest`/`HttpResponse`, `extern fn` обёртки над
   `runtime/include/mlc/net/http_request.hpp`/`http_router.hpp`.
3. Bare-name резолвинг в `mlcc` (`path_normalize.mlc:86`) — добавить второй
   ключ в таблицу (сейчас только `"Tcp" => "net/tcp.mlc"`), не ломая `Tcp`.
4. Демо: один `.mlc`-файл — `Tcp.bind`/`accept` + `spawn` (переиспользовать
   `PIPELINE_MERGE_TCP_SPAWN`) на каждое соединение, парсинг через новый
   модуль, роутинг на MLC (минимум `GET /` и `GET /health`), ответ. Гейт:
   реальный `curl http://127.0.0.1:<port>/` из скрипта.
5. Docs: `STDLIB_BACKEND.md` §1 (исправить строку «HTTP/TCP сервер — есть» —
   уточнить C++-only vs MLC-reachable до этого трека), `PLAN.md`.
6. Verify-gate: self-host (`mlcc`→`mlcc2` diff), `regression_gate.sh`.

## Out of scope

- HTTP/2, TLS, chunked transfer, keep-alive — как в закрытом `STDLIB_NET_SERVER`.
- Замена C++ `serve_http_with_thread_pool` — остаётся как есть, этот трек
  добавляет параллельный MLC-путь.
- Async/non-blocking — v1 синхронный хендлер, как решено в `STDLIB_NET_SERVER`.
- WebSocket-интеграция (уже отдельно закрыта, upgrade поверх обычного accept).

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Decision: имя модуля/файла; handle vs record; роутинг на MLC vs C++; extern fn список. | pending |
| 2 | `extern fn` обёртки над `http_request.hpp`/`http_router.hpp` в новом MLC-модуле. | pending |
| 3 | Bare-name резолвинг в `mlcc` для нового модуля (без поломки `Tcp`). | pending |
| 4 | Демо: `Tcp`+`spawn`+HTTP-парсинг+роутинг в одном `.mlc`; `curl`-гейт. | pending |
| 5 | Docs (`STDLIB_BACKEND.md` §1 fix, `PLAN.md`) + example. | pending |
| 6 | Verify-gate: self-host diff, regression_gate, close. | pending |

<!-- sub-steps STEP=2: 1) HttpRequest/HttpResponse record types; 2) extern fn parse/write; 3) unit smoke без сети -->
<!-- sub-steps STEP=4: 1) reuse tcp_spawn_echo_mlcc.mlc accept-loop shape; 2) роутер на 2 хендлера; 3) run_*_gate.sh с curl -->

## Open questions

- Один module или два (`http_request`/`http_router` отдельно, ближе к
  структуре C++ заголовков) — решить на STEP=1, не критично для v1.
