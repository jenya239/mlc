# Шаг 338 — критика фазы 2

Нарушения типа `incarnation` нет. Карточка не возвращается.

`ScriptInstance.identity` — `i64`. `ScriptInstance.incarnation` — `i32` (`script_vm/instance.mlc`). `vm_dump.mlc` своих полей не объявляет: `dump_instance` печатает `instance.identity` и `instance.incarnation`, `dump_root_lines` печатает те же значения как `owner` и `generation`. Отдельного типа поколения в дампе нет, сузить `i32` до другого типа негде.

`value_handle_for` копирует те же поля: `owner_identity: instance.identity` (`i64`), `generation: instance.incarnation` (`i32`). `handle_is_current` сравнивает их с `instance.identity` и `instance.incarnation`. `ExportHandle` несёт только `owner_identity: i64`, без `generation`.

`std::bad_alloc` на границе ABI возвращает `MLC_HEAP_LIMIT`. Прочий `std::exception` — `MLC_UNSUPPORTED`. Segfault не перехватывается. Чужой поток на debug-сборке остаётся `assert` в `require_owner_thread`, не `MlcResult`. Это не расхождение `identity`/`incarnation`.

Фаза 2 (SVGUI-04/05/06, embedding, дамп VM) по этой сверке закрыта.
