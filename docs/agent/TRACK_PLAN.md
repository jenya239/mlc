# Track: mlcc optimization (plan 2026-05)

## Status: **closed** (commit `419de62`)

Baseline: `benchmarks/profile/record_baseline.sh` (post-commit).

---

# Track: PLAN.md roadmap

Parent: [../PLAN.md](../PLAN.md) §3 visitor + § порядок внедрения

## Status: visitor batch **done** (steps 1–14); pipeline + cleanup in progress

| Step | Item | Status |
|------|------|--------|
| 1 | `compiler/pass.mlc` — `trait CompilerPass<Input, Output>` | done (`f199e92`) |
| 2 | `ExprVisitor` sketch + dispatch stub | done (`f199e92`) |
| 3 | `ExprVisitor<string>` codegen sketch | done (`df3826f`) |
| 4 | `dispatch_expr` + test graph | done (`161c3b4`) |
| 5 | Wire `gen_expr_via_string_visitor` into `expr_eval` | done (`0afcfd8`) |
| 6 | ExprVisitor: bool + unit arms | done (`a9a2ca6`) |
| 7 | ExprVisitor: numeric literal arms | done (`4e293c2`) |
| 8 | ExprVisitor: ident arm | done (`274dd68`) |
| 9 | ExprVisitor: bin/unary ops arm | done (`678e5d0`) |
| 10 | ExprVisitor: call + method arms | done (`b6c857a`) |
| 11 | ExprVisitor: if + block arms | done (`008abce`) |
| 12 | ExprVisitor: match arm | done (`14d9b0d`) |
| 13 | Remaining `expr_eval` → visitor (record, array, lambda, …) | done (`9bc1001`) |
| 14 | Self-host diff after visitor migration batch | done (`6df3799`, docs `0a0d321`) |
| 15 | Parser `ref mut` | **deferred** (separate branch) |
| 16 | Shrink `expr_eval.mlc` — `dispatch_expr` only, drop duplicate `match` | done (pending commit) |
| 17 | Explicit `Pass` pipeline in `main.mlc` | pending |
| 18 | Folder restructure (PLAN §4) | **deferred** — after 16–17 + self-host green |

## Backlog (Planner maintains)

Source: PLAN.md §3 «Порядок внедрения» items 3–4.

- **Step 16:** `extend CodegenContext` methods — optional follow-up inside expr_eval shrink if needed
- **Step 17:** `CompilerPipeline` wiring replaces direct checker→codegen calls in `main.mlc`
- **Step 18:** `frontend/`, `ir/`, checker split — separate track; not until pipeline stable
- **CppExpr backend:** PLAN Phase 2 — not in this track (new TRACK when pipeline done)

## Step 14 detail (done — `6df3799`)

- parser `export trait`; trait codegen (requires, vtable, static_assert)
- verify: g++ mlcc2 + `diff -rq p1 p2` empty; 490 tests

## Step 16 detail (done)

- `expr_eval.mlc` → `eval_expr_with_visitor` (3 lines); match removed from expr_eval
- Single dispatch in `expr_visitor_string.mlc`; `StringExprVisitor` carries `gen_stmts` + `evaluate_expression`
- `dispatch_expr` in `expr_visitor.mlc` — full SExpr coverage + wildcard (if-chain codegen)
- verify: 490 tests pass

## Next step (Driver)

**STEP=17** — explicit `Pass` pipeline in `main.mlc`

## Planner checklist (2026-05-24 plan-refresh)

- [x] All PLAN §3 visitor arms have a TRACK row or explicit defer
- [x] No step bundles `compiler/` + `lib/mlc/`
- [x] Each pending step has verify gate (tests count or self-host)
- [x] Done rows cite commit hash
