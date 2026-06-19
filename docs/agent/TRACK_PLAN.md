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
- **Native module TU scaffolding:** [TRACK_MODULE_TU_BRIDGE.md](TRACK_MODULE_TU_BRIDGE.md) — **closed** (`df744e5`)
- **CppDeclFragment cleanup:** [TRACK_DECL_FRAGMENT_CLEANUP.md](TRACK_DECL_FRAGMENT_CLEANUP.md) — **closed** (step 5 audit 940/0; WIP `decl_cpp` uncommitted)
- **Apply record destructuring:** [TRACK_DESTRUCTURING_APPLY.md](TRACK_DESTRUCTURING_APPLY.md) — **closed** (`fd95af6`)
- **String match patterns:** [TRACK_STRING_MATCH.md](TRACK_STRING_MATCH.md) — **closed** (step 12 self-host diff green, 2026-06-05)
- **Type aliases:** [TRACK_TYPE_ALIASES.md](TRACK_TYPE_ALIASES.md) — **closed** (steps 1–5 + audit; STEP=6 deferred `TRACK_GENERICS_RECORD`)
- **Rename abbreviations:** [TRACK_RENAME_ABBREV.md](TRACK_RENAME_ABBREV.md) — **closed** (`0c68101b`, `7bc13d09`)
- **Loop contracts:** [TRACK_LOOP_CONTRACTS.md](TRACK_LOOP_CONTRACTS.md) — **closed** (`a28a4823`)
- **Visitor pattern:** [TRACK_VISITOR_PATTERN.md](TRACK_VISITOR_PATTERN.md) — **closed** (steps 1–13; gate **992**/0; uncommitted batch steps 7–12)
- **Lambda capture:** [TRACK_LAMBDA_CAPTURE.md](TRACK_LAMBDA_CAPTURE.md) — **closed** (steps 1–5; gate **998**/0; uncommitted; mlcc2 diff blocked)
- **mlcc2 checker parity:** [TRACK_MLCC2_CHECKER_PARITY.md](TRACK_MLCC2_CHECKER_PARITY.md) — **closed** (STEP=6; diff_exit=0; uncommitted)
- **COW fold audit (mlcc2):** [TRACK_COW_FOLD_AUDIT.md](TRACK_COW_FOLD_AUDIT.md) — **closed** (STEP=5; P0 fixed; P1 deferred; diff_exit=0; uncommitted)
- **COW while/closure audit (mlcc2):** [TRACK_COW_WHILE_AUDIT.md](TRACK_COW_WHILE_AUDIT.md) — **closed** (STEP=5; 11 P1 sites fixed; diff_exit=0; uncommitted)
- **COW P2 audit (mlcc2):** [TRACK_COW_P2_AUDIT.md](TRACK_COW_P2_AUDIT.md) — **closed** (STEP=5; 33 sites fixed; diff_exit=0; uncommitted)
- **Variant ctor brace init:** [TRACK_VARIANT_CTOR_BRACE.md](TRACK_VARIANT_CTOR_BRACE.md) — **closed** (steps 1–5; ctor green; **diff_exit=0**; uncommitted; E046 deferred)
- **E046 filter predicate:** [TRACK_E046_FILTER_PREDICATE.md](TRACK_E046_FILTER_PREDICATE.md) — **closed** (steps 1–5; **999/0**; **diff_exit=0**; uncommitted)
- **Registry COW:** [TRACK_REGISTRY_COW.md](TRACK_REGISTRY_COW.md) — **closed** (`bf6c46e8`)
- **Generic record types:** [TRACK_GENERICS_RECORD.md](TRACK_GENERICS_RECORD.md) — **closed** (`bf6c46e8`)
- **ParseResult migration:** [TRACK_PARSE_RESULT.md](TRACK_PARSE_RESULT.md) — **closed** (`bf6c46e8`)
- **ExprResult migration:** [TRACK_EXPR_RESULT.md](TRACK_EXPR_RESULT.md) — **closed** (`bf6c46e8`)
- **TypeParamsResult migration:** [TRACK_TYPE_PARAMS_RESULT.md](TRACK_TYPE_PARAMS_RESULT.md) — **closed** (`bf6c46e8`)
- **ParseProgramResult cleanup:** [TRACK_PARSE_PROGRAM_RESULT.md](TRACK_PARSE_PROGRAM_RESULT.md) — **open** STEP=1

## Priority queue (2026-06-19 full-replan)

Strict order; each track depends on previous unless noted.

| # | Track | Phase | Status |
|---|-------|-------|--------|
| 1 | [TRACK_PARSE_PROGRAM_RESULT](TRACK_PARSE_PROGRAM_RESULT.md) | 2.5 | **open** STEP=1 |
| 2 | [TRACK_CODE_QUALITY](TRACK_CODE_QUALITY.md) | 2.6/code | **closed** |
| 3 | [TRACK_FORMATTER](TRACK_FORMATTER.md) | 3 | planned |
| 4 | [TRACK_PHASE26_REMAINING](TRACK_PHASE26_REMAINING.md) | 2.6 | planned |
| 5 | [TRACK_SELF_HOST_BOOTSTRAP](TRACK_SELF_HOST_BOOTSTRAP.md) | 4 | planned |
| 6 | [TRACK_LSP](TRACK_LSP.md) | 3 | planned |
| 7 | [TRACK_CPP_HEADER_IMPORT](TRACK_CPP_HEADER_IMPORT.md) | 3.5 | planned |
| 8 | [TRACK_REDDIT_DEMO](TRACK_REDDIT_DEMO.md) | 5 | planned |

```
PARSE_PROGRAM_RESULT → CODE_QUALITY → FORMATTER → PHASE26_REMAINING
  → SELF_HOST_BOOTSTRAP → LSP → CPP_HEADER_IMPORT → REDDIT_DEMO
```

**Deferred:** parser `ref mut` (TRACK_PLAN step 15); MLC IR; package manager.

## Next step (Driver)

> **Immediate:** [TRACK_PARSE_PROGRAM_RESULT](TRACK_PARSE_PROGRAM_RESULT.md) **STEP=1** (guard resumes after replan).

## Next step (Planner)

> plan-refresh after each track close or every ~8 driver turns.

## Meta-Planner checklist (2026-06-19 full-replan)

- [x] Updated [PLAN.md](../PLAN.md) §5 — phase status table + dependency graph
- [x] Priority queue in TRACK_PLAN (8 tracks)
- [x] Skeleton tracks: CODE_QUALITY, FORMATTER, PHASE26_REMAINING, SELF_HOST_BOOTSTRAP, LSP, CPP_HEADER_IMPORT, REDDIT_DEMO
- [x] Commit docs; enqueue Planner plan-refresh

## Planner checklist (2026-06-19 plan-refresh — PARSE_PROGRAM_RESULT open)

- [x] All tracks closed; batch committed **`bf6c46e8`**; gate **1005/0**
- [x] Priority **stability** > security > performance — `ParseProgramResult` remainder (PLAN §2.5.5)
- [x] Opened **TRACK_PARSE_PROGRAM_RESULT**
- [x] Deferred: parser `ref mut`; security (parser panic audit) after stability
- [x] Enqueue Driver STEP=1 PARSE_PROGRAM_RESULT

## Meta checklist (2026-06-15 meta-review — batch commit)

- [x] Supervisor: 292 uncommitted since closed tracks; step-loop / stale Driver re-enqueue
- [x] Recovery: `git add -A && git commit` → **`bf6c46e8`** (293 files); gate baseline **1005/0**
- [x] No re-enqueue closed Driver STEP (TYPE_PARAMS_RESULT STEP=4, etc.)
- [x] RESEARCH recovery log updated
- [x] Enqueue **Planner plan-refresh** once

## Planner checklist (2026-05-19 plan-refresh — TYPE_PARAMS_RESULT open)

- [x] All tracks closed; EXPR_RESULT gate **1005/0**; **diff_exit=0**
- [x] Priority **stability** > security > performance — ParseResult remainder (`TypeParamsResult`)
- [x] Opened **TRACK_TYPE_PARAMS_RESULT**
- [ ] Uncommitted batch — commit on user command
- [x] Deferred: `ParseProgramResult`; parser `ref mut`
- [x] Enqueue Driver STEP=1 TYPE_PARAMS_RESULT

## Planner checklist (2026-05-19 plan-refresh — EXPR_RESULT closed)

- [x] All tracks closed; PARSE_RESULT gate **1005/0**; **diff_exit=0**
- [x] Priority **stability** > security > performance — finish ParseResult (`ExprResult` deferred)
- [x] Opened **TRACK_EXPR_RESULT**
- [ ] Uncommitted batch — commit on user command
- [x] Enqueue Driver STEP=1 EXPR_RESULT

## Planner checklist (2026-06-19 plan-refresh — PARSE_RESULT open)

- [x] All prior tracks closed; GENERICS_RECORD gate **1003/0**; **diff_exit=0**
- [x] Priority scan: stability → **ParseResult** remainder (PLAN §2.5.5)
- [x] Opened **TRACK_PARSE_RESULT**
- [ ] Uncommitted batch — commit on user command
- [x] Enqueue Driver STEP=1 PARSE_RESULT

## Planner checklist (2026-05-19 plan-refresh — GENERICS_RECORD open)

- [x] TRACK_REGISTRY_COW closed; stability queue scanned
- [x] Priority **stability** > security > performance — no open stability/security tracks
- [x] Opened **TRACK_GENERICS_RECORD** (TYPE_ALIASES STEP=6 deferred)
- [ ] Uncommitted batch — commit on user command
- [x] Enqueue Driver STEP=1 GENERICS_RECORD

## Planner checklist (2026-05-19 plan-refresh — REGISTRY_COW closed)

- [x] TRACK_REGISTRY_COW closed (steps 1–5); **0** `.push` in `registry.mlc`
- [x] Gate: build_tests **999/0**; parity **2/2**; **diff_exit=0**; mlcc **2.58s** / mlcc2 **1.56s**
- [x] Open next track (perf/stability) → **GENERICS_RECORD**
- [ ] Uncommitted batch — commit on user command
- [x] Enqueue Driver STEP=1

## Planner checklist (2026-05-19 plan-refresh — REGISTRY_COW open)

- [x] TRACK_E046_FILTER_PREDICATE closed; build_tests **999/0**; **diff_exit=0**
- [x] Stability queue empty — next **performance**: `registry.mlc` COW (10 sites)
- [x] Opened **TRACK_REGISTRY_COW**
- [x] Uncommitted batch — commit on user command
- [x] Deferred: parser `ref mut`
- [x] Driver enqueued STEP=1 REGISTRY_COW

## Planner checklist (2026-05-19 plan-refresh — E046 closed)

- [x] TRACK_E046_FILTER_PREDICATE closed (steps 1–5); build_tests **999/0**; **diff_exit=0**
- [x] Fix: `infer_lambda_context.mlc` lambda env in fold closure
- [x] Uncommitted batch (COW + ctor + E046) — commit on user command
- [x] Draft next track: `registry.mlc` COW perf vs parser `ref mut`
- [x] Update TRACK_PLAN; enqueue Driver STEP=1

## Planner checklist (2026-05-19 plan-refresh — E046 open)

- [x] TRACK_VARIANT_CTOR_BRACE closed (steps 1–5); **diff_exit=0**; mlcc **1.81s** / mlcc2 **1.56s**
- [x] Priority **stability** > security > performance — E046 (998/1) before `registry.mlc` COW
- [x] Opened **TRACK_E046_FILTER_PREDICATE** — last build_tests failure
- [x] Uncommitted batch — commit on user command
- [x] Deferred: `registry.mlc` COW; parser `ref mut`; E064 verify only
- [x] Driver enqueued STEP=1 E046

## Planner checklist (2026-05-19 plan-refresh — VARIANT_CTOR_BRACE closed)

- [x] TRACK_VARIANT_CTOR_BRACE closed (steps 1–5); ctor `Ok{42}`/`Err{…}` green
- [x] Gate: build_tests **998/1** (E046 only); parity **2/2**; **diff_exit=0**; mlcc **1.81s** / mlcc2 **1.56s**
- [x] Uncommitted batch (COW + ctor) — commit on user command
- [x] Draft next track: E046 stability vs `registry.mlc` COW perf
- [x] Update TRACK_PLAN; enqueue Driver STEP=1

## Planner checklist (2026-05-19 plan-refresh — VARIANT_CTOR_BRACE open)

- [x] TRACK_COW_P2_AUDIT closed; checker scope 0 `.push` (except `registry.mlc`)
- [x] Next stability: **VARIANT_CTOR_BRACE** — fix 2–3 failing `test_codegen` ctor cases
- [x] Priority **stability** > security > performance — ctor before registry COW / parser
- [x] Uncommitted MLCC2+COW batch — commit on user command
- [x] Baseline build_tests **996+/3 fail**; mlcc **7.01s** / mlcc2 **2.57s**; **diff_exit=0**
- [x] Active track: **VARIANT_CTOR_BRACE** STEP=1
- [x] Deferred: `registry.mlc` COW; parser `ref mut`
- [x] Driver enqueued STEP=1 VARIANT_CTOR_BRACE

## Planner checklist (2026-05-19 plan-refresh — COW_P2 closed)

- [x] TRACK_COW_P2_AUDIT closed (steps 1–5); 33 in-scope `.push` → fold/`concat`; gate parity 2/2; **diff_exit=0**
- [x] `registry.mlc` (10) deferred — perf track
- [x] Uncommitted MLCC2+COW batch — commit on user command
- [x] Baseline build_tests ok; mlcc **7.01s** / mlcc2 **2.57s**
- [x] Draft next track; update TRACK_PLAN; enqueue Driver STEP=1
- [x] Planner enqueued plan-refresh

## Planner checklist (2026-05-19 plan-refresh — COW_P2 open)

- [x] TRACK_COW_WHILE_AUDIT closed (steps 1–5); **diff_exit=0**
- [x] P2 survivors → new **TRACK_COW_P2_AUDIT** (5 steps; 33 `.push` / 6 files)
- [x] Priority **stability** > security > performance — COW P2 before registry/parser/perf
- [x] Uncommitted MLCC2+COW batch — commit on user command
- [x] Baseline build_tests **997**/2 fail (ctor brace pre-existing); mlcc **3.31s** / mlcc2 **2.09s**
- [x] Active track: **COW_P2_AUDIT** STEP=1
- [x] Deferred: `registry.mlc`; parser `ref mut`; test_codegen ctor brace; security/perf tracks (closed)
- [x] Driver enqueued STEP=1 COW_P2_AUDIT

## Planner checklist (2026-05-19 plan-refresh — COW_WHILE closed)

- [x] TRACK_COW_WHILE_AUDIT closed (steps 1–5); 11 in-scope `.push` → fold/`concat`; gate parity 2/2; **diff_exit=0**
- [x] P2 survivors deferred: `infer_array_method`, `infer_result_option_method`, `transform_decl`, `registry`
- [x] Uncommitted MLCC2+COW batch — commit on user command
- [x] Baseline build_tests **997**/2 fail (ctor brace pre-existing); mlcc **3.31s** / mlcc2 **2.09s**
- [x] Draft next track; update TRACK_PLAN; enqueue Driver STEP=1
- [x] Planner enqueued plan-refresh

## Planner checklist (2026-05-19 plan-refresh — COW_FOLD closed, COW_WHILE open)

- [x] TRACK_COW_FOLD_AUDIT closed (steps 1–5); gate parity 2/2; **diff_exit=0**
- [x] P1 while/closure sites deferred → new **TRACK_COW_WHILE_AUDIT** (5 steps)
- [x] Priority **stability** > security > performance — COW_WHILE before parser `ref mut` / Phase 2.7
- [x] Uncommitted MLCC2+COW batch — commit on user command
- [x] Active track: **COW_WHILE_AUDIT** STEP=1
- [x] Deferred: parser `ref mut`; GENERIC_ALIASES; security/perf tracks (all closed)
- [x] Driver enqueued STEP=1 COW_WHILE_AUDIT

## Planner checklist (2026-06-19 plan-refresh — MLCC2_CHECKER_PARITY closed)

- [x] TRACK_MLCC2_CHECKER_PARITY closed (steps 1–6); gate mlcc2 main=0; parity 2/2; **diff_exit=0**
- [x] Root cause class documented: COW `.push` in `.fold` (names, desugar record, record_lit_merge)
- [x] Uncommitted batch: names, check_mutations, record_lit_merge, exprs, partial_application_desugar, corpus — commit on user command
- [x] Priority **stability** — COW_FOLD_AUDIT before Phase 2.7 / security / performance
- [x] Active track: **COW_FOLD_AUDIT** STEP=1
- [x] Deferred: parser `ref mut`; GENERIC_ALIASES; CPP_HEADER_IMPORT
- [x] Driver enqueued STEP=1 COW_FOLD_AUDIT


- [x] TRACK_LAMBDA_CAPTURE closed (steps 1–5); gate **998**/0
- [x] Uncommitted: VISITOR steps 7–12 + LAMBDA steps 1–5 — commit on user command
- [x] mlcc2 self-host `diff_exit=1` — checker `undefined` in `main.mlc` do-blocks (pre-existing parity gap)
- [x] Active track: **MLCC2_CHECKER_PARITY** STEP=1 (stability blocker)
- [x] Phase 2.6 roadmap items 6–9 closed (VISITOR, REGISTRY_SPLIT, LAMBDA, LOOP)
- [x] Baseline **998**/0
- [x] Driver enqueued STEP=1 MLCC2_CHECKER_PARITY

## Planner checklist (2026-06-16 plan-refresh — VISITOR_PATTERN closed)

- [x] TRACK_VISITOR_PATTERN closed (steps 1–13); gate **992**/0; `diff_exit=0`
- [x] Uncommitted VISITOR batch steps 7–12 — see TRACK STEP=13 note; commit on user command
- [x] Active track: **LAMBDA_CAPTURE** (correctness / MEMORY_MODEL `[=]`); step 1 done uncommitted
- [x] Priority **stability** — finish LAMBDA_CAPTURE (steps 2–5) before new tracks
- [x] Baseline **992**/0; self-host `diff_exit=0`
- [x] Driver enqueued STEP=2 LAMBDA_CAPTURE

## Planner checklist (2026-06-15 plan-refresh — LOOP_CONTRACTS closed)

- [x] TRACK_LOOP_CONTRACTS closed (`a28a4823`); regression_gate **14/0**
- [x] TRACK_TYPE_ALIASES closed (audit 2026-06-06); TYPE_ALIASES STEP=6 deferred (generics)
- [x] Priority **stability** — finish VISITOR_PATTERN (steps 4–13) before LAMBDA_CAPTURE / Phase 2.7
- [x] Active track unchanged: **VISITOR_PATTERN** STEP=4 (mid-track; no new STEP=1)
- [x] Deferred: LAMBDA_CAPTURE; generic record aliases; parser `ref mut` branch; regex string match
- [x] Baseline **984**/0; self-host `diff_exit=0` (step 3)
- [x] Driver enqueued STEP=4 VISITOR_PATTERN

## Planner checklist (2026-05-19 plan-refresh — RENAME_ABBREV closed)

- [x] RENAME_ABBREV closed (`0c68101b`, `7bc13d09`); gate **961**/0; self-host diff empty
- [x] Priority **stability** — TYPE_ALIASES before VISITOR_PATTERN
- [x] Next track: **TYPE_ALIASES** STEP=1 (parser `DeclTypeAlias`)
- [x] Deferred: VISITOR_PATTERN; regex string match; parser `ref mut` branch
- [x] Driver enqueued STEP=1 TYPE_ALIASES

## Planner checklist (2026-06-05 plan-refresh — 8 driver turns, RENAME_ABBREV mid-track)

- [x] 8 driver turns since last plan-refresh (steps 14–21 RENAME_ABBREV)
- [x] Priority **stability > security > performance** — finish RENAME (22–23) before TYPE_ALIASES / VISITOR
- [x] RENAME steps 1–21 done; gate **961**/0; self-host `diff_exit=0`; **uncommitted** (steps 18–21 batch)
- [x] STEP=22 scoped: `"` literal audit across `compiler/`; no blind `replace_all` on short tokens
- [x] Deferred: TYPE_ALIASES, VISITOR_PATTERN, regex string match; parser `ref mut` branch; e2e/fuzz corpus renames
- [x] Driver enqueued STEP=22 RENAME_ABBREV

## Planner checklist (2026-05-19 plan-refresh — STRING_MATCH steps 1–11 done)

- [x] TRACK_STRING_MATCH steps 1–11 done (960/0; pipe `match` + audit step 11)
- [x] Step 12 pending: Driver close + self-host diff + TRACK status **closed**
- [x] Priority: finish STRING_MATCH close before new Phase 2.5/2.6 tracks
- [x] Next track: **TYPE_ALIASES** (PLAN §2.5 #4) — before RENAME_ABBREV / VISITOR (stability: smaller surface than rename)
- [x] Deferred: regex string match (§2.5 #2); **RENAME_ABBREV** (step 1 partial WIP uncommitted — do not re-loop STEP=1 blind); **VISITOR_PATTERN** (after rename)
- [x] Baseline **960**/0; `build.sh` OK; self-host diff pending step 12
- [x] Driver enqueued STEP=12 STRING_MATCH close

## Planner checklist (2026-06-04 plan-refresh — REGISTRY_SPLIT closed)

- [x] TRACK_REGISTRY_SPLIT closed (`20f9d45`; steps 1–11)
- [x] Priority: stability first — REGISTRY done; open Phase 2.5 language tracks
- [x] Chosen **STRING_MATCH** (PLAN §2.5 #1) before TYPE_ALIASES / RENAME / VISITOR
- [x] Deferred: regex string match (§2.5 #2); TYPE_ALIASES (#4); VISITOR (#2.6); RENAME_ABBREV (large, after aliases)
- [x] Baseline **940**/0; gate per TRACK
- [x] Driver enqueued STEP=1 STRING_MATCH

## Planner checklist (2026-06-04 plan-refresh — 8 driver turns, REGISTRY_SPLIT mid-track)

- [x] 8 driver turns since last plan-refresh (`01a04df`…`ad4cdf7`)
- [x] Priority **stability > security > performance** — finish REGISTRY_SPLIT before Phase 2.5 language tracks
- [x] REGISTRY steps 1–6 done: indices, migrate, cpp_printer join, codegen map, `named_args`, `substitution`
- [x] Baseline **940**/0; `build.sh` OK; self-host diff N/A (`build_bin` pre-existing)
- [x] STEP=7 scoped: `hof_method_spec` unifies infer/transform HOF dispatch (not full visitor rewrite)
- [x] Deferred: STRING_MATCH, TYPE_ALIASES, RENAME_ABBREV, VISITOR_PATTERN; parser `ref mut` branch
- [x] Driver enqueued STEP=7 REGISTRY_SPLIT

## Planner checklist (2026-05-19 plan-refresh — REGISTRY_SPLIT open)

- [x] TRACK_MODULE_TU_BRIDGE closed (`df744e5`; step 6 commit)
- [x] TRACK_DECL_FRAGMENT_CLEANUP closed (940/0; `decl_cpp`/`cpp_ast` survivors WIP uncommitted)
- [x] Priority Phase 2.6 §7 → REGISTRY_SPLIT (stability: shrink TypeRegistry god-object)
- [x] REGISTRY step 1 done worktree; **commit pending** before step 2 migrate
- [x] Deferred: DECL_FRAGMENT WIP commit batch, self-host `build_bin` pre-existing
- [x] Driver enqueued STEP=1 REGISTRY_SPLIT (commit registry.mlc)

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

## Status: **closed** (2026-06-16 — steps 1–9 child tracks done)

| Step | Item | Track | Status |
|------|------|-------|--------|
| 6 | Visitor-паттерн + методы на контексте + Display | [TRACK_VISITOR_PATTERN.md](TRACK_VISITOR_PATTERN.md) | **closed** |
| 8 | Lambda capture: `[&]` → `[=]` в codegen | [TRACK_LAMBDA_CAPTURE.md](TRACK_LAMBDA_CAPTURE.md) | **closed** |

## Planner checklist (2026-06-03 plan-refresh)

- [x] CODE_REVIEW_2026_06.md создан — полный анализ 138 файлов
- [x] Phase 2.5 описана в PLAN.md (языковые улучшения)
- [x] Phase 2.6 описана в PLAN.md (структурный рефакторинг)
- [x] Треки созданы для всех задач Phase 2.5–2.6
- [x] MODULE_TU_BRIDGE закрыт (завершение Phase 2)
- [x] TRACK_DECL_FRAGMENT_CLEANUP закрыт (940/0)
- [ ] TRACK_STRING_MATCH выполнен
- [x] TRACK_DESTRUCTURING_APPLY выполнен (`fd95af6`)
- [ ] TRACK_TYPE_ALIASES выполнен
- [ ] TRACK_RENAME_ABBREV выполнен
- [ ] TRACK_VISITOR_PATTERN выполнен
- [x] TRACK_REGISTRY_SPLIT выполнен (`20f9d45`)

---

# Track: Phase 3.5 — C++ Header Import (2026-06)

Parent: [../PLAN.md](../PLAN.md) §Phase 3.5

## Status: **planned** (после Phase 2.5–2.6)

Дизайн: [../CPP_PARSER_DESIGN.md](../CPP_PARSER_DESIGN.md)

| Step | Item | Track | Status |
|------|------|-------|--------|
| A | Расширить лексер cpp/lexer.mlc | TRACK_CPP_HEADER_IMPORT.md | planned |
| B | Расширить CppType + CppDecl в cpp_ast.mlc | TRACK_CPP_HEADER_IMPORT.md | planned |
| C | Парсер типов compiler/cpp/parser/types.mlc | TRACK_CPP_HEADER_IMPORT.md | planned |
| D | Парсер деклараций compiler/cpp/parser/decls.mlc | TRACK_CPP_HEADER_IMPORT.md | planned |
| E | Парсер выражений compiler/cpp/parser/exprs.mlc | TRACK_CPP_HEADER_IMPORT.md | planned |
| F | Тесты test_cpp_parser.mlc (≥30 кейсов) | TRACK_CPP_HEADER_IMPORT.md | planned |
| G | Интеграция с import системой mlcc | TRACK_CPP_HEADER_IMPORT.md | planned |

---

# Track: Phase 2.7 — Language prerequisites for C++ parser (2026-06)

Parent: [../PLAN.md](../PLAN.md) §Phase 2.7

## Status: **planned** (после Phase 2.5–2.6)

Цель: до начала C++ парсера язык MLC должен иметь всё необходимое для идиоматичного парсера.
Чтобы не переписывать потом.

| Step | Item | Track | Status |
|------|------|-------|--------|
| 1 | Обобщённые алиасы `type ParseResult<T>` | TRACK_GENERIC_ALIASES.md | planned |
| 2 | String match (уже в Phase 2.5) | TRACK_STRING_MATCH.md | planned |
| 3 | `guard let` (ранный выход без пирамиды match) | TRACK_GUARD_LET.md | planned |
| 4 | `char` как первоклассный тип для лексера | TRACK_CHAR_TYPE.md | deferred |

### Почему каждый пункт важен

**1. Обобщённые алиасы** — без них для C++ парсера понадобится ~10 отдельных result-типов:
```mlc
// Без обобщённых алиасов — как сейчас:
type CppTypeResult = CppTypeResult { value: Shared<CppType>, parser: CppParser }
type CppDeclResult = CppDeclResult { value: Shared<CppDecl>, parser: CppParser }
type CppExprResult = CppExprResult { value: Shared<CppExpr>, parser: CppParser }
// ... и так для каждого типа

// С обобщёнными алиасами:
type CppParseResult<T> = CppParseResult { value: T, parser: CppParser }
fn parse_type(parser: CppParser) -> CppParseResult<Shared<CppType>> = ...
fn parse_decl(parser: CppParser) -> CppParseResult<Shared<CppDecl>> = ...
```

**2. String match** — `cpp_keyword_kind` сейчас 50 строк if/else. С string match — 15 строк match.

**3. `guard let`** — без него парсер пишется через глубокие match-ветки:
```mlc
// Без guard let — пирамида:
let result = parse_type(parser)
match result {
  Ok({ value, parser: p2 }) =>
    let result2 = parse_name(p2)
    match result2 {
      Ok({ value: name, parser: p3 }) => ...
    }
}

// С guard let + ?:
let { value: type_value, parser: p2 } = parse_type(parser)?
let { value: name, parser: p3 } = parse_name(p2)?
```

**4. `char` тип** (деферред) — сейчас `is_alpha`, `is_digit` работают через `string` с длиной 1. Достаточно.

**Уже в языке (NB):**
- Синтаксис обновления записей: `RecordType { ...base, field: val }` — **уже работает**
- `?` оператор — **уже работает**
- `Option<T>` с `.map()`, `.filter()`, `.find()` — **уже работает**
- `extend Type { fn method(self) }` — **уже работает**
- `Parser` как cursor с методами — **паттерн уже есть**, перенести на CppParser
