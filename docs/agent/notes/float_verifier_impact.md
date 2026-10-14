# Float и верификатор

Новой ветки по тегу float нет: в верификаторе нет места, которое матчит тег значения.

## Набор файлов

`Dir.glob("script_vm/verify*.mlc")` пуст. Имя `verifier.mlc` не начинается с `verify` (`verifier` — `v e r i f i e r`). Единственный исходник верификатора — `script_vm/verifier.mlc`. Поиск тега значения сделан по нему:

`rg -n 'decode_kind|ValueInt|ValueBool|ValueNil|ValueFloat|ValueHeap|script_vm_decode|value_is_|k_tag_|kind ==' script_vm/verifier.mlc`

Совпадений нет (exit 1). `script_vm/value.mlc` верификатор не импортирует.

## Места, которые матчат тег

Список пуст. Ни одно место не требует новой ветки для float.

Шаг 202 не добавляет тег: float остаётся видом 0, `ValueFloat64`. `verify_function` этот вид не читает, поэтому повторное использование вида 0 верификатор не меняет.

## Окно CALL

`registers_ok` в `script_vm/verifier.mlc`, ветка `instruction_opcode_call`: регистр A в диапазоне, регистр B в диапазоне, и `decoded.b + decoded.c < register_count`. Это граница окна аргументов по индексам регистров. Тег значения аргумента или вызываемого регистра не читается. Ветки для float не нужно.

## Сравнения i32

`registers_ok`, ветки `eq`, `ne`, `lt`, `le`, `gt`, `ge`: у каждой три проверки `register_in_range` для A, B и C. `is_known_opcode` сравнивает номер opcode с `instruction_opcode_eq` и остальными пятью, не тег значения. Ветки для float не нужно.
