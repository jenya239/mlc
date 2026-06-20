# Track: Phase 2.6 remaining (visitor + context)

Parent: [../PLAN.md](../PLAN.md) §Phase 2.6; follows [TRACK_FORMATTER.md](TRACK_FORMATTER.md)

## Status: **closed** (2026-05-19, STEP=5)

**Baseline:** build_tests **1030/0**; mlcc2 checker parity ok; **diff_exit=0**.

**Goal:** eliminate remaining duplicate `match expression` in names/mutations; spread CodegenContext methods.

## Verify gate

```
compiler/tests/build_tests.sh
bash compiler/tests/run_mlcc2_checker_parity.sh
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | `names.mlc` — legacy `check_names_expr` → visitor-only entry; parity tests | done |
| 2 | `check_mutations.mlc` — legacy fn-body checks → visitor-only; parity tests | done |
| 3 | `transform.mlc` — remove `transform_expr_maybe_lambda_context` duplicate match where visitor covers | done |
| 4 | `codegen/` — `extend CodegenContext` methods for stmt/decl entry points | done |
| 5 | Audit survivors + full gate; close track | done |

**STEP=5 note (2026-05-19 Driver):** Gate **1030/0**; mlcc2 checker parity ok; **diff_exit=0**. **Removed this track:** legacy `check_mutation_expr`; `transform_expr_maybe_lambda_context`; per-form `check_names_expr` match body. **CodegenContext methods:** `gen_decl`/`gen_proto` (`decl.mlc`), `gen_decl_cpp`/`gen_proto_cpp` (`decl_cpp.mlc`), `gen_stmts_cpp` (`stmt_cpp.mlc`); export wrappers retained. **Intentional survivors:** (1) `dispatch_*_pass` single-match routers; (2) thin entry wrappers `check_names_expr`, `transform_expr`, `gen_expr`, `gen_decl`, `gen_stmts_cpp`; (3) `check_names_statements` Stmt-scope fold; (4) `partial_application_desugar_expr` pre-pass; (5) cross-module codegen: `eval.mlc` calls `gen_stmts_cpp(statements, self)` not `.gen_stmts_cpp` (C++ link); (6) `context.mlc` lookup/qualify free fns + `mutate_context_from_statement` hot path; (7) TypeRegistry sub-indices — separate perf track.

### Out of scope

- TypeRegistry sub-indices (separate perf track)
- Full rename-abbrev sweep (TRACK_RENAME_ABBREV closed)
