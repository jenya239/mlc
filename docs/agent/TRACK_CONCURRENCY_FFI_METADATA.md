# Track: Concurrency FFI Metadata

Parent: [TRACK_MLC_CONCURRENCY_REFINEMENT.md](TRACK_MLC_CONCURRENCY_REFINEMENT.md) §47 **#8**.
Source: [CONCURRENCY_V2.md](../CONCURRENCY_V2.md) §26/§27/§44. Size **M**.

## Status: **active** (2026-07-21) — STEP=3 done → Critic

## Next step

**STEP=4** — Critic: gates; archive.

## Decision (STEP=0) — **frozen** 2026-07-21

| Item | Choice |
|------|--------|
| Problem | CONCURRENCY_V2 §26/§27: every `extern` type/fn needs concurrency metadata; affinity calls from worker must fail. **Already present** (TRACK_FFI_SAFETY): parse `blocking` / `thread_safe` / `thread_affine(Name)` / `!…`; W-EXTERN-ATTR when `from` header lacks attrs; `type_is_thread_affine` → `!Send`/`!Sync` in `send_safe.mlc` (Arc.new rejects affine types). **Real gap:** calling a `thread_affine(…)`-tagged **extern fn** inside `spawn` / `TaskScope.spawn` `--check-only` **EXIT=0** (probed 2026-07-21). Parser does **not** accept `sendable` / `callback_concurrent` yet |
| Boundaries in scope | (1) Checker error when invoking `thread_affine(…)` extern fn from bare `spawn` or `TaskScope.spawn` body — freeze **`error[E094]`**. (2) Red/green harness matching backlog gate. (3) Optional if cheap in same wire: parse+store `sendable` / `callback_concurrent` (no deep semantics required for gate). (4) Docs: MEMORY_MODEL / CONCURRENCY_V2 §26–§27 status; cite track |
| Boundaries out | Runtime MainThread TLS enforcement; rewriting all stdlib extern attrs; `!Send` sugar on types beyond existing attrs; GUI widget demos; `lib/mlc/`; changing W-EXTERN-ATTR into hard error for all headers |
| Wire location | `compiler/checker/` (spawn/call path + registry fn attrs); tests under `compiler/tests/`; docs |
| Token / gate | (a) red: affine fn in spawn checks clean (**done** `ok extern_thread_affine_spawn_red`); (b) green: **E094** on that pattern; TaskScope.spawn same; main-thread call still OK; (c) `mlcc --check-only`; Tier B + self-host if `compiler/` touched |
| Docs | MEMORY_MODEL / CONCURRENCY_V2 §26–§27; cite TRACK |
| REG | yes if `lib/mlc/` touched (expect **no**); else skip |
| Out of scope | full sendable/callback_concurrent semantics beyond parse; Isolate/Supervisor (#9–#10) |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: affine fn in spawn allowed | **done** — `ok extern_thread_affine_spawn_red` (freeze E094) |
| 2 | Wire call-site affinity check; green; Tier B; self-host if compiler/ | **done** — `ok extern_thread_affine_spawn` (E094); main_ok; Tier B; self-host DIFF 0 |
| 3 | MEMORY_MODEL + CONCURRENCY_V2 §26/§27 sync | **done** — E094 + attrs status; §26/§27 |
| 4 | Critic: gates; archive | open |

## Out of scope

§47 #9–#11; runtime affinity TLS; `lib/mlc/`.
