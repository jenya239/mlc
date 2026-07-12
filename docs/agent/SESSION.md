# Agent session log

## Session 2026-05-22

| Field | Value |
|-------|-------|
| instructions_rev | `2026-07-12-multi-track-order` |
| agent_token_last | — |
| driver_turns_since_plan | 5 |
| step_last | 5 |
| active_track | TRACK_FFI_EXTERN_DEDUP |
| test_gate | examples sweep ok=106 fail=0 skip=1 |

### Turn 2026-07-12 05:20 (Driver TRACK_FFI_EXTERN_DEDUP STEP=5 — examples sweep)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 5 |
| track   | TRACK_FFI_EXTERN_DEDUP |
| started | 2026-07-12 05:17 |
| elapsed | ~5 min |
| done    | Full `run_examples_compile_sweep.sh` after E090/Hybrid; gui_button demos included. |
| verify  | summary **ok=106 fail=0 skip=1** (dynrecord); `gui_button_demo` OK. |
| result  | STEP=5 done. Plain: examples still green post-dedup. |
| issues  | Foreign `compiler/out/*` left. |
| next    | ROLE=Driver STEP=6 TRACK_FFI_EXTERN_DEDUP — mlcc→mlcc2 self-host diff |

### Turn 2026-07-12 05:10 (Driver TRACK_FFI_EXTERN_DEDUP STEP=4 — E090 mismatch)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 4 |
| track   | TRACK_FFI_EXTERN_DEDUP |
| started | 2026-07-12 04:28 |
| elapsed | ~40 min |
| done    | `extern_dedup_lint.mlc` E090; `check.mlc` wire; `mismatch.mlc` + smoke; arity lint `let mut result` index fix. |
| verify  | `run_extern_dedup_mismatch_smoke.sh` ok; `run_extern_header_arity_smoke.sh` ok; `run_extern_dedup_repro.sh` ok. |
| result  | STEP=4 done. Plain: arity conflict → E090 with earlier site in message. |
| issues  | Foreign `compiler/out/*` left; Map-return match codegen void bug worked around. |
| next    | ROLE=Driver STEP=5 TRACK_FFI_EXTERN_DEDUP — examples compile sweep |

### Turn 2026-07-12 04:20 (Driver TRACK_FFI_EXTERN_DEDUP STEP=3 — Hybrid skip-emit)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 3 |
| track   | TRACK_FFI_EXTERN_DEDUP |
| started | 2026-07-12 04:15 |
| elapsed | ~15 min |
| done    | `decl_cpp.mlc`: `ffi_extern_reuses_imported_binding` skips FFI proto/def when `qualified.has(name)`; local emit uses `cpp_safe`. Flipped `run_extern_dedup_repro.sh`. Rebuilt mlcc. |
| verify  | `TMPDIR=$PWD/tmp compiler/tests/run_extern_dedup_repro.sh` → ok; hpp has only `main` proto. |
| result  | STEP=3 done. Plain: import wins; no enclose-namespace. |
| issues  | Foreign `compiler/out/*` left unstaged except if needed; rebuild refreshed out. |
| next    | ROLE=Driver STEP=4 TRACK_FFI_EXTERN_DEDUP — mismatch signature → mlcc error |

### Turn 2026-07-12 04:05 (Driver TRACK_FFI_EXTERN_DEDUP STEP=2 — repro fixture)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_FFI_EXTERN_DEDUP |
| started | 2026-07-12 04:04 |
| elapsed | ~8 min |
| done    | `fixtures/extern_dedup/{provider,redeclare}.mlc`; `run_extern_dedup_repro.sh` — clang `does not enclose namespace 'provider'` on `provider::sleep_ms_probe` inside `namespace redeclare`. |
| verify  | `TMPDIR=$PWD/tmp compiler/tests/run_extern_dedup_repro.sh` → exit 0 (baseline fail). |
| result  | STEP=2 done. Plain: repro locked; STEP=3 flips script to pass. |
| issues  | Foreign `compiler/out/*` left. |
| next    | ROLE=Driver STEP=3 TRACK_FFI_EXTERN_DEDUP — Hybrid: skip second FFI emit / fix import proto |

### Turn 2026-07-12 03:55 (Driver TRACK_FFI_EXTERN_DEDUP STEP=1 — design Hybrid)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_FFI_EXTERN_DEDUP |
| started | 2026-07-12 03:54 |
| elapsed | ~8 min |
| done    | Read `decl_cpp.mlc:694-729` (`gen_ffi_fn_decl_cpp`), `module.mlc:184-213` (per-TU namespace), `extern_header_arity_lint.mlc` (no cross-module key). **Decision: option 3 Hybrid.** PLAN/DEVELOPMENT/TRACK updated. |
| verify  | TRACK Decision section + STEP=1 done; no `compiler/` edits. |
| result  | STEP=1 done. Plain: identical key+sig reuse; mismatch → mlcc error. |
| issues  | Foreign `compiler/out/*` left. |
| next    | ROLE=Driver STEP=2 TRACK_FFI_EXTERN_DEDUP — two-module identical-redeclare repro |

### Turn 2026-07-12 03:50 (Planner — plan-refresh → FFI_EXTERN_DEDUP)

| field   | value |
|---------|-------|
| role    | Planner |
| step    | plan-refresh |
| track   | TRACK_FFI_EXTERN_DEDUP |
| started | 2026-07-12 03:49 |
| elapsed | ~10 min |
| done    | Activated §22 after EXAMPLES_CI Critic OK. STEP=1 sub-steps; PLAN §22 **active**; DEVELOPMENT pointer; fixed EXAMPLES_CI archive link in TRACK. |
| verify  | TRACK Next=STEP=1; PLAN queue FFI_EXTERN_DEDUP first open; no `compiler/` edits. |
| result  | Active track = FFI_EXTERN_DEDUP. Plain: next is design pick for extern dedup. |
| issues  | Foreign `compiler/out/*` left. |
| next    | ROLE=Driver STEP=1 TRACK_FFI_EXTERN_DEDUP — design decision (options 1/2/3) |

### Turn 2026-07-12 03:45 (Critic TRACK_EXAMPLES_CI — critique-audit)

| field   | value |
|---------|-------|
| role    | Critic |
| step    | critique-audit |
| track   | TRACK_EXAMPLES_CI |
| started | 2026-07-12 03:45 |
| elapsed | ~10 min |
| done    | Audited STEP=1–11 (`cd598a50`…`d48ec41c`); sweep+gate+docs+archive/PLAN; DRY ok. **reopen: none**. |
| verify  | DRY run=106 skip=1 gui_button_demo=1; `test ! -f docs/agent/TRACK_EXAMPLES_CI.md`; PLAN §21 Critic OK. |
| result  | Critic OK. Residual: dynrecord_demo allowlisted (mlcc segfault). |
| issues  | Foreign `compiler/out/*` left. |
| next    | ROLE=Planner STEP=plan-refresh — activate FFI_EXTERN_DEDUP (§22) |

### Turn 2026-07-12 03:40 (Driver TRACK_EXAMPLES_CI STEP=6 — verify-gate close)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 6 |
| track   | TRACK_EXAMPLES_CI |
| started | 2026-07-12 03:19 |
| elapsed | ~25 min |
| done    | `regression_gate.sh` EXIT=0; TRACK → `archive/tracks/`; PLAN §21 **done**. |
| verify  | `regression_gate: 20 passed, 0 failed`; sweep ok=106 fail=0 skip=1. |
| result  | STEP=6 done; track **closed**. Plain: examples compile sweep gated in CI path. |
| issues  | Foreign `compiler/out/*` left. |
| next    | ROLE=Critic STEP=critique-audit TRACK_EXAMPLES_CI |

### Turn 2026-07-12 03:15 (Driver TRACK_EXAMPLES_CI STEP=5 — docs sweep-gate)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 5 |
| track   | TRACK_EXAMPLES_CI |
| started | 2026-07-12 03:13 |
| elapsed | ~5 min |
| done    | README §Тесты + DEVELOPMENT Tier C: `regression_gate.sh` runs examples compile sweep. |
| verify  | Lines present in README.md and docs/agent/DEVELOPMENT.md. |
| result  | STEP=5 done. Plain: docs mention the new gate step. |
| issues  | Foreign `compiler/out/*` left. |
| next    | ROLE=Driver STEP=6 TRACK_EXAMPLES_CI — full `regression_gate.sh`; close → Critic |

### Turn 2026-07-12 03:10 (Driver TRACK_EXAMPLES_CI STEP=4 — wire sweep into gate)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 4 |
| track   | TRACK_EXAMPLES_CI |
| started | 2026-07-12 03:07 |
| elapsed | ~10 min |
| done    | `regression_gate.sh` runs `run_examples_compile_sweep.sh` after program PASS. |
| verify  | `bash -n` ok; full sweep ok=106 fail=0 skip=1 (dynrecord). |
| result  | STEP=4 done. Plain: examples compile sweep is part of regression_gate. |
| issues  | Foreign `compiler/out/*` left. Full `regression_gate` left for STEP=6. |
| next    | ROLE=Driver STEP=5 TRACK_EXAMPLES_CI — docs line for sweep-gate |

### Turn 2026-07-12 02:55 (Driver TRACK_EXAMPLES_CI STEP=11 — fix cluster E vm_*)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 11 |
| track   | TRACK_EXAMPLES_CI |
| started | 2026-07-12 02:51 |
| elapsed | ~25 min |
| done    | Rewrote 8 vm_*: Choice helpers for match; pop→index; `Result<T,E>`+`i32!i32`+`?`; record fields. |
| verify  | ONLY eight → ok=8 fail=0. |
| result  | STEP=11 done. Plain: cluster E examples compile+link under mlcc. |
| issues  | Foreign `compiler/out/*` left. Match must visit typed sum param (codegen bug workaround). |
| next    | ROLE=Driver STEP=4 TRACK_EXAMPLES_CI — wire sweep into regression_gate.sh |

### Turn 2026-07-12 02:45 (Driver TRACK_EXAMPLES_CI STEP=10 — fix cluster D)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 10 |
| track   | TRACK_EXAMPLES_CI |
| started | 2026-07-12 02:42 |
| elapsed | ~15 min |
| done    | Rewrote 7 D-demos (HOF/fold/index/todo/safety/Shared); mlcc `[i32]`/`string`/`.to_string()`. |
| verify  | ONLY seven → ok=7 fail=0. |
| result  | STEP=10 done. Plain: cluster D examples compile+link. |
| issues  | Foreign `compiler/out/*` left. pointer_types reduced to Shared smoke. |
| next    | ROLE=Driver STEP=11 TRACK_EXAMPLES_CI — fix cluster E vm_* |

### Turn 2026-07-12 02:35 (Driver TRACK_EXAMPLES_CI STEP=9 — ABI soft-skip + link)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 9 |
| track   | TRACK_EXAMPLES_CI |
| started | 2026-07-12 02:27 |
| elapsed | ~20 min |
| done    | Sweep: sodium/libpq probe + `mlc_link_libs` (-L for `.tmp_*`); `build_bin.sh` pass-through for `-` lines. |
| verify  | ONLY four (crypto/postgres abi+demo) → ok=4 fail=0. |
| result  | STEP=9 done. Plain: ABI examples link when `.tmp_libsodium`/`.tmp_libpq` present. |
| issues  | Foreign `compiler/out/*` left. Subshell LIBRARY_PATH bug fixed via direct call + `-L`. |
| next    | ROLE=Driver STEP=10 TRACK_EXAMPLES_CI — fix cluster D one-offs |

### Turn 2026-07-12 02:20 (Driver TRACK_EXAMPLES_CI STEP=8 — fix cluster B to_string)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 8 |
| track   | TRACK_EXAMPLES_CI |
| started | 2026-07-12 02:14 |
| elapsed | ~25 min |
| done    | Rewrote 7 demos: `.to_string()`, mlcc match/`let mut`/`string`/`x =>`; dropped free `to_string`. |
| verify  | ONLY seven → ok=7 fail=0. |
| result  | STEP=8 done. Plain: cluster B demos compile+link under mlcc. |
| issues  | Foreign `compiler/out/*` left. Generics demo uses monomorphic helpers (no `<T>`). |
| next    | ROLE=Driver STEP=9 TRACK_EXAMPLES_CI — ABI soft-skip crypto/postgres |

### Turn 2026-07-12 01:55 (Driver TRACK_EXAMPLES_CI STEP=7 — fix cluster A imports)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 7 |
| track   | TRACK_EXAMPLES_CI |
| started | 2026-07-12 01:53 |
| elapsed | ~25 min |
| done    | mlcc imports on A-demos; soft-skip sodium/libpq; dynrecord stdlib relative imports; allowlist dynrecord_demo (mlcc segfault). |
| verify  | ONLY six → ok=3 fail=0 skip=3; no `misc/examples/.mlc` miss. |
| result  | STEP=7 done. Plain: Ruby `import Env::` demos now mlcc-parse. |
| issues  | Foreign `compiler/out/*` left. dynrecord allowlisted. |
| next    | ROLE=Driver STEP=8 TRACK_EXAMPLES_CI — fix cluster B (`to_string`) |

### Turn 2026-07-12 01:45 (Driver TRACK_EXAMPLES_CI STEP=3 — full sweep inventory)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 3 |
| track   | TRACK_EXAMPLES_CI |
| started | 2026-07-12 01:34 |
| elapsed | ~25 min |
| done    | Full `run_examples_compile_sweep.sh`; recorded 30 failures; clusters A–E + STEPs 7–11 before wire. |
| verify  | summary ok=77 fail=30 skip=0; failure list in TRACK. |
| result  | STEP=3 done. Plain: sweep finds 30 broken examples; fix queue opened. |
| issues  | Foreign `compiler/out/*` left. Log in `tmp/examples_sweep_full.log` (not committed). |
| next    | ROLE=Driver STEP=7 TRACK_EXAMPLES_CI — fix cluster A (`…/.mlc` imports) |

### Turn 2026-07-12 01:28 (Driver TRACK_EXAMPLES_CI STEP=2 — sysdep soft-skip)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_EXAMPLES_CI |
| started | 2026-07-12 01:27 |
| elapsed | ~10 min |
| done    | Soft-skip in `run_examples_compile_sweep.sh` (glfw3/freetype2/harfbuzz/font path); empty `examples_compile_sweep_allowlist.txt`. |
| verify  | Fake pkg-config: SKIP gui+freetype, RUN minimal; allowlist: SKIP gui + OK minimal (exit 0). Dry w/ deps: skip=0 run=107. |
| result  | STEP=2 done. Plain: CI without GLFW/fonts soft-skips instead of hard-fail. |
| issues  | Foreign `compiler/out/*` left. |
| next    | ROLE=Driver STEP=3 TRACK_EXAMPLES_CI — full sweep failure inventory |

### Turn 2026-07-12 01:20 (Driver TRACK_EXAMPLES_CI STEP=1 — compile sweep script)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_EXAMPLES_CI |
| started | 2026-07-12 01:18 |
| elapsed | ~15 min |
| done    | Added `scripts/run_examples_compile_sweep.sh` (discover `fn main`, mlcc+build_bin, DRY/ONLY/MAX). |
| verify  | DRY: 107 entries, `gui_button_demo` present; `EXAMPLES_SWEEP_ONLY='minimal.mlc gui_button_demo.mlc'` → ok=2 fail=0. |
| result  | STEP=1 done. Plain: examples compile+link sweep script works. |
| issues  | Foreign `compiler/out/*` left. Full 107 inventory deferred to STEP=3. |
| next    | ROLE=Driver STEP=2 TRACK_EXAMPLES_CI — sysdep soft-skip / allowlist |

### Turn 2026-07-12 01:10 (Planner — plan-refresh → EXAMPLES_CI)

| field   | value |
|---------|-------|
| role    | Planner |
| step    | plan-refresh |
| track   | TRACK_EXAMPLES_CI |
| started | 2026-07-12 01:09 |
| elapsed | ~10 min |
| done    | Rev sync `2026-07-11-runtime-stays-cpp` → `2026-07-12-multi-track-order`. Activated §21 `TRACK_EXAMPLES_CI` (queue head after ERROR_UNION). STEP=1 sub-steps; PLAN §21 **active**; DEVELOPMENT pointer. |
| verify  | TRACK Next=STEP=1; PLAN queue EXAMPLES_CI first; no `compiler/` edits. |
| result  | Active track = EXAMPLES_CI. Plain: next work is examples compile sweep. |
| issues  | Foreign `compiler/out/*` left. |
| next    | ROLE=Driver STEP=1 TRACK_EXAMPLES_CI — `scripts/run_examples_compile_sweep.sh` |

### Turn 2026-07-11 34:05 (Critic TRACK_LANG_ERROR_UNION — critique-audit)

| field   | value |
|---------|-------|
| role    | Critic |
| step    | critique-audit |
| track   | TRACK_LANG_ERROR_UNION |
| started | 2026-07-11 33:50 |
| elapsed | ~15 min |
| done    | Audited STEP=1–5 (`57913a3f`…`4ee642cc`); Ruby+mlcc desugar; e2e+`?`; MLC.md; archive/PLAN; re-ran smoke + Ruby 6/0 + main 0. **reopen: none**. |
| verify  | smoke ok; Ruby 6/0; `test ! -f docs/agent/TRACK_LANG_ERROR_UNION.md`; PLAN §17 Critic OK. |
| result  | Critic OK. Residual: inferred `T!` out of scope by design. |
| issues  | Foreign `compiler/out/*` left. |
| next    | ROLE=Planner STEP=plan-refresh — next after ERROR_UNION (DEBUG_SOURCE_MAP or PLAN pick) |

### Turn 2026-07-11 33:50 (Driver TRACK_LANG_ERROR_UNION STEP=5 — docs + close)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 5 |
| track   | TRACK_LANG_ERROR_UNION |
| started | 2026-07-11 33:25 |
| elapsed | ~45 min |
| done    | `MLC.md` T!E; REG 20/0; self-host DIFF 0; TRACK → `archive/tracks/`; PLAN §17 **done**. |
| verify  | `regression_gate: 20 passed, 0 failed`; `diff -rq p1 p2` empty; smoke ok. |
| result  | STEP=5 done; track **closed**. Plain: T!E sugar shipped and gated. |
| issues  | Foreign `compiler/out/*` left. |
| next    | ROLE=Critic STEP=critique-audit TRACK_LANG_ERROR_UNION |

### Turn 2026-07-11 33:25 (Driver TRACK_LANG_ERROR_UNION STEP=4 — e2e + ?)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 4 |
| track   | TRACK_LANG_ERROR_UNION |
| started | 2026-07-11 33:10 |
| elapsed | ~15 min |
| done    | Fixture `with_question.mlc`; smoke compile/link/run; Ruby `?`+to_cpp tests. |
| verify  | smoke ok (stdout `6` / `division by zero`); `bundle exec ruby … error_union_sugar_test.rb` 6/0. |
| result  | STEP=4 done. Plain: `T!E` works with `?` end-to-end. |
| issues  | Foreign `compiler/out/*` left. |
| next    | ROLE=Driver STEP=5 TRACK_LANG_ERROR_UNION — MLC.md + REG + self-host; close → Critic |

### Turn 2026-07-11 33:10 (Driver TRACK_LANG_ERROR_UNION STEP=3 — self-hosted desugar)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 3 |
| track   | TRACK_LANG_ERROR_UNION |
| started | 2026-07-11 32:50 |
| elapsed | ~20 min |
| done    | `types.mlc` `apply_error_union_sugar`; fixtures + `run_error_union_sugar_smoke.sh`; `test_parser.mlc` T!E cases; rebuilt `mlcc`. |
| verify  | smoke ok; `ok.mlc` check-only clean; `bang_missing` → `expected type after ! in T!E`. |
| result  | STEP=3 done. Plain: mlcc parses `i32!string` as `Result`. |
| issues  | Foreign `compiler/out/*` left. Nested `else do` broke Ruby codegen — used early `return`. |
| next    | ROLE=Driver STEP=4 TRACK_LANG_ERROR_UNION — e2e + `?` compat tests |

### Turn 2026-07-11 32:50 (Driver TRACK_LANG_ERROR_UNION STEP=2 — Ruby desugar)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_LANG_ERROR_UNION |
| started | 2026-07-11 32:35 |
| elapsed | ~15 min |
| done    | `type_parser.rb` `apply_error_union_sugar`; `test/mlc/error_union_sugar_test.rb` (ret/field/explicit/`T!` error). |
| verify  | `bundle exec ruby -Ilib:test test/mlc/error_union_sugar_test.rb` → 4 runs, 0 failures. |
| result  | STEP=2 done. Plain: Ruby parses `i32!string` as `Result<i32,string>`. |
| issues  | Foreign `compiler/out/*` left. |
| next    | ROLE=Driver STEP=3 TRACK_LANG_ERROR_UNION — self-hosted `types.mlc` mirror |

### Turn 2026-07-11 32:35 (Driver TRACK_LANG_ERROR_UNION STEP=1 — Design)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_LANG_ERROR_UNION |
| started | 2026-07-11 32:20 |
| elapsed | ~10 min |
| done    | Locked Decision: type postfix `!` after base/before `->`; desugar `TyGeneric("Result")`; no new AST; unary `!`/`?` expr-only; cites `lexer.mlc:430-431`, `exprs.mlc` `parse_unary`. |
| verify  | Decision table in TRACK; PLAN §17 STEP=2; `rg 'parse_unary\|Op\(\"!\"\)' compiler/frontend`. |
| result  | STEP=1 done. Plain: `T!E` = sugar for `Result<T,E>` in type grammar. |
| issues  | Foreign `compiler/out/*` left. |
| next    | ROLE=Driver STEP=2 TRACK_LANG_ERROR_UNION — Ruby type_parser `T!E` desugar |

### Turn 2026-07-11 32:20 (Planner — activate TRACK_LANG_ERROR_UNION)

| field   | value |
|---------|-------|
| role    | Planner |
| step    | plan-refresh |
| track   | TRACK_LANG_ERROR_UNION |
| started | 2026-07-11 32:05 |
| elapsed | ~10 min |
| done    | After FFI Critic OK: activated §17 `TRACK_LANG_ERROR_UNION` **active** STEP=1 + sub-steps 1–5; skipped §15 DEBUG (research); PLAN queue/DEVELOPMENT. |
| verify  | TRACK Status **active**; PLAN §17 **active** STEP=1; `test -f docs/agent/TRACK_LANG_ERROR_UNION.md`. |
| result  | STEP=plan-refresh done. Plain: next is Design for `-> T!E` sugar. |
| issues  | Foreign `compiler/out/*` left. DEBUG/PACKAGE/AUTO_CYCLE stay queued. |
| next    | ROLE=Driver STEP=1 TRACK_LANG_ERROR_UNION — Design: lock `-> T!E` grammar |

### Turn 2026-07-11 32:05 (Critic TRACK_FFI_SAFETY — critique-audit)

| field   | value |
|---------|-------|
| role    | Critic |
| step    | critique-audit |
| track   | TRACK_FFI_SAFETY |
| started | 2026-07-11 31:50 |
| elapsed | ~15 min |
| done    | Audited STEP=1–5 (`e1db7d81`…`3806d49c`); Decision B; lints+hooks; §9; archive/PLAN; re-ran both smokes + main check-only. **reopen: none**. |
| verify  | smokes 0; main 0; `test ! -f docs/agent/TRACK_FFI_SAFETY.md`; PLAN §14 Critic OK. |
| result  | Critic OK. Residuals: arity by C-name; websocket/glad/… full-form still unannotated (warn). |
| issues  | Foreign `compiler/out/*` left. |
| next    | ROLE=Planner STEP=plan-refresh — next after FFI_SAFETY (low-pri §15/§17 or PLAN pick) |

### Turn 2026-07-11 31:50 (Driver TRACK_FFI_SAFETY STEP=5 — verify-gate + close)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 5 |
| track   | TRACK_FFI_SAFETY |
| started | 2026-07-11 31:00 |
| elapsed | ~50 min |
| done    | `regression_gate` 20/0; self-host p1↔p2 DIFF 0; smokes ok; TRACK → `archive/tracks/`; PLAN §14 **done**; FFI_LAYER/DEVELOPMENT links. |
| verify  | REG: `20 passed, 0 failed`; `diff -rq p1 p2 --exclude=obj` empty; `run_extern_*_smoke.sh` ok. |
| result  | STEP=5 done; track **closed**. Plain: FFI safety diagnostics + docs gated. |
| issues  | Foreign `compiler/out/*` left. Residuals: arity by C-name only; misc may warn. |
| next    | ROLE=Critic STEP=critique-audit TRACK_FFI_SAFETY |

### Turn 2026-07-11 31:00 (Driver TRACK_FFI_SAFETY STEP=4 — FFI_LAYER §9)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 4 |
| track   | TRACK_FFI_SAFETY |
| started | 2026-07-11 30:45 |
| elapsed | ~10 min |
| done    | Rewrote `docs/FFI_LAYER.md` §9 contract table; archived old SESSION turns → `SESSION_HISTORY.md`; fixed `test_checker.mlc` import for `W-EXTERN-ARITY`. |
| verify  | `rg 'W-EXTERN-ATTR\|W-EXTERN-ARITY\|Decision B' docs/FFI_LAYER.md` hits §9; SESSION lines &lt;100; PLAN/TRACK STEP=5. |
| result  | STEP=4 done. Plain: safety contract documented; lifetime/null stay author duty. |
| issues  | Foreign `compiler/out/*` left. |
| next    | ROLE=Driver STEP=5 TRACK_FFI_SAFETY — regression_gate + self-host; close → Critic |

### Turn 2026-07-11 30:45 (Driver TRACK_FFI_SAFETY STEP=3 — W-EXTERN-ARITY)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 3 |
| track   | TRACK_FFI_SAFETY |
| started | 2026-07-11 30:20 |
| elapsed | ~25 min |
| done    | `extern_header_arity_lint.mlc` + `W-EXTERN-ARITY`; header stubs marker/`PatternUnit`/`CppFnProto`; hook in `check.mlc`; smoke `run_extern_header_arity_smoke.sh`. |
| verify  | arity smoke ok (mismatch→W-EXTERN-ARITY; match_ok/no_import clean); concurrency smoke ok; `mlcc --check-only compiler/main.mlc` 0. |
| result  | STEP=3 done. Plain: full-form arity ≠ imported header stub arity → warning. |
| issues  | Foreign `compiler/out/*`. Best-effort by C symbol name only (no path link). |
| next    | ROLE=Driver STEP=4 TRACK_FFI_SAFETY — Safety contract docs in FFI_LAYER.md §9 |

### Turn 2026-07-11 30:20 (Driver TRACK_FFI_SAFETY STEP=2 — W-EXTERN-ATTR)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_FFI_SAFETY |
| started | 2026-07-11 29:55 |
| elapsed | ~25 min |
| done    | `extern_concurrency_lint.mlc` + `W-EXTERN-ATTR`; hook in `check.mlc`; env+tcp full-form → `blocking`; smoke `run_extern_concurrency_attr_smoke.sh`; tests in `test_checker.mlc`. |
| verify  | smoke ok; `mlcc --check-only` missing→warning / blocking+shorthand→clean; `run_env_log_gate.sh` OK; `mlcc --check-only compiler/main.mlc` 0. |
| result  | STEP=2 done. Plain: missing concurrency attr warns; stdlib full-form annotated. |
| issues  | Foreign `compiler/out/*`. Ruby `spawn_capture` parse still blocks `rake test_compiler_mlc`; `build_tests_self` path pre-broken. misc full-form may still warn. |
| next    | ROLE=Driver STEP=3 TRACK_FFI_SAFETY — ABI arity sanity vs header_import |

### Turn 2026-07-11 29:55 (Driver TRACK_FFI_SAFETY STEP=1 — Design)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_FFI_SAFETY |
| started | 2026-07-11 29:40 |
| elapsed | ~15 min |
| done    | Inventory `lib/mlc`: 255 `extern fn`, 0 concurrency attrs; Decision **B** (no `unsafe` syntax); severity=warning; no implicit attr; STEP=2 must annotate stdlib or exempt. |
| verify  | `rg -c 'extern fn' lib/mlc` sum=255; `rg blocking\|thread_safe\|thread_affine lib/mlc --glob '*.mlc'` empty; Decision table in TRACK. |
| result  | STEP=1 done. Plain: `extern` is the marker; warn, don't invent `unsafe`. |
| issues  | Foreign `compiler/out/*` left. |
| next    | ROLE=Driver STEP=2 TRACK_FFI_SAFETY — checker warning missing concurrency attr + stdlib annotate |

### Turn 2026-07-11 29:40 (Planner — activate TRACK_FFI_SAFETY)

| field   | value |
|---------|-------|
| role    | Planner |
| step    | plan-refresh |
| track   | TRACK_FFI_SAFETY |
| started | 2026-07-11 29:25 |
| elapsed | ~15 min |
| done    | §20 parent → **done** (20a–e closed); activated §14 `TRACK_FFI_SAFETY` **active** STEP=1 + sub-steps 1–5; PLAN queue/DEVELOPMENT. |
| verify  | TRACK Status **active**; PLAN §14 **active** STEP=1; PLAN §20 **done**; `test -f docs/agent/TRACK_FFI_SAFETY.md`. |
| result  | STEP=plan-refresh done. Plain: next is FFI safety Design (no new codegen). |
| issues  | Foreign `compiler/out/*` left. ERROR_UNION/DEBUG_SOURCE_MAP stay queued. |
| next    | ROLE=Driver STEP=1 TRACK_FFI_SAFETY — Design: unsafe marker vs diagnostics-only |

Older turns: [SESSION_HISTORY.md](../archive/SESSION_HISTORY.md).
