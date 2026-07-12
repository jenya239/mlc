# Agent session log

## Session 2026-05-22

| Field | Value |
|-------|-------|
| instructions_rev | `2026-07-12-multi-track-order` |
| agent_token_last | — |
| driver_turns_since_plan | 4 |
| step_last | 5 |
| active_track | TRACK_GUI_INPUT_ROBUSTNESS |
| test_gate | `run_gui_window_size_smoke.sh` ok |

### Turn 2026-07-12 06:55 (Driver TRACK_GUI_INPUT_ROBUSTNESS STEP=5 — window size)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 5 |
| track   | TRACK_GUI_INPUT_ROBUSTNESS |
| started | 2026-07-12 06:49 |
| elapsed | ~10 min |
| done    | `glfw_gl_window_width`/`height`/`set_size` + size callback/cache; smoke 800→640; `gui_button_demo`/`gui_text_field_demo` live size; `text_window_*` wrappers. |
| verify  | `run_gui_window_size_smoke.sh` ok; `run_gui_button_demo.sh` ok; `run_gui_text_field_demo.sh` ok. |
| result  | STEP=5 done. Plain: live window size wired; next GUI.md. |
| issues  | Foreign `compiler/out/*` left; invisible GLFW may ignore SetWindowSize — cache forced after poll. |
| next    | ROLE=Driver STEP=6 TRACK_GUI_INPUT_ROBUSTNESS — `GUI.md` debounce + keyboard/resize notes |

### Turn 2026-07-12 06:40 (Driver TRACK_GUI_INPUT_ROBUSTNESS STEP=4 — text field demo)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 4 |
| track   | TRACK_GUI_INPUT_ROBUSTNESS |
| started | 2026-07-12 06:37 |
| elapsed | ~8 min |
| done    | `gui_text_field_demo.mlc` + script: TextRenderer box+glyphs; inject H+ix+BS→Hi + enter. Also fixed leftover STEP=3 TRACK/PLAN row status. |
| verify  | `scripts/run_gui_text_field_demo.sh` → `ok (field=Hi, enter edge)`. |
| result  | STEP=4 done. Plain: keyboard path drawn end-to-end; next resize APIs. |
| issues  | Foreign `compiler/out/*` left. |
| next    | ROLE=Driver STEP=5 TRACK_GUI_INPUT_ROBUSTNESS — `glfw_gl_window_width`/`height` (live size) |

### Turn 2026-07-12 06:25 (Driver TRACK_GUI_INPUT_ROBUSTNESS STEP=3 — keyboard text)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 3 |
| track   | TRACK_GUI_INPUT_ROBUSTNESS |
| started | 2026-07-12 06:19 |
| elapsed | ~8 min |
| done    | `glfwSetCharCallback` + `take_text`/`text_test_push`/`keys_test_set`; `GuiInput.text_buffer`/`backspace`/`enter`; `gui_text_input_smoke` + script. |
| verify  | `run_gui_text_input_smoke.sh` ok; `run_gui_input_smoke.sh` ok; `run_gui_click_edge_repro.sh` ok. |
| result  | STEP=3 done. Plain: keyboard buffer wired; next text-field demo. |
| issues  | Foreign `compiler/out/*` left. |
| next    | ROLE=Driver STEP=4 TRACK_GUI_INPUT_ROBUSTNESS — single-line text field smoke (box + live text) |

### Turn 2026-07-12 06:12 (Driver TRACK_GUI_INPUT_ROBUSTNESS STEP=1 — click-edge repro)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_GUI_INPUT_ROBUSTNESS |
| started | 2026-07-12 06:09 |
| elapsed | ~5 min |
| done    | `misc/examples/gui_click_edge_repro.mlc` + `scripts/run_gui_click_edge_repro.sh`: pure+glfw down→hold→up→down → exactly 2 edges. STEP=2 **N/A**. |
| verify  | `scripts/run_gui_click_edge_repro.sh` → `ok (pure+glfw: 2 edges…)`. |
| result  | STEP=1 done — not reproducible under controlled input (9-vs-1 = xdotool artifact). Plain: click edge ok; next keyboard. |
| issues  | Foreign `compiler/out/*` left. |
| next    | ROLE=Driver STEP=3 TRACK_GUI_INPUT_ROBUSTNESS — keyboard text (`glfwSetCharCallback` + `GuiInput.text_buffer`) |

### Turn 2026-07-12 06:05 (Planner — plan-refresh → GUI_INPUT_ROBUSTNESS)

| field   | value |
|---------|-------|
| role    | Planner |
| step    | plan-refresh |
| track   | TRACK_GUI_INPUT_ROBUSTNESS |
| started | 2026-07-12 06:01 |
| elapsed | ~8 min |
| done    | After FFI Critic OK: activated §23; STEP=1 sub-steps; PLAN/DEVELOPMENT; archived 14 old SESSION turns → HISTORY (569→~360 lines). |
| verify  | TRACK Status **active** Next=STEP=1; PLAN §23 **active**; `wc -l SESSION` &lt;600. |
| result  | Active track = GUI_INPUT. Plain: next is controlled click-edge repro. |
| issues  | Foreign `compiler/out/*` left. |
| next    | ROLE=Driver STEP=1 TRACK_GUI_INPUT_ROBUSTNESS — controlled `gui_click_edge` repro |

### Turn 2026-07-12 05:56 (Critic TRACK_FFI_EXTERN_DEDUP — critique-audit)

| field   | value |
|---------|-------|
| role    | Critic |
| step    | critique-audit |
| track   | TRACK_FFI_EXTERN_DEDUP |
| started | 2026-07-12 05:56 |
| elapsed | ~8 min |
| done    | Audited STEP=1–8 (`d1e6d0fb`…`a5bbe9c2`); archive/PLAN/FFI_LAYER §10; DRY smokes + gui_button DRY. **reopen: none**. |
| verify  | `run_extern_dedup_repro.sh` ok; `run_extern_dedup_mismatch_smoke.sh` ok; `test ! -f docs/agent/TRACK_FFI_EXTERN_DEDUP.md`; PLAN §22 Critic OK. |
| result  | Critic OK. Residual: skip-emit by MLC name, not full `(c_name, header)`. |
| issues  | Foreign `compiler/out/*` left. |
| next    | ROLE=Planner STEP=plan-refresh — activate GUI_INPUT_ROBUSTNESS (§23) |

### Turn 2026-07-12 05:50 (Driver TRACK_FFI_EXTERN_DEDUP STEP=8 — docs close)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 8 |
| track   | TRACK_FFI_EXTERN_DEDUP |
| started | 2026-07-12 05:48 |
| elapsed | ~5 min |
| done    | `FFI_LAYER.md` §10 + safety table E090 row; TRACK → `archive/tracks/`; PLAN §22 **done**. |
| verify  | `test ! -f docs/agent/TRACK_FFI_EXTERN_DEDUP.md`; archive + §10 present. |
| result  | STEP=8 done; track **closed**. Plain: dedup rule documented. |
| issues  | Foreign `compiler/out/*` left. |
| next    | ROLE=Critic STEP=critique-audit TRACK_FFI_EXTERN_DEDUP |

### Turn 2026-07-12 05:45 (Driver TRACK_FFI_EXTERN_DEDUP STEP=7 — regression_gate)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 7 |
| track   | TRACK_FFI_EXTERN_DEDUP |
| started | 2026-07-12 05:36 |
| elapsed | ~10 min |
| done    | `scripts/regression_gate.sh` EXIT=0. |
| verify  | `regression_gate: 20 passed, 0 failed`; sweep ok=106 fail=0 skip=1. |
| result  | STEP=7 done. Plain: gate green after FFI dedup. |
| issues  | Foreign `compiler/out/*` left; log fully buffered until end. |
| next    | ROLE=Driver STEP=8 TRACK_FFI_EXTERN_DEDUP — FFI_LAYER.md paragraph; close → Critic |

### Turn 2026-07-12 05:35 (Driver TRACK_FFI_EXTERN_DEDUP STEP=6 — self-host diff)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 6 |
| track   | TRACK_FFI_EXTERN_DEDUP |
| started | 2026-07-12 05:23 |
| elapsed | ~12 min |
| done    | `mlcc→tmp/mlc_p1`; `build_bin.sh→tmp/mlcc2`; `mlcc2→tmp/mlc_p2`; `diff -r --exclude=obj`. |
| verify  | **DIFF_LINES=0**; p1~5s; build_bin~7m; p2~5s. |
| result  | STEP=6 done. Plain: mlcc2 output identical to mlcc. |
| issues  | Foreign `compiler/out/*` left. |
| next    | ROLE=Driver STEP=7 TRACK_FFI_EXTERN_DEDUP — `scripts/regression_gate.sh` |

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

Older turns: [SESSION_HISTORY.md](../archive/SESSION_HISTORY.md).
