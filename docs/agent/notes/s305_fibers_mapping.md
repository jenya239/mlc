# Шаг 305 — fibers и нарезка исполнения

Fibers пункта (Б) — это `Execution` из §6.1 документа реализации GUI. Отдельного native-примитива, типа и opcode fiber нет. `script_vm/tests/closures_fibers_unit.mlc` проверяет кадры вызова и замыкания, не этот механизм.

На MVP один actor — один `ScriptInstance` — одно активное `Execution`. Несколько ожидающих execution в одном actor — после корней сборки мусора, не в первой нарезке.

Сохранённый стек fiber — уже существующий `VmState`: `stack`, `frame_proto`, `frame_program_counter`, `frame_base`, `frame_closure`, `frame_return_register`, `frame_depth`. Перед возвратом из `run_slice` актуальный кадр записывается в этот `VmState` (`materialize_current_frame` в §6.2). Локальная копия регистров исполнителя стеком fiber не является.

| Fiber | Execution |
|---|---|
| fiber.yield | `status` становится `Yielded`. `run_slice` возвращает `PollResult.Yielded`, когда исчерпан quantum. Кадр уже материализован. |
| fiber.resume | Следующий `run_slice` того же `Execution`: `restore_current_frame_cache`, затем инструкции с сохранённого `frame_program_counter`. Новый стек не создаётся. |
| fiber dump | Текст шага 304 (`docs/agent/notes/s304_vm_dump_format.md`): кадры `VmState`, регистры и таблица объектов кучи. Это не Checkpoint SVGUI-27. |

`Waiting` — не yield. Его ставит host-запрос: `set_waiting(request, destination_register, resume_pc)`, результат `PollResult.Waiting`. Возобновление после ответа хоста тоже идёт через `run_slice`, но статус до этого `Waiting`, не `Yielded`.

Шаги 339–360 берут эту таблицу. Новый примитив fiber они не вводят.
