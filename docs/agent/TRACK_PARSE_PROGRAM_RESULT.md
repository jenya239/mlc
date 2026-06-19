# Track: ParseProgramResult cleanup (frontend)

Parent: [../PLAN.md](../PLAN.md) §Phase 2.5 п.5; follows [TRACK_TYPE_PARAMS_RESULT.md](TRACK_TYPE_PARAMS_RESULT.md) (**closed**, `bf6c46e8`)

## Status: **closed** (STEP=4 done, 2026-06-19)

**Deferred from PARSE_RESULT / TYPE_PARAMS:** `ParseProgramResult { program, errors }` — last non-`ParseResult` parser result alias in `predicates.mlc` (no `parser` field; entry-point).

**Baseline (2026-06-19):** build_tests **1005/0**; parity **2/2**; **diff_exit=0**; committed `bf6c46e8`.

**Priority:** stability (finish frontend parser result types).

### Out of scope

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
| 1 | `ProgramParseValue` + `program_parse_result`; `parse_program_with_errors` in `decls.mlc` | done |
| 2 | Migrate `main.mlc` / `test_parser.mlc` if needed | done |
| 3 | `build_tests`; self-host `build_bin` | done |
| 4 | Full gate; close track | done |

### Notes

- Record alias `ParseProgramResult = ProgramParseValue` duplicates C++ struct — use `ProgramParseValue` only; call sites keep `.program` / `.errors`.
- Use `program_parse_result(program, errors)` — explicit return type.
- Field names `.program` / `.errors` unchanged at call sites.
