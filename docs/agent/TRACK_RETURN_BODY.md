# Track: Return-body string bridge elimination (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_STMT_BRIDGE.md](TRACK_STMT_BRIDGE.md) (**closed**, `7084227`)

## Status: **active** (step 4 pending)

**Goal:** eliminate `cpp_stmts_from_string_output(gen_return_body(...))` in `codegen/stmt/return_body.mlc` — native `[Shared<CppStmt>]` for function/return bodies used by `decl_cpp.mlc`.

**Constraints:**
- One concern per sub-step; no `compiler/` + `lib/mlc/` in one commit.
- Self-host gate when `compiler/**` touched: `build_bin.sh` + diff empty.

## Verify gate (every step)

```
bundle exec rake test_compiler_mlc   # 778 pass (baseline post step 3)
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | Leaf returns — unit, direct expr, `?` as native CppReturn/CppBlock | done |
| 2 | `SExprBlock` return body — stmts via `gen_stmts_cpp` + trailing return | done |
| 3 | Return if/else-if chains — native CppIf (no string fragment) | done |
| 4 | `gen_return_body_cpp` fully native; `decl_cpp` wired; drop string round-trip | pending |
| 5 | Audit `cpp_stmts_from_string_output` callers; close track | pending |

## Survivors (post step 3)

- `SExprMatch` trailing in block still via string bridge (recursive gen_return_body_cpp).
- `decl.mlc`: string `gen_fn_body` (parallel path; step 4).

## Next step (Driver)

**STEP=4** — wire `decl_cpp`; drop string round-trip where fully native.

## Deferred (out of track)

- `let_pat_cpp.mlc` internal `CppStmtFragment` (structured binding, ctor prelude).
- `mut_actual_argument.mlc` mut-ref prelude fragments.
- Phase 4 `MLCC_BOOTSTRAP=1` — separate track.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
