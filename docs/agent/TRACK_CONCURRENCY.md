# Track: Concurrency (message-passing)

Parent: [../PLAN.md](../PLAN.md) �Phase 6; research: [RESEARCH.md](RESEARCH.md) �???????????????

## Status: **planned** STEP=1

**Depends on:** TRACK_REDDIT_DEMO closed (or deferred); COW detach audit; runtime `task.hpp` review.

**Baseline:** single-threaded by design; `Shared<T>` atomic refcount; COW detach **not** atomic; no `Channel`/`spawn`/`Arc`/`Mutex` in language.

**Goal:** ??????? ??????????????? ????? message passing (Go/Rust-style): ????? ????? ????????, ??? shared mutable state.

## Verify gate

```
compiler/tests/build_tests.sh
compiler/build.sh
# concurrency unit tests (new):
compiler/out/run_tests   # channel/spawn smoke
# TSAN build optional: MLC_TSAN=1 compiler/build.sh && ./stress_channel
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | Audit COW detach + `Array`/`Map`/`String` � document unsafe sharing; fix or forbid | pending |
| 2 | Runtime `Channel<T>` � bounded queue, copy in/out (`runtime/concurrency/`) | pending |
| 3 | Language: `spawn { ... }` + `Task<T>` � wire `task.hpp` coroutines or std::thread | pending |
| 4 | `channel.send` / `channel.recv` syntax; checker: Send-safe types only | pending |
| 5 | `Arc<T>` � explicit thread-safe shared ownership (distinct from `Shared<T>`) | pending |
| 6 | `Mutex<T>` � scoped `mutex.lock(fn mut val => ...)`; no global mutex | pending |
| 7 | Tests: race-free channel stress; TSAN smoke; docs; close track | pending |

### Design (from RESEARCH.md)

**Model:** ??????????? ???????, ?? ???????? ??????.

| ???????? | ?? ????????? |
|----------|--------------|
| `Channel<T>` | ?????????? `Mutex` |
| `spawn` + `Task<T>` | ?????? `async/await` |
| `Arc<T>` | `AtomicInt` ??? ???????? ??? |
| `Mutex<T>` scoped | shared mutable `Array`/`Map` ??? sync |

**Runtime ??? ????:** `runtime/src/core/task.hpp` (coroutine stub), `net/http.hpp` (internal threads). ????????? API ???.

**Blocker STEP=1:** COW detach checks `use_count` + deep copy � not atomic. Mutating shared collections across threads is UB today.

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
