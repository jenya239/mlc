# Стратегия лоуэра `match`

Новый opcode не вводится. `match` среза шага 230 снижается в те же сравнения и переходы, что и `if`.

## Что эмитится

Коды из `script_vm/bytecode.mlc`: LOAD_CONST 1, MOVE 2, JUMP 8, JUMP_IF_FALSE 9, EQ 10.

Форма та же, что `lower_if_branches` в `script_vm/lower/lower_expression.mlc`: `proto_builder_emit_jump_if_false`, тело, `proto_builder_emit_jump`, патч обоих мест (`script_vm/lower/lower_builder.mlc`). Сравнение `==` уже даёт EQ через `comparison_opcode` в `script_vm/lower/lower_control.mlc`.

| Паттерн рукава | Слова |
|---|---|
| `PatternInt` | LOAD_CONST литерала, EQ со значением `match`, JUMP_IF_FALSE на следующий рукав, тело, JUMP на общий выход |
| `PatternBool` | то же с литералом `true` или `false` |
| `PatternWild` | тело и JUMP на выход, без сравнения |
| `PatternIdent` | MOVE значения `match` в регистр биндинга, тело, JUMP на выход. Это catch-all: следующих рукавов нет |

`PatternRecord` и вложенные паттерны сюда не входят. Для них шаг 247 — именованный отказ, не новый opcode.

## Почему не новый opcode

Срез — равенство литералу и провал в `_` или биндинг. Для этого хватает EQ и JUMP_IF_FALSE. Отдельный opcode `match` всё равно разворачивался бы в те же сравнения либо дублировал бы их внутри исполнителя.

В `script_vm/executor.mlc` целочисленный ADD/SUB/MUL/DIV — первая ветка. Дальше идут `is_float_binary_opcode` и `is_float_compare_opcode`, и только затем целочисленные сравнения и JUMP_IF_FALSE. Новый код, вставленный после ADD, не ускоряет B1 и добавляет проверку каждому opcode, который до ADD не дошёл, включая EQ и JUMP_IF_FALSE, из которых как раз состоит `if`.

Замер шага 228, то же дерево, что и этот шаг: `B1.ns_per_insn` 75 при пределе 88, `B3.ns_per_call` 2524 при пределе 4011 (`baseline * 50 / 100` от 176 и 8023). Дерево до float-диспетча `7065c2e1`: B1 86, B3 2797, тоже внутри этих пределов. Две float-проверки гейт не пробили, но они уже стоят на пути EQ и JUMP_IF_FALSE. Третья проверка для `match` этот путь только удлиняет.

Шаг 244 сравнивает `match` из пяти рукавов с ручной цепочкой `if` и требует разницу не больше 10%. Та же последовательность EQ + JUMP_IF_FALSE + JUMP, что у `if`, этому соответствует. Новый opcode добавил бы диспетчеризацию сверх этой последовательности.
