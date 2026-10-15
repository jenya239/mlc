# Замыкания в байткоде и лоуэре

Опкоды 23–25 уже есть. Лоуэр из исходника их для захвата не эмитирует.

Слово инструкции — одно `i32`: биты 0–7 opcode, 8–15 A, 16–23 B, 24–31 C (`instruction_encode` в `script_vm/bytecode.mlc`). Коды: `script_vm/bytecode.mlc` (`opcode_make_closure` 23, `opcode_get_upval` 24, `opcode_set_upval` 25). Имена в дампе: `MAKE_CLOSURE`, `GET_UPVAL`, `SET_UPVAL`.

Объект кучи — `heap_type_closure` = 4 (`script_vm/heap_types.mlc`). `proto_index` лежит в заголовке (`heap_in_place_closure_proto_index` в `script_vm/heap_access.mlc`). Upvalue — элементы того же объекта (`heap_in_place_alloc_closure` в `script_vm/heap_alloc.mlc`).

Слово источника upvalue (`instruction_upvalue_source_word` в `script_vm/bytecode.mlc`): биты 0–7 индекс, биты 8–15 признак локала. Ненулевой признак — регистр текущего кадра. Нулевой — слот upvalue родительского замыкания.

## Opcode

| Имя | Код | A | B | C | Хвост | Файл кода | Исполнитель | Верификатор | Лоуэр | Unit байткода |
|---|---|---|---|---|---|---|---|---|---|---|
| MAKE_CLOSURE | 23 | регистр результата | `proto_index` | число источников | C слов источников сразу после primary | `script_vm/bytecode.mlc` | `executor_exec_make_closure` в `script_vm/executor_calls.mlc`, вызов из `script_vm/executor.mlc` | `script_vm/verifier.mlc`: A в диапазоне регистров; локальный источник — индекс меньше числа регистров; родительский индекс не сверяется с числом upvalue родителя; span = 1 + C | `script_vm/lower/lower_call.mlc` `emit_call_closure`: только пустой список источников | да: `script_vm/tests/char_frames_unit.mlc` (пустой список, локальный источник, источник родителя); `script_vm/tests/verifier_hardening_unit.mlc` (локальный индекс внутри и вне диапазона, родительский индекс не отвергается) |
| GET_UPVAL | 24 | регистр результата | индекс upvalue | 0 | нет | `script_vm/bytecode.mlc` | `executor_exec_get_upval` в `script_vm/executor_calls.mlc`, вызов из `script_vm/executor.mlc` | `script_vm/verifier.mlc`: A в диапазоне регистров; индекс upvalue верификатор не ограничивает | не эмитирует | да: `script_vm/tests/char_frames_unit.mlc` (`capture_local_prototypes`, `parent_prototypes`, `outside_upvalue_prototypes`, `bounds_upvalue_prototypes`) |
| SET_UPVAL | 25 | индекс upvalue | регистр источника | 0 | нет | `script_vm/bytecode.mlc` | `executor_exec_set_upval` в `script_vm/executor_calls.mlc`, вызов из `script_vm/executor.mlc` | `script_vm/verifier.mlc`: B в диапазоне регистров; индекс upvalue верификатор не ограничивает | не эмитирует | да: `script_vm/tests/char_frames_unit.mlc` (`set_upvalue_prototypes`: SET_UPVAL затем GET_UPVAL того же слота) |

`char_frames` зарегистрирован в `scripts/run_script_vm_gate.rb`. Это unit уровня байткода из очереди до S200: прототипы собираются вызовами `instruction_words_*`, не через лоуэр исходника.

Лоуэр прямого вызова кладёт `MAKE_CLOSURE` с нулём источников и сразу `CALL`. Захват локала, чтение и запись upvalue из текста MLC не строятся.
