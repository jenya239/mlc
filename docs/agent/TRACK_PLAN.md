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
| 9 | ExprVisitor: bin/unary ops arm | done (2026-05-22) |
| 10 | ExprVisitor: call + method arms | done (2026-05-22) |
| 11 | ExprVisitor: if + block arms | done (2026-05-22) |
| 12 | ExprVisitor: match arm | done (2026-05-22) |
| 13 | Remaining `expr_eval` → visitor (record, array, lambda, …) | done (2026-05-23) |
| 14 | Self-host diff after visitor migration batch | done (2026-05-24) — gen_trait_decl requires + static_assert; mlcc2 diff p1/p2 identical |
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

## Step 9 detail (done)

- `visit_bin`/`visit_un`; `gen_binary_via_visitor`/`gen_unary_via_visitor` with evaluate_expression callback; 477 tests

## Step 10 detail (done)

- `visit_call`/`visit_method`; `gen_call_via_visitor`/`gen_method_via_visitor`; string visitor tests for Ok{42} and push; 479 tests

## Step 11 detail (done)

- `visit_if`/`visit_block`; `gen_if_via_visitor`/`gen_block_via_visitor`; string visitor tests for if ternary and empty block; 481 tests

## Step 12 detail (done)

- `visit_match`; `gen_match_via_visitor`; string visitor tests for std::visit and match arm body; 483 tests

## Step 13 detail (done)

- Remaining arms: field/index/while/for/record/array/tuple/question/lambda/with/extern via visitor; string visitor tests for array and record; 485 tests; `dispatch_expr` still partial (record+ → unsupported)

## Step 14 detail (done — 14-fix-d)

- **14-fix:** self-hosted parser `export trait` in `parser/decls.mlc`
- **14-fix-b:** impl `#include` + `using namespace`; extend helper protos + cpp forwards
- **14-fix-c:** vtable `{Trait}_vtable` in cpp phase 4 — no concept clash in hpp
- **14-fix-d:** `gen_trait_decl` requires use prefixed `ExprVisitor_visit_*` + real return types; `trait_concept_adapters`; `static_assert(ExprVisitor<mlc::String, StringExprVisitor>)` in `expr_visitor_string.cpp`
- **verify:** g++ mlcc2 (+ `runtime/src/core/profile.cpp`); `diff -rq mlc_p1 mlc_p2` empty

## Next step (Driver)

Step **14-commit** — commit visitor migration batch (uncommitted >15 files); user command required

## Planner checklist

When `STEP=plan-refresh`:

- [ ] All PLAN §3 visitor arms have a TRACK row or explicit defer
- [ ] No step bundles `compiler/` + `lib/mlc/`
- [ ] Each pending step has verify gate (tests count or self-host)
- [ ] Done rows cite commit hash
