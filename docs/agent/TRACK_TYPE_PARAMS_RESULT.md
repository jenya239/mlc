# Track: TypeParamsResult → ParseResult (frontend)

Parent: [../PLAN.md](../PLAN.md) §Phase 2.5 п.5; follows [TRACK_EXPR_RESULT.md](TRACK_EXPR_RESULT.md) (**closed**)

## Status: **closed** (2026-05-19, STEP=4)

**Deferred from PARSE_RESULT / EXPR_RESULT:** `TypeParamsResult { params, bounds, parser }` — last multi-field legacy parser result in `predicates.mlc`.

**Final gate (2026-05-19):** build_tests **1005/0**; parity **2/2**; **diff_exit=0**; mlcc p1 **7.59s** / mlcc2 p2 **5.82s**.

**STEP=1 verify (2026-05-19):** `build.sh` ok; `build_tests` **1005/0**.

**STEP=3 verify (2026-05-19):** `build_tests` **1005/0**; self-host **diff_exit=0**; mlcc p1 **1.95s** / mlcc2 p2 **1.81s**.

### Out of scope

- `ParseProgramResult` (no `parser` field) — separate track if needed
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
| 1 | `TypeParamsValue` + `TypeParamsResult` alias + `type_params_parse_result`; `parse_type_params_opt` in `decls.mlc` | **done** |
| 2 | Migrate `decls.mlc` call sites (`.params`/`.bounds` → `.value`) | **done** (with step 1) |
| 3 | `build_tests`; self-host `build_bin` | **done** |
| 4 | Full gate; close track | **done** |

### Notes

- `type TypeParamsValue = TypeParamsValue { params: [string], bounds: [[string]] }` then `TypeParamsResult = ParseResult<TypeParamsValue>`.
- Use `type_params_parse_result(params, bounds, parser)` — explicit return type (same pattern as `expression_parse_result`).
- Empty params: `type_params_parse_result([], [], parser)` not bare record literal on alias.
