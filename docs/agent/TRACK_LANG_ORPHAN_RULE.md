# Track: orphan rule for `extend Type : Trait`

Parent: [../PLAN.md](../PLAN.md). Source:
[../LANGUAGE_AUDIT_2026_07.md](../LANGUAGE_AUDIT_2026_07.md) #7.

## Status: **closed** (2026-07-09) — STEP=5 verify-gate green

`extend T : Trait` only in the module defining **T** or **Trait** (E086).
Bare `extend T` unchanged.

## Steps

| Step | Status | Notes |
|------|--------|-------|
| 1 | **done** | Inventory: Span.file / LoadItem.path; chose defining-path maps |
| 2 | **done** | `type_defining_path` / `trait_defining_path` from Decl span.file |
| 3 | **done** | E086 + `orphan_lint.mlc` in `gather_program_check` |
| 4 | **done** | Self-check 6/0 orphans; From\<T\> tests; import smoke |
| 5 | **done** | build.sh; self-host DIFF_EXIT:0; regression 20/0; build_tests 1471/0 |

## Implementation

| Piece | Location |
|-------|----------|
| Defining paths | `checker/registry.mlc` |
| Lint | `checker/orphan_lint.mlc` → `orphan_impl_diagnostics` |
| Wire | `checker/check/check.mlc` `gather_program_check` |
| Code | E086 in `diagnostic_codes.mlc` (catalog 87) |

Policy: empty extend path skips; both defining paths empty skips; builtin + local trait ok; `trait_base_name` for `From<T>`.

## Verify gate (STEP=5, 2026-07-09)

```
DIFF_EXIT:0
regression_gate 20/0
build_tests TESTS_EXIT:0 (1471 passed)
```
