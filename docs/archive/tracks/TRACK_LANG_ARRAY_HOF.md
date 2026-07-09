# Track: Array HOF (`map`/`filter`/`fold`/...)

Parent: [../PLAN.md](../PLAN.md), [../MLC.md](../MLC.md) §A1. Source:
[../LANGUAGE_AUDIT_2026_07.md](../LANGUAGE_AUDIT_2026_07.md) #1.

## Status: **closed** (2026-07-09) — language feature already shipped

Аудит/`MLC.md` описывали A1 как будущую работу. Инвентаризация STEP=1
показала: MVP уже в runtime + checker + codegen (Ruby и mlcc). Repro из
трека exits **14** на обоих.

## STEP=1 — inventory Ruby vs mlcc — **done** (2026-07-09)

| Layer | Finding |
|-------|---------|
| Runtime | `runtime/include/mlc/core/array.hpp`: `map`, `filter`, `fold`, `any`, `all`, `find`, `find_index`, `sort_by`, `take`, `drop`, `flat_map`, `zip`, `enumerate`, `flat`, `group_by`, `sum`, `join` |
| mlcc checker | `array_method_types.mlc` `is_array_hof_method` + `array_hof_call_result_type`; `infer_array_method.mlc`; tests in `test_checker.mlc` (~818+) |
| mlcc codegen | method call → `xs.map([=](…){…})` on `mlc::Array` (no separate std::transform wrapper) |
| Ruby | `call_rule.rb` lowers array HOF (member call or `mlc::collections::*`); same runtime |
| Repro | `[1,2,3,4].map/filter/fold` → exit **14** (mlcc + Ruby) |
| All 17 MVP names | `--check-only` OK on mlcc |

**Not a gap:** `builtin_method_return_type` in `semantic_type_structure.mlc`
does **not** list HOF — routing is via `infer_array_method` /
`hof_method_spec`, not that helper. Track file path was stale.

**Remaining (out of scope for this track):** ~704 `while` in `compiler/**/*.mlc`
(excl. out/tests) vs ~247 `.map/.filter/.fold` calls — adoption/cleanup, not
missing language surface. Open a separate TRACK if prioritizing while→HOF
rewrites in self-hosted sources.

## Verify (STEP=1)

```
mlcc --check-only + build_bin repro → exit 14
Ruby bin/mlc same repro → exit 14
```
