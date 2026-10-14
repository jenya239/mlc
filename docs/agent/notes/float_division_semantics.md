# Деление float на ноль и NaN

Float-деление на ноль не считается ошибкой рантайма. Целое деление на ноль остаётся ошибкой. Сравнение NaN через `==` даёт `false`.

Коды взяты из `docs/agent/notes/float_opcode_decision.md`. Эта семантика относится к FDIV (31) и FCMP_EQ..FCMP_GE (32–37). Opcode DIV (6) не переписывается: два `i32` с делителем 0 по-прежнему дают `err_code` `"div_zero"` (`run_err_div_zero` в `script_vm/run_result.mlc`).

## FDIV

Оба операнда — `ValueFloat64`. `err_code` пустой. Результат — `ValueFloat64` с битами IEEE-754 binary64, теми же, что даёт `value_encode(ValueFloat64(левый / правый))` на хосте.

| Левый | Правый | Результат |
|---|---|---|
| 1.0 | 0.0 | +∞ |
| 1.0 | −0.0 | −∞ |
| −1.0 | 0.0 | −∞ |
| −1.0 | −0.0 | +∞ |
| 0.0 | 0.0 | NaN |
| −0.0 | 0.0 | NaN |

Проверка в unit-тесте: `err_code == ""` и `value_raw_equal` с `value_encode(ValueFloat64(...))` от того же частного. Для строки `"div_zero"` тест ждёт несовпадение.

## FCMP

Результат — `ValueBool`, `err_code` пустой. NaN — значение FDIV `0.0 / 0.0`.

| Opcode | Пара | Bool |
|---|---|---|
| FCMP_EQ | NaN, NaN | false |
| FCMP_EQ | NaN, 1.0 | false |
| FCMP_EQ | 1.0, NaN | false |
| FCMP_EQ | 1.0, 1.0 | true |
| FCMP_EQ | 0.0, −0.0 | true |
| FCMP_NE | NaN, NaN | true |
| FCMP_NE | NaN, 1.0 | true |
| FCMP_LT | NaN, NaN | false |
| FCMP_LT | NaN, 1.0 | false |
| FCMP_LT | 1.0, NaN | false |
| FCMP_LE | NaN, 1.0 | false |
| FCMP_GT | NaN, 1.0 | false |
| FCMP_GE | 1.0, NaN | false |
| FCMP_LT | 1.0, 2.0 | true |
| FCMP_EQ | +∞, +∞ | true |
| FCMP_LT | 1.0, +∞ | true |
| FCMP_LT | +∞, −∞ | false |

+∞ — результат FDIV `1.0 / 0.0`. Любое из четырёх упорядоченных сравнений, где хотя бы один операнд NaN, даёт `false`.
