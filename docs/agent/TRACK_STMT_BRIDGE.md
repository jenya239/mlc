# Track: Stmt string bridge elimination (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_MATCH_BRIDGE.md](TRACK_MATCH_BRIDGE.md) (**closed**, `3e47ca4`)

## Status: **closed**

**Goal:** eliminate `stmt_via_string_bridge` in `codegen/stmt_cpp.mlc` — native `CppStmt` for remaining statement arms that round-trip through `print_expr` → string → `CppStmtFragment`.

---

| Step | Item | Status |
|------|------|--------|
| 1 | `SStmtReturn` + `?` — native CppReturn (no stmt_via_string_bridge) | done (`2bde7d1`) |
| 2 | `SStmtLet` / `SStmtLetConst` — question, block, if initializer paths native | done (`b158885`) |
| 3 | `SStmtExpr` — assign, if/while/for/with/block native CppExprStmt | done (`59e6d5a`) |
| 4 | `SStmtLetPat` — native CppStmt decomposition | done (`60315e8`) |
| 5 | Remove `stmt_via_string_bridge`; audit `CppStmtFragment`; close track | done |

## Step 5 audit (`CppStmtFragment` survivors)

`stmt_via_string_bridge` **removed** from `stmt_cpp.mlc`. `SStmtLet` + `Map.new` native via `CppCall` on typed `mlc::HashMap<K,V>()`.

Remaining intentional `CppStmtFragment` (not stmt string bridge):

| Site | Use |
|------|-----|
| `let_pat_cpp.mlc` | structured binding, array rest slice, ctor holds_alternative prelude |
| `return_body.mlc` | ~~`cpp_stmts_from_string_output` for multi-stmt return bodies~~ — migrated (TRACK_RETURN_BODY closed) |
| `mut_actual_argument.mlc` | mut-ref prelude statements |
| `expr_visitor_cpp.mlc` | unit-if IIFE body fragments |
| `emit_helpers.mlc` | printer/decl fragment helpers |

## Deferred (out of track)

- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
- Phase 4 `MLCC_BOOTSTRAP=1` — separate track (after stmt bridges stable).
- `return_body` string → CppStmt migration.
- `codegen/expr/expr.mlc` templates inside guarded match body strings.
