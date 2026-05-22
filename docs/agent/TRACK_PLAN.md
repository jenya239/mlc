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
| 6 | ExprVisitor: bool + unit arms | done (2026-05-22) |
| 7 | Parser `ref mut` | deferred (separate branch) |

## Step 6 detail

- Trait: `visit_bool`, `visit_unit`; `dispatch_expr` + StringExprVisitor
- `expr_eval`: bool/unit via visitor; 2 new tests (468 total)

## Next step

Parser `ref mut` (separate branch) or numeric literal arms (float/i64/u8/…).
