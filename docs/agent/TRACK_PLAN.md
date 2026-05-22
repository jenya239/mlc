# Track: mlcc optimization (plan 2026-05)

## Status: **closed** (commit `419de62`)

Baseline: `benchmarks/profile/record_baseline.sh` (post-commit).

---

# Track: PLAN.md roadmap

Parent: [../PLAN.md](../PLAN.md) §3 visitor + § порядок внедрения

## Status: in progress

| Step | Item | Status |
|------|------|--------|
| 1 | `compiler/pass.mlc` — `trait CompilerPass<Input, Output>` | done (2026-05-20) |
| 2 | `ExprVisitor` sketch + dispatch stub | done (2026-05-22) |
| 3 | `ExprVisitor<string>` codegen sketch | done (2026-05-22) |
| 4 | `dispatch_expr` + test graph (466 tests) | done (2026-05-22) |
| 5 | Wire `gen_expr_via_string_visitor` into `expr_eval` | done (2026-05-22) |
| 6 | ExprVisitor: bool + unit arms | done (2026-05-22) |
| 7 | ExprVisitor: numeric literal arms | done (2026-05-22) |
| 8 | ExprVisitor: ident arm | done (2026-05-22) |
| 9 | ExprVisitor: bin/unary ops arm | pending |
| 10 | ExprVisitor: call + method arms | pending |
| 11 | ExprVisitor: if + block arms | pending |
| 12 | ExprVisitor: match arm | pending |
| 13 | Remaining `expr_eval` → visitor (record, array, lambda, …) | pending |
| 14 | Self-host diff after visitor migration batch | pending |
| 15 | Parser `ref mut` | deferred (separate branch) |

## Backlog (Planner maintains)

Source: PLAN.md «Visitor pattern», «Порядок внедрения» items 2–4.

- After step 13: shrink manual `match` in `expr_eval.mlc` to `dispatch_expr` only
- Then: explicit pipeline in `main.mlc` (Pass wiring) — **new TRACK section or TRACK_PIPELINE.md**
- Folder restructure (PLAN §4) — only after visitor batch + self-host green

## Step 7 detail (done)

- Trait: `visit_float`, … `visit_char`; `expr_eval` via visitor; 473 tests

## Step 8 detail (done)

- `SExprIdent` via `gen_expr_via_string_visitor`; string visitor tests for x/print/println; 475 tests

## Next step (Driver)

Step **9** — bin/unary ops arm in `expr_eval` + tests. Verify `build_tests.sh`.

## Planner checklist

When `STEP=plan-refresh`:

- [ ] All PLAN §3 visitor arms have a TRACK row or explicit defer
- [ ] No step bundles `compiler/` + `lib/mlc/`
- [ ] Each pending step has verify gate (tests count or self-host)
- [ ] Done rows cite commit hash
