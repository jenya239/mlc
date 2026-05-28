# Track: Diagnostics — remaining bare codes (Phase 1 §1 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 1 §1; previous: [TRACK_COVERAGE.md](TRACK_COVERAGE.md) (**closed**, `9bddbb3`), [TRACK_DIAGNOSTICS.md](TRACK_DIAGNOSTICS.md) (**closed**, `f6863fe`)

## Status: **closed** (`d055c49`)

**Goal:** replace remaining bare `diagnostic_error(` in checker with `diagnostic_error_with_code`; extend catalog from E035; negative tests assert exact `error[Exxx]:` format where touched.

**Result:** catalog E001–E076 (76 codes); no bare `diagnostic_error(` in `compiler/checker/**`.

---

| Step | Item | Status |
|------|------|--------|
| 1 | `check.mlc` — default-param + record-field defaults → E036+; catalog smoke | done (`f74e0fa`) |
| 2 | `infer_array_method.mlc` — array HOF diagnostics → codes | done (`7eb356e`) |
| 3 | `infer_result_option_method.mlc` — Result/Option method diagnostics → codes | done (`2fd25e9`) |
| 4 | `infer.mlc` + `infer_question_expression.mlc` — spread, let-else, private ctor | done (`678895a`) |
| 5 | `derive_validation.mlc` + transform expand modules + audit; catalog count test | done (`d055c49`) |

## Verify gate (final)

```
bundle exec rake test_compiler_mlc   # 706 pass
compiler/build.sh
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```
