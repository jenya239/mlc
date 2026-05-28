# Track: SemanticIR decl spans (Phase 1 §1 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 1 §1; previous: [TRACK_BUILD.md](TRACK_BUILD.md) (**closed**, `1d6f4c5`)

## Status: **closed** (`e826f1a`)

**Closed:** step 5 audit — remaining span-less SemanticIR nodes documented; no code changes.

**Goal:** `SDecl` variants in `semantic_ir.mlc` carry source spans; transform propagates; diagnostics use decl spans where available.

**Constraints:**
- One concern per sub-step; no `compiler/` + `lib/mlc/` in one commit.
- Self-host gate when `compiler/**` touched: `build_bin.sh` + diff empty.
- One decl variant group per step where possible.

## Verify gate (every step)

```
bundle exec rake test_compiler_mlc   # 755 pass (baseline post step 4)
compiler/build.sh                    # when compiler/** touched
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | `SDeclFn` + `Span`; propagate in `transform_decl.mlc`; test | done (`7a0a3cc`) |
| 2 | `SDeclType` span from `DeclType.name_span` | done (`3e214fc`) |
| 3 | `SDeclTrait` / `SDeclExtend` spans | done (`326b173`) |
| 4 | Checker diagnostics on decl paths use `SDecl` span helpers | done (`860bafc`) |
| 5 | Audit remaining span-less IR; close track | done (`e826f1a`) |

## Step 5 audit (remaining span-less SemanticIR)

Reviewed `semantic_ir.mlc` and `sdecl_span` — **no fixable paths**; no code changes.

| Node | Span? | Rationale |
|------|-------|-----------|
| `SExpr` / `SStmt` (all variants) | yes | pre-track; `sexpr_span` / stmt span on each variant |
| `SDeclFn` / `SDeclType` / `SDeclTrait` / `SDeclExtend` / `SDeclAssocBind` | yes | steps 1–3; `sdecl_span` |
| `SDeclImport` | no | module path + symbol list; no single decl name token; rarely diagnostic site |
| `SDeclExported` | wrapper | span on inner via `sdecl_span(sdecl_inner(d))` |
| `SMatchArm` | no | parent `SExprMatch` carries match span; pat/body have own spans from transform |
| `SProgram` / `SLoadItem` / `SNamespaceImportAlias` | N/A | containers, not diagnostic nodes |

AST mirror: `decl_span` covers all user-facing `Decl` variants (step 4). `DeclImport` / `DeclFn` without name span use body or `span_unknown` — same as pre-track.

Test-only `span_unknown` in `compiler/tests/**` — out of scope.

## Context

- `SExpr` / `SStmt` already carry `Span` (`semantic_ir.mlc`).
- `SDeclFn`, `SDeclType`, `SDeclTrait`, `SDeclExtend` carry spans; `SDeclAssocBind` has span.
- `DeclType` / `DeclTrait` / `DeclExtend` have `name_span` in AST.

## Deferred (not in this track)

- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
- `lib/mlc/` parity — not in scope.
- Record default expression parity (E4) — separate track if needed.
- `SDeclImport` span — deferred (low diagnostic value).

## Next step

Track **closed**. Planner: plan-refresh.
