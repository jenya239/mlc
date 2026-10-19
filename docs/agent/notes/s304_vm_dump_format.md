# Шаг 304 — формат дампа VM

Текст для отладки. Не бинарный снимок и не Checkpoint SVGUI-27. Шаг 335 добавляет identity `ScriptInstance`, когда этот тип появится. Шаг 337 печатает этот текст из `script_vm/vm_dump.mlc` функцией `dump_instance`. Порядок строк и полей фиксирован.

Поля берутся из существующих типов. Отдельного счётчика поколения сборки в `Heap` и `VmState` нет. В дампе на его месте стоит бит метки `ObjectHeader.gc_flags & 1` (`slot_is_marked` в `script_vm/heap_collect.mlc`) и счётчики кучи ниже. Новое поле поколения не вводится.

## module

Источника имени модуля нет. `DecodedModule` — это `exports` и `protos`.

- строка `export <ModuleExport.name> proto <ModuleExport.proto_index>` для каждого экспорта по индексу массива;
- строка `proto <index> registers <FunctionProto.register_count> words <words.length> constants <constants.length>` для каждого прототипа. Слова байткода и константы в дамп не входят.

## registers

Источник — `VmState`. Кадры от 0 до `frame_depth - 1`:

`frame <index> proto <frame_proto> pc <frame_program_counter> base <frame_base> return <frame_return_register>`

Регистры текущего кадра — окно `stack` от `frame_base` длиной `FunctionProto.register_count` этого `frame_proto`. Строка `register <index> <kind> <payload>`. `kind` — имя `ValueRep`: `nil`, `bool`, `int32`, `float64`, `heap`. Для `heap` payload — `object_id`, не `Value.raw`.

## heap

Одна строка на слот `Heap.slots` по возрастанию индекса, включая `live == false`:

`object <index> type <name> live <0|1> mark <0|1> flags <object_flags> shape <shape_or_meta> elements <element_start> <element_count> class <size_class> child <kind> <payload>`

`type` по `ObjectHeader.type_id`: 1 `cell`, 2 `array`, 3 `record`, 4 `closure`, 5 `string` (`heap_type_*` в `script_vm/heap_types.mlc`). Иное число печатается как десятичное. `mark` — младший бит `gc_flags`. `child` декодируется так же, как регистр. Содержимое `element_arena` по ячейкам не печатается.

## gc

```
live_count <Heap.live_count>
write_barrier_hits <Heap.write_barrier_hits>
gc_threshold <Heap.gc_threshold>
free_object_count <Heap.free_object_count>
arena_cells <element_arena.length>
```

## Не Checkpoint

Checkpoint SVGUI-27 (§11.3) хранит версионированное логическое состояние, курсор почты и незавершённые эффекты. Он не хранит сырые идентификаторы кучи и кадры. Этот дамп печатает и идентификаторы объектов, и кадры `VmState`. Обратного чтения в VM нет.
