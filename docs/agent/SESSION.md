# Agent session log

## Session 2026-05-22

| Field | Value |
|-------|-------|
| instructions_rev | `2026-07-09-plan-sync` |
| agent_token_last | — |
| driver_turns_since_plan | 2 |
| step_last | 2 |
| active_track | TRACK_API_CLIENT STEP=3 |
| test_gate | ok (derive_json_test 4/0) |

### Turn 2026-07-09 20:21 (Driver TRACK_API_CLIENT STEP=2 — JsonError + derive Json records)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_API_CLIENT |
| started | 2026-07-09 19:42 |
| elapsed | ~40 min |
| done    | `JsonError` in `json.mlc` + C++ runtime; Ruby `derive { Json }` for records (`to_json`/`from_json`); inference/call/static lowering; fixed `to_nlohmann_json` array string lifetime; test `derive_json_test.rb`. |
| verify  | `bundle exec ruby -Ilib:test test/mlc/derive_json_test.rb` 4 runs 0 fail; `derive_test.rb` 18/0. |
| result  | STEP=2 done. Plain: record Json derive round-trips Option/Array. |
| issues  | Dirty `compiler/out/mlcc` left uncommitted; self-hosted Json derive = STEP=4. |
| next    | ROLE=Driver STEP=3 TRACK_API_CLIENT — derive { Json } for sum types |

### Turn 2026-07-09 19:40 (Driver TRACK_API_CLIENT STEP=1 — JsonNumber f64, JsonObject Map)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_API_CLIENT |
| started | 2026-07-09 19:37 |
| elapsed | ~5 min |
| done    | `json.mlc`: `JsonNumber(f64)`, `JsonObject(Map<str, JsonValue>)`; `as_number`/`json_number` f64; `as_object`/`json_object`; test `json_value_type_sync_test.rb`. |
| verify  | `bundle exec ruby -Ilib:test test/mlc/json_value_type_sync_test.rb` 2 runs 0 fail. |
| result  | STEP=1 done. Plain: MLC JsonValue matches C++ double + map. |
| issues  | Dirty `compiler/out/mlcc` left uncommitted. |
| next    | ROLE=Driver STEP=2 TRACK_API_CLIENT — JsonError + derive { Json } for records |

### Turn 2026-07-09 19:35 (Driver TRACK_FFI_LAYER STEP=8 — verify-gate + close)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 8 |
| track   | TRACK_FFI_LAYER |
| started | 2026-07-09 19:22 |
| elapsed | ~13 min |
| done    | Self-host `mlcc`→`mlcc2`→`diff` identical; `regression_gate` 20/0; `FFI_LAYER.md` §6 filled; TRACK archived; PLAN/TEXT_RENDERING/API queue updated. |
| verify  | BUILD/P1/MLCC2/P2/DIFF exit 0 (empty diff); regression_gate 20 passed 0 failed. |
| result  | TRACK_FFI_LAYER **closed**. Plain: FFI layer accepted; next API_CLIENT. |
| issues  | `owned` return-marker + ASan drop smoke deferred; dirty `compiler/out/mlcc` not committed. |
| next    | ROLE=Driver STEP=1 TRACK_API_CLIENT — JsonNumber f32→f64, JsonObject→Map |

### Turn 2026-07-09 19:25 (Driver TRACK_FFI_LAYER STEP=7 — concurrency metadata on extern)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 7 |
| track   | TRACK_FFI_LAYER |
| started | 2026-07-09 18:51 |
| elapsed | ~35 min |
| done    | Parse `blocking`/`thread_safe`/`!thread_safe`/`thread_affine(Name)` on extern fn/type; AST/IR `[string]` attrs; registry maps; Send/Sync honor thread_safe/affine. |
| verify  | `thread_safe` Arc.new check 0; `thread_affine` Arc.new E082; `blocking` parse check 0; `mlcc --check-only main` 0. |
| result  | STEP=7 done. Plain: extern concurrency attrs → registry + Send/Sync. |
| issues  | `rake test_compiler_mlc` blocked by pre-existing Ruby parse fail on `spawn_capture.mlc` (MATCH); dirty `compiler/out/mlcc` not committed. |
| next    | ROLE=Driver STEP=8 TRACK_FFI_LAYER — verify-gate + close (mlcc2 diff, regression_gate, FFI_LAYER.md §6) |

### Turn 2026-07-09 18:50 (Driver TRACK_FFI_LAYER STEP=6 — extern fn type as C function pointer)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 6 |
| track   | TRACK_FFI_LAYER |
| started | 2026-07-09 18:20 |
| elapsed | ~30 min |
| done    | Parse `extern fn(...)` → `TyGeneric("__ExternFn",…)`; codegen `std::type_identity_t<Ret(*)(Args…)>`; unify TFn→ExternFn; reject non-Ident args; no `__F*` for ExternFn. |
| verify  | probe `apply_callback(double_value,21)` → `type_identity_t<int(*)(int)>`, link exit 42; lambda arg → E003 top-level; `mlcc --check-only main` 0. |
| result  | STEP=6 done. Plain: extern fn type → C function pointer. |
| issues  | Nested Ident (local fn binding) still accepted; dirty `compiler/out/mlcc` not committed. |
| next    | ROLE=Driver STEP=7 TRACK_FFI_LAYER — concurrency metadata on extern (blocking/thread_safe/thread_affine) |

### Turn 2026-07-09 18:15 (Driver TRACK_FFI_LAYER STEP=5 — extern type + drop RAII)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 5 |
| track   | TRACK_FFI_LAYER |
| started | 2026-07-09 18:03 |
| elapsed | ~12 min |
| done    | `DeclExternType`/`SemanticDeclarationExternType`; parse `extern type Name = "C" from "<h>"` + optional `drop "fn"`; registry opaque; codegen `using` + `OwnedName` unique_ptr deleter; FFI `#include` + `<memory>`. |
| verify  | probe FILE/fclose → `using FILE = ::FILE`, `OwnedFILE`, `#include <stdio.h>`/`<memory>`; `mlcc --check-only main` 0. |
| result  | STEP=5 done. Plain: extern type + drop → using + unique_ptr RAII typedef. |
| issues  | `owned` return-marker wiring deferred; dirty `compiler/out/mlcc` not committed. |
| next    | ROLE=Driver STEP=6 TRACK_FFI_LAYER — extern fn(...) type as C function pointer |

### Turn 2026-07-09 15:55 (Driver TRACK_FFI_LAYER STEP=4 — extern lib → -l)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 4 |
| track   | TRACK_FFI_LAYER |
| started | 2026-07-09 15:32 |
| elapsed | ~23 min |
| done    | `DeclExternLib`/`SemanticDeclarationExternLib`; parse `extern lib "name"`; `mlc_link_libs.txt` from pipeline; `build_bin.sh` adds `-l*`. |
| verify  | probe `extern lib "m"`+`sqrt` → sidecar `m`, link `-lm`, bin exit 0; `mlcc --check-only main` 0. |
| result  | STEP=4 done. Plain: extern lib → -l at link. |
| issues  | Dirty runtime binaries / out/mlcc left uncommitted. |
| next    | ROLE=Driver STEP=5 TRACK_FFI_LAYER — extern type + drop "c_function" RAII |

### Turn 2026-07-09 15:45 (Driver TRACK_FFI_LAYER STEP=3 — self-hosted extern fn codegen)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 3 |
| track   | TRACK_FFI_LAYER |
| started | 2026-07-09 15:09 |
| elapsed | ~36 min |
| done    | `ExprExtern`/`SemanticExpressionExtern` +c_name/header; parse `= "c" from "<h>"` in `decls.mlc`; `decl_cpp` FFI binder + `#include`; skip E004 on extern body; `module.mlc` FFI includes. |
| verify  | `compiler/build.sh` ok; probe `cabs` → `#include <complex.h>` + `mlc_ffi_bind_cabs` + `&::cabs`; `mlcc --check-only compiler/main.mlc` 0. |
| result  | STEP=3 done (Ruby+self-hosted). Plain: mlcc emits include + C binder wrapper. |
| issues  | Dirty runtime binaries / out stamp left uncommitted. Full mlcc2 diff deferred. |
| next    | ROLE=Driver STEP=4 TRACK_FFI_LAYER — extern lib "name" → -l link flags |

### Turn 2026-07-09 15:05 (Driver TRACK_FFI_LAYER STEP=3 — extern fn Ruby codegen)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 3 |
| track   | TRACK_FFI_LAYER |
| started | 2026-07-09 15:05 |
| elapsed | ~4 min |
| done    | Parse `= "c" from "<h>"`; AST/IR `extern_c_name`/`extern_header`; codegen `#include` + `&::c` binder wrapper; `extern_ffi_binding_test.rb`. |
| verify  | `bundle exec ruby -Ilib:test test/mlc/extern_ffi_binding_test.rb` 2 runs 0 fail. |
| result  | STEP=3 Ruby slice done. Plain: extern fn → include + C call. |
| issues  | Self-hosted `decl_cpp.mlc` still empty for extern; f64 literal smoke deferred. |
| next    | ROLE=Driver STEP=3 TRACK_FFI_LAYER — self-hosted extern fn codegen in decl_cpp.mlc |

### Turn 2026-07-09 14:52 (Driver TRACK_FFI_LAYER STEP=2 — RawPointer self-hosted)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_FFI_LAYER |
| started | 2026-07-09 14:52 |
| elapsed | ~12 min |
| done    | `type_gen.mlc`/`expr.mlc` RawPointer→T*; `check.mlc` collect_globals; `test_raw_pointer_syntax.mlc`; suite_registry wire; out regenerated. |
| verify  | `compiler/build.sh` ok; `mlcc --check-only main` 0; probe `RawPointer<i32>` → `int*` in .hpp/.cpp. |
| result  | STEP=2 done. Plain: mlcc знает RawPointer как T*. |
| issues  | Ruby `rake test_compiler_mlc` still red (pre-existing MATCH/HOF); verified via mlcc probes. |
| next    | ROLE=Driver STEP=3 TRACK_FFI_LAYER — extern fn real codegen from header |

### Turn 2026-07-09 14:38 (Driver TRACK_FFI_LAYER STEP=1 — RawPointer Ruby)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_FFI_LAYER |
| started | 2026-07-09 14:38 |
| elapsed | ~12 min |
| done    | `memory.mlc` RawPointer opaque + null/is_null; `type_mapper` → T*; call_rule/member_rule; purity/inference; `safety_model_test` 4 cases. |
| verify  | `bundle exec ruby -Ilib:test test/mlc/safety_model_test.rb -n /raw_pointer/` 4 runs 0 fail. |
| result  | STEP=1 done. Plain: RawPointer → T* в Ruby bootstrap. |
| issues  | `== null` keyword нет в языке — `is_null`/`null()` как Shared; TRACK `type_registry.rb` = opaque via stdlib (как Shared). |
| next    | ROLE=Driver STEP=2 TRACK_FFI_LAYER — RawPointer self-hosted checker/codegen |

### Turn 2026-07-09 14:35 (Driver TRACK_CONCURRENCY_ISOLATE STEP=4 — close + FFI handoff)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 4 |
| track   | TRACK_CONCURRENCY_ISOLATE |
| started | 2026-07-09 14:35 |
| elapsed | ~3 min |
| done    | MEMORY_MODEL ThreadPool/Isolate; PLAN §8; archive ISOLATE; handoff `TRACK_CONCURRENCY_SUPERVISOR.md` (deferred); FFI STEP=1 next. |
| verify  | `mlcc --check-only main` 0; `run_concurrency_smoke.sh` exit 0. |
| result  | TRACK closed. Plain: pool+isolate сданы; дальше FFI. |
| issues  | Dirty runtime binaries left uncommitted. |
| next    | ROLE=Driver STEP=1 TRACK_FFI_LAYER — RawPointer[T] Ruby bootstrap |

### Turn 2026-07-09 14:31 (Driver TRACK_CONCURRENCY_ISOLATE STEP=3 — StopToken shutdown)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 3 |
| track   | TRACK_CONCURRENCY_ISOLATE |
| started | 2026-07-09 14:31 |
| elapsed | ~5 min |
| done    | ThreadPool/Isolate: StopSource, `token`/`request_cancel`, cancellable send/submit, `submit_with_token`; shutdown cancels; tests cancel-unblock + token. |
| verify  | `run_concurrency_smoke.sh` exit 0 (pool 77, isolate 879). |
| result  | STEP=3 done. Plain: cancel будит blocked submit/send. |
| issues  | Dirty runtime binaries left uncommitted. |
| next    | ROLE=Driver STEP=4 TRACK_CONCURRENCY_ISOLATE — docs + self-host / close or Supervisor handoff |

### Turn 2026-07-09 14:27 (Driver TRACK_CONCURRENCY_ISOLATE STEP=2 — Isolate mailbox)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_CONCURRENCY_ISOLATE |
| started | 2026-07-09 14:27 |
| elapsed | ~4 min |
| done    | `isolate.hpp` Isolate&lt;State,Message&gt; (bounded mailbox, owner thread, serial handler, Block); `test_isolate.cpp` 5 cases; smoke + `mlc.hpp`. |
| verify  | `run_concurrency_smoke.sh` exit 0 (test_isolate 870/870). |
| result  | STEP=2 done. Plain: state только на одном owner thread. |
| issues  | Dirty runtime binaries left uncommitted. |
| next    | ROLE=Driver STEP=3 TRACK_CONCURRENCY_ISOLATE — StopToken into pool/isolate shutdown |

### Turn 2026-07-09 14:23 (Driver TRACK_CONCURRENCY_ISOLATE STEP=1 — ThreadPool)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_CONCURRENCY_ISOLATE |
| started | 2026-07-09 14:23 |
| elapsed | ~4 min |
| done    | `thread_pool.hpp` ThreadPool (Channel queue, submit/shutdown); `test_thread_pool.cpp` 4 cases; smoke + `mlc.hpp` wire. |
| verify  | `run_concurrency_smoke.sh` exit 0 (test_thread_pool 70/70). |
| result  | STEP=1 done. Plain: fixed pool + bounded queue. |
| issues  | Dirty runtime binaries left uncommitted. |
| next    | ROLE=Driver STEP=2 TRACK_CONCURRENCY_ISOLATE — Isolate mailbox |

### Turn 2026-07-09 14:17 (Driver TRACK_CONCURRENCY_TEST_HARNESS STEP=5 — cancel stress)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 5 |
| track   | TRACK_CONCURRENCY_TEST_HARNESS |
| started | 2026-07-09 14:17 |
| elapsed | ~3 min |
| done    | `stress_channel.cpp`: `test_cancel_during_receive_stress` (200×), `test_cancel_during_send_stress` (200×), `test_cancel_many_blocked_receivers` (50×16); TRACK T5 done; PLAN/ISOLATE unblocked. |
| verify  | `runtime/test/run_concurrency_smoke.sh` exit 0 (stress_channel 1014527 checks). |
| result  | STEP=5 done. Plain: cancel stress в Layer 2 зелёный. |
| issues  | Dirty runtime test binaries left uncommitted (rebuild artifacts). |
| next    | ROLE=Driver STEP=1 TRACK_CONCURRENCY_ISOLATE — ThreadPool runtime |

### Turn 2026-07-09 14:14 (Driver TRACK_CONCURRENCY_TASKSCOPE STEP=4 — close + Isolate handoff)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 4 |
| track   | TRACK_CONCURRENCY_TASKSCOPE |
| started | 2026-07-09 14:01 |
| elapsed | ~13 min |
| done    | Self-host p1≡p2; MEMORY_MODEL Sync-safe+cancel+TaskScope; PLAN §8; archive TASKSCOPE; handoff `TRACK_CONCURRENCY_ISOLATE.md`; HARNESS T5 unblocked note. |
| verify  | `mlcc --check-only main` 0; smoke exit 0; `diff -rq p1 p2 --exclude=obj` empty. |
| result  | TRACK closed. Plain: cancel/TaskScope/Sync-safe сданы; дальше HARNESS T5. |
| issues  | Dirty `compiler/out/mlcc` + stress binaries left uncommitted (build artifacts). |
| next    | ROLE=Driver STEP=5 TRACK_CONCURRENCY_TEST_HARNESS — cancel-during-send/recv stress matrix |

### Turn 2026-07-09 13:55 (Driver TRACK_CONCURRENCY_TASKSCOPE STEP=3 — Sync-safe spawn capture)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 3 |
| track   | TRACK_CONCURRENCY_TASKSCOPE |
| started | 2026-07-09 13:34 |
| elapsed | ~21 min |
| done    | `spawn_capture.mlc` `type_is_sync_safe_shared` (Mutex/Arc); type hints for `Mutex.new`/`Arc.new`; wired registry into capture pass; tests Mutex/Arc ok. |
| verify  | `compiler/build.sh` ok; `mlcc --check-only main` 0; mutex/arc spawn 0; mut i32 E087 exit 1. |
| result  | STEP=3 done. Plain: Sync-safe Mutex/Arc можно в spawn без move. |
| issues  | Ruby `dev_gate_fast` still red (known). |
| next    | ROLE=Driver STEP=4 TRACK_CONCURRENCY_TASKSCOPE — self-host + docs / close or handoff |

### Turn 2026-07-09 13:33 (Driver TRACK_CONCURRENCY_TASKSCOPE STEP=2 — TaskScope)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_CONCURRENCY_TASKSCOPE |
| started | 2026-07-09 13:31 |
| elapsed | ~2 min |
| done    | `task_scope.hpp` TaskScope: spawn/spawn_with_token/join/dtor cancel+join; `test_task_scope.cpp` 4 checks; smoke + `mlc.hpp`. |
| verify  | `run_concurrency_smoke.sh` exit 0 (test_task_scope 4/4). |
| result  | STEP=2 done. Plain: structured scope ждёт children. |
| issues  | MLC `scope` syntax + error policy deferred. |
| next    | ROLE=Driver STEP=3 TRACK_CONCURRENCY_TASKSCOPE — Sync-safe shared capture without move |

### Turn 2026-07-09 13:28 (Driver TRACK_CONCURRENCY_TASKSCOPE STEP=1 — Channel cancel wake)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_CONCURRENCY_TASKSCOPE |
| started | 2026-07-09 13:25 |
| elapsed | ~3 min |
| done    | `ChannelStatus` + `send`/`receive(StopToken)`; `stop_callback` wakes waiters; `StopToken::native_token()`; 3 cancel tests in `test_channel.cpp`. |
| verify  | `run_concurrency_smoke.sh` exit 0 (test_channel 48/48). |
| result  | STEP=1 done. Plain: cancel будит blocked channel. |
| issues  | TestChannel cancel + HARNESS T5 stress matrix still open. |
| next    | ROLE=Driver STEP=2 TRACK_CONCURRENCY_TASKSCOPE — TaskScope structured spawn |

### Turn 2026-07-09 13:23 (Driver TRACK_CONCURRENCY_V2 STEP=6 — close + handoff)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 6 |
| track   | TRACK_CONCURRENCY_V2 |
| started | 2026-07-09 13:10 |
| elapsed | ~13 min |
| done    | MEMORY_MODEL Send/Sync+E087/E088/StopToken table; self-host p1/mlcc2/p2 identical; archived TRACK to `docs/archive/tracks/`; handoff `TRACK_CONCURRENCY_TASKSCOPE.md`; PLAN §8 partial. |
| verify  | `mlcc --check-only main` 0; `diff -rq p1 p2 --exclude=obj` empty; smoke exit 0. |
| result  | TRACK_CONCURRENCY_V2 closed. Plain: фазы 1–4 сданы; дальше TaskScope/cancel. |
| issues  | Channel wake-on-cancel still open (TASKSCOPE STEP=1 / HARNESS T5). |
| next    | ROLE=Driver STEP=1 TRACK_CONCURRENCY_TASKSCOPE — Channel cancel wake + Cancelled |

### Turn 2026-07-09 13:08 (Driver TRACK_CONCURRENCY_V2 STEP=5 — StopSource/StopToken)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 5 |
| track   | TRACK_CONCURRENCY_V2 |
| started | 2026-07-09 13:06 |
| elapsed | ~2 min |
| done    | `stop.hpp` `StopSource`/`StopToken` (wrap `std::stop_source`/`stop_token`); `test_stop.cpp` 9 checks; smoke + `mlc.hpp` include. |
| verify  | `run_concurrency_smoke.sh` exit 0 (test_stop 9/9). |
| result  | STEP=5 done. Plain: cooperative cancel primitive в runtime. |
| issues  | Channel/sleep wake-on-cancel out of scope. |
| next    | ROLE=Driver STEP=6 TRACK_CONCURRENCY_V2 — MEMORY_MODEL + self-host / close or handoff |

### Turn 2026-07-09 13:05 (Driver TRACK_CONCURRENCY_V2 STEP=4b — move + E088)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 4b |
| track   | TRACK_CONCURRENCY_V2 |
| started | 2026-07-09 12:51 |
| elapsed | ~14 min |
| done    | `KMove`/`move` → `ExprUn("move")`; `move_check.mlc` E088; spawn `move x` exempts E087; codegen `std::move`; catalog 89; tests in `test_spawn.mlc`. |
| verify  | `compiler/build.sh` ok; `mlcc --check-only main` 0; move_ok 0; after_move/spawn_then_use E088 exit 1; still_e087 exit 1. |
| result  | STEP=4 done (4a+4b). Plain: move + use-after-move работают. |
| issues  | Ruby `dev_gate_fast` still red (pre-existing). Sync-safe capture without move deferred. |
| next    | ROLE=Driver STEP=5 TRACK_CONCURRENCY_V2 — StopSource/StopToken runtime primitive |

### Turn 2026-07-09 12:50 (Driver TRACK_CONCURRENCY_V2 STEP=4a — spawn mutable capture E087)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 4a |
| track   | TRACK_CONCURRENCY_V2 |
| started | 2026-07-09 12:39 |
| elapsed | ~11 min |
| done    | `spawn_capture.mlc` + E087; wired in `check.mlc`; tests in `test_spawn.mlc`; catalog 88. Free `let mut` in `spawn` rejected; immutable + local mut ok. |
| verify  | `compiler/build.sh` ok; `mlcc --check-only main` 0; probes ok/local 0, bad E087 exit 1. Ruby `dev_gate_fast` still red (known MATCH parse). |
| result  | STEP=4a done (not full STEP=4). Plain: mutable capture across spawn запрещён. |
| issues  | Ruby rebuild tests still red (pre-existing). |
| next    | ROLE=Driver STEP=4b TRACK_CONCURRENCY_V2 — move keyword + use-after-move into spawn |

### Turn 2026-07-09 12:38 (Driver TRACK_CONCURRENCY_V2 STEP=3 — Sender/Receiver)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 3 |
| track   | TRACK_CONCURRENCY_V2 |
| started | 2026-07-09 12:37 |
| elapsed | ~2 min |
| done    | `Sender`/`Receiver` + `open_channel`/`Channel::sender|receiver`; `clone`; last Sender drop closes after drain; `Sender::close()` wakes. Legacy `Channel` API kept. |
| verify  | `run_concurrency_smoke.sh` exit 0 (test_channel 41/41). |
| result  | STEP=3 done. Plain: Sender/Receiver split в runtime. |
| issues  | — |
| next    | ROLE=Driver STEP=4 TRACK_CONCURRENCY_V2 — spawn_thread move-state tracking (compiler) |

### Turn 2026-07-09 12:36 (Driver TRACK_CONCURRENCY_V2 STEP=2 — rendezvous Channel)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_CONCURRENCY_V2 |
| started | 2026-07-09 12:35 |
| elapsed | ~2 min |
| done    | `Channel(0)` rendezvous in `channel.hpp`; `TestChannel` capacity-0; `test_channel.cpp` handoff/close/try_send cases. Capacity max still 1048576; 0 allowed. |
| verify  | `run_concurrency_smoke.sh` exit 0 (test_channel 27/27). |
| result  | STEP=2 done. Plain: rendezvous channel работает. |
| issues  | — |
| next    | ROLE=Driver STEP=3 TRACK_CONCURRENCY_V2 — Sender/Receiver split + close semantics |

### Turn 2026-07-09 12:35 (Driver TRACK_BUILD_SPEED3 STEP=4 — close + PLAN)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 4 |
| track   | TRACK_BUILD_SPEED3 |
| started | 2026-07-09 12:34 |
| elapsed | ~1 min |
| done    | PLAN §2.9 → done; archived TRACK + ftime-trace under `docs/archive/tracks/`; noted modules/MIR_VM out-of-scope; V2 STEP=2 acceptance written. |
| verify  | Anti-false-done: STEP=1–3 commits `0e976592`/`5bd39424`/`5c5a381e` present. |
| result  | BUILD_SPEED3 closed. Plain: очередь снова на rendezvous Channel. |
| issues  | — |
| next    | ROLE=Driver STEP=2 TRACK_CONCURRENCY_V2 — rendezvous Channel capacity 0 |

### Turn 2026-07-09 12:34 (Driver TRACK_BUILD_SPEED3 STEP=3 — decision c)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 3 |
| track   | TRACK_BUILD_SPEED3 |
| started | 2026-07-09 12:33 |
| elapsed | ~1 min |
| done    | Chose **(c)** from STEP=2 table: reject (a) extern-template Shared/Array/function; reject (b) ninja. Residual AST `std::variant` = future separate track only if re-measured. Updated ftime-trace note. |
| verify  | Decision grounded in `BUILD_SPEED3_FTIME_TRACE_2026_07_09.md` (function 89ms; variant dominates). |
| result  | STEP=3 done (c). Plain: в этом треке дальше не пилим без нового замера. |
| issues  | — |
| next    | ROLE=Driver STEP=4 TRACK_BUILD_SPEED3 — PLAN §2.9 docs + close; then V2 STEP=2 |

### Turn 2026-07-09 12:33 (Driver TRACK_BUILD_SPEED3 STEP=2 — ftime-trace)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_BUILD_SPEED3 |
| started | 2026-07-09 12:32 |
| elapsed | ~2 min |
| done    | `-ftime-trace` on `compiler/out/exprs.cpp` (1245 lines, largest). Wrote `docs/agent/BUILD_SPEED3_FTIME_TRACE_2026_07_09.md`. |
| verify  | wall 9.95s; InstantiateFunction 7220× / ~4.4s Total; variant~14s family; std::function 89ms; Array 132ms; shared_ptr ~2s. |
| result  | STEP=2 done. Plain: CLOSURE_ESCAPE убрал std::function; узкое место — AST variant. |
| issues  | — |
| next    | ROLE=Driver STEP=3 TRACK_BUILD_SPEED3 — decide a/b/c from ftime-trace table |

### Turn 2026-07-09 12:32 (Driver TRACK_BUILD_SPEED3 STEP=1 — CI ccache)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_BUILD_SPEED3 |
| started | 2026-07-09 12:31 |
| elapsed | ~2 min |
| done    | `ci.yml` / `build-mlcc-once.yml` / `triple-bootstrap.yml`: `CCACHE_DIR=.ccache`, key `os/ref/sha` + restore-keys, `CCACHE_SLOPPINESS`; `ccache -s` on once+triple. `.gitignore` `.ccache/`. |
| verify  | YAML parse ok; local `MLCC_OBJ_CLEAN=1`×2: ccache 153/153 direct hits, wall ~2.3s. |
| result  | STEP=1 done. Plain: CI ccache keys aligned with TRACK. |
| issues  | Cache step already existed (hashFiles); upgraded to ref/sha keys per TRACK text. |
| next    | ROLE=Driver STEP=2 TRACK_BUILD_SPEED3 — ftime-trace largest TU |

### Turn 2026-07-09 12:30 (Driver TRACK_CONCURRENCY_V2 STEP=1 — Send/Sync predicates)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_CONCURRENCY_V2 |
| started | 2026-07-09 11:56 |
| elapsed | ~34 min |
| done    | `send_safe.mlc`: `type_is_send` + alias `type_is_send_safe`; `type_is_sync`. Arc/Mutex Send iff T Send; Mutex always Sync; Arc Sync iff T Sync. Tests: `test_send_sync.mlc` wired in suite_registry. |
| verify  | mlcc probe ALL PASSED; `--check-only` channel ok / rejects Array; Arc.new rejects Array; `mlcc --check-only compiler/main.mlc` 0. |
| result  | STEP=1 done. Plain: Send/Sync оси разведены. |
| issues  | Full `build_tests.sh` still red (Ruby HOF/namespace) — not used for verify. PLAN inserts BUILD_SPEED3 before V2 STEP=2. |
| next    | ROLE=Driver STEP=1 TRACK_BUILD_SPEED3 — CI ccache cache in workflows |


Инструкции обновлены (rev 2026-05-28-cleaner → 2026-07-09-plain-queue): plain Cursor queue, без agent-loop MCP.
---

**История до 2026-07-09 11:55** (turn'ы 02:19–11:54 архивированы отдельным блоком; более старые — с 06-28): [docs/archive/SESSION_HISTORY.md](../archive/SESSION_HISTORY.md).
