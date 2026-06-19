# Track: registry.mlc COW audit (perf / mlcc2)

Parent: [../PLAN.md](../PLAN.md) §Phase 1; follows [TRACK_E046_FILTER_PREDICATE.md](TRACK_E046_FILTER_PREDICATE.md) (**closed**)

## Status: **closed** (2026-05-19 Driver STEP=5)

**STEP=5 note:** 10/10 `.push` removed; gate STEP=4 green (**999/0**, parity **2/2**, **diff_exit=0**); mlcc **2.58s** / mlcc2 **1.56s**.

**Baseline (2026-05-19):** build_tests **999/0**; **diff_exit=0**; mlcc **1.88s** / mlcc2 **1.43s**.

**Pattern:** fold+`concat`, `ref mut` single accumulator (`build_registry` already uses mut registry per PLAN.md), avoid `.push` on outer `let mut`/`const` arrays in loops.

### Out of scope

- parser `ref mut` — TRACK_PLAN step 15 (separate branch)
- checker files outside `registry.mlc` (0 `.push` in scope per COW_P2)
- `registry.mlc` structural split — [TRACK_REGISTRY_SPLIT.md](TRACK_REGISTRY_SPLIT.md) closed

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
| 1 | Grep inventory; map fn/line for 10 `.push` sites | **done** |
| 2 | Fix resolution_stack + Result ctor type pushes (5 sites) | **done** |
| 3 | Fix `build_registry` trait/ADT pushes (5 sites) | **done** |
| 4 | Full gate; compare translate times vs baseline | **done** |
| 5 | Close track | **done** |

### Step 1 sub-steps

1. `rg '\.push' compiler/checker/registry.mlc` — confirm 10
2. Classify: `extended_stack` (3), `ok/error_variant` (2), trait loop (2), `private_constructors` (3)
3. Document in inventory; gate skip ok

### Step 2–3 sub-steps

1. Replace with `concat` / fold state / return-updated stack
2. `build_tests` after each batch

### Step 4 sub-steps

1. Full gate per TRACK; note mlcc p1 time vs **1.88s** baseline

### Step 5 sub-steps

1. Mark closed; update TRACK_PLAN

---

## Inventory (STEP=1)

**Total:** 10 `.push` in `registry.mlc` only (checker scope 0 elsewhere per COW_P2).

| Line | Function | Binding | Pattern |
|------|----------|---------|---------|
| 166 | `empty_registry` | `ok_variant_parameter_types` | `const` + `.push` → use `[Shared.new(TUnknown)]` literal |
| 167 | `empty_registry` | `error_variant_parameter_types` | same |
| 249 | `resolve_nominal_type_name_for_fields` | `extended_stack` | `let mut` copy + `.push` → `resolution_stack.concat([type_name])` |
| 271 | `resolve_named_type_with_registry` | `extended_stack` | same |
| 325 | `type_alias_has_cycle_from_name` | `extended_stack` | same |
| 392 | `register_decl_into` | `trait_associated_type_names` | while-loop `.push` → fold/`concat` |
| 404 | `register_decl_into` | `trait_implementations` | single `.push` → `concat([trait_name])` |
| 498 | `register_variant_into` | `private_constructors` | conditional → `concat` if private |
| 505 | `register_variant_into` | `private_constructors` | same (VarTuple) |
| 530 | `register_variant_into` | `private_constructors` | same (VarRecord) |

### STEP=2 change

- `empty_registry`: `[Shared.new(TUnknown)]` literals (was `const` + `.push`)
- `resolve_nominal_type_name_for_fields` / `resolve_named_type_with_registry` / `type_alias_has_cycle_from_name`: `resolution_stack.concat([name])` (was `let mut` + `.push`)

### STEP=3 change

- `trait_associated_type_names`: `methods.fold` + `accumulate_trait_associated_type_name` (concat)
- `trait_implementations`: existing list `.concat([trait_name])`
- `private_constructors`: `[string]` → `Map<string, bool>` + `.set` (ref mut cannot rebind nested array)
- `DeclTrait` nested decl walk: `if !is_assoc_type` (avoid void/unit match codegen)
