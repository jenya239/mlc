# Track: Editor horizontal-scrollbar max-line-columns cache (§105)

Parent: [../PLAN.md](../PLAN.md) §105. Found while investigating §101
(`TRACK_EDITOR_CARET_VISUAL_ROW_CACHE.md`) — empirical instrumentation of
`scripts/run_editor_demo_live_perf_smoke.sh` showed the caret walk PLAN.md
§101 named as "confirmed root cause" of the 100k-line perf smoke's huge
`draw_us` is **not** the actual bottleneck in that measured scenario. The
real bottleneck is a separate, previously-undiagnosed bug of the same
disease class.

## Status: **CLOSED** 2026-07-31 — STEP=0-3 all done, Critic-audited

## Correction to PLAN.md §101 (2026-07-28 entry)

PLAN.md line 499 (§101) claimed, "confirmed by code read": walking
`visual_row_index_for_caret_pixel_budget`'s `line = 0 .. caret_line` loop
"on the 100k-line perf fixture with caret near end-of-file … account[s] for
essentially all of the measured `draw_us=12063257`". This was **not
verified by running anything** — "confirmed by code read" meant only that
the loop exists in the source, not that it dominates the measured number.

Direct instrumentation (temporary debug timers added to `demo_live.mlc`,
run through the real `scripts/run_editor_demo_live_perf_smoke.sh`, then
fully reverted before any commit — see Decision below for exact numbers)
shows:

- `caret_line=0` for all 30 frames of the smoke run (fresh file open, no
  saved session, script injects no scroll/caret movement) — the caret walk
  executes **zero** iterations of the expensive per-line call.
- The caret computation's own wall-clock cost is 74–330 µs/frame — not
  "essentially all" of anything.
- The actual dominant cost, isolated with its own timer, is
  `editor_ux_max_line_columns_from_index` (`ux/overflow.mlc:44`, called
  unconditionally every frame from `demo_live.mlc` for the horizontal
  scrollbar): 249370–320719 µs/frame, i.e. **>95% of `draw_us`**.

The §101 caret bug is still real (uncached O(caret_line) walk, will matter
once caret is genuinely deep in a large file with substantial per-line
content) and stays queued — see
[TRACK_EDITOR_CARET_VISUAL_ROW_CACHE](TRACK_EDITOR_CARET_VISUAL_ROW_CACHE.md).
It is just not what the existing smoke fixture measures. This track (§105)
fixes the bug that the smoke fixture's number actually reflects.

## Decision (STEP=0) — frozen 2026-07-31

| Item | Choice |
|------|--------|
| Problem | `editor_ux_max_line_columns_from_index` (`misc/editor/ux/overflow.mlc:44`) walks every document line (`line_index_line_count`, 0..count) calling `line_display_columns` → `utf8_count_display_columns` (full UTF-8 decode of each line) — an O(document size) full rescan. It is called **unconditionally, every draw frame**, from `demo_live.mlc` at the horizontal-scrollbar content-width site (was line ~2280), plus 3 more event-driven call sites (shift+wheel horizontal scroll, scrollbar-track click, scrollbar drag-move) — none cached, 4 independent call sites computing the same value from scratch. Same disease class as the already-fixed §36 and the still-open §101, different call site, this one empirically dominant on the existing 100k-line perf fixture: baseline `frames=30 layout_us=59041 draw_us=8927341 total_us=9064023` (fresh measurement, same order of magnitude as PLAN.md's older number from a different machine/run) |
| Strategy (v1) | Cache the scalar result. Unlike `wrap_count_cache` (`layout/wrap_cache.mlc`), this value depends only on **text** (not viewport/font/pixel size — it is a column count, pixel width is `columns * char_width` computed by the caller), so the invalidation key is the same simple text-equality check `wrap_count_cache_needs_recompute` already uses, just without the wrap/viewport/font fields. New `MaxLineColumnsCache { text, max_columns, recompute_count }` + `max_line_columns_cache_new`/`_tick` added directly in `ux/overflow.mlc` (co-located with the function it wraps, avoids a `layout/` → `ux/` dependency that would not otherwise exist). Wired into the single existing per-frame cache home, `EditorFrameLayout` (`app/frame_layout.mlc`, §97b) as a new `max_columns_cache` field + `frame_layout_tick_max_columns`, ticked once per frame in `demo_live.mlc`'s layout phase (same place as the existing wrap tick) — **not** gated by `skip_full_pixel_wrap_now` (unlike the wrap cache): this value is a cheap arithmetic-only scan (no HarfBuzz/font shaping), so paying it once on the first real-text frame and then amortizing to zero for unchanged text is strictly better than skipping it outright, which would leave the horizontal scrollbar permanently reporting `max_columns=0`. All 4 call sites in `demo_live.mlc` route through `frame_layout.max_columns_cache.max_columns` instead of calling the function directly — no second, differently-invalidated cache next to the frame-layout one, per §97b's own architecture rule |
| Primary gate | Red: isolated timer around the call site shows it dominates `draw_us` (done, see numbers above — this stands in for a red harness since the existing perf smoke already reproduces the regression, a red-specific script would just re-derive the same aggregate number). Green: `scripts/run_editor_demo_live_perf_smoke.sh` `draw_us` drops by ≥1 order of magnitude on the 100k-line fixture; new L1 scenario `max_line_columns_cache_stable` (idle ticks do not recompute, text edit does, value matches the uncached scan); `run_ux_gate.sh` ×2 clean; Critic re-verifies independently |
| Module touch | `misc/editor/ux/overflow.mlc` (new cache type + 2 functions), `misc/editor/app/frame_layout.mlc` (new field + tick fn, all 6 existing constructor sites updated), `misc/editor/demo_live.mlc` (1 tick call + 4 call sites routed through the cache, 1 now-unused direct import removed), new `misc/editor/ux_scenarios/max_line_columns_cache_stable.mlc` + `scripts/run_ux_max_line_columns_cache_stable.sh` |
| REG | no (`compiler/**` untouched) |
| Out of scope | §101's own caret cache (separate track); changing `utf8_count_display_columns`'s own algorithm; horizontal-scrollbar visual behavior |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + root-cause correction to PLAN.md §101 | **done** |
| 1 | Red: isolated per-call timer proves `editor_ux_max_line_columns_from_index` is >95% of `draw_us` on the 100k-line smoke fixture (249370–320719 µs/frame vs an 8.7–8.9M µs/30-frame total); caret walk isolated separately and shown negligible (74–330 µs/frame, `caret_line=0` throughout) | **done** |
| 2 | Green: `MaxLineColumnsCache` + `frame_layout_tick_max_columns`; all 4 `demo_live.mlc` call sites routed through `frame_layout.max_columns_cache`; new scenario `max_line_columns_cache_stable` green; perf smoke re-measured: `frames=30 layout_us=375140 draw_us=84844 total_us=532723` (draw_us **105× lower**, total_us **17× lower**) | **done** |
| 3 | Critic: independent rebuild + re-measure + `run_ux_gate.sh` ×2 + related scenarios | **done** |

## Critic audit — 2026-07-31

Independent from the Driver's own numbers above:

- `scripts/run_ux_gate.sh` run 1: `[ux gate] all ok (113 scenarios)`, 0 failures — includes the new `max_line_columns_cache_stable` scenario.
- `scripts/run_ux_gate.sh` run 2 (stability ×2, warm object cache): `[ux gate] all ok (113 scenarios)`, 0 failures.
- Independent rebuild + re-measure of `scripts/run_editor_demo_live_perf_smoke.sh` (own `EDITOR_DEMO_LIVE_PERF_OUT`, not reusing the Driver's build dir): `frames=30 layout_us=355593 draw_us=78062 total_us=501532` — consistent with the Driver's `draw_us=84844 total_us=532723` (same order of magnitude, both ~100× and ~17× below the pre-fix baseline `draw_us=8927341 total_us=9064023`). Confirms the fix, no regression, no false-done.

<!-- STEP=1/2: temporary debug `println` timers added to demo_live.mlc to bisect draw_us (cp_a/cp_b/cp_c checkpoints + isolated hscroll_us/caret_loop_us/trailing_ws_us timers), fully reverted (git checkout) before any commit — not part of the shipped diff. Fixture: scripts/run_editor_demo_live_perf_smoke.sh's own 100k-line/30-frame run (MLC_EDITOR_PERF=1). -->
