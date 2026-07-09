# Track: mlcc2 checker parity (do-block bindings)

Parent: [../PLAN.md](../PLAN.md) §Phase 4 self-hosting; blocker after [TRACK_LAMBDA_CAPTURE.md](TRACK_LAMBDA_CAPTURE.md)

## Status: **closed** (2026-06-19 Driver STEP=6 — verify gate green)

**STEP=6 note (2026-06-19 Driver):** Root cause E030: `partial_application_desugar_record_lit_parts` + `partial_application_desugar_field_values_sequence` — `const output_parts` / `.push` inside `parts.fold` (same COW class as STEP=3 `names.mlc`). Fix: `RecordLitPartsFoldState` + `concat`; field/expression sequences `concat` not `push`. Also: `record_lit_merge.mlc` fold refactor; parser `exprs.mlc` recursive `concat`; corpus `record_literal_explicit_fields.mlc`. Gate: `build.sh` ok; mlcc2 `--check-only` main=0; parity 2/2; **`diff_exit=0`**.

**STEP=6 note (2026-06-18 Meta recovery):** Guard stuck `Driver:5` … `expr_visitor_cpp.mlc`: non-nullary ctor → `CppCall` (`RecordLitFields(...)` not `{...}`); p1 `exprs.cpp` confirms parens. **Still:** mlcc2 `--check-only` E030 on `/tmp/point_repro.mlc`, `compiler/main.mlc`; corpus `do_block_tail_let_mut.mlc` mlcc2=0. Bisect: mlcc `record_one` misses only `right`; mlcc2 misses **all** fields → empty `explicit_field_values_flat` in mlcc2 binary (parser or checker in p1, not only `record_lit_merge`). Self-host `diff` not run. **Do not re-enqueue Driver STEP=5** — enqueue **STEP=6**.

**STEP=5 note (2026-06-18 Driver):** `exprs.mlc` parser `.push`→`concat`/`while`; `expr_visitor_cpp.mlc` ctor codegen fix; `build.sh` ok; p1 regen; mlcc2 build ok; mlcc=0 mlcc2=1 on record literals; diff_exit unreached.

**STEP=4 note (2026-06-18 Meta recovery):** Guard stuck `Driver:4`. Worktree: `names.mlc`, `check_mutations.mlc`, `record_lit_merge.mlc`, `exprs.mlc`, `expr_visitor_cpp.mlc` (uncommitted). Gate: `build_tests.sh` ok, `compiler/build.sh` ok, `mlcc --check-only compiler/main.mlc` exit 0, **`mlcc2 --check-only` exit 1 (E030)**. Minimal repro `Pair { left: 0, right: 0 }` — mlcc=0 mlcc2=1. `record_lit_merge` fold refactors alone did not fix. Hypothesis updated: mlcc2-compiled **parser** loses `RecordLitFields` field list (mlcc2 sees zero explicit fields). **Do not re-enqueue Driver STEP=4**.

**STEP=1 note (2026-06-16 Driver):** Corpus `compiler/tests/mlcc2_parity_corpus/do_block_tail_let_mut.mlc` (3 lines); `run_mlcc2_checker_parity.sh` (mlcc=0, mlcc2=1); `test_checker.mlc` assertion. Gate **999**/0.

**STEP=2 note (2026-06-19 Driver):** Bisect **names** — first (and only) pass that fails on mlcc2 for this repro. See **Bisect** below.

**STEP=3 note (2026-06-19 Driver):** `names.mlc` — thread scope via `fold` + `scope.concat` / `scope_with_bindings` (no `const scope = locals` + `.push` in fold). Corpus mlcc2 `--check-only` exit 0; `run_mlcc2_checker_parity.sh` expects mlcc=0 mlcc2=0. `main.mlc` still fails (E030/E031 — separate checker gaps, STEP=4).

**Проблема:** Ruby-bootstrap `compiler/out/mlcc` компилирует и проверяет `compiler/main.mlc`. Бинарь `mlcc2` (g++ из p1) падает на `--check-only` / `-o` с каскадом `undefined:` для `let`/`let mut`/`const` в `do`-блоках (`last_slash_index`, `options`, missing record fields). `diff_exit=1` — self-host gate заблокирован.

**Minimal repro:**
```mlc
fn main() -> i32 = do
  let mut counter = 0
  counter
end
```
```bash
compiler/out/mlcc --check-only compiler/tests/mlcc2_parity_corpus/do_block_tail_let_mut.mlc   # 0
.tmp_selfhost/mlcc2 --check-only compiler/tests/mlcc2_parity_corpus/do_block_tail_let_mut.mlc # 0 (was 1 before STEP=3)
bash compiler/tests/run_mlcc2_checker_parity.sh
```

## Bisect (STEP=2)

**Symptom on mlcc2:** `error[E001]: undefined: counter` (tail ident after `let mut` in `do` block).

| Pass | Runs when | Emits `undefined:` / E001? | Fails on repro? |
|------|-----------|----------------------------|-----------------|
| **names** | `check_names_expr` before infer (`check.mlc:436`) | yes — sole source (`names.mlc:58`, `diagnostic_code_e001`) | **yes** (mlcc2) |
| **infer** | `infer_expr` after names (`check.mlc:439`) | no `undefined:` string in `compiler/checker/infer/**` | not reached for E001 |
| **transform** | inside infer (`transform_expr` / `transform_stmts`) | no undefined-identifier diagnostics | not reached for E001 |

**Pipeline order (DeclFn body):** `partial_application_desugar_expr` → `check_names_expr` → `check_fn_body_mutations` → `infer_expr` (transform is sub-step of infer, not a separate scope pass).

**Failure locus (names, parser-Expr path):**
- `ExprBlock` → `check_names_block_expression` (`names.mlc:95–98`): tail `result` checked with `statements_parsed.scope`.
- `check_names_statements` (`names.mlc:243–285`): `const scope = locals`; after each `StmtLet` / `StmtLetConst`, `scope.push(name)`; returned `NameCheckResult { scope }` must include bindings for tail.

**STEP=3 hypothesis:** `const scope = locals` + `scope.push` on COW `[string]` inside `statements.fold` — binding not visible in returned `scope` when checker runs inside mlcc2 binary (codegen/runtime parity). Fix in `names.mlc` (and mirror `visit_block` / `check_names_semantic_statements` if same pattern).

## Verify gate

```
compiler/tests/build_tests.sh
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | Minimal repro: `do`-block `let mut` + tail ident; `test_checker.mlc` or dedicated `.mlc` corpus | done (uncommitted) |
| 2 | Bisect: names vs infer vs transform — first pass that loses scope | done (uncommitted) |
| 3 | Fix checker scope for `do`/block tail (root cause) | done (uncommitted) |
| 4 | `mlcc2 --check-only compiler/main.mlc` green | done (uncommitted) |
| 5 | Self-host `diff_exit=0`; close track | done (uncommitted) |
| 6 | Bisect mlcc2 empty record fields: p1 parser runtime vs `collect_explicit_field_values_flat`; add `record_literal` corpus; re-run verify gate | done (uncommitted) |
