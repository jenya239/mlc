# Track: or-patterns with binding + guard exhaustiveness

Parent: [../PLAN.md](../PLAN.md), [../MLC.md](../MLC.md) §B2, §E2. Source:
[../LANGUAGE_AUDIT_2026_07.md](../LANGUAGE_AUDIT_2026_07.md) #6.

## Status: **open** (STEP=1–2 code done; pending Tier B / self-host verify-gate)

Два независимых, но связанных пробела в `match`, оба уже частично
специфицированы (`MLC.md` §B2, §E2). Делать двумя шагами.

## STEP=1 — or-patterns с биндингом — **done** (2026-07-09)

**Проблема (`MLC.md` §B2):** `A | B => expr` работал для unit и для
совпадающих имён биндинга (codegen `expand_or_arms` уже ок: `Circle(3)`→9,
`Square(4)`→16). Разные имена (`Circle(r) | Square(side)`) молча принимались.

**Fix:** `compiler/checker/names.mlc` —
`check_pattern_or_binding_consistency` / `check_or_pattern_alternatives_same_bindings`
в `visit_match`; E083 в `diagnostic_codes.mlc`; тесты в `test_checker.mlc`.
Совпадающие имена — 0 ошибок; mismatch — `error[E083]: all alternatives of an
or-pattern must bind the same names`.

## STEP=2 — guards + корректная exhaustiveness — **done** (2026-07-09)

Guards уже парсились (E032). ADT exhaustiveness в mlcc не было.

**Fix:**
- `registry.mlc`: `algebraic_decl_variant_names` + `algebraic_decl_variant_names_for`
  (заполняется в `register_decl_type_into_registry`; builtin Result **не** в индексе —
  иначе ломаются неполные `match Ok` в compiler).
- `infer_match.mlc`: `match_exhaustiveness_diagnostic` — ветка с `has_guard` **не**
  покрывает; `_` / `PatternIdent` — catch-all; E084
  `non-exhaustive match: missing patterns for …`.
- Тесты: guard-only Active → E084+Inactive; Active без Inactive → E084; полный
  match / guard+`_` → 0 ошибок.

## STEP=verify-gate — Tier B + self-host (pending)

```bash
bundle exec rake test_compiler_mlc
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2
scripts/regression_gate.sh
```

После зелёного gate — Status: **closed**.
