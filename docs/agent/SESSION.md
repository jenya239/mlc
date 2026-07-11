# Agent session log

## Session 2026-05-22

| Field | Value |
|-------|-------|
| instructions_rev | `2026-07-11-self-hosted-runtime-priority` |
| agent_token_last | — |
| driver_turns_since_plan | 7 |
| step_last | 7 |
| active_track | TRACK_STDLIB_HTTP_MLC closed — Critic |
| test_gate | self-host + regression 20/0 + HTTP gates |

### Turn 2026-07-11 13:00 (Driver TRACK_STDLIB_HTTP_MLC STEP=7 — verify-gate / close)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 7 |
| track   | TRACK_STDLIB_HTTP_MLC |
| started | 2026-07-11 12:41 |
| elapsed | ~19 min |
| done    | Verify-gate: `mlcc`→`mlcc2` DIFF_EXIT=0; `regression_gate` 20/0; parse+curl EXIT 0; TRACK/PLAN/DEVELOPMENT closed. |
| verify  | DIFF_EXIT=0; REG 20/0; PARSE=0; CURL=0. |
| result  | Track closed. Plain: MLC HttpServer verified end-to-end. |
| issues  | Queued prompt VM STEP=2 stale; ran SESSION STEP=7. Foreign `compiler/out/*` + dirty `FFI_LAYER.md` left. Residuals: `websocket_http.hpp`; demo if/else routes; Ruby registry bare-name. |
| next    | ROLE=Critic STEP=critique-audit TRACK_STDLIB_HTTP_MLC |

### Turn 2026-07-11 12:39 (Driver TRACK_STDLIB_HTTP_MLC STEP=6 — docs)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 6 |
| track   | TRACK_STDLIB_HTTP_MLC |
| started | 2026-07-11 12:39 |
| elapsed | ~5 min |
| done    | `STDLIB_BACKEND.md` §1/§2/§5; `MLC.md` Tcp/HTTP + bare table; `FFI_LAYER.md` §8; PLAN→STEP=7. |
| verify  | Docs state matches `HttpServer` + deleted public C++ HTTP. |
| result  | STEP=6 done. Plain: docs say HTTP server is MLC-native. |
| issues  | Queued prompt VM STEP=2 stale. |
| next    | ROLE=Driver STEP=7 TRACK_STDLIB_HTTP_MLC — verify-gate (self-host, regression_gate, close) |

### Turn 2026-07-11 12:33 (Driver TRACK_STDLIB_HTTP_MLC STEP=5 — delete C++ HTTP)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 5 |
| track   | TRACK_STDLIB_HTTP_MLC |
| started | 2026-07-11 12:33 |
| elapsed | ~12 min |
| done    | Deleted `http_{request,router,server}.hpp` + `test_http_*.cpp`; `websocket_http.hpp` for WS upgrade; `mlc.hpp` updated; `serve_http_with_thread_pool` removed. |
| verify  | `test_websocket` 19/0; `run_http_server_curl_gate.sh` EXIT 0; parse smoke EXIT 0. |
| result  | STEP=5 done. Plain: public C++ HTTP server API gone; WS keeps private parse. |
| issues  | Queued prompt VM STEP=2 stale. Residual: `websocket_http.hpp` until WEBSOCKET_TO_MLC. |
| next    | ROLE=Driver STEP=6 TRACK_STDLIB_HTTP_MLC — STDLIB_BACKEND/docs sync |

### Turn 2026-07-11 12:31 (Driver TRACK_STDLIB_HTTP_MLC STEP=4 — curl demo)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 4 |
| track   | TRACK_STDLIB_HTTP_MLC |
| started | 2026-07-11 12:31 |
| elapsed | ~8 min |
| done    | `http_server_curl_demo.mlc` (Tcp+spawn; route `/`→home, `/health`→ok); `run_http_server_curl_gate.sh`. |
| verify  | curl gate EXIT 0 (bodies home/ok). |
| result  | STEP=4 done. Plain: curl hits MLC HTTP server. |
| issues  | Queued prompt VM STEP=2 stale. Routing local in demo (not HttpRoute array yet). |
| next    | ROLE=Driver STEP=5 TRACK_STDLIB_HTTP_MLC — delete C++ http_request/router (+ thread_pool check) |

### Turn 2026-07-11 12:29 (Driver TRACK_STDLIB_HTTP_MLC STEP=3 — bare-name)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 3 |
| track   | TRACK_STDLIB_HTTP_MLC |
| started | 2026-07-11 12:29 |
| elapsed | ~8 min |
| done    | `path_normalize.mlc` `"HttpServer"`→`net/http_server.mlc`; `test_driver` assert; smoke `from 'HttpServer'`; rebuilt mlcc; archived SESSION turns ≤GUI Critic. |
| verify  | parse smoke EXIT 0; `mlcc --check-only` Tcp example EXIT 0. |
| result  | STEP=3 done. Plain: bare HttpServer import works. |
| issues  | Queued prompt VM STEP=2 stale. Foreign `compiler/out/*` left uncommitted. Ruby `registry.rb` Tcp-only — residual (lib layer). |
| next    | ROLE=Driver STEP=4 TRACK_STDLIB_HTTP_MLC — Tcp+spawn HTTP demo + curl gate |

### Turn 2026-07-11 12:19 (Driver TRACK_STDLIB_HTTP_MLC STEP=2 — parser)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_STDLIB_HTTP_MLC |
| started | 2026-07-11 12:19 |
| elapsed | ~25 min |
| done    | `lib/.../http_server.mlc` records+parse+format; `HttpParseResult` (not Option); smoke+`run_http_server_parse_smoke.sh` (codegen+link). Amended Decision: local sum. |
| verify  | `bash scripts/run_http_server_parse_smoke.sh` EXIT 0. |
| result  | STEP=2 done. Plain: pure-MLC HTTP parse/format works. |
| issues  | Queued prompt VM STEP=2 stale; `mlcc --run` lacks string methods; Option import segfault → HttpParseResult. |
| next    | ROLE=Driver STEP=3 TRACK_STDLIB_HTTP_MLC — bare-name HttpServer in path_normalize |

### Turn 2026-07-11 12:16 (Driver TRACK_STDLIB_HTTP_MLC STEP=1 — Decision)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_STDLIB_HTTP_MLC |
| started | 2026-07-11 12:16 |
| elapsed | ~3 min |
| done    | Decision locked: `HttpServer`/`http_server.mlc`; bare `HttpServer`; records+parse grammar+router; one module; reject extern wrap / bare `Http`. PLAN→STEP=2. |
| verify  | TRACK Decision section + STEP=1 done; impact table lists STEP=2–6 files. |
| result  | STEP=1 done. Plain: pure-MLC HTTP server API locked. |
| issues  | Queued prompt STEP=2 VM_TYPED stale; ran SESSION Driver STEP=1 HTTP. |
| next    | ROLE=Driver STEP=2 TRACK_STDLIB_HTTP_MLC — HttpRequest/Response + parse_http_request |

### Turn 2026-07-11 12:14 (Planner — activate TRACK_STDLIB_HTTP_MLC)

| field   | value |
|---------|-------|
| role    | Planner |
| step    | plan-refresh |
| track   | TRACK_STDLIB_HTTP_MLC |
| started | 2026-07-11 12:14 |
| elapsed | ~2 min |
| done    | Activated after VM typed Critic OK. Status **active** STEP=1; STEP=1 sub-steps; PLAN §11/§11a + queue; DEVELOPMENT. |
| verify  | TRACK STEP=1 pending; PLAN links `agent/TRACK_STDLIB_HTTP_MLC.md` active. |
| result  | Plan refresh done. Plain: HTTP-on-MLC is next Driver work. |
| issues  | Queued prompt STEP=2 VM_TYPED stale; ran SESSION Planner. |
| next    | ROLE=Driver STEP=1 TRACK_STDLIB_HTTP_MLC — lock Decision (module/records/parse/router) |

### Turn 2026-07-11 12:10 (Critic TRACK_VM_TYPED_COLLECTIONS — critique-audit)

| field   | value |
|---------|-------|
| role    | Critic |
| step    | critique-audit |
| track   | TRACK_VM_TYPED_COLLECTIONS |
| started | 2026-07-11 12:09 |
| elapsed | ~3 min |
| done    | Audited `cf613f0b`…`cdffcbf8` vs STEP 1–4; value/native/examples/gate on disk; archived track; PLAN §13c + MIR_VM_FULL gap → closed; re-ran typed gate. |
| verify  | gate 6/0; no pending STEPs; archive path OK; no false-done. |
| result  | **reopen: none**. Residuals: length-only array/map equal; Decision A wording vs VmFieldSlot carrier (documented). |
| issues  | Queued prompt STEP=2 stale; ran SESSION Critic. Foreign `compiler/out/*` left. |
| next    | ROLE=Planner STEP=plan-refresh — activate TRACK_STDLIB_HTTP_MLC (PLAN queue) |

### Turn 2026-07-11 12:05 (Driver TRACK_VM_TYPED_COLLECTIONS STEP=4 — verify-gate)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 4 |
| track   | TRACK_VM_TYPED_COLLECTIONS |
| started | 2026-07-11 11:40 |
| elapsed | ~25 min |
| done    | Verify-gate: `mlcc`→`mlcc2`→`diff -rq` identical; `regression_gate` 20/0; typed 6 + examples 28 + single/cpp_diff 18 + elif 6 + trampoline depth; TRACK/PLAN closed. |
| verify  | DIFF_EXIT=0; REG 20/0; CORPUS_EXIT=0; EXTRA_VM_EXIT=0. |
| result  | Track closed. Plain: Shared/VmFieldSlot collections verified end-to-end. |
| issues  | Queued prompt still STEP=2 stale; ran SESSION STEP=4. Foreign `compiler/out/*` left. |
| next    | ROLE=Critic STEP=critique-audit TRACK_VM_TYPED_COLLECTIONS |

### Turn 2026-07-11 11:37 (Driver TRACK_VM_TYPED_COLLECTIONS STEP=3 — nested field-slot smokes)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 3 |
| track   | TRACK_VM_TYPED_COLLECTIONS |
| started | 2026-07-11 11:29 |
| elapsed | ~8 min |
| done    | Smokes `vm_record_holds_array`/`_map`/`vm_nested_array`; gate +3 expects; TRACK STEP=3 done; PLAN §13c→STEP=4. VmFieldSlot arms already from STEP=2. |
| verify  | `run_vm_typed_collections_gate.sh` EXIT 0 (6 programs). |
| result  | STEP=3 done. Plain: record↔array/map and nested arrays work via field slots. |
| issues  | Queued prompt said STEP=2 (stale); executed SESSION `next` STEP=3. Foreign `compiler/out/*` left uncommitted. |
| next    | ROLE=Driver STEP=4 TRACK_VM_TYPED_COLLECTIONS — verify-gate (self-host diff, regression_gate, VM corpus) |

### Turn 2026-07-11 05:35 (Driver TRACK_VM_TYPED_COLLECTIONS STEP=2 — Shared elements)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_VM_TYPED_COLLECTIONS |
| started | 2026-07-11 05:12 |
| elapsed | ~23 min |
| done    | `value.mlc`: array/map hold `[VmFieldSlot]`; `VmFieldRecord`/`Array`/`Map`; native push/set any value; examples + `run_vm_typed_collections_gate.sh`; rebuilt mlcc. |
| verify  | gate ok (string exit 4, record exit 9, corpus exit 7); map-of-string smoke exit 2. |
| result  | STEP=2 done. Plain: VM arrays/maps hold non-i32 via VmFieldSlot. |
| issues  | Foreign dirty `literals.cpp`/`module.cpp`/`type_gen.cpp` left; out/ regenerated locally not committed. |
| next    | ROLE=Driver STEP=3 TRACK_VM_TYPED_COLLECTIONS — field-slot nested smokes (record↔array) |

### Turn 2026-07-11 05:10 (Driver TRACK_VM_TYPED_COLLECTIONS STEP=1 — Design)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_VM_TYPED_COLLECTIONS |
| started | 2026-07-11 05:07 |
| elapsed | ~3 min |
| done    | Decision locked: Shared-indirection for array/map elements (mirror `VmFieldVariant`); reject bare `[VmValue]` and split-out; STEP=3 field slots same strategy. PLAN→STEP=2. |
| verify  | TRACK Decision section + STEP=1 done; impact table lists value/native/mir_eval. |
| result  | STEP=1 done. Plain: Shared elements locked for VM containers. |
| issues  | Foreign dirty `literals.cpp`/`module.cpp`/`type_gen.cpp` left. |
| next    | ROLE=Driver STEP=2 TRACK_VM_TYPED_COLLECTIONS — VmArray/VmMap Shared elements + native + repro |

### Turn 2026-07-11 05:05 (Planner — activate TRACK_VM_TYPED_COLLECTIONS)

| field   | value |
|---------|-------|
| role    | Planner |
| step    | plan-refresh |
| track   | TRACK_VM_TYPED_COLLECTIONS |
| started | 2026-07-11 05:02 |
| elapsed | ~3 min |
| done    | Chose VM_TYPED_COLLECTIONS over low-pri FFI/ERROR_UNION/DEBUG (correctness gap). Status **active** STEP=1; sub-steps; PLAN §13c/§11 + queue; DEVELOPMENT. |
| verify  | TRACK STEP=1 pending; PLAN links `agent/TRACK_VM_TYPED_COLLECTIONS.md` active. |
| result  | plan-refresh done. Plain: VM typed collections activated. |
| issues  | Foreign dirty `literals.cpp`/`module.cpp`/`type_gen.cpp` left. |
| next    | ROLE=Driver STEP=1 TRACK_VM_TYPED_COLLECTIONS — lock Design (Shared vs split types) |
