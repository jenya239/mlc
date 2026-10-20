# Шаг 335 — дамп ScriptInstance

Текст для отладки. Продолжает `docs/agent/notes/s304_vm_dump_format.md`. Не бинарный снимок и не Checkpoint SVGUI-27. Шаг 336 вызывает функцию, которой ещё нет. Шаг 337 печатает этот текст из `script_vm/vm_dump.mlc` функцией `dump_instance(instance: ScriptInstance) -> string`. Порядок секций и полей фиксирован. Новых полей у `ScriptInstance`, `LoadedModule` и `Heap` дамп не вводит.

`ScriptInstance` не хранит `VmState`. Кадры и регистры из шага 304 в `dump_instance` не печатаются.

## instance

Источник — `ScriptInstance` (`script_vm/instance.mlc`).

`instance identity <identity> incarnation <incarnation>`

`identity` — `i64`, десятичная запись. `incarnation` — `i32`. У `script_instance_new` это `1` и `0`.

## module

У `LoadedModule` нет имени и отдельного идентификатора. Identity модуля — индекс в `ScriptInstance.loaded_modules`, с нуля, по возрастанию.

`module <index> bytes <bytes.length>`

Дальше для этого модуля строки шага 304, в том же порядке:

- `export <ModuleExport.name> proto <ModuleExport.proto_index>`
- `proto <index> registers <FunctionProto.register_count> words <words.length> constants <constants.length>`

Байты `LoadedModule.bytes`, слова байткода и константы в дамп не входят.

## roots

Корни экземпляра — `ScriptInstance.rooted_values`. Это `[Value]`, не массив `ValueHandle`. Хранимая ручка собирается как `value_handle_for`: `owner_identity` равен `instance.identity`, `generation` равен `instance.incarnation`, `rooted` равен 1, если значение — `ValueHeapRef`.

`root <index> <kind> <payload> owner <identity> generation <incarnation> rooted <0|1>`

`kind` и `payload` — как регистр в шаге 304: `nil`, `bool`, `int32`, `float64`, `heap`. Для `heap` payload — `object_id`, не `Value.raw`.

Отдельная таблица GC-корней — `Heap.roots` той же кучи. Это тоже `[Value]`, не `ValueHandle`.

`heap_root <index> <kind> <payload>`

## heap

Таблица объектов — все слоты `instance.heap.slots` по возрастанию индекса, включая `live == false`. Строка `object` и правила `type` / `mark` / `child` — без изменений из шага 304 (`ObjectHeader.type_id`, `gc_flags & 1`, `slot_is_marked`). Ячейки `element_arena` по одной не печатаются.

## gc

Счётчики `instance.heap`, те же пять строк, что в шаге 304:

```
live_count <Heap.live_count>
write_barrier_hits <Heap.write_barrier_hits>
gc_threshold <Heap.gc_threshold>
free_object_count <Heap.free_object_count>
arena_cells <element_arena.length>
```

## Не Checkpoint

Checkpoint SVGUI-27 хранит версионированное логическое состояние, курсор почты и незавершённые эффекты. Этот дамп печатает `identity` / `incarnation`, индекс загруженного модуля, ручки `rooted_values` и сырые слоты `Heap`. Обратного чтения в VM нет. Перезагрузка и подмена ручек сюда не входят.
