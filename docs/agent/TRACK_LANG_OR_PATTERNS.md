# Track: or-patterns with binding + guard exhaustiveness

Parent: [../PLAN.md](../PLAN.md), [../MLC.md](../MLC.md) §B2, §E2. Source:
[../LANGUAGE_AUDIT_2026_07.md](../LANGUAGE_AUDIT_2026_07.md) #6.

## Status: **closed** (2026-07-09)

## STEP=1 — or-patterns с биндингом — **done**

E083: or-pattern alternatives must bind the same names (`names.mlc`).

## STEP=2 — guards + exhaustiveness — **done**

E084: ADT match exhaustiveness; guarded arms do not cover
(`infer_match.mlc` + `registry.mlc` `algebraic_decl_variant_names`).

## STEP=verify-gate — **done** (2026-07-09)

- `compiler/build.sh` → fresh `mlcc`
- self-host: p1 → mlcc2 → p2; `diff -rq` empty (exclude obj)
- `dev_gate_fast` **1432/0**
- `scripts/regression_gate.sh` **20/0**
- Fix during gate: `join_variant_names` → `fold` (if-assign codegen broke g++)
