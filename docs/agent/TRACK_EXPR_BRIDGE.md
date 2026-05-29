# Track: Expr fragment bridge elimination (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_PARSER_PARITY.md](TRACK_PARSER_PARITY.md) (**closed**, `d5442d6`), [TRACK_CPPGEN.md](TRACK_CPPGEN.md) (**closed**, `005c65b`)

## Status: **closed**

**Goal:** remove remaining `expr_fragment_codegen` string bridges in `expr_visitor_cpp`; emit native `CppExpr` for call/method/if/block/array/lambda/record_update arms.

## Verify gate (every step)

```
bundle exec rake test_compiler_mlc
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | `expr_visitor_cpp` — call + method as native CppExpr | done (`d9505b9`) |
| 2 | `expr_visitor_cpp` — if + block as native CppExpr | done (`0ba605c`) |
| 3 | `expr_visitor_cpp` — array + lambda as native CppExpr | done (`e66e547`) |
| 4 | `expr_visitor_cpp` — record_update as native CppExpr | done (`4274d91`) |
| 5 | Delete `expr_fragment_codegen.mlc`; audit; close track | done (`91ece96`) |

## Audit (post step 5)

- `expr_fragment_codegen.mlc` **deleted** — no importers.
- `expr_visitor_cpp.mlc`: all expr arms native CppExpr except string-bridge helpers for match (`match_codegen.gen_match` / `gen_arm` when guards or handler fragments).
- `codegen/expr/expr.mlc` string templates remain for stmt/decl/match/module (TRACK_CPPGEN deferred scope).

## Deferred (out of track)

- Match guard / arm string bridge → native CppExpr (future track if needed).
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
- `lib/mlc/` Ruby backend parity — not in scope.
- Phase 4 `MLCC_BOOTSTRAP=1` — separate track.
