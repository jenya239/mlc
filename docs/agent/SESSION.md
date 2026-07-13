# Agent session log

## Session 2026-05-22

| Field | Value |
|-------|-------|
| instructions_rev | `2026-07-12-multi-track-order` |
| agent_token_last | — |
| driver_turns_since_plan | 0 |
| step_last | critique-audit |
| active_track | — (DEBUG closed; next GUI_CANVAS_GRAPH) |
| test_gate | Planner → GUI_CANVAS_GRAPH Phase A |

### Turn 2026-07-13 14:54 (Critic TRACK_DEBUG_SOURCE_MAP — critique-audit)

| field   | value |
|---------|-------|
| role    | Critic |
| step    | critique-audit |
| track   | TRACK_DEBUG_SOURCE_MAP |
| started | 2026-07-13 14:47 |
| elapsed | ~7 min |
| done    | Audited STEP1–5 vs `195d48a0`…`b7b6d878`; PLAN/archive; re-ran line_directive + gdb + if_body #line. |
| verify  | test 3/0; NOJAM; gdb `crash.mlc:3`; mlcc if leaf `#line`; DEBUGGING.md in HEAD. |
| result  | **reopen: none**. Residuals: stale TRACK «Проблема»; mlcc #line on if leaves not before `if`; PIE addr2line. |
| issues  | Foreign SCRIPT_VM/arkanoid/solid_renderer left. |
| next    | ROLE=Planner STEP=plan-refresh — activate GUI_CANVAS_GRAPH Phase A |

### Turn 2026-07-13 14:44 (Driver TRACK_DEBUG_SOURCE_MAP STEP=5)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 5 |
| track   | TRACK_DEBUG_SOURCE_MAP |
| started | 2026-07-13 14:04 |
| elapsed | ~40 min |
| done    | `docs/DEBUGGING.md` + MLC/specs links; Ruby `\n#line` fix (`context.rb`); archive TRACK; close. |
| verify  | `line_directive_test.rb` 3/0; `regression_gate.sh` 20/0; examples sweep ok=117 fail=0 skip=1. |
| result  | STEP=5 done; track **closed**. Plain: docs+gate green; compact `#line` jam fixed. |
| issues  | first gate 12 fail on `{#line` mid-line — fixed before close |
| next    | ROLE=Critic STEP=critique-audit TRACK_DEBUG_SOURCE_MAP |

### Turn 2026-07-13 14:02 (Driver TRACK_DEBUG_SOURCE_MAP STEP=4)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 4 |
| track   | TRACK_DEBUG_SOURCE_MAP |
| started | 2026-07-13 14:00 |
| elapsed | ~5 min |
| done    | Probe `.tmp/line_debug/crash.mlc` (`1/zero`); `MLCC_DEV=1` `-g`; gdb bt frame2 → `crash.mlc:3`. |
| verify  | gdb `info line *$pc` → line 3 of `.tmp/line_debug/crash.mlc`; DWARF `decodedline` has crash.mlc:2–3. |
| result  | STEP=4 done. Plain: `-g` backtrace maps panic to `.mlc` file+line. |
| issues  | — |
| next    | ROLE=Driver STEP=5 TRACK_DEBUG_SOURCE_MAP — docs + verify-gate + close → Critic |

### Turn 2026-07-13 13:59 (Driver TRACK_DEBUG_SOURCE_MAP STEP=3)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 3 |
| track   | TRACK_DEBUG_SOURCE_MAP |
| started | 2026-07-13 13:40 |
| elapsed | ~20 min |
| done    | `sstmt_span`; `line_directive_cpp_statements` + emit in `eval_stmts_cpp_with_try` / `append_block_trailing_cpp` / `gen_return_body_*`. |
| verify  | probe: `#line 2/3 ".tmp/line_probe/probe.mlc"`; self-host p1≡p2 `DIFF_EXIT:0`. |
| result  | STEP=3 done. Plain: mlcc emits `#line` per stmt when span known. |
| issues  | — |
| next    | ROLE=Driver STEP=4 TRACK_DEBUG_SOURCE_MAP — `-g` + abort/panic + gdb/addr2line → `.mlc` |

### Turn 2026-07-13 13:50 (Driver TRACK_DEBUG_SOURCE_MAP STEP=2)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_DEBUG_SOURCE_MAP |
| started | 2026-07-13 13:32 |
| elapsed | ~15 min |
| done    | Ruby `Context#attach_line_directive`; codegen/if/block bypasses; `line_directive_test.rb`. |
| verify  | `bundle exec ruby -Ilib:test test/mlc/line_directive_test.rb` → 2/0; sample cpp has `#line 2/3`. |
| result  | STEP=2 done. Plain: Ruby emits `#line` per stmt when filename known. |
| issues  | — |
| next    | ROLE=Driver STEP=3 TRACK_DEBUG_SOURCE_MAP — self-hosted codegen `#line` emit |

### Turn 2026-07-13 13:45 (Driver TRACK_DEBUG_SOURCE_MAP STEP=1)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_DEBUG_SOURCE_MAP |
| started | 2026-07-13 13:30 |
| elapsed | ~8 min |
| done    | TRACK **Decision (STEP=1)**: Span SoT (ast/SemanticIR/`SourceOrigin`); `#line N "path"`; per-stmt; skip unknown; hooks for Ruby/mlcc. |
| verify  | `rg 'Decision \\(STEP=1\\)' docs/agent/TRACK_DEBUG_SOURCE_MAP.md`; STEP=1 done in table; PLAN §15 STEP=2. |
| result  | STEP=1 done. Plain: `#line` mapping frozen before Ruby emit. |
| issues  | — |
| next    | ROLE=Driver STEP=2 TRACK_DEBUG_SOURCE_MAP — Ruby codegen emit `#line` per statement |

### Turn 2026-07-13 13:40 (Planner DEBUG_SOURCE_MAP plan-refresh)

| field   | value |
|---------|-------|
| role    | Planner |
| step    | plan-refresh |
| track   | TRACK_DEBUG_SOURCE_MAP |
| started | 2026-07-13 13:28 |
| elapsed | ~8 min |
| done    | Activated DEBUG_SOURCE_MAP; STEP=1 sub-steps; PLAN §15 + queue ahead of GUI_CANVAS. |
| verify  | TRACK Status=active; PLAN queue STEP=1; no compiler/ edits. |
| result  | plan-refresh done. Plain: Driver freezes `#line` Decision next. |
| issues  | Left SCRIPT_VM/arkanoid/solid_renderer untracked foreign. |
| next    | ROLE=Driver STEP=1 TRACK_DEBUG_SOURCE_MAP — Decision: `#line` format + granularity |

### Turn 2026-07-13 13:35 (Critic TRACK_TEXT_GLYPH_CACHE_SCALING — critique-audit)

| field   | value |
|---------|-------|
| role    | Critic |
| step    | critique-audit |
| track   | TRACK_TEXT_GLYPH_CACHE_SCALING |
| started | 2026-07-13 13:26 |
| elapsed | ~8 min |
| done    | Audited STEP1–3 vs `c323556f`…`14972c49`; PLAN/archive links; re-ran corpus. |
| verify  | corpus PASS ide 1.14s / dash 0.26s; Map/LayoutCache/script in HEAD. |
| result  | **reopen: none**. Residuals: layout cache demo-only; corpus not in REG; stale TRACK snippet. |
| issues  | Left SCRIPT_VM/arkanoid/solid_renderer untracked foreign. |
| next    | ROLE=Planner STEP=plan-refresh — next queue (DEBUG_SOURCE_MAP / GUI_CANVAS_GRAPH) |

### Turn 2026-07-13 13:30 (Driver TRACK_TEXT_GLYPH_CACHE_SCALING STEP=3)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 3 |
| track   | TRACK_TEXT_GLYPH_CACHE_SCALING |
| started | 2026-07-13 13:20 |
| elapsed | ~12 min |
| done    | Dashboard re-bench 0.29s; `scripts/run_text_gl_perf_corpus.rb` (ide≤3s, dash≤1s); TRACK archived closed. |
| verify  | corpus PASS: ide 0.98s, dashboard 0.21s. |
| result  | STEP=3 done; track **closed**. Plain: wall-time corpus gates both demos. |
| issues  | — |
| next    | ROLE=Critic STEP=critique-audit TRACK_TEXT_GLYPH_CACHE_SCALING |

### Turn 2026-07-13 13:25 (Driver TRACK_TEXT_GLYPH_CACHE_SCALING STEP=2)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_TEXT_GLYPH_CACHE_SCALING |
| started | 2026-07-13 13:16 |
| elapsed | ~10 min |
| done    | `text_ide_panels_demo`: `LayoutCache`/`GlyphRun`; skip reshape on key hit. |
| verify  | ide bench 300f: **0.97s** (&lt;3s). |
| result  | STEP=2 done. Plain: static lines reshape once; status/prompt still miss. |
| issues  | — |
| next    | ROLE=Driver STEP=3 TRACK_TEXT_GLYPH_CACHE_SCALING — dashboard re-bench + perf corpus |

### Turn 2026-07-13 13:20 (Driver TRACK_TEXT_GLYPH_CACHE_SCALING STEP=1)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_TEXT_GLYPH_CACHE_SCALING |
| started | 2026-07-13 13:07 |
| elapsed | ~25 min |
| done    | `GlyphCache` → `Map<i64,GlyphCacheEntry>` + FIFO `order`; packed key; no hit rebuild. `text_renderer.mlc`. Gate revised &lt;16s (reshape residual). |
| verify  | ide bench 300f: 21.65s → **14.40s** (&lt;16s). Public get/insert/new unchanged. |
| result  | STEP=1 done. Plain: HashMap+FIFO; reshape still ~14s → STEP=2. |
| issues  | Original &lt;3s gate unreachable without layout cache; revised in TRACK. |
| next    | ROLE=Driver STEP=2 TRACK_TEXT_GLYPH_CACHE_SCALING — per-line layout cache; bench &lt;3s |

### Turn 2026-07-13 13:04 (Planner TEXT_GLYPH_CACHE_SCALING plan-refresh)

| field   | value |
|---------|-------|
| role    | Planner |
| step    | plan-refresh |
| track   | TRACK_TEXT_GLYPH_CACHE_SCALING |
| started | 2026-07-13 13:04 |
| elapsed | ~6 min |
| done    | Activated GLYPH track; STEP=1 sub-steps; PLAN §32; committed demo fixture. |
| verify  | TRACK Status=active; PLAN queue STEP=1; `text_ide_panels_demo.mlc` tracked. |
| result  | plan-refresh done. Plain: Driver starts HashMap GlyphCache next. |
| issues  | Left SCRIPT_VM/arkanoid/solid_renderer untracked foreign. |
| next    | ROLE=Driver STEP=1 TRACK_TEXT_GLYPH_CACHE_SCALING — HashMap GlyphCache + FIFO |

### Turn 2026-07-13 13:00 (Critic TRACK_PACKAGE_MANAGER — critique-audit)

| field   | value |
|---------|-------|
| role    | Critic |
| step    | critique-audit |
| track   | TRACK_PACKAGE_MANAGER |
| started | 2026-07-13 13:00 |
| elapsed | ~8 min |
| done    | Audited STEP1–10 vs `5e101b04`…`ad0ff1bf`; PLAN/archive links; re-ran unit+E2E. |
| verify  | manifest 9/0; fetcher 3/0; `run_e2e.rb` PASS → 42; all claimed paths in HEAD. |
| result  | **reopen: none**. Residuals: Ruby ModuleGraph no pkg resolve; flat deps only. |
| issues  | Foreign PLAN/GLYPH/SCRIPT_VM leftovers left. |
| next    | ROLE=Planner STEP=plan-refresh — next queue (TEXT_GLYPH_CACHE_SCALING / DEBUG_SOURCE_MAP) |

### Turn 2026-07-13 12:15 (Driver TRACK_PACKAGE_MANAGER STEP=10)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 10 |
| track   | TRACK_PACKAGE_MANAGER |
| started | 2026-07-13 12:15 |
| elapsed | ~45 min |
| done    | Self-host p1≡p2; REG 20/0; examples sweep 117/0/1; TRACK archived closed. |
| verify  | `diff -rq p1 p2 --exclude=obj` empty; `regression_gate.sh` exit 0. |
| result  | STEP=10 done; track **closed**. Plain: package manager verify gate green. |
| issues  | — |
| next    | ROLE=Critic STEP=critique-audit TRACK_PACKAGE_MANAGER |

### Turn 2026-07-13 12:12 (Driver TRACK_PACKAGE_MANAGER STEP=9)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 9 |
| track   | TRACK_PACKAGE_MANAGER |
| started | 2026-07-13 12:12 |
| elapsed | ~5 min |
| done    | PACKAGE_MANAGER.md §2 Usage; README Roadmap one-liner → that doc. |
| verify  | `rg -n '## 2. Usage|mlc_pkg_fetch' docs/PACKAGE_MANAGER.md README.md`; both hit. |
| result  | STEP=9 done. Plain: fetch/import/compile workflow is documented. |
| issues  | — |
| next    | ROLE=Driver STEP=10 TRACK_PACKAGE_MANAGER — verify gate; close → Critic |

### Turn 2026-07-13 12:05 (Driver TRACK_PACKAGE_MANAGER STEP=8)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 8 |
| track   | TRACK_PACKAGE_MANAGER |
| started | 2026-07-13 12:05 |
| elapsed | ~15 min |
| done    | `compiler/tests/e2e/package_manager/run_e2e.rb`; rebuilt mlcc with STEP=7 fix. |
| verify  | `bundle exec ruby compiler/tests/e2e/package_manager/run_e2e.rb` → PASS → 42. |
| result  | STEP=8 done. Plain: fetch+resolve+compile works end-to-end on local git pin. |
| issues  | First mlcc binary lacked manifest-scan fix; rebuilt before E2E green. |
| next    | ROLE=Driver STEP=9 TRACK_PACKAGE_MANAGER — usage docs in PACKAGE_MANAGER.md + README |

### Turn 2026-07-13 11:24 (Driver TRACK_PACKAGE_MANAGER STEP=7)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 7 |
| track   | TRACK_PACKAGE_MANAGER |
| started | 2026-07-13 11:24 |
| elapsed | ~40 min |
| done    | `path_normalize` package resolve + prefix check; probe + test_driver; `/.mlc_packages/` gitignore. |
| verify  | rebuilt `compiler/out/mlcc`; `/tmp/pkg_resolve_probe` → `pkg_resolve_probe ok`. |
| result  | STEP=7 done. Plain: `demo_pkg/math` resolves under `.mlc_packages/` when listed in `mlc.json`. |
| issues  | Ruby `test_parser` parse fail blocks `build_tests_fast` (pre-existing); verify via mlcc probe. |
| next    | ROLE=Driver STEP=8 TRACK_PACKAGE_MANAGER — E2E local git fixture + fetch + compile |

### Turn 2026-07-13 11:13 (Driver TRACK_PACKAGE_MANAGER STEP=6)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 6 |
| track   | TRACK_PACKAGE_MANAGER |
| started | 2026-07-13 11:13 |
| elapsed | ~8 min |
| done    | `Fetcher` + `scripts/mlc_pkg_fetch.rb`; `.gitignore` `.mlc_packages/`; `fetcher_test.rb`. |
| verify  | `bundle exec ruby -Ilib:test test/mlc/package_manager/fetcher_test.rb` → 3 runs, 0 failures. |
| result  | STEP=6 done. Plain: deps clone/checkout into `.mlc_packages/` idempotently. |
| issues  | Foreign PLAN stashed for edit. |
| next    | ROLE=Driver STEP=7 TRACK_PACKAGE_MANAGER — resolve `pkg/path` under `.mlc_packages/` |

### Turn 2026-07-13 11:01 (Driver TRACK_PACKAGE_MANAGER STEP=5)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 5 |
| track   | TRACK_PACKAGE_MANAGER |
| started | 2026-07-13 11:01 |
| elapsed | ~8 min |
| done    | Added `lib/mlc/package_manager/manifest.rb` + `test/mlc/package_manager/manifest_test.rb`. |
| verify  | `bundle exec ruby -Ilib:test test/mlc/package_manager/manifest_test.rb` → 9 runs, 0 failures. |
| result  | STEP=5 done. Plain: Ruby loads/validates `mlc.json` per design freeze. |
| issues  | Foreign PLAN stashed for edit; will restore after commit. |
| next    | ROLE=Driver STEP=6 TRACK_PACKAGE_MANAGER — `scripts/mlc_pkg_fetch.rb` |

### Turn 2026-07-13 10:53 (Driver TRACK_PACKAGE_MANAGER STEP=4)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 4 |
| track   | TRACK_PACKAGE_MANAGER |
| started | 2026-07-13 10:53 |
| elapsed | ~8 min |
| done    | Wrote `docs/PACKAGE_MANAGER.md` freezing STEP=1–3; TRACK/PLAN mark design frozen. |
| verify  | `test -f docs/PACKAGE_MANAGER.md`; cites mlc.json, `.mlc_packages/`, `pkg/path`, no version gate. |
| result  | STEP=4 done. Plain: package-manager design is frozen for Steps 5–10. |
| issues  | Foreign dirty stashed/restored; untracked GLYPH_CACHE/SCRIPT_VM left. |
| next    | ROLE=Driver STEP=5 TRACK_PACKAGE_MANAGER — Ruby `mlc.json` manifest parser |

### Turn 2026-07-13 10:44 (Driver TRACK_PACKAGE_MANAGER STEP=3)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 3 |
| track   | TRACK_PACKAGE_MANAGER |
| started | 2026-07-13 10:44 |
| elapsed | ~10 min |
| done    | Froze version-skew Decision: no compile gate; `mlc_version` deferred; stdlib not a package. |
| verify  | TRACK Decision STEP=3 present; no `docs/PACKAGE_MANAGER.md`; PLAN §18 STEP=3 done. |
| result  | STEP=3 done. Plain: packages have no toolchain version check in v1. |
| issues  | Stashed foreign PLAN/CONTINUITY/… during commit; restored after. Untracked GLYPH_CACHE/SCRIPT_VM left. |
| next    | ROLE=Driver STEP=4 TRACK_PACKAGE_MANAGER — write `docs/PACKAGE_MANAGER.md` |

### Turn 2026-07-13 02:07 (Driver TRACK_PACKAGE_MANAGER STEP=2)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_PACKAGE_MANAGER |
| started | 2026-07-13 02:07 |
| elapsed | ~8 min |
| done    | Froze `.mlc_packages/<pkg>/`, import `pkg/path`, security prefix, flat root deps. |
| verify  | TRACK Decision STEP=2 present; PLAN §18 STEP=2 done. |
| result  | STEP=2 done. Plain: deps resolve via first path segment → vendor dir. |
| issues  | — |
| next    | ROLE=Driver STEP=3 TRACK_PACKAGE_MANAGER — version skew limitation |

### Turn 2026-07-13 02:02 (Driver TRACK_PACKAGE_MANAGER STEP=1)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_PACKAGE_MANAGER |
| started | 2026-07-13 02:02 |
| elapsed | ~8 min |
| done    | Froze Decision: `mlc.json` + `{git,rev}` SHA; cited path_normalize/module_loader; rejected alts table. |
| verify  | TRACK Decision present; no `docs/PACKAGE_MANAGER.md`; PLAN §18 STEP=1 done. |
| result  | STEP=1 done. Plain: deps model is git+SHA pins in `mlc.json`. |
| issues  | — |
| next    | ROLE=Driver STEP=2 TRACK_PACKAGE_MANAGER — `.mlc_packages/` + import form |

### Turn 2026-07-13 01:59 (Planner TRACK_PACKAGE_MANAGER plan-refresh)

| field   | value |
|---------|-------|
| role    | Planner |
| step    | plan-refresh |
| track   | TRACK_PACKAGE_MANAGER |
| started | 2026-07-13 01:59 |
| elapsed | ~5 min |
| done    | Activated PACKAGE_MANAGER; STEP=1 sub-steps + `mlc.json` candidate; PLAN/DEVELOPMENT synced. |
| verify  | TRACK status active; PLAN §18 active; queue points at STEP=1. |
| result  | plan-refresh done. Plain: package-manager design starts at STEP=1. |
| issues  | Foreign `text_ide_panels_demo.mlc` / lint leftovers left. |
| next    | ROLE=Driver STEP=1 TRACK_PACKAGE_MANAGER — freeze deps model Decision |

### Turn 2026-07-13 01:55 (Critic TRACK_TEXT_SHIM_TO_MLC — critique-audit)

| field   | value |
|---------|-------|
| role    | Critic |
| step    | critique-audit |
| track   | TRACK_TEXT_SHIM_TO_MLC |
| started | 2026-07-13 01:52 |
| elapsed | ~10 min |
| done    | Audited STEP1–10 vs `75263977`…`21b33afb`; PLAN/FFI/archive; re-ran vs_shim+field. |
| verify  | `text_shaping_vs_shim_ok`; `gui_text_field_demo` exit 0; no CachedFontFace; demos on text_shaping. |
| result  | **reopen: none**. Residuals: deprecated shim; bench wall vs GL_PERF-after. |
| issues  | Foreign `text_ide_panels_demo.mlc` / lint leftovers left. |
| next    | ROLE=Planner STEP=plan-refresh — activate TRACK_PACKAGE_MANAGER STEP=1 |

### Turn 2026-07-13 01:44 (Driver TRACK_TEXT_SHIM_TO_MLC STEP=10 — close)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 10 |
| track   | TRACK_TEXT_SHIM_TO_MLC |
| started | 2026-07-13 01:44 |
| elapsed | ~15 min |
| done    | Bench 300f User 0.33s (~27× vs 8.97s); a8 Hxpjy smoke 0; track archived. |
| verify  | `/usr/bin/time -v` bench exit 0; `text_renderer_a8_string_smoke` exit 0. |
| result  | STEP=10 done; track **closed**. Plain: MLC text_shaping path measured+archived. |
| issues  | Foreign `text_ide_panels_demo.mlc` + lint leftovers left. |
| next    | ROLE=Critic STEP=critique-audit TRACK_TEXT_SHIM_TO_MLC |

### Turn 2026-07-13 01:40 (Driver TRACK_TEXT_SHIM_TO_MLC STEP=9)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 9 |
| track   | TRACK_TEXT_SHIM_TO_MLC |
| started | 2026-07-13 00:50 |
| elapsed | ~50 min |
| done    | Docs TEXT_RENDERING §3.1/§5.1/§9.1 + FFI_LAYER §8; SESSION archive trim; self-host+REG. |
| verify  | `diff -rq p1 p2 --exclude=obj` empty; REG 20/0; sweep ok=115 fail=0 skip=1. |
| result  | STEP=9 done. Plain: docs+gates green for shim→MLC. |
| issues  | Archived REGION/GL_PERF turns → SESSION_HISTORY. |
| next    | ROLE=Driver STEP=10 TRACK_TEXT_SHIM_TO_MLC — CPU%/visual; close → Critic |

### Turn 2026-07-13 00:47 (Driver TRACK_TEXT_SHIM_TO_MLC STEP=8)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 8 |
| track   | TRACK_TEXT_SHIM_TO_MLC |
| started | 2026-07-13 00:40 |
| elapsed | ~15 min |
| done    | Shim cache removed; `freetype_shim`/`harfbuzz_shim` thin wrappers over abi. |
| verify  | `text_shaping_vs_shim_ok`; freetype/harfbuzz smokes exit 0. |
| result  | STEP=8 done. Plain: no C++ path/size face cache; MLC owns bookkeeping. |
| issues  | — |
| next    | ROLE=Driver STEP=9 TRACK_TEXT_SHIM_TO_MLC — self-host + REG + docs |

### Turn 2026-07-13 00:37 (Driver TRACK_TEXT_SHIM_TO_MLC STEP=7)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 7 |
| track   | TRACK_TEXT_SHIM_TO_MLC |
| started | 2026-07-13 00:33 |
| elapsed | ~15 min |
| done    | Dirty-flag atlas: field demo persistent cache + skip rebuild/upload; dashboard `lines_key` skip. |
| verify  | `gui_text_field_demo` exit 0 (rebuild/upload < frames); dashboard `--check-only` 0. |
| result  | STEP=7 done. Plain: unchanged lines skip atlas rebuild/upload. |
| issues  | — |
| next    | ROLE=Driver STEP=8 TRACK_TEXT_SHIM_TO_MLC — deprecate shim cache helpers |

### Turn 2026-07-13 00:11 (Driver TRACK_TEXT_SHIM_TO_MLC STEP=6)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 6 |
| track   | TRACK_TEXT_SHIM_TO_MLC |
| started | 2026-07-13 00:11 |
| elapsed | ~25 min |
| done    | Switched dashboard/field/window A8 to `text_shaping`; `ft_glyph_a8_data` + blit prefer abi. |
| verify  | `gui_text_field_demo` exit 0; `text_shaping_vs_shim_ok`; dashboard/window check-only 0. |
| result  | STEP=6 done. Plain: live demos use MLC face/font cache. |
| issues  | shim smokes (`freetype_glyph_smoke`/`harfbuzz_shape_smoke`) kept for STEP=8 |
| next    | ROLE=Driver STEP=7 TRACK_TEXT_SHIM_TO_MLC — dirty-flag atlas skip |

### Turn 2026-07-13 00:04 (Driver TRACK_TEXT_SHIM_TO_MLC STEP=5)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 5 |
| track   | TRACK_TEXT_SHIM_TO_MLC |
| started | 2026-07-13 00:04 |
| elapsed | ~10 min |
| done    | `text_shaping_vs_shim_gate.mlc` — shape+raster byte-exact vs legacy shim. |
| verify  | run prints `text_shaping_vs_shim_ok` exit 0 (`Hi`/`Привет`/`A`). |
| result  | STEP=5 done. Plain: MLC path matches shim output. |
| issues  | none |
| next    | ROLE=Driver STEP=6 TRACK_TEXT_SHIM_TO_MLC — switch demos to text_shaping |

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
