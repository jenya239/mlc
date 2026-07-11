# Track: Self-hosted runtime — `core`/`concurrency` на MLC, не C++

Parent: [../FFI_LAYER.md](../FFI_LAYER.md) §8, [../PLAN.md](../PLAN.md).
Trigger: пользователь 2026-07-11 — «ручного C++ не должно быть нигде вообще,
иначе в MLC нет смысла». Исправляет более раннюю границу (§8 таблица —
«рантайм языка остаётся C++ навсегда») — это была неверная уступка, не
принцип. Единственное реально неустранимое — прямой импорт заголовков ОС/
libc (не наша логика), не целые классы/алгоритмы.

## Status: open — STEP=1 next (Decision + risk assessment, не реализация)

## Масштаб (проверено в коде, 2026-07-11)

| Слой | Файлы | Строк | Что внутри |
|------|-------|-------|-----------|
| `core/` | `string.hpp`, `array.hpp`, `hashmap.hpp`, `option.hpp`, `result.hpp`, `collections.hpp`, `cow_detach.hpp`, `arith.hpp`, `int_arith.hpp`, `match.hpp`, `symbol.hpp`, `task.hpp`, `*_combinators.hpp` | 2965 | COW `Array<T>`/`HashMap<K,V>`, refcounting (`Shared<T>`), `String`, `Option`/`Result`, integer arithmetic семантика |
| `concurrency/` | `mutex.hpp`, `channel.hpp`, `arc.hpp`, `stop.hpp`, `task_scope.hpp`, `thread_pool.hpp`, `job_queue.hpp`, `isolate.hpp`, `spawn.hpp` | 1557 | `Mutex[T]`/`Channel[T]`/`Task[T]`/`ThreadPool`/`Isolate`/`Arc[T]` |

Это то, во что компилируется **любой** MLC-код, использующий массивы,
строки, `Shared`, `spawn`/`Mutex`/`Channel` — не FFI-адаптер к сторонней
библиотеке, а бизнес-логика (growth policy, COW-detach, refcount inc/dec,
scheduler), написанная нами на C++ вручную.

## Истинно неустранимое (проверено — что реально используется)

`grep` по `core/`/`concurrency/` — нет прямого `malloc`/`realloc` (память
через STL-контейнеры/`std::shared_ptr`, не raw pointer), есть
`std::atomic`/`<atomic>` (`arc.hpp`, `job_queue.hpp`) и `std::thread`/
`std::mutex`/`std::async` (весь `concurrency/`).

| Примитив | Сегодня | После | Тип операции |
|----------|---------|-------|--------------|
| Аллокация памяти | `std::vector`/`std::shared_ptr` (implicit `::operator new`) | `extern fn malloc/realloc/free = "..." from "<cstdlib>"` + ручная capacity-growth логика на MLC | прямой импорт заголовка libc, не наша логика |
| Атомарный inc/dec (refcount) | `std::atomic<int>` | `extern fn` к GCC/Clang builtin (`__atomic_fetch_add`) **или** `<stdatomic.h>` (C11, реальный заголовок) | прямой импорт заголовка |
| Создание потока | `std::thread`/`std::async` | `extern fn pthread_create/pthread_join = "..." from "<pthread.h>"` | прямой импорт заголовка (реальная POSIX C-библиотека, не C++ STL-обёртка) |
| Mutex/condvar (примитив ОС, не наш `Mutex[T]`) | `std::mutex`/`std::condition_variable` | `extern fn pthread_mutex_lock/pthread_cond_wait = "..." from "<pthread.h>"` | прямой импорт заголовка |

Все четыре — уже закрытый FFI-слой, без новых примитив языка. Разница с
сегодняшним подходом — не вызывать C++ STL-обёртки (`std::thread`,
`std::atomic`), а напрямую биндиться на POSIX/libc, который эти обёртки
сами используют внутри. Growth policy/COW-detach/scheduler-логика поверх
этих примитивов — MLC-код (self-hosted stdlib, тот же слой, что
`lib/mlc/common/stdlib/`).

## Риск (максимальный в проекте — явно зафиксировать, не замалчивать)

Это самый высокий blast radius из всех треков: **любая** регрессия в новом
`Array`/`String`/`Shared` ломает весь компилятор (сам `mlcc` генерирует C++,
использующий эти типы) и любую MLC-программу. Не решение "сделать один
трек и всё" — это фундаментальная замена, требующая параллельной
реализации + исчерпывающего сравнения, не одномоментного cutover.

## Decision (STEP=1)

| Вопрос | Вариант | Рекомендация |
|--------|---------|--------------|
| Порядок портирования (риск возрастает) | 1) `Option`/`Result` (просто, без аллокации) → 2) `String` (аллокация, без refcount cycles) → 3) `Array<T>` (growth policy) → 4) `HashMap<K,V>` (сложнее, хеш+bucket) → 5) `Shared<T>`/`Owned<T>` (refcounting, самое рискованное — циклы/`Weak`) → 6) `Mutex`/`Channel`/`Task`/`ThreadPool` (concurrency, сложнее всего тестировать) | строго по возрастанию риска, каждый этап — отдельный STEP с verify-gate, не пропускать |
| Стратегия cutover | Параллельная реализация (`mlc::self_hosted::Array` рядом с `mlc::Array`) + флаг компиляции для A/B, полное `run_tests`/`test_mlc`/`regression_gate` на обоих перед удалением старого | да — не удалять старую реализацию до зелёного A/B на полном тестовом наборе |
| Производительность | Замер до/после на существующих бенчмарках (`points_mem.mlc` — 5M records) — допустить деградацию по договорённости, но не молча | зафиксировать порог регрессии (напр. не хуже 2x) как gate |
| Атомики: builtin vs `<stdatomic.h>` | `<stdatomic.h>` (C11, реальный заголовок, переносимее builtin) | предпочесть заголовок, builtin — fallback если `<stdatomic.h>` не даёт нужной операции |
|関係 с текущими треками (`FFI_SHIM_MIGRATION`, `STDLIB_HTTP_MLC` и т.п.) | Независимы — они вызывают `Array`/`String` API, не зависят от того, что под капотом | можно вести параллельно, не блокировать друг друга |

## Scope (STEP=1 — только decision, не реализация)

1. Decision (таблица выше) — зафиксировать порядок, cutover-стратегию,
   perf-gate.
2. Прочитать целиком `cow_detach.hpp`/`array.hpp`/`hashmap.hpp` — понять
   текущий COW-механизм 1:1 перед портом (не переизобретать другой).
3. Оценить объём каждого этапа отдельно (STEP=2 плана — по одному типу,
   не создавать все под-треки сразу, см. общий принцип проекта).

## Out of scope (этого трека, STEP=1)

- Сама реализация — начинается отдельными STEP после Decision, по одному
  типу (`Option`/`Result` первый, самый безопасный).
- Убирать C++ **компилятор** как таковой (`clang++`/линковка) — MLC всё
  равно компилируется в C++, это не обсуждается (см. `FFI_LAYER.md` §8 —
  граница "рантайм языка" была неверно сформулирована как исключение из
  уборки C++-**логики**, но не как исключение из "MLC генерирует C++").

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Decision: порядок, cutover-стратегия, perf-gate, атомики. | pending |
| 2 | Прочитать `cow_detach.hpp`/`array.hpp`/`hashmap.hpp` целиком — зафиксировать текущий алгоритм. | pending |
| 3+ | По одному типу (`Option`/`Result` → `String` → `Array` → `HashMap` → `Shared` → concurrency) — каждый: MLC-реализация + A/B verify-gate + cutover. Детализировать после STEP=1/2, не раньше. | pending |
