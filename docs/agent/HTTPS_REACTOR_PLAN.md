# HTTPS reactor

Единственный файл состояния этой работы. Очередь не смотрит `SESSION.md` и не смотрит `docs/agent/CONTINUITY.md`.

Источник схемы: разговор 2026-10-05 и ответ Sonnet `mlc-support/responses/async_plan_20261005_213551.md`. Этот файл важнее того ответа там, где пути расходятся.

```
next_step: 4
step_1: done — bash scripts/run_reactor_cpp_smoke.sh exit 0, elapsed_milliseconds=100, thread_count=1
step_2: done — bash scripts/run_reactor_https_cpp_smoke.sh exit 0; HTTPS_CLIENT_REQUIRE=1 bash scripts/run_https_client_gate.sh exit 0
step_3: done — bash scripts/run_reactor_cpp_smoke.sh exit 0, block_on_sleep elapsed_milliseconds=30, spawn_without_event_loop
step_4: pending
step_5: pending
step_6: pending
step_7: pending
```

Поле шага меняется только на `pending`, `done` или `blocked`. Рядом с `done` пишется короткая строка: коммит и команда, которая завершилась с кодом 0. Рядом с `blocked` пишется факт, который остановил шаг. `next_step` — наименьший номер со статусом не `done`.

## Схема, её не менять

Новых ключевых слов нет. В текст MLC не добавляются `await`, `async`, `co_await`. `co_await` допустим только в C++ рантайме.

Асинхронный объект один: уже существующий `Task<T>` (`mlc::Task<T>` в `runtime/include/mlc/core/task.hpp`). Тип Promise не вводится.

- `https_send` остаётся блокирующим вызовом `curl_easy_perform`.
- `https_send_async(request) -> Task<HttpsResult>` сразу возвращает `Task` и сразу регистрирует передачу в общем `CURLM*`.
- `block_on` уже есть. Для реакторной задачи он крутит цикл, пока эта задача не готова. Уже зарегистрированные передачи за это время продвигаются.
- `task_all` возвращает `Task` с обоими результатами.
- `task_then` принимает `Task` и функцию, которая по значению возвращает следующий `Task`.

Целевой вид на MLC:

```mlc
let response = block_on(https_send_async(request))

let user = block_on(https_send_async(user_request))
let orders = block_on(https_send_async(orders_request(user)))

let models = https_send_async(models_request)
let billing = https_send_async(billing_request)
let pair = block_on(task_all(models, billing))

fn load_order(identifier: string) -> Task<HttpsResult> =
  task_then(https_send_async(user_request(identifier)), user =>
    https_send_async(order_request(user))
  )
```

Ошибка — значение `HttpsErr`, не исключение. Ветки пишутся `match result { HttpsOk(response) => ... HttpsErr(_) => ... }`. Суммы не сравниваются через `==`.

## Факты репозитория

Их не переоткрывать чтением «на всякий случай» дольше, чем нужно, чтобы попасть в существующий вызов.

- Тесты C++ HTTPS живут в `runtime/test/*.cpp`. Их собирает shell-скрипт, не CMake. Образец: `scripts/run_curl_abi_cpp_smoke.sh` (`c++ -std=c++20 -pthread -I runtime/include`).
- Программы MLC для смоуков живут в `misc/examples/*.mlc`. Их гоняют `scripts/run_https_*.sh` через `compiler/out/mlcc` и `compiler/build_bin.sh`.
- `curl_abi.hpp` — заголовок с inline-функциями. Новый реактор делать так же: `runtime/include/mlc/reactor/*.hpp`, без новой библиотечной цели.
- `Task<void>` уже есть (`runtime/include/mlc/core/task.hpp`).
- `mlc::block_on` только вызывает `resume`, пока `done()` (строки 170–176 и 279–282 того же файла). Для сокета этого мало.
- `spawn` — `std::async(std::launch::async)` в `runtime/include/mlc/concurrency/spawn.hpp`. Этот путь остаётся системным потоком.
- Примитивы HTTPS объявляются в `lib/mlc/common/stdlib/net/https_client.mlc` как `extern fn ... from "mlc/net/curl_abi.hpp"` с `blocking` или `thread_safe`. Новый MLC-вызов регистрируется так же, во новом модуле. `compiler/**` не менять.
- В `lib/mlc/common/stdlib` нет `extern fn`, который принимает функцию. `spawn` — ключевое слово компилятора. Шаг 5 обязан проверить, что mlcc уже компилирует лямбду аргументом такого extern. Если нет — шаг `blocked`, `compiler/**` не трогать.
- `scripts/https_test_server.rb` уже пишет `connection_count`, `silent_port`, `proxy_port`, `good_port`. Сервер не расширять, пока существующего маршрута хватает.
- Локальный стенд только `127.0.0.1` и свой CA. Публичный API не вызывать.
- mlcc с libcurl не линкуется. Вызовы curl остаются в рантайме.
- Имена в новом коде — полные английские слова.
- В рабочем дереве уже есть чужой незакоммиченный diff (`compiler/**`, `misc/textui/**`, шимы freetype/harfbuzz, `CLAUDE.md`, `README.md`). Его не добавлять в коммит, не откатывать.

Коды результата, как у текущего клиента: 1 неверный запрос, 2 имя не разрешилось, 3 соединение не установлено, 4 таймаут, 5 сертификат отвергнут, 6 тело больше лимита, 7 транспорт. Отмена: сообщение ровно `stop requested`, код 7. Лимит тела считается по раскодированным байтам. Пустой `proxy_url` ставит `CURLOPT_PROXY ""`. Протоколы только https. `FOLLOWLOCATION` остаётся 0. HTTP/2 — `CURL_HTTP_VERSION_2TLS` с откатом на HTTP/1.1.

## Вне шагов

Канал, файлы, `https_stream_open`, keep-alive сессии, `epoll`, `io_uring`, Asio, ключевое слово `await`, правки `compiler/**`, правки `lib/mlc/common/stdlib/net/http.mlc`, генерация OpenAPI, перевод `spawn` на реактор, `ThreadPool` для ожидания сокета, `resume` из колбэка curl.

## Шаг 1. Цикл и таймеры

Статус: `done`. `bash scripts/run_reactor_cpp_smoke.sh` завершился с кодом 0 (`elapsed_milliseconds=100`, `thread_count=1`).

Файлы: `runtime/include/mlc/reactor/event_loop.hpp`, `timer_heap.hpp`, `wakeup_descriptor.hpp`, `sleep_task.hpp`, `runtime/test/test_reactor_timers.cpp`, `scripts/run_reactor_cpp_smoke.sh`.

Сделать:

- `WakeupDescriptor`: `eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC)`, `signal`, `drain`, деструктор закрывает дескриптор.
- `TimerHeap`: min-heap, ленивая отмена по идентификатору, `next_deadline`, `pop_expired`.
- `EventLoop::current()` — один цикл на системный поток (`thread_local`). `run_until(predicate)` ждёт в `poll` (wake-up fd и позже сокеты). Таймаут — до ближайшего таймера. `EINTR` повторяется. Истёкшие продолжения собираются в локальный вектор и только потом получают `resume`.
- `sleep_for_milliseconds` возвращает `Task<void>`. `co_await` только здесь.
- Тест: два сна, 50 и 100 мс, оба готовы быстрее 140 мс. Поле `Threads:` в `/proc/self/status` на время теста не вырастает. `EventLoop::current()` с другого системного потока — другой адрес.

Проверка: `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_reactor_cpp_smoke.sh` завершается с кодом 0.

## Шаг 2. Одна HTTPS-передача

Статус: `done`. `bash scripts/run_reactor_https_cpp_smoke.sh` и `HTTPS_CLIENT_REQUIRE=1 bash scripts/run_https_client_gate.sh` завершились с кодом 0. `scripts/run_reactor_cpp_smoke.sh` линкует libcurl, потому что `EventLoop` владеет `CURLM*`.

Файлы: `runtime/include/mlc/reactor/https_transfer.hpp`, `runtime/test/test_reactor_https.cpp`, `scripts/run_reactor_https_cpp_smoke.sh`. Меняются `event_loop.hpp` и `runtime/include/mlc/net/curl_abi.hpp` только выносом общей настройки easy-handle.

Сделать:

- Участок опций easy-handle (только https, `FOLLOWLOCATION` 0, прокси, `ACCEPT_ENCODING ""`, `CURL_HTTP_VERSION_2TLS`, CA, таймаут, лимит раскодированного тела) вынести в функцию, которую вызывают и `perform_request`, и реактор. `perform_request` дальше по-прежнему вызывает `curl_easy_perform`.
- Классификацию кодов 1–7 вызывать и оттуда, и из реактора.
- `EventLoop` лениво создаёт один `CURLM*`. `CURLMOPT_SOCKETFUNCTION` и `CURLMOPT_TIMERFUNCTION` только записывают желаемые события и срок. `curl_multi_socket_action` и `curl_multi_info_read` вызываются из `run_until`, не из колбэка.
- `start_https_transfer` — обычная функция, не корутина на время регистрации: easy-handle добавляется в `CURLM*` до возврата `Task`. Состояние передачи держит `std::shared_ptr`, не кадр корутины. Уже запрошенный `StopToken` не создаёт easy-handle и возвращает готовый результат с кодом 7 и текстом `stop requested`.
- Тест на `scripts/https_test_server.rb`: GET с телом, чужой сертификат (код 5), закрытый порт (код 3), неразрешимое имя (код 2). Каждый случай сравнить с блокирующим путём: код, сообщение, тело.

Проверка: `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_reactor_https_cpp_smoke.sh` и `HTTPS_CLIENT_REQUIRE=1 TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_https_client_gate.sh`, оба кода 0.

## Шаг 3. `block_on` качает цикл

Статус: `done`. `bash scripts/run_reactor_cpp_smoke.sh` завершился с кодом 0 (`block_on_sleep elapsed_milliseconds=30`, `spawn_without_event_loop`).

Файлы: `runtime/include/mlc/core/task.hpp`, при необходимости `event_loop.hpp`, `runtime/test/test_reactor_block_on.cpp`. Подключить тест к `scripts/run_reactor_cpp_smoke.sh`.

Сделать:

- У `Task` признак `Plain` или `Reactor`. По умолчанию `Plain`. Реакторные задачи помечаются `Reactor`.
- `Plain`: прежнее тело `block_on`.
- `Reactor`: `EventLoop::current().run_until`, пока кадр не `done()`, затем результат.
- `is_ready` цикл не вызывает.
- Тест: `block_on(sleep_for_milliseconds(30))` возвращается. `is_ready` реакторной задачи остаётся ложью после `std::this_thread::sleep_for` 100 мс без `block_on`. `spawn` плюс `block_on` возвращает значение и не создаёт `EventLoop` (`has_current()` ложь).

Проверка: `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_reactor_cpp_smoke.sh`, код 0.

## Шаг 4. Две передачи на одном потоке

Статус: `pending`.

Файлы: `runtime/test/test_reactor_parallel.cpp`, подключение в `scripts/run_reactor_https_cpp_smoke.sh`. `scripts/https_test_server.rb` не менять: `silent_port` и `connection_count` уже есть.

Сделать:

- Два обычных запроса стартуют до `block_on`. К возврату из ожидания оба завершены. `connection_count` вырос на 2. Если HTTP/2 сольёт их в одно соединение, развести запросы на `good_port` и второй слушатель того же сертификата (`cross_port` в том же сервере, если он есть; иначе зафиксировать в тесте, каким путём получены два accept). `Threads:` не растёт.
- `block_on` ждёт тихий порт с коротким таймаутом (код 4). Обычный запрос, стартовавший раньше, к этому моменту уже успешен.

Проверка: `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_reactor_https_cpp_smoke.sh`, код 0. В выводе есть рост `connection_count` на 2 и неизменное число потоков.

## Шаг 5. Поверхность MLC

Статус: `pending`.

Файлы: `runtime/include/mlc/reactor/task_combinators.hpp`, `lib/mlc/common/stdlib/net/https_async.mlc`, `misc/examples/https_send_async_basic.mlc`, `https_send_async_sequential.mlc`, `https_task_all_smoke.mlc`, `https_task_then_smoke.mlc`, `scripts/run_reactor_mlc_smoke.sh`.

Сделать:

- Сначала минимальная программа, которая передаёт лямбду в новый `extern fn`, и прогон её через `compiler/out/mlcc`. Если mlcc отвергает вызов — записать `step_5: blocked` и остановиться. `compiler/**` не менять.
- Если вызов принимается: `task_all` возвращает `Task` пары результатов (запись с двумя полями, не `std::pair` в тексте MLC). `task_then` ждёт источник, вызывает функцию, ждёт возвращённый ею `Task`. `HttpsErr` функция видит сама, через `match`.
- `https_send_async` объявить в `https_async.mlc` тем же видом `extern fn ... from`, что `perform_request` в `https_client.mlc`. Токена нет — передаётся не запрошенный `StopToken`.
- Четыре примера из раздела «Схема» собираются mlcc, линкуются `compiler/build_bin.sh` и проходят на локальном сервере. Маршруты брать из `scripts/https_test_server.rb`.

Проверка: `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_reactor_mlc_smoke.sh` и шлюз `run_https_client_gate.sh`, оба кода 0.

## Шаг 6. Редиректы, прокси, лимит, отмена

Статус: `pending`.

Файлы: `lib/mlc/common/stdlib/net/https_redirect_async.mlc`, примеры в `misc/examples/`, `runtime/test/test_reactor_cancel.cpp`. Меняются `event_loop.hpp`, `https_transfer.hpp` и, если разбор `Location` спрятан внутри одной функции, `lib/mlc/common/stdlib/net/https_redirect.mlc` только выносом чистых функций. Поведение `https_send_following_redirects` не меняется.

Сделать:

- `https_send_following_redirects_async` повторяет правила блокирующей функции: 301, 302, 303, 307, 308, только https, бюджет хопов, снятие `Authorization` и `Cookie` при смене authority, те же правила метода и тела. Хопы собираются через `task_then`. Для уже готового значения — `task_ready`, если такой функции ещё нет, добавить её в рантайм с признаком `Reactor`.
- Пустой и непустой `proxy_url` проверить тем же способом, что `scripts/run_https_proxy_smoke.sh`: мёртвый `https_proxy` при пустом `proxy_url` не используется; явный `proxy_url` увеличивает `proxy_connect_count`.
- Gzip-маршрут `/compressed` уже есть. Лимит ниже раскодированной длины даёт код 6.
- Если у `StopToken` нет подписки, добавить `subscribe` / снятие подписки. Подписка вызывает `signal` у `WakeupDescriptor`. После уничтожения цикла запись в дескриптор не выполняется (`shared_ptr` на дескриптор). Уже запрошенный токен не увеличивает `connection_count`. Отмена со второго системного потока во время тихого порта возвращает код 7 и текст `stop requested` быстрее таймаута запроса.
- Существующие `scripts/run_https_redirect_smoke.sh` и `scripts/run_https_proxy_smoke.sh` остаются зелёными.

Проверка: `scripts/run_reactor_mlc_smoke.sh`, `scripts/run_reactor_https_cpp_smoke.sh`, код 0.

## Шаг 7. Матрица

Статус: `pending`.

Файлы: `scripts/run_reactor_matrix.sh`, недостающие кейсы в уже созданных тестах (`runtime/test/test_reactor_lifecycle.cpp`, если жизни дескриптора ещё нет отдельным файлом).

`scripts/run_reactor_matrix.sh` запускает смоуки реактора и регрессию. Ненулевой код любого пункта останавливает скрипт.

| Проверка | Где |
|---|---|
| GET 200 | `test_reactor_https.cpp`, `https_send_async_basic.mlc` |
| POST 200, тело эха | `test_reactor_https.cpp`, маршрут `/session_echo` или существующий echo |
| Таймаут, код 4 | тихий порт |
| Чужой сертификат, код 5 | сверка с `https_send` |
| Раскодированное тело больше лимита, код 6 | `/compressed` |
| Пустой `proxy_url` игнорирует `https_proxy` | MLC-смоук |
| Непустой `proxy_url` даёт CONNECT | MLC-смоук |
| Отмена до соединения | `connection_count` не растёт |
| Отмена на тихом порту с другого потока | `test_reactor_cancel.cpp` |
| Два параллельных запроса | `test_reactor_parallel.cpp`, `https_task_all_smoke.mlc` |
| Цепочка | `https_task_then_smoke.mlc` |
| Редирект по правилам блокирующего клиента | async-смоук |
| `is_ready` без `block_on` ложь 200 мс | тест шага 3 |
| `spawn` + `block_on` не создаёт цикл | тест шага 3 |
| `https_send` сохраняет коды 1–7 | шлюз |
| 100 запросов подряд | `Threads:` после прогрева не растёт на число запросов |
| Поток с циклом завершился | число записей `/proc/self/fd` то же; `wake` после уничтожения не падает |

Регрессия в конце матрицы:

```bash
HTTPS_CLIENT_REQUIRE=1 TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_https_client_gate.sh
TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_curl_abi_cpp_smoke.sh
```

и каждый `scripts/run_https_*.sh`, кроме самого шлюза, если шлюз его уже включает. Перед началом шага 7 снять вывод этих команд. После шага вывод тех же команд совпадает по коду выхода 0.

Проверка шага: `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_reactor_matrix.sh`, код 0.
