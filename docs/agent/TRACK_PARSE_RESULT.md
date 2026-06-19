# Track: ParseResult migration (frontend)

Parent: [../PLAN.md](../PLAN.md) §Phase 2.5 п.5; follows [TRACK_GENERICS_RECORD.md](TRACK_GENERICS_RECORD.md) (**closed**)

## Status: **closed** (2026-05-19, STEP=5)

**Delivered:** frontend parser result types migrated to `ParseResult<Value>` / typed helpers; `.value` at call sites; `statements_result_to_block_expr` (no `extend` on alias).

**Gate (STEP=5):** build_tests **1005/0**; parity **2/2**; **diff_exit=0**; mlcc **1.91s** / mlcc2 **1.77s**.

**Deferred (out of scope):** `ExprResult` (`expression` field); `TypeParamsResult`; `ParseProgramResult`; parser `ref mut`.

### Out of scope (unchanged)

- `TypeParamsResult` multi-field redesign (params + bounds) — keep or minimal alias only
- `ParseProgramResult` (no `parser` field)
- parser `ref mut` — TRACK_PLAN step 15

## Verify gate

```
compiler/tests/build_tests.sh
bash compiler/tests/run_mlcc2_checker_parity.sh
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 --check-only compiler/main.mlc
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | Inventory + migrate single-value types (`TypeResult`, `PatternResult`, `StmtResult`, `VariantResult`) | **done** |
| 2 | Migrate list-value types (`ExprsResult`, `StmtsResult`, `TypesResult`, …) | **done** |
| 3 | Migrate `DeclResult`, `TraitBodyResult`, `WhereClauseParseResult` | **done** |
| 4 | Parser tests; self-host `build_bin` | **done** |
| 5 | Full gate; close track | **done** |

### Notes

- RHS generic record: `type X = { value: T, parser: Parser }` — not `X<T> { … }` on RHS (Ruby bootstrap).
- Empty `[]` in `parse_result`: use typed helper or `bounds_parse_result` pattern.
- Generic `parse_result` in match arms: codegen mis-infers `Shared<T>` — use typed helper with explicit return type.
- No `extend` on `ParseResult` alias — Ruby bootstrap resolves methods on `ParseResult`, not alias (`statements_result_to_block_expr`).
- Avoid nested `if … then do … end else` in checker paths (mlcc codegen).
