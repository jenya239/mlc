# Track: orphan rule for `extend Type : Trait`

Parent: [../PLAN.md](../PLAN.md). Source:
[../LANGUAGE_AUDIT_2026_07.md](../LANGUAGE_AUDIT_2026_07.md) #7.

## Status: **open** (STEP=3 done 2026-07-09 — E086 orphan diagnostic)

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
| 4 | pending | Self-check compiler/ trait-extends under E086; any debt notes |
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
2. **`compiler/out/mlcc` was stale** after STEP=2–3 (last binary from RESULT close); rebuilt this audit.
3. Unit orphan tests use **concat programs** with distinct `source_path`, not real `import` merge. Real driver paths go through `resolve_dotdot` — STEP=4 must self-check `compiler/main.mlc` and a two-file import repro.
4. No unit test for generic trait name on extend (`From<T>` → `trait_base_name`) under E086.
5. Policy: empty extend path skips E086; both defining paths empty skips; builtin+local trait allowed — covered by tests.

## Verify gate

```bash
scripts/dev_gate_fast.sh
# before close:
compiler/build.sh
# self-host p1/mlcc2/p2 + regression_gate + build_tests.sh
```
