# Track: Build determinism + compile_commands (Phase 1 §6)

Parent: [../PLAN.md](../PLAN.md) §Phase 1 §6; previous: [TRACK_SPAN_CHECKER.md](TRACK_SPAN_CHECKER.md) (**closed**, `a8bf7a1`)

## Status: **closed** (`1d6f4c5`)

**Closed:** step 5 build audit — all deliverables wired; 751 tests; deferred items documented; no code changes.

**Goal:** deterministic C++ link/compile order; `--emit-compile-commands` for clangd.

**Constraints:**
- One concern per sub-step; no `compiler/` + `lib/mlc/` in one commit.
- Self-host gate when `compiler/**` touched: `build_bin.sh` + diff empty.
- Shell-only steps may skip self-host if no `compiler/**/*.mlc` change.

## Verify gate (every step)

```
bundle exec rake test_compiler_mlc   # 751 pass (baseline post step 4)
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
| 2 | `compile_options.mlc` — `--emit-compile-commands` flag parsing | done (`6077707`) |
| 3 | Modular pipeline — emit `compile_commands.json` to out dir | done (`a60052a`) |
| 4 | Test/smoke — flag + json output | done (`ab0f34d`) |
| 5 | Build audit; close track | done (`1d6f4c5`) |

## Step 5 audit

| Area | Status | Notes |
|------|--------|-------|
| `build_bin.sh` sort | OK | `LC_ALL=C sort` on `$CPP_DIR/*.cpp` before parallel compile (`ed8ac00`) |
| CLI flag | OK | `--emit-compile-commands` → `CompileOptions.emit_compile_commands`; default false |
| Pipeline wiring | OK | `main.mlc` → `ModularCompileInput` → `run_codegen_pass`; skipped when `check_only` |
| JSON format | OK | `[{directory,file,arguments}]`; `clang++ -std=c++20 -O2 -I out -I runtime/include -c file` |
| Tests | OK | `test_pass.mlc` (3 flag) + `test_compile_commands.mlc` (9) + smoke phase 6/6 |
| Self-host | OK | diff empty after steps 2–4 |

No fixable gaps; deferred below.

## Deferred (not in this track)

- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
- Ruby `ModularCompiler` compile_commands — mlcc-only first.
- `lib/mlc/` parity — not in scope.

## Step 1 detail

- File: `compiler/build_bin.sh` only.
- Sort `"$CPP_DIR"/*.cpp` before parallel compile (stable basename order).

## Step 2 detail

- `CompileOptions.emit_compile_commands: bool`; parse `--emit-compile-commands`.
- Usage string updated; 3 tests in `test_pass.mlc`.

## Step 3 detail

- `compile_commands.mlc`: JSON builder + `write_compile_commands_file`.
- `ModularCompileInput.emit_compile_commands`; wired in `main.mlc`.
- `run_codegen_pass` writes `compile_commands.json` after cpp emission.

## Step 4 detail

- `test_compile_commands.mlc`: JSON builder, pipeline integration (+9 tests).
- `run_compile_commands_smoke.sh`: mlcc `--emit-compile-commands` on hello.mlc; phase 6/6 in `build_tests.sh`.
