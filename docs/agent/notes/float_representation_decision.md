# Представление float в script_vm

Новый тег не добавляется. Коробочные теги 1–4 не расширяются. `script_vm/value.mlc` этим шагом не менялся.

## Тег и ширина

Значение — `Value { raw: i64 }` в `script_vm/value.mlc`. Кодировка — NaN-box в `runtime/include/mlc/script_vm/value_rep_abi.hpp`.

Коробочные значения: старшие 16 бит `0x7FFC`. Тег в битах 32–47:

| `decode_kind` | тег | `ValueRep` |
|---|---|---|
| 1 | 1 | `ValueNil` |
| 2 | 2 | `ValueBool` |
| 3 | 3 | `ValueInt32` |
| 4 | 4 | `ValueHeapRef` |

Float — всё, что не попало в этот префикс. `decode_kind` возвращает 0. Вариант `ValueRep` — `ValueFloat64(f64)`. Ширина 64 бита, IEEE binary64. `encode_float64` копирует 64 бита числа; если они совпали с префиксом коробки, подставляет `k_float_nan_escape` (`0x7FF8000000000001`).

Отдельного вида и отдельного тега для `f32` нет. Имени `encode_float32` в ABI нет. Литерал frontend — `ExprFloat(string)` без типа ширины (`docs/agent/notes/float_frontend_survey.md`): и `f32`, и `f64` остаются `TyNamed`, суффикс лексера в токен не входит. Оба написания, когда лоуэр начнёт их принимать, кладутся в этот же `ValueFloat64`.

## Буфер регистров

Регистры кадра — непрерывный участок `VmState.stack: [Value]` в `script_vm/vm_state.mlc`. База кадра — `frame_base`, слот `index` — `stack[base + index]`. `vm_state_push_frame` растит стек на `register_count` элементов и заполняет их `ValueNil`.

`value_byte_size()` возвращает 8. Слот уже ширины одного `Value`. Float занимает один слот, как `i32`, bool, nil и ссылка на кучу. Второй машинный слот под float не нужен, длина элемента стека не меняется.
