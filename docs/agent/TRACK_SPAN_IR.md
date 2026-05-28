# Track: SemanticIR decl spans (Phase 1 §1 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 1 §1; previous: [TRACK_BUILD.md](TRACK_BUILD.md) (**closed**, `1d6f4c5`)

## Status: **active** (step 4 pending)

**Goal:** `SDecl` variants in `semantic_ir.mlc` carry source spans; transform propagates; diagnostics use decl spans where available.

**Constraints:**
- One concern per sub-step; no `compiler/` + `lib/mlc/` in one commit.
- Self-host gate when `compiler/**` touched: `build_bin.sh` + diff empty.
- One decl variant group per step where possible.

## Verify gate (every step)

```
bundle exec rake test_compiler_mlc   # 755 pass (baseline post step 3)
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
| 4 | Checker diagnostics on decl paths use `SDecl` span helpers | pending |
| 5 | Audit remaining span-less IR; close track | pending |

## Context

- `SExpr` / `SStmt` already carry `Span` (`semantic_ir.mlc`).
- `SDeclFn`, `SDeclType`, `SDeclTrait`, `SDeclExtend` carry spans; `SDeclAssocBind` has span.
- `DeclType` / `DeclTrait` already have `name_span` in AST (TRACK_SPAN_CHECKER step 2).

## Deferred (not in this track)

- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
- `lib/mlc/` parity — not in scope.
- Record default expression parity (E4) — separate track if needed.

## Step 1 detail

- `SDeclFn` +8th field `Span`; `sdecl_span` helper.
- `transform_decl`: `expr_span(body)`.
- Test in `test_decl_gen.mlc`.

## Step 2 detail

- `SDeclType` +5th field `Span`; `sdecl_span` extended.
- `transform_decl`: `DeclType.name_span`.
- Test `transform_decl SDeclType span from name` in `test_decl_gen.mlc`.

## Step 3 detail

- `SDeclTrait` +4th field `Span`; `SDeclExtend` +4th field `Span`; `DeclExtend` +4th field `Span` in AST.
- `transform_decl`: `DeclTrait.name_span`, `DeclExtend.name_span` (type name span from parser).
- Tests `transform_decl SDeclTrait span from name`, `SDeclExtend span from name` in `test_decl_gen.mlc`.

## Next step (Driver)

**STEP=4** — checker diagnostics on decl paths use `SDecl` span helpers.
