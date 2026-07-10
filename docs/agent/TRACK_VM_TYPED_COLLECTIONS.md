# Track: VM — типизированные коллекции (массивы/map только `i32`)

Parent: [TRACK_MIR_VM_FULL.md](TRACK_MIR_VM_FULL.md) (§5.2 «Runtime value model»).
Найдено 2026-07-10 при попытке запустить через `mlcc --run` пример с
`[Point]` (массив records). Шире и старее, чем закрытые
[TRACK_VM_LOWERING_GAPS](../archive/tracks/TRACK_VM_LOWERING_GAPS.md) /
[TRACK_VM_BLOCK_ID_COLLISION](../archive/tracks/TRACK_VM_BLOCK_ID_COLLISION.md)
— это не баг лоуэринга и не CFG-баг, а архитектурное ограничение value-модели
VM, зафиксированное в самом типе `VmArrayValue`/`VmMapValue`.

## Status: open

## Проблема

`compiler/vm/value.mlc:5-10`:

```mlc
export type VmArrayValue = VmArrayValue { integer_elements: [i32] }
export type VmMapValue = VmMapValue {
  keys: [string],
  integer_values: [i32],
  entry_count: i32
}
```

`VmArrayValue` хранит **только** `[i32]`. `VmMapValue` хранит значения
**только** как `[i32]`. Любой `push`/`set` значения, которое не `VmI32`,
падает в native-слое (`__mir_array_push`/`__mir_map_set`,
`compiler/vm/native.mlc`) с `error: vm: expected i32 value`.

Это делает `Epic 3 STEP=8` («Array/map/string parity with C++ on corpus
file» — отмечен **done** в `TRACK_MIR_VM_FULL.md`) верным только для узкого
корпуса (массив `i32`, typed `Map` с int-значениями), не для массивов
произвольного типа — то же расхождение claim/реальность, что уже было
исправлено для Epic 4 (`TRAMPOLINE`) и Epic 3/tail-`if` (`LOWERING_GAPS`).

Смежная, более узкая проблема уже задокументирована в §5.2 этого же файла:
`VmFieldSlot` (поля record/variant) поддерживает только `i32`/`bool`/
`string`/вложенный `variant` (через `Shared`) — **не** `array`/`map`/
вложенный `record`. `vm_field_slot_from_value` (`compiler/vm/value.mlc:61-68`)
явно возвращает `Err` для `VmArray`/`VmMap`/`VmRecord`.

## Репро (все дают `error: vm: expected i32 value`)

Массив records:

```mlc
type Point = { x: i32, y: i32 }
fn main() -> i32 = do
  let mut points: [Point] = []
  points.push(Point { x: 5, y: 6 })
  0
end
```

Массив строк:

```mlc
fn main() -> i32 = do
  let mut names: [string] = []
  names.push("a")
  0
end
```

(Одиночный record без массива — `p.x` на `Point { x: 5, y: 6 }` — работает
корректно; проблема именно в контейнерах.)

## Оценка объёма

Не point-fix (в отличие от `VM_BLOCK_ID_COLLISION`). Нужен redesign
value-модели контейнеров:

1. `VmArrayValue`/`VmMapValue` → `elements: [VmValue]` вместо `[i32]`.
   Блокер, отмеченный в `TRACK_MIR_VM_FULL.md` §5.2: рекурсивный `VmValue`
   внутри `VmValue` ломает C++-codegen для типа `VmArrayValue`, если этот
   же self-hosted тип компилируется и в C++-путь (не только в VM). Нужно
   решить так же, как с `VmFieldSlot` (`Shared<...>` индирекция) или
   держать VM-типы полностью отдельно от типов, идущих в релизный codegen.
2. Обновить все native-функции (`__mir_array_*`, `__mir_map_*`) под новый
   layout.
3. `vm_value_is_truthy`, `vm_values_equal` (`compiler/vm/mir_eval.mlc`) —
   расширить сравнение на вложенные `VmValue` (сейчас `VmArray`/`VmMap`
   сравниваются по длине, не по содержимому — тоже неточность, но ниже
   приоритетом).
4. `vm_field_slot_from_value`/`vm_value_from_field_slot` — расширить на
   `array`/`map`/вложенный `record`, если решение п.1 это позволяет.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design-решение: как обойти рекурсивный `VmValue` в контейнерах (Shared-индирекция vs раздельные VM/codegen типы) — записать решение в этот трек до кода | open |
| 2 | `VmArrayValue`/`VmMapValue` → произвольный `VmValue` элемент/значение; native-функции; regression на репро выше | open |
| 3 | `VmFieldSlot` — добавить `array`/`map`/nested `record` варианты (если п.1 это допускает) | open |
| 4 | Verify-gate: self-host (`mlcc`→`mlcc2`→`diff -rq`), `regression_gate.sh`, полный VM-корпус | open |

## Out of scope

- Полная унификация VM value model с типом, который видит C++-codegen —
  если п.1 решается через разделение типов, унификация не нужна и не
  входит в этот трек.
- Производительность контейнеров (COW, аллокации) — этот трек про
  функциональную полноту, не про скорость.
