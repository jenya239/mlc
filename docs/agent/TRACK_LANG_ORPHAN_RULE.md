# Track: orphan rule for `extend Type : Trait`

Parent: [../PLAN.md](../PLAN.md). Source:
[../LANGUAGE_AUDIT_2026_07.md](../LANGUAGE_AUDIT_2026_07.md) #7.

## Status: **open** (STEP=4 done 2026-07-09 — self-check + From\<T\> tests)

**Проблема:** сейчас `extend T : Trait` разрешён в любом модуле, без
проверки когерентности.

## Правило

`extend T : Trait` разрешён только в модуле, где объявлен **`T`** ИЛИ
**`Trait`**. Bare `extend T { … }` (без `: Trait`) — вне правила.

## Steps

| Step | Status | Notes |
|------|--------|-------|
| 1 | **done** | Inventory: no module maps in TypeRegistry; ownership via Span.file / LoadItem.path |
| 2 | **done** | `type_defining_path` / `trait_defining_path` + accessors; record from DeclType/Alias/Trait span.file |
| 3 | **done** | E086 + `orphan_lint.mlc`; wired in `gather_program_check`; tests same-module / orphan / builtin / bare / empty path |
| 4 | **done** | Self-check: 6 trait-extends, 0 orphans; main/e2e check-only 0; From\<T\> unit + import smoke |
| 5 | pending | verify-gate + close |

## Inventory (STEP=1, `compiler/` only)

### TypeRegistry — defining paths **added** (STEP=2)

`type_defining_path` / `trait_defining_path` on `TypeRegistry`; filled from
`DeclType` / `DeclTypeAlias` / `DeclTrait` `name_span.file` (empty path not
stored). Accessors: `defining_path_for_type` / `defining_path_for_trait`.

`build_registry(program)` still walks a **flattened** `Program` — ownership
comes from AST spans, not `LoadItem.path`.

### Where module path *also* exists (unused by orphan lint today)

| Layer | Fact |
|-------|------|
| `LoadItem.path` | Per-module after `merge_program` |
| `ModularCompileInput.load_items` | Driver → pipeline |
| `Span.file` | **used** for defining paths + extend site |
| `check` / `gather_program_check` | Flat program; calls `orphan_impl_diagnostics` |

### Critic notes (2026-07-09)

1. TRACK inventory text above was stale (“no module ownership”) after STEP=2 — fixed.
2. **`compiler/out/mlcc` was stale** after STEP=2–3; rebuilt in critique-audit.
3. Unit orphan tests use **concat programs** with distinct `source_path`; import merge covered by mlcc smoke in STEP=4.
4. ~~No unit test for `From<T>`~~ — added STEP=4 (`same-module From` + orphan `trait_base_name`).
5. Policy: empty extend path skips E086; both defining paths empty skips; builtin+local trait allowed — covered by tests.

### Self-check (STEP=4, 2026-07-09)

Trait extends in `compiler/` (exclude `out/`): **6**. All allowed (own T or local Trait for builtin i32).  
`mlcc --check-only`: `compiler/main.mlc`, `trait_e2e`, `trait_as_param` → exit 0.  
Import orphan smoke (`From` / `Display`) → E086 exit 1. **Debt: none.**

## Verify gate

```bash
scripts/dev_gate_fast.sh
# before close:
compiler/build.sh
# self-host p1/mlcc2/p2 + regression_gate + build_tests.sh
```
