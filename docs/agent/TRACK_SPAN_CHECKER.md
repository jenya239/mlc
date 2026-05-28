# Track: Checker/codegen span audit (Phase 1 §1 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 1 §1; previous: [TRACK_PARSER_PANIC.md](TRACK_PARSER_PANIC.md) (**closed**, `5525d03`)

## Status: **active** (step 5 pending)

**Goal:** replace fixable `span_unknown()` in checker/codegen diagnostics and IR with source spans; document remaining synthetics.

**Constraints:**
- One concern per sub-step; no `compiler/` + `lib/mlc/` in one commit.
- Self-host gate when `compiler/**` touched: `build_bin.sh` + diff empty.
- One module group per step (mirror TRACK_PARSER_PANIC).

## Verify gate (every step)

```
bundle exec rake test_compiler_mlc   # 738 pass (baseline post PARSER_PANIC)
compiler/build.sh                    # when compiler/** touched
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | `checker/check/` — `check.mlc`, `call_argument_unify.mlc` real spans on diagnostics | done (`4002943`) |
| 2 | `checker/transform/trait_param_expand.mlc` — E073 on decl name span | done (`6d22fe4`) |
| 3 | `checker/transform/` — `transform.mlc`, `param_destructure_expand.mlc` audit + fix where span available | done (`7a94caa`) |
| 4 | `codegen/expr/` — `record_gen.mlc`, `expression_support.mlc` span propagation | done (`7132801`) |
| 5 | Grep audit checker+codegen `span_unknown`; close track | pending |

## Step 1 detail

- Files: `compiler/checker/check/check.mlc`, `compiler/checker/check/call_argument_unify.mlc` only.
- Targets: record-field-default generic error (~L58), derive diagnostics (~L444), call unify span (~L246).
- Tests: extend `test_checker.mlc` only if assert location changes.
- Do not touch transform/ or codegen in this step.

## Deferred (not in this track)

- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
- `--emit-compile-commands` / build determinism — future TRACK_BUILD.
- `lib/mlc/` Ruby parity — not in scope.
- Test-only `span_unknown` in `compiler/tests/**` — out of scope.

## Next step (Driver)

**STEP=5** — grep audit checker+codegen `span_unknown`; close track.
