# Track: Concurrency Atomics

Parent: [TRACK_MLC_CONCURRENCY_REFINEMENT.md](../../agent/TRACK_MLC_CONCURRENCY_REFINEMENT.md) §47 **#7**.
Source: [CONCURRENCY_V2.md](../CONCURRENCY_V2.md) §14/§44. Size **M**.

## Status: **closed** (2026-07-21) — Critic OK

**Critic 2026-07-21 (STEP=4):** Re-ran `run_atomic_i32.sh` + `test_atomic` +
`--check-only` fixture/`main.mlc`. Anti-false-done: `ad3cb588`…`66c10845`
(STEP=0–3); no `lib/mlc/` → REG skip. Wire: C++ `atomic.hpp` AtomicBool/I32/I64/U64
seq_cst; MLC infer/codegen/send_safe/spawn; MEMORY_MODEL + CONCURRENCY_V2 §0/§14/§43/§44.
**reopen: none**.

Honest residual: no acquire/release/relaxed; no `Atomic[T]` generic; AtomicU64 MLC
value typed as i64 stand-in (no u64 yet); TSan CI job not added (`MLC_TSAN=1` local ok);
optional MLC multi-thread smoke deferred (runtime concurrent sum covers gate).

| Gate | Result |
|------|--------|
| `run_atomic_i32.sh` | `ok atomic_i32` EXIT=0 |
| `test_atomic` (8×10k fetch_add) | ALL 13 checks PASSED |
| fixture `--check-only` | EXIT=0 |
| `mlcc --check-only compiler/main.mlc` | EXIT=0 |
| Tier B / self-host | STEP=2: Tier B EXIT=0; p1≡p2 DIFF_EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§47 `#8 CONCURRENCY_FFI_METADATA`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `ad3cb588` | Decision freeze + open |
| 1 | `ac613cff` | Red harness (E001 gap) |
| 2 | `448f69a6` | atomic.hpp + MLC wire; Tier B + self-host |
| 3 | `66c10845` | MEMORY_MODEL + CONCURRENCY_V2 sync |
| 4 | this Critic | close + archive |

## Decision (STEP=0) — **frozen** 2026-07-20

| Item | Choice |
|------|--------|
| Problem | CONCURRENCY_V2 §14 requires `Atomic[Bool/Int32/Int64/UInt64]` with `load`/`store`/`exchange`/`compare_exchange`/`fetch_add`/`fetch_sub`, **seq_cst only**. No `atomic.hpp` under `runtime/include/mlc/concurrency/`; MLC `AtomicInt32` / `Atomic` → **E001**. `type_is_sync` has Mutex/Arc but not Atomic (Sync for Atomic deferred from §47 #4). Tests use raw `std::atomic` only |
| Boundaries in scope | (1) C++ wrappers: `AtomicBool` / `AtomicI32` / `AtomicI64` / `AtomicU64` over `std::atomic`, seq_cst methods only. (2) MLC surface: **`AtomicI32.new` / `.fetch_add`** (and siblings) matching `StopSource.new`. (3) Checker: Atomic types **Send + Sync**; free mut capture across spawn OK like Mutex. (4) Gate: N-thread concurrent `fetch_add` exact sum (runtime test + optional MLC smoke). (5) Docs MEMORY_MODEL |
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
| 1 | Red: Atomic API missing | **done** — `ok atomic_i32_red` |
| 2 | Wire C++ + MLC Atomic*; green; Tier B; self-host if compiler/ | **done** — `ok atomic_i32`; runtime concurrent sum; Tier B; self-host DIFF 0 |
| 3 | MEMORY_MODEL + CONCURRENCY_V2 §14 sync | **done** — Atomic* Send/Sync + seq_cst; §0/§14/§44 |
| 4 | Critic: gates; archive | **done** — closed |

## Out of scope

§47 #8–#11; acquire/release; RwLock; `lib/mlc/`.
