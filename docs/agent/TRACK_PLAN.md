# Track: mlcc optimization (plan 2026-05)

## Status: **closed** (commit `419de62`)

All phases done except parser `ref mut` (deferred, separate branch).

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
| 4 | `dispatch_expr` trait dispatch + test graph fix | **next** |
| 5 | Parser `ref mut` | deferred (separate branch) |

## Step 3 detail

- `compiler/codegen/expr_visitor_string.mlc` — `StringExprVisitor` + `gen_expr_via_string_visitor`
- Standalone compile OK; not in `tests_main` graph (trait bridge C++ pollution — step 4)

## Next step

Wire `dispatch_expr` trait method calls; fix trait codegen in test modules or add isolated test target.
