# Track: VM — типизированные коллекции (массивы/map только `i32`)

Parent: [TRACK_MIR_VM_FULL.md](TRACK_MIR_VM_FULL.md) (§5.2 «Runtime value model»).
Найдено 2026-07-10 при попытке запустить через `mlcc --run` пример с
`[Point]` (массив records). Шире и старее, чем закрытые
[TRACK_VM_LOWERING_GAPS](../archive/tracks/TRACK_VM_LOWERING_GAPS.md) /
[TRACK_VM_BLOCK_ID_COLLISION](../archive/tracks/TRACK_VM_BLOCK_ID_COLLISION.md)
— это не баг лоуэринга и не CFG-баг, а архитектурное ограничение value-модели
VM, зафиксированное в самом типе `VmArrayValue`/`VmMapValue`.

## Status: **closed** (2026-07-11) — STEP=1–4 done — pending Critic

**Driver 2026-07-11:** STEP=4 — self-host `mlcc`→`mlcc2`→`diff -rq` identical;
`regression_gate` 20/0; VM corpus (typed 6, examples 28, single+cpp_diff 18,
elif 6, trampoline depth). Track closed.

**Driver 2026-07-11:** STEP=3 — nested field-slot smokes: record↔array,
record↔map, `[[i32]]`; gate extended (`vm_record_holds_*`, `vm_nested_array`).
VmFieldSlot Record/Array/Map arms were already in STEP=2; this step proves
round-trip via `__mir_record_*` / nested push.

**Driver 2026-07-11:** STEP=2 — `VmArrayValue.elements` / `VmMapValue.values` as
`[VmFieldSlot]`; added `VmFieldRecord`/`Array`/`Map` Shared arms; native push/set
any value; gate `run_vm_typed_collections_gate.sh` (string/record/i32 corpus).

**Driver 2026-07-11:** STEP=1 — Decision **locked**: Shared-indirection (same
pattern as `VmFieldVariant`); reject bare `[VmValue]` and split-out VM tree.

**Planner 2026-07-11:** activated after GUI Critic OK (`92e15855`). Prefer over
low-pri FFI_SAFETY/ERROR_UNION/DEBUG_SOURCE_MAP: concrete VM correctness gap
(narrows Epic 3 STEP=8 claim).

## Decision (STEP=1, 2026-07-11) — **locked**

### Options

| Вариант | Notes |
|---------|-------|
| **A. Shared-indirection** | `elements: [Shared<VmValue>]` / map values same; mirrors `VmFieldVariant(Shared<VmVariantValue>)` |
| B. Bare `[VmValue]` inside `VmArrayValue` | Recursive sum in C++ layout — **rejected** (§5.2) |
| C. Split VM types out of `compiler/` codegen path | VM still compiled into `mlcc` → C++; fake split — **rejected** |
| D. New `VmBoxedValue` heap module only | Extra indirection layer; no gain over `Shared` already in language — **rejected** (v0) |

### Locked shape (STEP=2 — implemented)

Carrier is **`VmFieldSlot`** (Shared-wrapped nested ADTs), not bare
`Shared<VmValue>` — same Decision A, matches existing `VmFieldVariant` unwrap
via record fields:

```mlc
export type VmArrayValue = VmArrayValue { elements: [VmFieldSlot] }
export type VmMapValue = VmMapValue {
  keys: [string],
  values: [VmFieldSlot],
  entry_count: i32
}
// VmFieldSlot += VmFieldRecord/Array/Map(Shared<...>)
```

- Keys stay `[string]` (unchanged).
- `Shared.new` in VM remains identity wrap (no refcount) — same as today for
  variant fields (`native.mlc` comment).
- Native `__mir_array_push` / `__mir_map_set`: accept any `VmValue`, store
  `Shared.new(value)`; get unwraps Shared → `VmValue`.
- Equality/truthy (STEP=2 minimal): length-based OK to keep; deep equal deferred.

### STEP=3 implication

Extend `VmFieldSlot` with Shared-wrapped container/record arms (e.g.
`VmFieldArray(Shared<VmArrayValue>)`, `VmFieldMap(...)`, `VmFieldRecord(...)`)
so records/variants can hold arrays — same Decision A, not a new strategy.

### Impact

| File | Change |
|------|--------|
| `compiler/vm/value.mlc` | types + field_slot helpers |
| `compiler/vm/native.mlc` | array/map empty/push/pop/get/set/length |
| `compiler/vm/mir_eval.mlc` | truthy on `elements` (not `integer_elements`) |
| gates | repro `[string]` / `[Point]` push must succeed |

### Non-goals (locked)

- Real refcounting / GC for Shared in VM
- COW / perf of containers
- Unifying VM values with user-program C++ `mlc::Array` layout


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
| 1 | Design-решение: как обойти рекурсивный `VmValue` в контейнерах (Shared-индирекция vs раздельные VM/codegen типы) — записать решение в этот трек до кода | **done** (2026-07-11: Shared-indirection locked) |
| 2 | `VmArrayValue`/`VmMapValue` → произвольный `VmValue` элемент/значение; native-функции; regression на репро выше | **done** (2026-07-11: VmFieldSlot elements; gate string/record/corpus ok) |
| 3 | `VmFieldSlot` — добавить `array`/`map`/nested `record` варианты (если п.1 это допускает) | **done** (2026-07-11: arms in STEP=2; nested smokes + gate) |
| 4 | Verify-gate: self-host (`mlcc`→`mlcc2`→`diff -rq`), `regression_gate.sh`, полный VM-корпус | **done** (2026-07-11: diff identical; regression 20/0; corpus ok) |

<!-- sub-steps STEP=1: 1) compare Shared-indirection vs VM-only types vs boxed heap slots; 2) lock one in Decision section; 3) note impact on native.mlc / mir_eval -->
<!-- sub-steps STEP=2: 1) change value.mlc types; 2) native push/set/get; 3) repro [string]/[Point] gate -->
<!-- sub-steps STEP=3: 1) VmFieldSlot variants; 2) from/to value; 3) nested record smoke -->
<!-- sub-steps STEP=4: 1) self-host diff; 2) regression_gate; 3) VM corpus -->


## Out of scope

- Полная унификация VM value model с типом, который видит C++-codegen —
  если п.1 решается через разделение типов, унификация не нужна и не
  входит в этот трек.
- Производительность контейнеров (COW, аллокации) — этот трек про
  функциональную полноту, не про скорость.
