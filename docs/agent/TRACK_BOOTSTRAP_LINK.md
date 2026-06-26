# Track: Bootstrap link parity (fresh emit → g++)

Parent: [../PLAN.md](../PLAN.md) §Phase 4; regresses [TRACK_SELF_HOST_BOOTSTRAP.md](TRACK_SELF_HOST_BOOTSTRAP.md)

## Status: **open** STEP=4 **done (partial)** → STEP=5

**STEP=4 done partial (2026-06-26):** catalog source fixes (if-else tail codegen workarounds): `send_safe.mlc`, `verify_semantic_ir.mlc`, `verify_ast.mlc`, `const_fold.mlc`, `mir_bootstrap_report.mlc`, `partial_application_desugar.mlc`, `lower_fn.mlc`, `simplify_cfg.mlc`, `mir_to_cpp.mlc`, `match_gen.mlc`, `vm/interpreter.mlc`, `literals.mlc`. Ruby emit `/tmp/mlc_emit_ruby`: **14/14** catalog TU `g++ -fsyntax-only` **ok**. **Blocker (P0 stability):** `compiler/out/mlcc` **SEGV** on any `-o` compile (CppAst dtor in `gen_module`); bootstrap gate not run.

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
| 5 | mlcc stability + `MLCC_BOOTSTRAP=1` + parity + self-host diff; close track | pending |

### STEP=5 sub-steps (stability first)

1. **P0:** fix mlcc SEGV on compile — match if-chain fn-body path must end with `std::abort()` (see `send_safe.cpp` `-Wreturn-type`); verify `compiler/out/mlcc -o /tmp/x compiler/checker/send_safe.mlc` exit 0
2. `MLCC_FORCE_RUBY=1 compiler/build.sh` → fresh mlcc emit → `MLCC_BOOTSTRAP=1 compiler/build.sh` green
3. `bash compiler/tests/run_mlcc_bootstrap_parity.sh`; mlcc2 self-host `diff -rq`
4. Gate: full TRACK verify block

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
