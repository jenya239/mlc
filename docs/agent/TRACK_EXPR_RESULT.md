# Track: ExprResult → ParseResult (frontend)

Parent: [../PLAN.md](../PLAN.md) §Phase 2.5 п.5; follows [TRACK_PARSE_RESULT.md](TRACK_PARSE_RESULT.md) (**closed**)

## Status: **closed** (2026-05-19, STEP=4)

**Deferred from PARSE_RESULT:** `ExprResult { expression, parser }` — last legacy single-value parser result in `predicates.mlc`.

**Final gate (2026-05-19):** build_tests **1005/0**; parity **2/2**; **diff_exit=0**; mlcc p1 **2.08s** / mlcc2 p2 **1.70s**.

**STEP=1 verify (2026-05-19):** `build.sh` ok; `build_tests` **1005/0**.

**STEP=3 verify (2026-05-19):** `build_tests` **1005/0** (parser smoke + fuzz); self-host **diff_exit=0**; mlcc p1 **2.32s** / mlcc2 p2 **1.76s**.

### Out of scope

- `TypeParamsResult`, `ParseProgramResult` — unchanged
- parser `ref mut` — TRACK_PLAN step 15
- `lib/mlc/` Ruby parity

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
| 1 | `ExprResult` alias + `expression_parse_result`; migrate `exprs.mlc` | **done** |
| 2 | Migrate `decls.mlc`, `comma_separated.mlc`, `test_parser.mlc` | **done** (unblocked build during step 1) |
| 3 | Parser smoke tests; self-host `build_bin` | **done** |
| 4 | Full gate; close track | **done** |

### Notes

- Use `expression_parse_result` (explicit return type) — same codegen pattern as `type_parse_result`.
- `.expression` on **Expr** AST nodes unchanged; only **ExprResult** field becomes `.value`.
- `parse_expr(...).expression` in tests → `.value` after alias.
