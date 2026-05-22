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
| 4a | `dispatch_expr` wired in `expr_visitor_string` — **isolated compile** (not in tests_main) | **next** |
| 4b | Parser/checker: generic trait bounds in `where` (`ExprVisitor<VisitorResult>`) | pending |
| 4c | Trait C++ codegen pollution fix + add to test graph / `build_tests` green | pending |
| 5 | Parser `ref mut` | deferred (separate branch) |

## Step 3 detail (done)

- `compiler/codegen/expr_visitor_string.mlc` — `StringExprVisitor` + `gen_expr_via_string_visitor`
- Commit `df3826f`; standalone compile OK; not in `tests_main` (pollution — 4c)

## Step 4 — split (do not merge sub-steps)

### 4a — isolated compile (next)

- `dispatch_expr` + `call_visit_*` in `expr_visitor.mlc`
- `expr_visitor_string.mlc` uses `dispatch_expr` (no manual match)
- Verify: compile module graph **without** `tests_main` (dedicated script or minimal entry)
- Gate: one layer (`compiler/` only unless one-line Ruby fix blocks compile)
- Uncommitted ≤ 15 files before enqueue 4b

### 4b — semantic/parser

- Parser accepts generic trait type in `where` clauses
- Blocker ref: mlc-memory `known_limitations` — parser + `ExprVisitor<VisitorResult>`
- Verify: Ruby semantic on minimal `expr_visitor.mlc` snippet
- Gate: `lib/mlc/` only; `rake test_mlc` if checker/parser touched

### 4c — test graph

- Fix trait adapter C++ pollution in test modules
- Add to `tests_main` or isolated test target
- Verify: `build_tests.sh` 463+; self-host diff if needed

## WIP

Large uncommitted step-4 work on `main` → move to branch `feat/expr-visitor-dispatch` before continuing 4a.
