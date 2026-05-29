# Track: Expr fragment bridge elimination (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_PARSER_PARITY.md](TRACK_PARSER_PARITY.md) (**closed**, `d5442d6`), [TRACK_CPPGEN.md](TRACK_CPPGEN.md) (**closed**, `005c65b`)

## Status: **active** (step 2 pending)

**Goal:** remove remaining `expr_fragment_codegen` string bridges in `expr_visitor_cpp`; emit native `CppExpr` for call/method/if/block/array/lambda/record_update arms.

**Constraints:**
- One concern per sub-step; no `compiler/` + `lib/mlc/` in one commit.
- Self-host gate when `compiler/**` touched: `build_bin.sh` + diff empty.
- One expr arm batch per step (mirror TRACK_CPPGEN steps 1–3).

## Verify gate (every step)

```
bundle exec rake test_compiler_mlc   # 756 pass (baseline post PARSER_PARITY)
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | `expr_visitor_cpp` — call + method as native CppExpr (no fragment bridge) | done (`d9505b9`) |
| 2 | `expr_visitor_cpp` — if + block as native CppExpr | pending |
| 3 | `expr_visitor_cpp` — array + lambda as native CppExpr | pending |
| 4 | `expr_visitor_cpp` — record_update as native CppExpr | pending |
| 5 | Shrink/delete `expr_fragment_codegen.mlc`; audit; close track | pending |

## Survivors (post step 1)

`expr_visitor_cpp.mlc` still routes 6 arms through `expr_fragment_codegen.*_via_visitor`: if, block, array, lambda, record_update (+ match guards).

## Next step (Driver)

**STEP=2** — if + block native CppExpr in `expr_visitor_cpp.mlc`.

## Survivors (baseline)

- File: `compiler/expr_visitor_cpp.mlc` (+ `test_codegen.mlc` if needed).
- Replace fragment bridge for call + method with direct `CppCall` / `CppMember` construction (mirror CPPGEN step 1 field/index).
- No `stmt_cpp` / `decl_cpp` changes yet.

## Deferred (not in this track)

- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15, separate branch.
- `lib/mlc/` Ruby backend parity — not in scope.
- Record default expression parity (E4) — separate track if needed.
- Phase 4 `MLCC_BOOTSTRAP=1` — separate track after Phase 2 bridges.

## Next step (Driver)

**STEP=1** — call + method native CppExpr in `expr_visitor_cpp.mlc`.
