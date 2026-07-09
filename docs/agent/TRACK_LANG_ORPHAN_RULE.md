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

### TypeRegistry — **no module ownership today**

`TypeRegistry` / `AdtIndex` / `FunctionIndex` / `RecordIndex` store names only
(`trait_impls: Map<string, [string]>`, algebraic/record maps, etc.).
`register_decl_extend_into_registry` / `register_decl_type_into_registry` /
`register_decl_trait_into_registry` take **no path**.

`build_registry(program)` walks a **flattened** `Program` after merge — path
association from `LoadItem` is discarded at registry build.

### Where module path *does* exist

| Layer | Fact |
|-------|------|
| `LoadItem.path` | Per-module after `merge_program` (`ir/load_item.mlc`) |
| `ModularCompileInput.load_items` | Driver → pipeline has items |
| `Span.file` | Set from `Parser.source_path` on DeclType / DeclTrait / DeclExtend |
| `check` / `gather_program_check` | Sees flat `full_program` only — no LoadItems |

**Chosen approach for STEP=2:** record defining path from `decl_span(…).file`
when registering `DeclType` / `DeclTypeAlias` / `DeclTrait` (and exported
wrappers). Orphan check compares `DeclExtend` span.file to
`type_defining_path[T]` and `trait_defining_path[Trait]` (use
`trait_base_name`). Prefer this over threading LoadItems into check — spans
already on AST; unit tests that parse with `source_path=""` need explicit
paths in orphan tests.

**Not chosen:** scanning LoadItems only in driver — checker unit tests call
`check`/`program_diagnostics` without merge.

### Self-check snapshot (`compiler/`, exclude `out/`)

Trait extends (`extend T : Trait`): **6** total.

| Site | T | Trait | Same-file T? | Allowed under rule? |
|------|---|-------|--------------|---------------------|
| `names.mlc` | NamesPass | ExprVisitor | yes | yes (owns T) |
| `check_mutations.mlc` | MutationsPass | ExprVisitor | yes | yes |
| `expr_visitor_cpp.mlc` | CodegenPass | ExprVisitor | yes | yes |
| `test_expr_visitor.mlc` | TagExprVisitor | ExprVisitor | yes (local type) | yes |
| `tests/e2e/trait_*.mlc` | i32 | Display | Display local; i32 builtin | yes if builtins treated as “any module may extend with local Trait” |

`ExprVisitor` defined in `expr_visitor.mlc` — cross-file trait is fine when T
is local. **0** production orphan candidates found among current trait extends.

Bare `extend T` (methods only): many (Shared, Parser, TypeRegistry, …) — **out
of scope** for orphan rule.

### Policy notes (fix in STEP=2/3)

1. **Builtins** (`i32`, `string`, …): no defining path → orphan allowed iff
   Trait is defined in the extend module (matches e2e Display).
2. **Generic trait name** on extend: compare with `trait_base_name` (same as
   From/`trait_impls`).
3. **Empty `source_path`**: skip orphan emit or treat as same-module — decide
   in STEP=3 tests (prefer: no diagnostic when either path empty, so existing
   string-parse tests stay green).

## Repro (target after STEP=3)

```mlc
// a.mlc
export type Point = { x: i32, y: i32 }
export trait Display { fn to_string(self: Display) -> string }

// b.mlc
import { Point, Display } from "./a"
extend Point : Display { fn to_string(self: Point) -> string = "p" }
```

Expect: orphan error. Control: `extend` in `a.mlc` — ok.

## Verify gate

```bash
scripts/dev_gate_fast.sh
# before close:
compiler/build.sh
# self-host p1/mlcc2/p2 + regression_gate + build_tests.sh
```
