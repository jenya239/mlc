# Track: Concurrency Send Bound

Parent: [TRACK_MLC_CONCURRENCY_REFINEMENT.md](TRACK_MLC_CONCURRENCY_REFINEMENT.md) §47 **#2**.
Source: [CONCURRENCY_V2.md](../CONCURRENCY_V2.md) §2/§44 phase 3. Size **M**.

## Status: **active** (2026-07-20) — STEP=0 done → Driver STEP=1

## Next step

**STEP=1** — red checker harness: free capture of `!Send` (`Shared[i32]`) in bare `spawn` / `TaskScope.spawn` must emit **E092** (today no Send diagnostic).

## Decision (STEP=0) — **frozen** 2026-07-20

| Item | Choice |
|------|--------|
| Problem | `type_is_send` / `type_is_send_safe` already compositional (`send_safe.mlc`). **Used today only** at `Channel.send` (`infer_channel_method.mlc`) and `Arc.new` (`infer_arc_method.mlc`), both via **E082**. **Gap:** bare `ExprSpawn` and `TaskScope.spawn` (`ExprMethod` name=`spawn`) do **not** reject free captures / moved bindings whose type is `!Send`. Verified gap class: `Shared[T]` is `!Send` in `test_send_sync.mlc`; spawn of a body that free-uses a `Shared[…]` binding is not gated by Send today |
| Boundaries in scope | (1) bare `spawn` body free idents + `move` bindings crossing into spawn; (2) `TaskScope.spawn` / `.spawn` body — same walk sites as §47 #1 (`method_name == "spawn"`); (3) keep `Channel.send` / `Arc.new` Send checks; **migrate** their diagnostic code **E082 → E092** so one code means Send-bound failure |
| Boundaries out | `Sync` trait / Sync inference polish → §47 **#4**; move-use-after → **#3**; ThreadPool/Isolate MLC; changing `Array` Send rule (`TArray => false` today vs CONCURRENCY_V2 “Send iff T Send”) — **residual**, do not retune Array predicate in this track; `lib/mlc/` |
| Fixture for red/green | Prefer **`Shared[i32]`** (or other proven `!Send` from `type_is_send`) as the spawn fixture. Backlog text says `RawPtr[T]` — **no MLC surface `RawPtr` type** in checker today; do not invent one. Positive control: `i32` / `Arc[i32]` free or moved capture stays green |
| Diagnostic | New **E092** — message family: value/type is not Send across concurrency boundary (spawn) / Channel.send / Arc.new. Catalog in `diagnostic_codes.mlc`. Do not overload E087 (mutable) or E088 (moved) |
| Wire location | Prefer extend `spawn_capture.mlc` (or thin helper next to it) using existing type environment + `type_is_send`; call from same spawn-site walk as E087. Channel/Arc: swap `diagnostic_code_e082` → `e092` only for Send-safe failures (leave other E082 uses alone) |
| Token / gate | Checker: (a) bare `spawn` free `Shared[i32]` → ≥1 `error[E092]`; (b) `TaskScope.spawn` same → E092; (c) `move` of Send value → 0; (d) existing Channel.send `!Send` still errors, now E092; (e) `mlcc --check-only` probes. Tier B: `compiler/tests/build_tests.sh`. Self-host: `build.sh` + mlcc2 diff |
| Docs | MEMORY_MODEL Send/Sync table: Channel/Arc rows cite **E092**; note spawn/TaskScope.spawn free `!Send` rejected |
| REG | yes if `lib/mlc/` touched (expect **no**); else skip |
| Out of scope | New Send predicate categories beyond wiring existing `type_is_send`; Sync trait; Array Send retune; ThreadPool/Isolate |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red test: spawn / TaskScope.spawn `Shared` capture → expect E092 (fails today) | open |
| 2 | Wire Send check on spawn sites + E092 catalog; migrate Channel/Arc Send diag; green tests; Tier B; self-host | open |
| 3 | MEMORY_MODEL sync (E092 / spawn Send) | open |
| 4 | Critic: gates; archive | open |

## Out of scope

§47 #3–#11; Array Send predicate retune; Sync trait; ThreadPool/Isolate MLC; `lib/mlc/`.
