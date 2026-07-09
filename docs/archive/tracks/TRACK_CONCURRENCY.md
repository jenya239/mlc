# Track: Concurrency (message-passing)

Parent: [../PLAN.md](../PLAN.md) �Phase 6; research: [RESEARCH.md](RESEARCH.md) �???????????????

## Status: **closed** STEP=7 done

**STEP=7 note (2026-06-26):** `stress_channel.cpp` (4×4, 10k msgs); `run_concurrency_smoke.sh`; `MEMORY_MODEL` + `PLAN` §Phase 6; gate **1362/0**; runtime smoke **36** checks; `MLC_TSAN=1` ok.

**STEP=6 note (2026-06-26):** `Mutex<T>`; `mutex.hpp` (`lock_guard` scoped); `infer_mutex_method.mlc`; `mutex.lock(lambda)` ref param codegen; `test_mutex_syntax.mlc` (6); `test_mutex.cpp` (2); gate **1362/0**. Security: lambda-only lock; E082 wrong receiver/arity; `Mutex` forbidden on channel send; allows `Array`/`Map` inner.

**STEP=5 note (2026-06-26):** `Arc<T>`; `mlc/concurrency/arc.hpp` (atomic refcount); `infer_arc_method.mlc` (Send-safe inner); `test_arc_syntax.mlc` (4); `test_arc.cpp` (5); gate **1356/0**. Security: `Arc.new` requires Send-safe; `Arc` forbidden on channel send.

**STEP=4 note (2026-06-26):** `channel.send`/`channel.recv`; `make_channel`; `send_safe.mlc` (E082); `test_channel_syntax.mlc` (5); gate **1352/0**. Security: forbid `Array`/`Map`/`Shared`/`Channel`/`Task` on send; copy-in at runtime.

**STEP=3 note (2026-06-26):** `spawn do … end` → `spawn_task`; `mlc/concurrency/spawn.hpp` (`std::async`, result copied via `future.get()`); `test_spawn.cpp` (2); `test_spawn.mlc` (3); gate **1347/0**. Security: lambda captures run on worker thread — no shared mutable COW (`Array`/`HashMap`) without STEP=5/6.

**STEP=2 note (2026-06-25):** `mlc/concurrency/channel.hpp`; `test_channel.cpp` (18 checks); gate **1344/0**.

**Baseline:** `build_tests` **1344/0**; COW audit documented; `cow_detach.hpp` + runtime smoke.

**STEP=1 note (2026-06-25):** audit `Array`/`HashMap`/`String` COW; `cow_detach.hpp`; `test_cow_detach.cpp`; MEMORY_MODEL §thread safety; gate **1344/0**.

**Goal:** ??????? ??????????????? ????? message passing (Go/Rust-style): ????? ????? ????????, ??? shared mutable state.

## Verify gate

```
compiler/tests/build_tests.sh
compiler/build.sh
# concurrency unit tests (new):
runtime/test/run_concurrency_smoke.sh
# TSAN optional:
MLC_TSAN=1 runtime/test/run_concurrency_smoke.sh
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | Audit COW detach + `Array`/`Map`/`String` � document unsafe sharing; fix or forbid | done |
| 2 | Runtime `Channel<T>` � bounded queue, copy in/out (`runtime/concurrency/`) | done |
| 3 | Language: `spawn { ... }` + `Task<T>` � wire `task.hpp` coroutines or std::thread | done |
| 4 | `channel.send` / `channel.recv` syntax; checker: Send-safe types only | done |
| 5 | `Arc<T>` � explicit thread-safe shared ownership (distinct from `Shared<T>`) | done |
| 6 | `Mutex<T>` � scoped `mutex.lock(fn mut val => ...)`; no global mutex | done |
| 7 | Tests: race-free channel stress; TSAN smoke; docs; close track | done |

### Design (from RESEARCH.md)

**Model:** ??????????? ???????, ?? ???????? ??????.

| ???????? | ?? ????????? |
|----------|--------------|
| `Channel<T>` | ?????????? `Mutex` |
| `spawn` + `Task<T>` | ?????? `async/await` |
| `Arc<T>` | `AtomicInt` ??? ???????? ??? |
| `Mutex<T>` scoped | shared mutable `Array`/`Map` ??? sync |

**Runtime ??? ????:** `runtime/src/core/task.hpp` (coroutine stub), `net/http.hpp` (internal threads). ????????? API ???.

**Blocker STEP=1 (resolved):** COW detach non-atomic — documented; cross-thread `Array`/`HashMap` mutation forbidden until STEP=5/6.

### STEP=1 audit inventory

| Component | Mutate path | Detach | Cross-thread |
|-----------|-------------|--------|--------------|
| `Array<T>` | `push_back`, `pop_back`, `set`, `reserve` | `mlc::cow::detach_shared_buffer` | UB if shared copies mutate |
| `HashMap<K,V>` | `set`, `remove` | same | UB |
| `String` | `operator+=` builds new; no shared heap mutate | N/A | read-share OK |
| `reverse`/`sort` | `Array(*data_)` vector copy | isolated | N/A |

**Artifacts:** `runtime/include/mlc/core/cow_detach.hpp`; `runtime/test/test_cow_detach.cpp` (6 checks).

### Out of scope

- Full async/await ecosystem
- Parallel compiler / incremental query engine (separate tracks)
- GPU / SIMD parallelism

---

## Per-turn template

```
| step | <1�7> |
| done | <one line> |
| verify | build_tests N/0; channel smoke |
| next | ROLE=Driver STEP=<n+1> |
```
