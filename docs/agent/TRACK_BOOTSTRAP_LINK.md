# Track: Bootstrap link parity (fresh emit → g++)

Parent: [../PLAN.md](../PLAN.md) §Phase 4; regresses [TRACK_SELF_HOST_BOOTSTRAP.md](TRACK_SELF_HOST_BOOTSTRAP.md)

## Status: **open** STEP=8-P8d (bootstrap link → parity → self-host)

**Priority (2026-06-28 plan-refresh):** `build_tests.sh` green → **`MLCC_BOOTSTRAP=1`** → parity → self-host diff. **Не enqueue bare `STEP=8`**. P8d catalog (incremental) **done** (144/0).

**Loop guard:** **не enqueue `Driver:8`** (4× stuck); use **`STEP=8-P8d0`…`8-P8e`**; **не enqueue `Driver:7`**.

**STEP=6 verify (2026-06-27):** mlcc emit **7/7** parser/checker TUs `g++ -fsyntax-only` **ok** (`predicates`, `exprs`, `decls`, `types`, `type_index`, `infer_question_expression`, `expr_visitor_cpp`); `TokenKind_is_eof` unqualified in `Parser_at_eof`; `build_tests.sh` **ok**; mlcc main **~2.4–3.1s**; `MLCC_BOOTSTRAP=1` **FAIL** (`.tmp_bootstrap_gate/build.log`: `check.cpp`, `context.cpp`, `check_mutations.cpp`, …); parity/self-host diff **pending**.

**STEP=6 codegen (2026-06-27):** `type_index.mlc` `ref mut` method_owners; `module.mlc` per-module `method_owners` + `extend_qualified_map`; `context.mlc` `qualify_function_callee`/`context_resolve`; `transform.mlc` extend→call; `decl_index.mlc` mangled extend qualified map.

**STEP=8 P8d6 sweep (2026-06-28):** `compiler/out` g++ sweep **144 ok / 0 fail** (~12–19 min); incremental emit catalog **green**; `MLCC_BOOTSTRAP=1` **next**.

**STEP=8 P8d4 (2026-06-28):** `mir_eval.cpp`+`execute.cpp`+`eval.cpp` **0 err**; P8d5 sample (`lexer`,`print`,`symbols`,`substitution`) **0 err**; emit **~4.0s**; `build_tests.sh` **ok**; bootstrap gate **pending**.

**STEP=8 P8d3 (2026-06-28):** `infer_match.cpp`+`infer_lambda_context.cpp`+`infer_operand_combine.cpp` **0 err**; emit **~7.0s**; `build_tests.sh` **ok**; bootstrap gate **pending**.

**STEP=8 P8d2 (2026-06-28):** `stmt_eval.cpp`+`stmt_cpp.cpp`+`let_pat.cpp`+`let_pat_cpp.cpp` **0 err**; emit **~4.9s**; `build_tests.sh` **ok**; bootstrap gate **pending**.

**STEP=8 P8d2 partial (2026-06-28):** `stmt_cpp.cpp`+`let_pat.cpp`+`let_pat_cpp.cpp` **0 err**; `stmt_eval.cpp` **578 err** (in progress); emit **~2.9s**; gate **FAIL**.

**STEP=8 P8d1b (2026-06-28):** `match_gen.cpp` **0 err** (gen_arm helpers); P8d1 `method_gen.cpp` **0 err**; emit **~3.6s**; `build_tests.sh` **ok**; gate **FAIL**.

**STEP=8 P8d1 (2026-06-28):** `method_gen.cpp` **0 err**; `match_gen.cpp` partial (type helpers ok; **257 err** gen_arm); emit **~2.7s**.

**STEP=8 baseline (2026-06-28):** g++ sweep bootstrap **113 ok / 31 fail**; `build_tests.sh` **ok**; emit **~2.3s**; gate **FAIL**; uncommitted **78 files**.

**STEP=8 P8c (2026-06-28):** `infer_call_support.mlc`+`let_pattern_infer.mlc` → **0 err**; emit **~2.3s**; bootstrap **32→29 TU** (`build_step8b.log`); gate **FAIL**.

**STEP=8 P8b (2026-06-28):** `type_diagnostics.mlc`+`type_gen.mlc` → **0 err**; checker batch **green**; bootstrap **32 TU** (`build_step8.log`); emit **~2.8s**; gate **FAIL**.

**STEP=8 P8a (2026-06-28):** `registry.mlc` dispatch helpers + field lookup helpers → `registry.cpp` **0 err**; bootstrap **57→35** TU (`build3.log`); emit **~2.5s**; gate **FAIL**.

**STEP=6-gate verify (2026-06-28):** P2 **done**; P2e **wip:** `decl_segment_for_extend` phase-2 protos → `predicates.hpp` extend decls; bootstrap **71→57** TU; `comma_separated.cpp` **ok**; log `.tmp_bootstrap_gate/build2.log`; parity **pending**.

**STEP=6-gate verify (2026-06-27):** P0 **ok:** `check.cpp`+`check_mutations.cpp`. P1 **ok:** `context.cpp`. P2 **wip:** `array_method_types.mlc` — убран `do` wrapper; mlcc emit **g++ ok**; emit **~2.3s**; `MLCC_BOOTSTRAP=1` **pending**.

**STEP=6 partial done (2026-06-26):** source workarounds + `type_index.mlc` (no `return end` in `do`); Ruby emit `build.sh` **ok**; mlcc self-emit blocked by extend-method codegen parity (not if-tail splice alone).

**STEP=5 done partial (2026-06-26):** P0 mlcc ok; `types.cpp` + `infer_call.cpp` fresh emit `g++ -fsyntax-only` **ok** (`types.mlc`, `infer_call.mlc`, partial `exprs.mlc`). **Remain:** `exprs.cpp`/`decls.cpp`/`infer_question_expression.cpp`/`expr_visitor_cpp.cpp` (if-else tail / `const x = if` in `do`).

**STEP=5 partial (2026-06-26):** P0 mlcc stability **ok**. `types.cpp` + `infer_call.cpp` fresh emit `g++ -fsyntax-only` **ok**. `exprs.cpp`/`decls.cpp`/`infer_question_expression.cpp`/`expr_visitor_cpp.cpp` still fail (if-else tail / `const x = if` in `do`). `MLCC_BOOTSTRAP=1` **pending**.

**STEP=4 done partial (2026-06-26):** catalog source fixes (if-else tail codegen workarounds): `send_safe.mlc`, `verify_semantic_ir.mlc`, `verify_ast.mlc`, `const_fold.mlc`, `mir_bootstrap_report.mlc`, `partial_application_desugar.mlc`, `lower_fn.mlc`, `simplify_cfg.mlc`, `mir_to_cpp.mlc`, `match_gen.mlc`, `vm/interpreter.mlc`, `literals.mlc`. Ruby emit `/tmp/mlc_emit_ruby`: **14/14** catalog TU `g++ -fsyntax-only` **ok**.

**STEP=3 done (2026-06-26):** parser `parse_if_expr` else: same-line → `parse_expr`, multiline → stmts until `end`/toplevel decl; `gen_char_literal` `\n`→10; `mir_bootstrap_report.mlc` spurious `end` removed; fresh mlcc emit `g++ -fsyntax-only` ok on `const_fold.cpp`, `dump_flags.cpp`, `mir_passes.cpp`. Gate: `build.sh` ok; `build_tests.sh` run_tests **segfault** (dump-mir probe, pre-existing).

**Depends on:** TRACK_SELF_HOST_BOOTSTRAP closed (gate regressed).

**STEP=1 note (2026-06-26):** [BOOTSTRAP_LINK_CATALOG.md](BOOTSTRAP_LINK_CATALOG.md) — 19 failing `.cpp`; log `.tmp_verify/bootstrap_link.log`; incremental `build.sh` ok; bootstrap ~437s → **FAIL**.

**Baseline (2026-06-26):** `compiler/out/mlcc` ok; codegen self-host `diff_exit=0`; `MLCC_BOOTSTRAP=1 compiler/build.sh` **FAIL** on fresh `compiler/out/bootstrap/*.cpp` (g++ compile errors).

**Known failing areas (sample):** `const_fold.cpp` (undeclared MIR helpers, broken lambdas); `pipeline.cpp` (`std::visit` return-type mismatch on `Result`); `dump_flags.cpp` (syntax); `lower_fn.cpp` / `lower_program.cpp`; `infer_mutex_method.cpp` (`?` void vs `Array<Diagnostic>`).

**Goal:** `MLCC_BOOTSTRAP=1 compiler/build.sh` → `out/bootstrap/mlcc_bootstrap`; `run_mlcc_bootstrap_parity.sh` **diff_exit=0**.

## Verify gate (every step)

```
compiler/tests/build_tests.sh
MLCC_BOOTSTRAP=1 compiler/build.sh
bash compiler/tests/run_mlcc_bootstrap_parity.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2
```

Use `TMPDIR` in repo if `/tmp` tight.

---

| Step | Item | Status |
|------|------|--------|
| 1 | Reproduce + catalog g++ errors on fresh bootstrap emit (`MLCC_BOOTSTRAP=1`; log failing `.cpp`) | done |
| 2 | Fix `pipeline.cpp` / `Result` `std::visit` codegen | done (if-chain + enclosing return; STEP=4) |
| 3 | Fix MIR emit: `const_fold.cpp`, `dump_flags.cpp`, `mir_passes.cpp` | done |
| 4 | Fix remaining bootstrap compile errors (`lower_*`, checker emit) until catalog `g++` green | done (partial — Ruby emit; mlcc SEGV open) |
| 5 | mlcc stability + types/infer_call emit green | done (partial) |
| 6 | mlcc STEP=6 TUs + perf; gate bootstrap/parity/self-host | done (partial) |
| 7 | STEP=6-gate: catalog emit P2 + bootstrap protos fix | done (partial) |
| 8 | Bootstrap link green + parity + self-host | pending (incremental sweep **144/0**; bootstrap build pending) |

### STEP=8 sub-steps (active)

**Priority:** stability (build_tests + incremental mlcc) > gate green > perf.

1. **P8a registry emit:** `registry.cpp` — **done** (0 err; bootstrap 57→35 TU)
2. **P8b checker/transform emit:** `transform`, `names`, `semantic_type_structure`, `type_diagnostics`, `type_gen` — **done** (0 err)
3. **P8c infer/let-pattern emit:** `infer_call_support`, `let_pattern_infer` — **done** (0 err)
4. **P8d residual catalog** — **done** (incremental sweep **144/0**)
5. **P8e gate:** `MLCC_BOOTSTRAP=1` → `run_mlcc_bootstrap_parity.sh` → p1→mlcc2→p2 `diff -rq` — **active**

#### P8d batches (enqueue as `STEP=8-P8dN`, not `STEP=8`)

| Id | Scope | `.mlc` targets | verify |
|----|-------|----------------|--------|
| **P8d0** | **Commit split** (78 uncommitted) | **commit #1 (≤8):** `registry.mlc`, `transform.mlc`, `names.mlc`, `semantic_type_structure.mlc`, `type_diagnostics.mlc`, `type_gen.mlc`, `infer_call_support.mlc`, `let_pattern_infer.mlc` + matching `docs/agent/*` | `build_tests.sh` + `MLCC_FORCE_RUBY=1 compiler/build.sh` after commit |
| **P8d1** | Codegen expr/stmt | `match_gen.mlc`, `method_gen.mlc` | `g++ -fsyntax-only` on emitted `.cpp`; emit timing |
| **P8d2** | Codegen let/stmt | `stmt_eval.mlc`, `stmt_cpp.mlc`, `let_pat.mlc` | **done** (4/4 TU 0 err) |
| **P8d3** | Infer cluster | `infer_match.mlc`, `infer_lambda_context.mlc`, `infer_operand_combine.mlc` | **done** (3/3 TU 0 err) |
| **P8d4** | VM/MIR | `mir_eval.mlc`, `execute.mlc`, `eval.mlc` (if in tree) | **done** (3/3 TU 0 err) |
| **P8d5** | Residual | `lexer.mlc`, `print.mlc`, `symbols.mlc`, `substitution.mlc`, … | **done** (sweep 144/0) |
| **P8d6** | Recount | full bootstrap `g++` sweep + `MLCC_BOOTSTRAP=1` | incremental **144/0** **done**; bootstrap build → **P8e** |

**Codegen fix pattern (repeat):** no `_ =>` in value-returning `match`; no `do`+`match` tail; helper fns with explicit return types; if-else expr not deep chains; fold lambdas → named fns / while-loops.

**Known failing `.cpp` (2026-06-28 sweep):** `match_gen`, `method_gen`, `stmt_eval`, `stmt_cpp`, `mir_eval`, `execute`, `eval`, `infer_match`, `infer_lambda_context`, `let_pat`, `let_pat_cpp`, `lexer`, `print`, … (31 total).

### STEP=6-gate sub-steps (done partial)

1. **P0 checker emit:** `check.mlc` / `check_mutations.mlc` — **done** (`check.cpp`, `check_mutations.cpp` g++ ok)
2. **P1 context emit:** `context.mlc` — **done** (`context.cpp` g++ ok)
3. **P2 catalog emit (mlcc bootstrap TU):** **done** (catalog subset g++ ok)
   - **P2a–P2d done:** checker/codegen/cpp_parse catalog TUs **g++ ok**
   - **P2e partial:** extend phase-2 protos **done**; bootstrap **57** TU
4. **`run_mlcc_bootstrap_parity.sh`** — after P2e
5. **Self-host:** p1 → mlcc2 → p2 `diff -rq`
6. Close track if gate green

### STEP=6 sub-steps (done partial)

1. **Parser emit:** `exprs.mlc` (`parse_if_expr`, `block_result`, `parse_arms_do_delimited`), `decls.mlc` — no `const x = if` in `do`; early `return`/`match`
2. **Checker/codegen emit:** `infer_question_expression.mlc`, `expr_visitor_cpp.mlc`
3. **`MLCC_BOOTSTRAP=1 compiler/build.sh`** green → `run_mlcc_bootstrap_parity.sh`
4. **Self-host:** p1 → mlcc2 → p2 `diff -rq`

### STEP=5 sub-steps

1. **P0 mlcc SEGV** — **done:** `match_gen`/`decl_cpp.with_enclosing_function_return_type`; `send_safe` emit ok
2. **`MLCC_BOOTSTRAP=1`** — **done partial:** `types.mlc` + `infer_call.mlc` emit green
3. **parity + self-host diff** — pending (blocked by bootstrap link / p1 `types.cpp`)
4. **perf:** `mlcc -o main.mlc` ~4.2s (ok)

### STEP=4 sub-steps (done partial)

1. Source workarounds: no trailing `if/then/else` one-liner fn bodies; pure `match` or `do`+early `return`
2. Ruby emit catalog: 14 TU `g++ -fsyntax-only` ok (`/tmp/mlc_emit_ruby`)
3. Deferred to STEP=5: mlcc runtime + bootstrap link gate

### STEP=2 sub-steps (wip — resume after STEP=3 or dedicated finish turn)

1. Commit or revert partial WIP in `match_gen.mlc`, `expr_visitor_cpp.mlc`, `cpp_ast.mlc`, `print.mlc`
2. `match_expression_return_cpp_type`: full `ast::Result<T,E>` when `sem_type_to_cpp` is bare `ast::Result` (no subject ok-type fallback for visit return)
3. `MLCC_FORCE_RUBY=1 compiler/build.sh` → `compiler/out/mlcc -o .tmp_verify/emit compiler/main.mlc` → `g++ -fsyntax-only -std=c++20 -I .tmp_verify/emit -I runtime/include .tmp_verify/emit/pipeline.cpp`
4. Compare visit arms to good Ruby incremental `compiler/out/pipeline.cpp` (`run_modular_compiler_pipeline`, `run_codegen_pipeline_pass`)

### STEP=3 sub-steps

1. Fresh emit: `rm -rf compiler/out/bootstrap && MLCC_BOOTSTRAP=1 compiler/build.sh` — capture `const_fold.cpp`, `dump_flags.cpp`, `mir_passes.cpp` errors
2. `const_fold.cpp`: stmt order / undeclared MIR helpers (`mir/const_fold.mlc`)
3. `dump_flags.cpp`: syntax `expected expression` (`driver/dump_flags.mlc`)
4. `mir_passes.cpp`: undeclared `const_fold_mir_function` forward ref (`mir/mir_passes.mlc`)
5. Gate: `compiler/tests/build_tests.sh`; incremental `compiler/build.sh` ok

### STEP=1 sub-steps

1. `rm -rf compiler/out/bootstrap && MLCC_BOOTSTRAP=1 compiler/build.sh 2>&1 | tee /tmp/bootstrap_link.log`
2. List first 10 failing `.cpp` + error class (undeclared / visit / syntax)
3. Confirm incremental `compiler/out/mlcc` still builds via `compiler/build.sh` (no bootstrap)

### Out of scope

- Ruby bootstrap path removal (done)
- Build-speed / PCH (TRACK_BUILD_SPEED closed)
- Reddit demo assets
