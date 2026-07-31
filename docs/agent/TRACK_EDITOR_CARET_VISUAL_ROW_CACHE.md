# Track: Editor caret visual-row cache (§101)

Parent: [../PLAN.md](../PLAN.md) §101. §97a residual, named by the user
2026-07-28 override (d): "§101 (§97a residual, confirmed root cause —
uncached O(caret_line) walk in `visual_row_index_for_caret_pixel_budget`)
is next, PRIORITY over further stringify."

## Status: **CLOSED** 2026-07-31 — STEP=0-3 all done, Critic-audited

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
| 1 | Red: confirmed no `prefix_visual_rows` field / cached lookup existed anywhere in `misc/editor/**` before this step (`grep -rn "prefix_visual_rows" misc/editor` — zero matches) | **done** |
| 2 | Green: prefix table + cached lookup wired into `demo_live.mlc`, scenario green | **done** |
| 3 | Critic | **done** |

## Critic audit — 2026-07-31

Independent from the Driver's own numbers above, none of the Driver's build/output directories reused:

- `git show --stat 5399ac90`: exactly the 10 files the Decision's Module-touch row + new-scenario/script lines list — `word_wrap.mlc`, `wrap_cache.mlc`, `demo_live.mlc`, the new scenario + runner script, and the 5 doc files. No stray file.
- Read the `wrap_cache.mlc` diff directly: confirmed `wrap_count_cache_new_pixel`/`wrap_count_cache_tick_pixel` **replace** (not add alongside) their previous single call to `document_visual_row_count_pixel_budget` with one call to `document_visual_row_prefix_pixel_budget`, deriving `visual_row_count` from the prefix array's last element — exactly one O(n) walk per constructor/tick, confirming the "no added asymptotic cost" claim rather than trusting the prose.
- Read the `word_wrap.mlc` diff directly: `visual_row_index_for_caret_pixel_budget_cached`'s tail (single-line row scan) is byte-identical to the original uncached function's own tail, modulo `visual` being sourced from `prefix_visual_rows[caret_line]` instead of a walk-accumulated sum — confirms zero behavior drift beyond the intended optimization.
- Independent rerun of `scripts/run_ux_caret_visual_row_cache_stable.sh` in a separate output directory (`UX_CARET_VISUAL_ROW_CACHE_STABLE_OUT=/tmp/critic_101_scenario`): `ux_ok caret_visual_row_cache_stable`, exit 0.
- Independent rerun of `scripts/run_editor_demo_live_perf_smoke.sh` in a separate output directory (`EDITOR_DEMO_LIVE_PERF_OUT=/tmp/critic_101_perf`): `frames=30 layout_us=447786 draw_us=91929 total_us=614481` — same order of magnitude as the Driver's `draw_us=77818`/§105's baseline `draw_us=84844`/`78062`, confirms no regression (this fixture keeps caret at line 0 throughout, so §101's own fix is not exercised by it, matching the track's own framing — not a gap in this audit, an intrinsic property of the standing fixture).
- `scripts/run_ux_gate.sh` run 1 (independent, own log): `[ux gate] all ok (114 scenarios)`, 0 failures.
- `scripts/run_ux_gate.sh` run 2 (stability ×2): `[ux gate] all ok (114 scenarios)`, 0 failures.
- Confirmed non-track WIP (`CLAUDE.md`/`README.md`/`capture_analyzer.rb`/`docs/reddit-*`/`.vscode/`) still present and uncommitted after this audit.

No false-done found. All Driver claims independently reproduced.

## Steps 1-2 (red/green) — done 2026-07-31

Red: `grep -rn "prefix_visual_rows" misc/editor` returned zero matches before
this step — no cached caret-row lookup existed.

Green:

- `layout/word_wrap.mlc`: new `document_visual_row_prefix_pixel_budget`
  (same O(n) walk as `document_visual_row_count_pixel_budget`, returns the
  per-line cumulative prefix array instead of just the final total) and new
  `visual_row_index_for_caret_pixel_budget_cached` (O(1) prefix lookup +
  single-line scan; falls back to the existing uncached
  `visual_row_index_for_caret_pixel_budget` when
  `prefix_visual_rows.length() <= caret_line`).
- `layout/wrap_cache.mlc`: new `prefix_visual_rows: [i32]` field on
  `DocumentWrapCountCache`. `wrap_count_cache_new_pixel`/
  `wrap_count_cache_tick_pixel` compute the prefix array **once** via
  `document_visual_row_prefix_pixel_budget` and derive `visual_row_count`
  from its last element — no added asymptotic cost, matches the Decision.
  Column-budget (non-pixel) `wrap_count_cache_new`/`_tick` set
  `prefix_visual_rows: []` (out of scope, unused by the live loop, confirmed
  by grep).
- `demo_live.mlc`: the 1 live call site (inside the per-caret loop, ~line
  2214) now calls `visual_row_index_for_caret_pixel_budget_cached` with
  `frame_layout.wrap_count_cache.prefix_visual_rows`; direct import of the
  uncached function removed.
- New L2 scenario `ux_scenarios/caret_visual_row_cache_stable.mlc` +
  `scripts/run_ux_caret_visual_row_cache_stable.sh`: builds a 40-line pixel-
  budget fixture, compares the cached lookup against the uncached walk at
  caret line 0, mid-file (20), and the last line (40, the trailing empty
  line after 40 `\n`s); separately checks the fallback path with an empty
  `prefix_visual_rows` array; checks 5 idle ticks do not bump
  `recompute_count`; checks a text edit does bump it and the prefix table
  length grows by 1. Ran standalone: `ux_ok caret_visual_row_cache_stable`,
  exit 0.
- Perf smoke re-run (regression check against §105's post-fix baseline):
  `frames=30 layout_us=340918 draw_us=77818 total_us=485982` — same order
  of magnitude as §105's `draw_us=84844 total_us=532723`/Critic's
  `draw_us=78062 total_us=501532`, no regression (caret stays at line 0 on
  this fixture, so this step's own win is not visible here — expected,
  matches the Decision's own framing).
- `scripts/run_ux_gate.sh` run twice from a clean log: `[ux gate] all ok
  (114 scenarios)` both times, 0 failures (114 = the standing 113 + the new
  `caret_visual_row_cache_stable` scenario).
