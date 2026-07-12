# MLC Concurrency v2 — технические требования

Источник: внешний research-запрос пользователя (2026-07-09), сверен с текущим
кодом. Статус: **design, не реализовано** (кроме того, что явно отмечено как
"уже есть").

## North star

> На MLC должно быть возможно писать месяцами живущие серверные приложения с
> несколькими ядрами, тысячами соединений, контролируемой перегрузкой,
> воспроизводимым shutdown и без случайного shared mutable state.

Эталонный сценарий (не проект, а критерий приёмки): чат-сервер — WebSocket
соединения, общий поток событий, БД, медленные клиенты, фоновые workers, rate
limiting, graceful shutdown.

Заимствования (осознанно, не копируем целиком ни одну модель):
- **Rust** — различение "можно перенести между потоками" (`Send`) и "можно
  безопасно разделять между потоками" (`Sync`, здесь ниже называется `Shared`
  — см. ⚠️ в §2 про коллизию имени).
- **Swift** — изоляция mutable state и структурированный жизненный цикл задач
  (actor isolation, structured concurrency — `TaskScope`).
- **Erlang/OTP** — supervision как отдельный уровень отказоустойчивости
  (`Supervisor`, restart policies), НЕ actor model целиком.
- **C++20 backend** — `std::jthread`, `stop_source`/`stop_token`, atomics как
  **внутренний backend mechanism**, не экспортируется в MLC API напрямую.

## 0. Что уже есть (TRACK_CONCURRENCY, closed, MVP) — проверено по коду

| Компонент | Файл | Реальное поведение |
|-----------|------|---------------------|
| `Channel<T>` | `runtime/include/mlc/concurrency/channel.hpp` | bounded (default 64, max 1048576), **capacity 0 запрещён** (кидает `invalid_argument`) — rendezvous-варианта нет |
| `spawn { ... }` | `runtime/include/mlc/concurrency/spawn.hpp` | `std::async` + `future.get()` через `mlc::Task` coroutine; **нет TaskScope, нет explicit owner beyond Task handle, нет cancellation** |
| `Arc<T>` | `runtime/include/mlc/concurrency/arc.hpp` | atomic refcount, требует "send-safe" inner |
| `Mutex<T>` | `runtime/include/mlc/concurrency/mutex.hpp` | scoped `mutex.lock(fn mut val => ...)`, lambda-only — уже соответствует §13 требования "lexical API, не lock()/unlock()" |
| "Send-safe" check | `compiler/checker/send_safe.mlc` (`type_is_send_safe`) | компоновочный (compositional) предикат, **уже структурно решает Rust-style Send inference**, но: (a) используется только в `Channel.send`, не как общий bound; (b) конфлирует Send и Shared в одно понятие — `Arc<T>` в текущем предикате `false` (не send-safe), хотя по предложению `Arc<ImmutableConfig>` должен быть и `Send`, и `Shared` |

Замыкания в MLC **всегда** захватывают по значению (`MEMORY_MODEL.md` §Замыкания,
`[=]` не `[&]`) — это уже снимает часть проблемы "случайный `[&]` capture в
thread closure" из исходного текста (в C++ такое возможно, в MLC синтаксически
недостижимо). Но COW-буфер `Array`/`HashMap` расшаривается через `shared_ptr`
даже при capture-by-value, а COW detach **не атомарен**
(`MEMORY_MODEL.md` §Известные ограничения, п.2) — то есть ровно тот data race,
от которого предостерегает §1 ниже, **уже документирован как known limitation**
и уже воспроизводим сегодня через `spawn { array.push(x) }` из двух потоков на
одной и той же COW-расшаренной переменной.

## ⚠️ Открытый вопрос: коллизия имени `Shared`

Источник предлагает трейт `Shared[T]` ("можно одновременно использовать из
нескольких потоков"). В MLC уже существует **тип** `Shared<T>` (`std::shared_ptr`,
однопоточное дерево владения, не thread-safe, см. `MEMORY_MODEL.md` §Shared/Weak).
Трейт `T: Shared` и тип `Shared<Node>` в одном языке — гарантированная путаница
(`Shared<Node>: Shared`? нет — `Shared<T>` сам по себе `!Shared`-трейт, т.к.
refcount неатомарен).

Варианты (нужно решить до реализации, Planner):
1. Назвать трейт `Sync` (как в Rust) — без коллизии, но менее "очевидно" для
   новых пользователей языка (аргумент источника против этого имени).
2. Переименовать текущий тип `Shared<T>` → `Rc<T>` (источник сам предлагает
   именно `Rc<T>` для этой роли в §38), освободив имя `Shared` под трейт —
   **breaking rename**, затронет весь `compiler/` и `lib/mlc/` (текущий
   self-hosted компилятор сам активно использует `Shared<T>` для AST/SemanticIR
   узлов).
3. Оставить трейт без явного имени в MVP, добавить синтаксис позже.

Рекомендация: **вариант 1 (`Sync`)** — нулевая стоимость миграции, соответствует
уже привычной Rust-терминологии, которую источник и так использует как основу.

## 1. Главный инвариант: mutable значение не shared по умолчанию

Любая граница параллельного исполнения обязана быть видима checker'у:

```text
spawn_thread / spawn
TaskScope.spawn
ThreadPool.submit
Channel.send
Isolate.send
parallel_map
```

Захват mutable значения (не через `move`/`Arc`/`Channel`) в замыкание,
пересекающую такую границу, — ошибка компиляции, а не UB в рантайме (сегодня —
тихий data race через COW, см. §0).

## 2. Типовой признак `Send[T]`

> Значение типа `T` можно безопасно передать во владение другому потоку.

Компоновочный вывод (как `type_is_send_safe`, но как настоящий bound, а не
только для `Channel.send`):

```text
i32/i64/f64/bool/unit         Send
String                         Send
Tuple[A, B]                    Send iff A и B Send
Array[T] (перенос целиком)     Send iff T Send
Rc[T] (было Shared<T>)         !Send
Arc[T]                         Send iff T Send/Sync
RawPtr[T]                      !Send
```

Не строим полный Rust borrow checker. Достаточно 4 категорий: owned / immutable
/ local mutable / explicitly shared.

## 3. Типовой признак `Sync[T]` (было предложено как `Shared`, см. ⚠️ выше)

> Ссылку на `T` можно одновременно использовать из нескольких потоков.

```text
Atomic[T]              Sync
Mutex[T]                Sync
Arc[ImmutableConfig]    Sync
HashMap[K, V]           !Sync
Array[T] (mutable)      !Sync
RefCell[T]              !Sync
```

`Send` = можно передать. `Sync` = можно разделять одновременно. Разные оси,
композиция обеих даёт полную картину (как в Rust).

## 4. Проверка capture в closure границ параллелизма

```mlc
spawn_thread(move job) {
  process(job)
}
```

После `move` — `use(job)` снаружи запрещён (`error: value 'job' was moved into
spawned thread`). Immutable shared — либо явный `spawn_thread(shared config)`,
либо checker сам видит `Arc[Config]: Sync` и разрешает без ключевого слова.

Запрещённый случай (сегодня — тихий баг, должен стать ошибкой компиляции):

```mlc
let buffer = ByteBuffer()
spawn_thread { buffer.append(x) }   // error: mutable value crosses concurrency boundary
```

## 5. Detached threads — только `unsafe`/runtime internals/FFI

Обычный код обязан иметь handle:

```mlc
let thread = Thread.spawn { work() }
thread.join()?
```

или structured scope (§6). Не `Thread.spawn { forever() }` без handle.

## 6. Structured concurrency — `TaskScope`

```mlc
scope |s| {
  let a = s.spawn { load_a() }
  let b = s.spawn { load_b() }
  combine(a.join()?, b.join()?)
}
```

Инвариант: `scope` не завершается, пока все children не завершены либо
отменены и присоединены. `Task` не переживает свой `TaskScope`, кроме явного
`BackgroundService.start(...)` с собственным owner.

## 7. Ошибка child task не может потеряться

По умолчанию:

```text
child failure → cancel siblings → wait for children → propagate error to parent
```

```mlc
scope |s| {
  s.spawn { a()? }
  s.spawn { b()? }
}?
```

Политики (allow-all-fail, restart-on-fail, ...) — после MVP.

## 8. Cancellation — с первого дня, не после async/await

```mlc
let stop = StopSource.new()
let thread = Thread.spawn(stop.token) |token| {
  while !token.requested { work_once() }
}
stop.request()
thread.join()?
```

Backend: C++20 `stop_source`/`stop_token` (не экспортировать наружу как API).
В structured code token распространяется автоматически через `scope`/`Task`.

## 9. Cancellation обязана будить блокирующие операции

`Channel.recv`, `Channel.send`, `sleep`, `Timer.wait`, `Task.join`,
`Socket.read`, `Socket.accept` — все обязаны просыпаться на cancel и возвращать
`Cancelled`, а не молча висеть:

```mlc
match channel.recv() {
  Ok(value) => ...
  Closed    => ...
  Cancelled => ...
}
```

Не `nullable nil` — три различных исхода.

## 10. Bounded channel — центральный primitive модели

```mlc
let jobs = Channel[Job].bounded(256)
jobs.send(job)?
let job = jobs.recv()?
jobs.close()
```

Варианты:

| Вид | API | Статус сегодня |
|-----|-----|-----------------|
| Bounded (основной) | `Channel[T].bounded(256)` | есть (`channel.hpp`, default 64) |
| Rendezvous | `Channel[T].bounded(0)` | **нет** — сегодня `capacity == 0` кидает исключение, надо разрешить как синхронный handoff |
| Unbounded | `Channel[T].unbounded()` | нет, явный "не рекомендуется для server ingress" API |

Без backpressure сервер нестабилен по определению
(`producer 100k msg/s > consumer 20k msg/s → unbounded queue → OOM`).

## 11. Семантика закрытия channel

Нужно явно определить (сегодня не определено на уровне MLC-API): владение
`Sender`, множественность senders/receivers, что происходит при close с
buffered/blocked операциями.

```mlc
let channel = Channel[Job].bounded(256)
let tx = channel.sender()
let rx = channel.receiver()
let tx2 = tx.clone()
```

Последний `Sender` уничтожен → receiver дочитывает buffer → получает `Closed`.
Явный `tx.close()` будит blocked receivers немедленно.

## 12. MPMC для MVP, не только SPSC

Thread pool = many producers → job queue → many workers. Реализация — mutex +
condition variable **сначала**, lock-free — не начинать с него, добавить позже
под профилированием (текущий `channel.hpp` уже на mutex+condvar — верное
направление, не менять на lock-free без профиля).

## 13. Shared mutable state — только через explicit wrappers

`Mutex[T]`/`RwLock[T]`, lexical API (`state.with_lock |s| { ... }`) —
`Mutex<T>.lock(lambda)` уже реализован именно так (см. §0). `RwLock[T]` — нет,
надо добавить.

## 14. Atomics — без memory ordering в v1

`Atomic[Bool/Int32/Int64/UInt64]`: `load/store/exchange/compare_exchange/
fetch_add/fetch_sub`. Default = sequentially consistent. `.acquire`/`.release`
— не в первой версии, отдельная большая поверхность ошибок.

## 15. Mutable globals — запрещены по умолчанию

```mlc
var users = HashMap[String, User]()   // module-level — error по умолчанию
shared var users = Mutex.new(...)     // явно разрешённая форма
```

## 16. `Isolate[State, Message]`

Второй по ценности high-level primitive после channels:

```mlc
let room = Isolate[RoomState, RoomMsg].start(RoomState.new()) |state, msg| {
  match msg {
    .Join(user, client) => state.clients.put(user, client)
    .Leave(user)         => state.clients.remove(user)
    .Post(user, text)    => state.append_message(user, text)
  }
}
room.send(.Post(user, text))?
```

Гарантия: `RoomState` никогда не обрабатывается двумя workers одновременно.
Не полный actor model — state + bounded mailbox + serial handler.

## 17. `Isolate` mailbox обязан быть bounded

```mlc
Isolate.start(capacity: 1024, ...)
```

Overflow policy явная: `Block | DropNewest | DropOldest | Reject`.

## 18. `ThreadPool`

```mlc
let pool = ThreadPool.new(workers: 8, queue: 1024)
let handle = pool.submit(move job) { process(job) }
```

Гарантии: bounded job queue, controlled shutdown, no silent task loss, error
и panic observable. Work stealing — не в первой версии, fixed-size pool.

## 19. CPU vs blocking tasks — различать архитектурно с самого начала

```mlc
runtime.spawn_cpu { ... }
runtime.spawn_blocking { ... }
```

8 workers, все делают blocking DB read → CPU jobs умирают без этого разделения.

## 20. Не начинать с async/await

Порядок: `Thread → Send/Sync → Mutex → Atomics → BoundedChannel → TaskScope →
Cancellation → ThreadPool → Isolate`, и только потом `Future/async/await/
IoReactor/select`. async/await без cancellation/ownership/backpressure/task
lifetime/shutdown — красивый синтаксис поверх хаоса.

## 21. Task ≠ OS thread — предусмотреть границу заранее

```text
Thread   OS thread (низкий уровень)
Task     логическая concurrent-операция (может исполняться на pool)
```

Первая реализация `Task` может использовать `ThreadPool` внутри — важно не
завязывать API пользователя на "1 Task = 1 OS thread" с самого начала, иначе
переход на будущий scheduler невозможен без breaking change.

## 22. `select` — позже, сначала библиотечно

```mlc
Selector.new().recv(messages).recv(control).timeout(5.sec).wait()
```

Синтаксис `select { ... }` — после того, как библиотечная форма показала себя
достаточной для server-приложений.

## 23. Timers и monotonic clock

`Instant.now()`, `Duration`, `sleep`, `deadline`, `timeout`, `interval` — только
monotonic clock (не wall clock — системное время может двигаться назад).

## 24. Panic/fatal failure через thread boundary — контролируемо

Сгенерированный C++ не может допускать `exception escapes thread function →
std::terminate`. На уровне MLC — `TaskFailure { kind, message, stack }`,
видимый parent'у. Разделять `Result error / Panic / Process crash /
Cancellation` — не сваливать всё в один `Error`.

## 25. `defer` — обязателен для concurrency, не косметика

```mlc
let lock = mutex.lock()?
defer lock.release()
```

Предпочтительно lexical guards (`with_lock`), но `defer` нужен и как fallback.

## 26. FFI concurrency-контракт

Каждый `extern c` type/fn — метаданные: `thread_safe | sendable | blocking |
callback_concurrent | thread_affine`.

```mlc
extern c fn sqlite_step(...) blocking !thread_safe
extern c type GtkWidget !Send thread_affine(MainThread)
```

Без этого один FFI handle обесценивает всю checker-модель (особенно критично
для GUI/DB-биндингов).

## 27. Thread affinity

```mlc
fn update(widget: GtkWidget) { ... }
```

Вызов из worker-потока → `error: GtkWidget is bound to MainThread`.

## 28. `Supervisor` — после Isolate, не раньше

**Статус (2026-07-12):** C++ v1 **implemented** —
`runtime/include/mlc/concurrency/supervisor.hpp` (`mlc::concurrency::Supervisor`);
smoke `runtime/test/test_supervisor.cpp` + `run_concurrency_smoke.sh`.
MLC surface **deferred** (same closure gap as JobQueue; see
[TRACK_CONCURRENCY_SUPERVISOR](archive/tracks/TRACK_CONCURRENCY_SUPERVISOR.md) Decision).
Sketch below is the long-term MLC shape, not shipped syntax.

```mlc
supervisor {
  child("gateway", restart: .permanent) { run_gateway() }
  child("room", restart: .permanent) { run_room() }
  child("notifications", restart: .transient) { run_notifications() }
}
```

Политики: `permanent | transient | temporary` — в C++: `RestartPolicy`.
Стратегии: v1 = `one_for_one` only; `one_for_all`/`rest_for_one` — позже.
Prerequisite (channels/TaskScope/cancellation) met before this track.

## 29. Restart storm protection

**Статус (2026-07-12):** C++ v1 **implemented** —
`Supervisor::set_restart_intensity(max, within)`; exceeding the rolling window
sets `storm_tripped()` and requests supervisor stop. Exponential **backoff**
deferred. MLC sketch below is not shipped.

```mlc
restart: max: 5, within: 30.sec, backoff: exponential
```
## 30. Shutdown — отдельная спецификация (будущий документ)

`docs/concurrency/shutdown.md` (создать при подходе к Фазе 9):

```text
1. stop accepting new work
2. reject new external requests
3. request cancellation
4. close producer channels
5. drain accepted work where policy permits
6. stop workers
7. flush persistence
8. join children
9. close resources
10. exit
```

Тестируемо, не `SIGTERM → exit(0)`.

## 31. Референсная архитектура (иллюстративный пример, не проект)

```text
WebSocket connections → Network Gateway → bounded channel → GlobalRoom Isolate
  → Persistence Channel → DB Workers
  → Moderation Channel  → Mod Workers
  → Fanout Channel      → Send Workers
```

`GlobalRoomState` принадлежит одному isolate, не shared. Сетевые workers шлют
только `room.send(.Post(...))`.

## 32. Slow consumer — часть модели, не edge case

Каждому connection — bounded outbound queue (например capacity 256). При
overflow: `disconnect slow client` либо явная drop-политика. Обязательный
stress-кейс, не опциональный.

## 33. Fairness — не катастрофическая, не идеальная

Per-source quotas, bounded ingress queues, batch limits, yield points — чтобы
один producer не мог полностью вытеснить остальных.

## 34. Deterministic `TestRuntime`

Развёрнутый дизайн (deterministic scheduler, полная stress-матрица, sanitizer
CI gate) — [CONCURRENCY_TEST_HARNESS.md](CONCURRENCY_TEST_HARNESS.md);
трек: [archive/tracks/TRACK_CONCURRENCY_TEST_HARNESS.md](archive/tracks/TRACK_CONCURRENCY_TEST_HARNESS.md)
(**closed** 2026-07-12, awaiting Critic).

**Статус (2026-07-12):** C++ v1 **implemented** —
`runtime/include/mlc/concurrency/testing/scheduler.hpp` (`TestScheduler`) +
`TestMutex`/`TestChannel`; smoke + sanitize + nightly seed fuzz
(`scripts/concurrency_fuzz_gate.sh`). MLC `TestRuntime.new(seed:)` **deferred**
(same closure gap as JobQueue/Supervisor; see track Decision T7). Sketch below
is long-term MLC shape, not shipped syntax.

```mlc
let rt = TestRuntime.new(seed: 918271)
rt.spawn { actor_a() }
rt.spawn { actor_b() }
rt.run()
```

При падении — `seed` + `schedule` для воспроизведения. Не обязательно полный
model checker на старте — даже собственный deterministic scheduler для
`Task`/`Channel` тестов даёт огромную пользу против "прошёл 999 раз, упал один
раз ночью".

## 35. Обязательные stress suites на каждый primitive

`Channel`: single/many sender-receiver, full/empty queue, close during
send/recv, cancel during send/recv, sender/receiver destruction, rapid
open/close, 1M messages. `Mutex`: high contention, panic while guarded,
cancellation while waiting. `Scope`: child success/error/panic, parent
cancellation, nested scopes, simultaneous sibling errors.

## 36. Sanitizers — first-class часть `mlcc`

```bash
mlcc test --sanitize=thread
mlcc test --sanitize=address
mlcc test --sanitize=undefined
```

CI матрица: `normal-debug | release | asan | ubsan | tsan`. Сегодня в проекте
уже есть `MLC_TSAN=1 runtime/test/run_concurrency_smoke.sh` (TRACK_CONCURRENCY
STEP=7) — нужно поднять до отдельной `mlcc test --sanitize=*` команды и
включить в постоянный CI-矢trix, не только ручной smoke.

## 37. Clang Thread Safety Analysis в generated C++ (второй эшелон)

Генерировать `-Wthread-safety` annotations для `Mutex[T]`/guarded fields —
второй слой проверки поверх MLC checker + TSan runtime. Не для MVP.

## 38. Категории владения — согласовать с текущими именами

| Источник предлагает | В MLC сегодня | Решение |
|----------------------|----------------|---------|
| `Owned[T]` | обычное value/move | нет отдельного типа — уже есть по умолчанию через move-семантику |
| `Arc[T]` | `Arc<T>` (TRACK_CONCURRENCY STEP=5) | совпадает, ничего менять не надо |
| `Rc[T]` | `Shared<T>` (`std::shared_ptr`, не thread-safe) | см. ⚠️ выше — переименование или трейт `Sync` без переименования типа |

## 39. Concurrency-семантика для `String`/`Array`/`HashMap`

```text
String (immutable):     Send, Sync
Array[T]:                Send iff T: Send; !Sync while shared/mutable (COW risk — см. §0)
HashMap[K,V]:            Send iff K,V: Send; !Sync while shared/mutable
```

Переносить контейнер (move) — можно. Одновременно мутировать shared COW-копию
из двух потоков — нет, и это должно стать **compile error**, а не runtime UB
(сегодня — runtime UB, см. §0 "known limitation" уже в `MEMORY_MODEL.md`).

## 40. Memory visibility / happens-before — записать в спеку

```text
Channel.send(x) happens-before успешный Channel.recv() → x
Mutex unlock happens-before следующий успешный lock
Thread completion happens-before join() returns
```

## 41. Concurrency vs parallelism — явная граница

`Task/Channel/Isolate` не обещают отдельное CPU core. `Thread/ThreadPool/
parallel_map` могут реально исполняться параллельно. Важно для будущего async
runtime — не путать эти два понятия в документации/API.

## 42. `parallel_map` — поверх той же системы, позже

```mlc
let results = items.parallel_map(workers: 8) |item| { process(item) }
```

Только если closure и output — `Send`. Никакой отдельной магической модели.

## 43. Рекомендуемый публичный API v1

```text
Thread, JoinHandle[T]
Send, Sync (было "Shared" — см. ⚠️)
Mutex[T], RwLock[T]
AtomicBool, AtomicI64, AtomicU64
Channel[T], Sender[T], Receiver[T]
StopSource, StopToken
Task, TaskHandle[T], TaskScope
ThreadPool
Instant, Duration, Timer
```

После v1: `Isolate[State, Msg]` (**done** C++), `Supervisor` (**done** C++ v1;
MLC deferred), `Select`. После них: `Future`,
`async/await`, `IoReactor`, `AsyncSocket`.

## 44. Порядок реализации (маппинг на текущий pipeline `Lexer → Parser → AST →
Checker → SemanticIR → C++20`)

| Фаза | Содержание | Изменения языка | Готовность сегодня |
|------|------------|-------------------|----------------------|
| 1 | `Thread`/`Mutex`/`Atomics`/`BoundedChannel`/`StopToken` как runtime-библиотека | нет | Channel/Mutex/Arc частично есть; Atomics/StopToken — нет |
| 2 | `spawn_thread { }` + `.join()?`, conservative checker (mutable capture forbidden) | минимальный parser/AST/SemanticIR | `spawn` есть, capture-check — нет |
| 3 | Типовое свойство `Send` (компоновочный inference) | checker | `type_is_send_safe` есть, но привязан только к `Channel.send` — нужно обобщить в bound |
| 4 | `spawn_thread(move job) { ... }` — move-tracking | простой move-state tracking, не полный borrow checker | нет |
| 5 | `Sync` (см. ⚠️) | checker | нет |
| 6 | `TaskScope` (`scope \|s\| { s.spawn { ... } }`) | новый construct | нет |
| 7 | Cancellation propagation (scope failure/cancel) | runtime + checker | нет |
| 8 | `Isolate[State, Msg]` (сначала библиотечно) | нет (library) | нет |
| 9 | **Стоп. Не добавлять фичи, пока эталонное приложение (чат, см. north star) не выдержит:** 1000 connections, 100k messages, slow clients, random disconnect, SIGTERM, DB delays, worker crashes, TSan | — | — |
| 10 | `Supervisor` | library | **есть** C++ v1 (2026-07-12): `supervisor.hpp`; policies + one_for_one + storm intensity; MLC deferred ([TRACK_CONCURRENCY_SUPERVISOR](archive/tracks/TRACK_CONCURRENCY_SUPERVISOR.md) **closed**) |
| 11 | async I/O (`Future`/`async`/`await`/`IoReactor`) | язык + runtime | нет |

## Критерий приёмки ("MLC умеет стабильную многопоточность")

Не считается выполненным на основании того, что работает
`Thread.spawn { print("hello") }`. Все пункты одновременно:

1. Mutable значение нельзя случайно разделить между threads.
2. Передача ownership проверяется checker'ом.
3. Есть `Send`/`Sync`-подобные свойства типов.
4. Есть bounded MPMC channels.
5. `Channel` close/cancel семантика полностью определена.
6. Child tasks имеют owner.
7. Нет случайно потерянных detached tasks.
8. Cancellation распространяется вниз.
9. Blocking waits просыпаются при cancellation.
10. Ошибка worker не теряется.
11. Shutdown детерминирован.
12. Slow consumer не может съесть бесконечную память.
13. Есть deterministic concurrency tests.
14. Есть stress tests.
15. Generated C++ проходит TSan.
16. FFI types имеют concurrency metadata.
17. Runtime выдерживает реальное многопоточное приложение (эталонный чат).

## Итоговая формула архитектуры

**Owned mutable state + explicit transfer + bounded message passing +
structured lifetime + isolated state machines.**

```mlc
let jobs = Channel[Job].bounded(256)

scope |s| {
  for _ in 0..<8 {
    let receiver = jobs.receiver()
    s.spawn(move receiver) {
      while let Some(job) = receiver.recv()? {
        process(job)?
      }
      Ok(())
    }
  }
  for job in load_jobs()? {
    jobs.send(move job)?
  }
  jobs.close()
}
```

Не:

```mlc
let global_mutex = Mutex.new()
let global_state = ...
spawn 50 threads {
  global_mutex.lock()
  mutate_everything()
}
```

## Ссылки

- Текущая MVP-реализация (closed): [archive/tracks/TRACK_CONCURRENCY.md](archive/tracks/TRACK_CONCURRENCY.md)
- Модель памяти: [MEMORY_MODEL.md](MEMORY_MODEL.md) §Известные ограничения п.2
- Реализация: `runtime/include/mlc/concurrency/`, `compiler/checker/send_safe.mlc`
- Дорожная карта: [PLAN.md](PLAN.md) Фаза 8
- Первый исполняемый трек: [agent/TRACK_CONCURRENCY_V2.md](agent/TRACK_CONCURRENCY_V2.md)
- Тестовый суперстенд: [CONCURRENCY_TEST_HARNESS.md](CONCURRENCY_TEST_HARNESS.md), [agent/TRACK_CONCURRENCY_TEST_HARNESS.md](agent/TRACK_CONCURRENCY_TEST_HARNESS.md)
