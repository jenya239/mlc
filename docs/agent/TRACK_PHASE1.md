# Track: Phase 1 — stabilization

Parent: [../PLAN.md](../PLAN.md) §Phase 1; previous: [TRACK_CPPGEN.md](TRACK_CPPGEN.md) (**closed**, step 10 `005c65b`)

## Status: in progress (step 2 pending)

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
| 2 | `Diagnostic.code` field + `error[E001]:` prefix in format | pending |
| 3 | Replace `infer_messages_as_diagnostics` — infer emits `Diagnostic` with span at source | pending |
| 4 | Audit checker paths with `span_unknown`; fix high-traffic infer_call / type mismatch | pending |
| 5 | Negative tests in `test_checker.mlc` (invalid input → specific diagnostic) | pending |
| 6 | `--check-only` CLI flag (checker + transform, no codegen) | pending |
| 7 | Default output dir — temp under `$TMPDIR` instead of workspace `out/` | pending |
| 8 | Fuzz smoke: random program generator skeleton + mlcc no-crash test | pending |

## Step 1 detail

- File: `compiler/frontend/ast.mlc` (`diagnostic_format`), `compiler/tests/test_runner.mlc` (assert helpers).
- Output when span present:
  ```
  error: message
    --> file:line:col
  ```
- Keep backward-compatible fields; only format change + tests.
- No error codes yet (step 2).

## Deferred (not in this track)

- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15, separate branch.
- `expr_fragment_codegen` string bridges — bootstrap edge cases; revisit after Phase 1.
- `lib/mlc/` Ruby parity — not in scope.
- Phase 3 tooling (fmt, LSP) — new TRACK after Phase 1 closed.

## Next step (Driver)

**STEP=2** — `Diagnostic.code` field + `error[E001]:` prefix in format.
