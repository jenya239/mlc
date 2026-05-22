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
| 3 | `ExprVisitor<string>` codegen sketch | **next** |
| 4 | Parser `ref mut` | deferred (separate branch) |

## Step 2 detail

- `compiler/expr_visitor.mlc` — trait + `dispatch_expr` stub (variant routing only; trait dispatch later)
- `compiler/tests/test_expr_visitor.mlc` — compile hook
- Trait renamed `Pass` → `CompilerPass` (avoid `TestResult::Pass` collision)

## Next step

Sketch `ExprVisitor<string>` impl module for codegen (no eval.mlc migration yet).
