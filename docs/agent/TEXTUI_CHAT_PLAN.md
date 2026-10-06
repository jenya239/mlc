# Textui chat

Единственный файл состояния этой работы. Очередь не смотрит `SESSION.md`, не смотрит `docs/agent/CONTINUITY.md` и не смотрит `docs/agent/TRACK_TEXTUI_FILE_MANAGER.md`.

Источник шагов: `mlc-support/responses/textui_chat_gap_20261006_093251.md` (инвентарь и шаг 1) и `mlc-support/responses/textui_chat_gap_20261006_094033.md` (шаги 2–19). Этот файл важнее тех ответов там, где пути расходятся.

```
next_step: 16
step_1: done — TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh exit 0; bash scripts/run_textui_slice01_smoke.sh exit 0; ruby scripts/run_textui_file_size.rb exit 0
step_2: done — TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh exit 0; ruby scripts/run_textui_file_size.rb exit 0
step_3: done — TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh exit 0; TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_slice01_smoke.sh exit 0; ruby scripts/run_textui_file_size.rb exit 0
step_4: done — TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh exit 0; TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_slice0_smoke.sh exit 0; TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_slice01_smoke.sh exit 0; ruby scripts/run_textui_file_size.rb exit 0
step_5: done — TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh exit 0; ruby scripts/run_textui_file_size.rb exit 0
step_6: done — TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh exit 0; TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_slice0_smoke.sh exit 0; TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_slice01_smoke.sh exit 0; TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_slice12_smoke.sh exit 0; TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_slice13_smoke.sh exit 0; TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_slice14_smoke.sh exit 0; TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_slice15_smoke.sh exit 0; TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_slice18_smoke.sh exit 0; TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_slice19_smoke.sh exit 0; ruby scripts/run_textui_file_size.rb exit 0
step_7: done — TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh exit 0; TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_slice12_smoke.sh exit 0; TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_slice13_smoke.sh exit 0; ruby scripts/run_textui_file_size.rb exit 0
step_8: done — TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh exit 0; TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_slice13_smoke.sh exit 0; TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_slice14_smoke.sh exit 0; ruby scripts/run_textui_file_size.rb exit 0
step_9: done — TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh exit 0; ruby scripts/run_textui_file_size.rb exit 0
step_10: done — TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh exit 0; ruby scripts/run_textui_file_size.rb exit 0; ruby scripts/run_text_gl_perf_corpus.rb exit 0
step_11: done — TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh exit 0; ruby scripts/run_textui_file_size.rb exit 0
step_12: done — TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh exit 0; ruby scripts/run_textui_file_size.rb exit 0
step_13: done — TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh exit 0; ruby scripts/run_textui_file_size.rb exit 0
step_14: done — TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh exit 0; ruby scripts/run_textui_file_size.rb exit 0
step_15: done — TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh exit 0; ruby scripts/run_textui_file_size.rb exit 0
step_16: pending
step_17: pending
step_18: pending
step_19: pending
```

Поле шага меняется только на `pending`, `done` или `blocked`. Рядом с `done` пишется короткая строка: коммит и команда, которая завершилась с кодом 0. Рядом с `blocked` пишется факт, который остановил шаг. `next_step` — наименьший номер со статусом не `done`.

## Схема, её не менять

Чат — окно того же набора `misc/textui/`. Файловый менеджер остаётся. `misc/editor/` не открывать и не копировать.

Новых ключевых слов нет. `compiler/**` не менять. `spawn` остаётся системным потоком и на реактор не переводится. `block_on` на потоке окна не вызывать: он замораживает интерфейс. Отмена передачи не проектируется. «Stop» только увеличивает поколение, поздний ответ отбрасывается.

Один файл — одна задача, не длиннее 400 строк. Перед правкой файла, который уже длиннее 360 строк, вынести кусок в этом же шаге. `app.mlc` меняется только на шаге 19. Новый `WidgetKind` не добавлять. Многострочное поле — `KindTextField` с `FieldData.max_lines`. Лента — данные и отрисовка, не виджет на реплику.

`TextLayout` сейчас всегда одна строка. `text_layout_index_at` не читает `y`. Прямоугольники выделения стоят в `y: 0`. Вставка в поле режется `first_line`. `draw_text_batch` красит весь текст в `theme.foreground`.

Сеть — уже существующие `https_send_async` и `block_on` на том потоке, где создан реактор. Неблокирующего чтения `Task` в показанном API нет: шаг 16 сначала ищет его и только потом добавляет слот. Стриминга, markdown, IME, вложений и записи истории нет.

Имена — полные английские слова. Проверка шага — его команда, с `TMPDIR=/home/jenya/workspaces/current/mlc/tmp`, если команда пишет во временный каталог.

## Шаг 1. Жёсткие переносы в TextLayout

Статус: `done`. Смоук чата, `bash scripts/run_textui_slice01_smoke.sh` и `ruby scripts/run_textui_file_size.rb` завершились с кодом 0. Литерал `Theme` в `slice01_labels.mlc` дополнен полями, которые уже есть у `theme_default`.

Файлы: `misc/textui/text_layout.mlc`, `misc/textui/test/chat01_text_lines.mlc`, `scripts/run_textui_chat_smoke.sh`.

Сделать: разбить текст по `"\n"` через `byte_at`. На каждый непустой абзац — `text_shaping_shape`, сразу прочитать advance и cluster: следующий вызов затирает слот. Пустой абзац не шейпить. `byte_offset` кластера сдвинуть на начало абзаца, `line_index` заполнить, `x` от начала строки. `TextLine.top = индекс * line_height`. Замыкающий `\n` даёт пустую последнюю строку. Переноса по ширине нет.

Скрипт смоука — как `scripts/run_textui_slice01_smoke.sh`, но прогоняет все `misc/textui/test/chat*.mlc` по имени. Ненулевой код бинаря останавливает скрипт.

Проверка: `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh`, затем `bash scripts/run_textui_slice01_smoke.sh` и `ruby scripts/run_textui_file_size.rb`. Все три кода 0. Тест: `"a\nb\nc"` — 3 строки и высота `3*h`; строка без `\n` совпадает с прежней шириной и кластерами; `"a\n"` — 2 строки.

## Шаг 2. Перенос по ширине

Статус: `done`. `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh` и `ruby scripts/run_textui_file_size.rb` завершились с кодом 0.

Файлы: `misc/textui/text_layout_wrap.mlc`, `misc/textui/text_layout.mlc`, `misc/textui/test/chat02_text_wrap.mlc`.

Сделать: `wrap_line_starts` без импорта `text_layout.mlc`. Жадно, разрыв после пробела, слово шире лимита рвётся по кластеру, на строке минимум один кластер. `WrapWidthNone` не вызывает перенос. Ключ кэша не менять.

Проверка: `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh` и `ruby scripts/run_textui_file_size.rb`, оба кода 0.

## Шаг 3. Попадание и выделение по строкам

Статус: `done`. `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh`, `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_slice01_smoke.sh` и `ruby scripts/run_textui_file_size.rb` завершились с кодом 0.

Файлы: `misc/textui/text_layout_lines.mlc`, `misc/textui/text_layout_hit.mlc`, `misc/textui/test/chat03_hit_lines.mlc`.

Сделать: `text_layout_index_at` читает `y`. Каретка и прямоугольники выделения считаются по строке. Новая точка каретки `{ x, y, height }`. `paint_selection` и `paint_field_selection` не трогать.

Проверка: смоук чата, `bash scripts/run_textui_slice01_smoke.sh`, `ruby scripts/run_textui_file_size.rb`, все коды 0.

## Шаг 4. Построчная отрисовка метки

Статус: `done`. `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh`, `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_slice0_smoke.sh`, `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_slice01_smoke.sh` и `ruby scripts/run_textui_file_size.rb` завершились с кодом 0. Кнопки вынесены в `paint_button.mlc`, потому что `paint_pass.mlc` был длиннее 360 строк.

Файлы: `misc/textui/text_layout_paint.mlc`, `misc/textui/paint_pass.mlc`, `misc/textui/test/chat04_label_lines.mlc`.

Сделать: `text_layout_text_ops` — один `TextOp` на непустую строку, без хвостового `\n`. Ветка `KindLabel` пушит эти ops. Метка по-прежнему меряется с `wrap_width_none()`.

Проверка: смоук чата, `bash scripts/run_textui_slice0_smoke.sh`, `bash scripts/run_textui_slice01_smoke.sh`, `ruby scripts/run_textui_file_size.rb`, все коды 0.

## Шаг 5. Вертикальное движение каретки

Статус: `done`. `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh` и `ruby scripts/run_textui_file_size.rb` завершились с кодом 0.

Файлы: `misc/textui/text_layout_nav.mlc`, `misc/textui/test/chat05_nav.mlc`.

Сделать: `text_layout_move_vertical` пересчитывает колонку из `caret_x` на каждое нажатие. В `UiState` колонка не хранится.

Проверка: смоук чата и `ruby scripts/run_textui_file_size.rb`, оба кода 0.

## Шаг 6. Поле меряется шириной колонки

Статус: `done`. `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh`, `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_slice0_smoke.sh`, `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_slice01_smoke.sh`, `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_slice12_smoke.sh`, `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_slice13_smoke.sh`, `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_slice14_smoke.sh`, `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_slice15_smoke.sh`, `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_slice18_smoke.sh`, `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_slice19_smoke.sh` и `ruby scripts/run_textui_file_size.rb` завершились с кодом 0. Измерение вынесено в `layout_measure.mlc`, типы — в `layout_work.mlc`, потому что `layout_box.mlc` был длиннее 360 строк.

Файлы: `misc/textui/store.mlc`, `misc/textui/widget_field.mlc`, `misc/textui/field_history.mlc`, `misc/textui/layout_width.mlc`, `misc/textui/layout_box.mlc`, `misc/textui/layout_measure.mlc`, `misc/textui/layout_work.mlc`, `misc/textui/layout_pass.mlc`, `misc/textui/overlay.mlc`, `misc/textui/test/chat06_field_layout.mlc`.

Сделать: `FieldData.max_lines`, `field_data_new` ставит 1, `field_data_multiline` для чата. `measure_node_wide` для поля с `max_lines > 1` строит `WrapWidthPixels`. Колонка меряет детей этой функцией. Строка остаётся на старом измерении. Если `layout_box.mlc` переходит 400 строк, вынести измерение в `layout_measure.mlc` в этом же шаге.

Проверка: смоук чата, `slice0`, `slice01`, `slice12`–`slice15`, `slice18`, `slice19` (те скрипты, которые есть), `ruby scripts/run_textui_file_size.rb`. Все коды 0.

## Шаг 7. Отрисовка и вертикальная прокрутка поля

Статус: `done`. `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh`, `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_slice12_smoke.sh`, `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_slice13_smoke.sh` и `ruby scripts/run_textui_file_size.rb` завершились с кодом 0.

Файлы: `misc/textui/widget_field_multi.mlc`, `misc/textui/widget_field.mlc`, `misc/textui/test/chat07_field_paint.mlc`.

Сделать: у многострочного поля существующий `field_scroll_x` — вертикальные пиксели. Режим по `wrap_width`. Новых полей в `UiState` нет. Клип — уже стоящий `OpPushClip`.

Проверка: смоук чата, `slice12`, `slice13`, `ruby scripts/run_textui_file_size.rb`, все коды 0.

## Шаг 8. Клавиши многострочного поля

Статус: `done`. `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh`, `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_slice13_smoke.sh`, `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_slice14_smoke.sh` и `ruby scripts/run_textui_file_size.rb` завершились с кодом 0. Правка поля вынесена в `field_key_edit.mlc`, потому что `update_field_keys.mlc` был длиннее 360 строк.

Файлы: `misc/textui/field_multiline_keys.mlc`, `misc/textui/field_key_edit.mlc`, `misc/textui/update_field_keys.mlc`, `misc/textui/test/chat08_field_keys.mlc`.

Сделать: Shift+Enter вставляет `"\n"`. Enter без Shift по-прежнему отправка. Вставка многострочного текста сохраняет `\n` (`\r\n` и `\r` становятся `\n`). Однострочное поле по-прежнему режется `first_line`.

Проверка: смоук чата, `slice13`, `slice14`, `ruby scripts/run_textui_file_size.rb`, все коды 0.

## Шаг 9. Индекс блоков разной высоты

Статус: `done`. `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh` и `ruby scripts/run_textui_file_size.rb` завершились с кодом 0.

Файлы: `misc/textui/block_index.mlc`, `misc/textui/test/chat09_block_index.mlc`.

Сделать: чистый модуль без импортов из textui. Высоты, двоичный поиск видимого диапазона, зажим прокрутки, признак «у нижнего края».

Проверка: смоук чата и `ruby scripts/run_textui_file_size.rb`, оба кода 0.

## Шаг 10. Цвет текста доходит до экрана

Статус: `done`. `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh`, `ruby scripts/run_textui_file_size.rb` и `ruby scripts/run_text_gl_perf_corpus.rb` завершились с кодом 0.

Файлы: `misc/textui/submit_color.mlc`, `misc/textui/submit_flush.mlc`, `misc/textui/submit.mlc`, `misc/textui/test/chat10_text_color.mlc`.

Сделать: смена `TextOp.color` сбрасывает батч и рисует его этим цветом. `submit.mlc` не растёт: рисование батча уезжает в `submit_flush.mlc`. `Theme` не менять.

Проверка: смоук чата, `ruby scripts/run_textui_file_size.rb`, `ruby scripts/run_text_gl_perf_corpus.rb`. Все коды 0.

## Шаг 11. Модель чата

Статус: `done`. `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh` и `ruby scripts/run_textui_file_size.rb` завершились с кодом 0.

Файлы: `misc/textui/chat_model.mlc`, `misc/textui/test/chat11_model.mlc`.

Сделать: `ChatRole`, `ChatMessage`, `ReplyOutcome`, `ChatModel`. Пустая отправка и вторая отправка при `pending` не принимаются. `chat_apply_reply` игнорирует чужое поколение. `chat_cancel` и `chat_clear` увеличивают поколение. Сообщения с `failed` в запрос не входят.

Проверка: смоук чата и `ruby scripts/run_textui_file_size.rb`, оба кода 0.

## Шаг 12. Лента

Статус: `done`. `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh` и `ruby scripts/run_textui_file_size.rb` завершились с кодом 0.

Файлы: `misc/textui/chat_transcript.mlc`, при переполнении `misc/textui/chat_transcript_layout.mlc`, `misc/textui/text_layout.mlc`, `misc/textui/test/chat12_transcript.mlc`.

Сделать: painter над прямоугольником, не новый вид виджета. Видимые блоки только. Пока `pending`, статический текст `"…"`, без анимации. Кэш раскладки при длине больше 4096 сбрасывается перед вставкой. Фон реплики — существующие цвета темы. Новых полей `Theme` нет.

Проверка: смоук чата и `ruby scripts/run_textui_file_size.rb`, оба кода 0.

## Шаг 13. Ввод ленты

Статус: `done`. `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh` и `ruby scripts/run_textui_file_size.rb` завершились с кодом 0.

Файлы: `misc/textui/chat_transcript_input.mlc`, при переполнении `misc/textui/chat_transcript_hit.mlc`, `misc/textui/test/chat13_transcript_input.mlc`.

Сделать: колесо, полоса, выделение внутри одной реплики. Нажатие в ленте снимается с ввода до `ui_update`, чтобы не сбросить фокус поля. Копирование — срез одной реплики.

Проверка: смоук чата и `ruby scripts/run_textui_file_size.rb`, оба кода 0.

## Шаг 14. Сцена

Статус: `done`. `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh` и `ruby scripts/run_textui_file_size.rb` завершились с кодом 0.

Файлы: `misc/textui/chat_scene.mlc`, `misc/textui/chat_update.mlc`, `misc/textui/test/chat14_scene.mlc`.

Сделать: колонка — заголовок, лента с `grow 1`, поле `max_lines 6`, кнопка Send. Stop виден только при `pending`. Enter и Send вызывают `chat_submit` и очищают поле. Сети нет.

Проверка: смоук чата и `ruby scripts/run_textui_file_size.rb`, оба кода 0.

## Шаг 15. Тело запроса и разбор ответа

Статус: `done`. `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh` и `ruby scripts/run_textui_file_size.rb` завершились с кодом 0.

Файлы: `misc/textui/chat_json.mlc` либо существующий JSON из `lib/mlc/common/stdlib`, `misc/textui/chat_http.mlc`, `misc/textui/test/chat15_http.mlc`, `docs/agent/TRACK_TEXTUI_CHAT.md`.

Сделать: прочитать `https_request.mlc` и поискать JSON в stdlib. Протокол — `POST {base}/chat/completions` без стриминга. Адрес, модель, ключ и необязательный system — из `MLC_CHAT_BASE_URL`, `MLC_CHAT_MODEL`, `MLC_CHAT_API_KEY`, `MLC_CHAT_SYSTEM`. Ключ не печатать. Пустой ключ — `ReplyFailed` без запроса.

Проверка: смоук чата и `ruby scripts/run_textui_file_size.rb`, оба кода 0. Живой API не вызывать.

## Шаг 16. Транспорт

Статус: `pending`.

Файлы: `misc/textui/chat_transport.mlc`, `misc/textui/test/chat16_transport.mlc`, `docs/agent/TRACK_TEXTUI_CHAT.md`. Слот — только если поиск ничего не нашёл.

Сделать: записать в трек, есть ли неблокирующее чтение `Task` или готовый способ вернуть значение из `spawn`. Порядок: чтение `Task` на потоке окна; иначе `spawn`, а `block_on` на рабочем потоке; иначе один слот под мьютексом и `extern`, `compiler/**` не трогать. В полёте один запрос. Тест бьёт в `https://127.0.0.1:9/`, не в публичный API. `transport_start` возвращается быстрее 50 мс.

Проверка: смоук чата и `ruby scripts/run_textui_file_size.rb`, оба кода 0.

## Шаг 17. Сессия

Статус: `pending`.

Файлы: `misc/textui/chat_session.mlc`, `misc/textui/test/chat17_session.mlc`.

Сделать: `dispatch` стартует транспорт, `poll` применяет ответ своего поколения. При `pending` ожидание окна 0.05 с, иначе ждать событие. Stop до ответа оставляет «Остановлено», поздний ответ историю не меняет.

Проверка: смоук чата и `ruby scripts/run_textui_file_size.rb`, оба кода 0. Публичный API не вызывать.

## Шаг 18. Кадр без окна

Статус: `pending`.

Файлы: `misc/textui/chat_world.mlc`, `misc/textui/chat_frame.mlc`, `misc/textui/test/chat18_frame.mlc`.

Сделать: один кадр теми же функциями, что окно. Два холостых кадра: второй не рисует и не пересобирает раскладку. `app.mlc` не трогать.

Проверка: смоук чата и `ruby scripts/run_textui_file_size.rb`, оба кода 0.

## Шаг 19. Окно

Статус: `pending`.

Файлы: `misc/textui/chat_host.mlc`, `misc/textui/app.mlc`, `docs/agent/TRACK_TEXTUI_CHAT.md`. Если `app.mlc` переходит 400 строк — вынести `input_clear_buttons` и `button_caption` в `misc/textui/app_input.mlc` в этом же шаге.

Сделать: `chat_main` повторяет цикл `app.mlc` без логики менеджера. В начале `main` — возврат в `chat_main`, когда задано `MLC_TEXTUI_CHAT`. История на диск не пишется.

Проверка: `TMPDIR=/home/jenya/workspaces/current/mlc/tmp bash scripts/run_textui_chat_smoke.sh`, `ruby scripts/run_textui_file_size.rb`, `bash scripts/run_textui_slice0_smoke.sh`, `bash scripts/run_textui_slice01_smoke.sh`, и остальные `scripts/run_textui_slice*.sh`, которые есть. Все коды 0. Публичный API не вызывать.
