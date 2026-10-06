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

## Шаг 16. Поиск чтения Task и слот

Поиск по `lib/mlc/common/stdlib`, `runtime/include/mlc/core/task.hpp`, `runtime/include/mlc/concurrency/spawn.hpp`, `runtime/include/mlc/concurrency/channel.hpp`, `runtime/include/mlc/reactor/https_async_bridge.hpp`.

- `is_ready` только смотрит `handle.done()`. Значение Task читает один `block_on`. Для реактора он крутит цикл до конца. Это не чтение готового Task без ожидания.
- `https_async.mlc`: `https_send_async`, `task_ready`, `task_ready_pair`, `task_all`, `task_then`. `task_all` и `task_then` сами зовут `block_on`.
- `spawn` опускается в `spawn_task`: корутина делает `future.get()`. Пока её не возобновили, `is_ready` ложен, а возобновление ждёт future. Сброс Task ждёт поток (`std::async`, диагностика E089). Неблокирующего join со значением нет.
- `channel.hpp` имеет `try_receive`, в компиляторе этого имени нет. Из MLC доступен блокирующий `receive`.
- `Mutex.lock` держит мьютекс на время колбэка. `try_lock` нет. Атомики — скаляры, слот `(generation, status, body)` в них не кладётся.

Неблокирующего чтения Task нет, и у `spawn` нет готового неблокирующего возврата. Выбран слот: `runtime/include/mlc/chat/mailbox.hpp`.

- `transport_launch` при занятом слоте возвращает 0 и поток не создаёт. Иначе помечает полёт и отделяет поток.
- Поток вызывает `block_on(https_send_async(...))` на своём реакторе. Таймаут запроса ставится в 60000 мс.
- Слот хранит `generation`, признак транспортной ошибки, HTTP-статус и текст. `mailbox_take_ready` забирает это на вызывающем потоке и снимает полёт. До забора второй `transport_start` ничего не запускает.
- Отмены нет. «Stop» по-прежнему только поколение; транспорт передачу не прерывает.
- `chat_transport.mlc` импортирует `https_async`, чтобы в программе были `https_async.hpp` и библиотека `curl`.

