# Track: mlcc optimization (plan 2026-05)

## Status: **closed** (commit `419de62`)

Baseline: `benchmarks/profile/record_baseline.sh` (post-commit).

---

# Track: PLAN.md roadmap

Parent: [../PLAN.md](../PLAN.md) §3 visitor + §4 folder restructure

## Status: folder restructure in progress (step 18 done; step 19 next)

| Step | Item | Status |
|------|------|--------|
| 1 | `compiler/pass.mlc` — `trait CompilerPass<Input, Output>` | done (`f199e92`) |
| 2 | `ExprVisitor` sketch + dispatch stub | done (`f199e92`) |
| 3 | `ExprVisitor<string>` codegen sketch | done (`df3826f`) |
| 4 | `dispatch_expr` + test graph | done (`161c3b4`) |
| 5 | Wire `gen_expr_via_string_visitor` into `expr_eval` | done (`0afcfd8`) |
| 6 | ExprVisitor: bool + unit arms | done (`a9a2ca6`) |
| 7 | ExprVisitor: numeric literal arms | done (`4e293c2`) |
| 8 | ExprVisitor: ident arm | done (`274dd68`) |
| 9 | ExprVisitor: bin/unary ops arm | done (`678e5d0`) |
| 10 | ExprVisitor: call + method arms | done (`b6c857a`) |
| 11 | ExprVisitor: if + block arms | done (`008abce`) |
| 12 | ExprVisitor: match arm | done (`14d9b0d`) |
| 13 | Remaining `expr_eval` → visitor (record, array, lambda, …) | done (`9bc1001`) |
| 14 | Self-host diff after visitor migration batch | done (`6df3799`, docs `0a0d321`) |
| 15 | Parser `ref mut` | **deferred** (separate branch) |
| 16 | Shrink `expr_eval.mlc` — `dispatch_expr` only, drop duplicate `match` | done (`6c5697a`) |
| 17 | Explicit `Pass` pipeline in `main.mlc` | done (`6f998e9`) |
| 18 | Move `frontend/` — lexer, ast, ast_tokens, parser/ | done (`bef5143`) |
| 19 | Move `ir/` — semantic_ir, record_defaults | done (`597b76b`) |
| 20 | Split `checker/` → infer/, transform/, check/ | done (`597b76b`) |
| 21 | Split `codegen/` → expr/, stmt/, decl/ | pending |
| 22 | Move root debug tests → `tests/`; delete junk | pending |
| 23 | Delete stale artefacts: `compiler/out2/`, `out2_tests/`, `out_modular/`, root `out/`, `main_modular_test.mlc`, `test_lexer.mlc`, `test_trait_e2e.mlc`; update `.gitignore` | pending |

## Backlog (Planner maintains)

Source: PLAN.md §4 «Порядок миграции».

- **Step 19:** `semantic_ir.mlc` → `ir/semantic_ir.mlc`; `record_field_default_initializer.mlc` → `ir/record_defaults.mlc`; imports only
- **Steps 20–22:** same gate per sub-step; do not bundle layers in one commit
- **Step 15:** parser `ref mut` — separate branch, not in this track
- **CppExpr backend:** PLAN Phase 2 — new TRACK when step 22 done

## Step 17 detail (done — `6f998e9`)

- `compiler/pipeline.mlc`: checker → transform → codegen passes
- `main.mlc`: `compile_modular_loop` removed; calls `run_modular_compiler_pipeline`
- verify: 490 tests; build.sh; self-host diff empty

## Step 18 detail (done — `bef5143`)

- moved: lexer, ast, ast_tokens → `compiler/frontend/`; parser/ → `frontend/parser/`; preds → predicates.mlc
- all import paths updated; no logic changes
- verify: 490 tests; build.sh; self-host diff empty

## Step 19 detail (commit-prep)

- `semantic_ir.mlc` → `ir/semantic_ir.mlc`; `record_field_default_initializer.mlc` → `ir/record_defaults.mlc`
- import paths updated (~30 modules); no logic changes
- verify: 490 tests; build.sh; self-host diff empty

## Next step (Driver)

**STEP=19-commit** — user `git commit`; then STEP=20

## Planner checklist (2026-05-25 plan-refresh #2)

- [x] Step 18 marked done with commit `bef5143`
- [x] Step 19 scoped with file list + verify gate
- [x] No step bundles `compiler/` + `lib/mlc/`
