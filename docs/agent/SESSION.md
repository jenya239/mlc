# Agent session log

## Session 2026-05-22

| Field | Value |
|-------|-------|
| instructions_rev | `2026-07-12-multi-track-order` |
| agent_token_last | — |
| driver_turns_since_plan | 5 |
| step_last | 5 |
| active_track | TRACK_GUI_SCENE_PHASE_A |
| test_gate | STEP=6: gui_button on scene migration smoke |

### Turn 2026-07-13 15:36 (Driver TRACK_GUI_SCENE_PHASE_A STEP=5)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 5 |
| track   | TRACK_GUI_SCENE_PHASE_A |
| started | 2026-07-13 15:32 |
| elapsed | ~10 min |
| done    | `scene_draw.mlc` + adopt `solid_renderer.mlc`; `scene_batch_smoke` (12 verts + GL flush). |
| verify  | `MLC_GLFW_VISIBLE=0` smoke exit 0. |
| result  | STEP=5 done. Plain: one solid batch for all RectFill nodes. |
| issues  | solid_renderer was foreign untracked — adopted for STEP=5 |
| next    | ROLE=Driver STEP=6 TRACK_GUI_SCENE_PHASE_A — gui_button scene migration smoke |

### Turn 2026-07-13 15:30 (Driver TRACK_GUI_SCENE_PHASE_A STEP=4)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 4 |
| track   | TRACK_GUI_SCENE_PHASE_A |
| started | 2026-07-13 15:28 |
| elapsed | ~5 min |
| done    | `scene_hit_test` + `scene_hit_smoke.mlc` (gui_button 40/40/160×48; nested; front wins). |
| verify  | mlcc+build_bin; smoke exit 0. |
| result  | STEP=4 done. Plain: screen→local hit via inverse world. |
| issues  | — |
| next    | ROLE=Driver STEP=5 TRACK_GUI_SCENE_PHASE_A — batched rect-fill draw |

### Turn 2026-07-13 15:27 (Driver TRACK_GUI_SCENE_PHASE_A STEP=3)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 3 |
| track   | TRACK_GUI_SCENE_PHASE_A |
| started | 2026-07-13 15:25 |
| elapsed | ~5 min |
| done    | `scene_update_world` (topo `.set` reuse); smoke nested (100,200)+(10,20)→(110,220). |
| verify  | mlcc+build_bin; `scene_types_smoke` exit 0. |
| result  | STEP=3 done. Plain: world = parent∘local in index order. |
| issues  | — |
| next    | ROLE=Driver STEP=4 TRACK_GUI_SCENE_PHASE_A — hit-test via inverse world |

### Turn 2026-07-13 15:22 (Driver TRACK_GUI_SCENE_PHASE_A STEP=2)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 2 |
| track   | TRACK_GUI_SCENE_PHASE_A |
| started | 2026-07-13 15:08 |
| elapsed | ~15 min |
| done    | `misc/gui/scene.mlc` + `scene_types_smoke.mlc`; Decision precision → `f64`; SESSION archive trim. |
| verify  | mlcc+build_bin; smoke exit 0 (tree + multiply/invert). |
| result  | STEP=2 done. Plain: flat Scene+Affine2x3 constructors green. |
| issues  | mlcc f32 arith broken → f64 amendment |
| next    | ROLE=Driver STEP=3 TRACK_GUI_SCENE_PHASE_A — world-transform pass |

### Turn 2026-07-13 15:01 (Driver TRACK_GUI_SCENE_PHASE_A STEP=1)

| field   | value |
|---------|-------|
| role    | Driver |
| step    | 1 |
| track   | TRACK_GUI_SCENE_PHASE_A |
| started | 2026-07-13 15:00 |
| elapsed | ~5 min |
| done    | TRACK **Decision (STEP=1)**: flat `nodes`+`parent`/`children`; `Affine2x3` Flash map; camera-relative f32 GPU. |
| verify  | `rg 'Decision \\(STEP=1\\)' docs/agent/TRACK_GUI_SCENE_PHASE_A.md`; STEP=1 done; PLAN §29 STEP=2. |
| result  | STEP=1 done. Plain: foundation shape frozen before `scene.mlc`. |
| issues  | — |
| next    | ROLE=Driver STEP=2 TRACK_GUI_SCENE_PHASE_A — `misc/gui/scene.mlc` types + constructors |

### Turn 2026-07-13 14:55 (Planner GUI_SCENE_PHASE_A plan-refresh)

| field   | value |
|---------|-------|
| role    | Planner |
| step    | plan-refresh |
| track   | TRACK_GUI_SCENE_PHASE_A |
| started | 2026-07-13 14:55 |
| elapsed | ~5 min |
| done    | Created `TRACK_GUI_SCENE_PHASE_A.md`; epic → Phase A; PLAN §10c/§29 + queue; DEVELOPMENT. |
| verify  | Status=active; STEP=1 pending + sub-steps; no compiler/lib edits. |
| result  | plan-refresh done. Plain: Driver freezes tree/matrix/precision Decision. |
| issues  | Foreign SCRIPT_VM/arkanoid/solid_renderer left. |
| next    | ROLE=Driver STEP=1 TRACK_GUI_SCENE_PHASE_A — Decision: tree / Affine2×3 / camera-relative |

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

