# SCRIPT_VM_QUEUE_200 — шаги 101–200: язык небольших программ и встраиваемый REPL

Состояние: [SCRIPT_VM_QUEUE_200_STATE.md](SCRIPT_VM_QUEUE_200_STATE.md). Этот файл — спецификация. Прогресс нигде больше не хранится.

Очередь 1–100 закрыта. Её файлы `docs/agent/SCRIPT_VM_QUEUE_100.md` и `docs/agent/SCRIPT_VM_QUEUE_STATE.md` не править. Не путать с MIR VM (`compiler/vm/`, `mlcc --run`).

Готовность этой очереди — (А): подмножество, на котором уже пишется небольшая программа, и многострочный REPL. Ядро REPL живёт в `script_vm/` и его же вызывает терминал и C ABI. Живой памяти VM между вводами нет: сессия заново переводит накопленный текст.

Не входит: `match`, лямбды, методы, generics, float в лоуэре, имена полей в рантайме, fibers, инкрементальный GC, shapes, inline caches, JIT, opcode DSL, textui, `mlcc --emit-script`, eval, BigInt. `for` в этой очереди — только `for имя in массив`, синтаксис `..` в лексер не входит и не добавляется.

## Цель

- REPL принимает кусок текста и отвечает: нужно ещё ввода, объявление принято, значение `i32` или строка, диагностика с позицией.
- Незакрытый `do` / `end`, `{` / `}`, одиночная кавычка и блочный комментарий определяются в `script_vm/` до `lower_source`. Парсер `compiler/frontend/` не меняется.
- Строка — объект кучи. Массив, `len`, `break`, `continue` и `for имя in массив` лоуэрятся из уже существующего AST.
- C-хост передаёт буфер в ту же сессию и получает тот же ответ.

## Уже верно, шаг 101 обязан перепроверить

| Факт | Источник |
|------|----------|
| Типы кучи: cell 1, array 2, record 3, closure 4. Строкового типа нет | `script_vm/heap_types.mlc` |
| Опкоды 1–25, последний `SET_UPVAL`. `LENGTH` и `MAKE_STRING` нет | `script_vm/bytecode.mlc` |
| `ExprStr`, `ExprArray`, `ExprIndex`, `ExprFor`, `StmtBreak`, `StmtContinue` парсер уже строит | `compiler/frontend/ast.mlc`, `compiler/frontend/parser/exprs.mlc` |
| Строковый литерал — в одинарных кавычках. Комментарии `//` и `/* */` | `compiler/frontend/lexer.mlc` |
| `for` — `for имя in выражение do ... end`. Одного диапазона `..` нет | `parse_for_expr` |
| `break` / `continue` лоуэр уже отклоняет кодами `unsupported_break` и `unsupported_continue` | `script_vm/lower/lower_statement.mlc` |
| `lower_expression.mlc` 399 строк, `lower_records.mlc` 395, `bytecode.mlc` 388. Расти им нельзя | `wc -l` |
| `mlc_vm_load_module` есть. Функций `mlc_repl_*` нет | `runtime/include/mlc/script_vm/embedding_abi.h` |
| `script_tool` умеет `run` и `compile`. Подкоманды `repl` нет | `script_vm/tools/script_tool.mlc` |
| Записи ссылочные, pin `record_alias_shares_object` | очередь 1–100, S3 |

## Общие правила

### Обозначения

- `STATE` = `docs/agent/SCRIPT_VM_QUEUE_200_STATE.md`
- `PLAN200` = `docs/agent/SCRIPT_VM_QUEUE_200.md`
- `F0`: `compiler/**`, `lib/mlc/**`, `misc/textui/**`, `misc/editor/**`, `docs/PLAN.md`, `docs/agent/SESSION.md`, `docs/agent/CONTINUITY.md`, `docs/agent/SCRIPT_VM_QUEUE_100.md`, `docs/agent/SCRIPT_VM_QUEUE_STATE.md`, любые другие `docs/**` кроме `PLAN200`, `runtime/**` вне файлов, которые карточка явно перечислила, `compiler/out/**`, чужие незакоммиченные правки. Исключение — пункт 18.
- `RUN(name)` = `ruby scripts/run_script_vm_unit.rb script_vm/tests/<name>_unit.mlc`. Успех: exit 0 и строка `[script_vm] <name>_unit ok`.
- `RED(name)` = та же команда, exit ≠ 0. В `last_verify` — первая строка отказа. Красный unit не регистрируется в `scripts/run_script_vm_gate.rb`.
- `GATE(list)` = `ruby scripts/run_script_vm_gate.rb <list>`. Успех: последняя строка `script_vm_gate ok units=<число>`.
- `SIZE` = `ruby scripts/run_script_vm_file_size.rb`. Успех: `script_vm file size: ok`.
- `BENCH(args)` = `ruby scripts/run_script_vm_bench.rb <args>`.

Строки ответа REPL, везде одни и те же:

- `repl answer=incomplete`
- `repl answer=accepted`
- `repl answer=value kind=int32 number=<целое>`
- `repl answer=value kind=string length=<целое>`
- `repl answer=diagnostic code=<код> line=<целое> column=<целое>`

### Дисциплина

1. Один шаг — один ход. Шаг `N` выполняется, только если каждый `depends` имеет коммит `script_vm queue step <d>:`.
2. Red перед green. Red коммитит падающий тест. Провал компиляции годится как red только там, где карточка прямо говорит «нет символа». Иначе первая диагностика теста — поведенческая.
3. Self-host diff и `compiler/build.sh` не запускать. `compiler/out/mlcc` не пересобирать. Нет бинаря — `blocked`.
4. Идентификаторы без сокращений. Комментариев в новом коде нет, кроме списка инвариантов.
5. Скрипты — Ruby `scripts/run_script_vm_*.rb`. Не Python и не новый bash.
6. Каждый `script_vm/**/*.mlc` ≤ 400 строк. `lower_expression.mlc`, `lower_records.mlc` и `bytecode.mlc` не растить: новый код — в новом модуле. Имена модулей не совпадают с `compiler/frontend/**`. Новые модули лоуэра — с префиксом `lower_`.
7. Публичные имена после разбиения сохраняются транзитивным `import { X } from './m'`.
8. Программы с возможным бесконечным циклом — только через Ruby `Timeout` 20 с.
9. Бенч: минимум из 5 запусков одной сборки. «Быстрее» — строго меньше эталона. Повтор один раз, затем `blocked` с числами.
10. Коммиты: `git add` явным списком. Сообщение `script_vm queue step <N>: <заголовок>` (red — с суффиксом ` (red)`). Без `--trailer`, без push. Чужие незакоммиченные правки не трогать. `tmp/` не коммитить.
11. В коммит входят изменённые файлы из `allowed_files`, `STATE`, и `PLAN200`, только если шаг дописывает Decision log.
12. В `PLAN200` дописывается только `## Decision log`: заголовок `### S<номер> — тема`.
13. Файл вне `allowed_files` или из `F0`, который текущий ответ Sonnet не назвал, — `blocked`.
14. Ответ Sonnet, который прямо называет путь и правку, разрешает менять этот файл, включая `compiler/`, `lib/mlc/`, `runtime/`, тесты. Неназванный файл не трогать.

### Протокол состояния

Поля: `current_step`, `status`, `last_result`, `last_verify`, `next_step`, `consult_count`, `consult_response`, `consult_verdict`.

`status`: `ready`, `in_progress`, `consulting`, `consult_ready`, `blocked`, `queue_complete`.

- Обычный ход начинается с `status: in_progress`, остальные поля не менять.
- Gate прошёл: `current_step: N`, `last_result: done step N: <одна строка>`, `last_verify: <команда> => <исход>`, `next_step: N+1`, `status: ready`, `consult_count: 0`, `consult_response` и `consult_verdict` пустые. После шага 200: `status: queue_complete`, `next_step: none`.
- Gate не прошёл: `next_step` и `consult_count` не менять, `status: blocked`, `last_result: blocked step N: <причина>`, `last_verify` — команда и первые строки. Код не коммитить. Sonnet в этом ходу не вызывать.

### Консультация при остановке

Только `ruby /home/jenya/workspaces/current/mlc-support/review_script_vm_queue_consult.rb`. Не субагент Cursor и не другая модель. Скрипт принимает номер шага 1..200 и пути этого файла и `STATE`.

Ход при `blocked` и `consult_count: 0`, либо `consulting` без `kind: grounds`:

1. `status: consulting`. Код не менять.
2. Перезаписать `/home/jenya/workspaces/current/mlc-support/steps/script_vm_queue_consult.md`. Первая строка `step: <N>`. Дальше текст шага, `last_result`, `last_verify`, секция `## Files` — голые пути из `allowed_files`, без префикса `- `.
3. `--dry-run`. Exit 2 или `consult_over_budget`: `consult_count: 1`, `consult_verdict: over_budget`, `status: blocked`, остановиться.
4. Иначе запуск без `--dry-run`. В `consult_response` путь, `consult_count: 1`, `consult_verdict` пусто, `status: consult_ready`. Коммит только `STATE`. Остановиться.

Следующий ход читает ответ.

- `verdict: human` или пустой verdict — `blocked`, `consult_verdict: human`, код не менять. Следующий ход — проверка основания.
- `verdict: retry` или `patch` — один раз выполнить шаг. Названные файлы вне `allowed_files` тоже править. Провал gate: `consult_count` остаётся 1, `consult_verdict: human`, второй совет по шагу не запрашивать.

### Проверка основания остановки

Вход: `blocked`, `consult_count: 1`, verdict не `stop` и не `over_budget`. Либо `consulting` и в consult-файле уже `kind: grounds`.

1. `status: consulting`. Код не менять.
2. Consult-файл: строки `step: <N>` и `kind: grounds`, затем `last_result`, `last_verify`, прежний verdict и цитата правила. `## Files`: этот план, `STATE` и раннер из `last_verify`, если путь существует.
3. `--dry-run`, затем без него. Потолок тот же.
4. Exit 2: `consult_count: 2`, `consult_verdict: over_budget`, остановиться.
5. Успех: `consult_count: 2`, `status: consult_ready`, коммит только `STATE`.

Ответ `consult_step: <N> grounds`:

- `verdict: amend`, либо `stop`/пусто, но совет называет файлы и правку — выполнить правку один раз. Шаг `next_step` не начинать. После правки `consult_count: 0`, поля совета пустые, `status: ready`, `next_step` не менять. Коммит названных файлов и `STATE`. Сообщение `script_vm queue step <N>: правка правила остановки`.
- `verdict: stop` или пустой verdict без правки файлов — `consult_verdict: stop`, `status: blocked`, остановиться.

`over_budget` и `stop` без неназванной ещё правки: API не вызывать, шаг не повторять.

### Шаблон шага

`kind`, `depends`, `goal`, `allowed_files`, `forbidden`, `contract`, `red`, `green`, `gate`, `sabotage`, `performance`, `done_when`, `state_update`.

## Идентичный промпт очереди

```
Ты исполнитель очереди SCRIPT_VM_QUEUE_200 (MLC Script VM, каталог script_vm/, шаги 101–200). Один ход — один шаг. Состояние хранится только в файле.

1. Прочитай docs/agent/SCRIPT_VM_QUEUE_200_STATE.md и docs/agent/SCRIPT_VM_QUEUE_200.md: разделы «Общие правила», «Протокол состояния», «Консультация при остановке» и «Проверка основания остановки», затем целиком шаг с номером next_step.
2. Если status равен queue_complete — ответь одной строкой «очередь завершена» и остановись.
3. Если current_step больше 0, команда `git log --oneline -1 --grep "^script_vm queue step <current_step>:"` обязана найти коммит. Не нашла — status blocked, consult_count не меняй, last_result «нет коммита шага <current_step>», остановись. Sonnet не вызывай.
4. Если status равен blocked и consult_verdict равен over_budget — API не вызывай, шаг не повторяй, остановись. Если status равен blocked и consult_verdict равен stop — API не вызывай. Прочитай consult_response. Если совет прямо называет файлы и правку, выполни её один раз и не жди человека. Файл, который совет не назвал, не трогай. Если такой правки нет — остановись. Если status равен blocked и consult_count равен 1, а consult_verdict не stop — код не правь. Выполни «Проверку основания остановки» и остановись. Если status равен blocked и consult_count больше 1, а ответ не называет ещё не применённую правку файлов — API не вызывай, шаг не повторяй, остановись.
5. Если status равен consulting, или status равен blocked и consult_count равен 0 — код не правь. Если в consult-файле есть `kind: grounds` для этого же next_step, выполни «Проверку основания остановки», иначе «Консультацию при остановке». Sonnet только через `ruby /home/jenya/workspaces/current/mlc-support/review_script_vm_queue_consult.rb`.
6. Если status равен consult_ready — прочитай consult_response. Первая строка `consult_step: <N> grounds`: `verdict: amend` примени один раз. `verdict: stop` или пустой verdict — если совет называет файлы и правку, выполни её как amend; иначе status blocked, consult_verdict stop, остановись. Иначе ищи `verdict:` совета по шагу. human или пустой: status blocked, consult_verdict human, остановись. over_budget: status blocked, consult_verdict over_budget, остановись. retry или patch: один раз выполни шаг next_step по совету. Названные файлы вне allowed_files правь. Неназванные не трогай.
7. Иначе запиши status: in_progress и выполни ровно шаг next_step.
8. Правь только allowed_files. Red до green. Gate запусти точно как записано. Sabotage, если он не none: испорти, дождись красного исхода, верни файл (`git checkout -- <файл>` только когда незакоммичена одна эта порча; иначе обратная правка), снова зелёный gate. Не запускай compiler/build.sh и не пересобирай compiler/out/mlcc.
9. Не редактируй docs/PLAN.md, docs/agent/SESSION.md, docs/agent/CONTINUITY.md, docs/agent/SCRIPT_VM_QUEUE_100.md, docs/agent/SCRIPT_VM_QUEUE_STATE.md, compiler/, lib/mlc/, misc/textui/, misc/editor/, кроме файлов, которые текущий ответ Sonnet прямо назвал. Чужие незакоммиченные правки не трогай.
10. Gate прошёл: обнови STATE по «Протоколу состояния». Закоммить явный список файлов шага и STATE (и PLAN200, если шаг дописывал Decision log). Сообщение `script_vm queue step <N>: <заголовок>` (red — с суффиксом ` (red)`). Без --trailer, без push.
11. Gate не прошёл: next_step и consult_count не меняй, status blocked, код не коммить, Sonnet в этом ходу не вызывай, следующий шаг не начинай.
12. Один шаг за ход. После коммита, консультации или blocked остановись и ответь коротко: номер шага, команда gate или путь consult_response, исход.
```

## Фазы

| Фаза | Шаги | Цель |
|------|------|------|
| 1. Решения | 101–106 | Контракт сессии, баланса, строки, массива, C ABI и бюджета времени |
| 2. Баланс ввода | 107–112 | `do`/`end`, скобки, кавычки и комментарии до вызова лоуэра |
| 3. Сессия | 113–124 | Накопление объявлений, значение выражения, диагностика |
| 4. Терминал и бюджет | 125–128 | `script_tool repl` как клиент ядра, замер повторного перевода |
| 5. Строка в машине | 129–138 | Тип кучи, длина, склейка, сравнение, опкоды |
| 6. Строка в языке и REPL | 139–148 | Литерал, `+`, `len`, ответ REPL |
| 7. Массивы | 149–160 | Литерал, индекс, `len`, ссылочная семантика, REPL |
| 8. break и continue | 161–168 | Внутри `while`, диагностика снаружи, REPL |
| 9. for | 169–176 | `for имя in массив`, пустой массив, `break`, REPL |
| 10. C ABI | 177–188 | Та же сессия из хоста |
| 11. Сквозной gate | 189–191 | Одна сессия со всеми конструкциями, gate очереди |
| 12. Критика и граница | 192–200 | Sabotage, регистрация, список того, что осталось до (Б) и (В) |

## Шаги

### Step 101 — Факты очереди 200 (S101)

- kind: analysis
- depends: нет
- goal: перепроверить таблицу «Уже верно» и записать расхождения
- allowed_files: `PLAN200` (Decision log), `STATE`
- forbidden: F0; правки `script_vm/**`
- contract: кода нет
- red: none
- green: дописать `### S101 — факты`. Для каждой строки таблицы — `CONFIRMED` или `CORRECTED: <как на самом деле>` с файлом и строкой. Отдельно: `wc -l` файлов из таблицы
- gate: `rg -c '^### S101 ' docs/agent/SCRIPT_VM_QUEUE_200.md` → `1`
- sabotage: none
- performance: not a perf step
- done_when: у каждой строки таблицы есть пометка
- state_update: gate и `1`

### Step 102 — Контракт ответов сессии (S102)

- kind: decision
- depends: 101
- goal: заморозить четыре ответа и то, как кусок текста становится программой
- allowed_files: `PLAN200`, `STATE`
- forbidden: F0; `script_vm/**`
- contract: кода нет
- red: none
- green: дописать `### S102 — сессия`. (1) Модуль `script_vm/repl_session.mlc`, функция `repl_session_submit(session, chunk) -> ReplAnswer`. Сессия хранит `accepted_source` и `pending`. (2) Пустой и пробельный кусок дописывается в `pending` и даёт `incomplete`. (3) Кусок, у которого первое слово `fn`, `export` или `type`, — объявление. После баланса он дописывается в `accepted_source`, `pending` очищается, текст заново проходит `lower_source`. Успех — `accepted`. Диагностика — объявление не сохраняется. (4) Иной завершённый кусок — выражение. Программа = `accepted_source` плюс `export fn repl_entry() -> i32 = do` + кусок + `end`. Вызов `repl_entry` без аргументов. Успех i32 — `value kind=int32`. Строка — отдельное правило шагов 143–144, до них строковый результат есть диагностика `repl_result_not_integer`. (5) Пять печатных строк из раздела «Обозначения». (6) Между вводами куча не хранится
- gate: `rg -c '^### S102 ' docs/agent/SCRIPT_VM_QUEUE_200.md` → `1`
- sabotage: none
- performance: not a perf step
- done_when: S102 содержит пункты (1)–(6)
- state_update: gate и `1`

### Step 103 — Контракт баланса (S103)

- kind: decision
- depends: 102
- goal: заморозить проверку незавершённости без правки парсера
- allowed_files: `PLAN200`, `STATE`
- forbidden: F0; `script_vm/**`
- contract: `compiler/**` не правится
- red: none
- green: дописать `### S103 — баланс`. Модуль `script_vm/repl_balance.mlc`, функция `repl_balance_incomplete(text) -> bool`. Сканер символов, не токены frontend. Пропускает `//` до конца строки и `/* */`. Одинарная кавычка начинает строку; `\'` внутри не закрывает её. Незакрытая кавычка и незакрытый `/*` — incomplete. Слова `do` и `end` считаются только как отдельные слова: следующий и предыдущий символ не буква, цифра и не `_`. `doing` не считается. `{` и `}` считаются вне строк и комментариев. Незакрытая круглая скобка балансом не ловится: такой текст завершён и дальше даёт диагностику лоуэра. `repl_balance_incomplete` истинно, когда счётчик `do` больше `end`, либо скобок `{` больше `}`, либо кавычка или блочный комментарий не закрыты
- gate: `rg -c '^### S103 ' docs/agent/SCRIPT_VM_QUEUE_200.md` → `1`
- sabotage: none
- performance: not a perf step
- done_when: S103 описывает кавычки, оба комментария, слова `do`/`end` и скобки
- state_update: gate и `1`

### Step 104 — Контракт строки и массива (S104)

- kind: decision
- depends: 101
- goal: заморозить представление, не меняя формат blob MLSC v1
- allowed_files: `PLAN200`, `STATE`
- forbidden: F0; `script_vm/**`
- contract: номер версии blob остаётся 1. Новые опкоды — 26 и 27
- red: none
- green: дописать `### S104 — строка и массив`. (1) `heap_type_string() = 5`. Элементы слота — `Int32` 0..255, по одному на байт, дочернего объекта нет. (2) Модуль `script_vm/string_heap.mlc`: `string_heap_from_bytes`, `string_heap_length`, `string_heap_byte_at`, `string_heap_equal`, `string_heap_concatenate`. (3) Опкоды в новом `script_vm/bytecode_text.mlc`: `instruction_opcode_length() = 26`, `instruction_opcode_make_string() = 27`. `bytecode.mlc` их только импортирует, если без этого не компилируется существующий диспетчер, и тогда шаг, который это сделает, сначала выносит диспетчер, а не дописывает 388 строк. (4) `LENGTH`: A = длина массива или строки в B, иначе `RunErr("type")`. (5) `MAKE_STRING`: A = строка из массива целых в B; байт вне 0..255 — `RunErr("bounds")`. (6) `ADD` двух строк склеивает; `EQ`/`NE` двух строк сравнивают байты. Два других heap-объекта `EQ` не начинают сравнивать по содержимому. (7) Лоуэр строки — `script_vm/lower/lower_text.mlc`. Литерал `'ab'` — массив байтов и `MAKE_STRING`. `len(expr)` — встроенное имя, не метод, опкод `LENGTH`. (8) Лоуэр массива — `script_vm/lower/lower_array.mlc`: `ExprArray`, `ExprIndex`, присваивание индекса. Элементы этой очереди — `i32` и `bool`. (9) Массив ссылочный, как запись. (10) `break`/`continue` — `script_vm/lower/lower_iteration.mlc`. Коды `break_outside_loop` и `continue_outside_loop`. `for имя in массив` снижает к индексу и `while`
- gate: `rg -c '^### S104 ' docs/agent/SCRIPT_VM_QUEUE_200.md` → `1`
- sabotage: none
- performance: not a perf step
- done_when: S104 содержит пункты (1)–(10)
- state_update: gate и `1`

### Step 105 — Контракт C ABI сессии (S105)

- kind: decision
- depends: 102
- goal: заморозить функции хоста рядом с существующим embedding ABI
- allowed_files: `PLAN200`, `STATE`
- forbidden: F0; `script_vm/**`
- contract: каталога `script_vm/c_abi/` нет. Старые `mlc_vm_*` не меняют сигнатуру
- red: none
- green: дописать `### S105 — C ABI REPL`. Объявления в `runtime/include/mlc/script_vm/embedding_abi.h`, реализация в `runtime/src/script_vm/embedding_abi.cpp`, логика в `script_vm/repl_embed.mlc`. `MlcRepl` — отдельный объект, не поле `MlcVm`. `mlc_repl_create(const MlcVmConfig*)`, `mlc_repl_destroy`, `mlc_repl_submit(MlcRepl*, const char* text, size_t size, MlcReplAnswer* answer)`. `MlcReplAnswer`: `status` (`MLC_REPL_INCOMPLETE` 0, `MLC_REPL_ACCEPTED` 1, `MLC_REPL_VALUE` 2, `MLC_REPL_DIAGNOSTIC` 3), `value` int32, `string_length` int32 (−1, если значение не строка), `line`, `column`, `code_text[64]`. Код диагностики, который не влезает в 63 символа, обрезается. `mlc_repl_submit` на `NULL` сессии или `NULL` ответе возвращает −1 и не вызывает panic. Повторный `mlc_repl_destroy` одной сессии пишет в panic callback текст с `double release` и не вызывает `abort`. Хост-тест: `script_vm/tests/repl_host.cpp` и `scripts/run_script_vm_repl_host.rb` по образцу `scripts/run_script_vm_lowered_host.rb`
- gate: `rg -c '^### S105 ' docs/agent/SCRIPT_VM_QUEUE_200.md` → `1`
- sabotage: none
- performance: not a perf step
- done_when: S105 называет три файла реализации и поля `MlcReplAnswer`
- state_update: gate и `1`

### Step 106 — Бюджет повторного перевода (S106)

- kind: decision
- depends: 102
- goal: назначить предел времени сессии внутри уже запущенного процесса
- allowed_files: `PLAN200`, `STATE`
- forbidden: F0; `script_vm/**`
- contract: в бюджет не входит старт процесса. 101–128 мс команды `script_tool run` — это другой замер
- red: none
- green: дописать `### S106 — бюджет`. Сценарий: 20 объявлений `export fn function_<n>() -> i32 = <n>` отдельными submit, затем один submit выражения `function_20()`. Время от первого submit до ответа на выражение, внутри одного процесса, минимум из 5. Предел `repl_submit_us` ≤ 500000. Превышение на шаге 128 — `blocked` с числами, самовольная смена предела запрещена
- gate: `rg -c '^### S106 ' docs/agent/SCRIPT_VM_QUEUE_200.md` → `1`
- sabotage: none
- performance: not a perf step
- done_when: S106 содержит сценарий и предел 500000
- state_update: gate и `1`

### Step 107 — Red: незакрытый do

- kind: red
- depends: 103
- goal: падающий unit баланса
- allowed_files: `script_vm/tests/repl_balance_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/repl_balance.mlc`
- contract: unit вызывает `repl_balance_incomplete`. Текст `fn value() -> i32 = do` даёт true. Текст `fn value() -> i32 = do 1 end` даёт false. Печатает `[script_vm] repl_balance_unit ok` только если оба исхода верны
- red: `RED(repl_balance)` падает, потому что модуля `repl_balance` нет. Это названный отказ «нет символа»
- green: none
- gate: `RED(repl_balance)`
- sabotage: none
- performance: not a perf step
- done_when: тест закоммичен и не зарегистрирован в gate
- state_update: первая строка отказа

### Step 108 — Green: баланс do/end

- kind: green
- depends: 107
- goal: реализовать правило S103 для `do`/`end`
- allowed_files: `script_vm/repl_balance.mlc`, `scripts/run_script_vm_gate.rb`, `STATE`
- forbidden: F0; `compiler/**`
- contract: кавычки и комментарии можно пока считать обычным текстом, если тесты 109 ещё не существуют. `do`/`end` — отдельные слова
- red: none
- green: `RUN(repl_balance)` и регистрация `repl_balance` в gate
- gate: `RUN(repl_balance)`; `GATE(repl_balance)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: обе команды зелёные, файл ≤ 400 строк
- state_update: `unit ok repl_balance`

### Step 109 — Red: do внутри кавычек и комментария

- kind: red
- depends: 108
- goal: баланс не считает `do` внутри строки и `//`
- allowed_files: `script_vm/tests/repl_balance_unit.mlc`, `STATE`
- forbidden: F0
- contract: дописать случаи, не ломая прежние. `'do'` при равных `do`/`end` снаружи — complete. Строка `// do` без настоящего `do` — complete. `fn value() -> i32 = do // end` — incomplete
- red: `RED(repl_balance)`, первая неверная проверка — поведенческая
- green: none
- gate: `RED(repl_balance)`
- sabotage: none
- performance: not a perf step
- done_when: красный исход из новой проверки
- state_update: имя упавшей проверки

### Step 110 — Green: кавычки и комментарии в балансе

- kind: green
- depends: 109
- goal: довести сканер до S103 в части кавычек и `//`
- allowed_files: `script_vm/repl_balance.mlc`, `STATE`
- forbidden: F0; `compiler/**`
- contract: `\'` не закрывает строку
- red: none
- green: `RUN(repl_balance)`
- gate: `RUN(repl_balance)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: unit зелёный
- state_update: `unit ok repl_balance`

### Step 111 — Red: незакрытая кавычка, блочный комментарий и слово doing

- kind: red
- depends: 110
- goal: остальные края S103
- allowed_files: `script_vm/tests/repl_balance_unit.mlc`, `STATE`
- forbidden: F0
- contract: незакрытая `'` — incomplete. Незакрытый `/*` — incomplete. `/* do */` без внешнего `do` — complete. `doing` — не `do`. `{` без `}` — incomplete. `(` без `)` при равных `do`/`end` и скобках — complete
- red: `RED(repl_balance)`, поведенческий отказ
- green: none
- gate: `RED(repl_balance)`
- sabotage: none
- performance: not a perf step
- done_when: красный исход из нового случая
- state_update: имя упавшей проверки

### Step 112 — Green: полный баланс S103

- kind: green
- depends: 111
- goal: закрыть края шага 111
- allowed_files: `script_vm/repl_balance.mlc`, `STATE`
- forbidden: F0
- contract: файл остаётся ≤ 400. Если не влезает — вынести сканер строки в `script_vm/repl_balance_text.mlc` в этом же шаге
- red: none
- green: `RUN(repl_balance)`
- gate: `RUN(repl_balance)`; `GATE(repl_balance)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: unit зелёный
- state_update: `unit ok repl_balance`

### Step 113 — Red: пустой submit

- kind: red
- depends: 102, 112
- goal: сессии ещё нет, тест уже описывает пустой ввод
- allowed_files: `script_vm/tests/repl_session_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/repl_session.mlc`
- contract: пустой submit печатает `repl answer=incomplete` и `[script_vm] repl_session_unit ok`
- red: `RED(repl_session)` из-за отсутствия `repl_session_submit`. Это «нет символа»
- green: none
- gate: `RED(repl_session)`
- sabotage: none
- performance: not a perf step
- done_when: тест не зарегистрирован
- state_update: первая строка отказа

### Step 114 — Green: пустая сессия

- kind: green
- depends: 113
- goal: создать сессию и ответ на пустой ввод
- allowed_files: `script_vm/repl_session.mlc`, `script_vm/repl_answer.mlc`, `scripts/run_script_vm_gate.rb`, `STATE`
- forbidden: F0
- contract: `repl_answer.mlc` содержит тип ответа и печать пяти строк. `repl_session.mlc` вызывает `repl_balance_incomplete`
- red: none
- green: `RUN(repl_session)` и регистрация
- gate: `RUN(repl_session)`; `GATE(repl_session)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: unit зелёный
- state_update: `unit ok repl_session`

### Step 115 — Red: объявление принимается

- kind: red
- depends: 114
- goal: завершённая функция даёт accepted
- allowed_files: `script_vm/tests/repl_session_unit.mlc`, `STATE`
- forbidden: F0
- contract: submit `export fn identity(number: i32) -> i32 = number` печатает `repl answer=accepted`. Прежняя проверка пустого ввода остаётся
- red: `RED(repl_session)`, поведенческий отказ, не отсутствие символа
- green: none
- gate: `RED(repl_session)`
- sabotage: none
- performance: not a perf step
- done_when: отказ на accepted
- state_update: строка, которую напечатал тест вместо accepted

### Step 116 — Green: приём объявления

- kind: green
- depends: 115
- goal: сохранять принятый текст и переводить его через `lower_source`
- allowed_files: `script_vm/repl_session.mlc`, `script_vm/repl_answer.mlc`, `STATE`
- forbidden: F0; `compiler/**`
- contract: при диагностике лоуэра текст не остаётся в `accepted_source`
- red: none
- green: `RUN(repl_session)`
- gate: `RUN(repl_session)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: unit зелёный
- state_update: `unit ok repl_session`

### Step 117 — Red: второе объявление видит первое

- kind: red
- depends: 116
- goal: выражение вызывает уже принятую функцию
- allowed_files: `script_vm/tests/repl_session_unit.mlc`, `STATE`
- forbidden: F0
- contract: после accepted функции `identity` submit `identity(4)` печатает `repl answer=value kind=int32 number=4`
- red: `RED(repl_session)`, поведенческий
- green: none
- gate: `RED(repl_session)`
- sabotage: none
- performance: not a perf step
- done_when: отказ на number=4
- state_update: фактическая строка ответа

### Step 118 — Green: вызов принятой функции

- kind: green
- depends: 117
- goal: собирать программу из принятого текста и обёртки `repl_entry`
- allowed_files: `script_vm/repl_session.mlc`, `STATE`
- forbidden: F0
- contract: обёртка не записывается в `accepted_source`. Вызов через `emb_call_export_bytes`
- red: none
- green: `RUN(repl_session)`
- gate: `RUN(repl_session)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: number=4
- state_update: `unit ok repl_session`

### Step 119 — Red: плохое объявление не остаётся

- kind: red
- depends: 118
- goal: диагностика не портит сессию
- allowed_files: `script_vm/tests/repl_session_unit.mlc`, `STATE`
- forbidden: F0
- contract: submit `export fn broken( -> i32 = 1` даёт diagnostic. Следующий submit годной функции снова даёт accepted, и вызов прежней `identity(4)` по-прежнему 4
- red: `RED(repl_session)`, поведенческий
- green: none
- gate: `RED(repl_session)`
- sabotage: none
- performance: not a perf step
- done_when: отказ показывает, что битый текст сохранился или потерялась старая функция
- state_update: фактические строки

### Step 120 — Green: откат битого объявления

- kind: green
- depends: 119
- goal: не принимать текст с диагностикой
- allowed_files: `script_vm/repl_session.mlc`, `STATE`
- forbidden: F0
- contract: `pending` после диагностики тоже пуст
- red: none
- green: `RUN(repl_session)`
- gate: `RUN(repl_session)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: unit зелёный
- state_update: `unit ok repl_session`

### Step 121 — Red: выражение и не-i32

- kind: red
- depends: 120
- goal: голое выражение считается, `true` не роняет процесс
- allowed_files: `script_vm/tests/repl_session_unit.mlc`, `STATE`
- forbidden: F0
- contract: `1 + 2` → `number=3`. `true` → `repl answer=diagnostic code=repl_result_not_integer` либо уже существующий код лоуэра, но процесс жив и следующая команда `1` даёт `number=1`
- red: `RED(repl_session)`, поведенческий
- green: none
- gate: `RED(repl_session)`
- sabotage: none
- performance: not a perf step
- done_when: отказ на `1 + 2` или на жизнь процесса после `true`
- state_update: фактическая строка

### Step 122 — Green: выражение

- kind: green
- depends: 121
- goal: обёртка `do ... end` для куска, который не объявление
- allowed_files: `script_vm/repl_session.mlc`, `script_vm/repl_answer.mlc`, `STATE`
- forbidden: F0
- contract: многострочный кусок копится в `pending`, пока баланс не завершён, и только потом переводится
- red: none
- green: `RUN(repl_session)`
- gate: `RUN(repl_session)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: number=3 и процесс жив после `true`
- state_update: `unit ok repl_session`

### Step 123 — Red: позиция диагностики

- kind: red
- depends: 122
- goal: неизвестное имя сообщает строку и колонку
- allowed_files: `script_vm/tests/repl_session_unit.mlc`, `STATE`
- forbidden: F0
- contract: submit `missing_name` печатает diagnostic с `code=unknown_identifier`, `line` и `column` больше 0
- red: `RED(repl_session)`, поведенческий
- green: none
- gate: `RED(repl_session)`
- sabotage: none
- performance: not a perf step
- done_when: отказ на код или на нулевую позицию
- state_update: фактическая строка

### Step 124 — Green: проброс диагностики лоуэра

- kind: green
- depends: 123
- goal: поля `LowerDiagnostic` попадают в строку ответа
- allowed_files: `script_vm/repl_session.mlc`, `script_vm/repl_answer.mlc`, `STATE`
- forbidden: F0
- contract: берётся первая диагностика
- red: none
- green: `RUN(repl_session)`
- gate: `RUN(repl_session)`; `GATE(repl_session)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: unit зелёный
- state_update: `unit ok repl_session`

### Step 125 — Red: подкоманда repl

- kind: red
- depends: 124
- goal: раннер терминального клиента падает, потому что подкоманды нет
- allowed_files: `scripts/run_script_vm_repl_cli.rb`, `STATE`
- forbidden: F0; `script_vm/tools/script_tool.mlc`
- contract: раннер собирает `script_tool` так же, как `scripts/run_script_vm_lower_tool.rb`, и пишет в stdin три строки: объявление `identity`, затем `identity(4)`, затем конец файла. Успех будущего green: stdout содержит `repl answer=accepted` и `repl answer=value kind=int32 number=4`, exit 0. Сейчас подкоманда неизвестна, exit ≠ 0
- red: `ruby scripts/run_script_vm_repl_cli.rb` → exit ≠ 0
- green: none
- gate: эта команда
- sabotage: none
- performance: not a perf step
- done_when: раннер закоммичен и падает
- state_update: код выхода

### Step 126 — Green: script_tool repl

- kind: green
- depends: 125
- goal: терминал читает строки и вызывает то же ядро
- allowed_files: `script_vm/tools/script_repl.mlc`, `script_vm/tools/script_tool.mlc`, `STATE`
- forbidden: F0
- contract: `script_tool.mlc` только распознаёт слово `repl` и вызывает функцию из `script_repl.mlc`. Незакрытый кусок не печатает diagnostic, а ждёт следующую строку. EOF посреди incomplete — exit 1 и `repl answer=diagnostic code=repl_incomplete_eof line=1 column=1`
- red: none
- green: раннер шага 125 зелёный
- gate: `ruby scripts/run_script_vm_repl_cli.rb` → exit 0 и обе строки ответа; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: number=4 из терминала
- state_update: две строки ответа

### Step 127 — Замер сессии (S127)

- kind: measure
- depends: 106, 126
- goal: измерить сценарий S106
- allowed_files: `script_vm/tests/repl_budget_unit.mlc`, `scripts/run_script_vm_repl_budget.rb`, `STATE`
- forbidden: F0; не менять предел
- contract: unit печатает `repl_submit_us=<целое>` и `[script_vm] repl_budget_unit ok`, если ответы сценария верны (`accepted` двадцать раз и `number=20`). Раннер: одна сборка, 5 запусков, печатает минимум `repl_submit_us`
- red: none
- green: раннер exit 0
- gate: `ruby scripts/run_script_vm_repl_budget.rb` → exit 0 и строка `repl_submit_us=`
- sabotage: none
- performance: записать минимум. Сравнение с 500000 делает шаг 128
- done_when: число напечатано
- state_update: минимум `repl_submit_us`

### Step 128 — Сверка с бюджетом S106

- kind: measure
- depends: 127
- goal: принять замер или остановить очередь
- allowed_files: `PLAN200`, `scripts/run_script_vm_repl_budget.rb`, `STATE`
- forbidden: F0; не поднимать предел, чтобы тест стал зелёным
- contract: раннер получает `--max-absolute repl_submit_us=500000` и завершается 0 только при выполнении предела. В Decision log `### S128 — бюджет`: минимум и вердикт `within_budget` или шаг `blocked`
- red: none
- green: предел выполнен, S128 дописан
- gate: `ruby scripts/run_script_vm_repl_budget.rb --max-absolute repl_submit_us=500000` → exit 0
- sabotage: none
- performance: превышение — `blocked`, код бюджета не коммитить как успех
- done_when: S128 содержит число и `within_budget`
- state_update: число и вердикт

### Step 129 — Red: тип строки в куче

- kind: red
- depends: 104
- goal: тестам нужен `string_heap_from_bytes`, символа нет
- allowed_files: `script_vm/tests/string_heap_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/string_heap.mlc`
- contract: unit строит кучу, кладёт байты 104 и 105 (`hi`), читает длину 2 и байт 0. Печатает `[script_vm] string_heap_unit ok`
- red: `RED(string_heap)` — нет символа
- green: none
- gate: `RED(string_heap)`
- sabotage: none
- performance: not a perf step
- done_when: отказ «нет символа»
- state_update: первая строка

### Step 130 — Green: выделение строки

- kind: green
- depends: 129
- goal: heap type 5 и чтение байтов
- allowed_files: `script_vm/string_heap.mlc`, `script_vm/heap_types.mlc`, `scripts/run_script_vm_gate.rb`, `STATE`
- forbidden: F0
- contract: `heap_type_string` возвращает 5. Существующие типы 1–4 не меняются
- red: none
- green: `RUN(string_heap)` и регистрация
- gate: `RUN(string_heap)`; `GATE(string_heap heap_gc_arena)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: unit зелёный и старый heap-тест зелёный
- state_update: `unit ok string_heap`

### Step 131 — Red: длина, равенство, склейка

- kind: red
- depends: 130
- goal: операции строки ещё не полны
- allowed_files: `script_vm/tests/string_heap_unit.mlc`, `STATE`
- forbidden: F0
- contract: склейка `h`+`i` даёт длину 2 и байты 104, 105. Разные байты не равны. Пустая строка имеет длину 0
- red: `RED(string_heap)`, поведенческий, если функций ещё нет — карточка разрешает «нет символа» только для `string_heap_equal` и `string_heap_concatenate`
- green: none
- gate: `RED(string_heap)`
- sabotage: none
- performance: not a perf step
- done_when: красный исход
- state_update: первая строка

### Step 132 — Green: операции строки

- kind: green
- depends: 131
- goal: равенство по байтам и склейка новым объектом
- allowed_files: `script_vm/string_heap.mlc`, `STATE`
- forbidden: F0
- contract: склейка не портит исходные объекты
- red: none
- green: `RUN(string_heap)`
- gate: `RUN(string_heap)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: unit зелёный
- state_update: `unit ok string_heap`

### Step 133 — Red: опкод LENGTH на массиве

- kind: red
- depends: 104, 132
- goal: рукописный байткод просит длину массива из трёх элементов и получает 3
- allowed_files: `script_vm/tests/string_opcode_unit.mlc`, `STATE`
- forbidden: F0
- contract: программа через существующий исполнитель. Ожидание `length=3`. Сейчас опкода нет, исход — ошибка неподдерживаемого опкода либо нет символа `instruction_opcode_length`
- red: `RED(string_opcode)`. Допустимы поведенческий отказ и «нет символа»
- green: none
- gate: `RED(string_opcode)`
- sabotage: none
- performance: not a perf step
- done_when: тест не зарегистрирован
- state_update: первая строка

### Step 134 — Green: LENGTH

- kind: green
- depends: 133
- goal: опкод 26 для массива
- allowed_files: `script_vm/bytecode_text.mlc`, `script_vm/executor_text.mlc`, `script_vm/executor.mlc`, `script_vm/verifier.mlc`, `scripts/run_script_vm_gate.rb`, `STATE`
- forbidden: F0; не раздувать `bytecode.mlc` и `executor.mlc` сверх 400. Если диспетчер не влезает — вынести ветку в `executor_text.mlc` в этом же шаге
- contract: неверный тип — `RunErr("type")`. Верификатор принимает опкод 26
- red: none
- green: `RUN(string_opcode)` и регистрация
- gate: `RUN(string_opcode)`; `GATE(string_opcode arrays_records)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: length=3
- state_update: `unit ok string_opcode`

### Step 135 — Red: MAKE_STRING

- kind: red
- depends: 134
- goal: массив байтов становится строкой длины 2
- allowed_files: `script_vm/tests/string_opcode_unit.mlc`, `STATE`
- forbidden: F0
- contract: байты 104, 105. Байт 256 — `bounds`. Ожидание печатает `make_string length=2`
- red: `RED(string_opcode)`, поведенческий или нет символа `instruction_opcode_make_string`
- green: none
- gate: `RED(string_opcode)`
- sabotage: none
- performance: not a perf step
- done_when: красный исход
- state_update: первая строка

### Step 136 — Green: MAKE_STRING

- kind: green
- depends: 135
- goal: опкод 27
- allowed_files: `script_vm/bytecode_text.mlc`, `script_vm/executor_text.mlc`, `script_vm/verifier.mlc`, `STATE`
- forbidden: F0
- contract: исходный массив остаётся. Новая строка — другой object id
- red: none
- green: `RUN(string_opcode)`
- gate: `RUN(string_opcode)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: length=2 и bounds на 256
- state_update: `unit ok string_opcode`

### Step 137 — Red: склейка и сравнение в исполнителе

- kind: red
- depends: 136
- goal: ADD двух строк и EQ по байтам
- allowed_files: `script_vm/tests/string_opcode_unit.mlc`, `STATE`
- forbidden: F0
- contract: `'h'+'i'` через опкоды даёт байты 104, 105. Две строки из одинаковых байтов: EQ пишет Bool true. Два массива по-прежнему не сравниваются как строки
- red: `RED(string_opcode)`, поведенческий
- green: none
- gate: `RED(string_opcode)`
- sabotage: none
- performance: not a perf step
- done_when: красный исход
- state_update: первая строка

### Step 138 — Green: ADD и EQ строк

- kind: green
- depends: 137
- goal: диспетчер двух строк в существующих ADD и EQ
- allowed_files: `script_vm/executor_text.mlc`, `script_vm/executor.mlc`, `script_vm/arith.mlc`, `STATE`
- forbidden: F0
- contract: Int32 ADD не меняет прежнее переполнение. Сборка мусора не уничтожает байты строки, на которую есть корень. Если файл переходит 400 строк — вынести ветку в этом же шаге
- red: none
- green: `RUN(string_opcode)`; `RUN(int32_div_overflow)`
- gate: `GATE(string_opcode int32_div_overflow)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: оба unit зелёные
- state_update: `script_vm_gate ok`

### Step 139 — Red: литерал строки в лоуэре

- kind: red
- depends: 138
- goal: `'hi'` ещё не лоуэрится
- allowed_files: `script_vm/tests/lower_text_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/lower/**`
- contract: `export fn run() -> i32 = len('hi')` через `lower_source` и вызов даёт 2. Сейчас диагностика `unsupported_expression` или иная, unit ждёт `text_length=2` и поэтому красный
- red: `RED(lower_text)`, поведенческий
- green: none
- gate: `RED(lower_text)`
- sabotage: none
- performance: not a perf step
- done_when: тест не зарегистрирован
- state_update: фактическая диагностика

### Step 140 — Green: литерал строки

- kind: green
- depends: 139
- goal: `ExprStr` в новом модуле
- allowed_files: `script_vm/lower/lower_text.mlc`, `script_vm/lower/lower_expression.mlc`, `scripts/run_script_vm_gate.rb`, `STATE`
- forbidden: F0; `lower_expression.mlc` не переходит 400 строк. Вызов — одна строка диспетчеризации; тело литерала живёт в `lower_text.mlc`. Если одной строки мало и файл уже 399 — вынести существующий кусок в новый модуль в этом же шаге и только потом добавить вызов
- contract: байты литерала — коды символов исходника
- red: none
- green: `RUN(lower_text)` и регистрация
- gate: `RUN(lower_text)`; `GATE(lower_text lower_arithmetic)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: text_length=2
- state_update: `unit ok lower_text`

### Step 141 — Red: склейка и len в исходнике

- kind: red
- depends: 140
- goal: `'ab' + 'c'` и `len`
- allowed_files: `script_vm/tests/lower_text_unit.mlc`, `STATE`
- forbidden: F0
- contract: функция возвращает `len('ab' + 'c')` = 3
- red: `RED(lower_text)`, поведенческий
- green: none
- gate: `RED(lower_text)`
- sabotage: none
- performance: not a perf step
- done_when: красный исход
- state_update: фактическое значение или диагностика

### Step 142 — Green: + и len для строк

- kind: green
- depends: 141
- goal: лоуэр `+` двух строк и встроенного `len`
- allowed_files: `script_vm/lower/lower_text.mlc`, `script_vm/lower/lower_expression.mlc`, `STATE`
- forbidden: F0; предел 400
- contract: `len` от i32 — `type_mismatch`. Метод `.length` не вводится
- red: none
- green: `RUN(lower_text)`
- gate: `RUN(lower_text)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: length=3
- state_update: `unit ok lower_text`

### Step 143 — Red: REPL возвращает длину строки

- kind: red
- depends: 142, 124
- goal: сессия умеет показать строковый результат
- allowed_files: `script_vm/tests/repl_session_unit.mlc`, `STATE`
- forbidden: F0
- contract: submit `len('hi')` печатает `repl answer=value kind=int32 number=2`. Отдельный submit `'hi'` печатает `repl answer=value kind=string length=2`
- red: `RED(repl_session)`, поведенческий
- green: none
- gate: `RED(repl_session)`
- sabotage: none
- performance: not a perf step
- done_when: красный исход
- state_update: фактическая строка

### Step 144 — Green: строковый ответ REPL

- kind: green
- depends: 143
- goal: снять запрет `repl_result_not_integer` для строки
- allowed_files: `script_vm/repl_session.mlc`, `script_vm/repl_answer.mlc`, `STATE`
- forbidden: F0
- contract: i32-ответы прежних проверок не меняются. `true` по-прежнему diagnostic
- red: none
- green: `RUN(repl_session)`
- gate: `RUN(repl_session)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: length=2 и number=2
- state_update: `unit ok repl_session`

### Step 145 — Red: строка плюс число

- kind: red
- depends: 144
- goal: смешение типов — диагностика, не падение
- allowed_files: `script_vm/tests/lower_text_unit.mlc`, `STATE`
- forbidden: F0
- contract: `'a' + 1` даёт `type_mismatch` и не исполняется
- red: `RED(lower_text)`, поведенческий
- green: none
- gate: `RED(lower_text)`
- sabotage: none
- performance: not a perf step
- done_when: красный исход
- state_update: фактическая диагностика

### Step 146 — Green: запрет смешения string и i32

- kind: green
- depends: 145
- goal: диагностика до исполнения
- allowed_files: `script_vm/lower/lower_text.mlc`, `STATE`
- forbidden: F0
- contract: две строки по-прежнему склеиваются
- red: none
- green: `RUN(lower_text)`
- gate: `RUN(lower_text)`; `GATE(lower_text)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: unit зелёный
- state_update: `unit ok lower_text`

### Step 147 — Строки не ломают бенч

- kind: measure
- depends: 146
- goal: B1 и B3 остаются в пределах baseline:50
- allowed_files: `STATE`
- forbidden: F0; не переписывать `bench_baseline.txt` и `bench_current.txt`
- contract: только прогон
- red: none
- green: none
- gate: `BENCH(--max-ratio B1.ns_per_insn=baseline:50 --max-ratio B3.ns_per_call=baseline:50)` → exit 0
- sabotage: none
- performance: записать напечатанные B1 и B3 в `last_verify`
- done_when: exit 0
- state_update: B1 и B3

### Step 148 — Critic: EQ строк сравнивает байты

- kind: critic
- depends: 147
- goal: подмена сравнения строк на сравнение object id должна краснить `string_opcode`
- allowed_files: `STATE`
- forbidden: F0; порча не коммитится
- contract: в `string_heap_equal` временно вернуть сравнение идентификаторов. `RUN(string_opcode)` красный. Вернуть функцию. Снова зелёный
- red: none
- green: none
- gate: красный прогон, затем `RUN(string_opcode)` exit 0
- sabotage: сравнение идентификаторов
- performance: not a perf step
- done_when: после отката unit зелёный и diff файла пуст
- state_update: имя красной проверки и факт отката

### Step 149 — Red: литерал массива

- kind: red
- depends: 104, 134
- goal: `[1, 2, 3]` ещё не лоуэрится
- allowed_files: `script_vm/tests/lower_array_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/lower/**`
- contract: `export fn run() -> i32 = len([1, 2, 3])` ожидается 3
- red: `RED(lower_array)`, поведенческий
- green: none
- gate: `RED(lower_array)`
- sabotage: none
- performance: not a perf step
- done_when: тест не зарегистрирован
- state_update: диагностика

### Step 150 — Green: литерал массива и len

- kind: green
- depends: 149
- goal: `ExprArray` целых чисел
- allowed_files: `script_vm/lower/lower_array.mlc`, `script_vm/lower/lower_expression.mlc`, `scripts/run_script_vm_gate.rb`, `STATE`
- forbidden: F0; `lower_expression.mlc` ≤ 400
- contract: пустой массив в этом шаге можно ещё не принимать
- red: none
- green: `RUN(lower_array)` и регистрация
- gate: `RUN(lower_array)`; `GATE(lower_array arrays_records)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: length=3
- state_update: `unit ok lower_array`

### Step 151 — Red: чтение индекса

- kind: red
- depends: 150
- goal: `values[1]` из `[10, 20, 30]` равно 20
- allowed_files: `script_vm/tests/lower_array_unit.mlc`, `STATE`
- forbidden: F0
- contract: индекс — выражение, не только литерал
- red: `RED(lower_array)`, поведенческий
- green: none
- gate: `RED(lower_array)`
- sabotage: none
- performance: not a perf step
- done_when: красный исход
- state_update: фактическое значение

### Step 152 — Green: чтение индекса

- kind: green
- depends: 151
- goal: `ExprIndex` в `GET_INDEX`
- allowed_files: `script_vm/lower/lower_array.mlc`, `script_vm/lower/lower_expression.mlc`, `STATE`
- forbidden: F0
- contract: индекс не i32 — `type_mismatch`
- red: none
- green: `RUN(lower_array)`
- gate: `RUN(lower_array)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: value=20
- state_update: `unit ok lower_array`

### Step 153 — Red: запись индекса

- kind: red
- depends: 152
- goal: `let mut values = [1, 2]` затем `values[0] = 9` даёт 9
- allowed_files: `script_vm/tests/lower_array_unit.mlc`, `STATE`
- forbidden: F0
- contract: запись в `const` — `assign_immutable`
- red: `RED(lower_array)`, поведенческий
- green: none
- gate: `RED(lower_array)`
- sabotage: none
- performance: not a perf step
- done_when: красный исход
- state_update: фактическое значение

### Step 154 — Green: запись индекса

- kind: green
- depends: 153
- goal: присваивание `ExprIndex` в `SET_INDEX`
- allowed_files: `script_vm/lower/lower_array.mlc`, `script_vm/lower/lower_statement.mlc`, `STATE`
- forbidden: F0; `lower_statement.mlc` ≤ 400. Тело вынести в `lower_array.mlc`, в statement оставить вызов
- contract: выход за пределы на исполнении остаётся `bounds`, лоуэр его не прячет
- red: none
- green: `RUN(lower_array)`
- gate: `RUN(lower_array)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: value=9
- state_update: `unit ok lower_array`

### Step 155 — Red: пустой массив и bool-элементы

- kind: red
- depends: 154
- goal: `len([])` = 0 и чтение `[true, false][0]` как 1 через сравнение
- allowed_files: `script_vm/tests/lower_array_unit.mlc`, `STATE`
- forbidden: F0
- contract: `if [true, false][1] then 1 else 0 end` ожидается 0
- red: `RED(lower_array)`, поведенческий
- green: none
- gate: `RED(lower_array)`
- sabotage: none
- performance: not a perf step
- done_when: красный исход
- state_update: какая из двух проверок упала

### Step 156 — Green: пустой массив и bool

- kind: green
- depends: 155
- goal: закрыть шаг 155
- allowed_files: `script_vm/lower/lower_array.mlc`, `STATE`
- forbidden: F0
- contract: смесь `i32` и `bool` в одном литерале — `type_mismatch`
- red: none
- green: `RUN(lower_array)`
- gate: `RUN(lower_array)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: unit зелёный
- state_update: `unit ok lower_array`

### Step 157 — Red: вложенный массив отклонён

- kind: red
- depends: 156
- goal: `[ [1] ]` получает отдельный код, не молчаливое исполнение
- allowed_files: `script_vm/tests/lower_array_unit.mlc`, `STATE`
- forbidden: F0
- contract: диагностика `unsupported_nested_array`. Тот же unit добавляет проверку `array_alias_shares_object`, но она в этом шаге ещё не обязана быть зелёной: красный исход — именно код вложенного массива
- red: `RED(lower_array)`, поведенческий, сегодня код другой
- green: none
- gate: `RED(lower_array)`
- sabotage: none
- performance: not a perf step
- done_when: красный исход на имени кода
- state_update: фактический код

### Step 158 — Green: вложенный массив и ссылка

- kind: green
- depends: 157
- goal: код `unsupported_nested_array` и pin `array_alias_shares_object` = 9
- allowed_files: `script_vm/lower/lower_array.mlc`, `script_vm/lower/lower_statement.mlc`, `script_vm/tests/lower_array_unit.mlc`, `STATE`
- forbidden: F0
- contract: `let mut left = [1]`, `let mut right = left`, `right[0] = 9`, чтение `left[0]` равно 9. Копию объекта не вводить. Вложенный литерал не исполняется
- red: none
- green: `RUN(lower_array)`
- gate: `RUN(lower_array)`; `GATE(lower_array)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: оба исхода зелёные
- state_update: `unit ok lower_array`

### Step 159 — Red: массив в REPL

- kind: red
- depends: 158, 144
- goal: `len([1, 2, 3])` из сессии даёт 3
- allowed_files: `script_vm/tests/repl_session_unit.mlc`, `STATE`
- forbidden: F0
- contract: одна новая проверка, старые не удалять
- red: `RED(repl_session)`, поведенческий
- green: none
- gate: `RED(repl_session)`
- sabotage: none
- performance: not a perf step
- done_when: красный исход
- state_update: фактическая строка

### Step 160 — Green: массив в REPL

- kind: green
- depends: 159
- goal: сессия уже должна принять этот кусок через общий лоуэр; если нет — починить сессию, не второй лоуэр
- allowed_files: `script_vm/repl_session.mlc`, `script_vm/lower/lower_array.mlc`, `STATE`
- forbidden: F0
- contract: чинить только причину красного шага 159
- red: none
- green: `RUN(repl_session)`
- gate: `RUN(repl_session)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: number=3
- state_update: `unit ok repl_session`

### Step 161 — Red: break в while

- kind: red
- depends: 104
- goal: `break` внутри цикла возвращает значение раньше конца
- allowed_files: `script_vm/tests/lower_iteration_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/lower/**`
- contract: функция с `while` и `break`, когда сумма достигла 3, возвращает 3, а не полную сумму. Сейчас код `unsupported_break`
- red: `RED(lower_iteration)`, поведенческий
- green: none
- gate: `RED(lower_iteration)`
- sabotage: none
- performance: not a perf step
- done_when: тест не зарегистрирован
- state_update: диагностика

### Step 162 — Green: break

- kind: green
- depends: 161
- goal: патч перехода на конец ближайшего `while`
- allowed_files: `script_vm/lower/lower_iteration.mlc`, `script_vm/lower/lower_statement.mlc`, `scripts/run_script_vm_gate.rb`, `STATE`
- forbidden: F0; `lower_statement.mlc` ≤ 400
- contract: вложенный `while`: `break` выходит только из внутреннего
- red: none
- green: `RUN(lower_iteration)` и регистрация
- gate: `RUN(lower_iteration)`; `GATE(lower_iteration lower_loops)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: value=3
- state_update: `unit ok lower_iteration`

### Step 163 — Red: continue

- kind: red
- depends: 162
- goal: `continue` пропускает остаток тела
- allowed_files: `script_vm/tests/lower_iteration_unit.mlc`, `STATE`
- forbidden: F0
- contract: чётные шаги не прибавляются, сумма 1..4 с пропуском чётных равна 4
- red: `RED(lower_iteration)`, поведенческий
- green: none
- gate: `RED(lower_iteration)`
- sabotage: none
- performance: not a perf step
- done_when: красный исход
- state_update: фактическое значение

### Step 164 — Green: continue

- kind: green
- depends: 163
- goal: переход к проверке условия того же `while`
- allowed_files: `script_vm/lower/lower_iteration.mlc`, `STATE`
- forbidden: F0
- contract: `continue` не пропускает увеличение индекса, если увеличение стоит до `continue` в тесте. Тест шага 163 сам расставляет индекс так, чтобы цикл закончился
- red: none
- green: `RUN(lower_iteration)`
- gate: `RUN(lower_iteration)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: value=4
- state_update: `unit ok lower_iteration`

### Step 165 — Red: break вне цикла

- kind: red
- depends: 164
- goal: код диагностики меняется с `unsupported_break` на `break_outside_loop`
- allowed_files: `script_vm/tests/lower_iteration_unit.mlc`, `STATE`
- forbidden: F0
- contract: `break` на верхнем уровне функции — `break_outside_loop`. `continue` — `continue_outside_loop`
- red: `RED(lower_iteration)`, поведенческий: сегодня код другой
- green: none
- gate: `RED(lower_iteration)`
- sabotage: none
- performance: not a perf step
- done_when: красный исход на имени кода
- state_update: фактический код

### Step 166 — Green: диагностика вне цикла

- kind: green
- depends: 165
- goal: новые коды
- allowed_files: `script_vm/lower/lower_iteration.mlc`, `script_vm/lower/lower_statement.mlc`, `STATE`
- forbidden: F0
- contract: внутри цикла коды `unsupported_*` больше не выдаются
- red: none
- green: `RUN(lower_iteration)`
- gate: `RUN(lower_iteration)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: оба новых кода
- state_update: `unit ok lower_iteration`

### Step 167 — Red: break в REPL

- kind: red
- depends: 166, 160
- goal: сессия исполняет функцию с `break`
- allowed_files: `script_vm/tests/repl_session_unit.mlc`, `STATE`
- forbidden: F0
- contract: объявление функции с `break`, затем вызов, число 3, как в шаге 161
- red: `RED(repl_session)`, поведенческий
- green: none
- gate: `RED(repl_session)`
- sabotage: none
- performance: not a perf step
- done_when: красный исход
- state_update: фактическая строка

### Step 168 — Green: break в REPL

- kind: green
- depends: 167
- goal: общий лоуэр уже должен это уметь; чинить сессию или пропущенный импорт модуля итерации
- allowed_files: `script_vm/repl_session.mlc`, `script_vm/lower/lower_iteration.mlc`, `script_vm/lower/lower_module.mlc`, `STATE`
- forbidden: F0; `lower_module.mlc` ≤ 400
- contract: не копировать второй лоуэр внутрь REPL
- red: none
- green: `RUN(repl_session)`
- gate: `RUN(repl_session)`; `GATE(repl_session lower_iteration)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: number=3
- state_update: `unit ok repl_session`

### Step 169 — Red: for по массиву

- kind: red
- depends: 158, 162
- goal: `for item in [1, 2, 3]` суммирует 6
- allowed_files: `script_vm/tests/lower_for_unit.mlc`, `STATE`
- forbidden: F0
- contract: синтаксис только `for item in values do ... end`. Не писать `..`
- red: `RED(lower_for)`, поведенческий
- green: none
- gate: `RED(lower_for)`
- sabotage: none
- performance: not a perf step
- done_when: тест не зарегистрирован
- state_update: диагностика

### Step 170 — Green: for

- kind: green
- depends: 169
- goal: снизить `ExprFor` к индексу и `while`
- allowed_files: `script_vm/lower/lower_iteration.mlc`, `script_vm/lower/lower_statement.mlc`, `scripts/run_script_vm_gate.rb`, `STATE`
- forbidden: F0
- contract: имя цикла — новый локал на время тела. Обход не по ссылке на элемент: присваивание имени не пишет в массив
- red: none
- green: `RUN(lower_for)` и регистрация
- gate: `RUN(lower_for)`; `GATE(lower_for lower_array)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: sum=6
- state_update: `unit ok lower_for`

### Step 171 — Red: пустой for

- kind: red
- depends: 170
- goal: тело пустого массива не исполняется
- allowed_files: `script_vm/tests/lower_for_unit.mlc`, `STATE`
- forbidden: F0
- contract: сумма остаётся 0
- red: `RED(lower_for)`, поведенческий
- green: none
- gate: `RED(lower_for)`
- sabotage: none
- performance: not a perf step
- done_when: красный исход
- state_update: фактическое значение

### Step 172 — Green: пустой for

- kind: green
- depends: 171
- goal: длина 0 не входит в тело
- allowed_files: `script_vm/lower/lower_iteration.mlc`, `STATE`
- forbidden: F0
- contract: нет деления и нет чтения индекса 0
- red: none
- green: `RUN(lower_for)`
- gate: `RUN(lower_for)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: sum=0
- state_update: `unit ok lower_for`

### Step 173 — Red: break внутри for

- kind: red
- depends: 172
- goal: `break` выходит из `for`
- allowed_files: `script_vm/tests/lower_for_unit.mlc`, `STATE`
- forbidden: F0
- contract: сумма останавливается на 3
- red: `RED(lower_for)`, поведенческий
- green: none
- gate: `RED(lower_for)`
- sabotage: none
- performance: not a perf step
- done_when: красный исход
- state_update: фактическое значение

### Step 174 — Green: break внутри for

- kind: green
- depends: 173
- goal: тот же стек циклов, что у `while`
- allowed_files: `script_vm/lower/lower_iteration.mlc`, `STATE`
- forbidden: F0
- contract: `continue` в `for` переходит к следующему индексу
- red: none
- green: `RUN(lower_for)`
- gate: `RUN(lower_for)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: sum=3
- state_update: `unit ok lower_for`

### Step 175 — Red: for в REPL

- kind: red
- depends: 174, 168
- goal: сессия считает сумму циклом `for`
- allowed_files: `script_vm/tests/repl_session_unit.mlc`, `STATE`
- forbidden: F0
- contract: выражение или объявление плюс вызов, результат 6
- red: `RED(repl_session)`, поведенческий
- green: none
- gate: `RED(repl_session)`
- sabotage: none
- performance: not a perf step
- done_when: красный исход
- state_update: фактическая строка

### Step 176 — Green: for в REPL

- kind: green
- depends: 175
- goal: сессия использует общий лоуэр
- allowed_files: `script_vm/repl_session.mlc`, `script_vm/lower/lower_iteration.mlc`, `STATE`
- forbidden: F0
- contract: не дублировать снижение `for` в REPL
- red: none
- green: `RUN(repl_session)`
- gate: `RUN(repl_session)`; `GATE(repl_session lower_for)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: number=6
- state_update: `unit ok repl_session`

### Step 177 — Red: хост mlc_repl_submit

- kind: red
- depends: 105, 126
- goal: C-тест зовёт функцию, которой нет
- allowed_files: `script_vm/tests/repl_host.cpp`, `scripts/run_script_vm_repl_host.rb`, `STATE`
- forbidden: F0; не добавлять символ в заголовок в этом шаге
- contract: раннер по образцу `run_script_vm_lowered_host.rb`. Хост создаёт сессию, передаёт `1 + 2`, ждёт status value и число 3. Сейчас сборка падает: нет `mlc_repl_submit`. Это «нет символа»
- red: `ruby scripts/run_script_vm_repl_host.rb` → exit ≠ 0
- green: none
- gate: эта команда
- sabotage: none
- performance: not a perf step
- done_when: отказ линковки или компиляции на отсутствующем символе
- state_update: первая строка ошибки

### Step 178 — Green: создание сессии из C

- kind: green
- depends: 177
- goal: провести один submit выражения
- allowed_files: `runtime/include/mlc/script_vm/embedding_abi.h`, `runtime/src/script_vm/embedding_abi.cpp`, `script_vm/repl_embed.mlc`, `script_vm/embedding.mlc`, `STATE`
- forbidden: F0 кроме двух названных runtime-файлов; сигнатуры старых `mlc_vm_*` не менять. `embedding.mlc` ≤ 400, иначе логика только в `repl_embed.mlc`
- contract: поля ответа как в S105
- red: none
- green: раннер хоста exit 0, строка `case expression ok`
- gate: `ruby scripts/run_script_vm_repl_host.rb` → exit 0; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: value 3
- state_update: `case expression ok`

### Step 179 — Red: два submit одной сессии

- kind: red
- depends: 178
- goal: второе обращение видит функцию из первого
- allowed_files: `script_vm/tests/repl_host.cpp`, `STATE`
- forbidden: F0
- contract: первый буфер — объявление `identity`, status accepted. Второй — `identity(4)`, value 4. Случай печатает `case accumulate ok`
- red: раннер exit ≠ 0, поведенческий `case accumulate`
- green: none
- gate: `ruby scripts/run_script_vm_repl_host.rb`
- sabotage: none
- performance: not a perf step
- done_when: отказ accumulate
- state_update: строка case

### Step 180 — Green: накопление в C-сессии

- kind: green
- depends: 179
- goal: один `MlcRepl` держит accepted source
- allowed_files: `script_vm/repl_embed.mlc`, `runtime/src/script_vm/embedding_abi.cpp`, `STATE`
- forbidden: F0 кроме названного cpp
- contract: каждый submit не создаёт новую сессию
- red: none
- green: `case accumulate ok`
- gate: `ruby scripts/run_script_vm_repl_host.rb` → exit 0
- sabotage: none
- performance: not a perf step
- done_when: value 4
- state_update: `case accumulate ok`

### Step 181 — Red: диагностика структурой

- kind: red
- depends: 180
- goal: хост читает код, строку и колонку из полей, не из текста stdout
- allowed_files: `script_vm/tests/repl_host.cpp`, `STATE`
- forbidden: F0
- contract: submit `missing_name`. status diagnostic, `code_text` начинается с `unknown_identifier`, line и column > 0. Строка `case diagnostic ok`
- red: раннер exit ≠ 0
- green: none
- gate: `ruby scripts/run_script_vm_repl_host.rb`
- sabotage: none
- performance: not a perf step
- done_when: отказ diagnostic
- state_update: фактические поля, которые напечатал хост

### Step 182 — Green: поля диагностики

- kind: green
- depends: 181
- goal: заполнить `MlcReplAnswer` из ответа ядра
- allowed_files: `script_vm/repl_embed.mlc`, `runtime/src/script_vm/embedding_abi.cpp`, `runtime/include/mlc/script_vm/embedding_abi.h`, `STATE`
- forbidden: F0 кроме названных runtime-файлов
- contract: обрезка кода длиннее 63 символов не нужна для `unknown_identifier`, но буфер заканчивается нулём
- red: none
- green: `case diagnostic ok`
- gate: `ruby scripts/run_script_vm_repl_host.rb` → exit 0
- sabotage: none
- performance: not a perf step
- done_when: поля совпали
- state_update: `case diagnostic ok`

### Step 183 — Red: NULL и повторный destroy

- kind: red
- depends: 182
- goal: дурное использование не вызывает abort
- allowed_files: `script_vm/tests/repl_host.cpp`, `STATE`
- forbidden: F0
- contract: `mlc_repl_submit(nullptr, ...)` возвращает −1. Повторный destroy печатает panic с `double release` и программа доходит до `case misuse ok`
- red: раннер exit ≠ 0 или abort
- green: none
- gate: `ruby scripts/run_script_vm_repl_host.rb`
- sabotage: none
- performance: not a perf step
- done_when: отказ misuse
- state_update: как завершился процесс

### Step 184 — Green: безопасный destroy

- kind: green
- depends: 183
- goal: повторить правило handle: panic callback, не abort
- allowed_files: `runtime/src/script_vm/embedding_abi.cpp`, `script_vm/repl_embed.mlc`, `STATE`
- forbidden: F0 кроме названного cpp
- contract: `delete` уже освобожденной сессии не делать. NULL destroy — пустая операция
- red: none
- green: `case misuse ok`
- gate: `ruby scripts/run_script_vm_repl_host.rb` → exit 0
- sabotage: none
- performance: not a perf step
- done_when: misuse зелёный и процесс не сигнальный
- state_update: `case misuse ok`

### Step 185 — Red: строка через C

- kind: red
- depends: 184, 144
- goal: `string_length` для `'hi'` равен 2
- allowed_files: `script_vm/tests/repl_host.cpp`, `STATE`
- forbidden: F0
- contract: status value, `string_length` 2, `value` 0. Строка `case string ok`
- red: раннер exit ≠ 0
- green: none
- gate: `ruby scripts/run_script_vm_repl_host.rb`
- sabotage: none
- performance: not a perf step
- done_when: отказ string
- state_update: фактические поля

### Step 186 — Green: строковый ответ в C

- kind: green
- depends: 185
- goal: проставить `string_length`, для i32 оставлять −1
- allowed_files: `script_vm/repl_embed.mlc`, `runtime/src/script_vm/embedding_abi.cpp`, `STATE`
- forbidden: F0 кроме названного cpp
- contract: случай `identity(4)` по-прежнему value 4 и `string_length` −1
- red: none
- green: все прежние case и `case string ok`
- gate: `ruby scripts/run_script_vm_repl_host.rb` → exit 0
- sabotage: none
- performance: not a perf step
- done_when: string_length=2
- state_update: `case string ok`

### Step 187 — Замер submit из C

- kind: measure
- depends: 186, 128
- goal: тот же сценарий S106 через `mlc_repl_submit`, без старта `script_tool` на каждый submit
- allowed_files: `script_vm/tests/repl_host.cpp`, `scripts/run_script_vm_repl_host_budget.rb`, `STATE`
- forbidden: F0
- contract: раннер печатает минимум из 5 `repl_host_submit_us=<целое>`. Ответы сценария проверяются
- red: none
- green: none
- gate: `ruby scripts/run_script_vm_repl_host_budget.rb` → exit 0 и строка метрики
- sabotage: none
- performance: записать число. Предел проверяет шаг 188
- done_when: число напечатано
- state_update: минимум

### Step 188 — Сверка хостового бюджета

- kind: measure
- depends: 187
- goal: тот же предел 500000 мкс
- allowed_files: `PLAN200`, `scripts/run_script_vm_repl_host_budget.rb`, `STATE`
- forbidden: F0; предел не поднимать
- contract: `--max-absolute repl_host_submit_us=500000`. Decision log `### S188 — бюджет хоста`
- red: none
- green: S188 с `within_budget`
- gate: команда с `--max-absolute` → exit 0
- sabotage: none
- performance: превышение — `blocked`
- done_when: S188 содержит число
- state_update: число и вердикт

### Step 189 — Green: одна сессия со всеми конструкциями

- kind: green
- depends: 176, 186
- goal: зафиксировать совместную работу, а не новый язык
- allowed_files: `script_vm/tests/repl_language_unit.mlc`, `scripts/run_script_vm_gate.rb`, `STATE`
- forbidden: F0
- contract: одна сессия по очереди принимает строку, массив, `for`, `break` и вызов. Итог — заранее посчитанное i32, напечатанное как `repl answer=value kind=int32 number=<n>` и `[script_vm] repl_language_unit ok`. Если это не собирается, чинить стык в `allowed_files` нельзя: шаг `blocked` с диагностикой, без ослабления ожидания
- red: none
- green: `RUN(repl_language)` и регистрация
- gate: `RUN(repl_language)`; `GATE(repl_language)`; `SIZE`
- sabotage: none
- performance: not a perf step
- done_when: unit зелёный. Если он красный из-за стыка, это `blocked`, не правка наугад в файлах вне списка
- state_update: число результата

### Step 190 — Gate очереди 200

- kind: green
- depends: 189, 147
- goal: одна команда гоняет старую очередь и новые проверки
- allowed_files: `scripts/run_script_vm_queue200_gate.rb`, `STATE`
- forbidden: F0; `scripts/run_script_vm_queue_gate.rb` не менять
- contract: по порядку: `ruby scripts/run_script_vm_queue_gate.rb` (последняя строка `stages=7`), `GATE(repl_balance repl_session string_heap string_opcode lower_text lower_array lower_iteration lower_for repl_language)`, `ruby scripts/run_script_vm_repl_cli.rb`, `ruby scripts/run_script_vm_repl_host.rb`. Успех: `script_vm_queue200_gate ok`
- red: none
- green: эта команда
- gate: `ruby scripts/run_script_vm_queue200_gate.rb` → exit 0 и последняя строка `script_vm_queue200_gate ok`
- sabotage: none
- performance: not a perf step
- done_when: последняя строка совпала
- state_update: `script_vm_queue200_gate ok`

### Step 191 — Замер сквозной сессии

- kind: measure
- depends: 190
- goal: записать время сценария шага 189 внутри процесса
- allowed_files: `script_vm/tests/repl_language_unit.mlc`, `scripts/run_script_vm_repl_language_budget.rb`, `PLAN200`, `STATE`
- forbidden: F0; предел S106 не менять этим шагом
- contract: минимум из 5, строка `repl_language_us=<целое>`. Decision log `### S191 — сквозная сессия` копирует число. Сравнение с 500000 не требуется: сценарий другой, это запись для следующей очереди
- red: none
- green: раннер exit 0
- gate: `ruby scripts/run_script_vm_repl_language_budget.rb` → exit 0
- sabotage: none
- performance: записать число
- done_when: S191 содержит число
- state_update: `repl_language_us`

### Step 192 — Critic: баланс не смотрит внутрь кавычек

- kind: critic
- depends: 190
- goal: отключение пропуска строк должно краснить `repl_balance`
- allowed_files: `STATE`
- forbidden: порча не коммитится
- contract: в `repl_balance.mlc` временно не пропускать одинарные кавычки. `RUN(repl_balance)` красный. Вернуть файл. `RUN(repl_balance)` зелёный
- red: none
- green: none
- gate: красный, затем зелёный `RUN(repl_balance)`
- sabotage: кавычки считаются обычным текстом
- performance: not a perf step
- done_when: diff `repl_balance.mlc` пуст
- state_update: красная проверка

### Step 193 — Critic: сессия помнит принятый текст

- kind: critic
- depends: 192
- goal: сброс `accepted_source` на каждом submit краснит `repl_session`
- allowed_files: `STATE`
- forbidden: порча не коммитится
- contract: не конкатенировать прошлый текст. Красный `RUN(repl_session)` на вызове `identity`. Откат. Зелёный
- red: none
- green: none
- gate: красный, затем `RUN(repl_session)`
- sabotage: пустой accepted source
- performance: not a perf step
- done_when: diff пуст
- state_update: красная проверка

### Step 194 — Critic: индекс массива проверяет границы

- kind: critic
- depends: 193
- goal: `executor_check_array_index` нельзя отключить незаметно
- allowed_files: `STATE`
- forbidden: порча не коммитится
- contract: убрать отказ `run_err_bounds` в `executor_check_array_index`. `RUN(char_heap_ops)` или `RUN(lower_array)` красный. Откат. Зелёный `GATE(char_heap_ops lower_array)`
- red: none
- green: none
- gate: красный прогон, затем зелёный gate
- sabotage: пропуск проверки границ
- performance: not a perf step
- done_when: diff `executor_heap.mlc` пуст
- state_update: какой unit поймал порчу

### Step 195 — Critic: break не становится пустой инструкцией

- kind: critic
- depends: 194
- goal: пустой `break` должен краснить `lower_iteration`
- allowed_files: `STATE`
- forbidden: порча не коммитится
- contract: в `lower_iteration.mlc` не ставить переход для `break`. `RUN(lower_iteration)` красный. Откат. Зелёный
- red: none
- green: none
- gate: красный, затем `RUN(lower_iteration)`
- sabotage: break ничего не делает
- performance: not a perf step
- done_when: diff пуст
- state_update: красная проверка

### Step 196 — Critic: for идёт по всем элементам

- kind: critic
- depends: 195
- goal: цикл, который смотрит только первый элемент, красный
- allowed_files: `STATE`
- forbidden: порча не коммитится
- contract: в снижении `for` остановить индекс после 0. `RUN(lower_for)` красный на сумме 6. Откат. Зелёный
- red: none
- green: none
- gate: красный, затем `RUN(lower_for)`
- sabotage: один проход
- performance: not a perf step
- done_when: diff пуст
- state_update: красная проверка

### Step 197 — Critic: C-сессия не создаётся заново на submit

- kind: critic
- depends: 196
- goal: новый `MlcRepl` на каждый submit краснит accumulate
- allowed_files: `STATE`
- forbidden: порча не коммитится
- contract: в `embedding_abi.cpp` внутри submit создать другую сессию и звать её. `ruby scripts/run_script_vm_repl_host.rb` красный на accumulate. Откат. Зелёный
- red: none
- green: none
- gate: красный хост, затем зелёный
- sabotage: новая сессия
- performance: not a perf step
- done_when: diff cpp пуст
- state_update: case, который поймал порчу

### Step 198 — Critic: откат предыдущих порч полный

- kind: critic
- depends: 197
- goal: убедиться, что шаги 192–197 ничего не оставили
- allowed_files: `STATE`
- forbidden: не чинить найденный diff этим шагом молча. Если diff не пуст — `blocked`
- contract: `git diff -- script_vm/repl_balance.mlc script_vm/repl_session.mlc script_vm/executor_heap.mlc script_vm/lower/lower_iteration.mlc runtime/src/script_vm/embedding_abi.cpp` пуст. Затем `ruby scripts/run_script_vm_queue200_gate.rb` зелёный
- red: none
- green: none
- gate: пустой diff и `script_vm_queue200_gate ok`
- sabotage: none
- performance: not a perf step
- done_when: оба условия
- state_update: `clean`

### Step 199 — Critic: новые unit зарегистрированы

- kind: critic
- depends: 198
- goal: общий gate знает каждый новый unit этой очереди
- allowed_files: `scripts/run_script_vm_gate.rb`, `STATE`
- forbidden: F0; не удалять старые имена
- contract: в списке есть `repl_balance`, `repl_session`, `string_heap`, `string_opcode`, `lower_text`, `lower_array`, `lower_iteration`, `lower_for`, `repl_language`. `GATE(all)` зелёный
- red: none
- green: недостающие имена добавить, если зелёный шаг забыл регистрацию
- gate: `ruby scripts/run_script_vm_gate.rb` → `script_vm_gate ok units=` и число не меньше прежнего 42 плюс 9
- sabotage: none
- performance: not a perf step
- done_when: девять имён находятся `rg` в раннере
- state_update: число units

### Step 200 — Граница после (А)

- kind: analysis
- depends: 199
- goal: записать, что эта очередь закрыла и что осталось
- allowed_files: `PLAN200`, `STATE`
- forbidden: F0; кода нет
- contract: `### S200 — граница`. Копией, без нового прогона, числа S128, S188, S191 и B1/B3 из шага 147. Список «ещё не готово»: float в лоуэре, `match`, лямбды и замыкания из исходника, методы и generics, имена полей в рантайме, живая память между вводами REPL, fibers, дамп VM, инкрементальный GC, лимит памяти элементов, shapes, inline caches, baseline JIT, tier-2 JIT, opcode DSL, textui, `mlcc --emit-script`, BigInt, eval. Открытых блокеров нет — либо перечислить номера, которые остались blocked
- red: none
- green: S200 дописан
- gate: `rg -c '^### S200 ' docs/agent/SCRIPT_VM_QUEUE_200.md` → `1`; `ruby scripts/run_script_vm_queue200_gate.rb` → exit 0
- sabotage: none
- performance: not a perf step
- done_when: список и числа на месте. STATE после шага: `current_step: 200`, `status: queue_complete`, `next_step: none`
- state_update: `queue_complete`

## Decision log

### S101 — факты

`wc -l`: `script_vm/heap_types.mlc` 103, `script_vm/bytecode.mlc` 388, `script_vm/lower/lower_statement.mlc` 368, `script_vm/lower/lower_expression.mlc` 399, `script_vm/lower/lower_records.mlc` 395, `script_vm/tools/script_tool.mlc` 123, `runtime/include/mlc/script_vm/embedding_abi.h` 77, `compiler/frontend/ast.mlc` 348, `compiler/frontend/lexer.mlc` 553.

- Типы кучи: cell 1, array 2, record 3, closure 4. Строкового типа нет. CONFIRMED `script_vm/heap_types.mlc:32-35`. Функции `heap_type_string` нет.
- Опкоды 1–25, последний `SET_UPVAL`. `LENGTH` и `MAKE_STRING` нет. CONFIRMED `script_vm/bytecode.mlc:15-39`.
- `ExprStr`, `ExprArray`, `ExprIndex`, `ExprFor`, `StmtBreak`, `StmtContinue` парсер уже строит. CONFIRMED `compiler/frontend/ast.mlc:102`, `:111`, `:115`, `:119`, `:204`. Построение: `compiler/frontend/parser/exprs.mlc:257`, `:258`, `:677`, `:752`, `:986`, `:990`, `:1105`.
- Строковый литерал — в одинарных кавычках. Комментарии `//` и `/* */`. CORRECTED: комментарии CONFIRMED `compiler/frontend/lexer.mlc:400` (`//`) и `:403` (`/*`). Строки: двойная кавычка тоже даёт `LStr` (`lexer.mlc:535`, `scan_string` `:264`, токен `:298`). Одинарная кавычка (`:538`) при длине содержимого 1 даёт `LChar` (`:219`), иначе `LStr` (`:220`).
- `for` — `for имя in выражение do ... end`. Одного диапазона `..` нет. CONFIRMED `parse_for_expr` `compiler/frontend/parser/exprs.mlc:1099-1105`: имя, `advance_by(2)` пропускает имя и `in`, выражение, затем тело до `end`. В `compiler/frontend/lexer.mlc` последовательности `..` нет.
- `break` / `continue` лоуэр уже отклоняет кодами `unsupported_break` и `unsupported_continue`. CONFIRMED `script_vm/lower/lower_statement.mlc:356` и `:358`.
- `lower_expression.mlc` 399 строк, `lower_records.mlc` 395, `bytecode.mlc` 388. CONFIRMED `wc -l`.
- `mlc_vm_load_module` есть. Функций `mlc_repl_*` нет. CONFIRMED `runtime/include/mlc/script_vm/embedding_abi.h:56`. Поиск `mlc_repl_` в этом заголовке пуст.
- `script_tool` умеет `run` и `compile`. Подкоманды `repl` нет. CONFIRMED `script_vm/tools/script_tool.mlc:112-114`.
- Записи ссылочные, pin `record_alias_shares_object`. CONFIRMED очередь 1–100, S3 пункт 4, и `script_vm/tests/lower_records_unit.mlc:143`.

### S102 — сессия

1. Модуль `script_vm/repl_session.mlc`, функция `repl_session_submit(session, chunk) -> ReplAnswer`. Сессия хранит `accepted_source` и `pending`.
2. Пустой и пробельный кусок дописывается в `pending` и даёт `incomplete`.
3. Кусок, у которого первое слово `fn`, `export` или `type`, — объявление. После баланса он дописывается в `accepted_source`, `pending` очищается, текст заново проходит `lower_source`. Успех — `accepted`. Диагностика — объявление не сохраняется.
4. Иной завершённый кусок — выражение. Программа = `accepted_source` плюс `export fn repl_entry() -> i32 = do` + кусок + `end`. Вызов `repl_entry` без аргументов. Успех i32 — `value kind=int32`. Строка — отдельное правило шагов 143–144, до них строковый результат есть диагностика `repl_result_not_integer`.
5. Пять печатных строк: `repl answer=incomplete`; `repl answer=accepted`; `repl answer=value kind=int32 number=<целое>`; `repl answer=value kind=string length=<целое>`; `repl answer=diagnostic code=<код> line=<целое> column=<целое>`.
6. Между вводами куча не хранится.

### S103 — баланс

Модуль `script_vm/repl_balance.mlc`, функция `repl_balance_incomplete(text) -> bool`. Сканер символов, не токены `compiler/frontend`. `compiler/**` не правится.

- Кавычки. Одинарная кавычка начинает строку; `\'` внутри не закрывает её. Двойная кавычка тоже начинает строку (S101: `"` всегда даёт `LStr`); `\"` внутри не закрывает её. Незакрытая кавычка — incomplete.
- Комментарии. `//` пропускается до конца строки. `/* */` пропускается целиком. Незакрытый `/*` — incomplete.
- Слова `do` и `end` считаются только как отдельные слова: соседний символ не буква, не цифра и не `_`. `doing` не считается.
- `{` и `}` считаются вне строк и комментариев. Незакрытая круглая скобка балансом не ловится: такой текст завершён и дальше даёт диагностику лоуэра.
- `repl_balance_incomplete` истинно, когда счётчик `do` больше `end`, либо скобок `{` больше `}`, либо кавычка или блочный комментарий не закрыты.

### S104 — строка и массив

Номер версии blob MLSC остаётся 1. Новые опкоды — 26 и 27.

1. `heap_type_string() = 5`. Элементы слота — `Int32` 0..255, по одному на байт, дочернего объекта нет.
2. Модуль `script_vm/string_heap.mlc`: `string_heap_from_bytes`, `string_heap_length`, `string_heap_byte_at`, `string_heap_equal`, `string_heap_concatenate`.
3. Опкоды в новом `script_vm/bytecode_text.mlc`: `instruction_opcode_length() = 26`, `instruction_opcode_make_string() = 27`. `bytecode.mlc` их только импортирует, если без этого не компилируется существующий диспетчер, и тогда шаг, который это сделает, сначала выносит диспетчер, а не дописывает 388 строк.
4. `LENGTH`: A = длина массива или строки в B, иначе `RunErr("type")`.
5. `MAKE_STRING`: A = строка из массива целых в B; байт вне 0..255 — `RunErr("bounds")`.
6. `ADD` двух строк склеивает; `EQ`/`NE` двух строк сравнивают байты. Два других heap-объекта `EQ` не начинают сравнивать по содержимому.
7. Лоуэр строки — `script_vm/lower/lower_text.mlc`. Литерал `'ab'` — массив байтов и `MAKE_STRING`. Двойная кавычка — тот же путь `LStr` (S101). `len(expr)` — встроенное имя, не метод, опкод `LENGTH`.
8. Лоуэр массива — `script_vm/lower/lower_array.mlc`: `ExprArray`, `ExprIndex`, присваивание индекса. Элементы этой очереди — `i32` и `bool`.
9. Массив ссылочный, как запись.
10. `break`/`continue` — `script_vm/lower/lower_iteration.mlc`. Коды `break_outside_loop` и `continue_outside_loop`. `for имя in массив` снижается к индексу и `while`.

### S105 — C ABI REPL

Каталога `script_vm/c_abi/` нет. Сигнатуры старых `mlc_vm_*` не меняются.

Три файла реализации: объявления в `runtime/include/mlc/script_vm/embedding_abi.h`, реализация в `runtime/src/script_vm/embedding_abi.cpp`, логика в `script_vm/repl_embed.mlc`.

`MlcRepl` — отдельный объект, не поле `MlcVm`. Функции: `mlc_repl_create(const MlcVmConfig*)`, `mlc_repl_destroy`, `mlc_repl_submit(MlcRepl*, const char* text, size_t size, MlcReplAnswer* answer)`.

Поля `MlcReplAnswer`: `status` (`MLC_REPL_INCOMPLETE` 0, `MLC_REPL_ACCEPTED` 1, `MLC_REPL_VALUE` 2, `MLC_REPL_DIAGNOSTIC` 3), `value` int32, `string_length` int32 (−1, если значение не строка), `line`, `column`, `code_text[64]`. Код диагностики длиннее 63 символов обрезается, буфер заканчивается нулём.

`mlc_repl_submit` на `NULL` сессии или `NULL` ответе возвращает −1 и не вызывает panic. Повторный `mlc_repl_destroy` одной сессии пишет в panic callback текст с `double release` и не вызывает `abort`. `NULL` destroy — пустая операция.

Хост-тест: `script_vm/tests/repl_host.cpp` и `scripts/run_script_vm_repl_host.rb` по образцу `scripts/run_script_vm_lowered_host.rb`.

### S106 — бюджет

В бюджет не входит старт процесса. 101–128 мс команды `script_tool run` — другой замер.

Сценарий: 20 объявлений `export fn function_<n>() -> i32 = <n>` отдельными submit, затем один submit выражения `function_20()`. Время от первого submit до ответа на выражение, внутри одного процесса, минимум из 5. Предел `repl_submit_us` ≤ 500000. Превышение на шаге 128 — `blocked` с числами, самовольная смена предела запрещена.
