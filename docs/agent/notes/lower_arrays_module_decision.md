# Модуль массивов

Вопрос шага 219: существует ли отдельный модуль `script_vm/lower_arrays.mlc`, и нужно ли выносить логику массивов из `lower_expression.mlc`.

## Ответ

Вынос не нужен. Отдельный модуль уже есть. Создавать `script_vm/lower_arrays.mlc` и переносить код из `lower_expression.mlc` не следует.

## Что есть на диске

Файла `script_vm/lower_arrays.mlc` нет. Файла `script_vm/lower/lower_arrays.mlc` нет.

Модуль массивов — `script_vm/lower/lower_array.mlc`, 400 строк. Имя в единственном числе, не `lower_arrays`. Карточка 218 писала множественное число; шаг 218 вызвал этот файл как точку вызова.

Float-элементы живут рядом, не внутри `lower_expression.mlc`: `script_vm/lower/lower_array_float.mlc`, 217 строк. Его импортирует только `lower_array.mlc`.

## Где тела, а где вызовы

Тела литерала, `len`, чтения индекса и записи элемента — в `lower_array.mlc`: `place_array_literal`, `lower_length_or_text`, `finish_array_index`, `emit_array_element_store`. Ветка float — `place_other_array_literal` и `lower_float_array_length_or_text` в `lower_array_float.mlc`.

`script_vm/lower/lower_expression.mlc` (394 строки) массивы не реализует. Импорт из `./lower_array` и три вызова: `lower_array_named` (строка 284), `place_integer_array` (строка 326), `finish_array_index` (строка 329).

`script_vm/lower/lower_statement.mlc` (393 строки) тоже только вызывает: `place_integer_array`, `array_store_register`, `emit_array_element_store`.

## Шаг 220

Рефакторинг выноса не выполнять. `lower_expression.mlc` не уменьшать: вычитать из него нечего, уменьшение строк означало бы снос вызовов. Новый `lower_arrays.mlc` не создавать: это был бы второй модуль рядом с `lower_array.mlc`. Поведение array-тестов не менять. Gate шага 220 для этого решения не является проверкой переноса.
