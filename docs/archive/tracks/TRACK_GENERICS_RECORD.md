# Track: Generic record types (ParseResult\<T\>)

Parent: [../PLAN.md](../PLAN.md) §Phase 2.5 п.5; follows [TRACK_TYPE_ALIASES.md](TRACK_TYPE_ALIASES.md) (**closed**, STEP=6 deferred)

## Status: **closed** (2026-06-19)

**Gate:** build_tests **1003/0**; parity **2/2**; **diff_exit=0**; mlcc **1.95s** / mlcc2 **1.79s** (p2).

**Delivered:** generic alias type params + `TyGeneric` expansion; `ParseResult<Value>` + partial frontend migration (`BoundsResult`, `ParamResult`, `NamesResult`, `DeriveResult`, `MatchArmParseOutcome`).

**Deferred:** remaining 17 predicates result types; full `ParseResult<T>` unify (TYPE_ALIASES STEP=6 scope absorbed).

**Priority:** stability (correctness / self-host) — not performance.

### Out of scope

- Full generic ADT / trait generics beyond minimal record-alias path
- parser `ref mut` — TRACK_PLAN step 15 (separate branch)
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
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

Record mlcc/mlcc2 translate times each gate step.

---

| Step | Item | Status |
|------|------|--------|
| 1 | Inventory ParseResult-like structs; document substitution/codegen blocker | **done** |
| 2 | Fix checker generic record substitution (minimal `apply_type_parameter_substitution` path) | **done** |
| 3 | `ParseResult<T>` type alias + one `predicates.mlc` consumer | **done** |
| 4 | Refactor remaining frontend duplicates; tests | **done** |
| 5 | Full gate; close track | **done** |

### Step 1 sub-steps

1. `rg 'value:.*parser: Parser' compiler/frontend` — list sites
2. Re-read TYPE_ALIASES STEP=6 revert notes + `semantic_type_structure.mlc` substitution
3. Minimal repro for self-host failure; gate skip ok (docs)

### Step 2–4 sub-steps

1. One concern per commit batch; `build_tests` after checker fix
2. Self-host `build_bin` before broad predicates refactor

### Step 5 sub-steps

1. Full gate per TRACK; update TRACK_PLAN

---

## Inventory (STEP=1)

### ParseResult-like types (**24** total)

**`predicates.mlc` exports (22)** — all `{ …, parser: Parser }`:

| Type | Value field |
|------|-------------|
| `ExprResult` | `expression` |
| `TypeResult` | `type_expr` |
| `PatternResult` | `pattern` |
| `StmtResult` | `statement` |
| `StmtsResult` | `stmts` |
| `ExprsResult` | `exprs` |
| `ArmsResult` | `arms` |
| `VariantResult` | `variant` |
| `VariantsResult` | `variants` |
| `ParamsResult` | `params` |
| `DeclResult` | `declaration` |
| `FieldValsResult` | `field_vals` |
| `RecordLitPartsResult` | `lit_parts` |
| `PatternsResult` | `patterns` |
| `FieldDefsResult` | `field_defs` |
| `TypesResult` | `types` |
| `ParamResult` | `param` |
| `NamesResult` | `exprs` (names) |
| `TypeParamsResult` | `params` + `bounds` |
| `BoundsResult` | `bounds` |
| `WhereClauseParseResult` | `where_bounds` |
| `TraitBodyResult` | `methods` |

**Local (2):** `decls.mlc` `DeriveResult { traits, parser }`; `exprs.mlc` `MatchArmParseOutcome { arm, parser }`.

**Exception:** `ParseProgramResult { program, errors }` — no `parser` field.

### What already works

- Generic **algebraic record** `type Box<T> = { value: T }` — checker + field access (`test_checker.mlc` L284, L888–894).
- Parser: `type Name<T> = { … }` after `=` → **DeclType** (`decls.mlc` L635–640), not `DeclTypeAlias`.
- `substitute_type` in `semantic_type_structure.mlc` L261 — substitutes `TNamed` params in `Type` trees (ADT match arms).

### Blockers (TYPE_ALIASES STEP=6 revert)

| Gap | Location |
|-----|----------|
| `DeclTypeAlias` type params **ignored** at register | `registry.mlc` L458–459 — only `type_alias_annotations.set(name, expr)` |
| Alias resolve has no `TyGeneric(alias, args)` expansion | `resolve_named_type_with_registry` L264–273 |
| `type_alias_named_target` only `TyNamed` | `registry.mlc` L232–236 — record/alias body not a single name |
| `field_type_from_object` no param substitution on fields | `registry.mlc` L539–550 — `TGeneric` name resolve only |
| TYPE_ALIASES STEP=6 revert | `substitute_type` / `field_type_from_object` extension → self-host `build_bin` fail (`Value` leak, match codegen) |

### STEP=2 target (minimal)

1. Persist alias type parameter names in registry (parallel to `algebraic_decl_type_parameter_names`).
2. On `TyGeneric(alias, args)` in `type_from_annotation_with_registry`, expand alias body with arg→param `substitute_type` on **TypeExpr** (or lower-then-substitute).
3. `field_type_from_object`: apply substitution when scrutinee is `TGeneric(record, args)`.

### Refactor note (STEP=3+)

Unified `type ParseResult<Value> = { value: Value, parser: Parser }` as **DeclType** implies renaming payload fields (`expression`→`value`, etc.) across parser — or keep per-type records until alias path works.
