# Track: mlcc optimization (plan 2026-05)

## Status: **closed** (commit `419de62`)

Baseline: `benchmarks/profile/record_baseline.sh` (post-commit).

---

# Track: PLAN.md roadmap

Parent: [../PLAN.md](../PLAN.md)

## Status: in progress

| Step | Item | Status |
|------|------|--------|
| 1 | `compiler/pass.mlc` — `trait CompilerPass<Input, Output>` | done (2026-05-20) |
| 2 | `ExprVisitor` sketch + dispatch stub | done (2026-05-22) |
| 3 | `ExprVisitor<string>` codegen sketch | done (2026-05-22) |
| 4 | `dispatch_expr` + test graph (466 tests) | done (2026-05-22) |
| 5 | Wire `gen_expr_via_string_visitor` into `expr_eval` | done (2026-05-22) |
| 6 | Parser `ref mut` | deferred (separate branch) |

## Step 5 detail

- `expr_eval.mlc`: int/str/ident → `gen_expr_via_string_visitor`
- build_tests 466 pass; mlcc rebuild OK

## Next step

Parser `ref mut` (separate branch) or migrate more `eval_expr` arms to ExprVisitor.
