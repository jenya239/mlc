# Bootstrap link error catalog (STEP=1)

Log: `.tmp_verify/bootstrap_link.log` (2026-06-26). Command: `rm -rf compiler/out/bootstrap && MLCC_BOOTSTRAP=1 compiler/build.sh`.

**Result:** `build_bin.sh: compile errors`; **19** `.cpp` with g++ errors; `mlcc_bootstrap` not produced. Incremental `compiler/build.sh` (no bootstrap): **ok** (mlcc skip/up to date).

**Build time (bootstrap attempt):** ~437s wall (parallel `build_bin.sh` on fresh emit).

## Failing translation units (by error-line count)

| # | File | Errors (approx) | Class | Likely MLC source |
|---|------|-----------------|-------|-------------------|
| 1 | `send_safe.cpp` | 333 | undeclared / broken tuple emit | `checker/send_safe.mlc` |
| 2 | `verify_semantic_ir.cpp` | 74 | `std::visit` / lambda capture | `checker/verify_semantic_ir.mlc` |
| 3 | `pipeline.cpp` | 57 | `std::visit` ret-type; `__mlcT` shadow; wrong call | `driver/pipeline.mlc` |
| 4 | `simplify_cfg.cpp` | 29 | visit + forward-ref in MIR pass | `mir/simplify_cfg.mlc` |
| 5 | `const_fold.cpp` | 24 | stmt reorder: calls before defs; type names as vars | `mir/const_fold.mlc` |
| 6 | `verify_ast.cpp` | 21 | `shared_ptr` / wrong arg type | `checker/verify_ast.mlc` |
| 7 | `mir_bootstrap_report.cpp` | 21 | undeclared helpers; `string` as identifier | `mir/mir_bootstrap_report.mlc` |
| 8 | `lower_fn.cpp` | 16 | undeclared locals; broken lambda/`Result` emit | `mir/lower_fn.mlc` |
| 9 | `partial_application_desugar.cpp` | 6 | visit / emit | `checker/transform/...` |
| 10 | `mir_to_cpp.cpp` | 6 | visit lambda return | `mir/mir_to_cpp.mlc` |
| 11 | `dump_flags.cpp` | 6 | syntax `expected expression` | `driver/dump_flags.mlc` |
| 12 | `infer_mutex_method.cpp` | 1 | `?` void vs `Array<Diagnostic>` | `checker/infer/infer_mutex_method.mlc` |
| 13 | `lower_program.cpp` | 1 | undeclared `lower_semantic_function` | `mir/lower_program.mlc` |
| 14 | `mir_passes.cpp` | 1 | undeclared `const_fold_mir_function` | `mir/mir_passes.mlc` |
| 15–19 | `cpp_decls.cpp`, `decl_cpp.cpp`, `decls.cpp`, `exprs.cpp`, `lexer.cpp` | ≤6 each | secondary / cascade | parser/codegen |

## Error classes (fix order per TRACK)

1. **visit / Result** — `pipeline.cpp` (STEP=2): `Ok`/`Err` arms must share return type.
2. **MIR pass emit** — `const_fold.cpp`, `dump_flags.cpp`, `mir_passes.cpp`, `simplify_cfg.cpp` (STEP=3).
3. **Checker / verifier** — `verify_semantic_ir.cpp`, `verify_ast.cpp`, `infer_mutex_method.cpp`, `send_safe.cpp` (STEP=4).
4. **MIR lower** — `lower_fn.cpp`, `lower_program.cpp`, `mir_bootstrap_report.cpp`, `mir_to_cpp.cpp` (STEP=4).

## Gate snapshot (STEP=1)

| Check | Result |
|-------|--------|
| `build_tests.sh` | ok (arch_lint 0 fail) |
| `compiler/build.sh` (no bootstrap) | ok |
| `MLCC_BOOTSTRAP=1 build.sh` | **FAIL** |
| `run_mlcc_bootstrap_parity.sh` | skip (no binary) |
