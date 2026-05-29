# Track: Match expr string bridge elimination (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_EXPR_BRIDGE.md](TRACK_EXPR_BRIDGE.md) (**closed**, `91ece96`)

## Status: **active** (step 3 pending)

**Goal:** eliminate match string bridges in `expr_visitor_cpp` — native `CppExpr` for `CppStdVisit` arm handlers and guarded `gen_match`.

**Constraints:**
- One concern per sub-step; no `compiler/` + `lib/mlc/` in one commit.
- Self-host gate when `compiler/**` touched: `build_bin.sh` + diff empty.

## Verify gate (every step)

```
bundle exec rake test_compiler_mlc   # 758 pass (baseline post EXPR_BRIDGE)
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | `gen_match_via_cpp_visitor` — arm handlers as native CppExpr (no-guard StdVisit path) | done |
| 2 | `gen_match_via_cpp_visitor` — guarded match as native CppExpr | done |
| 3 | Remove `cpp_expr_from_rendered_fragment` / `eval_expr_cpp_as_string` from expr_visitor; audit; close track | pending |

## Survivors (post step 2)

`expr_visitor_cpp.mlc`: `cpp_expr_from_rendered_fragment` unused; `eval_expr_cpp_as_string` still exported for `stmt_visitor_cpp` (step 3 audit).

## Next step (Driver)

**STEP=3** — remove dead bridges from expr_visitor; audit; close track.

## Deferred (out of track)

- `codegen/expr/expr.mlc` string templates for stmt/decl/module (TRACK_CPPGEN residual).
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
- Phase 4 `MLCC_BOOTSTRAP=1` — separate track.
