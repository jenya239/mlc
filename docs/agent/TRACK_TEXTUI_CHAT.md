# Textui chat

Заметки шагов 15–16. Состояние очереди — `docs/agent/TEXTUI_CHAT_PLAN.md`.

## Шаг 15. Поля запроса и JSON

Прочитан `lib/mlc/common/stdlib/net/https_request.mlc`.

- `HttpsRequest`: `method`, `url`, `headers`, `body`, `timeout_milliseconds`, `max_response_bytes`, `ca_bundle_path`, `proxy_url`
- `HttpsHeader`: `name`, `value`
- `HttpsResponse`: `status`, `headers`, `body`
- `HttpsResult`: `HttpsOk(HttpsResponse)` | `HttpsErr(HttpsFailure)`
- `HttpsFailure`: `kind`, `message`

JSON в stdlib:

- `lib/mlc/common/stdlib/data/json_text.mlc` — разбор через `mlc/json/json_abi.hpp`. Вызовы: `json_read`, `json_read_field`, `json_read_index`. Строка уже декодирована; `\uXXXX` и суррогатные пары разбирает библиотека. Модуль не собирает и не экранирует текст.
- `lib/mlc/common/stdlib/data/json.mlc` — отдельный набросок (`str`, `Option`, wildcard, extern `parse_json`). `json_abi.hpp` прямо говорит, что это не этот ABI. Не используется.

Разбор ответа идёт через `json_text`. Сборка тела — `misc/textui/chat_json.mlc`, потому что кодировщика в stdlib нет.

`chat_build_request` ставит `POST`, адрес `{base}/chat/completions`, таймаут 60000 мс, предел ответа 1048576 байт. Поля `stream` нет. Ключ в тело не входит: только заголовок `Authorization`. Пустой `api_key` даёт `ChatRequestRefused` с `ReplyFailed("MLC_CHAT_API_KEY не задан")`, `HttpsRequest` при этом не строится.
