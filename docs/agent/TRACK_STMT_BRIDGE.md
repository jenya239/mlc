# Track: Stmt string bridge elimination (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_MATCH_BRIDGE.md](TRACK_MATCH_BRIDGE.md) (**closed**, `3e47ca4`)

## Status: **active** (step 3 pending)

**Goal:** eliminate `stmt_via_string_bridge` in `codegen/stmt_cpp.mlc` — native `CppStmt` for remaining statement arms that round-trip through `print_expr` → string → `CppStmtFragment`.

**Constraints:**
- One concern per sub-step; no `compiler/` + `lib/mlc/` in one commit.
- Self-host gate when `compiler/**` touched: `build_bin.sh` + diff empty.

## Verify gate (every step)

```
bundle exec rake test_compiler_mlc   # 762 pass (baseline post MATCH_BRIDGE)
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | `SStmtReturn` + `?` — native CppReturn (no stmt_via_string_bridge) | done (`2bde7d1`) |
| 2 | `SStmtLet` / `SStmtLetConst` — question, block, if initializer paths native | done (`b158885`) |
| 3 | `SStmtExpr` — assign, if/while/for/with/block native CppExprStmt | pending |
| 4 | `SStmtLetPat` — native CppStmt decomposition | pending |
| 5 | Remove `stmt_via_string_bridge`; audit `CppStmtFragment`; close track | pending |

## Survivors (post step 2)

`SStmtLet` + `?`, `SStmtLetConst` block/if use native `CppAutoDecl`/`CppConstexprAutoDecl`. Remaining bridges: `Map.new`, let-pat, expr stmt.

## Next step (Driver)

**STEP=3** — `SStmtExpr` native paths in `stmt_cpp.mlc`.

## Deferred (out of track)

- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
- Phase 4 `MLCC_BOOTSTRAP=1` — separate track (after stmt bridges stable).
- `codegen/expr/expr.mlc` templates still used inside guarded match body strings.
