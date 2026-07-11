# Agent session log

## Session 2026-05-22

| Field | Value |
|-------|-------|
| instructions_rev | `2026-07-11-runtime-stays-cpp` |
| agent_token_last | — |
| driver_turns_since_plan | 4 |
| step_last | 4 |
| active_track | TRACK_LANG_ERROR_UNION STEP=5 |
| test_gate | STEP=4: smoke e2e T!E+`?` stdout ok; Ruby error_union 6/0 |

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
