# MLC Script VM — концепт динамического профиля языка

Статус: **design-only, реализация НЕ авторизована**. Это документ-концепт
для будущего трека, не текущая работа. Источник: предложение пользователя
2026-07-13 (запрос к внешней модели по архитектуре embeddable dynamic VM).

## 0. Disambiguation — НИКОГДА не путать с MIR VM

В проекте два совершенно разных объекта носят слово «VM». Правило: **бэйр
слово «VM» без уточнения запрещено** в доках/треках/коммитах, начиная с этого
документа. Всегда пишем полное имя.

| | **MIR VM** (существует) | **MLC Script VM** (этот документ, design-only) |
|---|---|---|
| Что это | Tree-walking интерпретатор над MIR — debug/bootstrap инструмент компилятора | Отдельная встраиваемая (embeddable) VM для динамического профиля языка |
| Зачем | `mlcc --run` без `g++`; self-host proof-of-concept; CI parity против C++ backend | Запуск `mlc script`-модулей (implicit `dyn`) в игровых/UI/конфиг/серверных скриптах |
| Код живёт в | `compiler/vm/` (часть self-hosted `mlcc`) | **Не создавать в `compiler/vm/`.** Отдельный top-level каталог `script_vm/` (design-only, каталога пока нет) |
| Трек | [agent/TRACK_MIR_VM_FULL.md](agent/TRACK_MIR_VM_FULL.md) | [agent/TRACK_MLC_SCRIPT_VM.md](agent/TRACK_MLC_SCRIPT_VM.md) |
| Bytecode | Нет — прямой обход `MirStmt`/`MirTerminator` structs | Да — регистровый bytecode (§4) |
| GC | Нет — копирует `[VmValue]`, никакого heap/refcount | Да — non-moving incremental mark-sweep (§8) |
| JIT | Явно **out of scope** (см. TRACK_MIR_VM_FULL §"Explicitly out of scope") | Опциональный слой, baseline JIT после adaptive interpreter (§5) |
| Embedding | Не предназначена — внутренний инструмент | Да, стабильный C ABI — основная цель (§10) |
| Значения | `VmValue` — MLC-нативный variant, `integer_fields: [i32]` (см. TRACK_VM_TYPED_COLLECTIONS) | `Value { raw: u64 }` — tagged/NaN-boxed (§6) |
| Целевой язык | Тот же статический MLC, что компилируется в C++ (просто другой backend для дев-цикла) | Отдельный **script profile** того же синтаксиса — implicit `dyn`, а не полноценный статический MLC |
| Замена C++ release backend | Нет, никогда (явный non-goal) | Нет — это третий путь исполнения, не замена AOT |

Если задача про `mlcc --run`, `compiler/vm/`, self-host bootstrap без `g++` —
это **MIR VM**, читать `TRACK_MIR_VM_FULL.md`, не этот документ.

Если задача про `dyn`, bytecode, embeddable runtime, JIT, GC — это
**MLC Script VM**, этот документ и его трек.

## 1. Зачем два профиля одного языка, а не два языка

`mlc native` — статическая типизация, AOT, unboxed-значения (то, что есть
сейчас). `mlc script` — тот же синтаксис, модули и базовая семантика, но
неаннотированные типы по умолчанию `dyn`, выполнение — через MLC Script VM.
Тип `dyn` — явная точка перехода между мирами.

Пересечение максимально сверху (синтаксис, HIR, модули, стандартная
библиотека) и минимально снизу (представление значений, нижний IR,
codegen):

| Слой | Пересечение |
|------|-------------|
| Lexer/parser | 90–100% |
| AST/HIR | 80–95% |
| Name resolution/modules | 80–100% |
| Closures/control flow/patterns | 70–90% |
| Стандартная библиотека API | 60–90% |
| Type checking | 20–50% |
| Lowering | 20–40% |
| Runtime representation | 10–30% |
| Codegen | почти разное |

## 2. Соотношение с уже принятым решением по `any`/`dyn`

[LANGUAGE_DESIGN.md](LANGUAGE_DESIGN.md) §«Что не войдёт в язык» отклоняет
встроенный `any`/`dynamic` тип **для статического MLC** — это решение не
отменяется. MLC Script VM не противоречит ему: `dyn` — часть отдельного
**script profile** с собственным lowering/runtime, не тип, который можно
внезапно получить в native-коде без explicit границы. В native MLC `dyn`
допустим только как явно аннотированный тип на границе (см. §3), не как
инференс по умолчанию.

## 3. `dyn` vs `any` — разные типы

```mlc
let x: any   // неизвестный статический тип; pattern-match/store/pass — можно, `x + 1` — нет
let x: dyn   // динамические операции разрешены: x + 1, x.name, x(...) → DYN_ADD/DYN_GET/DYN_CALL
```

В native-профиле `dyn` — явный, редкий, на границе с script/FFI. В
script-профиле неаннотированные параметры/локали по умолчанию `dyn`.

## 4. Bytecode — регистровый, не стековый

```text
LOAD_CONST   r0, #12
GET_PROP     r1, r0, :velocity
MUL          r2, r1, r3
SET_PROP     r0, :position, r2
CALL         r4, r5, argc=2
RETURN       r4
```

Формат: `[ opcode:8 | A:8 | B:8 | C:8 ]` (32 бита) + extension-инструкции для
больших констант/длинных переходов. Регистровый выбран вместо стекового —
ближе к CoreIR, лучше для JIT, меньше dispatch, проще специализация, frame
как массив `Value`.

## 5. Прогрессивные тиры исполнения

| Tier | Что | Когда |
|------|-----|-------|
| 0 | Обычный интерпретатор над bytecode | Всегда |
| 0.5 | Quickening — инструкции специализируются по наблюдаемым типам (`ADD` → `ADD_I64`/`ADD_F64`), property access через shapes + inline cache | После нескольких выполнений; практически полезный продукт уже на этой стадии |
| 1 | Baseline JIT x86-64 — компилирует целую функцию в шаблонный machine code, guards со slow-path, VM frame остаётся материальным (без deopt) | Опционально, отключаемый слой |
| 2 | Function/region JIT с SSA IR, type propagation, guard elimination, basic-block versioning | Только по профилям реальных приложений, не trace JIT (LuaJIT-style side exits — сознательно избегаем) |

Deegen (arXiv 2411.11469) и CPython PEP 659/744 — референсы для генерации
interpreter+JIT из одного описания инструкций (см. §9).

## 6. Представление значений

```mlc
struct Value { raw: u64 }
struct ObjectHeader { type_id: u32, gc_flags: u16, object_flags: u16, shape_or_meta: u64 }
struct TypeDesc { name: Symbol, trace: fn(*Object, &mut GcTracer), drop: fn(*Object), methods: *MethodTable }
```

NaN-boxing/tagged union — скрыто за `ValueRep` abstraction layer, не
захардкожено в язык. Статические объекты (`Entity`, `Vec3`, `Array<T>`) не
обязаны боксироваться — при передаче в script создаётся `NativeRef` wrapper
(ptr + type descriptor + ownership flags), не полная копия. Это даёт FFI без
требования жить в GC heap целиком.

## 7. Numeric semantics — решить заранее, не притворяться одинаковыми

```text
script Int   = tagged signed integer → overflow → boxed i64 → overflow → error (без BigInt в core)
script Float = f64
native i8/i32/u64/f32 — сохраняются как есть; переход границы = explicit проверяемая конверсия
```

## 8. GC и изоляция

Non-moving incremental mark-sweep, size-class arenas, write barrier, weak
refs только при необходимости, host handles как GC roots. Параллелизм — не
concurrent GC, а несколько независимых `Vm` (isolates) + message passing,
согласуется с однопоточной async-моделью MLC + отдельный worker pool.

```text
Runtime: immutable opcode tables, built-in type descriptors, JIT backend
Vm: heap, globals, modules, fibers, handle table, allocation limits, instruction budget
```

Один `Vm` — один поток.

## 9. Opcode DSL — VM как self-hosting dogfood

```mlc
bytecode Add(dst, lhs, rhs) {
  specialization (Int, Int) {
    let value = checked_add(unbox_int(lhs), unbox_int(rhs)) else slow_path
    store(dst, box_int(value))
  }
  specialization (Float, Float) {
    store(dst, box_float(unbox_float(lhs) + unbox_float(rhs)))
  }
  fallback { store(dst, runtime.dynamic_add(lhs, rhs)) }
}
```

Из одного описания — interpreter handler, quickened variants, baseline JIT
templates, verifier metadata, disassembler, stack/safepoint metadata,
equivalence tests, документация. Внедрять **после** рабочего интерпретатора
(§12, фаза 4), не до.

## 10. Embedding ABI

```c
MlcVm* mlc_vm_create(const MlcVmConfig*);
void mlc_vm_destroy(MlcVm*);
MlcResult mlc_vm_load_module(MlcVm*, const char* name, const void* source_or_bytecode, size_t size);
MlcHandle mlc_vm_get_export(MlcVm*, const char* module, const char* name);
MlcResult mlc_vm_call(MlcVm*, MlcHandle callable, const MlcValue* args, size_t argc, MlcValue* result);
void mlc_handle_release(MlcVm*, MlcHandle);
```

Конфигурация без скрытого глобального состояния: allocator, module resolver,
clock, random source, logger, native module resolver, instruction limit,
heap limit, JIT code-cache limit, panic callback. Для untrusted-кода —
verifier (индексы регистров/констант, control-flow targets, форма
инструкций, ресурсные лимиты) — LuaJIT сознательно не верифицирует свой
bytecode для untrusted источников, MLC Script VM не должна повторять это
ограничение.

## 11. Deoptimization — не делать сложную в первой версии

VM frame остаётся материальным всегда:

```mlc
struct Frame { function: *FunctionCode, instruction: u32, registers: *Value, caller: ?*Frame }
```

JIT читает/пишет logical registers в frame, держит значения в CPU-регистрах
только внутри коротких последовательностей, синхронизирует frame перед
safepoint. Guard fail → slow path или возврат в интерпретатор — почти
бесплатно, без восстановления виртуальных объектов.

## 12. Рекомендуемая последовательность (для будущего трека)

1. VM без JIT: register bytecode, verifier, closures, arrays/records, tagged
   `Value`, non-moving GC, embedding ABI, native type descriptors, fibers,
   bytecode dump/trace.
2. Адаптивный интерпретатор: counters, quickening, shapes, inline caches,
   specialized arithmetic/native calls — **уже практически полезный продукт**.
3. Baseline JIT x86-64: whole-function compile, guards+slow stubs,
   материальный VM frame, bounded code cache, только синхронная компиляция.
4. Opcode DSL (§9) — после того как семантика инструкций стабилизировалась.
5. Tier 2 — только по профилям реальных приложений; возможно, tier 0.5+1
   закрывают 95% задач и tier 2 не понадобится вовсе.

## 13. Ожидаемый размер (design target, не обещание)

| Конфигурация | Код runtime | Пустой isolate |
|---|---:|---:|
| Простой bytecode-интерпретатор | 200–700 КБ | 50–250 КБ |
| + shapes, inline caches | 400 КБ–1 МБ | 100–400 КБ |
| + baseline JIT x86-64 | 0.8–2 МБ | + code cache |
| + полноценный optimizing JIT | 2–10+ МБ | зависит от IR/профилей |

## 14. Явно исключено (в первой версии)

Prototype chain, неявные string↔number конверсии, повсеместный `null`,
изменение классов на лету, `eval` в базовом runtime, динамическая область
видимости, магическое разрешение методов, BigInt в core, concurrent/moving
GC, trace JIT (LuaJIT-style), optimizing JIT до появления рабочего tier 0/0.5.

## 15. Синтаксис script-модуля (иллюстрация, не финальный грамматика)

```mlc
script module game.ai

fn update(entity, dt) {
    entity.position += entity.velocity * dt
    if entity.health < 20 {
        entity.state = :escaping
    }
}
```

Native-эквивалент того же модуля:

```mlc
native module game.ai

export fn update(entity: &mut Entity, dt: f32) {
    entity.position += entity.velocity * dt
    if entity.health < 20 {
        entity.state = .escaping
    }
}
```

## 16. Границы script ↔ native вызовов

Script → native: компилятор генерирует `NativeFunctionDesc` (name, arity,
типизированные args/result, wrapper); call site кэширует descriptor →
`CALL_NATIVE_TYPED`. Native → script: `vm.bind[fn(&mut Entity, f32) -> void]("game.ai.update")`
проверяет совместимость один раз, возвращает typed stub (box → VM call →
unbox), JIT может специализировать stub позже.

## 17. Не готовый JIT backend — свой baseline emitter

LLVM/MIR (vnmakarov) — не для core: мешают цели «маленькая embeddable VM».
Свой `jit/` (machine_ir, assembler, executable_memory, abi_x64, emitter_x64,
patcher) на самом MLC — компактнее и лучше согласован с tagged values и slow
paths. Полноценный optimizer — отдельный крупный подпроект, не в первой
версии.

## Итоговая формулировка концепта

> `mlc script` — script profile языка MLC с неявным `dyn`, VM object model
> и динамическим dispatch. Использует общий frontend и HIR со статическим
> MLC (native profile), но имеет отдельный dynamic lowering, bytecode и
> runtime — MLC Script VM. Взаимодействие со статическим MLC — через
> генерируемые типизированные адаптеры (§16), не через общий CoreIR.

MLC Script VM: регистровая, изолированная (один `Vm` — один поток),
incremental GC, quickening + inline caches, отключаемый baseline JIT,
собственный компактный assembler backend, без обязательного optimizing JIT.
