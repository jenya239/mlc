# Track: Concurrency Sync Trait

Parent: [TRACK_MLC_CONCURRENCY_REFINEMENT.md](TRACK_MLC_CONCURRENCY_REFINEMENT.md) §47 **#4**.
Source: [CONCURRENCY_V2.md](../CONCURRENCY_V2.md) §3/§39/§44 phase 5. Size **M**.

## Status: **active** (2026-07-20) — STEP=2 done → Driver STEP=3

## Next step

**STEP=3** — MEMORY_MODEL sync (Sync / E093 / Array Send).

## Decision (STEP=0) — **frozen** 2026-07-20

| Item | Choice |
|------|--------|
| Problem | `type_is_sync` already exists (`send_safe.mlc`) and gates E087 Sync-safe mut capture (`Mutex` / `Arc` iff `T` Sync). Arc[record] immutable spawn already green. **Gap vs CONCURRENCY_V2 §39:** `TArray` / `Map` are always `!Send` in `type_is_send` (residual from §47 #2 Critic). Spec: `Array[T]` Send iff `T` Send, **!Sync** while shared/mutable; same for `HashMap`. Verified 2026-07-20: `move items` of `[i32]` into bare `spawn` → `error[E092]` (should be OK after Send retune). Free imm Array spawn → E092 today (after retune must stay rejected as **!Sync** share, not Send) |
| Boundaries in scope | (1) Retune `type_is_send` / `type_is_send_generic`: `Array` → Send iff elem Send; `Map` → Send iff K,V Send; keep `type_is_sync` Array/Map = false. (2) Free (non-`move`) capture across bare `spawn` / `TaskScope.spawn`: require **Sync** in addition to Send → new **E093** when `!Sync` (so free Array share stays illegal after Send retune). (3) `move` of Send `!Sync` (e.g. Array[i32]) → OK. (4) Keep E087 mut / E092 !Send / E088 move-use |
| Boundaries out | Atomic[T] Sync → §47 **#5**/#7 when Atomic lands; full Rust `&T: Send` iff `T: Sync` reference model; ThreadPool/Isolate MLC; `lib/mlc/`; changing E087/E092 messages |
| Detecting `.spawn` | Same as #1–#3: `method_name == "spawn"` |
| Diagnostic | New **E093** — free capture of `!Sync` type across concurrency boundary (spawn). Catalog in `diagnostic_codes.mlc`. Do not overload E087/E088/E092 |
| Wire location | `send_safe.mlc` Send retune; `spawn_capture.mlc` free-capture Sync check beside existing Send walk |
| Token / gate | (a) `move` Array[i32] into spawn → 0 E092/E093; (b) free imm Array[i32] spawn → ≥1 **E093**; (c) free Arc[Config]/i32 Sync → 0; (d) mut Array → still E087; (e) Shared → still E092; (f) `mlcc --check-only`. Tier B + self-host |
| Docs | MEMORY_MODEL: Array/Map Send iff T; !Sync; E093 on free !Sync share; Sync name (not Shared type) |
| REG | yes if `lib/mlc/` touched (expect **no**); else skip |
| Out of scope | Atomic runtime; Sync as user-declared trait syntax; general lambda Sync export |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: move Array into spawn still E092 (gap) | **done** — `ok spawn_move_array_send_red` |
| 2 | Send retune Array/Map + E093 free !Sync; green; Tier B; self-host | **done** — `ok spawn_array_sync_e093`; Tier B 1471/0; p1≡p2 |
| 3 | MEMORY_MODEL sync (Sync / E093 / Array Send) | open |
| 4 | Critic: gates; archive | open |

## Out of scope

§47 #5–#11; Atomic Sync; user-facing `trait Sync` syntax; `lib/mlc/`.
