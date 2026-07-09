# Agent session log

## Session 2026-05-22

| Field | Value |
|-------|-------|
| instructions_rev | `2026-07-09-plan-sync` |
| agent_token_last | — |
| driver_turns_since_plan | 0 |
| step_last | plan-refresh |
| active_track | TRACK_TEXT_RENDERING STEP=1 |
| test_gate | ok (docs-only Planner; prior examples gate 28/0) |

### Turn 2026-07-10 00:06 (Planner — queue after MIR_VM_FULL Epic 4)

| field   | value |
|---------|-------|
| role    | Planner |
| step    | plan-refresh |
| track   | PLAN.md queue |
| started | 2026-07-10 00:06 |
| elapsed | ~2 min |
| done    | Skipped SUPERVISOR (deferred), REGION (design-blocked), MIR Epic 5 (HARD STOP). Opened queue head: TRACK_TEXT_RENDERING STEP=1 (+sub-steps). Synced PLAN/DEVELOPMENT. |
| verify  | PLAN chain → TEXT_RENDERING STEP=1; TRACK STEP=1 pending with sub-steps; no compiler/ edits. |
| result  | Plain: next work is text-rendering design §5.1/§5.3. |
| issues  | STDLIB_NET_SERVER still uncreated (after TEXT_RENDERING in queue). Dirty `compiler/out/*` left. |
| next    | ROLE=Driver STEP=1 TRACK_TEXT_RENDERING — design TEXT_RENDERING.md §5.1 C array view + §5.3 msdfgen shim |

### Turn 2026-07-10 00:04 (Driver TRACK_MIR_VM_FULL STEP=12 — examples subset `--run`)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 12 |
| track   | TRACK_MIR_VM_FULL |
| started | 2026-07-10 00:03 |
| elapsed | ~1 min |
| done    | `vm_examples_run_programs.txt` (28); `run_examples_vm_gate.sh` (stdout+stderr `error:`). Epic 4 closed. |
| verify  | examples GATE:0 (28); `--check-only` main CHECK:0. |
| result  | STEP=12 done. Plain: misc/examples VM subset gated; HARD STOP before Epic 5. |
| issues  | Legacy demos excluded (to_string/unit main). Dirty `compiler/out/*` left. |
| next    | ROLE=Planner — pick next track from PLAN.md queue (NOT TRACK_MIR_VM_FULL STEP=13 / Epic 5) |

### Turn 2026-07-10 00:02 (Driver TRACK_MIR_VM_FULL STEP=11 — multi-module `--run`)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 11 |
| track   | TRACK_MIR_VM_FULL |
| started | 2026-07-09 23:59 |
| elapsed | ~3 min |
| done    | `lower_program.mlc`: lower `sdecl_inner` for export; `misc/examples/vm_multi/*`; `run_multi_module_vm_gate.sh`. |
| verify  | `--run` vm_multi=7; multi-module GATE:0; CHECK:0; BUILD:0; P1/MLCC2/P2/DIFF:0. |
| result  | STEP=11 done. Plain: import+export fn merges into one MirProgram for `--run`. |
| issues  | Dirty `compiler/out/*` left uncommitted. |
| next    | ROLE=Driver STEP=12 TRACK_MIR_VM_FULL — `--run` misc/examples subset script gate (then HARD STOP) |

### Turn 2026-07-09 23:58 (Driver TRACK_MIR_VM_FULL STEP=10 — single-file `--run` stable)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 10 |
| track   | TRACK_MIR_VM_FULL |
| started | 2026-07-09 23:50 |
| elapsed | ~8 min |
| done    | `compiler/tests/run_single_file_vm_gate.sh`; Tier B self-host p1≡p2. |
| verify  | gate GATE:0 (18 `--run` + diff 18/0); P1/MLCC2/P2/DIFF:0; `--check-only` main CHECK:0. |
| result  | STEP=10 done. Plain: single-file `--run` corpus stable under gate script. |
| issues  | Ruby `dev_gate_fast`/`build_tests` still MATCH-parse red — not blocker. Dirty `compiler/out/*` left. |
| next    | ROLE=Driver STEP=11 TRACK_MIR_VM_FULL — multi-module `--run` (import merge → one MirProgram) |

### Turn 2026-07-09 23:50 (Driver TRACK_MIR_VM_FULL STEP=9 — for / pop / contains)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 9 |
| track   | TRACK_MIR_VM_FULL |
| started | 2026-07-09 23:37 |
| elapsed | ~13 min |
| done    | `for`→index walk; `__mir_array_pop` + pop value; `__mir_string_contains`; fixtures. |
| verify  | `--run` for/pop/contains=7; for/contains C++=7; `run_vm_cpp_exit_diff` (pop VM-only). |
| result  | STEP=9 done. Plain: for-loops and pop/contains on MIR VM. |
| issues  | C++ Array has `pop_back` not `pop` — vm_pop not in diff. Dirty `compiler/out/mlcc` left. |
| next    | ROLE=Driver STEP=10 TRACK_MIR_VM_FULL — single-file `--run` stable (Tier B) |

### Turn 2026-07-09 23:40 (Driver TRACK_MIR_VM_FULL STEP=8 — collections corpus)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 8 |
| track   | TRACK_MIR_VM_FULL |
| started | 2026-07-09 23:29 |
| elapsed | ~11 min |
| done    | Fixtures vm_collections_corpus/map/string/array_index; smoke; expand diff manifest. |
| verify  | `--run` corpus/map/string=7 index=5; `run_vm_cpp_exit_diff` 16/0. |
| result  | STEP=8 done. Plain: array/map/string exit parity on corpus. |
| issues  | Untyped Map + Map.length() C++ gaps deferred (typed Map + get in fixtures). Dirty `compiler/out/mlcc` left. |
| next    | ROLE=Driver STEP=9 TRACK_MIR_VM_FULL — iteration, methods beyond push/get/set/length |

### Turn 2026-07-09 23:30 (Driver TRACK_MIR_VM_FULL STEP=7 — ? Result propagation)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 7 |
| track   | TRACK_MIR_VM_FULL |
| started | 2026-07-09 23:14 |
| elapsed | ~16 min |
| done    | `?` rvalue lower + `__mir_result_is_err`/`ok_value`; fixtures vm_question*; smoke. |
| verify  | `--run` ok/err/chain=7; dump question blocks; `run_vm_cpp_exit_diff` 12/0 (question VM-only). |
| result  | STEP=7 done. Plain: `expr?` early-returns Err Result on VM. |
| issues  | C++ `Result<>` needs `mlc::result::` — fixtures not in diff. Dirty `compiler/out/mlcc` left. |
| next    | ROLE=Driver STEP=8 TRACK_MIR_VM_FULL — array/map/string parity with C++ on corpus |

### Turn 2026-07-09 23:15 (Driver TRACK_MIR_VM_FULL STEP=6 — mutual recursion / defaults)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 6 |
| track   | TRACK_MIR_VM_FULL |
| started | 2026-07-09 23:03 |
| elapsed | ~12 min |
| done    | `MirParamDefault` + `vm_bind_call_arguments` fill; fixtures vm_mutual/vm_default_params; smoke. |
| verify  | `--run` mutual=7 default=7; `run_vm_cpp_exit_diff` 12/0; build EXIT:0. |
| result  | STEP=6 done. Plain: mutual calls + omitted default args work on VM. |
| issues  | Avoided `return` in match-loop (codegen). Dirty `compiler/out/mlcc` left. |
| next    | ROLE=Driver STEP=7 TRACK_MIR_VM_FULL — `?` / Result propagation in MIR + VM |

### Turn 2026-07-09 23:05 (Driver TRACK_MIR_VM_FULL STEP=5 — lambda calling convention)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 5 |
| track   | TRACK_MIR_VM_FULL |
| started | 2026-07-09 22:55 |
| elapsed | ~10 min |
| done    | `MirLambdaBinding` + bound-call inline; fixtures lambda/capture/immediate; TRACK convention note. |
| verify  | `--run` all=7; `run_vm_cpp_exit_diff` 10/0; build EXIT:0. |
| result  | STEP=5 done. Plain: bound/immediate lambdas inline at call site. |
| issues  | No first-class lambda VmValue. Dirty `compiler/out/mlcc` left. |
| next    | ROLE=Driver STEP=6 TRACK_MIR_VM_FULL — mutual recursion, default params desugared |

### Turn 2026-07-09 22:50 (Driver TRACK_MIR_VM_FULL STEP=4 — Option/Result/Shared corpus)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 4 |
| track   | TRACK_MIR_VM_FULL |
| started | 2026-07-09 22:39 |
| elapsed | ~11 min |
| done    | `Shared.new` → `__mir_shared_new` (identity); fixtures vm_option/result/shared; smoke + diff manifest. |
| verify  | `--run` all=7; dump `__mir_variant_new`/`__mir_shared_new`; `run_vm_cpp_exit_diff` 7/0. |
| result  | STEP=4 done. Plain: Option/Result/Shared in VM corpus. |
| issues  | Shared is value-copy (no refcount). Diff fixtures use helper+param (C++ visit on ctor temp gap). Dirty `compiler/out/mlcc` left. |
| next    | ROLE=Driver STEP=5 TRACK_MIR_VM_FULL — closures / lambda calling convention |

### Turn 2026-07-09 22:40 (Driver TRACK_MIR_VM_FULL STEP=3 — records)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 3 |
| track   | TRACK_MIR_VM_FULL |
| started | 2026-07-09 22:26 |
| elapsed | ~14 min |
| done    | `VmRecordValue` + `__mir_record_new`/`__mir_record_field`; lower literal/field/match; smoke + examples. |
| verify  | `--run` REC=7 MATCH=7; dump `__mir_record_*`; `run_vm_cpp_exit_diff` 4/0; build EXIT:0. |
| result  | STEP=3 done. Plain: records work on MIR VM. |
| issues  | PatternRecord fields are PatternIdent list (order=decl). Dirty `compiler/out/mlcc` left uncommitted. |
| next    | ROLE=Driver STEP=4 TRACK_MIR_VM_FULL — Option/Result/Shared as variants in VM corpus |

### Turn 2026-07-09 22:20 (Driver TRACK_MIR_VM_FULL STEP=2 — full variant match)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_MIR_VM_FULL |
| started | 2026-07-09 22:15 |
| elapsed | ~5 min |
| done    | Nested ctor test/bind; match guards via CondJump; `VmFieldVariant(Shared)`; examples or/guard/nested; smoke asserts. |
| verify  | `--run` OR=3, guard 1/2, nested=7; `run_vm_cpp_exit_diff` 4/0; build EXIT:0. |
| result  | STEP=2 done. Plain: guards/OR/nested work on MIR VM. |
| issues  | Nested needs Shared field slot (not recursive VmValue). Dirty `compiler/out/mlcc` left uncommitted. |
| next    | ROLE=Driver STEP=3 TRACK_MIR_VM_FULL — records (literal, field access, record patterns) |

### Turn 2026-07-09 22:14 (Driver TRACK_MIR_VM_FULL STEP=1 — VmVariant typed fields)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_MIR_VM_FULL |
| started | 2026-07-09 22:08 |
| elapsed | ~6 min |
| done    | `VmFieldSlot` = I32/Bool/String; `VmVariantValue.fields`; ctor/field natives; smoke + interpreter asserts. |
| verify  | `--run` Left=3, Yes(true)=1, Label("ab")==2; `run_vm_cpp_exit_diff` 4/0; build EXIT:0. |
| result  | STEP=1 done. Plain: variant payloads keep type, not coerced to i32. |
| issues  | Avoided `return` in match-loop (codegen); string `.length()` on bound field still uses `__mir_length` path quirk — compared via `==`. Dirty `compiler/out/mlcc` left uncommitted. |
| next    | ROLE=Driver STEP=2 TRACK_MIR_VM_FULL — full variant match (guards, PatternOr, nested) |

### Turn 2026-07-09 22:07 (Driver TRACK_MIR_VM_FULL STEP=C — variant ctor MIR metadata)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | C |
| track   | TRACK_MIR_VM_FULL |
| started | 2026-07-09 21:45 |
| elapsed | ~22 min |
| done    | Lower `Some`/`Left`/unit ctors to `__mir_variant_new(tag, …)`; drop A-Z `VmCallVariantCtor`; native + dump; smoke/fixture. |
| verify  | `--dump-mir` shows `__mir_variant_new`; `--run` Left=3 / Right=9; `run_vm_cpp_exit_diff` 4/0. |
| result  | Epic 0 STEP C done. Plain: variant ctors are MIR natives, not name heuristic. |
| issues  | Ruby `build_tests` still blocked by spawn_capture MATCH; C++ sum `visit` on ctor temp not in this step. Dirty `compiler/out/mlcc` left uncommitted. |
| next    | ROLE=Driver STEP=1 TRACK_MIR_VM_FULL — VmVariant typed fields (i32/bool/string) |

### Turn 2026-07-09 21:42 (Driver TRACK_API_CLIENT STEP=6 — verify-gate + close)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 6 |
| track   | TRACK_API_CLIENT |
| started | 2026-07-09 21:29 |
| elapsed | ~13 min |
| done    | Verify-gate: `mlcc`→p1→`mlcc2`→p2 `diff` identical; `regression_gate` 20/0; archive TRACK; PLAN §12 done; API_CLIENT §8 filled (§8.4 deferred). |
| verify  | P1/P2 EXIT:0; DIFF_EXIT:0; REG_EXIT:0; derive_json 6/0; openapi 5/0. |
| result  | TRACK closed. Plain: API_CLIENT done; next MIR_VM_FULL STEP C. |
| issues  | §8.4 mock fetch deferred (stubs only). Dirty `compiler/out/mlcc` left uncommitted. |
| next    | ROLE=Driver STEP=C TRACK_MIR_VM_FULL — variant ctor via MIR metadata |

### Turn 2026-07-09 21:27 (Driver TRACK_API_CLIENT STEP=5 — OpenAPI codegen MVP)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 5 |
| track   | TRACK_API_CLIENT |
| started | 2026-07-09 21:25 |
| elapsed | ~10 min |
| done    | `scripts/openapi_codegen.rb`: schemas→`derive { Json }` types + path ops→client stubs; fixture `mini_petstore.yaml`; test `openapi_codegen_test.rb`. |
| verify  | `bundle exec ruby -Ilib:test test/mlc/openapi_codegen_test.rb` 5 runs 0 fail. |
| result  | STEP=5 done. Plain: OpenAPI YAML → MLC types/fns. |
| issues  | Client stubs not wired to `fetch` yet (API_CLIENT §8.4 mock call deferred to STEP=6 or follow-up). Dirty `compiler/out/mlcc` left uncommitted. |
| next    | ROLE=Driver STEP=6 TRACK_API_CLIENT — verify-gate + close |

### Turn 2026-07-09 21:25 (Driver TRACK_API_CLIENT STEP=4 — self-hosted derive Json)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 4 |
| track   | TRACK_API_CLIENT |
| started | 2026-07-09 20:30 |
| elapsed | ~55 min |
| done    | Self-hosted `derive { Json }`: validation; `gen_derive_json_cpp`; registry `*_to_json`/`*_from_json`; globals; static `from_json` arity; type-name ident; sum access `self` (alias). |
| verify  | `compiler/build.sh` EXIT:0; C++ smoke Point+Color round-trip SMOKE:0. |
| result  | STEP=4 done. Plain: mlcc emits working Point/Color Json derive. |
| issues  | `rake test_compiler_mlc` blocked by pre-existing Ruby parse fail on `spawn_capture.mlc`; dirty `compiler/out/mlcc` left uncommitted. |
| next    | ROLE=Driver STEP=5 TRACK_API_CLIENT — OpenAPI codegen script |

### Turn 2026-07-09 20:30 (Driver TRACK_API_CLIENT STEP=3 — sum derive Json tagged)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 3 |
| track   | TRACK_API_CLIENT |
| started | 2026-07-09 20:23 |
| elapsed | ~15 min |
| done    | Sum `derive { Json }`: unit→string, 1-field→`{tag,value}`, N→`{tag,fields}`; convention in `API_CLIENT.md`; tests in `derive_json_test.rb`. |
| verify  | `bundle exec ruby -Ilib:test test/mlc/derive_json_test.rb` 6 runs 0 fail. |
| result  | STEP=3 done. Plain: sum Json round-trips Active/Inactive/Pair. |
| issues  | Dirty `compiler/out/mlcc` left uncommitted. |
| next    | ROLE=Driver STEP=4 TRACK_API_CLIENT — self-hosted derive { Json } |

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
