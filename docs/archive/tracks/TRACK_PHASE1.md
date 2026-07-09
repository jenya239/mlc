# Track: Phase 1 — stabilization

Parent: [../PLAN.md](../PLAN.md) §Phase 1; previous: [TRACK_CPPGEN.md](TRACK_CPPGEN.md) (**closed**, step 10 `005c65b`)

## Status: closed (step 8 done)

**Goal:** reliable mlcc — full diagnostic spans, test coverage, crash-safety, clean default output dir.

**Constraints:**
- One concern per sub-step; no `compiler/` + `lib/mlc/` in one commit.
- Self-host gate when `compiler/**` touched: `build_bin.sh` + `runtime/src/core/profile.cpp`.

## Verify gate (every step)

```
bundle exec rake test_compiler_mlc   # 511 pass (baseline post-CPPGEN)
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | `diagnostic_format` — rustc-style (`error: msg` + `  --> file:line:col`); update test_runner helpers | done (`a74d480`) |
| 2 | `Diagnostic.code` field + `error[E001]:` prefix in format | done (`01a15c5`) |
| 3 | Replace `infer_messages_as_diagnostics` — infer emits `Diagnostic` with span at source | done (`46e2280`) |
| 4 | Audit checker paths with `span_unknown`; fix high-traffic infer_call / type mismatch | done (`6d5150a`) |
| 5 | Negative tests in `test_checker.mlc` (invalid input → specific diagnostic) | done (`8b3fe71`) |
| 6 | `--check-only` CLI flag (checker + transform, no codegen) | done (`177659a`) |
| 7 | Default output dir — temp under `$TMPDIR` instead of workspace `out/` | done (`9c56ca9`) |
| 8 | Fuzz smoke: random program generator skeleton + mlcc no-crash test | done (`fd42eab`) |

## Step 1 detail

- File: `compiler/frontend/ast.mlc` (`diagnostic_format`), `compiler/tests/test_runner.mlc` (assert helpers).
- Output when span present:
  ```
  error: message
    --> file:line:col
  ```
- Keep backward-compatible fields; only format change + tests.
- No error codes yet (step 2).

## Step 2 detail

- File: `compiler/frontend/ast.mlc` — add `code: string` to `Diagnostic`; extend `diagnostic_new` / `diagnostic_error` (default `''`).
- `diagnostic_format`: when `code.length() > 0`, prefix `severity + '[' + code + ']: '` instead of `severity + ': '`.
- Example with span: `error[E001]: message\n  --> file:line:col`.
- Assign at least one real code in one checker path (e.g. undefined ident) + update `test_checker.mlc` / `assert_diagnostic_at` if needed.
- Do not migrate all call sites — bulk assignment is step 3–4 scope.

## Deferred (not in this track)

- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15, separate branch.
- `expr_fragment_codegen` string bridges — bootstrap edge cases; revisit after Phase 1.
- `lib/mlc/` Ruby parity — not in scope.
- Phase 3 tooling (fmt, LSP) — new TRACK after Phase 1 closed.

## Next step (Driver)

TRACK_PHASE1 **closed**. Planner: pick next track per [CONTINUITY.md](CONTINUITY.md) track closure order.
