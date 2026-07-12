# Agent session log

## Session 2026-05-22

| Field | Value |
|-------|-------|
| instructions_rev | `2026-07-12-multi-track-order` |
| agent_token_last | — |
| driver_turns_since_plan | 4 |
| step_last | 4 |
| active_track | TRACK_TEXT_SHIM_TO_MLC |
| test_gate | text_shaping_smoke → text_shaping_ok |

### Turn 2026-07-12 23:36 (Driver TRACK_TEXT_SHIM_TO_MLC STEP=4)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 4 |
| track   | TRACK_TEXT_SHIM_TO_MLC |
| started | 2026-07-12 23:36 |
| elapsed | ~15 min |
| done    | `misc/gui/text_shaping.mlc` + `text_shaping_smoke.mlc` (cache, pitch-copy, baseline). |
| verify  | mlcc+build_bin; run prints `text_shaping_ok` exit 0. |
| result  | STEP=4 done. Plain: MLC owns face/font cache over thin abi. |
| issues  | i64 zero via `i64_from_i32(0)` (mlcc rejects bare `0` / `0_i64` in files) |
| next    | ROLE=Driver STEP=5 TRACK_TEXT_SHIM_TO_MLC — golden vs shim glyph/shape output |

### Turn 2026-07-12 23:26 (Driver TRACK_TEXT_SHIM_TO_MLC STEP=3)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 3 |
| track   | TRACK_TEXT_SHIM_TO_MLC |
| started | 2026-07-12 23:26 |
| elapsed | ~8 min |
| done    | `harfbuzz_abi.hpp`/`.cpp` (create/destroy/shape + `hb_shape_glyph_*`); `build_bin.sh` wire. |
| verify  | `/tmp/hb_abi_smoke` prints `hb_abi_ok`. Old shim untouched. |
| result  | STEP=3 done. Plain: thin HB ABI ready for MLC shaping cache. |
| issues  | accessors named `hb_shape_glyph_*` to avoid link clash with shim |
| next    | ROLE=Driver STEP=4 TRACK_TEXT_SHIM_TO_MLC — `misc/gui/text_shaping.mlc` |

### Turn 2026-07-12 23:16 (Driver TRACK_TEXT_SHIM_TO_MLC STEP=2)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_TEXT_SHIM_TO_MLC |
| started | 2026-07-12 23:16 |
| elapsed | ~10 min |
| done    | `freetype_abi.hpp`/`.cpp` (i64 face, raw pitch slot, `ft_glyph_pitch`/`byte_at`); `build_bin.sh` wire. |
| verify  | `/tmp/ft_abi_smoke` prints `ft_abi_ok` (DejaVuSans). Old shim untouched. |
| result  | STEP=2 done. Plain: thin FT ABI ready for MLC cache. |
| issues  | shim cache strip deferred to STEP=8 (per Decision migration) |
| next    | ROLE=Driver STEP=3 TRACK_TEXT_SHIM_TO_MLC — `harfbuzz_abi.hpp` thin HB wrappers |

### Turn 2026-07-12 23:10 (Driver TRACK_TEXT_SHIM_TO_MLC STEP=1)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_TEXT_SHIM_TO_MLC |
| started | 2026-07-12 23:10 |
| elapsed | ~8 min |
| done    | Froze ABI Decision: i64 FT/HB abi; pitch via `ft_glyph_byte_at`; no RawPointer row ptr. |
| verify  | Cited shim lines; `memory.mlc` only null/is_null; TRACK Decision **frozen**. |
| result  | STEP=1 done. Plain: Driver can implement `freetype_abi` next. |
| issues  | none |
| next    | ROLE=Driver STEP=2 TRACK_TEXT_SHIM_TO_MLC — `freetype_abi.hpp` thin FT wrappers |

### Turn 2026-07-12 23:03 (Planner TRACK_TEXT_SHIM_TO_MLC plan-refresh)

| field   | value |
|---------|-------|
| role    | Planner |
| step    | plan-refresh |
| track   | TRACK_TEXT_SHIM_TO_MLC |
| started | 2026-07-12 23:03 |
| elapsed | ~5 min |
| done    | Ungated+activated TRACK; STEP=1 sub-steps; bearing note; PLAN/DEVELOPMENT sync; file tracked. |
| verify  | Status active; STEP=1 pending with sub-steps; PLAN §30 active; bearing exists in shim. |
| result  | Track active. Plain: next Driver freezes ABI Decision. |
| issues  | no MLC RawPointer byte-index found — prefer ABI `*_byte_at` |
| next    | ROLE=Driver STEP=1 TRACK_TEXT_SHIM_TO_MLC — freeze ABI Decision |

### Turn 2026-07-12 22:55 (Critic TRACK_LANG_REGION_ARENA critique-audit)

| field   | value |
|---------|-------|
| role    | Critic |
| step    | critique-audit |
| track   | TRACK_LANG_REGION_ARENA |
| started | 2026-07-12 22:55 |
| elapsed | ~8 min |
| done    | Audit STEP=1–9 vs `3b4df688`…`ca431a70`; close `9fc9c890`; re-smoke escape+cycle; sync LANGUAGE_AUDIT §8. |
| verify  | escape 3/3; cycle ok; region_alloc check-only 0; artifacts present; **reopen: none**. |
| result  | Critic OK. Plain: region arena shipped; queue → TEXT_SHIM. |
| issues  | residual: no assign-escape e2e fixture; STEP=10 deferred by design |
| next    | ROLE=Planner STEP=plan-refresh TRACK_TEXT_SHIM_TO_MLC — ungated activate STEP=1 |

### Turn 2026-07-12 22:37 (Driver TRACK_LANG_REGION_ARENA STEP=10)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 10 |
| track   | TRACK_LANG_REGION_ARENA |
| started | 2026-07-12 22:37 |
| elapsed | ~5 min |
| done    | STEP=10 skipped (ast.mlc region migration risky); track **closed** + archived. |
| verify  | TRACK Status closed; PLAN §7 done 8/8; `docs/archive/tracks/TRACK_LANG_REGION_ARENA.md`. |
| result  | Track closed. Plain: region feature done without parser AST rewrite. |
| issues  | none |
| next    | ROLE=Critic STEP=critique-audit TRACK_LANG_REGION_ARENA |

### Turn 2026-07-12 22:22 (Driver TRACK_LANG_REGION_ARENA STEP=9)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 9 |
| track   | TRACK_LANG_REGION_ARENA |
| started | 2026-07-12 22:22 |
| elapsed | ~5 min |
| done    | `docs/MLC.md` §C1 region/`RegionHandle`/`Region` + E091 vectors; table rows. |
| verify  | grep MLC.md: `region` / E091 / RegionHandle present under C1. |
| result  | STEP=9 done. Plain: language doc covers arena escape rules. |
| issues  | none |
| next    | ROLE=Driver STEP=10 TRACK_LANG_REGION_ARENA — optional hot-path or skip+close |

### Turn 2026-07-12 21:45 (Driver TRACK_LANG_REGION_ARENA STEP=8)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 8 |
| track   | TRACK_LANG_REGION_ARENA |
| started | 2026-07-12 21:39 |
| elapsed | ~34 min |
| done    | `scripts/regression_gate.sh` (specs + examples sweep). |
| verify  | 20 passed, 0 failed; examples ok=113 fail=0 skip=1. |
| result  | STEP=8 done. Plain: region changes do not break frozen stdout gate. |
| issues  | none |
| next    | ROLE=Driver STEP=9 TRACK_LANG_REGION_ARENA — docs MLC.md region/E091 |

### Turn 2026-07-12 21:25 (Driver TRACK_LANG_REGION_ARENA STEP=7)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 7 |
| track   | TRACK_LANG_REGION_ARENA |
| started | 2026-07-12 21:03 |
| elapsed | ~22 min |
| done    | Self-host: mlcc→`.tmp/mlc_p1` → `build_bin` →`.tmp/mlcc2` →`.tmp/mlc_p2`. |
| verify  | `diff -rq .tmp/mlc_p1 .tmp/mlc_p2 --exclude=obj` exit 0 (identical). |
| result  | STEP=7 done. Plain: region codegen/checker deterministic across mlcc2. |
| issues  | none |
| next    | ROLE=Driver STEP=8 TRACK_LANG_REGION_ARENA — `scripts/regression_gate.sh` |

### Turn 2026-07-12 20:57 (Driver TRACK_LANG_REGION_ARENA STEP=6)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 6 |
| track   | TRACK_LANG_REGION_ARENA |
| started | 2026-07-12 20:46 |
| elapsed | ~11 min |
| done    | `e2e/region_cycle.mlc` (index-cycle GraphNode allocs) + `run_region_cycle_smoke.sh`. |
| verify  | smoke: RegionHandle+alloc present; no shared_ptr/make_shared/std::atomic; run `cycle_ok`. |
| result  | STEP=6 done. Plain: cyclic graph in region without Shared refcount. |
| issues  | Cycle via next_index (Decision 3), not Region-typed fields (still E091). |
| next    | ROLE=Driver STEP=7 TRACK_LANG_REGION_ARENA — self-host mlcc→mlcc2 identical diff |

### Turn 2026-07-12 20:40 (Driver TRACK_LANG_REGION_ARENA STEP=5)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 5 |
| track   | TRACK_LANG_REGION_ARENA |
| started | 2026-07-12 20:32 |
| elapsed | ~8 min |
| done    | `e2e/region_escape_{return,closure,field}.mlc` + `run_region_escape_smoke.sh`. |
| verify  | smoke 3/3: each `--check-only` non-zero + `error[E091]` + vector message. |
| result  | STEP=5 done. Plain: three escape vectors are compile errors. |
| issues  | Foreign FFI_LAYER / TEXT_SHIM / extern_concurrency_lint left. |
| next    | ROLE=Driver STEP=6 TRACK_LANG_REGION_ARENA — cyclic graph positive; no Shared atomic in gen |

### Turn 2026-07-12 20:28 (Driver TRACK_LANG_REGION_ARENA STEP=4)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 4 |
| track   | TRACK_LANG_REGION_ARENA |
| started | 2026-07-12 19:44 |
| elapsed | ~44 min |
| done    | `mlc/memory/region.hpp` RegionHandle/RegionPtr; type_gen+cpp_naming maps; `e2e/region_alloc.mlc` smoke. |
| verify  | gen has `RegionHandle()`+`r.alloc(42)`; link+run `region_ok`; no shared_ptr in TU; self-host DIFF_EXIT=0. |
| result  | STEP=4 done. Plain: region allocates via pmr placement-new. |
| issues  | Foreign FFI_LAYER / TEXT_SHIM / extern_concurrency_lint left. |
| next    | ROLE=Driver STEP=5 TRACK_LANG_REGION_ARENA — escape e2e fixtures (return/closure/field) |

### Turn 2026-07-12 19:34 (Driver TRACK_LANG_REGION_ARENA STEP=3)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 3 |
| track   | TRACK_LANG_REGION_ARENA |
| started | 2026-07-12 18:13 |
| elapsed | ~81 min |
| done    | `region_escape.mlc` + E091 (return/closure/assign/field); wired in `check.mlc`; catalog 92; `test_region` escape asserts. |
| verify  | check-only ok=0; ret/closure/assign/field → E091; `main.mlc` 0; self-host DIFF_EXIT=0. |
| result  | STEP=3 done. Plain: region values cannot escape the block. |
| issues  | Ruby rake still MATCH-blocked; mlcc verify only. |
| next    | ROLE=Driver STEP=4 TRACK_LANG_REGION_ARENA — pmr codegen for region/alloc |

### Turn 2026-07-12 18:07 (Driver TRACK_LANG_REGION_ARENA STEP=2)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_LANG_REGION_ARENA |
| started | 2026-07-12 16:11 |
| elapsed | ~116 min |
| done    | `infer_region_method.mlc` RegionTag/Handle/alloc; infer+transform wire; `__region_handle_new` let binder; registry/globals; `test_region.mlc` check smokes. |
| verify  | `mlcc --check-only` empty/alloc/bind exit 0; `r.nope`→E082; `main.mlc` exit 0; self-host p1↔p2 DIFF_EXIT=0. |
| result  | STEP=2 done. Plain: `r.alloc` types as `Region<Tag,T>`; binder names-visible. |
| issues  | Ruby `rake test_compiler_mlc` still blocked pre-existing MATCH parse; used mlcc verify. |
| next    | ROLE=Driver STEP=3 TRACK_LANG_REGION_ARENA — escape diagnostic for Region\<Tag,T\> |

### Turn 2026-07-12 16:06 (Planner — plan-refresh)

| field   | value |
|---------|-------|
| role    | Planner |
| step    | plan-refresh |
| track   | TRACK_LANG_REGION_ARENA |
| started | 2026-07-12 15:59 |
| elapsed | ~7 min |
| done    | Chose REGION STEP=2 over TEXT_SHIM (CONTINUITY one-shot after TEXT_GL). TRACK active + STEP=2 sub-steps; PLAN §7/§30/queue; DEVELOPMENT priority. |
| verify  | TRACK Status=active Next=STEP=2; PLAN REGION active / TEXT_SHIM queued; no compiler/ this turn. |
| result  | plan-refresh done. Plain: REGION resumes; TEXT_SHIM stays open queued. |
| issues  | Foreign `FFI_LAYER.md` / untracked `TRACK_TEXT_SHIM_TO_MLC.md` / `extern_concurrency_lint.*` left. |
| next    | ROLE=Driver STEP=2 TRACK_LANG_REGION_ARENA — phantom RegionTag + `r.alloc` typing |

### Turn 2026-07-12 15:53 (Critic TRACK_TEXT_GL_PERF_BASELINE — critique-audit)

| field   | value |
|---------|-------|
| role    | Critic |
| step    | critique-audit |
| track   | TRACK_TEXT_GL_PERF_BASELINE |
| started | 2026-07-12 15:48 |
| elapsed | ~5 min |
| done    | Audited STEP=1–14 vs `c270ee16`…`4aaf5a5b`; caches/bearing/demos/docs/gate; PLAN/archive links. |
| verify  | `CachedFontFace`/`CachedShapingFont` present; `glyph_bearing_*` + 3 demos; fixtures hxpjy/privet/screenshots; REG 20/0 sweep 113/0/1 recorded at close. |
| result  | **reopen: none**. Residuals: STEP=8 3/6 placement files; STEP=9 MAE is shim FBO path (TRACK-specified); dual FT libs in FT/HB caches; TEXT_SHIM deferred. |
| issues  | Foreign `FFI_LAYER.md` / untracked `TRACK_TEXT_SHIM_TO_MLC.md` / `extern_concurrency_lint.*` left. |
| next    | ROLE=Planner STEP=plan-refresh — choose TEXT_SHIM_TO_MLC vs LANG_REGION_ARENA STEP=2 |

### Turn 2026-07-12 15:44 (Driver TRACK_TEXT_GL_PERF_BASELINE STEP=14 — close)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 14 |
| track   | TRACK_TEXT_GL_PERF_BASELINE |
| started | 2026-07-12 14:56 |
| elapsed | ~48 min |
| done    | `regression_gate.sh` green; TRACK closed + archived; SESSION history trimmed. |
| verify  | EXIT=0; programs **20/0**; examples sweep **ok=113 fail=0 skip=1** (`dynrecord_demo`). |
| result  | STEP=14 done; TRACK **closed**. Plain: face cache + baseline bearing shipped; gate green. |
| issues  | Foreign `FFI_LAYER.md` / untracked `TRACK_TEXT_SHIM_TO_MLC.md` / `extern_concurrency_lint.*` left. |
| next    | ROLE=Critic STEP=critique-audit TRACK_TEXT_GL_PERF_BASELINE |

### Turn 2026-07-12 14:55 (Driver TRACK_TEXT_GL_PERF_BASELINE STEP=13 — self-host N/A)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 13 |
| track   | TRACK_TEXT_GL_PERF_BASELINE |
| started | 2026-07-12 14:53 |
| elapsed | ~2 min |
| done    | Confirmed track range `c270ee16..80e7d241` needs no mlcc self-host rebuild. |
| verify  | `git diff --name-only`: no `lib/mlc/common/stdlib/`; `compiler/` only `tests/run_text_renderer_a8_string_smoke.sh`; no `compiler/**/*.mlc`. |
| result  | STEP=13 done. Plain: self-host gate N/A for this track. |
| issues  | Foreign `FFI_LAYER.md` / `TRACK_TEXT_SHIM_TO_MLC.md` / `extern_concurrency_lint.*` left. |
| next    | ROLE=Driver STEP=14 TRACK_TEXT_GL_PERF_BASELINE — regression_gate.sh green; close track |

### Turn 2026-07-12 14:53 (Driver TRACK_TEXT_GL_PERF_BASELINE STEP=12 — docs)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 12 |
| track   | TRACK_TEXT_GL_PERF_BASELINE |
| started | 2026-07-12 14:49 |
| elapsed | ~4 min |
| done    | `TEXT_RENDERING.md` §9 (face cache + bearing formula + file table); `GUI.md` text-field bearing note. |
| verify  | Docs cite `glyph_bearing_*`, `dest_y = pen_y - bearing_y`, affected demos/shims; links to TRACK + fixtures. |
| result  | STEP=12 done. Plain: positioning contract documented for GL live path. |
| issues  | Foreign `FFI_LAYER.md` / `TRACK_TEXT_SHIM_TO_MLC.md` / `extern_concurrency_lint.*` left. |
| next    | ROLE=Driver STEP=13 TRACK_TEXT_GL_PERF_BASELINE — confirm no stdlib/compiler signature change (self-host N/A) |

### Turn 2026-07-12 14:47 (Driver TRACK_TEXT_GL_PERF_BASELINE STEP=11 — visual)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 11 |
| track   | TRACK_TEXT_GL_PERF_BASELINE |
| started | 2026-07-12 14:17 |
| elapsed | ~30 min |
| done    | Visible screenshots of dashboard + text_window with `Hxpjy Agq`; window sleeps 50ms/frame when `MLC_GLFW_VISIBLE`. |
| verify  | `docs/agent/fixtures/text_dashboard_baseline_step11.png` (900×640), `text_window_baseline_step11.png` (800×600); `MLC_GLFW_VISIBLE=0 scripts/run_text_window_demo.sh` ok. |
| result  | STEP=11 done. Plain: mixed ascender/descender line sits on one baseline in both demos. |
| issues  | Foreign `FFI_LAYER.md` / `TRACK_TEXT_SHIM_TO_MLC.md` / `extern_concurrency_lint.*` left. |
| next    | ROLE=Driver STEP=12 TRACK_TEXT_GL_PERF_BASELINE — docs TEXT_RENDERING.md bearing note |

### Turn 2026-07-12 14:14 (Driver TRACK_TEXT_GL_PERF_BASELINE STEP=10 — privet golden)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 10 |
| track   | TRACK_TEXT_GL_PERF_BASELINE |
| started | 2026-07-12 14:11 |
| elapsed | ~3 min |
| done    | Confirmed `text_a8_privet_24.rgba` still valid; no regenerate. |
| verify  | `git hash-object` == `HEAD:…/text_a8_privet_24.rgba` (`d9e1cfbb`); `CCACHE_DISABLE=1 run_text_renderer_a8_string_smoke.sh` ok. |
| result  | STEP=10 done. Plain: Привет golden already matched bearing-aware CPU path; keep as-is. |
| issues  | Foreign `FFI_LAYER.md` / `TRACK_TEXT_SHIM_TO_MLC.md` / `extern_concurrency_lint.*` left. |
| next    | ROLE=Driver STEP=11 TRACK_TEXT_GL_PERF_BASELINE — visual MLC_GLFW_VISIBLE baseline check |

### Turn 2026-07-12 14:07 (Driver TRACK_TEXT_GL_PERF_BASELINE STEP=9 — mixed MAE)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 9 |
| track   | TRACK_TEXT_GL_PERF_BASELINE |
| started | 2026-07-12 14:00 |
| elapsed | ~7 min |
| done    | `"Hxpjy Agq"` in a8 string smoke + golden `text_a8_hxpjy_24.rgba` (GL vs CPU MAE≤8). |
| verify  | `CCACHE_DISABLE=1 compiler/tests/run_text_renderer_a8_string_smoke.sh` ok (Привет + Hxpjy); privet golden bytes unchanged. |
| result  | STEP=9 done. Plain: mixed ascender/descender baseline matches CPU reference within MAE. |
| issues  | Foreign `FFI_LAYER.md` / `TRACK_TEXT_SHIM_TO_MLC.md` / `extern_concurrency_lint.*` left. |
| next    | ROLE=Driver STEP=10 TRACK_TEXT_GL_PERF_BASELINE — confirm text_a8_privet_24.rgba still valid |

### Turn 2026-07-12 13:57 (Driver TRACK_TEXT_GL_PERF_BASELINE STEP=8 — wire bearing)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 8 |
| track   | TRACK_TEXT_GL_PERF_BASELINE |
| started | 2026-07-12 13:37 |
| elapsed | ~20 min |
| done    | `dest_x/y = cursor+bearing_x / pen_y-bearing_y` in dashboard, text_window, gui_text_field; GlyphCache stores bearings. |
| verify  | `CCACHE_DISABLE=1 scripts/run_text_renderer_native_smoke.sh` ok; `run_gui_text_field_demo.sh` ok; `run_text_window_demo.sh` ok. |
| result  | STEP=8 done. Plain: GL live path now baseline-aligns like CPU reference. |
| issues  | Foreign `FFI_LAYER.md` / `TRACK_TEXT_SHIM_TO_MLC.md` / `extern_concurrency_lint.*` left. Smokes without quad placement (freetype/harfbuzz/render_mode) unchanged. |
| next    | ROLE=Driver STEP=9 TRACK_TEXT_GL_PERF_BASELINE — mixed ascender MAE vs CPU reference |

### Turn 2026-07-12 13:35 (Driver TRACK_TEXT_GL_PERF_BASELINE STEP=7 — bearing ABI)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 7 |
| track   | TRACK_TEXT_GL_PERF_BASELINE |
| started | 2026-07-12 13:21 |
| elapsed | ~14 min |
| done    | `glyph_bearing_x`/`glyph_bearing_y` in freetype_shim; store `bitmap_left`/`bitmap_top` on last glyph buffer. |
| verify  | `CCACHE_DISABLE=1 compiler/tests/run_freetype_glyph_smoke.sh` → ok. Call sites not wired (STEP=8). |
| result  | STEP=7 done. Plain: bearing readable after glyph_bitmap_*; demos still top-aligned until STEP=8. |
| issues  | Foreign `FFI_LAYER.md` / `TRACK_TEXT_SHIM_TO_MLC.md` / `extern_concurrency_lint.*` left. Default ccache+PCH can stale-mtime fail smokes — use `CCACHE_DISABLE=1` if needed. |
| next    | ROLE=Driver STEP=8 TRACK_TEXT_GL_PERF_BASELINE — wire append_line dest_x/y with bearing |

### Turn 2026-07-12 13:18 (Driver TRACK_TEXT_GL_PERF_BASELINE STEP=6 — smokes)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 6 |
| track   | TRACK_TEXT_GL_PERF_BASELINE |
| started | 2026-07-12 13:16 |
| elapsed | ~3 min |
| done    | Ran Part A verify smokes after cache changes (no code edits). |
| verify  | `run_text_renderer_native_smoke.sh` ok; `run_gui_text_field_demo.sh` ok; `run_text_renderer_a8_string_smoke.sh` ok (golden); `run_freetype_glyph_smoke.sh` ok; `run_harfbuzz_shape_smoke.sh` ok. |
| result  | STEP=6 done. Plain: Part A did not regress text correctness. |
| issues  | Foreign `FFI_LAYER.md` / `TRACK_TEXT_SHIM_TO_MLC.md` / `extern_concurrency_lint.*` left. |
| next    | ROLE=Driver STEP=7 TRACK_TEXT_GL_PERF_BASELINE — glyph_bearing_x/y in freetype_shim |

### Turn 2026-07-12 13:15 (Driver TRACK_TEXT_GL_PERF_BASELINE STEP=5 — bench)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 5 |
| track   | TRACK_TEXT_GL_PERF_BASELINE |
| started | 2026-07-12 13:10 |
| elapsed | ~5 min |
| done    | `MLC_TEXT_DASHBOARD_BENCH=1` (300 frames, no sleep); timed before (pre-STEP1 shims+pre-STEP3 demo) vs after (STEPS 1–4). |
| verify  | `/usr/bin/time -v`: before User 8.97s / Elapsed 11.42s; after User 0.19s / Elapsed 0.36s (~47× user CPU). |
| result  | STEP=5 done. Plain: Part A cache stack ~47× cheaper on CPU. |
| issues  | Foreign `extern_concurrency_lint.*` / `TRACK_TEXT_SHIM_TO_MLC.md` / `.tmp/` left. |
| next    | ROLE=Driver STEP=6 TRACK_TEXT_GL_PERF_BASELINE — text smokes / golden MAE |

### Turn 2026-07-12 13:10 (Driver TRACK_TEXT_GL_PERF_BASELINE STEP=4 — atlas_dirty)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 4 |
| track   | TRACK_TEXT_GL_PERF_BASELINE |
| started | 2026-07-12 13:07 |
| elapsed | ~4 min |
| done    | `LineDrawResult.atlas_dirty`; upload only on glyph miss or box-brightness change; `fill_rect` skipped when brightness unchanged. |
| verify  | `mlcc`+`build_bin` ok; headless run timeout 124 (alive); gated `text_renderer_upload_atlas`. |
| result  | STEP=4 done. Plain: GPU atlas upload skipped on clean frames. |
| issues  | Foreign `extern_concurrency_lint.*` / `TRACK_TEXT_SHIM_TO_MLC.md` / `.tmp/` left. |
| next    | ROLE=Driver STEP=5 TRACK_TEXT_GL_PERF_BASELINE — bench 300 frames CPU time |

### Turn 2026-07-12 13:05 (Driver TRACK_TEXT_GL_PERF_BASELINE STEP=3 — GlyphCache wire)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 3 |
| track   | TRACK_TEXT_GL_PERF_BASELINE |
| started | 2026-07-12 13:03 |
| elapsed | ~5 min |
| done    | `text_dashboard_demo.mlc`: persistent `glyph_cache_new(512)` + atlas; `append_line` via get/insert; box slot packed once; `resize_zero` once at startup. Skipped `text_window_demo` (inline atlas). |
| verify  | `mlcc`+`build_bin.sh` ok; `MLC_GLFW_VISIBLE=0` run prints window open, no crash. |
| result  | STEP=3 done. Plain: glyphs reuse atlas slots across frames. |
| issues  | Foreign `extern_concurrency_lint.*` + `.tmp/` left. |
| next    | ROLE=Driver STEP=4 TRACK_TEXT_GL_PERF_BASELINE — atlas_dirty skip zero/upload |

### Turn 2026-07-12 13:02 (Driver TRACK_TEXT_GL_PERF_BASELINE STEP=2 — concurrency note)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_TEXT_GL_PERF_BASELINE |
| started | 2026-07-12 13:01 |
| elapsed | ~2 min |
| done    | Expanded concurrency docs in `freetype_shim.hpp`/`harfbuzz_shim.hpp` + matching `.cpp` cache comments (process-global, main/GL only, mutex before off-thread). |
| verify  | Headers mention mutex requirement; `rg std::mutex` on shim `.cpp` empty. |
| result  | STEP=2 done. Plain: thread constraint documented; no speculative lock. |
| issues  | Foreign `extern_concurrency_lint.*` + `.tmp/` left. |
| next    | ROLE=Driver STEP=3 TRACK_TEXT_GL_PERF_BASELINE — wire GlyphCache across frames in text_dashboard_demo |

### Turn 2026-07-12 13:00 (Driver TRACK_TEXT_GL_PERF_BASELINE STEP=1 — FT/HB face cache)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_TEXT_GL_PERF_BASELINE |
| started | 2026-07-12 12:56 |
| elapsed | ~5 min |
| done    | `CachedFontFace` in `freetype_shim.cpp`; `CachedShapingFont` in `harfbuzz_shim.cpp`; shared `FT_Library` for FT path; headers note single-thread. |
| verify  | `run_freetype_glyph_smoke.sh` exit 0; `run_harfbuzz_shape_smoke.sh` exit 0. |
| result  | STEP=1 done. Plain: faces/fonts reused; no per-call Init/Done. |
| issues  | Foreign `extern_concurrency_lint.*` + `.tmp/` left. |
| next    | ROLE=Driver STEP=2 TRACK_TEXT_GL_PERF_BASELINE — document single-thread cache constraint |

### Turn 2026-07-12 12:58 (Planner TRACK_TEXT_GL_PERF_BASELINE — activate)

| field   | value |
|---------|-------|
| role    | Planner |
| step    | plan-refresh |
| track   | TRACK_TEXT_GL_PERF_BASELINE |
| started | 2026-07-12 12:53 |
| elapsed | ~5 min |
| done    | Status **active**; Next=STEP=1 face/font cache sub-steps; REGION → **paused**; PLAN §21b + queue; DEVELOPMENT; CONTINUITY override row committed. |
| verify  | `FT_Init_FreeType` in `freetype_shim.cpp:56,89` + `harfbuzz_shim.cpp:28`; REGION STEP=1 commit `3b4df688` present. |
| result  | Track activated. Plain: Driver caches FT/HB faces. |
| issues  | Foreign `compiler/out/extern_concurrency_lint.*` + `.tmp/` left. |
| next    | ROLE=Driver STEP=1 TRACK_TEXT_GL_PERF_BASELINE — process-local FT/HB face cache |

### Turn 2026-07-12 12:55 (Driver TRACK_LANG_REGION_ARENA STEP=1 — ExprRegion parse)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_LANG_REGION_ARENA |
| started | 2026-07-12 12:25 |
| elapsed | ~30 min |
| done    | `ExprRegion` in `ast.mlc`; `parse_region_expr` in `exprs.mlc`; match arms infer/transform/escape/move/spawn/verify/partial_application; `test_region.mlc` + suite_registry; rebuild `compiler/out/mlcc`. |
| verify  | `mlcc --check-only` `region r do 0 end` (unit) exit 0; without `do` → `region: expected do`; `mlcc --check-only compiler/main.mlc` exit 0. Ruby `rake test_compiler_mlc` blocked pre-existing (MATCH parse on spawn_capture); full `build_tests_self` blocked pre-existing `../frontend/ast` resolve. |
| result  | STEP=1 done. Plain: `region name do…end` parses to ExprRegion. |
| issues  | Foreign `compiler/out/extern_concurrency_lint.*` left untracked. Next queue: TEXT_GL not REGION STEP=2 (CONTINUITY override). |
| next    | ROLE=Planner STEP=plan-refresh TRACK_TEXT_GL_PERF_BASELINE — activate; REGION STEP=2 paused |

### Turn 2026-07-12 12:26 (Planner TRACK_LANG_REGION_ARENA — activate)

| field   | value |
|---------|-------|
| role    | Planner |
| step    | plan-refresh |
| track   | TRACK_LANG_REGION_ARENA |
| started | 2026-07-12 12:23 |
| elapsed | ~4 min |
| done    | Status **active**; Next=STEP=1; AST = `ExprRegion` (∥ `ExprScope`); STEP=1 sub-steps; PLAN §7 + queue; DEVELOPMENT. |
| verify  | Decisions 1–3 present; `parse_scope_expr` cited as mirror; predecessor STDLIB_DOCS. |
| result  | Track activated. Plain: Driver starts region parser. |
| issues  | Foreign `compiler/out/*` left. |
| next    | ROLE=Driver STEP=1 TRACK_LANG_REGION_ARENA — ExprRegion AST + parse_region_expr |

