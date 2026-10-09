# SCRIPT_VM_QUEUE_100 — 100 шагов: базовая встраиваемая MLC Script VM

Состояние очереди: [SCRIPT_VM_QUEUE_STATE.md](SCRIPT_VM_QUEUE_STATE.md). Этот файл — спецификация. Прогресс нигде больше не хранится.

Не путать: **MIR VM** (`compiler/vm/`, `mlcc --run`) — не предмет этой очереди. **MLC Script VM** — `script_vm/`, дизайн [../MLC_SCRIPT_VM.md](../MLC_SCRIPT_VM.md), трек [TRACK_MLC_SCRIPT_VM.md](TRACK_MLC_SCRIPT_VM.md). Phase 1 (§103a–i) закрыт 2026-08-06. Фазы 2–5 дизайн-дока (quickening, baseline JIT, opcode DSL, tier-2 JIT) в эту очередь не входят.

## Цель

Из существующего скелета получить базовую пригодную к встраиванию VM:

- маленький script-модуль на подмножестве MLC лоуэрится в существующий bytecode blob (`MLSC`, version 1) и исполняется;
- хост вызывает экспорт через C ABI (`embedding_abi.h`);
- регистры и слоты кучи — непрерывные изменяемые буферы, а не пересборка COW-массивов на каждую запись;
- TDD, измеренная производительность там, где заявлено ускорение, sabotage для несущих проверок.

Не входит: JIT, shapes/inline cache, prototype chain, eval, monkey patch, BigInt, concurrent/moving GC, инициализация VM в textui (`misc/textui/chat_host.mlc`, `chat_world_open`), hover/paint textui, правки `compiler/**`, `lib/mlc/**`, `compiler/vm/`.

## Проверенные факты (чтение исходников 2026-10-08)

Каждый факт шаг 1 обязан перепроверить и при расхождении записать поправку в S1.

| Факт | Источник |
|------|----------|
| `registers_set` пересобирает весь массив регистров на каждую запись | `script_vm/interpreter.mlc:108` |
| Два независимых исполнителя: `execute_with_heap` (`:295`, опкоды 1–21, один фрейм) и `execute_program` (`:638`, фреймы, только LOAD_CONST/MOVE/ADD/SUB/RETURN/JUMP/JUMP_IF_FALSE/LE/CALL/MAKE_CLOSURE/GET_UPVAL/SET_UPVAL) | `interpreter.mlc` |
| `execute_program` копирует `CallFrame` (включая `registers`) через `frames_set_last` на каждую инструкцию | `interpreter.mlc:552`, `:681` и далее |
| `value_is_falsy`: Nil, false, Int32(0), Float64(0.0) → falsy | `interpreter.mlc:214` |
| `apply_binary` DIV Int32: `left_number / right_number` без проверки `INT_MIN / -1` (UB в C++) | `interpreter.mlc:150` |
| `slots_with`, `elements_with` копируют весь массив на каждую запись; `heap_alloc_slot` всегда `slots.push`, freelist только пополняется в `heap_collect` | `script_vm/heap.mlc:75`, `:89`, `:114`, `:376` |
| `mark_object` рекурсивный | `heap.mlc:334` |
| Нет `.pop()` у `mlc::Array` | `misc/examples/vm_pop.mlc:1` |
| `ref mut` параметры существуют и эмитятся как `T&` | `compiler/checker/registry.mlc:10`, `compiler/out/registry.cpp:15` |
| `instruction_limit`, `heap_limit_objects` хранятся в `EmbeddingVmState` и нигде не применяются | `runtime/src/script_vm/embedding_abi.cpp:19` |
| `mlc_vm_call` на каждый вызов заново декодирует и верифицирует blob | `embedding_abi.cpp`, `embedding.mlc:335` |
| `mlc_vm_get_export` не проверяет наличие экспорта | `embedding_abi.cpp:84` |
| `decode_module` не проверяет `proto_index` экспорта и `register_count` | `embedding.mlc:171` |
| Верификатор не проверяет окно аргументов CALL и локальные источники upvalue MAKE_CLOSURE | `verifier.mlc:123` (`registers_ok`) |
| Размеры: `interpreter.mlc` 977, `heap.mlc` 496, `embedding.mlc` 410, `bytecode.mlc` 388, `verifier.mlc` 258 строк | `wc -l` |
| Единственный перф-вывод: `interpreter_arithmetic_unit.mlc` печатает `ns_per_insn` для 4-инструкционной программы с `run_arithmetic` | `script_vm/tests/interpreter_arithmetic_unit.mlc:197` |
| Парсер: присваивание = `StmtExpr(ExprBin("=", lhs, rhs))`; `export fn` = `DeclExported(DeclFn(...))`; `tokenize` в `compiler/frontend/lexer.mlc:520`, `parse_program_with_errors` в `compiler/frontend/parser/decls.mlc:1083` | `compiler/frontend/` |
| В MLC нет синтаксиса `export { X } from`; работает транзитивный `import { X } from './m'` | `docs/PLAN.md` §104-15 |
| Остаток §103f: mlcc мог терять операторы после `if/else-if` в `do`-блоках | `TRACK_MLC_SCRIPT_VM.md` §103f |
| `compiler/out/mlcc` существует; очередь его не пересобирает | `ls compiler/out/mlcc` |

## Общие правила (действуют на каждом шаге)

### Обозначения

- `STATE` = `docs/agent/SCRIPT_VM_QUEUE_STATE.md`
- `PLAN100` = `docs/agent/SCRIPT_VM_QUEUE_100.md`
- `F0` (запрещено везде, если шаг не разрешил явно): `compiler/**`, `lib/mlc/**`, `misc/textui/**`, `misc/editor/**`, `docs/PLAN.md`, `docs/agent/SESSION.md`, `docs/agent/CONTINUITY.md`, любые другие `docs/**`, `runtime/**`, `scripts/run_script_vm_*_red.sh`, существующие `script_vm/tests/*_unit.mlc` и `scripts/run_script_vm_*_unit.sh`, `compiler/out/**`.
- `RUN(name)` = `ruby scripts/run_script_vm_unit.rb script_vm/tests/<name>_unit.mlc`; успех: exit 0 и в stdout строка `[script_vm] <name>_unit ok`.
- `RED(name)` = та же команда обязана завершиться ненулевым кодом; в `last_verify` записывается первая строка отказа.
- `GATE(list)` = `ruby scripts/run_script_vm_gate.rb <list>`; успех: exit 0 и последняя строка stdout `script_vm_gate ok units=<число имён в list>`. `GATE(all)` — без аргументов, все зарегистрированные.
- Наборы имён для `GATE`: `OLD9` = `value_rep bytecode_format verifier interpreter_arithmetic control_flow heap_gc_arena arrays_records closures_fibers embedding_abi`; `ARITH` = `interpreter_arithmetic control_flow char_arith`; `HEAPSET` = `heap_gc_arena arrays_records char_heap_ops`; `FRAMES` = `closures_fibers char_frames`; `EMBED` = `embedding_abi char_blob`.
- `SIZE` = `ruby scripts/run_script_vm_file_size.rb`; успех: exit 0.
- `BENCH(args)` = `ruby scripts/run_script_vm_bench.rb <args>`.

### Дисциплина

1. Один шаг — один ход. Шаг `N` выполняется только если все `depends` имеют коммит `script_vm queue step <d>:` в `git log`.
2. Red перед green. Red-шаг добавляет падающий тест и коммитится вместе с ним; красный unit не регистрируется в `GATE` до своего green-шага. Gate red-шага — `RED(...)`, а не зелёный прогон.
3. Self-host diff (`compiler/build.sh`, mlcc2, `diff -r`) в этой очереди **не запускается**: ни один шаг не меняет `compiler/**` и `lib/mlc/**`. Если шаг обнаруживает, что без правки `compiler/**` невозможен, он ставит `blocked` и пишет причину; решение за пользователем.
4. `compiler/out/mlcc` не пересобирать. Если `compiler/out/mlcc` отсутствует — `blocked`.
5. Идентификаторы без сокращений (`.cursor/rules/no-abbreviations.mdc`): `register`, `instruction`, `program_counter`, `constant`. Комментариев в новом коде нет, кроме контракта/инварианта списком (`no-narrative-comments`). Существующие заголовочные комментарии не трогать.
6. Скрипты — Ruby (`scripts/*.rb`), не Python, не новый bash. Существующие `scripts/run_script_vm_*_unit.sh` остаются как есть.
7. Каждый `script_vm/**/*.mlc` ≤ 400 строк (`SIZE`). Если шаг добавляет код и файл переходит 400 — разбить по ответственности в этом же шаге.
8. Имена файлов модулей уникальны во всём графе импортов, включая подтягиваемые `compiler/frontend/**`: нельзя называть модуль `ast`, `ast_tokens`, `lexer`, `decls`, `exprs`, `types`, `predicates`, `comma_separated`, `expect_close`, `value` (кроме существующего `script_vm/value.mlc`). Новые модули lowering — с префиксом `lower_`.
9. После разбиения модуля публичные имена сохраняются через транзитивный `import { X } from './m'` (синтаксиса `export { } from` нет). Проверка набора экспортов: `rg -o '^export (fn|type) \w+' <файлы> | sort` до и после — множества равны.
10. Побочное чтение времени в i32: `script_vm_perf_now_us()` возвращает `monotonic_micros_i32` (`runtime/include/mlc/core/profile.hpp`); считать только разности; один замер ≤ ~2 с, иначе `total_us * 1000` переполняет i32. Перед первым использованием прочитать `profile.hpp`.
11. Прогон программ с возможным бесконечным циклом — только через Ruby `Timeout` (20 с) и kill процесса.
12. Бенчмарки: минимум из 5 запусков одной сборки. Заявка «быстрее» = строго меньше эталона (`--require-lower`), числа печатаются. Если шаг заявляет скорость, а метрика не уменьшилась: один повтор прогона; при повторном провале — `blocked` с числами, откат правки не обязателен, решение за пользователем. Допуск «не медленнее» (`--max-ratio`) — только там, где шаг его называет.
13. Остатки mlcc (проверяются шагом 18, до этого — не полагаться): (R1) операторы после цепочки `if/else-if` в `do`-блоке могут теряться — цепочки, после которых есть код, выносить в отдельные функции, возвращающие значение; (R2) допустимость присваивания целого значения `ref mut`-параметру и пути `state.field.set(...)` — проверяет шаг 18.
14. Коммиты: `git add <явный список путей>` (никогда `-A`, `-a`); сообщение `script_vm queue step <N>: <короткий заголовок>` (для red-шага — с суффиксом ` (red)`); без `--trailer`; без `git push`. В рабочем дереве лежат чужие незакоммиченные правки `misc/textui/**` — не трогать, не stash, не checkout. `tmp/` и `.tmp/` не коммитить (в `.gitignore`).
15. В коммит каждого шага входят: реально изменённые файлы из `allowed_files`, `STATE`, и `PLAN100`, только если шаг дописывает Decision log.
16. Дописывать в `PLAN100` можно только секцию `## Decision log` внизу файла: заголовок `### S<номер шага> — тема`. Остальной текст файла не править.
17. Если шаг требует файла вне `allowed_files` или из `F0` — `blocked`, причина в `last_result`.

### Протокол состояния (`STATE`)

Восемь полей, формат `ключ: значение`. Значения `status`: `not_started`, `ready`, `in_progress`, `consulting`, `consult_ready`, `blocked`, `queue_complete`.

- Начало обычного хода: `status: in_progress`, остальные поля не менять.
- Gate прошёл: `current_step: <N>`, `last_result: done step <N>: <что сделано, 1 строка>`, `last_verify: <команда> => <наблюдаемый исход; для measure — числа>`, `next_step: <N+1>`, `status: ready`, `consult_count: 0`, `consult_response:` пусто, `consult_verdict:` пусто. Для шага 100: `status: queue_complete`, `next_step: none`.
- Gate не прошёл в этом ходу: `next_step` не меняется, `consult_count` не меняется, `status: blocked`, `last_result: blocked step <N>: <причина>`, `last_verify: <упавшая команда> => <первые строки вывода>`. Коммита кода нет. Sonnet в этом же ходу не вызывать.
- `status: in_progress` на входе — тот же `next_step` выполняется заново.
- `status: consulting` на входе — повторить только консультацию, код шага не трогать.

### Консультация при остановке

Sonnet доступен очереди только через соседний репозиторий `mlc-support`, скрипт `review_script_vm_queue_consult.rb`. Не вызывать Sonnet через субагента Cursor, Task, `cursor_llm_complete` и любой другой канал. Скрипт совета не применяет патчи и не пишет в дерево MLC.

На один номер шага — один платный ответ. Второй провал того же шага после совета останавливает очередь на человеке.

Ход консультации (вход: `blocked` и `consult_count: 0`, либо `consulting`):

1. Записать `status: consulting`. Код `script_vm/` не менять.
2. Перезаписать `/home/jenya/workspaces/current/mlc-support/steps/script_vm_queue_consult.md`. Первая строка `step: <N>`. Дальше текст шага `N` из этого плана, `last_result`, `last_verify` и секция `## Files` — только существующие пути из `allowed_files` этого шага. В список нельзя класть `compiler/`, `lib/mlc/`, `misc/textui/`, `misc/editor/`.
3. Запустить `ruby /home/jenya/workspaces/current/mlc-support/review_script_vm_queue_consult.rb --dry-run`.
4. Exit 2 или строка `consult_over_budget`: платный запрос не отправлять. `consult_count: 1`, `consult_verdict: over_budget`, `status: blocked`, в `last_result` сумма и потолок. Остановиться.
5. Иначе запустить тот же скрипт без `--dry-run`. Успех — строка `consult_response: <путь>`. Записать путь в `consult_response`, `consult_count: 1`, `consult_verdict:` пусто, `status: consult_ready`. Закоммитить только `STATE`. Остановиться.
6. Потолок по умолчанию $1.50 за оценку первого вызова (`MLC_SUPPORT_CONSULT_MAX_USD`). Ответ того же шага моложе 6 часов скрипт отдаёт повторно и не платит снова.

Следующий ход видит `consult_ready`. Прочитать ответ. Скрипт пишет сверху строку `consult_step:`. Строка совета начинается с `verdict: retry`, `verdict: patch` или `verdict: human`.

- `human` или пустой verdict — оставить `blocked`, `consult_verdict` скопировать, код не менять, остановиться.
- `retry` или `patch` — один раз выполнить шаг `next_step`. Правки только в `allowed_files`. Совет, который тянет `compiler/**` или `lib/mlc/**`, не выполнять: это `human`. Затем обычный gate. Успех — обычное продвижение и сброс полей консультации. Провал — `blocked`, `consult_count` остаётся 1, второй совет не запрашивать.

Вход `blocked` при `consult_count` не 0: API не вызывать, шаг не повторять, остановиться и написать, что нужен человек.

### Шаблон шага

Поля: `kind` (analysis | decision | red | green | critic | refactor | measure), `depends`, `goal`, `allowed_files`, `forbidden`, `contract`, `red`, `green`, `gate`, `sabotage`, `performance`, `done_when`, `state_update`.

## Идентичный промпт очереди (вставлять в Cursor без изменений)

```
Ты исполнитель очереди SCRIPT_VM_QUEUE (MLC Script VM, каталог script_vm/). Один ход — один шаг. Состояние хранится только в файле.

1. Прочитай docs/agent/SCRIPT_VM_QUEUE_STATE.md и docs/agent/SCRIPT_VM_QUEUE_100.md: разделы «Общие правила», «Протокол состояния» и «Консультация при остановке», затем целиком шаг с номером next_step.
2. Если status равен queue_complete — ответь одной строкой «очередь завершена» и остановись.
3. Если current_step больше 0, команда `git log --oneline -1 --grep "^script_vm queue step <current_step>:"` обязана найти коммит. Не нашла — status blocked, consult_count не меняй, last_result «нет коммита шага <current_step>», остановись. Sonnet не вызывай.
4. Если status равен blocked и consult_count не 0 — API не вызывай, шаг не повторяй, остановись: после консультации нужен человек.
5. Если status равен consulting, или status равен blocked и consult_count равен 0 — код не правь. Выполни раздел «Консультация при остановке» и остановись. Sonnet только через `ruby /home/jenya/workspaces/current/mlc-support/review_script_vm_queue_consult.rb`. Не вызывай субагента Cursor и не зови другую модель.
6. Если status равен consult_ready — прочитай consult_response и найди строку `verdict:`. human, пустой verdict или over_budget: status blocked, код не меняй, остановись. retry или patch: один раз выполни шаг next_step по совету, только внутри allowed_files. Совет про compiler/ или lib/mlc/ не выполняй.
7. Иначе сразу запиши в STATE: status: in_progress (остальные поля не менять) и выполни ровно шаг next_step.
8. Следуй полям шага: depends уже закоммичены; правь только allowed_files; red до green; запусти gate точно как записано; выполни sabotage, если он не «none», и верни файл в исходное состояние (`git checkout -- <файл>` для файла, закоммиченного ранее, иначе обратной правкой), затем повтори gate и убедись, что он снова зелёный.
9. Не редактируй docs/PLAN.md, docs/agent/SESSION.md, docs/agent/CONTINUITY.md, compiler/ (включая compiler/vm/), lib/mlc/, misc/textui/, misc/editor/. Не запускай compiler/build.sh, self-host diff, пересборку compiler/out/mlcc. Чужие незакоммиченные правки в рабочем дереве не трогай.
10. Gate прошёл: обнови STATE по разделу «Протокол состояния» (current_step = N, status ready, consult_count 0, consult_response и consult_verdict пустые, last_result, last_verify, next_step = N+1; после шага 100 — status queue_complete, next_step none). Затем закоммить только файлы шага и STATE (и PLAN100, если шаг дописывал Decision log) явным списком в `git add`; сообщение `script_vm queue step <N>: <заголовок шага>` (для red-шага с суффиксом ` (red)`); без --trailer, без push.
11. Gate не прошёл: next_step и consult_count не меняй, status blocked, в last_result причина, в last_verify упавшая команда и первые строки вывода. Не коммить код. Sonnet в этом ходу не вызывай. Следующий шаг не начинай.
12. Один шаг за ход. После коммита, консультации или blocked остановись и ответь коротко: номер шага, команда gate или путь consult_response, исход.
```

## Фазы

| Фаза | Шаги | Число | Цель |
|------|------|-------|------|
| 1. Анализ, решения, характеризация, стенд | 1–16 | 16 | Заморозить решения (falsy/Nil, ключ поля, место lowering, хранение состояния, лимиты/GC/коды), завести runner/gate/size-скрипты, зафиксировать текущее поведение тестами и записать базовые наносекунды |
| 2a. Пол производительности: регистры и фреймы | 17–33 | 17 | Изменяемый регистровый файл и стек фреймов (`VmState`), единый исполнитель вместо двух, замер до/после |
| 3. Структурное разбиение | 34–38 | 5 | `heap.mlc`, `embedding.mlc` разбить до ≤ 400 строк, поведение неизменно; затем строгий size-gate без allowlist |
| 2b. Пол производительности: куча | 39–47 | 9 | Слоты кучи и элементы — непрерывные буферы, pop-reuse freelist, итеративный mark, замер |
| 4. Пробелы корректности | 48–61 | 14 | Контракт falsy, DIV `INT_MIN / -1`, лимит инструкций, GC с корнями-стеком, лимит кучи, ужесточение верификатора и загрузки blob |
| 5. Lowering подмножества | 62–88 | 27 | `script_vm/lower/`: билдер функций, кодирование модуля, int/bool локалы, арифметика, if/while, вызовы, записи по индексу поля, диагностики, CLI, дифференциал с нативным mlcc |
| 6. Завершение embedding | 89–95 | 7 | Лимиты через C ABI, C-хост с blob от lowerer, жизненный цикл handle, замер стоимости вызова |
| 7. Критика и регрессия | 96–100 | 5 | Единый gate, sabotage-проходы, чистая пересборка и аудит области правок, список «чего ещё нет» |

Шаги 2a → 3 → 2b идут в таком порядке сознательно: ratchet размеров запрещает рост `heap.mlc`, поэтому разбиение кучи стоит до её переписывания.

Сквозные заморозки (решения S2–S5 содержат полный текст):

- falsy: прыжок JUMP_IF_FALSE только при `Bool(false)`; `Int32(0)`, `Float64(0.0)`, HeapRef — не прыгают; `Nil` — `RunErr("type")`;
- поле записи = индекс по порядку объявления, имён в рантайме нет, blob не меняется;
- lowering живёт в `script_vm/lower/`, читает `compiler/frontend/` как библиотеку, `compiler/**` не правит;
- изменяемое состояние — `ref mut` + `.set`, без `.pop()`; публичный функциональный API heap/interpreter сохраняется;
- лимиты: `instruction_limit` (код 9), `heap_limit_objects` (код 10), `0` = без лимита.

## Шаги

### Фаза 1 — Анализ, решения, характеризация, стенд (шаги 1–16, всего 16)

### Step 1 — Инвентаризация фактов (S1)

- kind: analysis
- depends: нет
- goal: записать в Decision log проверенную по коду таблицу фактов; расхождения с разделом «Проверенные факты» исправить
- allowed_files: `PLAN100` (только Decision log), `STATE`
- forbidden: F0; любые правки `script_vm/**`
- contract: кода не меняется; каждое утверждение S1 подтверждено командой `rg`/`wc`/чтением
- red: none (analysis)
- green: прочитать `script_vm/{value,bytecode,verifier,interpreter,heap,embedding}.mlc`, `runtime/include/mlc/script_vm/embedding_abi.h`, `runtime/src/script_vm/embedding_abi.cpp`, `docs/agent/TRACK_MLC_SCRIPT_VM.md`, `docs/MLC_SCRIPT_VM.md` §4, §6, §8, §10, §14. Дописать `### S1 — Факты`: (a) `wc -l script_vm/*.mlc script_vm/tests/*.mlc`; (b) таблицу «опкод 1–25 × {execute_with_heap, execute_program}: handled|unsupported» (проверить `rg 'instruction_opcode_\w+\(\) then' script_vm/interpreter.mlc` по диапазонам строк каждой функции); (c) список дыр из раздела «Проверенные факты» с номерами строк и пометкой `CONFIRMED` или `CORRECTED: <как на самом деле>`; (d) значения кодов `emb_*` и `run_err_*` как есть
- gate: `rg -c '^### S1 ' docs/agent/SCRIPT_VM_QUEUE_100.md` → `1`; `git diff --stat` затрагивает только `PLAN100`, `STATE`
- sabotage: none — анализ без исполняемого кода
- performance: not a perf step
- done_when: в S1 есть три перечисленные таблицы/списка и ни одной строки без пометки CONFIRMED/CORRECTED
- state_update: `last_verify` = команда gate и `1`

### Step 2 — Заморозка контракта falsy/Nil (S2)

- kind: decision
- depends: 1
- goal: зафиксировать текст нового контракта JUMP_IF_FALSE, не меняя поведение кода
- allowed_files: `PLAN100` (Decision log), `STATE`
- forbidden: F0; `script_vm/**`
- contract: до шагов 48–49 поведение `value_is_falsy` не меняется
- red: none (decision)
- green: дописать `### S2 — falsy/Nil`: «JUMP_IF_FALSE A: если `r[A]` = `Bool(false)` — переход; `Bool(true)`, любой `Int32` (включая 0), любой `Float64` (включая 0.0 и NaN), любой HeapRef — переход не выполняется; `Nil` — `RunErr("type", primary)`. Основание: `docs/MLC_SCRIPT_VM.md` §14 (нет неявных приведений, нет повсеместного null); Nil — значение пустого регистра, не значение языка. Текущее поведение (`value_is_falsy` в `interpreter.mlc:214`: Nil/false/Int32(0)/Float64(0.0) falsy) фиксируется characterization-тестами шагов 10 под именами `legacy_falsy_*` и заменяется в шагах 48–49. Lowerer всегда подаёт в JUMP_IF_FALSE результат сравнения или bool-операции»
- gate: `rg -c '^### S2 ' docs/agent/SCRIPT_VM_QUEUE_100.md` → `1`; `git diff --stat` только два дока
- sabotage: none — решение
- performance: not a perf step
- done_when: S2 содержит перечисленные шесть исходов (false, true, Int32, Float64, HeapRef, Nil) по одному
- state_update: команда gate и `1`

### Step 3 — Заморозка ключа поля записи (S3)

- kind: decision
- depends: 1
- goal: зафиксировать, что поле = индекс, и где живёт таблица имён
- allowed_files: `PLAN100` (Decision log), `STATE`
- forbidden: F0; `script_vm/**`
- contract: формат blob `MLSC` v1 и опкоды GET_PROP/SET_PROP/NEW_RECORD не меняются
- red: none (decision)
- green: дописать `### S3 — ключ поля`: (1) ключ поля — `u8`-индекс (`GET_PROP C`, `SET_PROP B`, `NEW_RECORD B` = число полей, ≤ 255); (2) раскладка полей записи = порядок объявления в `type Name = { ... }`; (3) таблица `имя поля → индекс` существует только в lowerer (`LowerRecordLayout { type_name: string, field_names: [string] }`), в blob и в рантайме имён нет; (4) запись в script-профиле — объект кучи с ссылочной семантикой (`let b = a` разделяет объект), это расхождение с нативной value-семантикой MLC принято и вносится в список шага 100; (5) поля подмножества — только `i32` и `bool`; тип с > 255 полей — диагностика `record_too_wide`; (6) символы/интернированные имена как ключи — вне очереди
- gate: `rg -c '^### S3 ' docs/agent/SCRIPT_VM_QUEUE_100.md` → `1`; diff только два дока
- sabotage: none — решение
- performance: not a perf step
- done_when: S3 содержит пункты (1)–(6)
- state_update: команда gate и `1`

### Step 4 — Заморозка места и объёма lowering (S4)

- kind: decision
- depends: 1, 3
- goal: зафиксировать модули, вход, подмножество и коды диагностик lowering
- allowed_files: `PLAN100` (Decision log), `STATE`
- forbidden: F0; `script_vm/**`
- contract: `compiler/**` не правится; source-загрузка через C ABI остаётся `emb_unsupported`
- red: none (decision)
- green: дописать `### S4 — lowering`: (1) каталоги: `script_vm/lower/lower_types.mlc`, `lower_builder.mlc`, `lower_scope.mlc`, `lower_records.mlc`, `lower_expression.mlc`, `lower_statement.mlc`, `lower_function.mlc`, `lower_module.mlc`; `script_vm/bytecode_wide.mlc`; `script_vm/tools/script_tool.mlc`; фикстуры `script_vm/tests/fixtures/*.mlc`; (2) вход — текст MLC; `tokenize` и `parse_program_with_errors` читаются из `compiler/frontend/` как библиотека (импорт `../../compiler/frontend/...`), никаких правок там; если шаг 9 покажет, что импорт невозможен — очередь `blocked`, решение за пользователем; (3) подмножество: top-level `fn`/`export fn` с параметрами и результатом `i32` или `bool`; `type Name = { f: i32, g: bool }`; выражения: литералы i32/bool, идентификатор, унарный `-`, `!`, бинарные `+ - * /`, `== != < <= > >=` (только i32 — bool-равенство VM не поддерживает: диагностика `bool_equality_unsupported`), `&&`, `||`, `if/else` (оба вида), вызов top-level функции по имени, литерал записи, чтение поля, присваивание локалу и полю; операторы: `const`, `let`, `let mut`, присваивание (`StmtExpr(ExprBin("=", ...))`), `while`, `return`; тело `do ... end` с финальным выражением; (4) не лоуэрятся: extern, trait, generics, строки, массивы, `match`, `for`, лямбды, `break`/`continue`, float, методы, `?`, spawn/with/scope/region; каждая такая конструкция даёт диагностику `unsupported_<вид>`; (5) блок со statements в позиции выражения — диагностика `unsupported_expression_block`; (6) экспортируются только `export fn` с параметрами/результатом `i32`/`bool`, иначе `export_signature`; (7) регистры: параметры 0..n-1, затем локалы, затем временные стеком; > 256 регистров — `too_many_registers`; (8) CALL: непрерывный блок временных `[функция, аргумент1..аргументN]`, функция — `MAKE_CLOSURE tmp, proto_index, 0`, результат в целевой регистр; (9) JUMP/JUMP_IF_FALSE lowerer всегда выпускает в wide-форме (2 слова) — размер инструкции не зависит от смещения, патч безопасен; (10) выход — `LowerResult { diagnostics: [LowerDiagnostic], bytes: [i32] }`, `LowerDiagnostic { code: string, message: string, line: i32, column: i32 }`; (11) полный список кодов: `syntax_error`, `unsupported_<вид>`, `unknown_identifier`, `unknown_function`, `call_arity`, `assign_immutable`, `unknown_record`, `unknown_field`, `record_missing_field`, `record_too_wide`, `export_signature`, `too_many_registers`, `type_mismatch`, `bool_equality_unsupported`; (12) CLI `script_tool` — `compile <вход.mlc> <выход.txt>` (blob десятичными байтами через пробел) и `run <вход.mlc> <экспорт> [целые аргументы]`
- gate: `rg -c '^### S4 ' docs/agent/SCRIPT_VM_QUEUE_100.md` → `1`; diff только два дока
- sabotage: none — решение
- performance: not a perf step
- done_when: S4 содержит пункты (1)–(12)
- state_update: команда gate и `1`

### Step 5 — Заморозка хранения состояния, лимитов, GC и кодов (S5)

- kind: decision
- depends: 1
- goal: зафиксировать модель изменяемого состояния и политику лимитов
- allowed_files: `PLAN100` (Decision log), `STATE`
- forbidden: F0; `script_vm/**`
- contract: публичный функциональный API `heap_*` и `run_*` остаётся — тесты импортируют его
- red: none (decision)
- green: дописать `### S5 — состояние, лимиты, GC`: (1) изменяемое состояние — `ref mut`-параметры и `.set(index, value)` по полям записи (образец: `register_decl_into(registry: ref mut TypeRegistry, ...)` в `compiler/checker/registry.mlc`); `mlc::Array::set` копирует буфер только если он разделяется; (2) у массивов нет `.pop()` — стек = массив + явный счётчик (`stack_top`, `frame_depth`, `free_object_count`); (3) `VmState { stack, stack_top, frame_proto, frame_program_counter, frame_base, frame_closure, frame_return_register, frame_depth, instruction_count, error_code, error_at, return_value }`; (4) `Heap`: слот — только скаляры (`header`, `child`, `element_start`, `element_count`, `size_class`, `live`), элементы — один `element_arena: [Value]`, свободные id — `free_object_ids` + `free_object_count`, `gc_threshold`; (5) новые функции кучи — префикс `heap_in_place_`, принимают `heap: ref mut Heap`; функциональные `heap_alloc_*`/`heap_write_barrier`/`heap_collect` реализуются поверх них копией на границе; (6) если шаг 17 покажет, что `.set` через `ref mut` копирует буфер, очередь `blocked` — переход на внешний буфер через C++ ABI без решения пользователя не делается; (7) `RunLimits { instruction_limit: i64, heap_limit_objects: i32 }`, `0` = без лимита; лимит инструкций L допускает ровно L исполненных инструкций, `RunErr("instruction_limit", word_index_следующей_инструкции)`; (8) `RunErr("heap_limit", primary)`, лимит L допускает не более L живых объектов после сборки; (9) автосборка на safepoint аллокации (NEW_ARRAY, NEW_RECORD, MAKE_CLOSURE): при `heap_limit_objects > 0` — когда `live_count >= лимит`; при `0` — когда `live_count >= gc_threshold` (старт 1024, после сборки `max(1024, 2 * live_count)`); корни = `heap.roots` + `stack[0 .. stack_top)` + `frame_closure[0 .. frame_depth)`; (10) коды embedding: `emb_instruction_limit() = 9`, `emb_heap_limit() = 10`, `emb_overflow() = 11`, `emb_div_zero() = 12` (сейчас `run_err_to_code` отображает `overflow`/`div_zero`/`no_return`/`unsupported_opcode` в `emb_type()`; после шага 90 `overflow` и `div_zero` получают свои коды, остальные остаются `emb_type()`); в ABI `instruction_limit` — `uint64_t`, MLC-сторона принимает `i64`; бридж клампит значение > `INT64_MAX` к `INT64_MAX`; `0` = без лимита; (11) числа: только Int32 (переполнение = `RunErr("overflow")`) и Float64; `INT_MIN / -1` = `overflow`; BigInt нет; (12) бенчи: B1 `sum_loop_heap` (`run_arithmetic`, N=60000, метрика `ns_per_insn`, инструкций `4 + 5*N + 3`), B2 `sum_loop_frames` (`run_program`, то же), B3 `fib20` (`run_program`, метрика `ns_per_call`, 21891 вызов), B4 `record_alloc` (`run_with_heap`, N=1000 и N=4000: `ns_per_alloc_n1000`, `ns_per_alloc_n4000`, `scaling_ratio_x100 = ns_per_alloc_n4000*100/ns_per_alloc_n1000`)
- gate: `rg -c '^### S5 ' docs/agent/SCRIPT_VM_QUEUE_100.md` → `1`; diff только два дока
- sabotage: none — решение
- performance: not a perf step
- done_when: S5 содержит пункты (1)–(12)
- state_update: команда gate и `1`

### Step 6 — Универсальный раннер unit-тестов

- kind: green
- depends: 1
- goal: один Ruby-скрипт собирает и запускает любой `script_vm/tests/<имя>_unit.mlc` через mlcc + `build_bin.sh`
- allowed_files: `scripts/run_script_vm_unit.rb`, `script_vm/tests/runner_selfcheck_unit.mlc`, `STATE`
- forbidden: F0; `compiler/out/mlcc` пересборка
- contract: смерть бинаря по сигналу (segfault, abort, Timeout-kill) — провал, exit 0 обязателен; поведение как у `scripts/run_script_vm_interpreter_arithmetic_unit.sh`: `ENV['TMPDIR']=<root>/tmp`, `MLCC_OBJ_CLEAN=1`, `MLCC_PCH=0`, `compiler/out/mlcc -o tmp/script_vm_<имя> <entry>`, `compiler/build_bin.sh tmp/script_vm_<имя> tmp/script_vm_<имя>/bin`; запуск бинаря с `Timeout` 60 с; exit 0 только если бинарь вернул 0 и stdout содержит `[script_vm] <имя>_unit ok`; stdout бинаря проксируется; последняя строка раннера `unit ok <имя>`
- red: `ruby scripts/run_script_vm_unit.rb script_vm/tests/runner_selfcheck_unit.mlc` до создания скрипта → exit ≠ 0 (`No such file`)
- green: создать `runner_selfcheck_unit.mlc` (`fn main() -> i32` печатает `[script_vm] runner_selfcheck_unit ok`, возвращает 0) и `run_script_vm_unit.rb` (аргумент — путь entry; имя берётся из имени файла без `_unit.mlc`)
- gate: `RUN(runner_selfcheck)`; затем `ruby scripts/run_script_vm_unit.rb script_vm/tests/missing_unit.mlc` → exit ≠ 0
- sabotage: временно заменить в `runner_selfcheck_unit.mlc` печатаемую строку на `ok2`, `RUN(runner_selfcheck)` обязан упасть (exit ≠ 0, «missing ... ok»); вернуть
- performance: not a perf step
- done_when: gate-команды дали ожидаемые коды; sabotage упал и откатан
- state_update: обе gate-команды и коды выхода

### Step 7 — Агрегатор gate

- kind: green
- depends: 6
- goal: `scripts/run_script_vm_gate.rb [имена]` запускает зарегистрированные unit-тесты
- allowed_files: `scripts/run_script_vm_gate.rb`, `STATE`
- forbidden: F0
- contract: реестр — массив в начале скрипта: `OLD9` (имя → `bash scripts/run_script_vm_<имя>_unit.sh`) и `runner_selfcheck` (→ `run_script_vm_unit.rb`); новые unit-тесты добавляются в реестр их green-шагом; неизвестное имя → exit 2; без аргументов — все; печатает по строке `unit <имя> ok|FAIL`, в конце `script_vm_gate ok units=<n>`; при первом FAIL — exit 1 и `script_vm_gate FAIL <имя>` (остальные не запускаются)
- red: `ruby scripts/run_script_vm_gate.rb --list` до создания скрипта → exit ≠ 0
- green: создать скрипт; поддержать `--list` (печатает реестр, exit 0)
- gate: `ruby scripts/run_script_vm_gate.rb runner_selfcheck` → последняя строка `script_vm_gate ok units=1`; затем `GATE(OLD9)` → `script_vm_gate ok units=9`
- sabotage: передать несуществующее имя: `ruby scripts/run_script_vm_gate.rb nonexistent` → exit 2
- performance: not a perf step
- done_when: обе gate-команды зелёные, sabotage даёт 2
- state_update: обе gate-команды и итоговые строки

### Step 8 — Size-gate с храповиком

- kind: green
- depends: 1
- goal: скрипт, проверяющий `script_vm/**/*.mlc` ≤ 400 строк, с временным списком уже превышающих файлов, которым рост запрещён
- allowed_files: `scripts/run_script_vm_file_size.rb`, `STATE`
- forbidden: F0
- contract: лимит 400; allowlist (временный, до шага 38): `script_vm/interpreter.mlc` 977, `script_vm/heap.mlc` 496, `script_vm/embedding.mlc` 410 — файл из списка обязан не превышать записанное число; любой другой файл > 400 → отказ; успех печатает `script_vm file size: ok`
- red: `ruby scripts/run_script_vm_file_size.rb` до создания → exit ≠ 0
- green: создать скрипт; значения allowlist взять из `wc -l` на момент шага (если числа отличаются от списка — взять фактические и записать их в `last_verify`)
- gate: `SIZE` → exit 0 и `script_vm file size: ok`
- sabotage: временно дописать одну строку в `script_vm/heap.mlc` (не коммитить) → `SIZE` обязан упасть; откатить `git checkout -- script_vm/heap.mlc`
- performance: not a perf step
- done_when: gate зелёный, sabotage красный, `git status` не содержит правки `heap.mlc`
- state_update: gate-команда и sabotage-исход

### Step 9 — Проба импорта `compiler/frontend` из `script_vm`

- kind: measure
- depends: 4, 7
- goal: проверить, что `script_vm/tests/frontend_probe_unit.mlc` импортирует лексер/парсер компилятора и разбирает `export fn main() -> i32 = 42`
- allowed_files: `script_vm/tests/frontend_probe_unit.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `frontend_probe`), `STATE`
- forbidden: F0 (в том числе любые правки `compiler/frontend/**`)
- contract: импорты `../../compiler/frontend/lexer` (`tokenize`), `../../compiler/frontend/parser/decls` (`parse_program_with_errors`), `../../compiler/frontend/ast` (`Program`, `Decl`, `DeclExported`, `DeclFn`); прочитать сигнатуры и тип `LexOut`/`ProgramParseValue` в исходниках до написания теста
- red: none — проба; если компиляция падает на разрешении модуля или конфликте имён модулей, это `blocked` с текстом ошибки (S4 п.2)
- green: unit токенизирует и разбирает источник, проверяет: ошибок лексера нет, ошибок парсера нет, 1 декларация, это `DeclExported` над `DeclFn` с именем `main`; печатает `frontend_probe=ok`
- gate: `RUN(frontend_probe)`; затем `GATE(frontend_probe)`
- sabotage: в источнике заменить `42` на `@` → unit обязан сообщить ошибки парсера/лексера и вернуть ≠ 0
- performance: not a perf step
- done_when: обе команды зелёные
- state_update: обе команды и строка `frontend_probe=ok`

### Step 10 — Characterization: арифметика, сравнения, falsy (`char_arith`)

- kind: green
- depends: 7
- goal: зафиксировать текущие результаты `run_arithmetic`/`run_with_heap` по ADD/SUB/MUL/DIV, EQ..GE, JUMP_IF_FALSE
- allowed_files: `script_vm/tests/char_arith_unit.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `char_arith`), `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: тест описывает поведение **как есть**, продуктовый код не меняется. Не включать `INT_MIN / -1` (UB). Справочные имена функций: `run_arithmetic`, `run_with_heap`, `instruction_words_binary`, `instruction_words_load_const`, `instruction_words_jump_if_false`, `instruction_words_return`
- red: none — characterization существующего поведения (тест обязан пройти сразу)
- green: unit покрывает: для каждого из ADD/SUB/MUL/DIV — Int32 успех, Float64 успех (битово), смешанные Int32/Float64 → `type`, Bool/Nil операнд → `type`; переполнение ADD/SUB/MUL → `overflow` с `word_index` ошибочной инструкции; DIV на 0 и на 0.0 → `div_zero`; DIV отрицательных `(-7)/2 = -3`; EQ/NE/LT/LE/GT/GE на Int32 и Float64, NaN: `NaN != NaN` истина, остальные ложь; смешанные/Bool → `type`; конец без RETURN → `no_return` с `word_index = words.length()`; `verify`-ошибка пробрасывается как `RunErr("verify", ...)`; `legacy_falsy_*`: JUMP_IF_FALSE прыгает на `Nil`, `false`, `Int32(0)`, `Float64(0.0)` и не прыгает на `Int32(1)`, `true`
- gate: `RUN(char_arith)`; `GATE(char_arith interpreter_arithmetic control_flow)`
- sabotage: временно в unit изменить ожидаемое для `legacy_falsy_int_zero` на «не прыгает» → `RUN(char_arith)` обязан упасть; вернуть
- performance: not a perf step
- done_when: unit зелёный, sabotage красный
- state_update: обе команды и число проверок, напечатанное unit

### Step 11 — Characterization: массивы и записи (`char_heap_ops`)

- kind: green
- depends: 7
- goal: зафиксировать результаты NEW_ARRAY/GET_INDEX/SET_INDEX/NEW_RECORD/GET_PROP/SET_PROP и счётчики кучи через `run_with_heap`
- allowed_files: `script_vm/tests/char_heap_ops_unit.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `char_heap_ops`), `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: поведение как есть; ошибки несут `word_index` инструкции; справочные функции: `run_with_heap`, `heap_new`, `heap_live_object_count`, `heap_write_barrier_hits`, `instruction_words_new_array`, `instruction_words_get_index`, `instruction_words_set_index`, `instruction_words_new_record`, `instruction_words_get_prop`, `instruction_words_set_prop`
- red: none — characterization
- green: unit покрывает: round-trip массива и записи; NEW_ARRAY с отрицательной длиной → `bounds`; с не-Int32 длиной → `type`; GET_INDEX/SET_INDEX: индекс < 0, ≥ длины → `bounds`, индекс не Int32 → `type`, операнд не массив (запись, Int32) → `type`; GET_PROP/SET_PROP: поле ≥ числа полей → `bounds`, объект не запись → `type`; `heap_write_barrier_hits` после N SET_* равен N; `heap_live_object_count` после K NEW_* равен K; ссылочная семантика: два регистра на один объект видят запись друг друга
- gate: `RUN(char_heap_ops)`; `GATE(char_heap_ops arrays_records)`
- sabotage: временно изменить ожидание `heap_write_barrier_hits` на N+1 → `RUN(char_heap_ops)` падает; вернуть
- performance: not a perf step
- done_when: unit зелёный, sabotage красный
- state_update: обе команды и число проверок

### Step 12 — Characterization: фреймы, замыкания, пробелы `run_program` (`char_frames`)

- kind: green
- depends: 7
- goal: зафиксировать поведение `run_program`/`run_program_with_args`, включая известные пробелы
- allowed_files: `script_vm/tests/char_frames_unit.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `char_frames`), `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: поведение как есть. Пробелы закрепляются тестами `gap_pin_*` (их ожидания перевернёт шаг 32). Справочно: `FunctionProto`, `run_program`, `run_program_with_args`, `heap_alloc_closure`, `instruction_words_call`, `instruction_words_make_closure`, `instruction_words_get_upval`, `instruction_words_set_upval`, `instruction_upvalue_source_word`; образец рекурсии — `script_vm/tests/closures_fibers_unit.mlc` (раздел «Recursion depth 8»)
- red: none — characterization
- green: unit покрывает: CALL/RETURN с передачей аргументов и записью результата в `A` вызывающего; изоляция регистров вызывающего и вызываемого; MAKE_CLOSURE/GET_UPVAL/SET_UPVAL (захват локала, захват через родителя); рекурсия глубины 8; глубина ≥ 256 → `RunErr("stack")`; CALL нейтрального значения (Int32) → `type`; CALL закрытия с `proto_index` вне диапазона → `type`; GET_UPVAL вне замыкания → `type`/`bounds`; `run_program` с `entry_proto_index` вне диапазона → `type`; `verify`-ошибка в любом прото → `RunErr("verify", ...)`; `gap_pin_mul_unsupported`: MUL в `run_program` → `unsupported_opcode`; `gap_pin_eq_unsupported`: EQ → `unsupported_opcode`; `gap_pin_new_array_unsupported`: NEW_ARRAY → `unsupported_opcode`
- gate: `RUN(char_frames)`; `GATE(char_frames closures_fibers)`
- sabotage: временно изменить ожидание `gap_pin_mul_unsupported` на `RunOk` → падает; вернуть
- performance: not a perf step
- done_when: unit зелёный, sabotage красный
- state_update: обе команды и число проверок

### Step 13 — Characterization: декодирование blob и коды embedding (`char_blob`)

- kind: green
- depends: 7
- goal: зафиксировать коды `emb_*` для разных повреждений blob и сам байтовый образ `emb_encode_add1_module()`
- allowed_files: `script_vm/tests/char_blob_unit.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `char_blob`), `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: поведение как есть; справочно: `emb_encode_add1_module`, `emb_verify_module_bytes`, `emb_call_export_bytes`, `emb_ok`, `emb_unsupported`, `emb_verify`, `emb_format`, `emb_not_found`, `emb_type`
- red: none — characterization
- green: unit покрывает: длина и первые 16 байт add1-blob записываются литералом (значения снять с текущей реализации и вписать); короче 8 байт → `emb_format`; чужая магия → `emb_unsupported`; версия ≠ 1 → `emb_format`; обрезка на каждом смещении 0..len-1 никогда не даёт `emb_ok` и не падает; лишний хвост → `emb_format`; blob с неверным регистром (verify fail) → `emb_verify`; неизвестный экспорт → `emb_not_found`; вызов add1(41) = 42; вызов add1 без аргументов: результат фиксируется как есть (регистр 0 = Nil → `type`-код, записать наблюдаемый код)
- gate: `RUN(char_blob)`; `GATE(char_blob embedding_abi)`
- sabotage: изменить в unit ожидаемый байт магии → падает; вернуть
- performance: not a perf step
- done_when: unit зелёный, sabotage красный
- state_update: обе команды и число проверок

### Step 14 — Фикстуры для бенчмарков (`bench_fixtures`)

- kind: green
- depends: 10, 12
- goal: модуль готовых программ для бенчей с проверкой результатов
- allowed_files: `script_vm/tests/bench_fixtures.mlc`, `script_vm/tests/bench_fixtures_unit.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `bench_fixtures`), `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: экспорт `fixture_sum_loop(iteration_limit: i32) -> FunctionProto` (регистры: sum, i, n, one, cond; тело `LE cond,i,n / JUMP_IF_FALSE cond,end / ADD sum,sum,i / ADD i,i,one / JUMP loop / RETURN sum`; образец — `control_flow_unit.mlc`); `fixture_sum_loop_instruction_count(iteration_limit) = 4 + 5 * iteration_limit + 3`; `fixture_fib_protos() -> [FunctionProto]` и `fixture_fib_call_count(n) -> i32`; `fixture_record_alloc_loop(iteration_count: i32) -> FunctionProto` (на каждой итерации NEW_RECORD с 2 полями и SET_PROP, возвращает счётчик итераций)
- red: none — тестовая инфраструктура; поведение VM не меняется
- green: unit проверяет: sum 1..100 = 5050 через `run_arithmetic`; sum 1..60000 = 1800030000 (укладывается в Int32) и через `run_program` тем же значением; fib(10) = 55 через `run_program` с замыканием, созданным MAKE_CLOSURE в прото 0; `fixture_fib_call_count(10) = 177`, `fixture_fib_call_count(20) = 21891`; record-цикл на 1000 итераций даёт 1000 и `heap_live_object_count` = 1000
- gate: `RUN(bench_fixtures)`; `GATE(bench_fixtures)`
- sabotage: изменить в `fixture_sum_loop` `ADD sum,sum,i` на `SUB` → unit падает; вернуть
- performance: not a perf step
- done_when: unit зелёный, sabotage красный
- state_update: обе команды

### Step 15 — Бенч-вход и раннер бенчей

- kind: green
- depends: 14
- goal: `bench_entry.mlc` печатает метрики B1–B4; `run_script_vm_bench.rb` берёт минимум из 5 запусков и сравнивает с эталоном
- allowed_files: `script_vm/tests/bench_entry.mlc`, `scripts/run_script_vm_bench.rb`, `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: формат строки бенча `[script_vm] bench name=<B1|B2|B3|B4> metric=<имя> value=<целое>`; метрики из S5 п.12; раннер: одна сборка (`mlcc` + `build_bin.sh` в `tmp/script_vm_bench`), 5 запусков бинаря, по `(name, metric)` минимум; опции: `--record baseline|current` (пишет `script_vm/tests/bench_<which>.txt`, строки `name.metric value`), `--require-lower name.metric=baseline|current` (строго меньше эталона, иначе exit 1), `--max-ratio name.metric=baseline|current:<процент>` (значение*100 ≤ эталон*процент), `--max-absolute name.metric=<число>`; без опций печатает значения. Каждый замер ≤ 2 с (правило 10)
- red: `ruby scripts/run_script_vm_bench.rb` до создания → exit ≠ 0
- green: создать оба файла; B4 запускает `run_with_heap(fixture_record_alloc_loop(N))` для N=1000 и N=4000; `ns_per_alloc = total_us * 1000 / N`
- gate: `BENCH()` без опций → exit 0, напечатано ровно 6 значений: `B1.ns_per_insn`, `B2.ns_per_insn`, `B3.ns_per_call`, `B4.ns_per_alloc_n1000`, `B4.ns_per_alloc_n4000`, `B4.scaling_ratio_x100`
- sabotage: записать вручную `script_vm/tests/bench_current.txt` со значением `B1.ns_per_insn 1` и вызвать `BENCH(--require-lower B1.ns_per_insn=current)` → exit 1; удалить временный файл
- performance: печатает базовые числа, но эталон ещё не записан
- done_when: gate-команда напечатала 6 значений; sabotage дал exit 1
- state_update: 6 напечатанных значений

### Step 16 — Запись baseline (S16)

- kind: measure
- depends: 15
- goal: записать эталонные наносекунды до любых изменений исполнителя
- allowed_files: `script_vm/tests/bench_baseline.txt`, `script_vm/tests/bench_current.txt`, `PLAN100` (Decision log), `STATE`
- forbidden: F0; `script_vm/*.mlc` и любые правки исполнителя; baseline после этого шага не перезаписывается никогда
- contract: числа — только то, что печатает бенч; не придумывать
- red: none — замер
- green: `BENCH(--record baseline)` затем `BENCH(--record current)`; дописать `### S16 — baseline` с 6 числами, датой и машиной (`uname -m`, `nproc`)
- gate: `test -s script_vm/tests/bench_baseline.txt` и `wc -l script_vm/tests/bench_baseline.txt` → `6`; `BENCH(--max-ratio B1.ns_per_insn=baseline:300)` → exit 0 (проверка, что раннер читает файл)
- sabotage: `BENCH(--require-lower B1.ns_per_insn=baseline)` сразу после записи обязан дать exit 1 (значение не строго меньше самого себя)
- performance: эти 6 чисел — эталон для шагов 19, 32, 33, 46, 47
- done_when: оба файла есть, S16 дописан
- state_update: 6 чисел baseline

### Фаза 2a — Пол производительности: регистры и фреймы (шаги 17–33, всего 17)

### Step 17 — Проба: запись через `ref mut` не копирует буфер

- kind: measure
- depends: 7, 5
- goal: подтвердить допущение S5 п.1 измерением
- allowed_files: `script_vm/tests/inplace_probe_unit.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `inplace_probe`), `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: определить `type Cells = { values: [Value] }` и `fn cells_set(cells: ref mut Cells, index: i32, value: Value) -> unit = do cells.values.set(index, value) end` (после проверки, что синтаксис принимается mlcc); три проверки: (1) видимость — запись внутри `cells_set` видна вызывающему; (2) COW сохранён — после `let mut copy = cells` запись в `copy` не меняет `cells`; (3) масштабирование — 200000 вызовов `cells_set` на `Cells` из 1000 и из 16000 элементов; печатает `probe_inplace_ns_per_set_small`, `probe_inplace_ns_per_set_large`, `probe_inplace_scaling_ratio_x100 = large*100/small`
- red: none — проба
- green: unit возвращает ≠ 0, если (1) или (2) нарушены или `probe_inplace_scaling_ratio_x100 > 300`
- gate: `RUN(inplace_probe)`; `GATE(inplace_probe)`
- sabotage: заменить в `cells_set` запись на полную пересборку (`let mut next = cells.values; next.set(index, value); cells.values = next`) → `probe_inplace_scaling_ratio_x100` обязан вырасти и unit упасть; вернуть
- performance: числа печатаются, эталона нет; пороги — проверка линейности, не заявка ускорения
- done_when: unit зелёный. Если красный: `blocked`, в `last_result` написать «S5 п.6: .set через ref mut копирует; нужно решение пользователя»
- state_update: три напечатанных числа

### Step 18 — Проба языковых остатков mlcc (S18)

- kind: measure
- depends: 17
- goal: записать, какие конструкции mlcc на самом деле поддерживает, чтобы остальные шаги не полагались на непроверенное
- allowed_files: `script_vm/tests/language_probe_unit.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `language_probe`), `PLAN100` (Decision log), `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: unit печатает по строке `probe_<имя>=ok|broken` и всегда возвращает 0 (измерение, а не проверка). Пробы: `tail_after_if_chain` (операторы после `if/else-if/else` в `do`-блоке исполняются, для 3 веток); `return_in_while_in_ref_mut` (`return` из `while` внутри функции с `ref mut` параметром); `ref_mut_whole_assignment` (в функции `fn reset(cells: ref mut Cells) -> unit = do cells = Cells { values: [] } end` присваивание целого значения видно вызывающему); `nested_field_set` (`state.inner.values.set(0, x)` через `ref mut state` с вложенной записью); `array_set_in_while` (`.set` в цикле по `ref mut` полю); `match_in_while` (match по sum-типу в `while`); `two_ref_mut_params` (функция с двумя `ref mut` параметрами разных типов)
- red: none — измерение
- green: дописать `### S18 — языковые пробы` в Decision log: таблица `проба → ok|broken` и для каждой `broken` правило обхода: `tail_after_if_chain` → цепочку выносить в функцию, возвращающую значение; `ref_mut_whole_assignment` → обработчики возвращают результат структурой, а не присваивают `ref mut` целиком; `nested_field_set` → состояние хранить плоскими полями; `two_ref_mut_params` → объединить в один `ref mut` контейнер
- gate: `RUN(language_probe)`; `rg -c '^### S18 ' docs/agent/SCRIPT_VM_QUEUE_100.md` → `1`
- sabotage: none — измерение возможностей компилятора, не проверка кода
- performance: not a perf step
- done_when: все 7 строк `probe_*` напечатаны и перенесены в S18
- state_update: 7 строк `probe_*`

### Step 19 — `execute_with_heap`: запись регистров через `.set`

- kind: green
- depends: 10, 11, 16, 17
- goal: первый измеримый срез пола производительности — убрать `registers_set` из одиночного исполнителя
- allowed_files: `script_vm/interpreter.mlc`, `STATE`
- forbidden: F0; `execute_program` и его `registers_set`/`frames_set_last` не трогать
- contract: результаты всех опкодов неизменны (characterization `char_arith`, `char_heap_ops`); `registers` в `execute_with_heap` — единственный владелец буфера (локальный `let mut`), запись через `registers.set(decoded.a, value)`
- red: none — рефакторинг скорости; поведение закреплено тестами шагов 10, 11
- green: в функции `execute_with_heap` (`interpreter.mlc:295–533`) заменить каждый `registers = registers_set(registers, X, Y)` на `registers.set(X, Y)` (LOAD_CONST, MOVE, ADD, SUB, MUL, DIV, EQ..GE, NEW_ARRAY, GET_INDEX, NEW_RECORD, GET_PROP). `registers_set` оставить для `execute_program`
- gate: `GATE(ARITH HEAPSET)`; `BENCH(--require-lower B1.ns_per_insn=baseline)` → exit 0
- sabotage: в обработчике ADD записать результат в `decoded.b` вместо `decoded.a` → `GATE(char_arith)` падает; вернуть
- performance: harness `BENCH` метрика `B1.ns_per_insn`; напечатать «baseline → новое» и отношение; новое строго меньше baseline
- done_when: оба gate-а зелёные, `SIZE` не хуже (храповик: `interpreter.mlc` не вырос)
- state_update: числа B1 baseline и новое

### Step 20 — Red: `VmState` (`vm_state`)

- kind: red
- depends: 18
- goal: тест нового модуля `script_vm/vm_state.mlc` до его появления
- allowed_files: `script_vm/tests/vm_state_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: API (S5 п.2–3) — `export type VmState` с полями из S5; `vm_state_new() -> VmState`; `vm_state_push_frame(state: ref mut VmState, proto_index: i32, register_count: i32, closure: Value, return_register: i32) -> unit`; `vm_state_pop_frame(state: ref mut VmState) -> unit`; `vm_state_frame_depth(state: ref mut VmState) -> i32`; `vm_state_register(state: ref mut VmState, index: i32) -> Value`; `vm_state_set_register(state: ref mut VmState, index: i32, value: Value) -> unit`; `vm_state_program_counter`, `vm_state_set_program_counter`, `vm_state_frame_proto`, `vm_state_frame_closure`, `vm_state_frame_return_register`, `vm_state_stack_capacity(state) -> i32` (= `stack.length()`), `vm_state_stack_top(state) -> i32`; (если S18 записал `two_ref_mut_params=broken`, аксессоры всё равно принимают один `ref mut VmState`)
- red: unit проверяет: свежий фрейм из 3 регистров — все `Nil`; запись в регистр 1 читается; изоляция: второй фрейм (2 регистра) не видит регистров первого, запись во второй не меняет первый, после `vm_state_pop_frame` первый фрейм цел и `vm_state_stack_top` вернулся; переиспользование: после pop и нового push тех же размеров регистр 0 снова `Nil` (стёртый); 10000 циклов push/pop не увеличивают `vm_state_stack_capacity` выше значения после первого цикла; `vm_state_frame_depth` отслеживает push/pop; program counter хранится по фреймам
- green: none — red-шаг (реализация в следующем шаге)
- gate: `RED(vm_state)` → exit ≠ 0 (ошибка компиляции об отсутствующем модуле `./vm_state`); в `last_verify` — первая строка ошибки
- sabotage: none — red-шаг: сам падающий результат и есть проверка
- performance: not a perf step
- done_when: unit написан, RED падает по отсутствию модуля, а не по ошибке синтаксиса теста (первая строка ошибки называет `vm_state`)
- state_update: команда и первая строка ошибки

### Step 21 — Green: `vm_state.mlc`

- kind: green
- depends: 20
- goal: реализовать `VmState` как непрерывные буферы с явными счётчиками
- allowed_files: `script_vm/vm_state.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `vm_state`), `STATE`
- forbidden: F0; `interpreter.mlc` не трогать
- contract: S5 п.2–3; без `.pop()`; `push_frame`: `base = stack_top`; если `stack.length() < base + register_count` — `stack.push(Nil)` до нужной длины; ячейки `[base, base + register_count)` `.set(Nil)`; `stack_top += register_count`; массивы фреймов: если `frame_depth < length` — `.set`, иначе `.push`; `frame_depth += 1`. `pop_frame`: `frame_depth -= 1`; `stack_top = frame_base[frame_depth]`. Регистр читается/пишется как `stack[frame_base[frame_depth-1] + index]`
- red: шаг 20 (`RED(vm_state)` падал)
- green: создать модуль; все аксессоры принимают `ref mut VmState`
- gate: `RUN(vm_state)`; `GATE(vm_state)`
- sabotage: в `vm_state_pop_frame` не восстанавливать `stack_top` → `RUN(vm_state)` падает; вернуть
- performance: not a perf step (микробенч — в шаге 32)
- done_when: unit зелёный, sabotage красный
- state_update: обе команды и число проверок

### Step 22 — Рефактор: `run_result.mlc`

- kind: refactor
- depends: 19
- goal: вынести типы результата и коды ошибок из `interpreter.mlc`, чтобы исполнитель не зависел от интерпретатора
- allowed_files: `script_vm/run_result.mlc`, `script_vm/interpreter.mlc`, `STATE`
- forbidden: F0
- contract: переносятся `RunResult`, `RunHeapResult`, `FunctionProto`, `run_err_verify/type/overflow/div_zero/no_return/unsupported_opcode/bounds/stack`; `interpreter.mlc` их импортирует; импорты тестов (`from '../interpreter'`) продолжают работать (правило 9); множество экспортов до/после: файл `interpreter.mlc` ∪ `run_result.mlc` равно прежнему множеству `export` `interpreter.mlc` (команда правила 9)
- red: none — refactor с characterization (шаги 10–13)
- green: создать `run_result.mlc` (импортирует `Heap` из `./heap`, `Value` из `./value`); `interpreter.mlc` импортирует перенесённое
- gate: `GATE(OLD9 char_arith char_heap_ops char_frames)`
- sabotage: изменить значение `run_err_type()` на `"typo"` → `GATE(char_arith)` падает; вернуть
- performance: not a perf step
- done_when: gate зелёный; `wc -l script_vm/interpreter.mlc` уменьшилось
- state_update: gate-команда и итог `units=<n>`

### Step 23 — Рефактор: `arith.mlc`

- kind: refactor
- depends: 22
- goal: вынести арифметику/сравнения/falsy
- allowed_files: `script_vm/arith.mlc`, `script_vm/interpreter.mlc`, `STATE`
- forbidden: F0
- contract: переносятся `ArithOpResult`, `apply_binary`, `apply_compare`, `value_is_falsy`, `float_is_zero`, четыре `extern fn script_vm_int32_*` и `jump_target`; ставшие межмодульными функции получают `export`; значения и ошибки неизменны (`char_arith`)
- red: none — refactor с characterization
- green: создать модуль, импортирующий `value`, `bytecode` (`instruction_opcode_*`, `instruction_jump_offset`), `run_result`; `interpreter.mlc` импортирует
- gate: `GATE(OLD9 char_arith char_heap_ops char_frames)`
- sabotage: в `apply_compare` заменить `<` на `<=` для LT → `GATE(char_arith)` падает; вернуть
- performance: not a perf step; дополнительно `BENCH(--max-ratio B1.ns_per_insn=current:130)` → exit 0 (вынос не должен ломать скорость; допуск 30% — шум, не заявка)
- done_when: gate зелёный
- state_update: gate-команда, итог и значение B1

### Step 24 — Red: единый исполнитель, скелет (`unified_executor`)

- kind: red
- depends: 21, 23
- goal: дифференциальный тест нового исполнителя против старых `run_with_heap`/`run_program_with_args` на простых опкодах
- allowed_files: `script_vm/tests/unified_executor_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: API — модуль `script_vm/executor.mlc`: `run_state_single_function(heap: Heap, words: [i32], constants: [Value], register_count: i32) -> RunHeapResult` (семантика `run_with_heap`: verify, затем исполнение, `no_return` при `word_index = words.length()`); `run_state_program(heap: Heap, protos: [FunctionProto], entry_proto_index: i32, args: [Value]) -> RunHeapResult` (семантика `run_program_with_args`). Хелпер теста `check_same(label, old_result, new_result)` сравнивает `RunOk` по `raw`, `RunErr` по коду и `word_index`, и `heap_live_object_count`/`heap_write_barrier_hits`
- red: unit содержит случаи для LOAD_CONST (узкий и wide), MOVE, RETURN, JUMP вперёд/назад (узкий и wide), JUMP_IF_FALSE (взятый, не взятый, `legacy_falsy` на Nil/Int32(0)), `no_return`, `verify`-ошибку, `args` в регистры 0..k-1 при `run_state_program`, `entry_proto_index` вне диапазона
- green: none — red-шаг (реализация в следующем шаге)
- gate: `RED(unified_executor)` → exit ≠ 0 (отсутствует модуль `./executor`); первая строка ошибки в `last_verify`
- sabotage: none — red-шаг
- performance: not a perf step
- done_when: RED падает по отсутствию `./executor`
- state_update: команда и первая строка ошибки

### Step 25 — Green: `executor.mlc` — цикл и простые опкоды

- kind: green
- depends: 24
- goal: единый цикл исполнения на `VmState`, пока только LOAD_CONST/MOVE/RETURN/JUMP/JUMP_IF_FALSE
- allowed_files: `script_vm/executor.mlc`, `script_vm/executor_step.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `unified_executor`), `STATE`
- forbidden: F0; `interpreter.mlc` пока не переключать
- contract: `executor_step.mlc` — `executor_step_continue() = 0`, `executor_step_returned() = 1`, `executor_step_error() = 2`, `executor_fail(state: ref mut VmState, code: string, at: i32) -> i32` (пишет `state.error_code`, `state.error_at`, возвращает `executor_step_error()`); `run_state_program` верифицирует все прото (`verify_function`), проверяет `entry_proto_index`, создаёт `VmState`, кладёт фрейм входа и `args`, крутит цикл: `decoded = instruction_decode`, `span = instruction_word_span`, диспетчеризация по опкоду; неподдержанный опкод → `executor_fail(state, run_err_unsupported_opcode(), primary)`; конец слов → `no_return` в `words.length()`; `value_is_falsy` берётся из `arith.mlc` (легаси-поведение сохраняется до шага 49); при `RETURN` последнего фрейма — `RunOk(value)`; кучу несёт локальный `let mut heap_state`, возвращается в `RunHeapResult`
- red: шаг 24
- green: создать два модуля; регистрация unit
- gate: `RUN(unified_executor)`; `GATE(unified_executor)`
- sabotage: в обработчике JUMP поменять знак смещения → `RUN(unified_executor)` падает; вернуть
- performance: not a perf step
- done_when: unit зелёный, sabotage красный, `SIZE` зелёный
- state_update: обе команды

### Step 26 — Red: арифметика и сравнения в едином исполнителе

- kind: red
- depends: 25
- goal: расширить дифференциальный unit опкодами ADD..GE
- allowed_files: `script_vm/tests/unified_executor_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: случаи шага 10 (`char_arith`) повторяются как пары `(run_with_heap, run_state_single_function)` и `(run_program_with_args, run_state_program)` — для второй пары MUL/DIV/EQ..GE ранее в старом исполнителе кадров не поддержаны (`gap_pin_*`), поэтому для кадров ожидания берутся из `run_with_heap`
- red: `RED(unified_executor)` падает на `unsupported_opcode` для ADD (первый упавший `label` в `last_verify`)
- green: none — red-шаг (реализация в следующем шаге)
- gate: `RED(unified_executor)` → exit ≠ 0
- sabotage: none — red-шаг
- performance: not a perf step
- done_when: RED падает по `unsupported_opcode`, а не по ошибке компиляции
- state_update: команда и первая упавшая метка

### Step 27 — Green: `executor_arith.mlc`

- kind: green
- depends: 26
- goal: ADD, SUB, MUL, DIV, EQ, NE, LT, LE, GT, GE в едином исполнителе
- allowed_files: `script_vm/executor_arith.mlc`, `script_vm/executor.mlc`, `STATE`
- forbidden: F0
- contract: `executor_exec_binary(state: ref mut VmState, decoded: DecodedAbc, primary: i32, span: i32) -> i32` и `executor_exec_compare(...)`; используют `apply_binary`/`apply_compare` из `arith.mlc`; ошибка → `executor_fail(state, code, primary)`; успех → `vm_state_set_register`, `program_counter += span`
- red: шаг 26
- green: создать модуль, подключить в диспетчер `executor.mlc`
- gate: `RUN(unified_executor)`; `GATE(ARITH unified_executor)`
- sabotage: в `executor_exec_binary` для SUB вызвать с опкодом ADD → `RUN(unified_executor)` падает; вернуть
- performance: not a perf step
- done_when: оба gate зелёные, `SIZE` зелёный
- state_update: обе команды

### Step 28 — Red: вызовы и замыкания в едином исполнителе

- kind: red
- depends: 27
- goal: дифференциальные случаи CALL/RETURN/MAKE_CLOSURE/GET_UPVAL/SET_UPVAL
- allowed_files: `script_vm/tests/unified_executor_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: случаи шага 12 (`char_frames`) как пары `(run_program_with_args, run_state_program)`: передача аргументов, возврат в `A` вызывающего, изоляция регистров, рекурсия глубины 8, рекурсия до `stack` (глубина 256), CALL не-замыкания → `type`, `proto_index` вне диапазона → `type`, захват локала и захват через родителя, `heap_write_barrier_hits` после SET_UPVAL
- red: `RED(unified_executor)` падает на `unsupported_opcode` для CALL
- green: none — red-шаг (реализация в следующем шаге)
- gate: `RED(unified_executor)` → exit ≠ 0
- sabotage: none — red-шаг
- performance: not a perf step
- done_when: RED падает по `unsupported_opcode`
- state_update: команда и первая упавшая метка

### Step 29 — Green: `executor_calls.mlc`

- kind: green
- depends: 28
- goal: CALL, RETURN (с возвратом в вызывающий фрейм), MAKE_CLOSURE, GET_UPVAL, SET_UPVAL
- allowed_files: `script_vm/executor_calls.mlc`, `script_vm/executor.mlc`, `STATE`
- forbidden: F0
- contract: лимит глубины `executor_max_frame_depth() = 256`: `frame_depth >= 256` на CALL → `stack`; CALL копирует `C` аргументов из регистров `B+1..B+C` в регистры нового фрейма `0..C-1` (только если источник в пределах `register_count` вызывающего); `RETURN` не последнего фрейма: значение в `frame_return_register` вызывающего, `vm_state_pop_frame`; `fill_closure_upvalues`/`fill_one_upvalue` переезжают из `interpreter.mlc` (старые копии остаются до шага 32); кучу обработчики получают `ref mut Heap` или возвращают её структурой — по результату `ref_mut_whole_assignment` из S18
- red: шаг 28
- green: создать модуль, подключить в диспетчер
- gate: `RUN(unified_executor)`; `GATE(FRAMES unified_executor)`
- sabotage: сделать `executor_max_frame_depth()` равным 257 → `RUN(unified_executor)` падает; вернуть
- performance: not a perf step
- done_when: оба gate зелёные, `SIZE` зелёный
- state_update: обе команды

### Step 30 — Red: опкоды массивов и записей в едином исполнителе

- kind: red
- depends: 29
- goal: дифференциальные случаи NEW_ARRAY..SET_PROP
- allowed_files: `script_vm/tests/unified_executor_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: случаи шага 11 (`char_heap_ops`) как пары `(run_with_heap, run_state_single_function)`: результаты, коды и `word_index` ошибок, `heap_live_object_count`, `heap_write_barrier_hits`
- red: `RED(unified_executor)` падает на `unsupported_opcode` для NEW_ARRAY
- green: none — red-шаг (реализация в следующем шаге)
- gate: `RED(unified_executor)` → exit ≠ 0
- sabotage: none — red-шаг
- performance: not a perf step
- done_when: RED падает по `unsupported_opcode`
- state_update: команда и первая упавшая метка

### Step 31 — Green: `executor_heap.mlc`

- kind: green
- depends: 30
- goal: NEW_ARRAY, GET_INDEX, SET_INDEX, NEW_RECORD, GET_PROP, SET_PROP
- allowed_files: `script_vm/executor_heap.mlc`, `script_vm/executor.mlc`, `STATE`
- forbidden: F0
- contract: пока через функциональный API кучи (`heap_alloc_array`, `heap_write_barrier`, `heap_get_element`, `heap_element_count`, `heap_object_type_id`); порядок проверок и коды как в `execute_with_heap` (`interpreter.mlc:421–510`): NEW_ARRAY: не Int32 → `type`, длина < 0 → `bounds`; GET_INDEX/SET_INDEX: тип объекта → `type`, индекс не Int32 → `type`, диапазон → `bounds`; GET_PROP/SET_PROP: тип объекта → `type`, поле ≥ числа полей → `bounds`
- red: шаг 30
- green: создать модуль, подключить
- gate: `RUN(unified_executor)`; `GATE(HEAPSET unified_executor)`
- sabotage: в SET_INDEX не вызывать `heap_write_barrier` → `RUN(unified_executor)` падает; вернуть
- performance: not a perf step
- done_when: оба gate зелёные, `SIZE` зелёный
- state_update: обе команды

### Step 32 — Переключение `run_*` на единый исполнитель, удаление старых

- kind: refactor
- depends: 31
- goal: `run_with_heap`, `run_arithmetic`, `run_program`, `run_program_with_args` вызывают `executor.mlc`; старые исполнители удалены
- allowed_files: `script_vm/interpreter.mlc`, `script_vm/tests/char_frames_unit.mlc` (только перевернуть `gap_pin_*`), `script_vm/tests/unified_executor_unit.mlc` (удалить файл), `scripts/run_script_vm_gate.rb` (снять регистрацию `unified_executor`), `script_vm/tests/bench_current.txt`, `STATE`
- forbidden: F0
- contract: публичные имена `run_with_heap`, `run_arithmetic`, `run_program`, `run_program_with_args` и экспорты `run_err_*`, `RunResult`, `RunHeapResult`, `FunctionProto` сохранены; `run_with_heap(heap, words, constants, register_count)` = `run_state_single_function`; `run_program` = `run_state_program(heap, protos, entry, [])`; пробелы закрыты: `gap_pin_mul_unsupported`, `gap_pin_eq_unsupported`, `gap_pin_new_array_unsupported` теперь ожидают корректные результаты MUL/EQ/NEW_ARRAY в `run_program`; из `interpreter.mlc` удалены `execute_with_heap`, `execute_program`, `registers_set`, `make_registers`, `frames_set_last`, `frames_without_last`, `CallFrame`, `UpvalueFill`, `fill_closure_upvalues`, `fill_one_upvalue`, `value_is_falsy`-дубликаты
- red: none — рефакторинг с characterization (шаги 10–13)
- green: переключить, удалить, обновить тест
- gate: `GATE(all)`; `BENCH(--require-lower B1.ns_per_insn=baseline --require-lower B2.ns_per_insn=baseline --require-lower B3.ns_per_call=baseline --max-ratio B4.ns_per_alloc_n1000=baseline:130)`; затем `BENCH(--record current)`; `wc -l script_vm/interpreter.mlc` < 100
- sabotage: временно пробросить в `run_program_with_args` вместо `args` пустой массив → `GATE(char_frames)` падает; вернуть
- performance: harness `BENCH`; метрики `B1.ns_per_insn`, `B2.ns_per_insn`, `B3.ns_per_call` строго ниже baseline; в `last_verify` — таблица baseline → новое по всем шести числам
- done_when: оба gate зелёные, `bench_current.txt` перезаписан
- state_update: gate, 6 чисел baseline → новое, `wc -l`

### Step 33 — Быстрый путь Int32 без `ValueRep`

- kind: green
- depends: 32
- goal: убрать конструирование `ValueRep` в горячем пути Int32
- allowed_files: `script_vm/value.mlc`, `script_vm/arith.mlc`, `script_vm/executor_arith.mlc`, `script_vm/tests/bench_current.txt`, `STATE`
- forbidden: F0; формат NaN-boxing и `value_rep_abi.hpp` не менять
- contract: `value.mlc` получает `export fn value_is_int32(value: Value) -> bool = script_vm_decode_kind(value.raw) == 3` и `export fn value_int32(value: Value) -> i32`; `apply_binary`/`apply_compare` и `executor_exec_*` проверяют быстрый случай «оба Int32» через эти функции до общего пути; семантика переполнения/деления (`overflow`, `div_zero`) неизменна (`char_arith`); `value_rep_unit` зелёный
- red: none — перф-правка, поведение закреплено `char_arith`; проверка скорости — gate
- green: добавить две функции и быстрые ветки для ADD, SUB, MUL, DIV, EQ..GE
- gate: `GATE(ARITH value_rep)`; `BENCH(--require-lower B1.ns_per_insn=current --require-lower B2.ns_per_insn=current)`; затем `BENCH(--record current)`
- sabotage: в быстрой ветке ADD вычислять `left - right` → `GATE(char_arith)` падает; вернуть
- performance: harness `BENCH`; `B1.ns_per_insn` и `B2.ns_per_insn` строго ниже значений `current` из шага 32; числа до/после в `last_verify`
- done_when: оба gate зелёные
- state_update: gate-команды, числа до/после

### Фаза 3 — Структурное разбиение (шаги 34–38, всего 5)

### Step 34 — Рефактор: `heap_types.mlc`

- kind: refactor
- depends: 32, 33
- goal: вынести типы и базовые помощники кучи из `heap.mlc` (496 строк)
- allowed_files: `script_vm/heap_types.mlc`, `script_vm/heap.mlc`, `STATE`
- forbidden: F0
- contract: переносятся `ObjectHeader`, `HeapSlot` (получает `export`), `Heap`, `HeapAlloc`, `heap_type_cell/array/record/closure`, `heap_new`, `heap_live_object_count`, `heap_write_barrier_hits`, `heap_size_class_bytes`, `slots_with`, `elements_with`, `make_nil_elements` (получают `export`); `heap.mlc` импортирует их; публичное множество экспортов `heap*.mlc` ⊇ прежнего `heap.mlc` (правило 9: снять `rg -o '^export (fn|type) \w+' script_vm/heap.mlc | sort > tmp/heap_exports_before.txt` до правки; после — каждая строка файла присутствует в объединении `heap*.mlc`)
- red: none — refactor с characterization (`heap_gc_arena`, `char_heap_ops`)
- green: создать модуль, поправить импорты `heap.mlc`
- gate: `GATE(HEAPSET FRAMES)`; `SIZE`
- sabotage: изменить значение `heap_type_record()` на 9 → `GATE(char_heap_ops)` падает; вернуть
- performance: not a perf step
- done_when: gate зелёные, проверка набора экспортов пройдена
- state_update: gate-команды и `wc -l script_vm/heap.mlc script_vm/heap_types.mlc`

### Step 35 — Рефактор: `heap_alloc.mlc`

- kind: refactor
- depends: 34
- goal: вынести аллокацию
- allowed_files: `script_vm/heap_alloc.mlc`, `script_vm/heap.mlc`, `STATE`
- forbidden: F0
- contract: переносятся `heap_alloc_slot`, `heap_alloc_cell`, `heap_alloc_array`, `heap_alloc_record`, `heap_alloc_closure`; правило 9 для множества экспортов
- red: none — refactor с characterization
- green: создать модуль
- gate: `GATE(HEAPSET FRAMES)`; `SIZE`
- sabotage: в `heap_alloc_closure` сохранить `proto_index` в `shape_or_meta` вместо `object_flags` → `GATE(char_frames)` падает; вернуть
- performance: not a perf step
- done_when: gate зелёные
- state_update: gate-команды и `wc -l`

### Step 36 — Рефактор: `heap_collect.mlc`

- kind: refactor
- depends: 35
- goal: вынести корни, mark, sweep
- allowed_files: `script_vm/heap_collect.mlc`, `script_vm/heap.mlc`, `STATE`
- forbidden: F0
- contract: переносятся `heap_root_push`, `heap_root_clear`, `mark_value`, `mark_elements`, `mark_object`, `heap_collect`; в `heap.mlc` остаются `heap_closure_proto_index`, `heap_cell_child`, `heap_object_type_id`, `heap_get_element`, `heap_element_count`, `heap_write_barrier`; правило 9
- red: none — refactor с characterization
- green: создать модуль
- gate: `GATE(HEAPSET FRAMES)`; `SIZE`
- sabotage: в `heap_collect` не очищать флаг mark у выживших → `GATE(heap_gc_arena)` падает на втором цикле; вернуть
- performance: not a perf step
- done_when: gate зелёные; `wc -l script_vm/heap.mlc` ≤ 400
- state_update: gate-команды и `wc -l`

### Step 37 — Рефактор: `embedding_codes.mlc`, `module_blob.mlc`

- kind: refactor
- depends: 33
- goal: разделить `embedding.mlc` (410 строк) на коды, байтовый образ модуля и API вызова
- allowed_files: `script_vm/embedding_codes.mlc`, `script_vm/module_blob.mlc`, `script_vm/embedding.mlc`, `runtime/src/script_vm/embedding_abi.cpp` (только если без этого бридж не компилируется, см. contract), `STATE`
- forbidden: F0 (кроме указанного файла бриджа); `runtime/include/**`
- contract: `embedding_codes.mlc` — `emb_ok..emb_bad_handle`; `module_blob.mlc` — `ModuleExport` (получает `export`), `DecodedModule`, `DecodeResult`, `magic_mlsc`, `append_u32_le`, `append_i64_le`, `read_u32_le`, `read_i64_le`, `bytes_from_pointer`, `name_from_bytes`, `append_words`, `append_name_bytes`, `decode_module`, `emb_encode_add1_module` и `extern fn embedding_*` объявления, нужные этим функциям; `embedding.mlc` остаётся с `verify_decoded`, `find_export_proto`, `run_err_to_code`, `emb_verify_module_bytes`, `emb_call_export_bytes`, `emb_verify_module_ptr`, `emb_call_export_ptr`, `emb_add1_fixture_*`. Внимание: C++-пространство имён функции определяется модулем, а бридж ссылается на `embedding::emb_ok()`, `embedding::emb_type()`, `embedding::emb_format()`, `embedding::emb_bounds()`, `embedding::emb_bad_handle()` и т.д. Тонкие обёртки с теми же именами в `embedding.mlc` запрещены (конфликт имён при импорте). Правило: сначала собрать и проверить, находит ли бридж имена (`using namespace` транзитивных модулей в сгенерированном `embedding.cpp`); если нет — в `embedding_abi.cpp` разрешена только замена префикса пространства имён (`embedding_codes::emb_*`, `module_blob::...`) и добавление `#include` соответствующих `.hpp`; логика бриджа не меняется. Описать выбранный вариант в `last_verify`
- red: none — refactor с characterization (`char_blob`, `embedding_abi` с C-хостом)
- green: выполнить разбиение; убедиться, что `embedding_abi_unit` по-прежнему линкует хост
- gate: `GATE(EMBED FRAMES)`; `bash scripts/run_script_vm_embedding_abi_unit.sh` → exit 0 (линкует C-хост с бриджем); `SIZE`
- sabotage: в `module_blob.mlc` изменить `magic_mlsc()` на другое число → `GATE(char_blob)` падает; вернуть
- performance: not a perf step
- done_when: gate зелёные; `wc -l script_vm/embedding.mlc` ≤ 400
- state_update: gate-команды и `wc -l`

### Step 38 — Строгий size-gate без allowlist

- kind: green
- depends: 36, 37
- goal: убрать храповик; лимит 400 действует на все `script_vm/**/*.mlc`
- allowed_files: `scripts/run_script_vm_file_size.rb`, `STATE`
- forbidden: F0
- contract: allowlist удалён из кода; любой файл > 400 → exit 1 и перечисление
- red: `SIZE` после правки скрипта на временном файле `script_vm/tmp_size_probe.mlc` из 401 строки → exit 1 (файл не коммитится)
- green: удалить allowlist
- gate: `SIZE` → exit 0 при чистом дереве; `GATE(all)`
- sabotage: создать `script_vm/tmp_size_probe.mlc` из 401 строки → `SIZE` exit 1; удалить файл
- performance: not a perf step
- done_when: gate зелёный, probe-файл удалён, `git status` без `tmp_size_probe`
- state_update: `SIZE` и `GATE(all)` итоги

### Фаза 2b — Пол производительности: куча (шаги 39–47, всего 9)

### Step 39 — Red: in-place API кучи (`heap_inplace`)

- kind: red
- depends: 38
- goal: тест нового `heap_in_place_*` API (без collect) до его появления
- allowed_files: `script_vm/tests/heap_inplace_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: API (S5 п.5) — `heap_in_place_alloc_cell(heap: ref mut Heap) -> Value`; `heap_in_place_alloc_array(heap, length: i32) -> Value`; `heap_in_place_alloc_record(heap, field_count: i32) -> Value`; `heap_in_place_alloc_closure(heap, proto_index: i32, upvalue_count: i32) -> Value`; `heap_in_place_write(heap, parent: Value, slot_index: i32, value: Value) -> unit` (счётчик барьера +1, для Cell пишет `child`); `heap_in_place_read(heap, parent, index) -> Value`; `heap_in_place_type_id(heap, value) -> i32`; `heap_in_place_element_count(heap, value) -> i32`; `heap_in_place_closure_proto_index`, `heap_in_place_cell_child`; `heap_in_place_root_push(heap, value) -> unit`, `heap_in_place_root_clear(heap) -> unit`; `heap_slot_count(heap: Heap) -> i32`
- red: unit проверяет: (1) эквивалентность — одна последовательность из 60 операций (аллокации всех видов, записи, чтения) на функциональном (`heap_alloc_*`/`heap_write_barrier`) и на in-place кучах даёт одинаковые `heap_live_object_count`, `heap_write_barrier_hits` и одинаковые чтения всех элементов; (2) изоляция копий — после `let snapshot = heap` и мутации `heap` in-place `snapshot` не меняется; (3) масштабирование — на N=1000 и N=4000 пар «alloc_record(2) + write» `ns` на пару: `ratio_x100 = ns_large*100/ns_small ≤ 250`; печатает `heap_inplace_ns_per_pair_n1000`, `..._n4000`, `heap_inplace_scaling_ratio_x100`
- green: none — red-шаг (реализация в следующем шаге)
- gate: `RED(heap_inplace)` → exit ≠ 0 (отсутствует `heap_in_place_alloc_cell`); первая строка ошибки в `last_verify`
- sabotage: none — red-шаг
- performance: числа печатаются в green-шаге
- done_when: RED падает по отсутствию `heap_in_place_*`
- state_update: команда и первая строка ошибки

### Step 40 — Рефактор: слоты кучи — скаляры + арена элементов

- kind: refactor
- depends: 39
- goal: убрать вложенные массивы из `HeapSlot`; элементы объектов — в одном `element_arena`
- allowed_files: `script_vm/heap_types.mlc`, `script_vm/heap_alloc.mlc`, `script_vm/heap_collect.mlc`, `script_vm/heap.mlc`, `STATE`
- forbidden: F0; публичный функциональный API не менять
- contract: `HeapSlot { header, child, element_start: i32, element_count: i32, size_class: i32, live: bool }`; `Heap` получает `element_arena: [Value]`; аллокация объекта с `k` элементами добавляет `k` ячеек `Nil` в конец арены; `heap_get_element`/`heap_element_count`/`heap_write_barrier` читают и пишут через `element_start`; sweep освобождает слот, арена пока не уплотняется (мёртвые диапазоны остаются до шага 45); все существующие тесты кучи зелёные
- red: none — refactor с characterization
- green: переписать типы и функции; не добавлять `heap_in_place_*` (шаг 41)
- gate: `GATE(HEAPSET FRAMES)`; `SIZE`; `BENCH(--max-ratio B4.ns_per_alloc_n1000=current:130)`
- sabotage: в `heap_get_element` читать `element_start + index + 1` → `GATE(char_heap_ops)` падает; вернуть
- performance: not a perf step (допуск 130% — защита от регрессии, не заявка)
- done_when: gate зелёные
- state_update: gate-команды и значение B4

### Step 41 — Green: in-place API кучи

- kind: green
- depends: 40
- goal: реализовать `heap_in_place_*`; функциональный API делегирует в него
- allowed_files: `script_vm/heap_types.mlc`, `script_vm/heap_alloc.mlc`, `script_vm/heap.mlc`, `script_vm/heap_access.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `heap_inplace`), `STATE`
- forbidden: F0
- contract: в `heap_alloc.mlc` — `heap_in_place_alloc_*` (добавление слота `slots.push` либо `.set`, арена `push` k ячеек `Nil`, `live_count += 1`); в новом `heap_access.mlc` — `heap_in_place_write/read/type_id/element_count/closure_proto_index/cell_child/root_push/root_clear` и `heap_slot_count`; функциональные версии: `let mut next = heap; const value = heap_in_place_alloc_record(next, n); HeapAlloc { heap: next, value: value }`; все функции кучи, уже не нужные в `heap.mlc`, остаются там ровно как тонкие обёртки (набор экспортов — правило 9)
- red: шаг 39
- green: реализовать
- gate: `RUN(heap_inplace)`; `GATE(HEAPSET FRAMES heap_inplace)`; `SIZE`
- sabotage: в `heap_in_place_write` не увеличивать счётчик барьера → `RUN(heap_inplace)` падает; вернуть
- performance: harness `heap_inplace_unit` (масштабирование N=1000/4000); `heap_inplace_scaling_ratio_x100 ≤ 250` печатается и проверяется unit-ом
- done_when: gate зелёные
- state_update: gate-команды и три числа масштабирования

### Step 42 — Red: повторное использование слотов (`heap_reuse`)

- kind: red
- depends: 41
- goal: тест pop-reuse свободных id
- allowed_files: `script_vm/tests/heap_reuse_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: sweep кладёт освобождённые id по возрастанию; аллокация берёт последний положенный (LIFO); `heap_slot_count` не растёт, пока есть свободные id; освобождённый объект пересоздаётся с `gc_flags = 0`, элементы `Nil`
- red: unit: 4 записи (id 0..3), в корне только id 1, `heap_collect`; затем три `heap_in_place_alloc_record` возвращают id 3, затем 2, затем 0 (`value_decode` → `ValueHeapRef(id)`), `heap_slot_count` = 4 после них; четвёртая новая запись получает id 4 и `heap_slot_count` = 5; все новые записи читаются как `Nil` по всем полям; `heap_live_object_count` корректен на каждом шаге
- green: none — red-шаг (реализация в следующем шаге)
- gate: `RED(heap_reuse)` → exit ≠ 0 (текущий код всегда дописывает: первый id = 4); первая упавшая метка в `last_verify`
- sabotage: none — red-шаг
- performance: not a perf step
- done_when: RED падает на значении id, а не на ошибке компиляции
- state_update: команда и первая упавшая метка

### Step 43 — Green: pop-reuse свободных id

- kind: green
- depends: 42
- goal: `free_object_ids` + `free_object_count` вместо `freelist_32..256`
- allowed_files: `script_vm/heap_types.mlc`, `script_vm/heap_alloc.mlc`, `script_vm/heap_collect.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `heap_reuse`), `STATE`
- forbidden: F0
- contract: S5 п.2, п.4: `Heap` теряет четыре `freelist_*` и `scratch_object_id`, получает `free_object_ids: [i32]`, `free_object_count: i32`; push на sweep: если `count < length` — `.set`, иначе `.push`; `count += 1`; pop при аллокации: `count -= 1`, id = `free_object_ids[count]`; размерный класс слота перезаписывается новым объектом (`heap_size_class_bytes` не меняется); вспомогательная функция выбора id — отдельная функция, возвращающая значение (правило 13, R1)
- red: шаг 42
- green: реализовать в in-place аллокаторах и в sweep
- gate: `RUN(heap_reuse)`; `GATE(HEAPSET FRAMES heap_inplace heap_reuse)`; `SIZE`
- sabotage: не сбрасывать элементы переиспользованного слота в `Nil` → `RUN(heap_reuse)` падает; вернуть
- performance: not a perf step
- done_when: gate зелёные
- state_update: gate-команды

### Step 44 — Red: in-place сборка (`heap_collect`-unit)

- kind: red
- depends: 43
- goal: тест `heap_in_place_collect`, итеративного mark и уплотнения арены
- allowed_files: `script_vm/tests/heap_collect_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: API — `heap_in_place_collect(heap: ref mut Heap, extra_roots: [Value]) -> unit`; `heap_arena_cell_count(heap: Heap) -> i32`
- red: unit: (1) цепочка из 100000 Cell (каждая указывает на следующую), корень — голова, `heap_in_place_collect(heap, [])` → `heap_live_object_count` = 100000 без падения; (2) цикл из двух записей: без корней собирается, с корнем одной — обе живы; (3) `extra_roots` сохраняют объект, не лежащий в `heap.roots`; (4) уплотнение арены: 10 массивов по 100 элементов, корень на один → после сборки `heap_arena_cell_count` = 100; (5) целостность после уплотнения: в каждый элемент выжившего массива записан уникальный Int32, после сборки все читаются; (6) функциональный `heap_collect` даёт те же результаты на (2)
- green: none — red-шаг (реализация в следующем шаге)
- gate: `RED(heap_collect)` → exit ≠ 0 (отсутствует `heap_in_place_collect`); первая строка ошибки в `last_verify`
- sabotage: none — red-шаг
- performance: not a perf step
- done_when: RED падает по отсутствию `heap_in_place_collect`
- state_update: команда и первая строка ошибки

### Step 45 — Green: `heap_in_place_collect`

- kind: green
- depends: 44
- goal: итеративный mark (явный стек `[i32]` + счётчик), sweep, уплотнение арены
- allowed_files: `script_vm/heap_collect.mlc`, `script_vm/heap_types.mlc`, `script_vm/heap.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `heap_collect`), `STATE`
- forbidden: F0
- contract: корни — `heap.roots` + `extra_roots`; mark ставит `gc_flags | 1` через `slots.set` и толкает дочерние id в рабочий стек; sweep: помеченные сбрасывают флаг, непомеченные живые → слот мёртв, `live_count` уменьшается, id кладётся в `free_object_ids`; уплотнение: новая арена строится обходом живых слотов по возрастанию id, `element_start` обновляется, мёртвые диапазоны отбрасываются; `heap_collect` (функциональный) = копия + `heap_in_place_collect(next, [])`; `mark_object` рекурсивный удаляется
- red: шаг 44
- green: реализовать
- gate: `RUN(heap_collect)`; `GATE(HEAPSET FRAMES heap_inplace heap_reuse heap_collect)`; `SIZE`
- sabotage: при уплотнении не обновлять `element_start` → `RUN(heap_collect)` падает на проверке (5); вернуть
- performance: not a perf step (замер — шаги 46–47)
- done_when: gate зелёные, `mark_object` не найден `rg 'fn mark_object' script_vm/` → пусто
- state_update: gate-команды

### Step 46 — Green: опкоды кучи исполнителя на in-place API

- kind: green
- depends: 45, 31
- goal: `executor_heap.mlc`, `executor_calls.mlc` (MAKE_CLOSURE, `fill_closure_upvalues`, SET_UPVAL) и `executor.mlc` работают с `heap: ref mut Heap`
- allowed_files: `script_vm/executor.mlc`, `script_vm/executor_heap.mlc`, `script_vm/executor_calls.mlc`, `STATE`
- forbidden: F0; публичные `run_*` не менять
- contract: результаты и коды ошибок неизменны (`char_heap_ops`, `char_frames`); `run_state_program` держит локальный `let mut heap_state`, копию делает один раз на входе, возвращает в `RunHeapResult`
- red: none — перф-правка, поведение закреплено characterization
- green: заменить вызовы функционального API на `heap_in_place_*`
- gate: `GATE(all)`; `BENCH(--require-lower B4.ns_per_alloc_n4000=baseline --max-absolute B4.scaling_ratio_x100=250 --max-ratio B1.ns_per_insn=current:110 --max-ratio B2.ns_per_insn=current:110 --max-ratio B3.ns_per_call=current:110)`
- sabotage: в `executor_exec_set_prop` пропустить запись → `GATE(char_heap_ops)` падает; вернуть
- performance: harness `BENCH`; `B4.ns_per_alloc_n4000` строго ниже baseline; `B4.scaling_ratio_x100 ≤ 250` (проверка линейности); B1–B3 не хуже `current` более чем на 10% (допуск шума)
- done_when: оба gate зелёные
- state_update: gate-команды и числа B4 baseline → новое

### Step 47 — Замер: итог пола производительности (S47)

- kind: measure
- depends: 46
- goal: зафиксировать итоговые числа и записать `current`
- allowed_files: `script_vm/tests/bench_current.txt`, `PLAN100` (Decision log), `STATE`
- forbidden: F0; любые изменения кода
- contract: числа — только напечатанные бенчем
- red: none — замер
- green: `BENCH(--record current)`; дописать `### S47 — итог пола производительности`: таблица baseline → current по 6 метрикам и отношение
- gate: `BENCH(--require-lower B1.ns_per_insn=baseline --require-lower B2.ns_per_insn=baseline --require-lower B3.ns_per_call=baseline --require-lower B4.ns_per_alloc_n4000=baseline --max-absolute B4.scaling_ratio_x100=250)` → exit 0
- sabotage: `BENCH(--require-lower B1.ns_per_insn=current)` сразу после записи обязан дать exit 1 (равенство не «строго ниже»)
- performance: итоговая таблица в S47; если какой-то `--require-lower` не прошёл — `blocked` с числами (правило 12)
- done_when: S47 дописан, gate зелёный
- state_update: таблица baseline → current

### Фаза 4 — Пробелы корректности (шаги 48–61, всего 14)

### Step 48 — Red: контракт falsy (`falsy_contract`)

- kind: red
- depends: 47, 2
- goal: тест нового контракта JUMP_IF_FALSE (S2)
- allowed_files: `script_vm/tests/falsy_contract_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: S2: прыжок только на `Bool(false)`; `Nil` → `RunErr("type", primary)`
- red: unit через `run_with_heap` и через `run_program` проверяет шесть исходов: `Bool(false)` прыгает; `Bool(true)` — нет; `Int32(0)` — нет (результат ветки «не прыгнули»); `Float64(0.0)` — нет; HeapRef (регистр с NEW_ARRAY) — нет; `Nil` (регистр, в который ничего не загружено) → `RunErr("type", <word_index инструкции>)`
- green: none — red-шаг (реализация в следующем шаге)
- gate: `RED(falsy_contract)` → exit ≠ 0; первая упавшая метка (`int_zero_falls_through`) в `last_verify`
- sabotage: none — red-шаг
- performance: not a perf step
- done_when: RED падает на поведении, а не на ошибке компиляции
- state_update: команда и первая упавшая метка

### Step 49 — Green: контракт falsy

- kind: green
- depends: 48
- goal: реализовать S2
- allowed_files: `script_vm/arith.mlc`, `script_vm/executor.mlc`, `script_vm/tests/char_arith_unit.mlc` (удалить блок `legacy_falsy_*`), `scripts/run_script_vm_gate.rb` (регистрация `falsy_contract`), `STATE`
- forbidden: F0
- contract: `value_is_falsy` заменяется на `value_jump_decision(value: Value) -> i32` (`0` — не прыгать, `1` — прыгать, `2` — ошибка `type`); обработчик JUMP_IF_FALSE в `executor.mlc` использует её; остальные тесты, где JUMP_IF_FALSE получает Bool (`control_flow`, `closures_fibers`, `bench_fixtures`), остаются зелёными
- red: шаг 48
- green: реализовать; убрать `legacy_falsy_*` из `char_arith`
- gate: `RUN(falsy_contract)`; `GATE(ARITH FRAMES HEAPSET falsy_contract bench_fixtures)`
- sabotage: пусть `Nil` даёт `0` (не прыгать) → `RUN(falsy_contract)` падает; вернуть
- performance: not a perf step
- done_when: gate зелёные; `rg 'value_is_falsy' script_vm/` пусто
- state_update: gate-команды

### Step 50 — Red: `INT_MIN / -1` (`int32_div_overflow`)

- kind: red
- depends: 49
- goal: тест, что `INT_MIN / -1` — `overflow`, а не крах процесса
- allowed_files: `script_vm/tests/int32_div_overflow_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: `INT_MIN = 0 - 2147483647 - 1` как константа Value; `DIV(INT_MIN, -1)` → `RunErr("overflow", word_index)` через `run_with_heap` и `run_program`; не ломаются `7 / -1 = -7`, `INT_MAX / -1 = -2147483647`, `0 / -1 = 0`, `INT_MIN / 1 = INT_MIN`, `(-7) / 2 = -3`
- red: `RED(int32_div_overflow)` → процесс падает по SIGFPE (раннер сообщает отсутствие кода выхода 0); в `last_verify` — сообщение раннера
- green: none — red-шаг (реализация в следующем шаге)
- gate: `RED(int32_div_overflow)` → exit ≠ 0
- sabotage: none — red-шаг
- performance: not a perf step
- done_when: RED падает именно в тесте `INT_MIN / -1` (до него печатаются метки успешных случаев)
- state_update: команда и последняя напечатанная метка

### Step 51 — Green: `INT_MIN / -1` → overflow

- kind: green
- depends: 50
- goal: защита DIV на всех путях, включая быстрый путь шага 33
- allowed_files: `script_vm/arith.mlc`, `script_vm/executor_arith.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `int32_div_overflow`), `STATE`
- forbidden: F0
- contract: если `right == -1` и `left == INT_MIN` → `run_err_overflow()`; остальное без изменений
- red: шаг 50
- green: реализовать в `apply_binary` и в быстрой ветке `executor_exec_binary`
- gate: `RUN(int32_div_overflow)`; `GATE(ARITH int32_div_overflow)`; `BENCH(--max-ratio B1.ns_per_insn=current:110)`
- sabotage: убрать проверку в быстрой ветке `executor_exec_binary` → `RUN(int32_div_overflow)` падает; вернуть
- performance: not a perf step (допуск 110% — защита)
- done_when: gate зелёные
- state_update: gate-команды и значение B1

### Step 52 — Red: лимит инструкций (`instruction_limit`)

- kind: red
- depends: 51, 5
- goal: тест лимита инструкций через `RunLimits`
- allowed_files: `script_vm/tests/instruction_limit_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: API (S5 п.7) — `run_result.mlc`: `export type RunLimits = { instruction_limit: i64, heap_limit_objects: i32 }`, `run_limits_unlimited() -> RunLimits`, `run_limits_new(instruction_limit: i64, heap_limit_objects: i32) -> RunLimits`, `run_err_instruction_limit() = "instruction_limit"`; `interpreter.mlc`: `run_program_with_args_limited(heap: Heap, protos: [FunctionProto], entry_proto_index: i32, args: [Value], limits: RunLimits) -> RunHeapResult`
- red: unit: программа из 4 инструкций (`LOAD_CONST, LOAD_CONST, ADD, RETURN`): лимит 4 → `RunOk`; лимит 3 → `RunErr("instruction_limit", 3)`; лимит 0 и отрицательный = без лимита; бесконечный цикл `JUMP -1` (`instruction_words_jump(0 - 1)`) с лимитом 1000 → `RunErr("instruction_limit", 0)`; wide-инструкция считается за одну; лимит считается по всем фреймам (рекурсия `fib` из `bench_fixtures` с лимитом 100 → ошибка, с лимитом `10000000` → `RunOk(55)` для fib(10))
- green: none — red-шаг (реализация в следующем шаге)
- gate: `RED(instruction_limit)` → exit ≠ 0 (отсутствует `run_limits_new`); первая строка ошибки
- sabotage: none — red-шаг
- performance: not a perf step
- done_when: RED падает по отсутствию API
- state_update: команда и первая строка ошибки

### Step 53 — Green: лимит инструкций

- kind: green
- depends: 52
- goal: счётчик инструкций и проверка лимита в цикле исполнителя
- allowed_files: `script_vm/run_result.mlc`, `script_vm/vm_state.mlc`, `script_vm/executor.mlc`, `script_vm/interpreter.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `instruction_limit`), `STATE`
- forbidden: F0
- contract: `VmState` получает `instruction_limit: i64`, `heap_limit_objects: i32` (заполняются из `RunLimits`); в начале каждой итерации цикла: если `instruction_limit > 0 && instruction_count >= instruction_limit` → `executor_fail(state, run_err_instruction_limit(), primary)`; иначе `instruction_count += 1`; `run_state_program` = `run_state_program_limited(..., run_limits_unlimited())`; функция `executor` публикует `run_state_program_limited`
- red: шаг 52
- green: реализовать; обновить `vm_state` unit только если конструктор изменился (поведение прежнее)
- gate: `RUN(instruction_limit)`; `GATE(ARITH FRAMES vm_state instruction_limit)`; `BENCH(--max-ratio B1.ns_per_insn=current:110 --max-ratio B2.ns_per_insn=current:110)`
- sabotage: заменить `>=` на `>` → `RUN(instruction_limit)` падает на границе (лимит 3); вернуть
- performance: harness `BENCH`; допуск 110% к `current` — счётчик не должен заметно замедлять цикл; числа в `last_verify`
- done_when: gate зелёные
- state_update: gate-команды и числа B1/B2

### Step 54 — Red: GC с корнями из стека (`gc_roots`)

- kind: red
- depends: 53, 47
- goal: тест автосборки по S5 п.9
- allowed_files: `script_vm/tests/gc_roots_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: используется `run_program_with_args_limited`; фикстуры `fixture_record_alloc_loop` из `bench_fixtures`
- red: unit: (1) churn — `fixture_record_alloc_loop(20000)` с `run_limits_new(0, 1000)`: `RunOk(20000)` и `heap_live_object_count(result.heap) <= 1000`; (2) живой корень — программа: r0 = запись с полем 0 = Int32(7), далее цикл на 5000 итераций с NEW_RECORD мусора в r1, затем `GET_PROP r0.0` возвращает 7, лимит 100 (сборок не меньше 40); (3) MAKE_CLOSURE-мусор: 5000 итераций MAKE_CLOSURE при лимите 100 → `RunOk`; (4) без лимита — `run_limits_unlimited()`, 5000 аллокаций: `heap_live_object_count(result.heap) < 5000` (порог 1024 сработал); (5) host-корень: объект, добавленный `heap_root_push` до запуска и не лежащий в регистрах, жив после запуска с churn
- green: none — red-шаг (реализация в следующем шаге)
- gate: `RED(gc_roots)` → exit ≠ 0 (случай (1): живых 20000); первая упавшая метка
- sabotage: none — red-шаг
- performance: not a perf step
- done_when: RED падает на значении счётчика, не на ошибке компиляции
- state_update: команда и первая упавшая метка

### Step 55 — Green: автосборка с корнями из стека

- kind: green
- depends: 54
- goal: сборка на safepoint аллокации по S5 п.9
- allowed_files: `script_vm/vm_state.mlc`, `script_vm/heap_types.mlc`, `script_vm/heap_collect.mlc`, `script_vm/executor_heap.mlc`, `script_vm/executor_calls.mlc`, `script_vm/executor_gc.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `gc_roots`), `STATE`
- forbidden: F0
- contract: `Heap` получает `gc_threshold: i32` (старт 1024); `vm_state_live_roots(state: ref mut VmState) -> [Value]` собирает `stack[0 .. stack_top)` и `frame_closure[0 .. frame_depth)`; `executor_gc.mlc`: `executor_collect_if_needed(state: ref mut VmState, heap: ref mut Heap) -> unit` вызывается перед NEW_ARRAY/NEW_RECORD/MAKE_CLOSURE: при `heap_limit_objects > 0` и `live_count >= heap_limit_objects` — `heap_in_place_collect(heap, vm_state_live_roots(state))`; при `heap_limit_objects == 0` и `live_count >= gc_threshold` — сборка и `gc_threshold = max(1024, 2 * live_count)`; ошибки `heap_limit` в этом шаге нет (шаг 57)
- red: шаг 54
- green: реализовать
- gate: `RUN(gc_roots)`; `GATE(all)`; `BENCH(--max-ratio B1.ns_per_insn=current:110 --max-ratio B3.ns_per_call=current:110)`
- sabotage: `vm_state_live_roots` возвращает пустой список → `RUN(gc_roots)` падает (живая запись освобождается и перезаписывается); вернуть
- performance: допуск 110% к `current` — защита; B4 в этом шаге не сравнивать (сборка меняет профиль)
- done_when: gate зелёные
- state_update: gate-команды и числа

### Step 56 — Red: лимит кучи (`heap_limit`)

- kind: red
- depends: 55
- goal: тест `RunErr("heap_limit", ...)`
- allowed_files: `script_vm/tests/heap_limit_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: API — `run_err_heap_limit() = "heap_limit"` в `run_result.mlc`; семантика S5 п.8
- red: unit: массив из 10 элементов в r0 (1 объект) + цикл, кладущий новые записи в массив (все живы): лимит 5 → `RunErr("heap_limit", <word_index NEW_RECORD>)` на пятой записи (в массиве после 4 записей живых объектов ровно 5); лимит 6 → пятая запись проходит; `heap_live_object_count(result.heap) == 5` после ошибки; NEW_ARRAY и MAKE_CLOSURE проверяются тем же способом отдельными программами; лимит 0 не ограничивает
- green: none — red-шаг (реализация в следующем шаге)
- gate: `RED(heap_limit)` → exit ≠ 0 (отсутствует `run_err_heap_limit`); первая строка ошибки
- sabotage: none — red-шаг
- performance: not a perf step
- done_when: RED падает по отсутствию `run_err_heap_limit`
- state_update: команда и первая строка ошибки

### Step 57 — Green: лимит кучи

- kind: green
- depends: 56
- goal: проверка лимита после сборки
- allowed_files: `script_vm/run_result.mlc`, `script_vm/executor_gc.mlc`, `script_vm/executor_heap.mlc`, `script_vm/executor_calls.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `heap_limit`), `STATE`
- forbidden: F0
- contract: `executor_collect_if_needed` возвращает `i32` статус: если после сборки `live_count >= heap_limit_objects` (при `heap_limit_objects > 0`) — `executor_fail(state, run_err_heap_limit(), primary)`; аллокация не выполняется
- red: шаг 56
- green: реализовать, вызывающие обработчики пробрасывают статус
- gate: `RUN(heap_limit)`; `GATE(all)`
- sabotage: заменить `>=` на `>` в проверке → `RUN(heap_limit)` падает на границе; вернуть
- performance: not a perf step
- done_when: gate зелёные
- state_update: gate-команды

### Step 58 — Red: ужесточение верификатора (`verifier_hardening`)

- kind: red
- depends: 57
- goal: тест проверки окна аргументов CALL и локальных источников upvalue
- allowed_files: `script_vm/tests/verifier_hardening_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: `verify_function(words, register_count, constant_count)` → `VerifyErr("register", primary)`; `run_program` → `RunErr("verify", primary)`
- red: unit: `CALL r0, r2, #3` при `register_count = 5` (окно r3..r5, r5 вне диапазона) → `register`; `CALL r0, r2, #2` при 5 → `VerifyOk`; `CALL r0, r4, #0` при 5 → `VerifyOk`; `MAKE_CLOSURE r0, #1, [is_local=1 index=9]` при `register_count = 3` → `register`; `[is_local=1 index=2]` → `VerifyOk`; `[is_local=0 index=200]` → `VerifyOk` (индекс родительского upvalue проверяется рантаймом); валидная программа замыканий из `char_frames` проходит
- green: none — red-шаг (реализация в следующем шаге)
- gate: `RED(verifier_hardening)` → exit ≠ 0 (текущий верификатор пропускает окно); первая упавшая метка
- sabotage: none — red-шаг
- performance: not a perf step
- done_when: RED падает на поведении верификатора
- state_update: команда и первая упавшая метка

### Step 59 — Green: ужесточение верификатора

- kind: green
- depends: 58
- goal: реализовать проверки
- allowed_files: `script_vm/verifier.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `verifier_hardening`), `STATE`
- forbidden: F0
- contract: в ветви CALL `registers_ok`: дополнительно `decoded.b + decoded.c < register_count`; отдельная функция `closure_sources_ok(words, primary, decoded, register_count) -> bool` для MAKE_CLOSURE вызывается из цикла проверки (`verify_function`): каждое слово источника с `instruction_upvalue_source_is_local != 0` обязано иметь `instruction_upvalue_source_index < register_count`; новый код — функциями, возвращающими значения (R1)
- red: шаг 58
- green: реализовать
- gate: `RUN(verifier_hardening)`; `GATE(verifier FRAMES verifier_hardening embedding_abi)`; `SIZE`
- sabotage: убрать проверку окна CALL → `RUN(verifier_hardening)` падает; вернуть
- performance: not a perf step
- done_when: gate зелёные
- state_update: gate-команды

### Step 60 — Red: границы загрузки blob (`blob_limits`)

- kind: red
- depends: 59
- goal: тест, что `emb_verify_module_bytes` отвергает blob с `proto_index` вне диапазона и `register_count` вне `[0, 256]`
- allowed_files: `script_vm/tests/blob_limits_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: образец blob — `emb_encode_add1_module()`; поля правятся по смещениям: `proto_index` экспорта — байт 20, `register_count` прото — байт 28 (имя экспорта `add1`, 4 байта; проверить по `char_blob` из шага 13); коды `emb_format()`
- red: unit: `proto_index = 1` при одном прото → `emb_format`; `proto_index = 0` → `emb_ok`; `register_count = 256` → не `emb_format` по этой причине (допустим `emb_ok`, если верификатор принимает); `register_count = 257` → `emb_format`; `register_count = 4294967295` (читается как −1) → `emb_format`; `emb_call_export_bytes` на blob с `proto_index = 7` → `emb_format`, не `type`
- green: none — red-шаг (реализация в следующем шаге)
- gate: `RED(blob_limits)` → exit ≠ 0; первая упавшая метка
- sabotage: none — red-шаг
- performance: not a perf step
- done_when: RED падает на значении кода, не на ошибке компиляции
- state_update: команда и первая упавшая метка

### Step 61 — Green: границы загрузки blob

- kind: green
- depends: 60
- goal: проверки в декодере
- allowed_files: `script_vm/module_blob.mlc`, `script_vm/embedding.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `blob_limits`), `STATE`
- forbidden: F0
- contract: `decode_module` после разбора: каждый `proto_index` экспорта `< proto_count` иначе `emb_format()`; для каждого прото `0 <= register_count <= 256` иначе `emb_format()`; порядок проверок не меняет ранее зафиксированных кодов (`char_blob`)
- red: шаг 60
- green: реализовать
- gate: `RUN(blob_limits)`; `GATE(all)`
- sabotage: убрать проверку `register_count` → `RUN(blob_limits)` падает; вернуть
- performance: not a perf step
- done_when: gate зелёные
- state_update: gate-команды

### Фаза 5 — Lowering подмножества (шаги 62–88, всего 27)

### Step 62 — Red: билдер прото (`lower_builder`)

- kind: red
- depends: 61, 4
- goal: тест `script_vm/lower/lower_builder.mlc` и широких переходов до их появления
- allowed_files: `script_vm/tests/lower_builder_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: API (все `ref mut`) — `export type ProtoBuilder`; `proto_builder_new(parameter_count: i32) -> ProtoBuilder`; `proto_builder_here(builder) -> i32` (индекс следующего слова); `proto_builder_emit(builder, words: [i32]) -> i32` (возвращает индекс первого слова); `proto_builder_constant(builder, value: Value) -> i32` (дедупликация по `raw`); `proto_builder_load_constant(builder, destination: i32, value: Value) -> unit`; `proto_builder_register_mark(builder) -> i32`; `proto_builder_register_allocate(builder) -> i32`; `proto_builder_register_release_to(builder, mark: i32) -> unit`; `proto_builder_emit_jump(builder) -> i32` и `proto_builder_emit_jump_if_false(builder, condition_register: i32) -> i32` (wide-форма, смещение-заглушка, возвращают индекс патча); `proto_builder_patch_to_here(builder, site: i32) -> unit`; `proto_builder_emit_jump_back(builder, target_index: i32) -> unit`; `proto_builder_finish(builder) -> FunctionProto`. Модуль `script_vm/bytecode_wide.mlc`: `instruction_words_jump_wide(offset: i32) -> [i32]`, `instruction_words_jump_if_false_wide(condition: i32, offset: i32) -> [i32]`
- red: unit проверяет: одинаковая константа дважды → `constants.length() = 1`; 300 различных констант → `LOAD_CONST` последней в wide-форме (`instruction_is_wide`) и результат корректен; wide-JUMP с офсетом `+1` не конфликтует с маркером; программа «`if` без else» с вперёд-патчем даёт правильный результат; программа суммы 1..100 с `emit_jump_back` даёт 5050 через `run_arithmetic`; `register_count` прото = максимум одновременно занятых регистров (alloc 3, release_to(1), alloc 2 → 4 при 0 параметров... с параметрами `parameter_count` регистров заняты сразу); `proto_builder_finish` проходит `verify_function`
- green: none — red-шаг (реализация в следующем шаге)
- gate: `RED(lower_builder)` → exit ≠ 0 (нет модуля `./lower/lower_builder`); первая строка ошибки
- sabotage: none — red-шаг
- performance: not a perf step
- done_when: RED падает по отсутствию модуля
- state_update: команда и первая строка ошибки

### Step 63 — Green: билдер прото и wide-переходы

- kind: green
- depends: 62
- goal: реализовать `lower_builder.mlc`, `bytecode_wide.mlc`
- allowed_files: `script_vm/lower/lower_builder.mlc`, `script_vm/bytecode_wide.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `lower_builder`), `STATE`
- forbidden: F0; `script_vm/bytecode.mlc` не править (≥ 388 строк, храповик)
- contract: wide-переход = `instruction_encode(opcode, condition_or_zero, 0, 1).word` + слово-смещение; `target = index_первого_слова + 2 + offset`; патч: `builder.words.set(site + 1, target - (site + 2))`; `register_count = max(high_water, parameter_count)`; константы — `[Value]` с линейным поиском по `raw` (допустимо: константы функции ограничены)
- red: шаг 62
- green: реализовать
- gate: `RUN(lower_builder)`; `GATE(lower_builder bytecode_format verifier)`; `SIZE`
- sabotage: в `proto_builder_patch_to_here` не вычитать 2 → `RUN(lower_builder)` падает; вернуть
- performance: not a perf step
- done_when: gate зелёные
- state_update: gate-команды

### Step 64 — Red: кодирование модуля (`module_encode`)

- kind: red
- depends: 61
- goal: тест `module_encode` до реализации
- allowed_files: `script_vm/tests/module_encode_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: API (в `module_blob.mlc`) — `module_encode(exports: [ModuleExport], protos: [FunctionProto]) -> [i32]` (байты 0..255); `emb_encode_add1_module()` после шага 65 строится через `module_encode`, байты неизменны
- red: unit: add1-модуль, собранный через `module_encode`, побайтово равен `emb_encode_add1_module()` и литералу из `char_blob`; round-trip: два прото и два экспорта (имена разной длины), `emb_verify_module_bytes` = `emb_ok`, `emb_call_export_bytes` вызывает каждый; константы Float64 и отрицательный Int32 сохраняются побитово (результат вызова); модуль с 0 экспортов и 0 прото: `emb_verify_module_bytes` = `emb_ok`, `emb_call_export_bytes` → `emb_not_found`; пустое имя экспорта допустимо и вызывается
- green: none — red-шаг (реализация в следующем шаге)
- gate: `RED(module_encode)` → exit ≠ 0 (нет `module_encode`); первая строка ошибки
- sabotage: none — red-шаг
- performance: not a perf step
- done_when: RED падает по отсутствию функции
- state_update: команда и первая строка ошибки

### Step 65 — Green: `module_encode`

- kind: green
- depends: 64
- goal: общий кодировщик blob
- allowed_files: `script_vm/module_blob.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `module_encode`), `STATE`
- forbidden: F0
- contract: порядок записи — как в `decode_module`: магия, версия 1, число экспортов, для каждого `name_length, name_bytes, proto_index`, число прото, для каждого `register_count, word_count, words, constant_count, constants(i64)`; `emb_encode_add1_module` переписан через `module_encode`
- red: шаг 64
- green: реализовать
- gate: `RUN(module_encode)`; `GATE(EMBED module_encode blob_limits)`; `SIZE`
- sabotage: записывать `word_count` на 1 больше → `RUN(module_encode)` падает; вернуть
- performance: not a perf step
- done_when: gate зелёные
- state_update: gate-команды

### Step 66 — Red: lowering, первый срез (`lower_stage1`)

- kind: red
- depends: 63, 65, 9
- goal: тест `lower_source` на литералах
- allowed_files: `script_vm/tests/lower_stage1_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: API — `lower/lower_types.mlc`: `export type LowerDiagnostic = { code: string, message: string, line: i32, column: i32 }`, `export type LowerResult = { diagnostics: [LowerDiagnostic], bytes: [i32] }`; `lower/lower_module.mlc`: `export fn lower_source(source: string) -> LowerResult`. Тесты вызывают результат через `emb_call_export_bytes`
- red: unit: `export fn main() -> i32 = 42` → 0 диагностик, `emb_verify_module_bytes` = `emb_ok`, `main` → Int32(42); `export fn answer() -> bool = true` → Bool(true); два прото, из которых экспортирован один: вызов неэкспортированного `helper` → `emb_not_found`; синтаксическая ошибка (`export fn main( -> i32 = 1`) → ≥ 1 диагностика с кодом `syntax_error`, `bytes` пусто; `type X = i32` (alias) → `unsupported_type_alias`
- green: none — red-шаг (реализация в следующем шаге)
- gate: `RED(lower_stage1)` → exit ≠ 0 (нет `lower/lower_module`); первая строка ошибки
- sabotage: none — red-шаг
- performance: not a perf step
- done_when: RED падает по отсутствию модуля
- state_update: команда и первая строка ошибки

### Step 67 — Green: lowering, скелет

- kind: green
- depends: 66
- goal: `lower_source` для функций с литеральным телом
- allowed_files: `script_vm/lower/lower_types.mlc`, `script_vm/lower/lower_function.mlc`, `script_vm/lower/lower_module.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `lower_stage1`), `STATE`
- forbidden: F0
- contract: `lower_source`: `tokenize` → `parse_program_with_errors`; ошибки лексера/парсера → диагностики `syntax_error` (с line/column, если доступны); обход `Program.decls`: `DeclExported(DeclFn)` и `DeclFn` → по одному прото в порядке объявления; экспорт-таблица = только `DeclExported`; `ExprInt`/`ExprBool` → `LOAD_CONST` + `RETURN`; прочие декларации → `unsupported_<вид>`; при любой диагностике `bytes` пусто; сборка прото через `ProtoBuilder`, blob через `module_encode`
- red: шаг 66
- green: реализовать
- gate: `RUN(lower_stage1)`; `GATE(lower_stage1 frontend_probe module_encode)`; `SIZE`
- sabotage: экспортировать все функции, не только `DeclExported` → `RUN(lower_stage1)` падает на `helper`; вернуть
- performance: not a perf step
- done_when: gate зелёные
- state_update: gate-команды

### Step 68 — Red: параметры, локалы, арифметика (`lower_arithmetic`)

- kind: red
- depends: 67
- goal: тест лоуэринга выражений i32
- allowed_files: `script_vm/tests/lower_arithmetic_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: `emb_call_export_bytes(bytes, name, args)` с аргументами `value_encode(ValueInt32(..)).raw`
- red: unit: `add(a, b) = a + b` (41,1 → 42); `sub`, `mul`, `div` (`7 / 2 = 3`, `(0 - 7) / 2 = -3`); унарный минус `-a`; приоритет `a + b * c`; левая ассоциативность `a - b - c`; локалы `do const x = a * 2 \n let y = x + 1 \n y end`; вложенные блоки; переполнение `2147483647 + 1` и деление на 0 → результат не `emb_ok`; `a + b` с `b: bool` → диагностика `type_mismatch` с line/column > 0; неизвестное имя → `unknown_identifier`
- green: none — red-шаг (реализация в следующем шаге)
- gate: `RED(lower_arithmetic)` → exit ≠ 0 (первая упавшая метка `add`); первая строка в `last_verify`
- sabotage: none — red-шаг
- performance: not a perf step
- done_when: RED падает на поведении (результат/диагностика), а не на компиляции теста
- state_update: команда и первая упавшая метка

### Step 69 — Green: параметры, локалы, арифметика

- kind: green
- depends: 68
- goal: `lower_scope`, `lower_expression`, `lower_statement` для i32
- allowed_files: `script_vm/lower/lower_types.mlc`, `script_vm/lower/lower_scope.mlc`, `script_vm/lower/lower_expression.mlc`, `script_vm/lower/lower_statement.mlc`, `script_vm/lower/lower_function.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `lower_arithmetic`), `STATE`
- forbidden: F0
- contract: `LowerType = LowerInt | LowerBool | LowerRecord(string)`; `LowerScope` (`ref mut`): имя → `{ register, type, is_mutable }`, `lower_scope_push/pop` (блок освобождает регистры локалов через `register_release_to`); `lower_expression(scope, builder, expression, target) -> LowerType` — результат в регистре `target`; операнд-идентификатор читается из его регистра без MOVE; сложный операнд — во временный регистр; ошибки типов — `type_mismatch` с позицией из `Span`; выражение-блок со statements в позиции выражения → `unsupported_expression_block`; тело функции — `ExprBlock(stmts, final)` или одиночное выражение; результат → `RETURN`
- red: шаг 68
- green: реализовать; файл, перешедший 400 строк, разбить по ответственности
- gate: `RUN(lower_arithmetic)`; `GATE(lower_stage1 lower_arithmetic lower_builder)`; `SIZE`
- sabotage: для `-` использовать ADD → `RUN(lower_arithmetic)` падает; вернуть
- performance: not a perf step
- done_when: gate зелёные
- state_update: gate-команды

### Step 70 — Red: сравнения и `if` (`lower_control`)

- kind: red
- depends: 69
- goal: тест сравнений и условных выражений
- allowed_files: `script_vm/tests/lower_control_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: результат сравнения — Bool; `if` — выражение со значением
- red: unit: все шесть сравнений i32 на пограничных значениях (`0 - 1`, `0`, `1`, `INT_MAX`); `max(a, b) = if a > b then a else b end`; вложенный `clamp(x, lo, hi)`; `if` как операнд арифметики; `if` в `const`; `==` на двух bool → `bool_equality_unsupported`; условие не bool → `type_mismatch`; ветки разных типов → `type_mismatch`; обе ветки bool → экспорт возвращает Bool
- green: none — red-шаг (реализация в следующем шаге)
- gate: `RED(lower_control)` → exit ≠ 0; первая упавшая метка
- sabotage: none — red-шаг
- performance: not a perf step
- done_when: RED падает на поведении
- state_update: команда и первая упавшая метка

### Step 71 — Green: сравнения и `if`

- kind: green
- depends: 70
- goal: сравнения и `if/else` через JUMP_IF_FALSE и wide-переходы
- allowed_files: `script_vm/lower/lower_expression.mlc`, `script_vm/lower/lower_control.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `lower_control`), `STATE`
- forbidden: F0
- contract: `cond = lower(condition)` во временный регистр; `JUMP_IF_FALSE cond → else`; ветка `then` → целевой регистр; `JUMP end`; метка `else`; ветка `else`; конец; регистры веток освобождаются после каждой ветки; тип `if` = общий тип ветвей
- red: шаг 70
- green: реализовать
- gate: `RUN(lower_control)`; `GATE(lower_arithmetic lower_control)`; `SIZE`
- sabotage: поменять ветки местами → `RUN(lower_control)` падает; вернуть
- performance: not a perf step
- done_when: gate зелёные
- state_update: gate-команды

### Step 72 — Red: логика `&&`, `||`, `!` (`lower_logic`)

- kind: red
- depends: 71
- goal: тест короткого замыкания
- allowed_files: `script_vm/tests/lower_logic_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: bool-параметры экспортов передаются `value_encode(ValueBool(..)).raw`
- red: unit: таблицы истинности `and(a, b)`, `or(a, b)`, `not(a)` по всем комбинациям; короткое замыкание: `safe(a, b) = b != 0 && a / b > 1` при `b = 0` → Bool(false) без `div_zero`; `unsafe_or(a, b) = b == 0 || a / b > 1` при `b = 0` → Bool(true); цепочка `a && b || c` с учётом приоритета; `!` перед скобками; не-bool операнд `&&` → `type_mismatch`
- green: none — red-шаг (реализация в следующем шаге)
- gate: `RED(lower_logic)` → exit ≠ 0; первая упавшая метка
- sabotage: none — red-шаг
- performance: not a perf step
- done_when: RED падает на поведении
- state_update: команда и первая упавшая метка

### Step 73 — Green: логика

- kind: green
- depends: 72
- goal: `&&`, `||`, `!` через переходы
- allowed_files: `script_vm/lower/lower_expression.mlc`, `script_vm/lower/lower_control.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `lower_logic`), `STATE`
- forbidden: F0
- contract: `a && b`: `target = lower(a); JUMP_IF_FALSE target → end; target = lower(b); end`; `a || b`: `target = lower(a); JUMP_IF_FALSE target → rhs; JUMP end; rhs: target = lower(b); end`; `!a`: `t = lower(a); JUMP_IF_FALSE t → yes; LOAD false → target; JUMP end; yes: LOAD true → target; end`; опираются только на контракт falsy S2 для Bool
- red: шаг 72
- green: реализовать
- gate: `RUN(lower_logic)`; `GATE(lower_control lower_logic falsy_contract)`; `SIZE`
- sabotage: в `||` не пропускать вычисление правой части (убрать `JUMP end`) → `RUN(lower_logic)` падает; вернуть
- performance: not a perf step
- done_when: gate зелёные
- state_update: gate-команды

### Step 74 — Red: присваивание и `while` (`lower_loops`)

- kind: red
- depends: 73
- goal: тест `let mut`, присваивания, циклов
- allowed_files: `script_vm/tests/lower_loops_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: присваивание — `StmtExpr(ExprBin("=", lhs, rhs))`; `while` — `ExprWhile(cond, body)` в позиции statement
- red: unit: `sum(n)`: сумма 1..n (n=10 → 55, n=100 → 5050); счётчик со вложенным `while` (n × m); `if` без `else` как statement с присваиванием в ветке; присваивание `x = x + 1` выполняется на месте регистра локала (результаты корректны при `x` как операнде справа); присваивание в `const` → `assign_immutable`; присваивание параметру без `mut` → `assign_immutable`; параметр `mut n: i32` присваиваем (если парсер поддерживает; иначе метка пропускается с выводом `skipped_mut_param`); условие `while` не bool → `type_mismatch`; присваивание неизвестному имени → `unknown_identifier`; несовпадение типа присваивания → `type_mismatch`
- green: none — red-шаг (реализация в следующем шаге)
- gate: `RED(lower_loops)` → exit ≠ 0; первая упавшая метка
- sabotage: none — red-шаг
- performance: not a perf step
- done_when: RED падает на поведении
- state_update: команда и первая упавшая метка

### Step 75 — Green: присваивание и `while`

- kind: green
- depends: 74
- goal: statements присваивания, `while`, `if` без `else`
- allowed_files: `script_vm/lower/lower_statement.mlc`, `script_vm/lower/lower_control.mlc`, `script_vm/lower/lower_scope.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `lower_loops`), `STATE`
- forbidden: F0
- contract: присваивание локалу: `lower_expression(rhs, target = регистр_локала)` с защитой — если `rhs` читает тот же регистр как операнд, запись происходит только последней инструкцией выражения (операция читает операнды до записи); `while`: `top = here; cond = lower(condition); JUMP_IF_FALSE cond → end; body; JUMP_BACK top; end`; регистры условия и тела освобождаются на каждой итерации лоуэринга (не на рантайме)
- red: шаг 74
- green: реализовать
- gate: `RUN(lower_loops)`; `GATE(lower_control lower_logic lower_loops)`; `SIZE`
- sabotage: для присваивания выделять новый регистр вместо регистра локала → `RUN(lower_loops)` падает (сумма не накапливается); вернуть
- performance: not a perf step
- done_when: gate зелёные
- state_update: gate-команды

### Step 76 — Red: `return` (`lower_returns`)

- kind: red
- depends: 75
- goal: тест ранних возвратов
- allowed_files: `script_vm/tests/lower_returns_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: `StmtReturn(expr)` — `RETURN` в любой позиции
- red: unit: `first_divisor(n)`: цикл с `if n / i * i == n then return i end`; возврат из вложенных циклов; `return` в обеих ветках `if`; код после `return` в блоке допустим (недостижим); тип `return` не совпадает с объявленным → `type_mismatch`; функция, чьё тело заканчивается `while` без значения (`ExprUnit`) при объявленном `i32` → `type_mismatch`
- green: none — red-шаг (реализация в следующем шаге)
- gate: `RED(lower_returns)` → exit ≠ 0; первая упавшая метка
- sabotage: none — red-шаг
- performance: not a perf step
- done_when: RED падает на поведении
- state_update: команда и первая упавшая метка

### Step 77 — Green: `return`

- kind: green
- depends: 76
- goal: lowering `StmtReturn`
- allowed_files: `script_vm/lower/lower_statement.mlc`, `script_vm/lower/lower_function.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `lower_returns`), `STATE`
- forbidden: F0
- contract: `return e` = `lower(e → временный); RETURN временный`; проверка типа против `LowerFunctionSignature.result_type`
- red: шаг 76
- green: реализовать
- gate: `RUN(lower_returns)`; `GATE(lower_loops lower_returns)`; `SIZE`
- sabotage: `RETURN` читать из регистра 0 вместо временного → `RUN(lower_returns)` падает; вернуть
- performance: not a perf step
- done_when: gate зелёные
- state_update: gate-команды

### Step 78 — Red: вызовы функций (`lower_calls`)

- kind: red
- depends: 77
- goal: тест вызовов top-level функций
- allowed_files: `script_vm/tests/lower_calls_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: S4 п.8
- red: unit: `double(x)` и `export fn main(a) = double(a) + 1`; вызов функции, объявленной позже; три аргумента; bool-результат; `fib(10) = 55` (рекурсия); взаимная рекурсия `is_even`/`is_odd`; глубина: `deep(100)` — результат 100, `deep(300)` → `emb_stack()`; несуществующая функция → `unknown_function`; неверная арность → `call_arity`; тип аргумента не совпадает → `type_mismatch`; вызов с вложенным вызовом в аргументе `f(g(1), g(2))`
- green: none — red-шаг (реализация в следующем шаге)
- gate: `RED(lower_calls)` → exit ≠ 0; первая упавшая метка
- sabotage: none — red-шаг
- performance: not a perf step
- done_when: RED падает на поведении
- state_update: команда и первая упавшая метка

### Step 79 — Green: вызовы функций

- kind: green
- depends: 78
- goal: таблица функций и блок CALL
- allowed_files: `script_vm/lower/lower_expression.mlc`, `script_vm/lower/lower_call.mlc`, `script_vm/lower/lower_module.mlc`, `script_vm/lower/lower_types.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `lower_calls`), `STATE`
- forbidden: F0
- contract: предпроход собирает `LowerFunctionSignature { name, proto_index, parameter_types, result_type }` по всем `DeclFn`/`DeclExported(DeclFn)` до лоуэринга тел (допускает ссылки вперёд); вызов: `base = register_allocate`; `register_allocate` ещё `n` регистров подряд; `LOAD`-`MAKE_CLOSURE base, proto_index, 0`; аргументы лоуэрятся в `base+1+i`; `CALL target, base, n`; блок освобождается
- red: шаг 78
- green: реализовать
- gate: `RUN(lower_calls)`; `GATE(lower_returns lower_calls closures_fibers)`; `SIZE`
- sabotage: аргументы писать в `base + i` → `RUN(lower_calls)` падает; вернуть
- performance: not a perf step
- done_when: gate зелёные
- state_update: gate-команды

### Step 80 — Red: записи (`lower_records`)

- kind: red
- depends: 79, 3
- goal: тест записей по S3
- allowed_files: `script_vm/tests/lower_records_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: `type Point = { x: i32, y: i32 }`; индекс поля = порядок объявления
- red: unit: литерал `Point { x: 3, y: 4 }` и чтение `p.x + p.y` = 7; литерал с полями в обратном порядке `Point { y: 4, x: 3 }` даёт `p.x = 3`; запись `p.x = 10` для `let mut p`; запись в `const p` → `assign_immutable`; запись передаётся в функцию `fn sum_point(p: Point) -> i32` и возвращается из функции; запись с полем `bool`; `pin record_alias_shares_object`: `let mut q = p; q.x = 9` меняет `p.x` (ссылочная семантика S3 п.4); неизвестное поле → `unknown_field`; неизвестный тип записи → `unknown_record`; пропущенное поле литерала → `record_missing_field`; тип значения поля не совпадает → `type_mismatch`; поле записи вложенного типа → `unsupported_nested_record`
- green: none — red-шаг (реализация в следующем шаге)
- gate: `RED(lower_records)` → exit ≠ 0; первая упавшая метка
- sabotage: none — red-шаг
- performance: not a perf step
- done_when: RED падает на поведении
- state_update: команда и первая упавшая метка

### Step 81 — Green: записи

- kind: green
- depends: 80
- goal: раскладки записей, литерал, чтение/запись поля
- allowed_files: `script_vm/lower/lower_records.mlc`, `script_vm/lower/lower_expression.mlc`, `script_vm/lower/lower_statement.mlc`, `script_vm/lower/lower_module.mlc`, `script_vm/lower/lower_types.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `lower_records`), `STATE`
- forbidden: F0
- contract: `LowerRecordLayout { type_name, field_names, field_types }` из `DeclType` с одним вариантом-записью; литерал: `NEW_RECORD target, n`, затем для каждого поля в порядке записи в исходнике `SET_PROP target, declared_index, value_register`; чтение `GET_PROP`; запись `SET_PROP`; `LowerRecord(name)` проходит через параметры, результаты, локалы
- red: шаг 80
- green: реализовать
- gate: `RUN(lower_records)`; `GATE(lower_calls lower_records arrays_records)`; `SIZE`
- sabotage: разворачивать порядок полей (`declared_index = n - 1 - index`) → `RUN(lower_records)` падает; вернуть
- performance: not a perf step
- done_when: gate зелёные
- state_update: gate-команды

### Step 82 — Red: диагностики (`lower_diagnostics`)

- kind: red
- depends: 81
- goal: тест диагностик неподдерживаемых конструкций и ограничений
- allowed_files: `script_vm/tests/lower_diagnostics_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: S4 п.4, п.6, п.7, п.11; при любой диагностике `bytes` пусто; диагностики собираются все, а не первая; порядок — по возрастанию `(line, column)`
- red: unit: строковый литерал → `unsupported_string`; литерал массива → `unsupported_array`; `match` → `unsupported_match`; `for` → `unsupported_for`; лямбда → `unsupported_lambda`; вызов метода `x.foo()` → `unsupported_method`; `expr?` → `unsupported_question`; float-литерал → `unsupported_float`; `break`/`continue` → `unsupported_break`/`unsupported_continue`; generic-функция → `unsupported_generic`; `trait` → `unsupported_trait`; `extern` → `unsupported_extern`; экспорт с `f64`/записью в параметре или результате → `export_signature`; функция с 300 локалами (источник генерируется в тесте) → `too_many_registers`; запись из 256 полей → `record_too_wide`; два ошибочных statement в одной функции → ≥ 2 диагностик; у каждой line ≥ 1 и column ≥ 1
- green: none — red-шаг (реализация в следующем шаге)
- gate: `RED(lower_diagnostics)` → exit ≠ 0; первая упавшая метка
- sabotage: none — red-шаг
- performance: not a perf step
- done_when: RED падает на поведении
- state_update: команда и первая упавшая метка

### Step 83 — Green: диагностики

- kind: green
- depends: 82
- goal: единый сбор диагностик, `export_signature`, `too_many_registers`, `record_too_wide`
- allowed_files: `script_vm/lower/lower_types.mlc`, `script_vm/lower/lower_expression.mlc`, `script_vm/lower/lower_statement.mlc`, `script_vm/lower/lower_function.mlc`, `script_vm/lower/lower_module.mlc`, `script_vm/lower/lower_records.mlc`, `script_vm/lower/lower_diagnostics.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `lower_diagnostics`), `STATE`
- forbidden: F0
- contract: `lower_diagnostics.mlc` — накопитель (`ref mut`), `lower_diagnostic_add(sink, code, message, span)`, сортировка по `(line, column)`; `too_many_registers` — когда `register_allocate` вышел за 256; обход продолжается после ошибки (возвращается `LowerInt` как тип-заглушка)
- red: шаг 82
- green: реализовать
- gate: `RUN(lower_diagnostics)`; `GATE(lower_stage1 lower_arithmetic lower_control lower_logic lower_loops lower_returns lower_calls lower_records lower_diagnostics)`; `SIZE`
- sabotage: не проверять `export_signature` → `RUN(lower_diagnostics)` падает; вернуть
- performance: not a perf step
- done_when: gate зелёные
- state_update: gate-команды

### Step 84 — Red: CLI `script_tool` и его раннер

- kind: red
- depends: 83
- goal: тест CLI на фикстурах
- allowed_files: `scripts/run_script_vm_lower_tool.rb`, `script_vm/tests/fixtures/sum_loop.mlc`, `script_vm/tests/fixtures/fib.mlc`, `script_vm/tests/fixtures/gcd.mlc`, `script_vm/tests/fixtures/point.mlc`, `script_vm/tests/fixtures/nested_loops.mlc`, `script_vm/tests/fixtures/bad_unsupported.mlc`, `STATE`
- forbidden: F0; `script_vm/*.mlc`, `script_vm/tools/**`
- contract: каждая годная фикстура содержит `export fn run() -> i32` и `fn main() -> i32 = run()` (нативный mlcc исполняет `main`, script-lowerer — `run`), результат < 256; ожидаемые значения: `sum_loop` = 55 (сумма 1..10), `fib` = 55 (fib(10)), `gcd` = 6 (gcd(48, 18)), `point` = 7, `nested_loops` = 100 (10 × 10); `bad_unsupported.mlc` содержит строковый литерал; раннер собирает `script_vm/tools/script_tool.mlc` (mlcc + `build_bin.sh`, `tmp/script_vm_tool`), затем: `script_tool run <fixture> run` печатает `result=<n>` и exit 0; `script_tool compile <fixture> <out.txt>` пишет файл десятичных байтов через пробел, exit 0; на `bad_unsupported.mlc` — exit 1 и в stderr строка вида `<путь>:<line>:<column>: unsupported_string: <message>`; без аргументов — exit 2; каждый запуск под `Timeout` 20 с
- red: `ruby scripts/run_script_vm_lower_tool.rb` → exit ≠ 0 (нет `script_vm/tools/script_tool.mlc`)
- green: none — red-шаг (реализация в следующем шаге)
- gate: та же команда → exit ≠ 0, первая строка отказа в `last_verify`
- sabotage: none — red-шаг
- performance: not a perf step
- done_when: RED падает по отсутствию CLI
- state_update: команда и первая строка отказа

### Step 85 — Green: CLI `script_tool`

- kind: green
- depends: 84
- goal: реализовать `script_vm/tools/script_tool.mlc`
- allowed_files: `script_vm/tools/script_tool.mlc`, `STATE`
- forbidden: F0
- contract: аргументы командной строки читаются тем же способом, что `compiler/main.mlc` (прочитать, как он получает argv); `File.read` для входа, `File.write` для выхода; `run`: `lower_source`, при диагностиках — печать и exit 1; иначе `emb_call_export_bytes(bytes, export, args)` с целыми аргументами → `result=<n>` при `emb_ok`, иначе `error=<code>` и exit 3
- red: шаг 84
- green: реализовать
- gate: `ruby scripts/run_script_vm_lower_tool.rb` → exit 0 и последняя строка `lower_tool ok fixtures=5`
- sabotage: печатать `result=` на 1 больше → раннер падает на `sum_loop`; вернуть
- performance: not a perf step
- done_when: gate зелёный
- state_update: команда и последняя строка

### Step 86 — Дифференциальный раннер: нативный mlcc против script VM

- kind: green
- depends: 85
- goal: `scripts/run_script_vm_differential.rb` сравнивает результат нативной сборки фикстуры с результатом script VM
- allowed_files: `scripts/run_script_vm_differential.rb`, `STATE`
- forbidden: F0
- contract: для каждой фикстуры из `script_vm/tests/fixtures/*.mlc`, кроме `bad_*`: `compiler/out/mlcc -o tmp/script_vm_diff/<имя> <фикстура>` + `compiler/build_bin.sh` + запуск (код выхода = результат `main`); параллельно `script_tool run <фикстура> run`; печатает `differential fixture=<имя> native=<n> script=<n>`; расхождение → exit 1; итог `differential ok fixtures=5`; все запуски под `Timeout` 60 с (сборка) и 20 с (запуск)
- red: `ruby scripts/run_script_vm_differential.rb` до создания → exit ≠ 0 (файл отсутствует)
- green: создать раннер
- gate: `ruby scripts/run_script_vm_differential.rb` → exit 0 и `differential ok fixtures=5`
- sabotage: временно в `script_vm/tests/fixtures/gcd.mlc` изменить вычисление, чтобы script и native разошлись (например `main` вызывает `run() + 1`, а `run` экспортируется без изменений) → раннер exit 1; `git checkout -- script_vm/tests/fixtures/gcd.mlc`
- performance: not a perf step
- done_when: gate зелёный, sabotage красный
- state_update: пять строк `differential fixture=...` и итог

### Step 87 — Critic: дифференциал ловит ошибки lowerer

- kind: critic
- depends: 86
- goal: убедиться, что дифференциальный раннер несущий
- allowed_files: `PLAN100` (Decision log), `STATE`
- forbidden: F0; правки остаются временными и откатываются `git checkout -- <файл>`
- contract: ни один файл кроме `PLAN100`/`STATE` не остаётся изменённым
- red: none — critic
- green: по очереди применить три мутации lowerer, запустить `ruby scripts/run_script_vm_differential.rb`, убедиться, что exit 1 и названа фикстура, откатить: (1) в `lower_expression.mlc` для `-` использовать ADD; (2) в `lower_control.mlc` поменять ветки `if`; (3) в `lower_call.mlc` сместить аргументы на 1. Если мутация не ловится дифференциалом — записать, какая фикстура недостаёт, и добавить её в `script_vm/tests/fixtures/` в этом же шаге (разрешено: `script_vm/tests/fixtures/*.mlc` для этого шага). Дописать `### S87 — критика дифференциала`: мутация → упавшая фикстура
- gate: `ruby scripts/run_script_vm_differential.rb` после откатов → exit 0 `differential ok`; `git status --short script_vm` пуст, кроме возможных новых фикстур
- sabotage: none — сам шаг и есть sabotage-проход
- performance: not a perf step
- done_when: три мутации поймано (или фикстуры добавлены и мутации поймались повторно)
- state_update: таблица «мутация → упавшая фикстура»

### Step 88 — Замер: lowered-код против рукописного (S88)

- kind: measure
- depends: 87, 47
- goal: записать, насколько lowered-программа суммы дороже рукописной B1 (одно и то же N)
- allowed_files: `script_vm/tests/bench_entry.mlc`, `script_vm/tests/bench_current.txt`, `PLAN100` (Decision log), `STATE`
- forbidden: F0; любой код VM/lowerer
- contract: добавить метрику `B5.total_us`: lowerer-источник `export fn run() -> i32` с суммой 1..N, где N — то же значение, что в B1 (`bench_fixtures`); ожидаемый результат N*(N+1)/2 считается в бенче и обязан помещаться в i32, иначе шаг `blocked`; результат `emb_call_export_bytes` обязан совпасть с ожидаемым; `B5.ratio_x100 = B5.total_us * 100 / B1.total_us` (оба — min из 5 запусков; `bench_entry.mlc` печатает и `B1.total_us`)
- red: none — замер
- green: расширить бенч-вход; `BENCH(--record current)`; дописать `### S88 — lowered против рукописного` с числами
- gate: `BENCH()` печатает `B5.total_us` и `B5.ratio_x100`; значение результата совпадает; `GATE(lower_loops)`
- sabotage: изменить N в lowered-источнике на N+1 → проверка результата в бенче падает; вернуть
- performance: числа печатаются и записываются без порога; сравнение с B1 — информационное
- done_when: S88 дописан, бенч стабилен
- state_update: `B5.total_us`, `B5.ratio_x100`

### Фаза 6 — Завершение embedding (шаги 89–95, всего 7)

### Step 89 — Red: лимиты и коды через embedding (`embedding_limits`)

- kind: red
- depends: 88, 55, 57, 53
- goal: тест лимитов и новых кодов на уровне `embedding.mlc`, до реализации
- allowed_files: `script_vm/tests/embedding_limits_unit.mlc`, `STATE`
- forbidden: F0; `script_vm/*.mlc`
- contract: API (в `embedding.mlc`) — `emb_call_export_bytes_limited(bytes: [i32], export_name: string, arg_raws: [i64], instruction_limit: i64, heap_limit_objects: i32) -> EmbCallResult`; `emb_call_export_ptr_limited(data_pointer: i64, size: i32, export_name: string, args_pointer: i64, argc: i32, result_out_pointer: i64, instruction_limit: i64, heap_limit_objects: i32) -> i32`; в `embedding_codes.mlc`: `emb_instruction_limit() = 9`, `emb_heap_limit() = 10`, `emb_overflow() = 11`, `emb_div_zero() = 12`; `emb_call_export_bytes(…)` эквивалентна `_limited(…, 0, 0)`
- red: unit (модули — через `lower_source`): `while true do end`-функция с лимитом 1000 → `emb_instruction_limit()`; граница инструкций: для того же L, что проверяется в `instruction_limit_unit` (шаг 52) через `run_program_with_args_limited`, `emb_call_export_bytes_limited` даёт `emb_instruction_limit()` тогда и только тогда, когда `run_program_with_args_limited` вернул `RunErr("instruction_limit", …)`; функция, держащая две записи живыми (`const a = P{…}; const b = P{…}`), с `heap_limit_objects = 1` → `emb_heap_limit()`, с `2` → `emb_ok()`; цикл создания мусорных записей (100000 итераций) с лимитом кучи 0 завершается `emb_ok()` (автосборка); `a / b` при `b = 0` → `emb_div_zero()`; `INT_MIN / -1` → `emb_overflow()`; `a + b` с аргументом Bool → `emb_type()`; `emb_call_export_bytes` без лимитов зациклившейся функции здесь не запускать
- green: none — red-шаг (реализация в следующем шаге)
- gate: `RED(embedding_limits)` → exit ≠ 0 (нет `emb_call_export_bytes_limited`); первая строка ошибки
- sabotage: none — red-шаг
- performance: not a perf step
- done_when: RED падает по отсутствию функции
- state_update: команда и первая строка ошибки

### Step 90 — Green: лимиты и коды через embedding

- kind: green
- depends: 89
- goal: реализация шага 89
- allowed_files: `script_vm/embedding.mlc`, `script_vm/embedding_codes.mlc`, `scripts/run_script_vm_gate.rb` (регистрация `embedding_limits`), `STATE`
- forbidden: F0
- contract: `run_err_to_code` отображает `instruction_limit` → 9, `heap_limit` → 10, `overflow` → 11, `div_zero` → 12; остальные неизвестные коды (`no_return`, `unsupported_opcode`) → `emb_type()` (pin в `char_blob` обновляется в этом шаге: разрешено править `script_vm/tests/char_blob_unit.mlc` только в части отображения `overflow`/`div_zero`); `_limited` строит `RunLimits` и вызывает `run_program_with_args_limited`; безлимитные функции делегируют `_limited(…, 0, 0)`
- red: шаг 89
- green: реализовать; `embedding.mlc` ≤ 400
- gate: `RUN(embedding_limits)`; `GATE(EMBED embedding_limits instruction_limit heap_limit)`; `SIZE`
- sabotage: отображать `div_zero` в `emb_type()` → `RUN(embedding_limits)` падает; вернуть
- performance: not a perf step
- done_when: gate зелёные
- state_update: gate-команды; файл `char_blob_unit.mlc` в diff — только строки отображения кодов

### Step 91 — Red: C-хост с blob от lowerer (`lowered_host`)

- kind: red
- depends: 90, 85
- goal: хост C++ загружает blob, полученный `script_tool compile`, и проверяет лимиты через ABI
- allowed_files: `script_vm/tests/lowered_host.cpp`, `script_vm/tests/fixtures/bad_spin.mlc`, `script_vm/tests/fixtures/bad_two_records.mlc`, `script_vm/tests/fixtures/bad_divide.mlc`, `scripts/script_vm_host_support.rb`, `scripts/run_script_vm_lowered_host.rb`, `STATE`
- forbidden: F0; `runtime/**`; `script_vm/*.mlc`
- contract: `script_vm_host_support.rb` — функции сборки хоста по образцу `scripts/run_script_vm_embedding_abi_unit.sh`: `mlcc -o <каталог> script_vm/embedding.mlc` (без `main`), копия `runtime/src/script_vm/embedding_abi.cpp` и host `.cpp`, `compiler/build_bin.sh`, запуск под `Timeout` 20 с; `TMPDIR` внутри `tmp/`. Blob хост читает из файла десятичных байтов (формат `script_tool compile`). Фикстуры: `bad_spin.mlc` (`export fn spin() -> i32` с вечным циклом), `bad_two_records.mlc` (две живые записи одновременно, экспорт `run`), `bad_divide.mlc` (`export fn divide(a: i32, b: i32) -> i32 = a / b`). Кейсы хоста в порядке (fail-fast, каждая строка `case <имя> ok`): `sum_loop` → 55; `fib` → 55; `divide_zero` (b=0) → `MLC_DIV_ZERO`=12; `overflow` (INT_MIN, -1) → 11; `heap_limit` (`heap_limit_objects=1` на `bad_two_records`) → 10; `instruction_limit` (`instruction_limit=1000` на `bad_spin`) → 9. Значения Int32 кодируются через `mlc/script_vm/value_rep_abi.hpp`, как в `embedding_abi_host.cpp`. Итог раннера: `lowered_host ok cases=6`
- red: `ruby scripts/run_script_vm_lowered_host.rb` → exit ≠ 0: кейс `heap_limit` возвращает 0 вместо 10 (бридж не передаёт лимиты)
- green: none — red-шаг (реализация в следующем шаге)
- gate: та же команда → exit ≠ 0; в `last_verify` — строка первого упавшего кейса (ожидается `heap_limit`); если упал более ранний — записать, это расхождение с ожиданием
- sabotage: none — red-шаг
- performance: not a perf step
- done_when: RED падает именно на кейсе лимита
- state_update: команда и первый упавший кейс

### Step 92 — Green: бридж передаёт лимиты, enum кодов в заголовке

- kind: green
- depends: 91
- goal: `mlc_vm_call` применяет `instruction_limit` и `heap_limit_objects` из `MlcVmConfig`
- allowed_files: `runtime/src/script_vm/embedding_abi.cpp`, `runtime/include/mlc/script_vm/embedding_abi.h`, `STATE`
- forbidden: F0 (кроме двух перечисленных файлов `runtime/**`)
- contract: бридж вызывает `emb_call_export_ptr_limited(…, instruction_limit, heap_limit_objects)`, значение `uint64_t instruction_limit` > `INT64_MAX` клампится к `INT64_MAX`; в заголовок добавлен `enum MlcResultCode { MLC_OK = 0, MLC_UNSUPPORTED = 1, MLC_VERIFY = 2, MLC_TYPE = 3, MLC_BOUNDS = 4, MLC_STACK = 5, MLC_FORMAT = 6, MLC_NOT_FOUND = 7, MLC_BAD_HANDLE = 8, MLC_INSTRUCTION_LIMIT = 9, MLC_HEAP_LIMIT = 10, MLC_OVERFLOW = 11, MLC_DIV_ZERO = 12 }`; `MlcResult.code` остаётся `int32_t`; существующий `embedding_abi_host.cpp` не меняется и проходит
- red: шаг 91
- green: реализовать
- gate: `ruby scripts/run_script_vm_lowered_host.rb` → exit 0 и `lowered_host ok cases=6`; `bash scripts/run_script_vm_embedding_abi_unit.sh` → exit 0; `GATE(EMBED embedding_limits)`
- sabotage: передавать в `_limited` `0` вместо `instruction_limit` → кейс `instruction_limit` зависает до Timeout 20 с → раннер exit ≠ 0; вернуть
- performance: not a perf step
- done_when: gate зелёные
- state_update: gate-команды

### Step 93 — Red: жизненный цикл handle и неверное использование (`handle_misuse_host`)

- kind: red
- depends: 92
- goal: хост-тест ошибок использования ABI
- allowed_files: `script_vm/tests/handle_misuse_host.cpp`, `scripts/run_script_vm_handle_host.rb`, `STATE`
- forbidden: F0; `runtime/**`; `script_vm/*.mlc`
- contract: раннер использует `scripts/script_vm_host_support.rb`; blob — `mlc_script_vm_fixture_add1`; кейсы (fail-fast, `case <имя> ok`): `get_export_missing_export` (`mlc_vm_get_export(vm, "demo", "missing")` → 0); `get_export_unknown_module` → 0; `call_zero_handle` → `MLC_BAD_HANDLE`; `call_after_release` → `MLC_BAD_HANDLE`; `double_release_panics` (panic-колбэк вызван ровно раз, сообщение содержит `double release`, процесс жив); `release_unknown_handle_panics` (`mlc_handle_release(vm, 9999)` → колбэк, сообщение содержит `bad handle`); `null_arguments` (`mlc_vm_call(NULL, …)` → `MLC_TYPE`; `mlc_vm_load_module(vm, NULL, …)` → `MLC_TYPE`; `mlc_vm_call` с `result == NULL` → `MLC_TYPE`; `mlc_vm_destroy(NULL)` и `mlc_vm_get_export(NULL, …)` без падения); `garbage_blob` (байты `"hello"` → `MLC_FORMAT`, source-текст не поддерживается); `failed_load_keeps_module` (повторная загрузка мусора под именем `demo` → `MLC_FORMAT`, ранее загруженный add1 по-прежнему вызывается: `add1(41) = 42`); `argc_without_args` (`argc = 1`, `args = NULL` → `MLC_BOUNDS`). Итог: `handle_host ok cases=10`
- red: `ruby scripts/run_script_vm_handle_host.rb` → exit ≠ 0 на первом кейсе (`get_export_missing_export` возвращает ненулевой handle)
- green: none — red-шаг (реализация в следующем шаге)
- gate: та же команда → exit ≠ 0; в `last_verify` — первый упавший кейс
- sabotage: none — red-шаг
- performance: not a perf step
- done_when: RED падает на поведении ABI, а не на сборке хоста
- state_update: команда и первый упавший кейс

### Step 94 — Green: жизненный цикл handle

- kind: green
- depends: 93
- goal: валидация экспорта в `mlc_vm_get_export`, защита от NULL
- allowed_files: `script_vm/embedding.mlc`, `runtime/src/script_vm/embedding_abi.cpp`, `STATE`
- forbidden: F0 (кроме `runtime/src/script_vm/embedding_abi.cpp`)
- contract: `emb_has_export_ptr(data_pointer: i64, size: i32, export_name: string) -> i32` (1 — экспорт есть, 0 — нет); `mlc_vm_get_export` возвращает 0, если экспорта нет; `mlc_vm_call` при `argc > 0 && args == NULL` → `MLC_BOUNDS`; повторный релиз и релиз неизвестного handle сообщают через `panic_callback`, не падая; загрузка не-MLSC байтов — `MLC_FORMAT` (поведение сохраняется, исходный текст по ABI не загружается — вне очереди)
- red: шаг 93
- green: реализовать
- gate: `ruby scripts/run_script_vm_handle_host.rb` → exit 0 и `handle_host ok cases=10`; `ruby scripts/run_script_vm_lowered_host.rb` → exit 0; `bash scripts/run_script_vm_embedding_abi_unit.sh` → exit 0; `SIZE`
- sabotage: `mlc_vm_get_export` не вызывает `emb_has_export_ptr` → кейс `get_export_missing_export` падает; вернуть
- performance: not a perf step
- done_when: gate зелёные
- state_update: gate-команды

### Step 95 — Замер: стоимость вызова через ABI (S95)

- kind: measure
- depends: 94, 88
- goal: числа фиксированной стоимости одного `mlc_vm_call` (декодирование + верификация blob на каждый вызов)
- allowed_files: `script_vm/tests/call_overhead_host.cpp`, `script_vm/tests/fixtures/call_add1.mlc`, `scripts/run_script_vm_call_overhead.rb`, `PLAN100` (Decision log), `STATE`
- forbidden: F0; любой код VM/бриджа; решение о кэше декодирования принимается только записью в Decision log
- contract: `call_add1.mlc`: `export fn add1(a: i32) -> i32 = a + 1`; хост (`std::chrono::steady_clock`) загружает blob из `script_tool compile`; метрики: `trivial_call_ns` (одиночный `add1`, среднее по 1000 вызовов в батче, минимум из 5 батчей); `sum_call_ns` (вызов `run` из `sum_loop.mlc`, минимум из 5); `fixed_share_x100 = trivial_call_ns * 100 / sum_call_ns`; раннер печатает три строки `S95.<имя>=<значение>` под `Timeout` 20 с
- red: none — замер
- green: собрать, прогнать, дописать `### S95 — стоимость вызова` в Decision log: три числа и решение «кэш декодирования не входит в эту очередь» (если `fixed_share_x100` > 50 — добавить пункт «первый кандидат следующей очереди»)
- gate: `ruby scripts/run_script_vm_call_overhead.rb` → exit 0, три строки `S95.*`, значения > 0; повторный прогон — те же три метрики (числа могут отличаться)
- sabotage: подменить в хосте `add1` на несуществующий экспорт → раннер exit ≠ 0 (проверка, что провал виден); вернуть
- performance: числа печатаются и записываются без порога
- done_when: S95 дописан
- state_update: три метрики

### Фаза 7 — Критика и регрессия (шаги 96–100, всего 5)

### Step 96 — Единый gate очереди

- kind: green
- depends: 95
- goal: одна команда прогоняет всё, что очередь создала
- allowed_files: `scripts/run_script_vm_queue_gate.rb`, `STATE`
- forbidden: F0
- contract: стадии строго по порядку, fail-fast, имя стадии печатается до запуска: `size` (`SIZE`), `units` (`GATE(all)`), `embedding_abi_unit` (`bash scripts/run_script_vm_embedding_abi_unit.sh`), `lowered_host`, `handle_host`, `lower_tool`, `bench` (`BENCH(--max-ratio B1.ns_per_insn=current:110 --max-ratio B3.ns_per_call=current:110)`); флаг `--with-differential` добавляет стадию `differential` после `lower_tool`; вывод стадий идёт в терминал без буферизации; итог `script_vm_queue_gate ok stages=7` (с флагом — `stages=8`); при падении — exit 1 и `script_vm_queue_gate FAIL stage=<имя>`
- red: `ruby scripts/run_script_vm_queue_gate.rb` до создания → exit ≠ 0 (файл отсутствует)
- green: создать
- gate: `ruby scripts/run_script_vm_queue_gate.rb` → exit 0, `stages=7`; в `last_verify` — итоговая строка
- sabotage: временно изменить ожидаемое значение любого `RUN`-юнита (одну константу в тесте) → стадия `units` красная, `FAIL stage=units`; `git checkout -- <тест>`
- performance: допуск 110% в стадии `bench` — защитный; это не заявка на ускорение
- done_when: gate зелёный, sabotage красный
- state_update: итоговая строка и список стадий

### Step 97 — Critic: sabotage-проход по VM (10 мутаций)

- kind: critic
- depends: 96
- goal: доказать, что юниты VM ловят порчу семантики
- allowed_files: `script_vm/tests/sweep_gap_unit.mlc` (только при непойманной мутации), `scripts/run_script_vm_gate.rb` (регистрация `sweep_gap`), `PLAN100` (Decision log), `STATE`; мутации временные в `script_vm/*.mlc`
- forbidden: F0; ни одна мутация не коммитится: после каждой `git checkout -- <файл>`
- contract: для каждой мутации: применить, запустить указанный гейт, убедиться, что он красный, откатить; функции искать `rg -n 'fn <имя>' script_vm` (имя может принадлежать преемнику после шагов 22–46): (1) сложение в исполнителе арифметики заменить вычитанием → `GATE(ARITH)`; (2) убрать проверку `INT_MIN / -1` → `RUN(int32_div_overflow)`; (3) `value_jump_decision` для Int32(0) возвращает `1` (прыгать) → `RUN(falsy_contract)`; (4) убрать проверку глубины стека фреймов (`stack`) → `RUN(char_frames)`; (5) убрать bounds-проверку SET_PROP → `RUN(char_heap_ops)`; (6) в `heap_collect` (или `heap_in_place_collect`) не помечать дочерние объекты → `RUN(heap_collect)`; (7) в аллокации не использовать `free_object_ids` → `RUN(heap_reuse)`; (8) `vm_state_live_roots` возвращает пустой список → `RUN(gc_roots)`; (9) в `verify_function` пропустить проверку окна аргументов CALL → `RUN(verifier_hardening)`; (10) в `decode_module` пропустить проверку магии → `RUN(blob_limits)`. Если гейт зелёный (мутация не поймана): добавить кейс в новый `sweep_gap_unit.mlc` (red→green в этом же шаге: сначала убедиться, что новый кейс падает на мутации и проходит на чистом коде), зарегистрировать
- red: none — critic
- green: таблица «мутация → гейт → красный/зелёный» дописывается как `### S97 — sabotage VM`
- gate: `git status --short script_vm` пуст (кроме возможных `sweep_gap_unit.mlc`); `GATE(all)` → exit 0
- sabotage: none — шаг и есть sabotage-проход
- performance: not a perf step
- done_when: 10 строк таблицы, все мутации пойманы (непойманные закрыты `sweep_gap`)
- state_update: сколько мутаций пойманы с первого раза и сколько закрыты `sweep_gap`

### Step 98 — Critic: sabotage-проход по lowerer и embedding (10 мутаций)

- kind: critic
- depends: 97
- goal: доказать, что юниты lowerer и хост-тесты ловят порчу
- allowed_files: `script_vm/tests/sweep_gap_unit.mlc` (только при непойманной мутации), `PLAN100` (Decision log), `STATE`; мутации временные в `script_vm/lower/*.mlc`, `script_vm/module_blob.mlc`, `script_vm/embedding.mlc`, `runtime/src/script_vm/embedding_abi.cpp`
- forbidden: F0 (кроме временных мутаций в перечисленных файлах `runtime/**`); ни одна мутация не коммитится
- contract: (1) `proto_builder_patch_to_here` — смещение на 1 → `RUN(lower_builder)`; (2) `JUMP_IF_FALSE` в лоуэринге `if` ведёт в конец вместо `else` → `RUN(lower_control)`; (3) для `-` выпускается ADD → `RUN(lower_arithmetic)`; (4) блок не освобождает регистры локалов → `RUN(lower_diagnostics)` или `RUN(lower_loops)` (записать, какой поймал); (5) аргументы вызова смещены на 1 → `RUN(lower_calls)`; (6) индекс поля записи инвертирован → `RUN(lower_records)`; (7) `export_signature` не проверяется → `RUN(lower_diagnostics)`; (8) `module_encode` пишет `word_count + 1` → `RUN(module_encode)`; (9) бридж не передаёт `heap_limit_objects` → `ruby scripts/run_script_vm_lowered_host.rb`; (10) бридж не проверяет `live` при `mlc_handle_release` → `ruby scripts/run_script_vm_handle_host.rb`. Не пойманные — как в шаге 97 (`sweep_gap_unit.mlc`; для хост-мутации — новый кейс в соответствующем `*_host.cpp` разрешён в этом шаге)
- red: none — critic
- green: таблица дописывается как `### S98 — sabotage lowerer/embedding`
- gate: `git status --short script_vm runtime` пуст (кроме `sweep_gap_unit.mlc` и возможных кейсов хоста); `ruby scripts/run_script_vm_queue_gate.rb` → exit 0
- sabotage: none — шаг и есть sabotage-проход
- performance: not a perf step
- done_when: 10 строк таблицы, все мутации пойманы
- state_update: сколько пойманы с первого раза

### Step 99 — Critic: чистая пересборка и аудит области правок

- kind: critic
- depends: 98
- goal: доказать, что очередь воспроизводима с нуля и не вышла за свою область
- allowed_files: `PLAN100` (Decision log), `STATE`
- forbidden: F0; никаких правок кода
- contract: (а) `rm -rf tmp/script_vm_* .tmp/script_vm_*`; (б) `ruby scripts/run_script_vm_queue_gate.rb --with-differential` → exit 0, `stages=8`; (в) аудит области: `git log --grep '^script_vm queue step' --name-only --format=` — множество путей; каждый путь обязан начинаться с одного из префиксов `script_vm/`, `scripts/run_script_vm_`, `scripts/script_vm_`, `runtime/src/script_vm/`, `runtime/include/mlc/script_vm/`, `docs/agent/SCRIPT_VM_QUEUE_`; ни одного пути из `compiler/`, `lib/mlc/`, `misc/`, `docs/PLAN.md`, `docs/agent/SESSION.md`, `docs/agent/CONTINUITY.md`; (г) для каждого `N` в 1..98 `git log --oneline --grep "^script_vm queue step $N:"` находит ровно один коммит (для red-шагов — с суффиксом `(red)` в заголовке); (д) `git diff --stat -- compiler lib` пуст (чужие изменения `misc/textui/**` в рабочем дереве не относятся к очереди)
- red: none — critic
- green: дописать `### S99 — аудит`: итог пунктов (б)–(д), число коммитов очереди, число файлов в `script_vm/`
- gate: пункты (б)–(д) выполнены; нарушение любого → `blocked` с перечислением путей/номеров
- sabotage: none
- performance: not a perf step
- done_when: все пункты зелёные
- state_update: итоги пунктов (б)–(д)

### Step 100 — Закрытие: что ещё не язык

- kind: analysis
- depends: 99
- goal: зафиксировать результат и границу очереди
- allowed_files: `PLAN100` (Decision log), `STATE`
- forbidden: F0; любой код
- contract: дописать `### S100 — итог и граница`: (1) сводка измеренных чисел из S16 (baseline), S32/S47 (после), S88 (lowered/ручной), S95 (вызов) — копией из соответствующих записей, без пересчёта; (2) список «ещё не готово», минимум: строки, массивы в lowerer, float в lowerer, `break`/`continue`, `for`, `match`, лямбды и замыкания в lowerer, методы и generics, символьные ключи полей и имена полей в рантайме, ссылочная (не значимая) семантика записей против нативного mlcc (pin `record_alias_shares_object`), кэш декодирования blob, инкрементальный/движущийся GC, лимит памяти элементов, shapes и inline caches, JIT, загрузка исходного текста через C ABI, интеграция в textui chat, `mlcc --emit-script`, BigInt, eval; (3) перечень открытых блокеров, если очередь шла через `blocked` (из `last_result` в git-истории STATE — по `git log -p -- docs/agent/SCRIPT_VM_QUEUE_STATE.md`)
- red: none
- green: дописать запись
- gate: `ruby scripts/run_script_vm_queue_gate.rb` → exit 0 (`stages=7`)
- sabotage: none
- performance: not a perf step
- done_when: запись есть, gate зелёный; `STATE`: `current_step: 100`, `status: queue_complete`, `next_step: none`
- state_update: `status: queue_complete`

## Decision log

Шаги дописывают сюда записи `### S<номер шага> — тема`. Остальной текст файла не менять.

### S1 — Факты

Проверено чтением и `rg`/`wc` 2026-10-09. Код не менялся.

#### (a) `wc -l script_vm/*.mlc script_vm/tests/*.mlc`

| Файл | Строк | Пометка |
|------|------:|---------|
| script_vm/bytecode.mlc | 388 | CONFIRMED |
| script_vm/embedding.mlc | 410 | CONFIRMED |
| script_vm/heap.mlc | 496 | CONFIRMED |
| script_vm/interpreter.mlc | 977 | CONFIRMED |
| script_vm/value.mlc | 66 | CONFIRMED |
| script_vm/verifier.mlc | 258 | CONFIRMED |
| script_vm/tests/arrays_records_unit.mlc | 217 | CONFIRMED |
| script_vm/tests/bytecode_format_unit.mlc | 183 | CONFIRMED |
| script_vm/tests/closures_fibers_unit.mlc | 229 | CONFIRMED |
| script_vm/tests/control_flow_unit.mlc | 183 | CONFIRMED |
| script_vm/tests/embedding_abi_unit.mlc | 61 | CONFIRMED |
| script_vm/tests/heap_gc_arena_unit.mlc | 131 | CONFIRMED |
| script_vm/tests/interpreter_arithmetic_unit.mlc | 220 | CONFIRMED |
| script_vm/tests/value_rep_unit.mlc | 108 | CONFIRMED |
| script_vm/tests/verifier_unit.mlc | 121 | CONFIRMED |

Итог 4048. Четыре размера из раздела «Проверенные факты» (977, 496, 410, 388, 258) совпали. CONFIRMED.

#### (b) Опкод 1–25 × исполнитель

`handled` — есть ветка `instruction_opcode_*() then` внутри функции. `unsupported` — провал в `else` с `run_err_unsupported_opcode()`.

| Опкод | Имя | execute_with_heap (с 295) | execute_program (с 638) |
|------:|-----|---------------------------|-------------------------|
| 1 | LOAD_CONST | handled | handled |
| 2 | MOVE | handled | handled |
| 3 | ADD | handled | handled |
| 4 | SUB | handled | handled |
| 5 | MUL | handled | unsupported |
| 6 | DIV | handled | unsupported |
| 7 | RETURN | handled | handled |
| 8 | JUMP | handled | handled |
| 9 | JUMP_IF_FALSE | handled | handled |
| 10 | EQ | handled | unsupported |
| 11 | NE | handled | unsupported |
| 12 | LT | handled | unsupported |
| 13 | LE | handled | handled |
| 14 | GT | handled | unsupported |
| 15 | GE | handled | unsupported |
| 16 | NEW_ARRAY | handled | unsupported |
| 17 | GET_INDEX | handled | unsupported |
| 18 | SET_INDEX | handled | unsupported |
| 19 | NEW_RECORD | handled | unsupported |
| 20 | GET_PROP | handled | unsupported |
| 21 | SET_PROP | handled | unsupported |
| 22 | CALL | unsupported | handled |
| 23 | MAKE_CLOSURE | unsupported | handled |
| 24 | GET_UPVAL | unsupported | handled |
| 25 | SET_UPVAL | unsupported | handled |

`execute_with_heap` закрывает 1–21 и на остальном ставит `unsupported` (interpreter.mlc:520). `execute_program` закрывает ровно LOAD_CONST, MOVE, ADD, SUB, RETURN, JUMP, JUMP_IF_FALSE, LE, CALL, MAKE_CLOSURE, GET_UPVAL, SET_UPVAL и на остальном ставит `unsupported` (interpreter.mlc:907). CONFIRMED.

#### (c) Дыры из раздела «Проверенные факты»

| Утверждение | Пометка |
|-------------|---------|
| `registers_set` пересобирает массив регистров, interpreter.mlc:108 | CONFIRMED, тело 108–119 |
| Два исполнителя: `execute_with_heap` :295 (опкоды 1–21, один фрейм) и `execute_program` :638 (фреймы, узкий набор опкодов) | CONFIRMED |
| `execute_program` копирует `CallFrame` через `frames_set_last` на каждую успешную инструкцию; определение :552, первый вызов в цикле :685 | CONFIRMED |
| `value_is_falsy` :214: Nil, false, Int32(0), Float64(0.0) ложны; HeapRef истинен | CONFIRMED, тело 214–220 |
| DIV Int32 не ловит `INT_MIN / -1` | CORRECTED: деление без этой проверки — строка 152 (`left_number / right_number`); строка 150 — только делитель 0. Дыра на месте |
| `slots_with` :75 и `elements_with` :89 копируют весь массив; `heap_alloc_slot` :114 всегда `slots.push` (:136); freelist только пополняется в `heap_collect` :376 (`push` :425–431) | CONFIRMED |
| `mark_object` :334 рекурсивный через `mark_value` :320 и `mark_elements` :373 | CONFIRMED |
| У `mlc::Array` нет `.pop()` | CONFIRMED: `misc/examples/vm_pop.mlc:1`; в `runtime/include/mlc` по маске array нет `fn pop` и `.pop(` |
| `ref mut` есть и эмитится как `T&` | CONFIRMED: `compiler/checker/registry.mlc:10` параметр `ref mut TypeRegistry`; `compiler/out/registry.cpp:15` — `TypeRegistry&` |
| `instruction_limit` и `heap_limit_objects` хранятся и не применяются | CONFIRMED: присваивания только `embedding_abi.cpp:22`, `:23`, `:46`, `:48`; чтений в `embedding.mlc` нет |
| `mlc_vm_call` каждый раз декодирует и верифицирует blob | CONFIRMED: `embedding_abi.cpp:133` вызывает `emb_call_export_ptr`; `embedding.mlc:340` `decode_module`, `:344` `verify_decoded` |
| `mlc_vm_get_export` не проверяет, что экспорт есть | CONFIRMED: `embedding_abi.cpp:87–101` кладёт живой handle с именем, если модуль найден |
| `decode_module` не проверяет `proto_index` экспорта и `register_count` | CONFIRMED: `embedding.mlc:205–207` и `:226` только читают числа |
| Верификатор не проверяет окно аргументов CALL и локальные источники upvalue MAKE_CLOSURE | CONFIRMED: `verifier.mlc:164–167` проверяет только регистры A/B у CALL и A у MAKE_CLOSURE |
| Единственный перф-вывод — `ns_per_insn` для 4 инструкций в `interpreter_arithmetic_unit.mlc` | CONFIRMED: `:196` `instruction_count = 4`, `:215` печать `ns_per_insn` |
| Присваивание — `StmtExpr(ExprBin("=", ...))` | CORRECTED: форма верна в `compiler/frontend/parser/exprs.mlc:247`; у `ExprBin` четвёртое поле `Span` (`compiler/frontend/ast.mlc:106`) |
| `export fn` — `DeclExported(DeclFn(...))` | CONFIRMED: `compiler/frontend/parser/decls.mlc:468–470` оборачивает внутреннюю декларацию в `DeclExported` |
| `tokenize` — `compiler/frontend/lexer.mlc:520` | CONFIRMED |
| `parse_program_with_errors` — `compiler/frontend/parser/decls.mlc:1083` | CONFIRMED |
| Нет синтаксиса `export { X } from` | CONFIRMED: в `compiler/frontend` нет `export {` |
| Остаток §103f: mlcc мог терять операторы после `if/else-if` в `do`-блоке | CONFIRMED как текст трека; этим шагом заново не воспроизводился |
| `compiler/out/mlcc` существует | CONFIRMED: файл исполняемый, не пересобирался |

#### (d) Коды `emb_*` и `run_err_*`

`script_vm/embedding.mlc:45–53`: `emb_ok` 0, `emb_unsupported` 1, `emb_verify` 2, `emb_type` 3, `emb_bounds` 4, `emb_stack` 5, `emb_format` 6, `emb_not_found` 7, `emb_bad_handle` 8. CONFIRMED.

`script_vm/interpreter.mlc:90–97`: `run_err_verify` `"verify"`, `run_err_type` `"type"`, `run_err_overflow` `"overflow"`, `run_err_div_zero` `"div_zero"`, `run_err_no_return` `"no_return"`, `run_err_unsupported_opcode` `"unsupported_opcode"`, `run_err_bounds` `"bounds"`, `run_err_stack` `"stack"`. CONFIRMED.

`run_err_to_code` (`embedding.mlc:312–323`) переводит verify→2, type→3, bounds→4, stack→5; overflow, div_zero, no_return и unsupported_opcode попадают в ветку `else` и становятся `emb_type` (3). CONFIRMED.
