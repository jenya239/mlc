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
- **Prefix return-body bridge:** [TRACK_PREFIX_BRIDGE.md](TRACK_PREFIX_BRIDGE.md) — **open**

## Next step (Driver)

**STEP=2** [TRACK_PREFIX_BRIDGE.md](TRACK_PREFIX_BRIDGE.md) — native while/for/break in return-body prefix.

## Planner checklist (2026-05-30 plan-refresh — PREFIX_BRIDGE)

- [x] TRACK_RETURN_BODY closed (`91d6752`; 5/5 steps)
- [x] TRACK_PREFIX_BRIDGE opened (5 steps: prefix-only, while/for prefix, nested arms, drop bridge, audit close)
- [x] PLAN §Phase 2 §3 — `return_body_needs_string_bridge` conservative fallback
- [x] Driver enqueued STEP=1

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
