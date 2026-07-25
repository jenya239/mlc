# Track: Editor render architecture — execute the 2026-07-15 review's P0 backlog

Parent: [../PLAN.md](../PLAN.md) §97. User directive (2026-07-25): "тормозит всё
адски, скроллы нормально не работают и это не из-за фоновых процессов. нужен
системный подход к быстрому рендерингу, скроллам и т. п. Максимально сильная,
тестируемая архитектура. clean architecture на максималках."

## Status: **open** — §97b shared_span_cache Decision **done**; next Driver STEP=1 (red)

## Why this track exists (root cause, not a new finding)

[`mlc-support/responses/gui_editor_architecture_20260715_105621.md`](../../../mlc-support/responses/gui_editor_architecture_20260715_105621.md)
(2026-07-15 architecture review, executive verdict) already diagnosed this
exact disease ten days before this track: `demo_live.mlc` is a god-loop that
duplicates `ux/*` logic instead of reusing it, with no single source of
truth for frame state. Its P0 roadmap items **#6 `EDITOR_FRAME_SPLIT`**, **#7
`EDITOR_UX_PROBE_FROM_LIVE_STATE`**, **#10 `EDITOR_REAL_PERF_HARNESS`** were
never opened as tracks. Instead, `PLAN.md` §48-§96 (~50 tracks) spent ten
days patching individual symptoms in the same file (wrap caching, minimap
clipping, idle CPU, hover tint, drag-selection, scrollbar-on-hover, stringify
sweeps) one at a time — each real and correctly fixed in isolation, but the
pattern of "one narrow fix → next narrow symptom in the same 2684-line file"
is exactly what an unaddressed architecture gap looks like.

Confirmed 2026-07-25 (`grep -c` on `demo_live.mlc`): 2684 lines; `frame_snapshot_cache_tick`
called from **25 separate scattered sites** (every edit-handling branch
threads it by hand — miss one and that branch silently paints a stale
frame); 4 more independently-threaded ad-hoc caches
(`wrap_count_cache`/`shared_span_cache`/`minimap_cache`/`frame_cache`), no
single per-frame "what changed, what needs recompute" model. This is the
mechanism behind the class of bug in the (now superseded) §94/§95/§96: model
tests pass against an isolated `EditorUxState` fixture while the live loop's
own separate locals can silently diverge from it.

## Non-goals (from the review's own §7, still binding)

- **No big-bang rewrite.** Extract one phase at a time behind a pixel/L2
  regression gate (`run_ux_gate.sh` ×2, per `GUI_UX_TESTING.md` standing
  discipline point 5), not a single giant `demo_live.mlc` rewrite commit.
- No second UI toolkit, no migrating chrome onto `SceneNode` (that is the
  review's item #8, a separate explicit Decision, out of scope here unless
  a later sub-track deliberately opens it).
- No `mlc-script`/VM involvement — unrelated.

## Sub-tracks, in order

### §97a `EDITOR_REAL_PERF_HARNESS` (do first — measure before refactoring)

#### Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | Existing `MLC_EDITOR_PERF` / `scripts/run_editor_perf_smoke.sh` targets dead `misc/editor/main.mlc` and asserts **zero** `layout_us`/`shaping_us`/`draw_us`. `demo_live.mlc` (the real editor) has no wall-clock frame instrumentation and no headless 100k-line scroll timing smoke — so "scroll lags" stays unmeasured guesswork before any §97b/§97c refactor |
| Strategy (v1) | (1) Wire real timers into **`demo_live.mlc`** under `MLC_EDITOR_PERF=1`: wall clock via existing `Profile` / `mlc::profile::monotonic_nanos` (same runtime as `Profile.scope_*` already used in `frame_snapshot_cache.mlc`); accumulate per-frame µs for at least **layout/wrap**, **syntax/highlight** (if on hot path), **draw**, plus **total frame**. Reuse/extend `ui/perf.mlc` counters so printed line is non-zero on a real frame. (2) Headless smoke script (new `scripts/run_editor_demo_live_perf_smoke.sh`): `MLC_GLFW_VISIBLE=0`, open/create **≥100k-line** fixture, drive **N≥30** synthetic scroll frames via `glfw_gl_input_test_*`, print aggregated ms/frame (or µs) line, assert `frames≥N` and **at least one phase / total_us > 0**. Leave old `main.mlc` stub smoke as-is (historical). Out of scope for §97a: extracting modules (§97b), unifying `EditorUxState` (§97c), changing cache algorithms |
| Primary gate | Red: no green demo_live perf smoke (stub-only or missing). Green: `ux_ok` / `[mlc-editor] demo_live_perf …` with real non-zero timings on 100k-line scroll; `demo_live` compile stays green |
| Module touch | `demo_live.mlc`; `ui/perf.mlc` (extend); new smoke script; optional tiny fixture generator under `misc/editor/` or `tmp/` |
| REG | no |
| Out of scope | §97b frame split; §97c unified live state; cache algorithm changes; SCRIPT_VM; MIR Epic 5 |

#### Steps (§97a)

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** |
| 1 | Red: demo_live still has no real perf smoke / zero-only path | **done** (`run_editor_demo_live_perf_red.sh`) |
| 2 | Wire timers + 100k scroll smoke; green | **done** |
| 3 | Critic: stable×2 + related + `run_ux_gate` | **done** (Critic OK 2026-07-25) |

<!-- STEP=1: red asserts demo_live lacks MLC_EDITOR_PERF real timings; stable stub; old main.mlc stub remains zeros -->
<!-- STEP=2: Profile/monotonic micros in demo_live phases; run_editor_demo_live_perf_smoke.sh; assert non-zero; under MLC_EDITOR_PERF skip full-doc pixel-wrap/syntax/minimap (100k hang) -->
<!-- STEP=3: Critic — smoke×2 + historical main.mlc stub + demo_live_fs_compile + run_ux_gate 107; §97a closed; residual: red harness obsolete post-green; scroll drive via editor_app_wheel_scroll (no glfw scroll inject) -->

### §97b `EDITOR_FRAME_SPLIT`

#### Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | `demo_live.mlc` god-loop: layout/cache work is duplicated and threaded by hand (`wrap_count_cache_tick_pixel` at **two** full-paint sites; `frame_snapshot_cache_tick` from **25** edit branches; plus independent `shared_span_cache` / `minimap_cache` / `frame_cache`). No single memoized layout object → §97a can measure lag but cannot refactor safely without regressions |
| Strategy (v1) | **Incremental extract, no big-bang.** (1) First green slice: add `misc/editor/app/frame_layout.mlc` with a small `EditorFrameLayout` (owns `wrap_count_cache` + derived `visual_row_count` / wrapped content height) and `frame_layout_tick_*` API; `demo_live` calls it **once** per full paint instead of two inline wrap ticks. Red harness proves dual inline sites / missing module. (2) Later slices in this sub-track (after first Critic, as STEP=4+ or a follow-on Decision row): fold one more cache at a time into the same layout module (`frame_cache` snapshot tick consolidation, then `shared_span_cache`, then `minimap_cache`), then extract `frame_input.mlc` (poll → intent, no paint). Each slice gates on `run_ux_gate.sh` ×2 + §97a `run_editor_demo_live_perf_smoke.sh` (must stay green; order-of-magnitude `total_us` blowup = blocker). (3) Do **not** unify live locals onto one `EditorUxState` here (§97c); do **not** change wrap/syntax/minimap **algorithms** beyond relocation; do **not** wire `EDITOR_COMMAND_BUS_WIRE` (review #5) |
| Primary gate | Red: no `app/frame_layout.mlc` / wrap still dual-inline in `demo_live`. Green: `demo_live` uses `frame_layout_*` for wrap; unit/scenario + `demo_live` compile; Critic: stable×2 + related + `run_ux_gate` + §97a perf smoke |
| Module touch | `misc/editor/app/frame_layout.mlc` (new); `demo_live.mlc`; red/stable scripts under `scripts/`; optional `ux_scenarios/` |
| REG | no |
| Out of scope | §97c unified live state; cache algorithm rewrites; SceneNode chrome migration; SCRIPT_VM; MIR Epic 5; command-bus dual dispatch |

#### Steps (§97b — first slice: wrap → `frame_layout`)

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** |
| 1 | Red: wrap still dual-inline / no `frame_layout.mlc` | **done** (`run_editor_frame_layout_wrap_red.sh`) |
| 2 | Green: extract wrap into `app/frame_layout.mlc`; wire `demo_live` | **done** |
| 3 | Critic: stable×2 + related + `run_ux_gate` + §97a perf smoke | **done** (Critic OK 2026-07-25) |

<!-- STEP=1: red — no frame_layout.mlc; ≥2 wrap_count_cache_tick_pixel(; stable stub not implemented -->
<!-- STEP=2: EditorFrameLayout + tick; demo_live uses frame_layout_tick_pixel; zero wrap_count_cache_tick_pixel in demo_live -->
<!-- STEP=3: Critic — stable×2 + wrap_count_cache_stable + perf smoke + run_ux_gate×2 (107); first slice closed; residual: dual frame_layout_tick early/late; next extract = frame_cache -->

#### Next extract — `frame_cache` / snapshot tick consolidation

#### Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | After wrap extract, `demo_live` still calls `frame_snapshot_cache_tick` from **~25** scattered edit/input branches (plus the per-frame dirty tick). Miss one → stale `frame`/`line_index`/`draw_text`. Snapshot cache is still a bare local, not part of `EditorFrameLayout` |
| Strategy (v1) | Extend `app/frame_layout.mlc`: `EditorFrameLayout` owns `DocumentFrameSnapshotCache` alongside wrap fields; add `frame_layout_tick_snapshot(layout, document, dirty)` (thin wrap of `frame_snapshot_cache_tick`) returning updated layout; `demo_live` replaces every direct `frame_snapshot_cache_tick(...)` + `frame = frame_cache.snapshot` with that API (and reads snapshot from layout). **No algorithm change** to snapshotting. Do **not** collapse the early/late wrap ticks in this slice. Do **not** touch `shared_span_cache` / `minimap_cache` / §97c |
| Primary gate | Red: ≥10 direct `frame_snapshot_cache_tick(` in `demo_live` / no `frame_layout_tick_snapshot`. Green: demo uses `frame_layout_tick_snapshot`; direct tick count = 0 in `demo_live`; compile + Critic: stable×2 + related + `run_ux_gate`×2 + §97a perf smoke |
| Module touch | `misc/editor/app/frame_layout.mlc`; `demo_live.mlc`; red/stable scripts |
| REG | no |
| Out of scope | wrap algorithm; span/minimap caches; unified `EditorUxState` (§97c); command bus |

#### Steps (§97b — slice: snapshot → `frame_layout`)

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** |
| 1 | Red: many inline `frame_snapshot_cache_tick` / no snapshot API on layout | **done** (`run_editor_frame_layout_snapshot_red.sh`) |
| 2 | Green: own cache on `EditorFrameLayout`; wire all demo_live sites | **done** |
| 3 | Critic: stable×2 + related + `run_ux_gate`×2 + §97a perf smoke | **done** (Critic OK 2026-07-25) |

<!-- STEP=1: red — ≥10 frame_snapshot_cache_tick(; no frame_layout_tick_snapshot; stable stub -->
<!-- STEP=2: snapshot_cache on EditorFrameLayout; 0× frame_snapshot_cache_tick in demo_live -->
<!-- STEP=3: Critic — snapshot stable×2 + wrap/idle related + perf smoke + run_ux_gate×2 (107); slice closed; residual: dual frame_layout_tick early/late; span/minimap still bare locals; next extract = shared_span_cache -->

#### Next extract — `shared_span_cache` consolidation

#### Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | After snapshot extract, `demo_live` still keeps `shared_span_cache` as a **bare local** (`editor_ux_syntax_span_cache_new` + one paint-path `editor_ux_syntax_span_cache_tick`, spans also read by minimap). Not owned by `EditorFrameLayout` → layout object is incomplete; paint/minimap can diverge from a future multi-site tick the same way snapshot did |
| Strategy (v1) | Extend `app/frame_layout.mlc`: `EditorFrameLayout` owns `SyntaxSpanCacheState` (`span_cache`); add `frame_layout_tick_spans(layout, text, range_start, range_end)` (thin wrap of `editor_ux_syntax_span_cache_tick`) returning updated layout; `demo_live` drops the bare local — init via layout ctor/`frame_layout_new_pixel`, tick via layout API, paint/minimap read `frame_layout.span_cache.spans`. Preserve perf-smoke skip (`if !perf_enabled` around tick). **No algorithm change** to highlighting. Do **not** touch `minimap_cache` / dual wrap ticks / §97c |
| Primary gate | Red: direct `editor_ux_syntax_span_cache_tick(` in `demo_live` / no `frame_layout_tick_spans`. Green: demo uses layout span API; zero direct tick in `demo_live`; compile + Critic: stable×2 + `shared_syntax_span_cache_stable` + `run_ux_gate`×2 + §97a perf smoke |
| Module touch | `misc/editor/app/frame_layout.mlc`; `demo_live.mlc`; red/stable scripts |
| REG | no |
| Out of scope | highlight algorithm; minimap cache extract; unified `EditorUxState` (§97c); command bus |

#### Steps (§97b — slice: spans → `frame_layout`)

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** |
| 1 | Red: bare span tick in `demo_live` / no `frame_layout_tick_spans` | pending |
| 2 | Green: own `span_cache` on `EditorFrameLayout`; wire `demo_live` | pending |
| 3 | Critic: stable×2 + related + `run_ux_gate`×2 + §97a perf smoke | pending |

<!-- STEP=1: red — editor_ux_syntax_span_cache_tick( in demo_live; no frame_layout_tick_spans; stable stub -->
<!-- STEP=2: span_cache on EditorFrameLayout; 0× editor_ux_syntax_span_cache_tick in demo_live -->

### §97c `EDITOR_UX_PROBE_FROM_LIVE_STATE`

Once §97b's phase boundaries exist: make the live loop actually construct
and mutate **one** `EditorUxState`/`EditorAppState` per frame (not the
current scattered locals), and derive every paint call from it, the same
struct the `ux_scenarios/*` fixtures already exercise. This closes the
§94/§95/§96 class of bug by construction (one state, one paint path) rather
than by patching each symptom found so far. Fold in §96's cheap
focus-independent-wheel-scroll regression scenario here as a quick add-on
once the unified state exists.

## Verification discipline for every sub-track

Same as every other `compiler/**`/`misc/editor/**` track in this repo:
scenario-first (red before green), `run_ux_gate.sh` twice back-to-back
before Critic close, `dev_gate_fast.sh` green, no STEP marked `done` without
a matching commit. §97a's perf smoke additionally becomes a permanent
regression gate for §97b/§97c — if either step regresses the measured
ms/frame on the 100k-line fixture, that is a blocker, not a nitpick.
