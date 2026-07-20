# Track: Concurrency Atomics

Parent: [TRACK_MLC_CONCURRENCY_REFINEMENT.md](TRACK_MLC_CONCURRENCY_REFINEMENT.md) §47 **#7**.
Source: [CONCURRENCY_V2.md](../CONCURRENCY_V2.md) §14/§44. Size **M**.

## Status: **active** (2026-07-20) — STEP=0 done → Driver STEP=1

## Next step

**STEP=1** — red harness: no MLC/C++ `Atomic*` API (`AtomicInt32` / `Atomic[i32]` undefined).

## Decision (STEP=0) — **frozen** 2026-07-20

| Item | Choice |
|------|--------|
| Problem | CONCURRENCY_V2 §14 requires `Atomic[Bool/Int32/Int64/UInt64]` with `load`/`store`/`exchange`/`compare_exchange`/`fetch_add`/`fetch_sub`, **seq_cst only**. No `atomic.hpp` under `runtime/include/mlc/concurrency/`; MLC `AtomicInt32` / `Atomic` → **E001**. `type_is_sync` has Mutex/Arc but not Atomic (Sync for Atomic deferred from §47 #4). Tests use raw `std::atomic` only |
| Boundaries in scope | (1) C++ wrappers: `AtomicBool` / `AtomicI32` / `AtomicI64` / `AtomicU64` (or one `Atomic<T>` template limited to those) over `std::atomic`, seq_cst methods only. (2) MLC surface: freeze name in STEP=1 red — prefer **`AtomicI32.new` / `.fetch_add`** style matching `StopSource.new` (not generic `Atomic[i32]` unless checker already supports). (3) Checker: Atomic types **Send + Sync**; free mut capture across spawn OK like Mutex. (4) Gate: N-thread concurrent `fetch_add` exact sum (runtime test + optional MLC smoke). (5) Docs MEMORY_MODEL |
| Boundaries out | `.acquire`/`.release`/relaxed ordering; `Atomic[T]` for arbitrary T; `AtomicUsize` unless free with I64; RwLock; FFI atomics; `lib/mlc/`; TSan CI job (local `MLC_TSAN=1` optional) |
| Wire location | `runtime/include/mlc/concurrency/atomic.hpp` (+ `runtime/test`); MLC registry/codegen/method infer; `send_safe.mlc` Sync/Send |
| Token / gate | (a) red: Atomic API undefined; (b) green: check-only + concurrent fetch_add sum; (c) `mlcc --check-only`; Tier B + self-host if `compiler/` touched |
| Docs | MEMORY_MODEL: Atomic* Send/Sync; seq_cst; cite track; CONCURRENCY_V2 §14 status |
| REG | yes if `lib/mlc/` touched (expect **no**); else skip |
| Out of scope | memory ordering knobs; Arbitrary Atomic[T] |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: Atomic API missing | open |
| 2 | Wire C++ + MLC Atomic*; green; Tier B; self-host if compiler/ | open |
| 3 | MEMORY_MODEL + CONCURRENCY_V2 §14 sync | open |
| 4 | Critic: gates; archive | open |

## Out of scope

§47 #8–#11; acquire/release; RwLock; `lib/mlc/`.
