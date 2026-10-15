# MLC Script VM + TextUI: план реализации по текущему коду

**Дата аудита:** 2026-10-15. **Статус:** проект реализации, не отчёт о выполнении. **Область:** `script_vm/`, embedding bridge, актуальный `misc/textui/`, runtime.

**База:** HEAD `71b473c9c36cb0751b3106674e26790f0872e6cf` и рабочее дерево `/home/jenya/workspaces/current/mlc` на момент чтения. На старте было 47 изменённых/неотслеживаемых файлов, включая TextUI, GLFW, build harness и state очереди Script VM. Эти изменения не принадлежат данному документальному шагу. Ниже разделены факты исходников, ранее записанные результаты и предлагаемые изменения. Наличие функции или теста не означает, что полный suite заново пройден.

**Родительский документ:** [Архитектура MLC VM в нативной OpenGL GUI-среде](MLC_VM_GUI_Architecture_RU.md), скопирован без изменений. Здесь общая архитектура адаптируется к реальному MLC; при расхождении описаний текущего поведения источником истины служит код.

**Граница поручения:** подготовка документов. Реализация пунктов ниже не начата; исторические треки не переоткрываются, очередь 201–300 не переключается. JIT и перевод всего приложения в Script VM не являются условиями первого результата. Слово «VM» в импортированном родительском документе здесь означает **MLC Script VM**, если явно не указана MIR VM.

## Содержание

1. [Основной вывод](#implementation-01)
2. [Что реально существует](#implementation-02)
3. [Разрывы и блокеры](#implementation-03)
4. [Целевая структура и владение](#implementation-04)
5. [Loaded module и безопасность байткода](#implementation-05)
6. [Постоянная Script VM и resumable execution](#implementation-06)
7. [Host API, данные и capabilities](#implementation-07)
8. [Интеграция с TextUI](#implementation-08)
9. [Actors, потоки, reactor и workers](#implementation-09)
10. [Процессная изоляция и IPC](#implementation-10)
11. [SQLite, resume и reload](#implementation-11)
12. [Agent/MCP и обновление host](#implementation-12)
13. [План поставки по шагам](#implementation-13)
14. [Тесты, измерения и приёмка](#implementation-14)
15. [Первые изменения и отложенные решения](#implementation-15)
16. [Доказательства и проверки документа](#implementation-16)

<a id="implementation-01"></a>
## 1. Основной вывод

MLC не нужно писать новую Script VM с нуля. Нужна эволюция существующего `script_vm/` из синхронного исполнителя функций в постоянную ограниченную среду с сохраняемым execution state, затем предметный host boundary и интеграция с TextUI. **MIR VM** в `compiler/vm/` остаётся инструментом компилятора и не становится GUI sandbox.

```text
аудит baseline и ограничения формата
 -> verified loaded module + постоянный heap
 -> run_slice / yield / cancel + GC roots
 -> nonblocking service boundary и поколения объектов
 -> маленький GUI command protocol
 -> live TextUI demo с существующим renderer
 -> HostCall + actors + async completion
 -> sandbox process + IPC + capabilities
 -> durable state / outbox / reload
 -> agent gateway и native host upgrade
```

Первый вертикальный результат: доверенный Script VM module обрабатывает кнопку и обновляет текст одного выделенного виджета через host command; UI продолжает отвечать при бесконечном цикле скрипта, а исполняющий поток не вызывает GL. Это не объявляется поддержкой hostile plugins до process-isolation gate.

Первый actor может иметь только `i32` counter, один export, очередь событий и mapped widget. Для него не нужны универсальный `dyn`, новый renderer, переписывание native concurrency и JIT.

<a id="implementation-02"></a>
## 2. Что реально существует

### 2.1. Три пути исполнения

| Путь | Точка | Назначение |
|---|---|---|
| Native MLC → C++20 | `compiler/`, Ruby bootstrap `lib/mlc/` | Компиляция host, TextUI и самой Script VM |
| MIR VM | `compiler/vm/` | Debug/bootstrap; не менять ради embedding |
| MLC Script VM | `script_vm/` | Развивать как профиль встроенной логики |

`script_vm/lower/lower_module.mlc` переиспользует lexer/parser из `compiler/frontend/` и строит собственные `FunctionProto`/module bytes. Это подмножество native языка: неподдержанные declarations получают `unsupported_import`, `unsupported_extern_lib` и другие diagnostics. Нельзя передать `misc/textui/app.mlc` этому lowering и ожидать интерпретируемый GUI.

### 2.2. Подтверждённые части Script VM

| Подсистема | Файлы / символы | Текущее поведение |
|---|---|---|
| Values | `value.mlc`, `value_rep_abi.hpp` | 8-byte `Value { raw: i64 }`; Nil/Bool/Int32/Float64/HeapRef |
| Bytecode | `bytecode.mlc`, `bytecode_wide.mlc`, `bytecode_text.mlc`, `exec_float.mlc` | Register instructions, extension words, text/float операции |
| Verifier | `verifier.mlc::verify_function` | Opcode, registers/constants, instruction boundaries, branches, часть closure sources |
| Module | `module_blob.mlc::decode_module` | `MLSC`, version 1, exports/protos/raw constants; registers ≤ 256 |
| Interpreter | `executor.mlc::run_state_program_limited` | Verify-first, синхронное выполнение до результата/ошибки |
| State | `vm_state.mlc::VmState` | Register stack, frame arrays, PC, closure roots и limits |
| Calls | `executor_calls.mlc` | Frames/upvalue cells; максимальная глубина 256 |
| Heap | `heap_types.mlc`, `heap_alloc.mlc`, `heap_access.mlc` | Slots, object ID references, element arena, free list |
| GC | `heap_collect.mlc::heap_in_place_collect` | Полный mark/sweep и compact element arena за один вызов |
| Roots | `vm_state_live_roots`, `executor_gc.mlc` | Registers/closures текущего execution и явные heap roots |
| Limits | `RunLimits`, embedding bridge | Instruction limit подключён; heap limit считает объекты |
| C ABI | `runtime/include/mlc/script_vm/embedding_abi.h` | Create/load/get_export/call/release и REPL |
| Bridge | `runtime/src/script_vm/embedding_abi.cpp` | Хранит blobs/handle slots, вызывает MLC; нет второго interpreter |
| Lowering | `script_vm/lower/` | Functions/control flow/arrays/records/text, развивающийся float-срез |
| REPL | `repl_session.mlc`, `repl_embed.mlc` | Accepted source/pending text, не persistent heap приложения |
| Tests | `script_vm/tests/`, `scripts/run_script_vm_*` | Units, ABI/misuse, lowering, differential, benchmarks |

Шапка [MLC_SCRIPT_VM.md](MLC_SCRIPT_VM.md), где всё design-only и каталога ещё нет, историческая. §103 описывает закрытую Phase 1; последующие очереди развили lowering, execution, limits и REPL. Историческое «лимиты только сохранены, но не применяются» уже неверно: текущий bridge вызывает `emb_call_export_ptr_limited`.

### 2.3. Точная семантика embedding

```text
mlc_vm_load_module:
 verify(bytes_from_pointer(caller_memory)) -> copy bytes в modules[name]
mlc_vm_get_export:
 decode для проверки export -> append HandleSlot с копией blob -> handles.size()
mlc_vm_call:
 index/live lookup -> emb_call_export_ptr_limited
 -> copy/decode/verify_decoded -> heap_new()
 -> run_program_with_args_limited -> verify_protos -> новый VmState -> Ok/Err
```

Следствия:

- `MlcVm` — registry/config, а не живой `Heap + VmState` между calls.
- Call повторно разбирает и проверяет модуль. Возможную экономию cache надо измерить.
- `MlcHandle` — export binding, не rooted heap reference и не capability.
- Старый export handle сохраняет старый blob; replacement `modules[name]` не переключает его автоматически.
- Scalar result подходит текущему ABI. Raw HeapRef не переносит lifetime heap за окончание call.
- Raw `Value` arguments нельзя использовать как доверенный wire format для hostile клиента.
- Load проверяет caller memory, затем копирует повторно. Для verified cache сначала нужны owned bytes и проверка именно этой копии. Повторная верификация на call не заменяет immutable-load contract.

### 2.4. Актуальный GUI

База — `misc/textui/`; `misc/editor/` и `demo_live` официально не продолжаются.

- `app.mlc::main`: GLFW context и `glfw_gl_swap_interval(0)`; владеет store/state/shaping/cache/submit resources; poll → update → layout при необходимости → paint/submit → swap.
- `store.mlc`: flat retained store, `WidgetId { index: i32 }`, children IDs. List rows — данные, не widgets на каждую строку.
- `frame_request.mlc`: `FrameSignature`, field/listing/overlay revisions, решение `None/Paint/Layout`.
- `hover_damage.mlc::submit_ui_frame`: реальный выбор full/damage paint. Новый demo не должен обходить его.
- `submit.mlc`, `submit_layers.mlc`: shared renderer; widgets производят `DrawOp`.
- `host_input.mlc`: GLFW input и wait-events; completion из Script VM должен будить wait.
- `chat_host.mlc::chat_frames`: вложенный event loop. Drain только в `app.mlc` перестанет обслуживаться при открытии chat.
- `window_session.mlc`: UI/preferences restore, не checkpoint Script actor.

Поиск `script_vm`, `mlc_vm_`, `mlc_repl_` в TextUI/GL integration не выявил связанного пути. Адаптер нужен явно.

### 2.5. Переиспользуемый runtime

| Есть | Использовать | Не считать уже решённым |
|---|---|---|
| Channel/StopToken/ThreadPool | Native queues/cancel, bounded CPU pool | Nonblocking admission и task/principal quotas |
| `Isolate<State, Message>` | Single-owner serial handler semantics | Pool-scheduled actors: сейчас OS thread на isolate |
| Supervisor | Restart policies | OS sandbox supervision и durable fencing |
| `reactor::EventLoop` | Async HTTPS/timers/wakeup | Сохранение native coroutine в Script continuation |
| SQLite ABI/stdlib | Storage service/transactions | Actor inbox/outbox/checkpoints |
| Chat transport generation | Пример late-response handling | Security principal/capability |
| Chat persistence/store | Прикладные SQLite/redaction примеры | Универсальный durable actor engine |

`ThreadPool::submit` и `Isolate::send` блокируются при заполненной очереди. `Isolate::shutdown` join-ит owner; его нельзя бездумно вызывать в UI. SQLite shim имеет `static thread_local Table`: connection/statements создаются, используются и закрываются на одном storage owner thread; numeric handles между потоками не переносятся.

<a id="implementation-03"></a>
## 3. Разрывы и блокеры

### 3.1. До hostile plugins

| ID | Наблюдение | Риск | Исправление |
|---|---|---|---|
| G01 | Новый heap/state на call | Нет persistent objects | `ScriptInstance` lifetime |
| G02 | Только RunOk/RunErr | Нет resume/cancel; долгий call | `run_slice`, Yield/Waiting/Cancelled |
| G03 | Registers/PC кешируются локально | Потеря state при наивном yield | Materialization всех выходов |
| G04 | instruction_count i32, limit i64 | Несогласованный диапазон | Wide checked/saturating counter |
| G05 | Quota по object count | Огромный array/string | Byte/arena/temporary quota до allocation |
| G06 | Полный GC за вызов | Fuel не ограничивает GC pause | Slice GC или ограниченный latency profile |
| G07 | Raw constants/arguments | Forged HeapRef на объект instance | Canonical values, запрет heap-ref constants |
| G08 | i32 length arithmetic | Overflow/неверная проверка lengths | Checked decoder и budgets |
| G09 | Handle=index/live | Нет generation/owner/epoch | Opaque scoped handle table |
| G10 | WidgetId=index | Нет внешней lifetime/authority | Host resource table поверх IDs |
| G11 | GUI revisions не транзакции | Stale/частичный patch | Stage/validate/publish |
| G12 | Нет Script Host imports | Нет контролируемых GUI/I/O effects | Versioned import table и broker |
| G13 | Нет связанного actor scheduler | Нет bounded async model | Mailbox + resumable tasks |
| G14 | Нет Script sandbox/IPC пути | Native сбой общей судьбы | Process boundary и Linux policy |
| G15 | Нет actor durable ledger | Потеря/повтор эффектов | SQLite inbox/outbox/reconcile |

G07/G08 — выводы по коду для negative tests/hardening, не воспроизведённые exploits. Текущий verifier структурный; runtime type checks для динамического профиля допустимы. Не нужно объявлять байткод полностью статически типизированным ради общей схемы.

### 3.2. Дополнительные ограничения

- `verify_function` не получает module-level signatures/upvalue/proto metadata. HostCall требует module verifier.
- `primary_index_at` ищет среди массива primaries. Проверить worst-case branch validation; при необходимости bitmap/index вместо repeated scan.
- C ABI не имеет полной exception boundary для allocations. `panic_callback` не восстанавливает процесс после heap corruption.
- `mlc_repl_destroy` ставит `released`, но не удаляет allocation: double-release detection оплачивается tombstone lifetime. Для долгоживущего host нужен явный ownership contract; повторный destroy удалённого raw pointer не обещать.
- GC сохраняет object IDs, но перемещает backing `element_arena`: native borrowed address не переживает collection.
- `directory_jail::path_inside_jail` — строковая нормализация/проверка, не OS sandbox и не symlink-safe broker.
- `host_now_ms` использует i32 из GLFW time. Для новых deadlines/leases нужен отдельный широкий monotonic API.
- Native `extern thread_safe/blocking` metadata не выдаёт право вызывать эту функцию из Script module.

### 3.3. Performance blocker

В `docs/agent/SCRIPT_VM_QUEUE_300_STATE.md`: `current_step: 227`, `next_step: 228`, `status: blocked`, `consult_verdict: stop`. Последние записанные B1: 71, затем 81 при limit 42; B3: 2739, затем 2904 при limit 1490. Это результаты предыдущей работы, не новый benchmark аудита.

`run_script_vm_float_bench.rb` задаёт S200 baselines 35 ns/instruction и 1242 ns/call, допуск 20%. Старые `bench_baseline.txt`/`bench_current.txt` — другая серия. Перед изменением execution loop нужен воспроизводимый baseline и разрешение текущего blocker в его собственной очереди. Новый документ не объявляет gate зелёным и не повышает бюджеты.

<a id="implementation-04"></a>
## 4. Целевая структура и владение

```text
Native MLC TextUI                    Script runtime service
 app/chat frame pump                   scheduler
 WidgetStore + UiState                 ScriptInstance per actor
 layout + damage + shared submit       verified modules + executions
          ^                                      |
          +--- immutable bounded messages -------+
                         broker
                     /     |      \
                 workers reactor storage
                         |
               sandbox transport adapter (позднее)
```

Broker policy и GUI authority остаются в host. Runtime service сначала работает на выделенном thread только для доверенного кода; затем тот же контракт переносится в sandbox process.

### 4.1. Предлагаемые файлы

Пути в таблице — **новые проектируемые модули**, не существующая реализация. Создавать по шагам, не пустым scaffold всей схемы.

| Зона | Модули | Ответственность |
|---|---|---|
| `script_vm/` | `loaded_module.mlc`, `module_limits.mlc`, `module_verify.mlc` | Verified immutable code |
| `script_vm/` | `instance.mlc`, `execution.mlc`, `execution_result.mlc` | Persistent heap, start/poll/resume |
| `script_vm/` | `executor_slice.mlc`, `execution_budget.mlc`, `execution_roots.mlc` | Slicing/metering/roots |
| `script_vm/` | `host_imports.mlc`, `host_request.mlc`, `executor_host_call.mlc` | HostCall и suspend |
| `script_vm/` | `actor.mlc`, `mailbox.mlc`, `scheduler.mlc` | Logical actors, без GUI/OS |
| `script_host/` | `protocol.mlc`, `capabilities.mlc`, `resources.mlc`, `requests.mlc` | Policy и предметные messages |
| `script_host/` | `service.mlc`, `subscriptions.mlc`, `supervision.mlc` | Admission/lifecycle/resync |
| `script_host/` | `state_store.mlc`, `effect_outbox.mlc`, `checkpoint.mlc`, `reload.mlc` | Durable workflow |
| `misc/textui/` | `script_adapter.mlc`, `script_transaction.mlc`, `script_damage.mlc` | Команды → GUI delta |
| `misc/textui/` | `script_events.mlc`, `script_frame_pump.mlc` | Event routing и общий drain |
| `runtime/include/mlc/script_vm/` | `embedding_v2.h` | Native ABI для lifetime/poll/marshalling |
| `runtime/include/mlc/script_host/`, `runtime/src/script_host/` | Transport/launcher/wakeup bridges | OS primitives, не второй interpreter |
| `script_host/tools/` | `sandbox_main.mlc`, позднее gateway entry | Process entry points |

Policy/SQLite/GUI protocol помещаются в `script_host/`, чтобы не смешивать их с ISA/GC и не раздувать `app.mlc`. Бизнес-логика преимущественно MLC; C++ — только названные ABI/OS holes. Hard cap 400 строк для Script VM/TextUI сохраняется.

### 4.2. Владение

| Owner | State | Наружу |
|---|---|---|
| UI thread | Store/state/layout/cache/GL | Events/snapshots/commit replies |
| Runtime service | Scheduler/instances/heaps/executions | Typed requests/results |
| Broker authority | Capabilities/resources/quotas | Tickets/denials |
| I/O thread | Reactor/native tasks | Completion messages |
| Workers | Immutable job input/owned result | Completion + revision |
| Storage thread | SQLite connection/statements | Durable ACK |

Начать с одного runtime service thread, heap на actor. Не мигрировать native thread-local ресурсы вместе с actor. Parallel execution вводить после measurements и ownership tests. Blocking joins не выполняются на UI path.

<a id="implementation-05"></a>
## 5. Loaded module и безопасность байткода

### 5.1. Не начинать с нового backend

Сохранить текущие `Value`, register ISA, opcode semantics и `FunctionProto` как исходную базу. Версия 1 остаётся читаемой для текущих fixtures. Новый persistent execution может сначала использовать v1, пока не понадобятся import/signature metadata. Формат v2 вводится отдельно с явным feature negotiation, а не тихой сменой значения старого поля.

```text
LoadedModule {                    // проектный тип, не готовый MLC source
  identity: ModuleIdentity
  bytecode_version: Integer
  content_hash: Bytes32
  verifier_version: Integer
  protos: Immutable<FunctionProto[]>
  exports: ExportTable
  signatures: FunctionSignature[]
  imports: HostImportDeclaration[]
  resource_cost: ModuleCost
}
ModuleLimits {
  maximum_blob_bytes, maximum_exports, maximum_protos,
  maximum_words, maximum_constants, maximum_name_bytes,
  maximum_verification_work
}
```

Слово `Immutable` здесь — требуемый ownership contract, не существующий синтаксис MLC. AOT MLC использует COW collections; неизменяемость должна обеспечиваться отсутствием mutating alias и закрытым API модуля. Один verified object может совместно читаться несколькими actors, но mutable heap и caches per actor не разделяются.

### 5.2. Decode hardening

Изменить `module_blob.mlc` небольшими шагами:

1. Проверять общий размер до `bytes_from_pointer`, чтобы не скопировать гигантский input до отказа.
2. Заменить выражения вида `offset + word_count * 4 + 4` на checked arithmetic или проверку `count <= remaining / width` после проверки nonnegative offset/count.
3. Проверять nonnegative export/proto counts и export proto index; отдельно reject duplicate export names и неизвестные required fields.
4. Ограничить суммарные counts, длины имён и work, а не только `register_count <= 256`.
5. Константы v1 разрешать только в canonical scalar representation. Heap references должны создаваться execution/heap allocator, а не загружаться как произвольные object IDs.
6. Для строк и compound constants v2 использовать data sections и безопасную instantiation, не raw pointers/references.
7. После decode/verify зарегистрировать module атомарно. Ошибка оставляет старый module registry entry.

Функция `mlc_vm_load_module` сначала копирует в собственное хранилище с size budget, затем проверяет эту копию. Cache key: module bytes hash + verifier/ISA/features version; filename/module name не является достаточным cache key.

### 5.3. Verifier contract

Переиспользовать `verify_function`, добавив внешний `verify_module`:

- Проверки всех protos, не только reachable entry.
- Cross-proto indices, arity, closure/upvalue descriptor consistency.
- Импорты: известный ID/version/signature, разрешённые value types, required features.
- Reject branch into extension words; отдельно сохранить или явно изменить текущую семантику jump-to-end с последующим `no_return`.
- Ограничить verification work. Динамические значения проверяются runtime, если verifier не может доказать тип.
- Root metadata либо генерируется доверенным runtime, либо проверяется; module не может скрыть живые references от GC.

Текущие registers инициализируются Nil: новый definite-initialization verifier не должен случайно изменить эту семантику. Если нужен строгий mode, это versioned profile с собственными тестами.

### 5.4. Кеширование без второго interpreter

`emb_call_export_bytes_limited` остаётся compatibility entry. Новый call path разрешает export в уже загруженный proto без decode каждого вызова. Interpreter продолжает быть MLC-кодом; C++ bridge не получает копию dispatch loop.

Внутренний trusted entry принимает только `LoadedModule`, который нельзя создать через произвольный public constructor. Непроверенный `FunctionProto[]` по старому test API по-прежнему проверяется. Нельзя просто убрать `verify_protos` повсюду ради ускорения.

<a id="implementation-06"></a>
## 6. Постоянная Script VM и resumable execution

### 6.1. Разделение instance и execution

```text
ScriptInstance {
  identity, incarnation, principal
  loaded_modules
  heap
  rooted_values
  executions
  persistent_state_root
  budgets
}
Execution {
  identity, module_identity, code_epoch
  vm_state: VmState
  status: Ready | Running | Yielded | Waiting | Finished | Failed | Cancelled
  quantum_remaining
  total_remaining
  pending_request
}
PollResult = Completed(OwnedOrRootedValue)
           | Yielded
           | Waiting(HostRequest)
           | Cancelled
           | Failed(ScriptError)
```

Heap принадлежит instance; execution — конкретному task/обработчику. На MVP один actor = один instance = одно активное execution. Несколько waiting executions в одном actor разрешаются позднее, когда GC roots и reentrancy contract явно поддерживают это.

### 6.2. Самое чувствительное изменение executor

`execute_current_frame` копирует current registers в локальный массив и держит PC локально. При HostCall/Yield/Cancel/GC boundary нужно материализовать **именно актуальный кадр** до возвращения:

```text
run_slice(instance, execution, quantum):
  claim_execution_owner()
  restore_current_frame_cache()
  loop:
    if cancel_requested: materialize_current_frame(); return Cancelled
    if quantum_exhausted: materialize_current_frame(); return Yielded
    if total_budget_exhausted: materialize_current_frame(); return Failed(Limit)
    account_checked_cost()
    outcome = execute_one_instruction_or_bounded_step()
    if outcome is HostRequest:
      materialize_current_frame()
      set_waiting(request, destination_register, resume_pc)
      return Waiting(request)
    if outcome ends execution: finalize_roots_and_return(outcome)
```

Это семантическая схема. При RETURN текущий frame уже может быть popped: нельзя в общем `finally` записать старые локальные registers в новый caller frame. Нужен явный результат dispatch с frame transition и единая функция сохранения **до** смены frame ownership. Tests должны останавливаться на каждом instruction boundary вокруг CALL/RETURN/closure creation.

Compatibility `run_program_with_args_limited` выполняет новый execution engine до завершения. Он может внутренне повторять slices, но не обнуляет total limit. Старый synchronous ABI не используется из UI, даже после такого refactor.

### 6.3. GC roots после suspend

Текущий `executor_gc` собирает roots только из переданного `VmState`. После появления persistent instance root set становится:

```text
instance roots = persistent state
               + live stack/closure roots каждого suspended/runnable execution
               + rooted host results
               + queued messages, если они содержат heap values
               + pending import arguments
```

На MVP mailbox лучше хранить serialized/canonical owned data и конвертировать при dequeue: меньше shared lifetime. Если у suspended execution есть HeapRef, другой execution/GC не имеет права его собрать. Returned heap value должен быть либо скопирован в host-owned message, либо зарегистрирован root handle до следующей GC.

Полный GC можно временно оставить на runtime service thread при жёстком малом heap profile и без обещания 10 ms cancellation. Для production bounded latency GC разбивается на mark/sweep/arena phases с work budget. При incremental marking текущий счётчик write-barrier hits не является доказательством корректной incremental barrier: нужны реальные invariant tests при mutation между slices. Compaction может требовать дополнительной памяти; резерв учитывается заранее.

### 6.4. Limits

Ввести отдельно:

- `quantum_fuel`: восстанавливается при следующем poll;
- `task_fuel_remaining`: не восстанавливается при yield/await/retry;
- principal CPU/work budget: не сбрасывается созданием новых actors;
- heap live bytes, arena capacity, stack bytes, pinned/rooted values, temporary buffers;
- limits длины строки/массива, module size, inbox bytes и pending request count.

Для `NEW_ARRAY` reserve учитывает число элементов, а не один объект. Для closure reserve включает closure и создаваемые upvalue cells, с all-or-nothing проверкой перед серией allocations. Для strings учесть, что текущие bytes представлены как `ValueInt32` в element arena: один логический byte стоит существенно больше одного byte плюс metadata/temporary copies.

Deadline использует wide monotonic clock; timeout waiting request и total execution budget — разные поля. Долгая native операция требует собственного bounded contract или helper process.

### 6.5. ABI v2

Не расширять старый `MlcVmConfig` без size/version discriminator: существующий C caller может передать структуру старого размера. Предлагается параллельный versioned API:

```c
/* Эскиз сигнатур, не существующий header. */
MlcStatus mlc_vm_create_v2(const MlcVmConfigV2*, MlcVmV2** out);
MlcStatus mlc_vm_load_v2(MlcVmV2*, MlcBytes, MlcModuleHandle* out);
MlcStatus mlc_vm_start_v2(MlcVmV2*, MlcExportHandle,
                        const MlcHostValue*, size_t, MlcExecutionHandle* out);
MlcStatus mlc_vm_poll_v2(MlcVmV2*, MlcExecutionHandle,
                       const MlcPollBudget*, MlcPollResult* out);
MlcStatus mlc_vm_resume_v2(MlcVmV2*, MlcRequestHandle, const MlcHostValue*);
MlcStatus mlc_vm_cancel_v2(MlcVmV2*, MlcExecutionHandle);
MlcStatus mlc_value_release_v2(MlcVmV2*, MlcRootHandle);
```

`MlcVmConfigV2` начинает с `struct_size`, `abi_version`, flags; reserved fields не объявляются работающими до реализации. Все exported native wrappers ловят допустимые native exceptions и возвращают status; не ловят segfault как обычную ошибку.

Owner thread фиксируется при create или явном attach. Poll/resume/load происходят на нём; другие потоки только отправляют commands в service queue. Cancel через queue/control path или строго ограниченный atomic token. После destroy сырой `MlcVmV2*` недействителен; повторный destroy запрещён контрактом, а безопасная публичная оболочка делает reset владения один раз.

<a id="implementation-07"></a>
## 7. Host API, данные и capabilities

### 7.1. Сначала маленький контракт

Для первого trusted demo достаточно host-initiated handler:

```text
host input -> runtime event -> call counter_handler(previous_state, event)
           -> owned scalar result -> host SetLabelText command
```

Такой thin slice не доказывает общий HostCall/await. Он проверяет thread boundary, responsiveness, resource mapping и real paint path без одновременной разработки всей ISA.

Следующий шаг вводит script-initiated импорты: `ui.commit`, `timer.after`, `task.compute`, `resource.read`, `subscription.open`. Не делать generic `call_native(symbol)` и не открывать native `extern` resolution недоверенным modules.

### 7.2. Import manifest и HostCall

```text
ImportDeclaration {
  operation_id, major_version, argument_schema, result_schema,
  effect_class: PureBounded | AsyncRead | AsyncMutation
}
HostRequest {
  session_epoch, actor_id, incarnation, code_epoch,
  request_id, operation_id, resource_handle, capability_reference,
  expected_revision, deadline_budget, idempotency_key, owned_arguments
}
```

Principal добавляет service/broker, не caller payload. Import доступен только через whitelist manifest, проверенный loader. HostCall записывает место результата и resume PC; запрос не выполняется reentrant callback на interpreter stack. Аргументы до yield копируются в owned message и проходят size accounting.

`resume(request)` разрешён один раз и только для совпадающих session/incarnation/code epoch. Completion может прийти мгновенно, поэтому `Waiting` state и request registry устанавливаются до публикации запроса. Повторный/поздний ответ освобождает ресурсы, но не исполняет continuation заново.

### 7.3. Boundary values

На IPC передаются tagged scalars, UTF-8 strings/bytes, bounded arrays/records и opaque resource references. Никакого `Value.raw` как внешнего формата, native address, GL name, SQLite handle или `WidgetId.index`.

```text
HostValue = Null | Bool | Int32 | Float64
          | Utf8(OwnedBytes) | Bytes(OwnedBytes)
          | Array(HostValue[]) | Record(FieldId -> HostValue)
          | Resource(ResourceHandle)
```

Для f64 закрепить NaN/infinity policy и canonical conversion, учитывая NaN boxing. HeapRef допустим только внутри конкретного ScriptInstance; host получает copy или root handle с owner/generation.

### 7.4. Resource и authority

```text
ResourceHandle { namespace, slot, generation, kind }
ResourceEntry { owner_principal, subtree_or_domain, internal_id, revision, live }
Capability { identity, owner, scope, operations, expires, policy_epoch, parent, revoked }
```

Сначала broker может использовать ordinal opaque refs только на аутентифицированном dedicated channel с привязкой owner/session; на внешней boundary IDs должны быть непредсказуемыми или защищёнными токенами. В обоих вариантах identity не заменяет проверку scope.

Порядок: decode/limits → authenticate principal → capability/ancestry → resource generation → expected revision → reserve cost → commit. Политика revoke и GUI commit сериализуется у UI authority; проверка в worker перед очередью не достаточна. Выполненный эффект не отменяется отзывом задним числом.

Новые права для scripts не наследуются из того, что native приложение уже умеет filesystem/clipboard/HTTPS. Script поддерево не включает trusted menus/approval overlays. Файлы открывает broker относительно разрешённого directory descriptor с symlink/escape policy; `directory_jail` переиспользуется для UX, не для security decision.

<a id="implementation-08"></a>
## 8. Интеграция с TextUI

### 8.1. Resource table вместо массового изменения WidgetId

На первом этапе оставить внутренний `WidgetId { index }` во всех существующих widgets. Добавить внешний `ResourceHandle -> WidgetId` mapping в `script_host/resources.mlc`. Это минимизирует риск для большого количества native callers и текущего WIP.

```text
UiResourceEntry {
  generation, owner_principal, subtree_identity,
  widget_id: WidgetId, revision, live
}
```

Инвалидация обязательна при удалении/rebuild store, уходе actor, закрытии окна и замене subtree. Пересоздание store меняет namespace/epoch. Проверки resource map запрещают одному script сослаться на индекс другого native widget. Позднее можно обогатить внутренний WidgetId, если реальные store reuse patterns этого потребуют.

### 8.2. Первый batch API

Первый набор: `SetLabelText`, `SetEnabled` только после появления такого свойства у native widget, `SetVisible`; для MVP достаточно **SetLabelText**. Нельзя придумывать поддержку свойства без native модели. Create/Delete/Move и сложные bindings добавляются после транзакций и lifetime tests.

```text
UiBatch {
  transaction_id, resource_scope, expected_revision,
  operations: [SetLabelText(handle, utf8_text)]
}
UiBatchResult = Committed(new_revision) | Rejected(reason)
```

`script_transaction.mlc` строит candidate store на copy/COW state, проверяет все ops до публикации. При reject видимое store/state/revision остаётся прежним. Native `widget_store_*` helpers, которые молча возвращают старый store при неверном ID, недостаточны для транзакции: adapter явно проверяет target kind/index/authority и формирует ошибку.

COW clone может быть O(store) на первом изменении. Это допустимая стартовая стратегия для малого bounded script subtree, но её стоимость измеряется; нельзя клонировать всё GUI на каждый hover. После profiling candidate representation можно заменить staged patch/preallocation, не меняя атомарность API.

### 8.3. Публикация и invalidation

Текущий `FrameSignature` не содержит универсальной revision любой label mutation. Поэтому «поменять текст в store» недостаточно: `frame_request` может не заказать нужный layout/paint.

`script_damage.mlc` возвращает явный `DamageKind`:

| Изменение | Требуемая invalidation |
|---|---|
| Text/visibility/structure | Layout + paint affected subtree |
| Только visual property без geometry | Paint affected area |
| No-op patch | Никакого нового frame |
| Batch rejected | Никакой invalidation |

Добавить отдельные script/content revisions в frame input или эквивалентный explicit invalidation канал. Не злоупотреблять `listing_revision` для чужих label updates. Subscription revisions и UI damage revisions имеют разные назначения.

### 8.4. Frame pump

```text
frame_tick:
  drain_control_and_script_commands(max_count, max_bytes, time_budget)
  apply_validated_ui_batches()
  poll_native_input()
  update_native_widgets()
  enqueue_script_events_nonblocking()
  compute_frame_request_with_script_damage()
  layout_if_needed()
  submit_ui_frame_using_existing_damage_path()
  present_if_needed()
```

Порядок внутри frame фиксируется тестами. Native input, изменивший relevant target, повышает revision до применения зависимого stale patch. Нельзя допускать несколько несогласованных commit points.

Общий `script_frame_pump` вызывается из gallery и `chat_frames`, либо chat позже переводится в единую host state machine отдельным измеряемым шагом. Не нужно переписывать chat loop ради MVP; нужно обеспечить service drain и там.

Worker/runtime completion кладёт сообщение, затем вызывает ограниченный thread-safe wakeup bridge (например, выбранный GLFW post-empty-event wrapper после проверки backend API). Queue authoritative; wakeup только hint. Гонка между проверкой очереди и wait закрывается sequence/notification protocol и проверяется тестом. Не добавлять постоянный busy polling ради работы Script VM.

### 8.5. Render ownership

В начале GL остаётся на текущем UI owner. Script runtime передаёт semantic changes, не DrawOp/vertices. Host сохраняет shaping, atlas и submit pipeline. Перенос GL на render thread потребует аудита global scratch/resources в `submit*` и runtime GL wrappers; одного `std::thread` недостаточно.

Не менять `SwapInterval(0)`, не добавлять `glFinish`/readback/synchronous queries в normal path. Сохранить hover gate: `hover_damage_min_us <= 1000` и `hover_damage_min_us * 3 < hover_full_min_us` на установленном harness. Общий документ с ориентиром 60 Hz не заменяет локальный контракт uncapped gallery.

<a id="implementation-09"></a>
## 9. Actors, потоки, reactor и workers

### 9.1. Actor поверх execution, не native Isolate на каждый actor

Существующий native Isolate полезен как пример владения, но один OS thread на каждый Script actor не нужен. Ввести scheduler runnable queue внутри runtime service. На первом этапе один service thread попеременно выполняет короткие slices actors. Позднее несколько service workers получают exclusive actor lease; никаких одновременных calls к одному instance.

```text
ActorRecord {
  logical_identity, incarnation, code_epoch, principal,
  instance, bounded_mailbox, active_execution,
  waiting_requests, lifecycle, budgets
}
```

Первая семантика — non-reentrant actor: до завершения текущего handler обычные сообщения не запускаются, но control/completion поступают. Это проще существующего executor. Quantum yield не открывает новый turn. Чтобы не получить deadlock, handler не должен ждать обычное сообщение самому себе или цикл зависимых actors; тесты и API diagnostics проверяют это. Reentrant await добавляется отдельным mode с revision validation состояния после каждого await.

Fairness — сначала между principals, затем actors, чтобы создание тысяч actors не увеличивало долю CPU. Reserve control mailbox хранится отдельно от обычных requests. Queue лимитируется и по count, и по bytes, и по outstanding requests.

### 9.2. Nonblocking host integration

Не вставлять `Isolate::send`/`ThreadPool::submit` в `ui_update` без нового admission contract. Возможные реализации:

- добавить `try_send`/`try_submit` в соответствующие native primitives отдельным runtime шагом;
- либо создать bounded bridge queue с nonblocking push и маленьким сервисным consumer, который взаимодействует с существующими blocking primitives вне UI.

Для MVP выбрать второй вариант, если он позволяет не затронуть широко используемый `Channel` API; но не дублировать сложный queue engine, если уже имеющийся Channel поддерживает достаточную операцию после проверки его API. В обоих вариантах `Full` возвращается вызывающему коду, а не теряет принятую команду.

ThreadPool полезен для pure jobs с owned input/result. Jobs не получают ссылки на Script heap или GUI store. Результат содержит исходную revision; stale result отбрасывается без мутации. Большие decoding jobs и неотменяемые native libraries выносятся в helper process.

### 9.3. Reactor

В `runtime/include/mlc/reactor/event_loop.hpp` есть curl/timer/wakeup machinery и `run_until`, возобновление native continuations происходит на owner thread после poll. Это можно использовать в I/O service, но нельзя вызвать blocking `run_until` на UI thread.

Адаптер:

```text
Script Waiting(request)
 -> broker validates scope
 -> I/O service creates native async task
 -> reactor completes native task on its owner
 -> serialize Completion(request_id, epochs, result)
 -> runtime service dequeues completion
 -> validates Waiting state -> resume execution
```

C++ coroutine handle не сохраняется в Script state/checkpoint. Restart повторно создаёт нужную операцию по durable intent/status policy. Cancel распространяется в StopToken там, где existing async API его поддерживает, но outcome всё равно определяется operation ledger.

### 9.4. Supervision

Разделить три отказа:

1. Script trap: fail текущий actor/task, сохранить host и другие instances.
2. Runtime service native crash в процессе host: может уничтожить host; recovery через supervisor внутри того же процесса не гарантирован.
3. Sandbox/helper crash: внешний supervisor фиксирует disconnect, fenced incarnation и restart budget.

Переиспользовать native Supervisor policy только после анализа, какие callbacks он перезапускает и на каких threads. OS process launcher/kill/reap, resource limits и restart ledger — отдельная реализация.

<a id="implementation-10"></a>
## 10. Процессная изоляция и IPC

### 10.1. Переезд service без смены предметного API

После in-process trusted demo добавить транспорт loopback и child-process transport к одному `HostRequest/Completion/UiBatch` schema. Security policy остаётся у host даже если child заверяет, что сам проверил байткод. При компрометации child нельзя доверять его type/limit checks.

Sandbox executable компилируется из `script_host/tools/sandbox_main.mlc` через существующий mlcc/build machinery; тонкий native launcher предоставляет OS operations. Первая версия принимает только compiled bytecode, чтобы не тянуть полный frontend и filesystem module resolution внутрь runtime. Если нужен source compile, frontend запускается в отдельном ограниченном compiler helper либо в sandbox с собственными parsing budgets.

### 10.2. Wire v1

```text
Header {
  protocol_major, protocol_minor, kind,
  payload_size, session_epoch, sequence, request_id
}
```

Messages: `Hello`, `LoadModule`, `StartActor`, `DeliverEvent`, `HostRequest`, `HostReply`, `GuiBatch`, `Cancel`, `StopActor`, `Health`, `ProtocolError`. У каждого — максимальный размер и разрешённые state transitions. Actor IDs/principal identity выводятся из channel registration; child не может выбрать другого principal.

Первый transport — Unix domain socket, owned-copy buffers, explicit framing. Ограничить суммарный вход, deep nesting, сообщения/сек и decode work. FD passing/shared memory отсутствуют до измеренной необходимости. Если добавляются позже, проверять fd type/size/rights и реальную immutability backing memory; read-only mapping у receiver не останавливает sender mutation.

Transport retry не выполняет GUI/filesystem эффект повторно без request dedup/status contract. EOF переводит pending operation в Failed или OutcomeUnknown по стадии, а не всегда в «не выполнялось».

### 10.3. Linux sandbox profile

Применить требования родительской архитектуры: minimal environment и fd allowlist, отсутствие display/session bus/GPU access, no_new_privs, syscall allowlist, filesystem/network isolation и доступные resource controls. Конкретная policy фиксируется после проверки kernel/distribution support; этот документ не заявляет, что эти механизмы уже есть в MLC.

Не передавать `DISPLAY`, X11 cookies, Wayland socket, D-Bus session connection, `/dev/dri` и домашние секреты. Очистка env не заменяет закрытие унаследованных fd и блокирование socket/device paths. Текущие X11 clipboard helpers остаются у host и доступны script только как отдельный разрешённый broker operation.

`directory_jail` не повышается до sandbox. Symlink-safe filesystem broker открывает ресурсы относительно разрешённого directory fd, проверяет фактические objects и не доверяет строковому prefix. Для сети ограничить destinations/redirects/разрешённые адреса и credentials.

`isolated-required` не запускает child при отсутствии обязательной policy. `trusted-in-process` — явно другой профиль, а не fallback при ошибке sandbox. Native code/FFI пользователя допускается только отдельным helper profile и не загружается в host.

### 10.4. Process acceptance

- Child пытается читать запрещённый file, подключиться к desktop sockets, GPU и произвольной сети — запрет подтверждён интеграционными тестами.
- Malformed child message не падает host и не меняет чужой widget.
- Infinite loop, memory flood и native crash не блокируют native UI.
- Kill/restart не сбрасывает principal quotas и не разрешает старые completions.
- На X11 проверяются не только env, но фактические transport paths и унаследованные descriptors.

<a id="implementation-11"></a>
## 11. SQLite, resume и reload

### 11.1. Использовать SQLite substrate, не chat schema как actor engine

В MLC уже есть `lib/mlc/common/stdlib/db/sqlite.mlc`, `runtime/include/mlc/db/sqlite_abi.hpp`, `misc/textui/chat_store.mlc`, `chat_persistence.mlc` и transaction helpers. Переиспользуются ABI, prepared statement patterns и проверенные transaction semantics. Actor state хранится в отдельной schema и собственном storage service; не добавлять поля runtime в таблицы chat messages.

Предлагаемая минимальная схема:

```sql
CREATE TABLE actor_state (
  actor_id TEXT PRIMARY KEY,
  schema_version INTEGER NOT NULL,
  module_hash TEXT NOT NULL,
  revision INTEGER NOT NULL,
  fencing_token INTEGER NOT NULL,
  state_blob BLOB NOT NULL
);
CREATE TABLE actor_inbox (
  actor_id TEXT NOT NULL,
  message_id TEXT NOT NULL,
  payload_hash TEXT NOT NULL,
  status TEXT NOT NULL,
  result_blob BLOB,
  PRIMARY KEY(actor_id, message_id)
);
CREATE TABLE effect_outbox (
  actor_id TEXT NOT NULL,
  effect_id TEXT NOT NULL,
  operation_kind TEXT NOT NULL,
  arguments_blob BLOB NOT NULL,
  idempotency_key TEXT NOT NULL,
  status TEXT NOT NULL,
  result_blob BLOB,
  PRIMARY KEY(actor_id, effect_id)
);
```

Это эскиз schema; размеры, индексы, leases и retention оформляются migration-файлами на implementation этапе. В SQLite `INTEGER` signed 64-bit: revision/fencing не wrap, на exhaustion — отказ и новая identity/миграция, не отрицательные значения. Busy timeout и fsync/journal policy фиксируются; долгий DB wait допустим на storage owner, но не UI.

### 11.2. Durable handler

```text
load state(revision R, fence F)
execute pure bounded reducer -> new state + effect intents
storage transaction:
  require revision == R AND fencing == F
  write state R+1
  insert inbox dedup outcome
  insert effect outbox
commit with configured durability
ack
dispatch effects asynchronously
```

Нельзя сначала сделать HTTP/file effect, затем записать «намерение» в БД. После crash между отправкой и записью ответа требуется idempotency/status reconciliation. Для сервиса без поддержки такой проверки outcome становится Unknown, а не автоматически retried.

Нативный GUI — projection durable state. SQLite commit и GUI publish не образуют одну ACID transaction. После durable commit потерянное UI update восстанавливается subscription snapshot. Это конкретный путь к устойчивости без distributed transaction между БД и OpenGL.

### 11.3. Checkpoint

Первый checkpoint — schema-versioned logical state, mailbox cursor и pending effects. Не сохранять raw Value heap IDs, native handles, GL state, SQLite connection или coroutine frame. Таймеры сохранять как logical deadlines с resume policy.

`window_session` можно использовать для UI preferences отдельно. REPL `accepted_source` — код/история declarations, не состояние actor; replay исходников не гарантирует восстановления эффектов.

Resume: load valid snapshot → schema validation → новый incarnation/fence → capability reacquisition → reconcile effects → rebuild resource handles/subscriptions → GUI snapshot → открыть admission. Если старые права отозваны, restore не выдаёт их повторно.

### 11.4. Reactive subscriptions

Начать с именованных projections (`actor.counter`, `job.progress`), без произвольного query engine. Subscribe возвращает snapshot revision и cursor атомарно относительно регистрации изменений; обычное read-then-subscribe теряет окно updates.

`Delta { base_revision, revision, sequence, subscription_epoch }` применяется только при непрерывности. Overflow — `ResetRequired`, не silently dropped delta. Progress можно coalesce как latest value. Повторные writes одного значения не должны вызывать вечный UI↔actor feedback loop.

### 11.5. Hot reload

Текущая замена blob в module registry не является full hot reload: старые handles удерживают свои копии. В новом протоколе:

1. Подготовить/verify новый module и migration без изменения live instance.
2. Остановить admission actor, достичь turn boundary; сохранить pending effects.
3. Отменить/дренировать continuations либо оставить старый module в явно ограниченном side-by-side режиме.
4. Pure migration logical state с отдельным fuel/heap budget.
5. Атомарно commit state + module identity + новый code epoch/fence.
6. Заново связать exports/resources/subscriptions и опубликовать GUI projection.

Для MVP reload только idle actor между turns, без переноса живого стека. После committed migration rollback к старому binary/state не обещается, если схема/эффекты уже несовместимы. Поздний ответ старой epoch не запускает callback, но результат долговечного эффекта сохраняется по effect ID.

<a id="implementation-12"></a>
## 12. Agent/MCP и обновление host

### 12.1. Не путать существующий chat с агентным gateway

TextUI уже имеет `chat_transport.mlc`, model/store/persistence и HTTPS bridge. Это полезный пример async reply/generation/redaction, но не доказательство наличия MCP authorization или permissioned управления приложением.

Gateway строится над `script_host` subject commands: read scoped snapshot, propose patch, apply patch с expected revision, start/cancel job, status. Он может работать без исполнения script-кода, но проходит те же policy/resource/revision checks.

Approval связан с точными normalized args, principal, target, expected revision и сроком. Текст модели/tool response не меняет права. Секреты остаются в host credential boundary; чтение документа и отправка его AI provider — отдельные permissions. После timeout изменяющий tool обращается к operation status; не повторяет эффект слепо.

Не расширять `chat_host.mlc` до god-gateway. Transport/authentication/schema mapping — отдельный entry/service; trusted approval UI остаётся native. Версия MCP закрепляется на момент реализации и проверяется по официальной спецификации из источников родительского документа.

### 12.2. Обновление native host

`app.mlc` уже содержит reload status 75 и сохранение window session, но это не полный verified update/handoff protocol. Использовать exit/relaunch UX как отправную точку, добавив отдельные stages:

```text
verified package staging -> compatibility checks -> quiesce
 -> durable actor/effect state -> release ownership -> restart new host
 -> new host namespace/fence -> restore/reconcile -> health confirmation
```

Не hot-unload native libraries с живыми objects/callbacks. Не сохранять GL names/SQLite handles. При overlapping hosts storage/broker fence гарантирует единственного writer. После необратимой schema migration старый executable не является безопасным fallback без compatibility path.

### 12.3. FFI

Использовать текущую традицию MLC logic + тонкий native ABI. Новые syscall/sandbox/clock/wakeup bridges имеют явные quotas, ownership и cancellation semantics. Script не получает произвольный native resolver из reserved config поля.

Pure bounded primitive допустима внутри runtime. Долгая trusted CPU работа — worker. Crash-prone codec/third-party/native plugin — helper process. Native C++ exceptions преобразуются на ABI boundary, memory corruption изолируется процессом. Lua-подобное «вызвать любой C function pointer» здесь не вводится.

<a id="implementation-13"></a>
## 13. План поставки по шагам

Все пункты имеют статус **proposed / не выполнено в рамках этого поручения**. Номера `SVGUI-*` — локальные идентификаторы плана, не подмена шагов 201–300. Перед implementation каждый пункт становится отдельным маленьким Decision → Red → Green → Review шагом в принятом workflow. Не добавлять одновременно compiler и Ruby bootstrap изменения.

### Wave A — основание и безопасный lifetime

| Шаг | Конкретные изменения | Red / приёмка | Зависимость |
|---|---|---|---|
| SVGUI-01 | Зафиксировать baseline для current HEAD+WIP; scripts float/queue200/call-overhead; разобрать blocker 228 в его очереди | Один harness/build profile, не смешивать старые bench txt; gate не повышать | До изменения executor performance |
| SVGUI-02 | `module_blob.mlc` + новый `module_limits.mlc`: checked lengths, counts, bytes/work budgets, scalar constant policy | Negative counts, overflowing widths, huge name, forged HeapRef, truncated input; отказ без crash | Независимо от GUI |
| SVGUI-03 | `module_verify.mlc`: module metadata validation; boundary index для branches при необходимости | Cross-proto/upvalue/arity ошибки, work cap; старые valid modules сохранены | 02 |
| SVGUI-04 | `loaded_module.mlc`, bridge load/get_export: owned immutable bytes, decode-once registry, no blob copy per handle | Caller buffer изменён после load — execution использует принятую копию; failed replacement не меняет старый module | 02–03 |
| SVGUI-05 | `instance.mlc`: heap и root table живут между вызовами; export и value handles различены | Call A возвращает rooted array, call B/GC не уничтожает его; release делает handle stale | 04 |
| SVGUI-06 | `resources`/ABI handles: owner namespace + generation/epoch; exception translation; config v2 size/version | Cross-instance, release/reuse, overflow/exhaustion, old config, allocation failure | 05 |

04 требует изменения bridge и Script registry boundary, но не второго interpreter. Performance claim возможен только по call-overhead measurements до/после. Synchronous v1 ABI сохраняется как compatibility adapter с текущими tests.

### Wave B — приостановка и responsiveness

| Шаг | Конкретные изменения | Red / приёмка | Зависимость |
|---|---|---|---|
| SVGUI-07 | `execution.mlc`, `execution_result.mlc`, split executor по 400-line cap | Создать execution отдельно от run; результат совпадает с old API | 05 |
| SVGUI-08 | `executor_slice.mlc`: materialize registers/PC на Yield, frame transition correctness | Yield после каждой instruction, CALL/RETURN/closure cases; итог и heap совпадают с uninterrupted run | 07 |
| SVGUI-09 | `execution_budget.mlc`: wide count, total vs quantum, cancel token | Миллион yields не обходит total limit; cancel на runnable task; counter boundary | 08 |
| SVGUI-10 | `execution_roots.mlc`: suspended/pending/rooted values в GC roots | Execution A suspended, B allocates/collects; A успешно resume с тем же object graph | 08 |
| SVGUI-11 | Allocation accounting: массивы/строки/closures/arena temporaries; GC slicing отдельными cuts | Один большой array не обходит quota; GC pause измеряется; barrier interleavings | 09–10 |

**Exit Wave B:** можно управлять execution снаружи, вернуть управление между slices и сохранить heap; нет GUI dependence. Ограничения GC latency честно указаны, пока incremental phases не закончены.

### Wave C — первый живой GUI thin slice

| Шаг | Конкретные изменения | Red / приёмка | Зависимость |
|---|---|---|---|
| SVGUI-12 | `script_host/service.mlc` + narrow queue/wakeup ABI: runtime owner thread, bounded nonblocking queues | Full queue возвращает Busy мгновенно; UI не ждёт service; lost-wakeup test | 08–10 |
| SVGUI-13 | `script_host/resources.mlc`: external resource handles поверх WidgetId | Старый namespace/generation и чужой subtree rejected | 12 |
| SVGUI-14 | `script_transaction.mlc`: только SetLabelText, full validate/stage/publish | Второй op ошибочен — первый не виден; revision не меняется | 13 |
| SVGUI-15 | `script_damage.mlc`, `frame_request.mlc`: корректная content invalidation | Text update действительно вызывает layout/paint; no-op/failed batch нет | 14 |
| SVGUI-16 | `script_events.mlc`, общий frame pump в gallery и chat; counter demo | Кнопка → Script → label через настоящий `submit_ui_frame`; infinite loop не блокирует input | 12–15 |

**Exit Wave C:** доверенный script управляет одним UI resource, service можно остановить, frame pipeline не продублирован. Один concern per cut; не объединять изменения frame_request, hover paint и native queue в один performance refactor.

### Wave D — общие imports и actors

| Шаг | Конкретные изменения | Red / приёмка | Зависимость |
|---|---|---|---|
| SVGUI-17 | Module v2 import signatures, `host_imports`, один HostCall opcode и lowering выбранного intrinsic | Unknown/signature mismatch rejected; v1 fixtures ещё работают | 03, 08 |
| SVGUI-18 | Waiting state/request table; timer import → async reply → resume | Reply-before-suspend, duplicate, timeout/cancel/late epoch | 17, 10 |
| SVGUI-19 | `capabilities.mlc`, broker authority: scope/parent/revocation | Cross-principal, revoked parent, queued commit после revoke denied | 13, 18 |
| SVGUI-20 | Actor mailboxes/scheduler: один active turn, fair budgets, bounded admission | Actor flood не увеличивает principal share; no double executor | 18–19 |
| SVGUI-21 | Reactor/worker adapters, subscriptions snapshot+cursor/resync | I/O не на UI; stale worker result не применяется; overflow требует reset | 20 |

Не нужно добавлять native language `await` для реализации Script Waiting: это runtime continuation protocol. Если требуется удобный source syntax, расширение frontend/lowering идёт отдельным compiler track с соответствующим regression gate.

### Wave E — недоверенные расширения

| Шаг | Конкретные изменения | Red / приёмка | Зависимость |
|---|---|---|---|
| SVGUI-22 | Versioned IPC codec + loopback/child transports | Fuzz lengths/nesting/state machine; identity привязана к channel | 19–20 |
| SVGUI-23 | Minimal launcher/sandbox entry, required Linux profile | FS/network/display/FD denial tests; unsupported profile fails closed | 22 |
| SVGUI-24 | Process supervision, fencing, principal quotas, bounded crash loop | Child segfault/hang сохраняет host; old reply/restart не обходят права/квоты | 23 |

**Exit Wave E:** только после этих шагов и отдельного security review допускается заявлять поддержку недоверенного Script bytecode. Host boundary tests выполняются с malicious child, который обходит свой interpreter полностью.

### Wave F — долговечность и обновления

| Шаг | Конкретные изменения | Red / приёмка | Зависимость |
|---|---|---|---|
| SVGUI-25 | Storage owner + SQLite actor schema/revision CAS/fence | Cross-thread handles не используются; disk-full/transaction abort не выдаёт ACK | 20 |
| SVGUI-26 | Durable inbox/outbox и effect status | Crash до/после send/commit; idempotency или OutcomeUnknown | 25 |
| SVGUI-27 | Logical checkpoint/restore, subscriptions и GUI projection rebuild | Handles меняются, logical state сохраняется; revoked rights не воскресают | 26 |
| SVGUI-28 | Idle-turn reload, pure migration, module/code epoch switch | Migration fails → старое state; late effect result не теряется | 27 |
| SVGUI-29 | Verified native update/restart protocol | Не больше одного storage writer; совместимый recovery после сбоя | 27–28 |
| SVGUI-30 | Agent/MCP gateway на subject API | Injection/approval-args mismatch denied; retries через status ledger | 19, 26 |

Wave F может частично развиваться параллельно с sandbox как независимые implementation tracks, но в данном поручении никакие дополнительные агенты или работы не запускались. Это граф технических зависимостей, не команда изменить текущую очередь.

<a id="implementation-14"></a>
## 14. Тесты, измерения и приёмка

### 14.1. Существующие команды

Следующие команды найдены в репозитории. Они перечислены для будущих implementation steps; кроме явно отмеченных в §16 статических проверок, **в ходе документального аудита не запускались**.

```bash
# Инвентаризация tests без сборки
ruby scripts/run_script_vm_gate.rb --list

# Структура модулей
ruby scripts/run_script_vm_file_size.rb
ruby scripts/run_textui_file_size.rb

# Точечные Script VM contracts
ruby scripts/run_script_vm_gate.rb verifier verifier_hardening blob_limits
ruby scripts/run_script_vm_gate.rb instruction_limit heap_limit gc_roots embedding_limits
bash scripts/run_script_vm_embedding_abi_unit.sh
ruby scripts/run_script_vm_handle_host.rb

# Обобщённые gates, lowering и сравнение поведения
ruby scripts/run_script_vm_gate.rb
ruby scripts/run_script_vm_queue200_gate.rb
ruby scripts/run_script_vm_differential.rb

# Раздельные performance harnesses
ruby scripts/run_script_vm_bench.rb
ruby scripts/run_script_vm_float_bench.rb
ruby scripts/run_script_vm_call_overhead.rb

# Реальный GUI/text path
ruby scripts/run_text_gl_perf_corpus.rb
bash scripts/run_textui_hover_bench.sh

# Native services
bash scripts/run_reactor_matrix.sh
bash scripts/run_sqlite_gate.sh all
```

Harnesses часто используют фиксированные `tmp/script_vm_*` каталоги и удаляют их перед сборкой. Не запускать конкурирующие экземпляры, пока им не добавлены отдельные output directories; не выполнять benchmark на фоне чужой тяжёлой сборки. Текущее рабочее дерево содержит WIP, поэтому до fresh performance gate фиксируется именно measured source state.

При изменении `compiler/**` обязательны `scripts/regression_gate.sh` и остальные gates из AGENTS/DEVELOPMENT, включая актуальный build/self-host path. Docs-only шаг от compiler regression освобождён. Для runtime/Script changes выбираются относящиеся gates; не подменять test success наличием бинарника от старой сборки.

### 14.2. Новые тесты по точкам изменения

Имена ниже предлагаемые; они не существуют до соответствующего шага.

| Файл теста | Смысл |
|---|---|
| `script_vm/tests/module_budget_unit.mlc` | Decode byte/work/length limits, overflow input |
| `script_vm/tests/module_constant_policy_unit.mlc` | Reject forged heap reference constants и noncanonical tags |
| `script_vm/tests/persistent_instance_unit.mlc` | Heap/state между calls и независимость instances |
| `script_vm/tests/execution_slice_unit.mlc` | Yield на каждой границе сохраняет точный результат |
| `script_vm/tests/execution_frame_yield_unit.mlc` | CALL/RETURN/frame transition и materialization |
| `script_vm/tests/suspended_roots_unit.mlc` | GC между suspend/resume не теряет object graph |
| `script_vm/tests/byte_budget_unit.mlc` | Array/string/closure allocations не обходят byte cap |
| `script_vm/tests/host_wait_unit.mlc` | Lost wakeup, duplicate completion, cancel/reply race |
| `script_vm/tests/embedding_v2_host.cpp` | ABI size/version, owner thread, root lifetime |
| `script_host/tests/capability_unit.mlc` | Scope/attenuation/revocation/owner |
| `script_host/tests/scheduler_unit.mlc` | Fairness, single ownership, non-reentrant turn |
| `misc/textui/test/script_transaction.mlc` | Batch rejection сохраняет store/state/revision |
| `misc/textui/test/script_frame_damage.mlc` | Label update инвалидирует нужный frame; no-op idle |
| `script_host/tests/sandbox_host.cpp` | Child process isolation и malicious IPC |
| `script_host/tests/state_crash_recovery.mlc` | Transaction/outbox crash points и fencing |
| `script_host/tests/reload_epoch.mlc` | New code не получает old continuation |

Регистрация нового теста в реальном gate обязательна; файл с passing `main` без включения в suite не считается защитой регрессии. Проверки утечек/reuse в C ABI дополняются ASan/UBSan где поддерживается; concurrent boundary — TSan/детерминированные interleavings. Security fuzzing проверяет не только отсутствие crash, но и ограничение памяти/времени.

### 14.3. Performance: что измерять

| Изменение | Метрика | Baseline |
|---|---|---|
| Module cache | Decode/verify count на load/call; trivial_call_nanoseconds | Тот же call-overhead harness до/после |
| Slicing | ns/instruction, ns/call, latency возврата poll | B1/B3 той же серии плюс новые slice counters |
| Persistent heap/GC | Live/arena/root bytes, GC work/pause, allocation scaling | Existing heap fixtures + suspended roots workload |
| Actor scheduler | Queue age, turn latency, fairness, outstanding bytes | 1 actor / many actors / flooding principal |
| TextUI adapter | `layout_us`, `draw_us`, `total_us` или отдельные named CPU counters | Реальная gallery с script off/on |
| Hover | `hover_damage_min_us`, `hover_full_min_us` | Существующий 2560×1440, 80 rows, 10 samples gate |
| Idle/wakeup | CPU idle, empty pump count, lost wakeups | Нет событий / редкий completion / flooding |
| IPC | Bytes/copies, request latency percentiles | Loopback thread vs child transport |
| Persistence | Commit latency, queue lag, unknown outcomes | File-backed SQLite с объявленной sync policy |

Обязательные counters нового execution engine: polls, yielded, waiting, cancelled, executed instructions, materialized registers, full-GC calls/steps, decoded modules и verifier invocations. По ним проверяется, что «decode once» действительно не декодирует на каждый call, а «idle» не выполняет script каждый frame.

Глобальные 1–2 ms slices и 60 Hz из родительского документа — исходные гипотезы, не выполненные MLC SLO. До измерения устанавливать только structural guarantees: bounded work count и отсутствие blocking waits на UI. Hard wall-clock bound для GC/native calls не заявлять без отдельной реализации.

### 14.4. Gate первого GUI результата

1. Native button event доходит до runtime owner, а ответ меняет label через batch.
2. Все GL calls остаются на существующем owner; script не видит GL names/WidgetId indices.
3. Infinite loop yield-ится, cancel останавливает task; native input и закрытие окна продолжают работать.
4. Full queue возвращает Busy без ожидания UI; принятый request имеет completion/status.
5. Invalid handle/revision/capability отклоняет весь batch.
6. Text change вызывает layout/paint, no-op не приводит к frame storm.
7. Gallery и chat обслуживают общий completion path.
8. File-size/text/hover gates проходят; если performance не сдвинулся, не заявлять ускорение.
9. Demo помечен trusted-only до Wave E; native crash isolation не обещается.

### 14.5. Gate production profile

К предыдущему списку добавляются malicious process tests, byte/work quotas всех boundary, GC roots всех suspended tasks, crash recovery state/outbox, code/session fencing, denied X11/Wayland/GPU/FS/network access, secure update chain, audit/redaction и approved MCP scope tests. Полный список инвариантов I01–I20 родительского документа становится traceability matrix к тестам, а не декларацией «всё есть».

<a id="implementation-15"></a>
## 15. Первые изменения и отложенные решения

### 15.1. Самый первый implementation cut

Рекомендуется начать с **SVGUI-02: безопасные module size/length budgets**, не с исполнения script из `app.mlc`.

Конкретный маленький cut:

1. Зафиксировать tests для overflowing `word_count`, отрицательных count/index и blob больше configured limit.
2. Добавить `ModuleLimits` с совместимым default для trusted tests; проверять bytes до копирования из C ABI.
3. Заменить один класс overflow-prone length checks на безопасный cursor helper, не менять ISA/float semantics.
4. Прогнать blob/verifier/embedding tests; сохранить failure-preserves-old-module contract.
5. Отдельно добавить raw constant policy и module verifier, чтобы не смешивать parser arithmetic и value representation в одном Green.

Почему это раньше persistent engine: новый loaded-module cache будет полагаться на корректность единственной проверки. Сначала сделать её bounded и immutable, затем убрать повторные проверки по trusted internal path.

Текущий performance blocker 228 обрабатывается отдельно по действующей очереди. Hardening cut не должен объявлять его исправленным и не должен менять float benchmark budget.

### 15.2. Что пока не делать

- Не переносить `misc/textui/` целиком в script profile.
- Не менять MIR VM и Ruby bootstrap ради runtime service.
- Не вводить JIT/quickening, пока module lifetime/slicing/security boundary не зафиксированы.
- Не переписывать renderer и не создавать widgets на каждый list row.
- Не переносить GL в thread одновременно с добавлением scripts.
- Не превращать `app.mlc`, `chat_host.mlc` или `embedding_abi.cpp` в универсальный менеджер actors/БД/IPC.
- Не использовать raw `Value`/`WidgetId`/SQLite handle как внешний RPC API.
- Не называть REPL declarations persistent application state.
- Не обещать exactly-once внешних действий без receiver idempotency/status support.
- Не сохранять native process stack как первый checkpoint format.

### 15.3. Зафиксированные адаптации общей архитектуры

| Общая идея | Конкретный выбор для MLC |
|---|---|
| Memory-safe bytecode | Развить текущий structural verifier + checked runtime, не писать новую статическую ISA |
| Actor heap | Persistent `ScriptInstance` на текущих Heap/VmState |
| Async continuations | Materialized register frames, не native coroutine snapshots |
| Thread pool actors | Сначала один service thread, затем scheduler workers; не native Isolate на actor |
| Transactional GUI | Adapter вокруг retained `WidgetStore`, staged patches и explicit damage |
| Resource identity | External resource map поверх `WidgetId.index` |
| Rendering | Существующий shared submit + hover damage, один GL owner |
| Persistence | Существующая SQLite ABI, новая actor schema и storage owner |
| Hot reload | Idle-turn logical migration; старые handle blobs не считать reload protocol |
| Process sandbox | Новый transport/launcher поверх общего Host API |
| Agent tools | Тот же subject broker, не privileged обход через chat |

<a id="implementation-16"></a>
## 16. Доказательства и проверки документа

### 16.1. Метод аудита

Прочитаны AGENTS, continuity/development instructions, spec index, относящиеся Script VM/TextUI материалы и текущие implementation files. Проверены call paths loader → embedding → executor, GC roots/collection, native queues/reactor/SQLite ownership, TextUI update/render и existing gate definitions. Поиск отсутствующих подсистем ограничен `script_vm`, relevant runtime и TextUI; «не найдено» означает отсутствие связанного пути в изученной области, не формальное доказательство отсутствия любого одноимённого файла во всём проекте.

Существующие WIP не редактировались и не откатывались. Full build, GUI bench и functional suites не выполнялись: задача документальная, а shared harnesses используют фиксированные build directories. Ни одного нового результата скорости или утверждения «реализация прошла тесты» этот документ не делает.

### 16.2. Выполненные read-only проверки

| Проверка | Результат |
|---|---|
| `git status`, HEAD, recent log | Зафиксирована база и WIP; индекс исходно пуст |
| `ruby scripts/run_script_vm_file_size.rb` | `script_vm file size: ok` |
| `ruby scripts/run_textui_file_size.rb` | `textui file size: ok (kit max 400, misc/textui/main.mlc freeze 637)` |
| `ruby scripts/run_script_vm_gate.rb --list` | Registry tests читается без запуска сборки |
| Сравнение исходного архитектурного файла с копией в docs | Проверяется побайтово при выдаче |
| Markdown internal links / evidence paths / code fences | Проверяется при выдаче |

### 16.3. Навигация по исходникам

Ссылки ниже ведут к реальным файлам относительно `docs/`; имена символов устойчивее line numbers. Номера строк фиксируют прочитанное рабочее дерево и могут измениться после следующих коммитов. Дальнейшие implementation шаги должны перепроверить эти точки, особенно файлы с WIP.

| Файл | Точка / строка на момент аудита | SHA-256 prefix содержимого |
|---|---|---|
| [script_vm/value.mlc](../script_vm/value.mlc) | `export type Value =` — строка 5 | `53523dd59ae44502` |
| [script_vm/module_blob.mlc](../script_vm/module_blob.mlc) | `export fn decode_module` — строка 229 | `7b1d08c62a440cee` |
| [script_vm/verifier.mlc](../script_vm/verifier.mlc) | `export fn verify_function` — строка 232 | `0ecb90a1cf984122` |
| [script_vm/embedding.mlc](../script_vm/embedding.mlc) | `export fn emb_call_export_bytes_limited` — строка 123 | `a4ef9231f027dfdb` |
| [script_vm/executor.mlc](../script_vm/executor.mlc) | `fn execute_current_frame` — строка 200 | `0ba58089af14e65d` |
| [script_vm/vm_state.mlc](../script_vm/vm_state.mlc) | `export type VmState` — строка 3 | `b0c371919727c672` |
| [script_vm/run_result.mlc](../script_vm/run_result.mlc) | `export type RunLimits` — строка 27 | `db8ad4e1d1158a0c` |
| [script_vm/executor_calls.mlc](../script_vm/executor_calls.mlc) | `export fn executor_max_frame_depth` — строка 39 | `81b401a38648c42a` |
| [script_vm/heap_collect.mlc](../script_vm/heap_collect.mlc) | `export fn heap_in_place_collect` — строка 253 | `b541dae16e4cfbc7` |
| [script_vm/executor_gc.mlc](../script_vm/executor_gc.mlc) | `export fn executor_collect_if_needed` — строка 52 | `9583cf2a4562890e` |
| [script_vm/repl_session.mlc](../script_vm/repl_session.mlc) | `export type ReplSession` — строка 14 | `701bebe1d53e16e7` |
| [script_vm/lower/lower_module.mlc](../script_vm/lower/lower_module.mlc) | `import { tokenize }` — строка 1 | `81588cffbcaa8ebe` |
| [runtime/include/mlc/script_vm/embedding_abi.h](../runtime/include/mlc/script_vm/embedding_abi.h) | `typedef struct MlcVmConfig` — строка 41 | `e431f5bf762e0799` |
| [runtime/src/script_vm/embedding_abi.cpp](../runtime/src/script_vm/embedding_abi.cpp) | `struct EmbeddingVmState` — строка 23 | `c1cd908fe46ad446` |
| [runtime/include/mlc/concurrency/isolate.hpp](../runtime/include/mlc/concurrency/isolate.hpp) | `class Isolate` — строка 21 | `e64772941e1905f5` |
| [runtime/include/mlc/concurrency/thread_pool.hpp](../runtime/include/mlc/concurrency/thread_pool.hpp) | `class ThreadPool` — строка 21 | `921696ff12251251` |
| [runtime/include/mlc/reactor/event_loop.hpp](../runtime/include/mlc/reactor/event_loop.hpp) | `class EventLoop` — строка 32 | `e826edbf5ccba72f` |
| [runtime/include/mlc/db/sqlite_abi.hpp](../runtime/include/mlc/db/sqlite_abi.hpp) | `inline Table& table()` — строка 79 | `1c4f534762808374` |
| [misc/textui/store.mlc](../misc/textui/store.mlc) | `export type WidgetId` — строка 1 | `3bd84b40a5f7ef5f` |
| [misc/textui/app.mlc](../misc/textui/app.mlc) | `fn main()` — строка 75 | `1468b7371b80f56c` |
| [misc/textui/frame_request.mlc](../misc/textui/frame_request.mlc) | `export type FrameSignature` — строка 10 | `5ce0c2a26690f9ce` |
| [misc/textui/hover_damage.mlc](../misc/textui/hover_damage.mlc) | `export fn submit_ui_frame` — строка 351 | `27a59815866ced75` |
| [misc/textui/host_input.mlc](../misc/textui/host_input.mlc) | `export fn host_wait_for_input` — строка 56 | `d73a0dfe9fcb72fd` |
| [misc/textui/chat_host.mlc](../misc/textui/chat_host.mlc) | `export fn chat_frames` — строка 89 | `2c9e5e71bd3b794c` |
| [misc/textui/directory_jail.mlc](../misc/textui/directory_jail.mlc) | `export fn path_inside_jail` — строка 51 | `f4ae43afd0ca4ee5` |
| [misc/textui/chat_transport.mlc](../misc/textui/chat_transport.mlc) | `export fn transport_start` — строка 43 | `473d5ad94defefbb` |
| [misc/textui/chat_persistence.mlc](../misc/textui/chat_persistence.mlc) | `export type ChatPersistence` — строка 35 | `d37630615028e190` |
| [scripts/run_script_vm_float_bench.rb](../scripts/run_script_vm_float_bench.rb) | `baseline_nanoseconds_per_instruction` — строка 7 | `b7d26be30765b2f0` |
| [docs/agent/SCRIPT_VM_QUEUE_300_STATE.md](agent/SCRIPT_VM_QUEUE_300_STATE.md) | `current_step:` — строка 1 | `5b4af57debf4aaa3` |
