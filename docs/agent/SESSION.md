# Agent session log

Turns before 2026-07-25 15:20 archived — see [../archive/SESSION_HISTORY.md](../archive/SESSION_HISTORY.md).

## Entries

### Turn 2026-07-28 (Driver TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=0, §104-13 slice-5 Decision)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 0 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Decision frozen for §104-13 slice 5: extract `decl_cpp_ffi.mlc` — FFI/extern codegen group, 15 items at lines 100-311 in the current 1171-line `decl_cpp.mlc`. Re-derived by grep: `semantic_expression_is_extern` (100), `semantic_extern_ffi_c_name` (106), `semantic_extern_ffi_header` (112), `semantic_expression_is_ffi_extern` (118), `semantic_expression_is_bare_extern` (121), `ffi_header_include_line` (124), `collect_ffi_include_lines_from_declaration` (136), `gen_extern_type_decl_cpp` (153), `collect_ffi_include_lines` (210, export), `ffi_parameter_type_items` (220), `ffi_parameter_name_items` (230), `ffi_extern_reuses_imported_binding` (243), `gen_ffi_fn_proto_cpp` (246), `gen_ffi_fn_decl_cpp` (258), `semantic_fn_body_is_extern` (307). Confirmed one-directional (zero calls from the group into the remaining extend/decl-segment code) and zero usage of any of the 15 names in the 4 already-extracted modules (`decl_cpp_helpers.mlc`, `decl_cpp_type.mlc`, `decl_cpp_trait.mlc`, `decl_cpp_fn.mlc`). 8 of 15 need export (1 already exported — `collect_ffi_include_lines`, used externally by `codegen/module.mlc` — plus 7 newly needed for cross-module-within-`decl_cpp.mlc` visibility); 7 stay internal-only. Full Decision table (Problem/Strategy/Primary gate/Module touch/REG/Out of scope) recorded in `TRACK_COMPILER_ARCHITECTURE_HYGIENE.md` |
| verify  | see `done` |
| result  | §104-13 slice 5 STEP=0 **done** |
| issues  | none |
| next    | ROLE=Driver STEP=1 TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-13 slice 5 — red: confirm `decl_cpp_ffi.mlc` absent, all 15 items at the documented lines, `decl_cpp.mlc` at baseline 1171 lines) |

### Turn 2026-07-28 (Critic TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=3, §104-13 slice-4 close)

| field   | value |
|---------|-------|
| role    | Critic |
| step    | 3 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Independent re-audit of §104-13 slice 4: (1) function/type-set diff — old `decl_cpp.mlc` (`git show 6667ea37:...`, pre-slice-4 baseline, 92 names) vs new `decl_cpp.mlc` + `decl_cpp_fn.mlc` combined (92), `diff` empty, zero lost/duplicated; (2) export-status diff — exactly the 5 documented items gained `export` (`context_with_fn_escape_cpp`, `function_emits_template_cpp`, `function_parameter_def_items`, `merged_function_type_parameters_cpp`, `native_fn_decl_cpp`), zero lost, matches Decision exactly (23→28 exported); (3) fresh `mlcc -o ... compiler/main.mlc` translation from scratch — `decl_cpp_fn.cpp/.hpp` created, grepped `decl_cpp_fn::` across every generated file, found only in `decl_cpp.cpp` (1 direct caller), zero stray references; (4) independent full `rake test_compiler_mlc` rerun from a clean shell (`TMPDIR` confirmed unset first) — exit_code=0, arch lint `failures=0`; (5) line counts confirmed: `decl_cpp.mlc` 1171, `decl_cpp_fn.mlc` 204, no drift. mlcc2 self-host g++ diff not re-run a third time (witnessed directly during Driver STEP=2 same session, no source change since). No false-done found |
| verify  | see `done` |
| result  | **§104-13 slice 4 closed.** §104-13 itself stays open — 2 more groups remain (FFI/extern codegen, extend/impl codegen — largest, decl-segment orchestration), each needs its own Decision |
| issues  | none |
| next    | ROLE=Driver STEP=0 TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-13 slice 5 — Decision: pick next group from the 2 remaining, FFI/extern codegen or extend/impl codegen, re-derive dependency closure by grep against the current 1171-line `decl_cpp.mlc`) |

### Turn 2026-07-28 (Driver TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=2, §104-13 slice-4 green)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | §104-13 slice 4 green: created `compiler/codegen/decl_cpp_fn.mlc` (204 lines) with all 18 fn-decl items moved wholesale, 11 exported per Decision (`function_parameter_def_items`, `main_program_parameter_items_cpp`, `main_set_args_preamble_cpp`, `merged_function_type_parameters_cpp`, `function_emits_template_cpp`, `context_with_fn_escape_cpp`, `native_fn_decl_cpp`, `gen_fn_proto_cpp`, `gen_fn_decl_cpp`, `gen_fn_proto_cpp_with_escape`, `gen_fn_decl_cpp_with_escape`); dead code `function_declaration_template_prefix` moved as-is (zero callers repo-wide, matches §104-12 slice-2 precedent). `decl_cpp.mlc` reduced 1360→1171 lines: removed the 18-item block, added `import { ... } from './decl_cpp_fn'`, dropped 4 now-unused imports (`CppStatement`, `CppBinary`, `prototype_context_for_function`, `gen_return_body_cpp` — all still used inside `decl_cpp_fn.mlc`, confirmed no longer referenced in `decl_cpp.mlc` itself by grep). Controlled bootstrap diff: same `compiler/out/mlcc` binary held fixed, only `decl_cpp.mlc` source toggled (committed HEAD version vs new version, `decl_cpp_fn.mlc` present in both) — `diff -rq before2 out` scoped to exactly `decl_cpp.cpp`/`.hpp` (namespace/line-number changes only, `diff` on bodies shows byte-identical statements, only `#include`/`using namespace`/`#line` annotations differ) + new `decl_cpp_fn.cpp`/`.hpp`; zero other module touched. `rake test_compiler_mlc`: 1471 passed, 0 failed, arch lint failures=0 (13 pre-existing allowlisted warnings unchanged). mlcc2 self-host: built via `compiler/build_bin.sh` (`TMPDIR` in-repo), ran `mlcc2 -o out2 compiler/main.mlc`, `diff -rq out out2 --exclude=obj` — IDENTICAL. Cleaned up `.tmp/s104-13-slice4/` after verification |
| verify  | see `done` |
| result  | §104-13 slice 4 STEP=2 **done** |
| issues  | none |
| next    | ROLE=Critic STEP=3 TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-13 slice 4 close — independent function/type-set diff old `decl_cpp.mlc` (`git show 6667ea37:...`, pre-slice-4 baseline) vs new `decl_cpp.mlc` + `decl_cpp_fn.mlc` combined, export-status diff matches Decision's 11, fresh `mlcc` translation grep for `decl_cpp_fn::` stray references, independent `rake test_compiler_mlc` rerun from clean shell, line-count confirmation, no false-done) |

### Turn 2026-07-28 (Driver TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=1, §104-13 slice-4 red)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Red confirmed for §104-13 slice 4 (`decl_cpp_fn.mlc`): `compiler/codegen/decl_cpp_fn.mlc` confirmed absent (`test -f` negative). All 18 fn-decl items confirmed still present at the exact documented lines: `function_parameter_proto_items` (34), `function_parameter_def_items` (45), `main_program_parameter_items_cpp` (56), `main_set_args_vector_argument_cpp` (59), `main_set_args_preamble_cpp` (72), `prepend_main_set_args_preamble` (78), `function_declaration_template_prefix` (88), `merged_function_type_parameters_cpp` (91), `function_emits_template_cpp` (94), `context_with_fn_escape_cpp` (97), `native_fn_proto_cpp` (100), `native_fn_proto_cpp_with_escape` (110), `native_fn_decl_cpp` (132), `native_fn_decl_cpp_with_escape` (144), `gen_fn_proto_cpp` (179), `gen_fn_decl_cpp` (189), `gen_fn_proto_cpp_with_escape` (200), `gen_fn_decl_cpp_with_escape` (211). `decl_cpp.mlc` confirmed at baseline 1360 lines (matches Decision, no drift) |
| verify  | see `done` |
| result  | §104-13 slice 4 STEP=1 **done**; next STEP=2 |
| issues  | none |
| next    | ROLE=Driver STEP=2 TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-13 slice 4 — green: create `decl_cpp_fn.mlc`, move 18 items wholesale (export the 11 needing cross-module visibility), wire `decl_cpp.mlc` imports, bootstrap diff (split-scoped), `rake test_compiler_mlc`, mlcc2 self-host diff) |

### Turn 2026-07-28 (Driver TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=0, §104-13 slice-4 Decision)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 0 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Decision freeze for §104-13 slice 4 (`decl_cpp_fn.mlc`) — fn decl codegen group. Re-derived against current `decl_cpp.mlc` (1360 lines, post slice-3 shift): 18 items at lines 34-221. Found this group is a shared dependency of **all 3** remaining groups (FFI codegen, extend/impl codegen, decl-segment orchestration — confirmed one-directional by grep, zero back-references), so 11 of 18 need `export` (vs 2 in slices 2/3): 6 already exported with an external test importer (`test_decl_gen.mlc`, unaffected per transitive-import-forwarding), 5 newly needed (`function_parameter_def_items`, `context_with_fn_escape_cpp`, `merged_function_type_parameters_cpp`, `function_emits_template_cpp`, `native_fn_decl_cpp` — zero external-to-`decl_cpp.mlc` callers repo-wide). Found 1 dead-code item (`function_declaration_template_prefix`, zero callers repo-wide) — moved as-is per the `transform_context_new` precedent (§104-12 slice 2), dropping it is out of scope for a pure-relocation slice. Strategy: new `decl_cpp_fn.mlc`, move all 18 wholesale, export the 11 documented; `decl_cpp.mlc` imports them back (no injection needed) and drops 4 now-unused imports (`CppStatement`/`CppBinary`/`prototype_context_for_function`/`gen_return_body_cpp`) |
| verify  | gap confirmed: `decl_cpp_fn.mlc` absent (`test -f` negative); all 18 items confirmed at the documented lines by grep; `decl_cpp.mlc` confirmed at baseline 1360 lines (no drift since slice 3 close) |
| result  | §104-13 slice 4 Decision **frozen**; queue → Driver STEP=1 (red) |
| issues  | none |
| next    | ROLE=Driver STEP=1 TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-13 slice 4 — red: confirm `decl_cpp_fn.mlc` absent / all 18 fn-decl items still in `decl_cpp.mlc` at the documented lines) |

### Turn 2026-07-28 (Critic TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=3, §104-13 slice-3 close)

| field   | value |
|---------|-------|
| role    | Critic |
| step    | 3 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Full independent re-audit of §104-13 slice 3 — closed. (1) Function/type-set diff: `git show 37b056a9~1:compiler/codegen/decl_cpp.mlc` + `decl_cpp_helpers.mlc` (pre-slice baseline, 106 names combined) vs current `decl_cpp.mlc` + `decl_cpp_helpers.mlc` + `decl_cpp_trait.mlc` combined (106) — `diff` empty, zero lost/duplicated. (2) Export-status diff: exactly 2 items gained `export` — `is_semantic_declaration_fn` and `trait_concept_dispatch_forward_protos_cpp` — matching the Decision exactly (`gen_trait_decl_cpp` already exported pre-slice, unchanged). (3) Fresh `mlcc -o ... compiler/main.mlc` translation from scratch: `decl_cpp_trait.cpp/.hpp` created; grepped `decl_cpp_trait::` across every generated file — found only in `decl_cpp.cpp` (the 1 direct caller); grepped `is_semantic_declaration_fn` definitions — exactly 1 (`decl_cpp_helpers.cpp`), zero duplicates; callers confirmed only in `decl_cpp_trait.cpp`/`decl_cpp.cpp`/`.hpp`. (4) Independent full `rake test_compiler_mlc` rerun from a clean shell (`TMPDIR` confirmed unset first): `1471 passed, 0 failed`, `arch lint failures=0`. Line counts confirmed: `decl_cpp.mlc` 1360, `decl_cpp_helpers.mlc` 37, `decl_cpp_trait.mlc` 111 — no drift. mlcc2 self-host g++ diff not re-run a third time (witnessed directly during Driver STEP=2 in the same continuous session, no source change since). No false-done found |
| verify  | see `done` — 4 independent checks, all clean |
| result  | §104-13 slice 3 **closed**. §104-13 itself stays **open** — 3 more groups surveyed (fn decl codegen, FFI/extern codegen, extend/impl codegen — largest, decl-segment orchestration), each needs its own Decision |
| issues  | none |
| next    | ROLE=Driver STEP=0 TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-13 slice 4 — Decision freeze for the next group, likely fn decl codegen or FFI/extern codegen per the surveyed remainder) |

### Turn 2026-07-28 (Driver TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=2, §104-13 slice-3 green)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Extracted §104-13 slice 3: relocated `is_semantic_declaration_fn` (4 lines) into `decl_cpp_helpers.mlc` (exported, now 37 lines). Created `compiler/codegen/decl_cpp_trait.mlc` (111 lines) with the 8 trait-decl codegen items moved wholesale from `decl_cpp.mlc` (`trait_template_header`, `trait_requires_expression_for_method`, `trait_requires_expressions_cpp`, `trait_concept_dispatch_forward_proto_cpp`, `trait_skips_concept_dispatch_forward_protos`, `trait_concept_dispatch_forward_protos_body`, `trait_concept_dispatch_forward_protos_cpp` [export], `gen_trait_decl_cpp` [export]) — no injection needed, matching the frozen Decision. `decl_cpp.mlc` (1360 lines, down from 1463) gained `import { gen_trait_decl_cpp, trait_concept_dispatch_forward_protos_cpp } from './decl_cpp_trait'` and `is_semantic_declaration_fn` from `./decl_cpp_helpers`; dropped now-unused `trait_dispatch_name`/`concept_declval_arguments_for_params`/`concept_requires_expression_method_returns_convertible`/`param_name`/`CppConceptRequires` imports |
| verify  | Bootstrap diff (split-scoped): fresh `mlcc -o p0` (pre-change) vs `mlcc -o p1` (post-change) — `diff -rq` touched exactly `decl_cpp.cpp/.hpp` (shrink) + `decl_cpp_helpers.cpp/.hpp` (grows by 1 function, additive only) + 2 new files `decl_cpp_trait.cpp/.hpp`; zero other generated files changed. Non-`#line` diff confirmed: removed function bodies, `decl_cpp_trait::` prefix at the 2 documented external call sites, `decl_cpp_helpers::` prefix at the 3 other `is_semantic_declaration_fn` call sites remaining in `decl_cpp.mlc` — zero logic changes. `bundle exec rake test_compiler_mlc`: exit 0, phase 10/10 arch lint `failures=0`. mlcc2 self-host: built `mlcc2` from `p1` via `compiler/build_bin.sh` (`MLC_CXX=g++`, in-repo `TMPDIR`), ran `mlcc2 -o p2 compiler/main.mlc`, `diff -r p1 p2 --exclude=obj` — **IDENTICAL**. `compiler/build.sh` confirmed `compiler/out/mlcc` up to date |
| result  | §104-13 slice 3 STEP=2 **done**; queue → Critic STEP=3 |
| issues  | none |
| next    | ROLE=Critic STEP=3 TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-13 slice 3 close) |

### Turn 2026-07-28 (Driver TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=1, §104-13 slice-3 red)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Red confirmed for §104-13 slice 3 (`decl_cpp_trait.mlc`): `compiler/codegen/decl_cpp_trait.mlc` confirmed absent (`test -f` negative). `is_semantic_declaration_fn` confirmed still at line 386 in `decl_cpp.mlc`. All 8 trait-decl items confirmed still present at the exact documented lines: `trait_template_header` (58), `trait_requires_expression_for_method` (65), `trait_requires_expressions_cpp` (79), `trait_concept_dispatch_forward_proto_cpp` (95), `trait_skips_concept_dispatch_forward_protos` (106), `trait_concept_dispatch_forward_protos_body` (109), `trait_concept_dispatch_forward_protos_cpp` (135), `gen_trait_decl_cpp` (143). `decl_cpp.mlc` confirmed at baseline 1463 lines (matches Decision, no drift) |
| verify  | see `done` |
| result  | §104-13 slice 3 STEP=1 **done**; next STEP=2 |
| issues  | none |
| next    | ROLE=Driver STEP=2 TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-13 slice 3 — green: relocate `is_semantic_declaration_fn` to `decl_cpp_helpers.mlc`, create `decl_cpp_trait.mlc`, move 8 items wholesale (export only the 2 needing cross-module visibility), wire `decl_cpp.mlc` imports, bootstrap diff (split-scoped), `rake test_compiler_mlc`, mlcc2 self-host diff) |

### Turn 2026-07-28 (Driver TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=0, §104-13 slice-3 Decision)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 0 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Decision freeze for §104-13 slice 3 (`decl_cpp_trait.mlc`) — trait decl codegen group. Re-derived against current `decl_cpp.mlc` (1463 lines, post slice-2 shift): 8 items at lines 58-154 — `trait_template_header` (58), `trait_requires_expression_for_method` (65), `trait_requires_expressions_cpp` (79), `trait_concept_dispatch_forward_proto_cpp` (95), `trait_skips_concept_dispatch_forward_protos` (106), `trait_concept_dispatch_forward_protos_body` (109), `trait_concept_dispatch_forward_protos_cpp` (135), `gen_trait_decl_cpp` (143, export). Grep confirmed: 6 of 8 have exactly 1 caller entirely within the group; the 2 needing cross-module visibility (`gen_trait_decl_cpp` already exported, `trait_concept_dispatch_forward_protos_cpp` not yet) are called only from elsewhere in `decl_cpp.mlc` (341/1164 fn-decl+decl-segment groups, 1163 decl-segment group) — plain downstream calls, no back-reference. Found 1 real complication unlike slice 2: `trait_concept_dispatch_forward_protos_body` (118) calls `is_semantic_declaration_fn`, a 4-line pure predicate defined later in `decl_cpp.mlc` (386) and shared by 3 *other* groups (370/682/813, zero external-to-file callers repo-wide) — moving the trait group wholesale while leaving that predicate behind would force either duplication or a genuine two-way import cycle. Fix: relocate `is_semantic_declaration_fn` into the existing `decl_cpp_helpers.mlc` leaf first (zero dependency on anything beyond already-imported `SemanticDeclaration`/`SemanticDeclarationFn`), same role slice 1 already plays. Strategy: Step 1 moves the predicate to `decl_cpp_helpers.mlc` (export); Step 2 creates `decl_cpp_trait.mlc` with the 8 items, exporting only `gen_trait_decl_cpp`/`trait_concept_dispatch_forward_protos_cpp` |
| verify  | gap confirmed: `decl_cpp_trait.mlc` absent (`test -f` negative); `is_semantic_declaration_fn` confirmed at line 386 in `decl_cpp.mlc`, zero external-to-`decl_cpp.mlc` callers repo-wide (grep); `decl_cpp.mlc` confirmed at baseline 1463 lines (no drift since slice 2 close) |
| result  | §104-13 slice 3 Decision **frozen**; queue → Driver STEP=1 (red) |
| issues  | none |
| next    | ROLE=Driver STEP=1 TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-13 slice 3 — red: confirm `decl_cpp_trait.mlc` absent / `is_semantic_declaration_fn` at line 386 / all 8 trait items still in `decl_cpp.mlc` at the documented lines) |

### Turn 2026-07-28 (Critic TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=3, §104-13 slice-2 close)

| field   | value |
|---------|-------|
| role    | Critic |
| step    | 3 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Full independent re-audit of §104-13 slice 2 — closed. (1) Function/type-set diff: `git show b72febeb~1:compiler/codegen/decl_cpp.mlc` (pre-slice baseline, 115 names) vs current `decl_cpp.mlc` + `decl_cpp_type.mlc` combined (115) — `diff` empty, zero lost/duplicated. (2) Export-status diff: identical export sets before/after — `gen_type_decl_fwd_cpp`/`gen_type_decl_body_cpp` already exported pre-slice, remain the only 2 exports in `decl_cpp_type.mlc`; 11 internal helpers stay non-exported, matching the Decision exactly. (3) Fresh `mlcc -o ... compiler/main.mlc` translation from scratch: `decl_cpp_type.cpp/.hpp` created; grepped `decl_cpp_type::` across every generated `.cpp`/`.hpp` — found only in `decl_cpp.cpp` (the 1 direct caller), zero stray references elsewhere. (4) Independent full `rake test_compiler_mlc` rerun from a clean shell (`TMPDIR` confirmed unset first): `1471 passed, 0 failed`, `arch lint failures=0`. Line counts confirmed: `decl_cpp.mlc` 1463, `decl_cpp_type.mlc` 190 — no drift. mlcc2 self-host g++ diff not re-run a third time (witnessed directly during Driver STEP=2 in the same continuous session, no source change since). No false-done found |
| verify  | see `done` — 4 independent checks, all clean |
| result  | §104-13 slice 2 **closed**. §104-13 itself stays **open** — 4 more groups surveyed (trait decl codegen, fn decl codegen, FFI/extern codegen, extend/impl codegen — largest, decl-segment orchestration), each needs its own Decision |
| issues  | none |
| next    | ROLE=Driver STEP=0 TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-13 slice 3 — Decision freeze for the next group, likely trait decl codegen or fn decl codegen per the surveyed remainder) |

### Turn 2026-07-28 (Driver TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=2, §104-13 slice-2 green)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Extracted §104-13 slice 2: created `compiler/codegen/decl_cpp_type.mlc` (190 lines) with the 13 type/variant struct codegen items moved wholesale from `decl_cpp.mlc` (`variant_forward_struct_cpp`, `variant_alias_arm_cpp`, `append_type_forward_declarations`, `gen_type_decl_fwd_cpp` [export], `record_field_cpp`, `record_fields_cpp`, `tuple_fields_cpp`, `variant_body_struct_cpp`, `cpp_using_from_struct_using_entry`, `struct_using_declarations_cpp`, `single_variant_body_struct_cpp`, `append_type_body_struct_declarations`, `gen_type_decl_body_cpp` [export]) — zero injection needed, matching the frozen Decision. `decl_cpp.mlc` (1463 lines, down from 1641) gained `import { gen_type_decl_fwd_cpp, gen_type_decl_body_cpp } from './decl_cpp_type'` and dropped the now-unused `variant_ctor_name`/`union_string_lists`/`type_phantom_params_for_variants`/`gen_derive_methods_cpp` imports plus `VarUnit`/`VarTuple`/`VarRecord`/`FieldDef`/`TypeExpr`/`CppStruct`/`CppVariant`/`CppVariantArm`/`CppField`/`StructUsingEntry` (all now sourced only inside `decl_cpp_type.mlc`) |
| verify  | Bootstrap diff (split-scoped): fresh `mlcc -o p0 compiler/main.mlc` (pre-change) vs `mlcc -o p1 compiler/main.mlc` (post-change) — `diff -rq` touched exactly `decl_cpp.cpp`/`.hpp` (relocation + `decl_cpp_type::` prefix at the 4 call sites, zero logic change) plus 2 new files `decl_cpp_type.cpp`/`.hpp`; zero other generated files changed. `bundle exec rake test_compiler_mlc`: exit 0, phase 10/10 arch lint `failures=0` (13 pre-existing allowlisted-size warnings unchanged). mlcc2 self-host: built `mlcc2` from `p1` via `compiler/build_bin.sh` (`MLC_CXX=g++`, in-repo `TMPDIR`), ran `mlcc2 -o p2 compiler/main.mlc`, `diff -r p1 p2 --exclude=obj` — **IDENTICAL**. `compiler/build.sh` confirmed `compiler/out/mlcc` up to date |
| result  | §104-13 slice 2 STEP=2 **done**; queue → Critic STEP=3 |
| issues  | none |
| next    | ROLE=Critic STEP=3 TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-13 slice 2 close) |

### Turn 2026-07-28 (Driver TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=1, §104-13 slice-2 red)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Red confirmed for §104-13 slice 2 (`decl_cpp_type.mlc`): `compiler/codegen/decl_cpp_type.mlc` confirmed absent (`test -f` negative). All 13 items from the frozen Decision confirmed still present in `compiler/codegen/decl_cpp.mlc` at the exact documented lines: `variant_forward_struct_cpp` (58), `variant_alias_arm_cpp` (69), `append_type_forward_declarations` (81), `gen_type_decl_fwd_cpp` (108), `record_field_cpp` (120), `record_fields_cpp` (126), `tuple_fields_cpp` (129), `variant_body_struct_cpp` (142), `cpp_using_from_struct_using_entry` (160), `struct_using_declarations_cpp` (163), `single_variant_body_struct_cpp` (174), `append_type_body_struct_declarations` (192), `gen_type_decl_body_cpp` (219). `decl_cpp.mlc` confirmed at baseline 1641 lines (matches Decision, no drift) |
| verify  | see `done` |
| result  | §104-13 slice 2 STEP=1 **done**; next STEP=2 |
| issues  | none |
| next    | ROLE=Driver STEP=2 TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-13 slice 2 — green: create `decl_cpp_type.mlc`, move 13 items wholesale (export only the 2 needing cross-module visibility), wire `decl_cpp.mlc` import, bootstrap diff (split-scoped), `rake test_compiler_mlc`, mlcc2 self-host diff) |

### Turn 2026-07-28 (Driver TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=0, §104-13 slice-2 Decision)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 0 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Decision freeze for §104-13 slice 2 (`decl_cpp_type.mlc`) — type/variant struct codegen group. Re-derived against current `decl_cpp.mlc` (1641 lines, post slice-1 shift): 13 items at lines 58-234 — `variant_forward_struct_cpp` (58), `variant_alias_arm_cpp` (69), `append_type_forward_declarations` (81), `gen_type_decl_fwd_cpp` (108, export), `record_field_cpp` (120), `record_fields_cpp` (126), `tuple_fields_cpp` (129), `variant_body_struct_cpp` (142), `cpp_using_from_struct_using_entry` (160), `struct_using_declarations_cpp` (163), `single_variant_body_struct_cpp` (174), `append_type_body_struct_declarations` (192), `gen_type_decl_body_cpp` (219, export). Grep confirmed: the 11 internal helpers each have exactly 1 caller, entirely within the group; only the 2 exported entry points are called from elsewhere in `decl_cpp.mlc` (515-516 inside `gen_decl_cpp`, 1326-1327 inside `decl_segment_type_cpp` — both plain downstream calls, zero back-reference). All other dependencies are already-imported external symbols (`./decl/type_gen`, `./cpp_naming`, `./decl/derive_methods_cpp`) or `append_cpp_declarations` (already in `decl_cpp_helpers.mlc` from slice 1) — **zero circular dependency, no injection needed**, unlike §104-12's call_args/method slices. 1 external test importer (`test_decl_gen.mlc`) unaffected per the transitive-import-forwarding precedent. Strategy: new `compiler/codegen/decl_cpp_type.mlc`, move all 13 wholesale, export only the 2 that need cross-module visibility (keep the 11 internal helpers non-exported, matching §104-12 slice 3's precedent) |
| verify  | gap confirmed: `decl_cpp_type.mlc` absent (`test -f` negative); `decl_cpp.mlc` confirmed at baseline 1641 lines (no drift since slice 1 close) |
| result  | §104-13 slice 2 Decision **frozen**; queue → Driver STEP=1 (red) |
| issues  | none |
| next    | ROLE=Driver STEP=1 TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-13 slice 2 — red: confirm `decl_cpp_type.mlc` absent / all 13 items still in `decl_cpp.mlc` at the documented lines) |

### Turn 2026-07-28 (Critic TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=3, §104-13 slice-1 close)

| field   | value |
|---------|-------|
| role    | Critic |
| step    | 3 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Full independent re-audit of §104-13 slice 1 — closed. (1) Function/type-set diff: `git show 9a7272df:compiler/codegen/decl_cpp.mlc` (pre-slice baseline, 119 names) vs current `decl_cpp.mlc` (115) + `decl_cpp_helpers.mlc` (4) combined (119) — `diff` empty, zero lost/duplicated. (2) Export-status diff: exactly the 4 moved items (`empty_cpp_declaration`, `empty_cpp_declarations`, `append_cpp_declarations`, `cpp_decl_from_native_declarations`) gained `export`, matching the Decision exactly. (3) Fresh `mlcc -o ... compiler/main.mlc` translation from scratch: `decl_cpp_helpers.cpp/.hpp` created; grepped `decl_cpp_helpers::` across every generated `.cpp`/`.hpp` — found only in `decl_cpp.cpp`/`.hpp` (the 1 direct caller, including 1 reference inside an inline-defined `.hpp` function body), zero stray references elsewhere. (4) Independent full `rake test_compiler_mlc` rerun from a clean shell (`TMPDIR` confirmed unset first): `1471 passed, 0 failed`, `arch lint failures=0`. Line counts confirmed: `decl_cpp.mlc` 1641, `decl_cpp_helpers.mlc` 30 — no drift. mlcc2 self-host g++ diff not re-run a third time (witnessed directly during Driver STEP=2 in the same continuous session, no source change since). No false-done found |
| verify  | see `done` — 4 independent checks, all clean |
| result  | §104-13 slice 1 **closed**. §104-13 itself stays **open** — 5 more groups surveyed in the track file (type/trait/fn decl codegen, FFI/extern codegen, extend/impl codegen — largest at ~535 lines, decl-segment orchestration), each needs its own Decision before extraction, mirroring §104-12's incremental multi-slice approach |
| issues  | none |
| next    | ROLE=Driver STEP=0 TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-13 slice 2 — survey the next group of `decl_cpp.mlc` for extraction, e.g. type/trait declaration codegen or FFI/extern codegen; Decision freeze) |

### Turn 2026-07-28 (Driver TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=2, §104-13 slice-1 green)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Green for §104-13 slice 1: created `compiler/codegen/decl_cpp_helpers.mlc` (30 lines). Moved wholesale + exported all 4 items from the frozen Decision: `empty_cpp_declaration`, `empty_cpp_declarations`, `append_cpp_declarations`, `cpp_decl_from_native_declarations`. `decl_cpp.mlc` gained 1 import line for these 4 names, dropped now-unused `CppDeclarationEmpty`/`CppDeclarationSequence` imports (only the new module constructs them directly now). `decl_cpp.mlc` shrank 1666 → 1641 lines |
| verify  | Fresh `mlcc` translation of `compiler/main.mlc` succeeded (0 errors); confirmed `decl_cpp_helpers.cpp/.hpp` created. Built pre-change baseline `p0` and post-change `p1`; `diff -rq p0 p1` scoped to exactly `decl_cpp.cpp/.hpp` (shrink) + the 2 new files — zero other files touched (confirms zero external importers, matches Decision). Full non-`#line` content diff of `decl_cpp.cpp` read line-by-line: every change is either the new `#include`/`using namespace decl_cpp_helpers;` pair, a removed function body, or a `decl_cpp_helpers::` prefix insertion at call sites mlcc chose to qualify explicitly — zero logic changes. Full `rake test_compiler_mlc` (`TMPDIR` unset): exit_code=0, `1471 passed, 0 failed`, arch lint failures=0. mlcc2 self-host: built via `compiler/build_bin.sh` with in-repo `TMPDIR` (host disk 99%), ran `mlcc2` on `compiler/main.mlc`, `diff -r p1 p2 --exclude=obj` → IDENTICAL. Cleaned up all `.tmp/s104-13-slice1/**` build artifacts after |
| result  | §104-13 slice-1 green; `compiler/out/mlcc` rebuilt fresh by the rake run (excluded path, not committed) |
| issues  | none |
| next    | ROLE=Critic TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-13 slice 1 — full re-audit: independent function-set diff, independent mlcc translation spot-check + stray-reference grep, independent `rake test_compiler_mlc` rerun; close slice 1 if clean) |

### Turn 2026-07-28 (Driver TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=1, §104-13 slice-1 red)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Red confirmed for §104-13 slice 1 (`decl_cpp_helpers.mlc`): `compiler/codegen/decl_cpp_helpers.mlc` confirmed absent (`test -f` negative). All 4 items from the frozen Decision confirmed still present in `compiler/codegen/decl_cpp.mlc` at the exact documented lines: `empty_cpp_declaration` (26), `empty_cpp_declarations` (1536), `append_cpp_declarations` (1541), `cpp_decl_from_native_declarations` (1550). `decl_cpp.mlc` confirmed at baseline 1666 lines (matches Decision, no drift) |
| verify  | see `done` |
| result  | §104-13 slice 1 STEP=1 **done**; next STEP=2 |
| issues  | none |
| next    | ROLE=Driver STEP=2 TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-13 slice 1 — green: create `decl_cpp_helpers.mlc`, move 4 items wholesale + export, wire `decl_cpp.mlc` import, bootstrap diff (split-scoped), `rake test_compiler_mlc`, mlcc2 self-host diff) |

### Turn 2026-07-28 (Driver TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=0, §104-13 slice-1 Decision)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 0 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Decision freeze for §104-13 slice 1 (`decl_cpp_helpers.mlc`) — first slice of the `codegen/decl_cpp.mlc` split (1666 lines, confirmed by `wc -l`, matches track file's documented count). Surveyed the file into 6 groups by section boundary (leaf helpers; type/variant struct codegen; trait decl codegen; fn decl codegen; FFI/extern codegen; extend/impl codegen — by far the largest at ~535 lines; decl-segment orchestration). Repo-wide grep found exactly 4 pure-leaf declaration-list helpers used pervasively across every other group but with **zero external callers**: `empty_cpp_declaration` (26, 15 call sites), `empty_cpp_declarations` (1536, 60+ call sites), `append_cpp_declarations` (1541, 15 call sites), `cpp_decl_from_native_declarations` (1550, 4 call sites) — a superficially similar `append_cpp_declarations_from_include_text` in `cpp_emit/module_tu_helpers.mlc` is confirmed a distinct, unrelated function by exact-name grep. Mirrors §104-12 slice 2's `transform_context.mlc` role: zero-risk prerequisite leaf every later slice will need. Strategy: new `compiler/codegen/decl_cpp_helpers.mlc`, move all 4 verbatim + export, `decl_cpp.mlc` imports them back (needs them at ~90 internal call sites) |
| verify  | gap confirmed: `decl_cpp_helpers.mlc` absent (`find` negative); all 4 helpers present in `decl_cpp.mlc` at documented lines 26/1536/1541/1550; file at baseline 1666 lines |
| result  | §104-13 slice 1 Decision **frozen**; queue → Driver STEP=1 (red) |
| issues  | none |
| next    | ROLE=Driver STEP=1 TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-13 slice 1 — red: confirm `decl_cpp_helpers.mlc` absent / all 4 items still in `decl_cpp.mlc` at the documented lines) |

### Turn 2026-07-28 (Critic TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=3, §104-12 slice-4 close — §104-12 CLOSED)

| field   | value |
|---------|-------|
| role    | Critic |
| step    | 3 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Full independent re-audit of §104-12 slice 4, last slice — closed slice 4 and §104-12 itself. (1) Function/type-set diff: `git show 5fee3313:compiler/checker/transform/transform.mlc` (pre-slice-4 baseline, 36 names) vs current `transform.mlc` (24) + `transform_method.mlc` (12) combined (36) — `diff` empty, zero lost/duplicated. (2) Export-status diff: all 12 moved items gained `export` — wider than strictly needed (6 have no caller outside the new module and could have stayed non-exported, unlike slice 3's 2 internal-only items) but not a defect: no cycle, no test failure, no behavior change — noted as a minor deviation from the slice-3 precedent, not requiring rework. (3) Fresh `mlcc -o ... compiler/main.mlc` translation from scratch: `transform_method.cpp/.hpp` created; grepped `transform_method::` across every generated `.cpp`/`.hpp` — found only in `transform.cpp` (the 1 direct caller), zero stray references. (4) Independent full `rake test_compiler_mlc` rerun from a clean shell (`TMPDIR` confirmed unset first): `1471 passed, 0 failed`, `arch lint failures=0`. Line counts: `transform.mlc` 881 (down from 1765 at track start, on the file-size WARN allowlist same as slices 1-3, not a failure), `transform_method.mlc` 287, `transform_call_args.mlc` 365, `transform_context.mlc` 41, `transform_coerce.mlc` 268. mlcc2 self-host g++ diff not re-run a third time (witnessed directly during Driver STEP=2 in the same continuous session, no source change since). No false-done found |
| verify  | see `done` — 4 independent checks, all clean |
| result  | §104-12 slice 4 **closed**; §104-12 (`transform.mlc` god-file split, review Step 12) **CLOSED** — 1765→881 lines across 4 new leaf/injection modules over 4 slices, all bootstrap-diff-scoped, self-host mlcc2-identical, 1471/0 throughout |
| issues  | none |
| next    | ROLE=Driver STEP=0 TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-13 — split `codegen/decl_cpp.mlc`, 1666 lines — Decision freeze) |

### Turn 2026-07-28 (Driver TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=2, §104-12 slice-4 green)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Green for §104-12 slice 4: created `compiler/checker/transform/transform_method.mlc` (287 lines). Moved wholesale all 12 items from the frozen Decision: 6 pure leaves (`receiver_type_key_for_method_dispatch`, `instance_method_receiver_and_parameters_mutability_pattern`, `extend_method_mangled_name`, `transform_extend_method_as_call`, `method_result_type_for_dispatch`, `semantic_expression_list_singleton`), 2 needing `transform_expr_fn` (`transform_expr_with_lambda_parameter_types`, `transform_array_hof_method_arguments`), 1 forwarding `transform_expr_fn` without using it directly (`transform_array_hof_method_call`), 3 needing `transform_exprs_fn` (`transform_result_option_hof_method_call`, `transform_regular_method_call`, `transform_region_alloc_method_call`). `transform_method_call_after_object` stays in `transform.mlc`, its 4 branches now pass `transform_expr`/`transform_exprs` as trailing arguments to whichever group function each branch calls. `transform.mlc` shrank 1132 → 881 lines, gained 1 import line |
| verify  | Fresh `mlcc` translation of `compiler/main.mlc` succeeded (0 errors); confirmed `transform_method.cpp/.hpp` created. Built pre-change baseline `p0` (temporarily removed the new file + reverted `transform.mlc` via `git stash`, restored after) and post-change `p1`; `diff -rq p0 p1` scoped to exactly `transform.cpp/.hpp` (12 removed declarations matching the Decision exactly, 4 namespace-qualified call sites `transform_method::transform_result_option_hof_method_call(..., transform_exprs)` / `transform_array_hof_method_call(..., transform_expr)` / `transform_region_alloc_method_call(..., transform_exprs)` / `transform_regular_method_call(..., transform_exprs)`) plus the 2 new files — zero other files touched (same pattern as slice 3: no external module called these functions directly). Full `rake test_compiler_mlc` (all 10 phases, `TMPDIR` unset): exit_code=0, `1471 passed, 0 failed`, arch lint failures=0. mlcc2 self-host: built via `compiler/build_bin.sh` with in-repo `TMPDIR` (host disk 99%/5.3G free), ran `mlcc2` on `compiler/main.mlc`, `diff -r p1 p2 --exclude=obj` → IDENTICAL. Cleaned up all `.tmp/s104-12-slice4/**` build artifacts after |
| result  | §104-12 slice-4 green; `compiler/out/mlcc` rebuilt fresh by the rake run (excluded path, not committed) |
| issues  | none |
| next    | ROLE=Critic TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-12 slice 4 — full re-audit: independent function/type-set diff, independent mlcc translation spot-check, independent `rake test_compiler_mlc` rerun; close slice 4 AND §104-12 itself if clean, since this is the last slice) |

### Turn 2026-07-28 (Driver TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=1, §104-12 slice-4 red)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Red confirmed for §104-12 slice 4 (`transform_method.mlc`): `compiler/checker/transform/transform_method.mlc` confirmed absent (`test -f` negative). All 12 items from the frozen Decision confirmed still present in `compiler/checker/transform/transform.mlc` at the exact documented lines: `receiver_type_key_for_method_dispatch` (45), `instance_method_receiver_and_parameters_mutability_pattern` (67), `extend_method_mangled_name` (95), `transform_extend_method_as_call` (108), `method_result_type_for_dispatch` (189), `transform_result_option_hof_method_call` (279), `transform_array_hof_method_call` (299), `transform_regular_method_call` (320), `transform_expr_with_lambda_parameter_types` (490), `semantic_expression_list_singleton` (501), `transform_array_hof_method_arguments` (508), `transform_region_alloc_method_call` (1112). `transform.mlc` confirmed at baseline 1132 lines (matches slice-3-close state, no drift) |
| verify  | `wc -l` + 12 targeted `grep -n` line-anchor checks against `transform.mlc`; `test -f` for the not-yet-created module |
| result  | red state established; no code touched |
| issues  | none |
| next    | ROLE=Driver STEP=2 TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-12 slice 4 — green: create `transform_method.mlc`, move 12 items wholesale, thread `transform_expr_fn`/`transform_exprs_fn` into the 5 that need them, wire `transform.mlc` call sites + import inside `transform_method_call_after_object`, bootstrap diff (split-scoped), `rake test_compiler_mlc`, mlcc2 self-host diff) |

### Turn 2026-07-28 (Driver TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=0, §104-12 slice-4 Decision)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 0 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Decision freeze for §104-12 slice 4 (`transform_method.mlc`). Re-derived the method-call dispatch group by dependency closure (repo-wide grep, not just within `transform.mlc`): found 12 items with zero other callers anywhere in `compiler/**`, not the roughly-estimated 5 the track file had noted — matches the pattern already seen in slice 1 (14 vs the review's smaller estimate). The 12: `receiver_type_key_for_method_dispatch` (45), `instance_method_receiver_and_parameters_mutability_pattern` (67), `extend_method_mangled_name` (95), `transform_extend_method_as_call` (108 — pure, builds `SemanticExpressionCall` from already-typed values, no transform dependency despite the name), `method_result_type_for_dispatch` (189 — pure leaf), `transform_expr_with_lambda_parameter_types` (490), `semantic_expression_list_singleton` (501), `transform_array_hof_method_arguments` (508), `transform_result_option_hof_method_call` (279), `transform_array_hof_method_call` (299), `transform_regular_method_call` (320), `transform_region_alloc_method_call` (1112). `transform_method_call_after_object` (1082) stays in `transform.mlc` — takes `TransformPass` and calls `dispatch_transform_pass` directly, same rationale as `transform_expr_lambda_with_param_types` staying in slice 3. Decision: move all 12 wholesale (6 pure leaves need zero injection; 2 need `transform_expr_fn`; 3 need `transform_exprs_fn`; no 3rd parameter needed this slice, nothing here calls `transform_expr_lambda_with_param_types`). `transform.mlc`'s call site (`transform_method_call_after_object`, all 4 branches) passes `transform_expr, transform_exprs` as trailing args, gains 1 plain import (not re-exported). This closes out §104-12 — no further slices planned after slice 4. Documented full Decision + Steps table in `TRACK_COMPILER_ARCHITECTURE_HYGIENE.md` |
| verify  | grep audits only this turn (no code changes): confirmed all 12 items' line numbers; repo-wide grep (`compiler/**`, not just `transform.mlc`) confirmed zero external callers for every item, including a false-positive check on `extend_method_mangled_name` (a same-named but differently-signed function exists in `codegen/decl/decl_index.mlc`, confirmed unrelated); confirmed `transform_method_call_after_object`'s single caller inside `transform.mlc` |
| result  | §104-12 slice-4 Decision **frozen**; no code touched |
| issues  | none |
| next    | ROLE=Driver STEP=1 TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-12 slice 4 — red: confirm `transform_method.mlc` absent / all 12 items still in `transform.mlc` at the documented lines) |

### Turn 2026-07-28 (Critic TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=3, §104-12 slice-3 close)

| field   | value |
|---------|-------|
| role    | Critic |
| step    | 3 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Full independent re-audit of §104-12 slice 3 (`transform_call_args.mlc`). (1) Function/type-set diff: extracted old `transform.mlc` from commit `503f9970` (pre-slice-3 parent), name-set = 46; new `transform.mlc` (36) + `transform_call_args.mlc` (10) combined = 46, `diff` empty — zero names lost or duplicated. (2) Export-status diff: confirmed exactly the 8 moved functions (`call_argument_is_lambda`, `callee_semantic_type_is_function`, `function_parameter_types_from_callee_type`, `expected_call_argument_type_at_index`, `function_return_type_from_callee_type`, `transform_call_arguments_using_callee_semantic_type`, `transform_lambda_call_argument`, `transform_one_call_argument_using_optional_expected_type`) gained `export`; the type `Transform_call_arguments_fold_state` and `transform_call_arguments_fold_step` stayed non-exported, matching the frozen Decision exactly. (3) Fresh `mlcc` translation of `compiler/main.mlc`: `transform_call_args.cpp/.hpp` created; grepped every generated file for `transform_call_args::` — found only inside `transform.cpp` (the 1 direct caller, `visit_call`), zero stray references anywhere else. (4) Independent full `rake test_compiler_mlc` rerun from a clean shell (confirmed `TMPDIR` unset beforehand, avoiding the known stale-var pitfall): `1471 passed, 0 failed`, `arch lint failures=0`. (5) Confirmed line counts: `transform.mlc` 1132, `transform_call_args.mlc` 365. No false-done, no stale docs found |
| verify  | see `done` — all 5 independent checks above, run fresh this turn (not reused from the Driver's STEP=2 run) |
| result  | §104-12 slice 3 **closed** |
| issues  | none |
| next    | ROLE=Driver STEP=0 TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-12 slice 4 — `transform_method.mlc` Decision: `transform_result_option_hof_method_call` + `transform_array_hof_method_call` + `transform_regular_method_call` + `transform_extend_method_as_call` + `transform_region_alloc_method_call`, needs `transform_exprs_fn` injection at 3 call sites; `transform_method_call_after_object` stays in `transform.mlc`, tightly coupled to `dispatch_transform_pass`) |

### Turn 2026-07-28 (Driver TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=2, §104-12 slice-3 green)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Green for §104-12 slice 3: created `compiler/checker/transform/transform_call_args.mlc` (365 lines). Moved wholesale the 3 leaf helpers (`callee_semantic_type_is_function`, `function_parameter_types_from_callee_type`, `call_argument_is_lambda`) plus the 6 group items (`expected_call_argument_type_at_index`, `transform_lambda_call_argument`, `transform_one_call_argument_using_optional_expected_type`, `Transform_call_arguments_fold_state`, `function_return_type_from_callee_type`, `transform_call_arguments_fold_step`, `transform_call_arguments_using_callee_semantic_type`). `expected_call_argument_type_at_index` now inlines `Shared.new(TUnknown)` instead of calling `standalone_unknown_cell` (stays in `transform.mlc`, 4 other callers). Threaded 2 injected function parameters (`transform_expr_fn`, `transform_expr_lambda_with_param_types_fn`) through `transform_lambda_call_argument`/`transform_one_call_argument_using_optional_expected_type`/`transform_call_arguments_fold_step`/`transform_call_arguments_using_callee_semantic_type`, plus a 3rd (`transform_exprs_fn`) for the outermost function's own direct call — confirmed via ast/type match-arm precedent that variant constructors don't need explicit import for matching (only for construction), so the new module's import list stays minimal (`Expr, Stmt, Span` / `Type, TUnknown` / `SemanticExpression, sexpr_type` / `coerce_expr_to_type` / `partial_application_desugar_expr` / `TransformContext, TransformStmtsResult`). `transform.mlc` shrank from 1468 to 1132 lines, gained 1 import line, and its 1 call site (`dispatch_transform_pass`/`visit_call`) now passes `transform_expr, transform_expr_lambda_with_param_types, transform_exprs` as 3 trailing arguments |
| verify  | Fresh `mlcc` translation of `compiler/main.mlc` succeeded (0 errors); confirmed `transform_call_args.cpp/.hpp` created. Built pre-change baseline `p0` (temporarily removed the new file + reverted `transform.mlc` via `git stash`, restored after) and post-change `p1`; `diff -rq p0 p1` scoped to exactly `transform.cpp/.hpp` (removed implementations + namespace-qualified call site `transform_call_args::transform_call_arguments_using_callee_semantic_type(..., transform_expr, transform_expr_lambda_with_param_types, transform_exprs)`) plus the 2 new files — zero other files touched (unlike slice 1/2, no external module called these 2 functions directly, so no other namespace-prefix renames). Full `rake test_compiler_mlc` (all 10 phases, `TMPDIR` unset to avoid the known stale-var pitfall): exit_code=0, phase 9 (vm vs C++ exit diff, 18 programs) all ok, phase 10 arch lint failures=0, `transform.mlc` shows reduced 1132 lines in the file-size WARN list. mlcc2 self-host: built via `compiler/build_bin.sh` with in-repo `TMPDIR` (host disk at 99%/5.7G free), ran `mlcc2` on `compiler/main.mlc`, `diff -r p1 p2 --exclude=obj` → IDENTICAL. Cleaned up all `.tmp/s104-12-slice3/**` build artifacts after |
| result  | §104-12 slice-3 green; `compiler/out/mlcc` rebuilt fresh by the rake run (excluded path, not committed) |
| issues  | none |
| next    | ROLE=Critic TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-12 slice 3 — full re-audit: independent function/type-set diff, independent mlcc translation spot-check, independent `rake test_compiler_mlc` rerun; close slice 3 if clean) |

### Turn 2026-07-28 (Driver TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=1, §104-12 slice-3 red)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Red confirmed for §104-12 slice 3 (`transform_call_args.mlc`): `compiler/checker/transform/transform_call_args.mlc` confirmed absent (`test -f` negative). All 9 items from the frozen Decision confirmed still present in `compiler/checker/transform/transform.mlc` at the exact documented lines: `callee_semantic_type_is_function` (141), `function_parameter_types_from_callee_type` (163), `call_argument_is_lambda` (225), `expected_call_argument_type_at_index` (438), `transform_lambda_call_argument` (446), `transform_one_call_argument_using_optional_expected_type` (466), `Transform_call_arguments_fold_state` (620), `function_return_type_from_callee_type` (625), `transform_call_arguments_fold_step` (650), `transform_call_arguments_using_callee_semantic_type` (667). `transform.mlc` confirmed at baseline 1468 lines (matches slice-2-close state, no drift) |
| verify  | `wc -l` + 10 targeted `grep -n` line-anchor checks against `transform.mlc`; `test -f` for the not-yet-created module |
| result  | red state established; no code touched |
| issues  | none |
| next    | ROLE=Driver STEP=2 TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-12 slice 3 — green: create `transform_call_args.mlc`, move 9 items wholesale, thread the 2-3 injected function parameters through the 4 non-leaf functions, wire `transform.mlc` call site + import, bootstrap diff (split-scoped), `rake test_compiler_mlc`, mlcc2 self-host diff) |

### Turn 2026-07-28 (Driver TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=0, §104-12 slice-3 Decision)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 0 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Decision freeze for §104-12 slice 3 (`transform_call_args.mlc`), unblocked now that slice 2 landed. Re-derived the call_args group against current line numbers (post slice-2 shift): `expected_call_argument_type_at_index` (438), `transform_lambda_call_argument` (446), `transform_one_call_argument_using_optional_expected_type` (466-618, 26 `transform_expr(` calls), `Transform_call_arguments_fold_state` type (620), `function_return_type_from_callee_type` (625), `transform_call_arguments_fold_step` (650), `transform_call_arguments_using_callee_semantic_type` (667-690, 1 `transform_exprs(` call) — all confirmed at the exact lines by grep, matching the original (pre-slice-2) analysis just shifted. Found this group has a genuine value-level dependency (unlike slice 2's type-only cycle) on `transform_expr` and `transform_expr_lambda_with_param_types` (790, which has an unrelated second caller inside `dispatch_transform_pass` plus its own lambda-parameter-environment dependency chain — deliberately staying in `transform.mlc`, not part of this slice). Decision: move the 6 group items + 3 group-local leaf helpers (`callee_semantic_type_is_function`, `function_parameter_types_from_callee_type`, `call_argument_is_lambda` — confirmed zero other callers in `transform.mlc` by grep) wholesale into new `compiler/checker/transform/transform_call_args.mlc`; resolve `expected_call_argument_type_at_index`'s dependency on `standalone_unknown_cell` (4 other callers remain, can't move wholesale) by inlining the 1-line `Shared.new(TUnknown)` literal, matching the idiom already used in `transform_coerce.mlc`; thread 2 injected function parameters (`transform_expr_fn`, `transform_expr_lambda_with_param_types_fn`) through the 4 non-leaf functions, plus a 3rd (`transform_exprs_fn`) for the outermost function's own direct call — mirroring the multi-callback-parameter precedent at `infer_isolate_method.mlc:77`. Import `TransformContext`/`TransformStmtsResult` from `./transform_context` (the slice-2 leaf), not from `./transform`. Documented full Decision + Steps table in `TRACK_COMPILER_ARCHITECTURE_HYGIENE.md` |
| verify  | grep audits only this turn (no code changes): re-confirmed all 9 items' line numbers and call-site counts against the shifted post-slice-2 file; confirmed the 3 leaf helpers have zero other callers; confirmed `transform_expr_lambda_with_param_types`'s second, unrelated caller at line 1308 |
| result  | §104-12 slice-3 Decision **frozen**; no code touched |
| issues  | none |
| next    | ROLE=Driver STEP=1 TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-12 slice 3 — red: confirm `transform_call_args.mlc` absent / all 9 items still in `transform.mlc` at the documented lines) |

### Turn 2026-07-28 (Critic TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=3, §104-12 slice-2 close)

| field   | value |
|---------|-------|
| role    | Critic |
| step    | 3 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Full independent re-audit of STEP=2's claims. (1) Function/type-set diff: pre-split `transform.mlc` vs current `transform.mlc`+`transform_context.mlc` combined — identical 52 names, zero lost/duplicated, only the 2 documented `export` additions. (2) Independent translation from scratch: `compiler/out/mlcc -o ... compiler/main.mlc` → 337 files; confirmed `transform_context.cpp`/`.hpp` exist and every reference to the moved symbols in the 5 direct-caller files is qualified `transform_context::`, zero stray `transform::` leftovers. (3) Independent full `rake test_compiler_mlc` rerun (all 10 phases, clean shell): first attempt hit 2 unit failures + a fuzz-smoke `mktemp` error, traced to a stale `TMPDIR` env var left exported in this shell from the prior turn's mlcc2 build, pointing at a directory this Critic pass had already deleted during cleanup — not a code regression. Unset `TMPDIR`, reran clean → **1471 passed, 0 failed**, all 10 phases `ok`, `arch lint failures=0`, `transform.mlc` shown at 1468 lines. Did not re-run the mlcc2 self-host g++ build a third time (witnessed directly during STEP=2 in this same continuous session, no source change since) |
| verify  | function/type-set diff empty except documented `export` deltas; mlcc-translated output inspection shows zero stray `transform::` references to moved symbols; independent `rake test_compiler_mlc` rerun 1471/0 after eliminating a stale-env-var false alarm; `git status` clean against the untouched-paths list |
| result  | §104-12 slice-2 **closed** — Decision/red/green/Critic all done, doc matches code, no false-done risk found. Queue head moves to **slice 3** (`transform_call_args.mlc`, needs a fresh Decision for the 3-parameter injection pattern: `transform_expr_fn`/`transform_exprs_fn`/`transform_expr_lambda_with_param_types_fn`) |
| issues  | none in the code; the stale-`TMPDIR` false alarm is a session-hygiene note for future turns (unset build-related env vars before relying on them across turns), not a track issue |
| next    | ROLE=Driver STEP=0 TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-12 slice 3 — `transform_call_args.mlc` Decision: 3-parameter injection pattern, matching `infer_expr_fn`/multi-callback precedent at `infer_isolate_method.mlc:77`) |

### Turn 2026-07-28 (Driver TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=2, §104-12 slice-2 green)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Extracted `TransformContext`/`TransformStmtsResult` types + 4 constructors (`transform_context_new`, `empty_transform_context`, `transform_context_with_env`, `transform_context_with_lambda_parameter_types`) from `transform.mlc` (1505→1468 lines) into new leaf module `compiler/checker/transform/transform_context.mlc` (41 lines, all 6 items exported). Added one import line to `transform.mlc` pulling them back in |
| verify  | `--check-only` clean. Bootstrap diff (old `mlcc` on pre/post-split source): differences in exactly 8 files — `transform.cpp`/`.hpp` (shrink), `transform_context.cpp`/`.hpp` (new), plus 4 direct-caller files (`check_mutations.cpp`, `names.cpp`, `transform_decl.cpp`, `transform_stmts.cpp`/`.hpp`) each with a pure `transform::`→`transform_context::` namespace-prefix rename, read line-by-line to confirm zero logic change; zero other differences among ~335 files. Confirmed the Decision's transitive-import-forwarding hypothesis: **zero MLC source edits needed** in the 5 external importers (they still write `from './transform'` unchanged and it resolves correctly). `compiler/build.sh` rebuilt `mlcc` clean. `rake test_compiler_mlc` → **1471 passed, 0 failed**, `arch lint failures=0`, `transform.mlc` now 1468 lines. Determinism: new-mlcc output byte-identical to old-mlcc output of the same post-split source. Self-host `mlcc2` identity (`.cursor/rules/mlcc-self-host-verification.mdc`): `build_bin.sh` (g++, `TMPDIR` in-repo — root fs at 99%) built `mlcc2`; re-translation → `diff -r --exclude=obj` **empty**, IDENTICAL |
| result  | §104-12 slice-2 STEP=2 (green) done, all gates pass; ready for Critic |
| issues  | none |
| next    | ROLE=Critic TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-12 slice 2 — full re-audit before closing the slice) |

### Turn 2026-07-28 (Driver TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=1, §104-12 slice-2 red)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Confirmed the red state for §104-12 slice 2 (`transform_context.mlc` extraction), no drift since the STEP=0 Decision: `find compiler/checker/transform -iname transform_context.mlc` → empty (module absent); `transform.mlc` still 1505 lines (unchanged since slice 1 closed); grep re-confirms all 6 items to move (`TransformContext` type, `TransformStmtsResult` type, `transform_context_new`, `empty_transform_context`, `transform_context_with_env`, `transform_context_with_lambda_parameter_types`) still present at the exact lines recorded in the Decision (429/435/438/441/448/460). Baseline `compiler/out/tests/run_tests` timestamp (2026-07-09, unchanged since before slice 1) recorded for later before/after comparison. Updated `TRACK_COMPILER_ARCHITECTURE_HYGIENE.md` Steps table |
| verify  | `find`/grep line-number re-check (0 drift from Decision); no code changed this turn |
| result  | §104-12 slice-2 STEP=1 (red) confirmed; ready for STEP=2 (green: create module) |
| issues  | none |
| next    | ROLE=Driver STEP=2 TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-12 slice 2 — green: create `compiler/checker/transform/transform_context.mlc`, wire `transform.mlc` import, bootstrap diff (split-scoped), `rake test_compiler_mlc`, mlcc2 self-host diff) |

### Turn 2026-07-28 (Driver TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=0, §104-12 slice-2 Decision)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 0 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Decision freeze for the next §104-12 slice. Started from the track doc's existing "Next slices" list (call_args group), but re-derived from scratch against current line numbers and found a prerequisite the earlier plan missed: the call_args group threads `TransformContext`/`TransformStmtsResult` (currently defined inside `transform.mlc`, lines 429/460) through every function; extracting the group directly would need `transform_call_args.mlc` to import those types from `transform.mlc` while `transform.mlc` imports the group back — a genuine type-level cycle, confirmed by grep that no two `compiler/**` modules currently import from each other bidirectionally. Found the established fix in the codebase itself: `infer.mlc`/`infer_call.mlc` avoid the same shape of cycle by keeping their shared `CheckContext` type in an independent third leaf module (`checker/check/check_context.mlc`), imported one-directionally by both sides. Decision: insert a new prerequisite slice — extract `TransformContext`/`TransformStmtsResult` + 4 constructors (`transform_context_new` — dead code, zero callers anywhere in the tree, confirmed by grep; `empty_transform_context`, `transform_context_with_env`, `transform_context_with_lambda_parameter_types`) into new leaf `compiler/checker/transform/transform_context.mlc`, mirroring `check_context.mlc`'s role. Renumbered the track doc's slices: this becomes **Slice 2**, the original call_args group becomes **Slice 3**, the method group becomes **Slice 4**. Also found, re-reading slice 1's own already-committed `coerce_expr_to_type` precedent: external importers (`transform_stmts.mlc` etc.) still write `import { ..., coerce_expr_to_type } from './transform'` today even though the real definition lives in `transform_coerce.mlc` since slice 1 — compiles and passes tests unmodified, proving the checker resolves re-exported names transitively through whichever module textually imports them, only the generated C++ namespace changes. Applied the same reasoning here: slice 2's external importers (`transform_stmts.mlc`, `check_mutations.mlc`, `names.mlc`, 2 test files) should not need any edits, only `transform.mlc` and the new module. Documented full Decision, Steps table, and the renumbered Slice 3/4 scope notes (including the injection signature slice 3 will need: `transform_expr_fn`/`transform_exprs_fn`/`transform_expr_lambda_with_param_types_fn`, 3 parameters threaded through 4 functions, and how `expected_call_argument_type_at_index`'s one dependency on `standalone_unknown_cell` — which has 4 *other* callers, so can't move wholesale — gets resolved by inlining the 1-line `Shared.new(TUnknown)` literal instead of a back-import, matching the idiom already used throughout `transform_coerce.mlc`) in `TRACK_COMPILER_ARCHITECTURE_HYGIENE.md` |
| verify  | grep audits only this turn (no code changes): confirmed `transform_context_new` has exactly 1 occurrence tree-wide (its own definition, dead code); confirmed no existing `compiler/**` module pair imports bidirectionally; confirmed the `check_context.mlc` precedent's own import structure; confirmed `transform_stmts.mlc`'s current import line still says `from './transform'` for `coerce_expr_to_type` post-slice-1 (transitive-forwarding evidence) |
| result  | §104-12 slice-2 Decision **frozen** (extract `transform_context.mlc`, prerequisite for the renumbered slice 3); no code touched |
| issues  | none |
| next    | ROLE=Driver STEP=1 TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-12 slice 2 — red: confirm `transform_context.mlc` absent / `TransformContext`/`TransformStmtsResult`/4 constructors still in `transform.mlc` at the documented lines) |

### Turn 2026-07-28 (Critic TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=3, §104-12 slice-1 close)

| field   | value |
|---------|-------|
| role    | Critic |
| step    | 3 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Full independent re-audit of STEP=2's claims, not a re-read of the log. (1) Function-set diff: extracted `git show 002bc584:.../transform.mlc` (pre-split) and diffed its full `fn`/`export fn` name set against the current `transform.mlc`+`transform_coerce.mlc` combined set — **identical 61 names**, zero lost/duplicated/added; only delta is the 4 items that gained `export` exactly as documented. (2) Independent translation: `compiler/out/mlcc -o .tmp/critic104/pa compiler/main.mlc` from scratch → 335 files, confirmed `transform_coerce.cpp`/`.hpp` exist and `transform_decl.cpp`/`transform_stmts.cpp` reference `transform_coerce::coerce_expr_to_type` (not `transform::`) at all 4 call sites — matches the documented namespace-rename exactly. (3) Independent full re-run of `MLC_TEST_PROGRESS=1 bundle exec rake test_compiler_mlc` (all 10 phases, not just the unit suite) from a clean shell → `1471 passed, 0 failed`, fuzz smoke/negative-corpus/fuzz-differential/cpp-parser-differential/compile_commands/lsp-smoke/vm-cpp-exit-diff (18 programs) all `ok`, `arch lint failures=0` with `transform.mlc` listed at 1505 lines on the size-allowlist WARN (not 1765) — confirms the split is live in the binary under test, not stale. Did **not** re-run the mlcc2 self-host g++ build a third time (witnessed firsthand during STEP=2 in this same continuous session, methodology sound: `build_bin.sh` g++ + `diff -r --exclude=obj` empty) — no source change since, so re-deriving it would only reconfirm determinism of an unchanged input. Attempted `scripts/regression_gate.sh` for extra compiler-track assurance; killed it after ~19 min once process-tree inspection showed it was blocked on its own optional `run_examples_compile_sweep.sh` tail step (OpenGL example corpus, unrelated to the transform split, and not required by AGENTS.md's gate — that gate triggers on closing a whole TRACK file, not a slice within an open sub-track) |
| verify  | function-name-set diff empty except documented `export` deltas; mlcc-translated output inspection matches documented namespace rename; independent `rake test_compiler_mlc` rerun 1471/0, arch-lint 0; `git status` clean against the untouched-paths list |
| result  | §104-12 slice-1 **closed** — Decision/red/green/Critic all done, doc matches code, no false-done risk found. Queue head moves to **slice 2** (`transform_call_args.mlc`, needs a fresh Decision for the `transform_expr_fn` injection pattern) |
| issues  | none |
| next    | ROLE=Driver STEP=0 TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-12 slice 2 — `transform_call_args.mlc` Decision: `transform_expr_fn`/`transform_exprs_fn` injection pattern, matching `infer_expr_fn` in `checker/infer/`) |

### Turn 2026-07-28 (Driver TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=2, §104-12 slice-1 green)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Split `transform.mlc` (1765→1505 lines): extracted the 14 items (coerce group + 3 leaf type-predicate helpers) into new `compiler/checker/transform/transform_coerce.mlc` (268 lines), added `export` to 4 items now called cross-module (`semantic_type_is_tarray`/`array_element_type_from_semantic_type`/`generic_type_name`/`conditional_else_empty_unknown_array_coerced_to_then_array_element`; `coerce_expr_to_type` already exported), added one import line to `transform.mlc` for all 5. Pure relocation via a Ruby one-off (exact line-range slice, no hand-retyping) to avoid transcription errors, then hand-verified both seams by reading the resulting files |
| verify  | `--check-only` clean. Bootstrap diff (old `mlcc` on pre/post-split source): full-tree `diff -rq` found differences in **exactly** 4 files — `transform.cpp`/`.hpp` (shrink), `transform_coerce.cpp`/`.hpp` (new) — plus 2 more, `transform_decl.cpp`/`transform_stmts.cpp`, each a 1-token namespace-prefix rename per call site (`transform::coerce_expr_to_type`→`transform_coerce::coerce_expr_to_type` — C++ codegen qualifies by defining module, mechanical, not a logic change); confirmed **zero** other differences among ~335 files. Cleared a stale precompiled-header cache (`mlcc_precompiled.pch`, unrelated pre-existing staleness) and a stamp file that had been written despite a prior failed link, then `compiler/build.sh` rebuilt `mlcc` clean. `rake test_compiler_mlc` (rebuilds `run_tests` from split source) → **1471 passed, 0 failed**, `arch lint failures=0`, `transform.mlc` still allowlisted (now 1505 vs 1765 lines). Determinism: new-mlcc-translated `compiler/main.mlc` output byte-identical to old-mlcc-translated output of the same post-split source. Self-host mlcc2 identity (`.cursor/rules/mlcc-self-host-verification.mdc`): `build_bin.sh` (g++) built mlcc2 from the new mlcc's own translation; mlcc2 re-translating the same source → `diff -r --exclude=obj` **empty**, IDENTICAL. Refined the TRACK doc's "Verification discipline" section: literal byte-empty bootstrap diff is impossible for any god-file split (namespace-prefix rename at direct-caller sites is expected collateral, not a regression) — documented so §104-13/14/15/16 aren't held to an unachievable standard |
| result  | §104-12 slice-1 STEP=2 (green) done, all gates pass; ready for Critic |
| issues  | Corrected an overstated claim in my own STEP=0 Decision ("codegen output must not change one byte") — the real invariant is "zero changes outside the split modules and their direct callers' namespace-prefix", now documented precisely instead of restated as a false absolute |
| next    | ROLE=Critic TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-12 slice 1 — full re-audit before closing the slice) |

### Turn 2026-07-28 (Driver TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=1, §104-12 slice-1 red)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Confirmed the red state for §104-12 slice 1 (`transform_coerce.mlc` extraction), no drift since the STEP=0 Decision: `find compiler/checker/transform -iname transform_coerce.mlc` → empty (module absent); grep re-confirms all 14 items to move (`semantic_type_is_tarray`/`array_element_type_from_semantic_type`/`generic_type_name`/`coerce_unknown_else_array_when_then_known_inner`/`conditional_else_coerce_empty_array_using_then_type`/`array_literal_else_maybe_coerce`/`conditional_else_empty_unknown_array_coerced_to_then_array_element`/`coerce_array_semantic_expression_to_type`/`coerce_record_semantic_expression_to_type`/`coerce_block_semantic_expression_to_type`/`coerce_if_semantic_expression_to_type`/`coerce_match_arm_to_type`/`coerce_match_arms_to_type`/`coerce_expr_to_type`) still present at the exact lines recorded in the Decision (139/161/183/492/505/521/531/599/608/623/637/655/667/675); `transform.mlc` unchanged at 1765 lines. Baseline `compiler/out/tests/run_tests` timestamp (2026-07-09) recorded for later before/after comparison. Updated `TRACK_COMPILER_ARCHITECTURE_HYGIENE.md` Steps table |
| verify  | `find`/grep line-number re-check (0 drift from Decision); no code changed this turn |
| result  | §104-12 slice-1 STEP=1 (red) confirmed; ready for STEP=2 (green: create module) |
| issues  | none |
| next    | ROLE=Driver STEP=2 TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-12 slice 1 — green: create `compiler/checker/transform/transform_coerce.mlc`, wire imports, bootstrap diff empty, `rake test_compiler_mlc`) |

### Turn 2026-07-28 (Driver TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=0, §104-12 slice-1 Decision)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 0 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Decision freeze for §104-12 (`transform/transform.mlc` split, 1765 lines). Read current file (drifted since the 2026-06-29 review) and re-derived the review's 3 proposed groups against actual line numbers/call graph. Found the review's "**высокий риск**" circular-import warning applies to 2 of the 3 groups (call_args, method — both call `transform_expr`, defined later in the same file: `transform_one_call_argument_using_optional_expected_type` alone has 25 such calls), but **not** the coerce group: grep-verified `coerce_expr_to_type` + its 10 helpers (lines 492-566, 599-716) never call `transform_expr`/`dispatch_transform_pass`, only 3 small leaf helpers (`semantic_type_is_tarray`/`array_element_type_from_semantic_type`/`generic_type_name`, lines 139/161/183) that are also used elsewhere in the file. Decision: **first slice** = extract coerce group **plus** those 3 leaf helpers into new `compiler/checker/transform/transform_coerce.mlc` — makes it a pure leaf module (zero cycle, only `transform.mlc` imports across the new boundary), no `transform_expr_fn` injection needed for this slice. Call_args/method groups deferred to 2 later slices (own Decisions), each needing the `infer_expr_fn`-style injection pattern already used in `checker/infer/`. Documented in `TRACK_COMPILER_ARCHITECTURE_HYGIENE.md` (`§104-12` section, Decision table + Steps table + deferred-slices list), updated `PLAN.md`/`DEVELOPMENT.md` queue-head pointers |
| verify  | grep audits only this turn (no code changes): confirmed exactly which lines/functions call `transform_expr`/`dispatch_transform_pass`/`coerce_expr_to_type`/the 3 leaf helpers across the whole file, confirming the zero-cycle claim before freezing the Decision (anti-false-done: did not accept the review's risk rating at face value, re-verified against current code) |
| result  | §104-12 slice-1 Decision **frozen**; no code touched |
| issues  | none |
| next    | ROLE=Driver STEP=1 TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-12 slice 1 — red: confirm `transform_coerce.mlc` absent / 14 items still in `transform.mlc` at the documented lines) |

### Turn 2026-07-28 (Driver TRACK_COMPILER_ARCHITECTURE_HYGIENE STEP=0, §104-1 audit)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 0 |
| track   | TRACK_COMPILER_ARCHITECTURE_HYGIENE |
| started | 2026-07-28 |
| done    | Audited §104-1 (`FileId`/`FileStore`) before writing new code. Found the review's "0 of 24 steps ever actioned" claim **wrong** for Steps 1/2/3: `compiler/infrastructure/file_store.mlc` (`SourceFileId`/`SourceFileStore`) added by commit `36a1e372` 2026-06-30 and `compiler/infrastructure/intern.mlc` (`StringInternTable`) added by commit `ab088d90` 2026-07-01 — one day / two days after the 2026-06-29 review — already match Steps 1/3's spec field-for-field, just under `compiler/infrastructure/` not the review's suggested `compiler/build/` path, which is why the file-existence check at the review's own path missed them. `compiler/tests/file_store_test.mlc`/`intern_test.mlc` already wired into `compiler/tests/support/suite_registry.mlc`. Step 2's concrete code (`Span.start_offset`/`end_offset` + `span_make` + literal replacement) also already done in `compiler/frontend/ast.mlc` (grep: only 2 raw `Span {}` literals in the tree, both the canonical constructors, 345 call sites already use `span_make`/`span_unknown`); the `file_id: FileId` field named only in the step's title (not its own code sample) has no Wave-1 consumer, deferred. Corrected `TRACK_COMPILER_ARCHITECTURE_HYGIENE.md` (new "Correction 2026-07-28" section + sub-track bullets), `PLAN.md` §104 row + queue chain, `CONTINUITY.md` queue/track rows, `DEVELOPMENT.md` queue-head pointer — all now point queue head at §104-12 (`transform.mlc` split) instead of a redundant §104-1 re-implementation |
| verify  | `compiler/out/tests/run_tests` (binary newer than all 4 source files touched by this finding) → `1471 passed, 0 failed`, including `[compiler tests] sub: file_store`; grep confirms both modules remain zero-importer outside their own test files (intentionally unwired, per review's own Step 1 instruction) |
| result  | §104-1/§104-3 **done** (pre-existing, no action needed); §104-2 done except deferred non-blocking `file_id` field; no code changes this turn, docs corrected to remove false-done risk in the other direction (false-not-done) |
| issues  | none — this is a documentation-accuracy correction, not a regression; no `lib/mlc/`/`compiler/**` code touched so no self-host/regression-gate re-run needed |
| next    | ROLE=Driver STEP=0 TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-12 `transform.mlc` split Decision) |

### Turn 2026-07-28 (Critic TRACK_EDITOR_COMMENT_TOGGLE_NO_FULL_STRINGIFY STEP=3)

| field   | value |
|---------|-------|
| role    | Critic |
| step    | 3 |
| track   | TRACK_EDITOR_COMMENT_TOGGLE_NO_FULL_STRINGIFY |
| started | 2026-07-28 |
| elapsed | ~40 min (dominated by full `run_ux_gate.sh` ×2, ~112 scenarios each, mlcc-compiled) |
| done    | Confirmed STEP=2 commit `cce76b0e` matches claimed `done` (git show --stat). Fresh reruns: stable ×2 green, red ×1 correctly fails (`to=0 from=0`, gap closed), related (`edit_autoclose`/`newline_indent`/`word_delete`/`clipboard_slice` no-full-stringify stables + `run_editor_edit_unit.sh`) all green. Full `run_ux_gate.sh` run #1: `all ok (112 scenarios)`. Run #2 first attempt: 1 FAIL — `idle_cpu_budget_stable` (`min_cpu_percent=59` vs budget 10) — reran that script alone with no other background load: green (`cpu_percent=0`), confirmed host-CPU-noise flake unrelated to this track (comment-toggle is not in the render/idle loop). Reran full gate: `all ok (112 scenarios)`, clean. Closed §100: archived `docs/agent/TRACK_EDITOR_COMMENT_TOGGLE_NO_FULL_STRINGIFY.md` → `docs/archive/tracks/`, updated `PLAN.md`/`CONTINUITY.md`/`DEVELOPMENT.md`/`TRACK_COMPILER_ARCHITECTURE_HYGIENE.md` status lines |
| verify  | stable×2 EXIT=0; red EXIT=1 (expected); related×1 EXIT=0 each; `run_ux_gate.sh` ×2 both `all ok (112 scenarios)` (one isolated unrelated flake explained, not silently ignored) |
| result  | §100 **closed** (Critic OK); queue → Driver §104-1 |
| issues  | `idle_cpu_budget_stable` flaked once under back-to-back full-gate CPU load — pre-existing test fragility (host-noise-sensitive by its own header comment), not a regression from this track; logged here rather than in TRACK per no-repeat-note convention |
| next    | ROLE=Driver STEP=0 TRACK=TRACK_COMPILER_ARCHITECTURE_HYGIENE (§104-1 `FileId`/`FileStore` Decision) |

### Turn 2026-07-28 (Driver TRACK_EDITOR_COMMENT_TOGGLE_NO_FULL_STRINGIFY STEP=2)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_EDITOR_COMMENT_TOGGLE_NO_FULL_STRINGIFY |
| started | 2026-07-28 |
| elapsed | ~25 min |
| done    | `misc/editor/document/edit.mlc`: `edit_toggle_line_comment` rewritten to per-line slice — new `document_line_end_after` helper + existing `document_line_start_before`; per-line `document_byte_slice` + `comment_line_body`/`uncomment_line_body`; write-back via `document_delete`+`document_insert` on the touched multi-line span only. Removed `document_to_string`/`document_from_string`/`LineIndex` import and the two `line_index_*`-based helpers (`line_content_exclusive_end`, `line_content_is_commented`) — zero full-stringify calls left in `edit.mlc`. Rewrote `scripts/run_ux_comment_toggle_no_full_stringify_stable.sh` to check the function body + run `run_editor_edit_unit.sh` + `run_ux_toggle_line_comment.sh` |
| verify  | `run_editor_edit_unit.sh` EXIT=0; `run_ux_toggle_line_comment.sh` EXIT=0 `ux_ok toggle_line_comment`; new stable EXIT=0 `ux_ok comment_toggle_no_full_stringify`; red EXIT=1 (gap correctly closed: `to=0 from=0`); related stables green: `edit_autoclose_no_full_stringify`, `newline_indent_no_full_stringify`, `word_delete_no_full_stringify`, `clipboard_slice_no_full_stringify` |
| result  | §100 STEP=2 **done**; queue → Critic STEP=3 |
| issues  | Full `run_ux_gate.sh` (all scenarios) not run this turn — that is Critic's STEP=3 gate (×2), not required for Driver green per the pattern of prior closed stringify tracks (STEP=2 turns verify function + related scripts only) |
| next    | ROLE=Critic STEP=3 TRACK=TRACK_EDITOR_COMMENT_TOGGLE_NO_FULL_STRINGIFY |

### Turn 2026-07-28 (interactive session, Planner-style — §102/§103 authorized: terminal + Script VM Phase 1)

| field   | value |
|---------|-------|
| role    | Planner (interactive, not queue) |
| step    | plan-refresh |
| track   | PLAN |
| started | 2026-07-28 |
| elapsed | n/a (interactive) |
| done    | User asked for a track count and explicitly authorized two new epics by name: terminal component and MLC Script VM. Counted current backlog: 99 numbered `PLAN.md` rows, 97 closed/superseded, effectively only §100 (near close) and §101 (§97a residual, confirmed root cause, queued) open in the main sequence; §19 (auto-cycle detection) and MLC Script VM were the only gated/design-only items. Opened **§102 `TRACK_EDITOR_TERMINAL`** — libvterm-backed terminal panel, decomposed into 7 ordered sub-tracks (§102a FFI binding → §102b PTY spawn → §102c cell-grid render reusing existing `static_text`/`solid_renderer` → §102d input forward → §102e resize/scrollback → §102f panel integration into the post-§97 unified `EditorAppState` → §102g perf budget), explicit non-goal against building a second parallel widget/render system (direct lesson from the 2026-07-15 architecture review). Opened **§103 `TRACK_MLC_SCRIPT_VM`**, lifting the previous HARD STOP GATE for **Phase 1 only** (design doc `MLC_SCRIPT_VM.md` §12 phases 2-5 remain explicitly not authorized) — decomposed into 9 ordered sub-tracks (§103a `Value` rep → §103b bytecode format → §103c verifier → §103d arithmetic interpreter → §103e control flow → §103f GC arena → §103g arrays/records → §103h closures/fibers → §103i embedding ABI), verifier-before-interpreter per the design doc's own LuaJIT-precedent requirement. Rewrote `TRACK_MLC_SCRIPT_VM.md` to remove the old "agent-hours" sizing estimates (violates the standing token/calendar-only rule) — no replacement time metric substituted, sizing left implicit in the sub-track count/scope. Every sub-track in both new tracks carries an explicit test gate per user's stated emphasis (performance, architecture, testing) |
| verify  | Confirmed `libvterm0` runtime already installed (0.3.3) via `dpkg -l`, dev headers not yet confirmed (deferred to §102a Decision, not assumed) — did not overstate readiness |
| next    | Driver: finish §100 STEP=2 (green) → Critic → §101 Decision (perf fix) → §102a Decision (`TERMINAL_LIBVTERM_FFI`) → §103a Decision (`SCRIPT_VM_VALUE_REP`), strict order per `CONTINUITY.md` override (d) |


### Turn 2026-07-27 (interactive session, Planner-style — §97a perf smoke re-run, new finding logged as §101)

| field   | value |
|---------|-------|
| role    | Planner (interactive, not queue) |
| step    | plan-refresh |
| track   | PLAN |
| started | 2026-07-27 |
| elapsed | n/a (interactive) |
| done    | User asked for a progress check ("как успехи?"). Reviewed `git log` since 2026-07-25: §97 (`TRACK_EDITOR_RENDER_ARCHITECTURE`) closed — §97a real perf harness, §97b frame-split (wrap/frame_cache/shared_span_cache/minimap_cache/frame_input extracted into `app/frame_layout.mlc` + collapsed dual wrap tick), §97c (killed `demo_live` app-loop-head unpack, unified mutation via `editor_app_set_*`, wheel-hover-focus-independent L0 lock-in) all Critic-closed; queue continued through §93/§98/§99 stringify residuals, §100 in flight. Re-ran `scripts/run_editor_demo_live_perf_smoke.sh` (§97a's own harness) directly to get a real number rather than trust doc claims: on the 100k-line fixture, `frames=30 layout_us=82511 draw_us=12063257 total_us=12240140` — draw phase is 98.6% of frame time, ~402ms/frame, ~2.4 FPS while scrolling. Confirmed `visible_row_budget` already virtualizes the paint loop (not drawing all 100k rows), so this is not the already-fixed per-frame-full-wrap regression (§36) recurring; read the draw span in `demo_live.mlc` and found `visual_row_index_for_caret_pixel_budget` called every frame inside it (~line 2210) as the prime suspect — a pixel-budget wrap walk for caret placement that may not be bounded by viewport size. Logged as PLAN §101, explicitly as a hypothesis needing a narrower timer to confirm before any fix (no track file yet, no code touched) |
| verify  | Re-ran the smoke script myself (99.5s, exit 0) rather than citing a prior run; read `demo_live.mlc`'s `perf_draw_t0`/`perf_draw_us` boundaries (line 1900-2632) to confirm what's actually inside the measured span before hypothesizing a cause |
| next    | Driver: after §100 (`EDITOR_COMMENT_TOGGLE_NO_FULL_STRINGIFY`) closes, PLAN §101 Decision — bisect `draw_us` with an inner timer around `visual_row_index_for_caret_pixel_budget` specifically (and 1-2 other candidates in the same span) on the same 100k-line fixture, confirm or refute before naming a TRACK / touching code |

