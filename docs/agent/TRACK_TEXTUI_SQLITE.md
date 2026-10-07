# TRACK_TEXTUI_SQLITE

Статус: реализовано 2026-10-07. Источник схемы: `mlc-support/responses/sqlite_app_20261007_090536.md`. Ревью кода: `МОЖНО КОММИТИТЬ` (`sqlite_app_code_review_20261007_104618.md`). `compiler/**` и Ruby не трогать. В `misc/textui/` файл не длиннее 400 строк. Имена без сокращений. Локальная функция не называется `importedName_rest`: mlcc префиксует её модулем импорта.

Этот коммит хранит чаты. Список разговоров в интерфейсе и журнал смены каталога остаются следующими шагами: они не нужны, чтобы сообщения переживали перезапуск, и журнал каталога потребовал бы соединение на кадре файлового менеджера.

## Решения, которые отличаются от ответа Sonnet

1. Соединение живёт только пока открыт чат. `chat_frames` открывает базу до цикла и закрывает на каждом выходе. Кадр файлового менеджера sqlite не вызывает. `window_session.mlc` и `window_placement.mlc` не меняются.
2. `chat_world_new` базу не открывает. Иначе `chat18` и прочие тесты писали бы в `~/.local/share`. Открытие только в `chat_world_open`, его зовёт `chat_frames`.
3. Путь: `MLC_TEXTUI_DATABASE`, если задан. Иначе `$XDG_DATA_HOME/mlc/textui.sqlite`, иначе `$HOME/.local/share/mlc/textui.sqlite`. При `MLC_TEXTUI_BENCH` без `MLC_TEXTUI_DATABASE` путь пустой, базы нет.
4. Пустой jail не считается запретом. `path_inside_jail("", path)` возвращает true на любой путь. Проверка «база внутри jail» выполняется только при непустом `jail_root`. Если путь внутри, чат работает в памяти.
5. `create_directories` не ставит режим 0700 (`std::filesystem::create_directories`). Отдельный FFI ради режима не добавляется.
6. `ChatModel` не меняется. `ChatWorld` получает `persistence`. `chat_frame` не импортирует sqlite. Запись идёт через `persistence_sync`: тот же префикс и та же длина пропускаются, рост пишет хвост, замена префикса стирает сообщения и пишет массив заново.
7. «New chat» по-прежнему `chat_clear`. В базе это удаление сообщений текущего разговора, строка разговора остаётся. Отдельный список разговоров в этом коммите не строится.
8. Кольцо `action` пишется вместе с событием чата. `directory_opened` в этом коммите нет.

## Схема

Файл новый, `PRAGMA user_version = 1`. До таблиц: `auto_vacuum=INCREMENTAL`. Затем `journal_mode=WAL`, `synchronous=NORMAL`, `foreign_keys=ON`, `busy_timeout` 50. Схема применяется через `exec`, не через `prepare`: несколько операторов.

- `conversation(id INTEGER PRIMARY KEY, title TEXT NOT NULL DEFAULT '', created_at INTEGER NOT NULL, updated_at INTEGER NOT NULL, archived INTEGER NOT NULL DEFAULT 0 CHECK (archived IN (0, 1)))`
- индекс `(archived, updated_at DESC, id DESC)`
- `message(id INTEGER PRIMARY KEY, conversation_id INTEGER NOT NULL REFERENCES conversation(id) ON DELETE CASCADE, ordinal INTEGER NOT NULL, role INTEGER NOT NULL CHECK (role IN (0, 1)), body TEXT NOT NULL, failed INTEGER NOT NULL CHECK (failed IN (0, 1)), created_at INTEGER NOT NULL, UNIQUE(conversation_id, ordinal))`
- `app_state(key TEXT PRIMARY KEY, value TEXT NOT NULL)` — только ключ `active_conversation`
- `action(id INTEGER PRIMARY KEY, at INTEGER NOT NULL, kind TEXT NOT NULL, detail TEXT NOT NULL)`

Роль 0 — пользователь, 1 — ассистент. `ordinal` = `COALESCE(MAX(ordinal), 0) + 1` внутри `BEGIN IMMEDIATE`. Время — `CAST(strftime('%s','now') AS INTEGER)`. Разговор создаётся на первом принятом сообщении, не при старте.

Заголовок — первые 60 кодовых точек первого сообщения пользователя, `UPDATE ... WHERE title = ''`. Обрезка не режет UTF-8 посередине байта-продолжения.

Ошибка с непустым ключом, если текст содержит ключ, пишется как `Ошибка (текст скрыт)`. Прочие ошибки обрезаются до 500 байт по той же границе. Ключ в `action` не попадает.

Кольцо: после вставки `DELETE FROM action WHERE id <= last_insert_rowid() - 200`. Виды этого коммита: `message_submitted`, `reply_canceled`, `conversation_cleared`. В `detail` — id разговора.

При открытии один раз: удалить архивные, если их больше 200 или `created_at` старше 90 суток, затем `incremental_vacuum(200)`. Автоудаления обычных разговоров нет. `VACUUM` нет.

## Память и кадр

`ChatPersistence`: `database: i32` (0 — выключено), `conversation_id: i64` (0 — разговора нет), `persisted_count: i32`, `failed_revision: i32`, `failed_length: i32`. После загрузки `persisted_count` равен длине массива в памяти, включая дописанное «Прервано».

Сравнение идёт с моделью на входе кадра, не с одной длиной. Если сообщения кадра — продолжение прежних, пишется хвост `messages[persisted_count..]`. Если префикс заменён (очистка и новое сообщение в одном кадре), сообщения разговора удаляются и записывается весь текущий массив. `persisted_count` меняется только после `commit` с кодом 0 и возвращается вместе с записью. Пока кадр не забрал это значение, повтор не считается успешным.

Повтор после ошибки не идёт на каждом следующем кадре: запоминаются `failed_revision` и `failed_length`. Тот же `revision` и та же длина sqlite не вызывают. Следующее изменение модели повторяет запись.

| Событие | Запись |
|---|---|
| префикс тот же и длина выросла | одна транзакция: хвост, заголовок, `message_submitted` для новой роли пользователя, `reply_canceled` если текст «Остановлено» |
| префикс заменён | удалить сообщения разговора, записать текущий массив, `conversation_cleared`, заголовок с первого сообщения пользователя или пустой |
| тот же `revision` и та же длина, что на входе кадра | sqlite не вызывается, тексты сообщений не сравниваются |
| префикс тот же, длина входа равна `persisted_count`, длина выхода не выросла | sqlite не вызывается |
| префикс входа не совпадает с `persisted_count` | стереть сообщения и записать текущий массив, даже если входной массив короче сохранённого |
| тот же `revision` и длина, что у неудачной записи | sqlite не вызывается |
| ошибка или BUSY | `rollback`, если транзакция открыта; модель в памяти остаётся |
| выход из чата | один выход из `chat_frames` после цикла, затем один `close_db` |

Путь базы и jail приводятся через `absolute_path` (лексическая нормализация, без нового FFI). Симлинки не раскрываются. Ключ вырезается до обрезки. Ошибка ассистента, где ключ встречается целиком, заменяется на `Ошибка (текст скрыт)`. Заголовок тоже проходит эту замену. Обрезка ошибки — 500 байт по границе UTF-8, заголовок — 60 кодовых точек.

Хвост идемпотентен за счёт того, что повтор пишет только ещё не записанный суффикс массива, а `ordinal` берётся из SQL. Повторная запись того же суффикса не делается, пока `persisted_count` не обновлён после успешного commit.

Рабочий поток чата handle не получает. `chat_transport.mlc` и `chat_http.mlc` не импортируют sqlite.

Восстановление в `chat_world_open`: активный разговор, иначе последний неархивный. Хвост `ORDER BY ordinal DESC LIMIT 1000`, затем разворот. Если последнее загруженное — пользователь без `failed`, дописывается ассистент `Прервано` и сразу пишется в базу. `chat_world_open` измеряет уже загруженную модель один раз, тем же проходом, что `chat_world_new`.

## Файлы

- `misc/textui/text_boundary.mlc` — обрезка по кодовым точкам и по байтам
- `misc/textui/chat_redaction.mlc` — ключ вырезается до обрезки
- `misc/textui/chat_database.mlc` — путь, каталог, open, pragma, схема, чистка архива, close
- `misc/textui/statement_query.mlc` — prepare, bind, step, один finalize
- `misc/textui/chat_store.mlc` — разговор, хвост, загрузка, очистка, активный id
- `misc/textui/action_ring.mlc` — вставка и кольцо
- `misc/textui/chat_persistence.mlc` — запись `ChatPersistence`, префикс и повтор после ошибки
- `misc/textui/test/store_schema.mlc`, `store_messages.mlc`, `store_ring.mlc`
- правки: `chat_world.mlc`, `chat_frame.mlc`, `chat_host.mlc`, `app.mlc` (аргумент jail в `chat_frames`), литералы `ChatWorld` в `chat18_frame.mlc`
- `scripts/run_textui_store_gate.sh`

Импорт `db/sqlite.mlc` разрешён только в `chat_database.mlc` и `statement_query.mlc`. Функции `chat_persistence.mlc` называются `persistence_*`.

## Проверка

`bash scripts/run_textui_store_gate.sh all` код 0. Нет `sqlite3.h` — код 2.

Зонд схемы: повторное открытие, `user_version = 1`, foreign keys, UNIQUE даёт constraint, WAL на файле, второй `close_db` не вызывается, путь внутри непустого jail отвергается, пустой jail путь не отвергает.

Зонд сообщений: круг запись-чтение, кириллица в заголовке, лимит 1000, идемпотентный хвост, откат не оставляет половину, ключ в ошибке скрыт, очистка, каскад, «Прервано» после пользователя без ответа.

Зонд кольца: 250 вставок оставляют 200, в `detail` нет текста сообщения.

`ruby scripts/run_textui_file_size.rb` код 0. `bash scripts/run_textui_chat_smoke.sh` код 0: тесты чата не задают `MLC_TEXTUI_DATABASE`. `bash scripts/run_sqlite_gate.sh all` код 0.

Кадр без события: в `chat_frame.mlc` нет импорта sqlite. Повторный `persistence_sync` с той же моделью не добавляет строк. Это проверка вместо strace всего окна.

## Следующие шаги, не этот коммит

- Список разговоров, переключение, архив. Пока `pending`, переключение запрещено.
- `directory_opened` с удержанием в памяти. Не на кадре файлового менеджера и не внутри jail-листинга.
