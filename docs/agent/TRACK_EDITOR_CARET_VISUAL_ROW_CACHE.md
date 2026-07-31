# Track: Editor caret visual-row cache (§101)

Parent: [../PLAN.md](../PLAN.md) §101. §97a residual, named by the user
2026-07-28 override (d): "§101 (§97a residual, confirmed root cause —
uncached O(caret_line) walk in `visual_row_index_for_caret_pixel_budget`)
is next, PRIORITY over further stringify."

## Status: **open** — Decision frozen 2026-07-31, deferred behind §105 (see below)

## Important correction before Decision

Investigating this track's own "confirmed root cause" claim (PLAN.md §101
row) by direct instrumentation of `scripts/run_editor_demo_live_perf_smoke.sh`
found the claim's supporting numbers do not hold: on that smoke fixture,
`caret_line=0` for all 30 frames (fresh file open, no session, no
scroll/caret-move injected) and the caret walk costs 74–330 µs/frame — not
"essentially all of the measured `draw_us`". The actual dominant cost on
that fixture is a **different** function
(`editor_ux_max_line_columns_from_index`, horizontal-scrollbar content
width), fixed as new §105 —
[TRACK_EDITOR_HORIZONTAL_SCROLLBAR_MAX_COLUMNS_CACHE](TRACK_EDITOR_HORIZONTAL_SCROLLBAR_MAX_COLUMNS_CACHE.md).
Full evidence and numbers are in that track's Decision.

The bug named by this track (§101) is still real — an uncached
O(caret_line) walk is a genuine architecture defect that will show up once
a user has a large file open with real per-line content and the caret is
deep into it (the existing smoke fixture just happens to keep the caret at
line 0 throughout, so it does not exercise this path). Given §105 already
delivered the dominant, measured win, this track proceeds next as a
correctness/architecture cleanup rather than the emergency perf fix its
original framing implied.

## Decision (STEP=0) — frozen 2026-07-31

| Item | Choice |
|------|--------|
| Problem | `visual_row_index_for_caret_pixel_budget` (`misc/editor/layout/word_wrap.mlc:422`), called once per caret every frame from `demo_live.mlc` (~line 2212), walks `line = 0 .. caret_line` calling `visual_rows_for_line_pixel_budget` (a font-glyph-measuring wrap computation, more expensive per line than the §105 bug's plain UTF-8 column count) for every line before the caret, uncached, every frame — regardless of whether text, wrap params, or caret line changed since the previous frame. Same disease class as §36/§105, unmeasured impact today because the standing perf fixture keeps caret at line 0, but real for any file where the user scrolls/navigates deep with the caret |
| Strategy (v1) | Extend `EditorFrameLayout`/`DocumentWrapCountCache` (§97b, `layout/wrap_cache.mlc`) with a per-line cumulative visual-row **prefix table** (`prefix_visual_rows: [i32]`, length `line_count + 1`), computed inside the same single O(n) pass `document_visual_row_count_pixel_budget` already performs when the wrap cache is (re)computed — no added asymptotic cost there, since that pass already walks every line once per text/wrap/viewport/font change. New `visual_row_index_for_caret_pixel_budget_cached` in `word_wrap.mlc` takes the prefix array (not the whole cache struct, to keep `word_wrap.mlc` cache-agnostic per existing separation of concerns) plus `caret_line`/`caret_column`, looks up `prefix_visual_rows[caret_line]` in O(1) instead of re-walking, then does the same single-line row scan the current function already does for the caret's own line (unavoidable — needed for `caret_column` placement regardless). Must handle the `skip_full_pixel_wrap_now` (perf-mode) case: when that shortcut is active, `wrap_count_cache` is deliberately NOT ticked with real pixel-wrap data (avoids an O(n) HarfBuzz pass on huge files) — the cached prefix table would be stale/absent in that mode, so the cached lookup function must fall back to the existing uncached walk when the prefix table does not cover `caret_line` (defensive: `prefix_visual_rows.length() <= caret_line`), matching wrap_count_cache's own graceful-fallback shape rather than crashing or silently reading a stale value |
| Primary gate | Green: new L1 scenario (analogous to `wrap_count_cache_stable`) proving the prefix table is text/wrap-column keyed, idle ticks do not recompute, and cached caret-row values match the uncached function for the same input across several caret positions including line 0, mid-file, and last line; `run_ux_gate.sh` ×2 clean; `demo_live.mlc` still compiles and the perf smoke (`draw_us`) does not regress from §105's post-fix baseline |
| Module touch | `layout/wrap_cache.mlc` (prefix table field + recompute), `layout/word_wrap.mlc` (new cached lookup fn, existing fn untouched for the fallback path and for the 1 remaining test-only column-budget caller), `demo_live.mlc` (1 call site), new `ux_scenarios/caret_visual_row_cache_stable.mlc` + gate script |
| REG | no |
| Out of scope | The column-budget (non-pixel) `visual_row_index_for_caret` — only used by `ux_scenarios/caret_after_word_wrap.mlc`, not the live loop, confirmed by grep; not perf-critical, not touched |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + correction (moved dominant-bug fix to §105) | **done** |
| 1 | Red: no cached lookup / prefix table exists | next |
| 2 | Green: prefix table + cached lookup wired into `demo_live.mlc`, scenario green | — |
| 3 | Critic | — |
