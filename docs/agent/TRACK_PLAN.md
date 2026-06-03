# Track: mlcc optimization (plan 2026-05)

## Status: **closed** (commit `419de62`)

Baseline: `benchmarks/profile/record_baseline.sh` (post-commit).

---

# Track: PLAN.md roadmap

Parent: [../PLAN.md](../PLAN.md) §3 visitor + §4 folder restructure

## Status: folder restructure **done** (steps 1–23)

| Step | Item | Status |
|------|------|--------|
| 1 | `compiler/pass.mlc` — `trait CompilerPass<Input, Output>` | done (`f199e92`) |
| 2 | `ExprVisitor` sketch + dispatch stub | done (`f199e92`) |
| 3 | `ExprVisitor<string>` codegen sketch | done (`df3826f`) |
| 4 | `dispatch_expr` + test graph | done (`161c3b4`) |
| 5 | Wire `gen_expr_via_string_visitor` into `expr_eval` | done (`0afcfd8`) |
| 6 | ExprVisitor: bool + unit arms | done (`a9a2ca6`) |
| 7 | ExprVisitor: numeric literal arms | done (`4e293c2`) |
| 8 | ExprVisitor: ident arm | done (`274dd68`) |
| 9 | ExprVisitor: bin/unary ops arm | done (`678e5d0`) |
| 10 | ExprVisitor: call + method arms | done (`b6c857a`) |
| 11 | ExprVisitor: if + block arms | done (`008abce`) |
| 12 | ExprVisitor: match arm | done (`14d9b0d`) |
| 13 | Remaining `expr_eval` → visitor (record, array, lambda, …) | done (`9bc1001`) |
| 14 | Self-host diff after visitor migration batch | done (`6df3799`, docs `0a0d321`) |
| 15 | Parser `ref mut` | **deferred** (separate branch) |
| 16 | Shrink `expr_eval.mlc` — `dispatch_expr` only, drop duplicate `match` | done (`6c5697a`) |
| 17 | Explicit `Pass` pipeline in `main.mlc` | done (`6f998e9`) |
| 18 | Move `frontend/` — lexer, ast, ast_tokens, parser/ | done (`bef5143`) |
| 19 | Move `ir/` — semantic_ir, record_defaults | done (`597b76b`) |
| 20 | Split `checker/` → infer/, transform/, check/ | done (`597b76b`) |
| 21 | Split `codegen/` → expr/, stmt/, decl/ | done (`ef3fedf`; split dirs `6b355c3`) |
| 22 | Move root debug tests → `tests/`; delete junk | done (`318a727`) |
| 23 | Delete stale artefacts; update `.gitignore` | done (`318a727`) |

## Backlog (Planner maintains)

Source: PLAN.md §4 «Порядок миграции» + §Phase 1.

- **Step 15:** parser `ref mut` — separate branch, not in this track
- **CppExpr backend:** [TRACK_CPPEXPR.md](TRACK_CPPEXPR.md) — closed (`96df4e0`)
- **CppAST cleanup:** [TRACK_CPPGEN.md](TRACK_CPPGEN.md) — closed (`005c65b`)
- **Phase 1 stabilization:** [TRACK_PHASE1.md](TRACK_PHASE1.md) — **closed** (`fd42eab`)
- **Safety / fuzz hardening:** [TRACK_SAFETY.md](TRACK_SAFETY.md) — **closed** (`32f8335`)
- **Performance:** [TRACK_PERFORMANCE.md](TRACK_PERFORMANCE.md) — **closed** (`0671422`)
- **Security / fuzz depth:** [TRACK_SECURITY.md](TRACK_SECURITY.md) — **closed** (`a035c3d`)
- **Diagnostics / error codes:** [TRACK_DIAGNOSTICS.md](TRACK_DIAGNOSTICS.md) — **closed** (`f6863fe`)
- **Test coverage:** [TRACK_COVERAGE.md](TRACK_COVERAGE.md) — **closed** (`9bddbb3`)
- **Diagnostics (remaining codes):** [TRACK_DIAGNOSTICS2.md](TRACK_DIAGNOSTICS2.md) — **closed** (`d055c49`)
- **Fuzz differential:** [TRACK_FUZZ_DIFF.md](TRACK_FUZZ_DIFF.md) — **closed** (`5463361`)
- **Parser panic audit:** [TRACK_PARSER_PANIC.md](TRACK_PARSER_PANIC.md) — **closed** (`5525d03`)
- **Checker/codegen spans:** [TRACK_SPAN_CHECKER.md](TRACK_SPAN_CHECKER.md) — **closed** (`a8bf7a1`)
- **Build determinism:** [TRACK_BUILD.md](TRACK_BUILD.md) — **closed** (`1d6f4c5`)
- **SemanticIR decl spans:** [TRACK_SPAN_IR.md](TRACK_SPAN_IR.md) — **closed** (`e826f1a`)
- **Parser parity (negative corpus):** [TRACK_PARSER_PARITY.md](TRACK_PARSER_PARITY.md) — **closed** (`d5442d6`)
- **Expr fragment bridges:** [TRACK_EXPR_BRIDGE.md](TRACK_EXPR_BRIDGE.md) — **closed** (`91ece96`)
- **Match expr bridges:** [TRACK_MATCH_BRIDGE.md](TRACK_MATCH_BRIDGE.md) — **closed** (`3e47ca4`)
- **Stmt string bridges:** [TRACK_STMT_BRIDGE.md](TRACK_STMT_BRIDGE.md) — **closed** (`7084227`)
- **Return-body bridges:** [TRACK_RETURN_BODY.md](TRACK_RETURN_BODY.md) — **closed** (step 5 audit)
- **Prefix return-body bridge:** [TRACK_PREFIX_BRIDGE.md](TRACK_PREFIX_BRIDGE.md) — **closed** (step 5 audit)
- **CppStmtFragment elimination:** [TRACK_FRAGMENT_BRIDGE.md](TRACK_FRAGMENT_BRIDGE.md) — **closed** (`49b7091`)
- **Structured binding fragments:** [TRACK_BINDING_BRIDGE.md](TRACK_BINDING_BRIDGE.md) — **closed** (`3904513`)
- **Native template fn decls:** [TRACK_TEMPLATE_BRIDGE.md](TRACK_TEMPLATE_BRIDGE.md) — **closed** (`6db2014` step 5 audit)
- **Native extend forward segments:** [TRACK_EXTEND_BRIDGE.md](TRACK_EXTEND_BRIDGE.md) — **closed** (`52f5c2f` step 5 audit)
- **Native zero-param main():** [TRACK_MAIN_BRIDGE.md](TRACK_MAIN_BRIDGE.md) — **closed** (`521f574` step 5 audit)
- **Native non-fn decls:** [TRACK_DECL_BRIDGE.md](TRACK_DECL_BRIDGE.md) — **closed** (`9e2d47e`)
- **Native derive methods:** [TRACK_DERIVE_BRIDGE.md](TRACK_DERIVE_BRIDGE.md) — **closed** (step 5 audit, uncommitted)
- **Native trait vtable struct:** [TRACK_TRAIT_STRUCT_BRIDGE.md](TRACK_TRAIT_STRUCT_BRIDGE.md) — **closed** (step 5 audit, uncommitted)
- **Native fn protos (phase 2):** [TRACK_FN_PROTO_BRIDGE.md](TRACK_FN_PROTO_BRIDGE.md) — **closed** (step 5 audit, uncommitted)
- **Native extend helper protos:** [TRACK_EXTEND_HELPER_BRIDGE.md](TRACK_EXTEND_HELPER_BRIDGE.md) — **closed** (step 5 audit, uncommitted)
- **Native SDeclExtend body:** [TRACK_EXTEND_BODY_BRIDGE.md](TRACK_EXTEND_BODY_BRIDGE.md) — **closed** (step 5 audit, uncommitted)
- **Native extend method bodies:** [TRACK_EXTEND_METHOD_BRIDGE.md](TRACK_EXTEND_METHOD_BRIDGE.md) — **closed** (step 5 audit, uncommitted)
- **Native extend trait suffix:** [TRACK_EXTEND_TRAIT_SUFFIX_BRIDGE.md](TRACK_EXTEND_TRAIT_SUFFIX_BRIDGE.md) — **closed** (step 5 audit, uncommitted)
- **Native extend extern stubs:** [TRACK_EXTEND_EXTERN_BRIDGE.md](TRACK_EXTEND_EXTERN_BRIDGE.md) — **closed** (step 5 audit, uncommitted)
- **Native extend non-SDeclFn fallback removal:** [TRACK_EXTEND_FALLBACK_BRIDGE.md](TRACK_EXTEND_FALLBACK_BRIDGE.md) — **closed** (step 5 audit, uncommitted)
- **Native gen_decl_cpp catch-all:** [TRACK_DECL_CPP_CATCHALL_BRIDGE.md](TRACK_DECL_CPP_CATCHALL_BRIDGE.md) — **closed** (step 5 audit, uncommitted)
- **Native gen_proto_cpp catch-all:** [TRACK_PROTO_CPP_CATCHALL_BRIDGE.md](TRACK_PROTO_CPP_CATCHALL_BRIDGE.md) — **closed** (step 5 audit, uncommitted)
- **Native extend helper proto guard:** [TRACK_EXTEND_HELPER_PROTO_GUARD_BRIDGE.md](TRACK_EXTEND_HELPER_PROTO_GUARD_BRIDGE.md) — **closed** (step 5 audit, uncommitted)
- **Native collect_decl_parts_cpp else fallback:** [TRACK_COLLECT_DECL_PARTS_CPP_BRIDGE.md](TRACK_COLLECT_DECL_PARTS_CPP_BRIDGE.md) — **closed** (step 5 audit, uncommitted)
- **Native gen_decl_cpp / gen_proto_cpp print round-trips:** [TRACK_DECL_PRINT_ROUNDTRIP_BRIDGE.md](TRACK_DECL_PRINT_ROUNDTRIP_BRIDGE.md) — **closed** (step 5 audit, uncommitted)
- **Native CppDecl string fragments:** [TRACK_DECL_CPP_FRAGMENT_BRIDGE.md](TRACK_DECL_CPP_FRAGMENT_BRIDGE.md) — **closed** (step 5 audit, uncommitted)
- **Native static_assert decls:** [TRACK_DECL_STATIC_ASSERT_BRIDGE.md](TRACK_DECL_STATIC_ASSERT_BRIDGE.md) — **closed** (step 5 audit, uncommitted)
- **Native multi-decl collapse:** [TRACK_DECL_MULTI_DECL_BRIDGE.md](TRACK_DECL_MULTI_DECL_BRIDGE.md) — **closed** (step 5 audit, uncommitted)
- **Native empty CppDecl:** [TRACK_DECL_EMPTY_FRAGMENT_BRIDGE.md](TRACK_DECL_EMPTY_FRAGMENT_BRIDGE.md) — **closed** (step 5 audit, uncommitted)
- **Structured struct_using:** [TRACK_DECL_STRUCT_USING_BRIDGE.md](TRACK_DECL_STRUCT_USING_BRIDGE.md) — **closed** (step 5 audit, uncommitted)
- **Native std::hash specialization:** [TRACK_DERIVE_HASH_FRAGMENT_BRIDGE.md](TRACK_DERIVE_HASH_FRAGMENT_BRIDGE.md) — **closed** (step 5 audit, uncommitted)
- **Native derive Hash body stmts:** [TRACK_DERIVE_HASH_STMT_BRIDGE.md](TRACK_DERIVE_HASH_STMT_BRIDGE.md) — **closed** (step 5 audit, uncommitted)
- **Native module TU scaffolding:** [TRACK_MODULE_TU_BRIDGE.md](TRACK_MODULE_TU_BRIDGE.md) — **open** (step 3 pending)

## Next step (Driver)

**STEP=3** — TRACK_MODULE_TU_BRIDGE wire `assemble_header_cpp_declarations` → native nodes.

## Planner checklist (2026-06-03 plan-refresh — MODULE_TU open)

- [x] TRACK_DERIVE_HASH_STMT_BRIDGE closed (5/5); stmt fragment **0** in derive hash path
- [x] Next track: MODULE_TU_BRIDGE (`module.mlc` `cpp_decl_from_string_output` TU scaffolding)
- [x] Deferred: type_gen string hash, expr_visitor unit-if fragment, let_pat structured-binding
- [x] verify baseline 928 pass; DIFF_EXIT=0
- [x] Driver enqueued STEP=1 MODULE_TU_BRIDGE

## Planner checklist (2026-06-03 plan-refresh — DERIVE_HASH_STMT close)

- [x] TRACK_DERIVE_HASH_STMT_BRIDGE closed (5/5); stmt fragment **0** in derive hash path
- [x] Survivors deferred: `type_gen` string hash, `stmt_cpp`/`expr_visitor_cpp` fragments, `emit_helpers` export
- [x] verify 928 pass; DIFF_EXIT=0
- [x] plan-refresh: pick next bridge from PLAN §Phase 2 or deferred backlog

## Planner checklist (2026-06-02 plan-refresh — DERIVE_HASH_STMT open)

- [x] TRACK_DERIVE_HASH_FRAGMENT_BRIDGE closed (5/5)
- [x] Next track: DERIVE_HASH_STMT_BRIDGE (3 stmt fragments `:362-370`; continuation of Hash native path)
- [x] Deferred: module TU scaffolding, type_gen string hash, `decl_cpp` string fallback
- [x] verify baseline 920 pass; DIFF_EXIT=0
- [x] Driver enqueued STEP=1 DERIVE_HASH_STMT_BRIDGE

## Planner checklist (2026-06-02 plan-refresh — DERIVE_HASH_FRAGMENT close)

- [x] TRACK_DERIVE_HASH_FRAGMENT_BRIDGE closed (5/5); decl fragment **0** in `derive_methods_cpp.mlc`
- [x] Survivors deferred: hash body stmt fragments (`:363-370`), `decl_cpp` string fallback, `type_gen` string hash
- [x] verify 920 pass; DIFF_EXIT=0
- [x] Draft next TRACK from deferred list
- [x] Driver enqueued STEP=1 next track

## Planner checklist (2026-06-02 plan-refresh — DERIVE_HASH_FRAGMENT open)

- [x] TRACK_DECL_STRUCT_USING_BRIDGE closed (5/5)
- [x] TRACK_DERIVE_HASH_FRAGMENT_BRIDGE drafted (5 steps; `hash_specialization_declaration` fragment)
- [x] Survivors deferred: module TU fragments, type_gen inline struct_using, `cpp_decls_from_string_parts`
- [x] verify 913 pass; DIFF_EXIT=0
- [x] Driver enqueued STEP=1 DERIVE_HASH_FRAGMENT_BRIDGE

## Next step (Planner)

~~**plan-refresh**~~ — done (2026-06-02).

## Planner checklist (2026-06-02 plan-refresh — DECL_STRUCT_USING close)

- [x] TRACK_DECL_STRUCT_USING_BRIDGE closed (5/5)
- [x] Survivors: module TU fragments, derive hash, `cpp_decls_from_string_parts`, type_gen inline struct_using
- [x] verify 913 pass; DIFF_EXIT=0
- [ ] Draft next track; update TRACK_PLAN; enqueue Driver STEP=1

## Next step (Driver)

~~**STEP=2**~~ — TRACK_DECL_STRUCT_USING_BRIDGE done (2026-06-02).

## Planner checklist (2026-06-02 plan-refresh — DECL_STRUCT_USING open)

- [x] TRACK_DECL_STRUCT_USING_BRIDGE drafted (5 steps; fallback `:192`)
- [x] Survivors deferred: derive hash fragment, `cpp_decls_from_string_parts`
- [x] verify 904 pass; DIFF_EXIT=0 (baseline from DECL_EMPTY_FRAGMENT close)
- [x] Driver enqueued STEP=1 DECL_STRUCT_USING_BRIDGE

## Next step (Planner)

~~**plan-refresh**~~ — done (2026-06-02).

## Planner checklist (2026-06-02 plan-refresh — DECL_EMPTY_FRAGMENT close)

- [x] TRACK_DECL_EMPTY_FRAGMENT_BRIDGE closed (5/5)
- [x] Survivors: struct_using `:192`, derive hash fragment, non-empty `cpp_decl_from_string_output`
- [x] verify 904 pass; DIFF_EXIT=0
- [x] Draft next track; update TRACK_PLAN; enqueue Driver STEP=1

## Next step (Driver)

~~**STEP=1**~~ — TRACK_DECL_EMPTY_FRAGMENT_BRIDGE done (2026-06-02).

## Planner checklist (2026-06-02 plan-refresh — DECL_EMPTY_FRAGMENT open)

- [x] TRACK_DECL_EMPTY_FRAGMENT_BRIDGE drafted (5 steps; `cpp_decl_from_string_output('')` in `decl_cpp.mlc`)
- [x] Survivors deferred: struct_using `:189`, derive hash fragment
- [x] verify 895 pass; DIFF_EXIT=0 (baseline from DECL_MULTI_DECL close)
- [x] Driver enqueued STEP=1 DECL_EMPTY_FRAGMENT_BRIDGE

## Next step (Planner)

~~**plan-refresh**~~ — done (2026-06-02).

## Planner checklist (2026-06-02 plan-refresh — DECL_MULTI_DECL close)

- [x] TRACK_DECL_MULTI_DECL_BRIDGE closed (5/5)
- [x] Survivors: empty fragments `:952`/`:417-466`, struct_using `:189`, derive hash fragment
- [x] verify 895 pass; DIFF_EXIT=0
- [x] Draft next track; update TRACK_PLAN; enqueue Driver STEP=1

## Next step (Driver)

~~**STEP=1**~~ — TRACK_DECL_MULTI_DECL_BRIDGE done (2026-06-02).

## Planner checklist (2026-06-02 plan-refresh — DECL_MULTI_DECL open)

- [x] TRACK_DECL_MULTI_DECL_BRIDGE drafted (5 steps; `cpp_decl_from_native_declarations` 2+ path)
- [x] Survivors deferred: empty fragments, struct_using fallback, derive hash fragment
- [x] verify 890 pass; DIFF_EXIT=0 (baseline from DECL_STATIC_ASSERT close)
- [x] Driver enqueued STEP=1 DECL_MULTI_DECL_BRIDGE

## Next step (Planner)

~~**plan-refresh**~~ — done (2026-06-02).

## Planner checklist (2026-06-02 plan-refresh — DECL_STATIC_ASSERT close)

- [x] TRACK_DECL_STATIC_ASSERT_BRIDGE closed (5/5)
- [x] Survivors: empty fragments, 2+ print, struct_using fallback, derive hash fragment
- [x] verify 890 pass; DIFF_EXIT=0
- [x] Draft next track; update TRACK_PLAN; enqueue Driver STEP=1

## Next step (Driver)

~~**STEP=1**~~ — done through STEP=5 (2026-06-02).

## Planner checklist (2026-06-02 plan-refresh — DECL_STATIC_ASSERT open)

- [x] TRACK_DECL_STATIC_ASSERT_BRIDGE drafted (5 steps; `extend_trait_static_assert_decl_cpp`)
- [x] Survivors from DECL_CPP_FRAGMENT deferred (empty fragments, 2+ print, struct_using fallback)
- [x] Deferred: multi-decl print helper, BOOTSTRAP, parser `ref mut`
- [x] Driver enqueued STEP=1 DECL_STATIC_ASSERT_BRIDGE

## Next step (Planner)

~~**plan-refresh**~~ — done (2026-06-02).

## Planner checklist (2026-06-02 plan-refresh — DECL_CPP_FRAGMENT close)

- [x] TRACK_DECL_CPP_FRAGMENT_BRIDGE closed (5/5)
- [x] Survivors: empty fragments, static_assert fragment, 2+ print path, struct_using parse fallback
- [x] Print parity: `body_statement_depth == -1` compact one-liner in cpp_printer
- [x] verify 887 pass; DIFF_EXIT=0
- [x] Draft next track; update TRACK_PLAN; enqueue Driver STEP=1

## Next step (Driver)

~~**STEP=5**~~ — done (2026-06-02).

## Planner checklist (2026-06-02 plan-refresh — DECL_CPP_FRAGMENT open)

- [x] TRACK_DECL_CPP_FRAGMENT_BRIDGE drafted (5 steps; struct_using + inline wrappers + extern stub)
- [x] Empty `cpp_decl_from_string_output('')` deferred
- [x] Deferred: multi-decl print helper, BOOTSTRAP, parser `ref mut`
- [x] Driver enqueued STEP=1 DECL_CPP_FRAGMENT_BRIDGE

## Next step (Driver)

**STEP=5** — parity audit; survivors; close track. SESSION: register+enqueue Planner plan-refresh.

## Planner checklist (2026-06-02 plan-refresh — DECL_PRINT_ROUNDTRIP close)

- [x] TRACK_DECL_PRINT_ROUNDTRIP_BRIDGE closed (5/5 steps; 887 pass; DIFF_EXIT=0)
- [x] Next track candidate: `cpp_decl_from_string_output` non-empty fragments (inline wrappers, struct_using)
- [x] Deferred: multi-decl print in helper 2+ path, BOOTSTRAP, parser `ref mut`
- [x] Planner enqueued plan-refresh

## Next step (Driver)

~~**STEP=5**~~ — done.

## Planner checklist (2026-06-02 plan-refresh — DECL_PRINT_ROUNDTRIP open)

- [x] TRACK_DECL_PRINT_ROUNDTRIP_BRIDGE drafted (5 steps; sites `:396-400`, `:403-405`, `:439`)
- [x] Return-type constraint documented (`Shared<CppDecl>` vs native arrays at callers)
- [x] Deferred: other `cpp_decl_from_string_output`, BOOTSTRAP, parser `ref mut`
- [x] Driver enqueued STEP=1 DECL_PRINT_ROUNDTRIP_BRIDGE

## Next step (Driver)

**STEP=5** — parity tests audit; survivors; close track. SESSION: register+enqueue Planner plan-refresh.

## Planner checklist (2026-06-02 plan-refresh — COLLECT_DECL_PARTS_CPP close)

- [x] TRACK_COLLECT_DECL_PARTS_CPP_BRIDGE closed (5/5 steps; 887 pass; DIFF_EXIT=0)
- [x] Next track candidate: `gen_decl_cpp`/`gen_proto_cpp` print round-trip removal
- [x] Deferred: `cpp_decl_from_string_output` fragments, BOOTSTRAP, parser `ref mut`
- [x] Planner enqueued plan-refresh

## Next step (Driver)

~~**STEP=5**~~ — done.

## Planner checklist (2026-06-02 plan-refresh — COLLECT_DECL_PARTS_CPP open)

- [x] TRACK_COLLECT_DECL_PARTS_CPP_BRIDGE drafted (5 steps; goal `:873` else fallback)
- [x] Production phases 0–4 mapped; else branch unreachable in production
- [x] Deferred: print wrappers, `cpp_decl_from_string_output`, BOOTSTRAP, parser `ref mut`
- [x] Driver enqueued STEP=1 COLLECT_DECL_PARTS_CPP_BRIDGE

## Next step (Driver)

**STEP=5** — audit survivors; close track.

## Planner checklist (2026-06-02 plan-refresh — EXTEND_HELPER_PROTO_GUARD close)

- [x] TRACK_EXTEND_HELPER_PROTO_GUARD_BRIDGE closed (5/5 steps; 886 pass; DIFF_EXIT=0)
- [x] Next track candidate: `collect_decl_parts_cpp` unknown-phase string fallback (`:873`)
- [x] Deferred: print wrappers, inline CppFnDef fragments, BOOTSTRAP, parser `ref mut`
- [x] Planner enqueued plan-refresh

## Next step (Driver)

~~**STEP=5**~~ — done.

## Planner checklist (2026-06-02 plan-refresh — EXTEND_HELPER_PROTO_GUARD open)

- [x] TRACK_PROTO_CPP_CATCHALL_BRIDGE closed (5/5 steps; 885 pass; DIFF_EXIT=0)
- [x] Next track: remove string `gen_proto` guard in `extend_helper_protos_for_exported_types_cpp`
- [x] Deferred: `collect_decl_parts_cpp` else fallback, print wrappers, inline CppFnDef fragments, BOOTSTRAP, parser `ref mut`
- [x] Driver enqueued STEP=1 EXTEND_HELPER_PROTO_GUARD_BRIDGE

## Next step (Planner)

**plan-refresh** — after EXTEND_HELPER_PROTO_GUARD_BRIDGE close or mid-track if blocked.

## Planner checklist (2026-06-02 plan-refresh — PROTO_CPP_CATCHALL close)

- [x] TRACK_PROTO_CPP_CATCHALL_BRIDGE closed (5/5 steps; 885 pass; DIFF_EXIT=0)
- [x] Next track candidate: `collect_decl_parts_cpp` string bridge or helper `gen_proto` guard removal
- [x] Deferred: `gen_decl_cpp`/`gen_proto_cpp` print wrappers, inline CppFnDef fragments, BOOTSTRAP, parser `ref mut`
- [x] Planner enqueued plan-refresh

## Next step (Driver)

~~**STEP=5**~~ — done.

## Planner checklist (2026-06-02 plan-refresh — PROTO_CPP_CATCHALL open)

- [x] TRACK_DECL_CPP_CATCHALL_BRIDGE closed (5/5 steps; 878 pass; DIFF_EXIT=0)
- [x] Next track: native `gen_proto_cpp` catch-all (`decl_cpp.mlc:418`)
- [x] Deferred: `gen_decl_cpp` print wrappers, `collect_decl_parts_cpp` string bridge, inline CppFnDef fragments, BOOTSTRAP, parser `ref mut`
- [x] Driver enqueued STEP=1 PROTO_CPP_CATCHALL_BRIDGE

## Next step (Planner)

**plan-refresh** — after PROTO_CPP_CATCHALL_BRIDGE close or mid-track if blocked.

## Planner checklist (2026-06-02 plan-refresh — DECL_CPP_CATCHALL close)

- [x] TRACK_EXTEND_FALLBACK_BRIDGE closed (5/5 steps; 871 pass; DIFF_EXIT=0)
- [x] Next track: native `gen_decl_cpp` catch-all (`decl_cpp.mlc:398`)
- [x] Deferred: `SDeclExtend` print wrapper, `gen_proto_cpp:408`, inline CppFnDef fragments, string-path elimination, BOOTSTRAP, parser `ref mut`
- [x] Driver enqueued STEP=1 DECL_CPP_CATCHALL_BRIDGE

## Next step (Planner)

**plan-refresh** — after DECL_CPP_CATCHALL_BRIDGE close or mid-track if blocked.

## Planner checklist (2026-06-02 plan-refresh — EXTEND_FALLBACK close)

- [x] TRACK_EXTEND_EXTERN_BRIDGE closed (5/5 steps; 869 pass; DIFF_EXIT=0)
- [x] Next track: remove non-SDeclFn `gen_decl` fallback in `gen_decl_extend_cpp`
- [x] Deferred: inline wrapper CppFnDef, `gen_decl_cpp` catch-all, string-path elimination, BOOTSTRAP, parser `ref mut`
- [x] Driver enqueued STEP=1 EXTEND_FALLBACK_BRIDGE

## Next step (Planner)

**plan-refresh** — after EXTEND_FALLBACK_BRIDGE close or mid-track if blocked.

## Planner checklist (2026-06-02 plan-refresh — EXTEND_EXTERN close)

- [x] TRACK_EXTEND_TRAIT_SUFFIX_BRIDGE closed (5/5 steps)
- [x] Next track: native extern extend stubs in `gen_decl_extend_cpp`
- [x] Deferred: non-SDeclFn fallback removal, inline wrapper CppFnDef, string-path elimination, BOOTSTRAP, parser `ref mut`
- [x] Driver enqueued STEP=1 EXTEND_EXTERN_BRIDGE

## Next step (Planner)

**plan-refresh** — after EXTEND_EXTERN_BRIDGE close or mid-track if blocked.

## Planner checklist (2026-06-02 plan-refresh — EXTEND_TRAIT_SUFFIX close)

- [x] TRACK_EXTEND_TRAIT_SUFFIX_BRIDGE closed (5/5 steps; step 5 audit)
- [x] Production trait suffix → `gen_decl_extend_trait_suffix_cpp`; string survivor `decl.mlc` only
- [x] 866 pass; DIFF_EXIT=0
- [x] Draft next track: TRACK_EXTEND_EXTERN_BRIDGE
- [x] Driver enqueued STEP=1 next track

## Next step (Planner)

**plan-refresh** — after EXTEND_EXTERN_BRIDGE close.

- [x] TRACK_EXTEND_METHOD_BRIDGE closed (5/5 steps)
- [x] Next track: native trait suffix in `gen_decl_extend_cpp` (drop string `gen_decl_extend_trait_suffix` wrap)
- [x] Deferred: extern stub native, non-SDeclFn fallback removal, string-path elimination, BOOTSTRAP, parser `ref mut`
- [x] Driver enqueued STEP=1 EXTEND_TRAIT_SUFFIX_BRIDGE

## Next step (Planner)

**plan-refresh** — after EXTEND_TRAIT_SUFFIX_BRIDGE close or mid-track if blocked.

## Next step (Driver)

**Planner `plan-refresh`** — pick next bridge track (draft: native trait suffix / extern extend stubs).

## Planner checklist (2026-06-01 plan-refresh — EXTEND_METHOD close)

- [x] TRACK_EXTEND_METHOD_BRIDGE closed (5/5 steps; step 5 audit)
- [x] Production extend methods → `native_fn_decl_cpp(..., 0)`; survivors documented
- [x] 863 pass; DIFF_EXIT=0
- [x] Draft next track: TRACK_EXTEND_TRAIT_SUFFIX_BRIDGE
- [x] Driver enqueued STEP=1 next track

## Next step (Planner)

**plan-refresh** — after EXTEND_TRAIT_SUFFIX_BRIDGE close.

## Planner checklist (2026-06-01 plan-refresh — EXTEND_BODY close)

- [x] TRACK_EXTEND_BODY_BRIDGE closed (5/5 steps; step 5 audit)
- [x] Production `gen_decl_cpp` / `decl_fn_def_cpp` → `gen_decl_extend_cpp`; string `gen_decl_extend` survivor `decl.mlc` only
- [x] 860 pass; DIFF_EXIT=0
- [x] Draft next track: TRACK_EXTEND_METHOD_BRIDGE
- [x] Driver enqueued STEP=1 next track

## Next step (Planner)

**plan-refresh** — after EXTEND_METHOD_BRIDGE close.

## Planner checklist (2026-05-31 plan-refresh #2 — mid-track)

- [x] EXTEND_HELPER steps 1–4 done; STEP=5 pending — no new track opened (CONTINUITY rule)
- [x] Priority: stability → finish STEP=5 before EXTEND_BODY_BRIDGE
- [x] Draft next track: TRACK_EXTEND_BODY_BRIDGE (5 steps)
- [x] Deferred: string-path elimination, BOOTSTRAP, parser `ref mut`
- [x] Driver enqueued STEP=5

## Next step (Planner)

**plan-refresh** — after EXTEND_HELPER_BRIDGE close.

## Planner checklist (2026-05-31 plan-refresh — EXTEND_HELPER mid-track)

- [x] EXTEND_HELPER steps 1–4 done (856 pass, DIFF_EXIT=0); step 5 pending — no new track
- [x] Priority: stability → finish EXTEND_HELPER_BRIDGE before next track
- [x] Next candidate after close: native `SDeclExtend` body (`TRACK_EXTEND_BODY_BRIDGE`)
- [x] Deferred: string-path elimination, BOOTSTRAP, parser `ref mut`
- [x] Driver enqueued STEP=5

## Next step (Planner)

**plan-refresh** — after EXTEND_HELPER_BRIDGE close.

## Planner checklist (2026-05-31 plan-refresh — EXTEND_HELPER_BRIDGE open)

- [x] TRACK_FN_PROTO_BRIDGE closed (5/5); survivor: extend_helper string wrap in `fn_protos`
- [x] Priority: stability → EXTEND_HELPER_BRIDGE (native exported-type extend helper protos)
- [x] Deferred: extend body native, string-path track, BOOTSTRAP, parser `ref mut`
- [x] Driver enqueued STEP=1

## Next step (Planner)

**plan-refresh** — after EXTEND_HELPER_BRIDGE close or mid-track if blocked.

## Planner checklist (2026-05-31 plan-refresh — FN_PROTO_BRIDGE close)

- [x] TRACK_FN_PROTO_BRIDGE closed (5/5 steps; step 5 audit)
- [x] Production `fn_protos` → native phase 2; extend_helper protos string survivor
- [x] String `gen_proto` survivors: `decl.mlc` parity only
- [x] Deferred: extend body native, string-path track, BOOTSTRAP, parser `ref mut`
- [x] Planner enqueued plan-refresh

## Planner checklist (2026-05-31 plan-refresh — FN_PROTO_BRIDGE open)

- [x] All prior bridge tracks closed (TRAIT_STRUCT step 5 audit)
- [x] Priority: stability > security > performance → FN_PROTO_BRIDGE (phase 2 fn protos native)
- [x] Deferred: extend body native, string-path track, BOOTSTRAP, parser `ref mut`
- [x] Driver enqueued STEP=1

## Next step (Planner)

**plan-refresh** — after FN_PROTO_BRIDGE close or mid-track if blocked.

## Planner checklist (2026-05-31 plan-refresh — TRAIT_STRUCT_BRIDGE close)

- [x] TRACK_TRAIT_STRUCT_BRIDGE closed (5/5 steps; step 5 audit)
- [x] Production phase 4 → native `gen_trait_struct_cpp`; dead `append_string_segments` removed
- [x] String `gen_trait_struct` survivors: `decl_extend.mlc`, `decl.mlc` parity only
- [x] Deferred: extend body native, string-path track, BOOTSTRAP, parser `ref mut`
- [x] Planner enqueued plan-refresh

## Next step (Driver)

**STEP=1** — pending Planner (no open track).

## Planner checklist (2026-05-31 plan-refresh — TRAIT_STRUCT_BRIDGE open)

- [x] All prior bridge tracks closed (DERIVE_BRIDGE step 5 audit)
- [x] Priority: stability > security > performance → TRAIT_STRUCT_BRIDGE (phase 4 `gen_trait_struct` native)
- [x] Deferred: extend body native, string-path track, BOOTSTRAP, parser `ref mut`
- [x] Driver enqueued STEP=1

## Next step (Planner)

**plan-refresh** — after TRAIT_STRUCT_BRIDGE close or mid-track if blocked.

## Planner checklist (2026-05-31 plan-refresh — DERIVE_BRIDGE close)

- [x] TRACK_DERIVE_BRIDGE closed (5/5 steps; step 5 audit)
- [x] `gen_type_decl_body_cpp` → native `gen_derive_methods_cpp` (Display/Eq/Ord/Hash)
- [x] String `gen_derive_methods` survivors: `type_gen.mlc`, `decl.mlc` parity only
- [x] Deferred: trait struct phase 4, extend body native, string-path track, BOOTSTRAP, parser `ref mut`
- [x] Planner enqueued plan-refresh

## Next step (Driver)

**STEP=1** — pending Planner (no open track).

## Planner checklist (2026-05-31 plan-refresh — DECL_BRIDGE close)

- [x] TRACK_DECL_BRIDGE closed (`9e2d47e`; 5/5 steps)
- [x] module.mlc → `collect_all_decl_parts_cpp`; SDeclType/SDeclTrait phases 0–1 native
- [x] Priority: stability > security > performance → DERIVE_BRIDGE (derive methods in type body)
- [x] Deferred: trait struct phase 4, extend body native, string-path track, BOOTSTRAP, parser `ref mut`
- [x] Driver enqueued STEP=1

## Next step (Planner)

**plan-refresh** — after DERIVE_BRIDGE close or mid-track if blocked.

## Planner checklist (2026-05-31 plan-refresh — DECL_BRIDGE open)

- [x] TRACK_MAIN_BRIDGE closed (`521f574`; 5/5 steps)
- [x] Priority: stability > security > performance → DECL_BRIDGE (native SDeclType/SDeclTrait phases 0–1)
- [x] Deferred: trait struct phase 4, extend body native, string-path track, BOOTSTRAP, parser `ref mut`
- [x] Driver enqueued STEP=1

## Next step (Planner)

**plan-refresh** — after DECL_BRIDGE close or mid-track if blocked.

## Planner checklist (2026-05-31 plan-refresh — MAIN_BRIDGE close)

- [x] TRACK_MAIN_BRIDGE closed (`f2aa065`; 5/5 steps)
- [x] Zero-param main native: argc/argv + set_args preamble; no fn string bridge
- [x] String `gen_fn_proto`/`gen_fn_decl` main branch remains parallel path in `decl.mlc`
- [x] Planner enqueued plan-refresh

## Next step (Driver)

**STEP=5** [TRACK_MAIN_BRIDGE.md](TRACK_MAIN_BRIDGE.md) — audit survivors; close track. **done**

## Planner checklist (2026-05-31 plan-refresh — EXTEND_BRIDGE close)

- [x] TRACK_EXTEND_BRIDGE closed (`7ab42a8`; 5/5 steps)
- [x] Native extend forwards in `collect_fn_defs_cpp`; string path parallel in `decl.mlc`
- [x] Deferred: main native, non-fn decl native, string-path track, BOOTSTRAP
- [x] Planner enqueued plan-refresh

## Next step (Planner)

**plan-refresh** — pick next track from PLAN backlog (stability > security > performance).

## Planner checklist (2026-05-31 plan-refresh — EXTEND_BRIDGE open)

- [x] TRACK_TEMPLATE_BRIDGE closed (`6db2014`; 5/5 steps)
- [x] Priority: stability > security > performance → EXTEND_BRIDGE (extend forward native decl)
- [x] Deferred: non-fn decl native, main native, string-path track, BOOTSTRAP, parser panic audit
- [x] Driver enqueued STEP=1

## Next step (Planner)

**plan-refresh** — after EXTEND_BRIDGE close or mid-track if blocked.

## Planner checklist (2026-05-30 plan-refresh — TEMPLATE_BRIDGE open)

- [x] TRACK_BINDING_BRIDGE closed (`3904513`; 5/5 steps)
- [x] Priority: stability > security > performance → TEMPLATE_BRIDGE (template fn native decl)
- [x] Deferred: string-path track, extend forwards native, BOOTSTRAP, parser panic audit
- [x] Driver enqueued STEP=1

## Next step (Planner)

**plan-refresh** — pick next track from PLAN backlog (stability > security > performance).

## Planner checklist (2026-05-30 plan-refresh — BINDING_BRIDGE open)

- [x] TRACK_FRAGMENT_BRIDGE closed (`49b7091`; 5/5 steps)
- [x] Priority: stability > security > performance → BINDING_BRIDGE (let_pat structured binding)
- [x] Deferred: template native, string-path track, BOOTSTRAP, parser panic audit
- [x] Driver enqueued STEP=1

## Planner checklist (2026-05-30 plan-refresh — FRAGMENT_BRIDGE close)

- [x] TRACK_FRAGMENT_BRIDGE closed (5/5 steps; step 5 audit)
- [x] Native paths cleared: PatTuple PatIdent, PatRecord/Array/Ctor, mut prelude, return_body `?`
- [x] Survivors documented: structured_binding fallback, string paths, template/decl bridges
- [x] Planner enqueued plan-refresh

## Next step (Planner)

**plan-refresh** — pick next track from PLAN backlog (stability > security > performance).

## Planner checklist (2026-05-30 plan-refresh — FRAGMENT_BRIDGE open)

- [x] All prior bridge tracks closed (PREFIX_BRIDGE `174b82a`)
- [x] Priority: stability > security > performance → FRAGMENT_BRIDGE (Phase 2 codegen)
- [x] Deferred: template native, BOOTSTRAP, parser panic audit (security backlog)
- [x] Driver enqueued STEP=1

## Planner checklist (2026-05-30 plan-refresh — PREFIX_BRIDGE close)

- [x] TRACK_RETURN_BODY closed (`91d6752`; 5/5 steps)
- [x] TRACK_PREFIX_BRIDGE closed (5/5 steps; step 5 audit)
- [x] PLAN §Phase 2 §3 — `return_body_needs_string_bridge` conservative fallback

## Planner checklist (2026-05-30 plan-refresh — RETURN_BODY)

- [x] TRACK_STMT_BRIDGE closed (`7084227`; 5/5 steps)
- [x] TRACK_RETURN_BODY opened (5 steps: leaf, block, if/else, decl_cpp wire, audit close)
- [x] PLAN §Phase 2 §3 — return_body string bridge (`cpp_stmts_from_string_output`)
- [x] Driver enqueued STEP=1

## Planner checklist (2026-05-29 plan-refresh — STMT_BRIDGE)

- [x] TRACK_MATCH_BRIDGE closed (`3e47ca4`; 3/3 steps)
- [x] TRACK_STMT_BRIDGE opened (5 steps: return/?, let/const, expr stmt, let-pat, audit close)
- [x] PLAN §Phase 2 §3 — stmt string bridge in `stmt_cpp.mlc` (stability priority)
- [x] Driver enqueued STEP=1

## Planner checklist (2026-05-29 plan-refresh — MATCH_BRIDGE)

- [x] TRACK_EXPR_BRIDGE closed (`91ece96`; 5/5 steps)
- [x] TRACK_MATCH_BRIDGE opened (3 steps: arm handlers, guarded match, audit close)
- [x] PLAN §Phase 2 §3 — match string bridge in `expr_visitor_cpp`
- [x] Driver enqueued STEP=1

## Next step (Driver)

**STEP=5** [TRACK_PARSER_PARITY.md](TRACK_PARSER_PARITY.md) — audit known divergences; close track. **done** (`d5442d6`)

## Next step (Driver)

**STEP=2** [TRACK_SPAN_IR.md](TRACK_SPAN_IR.md) — `SDeclType` span from `DeclType.name_span`. **done** (`3e214fc`)

## Next step (Driver)

**STEP=3** [TRACK_SPAN_IR.md](TRACK_SPAN_IR.md) — `SDeclTrait` / `SDeclExtend` spans. **done** (`326b173`)

## Next step (Driver)

**STEP=4** [TRACK_SPAN_IR.md](TRACK_SPAN_IR.md) — checker diagnostics use `decl_span`. **done** (`860bafc`)

## Next step (Driver)

**STEP=5** [TRACK_SPAN_IR.md](TRACK_SPAN_IR.md) — audit + close. **done** (`e826f1a`)

## Next step (Planner)

**plan-refresh** — pick next track from PLAN backlog.

## Planner checklist (2026-05-28 plan-refresh — EXPR_BRIDGE)

- [x] TRACK_PARSER_PARITY closed (`d5442d6`; 5/5 steps)
- [x] TRACK_EXPR_BRIDGE opened (5 steps: call/method, if/block, array/lambda, record_update, audit close)
- [x] PLAN §Phase 2 §3 — eliminate `expr_fragment_codegen` survivors in `expr_visitor_cpp`
- [x] Driver enqueued STEP=1

## Planner checklist (2026-05-28 plan-refresh — PARSER_PARITY)

- [x] TRACK_SPAN_IR closed (`e826f1a`; 5/5 steps)
- [x] TRACK_PARSER_PARITY opened (5 steps: unclosed block/record, empty type/trait, noise, lex string, audit close)
- [x] PLAN §Phase 1 §3 — shrink 6 known fuzz divergences
- [x] Driver enqueued STEP=1

## Planner checklist (2026-05-28 plan-refresh — SPAN_IR)

- [x] TRACK_BUILD closed (`1d6f4c5`; 5/5 steps)
- [x] TRACK_SPAN_IR opened (5 steps: SDeclFn, SDeclType, trait/extend, diagnostics, audit close)
- [x] PLAN §Phase 1 §1 residual — spans on SemanticIR decls
- [x] Driver enqueued STEP=1

## Planner checklist (2026-05-28 plan-refresh — BUILD)

- [x] TRACK_SPAN_CHECKER closed (`a8bf7a1`; 5/5 steps)
- [x] TRACK_BUILD opened (5 steps: build_bin, compile_options, pipeline json, test, close)
- [x] PLAN §Phase 1 §6 build determinism
- [x] Driver enqueued STEP=1

## Planner checklist (2026-05-28 plan-refresh — SPAN_CHECKER)

- [x] TRACK_PARSER_PANIC closed (`5525d03`; 5/5 steps)
- [x] TRACK_SPAN_CHECKER opened (5 steps: check/, trait_param, transform/, codegen/, audit close)
- [x] Deferred from PARSER_PANIC; PLAN §Phase 1 §1 diagnostics
- [x] No step bundles `compiler/` + `lib/mlc/`
- [x] Driver enqueued STEP=1

## Planner checklist (2026-05-28 plan-refresh — PARSER_PANIC)

- [x] TRACK_FUZZ_DIFF closed (`5463361`; 5/5 steps)
- [x] TRACK_PARSER_PANIC opened (5 steps: decls, exprs, parser tests, in-process smoke, audit close)
- [x] Deferred from TRACK_SECURITY step 4; Phase 1 §4 crash-safety
- [x] No step bundles `compiler/` + `lib/mlc/`
- [x] Driver enqueued STEP=1

## Planner checklist (2026-05-28 plan-refresh — FUZZ_DIFF close)

- [x] TRACK_FUZZ_DIFF closed (`5463361`; 5/5 steps)
- [x] 719 tests; self-host diff empty
- [x] Next active track from PLAN

## Planner checklist (2026-05-28 plan-refresh — fuzz differential)

- [x] TRACK_DIAGNOSTICS2 closed (`d055c49`)
- [x] TRACK_FUZZ_DIFF opened (5 steps: corpus exit, random seeds, error count, shell gate, in-process close)
- [x] No step bundles `compiler/` + `lib/mlc/`
- [x] Driver enqueued STEP=1

## Planner checklist (2026-05-28 plan-refresh — diagnostics2)

- [x] TRACK_COVERAGE closed (`9bddbb3`)
- [x] TRACK_DIAGNOSTICS2 opened (5 steps: check defaults, array HOF, Result/Option, infer paths, audit)
- [x] No step bundles `compiler/` + `lib/mlc/`
- [x] Driver enqueued STEP=1

## Planner checklist (2026-05-28 plan-refresh — coverage track)

- [x] TRACK_DIAGNOSTICS closed (`f6863fe`)
- [x] TRACK_COVERAGE opened (5 steps: checker lambda/array, tuple/record, match/for, codegen expr, codegen stmt)
- [x] No step bundles `compiler/` + `lib/mlc/`
- [x] Driver enqueued STEP=1

## Planner checklist (2026-05-28 plan-refresh — diagnostics closed)

- [x] TRACK_DIAGNOSTICS closed (`f6863fe`)
- [ ] Open next track from PLAN
- [ ] Driver enqueued STEP=1

## Planner checklist (2026-05-28 plan-refresh — diagnostics track)

- [x] TRACK_SECURITY closed (`a035c3d`)
- [x] TRACK_DIAGNOSTICS opened (5 steps: catalog, binary/type, method receiver, infer/transform, audit sweep)
- [x] No step bundles `compiler/` + `lib/mlc/`
- [x] Driver enqueued STEP=1

## Planner checklist (2026-05-28 plan-refresh — security track)

- [x] TRACK_PERFORMANCE closed (`0671422`)
- [x] TRACK_SECURITY opened (4 steps: corpus, generator, in-process sweep, panic audit)
- [x] No step bundles `compiler/` + `lib/mlc/`
- [x] Driver enqueued STEP=1

## Planner checklist (2026-05-28 plan-refresh — performance track)

- [x] TRACK_SAFETY closed (`32f8335`)
- [x] TRACK_PERFORMANCE opened (5 steps, verify + perf gate)
- [x] No step bundles `compiler/` + `lib/mlc/`
- [x] Driver enqueued STEP=1

## Planner checklist (2026-05-25 plan-refresh #3 — Phase 2)

- `compiler/pipeline.mlc`: checker → transform → codegen passes
- `main.mlc`: `compile_modular_loop` removed; calls `run_modular_compiler_pipeline`
- verify: 490 tests; build.sh; self-host diff empty

## Step 18 detail (done — `bef5143`)

- moved: lexer, ast, ast_tokens → `compiler/frontend/`; parser/ → `frontend/parser/`; preds → predicates.mlc
- all import paths updated; no logic changes
- verify: 490 tests; build.sh; self-host diff empty

## Step 19 detail (commit-prep)

- `semantic_ir.mlc` → `ir/semantic_ir.mlc`; `record_field_default_initializer.mlc` → `ir/record_defaults.mlc`
- import paths updated (~30 modules); no logic changes
- verify: 490 tests; build.sh; self-host diff empty

## Step 20 detail (commit-prep)

- checker split: `infer/`, `transform/`, `check/` (+ diagnostics/, method_types/)
- filenames kept (unique C++ basenames; short renames blocked by `path_to_module_base`)
- verify: 490 tests; build.sh; self-host diff empty

## Planner checklist (2026-05-25 plan-refresh #3 — Phase 2)

- [x] Step 18 marked done with commit `bef5143`
- [x] Step 19 scoped with file list + verify gate
- [x] No step bundles `compiler/` + `lib/mlc/`
- [x] TRACK_CPPEXPR.md created (21 steps, verify gate)
- [x] Folder restructure track closed; Phase 2 active

---

# Track: Phase 2.5–2.6 roadmap (2026-06)

Parent: [../PLAN.md](../PLAN.md) §Phase 2.5 + §Phase 2.6

## Status: **open** (step 1 pending)

| Step | Item | Track | Status |
|------|------|-------|--------|
| 1 | Phase 2 финал — MODULE_TU_BRIDGE закрыть | [TRACK_MODULE_TU_BRIDGE.md](TRACK_MODULE_TU_BRIDGE.md) | open |
| 2 | String match в языке | [TRACK_STRING_MATCH.md](TRACK_STRING_MATCH.md) | open |
| 3 | Применить record destructuring | [TRACK_DESTRUCTURING_APPLY.md](TRACK_DESTRUCTURING_APPLY.md) | open |
| 4 | Type aliases в языке | [TRACK_TYPE_ALIASES.md](TRACK_TYPE_ALIASES.md) | open |
| 5 | Переименование сокращений + кавычки | [TRACK_RENAME_ABBREV.md](TRACK_RENAME_ABBREV.md) | open |
| 6 | Visitor-паттерн + методы на контексте + Display | [TRACK_VISITOR_PATTERN.md](TRACK_VISITOR_PATTERN.md) | open |
| 7 | TypeRegistry split + HOF helpers + parser helpers | [TRACK_REGISTRY_SPLIT.md](TRACK_REGISTRY_SPLIT.md) | open |

## Planner checklist (2026-06-03 plan-refresh)

- [x] CODE_REVIEW_2026_06.md создан — полный анализ 138 файлов
- [x] Phase 2.5 описана в PLAN.md (языковые улучшения)
- [x] Phase 2.6 описана в PLAN.md (структурный рефакторинг)
- [x] Треки созданы для всех задач Phase 2.5–2.6
- [ ] MODULE_TU_BRIDGE закрыт (завершение Phase 2)
- [ ] TRACK_STRING_MATCH выполнен
- [ ] TRACK_DESTRUCTURING_APPLY выполнен
- [ ] TRACK_TYPE_ALIASES выполнен
- [ ] TRACK_RENAME_ABBREV выполнен
- [ ] TRACK_VISITOR_PATTERN выполнен
- [ ] TRACK_REGISTRY_SPLIT выполнен
