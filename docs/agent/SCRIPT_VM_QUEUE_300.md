# SCRIPT_VM_QUEUE_300 — шаги 201–300: срез VM без JIT

Состояние: [SCRIPT_VM_QUEUE_300_STATE.md](SCRIPT_VM_QUEUE_300_STATE.md). Этот файл — спецификация. Прогресс нигде больше не хранится.

Очереди 1–100 и 101–200 закрыты. Их файлы не править: `docs/agent/SCRIPT_VM_QUEUE_100.md`, `docs/agent/SCRIPT_VM_QUEUE_STATE.md`, `docs/agent/SCRIPT_VM_QUEUE_200.md`, `docs/agent/SCRIPT_VM_QUEUE_200_STATE.md`. Не путать с MIR VM (`compiler/vm/`, `mlcc --run`).

Готовность этой очереди — срез (Б), не весь пункт 1 §12. Внутри: float в лоуэре, `match`, лямбды и замыкания из исходника, методы без generics и без наследования, имя поля в диагностике. За этой сотней остаются generics, живая куча между вводами REPL, fibers, дамп VM, инкрементальный GC, лимит памяти элементов. Не входит: shapes, inline caches, JIT, opcode DSL, textui, `mlcc --emit-script`, eval, BigInt. Синтаксис `..` не добавлять.

Карточки взяты из ответа Sonnet `mlc-support/responses/script_vm_queue_next100_20261013_231652.md`. Исправления при переносе: шаг 201 зависит от 200; шаги 229, 249 и 275 зависят от конца предыдущей фазы; у шага 201 и у sabotage 293–296 названы файлы правки; модуль лоуэра из карточки `script_vm/lower_<тема>.mlc` создаётся как `script_vm/lower/lower_<тема>.mlc`.

## Общие правила

### Обозначения

- `STATE` = `docs/agent/SCRIPT_VM_QUEUE_300_STATE.md`
- `PLAN300` = `docs/agent/SCRIPT_VM_QUEUE_300.md`
- `F0`: `compiler/**`, `lib/mlc/**`, `misc/textui/**`, `misc/editor/**`, `docs/PLAN.md`, `docs/agent/SESSION.md`, `docs/agent/CONTINUITY.md`, `docs/agent/SCRIPT_VM_QUEUE_100.md`, `docs/agent/SCRIPT_VM_QUEUE_STATE.md`, `docs/agent/SCRIPT_VM_QUEUE_200.md`, `docs/agent/SCRIPT_VM_QUEUE_200_STATE.md`, любые другие `docs/**` кроме `PLAN300` и пути, который текущая карточка прямо перечислила в `allowed_files`, `runtime/**` вне файлов, которые карточка явно перечислила, `compiler/out/**`, чужие незакоммиченные правки.
- `RUN(name)` = `ruby scripts/run_script_vm_unit.rb script_vm/tests/<name>_unit.mlc`. Успех: exit 0 и строка `[script_vm] <name>_unit ok`.
- `RED(name)` = та же команда, exit ≠ 0. В `last_verify` — первая строка отказа. Красный unit не регистрируется в `scripts/run_script_vm_gate.rb`.
- `GATE(list)` = `ruby scripts/run_script_vm_gate.rb <list>`. Успех: последняя строка `script_vm_gate ok units=<число>`.
- `SIZE` = `ruby scripts/run_script_vm_file_size.rb`. Успех: `script_vm file size: ok`.
- `BENCH(args)` = `ruby scripts/run_script_vm_bench.rb <args>`.

Поле карточки, которого нет, значит `none`. `STATE` можно менять на каждом шаге, даже если карточка его не перечислила.

### Дисциплина

1. Один шаг — один ход. Шаг `N` выполняется, только если каждый `depends` имеет коммит `script_vm queue step <d>:`.
2. Red перед green. Red коммитит падающий тест. Провал компиляции годится как red только там, где карточка прямо говорит «нет символа». Иначе первая диагностика теста — поведенческая.
3. Self-host diff и `compiler/build.sh` не запускать. `compiler/out/mlcc` не пересобирать. Нет бинаря — `blocked`.
4. Идентификаторы без сокращений. Комментариев в новом коде нет, кроме списка инвариантов.
5. Скрипты — Ruby `scripts/run_script_vm_*.rb`. Не Python и не новый bash.
6. Каждый `script_vm/**/*.mlc` ≤ 400 строк. `lower_expression.mlc`, `lower_records.mlc` и `bytecode.mlc` не растить: новый код — в новом модуле. Имена модулей не совпадают с `compiler/frontend/**`. Новые модули лоуэра — `script_vm/lower/lower_<тема>.mlc`. Запись карточки `script_vm/lower_float.mlc`, `script_vm/lower_match.mlc`, `script_vm/lower_lambda.mlc`, `script_vm/lower_method.mlc` означает этот путь, не файл в корне `script_vm/`. Запись карточки `script_vm/test/test_<имя>.mlc` означает `script_vm/tests/<имя>_unit.mlc`. В общем gate это имя `<имя>`.
7. Публичные имена после разбиения сохраняются транзитивным `import { X } from './m'`.
8. Программы с возможным бесконечным циклом — только через Ruby `Timeout` 20 с.
9. Бенч: минимум из 5 запусков одной сборки. «Быстрее» — строго меньше эталона. Повтор один раз, затем `blocked` с числами.
10. Коммиты: `git add` явным списком. Сообщение `script_vm queue step <N>: <заголовок>` (red — с суффиксом ` (red)`). Без `--trailer`, без push. Чужие незакоммиченные правки не трогать. `tmp/` не коммитить.
11. В коммит входят изменённые файлы из `allowed_files`, `STATE`, и `PLAN300`, только если шаг дописывает Decision log.
12. В `PLAN300` дописывается только `## Decision log`: заголовок `### S<номер> — тема`. Заметка карточки в `docs/agent/notes/` пишется по пути из `allowed_files`, не вместо этого журнала.
13. Файл вне `allowed_files` или из `F0`, который текущий ответ Sonnet не назвал, — `blocked`.
14. Ответ Sonnet, который прямо называет путь и правку, разрешает менять этот файл, включая `compiler/`, `lib/mlc/`, `runtime/`, тесты. Неназванный файл не трогать.
15. Шаг, который заменяет код диагностики или начинает принимать конструкцию, которую уже зарегистрированный unit отвергает, добавляет каждый такой unit в `allowed_files` и в свой gate. Перед закрытием найти старый код через `rg` по `script_vm/tests`. Узкий `RUN` только нового unit этого не заменяет. Конечный critic такое не чинит: unit обновляет шаг, который сменил поведение.
16. В генерируемом MLC не писать `-1`: только `0 - 1`. У каждого `if` свой `end`. Результат функции — сам `if`/`else`, не утверждение после этой цепочки внутри `do`.

### Протокол состояния

Поля: `current_step`, `status`, `last_result`, `last_verify`, `next_step`, `consult_count`, `consult_response`, `consult_verdict`.

`status`: `ready`, `in_progress`, `consulting`, `consult_ready`, `blocked`, `queue_complete`.

- Обычный ход начинается с `status: in_progress`, остальные поля не менять.
- Gate прошёл: `current_step: N`, `last_result: done step N: <одна строка>`, `last_verify: <команда> => <исход>`, `next_step: N+1`, `status: ready`, `consult_count: 0`, `consult_response` и `consult_verdict` пустые. После шага 300: `status: queue_complete`, `next_step: none`.
- Gate не прошёл: `next_step` и `consult_count` не менять, `status: blocked`, `last_result: blocked step N: <причина>`, `last_verify` — команда и первые строки. Код не коммитить. Sonnet в этом ходу не вызывать.

### Консультация при остановке

Только `ruby /home/jenya/workspaces/current/mlc-support/review_script_vm_queue_consult.rb`. Не субагент Cursor и не другая модель. Скрипт принимает номер шага 1..300 и пути этого файла и `STATE`.

Ход при `blocked` и `consult_count: 0`, либо `consulting` без `kind: grounds`:

1. `status: consulting`. Код не менять.
2. Перезаписать `/home/jenya/workspaces/current/mlc-support/steps/script_vm_queue_consult.md`. Первая строка `step: <N>`. Дальше текст шага, `last_result`, `last_verify`, секция `## Files` — голые пути из `allowed_files`, без префикса `- `. `STATE` добавлять, если его нет в списке. Путь `script_vm/lower_<тема>.mlc` в списке заменять на `script_vm/lower/lower_<тема>.mlc`. Путь `script_vm/test/test_<имя>.mlc` заменять на `script_vm/tests/<имя>_unit.mlc`.
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

- `verdict: amend`, либо `stop`/пусто, но совет называет файлы и правку — выполнить правку один раз. Шаг `next_step` не начинать. После правки `consult_count: 0`, поля совета пустые, `status: ready`, `next_step` не менять. Коммит названных файлов и `STATE`. Сообщение `script_vm queue step <N>: правка правила остановки`. Если текст правки уже есть в названном файле и другой ещё не применённой правки нет — второй раз не применять и `ready` не ставить: `consult_verdict: stop`, `status: blocked`.
- `verdict: stop` или пустой verdict без правки файлов — `consult_verdict: stop`, `status: blocked`, остановиться.

`over_budget` и `stop` без неназванной ещё правки: API не вызывать, шаг не повторять.

### Шаблон шага

`kind`, `depends`, `goal`, `allowed_files`, `forbidden`, `contract`, `red`, `green`, `gate`, `sabotage`, `performance`, `done_when`, `state_update`.

## Идентичный промпт очереди

```
Ты исполнитель очереди SCRIPT_VM_QUEUE_300 (MLC Script VM, каталог script_vm/, шаги 201–300). Один ход — один шаг. Состояние хранится только в файле.

1. Прочитай docs/agent/SCRIPT_VM_QUEUE_300_STATE.md и docs/agent/SCRIPT_VM_QUEUE_300.md: разделы «Общие правила», «Протокол состояния», «Консультация при остановке» и «Проверка основания остановки», затем целиком шаг с номером next_step.
2. Если status равен queue_complete — ответь одной строкой «очередь завершена» и остановись.
3. Если current_step больше 0, команда `git log --oneline -1 --grep "^script_vm queue step <current_step>:"` обязана найти коммит. Не нашла — status blocked, consult_count не меняй, last_result «нет коммита шага <current_step>», остановись. Sonnet не вызывай.
4. Если status равен blocked и consult_verdict равен over_budget — API не вызывай, шаг не повторяй, остановись. Если status равен blocked и consult_verdict равен stop — API не вызывай. Прочитай consult_response. Если совет прямо называет файлы и правку, выполни её один раз и не жди человека. Файл, который совет не назвал, не трогай. Если такой правки нет — остановись. Если status равен blocked и consult_count равен 1, а consult_verdict не stop — код не правь. Выполни «Проверку основания остановки» и остановись. Если status равен blocked и consult_count больше 1, а ответ не называет ещё не применённую правку файлов — API не вызывай, шаг не повторяй, остановись.
5. Если status равен consulting, или status равен blocked и consult_count равен 0 — код не правь. Если в consult-файле есть `kind: grounds` для этого же next_step, выполни «Проверку основания остановки», иначе «Консультацию при остановке». Sonnet только через `ruby /home/jenya/workspaces/current/mlc-support/review_script_vm_queue_consult.rb`.
6. Если status равен consult_ready — прочитай consult_response. Первая строка `consult_step: <N> grounds`: `verdict: amend` примени один раз. Если эта правка уже есть в названном файле и другой правки нет — status blocked, consult_verdict stop, шаг не повторяй. `verdict: stop` или пустой verdict — если совет называет файлы и правку, выполни её как amend; иначе status blocked, consult_verdict stop, остановись. Иначе ищи `verdict:` совета по шагу. human или пустой: status blocked, consult_verdict human, остановись. over_budget: status blocked, consult_verdict over_budget, остановись. retry или patch: один раз выполни шаг next_step по совету. Названные файлы вне allowed_files правь. Неназванные не трогай.
7. Иначе запиши status: in_progress и выполни ровно шаг next_step.
8. Правь только allowed_files и STATE. Red до green. Gate запусти точно как записано. Sabotage, если он не none: испорти, дождись красного исхода, верни файл (`git checkout -- <файл>` только когда незакоммичена одна эта порча; иначе обратная правка), снова зелёный gate. Не запускай compiler/build.sh и не пересобирай compiler/out/mlcc.
9. Не редактируй docs/PLAN.md, docs/agent/SESSION.md, docs/agent/CONTINUITY.md, docs/agent/SCRIPT_VM_QUEUE_100.md, docs/agent/SCRIPT_VM_QUEUE_STATE.md, docs/agent/SCRIPT_VM_QUEUE_200.md, docs/agent/SCRIPT_VM_QUEUE_200_STATE.md, compiler/, lib/mlc/, misc/textui/, misc/editor/, кроме файлов, которые текущий ответ Sonnet прямо назвал. Чужие незакоммиченные правки не трогай.
10. Gate прошёл: обнови STATE по «Протоколу состояния». Закоммить явный список файлов шага и STATE (и PLAN300, если шаг дописывал Decision log). Сообщение `script_vm queue step <N>: <заголовок>` (red — с суффиксом ` (red)`). Без --trailer, без push.
11. Gate не прошёл: next_step и consult_count не меняй, status blocked, код не коммить, Sonnet в этом ходу не вызывай, следующий шаг не начинай.
12. Один шаг за ход. После коммита, консультации или blocked остановись и ответь коротко: номер шага, команда gate или путь consult_response, исход.
```

## Фазы

| Фаза | Шаги | Цель |
|------|------|------|
| 1. Решения | 201–206 | Что из float, match и лямбд лоуэр принимает без правки compiler/frontend |
| 2. Float | 207–228 | Литералы, арифметика, сравнения, поля записей, элементы массива |
| 3. match | 229–248 | match по i32, bool, строке и полю записи |
| 4. Лямбды | 249–274 | Замыкания из исходника, вызов значения-функции |
| 5. Методы | 275–290 | Методы без generics и без наследования, имя поля в диагностике |
| 6. Критика | 291–300 | Sabotage, общий gate, список остатка до конца (Б) и до (В) |

## Шаги

### Step 201 — Анализ: что frontend уже даёт для float-литералов
- kind: analysis
- depends: 200
- goal: Установить, какие AST-узлы float-литералов и float-арифметики уже производит `compiler/frontend`, без его изменения
- allowed_files: `docs/agent/notes/float_frontend_survey.md`, `STATE`
- contract: Документ перечисляет узлы AST для float-литерала, float-бинарных операций, типа `f32`/`f64` (какой есть), с конкретными путями файлов frontend и именами типов узлов
- red: none
- green: none
- gate: `test -f docs/agent/notes/float_frontend_survey.md`
- sabotage: none
- done_when: Документ содержит не менее одного реального примера AST-узла с указанием файла-источника в `compiler/frontend`

### Step 202 — Решение: формат float-значения в script_vm
- kind: decision
- depends: 201
- goal: Выбрать представление float-значения в регистрах/куче VM (какой тип машинного слова, какой тег значения)
- allowed_files: `docs/agent/notes/float_representation_decision.md`
- contract: Документ фиксирует: тег значения для float, ширина (32 или 64 бита), совместимость с существующим непрерывным буфером регистров
- red: none
- green: none
- gate: `test -f docs/agent/notes/float_representation_decision.md`
- sabotage: none
- done_when: Решение явно указывает, расширяется ли существующий тег значения или добавляется новый, без правки `script_vm/value.mlc` в этом шаге

### Step 203 — Анализ: совместимость float со сравнениями i32
- kind: analysis
- depends: 202
- goal: Проверить, не ломает ли добавление float-тега существующий верификатор CALL-окна и сравнения i32
- allowed_files: `docs/agent/notes/float_verifier_impact.md`
- contract: Документ перечисляет все места в `script_vm/verify*.mlc`, которые матчат тег значения, с указанием, требуют ли они новой ветки
- red: none
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb` (наблюдаем: количество unit не изменилось)
- sabotage: none
- done_when: Список мест полный, перепроверен по `grep` в `script_vm/verify*.mlc`

### Step 204 — Решение: opcode-набор для float-арифметики
- kind: decision
- depends: 203
- goal: Выбрать имена новых opcode (FADD, FSUB, FMUL, FDIV, FCMP_*) и их байт-кодировку, не пересекающуюся с существующими
- allowed_files: `docs/agent/notes/float_opcode_decision.md`
- contract: Таблица opcode: имя, числовой код, операнды, не конфликтует с `script_vm/bytecode.mlc` существующими кодами
- red: none
- green: none
- gate: `ruby scripts/run_script_vm_queue200_gate.rb`
- sabotage: none
- done_when: Таблица содержит все float-операции, нужные для литерала, арифметики, сравнения, присваивания полю и элементу массива

### Step 205 — Решение: деление на ноль и NaN для float
- kind: decision
- depends: 204
- goal: Определить семантику `1.0 / 0.0` и NaN-сравнений в script VM (вернуть IEEE-754 inf/NaN, а не код ошибки, как у int)
- allowed_files: `docs/agent/notes/float_division_semantics.md`
- contract: Документ явно говорит: float-деление на ноль не считается ошибкой рантайма, в отличие от int-деления; NaN сравнение через `==` даёт `false`
- red: none
- green: none
- gate: none
- sabotage: none
- done_when: Семантика сформулирована так, что её можно проверить unit-тестом без доп. решений
- performance: none

### Step 206 — Решение: синтаксис float-литерала в источнике
- kind: decision
- depends: 201
- goal: Зафиксировать, какой синтаксис float-литерала (`1.0`, `1.5`) лоуэр обязан принимать, опираясь только на то, что уже парсит frontend
- allowed_files: `docs/agent/notes/float_literal_syntax_decision.md`
- contract: Документ ссылается на конкретный узел AST из шага 201, подтверждает, что парсер (frontend) не требует изменений
- red: none
- green: none
- gate: none
- sabotage: none
- done_when: Явно указано, что `..` синтаксис диапазона тут ни при чём и не затрагивается

### Step 207 — Red: float-литерал как результат функции
- kind: red
- depends: 206, 204
- goal: Зарегистрировать красный unit: `fn f() -> f32 do return 1.5 end` должен исполниться и дать float-результат
- allowed_files: `script_vm/test/test_lower_float_literal.mlc` (новый), `scripts/run_script_vm_gate.rb`
- contract: Тест не регистрируется в `run_script_vm_queue200_gate.rb` до green
- red: Тест падает — лоуэр отвергает float-литерал как неизвестную конструкцию
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb` (наблюдаем явный провал нового unit, остальные 51 — ok)
- sabotage: none
- done_when: Красный тест воспроизводим, причина падения — отсутствие ветки в лоуэре, не опечатка в тесте

### Step 208 — Green: лоуэр float-литерала
- kind: green
- depends: 207
- goal: Лоуэр принимает float-литерал, эмитит константу float-типа, VM возвращает float-результат
- allowed_files: `script_vm/lower/lower_float.mlc`, `script_vm/lower/lower_expression.mlc`, `script_vm/lower/lower_diagnostics.mlc`, `script_vm/value.mlc`, `script_vm/tests/lower_float_literal_unit.mlc`, `script_vm/tests/lower_diagnostics_unit.mlc`
- contract: `lower_expression.mlc` и `bytecode.mlc` не растут — новая логика только в `lower_float.mlc`; каждый файл ≤ 400 строк
- red: none
- green: Тест шага 207 проходит
- gate: `ruby scripts/run_script_vm_gate.rb` (последняя строка подтверждает 52/52 ok)
- sabotage: Откат `lower_float.mlc` к пустой реализации должен вернуть тест в красное состояние
- done_when: Тест зарегистрирован и зелёный, `script_vm/**/*.mlc` каждый ≤ 400 строк

### Step 209 — Red: float-сложение
- kind: red
- depends: 208
- goal: `1.5 + 2.5` даёт `4.0`
- allowed_files: `script_vm/test/test_lower_float_arith.mlc` (новый)
- contract: Тест не в общем gate до green
- red: Падает — опкод FADD не эмитится
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Красный тест воспроизводим

### Step 210 — Green: float-арифметика `+ - * /`
- kind: green
- depends: 209, 205
- goal: Лоуэр эмитит FADD/FSUB/FMUL/FDIV для float-операндов, исполнитель их выполняет по семантике шага 205
- allowed_files: `script_vm/lower/lower_float.mlc`, `script_vm/exec_float.mlc`, `script_vm/lower/lower_expression.mlc`, `script_vm/verifier.mlc`, `script_vm/executor.mlc`, `scripts/run_script_vm_gate.rb`, `script_vm/tests/lower_float_arith_unit.mlc`
- contract: Новая логика исполнения — в `exec_float.mlc`, не в основном цикле исполнителя напрямую (точка входа — один диспетч на новый opcode)
- red: none
- green: Тест шага 209 и деление-на-ноль из 205 проходят
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: Замена FDIV на целочисленное деление должна провалить тест NaN/inf
- done_when: Все 4 операции покрыты, деление на ноль даёт inf, не код ошибки

### Step 211 — Red: смешение int и float запрещено без явного решения
- kind: red
- depends: 210
- goal: `1 + 1.5` должен быть именованным отказом лоуэра (не падением VM), код ошибки зафиксирован
- allowed_files: `script_vm/test/test_lower_float_mixed_reject.mlc` (новый)
- contract: Тест ожидает код отказа, не success и не crash
- red: Падает — сейчас либо тихо приводит типы, либо падает без кода
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Красный тест демонстрирует текущее некорректное поведение

### Step 212 — Green: отказ на смешанной int/float арифметике
- kind: green
- depends: 211
- goal: Лоуэр проверяет типы операндов арифметики и даёт именованный код отказа при смешении int/float без явного преобразования в языке
- allowed_files: `script_vm/lower_float.mlc`, `script_vm/lower_expression.mlc` (только точка вызова проверки типа, не рост логики), `script_vm/test/test_lower_float_mixed_reject.mlc`
- contract: В `lower_expression.mlc` добавляется не более одного вызова функции из `lower_float.mlc`
- red: none
- green: Тест 211 проходит
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: Удаление проверки типа должно вернуть тест в красное
- done_when: `lower_expression.mlc` не превысил 400 строк и не вырос более чем на 10 строк

### Step 213 — Red: float-сравнения
- kind: red
- depends: 212
- goal: `1.5 < 2.5`, `1.5 == 1.5`, `1.5 != 1.5` как `bool`-результат
- allowed_files: `script_vm/test/test_lower_float_compare.mlc` (новый)
- contract: none
- red: Падает — FCMP не эмитится
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Красный тест воспроизводим для `< <= > >= == !=`

### Step 214 — Green: float-сравнения
- kind: green
- depends: 213
- goal: Лоуэр и исполнитель поддерживают все 6 операций сравнения float, включая корректную семантику NaN (`NaN != NaN` истинно)
- allowed_files: `script_vm/lower_float.mlc`, `script_vm/exec_float.mlc`, `script_vm/lower/lower_expression.mlc`, `script_vm/executor.mlc`, `script_vm/verifier.mlc`, `script_vm/test/test_lower_float_compare.mlc`
- contract: В `lower_expression.mlc` и `executor.mlc`/`verifier.mlc` правка — только добавление диспетч-ветки на новый диапазон opcode (32–37) и типа `ValueBool`; рост общей логики сравнения внутри этих файлов не допускается — она остаётся в `lower_float.mlc`/`exec_float.mlc`.
- red: none
- green: Тест 213 проходит, включая NaN-кейс
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: Замена IEEE-сравнения на побитовое должно провалить NaN-кейс
- done_when: Все 6 операций и NaN-кейс зелёные

### Step 215 — Red: float как поле записи
- kind: red
- depends: 214
- goal: Запись с полем типа `f32`, чтение и запись поля
- allowed_files: `script_vm/test/test_lower_float_record_field.mlc` (новый)
- contract: none
- red: Падает — лоуэр записей не знает тип поля float
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Красный тест воспроизводим на чтении и на записи поля

### Step 216 — Green: float-поле записи
- kind: green
- depends: 215
- goal: `lower_records.mlc` принимает поле типа float без роста сверх текущего размера — новая ветка выносится в `lower_float_records.mlc`
- allowed_files: `script_vm/lower_float_records.mlc` (новый), `script_vm/lower_records.mlc` (только точка вызова), `script_vm/test/test_lower_float_record_field.mlc`
- contract: `lower_records.mlc` не растёт более чем на 10 строк — вся новая логика в `lower_float_records.mlc`
- red: none
- green: Тест 215 проходит
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: Откат `lower_float_records.mlc` должен вернуть красное
- done_when: Чтение и запись float-поля работают, pin `record_alias_shares_object` не нарушен (проверено отдельным assert в тесте)

### Step 217 — Red: float как элемент массива
- kind: red
- depends: 216
- goal: Массив `f32`: литерал, чтение индекса, запись индекса, `len`
- allowed_files: `script_vm/test/test_lower_float_array.mlc` (новый)
- contract: none
- red: Падает — массив float не поддержан
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Красный тест воспроизводим на все 4 операции

### Step 218 — Green: float-массив
- kind: green
- depends: 217
- goal: Лоуэр массивов принимает элемент float
- allowed_files: `script_vm/lower_float_array.mlc` (новый), `script_vm/lower_arrays.mlc` (точка вызова, если существует отдельно; иначе — решение откладывается на 219)
- contract: Вложенный массив float внутри массива остаётся отказом, как для i32/bool
- red: none
- green: Тест 217 проходит
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: Удаление ветки float должно вернуть красное на литерале
- done_when: Все 4 операции зелёные, вложенность по-прежнему отказ

### Step 219 — Decision: имя модуля массивов, если его не было отдельно
- kind: decision
- depends: 218
- goal: Зафиксировать, существует ли `script_vm/lower_arrays.mlc` как отдельный модуль; если логика массивов была в `lower_expression.mlc`, решить план выноса
- allowed_files: `docs/agent/notes/lower_arrays_module_decision.md`
- contract: Если вынос нужен — следующий шаг рефакторинга получает чёткое ТЗ
- red: none
- green: none
- gate: none
- sabotage: none
- done_when: Решение явно отвечает да/нет на вопрос выноса

### Step 220 — Refactor: вынос логики массивов (если решено в 219)
- kind: refactor
- depends: 219
- goal: Если требуется — вынести общую логику массивов из `lower_expression.mlc` в отдельный модуль без изменения поведения
- allowed_files: `script_vm/lower_expression.mlc`, `script_vm/lower_arrays.mlc` (новый, если нужен)
- contract: Поведение всех существующих array-тестов не меняется
- red: none
- green: none
- gate: `ruby scripts/run_script_vm_queue200_gate.rb`
- sabotage: none
- done_when: `lower_expression.mlc` уменьшился в строках ровно на объём вынесенной логики, gate зелёный

### Step 221 — Red: float-параметр и возврат функции
- kind: red
- depends: 218
- goal: `fn f(x: f32) -> f32 do return x * 2.0 end`, вызов с float-аргументом
- allowed_files: `script_vm/test/test_lower_float_function.mlc` (новый)
- contract: none
- red: Падает — CALL-окно верификатора не знает про float-слот
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Красный тест воспроизводим именно на верификаторе CALL

### Step 222 — Green: float в CALL-окне верификатора
- kind: green
- depends: 221
- goal: Верификатор принимает float-тег в параметрах и возврате CALL (включая лоуэр, если отказ возникает до верификатора)
- allowed_files: `script_vm/verify_call.mlc` (или существующий модуль верификации CALL — указать точно по результатам 203), `script_vm/test/test_lower_float_function.mlc`, `script_vm/lower_function.mlc`, `script_vm/lower_call.mlc`, `script_vm/lower/lower_expression.mlc`
- contract: Окно CALL для int/bool не меняет поведения — только добавление ветки. Если тег значения float отвергается раньше верификатора — в лоуэре функций/вызова, добавить ветку принятия float-тега там же, без роста общей логики за пределы точки вызова; верификатор правится только если проверка типов CALL-окна реализована именно в нём.
- red: none
- green: Тест 221 проходит
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: Удаление float-ветки верификатора должно вернуть красное именно на этапе верификации, не исполнения
- done_when: Существующие тесты верификатора (из S200) остаются зелёными

### Step 223 — Measure: стоимость float-арифметики относительно int
- kind: measure
- depends: 222
- goal: Замерить `ns_per_insn` для FADD/FMUL в сравнении с существующим baseline B1=35
- allowed_files: `scripts/run_script_vm_float_bench.rb` (новый)
- contract: Измерение воспроизводимо, печатает число, не меняет поведение VM
- red: none
- green: none
- gate: `ruby scripts/run_script_vm_float_bench.rb` (наблюдаем печать `float_ns_per_insn=<число>`)
- sabotage: none
- done_when: Число зафиксировано в выводе, не регресс более чем в 3 раза относительно B1

### Step 224 — Critic: float-срез
- kind: critic
- depends: 208, 210, 212, 214, 216, 218, 222
- goal: Проверить весь float-срез на согласованность: deno на границах int/float, NaN, деление на ноль, записи, массивы, функции
- allowed_files: `script_vm/test/test_lower_float_critic.mlc` (новый), любой файл float-модуля для исправления найденных дефектов
- contract: Критик имеет право чинить найденные баги в пределах float-модулей, не трогая `lower_expression.mlc`/`lower_records.mlc`/`bytecode.mlc` сверх точки вызова
- red: none
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: Один из найденных дефектов фиксируется как regression-тест с подтверждённым красным до правки
- done_when: Все float-тесты зелёные одним прогоном

### Step 225 — Green: включение float-тестов в общий gate
- kind: green
- depends: 224
- goal: Все float unit (207–224) регистрируются в `run_script_vm_queue200_gate.rb`
- allowed_files: `scripts/run_script_vm_queue200_gate.rb`
- contract: Список добавленных unit явный, без wildcard-подключения каталога
- red: none
- green: none
- gate: `ruby scripts/run_script_vm_queue200_gate.rb` (последняя строка `script_vm_queue200_gate ok`)
- sabotage: Удаление одной строки регистрации должно провалить gate
- done_when: Gate зелёный, число зарегистрированных unit выросло ровно на число float-тестов

### Step 226 — Red: float в `if`-условии через сравнение
- kind: red
- depends: 214
- goal: `if 1.5 < 2.5 do ... end` работает как условие (результат FCMP — bool, уже подаётся в JUMP_IF_FALSE)
- allowed_files: `script_vm/test/test_lower_float_if_condition.mlc` (новый)
- contract: none
- red: Падает, если лоуэр `if` не знает, что FCMP возвращает валидный bool-регистр
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Красный тест воспроизводим

### Step 227 — Green: float-условие в `if`/`while`
- kind: green
- depends: 226
- goal: Условие float-сравнения работает в `if` и `while` без изменения семантики `JUMP_IF_FALSE` (он по-прежнему прыгает только на `Bool(false)`)
- allowed_files: `script_vm/lower_float.mlc`, `script_vm/test/test_lower_float_if_condition.mlc`
- contract: `JUMP_IF_FALSE` не меняется — float-сравнение уже даёт `Bool`, интеграция только на уровне лоуэра
- red: none
- green: Тест 226 проходит
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Тест зелёный, `JUMP_IF_FALSE` opcode не тронут (diff пустой на файле opcode)

### Step 228 — Measure: полный float-срез против queue200 gate
- kind: measure
- depends: 225, 227
- goal: Подтвердить, что добавление float не ухудшило `B1.ns_per_insn` и `B3.ns_per_call` сверх допустимого
- allowed_files: `scripts/run_script_vm_float_bench.rb`
- contract: Сравнение с зафиксированными числами 35 и 1242
- red: none
- green: none
- gate: `ruby scripts/run_script_vm_float_bench.rb`
- sabotage: none
- done_when: Регресс не более 20% от baseline S200, число зафиксировано в выводе шага

---

### Step 229 — Анализ: AST узлы `match` во frontend
- kind: analysis
- depends: 228
- goal: Установить, какие формы `match` (по `i32`, `bool`, записи, строкам) уже парсит `compiler/frontend`
- allowed_files: `docs/agent/notes/match_frontend_survey.md`
- contract: Документ перечисляет поддерживаемые формы паттернов (литерал, wildcard `_`, биндинг переменной, деструктуризация записи) с указанием файла-источника AST
- red: none
- green: none
- gate: `test -f docs/agent/notes/match_frontend_survey.md`
- sabotage: none
- done_when: Для каждой формы паттерна указан конкретный тип узла AST

### Step 230 — Decision: подмножество `match`, принимаемое лоуэром
- kind: decision
- depends: 229
- goal: Выбрать срез `match`: литералы `i32`/`bool`, wildcard `_`, биндинг, без деструктуризации записи на этом шаге
- allowed_files: `docs/agent/notes/match_scope_decision.md`
- contract: Явно исключена деструктуризация записи и вложенные паттерны — они не входят в 231–248
- red: none
- green: none
- gate: none
- sabotage: none
- done_when: Список включённых и исключённых форм паттернов зафиксирован

### Step 231 — Decision: компиляция `match` в существующие opcode
- kind: decision
- depends: 230
- goal: Решить, компилируется ли `match` в цепочку сравнений + JUMP_IF_FALSE (без новых opcode) или требует новый opcode
- allowed_files: `docs/agent/notes/match_lowering_strategy.md`
- contract: Предпочтение отдаётся варианту без новых opcode, если он не хуже по характеристикам (обоснование в документе)
- red: none
- green: none
- gate: none
- sabotage: none
- done_when: Стратегия выбрана однозначно, с обоснованием через существующие измерения B1/B3

### Step 232 — Red: `match` по `i32`-литералам
- kind: red
- depends: 231
- goal: `match x do 1 -> 10 2 -> 20 _ -> 0 end` как выражение-результат функции
- allowed_files: `script_vm/test/test_lower_match_int.mlc` (новый)
- contract: none
- red: Падает — `match` именованный отказ
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Красный тест воспроизводим на 3 ветках, включая wildcard

### Step 233 — Green: `match` по `i32`-литералам
- kind: green
- depends: 232
- goal: Лоуэр компилирует `match` по i32-литералам в цепочку сравнений согласно стратегии 231
- allowed_files: `script_vm/lower_match.mlc` (новый модуль)
- contract: `lower_expression.mlc` получает не более одной точки вызова в `lower_match.mlc`
- red: none
- green: Тест 232 проходит
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: Откат `lower_match.mlc` к заглушке должен вернуть красное
- done_when: Тест зелёный, `lower_match.mlc` ≤ 400 строк

### Step 234 — Red: `match` без wildcard — отказ на непокрытости
- kind: red
- depends: 233
- goal: `match x do 1 -> 10 2 -> 20 end` без `_` должен быть именованным отказом лоуэра (неполное покрытие для `i32` недоказуемо статически)
- allowed_files: `script_vm/test/test_lower_match_no_wildcard_reject.mlc` (новый)
- contract: Код отказа отличим от прочих отказов
- red: Падает — сейчас либо падает без кода, либо тихо пропускает
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Красный тест демонстрирует текущее некорректное поведение

### Step 235 — Green: обязательность wildcard в `match` по `i32`
- kind: green
- depends: 234
- goal: Лоуэр требует `_`-ветку (или полное перечисление — не в этом срезе) для `match` по `i32`, иначе даёт именованный код отказа
- allowed_files: `script_vm/lower_match.mlc`, `script_vm/test/test_lower_match_no_wildcard_reject.mlc`
- contract: none
- red: none
- green: Тест 234 проходит
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: Удаление проверки должно вернуть красное
- done_when: Тест зелёный

### Step 236 — Red: `match` по `bool`
- kind: red
- depends: 235
- goal: `match flag do true -> 1 false -> 0 end`
- allowed_files: `script_vm/test/test_lower_match_bool.mlc` (новый)
- contract: none
- red: Падает
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Красный тест воспроизводим

### Step 237 — Green: `match` по `bool`
- kind: green
- depends: 236
- goal: Лоуэр принимает `match` по `bool` без wildcard, если покрыты обе ветки `true`/`false` (это статически полное покрытие)
- allowed_files: `script_vm/lower_match.mlc`, `script_vm/test/test_lower_match_bool.mlc`
- contract: Полнота покрытия bool проверяется как отдельная ветка проверки, не переиспользует неполную логику i32 "требуется wildcard"
- red: none
- green: Тест 236 проходит
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Тест зелёный, `match` по bool без `_` принимается, если обе ветки есть

### Step 238 — Red: `match` с биндингом переменной
- kind: red
- depends: 235
- goal: `match x do n -> n + 1 end` — биндинг вместо литерала, переменная `n` доступна в теле ветки
- allowed_files: `script_vm/test/test_lower_match_binding.mlc` (новый)
- contract: none
- red: Падает — биндинг не распознан
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Красный тест воспроизводим

### Step 239 — Green: биндинг переменной в `match`
- kind: green
- depends: 238
- goal: Лоуэр вводит локальную переменную биндинга, видимую только в теле соответствующей ветки
- allowed_files: `script_vm/lower_match.mlc`, `script_vm/test/test_lower_match_binding.mlc`
- contract: Биндинг действует как catch-all (эквивалент wildcard) — после него другие ветки недостижимы; это тоже проверяется
- red: none
- green: Тест 238 проходит
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: Удаление видимости переменной вне scope ветки должно провалить отдельный scope-тест
- done_when: Тест зелёный

### Step 240 — Red: `match` как statement, не только выражение
- kind: red
- depends: 233
- goal: `match x do 1 -> do_something() 2 -> do_other() end` без использования результата
- allowed_files: `script_vm/test/test_lower_match_statement.mlc` (новый)
- contract: none
- red: Падает, если лоуэр `match` поддержан только в позиции выражения-результата
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Красный тест воспроизводим

### Step 241 — Green: `match` как statement
- kind: green
- depends: 240
- goal: Лоуэр принимает `match` в позиции statement (без требования полноты покрытия и без значения результата)
- allowed_files: `script_vm/lower_match.mlc`, `script_vm/test/test_lower_match_statement.mlc`
- contract: Statement-форма не требует wildcard (решение зафиксировать явно в этом шаге: неполное покрытие допустимо, если результат не используется)
- red: none
- green: Тест 240 проходит
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Тест зелёный, различие statement/expression форм задокументировано в комментарии модуля

### Step 242 — Red: `match` как условие в `while`
- kind: red
- depends: 239
- goal: `match` внутри тела `while`, использующий `break`/`continue` в ветке
- allowed_files: `script_vm/test/test_lower_match_in_loop.mlc` (новый)
- contract: none
- red: Падает, если `break`/`continue` внутри ветки `match` не находят охватывающий `while`
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Красный тест воспроизводим

### Step 243 — Green: `break`/`continue` внутри ветки `match` в цикле
- kind: green
- depends: 242
- goal: Лоуэр корректно передаёт контекст охватывающего цикла внутрь веток `match`
- allowed_files: `script_vm/lower_match.mlc`, `script_vm/test/test_lower_match_in_loop.mlc`
- contract: `break_outside_loop`/`continue_outside_loop` по-прежнему корректно детектируются, если `match` вне цикла
- red: none
- green: Тест 242 проходит
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: Удаление передачи контекста цикла должно провалить тест, но не сломать detection вне цикла
- done_when: Тест зелёный, регрессия на `break_outside_loop` не появилась (проверено существующим unit)

### Step 244 — Measure: стоимость `match` относительно `if`-цепочки
- kind: measure
- depends: 243
- goal: Сравнить `ns_per_insn` для `match` с 5 ветками против эквивалентной ручной `if`/`else if`-цепочки
- allowed_files: `scripts/run_script_vm_match_bench.rb` (новый)
- contract: Сравнение приводится в выводе как два числа
- red: none
- green: none
- gate: `ruby scripts/run_script_vm_match_bench.rb`
- sabotage: none
- done_when: Разница не более 10% (подтверждает, что стратегия 231 не ввела накладных расходов)

### Step 245 — Critic: match-срез
- kind: critic
- depends: 233, 235, 237, 239, 241, 243
- goal: Проверить согласованность всех форм `match`: вложенность `match` внутри `match`, `match` внутри функции с ранним `return` в ветке
- allowed_files: `script_vm/test/test_lower_match_critic.mlc` (новый), файлы match-модуля для исправлений
- contract: Критик правит найденные дефекты только в `lower_match.mlc`
- red: none
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: Один найденный дефект фиксируется regression-тестом с подтверждённым красным до правки
- done_when: Все match-тесты зелёные одним прогоном

### Step 246 — Red: отказ на `match` по записи (деструктуризация)
- kind: red
- depends: 230
- goal: Подтвердить, что `match` по записи с деструктуризацией полей остаётся именованным отказом (не регресс в "тихое игнорирование")
- allowed_files: `script_vm/test/test_lower_match_record_reject.mlc` (новый)
- contract: none
- red: Падает, если сейчас это не отказ, а крэш или неверный результат
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Красный тест воспроизводим

### Step 247 — Green: именованный отказ на деструктуризацию записи в `match`
- kind: green
- depends: 246
- goal: Лоуэр даёт чёткий код отказа для `match` с паттерном деструктуризации записи
- allowed_files: `script_vm/lower_match.mlc`, `script_vm/test/test_lower_match_record_reject.mlc`
- contract: Код отказа отличим от "match без wildcard" и от "неизвестная конструкция"
- red: none
- green: Тест 246 проходит
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Тест зелёный

### Step 248 — Green: включение match-тестов в общий gate
- kind: green
- depends: 245, 247
- goal: Все match unit (232–247) регистрируются в `run_script_vm_queue200_gate.rb`
- allowed_files: `scripts/run_script_vm_queue200_gate.rb`
- contract: Список явный
- red: none
- green: none
- gate: `ruby scripts/run_script_vm_queue200_gate.rb` (последняя строка `script_vm_queue200_gate ok`)
- sabotage: Удаление одной строки регистрации проваливает gate
- done_when: Gate зелёный

---

### Step 249 — Анализ: байткодовая поддержка замыканий относительно лоуэра
- kind: analysis
- depends: 248
- goal: Сверить, какие именно opcode и структуры верификатора upvalue уже есть в байткоде (из (А)), и что из этого лоуэр пока не использует
- allowed_files: `docs/agent/notes/closure_bytecode_survey.md`
- contract: Документ перечисляет существующие opcode для MAKE_CLOSURE/захвата upvalue с указанием файла в `script_vm/`
- red: none
- green: none
- gate: `test -f docs/agent/notes/closure_bytecode_survey.md`
- sabotage: none
- done_when: Для каждого opcode указано, тестируется ли он уже unit-тестом байткодового уровня (из S200)

### Step 250 — Decision: синтаксис лямбды, принимаемый на этом срезе
- kind: decision
- depends: 249
- goal: Выбрать форму лямбды из того, что уже парсит frontend (например `|x| x + 1` или `fn(x) -> x + 1`, в зависимости от реального синтаксиса языка)
- allowed_files: `docs/agent/notes/lambda_syntax_decision.md`
- contract: Решение ссылается на конкретный узел AST, подтверждённый чтением `compiler/frontend`
- red: none
- green: none
- gate: none
- sabotage: none
- done_when: Явно указано, какой тип переменной хранит значение-лямбду (ссылка на объект кучи, как запись/строка)

### Step 251 — Red: лямбда без захвата, вызов сразу
- kind: red
- depends: 250, 249
- goal: `let f = |x: i32| -> i32 do x + 1 end` затем `f(5)` даёт `6`
- allowed_files: `script_vm/test/test_lower_lambda_basic.mlc` (новый)
- contract: none
- red: Падает — лоуэр не строит замыкание из исходника
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Красный тест воспроизводим

### Step 252 — Green: лямбда без захвата как значение
- kind: green
- depends: 251
- goal: Лоуэр эмитит MAKE_CLOSURE с пустым списком upvalue для лямбды без свободных переменных, присваивание переменной и вызов через неё работают
- allowed_files: `script_vm/lower_lambda.mlc` (новый модуль)
- contract: `lower_expression.mlc` получает одну точку вызова в `lower_lambda.mlc`
- red: none
- green: Тест 251 проходит
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: Откат `lower_lambda.mlc` к заглушке возвращает красное
- done_when: Тест зелёный, `lower_lambda.mlc` ≤ 400 строк

### Step 253 — Red: лямбда с захватом локальной переменной
- kind: red
- depends: 252
- goal: `let y = 10 let f = |x: i32| -> i32 do x + y end` затем `f(5)` даёт `15`
- allowed_files: `script_vm/test/test_lower_lambda_capture.mlc` (новый)
- contract: none
- red: Падает — захват `y` как upvalue не построен лоуэром
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Красный тест воспроизводим

### Step 254 — Green: захват одной переменной по значению
- kind: green
- depends: 253
- goal: Лоуэр анализирует тело лямбды, находит свободные переменные из внешнего scope, эмитит их как upvalue при MAKE_CLOSURE
- allowed_files: `script_vm/lower_lambda.mlc`, `script_vm/test/test_lower_lambda_capture.mlc`
- contract: Захват — по значению на момент создания замыкания (не по ссылке на регистр внешней функции) — явно зафиксировать в контракте, т.к. частая ошибка
- red: none
- green: Тест 253 проходит
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: Замена захвата по значению на захват по регистру должна провалить тест, где внешняя переменная меняется после создания лямбды
- done_when: Тест зелёный

### Step 255 — Red: захват нескольких переменных
- kind: red
- depends: 254
- goal: Лямбда захватывает две и более переменных из внешнего scope одновременно
- allowed_files: `script_vm/test/test_lower_lambda_multi_capture.mlc` (новый)
- contract: none
- red: Падает, если верификатор или лоуэр ограничены одним upvalue
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Красный тест воспроизводим на минимум 3 захваченных переменных

### Step 256 — Green: захват нескольких переменных
- kind: green
- depends: 255
- goal: Лоуэр строит список upvalue произвольной длины (в пределах лимита байткода, проверенного верификатором из (А))
- allowed_files: `script_vm/lower_lambda.mlc`, `script_vm/test/test_lower_lambda_multi_capture.mlc`
- contract: none
- red: none
- green: Тест 255 проходит
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Тест зелёный

### Step 257 — Red: лямбда как аргумент функции высшего порядка
- kind: red
- depends: 252
- goal: Top-level функция принимает параметр-функцию, вызывает его внутри: `fn apply(f: Fn(i32) -> i32, x: i32) -> i32 do return f(x) end`
- allowed_files: `script_vm/test/test_lower_lambda_as_argument.mlc` (новый)
- contract: none
- red: Падает — CALL-окно верификатора не знает, как типизировать параметр-функцию
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Красный тест воспроизводим

### Step 258 — Green: функция как параметр и вызов значения-функции
- kind: green
- depends: 257, 222
- goal: Лоуэр и верификатор поддерживают тип параметра "значение-функция", вызов через CALL_VALUE (или существующий байткодовый механизм вызова замыкания)
- allowed_files: `script_vm/lower_lambda.mlc`, `script_vm/verify_call.mlc` (точка добавления ветки), `script_vm/test/test_lower_lambda_as_argument.mlc`
- contract: Используется существующий байткодовый механизм вызова замыкания (из (А)), не вводится новый opcode
- red: none
- green: Тест 257 проходит
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: Удаление ветки верификатора должно вернуть красное на этапе верификации
- done_when: Тест зелёный, существующие тесты верификатора CALL остаются зелёными

### Step 259 — Red: top-level функция как значение (без `|...|`)
- kind: red
- depends: 258
- goal: Имя top-level функции используется как значение: `let f = my_function` затем `f(5)`
- allowed_files: `script_vm/test/test_lower_function_as_value.mlc` (новый)
- contract: none
- red: Падает — лоуэр трактует имя функции только как цель прямого вызова, не как значение
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Красный тест воспроизводим

### Step 260 — Green: top-level функция как значение
- kind: green
- depends: 259
- goal: Лоуэр умеет создавать замыкание с пустым upvalue-списком, указывающее на top-level функцию, когда имя функции используется не в позиции прямого вызова
- allowed_files: `script_vm/lower_lambda.mlc`, `script_vm/test/test_lower_function_as_value.mlc`
- contract: Прямой вызов `my_function(5)` по-прежнему компилируется в прямой CALL (не регрессирует в непрямой вызов через замыкание) — проверяется отдельной проверкой в тесте на отсутствие деградации
- red: none
- green: Тест 259 проходит
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: Удаление различия прямой/непрямой вызов проваливает performance-часть теста
- done_when: Тест зелёный

### Step 261 — Measure: стоимость непрямого вызова через значение-функцию
- kind: measure
- depends: 260
- goal: Сравнить `ns_per_call` для прямого CALL top-level функции (baseline 1242) и вызова через значение-функцию
- allowed_files: `scripts/run_script_vm_lambda_bench.rb` (новый)
- contract: none
- red: none
- green: none
- gate: `ruby scripts/run_script_vm_lambda_bench.rb`
- sabotage: none
- done_when: Оба числа напечатаны, разница задокументирована (накладные расходы на косвенность ожидаемы и не являются регрессом)

### Step 262 — Red: вложенная лямбда (лямбда возвращает лямбду)
- kind: red
- depends: 256
- goal: `let make_adder = |y: i32| -> Fn(i32) -> i32 do return |x: i32| -> i32 do x + y end end`
- allowed_files: `script_vm/test/test_lower_lambda_nested.mlc` (новый)
- contract: none
- red: Падает — вложенное замыкание, захватывающее upvalue внешней лямбды (а не внешней функции), не строится
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Красный тест воспроизводим

### Step 263 — Green: вложенные замыкания с транзитивным захватом
- kind: green
- depends: 262
- goal: Лоуэр корректно строит цепочку upvalue, когда внутренняя лямбда захватывает переменную из внешней лямбды (не из top-level функции)
- allowed_files: `script_vm/lower_lambda.mlc`, `script_vm/test/test_lower_lambda_nested.mlc`
- contract: Используется существующий верификатор источников upvalue из (А) без его изменения — проверяется на этапе шага, что изменений в `script_vm/verify*.mlc` не потребовалось (если потребовались — отдельный red/green)
- red: none
- green: Тест 262 проходит
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: Удаление транзитивного построения upvalue возвращает красное
- done_when: Тест зелёный

### Step 264 — Red: отказ на верификаторе, если вложенность нарушает окно upvalue
- kind: red
- depends: 263
- goal: Подтвердить, что если существующий верификатор (А) ограничивает глубину/источники upvalue, лоуэр получает диагностируемый отказ, а не падение VM
- allowed_files: `script_vm/test/test_lower_lambda_verifier_reject.mlc` (новый)
- contract: Тест конструирует заведомо некорректный случай (например, upvalue на несуществующий фрейм)
- red: Падает, если текущая реакция — не именованный отказ, а падение
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Красный тест воспроизводим

### Step 265 — Green: диагностируемый отказ лоуэра на нарушении окна upvalue
- kind: green
- depends: 264
- goal: Лоуэр перехватывает ошибку верификатора и транслирует её в именованный код отказа компиляции, не допуская падения VM
- allowed_files: `script_vm/lower_lambda.mlc`, `script_vm/test/test_lower_lambda_verifier_reject.mlc`
- contract: none
- red: none
- green: Тест 264 проходит
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Тест зелёный

### Step 266 — Red: лямбда, захватывающая запись (ссылочный тип)
- kind: red
- depends: 256
- goal: Лямбда захватывает переменную типа "запись", изменения через лямбду видны снаружи (ссылочная семантика записи сохраняется и через замыкание)
- allowed_files: `script_vm/test/test_lower_lambda_capture_record.mlc` (новый)
- contract: Тест проверяет именно сохранение ссылочной семантики, не копирование записи при захвате
- red: Падает, если захват копирует запись вместо ссылки
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Красный тест воспроизводим

### Step 267 — Green: захват записи с сохранением ссылочной семантики
- kind: green
- depends: 266
- goal: Upvalue для записи хранит ссылку на объект кучи, не копию полей
- allowed_files: `script_vm/lower_lambda.mlc`, `script_vm/test/test_lower_lambda_capture_record.mlc`
- contract: Pin `record_alias_shares_object` соблюдён и для захвата через замыкание — явно проверяется в тесте
- red: none
- green: Тест 266 проходит
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: Замена ссылки на копию при захвате должна провалить тест
- done_when: Тест зелёный

### Step 268 — Red: лямбда внутри `while` с `break`/`continue` снаружи неё
- kind: red
- depends: 254
- goal: Тело `while` содержит вызов лямбды, объявленной вне цикла; `break` внутри `while` (не внутри тела лямбды) работает как обычно — регрессионная проверка на то, что лямбды не путают верификатор циклов
- allowed_files: `script_vm/test/test_lower_lambda_near_loop.mlc` (новый)
- contract: none
- red: Падает, если добавление лямбд случайно сломало существующую обработку `break`/`continue`
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Красный тест либо подтверждает регресс, либо (если уже работает) шаг помечается как green сразу без изменений кода — зафиксировать явно в карточке результат

### Step 269 — Green: подтверждение отсутствия регресса `break`/`continue` рядом с лямбдами
- kind: green
- depends: 268
- goal: Если 268 выявил регресс — исправить; если не выявил — зарегистрировать тест как подтверждающий
- allowed_files: `script_vm/lower_lambda.mlc` (если нужна правка), `script_vm/test/test_lower_lambda_near_loop.mlc`
- contract: none
- red: none
- green: Тест 268 проходит
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Тест зелёный

### Step 270 — Red: лямбда, захватывающая `break`/`continue`-переменную по ошибке (запрещённый случай)
- kind: red
- depends: 269
- goal: Подтвердить, что лямбда не может быть объявлена так, чтобы содержать `break`/`continue`, адресующий цикл снаружи самой лямбды (лямбда — новая граница для break/continue, аналогично функции)
- allowed_files: `script_vm/test/test_lower_lambda_break_boundary_reject.mlc` (новый)
- contract: Ожидается код `break_outside_loop`/`continue_outside_loop`, так как лямбда — новая граница
- red: Падает, если сейчас `break` внутри лямбды ошибочно привязывается к внешнему циклу
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Красный тест воспроизводим

### Step 271 — Green: лямбда как граница для `break`/`continue`
- kind: green
- depends: 270
- goal: Лоуэр сбрасывает контекст цикла при входе в тело лямбды — `break`/`continue` внутри неё без собственного цикла дают именованный отказ
- allowed_files: `script_vm/lower_lambda.mlc`, `script_vm/test/test_lower_lambda_break_boundary_reject.mlc`
- contract: none
- red: none
- green: Тест 270 проходит
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: Удаление сброса контекста должно вернуть красное
- done_when: Тест зелёный, существующие `break_outside_loop`/`continue_outside_loop` тесты из (А) остаются зелёными

### Step 272 — Measure: стоимость создания замыкания с захватом относительно вызова top-level функции
- kind: measure
- depends: 267
- goal: Замерить стоимость MAKE_CLOSURE с 3 upvalue против прямого CALL (baseline 1242)
- allowed_files: `scripts/run_script_vm_lambda_bench.rb`
- contract: none
- red: none
- green: none
- gate: `ruby scripts/run_script_vm_lambda_bench.rb`
- sabotage: none
- done_when: Число напечатано, зафиксировано как новый baseline лямбда-слоя

### Step 273 — Critic: lambda-срез
- kind: critic
- depends: 252, 254, 256, 258, 260, 263, 265, 267, 271
- goal: Проверить согласованность всего lambda-среза: лямбда внутри `match`-ветки, лямбда как поле записи, повторный вызов одного и того же значения-лямбды несколько раз подряд
- allowed_files: `script_vm/test/test_lower_lambda_critic.mlc` (новый), файлы lambda-модуля для исправлений
- contract: Критик правит найденные дефекты только в `lower_lambda.mlc`
- red: none
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: Один найденный дефект фиксируется regression-тестом с подтверждённым красным до правки
- done_when: Все lambda-тесты зелёные одним прогоном

### Step 274 — Green: включение lambda-тестов в общий gate
- kind: green
- depends: 273
- goal: Все lambda unit (251–273) регистрируются в `run_script_vm_queue200_gate.rb`
- allowed_files: `scripts/run_script_vm_queue200_gate.rb`
- contract: Список явный
- red: none
- green: none
- gate: `ruby scripts/run_script_vm_queue200_gate.rb` (последняя строка `script_vm_queue200_gate ok`)
- sabotage: Удаление одной строки регистрации проваливает gate
- done_when: Gate зелёный

---

### Step 275 — Анализ: синтаксис методов (`impl`) во frontend
- kind: analysis
- depends: 274
- goal: Установить, какой синтаксис определения методов (`impl Type do fn method(self, ...) ... end end` или аналог) уже парсит `compiler/frontend`, без generics
- allowed_files: `docs/agent/notes/methods_frontend_survey.md`
- contract: Документ указывает конкретный узел AST для блока `impl` и для метода с параметром `self`
- red: none
- green: none
- gate: `test -f docs/agent/notes/methods_frontend_survey.md`
- sabotage: none
- done_when: Явно зафиксировано отсутствие generics-параметров в рассматриваемом срезе

### Step 276 — Decision: диспетчеризация методов без наследования
- kind: decision
- depends: 275
- goal: Решить, как лоуэр резолвит вызов `x.method(...)` в статический CALL top-level функции (без таблицы виртуальных методов, т.к. наследования нет)
- allowed_files: `docs/agent/notes/method_dispatch_decision.md`
- contract: Решение явно говорит: это статическая диспетчеризация по типу записи, известному на этапе лоуэринга, не динамическая
- red: none
- green: none
- gate: none
- sabotage: none
- done_when: Решение зафиксировано со ссылкой на отсутствие наследования записей в языке

### Step 277 — Red: метод без параметров, кроме `self`
- kind: red
- depends: 276
- goal: `impl Point do fn get_x(self) -> i32 do return self.x end end`, вызов `p.get_x()`
- allowed_files: `script_vm/test/test_lower_method_basic.mlc` (новый)
- contract: none
- red: Падает — методы не распознаются лоуэром
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Красный тест воспроизводим

### Step 278 — Green: метод без параметров как top-level функция
- kind: green
- depends: 277
- goal: Лоуэр компилирует метод в top-level функцию с неявным первым параметром `self`, вызов `p.get_x()` компилируется в прямой CALL с `p` как первым аргументом
- allowed_files: `script_vm/lower_method.mlc` (новый модуль)
- contract: `lower_expression.mlc` и `lower_records.mlc` получают не более одной точки вызова каждый в `lower_method.mlc`
- red: none
- green: Тест 277 проходит
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: Откат `lower_method.mlc` возвращает красное
- done_when: Тест зелёный, `lower_method.mlc` ≤ 400 строк

### Step 279 — Red: метод с дополнительными параметрами
- kind: red
- depends: 278
- goal: `fn add(self, n: i32) -> i32 do return self.x + n end`, вызов `p.add(5)`
- allowed_files: `script_vm/test/test_lower_method_with_args.mlc` (новый)
- contract: none
- red: Падает — дополнительные параметры метода не передаются
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Красный тест воспроизводим

### Step 280 — Green: метод с параметрами
- kind: green
- depends: 279
- goal: Лоуэр передаёт `self` плюс остальные аргументы в правильном порядке в CALL-окно
- allowed_files: `script_vm/lower_method.mlc`, `script_vm/test/test_lower_method_with_args.mlc`
- contract: none
- red: none
- green: Тест 279 проходит
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Тест зелёный

### Step 281 — Red: метод с `self: ref mut` изменяет поле записи
- kind: red
- depends: 280
- goal: `fn set_x(self: ref mut, v: i32) do self.x = v end`, вызов `p.set_x(10)` меняет `p.x`, видимое снаружи (ссылочная семантика)
- allowed_files: `script_vm/test/test_lower_method_mut_self.mlc` (новый)
- contract: Тест проверяет видимость изменения снаружи вызова, не только внутри метода
- red: Падает — `ref mut self` не поддержан
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Красный тест воспроизводим

### Step 282 — Green: метод с изменяемым `self`
- kind: green
- depends: 281
- goal: Лоуэр компилирует `self: ref mut` аналогично существующему `ref mut` параметру top-level функции (переиспользует существующую логику, не дублирует)
- allowed_files: `script_vm/lower_method.mlc`, `script_vm/test/test_lower_method_mut_self.mlc`
- contract: Запрет "целиком присвоить запись параметру `ref mut` нельзя" (E031) действует и для `self` — проверяется отдельным sub-тестом
- red: none
- green: Тест 281 проходит
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: Удаление переиспользования существующей E031-проверки должно провалить sub-тест на целиковое присваивание `self`
- done_when: Тест зелёный

### Step 283 — Red: два разных типа записи с одноимённым методом
- kind: red
- depends: 278
- goal: `impl A do fn describe(self) -> i32 ... end`, `impl B do fn describe(self) -> i32 ... end` — вызов `a.describe()` и `b.describe()` резолвятся в разные top-level функции без конфликта
- allowed_files: `script_vm/test/test_lower_method_name_disambiguation.mlc` (новый)
- contract: none
- red: Падает, если лоуэр резолвит метод только по имени без учёта типа записи
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Красный тест воспроизводим

### Step 284 — Green: резолвинг метода по типу записи и имени
- kind: green
- depends: 283
- goal: Лоуэр строит внутреннюю таблицу (тип записи, имя метода) → top-level функция, используется при резолвинге `x.method(...)`
- allowed_files: `script_vm/lower_method.mlc`, `script_vm/test/test_lower_method_name_disambiguation.mlc`
- contract: none
- red: none
- green: Тест 283 проходит
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: Удаление учёта типа записи должно вернуть красное
- done_when: Тест зелёный

### Step 285 — Red: вызов несуществующего метода — диагностика
- kind: red
- depends: 284
- goal: `p.unknown_method()`, где `unknown_method` не определён для типа `p` — именованный диагностируемый отказ с указанием строки и столбца, не падение
- allowed_files: `script_vm/test/test_lower_method_unknown_reject.mlc` (новый)
- contract: Тест проверяет формат диагностики, совместимый с общим форматом REPL-диагностик (`code=... line=... column=...`)
- red: Падает, если сейчас это падение VM или неясная ошибка
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Красный тест воспроизводим

### Step 286 — Green: диагностика неизвестного метода
- kind: green
- depends: 285
- goal: Лоуэр даёт именованный код отказа с позицией для неизвестного метода
- allowed_files: `script_vm/lower_method.mlc`, `script_vm/test/test_lower_method_unknown_reject.mlc`
- contract: none
- red: none
- green: Тест 285 проходит
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Тест зелёный

### Step 287 — Решение/Red: имя поля в диагностике рантайма вместо индекса
- kind: decision
- depends: 284
- goal: Решить, как лоуэр сохраняет соответствие (тип записи, индекс поля) → имя поля для использования в диагностических сообщениях рантайма (не меняя сам факт, что хранение полей — по индексу `u8`)
- allowed_files: `docs/agent/notes/field_name_diagnostics_decision.md`
- contract: Хранение поля по индексу `u8` в самом байткоде не меняется — имя используется только в отдельной диагностической таблице, формируемой лоуэром
- red: none
- green: none
- gate: none
- sabotage: none
- done_when: Решение явно подтверждает отсутствие изменений в существующем формате записи в куче

### Step 288 — Red: сообщение об ошибке доступа к несуществующему полю по имени
- kind: red
- depends: 287
- goal: `p.unknown_field` даёт диагностику с именем `unknown_field`, не только с индексом
- allowed_files: `script_vm/test/test_lower_field_name_diagnostic.mlc` (новый)
- contract: none
- red: Падает, если текущая диагностика оперирует только индексом или вообще отсутствует как отдельная проверка
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: none
- done_when: Красный тест воспроизводим

### Step 289 — Green: диагностическая таблица имён полей
- kind: green
- depends: 288
- goal: Лоуэр формирует таблицу (тип записи, индекс) → имя поля, используется при генерации диагностик доступа к полю
- allowed_files: `script_vm/lower_records.mlc` (точка вызова), `script_vm/lower_field_diagnostics.mlc` (новый), `script_vm/test/test_lower_field_name_diagnostic.mlc`
- contract: `lower_records.mlc` не растёт более чем на 10 строк
- red: none
- green: Тест 288 проходит
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: Удаление таблицы должно вернуть красное
- done_when: Тест зелёный

### Step 290 — Measure: стоимость вызова метода относительно top-level функции
- kind: measure
- depends: 282, 284
- goal: Замерить `ns_per_call` вызова метода против baseline прямого вызова top-level функции (1242)
- allowed_files: `scripts/run_script_vm_method_bench.rb` (новый)
- contract: none
- red: none
- green: none
- gate: `ruby scripts/run_script_vm_method_bench.rb`
- sabotage: none
- done_when: Число напечатано, разница с baseline объяснена (ожидаемо близко к прямому CALL, так как диспетчеризация статическая)

---

### Step 291 — Critic: методы и имена полей — согласованность
- kind: critic
- depends: 278, 280, 282, 284, 286, 289
- goal: Проверить методы совместно с lambda-срезом (метод, возвращающий лямбду, захватывающую `self`) и с match-срезом (`match` внутри тела метода)
- allowed_files: `script_vm/test/test_lower_method_critic.mlc` (новый), файлы method-модуля для исправлений
- contract: Критик правит найденные дефекты только в `lower_method.mlc`/`lower_field_diagnostics.mlc`
- red: none
- green: none
- gate: `ruby scripts/run_script_vm_gate.rb`
- sabotage: Один найденный дефект фиксируется regression-тестом с подтверждённым красным до правки
- done_when: Все method-тесты и их пересечения с lambda/match зелёные одним прогоном

### Step 292 — Green: включение method-тестов в общий gate
- kind: green
- depends: 291
- goal: Все method unit (277–290) регистрируются в `run_script_vm_queue200_gate.rb`
- allowed_files: `scripts/run_script_vm_queue200_gate.rb`
- contract: Список явный
- red: none
- green: none
- gate: `ruby scripts/run_script_vm_queue200_gate.rb` (последняя строка `script_vm_queue200_gate ok`)
- sabotage: Удаление одной строки регистрации проваливает gate
- done_when: Gate зелёный

### Step 293 — Sabotage-прогон: порча float-модуля
- kind: critic
- depends: 228
- goal: Подтвердить, что порча `script_vm/lower_float.mlc` (возврат к заглушке) ловится общим gate, а не только узким прогоном
- allowed_files: `script_vm/lower/lower_float.mlc`, `STATE`
- contract: Порча вносится, фиксируется провал `run_script_vm_queue200_gate.rb`, затем откатывается
- red: none
- green: none
- gate: `ruby scripts/run_script_vm_queue200_gate.rb` (наблюдаем провал при порче, затем `script_vm_queue200_gate ok` после отката)
- sabotage: Это и есть содержание шага — явная порча с фиксацией провала
- done_when: Оба состояния (провал и восстановление) задокументированы в выводе шага

### Step 294 — Sabotage-прогон: порча match-модуля
- kind: critic
- depends: 248
- goal: Аналогично 293 для `script_vm/lower/lower_match.mlc`
- allowed_files: `script_vm/lower/lower_match.mlc`, `STATE`
- contract: none
- red: none
- green: none
- gate: `ruby scripts/run_script_vm_queue200_gate.rb`
- sabotage: Явная порча wildcard-проверки с фиксацией провала
- done_when: Провал и восстановление задокументированы

### Step 295 — Sabotage-прогон: порча lambda-модуля
- kind: critic
- depends: 274
- goal: Аналогично для `script_vm/lower/lower_lambda.mlc` — порча границы `break`/`continue` (шаг 271)
- allowed_files: `script_vm/lower/lower_lambda.mlc`, `STATE`
- contract: none
- red: none
- green: none
- gate: `ruby scripts/run_script_vm_queue200_gate.rb`
- sabotage: Явная порча с фиксацией провала
- done_when: Провал и восстановление задокументированы

### Step 296 — Sabotage-прогон: порча method-модуля
- kind: critic
- depends: 292
- goal: Аналогично для `script_vm/lower/lower_method.mlc` — порча резолвинга по типу записи (шаг 284)
- allowed_files: `script_vm/lower/lower_method.mlc`, `STATE`
- contract: none
- red: none
- green: none
- gate: `ruby scripts/run_script_vm_queue200_gate.rb`
- sabotage: Явная порча с фиксацией провала
- done_when: Провал и восстановление задокументированы

### Step 297 — Measure: сводный замер против baseline S200 и queue200
- kind: measure
- depends: 228, 244, 261, 272, 290
- goal: Единым прогоном собрать все числа (float, match, lambda, method) рядом с историческими B1=35/176, B3=1242/8023, repl_language_us=1152
- allowed_files: `scripts/run_script_vm_s300_bench_summary.rb` (новый, вызывает уже существующие bench-скрипты)
- contract: Новый скрипт не дублирует логику измерения, только агрегирует вызовы существующих
- red: none
- green: none
- gate: `ruby scripts/run_script_vm_s300_bench_summary.rb` (наблюдаем печать всех чисел одним блоком)
- sabotage: none
- done_when: Все числа из 223, 228, 244, 261, 272, 290 присутствуют в едином выводе

### Step 298 — Green: единый gate 201–297 одной командой
- kind: green
- depends: 225, 248, 274, 292
- goal: Подтвердить, что `ruby scripts/run_script_vm_queue200_gate.rb` покрывает весь срез (Б) этой сотни одним вызовом без дополнительных узких прогонов
- allowed_files: `scripts/run_script_vm_queue200_gate.rb` (если нужны финальные правки регистрации)
- contract: Полный список unit в файле актуален
- red: none
- green: none
- gate: `ruby scripts/run_script_vm_queue200_gate.rb` (последняя строка `script_vm_queue200_gate ok`)
- sabotage: none
- done_when: Число зарегистрированных unit равно 51 (из S200) плюс все unit 207–290, подсчёт явный в выводе шага

### Step 299 — Документация: обновление §12 по факту сделанного в 201–300
- kind: refactor
- depends: 298
- goal: Обновить `docs/MLC_SCRIPT_VM.md` только в части перечисления того, что из пункта 1 §12 реализовано (float, match, лямбды, простые методы, имена полей в диагностике), без изменения формулировок пунктов 2–5
- allowed_files: `docs/MLC_SCRIPT_VM.md`
- contract: Правка — только дополнение статуса, не переписывание структуры §12/§14
- red: none
- green: none
- gate: `ruby scripts/run_script_vm_queue200_gate.rb`
- sabotage: none
- done_when: Диск-документ явно отличает сделанное (float/match/лямбды/методы) от оставшегося (generics, embedding, fibers, дамп, инкрементальный GC, лимит памяти)

### Step 300 — Critic: итоговый список остатка до конца (Б) и до (В)
- kind: critic
- depends: 299, 293, 294, 295, 296, 297
- goal: Зафиксировать явный список того, что осталось
- allowed_files: `docs/agent/notes/s300_remainder.md`
- contract: Документ перечисляет отдельно остаток (Б): generics в методах, embedding (живая память REPL между вводами, fibers, дамп VM), память производства (инкрементальный GC, лимит памяти элементов) — и отдельно весь (В): quickening, shapes, inline caches; явно отмечает, что baseline JIT и tier-2 не входят ни в один из списков этой очереди
- red: none
- green: none
- gate: `ruby scripts/run_script_vm_queue200_gate.rb` (последняя строка `script_vm_queue200_gate ok`)
- sabotage: none
- done_when: Документ существует, gate зелёный, список остатка (Б) и список (В) разделены заголовками

---

## Decision log

