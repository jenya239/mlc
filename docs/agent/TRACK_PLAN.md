# Track: mlcc optimization (plan 2026-05)

## Status: **closed** (commit `419de62`)

Baseline: `benchmarks/profile/record_baseline.sh` (post-commit).

---

# Track: PLAN.md roadmap

Parent: [../PLAN.md](../PLAN.md) §3 visitor + §4 folder restructure

## Status: folder restructure **done** (steps 1–23)

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
| 21 | Split `codegen/` → expr/, stmt/, decl/ | done (`ef3fedf`; split dirs `6b355c3`) |
| 22 | Move root debug tests → `tests/`; delete junk | done (`318a727`) |
| 23 | Delete stale artefacts; update `.gitignore` | done (`318a727`) |

## Backlog (Planner maintains)

Source: PLAN.md §4 «Порядок миграции» + §Phase 1.

- **Step 15:** parser `ref mut` — separate branch, not in this track
- **CppExpr backend:** [TRACK_CPPEXPR.md](TRACK_CPPEXPR.md) — closed (`96df4e0`)
- **CppAST cleanup:** [TRACK_CPPGEN.md](TRACK_CPPGEN.md) — closed (`005c65b`)
- **Phase 1 stabilization:** [TRACK_PHASE1.md](TRACK_PHASE1.md) — **closed** (`fd42eab`)
- **Safety / fuzz hardening:** [TRACK_SAFETY.md](TRACK_SAFETY.md) — **closed** (`32f8335`)
- **Performance:** [TRACK_PERFORMANCE.md](TRACK_PERFORMANCE.md) — **closed** (`0671422`)
- **Security / fuzz depth:** [TRACK_SECURITY.md](TRACK_SECURITY.md) — **closed** (`a035c3d`)
- **Diagnostics / error codes:** [TRACK_DIAGNOSTICS.md](TRACK_DIAGNOSTICS.md) — **closed** (`f6863fe`)
- **Test coverage:** [TRACK_COVERAGE.md](TRACK_COVERAGE.md) — **active** (step 4)

## Next step (Driver)

**STEP=4** [TRACK_COVERAGE.md](TRACK_COVERAGE.md) — codegen expr smoke.

## Planner checklist (2026-05-28 plan-refresh — coverage track)

- [x] TRACK_DIAGNOSTICS closed (`f6863fe`)
- [x] TRACK_COVERAGE opened (5 steps: checker lambda/array, tuple/record, match/for, codegen expr, codegen stmt)
- [x] No step bundles `compiler/` + `lib/mlc/`
- [x] Driver enqueued STEP=1

## Planner checklist (2026-05-28 plan-refresh — diagnostics closed)

- [x] TRACK_DIAGNOSTICS closed (`f6863fe`)
- [ ] Open next track from PLAN
- [ ] Driver enqueued STEP=1

## Planner checklist (2026-05-28 plan-refresh — diagnostics track)

- [x] TRACK_SECURITY closed (`a035c3d`)
- [x] TRACK_DIAGNOSTICS opened (5 steps: catalog, binary/type, method receiver, infer/transform, audit sweep)
- [x] No step bundles `compiler/` + `lib/mlc/`
- [x] Driver enqueued STEP=1

## Planner checklist (2026-05-28 plan-refresh — security track)

- [x] TRACK_PERFORMANCE closed (`0671422`)
- [x] TRACK_SECURITY opened (4 steps: corpus, generator, in-process sweep, panic audit)
- [x] No step bundles `compiler/` + `lib/mlc/`
- [x] Driver enqueued STEP=1

## Planner checklist (2026-05-28 plan-refresh — performance track)

- [x] TRACK_SAFETY closed (`32f8335`)
- [x] TRACK_PERFORMANCE opened (5 steps, verify + perf gate)
- [x] No step bundles `compiler/` + `lib/mlc/`
- [x] Driver enqueued STEP=1

## Planner checklist (2026-05-25 plan-refresh #3 — Phase 2)

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

## Step 20 detail (commit-prep)

- checker split: `infer/`, `transform/`, `check/` (+ diagnostics/, method_types/)
- filenames kept (unique C++ basenames; short renames blocked by `path_to_module_base`)
- verify: 490 tests; build.sh; self-host diff empty

## Planner checklist (2026-05-25 plan-refresh #3 — Phase 2)

- [x] Step 18 marked done with commit `bef5143`
- [x] Step 19 scoped with file list + verify gate
- [x] No step bundles `compiler/` + `lib/mlc/`
- [x] TRACK_CPPEXPR.md created (21 steps, verify gate)
- [x] Folder restructure track closed; Phase 2 active
