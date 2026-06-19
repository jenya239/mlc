# Track: Phase 2.6 remaining (visitor + context)

Parent: [../PLAN.md](../PLAN.md) §Phase 2.6; follows [TRACK_FORMATTER.md](TRACK_FORMATTER.md)

## Status: **open** (STEP=1 pending)

**Depends on:** TRACK_FORMATTER closed (or parallel after CODE_QUALITY if Planner reorders — default: after FORMATTER).

**Baseline:** TRACK_VISITOR_PATTERN closed; infer/transform/codegen on ExprVisitor.

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
| 1 | `names.mlc` — legacy `check_names_expr` → visitor-only entry; parity tests | pending |
| 2 | `check_mutations.mlc` — legacy fn-body checks → visitor-only; parity tests | pending |
| 3 | `transform.mlc` — remove `transform_expr_maybe_lambda_context` duplicate match where visitor covers | pending |
| 4 | `codegen/` — `extend CodegenContext` methods for stmt/decl entry points | pending |
| 5 | Audit survivors + full gate; close track | pending |

### Out of scope

- TypeRegistry sub-indices (separate perf track)
- Full rename-abbrev sweep (TRACK_RENAME_ABBREV closed)
