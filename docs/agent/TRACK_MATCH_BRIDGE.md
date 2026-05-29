# Track: Match expr string bridge elimination (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_EXPR_BRIDGE.md](TRACK_EXPR_BRIDGE.md) (**closed**, `91ece96`)

## Status: **closed** (`3e47ca4`)

**Goal:** eliminate match string bridges in `expr_visitor_cpp` — native `CppExpr` for `CppStdVisit` arm handlers and guarded `gen_match`.

| Step | Item | Status |
|------|------|--------|
| 1 | `gen_match_via_cpp_visitor` — arm handlers as native CppExpr (no-guard StdVisit path) | done (`953b659`) |
| 2 | `gen_match_via_cpp_visitor` — guarded match as native CppExpr | done (`6d7772c`) |
| 3 | Remove `cpp_expr_from_rendered_fragment` / `eval_expr_cpp_as_string` from expr_visitor; audit; close track | done (`3e47ca4`) |

## Audit (step 3)

- `cpp_expr_from_rendered_fragment` — removed (unused after steps 1–2).
- `eval_expr_cpp_as_string` — removed; callers use `print_expr(eval_expr_cpp(...))` directly (`stmt_cpp.mlc`, `eval.mlc`).

## Deferred (out of track)

- `codegen/expr/expr.mlc` string templates for stmt/decl/module (TRACK_CPPGEN residual).
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
- Phase 4 `MLCC_BOOTSTRAP=1` — separate track.
