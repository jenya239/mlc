# Track: Build determinism + compile_commands (Phase 1 §6)

Parent: [../PLAN.md](../PLAN.md) §Phase 1 §6; previous: [TRACK_SPAN_CHECKER.md](TRACK_SPAN_CHECKER.md) (**closed**, `a8bf7a1`)

## Status: **active** (step 2 pending)

**Goal:** deterministic C++ link/compile order; `--emit-compile-commands` for clangd.

**Constraints:**
- One concern per sub-step; no `compiler/` + `lib/mlc/` in one commit.
- Self-host gate when `compiler/**` touched: `build_bin.sh` + diff empty.
- Shell-only steps may skip self-host if no `compiler/**/*.mlc` change.

## Verify gate (every step)

```
bundle exec rake test_compiler_mlc   # 739 pass (baseline post SPAN_CHECKER)
compiler/build.sh                    # when compiler/** touched
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | `build_bin.sh` — deterministic sorted cpp compile order | done (`ed8ac00`) |
| 2 | `compile_options.mlc` — `--emit-compile-commands` flag parsing | pending |
| 3 | Modular pipeline — emit `compile_commands.json` to out dir | pending |
| 4 | Test/smoke — flag + json output | pending |
| 5 | Build audit; close track | pending |

## Step 1 detail

- File: `compiler/build_bin.sh` only.
- Sort `"$CPP_DIR"/*.cpp` before parallel compile (stable basename order).
- Do not touch `compile_options.mlc` or pipeline in this step.

## Deferred (not in this track)

- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
- Ruby `ModularCompiler` compile_commands — mlcc-only first.
- `lib/mlc/` parity — not in scope.

## Next step (Driver)

**STEP=2** — `compile_options.mlc` `--emit-compile-commands` flag.
