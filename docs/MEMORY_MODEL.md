# MLC Memory Model

## Философия

MLC компилируется в C++20 без GC и без borrow checker. Безопасность памяти достигается через три принципа:

1. **Value semantics** — всё передаётся по значению
2. **COW (Copy-on-Write)** — копирование коллекций O(1), мутация создаёт копию только при необходимости
3. **Capture by value** — замыкания всегда захватывают переменные по значению

Эта комбинация устраняет классы ошибок, требующие GC или borrow checker, сохраняя при этом удобный синтаксис в стиле Ruby/Kotlin.

---

## Типы данных

### Примитивы

`i32`, `i64`, `f64`, `bool`, `unit` — inline значения, без heap-аллокаций. Копирование тривиально.

### String

```mlc
let name = "hello"
let upper = name.upper()   // новая строка, name не изменилась
```

`mlc::String` — value type над `std::string`. Копирование по значению. Мутирующие методы (`.append()` и т.п.) возвращают новое значение, не изменяют оригинал.

### Array

```mlc
let arr = [1, 2, 3]
let arr2 = arr             // O(1) — shared buffer, refcount++
arr2.push(4)               // COW detach: arr2 получает свой буфер, arr не изменился
```

`mlc::Array<T>` — COW над `shared_ptr<vector<T>>`:
- Копирование: `shared_ptr` copy → O(1)
- Мутация при `use_count > 1` → deep copy buffer (detach)
- Мутация при `use_count == 1` → in-place, без копии
- RAII: буфер освобождается автоматически когда `use_count → 0`

### HashMap

```mlc
let m = HashMap.new()
let m2 = m.set("key", 1)   // m не изменился, m2 — обновлённая копия
```

`mlc::HashMap<K,V>` — COW над `shared_ptr<unordered_map<K,V>>`. Та же семантика что у Array.

### Shared\<T\> и Weak\<T\>

Для рекурсивных типов и явного разделения владения:

```mlc
type Node = { value: i32, children: [Shared<Node>] }

let root = Shared.new(Node { value: 1, children: [] })
```

`Shared<T>` → `std::shared_ptr<T>`. Используется для:
- Рекурсивных AST-узлов (дерево — ациклично, утечек нет)
- Явного разделения одного объекта между несколькими владельцами

`Weak<T>` → `std::weak_ptr<T>`. Используется для разрыва циклов:

```mlc
type Parent = { children: [Shared<Child>] }
type Child  = { parent: Weak<Parent> }   // обратная ссылка через Weak
```

---

## Система биндингов

### Три формы

| Синтаксис | Семантика | C++ |
|-----------|-----------|-----|
| `let x = expr` | rebindable — можно переприсвоить | `auto x = expr;` |
| `const x = expr` | non-rebindable — биндинг фиксирован | `auto x = expr;` (checker блокирует rebind) |
| `let const x = expr` | compile-time constant | `constexpr auto x = expr;` |

### Ключевое различие с Rust

В Rust `let` = immutable, `let mut` = mutable. Это про **мутацию значения**.

В MLC `let` vs `const` — только про **переприсваивание биндинга**. Мутация значения (`.push()` на массиве) разрешена в обоих случаях:

```mlc
const arr = []
arr.push(1)    // OK — мутируем содержимое, не биндинг
arr = []       // Error — нельзя переприсвоить const

let arr2 = []
arr2.push(1)   // OK
arr2 = []      // OK — можно переприсвоить
```

### Почему так

- В MLC нет aliasing mutable references (как в Rust), значит бессмысленно запрещать мутацию через `const`
- `const arr = []` + `arr.push(x)` — стандартный паттерн в JS/TS, хорошо понятен разработчикам
- Разграничение полезно: хочу убедиться что не переприсваиваю → `const`

### let const

```mlc
let const MAX_SIZE = 1024      // constexpr int MAX_SIZE = 1024;
let const PI = 3.14159         // constexpr double PI = 3.14159;
```

Ограничения: инициализатор должен быть compile-time вычислимым. Используется для именованных числовых констант и флагов.

---

## Замыкания

### Синтаксис

```mlc
fn double(x: i32) -> i32 = x * 2

let numbers = [1, 2, 3, 4, 5]
let doubled = numbers.map(fn x => x * 2)
let evens   = numbers.filter(fn x => x % 2 == 0)
let sum     = numbers.fold(0, fn acc x => acc + x)
```

### Правило захвата: всегда по значению

```mlc
let factor = 10
let transform = fn x => x * factor    // factor захвачен by value
// factor можно изменить — на transform не влияет
```

Генерируется C++ lambda с `[=]` capture:

```cpp
auto factor = 10;
auto transform = [=](int x) { return x * factor; };
```

### Почему не [&]

`[&]` (by reference) создаёт dangling reference если lambda переживает scope:

```cpp
// Опасно:
std::function<int(int)> make_adder(int n) {
    return [&n](int x) { return x + n; };  // n — dangling!
}

// Безопасно:
std::function<int(int)> make_adder(int n) {
    return [=](int x) { return x + n; };   // n скопирован
}
```

С COW копирование даже больших объектов дешёво:

```mlc
fn make_checker(allowed: [string]) -> (string -> bool) =
  fn word => allowed.contains(word)
// allowed скопирован в замыкание за O(1) через shared_ptr
```

### Блоки (do...end) — не замыкания

`do...end` блоки используются для последовательности операций в теле функции. Они компилируются в C++ IIFE (`[=]() { ... }()`) только когда используются как выражение (не как последняя строка функции). Захват — `[=]`.

---

## Отсутствие GC: почему достаточно

GC решает проблему неизвестного времени жизни. В MLC времени жизни явны:

| Ситуация | Решение без GC |
|----------|---------------|
| Функция возвращает массив | RAII — shared_ptr автоматически освобождает |
| Lambda захватывает локальную переменную | Capture by value — lambda владеет копией |
| Дерево AST-узлов | Shared<T> — дерево ациклично, нет утечек |
| Граф с обратными рёбрами | Weak<T> для обратных ссылок |
| `arr2 = arr` + `arr2.push(x)` | COW — безопасное разделение буфера |

**Что требует GC но отсутствует в MLC по дизайну:**
- Мутабельные ссылки на один объект из нескольких мест без копирования (заменяется COW)
- Циклические графы без явных `Weak` (документируется как ограничение)

---

## Известные ограничения

1. **Циклические структуры** требуют `Weak<T>`. Забытый `Weak` → утечка. Компилятор не детектирует.

2. **Thread safety ([TRACK_CONCURRENCY](archive/tracks/TRACK_CONCURRENCY.md) MVP +
   [TRACK_CONCURRENCY_V2](archive/tracks/TRACK_CONCURRENCY_V2.md) +
   [TRACK_CONCURRENCY_TASKSCOPE](archive/tracks/TRACK_CONCURRENCY_TASKSCOPE.md) +
   [TRACK_CONCURRENCY_ISOLATE](archive/tracks/TRACK_CONCURRENCY_ISOLATE.md) closed;
   §47 `#1`–`#11` MLC surface shipped — see rows below):**

   **Send / Sync (checker predicates in `send_safe.mlc`):**

   | Type | Send | Sync | Notes |
   |------|------|------|-------|
   | `i32` / scalars | yes | yes | |
   | `Array[T]` / `Map[K,V]` | iff elem/`K,V` Send | no | COW detach not atomic; free share across spawn → **E093** |
   | `Shared[T]` | no | no | not Send; spawn free capture → **E092** |
   | `Arc<T>` | iff `T` Send | iff `T` Sync | control block atomic |
   | `Mutex<T>` | iff `T` Send | yes | scoped `lock` |
   | `Channel` / `Sender` / `Receiver` | payload must be Send | — | **E092** on unsafe send |
   | `StopSource` / `StopToken` | yes | yes | MLC: `StopSource.new()`, `.token()`, `.request()`, `.requested()` ([TRACK_CONCURRENCY_CANCELLATION_WAKES_BLOCKING](archive/tracks/TRACK_CONCURRENCY_CANCELLATION_WAKES_BLOCKING.md)) |
   | `AtomicBool` / `AtomicI32` / `AtomicI64` / `AtomicU64` | yes | yes | seq_cst only; MLC `AtomicI32.new` / `.fetch_add` (siblings); ([TRACK_CONCURRENCY_ATOMICS](archive/tracks/TRACK_CONCURRENCY_ATOMICS.md)) |
   | `Isolate[State, Msg]` | no | no | !Send/!Sync; MLC `Isolate.start` / `.send` / `.shutdown` ([TRACK_CONCURRENCY_ISOLATE_MLC_SURFACE](archive/tracks/TRACK_CONCURRENCY_ISOLATE_MLC_SURFACE.md)) |
   | `Supervisor` | no | no | !Send/!Sync; MLC `Supervisor.new` / `.add` / `.start` / `.stop`; `RestartPolicy` Send+Sync ([TRACK_CONCURRENCY_SUPERVISOR_MLC_SURFACE](archive/tracks/TRACK_CONCURRENCY_SUPERVISOR_MLC_SURFACE.md)) |
   | `TestRuntime` | no | no | !Send/!Sync; MLC facade over `TestScheduler`; `TestRuntime.new(seed)` / `.spawn` / `.join` / `.log_event` / `.events_joined` / `.seed` ([TRACK_CONCURRENCY_TESTRUNTIME_MLC_SURFACE](archive/tracks/TRACK_CONCURRENCY_TESTRUNTIME_MLC_SURFACE.md)) |

   **Spawn capture (checker, [TRACK_CONCURRENCY_MUTABLE_CAPTURE_CHECK](archive/tracks/TRACK_CONCURRENCY_MUTABLE_CAPTURE_CHECK.md)
   + [TRACK_CONCURRENCY_SEND_BOUND](archive/tracks/TRACK_CONCURRENCY_SEND_BOUND.md)
   + [TRACK_CONCURRENCY_MOVE_TRACKING](archive/tracks/TRACK_CONCURRENCY_MOVE_TRACKING.md)
   + [TRACK_CONCURRENCY_SYNC_TRAIT](archive/tracks/TRACK_CONCURRENCY_SYNC_TRAIT.md)
   + [TRACK_CONCURRENCY_CANCELLATION_WAKES_BLOCKING](archive/tracks/TRACK_CONCURRENCY_CANCELLATION_WAKES_BLOCKING.md)
   + [TRACK_CONCURRENCY_ATOMICS](archive/tracks/TRACK_CONCURRENCY_ATOMICS.md)
   + [TRACK_CONCURRENCY_FFI_METADATA](archive/tracks/TRACK_CONCURRENCY_FFI_METADATA.md)
   + [TRACK_CONCURRENCY_ISOLATE_MLC_SURFACE](archive/tracks/TRACK_CONCURRENCY_ISOLATE_MLC_SURFACE.md)
   + [TRACK_CONCURRENCY_SUPERVISOR_MLC_SURFACE](archive/tracks/TRACK_CONCURRENCY_SUPERVISOR_MLC_SURFACE.md)
   + [TRACK_CONCURRENCY_TESTRUNTIME_MLC_SURFACE](archive/tracks/TRACK_CONCURRENCY_TESTRUNTIME_MLC_SURFACE.md)):**
   free use of enclosing `let mut` inside bare `spawn` **or** `TaskScope.spawn`
   (`scope |s| do s.spawn do … end` / `task_scope.spawn do … end`) →
   `error[E087]`, except Sync-safe shared types: `Mutex<_>`; `Atomic*`; `Arc<T>` iff
   `T` is Sync (`type_is_sync_safe_shared` in `spawn_capture.mlc`). Scalars /
   Array / Map still need `move` for mut capture. `move x` → `std::move`; later free use after
   `move` into bare `spawn` **or** `TaskScope.spawn` → `error[E088]`
   (`move_check.mlc`: ExprSpawn + ExprMethod `"spawn"` export marks; general
   ExprLambda does not). `move` of Send `!Sync` (e.g. `Array[i32]`) is OK.
   Immutable free capture OK **only if** the binding's type is **Send and Sync**;
   free capture (or `move`) of a `!Send` type (e.g. `Shared[T]`) → `error[E092]`;
   free (non-`move`) capture of Send but `!Sync` (e.g. `Array[T]`) → `error[E093]`
   (`free_send_capture_*` + Sync check; Sync trait name — not the `Shared` Rc type).
   `Channel.send` / `Arc.new` also emit **E092** for `!Send` payloads. Free mut
   across either boundary is a compile error, not a documented safe-code race.
   **Cancel wake (MLC):** `StopSource.new()` → `.token()` / `.request()`;
   `channel.recv(token)` → `ChannelReceiveResult[T]` with `.cancelled()` (C++
   `receive(StopToken)` → `ChannelStatus::Cancelled`); non-token `recv()` stays
   `Option[T]`; `send(value, token)` → `ChannelStatus`. Sleep/socket cancel deferred.
   **FFI affinity (MLC):** `extern fn … thread_affine(MainThread)` called inside
   `spawn` / `TaskScope.spawn` → **E094**; same fn on the main thread OK.
   `extern type … thread_affine` → `!Send`/`!Sync` (Arc.new / free capture → **E092**).
   Attrs: `blocking` / `thread_safe` / `thread_affine`; W-EXTERN-ATTR if `from` header
   lacks attrs. `sendable` / `callback_concurrent` not parsed yet.
   **Isolate (MLC):** `Isolate.start(state, capacity, handler)` → `Isolate[State, Msg]`;
   `.send(msg)` → `bool` (Block if full); `.shutdown()`; `.state_after_shutdown()`;
   Msg must be Send (**E092**); Isolate itself !Send/!Sync (no free share across spawn);
   overflow **Block** only (Drop*/Reject deferred). Concurrent-sender serial gate:
   `runtime/test/test_isolate.cpp` (+ MLC link smoke `run_isolate_mlc.sh`).
   ([TRACK_CONCURRENCY_ISOLATE_MLC_SURFACE](archive/tracks/TRACK_CONCURRENCY_ISOLATE_MLC_SURFACE.md))
   **Supervisor (MLC):** `Supervisor.new()` / `.add(name, RestartPolicy, handler)` /
   `.start()` / `.stop()`; handler `(StopToken) -> unit` (named fn or lambda);
   `RestartPolicy.Permanent|Transient|Temporary`; one_for_one; optional
   `.set_restart_intensity` / `.storm_tripped`; Supervisor !Send/!Sync;
   `RestartPolicy` Send+Sync. Block sugar `supervisor { child(...) {…} }` deferred.
   Gate: `run_supervisor_mlc.sh` + `runtime/test/test_supervisor.cpp`.
   ([TRACK_CONCURRENCY_SUPERVISOR_MLC_SURFACE](archive/tracks/TRACK_CONCURRENCY_SUPERVISOR_MLC_SURFACE.md))
   **TestRuntime (MLC):** `TestRuntime.new(seed)` → `TestScheduler`; `.spawn do … end`
   (handler `() -> unit`; C++ `[&]` capture — harness, not TaskScope Send gates);
   `.join()` / `.seed()` / `.log_event(string)` / `.events_joined()`; TestRuntime
   !Send/!Sync. Gate: same seed → identical `events_joined`
   (`run_testruntime_mlc.sh`) + `runtime/test/test_scheduler.cpp`.
   `TestMutex`/`TestChannel` MLC out.
   ([TRACK_CONCURRENCY_TESTRUNTIME_MLC_SURFACE](archive/tracks/TRACK_CONCURRENCY_TESTRUNTIME_MLC_SURFACE.md))
   | Type | Refcount | COW detach | Cross-thread |
   |------|----------|------------|--------------|
   | `Array<T>` / `HashMap<K,V>` | atomic (`shared_ptr`) | **not atomic** | Send iff `T`/`K,V` Send, **!Sync**; `move` into spawn OK; free share → **E093**; free mut → E087 |
   | `String` | atomic on heap | no in-place shared heap mutation | read-only share OK |
   | `Shared<T>` | atomic | N/A | not `Send`; `Channel.send` / spawn free capture → **E092** |
   | `Channel<T>` | internal mutex | copy in/out at send/recv | `Send`-safe payload (**E092** if not); capacity 0 = rendezvous; `make_channel(n)` bounded; **`make_unbounded_channel()`** (never blocks on full, [TRACK_CONCURRENCY_CHANNEL_RENDEZVOUS_UNBOUNDED](archive/tracks/TRACK_CONCURRENCY_CHANNEL_RENDEZVOUS_UNBOUNDED.md)); `Sender`/`Receiver` split; MLC `recv()` → `Option[T]`; `recv(StopToken)` → `ChannelReceiveResult[T]` (`.cancelled()`); `send`/`receive(StopToken)` → `Cancelled` on cancel wake |
   | `Arc<T>` | atomic (`Arc` control block) | N/A | read-share; `Send`/`Sync` follow `T`; Sync-safe mut capture across `spawn` / `TaskScope.spawn` OK; `Arc.new` requires Send inner (**E092**) |
   | `Mutex<T>` | N/A | N/A | scoped `lock(callback)`; always Sync; Send iff `T` Send; Sync-safe mut capture across `spawn` / `TaskScope.spawn` OK |
   | `AtomicBool` / `AtomicI32` / `AtomicI64` / `AtomicU64` | N/A | N/A | `runtime/include/mlc/concurrency/atomic.hpp`; seq_cst `load`/`store`/`exchange`/`compare_exchange`/`fetch_add`/`fetch_sub` (no add/sub on Bool); MLC `AtomicI32.new` / `.fetch_add` (+ siblings); Send+Sync; Sync-safe mut capture; ([TRACK_CONCURRENCY_ATOMICS](archive/tracks/TRACK_CONCURRENCY_ATOMICS.md)) |
   | `StopSource` / `StopToken` | N/A | N/A | MLC `StopSource.new()` / `.token()` / `.request()` / `.requested()`; channel wait wakes via `stop_callback` |
   | `TaskScope` | N/A | N/A | MLC `scope \|binder\| do … end` + `.spawn do … end`; dtor cancel+join; E087 free mut; E092 free `!Send`; E093 free `!Sync`; E088 use after `move` into `.spawn` |
   | `ThreadPool` | N/A | N/A | fixed workers + bounded Channel queue; blocking submit; `submit_with_token`; shutdown cancel+join |
   | `Isolate<State,Msg>` | N/A | N/A | `isolate.hpp` + MLC `Isolate.start` / `.send` / `.shutdown` / `.state_after_shutdown`; single owner + bounded mailbox; serial handler; Block overflow; !Send/!Sync; Msg Send on `.send` (**E092**); ([TRACK_CONCURRENCY_ISOLATE_MLC_SURFACE](archive/tracks/TRACK_CONCURRENCY_ISOLATE_MLC_SURFACE.md)) |
   | `Supervisor` | N/A | N/A | `supervisor.hpp` + MLC `Supervisor.new` / `.add` / `.start` / `.stop`; `RestartPolicy`; one_for_one; storm intensity; !Send/!Sync; ([TRACK_CONCURRENCY_SUPERVISOR_MLC_SURFACE](archive/tracks/TRACK_CONCURRENCY_SUPERVISOR_MLC_SURFACE.md)) |
   | `TestRuntime` | N/A | N/A | `testing/scheduler.hpp` (`TestScheduler`) + MLC `TestRuntime.new` / `.spawn` / `.join` / `.log_event` / `.events_joined` / `.seed`; deterministic seed; !Send/!Sync; ([TRACK_CONCURRENCY_TESTRUNTIME_MLC_SURFACE](archive/tracks/TRACK_CONCURRENCY_TESTRUNTIME_MLC_SURFACE.md)) |

   Runtime: `runtime/include/mlc/concurrency/` (`channel.hpp`, `spawn.hpp`,
   `arc.hpp`, `mutex.hpp`, `stop.hpp`, `task_scope.hpp`, `thread_pool.hpp`,
   `isolate.hpp`, `supervisor.hpp`, `atomic.hpp`, `testing/scheduler.hpp`).
   COW helper: `runtime/include/mlc/core/cow_detach.hpp`.
   Smoke: `runtime/test/run_concurrency_smoke.sh`; optional TSAN: `MLC_TSAN=1 …`.

3. **`let const` с runtime-значениями** → C++ compile error. Checker должен проверять.
