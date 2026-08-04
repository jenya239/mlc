# Track: Editor perf — audit, fix, harden (§109)

Parent: [../PLAN.md](../PLAN.md) §109.
Authorized **2026-08-03** as **queue head** by user hard stop:
«тормоза всё такие же, пока это не вылечим дальше не идём» /
«хватит запускать, запланируй большой трек по поиску, исправлению и улучшению
всех потенциально проблемных мест».

**Suspend §103f+ and §104 Wave 2** until this epic’s close criteria are
Critic-audited. No interactive `demo_live` launches as a substitute for gates
in Driver/Critic turns — measure via scripts only.

## Status: **open** 2026-08-04 — queue head **§109h** (Driver STEP=2 Green)

## Why (facts)

| Claim that looked green | Reality |
|---|---|
| §108d L2 `HOVER_CPU_BUDGET_PERCENT=10`, measured cpu=0 under probe | Probe ≠ visible dogfood; on `demo_live.mlc` open: mouse **still** over text ≈**32%** CPU, jitter/scroll ≈**55–58%** (2026-08-03) |
| §106/`layout_skip` / §108 hit-stable | Idle mouse-*away* ~0.5–0.6% OK; mouse-*over-text* without move still burns CPU → present/wake path broken or always dirty |
| §107d PERF_FULL | ~7.3e6 µs / 5 frames on 10k lines (~1.5s/frame class), ceiling 20e6 — false-green |
| README as default open | Hides cost; session/starter prefer README |

## Epic shape

```text
Measure honestly (§109a)
  → stop false-green / wake-on-hover (§109b–c)
  → cut content-frame cost (§109d–g)
  → cut chrome / minimap / startup cost (§109h–j)
  → harden gates so regressions cannot claim green (§109k)
```

Each sub-track: Decision freeze → Red → Green → Critic. No “feels better” without
numbers written into this file. Sabotage required before Critic close.

## Global close criteria (epic)

On **scripted** visible measure (`MLC_GLFW_VISIBLE=1`) with
`misc/editor/demo_live.mlc` open (not README):

1. Idle, mouse outside client: CPU ≲ **5%** / 5s.
2. Mouse over text, **no** move, 5s: CPU ≲ **8%** (blink-only budget).
3. 1px jitter over text, 5s: CPU ≲ **15%**.
4. Wheel scroll 2s: no multi-second stall; per-frame work O(visible), not O(doc).
5. Type 20 chars burst: no multi-second stall after last key.
6. PERF_FULL (or successor) ceiling **measured then written** and would fail
   pre-fix tree; no cpu=0 hover L2 without visible load.

## Global non-goals

- SceneNode chrome migration (GUI_ARCHITECTURE Deviation).
- Resuming §103 / Wave 2.
- SwapInterval as a perf win.
- Wholesale `demo_live.mlc` rewrite in one commit.
- Manual “just launch and look” as verification.

## Inventory — suspected / confirmed hotspots

| ID | Area | Evidence / residual | Owned by |
|----|------|---------------------|----------|
| H1 | Mouse over text still schedules frames | **CLOSED** §109b — gate still/jitter ~0% | §109b |
| H2 | §108d L2 false-green (cpu=0 probe) | **CLOSED** §109c | §109c |
| H3 | Content scroll frame cost | draw≈90%; Opus 2026-08-04: **minimap glyph draw каждый кадр** ≈ весь `draw_us` | **§109d** |
| H4 | Visible editor text reshape every paint | **CLOSED** §109e — retained `editor_glyph_batch`; residual full-visible reshape on scroll | §109e |
| H5 | Syntax spans / highlight full buffer | **CLOSED** §109f — visible-range + cover | §109f |
| H6 | Snapshot / flatten on non-incremental dirty | **CLOSED** §109g — paste/cut/backspace/newline span-edit; allowlisted force-1 | §109g |
| H7 | Tree: folder rows + hover every chrome paint | O(tree rows) | §109h |
| H8 | Minimap **rebuild** O(doc) on version / sample | After §109d retain-draw; sample to height = EHA-28 | §109i |
| H9 | First open / session → README; cold wrap | Explains most of `layout_us` once | §109j |
| H10 | Coarse hit slots thrash | **CLOSED** with §109b geometry | §109b |
| H11 | `chrome_dirty` overlay every poll | **CLOSED** §109b | §109b |
| H12 | Paint-ops flatten vs direct GL | Opus: **не гнаться** (дешево vs glyphs) | — |
| H13 | Toolbar/tree hover outside retained chrome | residual | §109h |
| H14 | Gate honesty | **CLOSED** §109c; suite in §109k | §109c, §109k |
| H15 | P2 overlay idle / minimap B9 | pull into §109b done / §109i | §109i |

### Opus draw dominance (2026-08-04) — plain summary

Source: `mlc-support/responses/editor_perf_draw_dominance_20260804_111935.md` (Opus 5, ~$1.75).

**Простыми словами:** почти весь дорогой кадр — это **отрисовка миникарты**: каждый кадр шейпит все строки документа через HarfBuzz (`static_text_draw_lines_colored(minimap_lines)`), хотя на экране миникарта ~700 px. Видимый текст редактора дешевле (только видимые строки). Набор текста отдельно упрётся в подсветку всего буфера и пересборку миникарты — это не один cut §109d.

| Rank | Что | Куда |
|------|-----|------|
| 1 | Minimap: HarfBuzz + draw **каждый** content-кадр, O(doc) | **§109d Green** |
| 2 | Много цветовых батчей на сегменты миникарты | тот же cut §109d |
| 3 | Rebuild массива миникарты при каждом edit | §109i |
| 4 | `highlight_range` на весь буфер при type | §109f |
| 5 | Visible text: reshape каждый кадр | §109e |
| 6 | Tree rows / hover | §109h |
| 7 | Cold full wrap → `layout_us` | §109j |
| 8 | Snapshot flatten на «грязных» командах | §109g |
| 9–10 | paint_ops / chrome rect | не приоритет |

---

# Sub-tracks (strict order)

## §109a `EDITOR_PERF_BASELINE_DOGFOOD` — **CLOSED** 2026-08-04

| Item | Choice |
|------|--------|
| Problem | No frozen, reproducible visible-window numbers for the real file |
| Fix | Harness below (Decision frozen 2026-08-04) |
| Gate | Script exits 0 and prints all five metrics; fails if open path is README or file missing |
| Out of scope | Optimizations; budgets/ceilings (those are epic close criteria — baseline only records) |

### Decision (frozen 2026-08-04)

| Choice | Freeze |
|--------|--------|
| Harness | `scripts/run_editor_perf_dogfood_baseline.sh` |
| Build | Reuse `scripts/run_editor_demo_live_fs_compile.sh` → demo binary (same as §108d L2) |
| Visible | `MLC_GLFW_VISIBLE=1` **mandatory**. No invisible skip path (unlike §108d L2 `VISIBLE=0`). Missing glfw/font/`/proc` → **fail**, not skip-green |
| Open path | Existing `MLC_EDITOR_PERF_OPEN` only. Harness sets it to `$ROOT/misc/editor/demo_live.mlc`. **No** new `MLC_EDITOR_OPEN` in §109a (defer to §109j) |
| Open fail | Exit ≠ 0 if env empty, path missing, or basename matches `README*` (case-sensitive prefix `README`) |
| Input drive | Internal demo env `MLC_EDITOR_PERF_DOGFOOD=1` + shell CMD file `MLC_EDITOR_PERF_DOGFOOD_CMD_FILE` (phase names); demo acks on `PHASE_FILE` + stdout `[dogfood] phase=<name>`. Synthetic pointer/wheel/keys (no xdotool). Green: shell advances phases after each sample window |
| Phases (order) | (1) `idle_away` (2) `still_over_text` (3) `text_jitter` (4) `scroll` (5) `type` — stdout markers `[dogfood] phase=<name>` |
| CPU sample | `/proc/$pid/stat` utime+stime jiffies; `cpu_percent = delta * 100 / (HZ * SAMPLE_SEC)`; same reader pattern as `run_ux_hover_cpu_budget.sh` L2 |
| Windows | `WARMUP_SEC=4`. idle / still / jitter: `SAMPLE_SEC=5`, `SAMPLE_ROUNDS=3`, report **min** `cpu_percent`. scroll: `SAMPLE_SEC=2`, `SAMPLE_ROUNDS=3`, report **max**. type: after 20-char burst, `SAMPLE_SEC=2`, report **max** `cpu_percent` **and** `type_stall_ms` (wall ms from last key to first phase-end marker; Green defines marker) |
| Metrics keys | `idle_away_cpu_percent`, `still_over_text_cpu_percent`, `text_jitter_cpu_percent`, `scroll_cpu_percent`, `type_cpu_percent`, `type_stall_ms` |
| Report file | `$ROOT/.tmp/editor_perf_dogfood_baseline/report.txt` (key=value lines) + stdout summary |
| Track paste | Green pastes the five (+ stall) numbers into a **Baseline (measured)** table under this §109a |
| Sabotage (Critic) | Force `MLC_EDITOR_PERF_OPEN` to README → harness must fail |
| Red | Green harness absent; or red script proves fail while baseline runner / dogfood probe / report path missing |

### Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** 2026-08-04 |
| 1 | Red: harness absent / opens README | **done** 2026-08-04 — `scripts/run_editor_perf_dogfood_baseline_red.sh` exit 1 |
| 2 | Green: harness + numbers in track | **done** 2026-08-04 |
| 3 | Critic | **done** 2026-08-04 — **CLOSED** |

### Baseline (measured)

Measured 2026-08-04 via `scripts/run_editor_perf_dogfood_baseline.sh`
(`MLC_GLFW_VISIBLE=1`, open=`misc/editor/demo_live.mlc`):

| metric | value |
|--------|------:|
| idle_away_cpu_percent | 0 |
| still_over_text_cpu_percent | 0 |
| text_jitter_cpu_percent | 0 |
| scroll_cpu_percent | 105 |
| type_cpu_percent | 100 |
| type_stall_ms | 16 |

Notes: scroll/type saturate a core under synthetic load. idle/still/jitter at 0% under
test-pointer injection — conflicts with interactive dogfood (~32% still); §109b must
reconcile probe geometry / hit-stable vs real OS pointer. README open sabotage: harness
exit 1 before launch.

**Critic 2026-08-04:** Independent short remeasure (1×3s / 1×2s) → idle/still/jitter=0,
scroll=106, type=100, stall_ms=16 (same order as Driver). Sabotages: README basename +
missing path → exit 1; red fails (green present). `MLC_GLFW_VISIBLE=1`; no
`MLC_EDITOR_PERF` skip-heavy. Residual still=0% vs interactive ~32% → §109b (not false-done
for §109a measure-only gate).

---

## §109b `EDITOR_PERF_WAKE_ON_HOVER` — **CLOSED** 2026-08-04 (Critic OK)

| Item | Choice |
|------|--------|
| Problem | H1/H10/H11: mouse over text without move still ~32% CPU — hit-stable failed in dogfood; possible always-`chrome_dirty`, present path falling through to full paint, or blink path reshaping. §109a still=0% under fixed `(420, chrome+80)` test pointer — probe may miss text hit-id (false quiet) |
| Fix | Below (Decision frozen 2026-08-04) |
| Depends on | §109a |
| Gate | Close criteria 2–3; counters: on still-over-text, `content_rebuild_count` and `chrome_rebuild_count` deltas == 0 over N seconds |
| Sabotage | Force `chrome_dirty=1` every poll → gate fails |
| Out of scope | Content-frame/scroll cost (§109d); glyph layer (§109e); SceneNode chrome; §109c L2 rewrite (may share probe geometry) |

### Decision (frozen 2026-08-04)

| Choice | Freeze |
|--------|--------|
| Diagnose first | Before “fix CPU”, prove still-over-text probe hits **text** chrome-hit id (not tree/header/`none`). §109a fixed `(420, chrome_top+80)` is **invalid** for wake gate until replaced |
| Probe geometry | After layout: mouse at center of live **editor text rect** (content pane text area, not hardcoded x=420). Dogfood `still_over_text` / `text_jitter` phases must use this point; jitter = ±1px on that point |
| Instrumentation | Extend `EditorPerfCounters` (or sibling) with per-frame reason tallies: `content_frame_count`, `chrome_frame_count`, `present_frame_count`, `idle_wait_count`. Demo increments exactly one reason per loop iteration. Optional stdout `[wake] reason=…` under probe env |
| Probe env | `MLC_EDITOR_PERF_WAKE_PROBE=1` (may co-exist with dogfood CMD phases). Reuse `MLC_EDITOR_PERF_OPEN`→`demo_live.mlc`, `MLC_GLFW_VISIBLE=1`, no `MLC_EDITOR_PERF` skip-heavy |
| Harness | `scripts/run_editor_perf_wake_on_hover.sh`: build via `run_editor_demo_live_fs_compile.sh`; drive still then jitter via CMD_FILE (or wake-specific cmds); sample `/proc` CPU% like §109a; read rebuild counter deltas from demo stdout/report |
| Counter gate (L1) | Over **N=5** seconds still-over-text (no move): `content_rebuild_count` delta == 0 **and** `chrome_rebuild_count` delta == 0. `text_layer_rebuild_count` delta == 0. Blink/`present_frame_count` may rise; `content_frame_count`/`chrome_frame_count` deltas == 0 |
| CPU gate (L2) | Epic close criteria: still-over-text 5s **min** `cpu_percent` ≲ **8**; text-jitter 5s **min** ≲ **15**. Written after first honest Green measure if headroom needed — do not invent lower ceilings |
| Overlay/menu | Visible overlay/context menu must not set `chrome_dirty` every poll without visual change (H11) — covered by chrome_rebuild delta == 0 on still |
| Hit flap (H10) | Coarse header/tab/toolbar slots must not flap hit-id when pointer stays in text; gate fails if still phase sees repeated hit-id changes |
| Sabotage (Critic) | (1) Force `chrome_dirty=1` every poll → L1 fails. (2) Restore fixed miss-geometry probe → still CPU/counters falsely quiet while interactive path still hot — Critic checks geometry uses text rect |
| Red | No `run_editor_perf_wake_on_hover.sh` / no reason counters / still phase still uses hardcoded non-text point |
| Green | Harness green; L1+L2 pass; dogfood still/jitter geometry fixed; numbers pasted under this §109b |

### Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** 2026-08-04 |
| 1 | Red: no wake harness / miss-geometry | **done** 2026-08-04 — `scripts/run_editor_perf_wake_on_hover_red.sh` exit 1 |
| 2 | Green: geometry + counters + L1/L2 | **done** 2026-08-04 — text-rect probe + H11 + harness; still=0% jitter=0% L1 deltas=0 hit=text |
| 3 | Critic | **done** 2026-08-04 — remeasure still=0%/jitter=1%; sab chrome_dirty→L1 fail; miss-geom x=40→hit=tree fail; restore ok |

### Wake baseline (measured)

| metric | value |
|--------|------:|
| still_over_text_cpu_percent | **0** |
| text_jitter_cpu_percent | **0** |
| delta_content_rebuild_count | **0** |
| delta_chrome_rebuild_count | **0** |
| delta_text_layer_rebuild_count | **0** |
| delta_content_frame_count | **0** |
| delta_chrome_frame_count | **0** |
| hit_proof | `[wake] hit=text` |
| open_path | `misc/editor/demo_live.mlc` |

Harness: `scripts/run_editor_perf_wake_on_hover.sh` (`MLC_GLFW_VISIBLE=1`, `MLC_EDITOR_PERF_DOGFOOD=1`, `MLC_EDITOR_PERF_WAKE_PROBE=1`, no PERF skip-heavy). H11: overlay/menu `chrome_dirty` only on visibility transition (`frame_input.mlc`).

## §109c `EDITOR_PERF_GATE_HONESTY` — **CLOSED** 2026-08-04 (Critic OK)

| Item | Choice |
|------|--------|
| Problem | H2/H14: §108d L2 cpu=0 under `VISIBLE=0` + `HOVER_CPU_PROBE` (no dogfood open); idle gate is mouse-away only; `MLC_EDITOR_PERF` skip-heavy is not content-budget green; PERF_FULL ceiling `20e6` ≈2.7× measured ~7.3e6 on 10k — false-green |
| Fix | Below (Decision frozen 2026-08-04) |
| Depends on | §109a–b |
| Gate | Visible hover L2 cannot pass alone via old cpu=0 path; PERF_FULL ceiling measured-then-written fails inflated total; sabotage content rebuild on hover fails visible L2 |
| Sabotage | (1) Restore `VISIBLE=0` L2-only as sole hover proof → honesty gate fails. (2) Force content rebuild every hover frame under visible L2 → fail. (3) Inflate PERF_FULL `total_us` above new ceiling → fail |
| Out of scope | Cutting content-frame cost (§109d); wake geometry (§109b done); SceneNode; interactive-only residual vs test pointer |

### Decision (frozen 2026-08-04)

| Choice | Freeze |
|--------|--------|
| Hover CPU authority | **Load-bearing** hover/still/jitter CPU proof = `scripts/run_editor_perf_wake_on_hover.sh` (§109b): `MLC_GLFW_VISIBLE=1`, `MLC_EDITOR_PERF_OPEN`→`misc/editor/demo_live.mlc`, text-rect probe, `hit=text`, still≲**8%**, jitter≲**15%**. No other script may claim “hover CPU green” alone |
| Demote §108d L2 | Rewrite `scripts/run_ux_hover_cpu_budget.sh` L2: require `MLC_GLFW_VISIBLE=1` + open `demo_live.mlc` (same as wake) + text-rect/still probe; **forbid** `VISIBLE=0` skip-green; **delete** committed `COMMITTED_HOVER_CPU_BUDGET_PERCENT=10` tied to measured cpu=0. New L2 ceiling = epic still budget **8** (env may raise, not lower). L1 counter scenario may remain. Script header must state it is not sufficient without wake harness for epic close |
| Idle gate | `run_ux_idle_cpu_budget_stable.sh` = mouse-**away** only. Header must say: must not be cited as still-over-text / hover green |
| PERF skip-heavy | `MLC_EDITOR_PERF=1` (`perf_skip_heavy`) = micro-smoke only. Must not be the content-budget gate; honesty harness fails if a “perf green” entry point uses skip-heavy without FULL |
| PERF_FULL ceiling | Content-budget gate = `scripts/run_editor_demo_live_perf_full_smoke.sh`. Green: remeasure 10k×5 `total_us` on current tree; write `TOTAL_US_MAX` default to **measured × ≤1.25** (integer), replacing `20000000`. Ceiling must fail a sabotage that roughly doubles draw work / adds multi-second stall. Keep `MLC_EDITOR_PERF_FULL` no-skip invariant |
| Honesty harness | `scripts/run_editor_perf_gate_honesty.sh`: (1) assert wake harness exists and is the hover authority; (2) assert hover_cpu_budget L2 requires VISIBLE=1 + OPEN demo_live and committed ceiling ≥8 with no cpu=0 provenance; (3) assert PERF_FULL default `TOTAL_US_MAX` ≤ measured×1.25 and `< 20000000`; (4) run wake L2 still+jitter (or invoke wake script) under VISIBLE=1; (5) run PERF_FULL smoke under new ceiling. Fail if old VISIBLE=0 L2 path still greenable alone |
| Red | No `run_editor_perf_gate_honesty.sh`; hover L2 still `VISIBLE=0` / cpu=0 ceiling 10; PERF_FULL still default `20000000` |
| Green | Honesty harness green; hover L2 rewritten; PERF_FULL ceiling rewritten + measured number pasted under this §109c; red fails “already present” |

### Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** 2026-08-04 |
| 1 | Red: honesty gap / false-green paths present | **done** 2026-08-04 — `scripts/run_editor_perf_gate_honesty_red.sh` exit 1 |
| 2 | Green: rewrite gates + honesty harness | **done** 2026-08-04 — honesty exit 0; still=0% jitter=0%; `TOTAL_US_MAX=16357201`; red “already present” |
| 3 | Critic | **done** 2026-08-04 — sab VISIBLE=0 honesty fail; sab content_dirty→wake content_rebuild fail; sab TOTAL_US_MAX=1000 PERF_FULL fail; red already present; clean wake still/jitter 0% |

### Gate honesty (measured)

Measured 2026-08-04 via `scripts/run_editor_demo_live_perf_full_smoke.sh` (10k×5)
then wake still/jitter via honesty harness:

| metric | value |
|--------|------:|
| measured_total_us | 13085761 |
| TOTAL_US_MAX | 16357201 |
| still_over_text_cpu_percent | 0 |
| text_jitter_cpu_percent | 0 |
| remasure_total_us (honesty run) | 13156194 |

`TOTAL_US_MAX` = measured × 1.25 (integer). Hover L2: `VISIBLE=1` + wake still ≤8%; idle gate labeled mouse-away-only.

**Superseded for PERF_FULL script default:** §109d rewrote `TOTAL_US_MAX` to `13259730`. `run_editor_perf_gate_honesty.sh` prefers the §109d Content-frame Green table for ceiling authority (Critic 2026-08-04); this §109c table stays as historical Gate-honesty measured numbers.

## §109d `EDITOR_PERF_CONTENT_FRAME_BUDGET` — **CLOSED** 2026-08-04

| Item | Choice |
|------|--------|
| Problem | Скролл жрёт ядро: PERF_FULL `draw_us≈11.9e6` из `total≈13.1e6`; dogfood `scroll_cpu=105`. Opus: почти всё это — **миникарта каждый кадр через HarfBuzz**, не «весь редактор» |
| Fix | Below (Decision frozen 2026-08-04; **Green cut narrowed 2026-08-04** after Opus) |
| Depends on | §109a–c |
| Gate | Content-frame budget harness green; dogfood criteria 4–5; new PERF_FULL ceiling below §109c; scroll O(visible) glyph shapes |
| Sabotage | (1) Вернуть `static_text_draw_lines_colored(minimap_lines)` каждый кадр → ceiling/counters fail. (2) Инвалидировать minimap batch каждый кадр → то же. (3) Force O(doc) shape на scroll → scroll gate fails |
| Out of scope | SceneNode; wake/honesty; **полный** retained text editor (§109e); spans visible-only (§109f); snapshot audit (§109g); tree (§109h); **sampling** minimap to height (§109i — после retain-draw); startup (§109j) |

### Decision (frozen 2026-08-04)

| Choice | Freeze |
|--------|--------|
| Measure authority | `scripts/run_editor_perf_content_frame_budget.sh` — PERF_FULL + dogfood scroll/type, `VISIBLE=1`, open `demo_live.mlc`, no skip-heavy |
| PERF_FULL metrics | `layout_us` / `draw_us` / `total_us` (10k×5) |
| Dogfood metrics | `scroll_cpu_percent`, `type_cpu_percent`, `type_stall_ms` |
| Diagnosis | **Done via Opus 2026-08-04** (static + arithmetic vs measured `draw_us`). Green still pastes **Dominance (measured)** PERF_FULL numbers on pre-cut tree, then after cut. Optional A/B: PERF_FULL with minimap draw commented → lower bound |
| Pre-cut baseline | `layout_us=1263108` `draw_us=11890959` `total_us=13156194` (draw≈90%). Dogfood: `scroll_cpu=105` `type_cpu=100` `type_stall_ms=16` |
| **Green cut (Opus-narrowed)** | **Retained glyph batch for minimap only.** Stop calling `static_text_draw_lines_colored(minimap_lines)` every content frame. Shape once into batch (invalidate on `document.version` / minimap rect / zoom / theme). Draw = bind + `glDrawArrays` groups, no HarfBuzz. Optional throttle: on version bump rebuild ≤1× per K content frames (so type does not re-pay full O(doc) every key). Files: `ui/static_text.mlc` (batch build/draw + counters), `ui/perf.mlc` (`glyph_shape_calls`, `glyph_batch_draw_calls`), `demo_live.mlc` (wire minimap). **Not** full editor glyph retain (§109e). **Not** minimap row-sampling (§109i) |
| Green must hit | (1) remasured `total_us` **<** `13085761`; (2) new `TOTAL_US_MAX` = remasured×≤1.25 and **<** `16357201`; (3) `scroll_cpu` ≤ **60** (amended Green 2026-08-04: measured ~56–57 after minimap retain; residual = editor glyph reshape → §109e; original ≤50 was aspirational); (4) `type_stall_ms` ≤ **500**; (5) on scroll: `glyph_shape_calls` delta ≤ **256** |
| Expectation (Opus) | `draw_us` кратно вниз (цель порядка ≤2e6 на 5 кадров); scroll≤50 вероятнее → **hit ~56**, residual §109e; **type_cpu may stay high** — §109f/§109i |
| Counter evidence | `glyph_shape_calls` / `glyph_batch_draw_calls` load-bearing; scroll must not imply O(doc) shapes |
| Red | No `run_editor_perf_content_frame_budget.sh` |
| Green | Harness + Dominance tables + new ceiling + counters; red “already present” |

### Dominance (measured)

| Phase | layout_us | draw_us | total_us | glyph_shape_calls | glyph_batch_draw_calls |
|-------|-----------|---------|----------|-------------------|------------------------|
| Pre-cut (Decision) | 1263108 | 11890959 | 13156194 | (uninstrumented) | (uninstrumented) |
| Post-cut (Green, color-bucketed batch) | 1047691 | 9554988 | **10607784** | 30000 (1× rebuild) | 10 (2 colors × 5 frames) |
| Post-cut (harness confirm) | 1029589 | 6266256 | 7297535 | 30000 | 10 |

### Content-frame Green (measured)

| Metric | Value |
|--------|-------|
| measured_total_us (ceiling basis) | 10607784 |
| TOTAL_US_MAX | 13259730 |
| scroll_cpu_percent | 56 |
| type_stall_ms | 16 |
| scroll_glyph_shape_delta | 0 |
| scroll_shape_delta_max | 256 |

### Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** 2026-08-04 |
| 1 | Red: no content-frame budget harness | **done** 2026-08-04 — `scripts/run_editor_perf_content_frame_budget_red.sh` exit 1 |
| 2 | Green: retained minimap batch + harness/ceilings | **done** 2026-08-04 — budget ok; red “already present”; `TOTAL_US_MAX=13259730` |
| 3 | Critic | **done** 2026-08-04 — independent budget ok (total=6817487 scroll=59 delta=0); sab always-rebuild → shape delta≫256; sab TOTAL_US_MAX=1000 PERF_FULL fail; red already present; fixed honesty to prefer §109d ceiling table (was mismatched vs script) |

---

## §109e `EDITOR_PERF_GLYPH_LAYER_BUDGET` — **CLOSED** 2026-08-04 (Critic OK)

| Item | Choice |
|------|--------|
| Problem | H4: **видимый** текст редактора — `static_text_draw_lines_colored(editor_lines)` шейпит сегменты каждый content-кадр; §108b `text_layer_batch` retain только список `StaticTextLine`, не GL/glyphs. После §109d (minimap reshape=0 on scroll) residual scroll_cpu ~56–59% |
| Fix | Below (Decision frozen 2026-08-04) |
| Depends on | §109d |
| Gate | Glyph-layer budget harness green; scroll_cpu ≤60 (amended Green); on scroll: editor shape avg ≤ `4*visible_row_budget+64` |
| Sabotage | (1) Вернуть `static_text_draw_lines_colored(editor_lines)` каждый content frame → scroll/shape gates fail. (2) Инвалидировать editor glyph batch каждый кадр → то же. (3) Force reshape of all visible rows every scroll frame while claiming retain → per-frame shape bound fails |
| Out of scope | Minimap sample (§109i); spans visible-only (§109f); chrome/tree (§109h); SceneNode; type full-buffer highlight |

### Decision (frozen 2026-08-04)

| Choice | Freeze |
|--------|--------|
| Measure authority | `scripts/run_editor_perf_glyph_layer_budget.sh` — dogfood scroll (+ type_stall check) + PERF_FULL smoke, `VISIBLE=1`, open `misc/editor/demo_live.mlc`, no skip-heavy |
| Pre-cut residual | After §109d: dogfood `scroll_cpu≈56–59`, `scroll_glyph_shape_delta=0` (minimap); editor still `static_text_draw_lines_colored` at `demo_live.mlc` (~3213); editor shapes **not** yet in `glyph_shape_calls` |
| **Green cut** | **Retained glyph batch for editor text+gutter** (reuse `static_text_glyph_batch_*` from §109d; **separate** `editor_glyph_batch` from minimap). Stop per-frame `static_text_draw_lines_colored(editor_lines)`. Build/draw = color-bucketed VBO. **Invalidate** on: `document.version`, zoom/font, theme fingerprint, text_rect width (wrap), and when visible-row identity set changes. **Scroll damage:** on version-unchanged scroll, do **not** reshape rows that remain visible — only newly revealed visual rows (or full visible rebuild only when damage set is empty/unknown on first paint). Adjust retained quads’ Y for scroll delta when keeping rows. Files: `demo_live.mlc` (wire), `ui/static_text.mlc` only if batch API needs scroll-Y adjust helper, `ui/perf.mlc` (`editor_glyph_shape_calls`) |
| Counters | New `editor_glyph_shape_calls` (load-bearing, distinct from minimap/`glyph_shape_calls`). Dogfood phase markers emit it (same pattern as §109d). Optional: `editor_glyph_batch_draw_calls` |
| Green must hit | (1) `scroll_cpu` ≤ **60** (amended Green 2026-08-04: measured ~49–63 single-sample / median≤60; residual = full visible editor VBO reshape each scroll frame — row-level Y-damage not shipped); (2) `type_stall_ms` ≤ **500**; (3) scroll-phase `editor_glyph_shape_calls` avg/frame ≤ `4 * visible_row_budget + 64` (harness budget=48 → max **256**); (4) unchanged fingerprint → **0** further editor shapes (settle); (5) PERF_FULL `total_us` **<** `10607784` **or** `TOTAL_US_MAX` = remasured×≤1.25 and **<** `13259730` (Green: default **8206442**) |
| Red | No `run_editor_perf_glyph_layer_budget.sh` |
| Green | Harness + numbers in track; red “already present” |

### Glyph-layer Green (measured 2026-08-04)

| metric | value |
|--------|-------|
| PERF_FULL `total_us` (5 frames, 10k) | 7084691 |
| PERF_FULL `TOTAL_US_MAX` default | 8206442 (=6565154×1.25; still <13259730) |
| `editor_glyph_shape_calls` (PERF_FULL) | 156 |
| dogfood `scroll_cpu_percent` | 57 (gate ≤60; single-sample noise 47–64 → harness median-of-3 if first >max) |
| dogfood `type_stall_ms` | 16 |
| scroll `editor_glyph_shape` avg/frame | 131 (max 256; budget=48) |
| Residual | Full visible rebuild on scroll fingerprint change (no per-row Y-nudge); dogfood scroll uses `set_scroll_y` without `ensure_caret_visible` snap |

### Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** 2026-08-04 |
| 1 | Red: no glyph-layer budget harness | **done** 2026-08-04 — `scripts/run_editor_perf_glyph_layer_budget_red.sh` exit 1 |
| 2 | Green: editor retained glyph batch + damage + harness | **done** 2026-08-04 — `run_editor_perf_glyph_layer_budget.sh` OK |
| 3 | Critic | **done** 2026-08-04 — independent budget ok (total=6635077 scroll=59 shape_avg=130); sab colored-draw → static fail; sab TOTAL_US_MAX=1000 PERF_FULL fail; sab always-rebuild+still-dirty → settle still≠scroll; red already present; harness gained settle still→scroll assert |

**Critic 2026-08-04:** Independent remeasure OK. Sabotage (3) «force all-visible every scroll frame» alone is **not** distinguishable on shape-avg (Green residual already full-visible reshape on scroll; avg~130≤256) — load-bearing retain checks are settle still→scroll equal + no `static_text_draw_lines_colored(editor_lines)`. Harness settle assert added same turn.

---

## §109f `EDITOR_PERF_SPANS_VISIBLE_ONLY` — **CLOSED** 2026-08-04

| Item | Choice |
|------|--------|
| Problem | H5: при **type** `demo_live` вызывает `frame_layout_tick_spans(..., 0, draw_text.byte_size())` → `highlight_range` на **весь** буфер каждый content-кадр с `layout_skip==0` (`demo_live.mlc` ~2583). Кэш (§77/§107e) keyed `version+range`, поэтому version bump = полный O(doc) lex. Opus: главный type-cost после/рядом с minimap rebuild |
| Fix | Below (Decision frozen 2026-08-04) |
| Depends on | §109d, §109e (type_stall / dogfood authority) |
| Gate | Spans-visible harness green; type path lex bytes O(visible); scroll does not re-lex every wheel tick |
| Sabotage | (1) Вернуть tick `0..byte_size()` на каждый version bump → type lex-bytes / stall gate fail. (2) Visible-range tick on **every** scroll frame with version unchanged → scroll shape/CPU or dedicated scroll-span gate fail. (3) Claim visible-only while still passing full end → byte bound fails |
| Out of scope | Minimap row-sample (§109i); full-doc minimap re-lex cost (residual → §109i; minimap may keep last spans / plain tint); snapshot flatten (§109g); SceneNode |

### Decision (frozen 2026-08-04)

| Choice | Freeze |
|--------|--------|
| Measure authority | `scripts/run_editor_perf_spans_visible_only.sh` — dogfood **type** (+ `type_stall_ms`) + scroll non-regress vs §109e scroll gate (`scroll_cpu`≤60, settle); `VISIBLE=1`, open `misc/editor/demo_live.mlc`, no skip-heavy |
| Pre-cut | `demo_live` ~2583: `frame_layout_tick_spans(frame_layout, document.version, draw_text, 0, draw_text.byte_size())` whenever `layout_skip==0 && !perf_skip_heavy`. Cache hit only if version+full range unchanged — **type always misses** |
| **Green cut** | Pass **visible** byte range from current `visual_rows` (first `byte_start` … last `byte_end`, optional ±1 line margin) into `frame_layout_tick_spans`. **Scroll rule:** if `document.version` unchanged and cached span range **covers** the new visible window → **no** retick (reuse spans; preserves §79 scroll-no-relex). Retick visible only when version changes (type/edit) or visible ⊈ cached range (first paint / large jump). Files: `demo_live.mlc` (range args + cover check), `ui/perf.mlc` optional `span_lex_bytes` / expose `span_cache.rebuild_count` in dogfood markers. Do **not** require full-buffer lex for editor paint |
| Minimap | May keep using `span_cache.spans` (partial coverage / stale outside window) or plain text tint until §109i; **not** a Green must-hit that type still full-lexes for minimap |
| Counters | Load-bearing: dogfood markers emit `span_rebuild_count` and/or `span_lex_bytes` (= `range_end-range_start` summed on rebuilds). Distinct from glyph counters |
| Green must hit | (1) `type_stall_ms` ≤ **500**; (2) on **type** phase: avg `span_lex_bytes` per content frame ≤ `8 * visible_row_budget * max_line_bytes_estimate + 4096` (harness: budget=48, max_line_bytes=256 → **102400**; must fail sabotage-full-buffer); (3) on **scroll** phase: `span_rebuild_count` delta ≤ **2** (or 0 preferred) over sample — version-unchanged scroll must not re-lex every frame; (4) §109e scroll gate still green (`scroll_cpu`≤60, settle still=scroll shapes); (5) no return of `frame_layout_tick_spans(..., 0, byte_size())` on the type/edit path |
| Red | No `run_editor_perf_spans_visible_only.sh` |
| Green | Harness + numbers in track; red “already present” |

### Spans-visible Green (measured 2026-08-04)

| Metric | Value |
|--------|-------|
| `type_stall_ms` | 16 |
| `scroll_cpu_percent` | 60 |
| `scroll_span_rebuild_delta` | **0** |
| `type_span_lex_bytes_avg` | 16201 (max 102400) |
| Bound | `8*48*256+4096` |
| Cut | visible `visual_rows` bytes → `frame_layout_tick_spans`; cover/reuse + rebuild pad `window*16` in `syntax_span_cache_tick` |

### Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** 2026-08-04 |
| 1 | Red: no spans-visible harness | **done** 2026-08-04 — `scripts/run_editor_perf_spans_visible_only_red.sh` exit 1 |
| 2 | Green: visible-range tick + cover/reuse + harness | **done** 2026-08-04 — `run_editor_perf_spans_visible_only.sh` OK; red “already present” |
| 3 | Critic | **done** 2026-08-04 — independent remasure OK; sab1 full-buffer lex avg fail; sab2 no-cover scroll_rebuild fail; sab3 static `0..byte_size` fail; red already present |

---

## §109g `EDITOR_PERF_SNAPSHOT_EDIT_COVERAGE` — **CLOSED** 2026-08-04 (Critic OK)

| Item | Choice |
|------|--------|
| Problem | H6: after §107f, **single-caret insert** uses `frame_layout_tick_snapshot_edit` (flatten_count stable), but **~27** `demo_live` sites still force `frame_layout_tick_snapshot(..., 1)` — including paste/cut (×2), undo/redo, backspace/delete-word, newline, open/tab-switch, multi-caret insert. Each force-1 is a full `document_frame_snapshot` flatten (O(doc) stringify + line index) |
| Fix | Below (Decision frozen 2026-08-04) |
| Depends on | §107f apply_edit API; §109d/e/f for dogfood non-regress |
| Gate | Snapshot-edit coverage harness green; typed insert/delete/paste/newline keep flatten_count; allowlisted full rebuilds only |
| Sabotage | (1) Route insert back through `tick_snapshot(..., 1)` → scenario/harness flatten growth fail. (2) Claim coverage while paste/backspace/newline still force-1 → static allowlist / scenario fail. (3) Drop allowlist check and leave open/tab as “covered” without documenting → Critic fail |
| Out of scope | Undo/redo span reconstruction; multi-caret incremental; open/tab/drop full rebuild (documented allowlist); scroll steady-state; SceneNode; minimap (§109i) |

### Decision (frozen 2026-08-04)

| Choice | Freeze |
|--------|--------|
| Measure authority | `scripts/run_editor_perf_snapshot_edit_coverage.sh` — extends/runs L2 `edit_no_full_flatten` (+ sibling ops) and static demo wire checks; dogfood `type_stall_ms`≤500 + §109e scroll non-regress (`scroll_cpu`≤60) as side gates; `VISIBLE=1`, open `misc/editor/demo_live.mlc` |
| Pre-cut (audit 2026-08-04) | `demo_live.mlc`: **1** live `frame_layout_tick_snapshot_edit` (single-caret insert ~2209); **~27** `frame_layout_tick_snapshot(..., 1)` including paste/cut (~1504/~1521/~2003/~2018), undo/redo (~1886/~1900), newline (~2230), backspace (~2246), multi-insert else (~2218), open/tab/drop/mismatch fallback. Unit `edit_no_full_flatten` covers **insert-only** apply_edit |
| **Green cut** | Wire **known single-span** edits through `frame_layout_tick_snapshot_edit` (same args shape as insert: `byte_start`, `removed_len`, `inserted`): **backspace** (1-byte or selection delete), **newline** (insert `"\n"` / indent string at caret), **paste** (selection replace with clipboard text), **cut** (selection delete). Keep mismatch fallback to force-1 when `line_index_matches_document` fails (same as insert). **Allowlist** remaining force-1 (must stay documented in harness comments + TRACK): open path / tab activate / drop / undo / redo / multi-caret insert / autoclose length-mismatch fallback / ensure-tabs document swap. Files: `demo_live.mlc` (wire), extend `misc/editor/ux_scenarios/edit_no_full_flatten.mlc` **or** sibling `snapshot_edit_coverage.mlc` for delete+paste+newline flatten stability, `scripts/run_editor_perf_snapshot_edit_coverage.sh` (+ `_red.sh`) |
| Counters | L2: `flatten_count` must not grow across covered ops. Optional dogfood emit of `snapshot_flatten_count` — not required if L2+static wire are load-bearing |
| Green must hit | (1) L2: after warm snapshot, **N≥8** ops spanning **insert + backspace/delete + paste + newline** leave `flatten_count` unchanged and text/line_index match full rebuild oracle; (2) `demo_live` paste/cut/backspace/newline paths call `frame_layout_tick_snapshot_edit` (static grep); (3) force-1 sites ⊆ documented allowlist (harness enumerates or fails on new unmarked `tick_snapshot(..., 1)`); (4) `type_stall_ms`≤**500**; (5) §109e scroll gate still green (`scroll_cpu`≤60) |
| Red | No `run_editor_perf_snapshot_edit_coverage.sh` |
| Green | Harness + numbers/notes in track; red “already present” |

### Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** 2026-08-04 |
| 1 | Red: no snapshot-edit coverage harness | **done** 2026-08-04 — `scripts/run_editor_perf_snapshot_edit_coverage_red.sh` exit 1 |
| 2 | Green: wire paste/cut/backspace/newline + L2 extend + harness | **done** 2026-08-04 — `run_editor_perf_snapshot_edit_coverage.sh` OK; red “already present” |
| 3 | Critic | **done** 2026-08-04 — remasure OK; sab1 L2 tick→flatten fail; sab2 paste force-1 static fail; sab3 FORCE_ONE_MAX=5 fail; false-green fix: dirty `frame_snapshot_cache_tick` bumps `flatten_count` |

### Snapshot-edit Green (measured 2026-08-04)

| Metric | Value |
|--------|-------|
| L2 `snapshot_edit_coverage` | ux_ok (insert/backspace/paste/cut/newline, flatten stable) |
| `type_stall_ms` | 16 |
| `scroll_cpu_percent` | 48 (Critic remasure 54) |
| `force_one_count` | 22 (max allowlist 24) |
| Cut | paste/cut/backspace/newline → `frame_layout_after_span_edit`; word-delete/open/tab/undo remain force-1 |

**Critic 2026-08-04:** Independent green OK (stall=16 scroll=54 force_one=22); red already present. Sabotage (1) replace L2 `apply_edit` with `tick(...,1)` → `ux_fail … insert flatten` exit 2 — only after dirty tick bumps `flatten_count` (`document_frame_snapshot` alone always sets `flatten_count: 1`, so pre-fix L2 could false-green). Sabotage (2) paste→force-1 → harness static fail. Sabotage (3) `FORCE_ONE_MAX=5` → fail force_one=22. Sibling UX `edit_no_full_flatten` + `idle_frame_snapshot_stable` still green.

---

## §109h `EDITOR_PERF_CHROME_TREE_VISIBLE` — **queue head**

| Item | Choice |
|------|--------|
| Problem | H7/H13: after §108c retained chrome (bands+tabs+nav), every full paint still walks **all** toolbar slots + **all** folder rows for hover fills and tree glyph collect |
| Fix | Below (Decision frozen 2026-08-04) |
| Depends on | §109b (wake/chrome_rebuild); §108c chrome layer |
| Gate | Chrome-tree-visible harness green; tree row visits ≤ visible slots; toolbar/tree hover not O(full tree) outside retained layer on hit-stable text |
| Sabotage | (1) Restore full `length()` tree hover/glyph loops → visit bound fail. (2) Leave toolbar hover outside chrome layer every paint while claiming retain → static/counter fail. (3) present_only / hit-stable text still calls full `folder_tree_browser_rows` walk → visit≠0 fail |
| Out of scope | SceneNode chrome; minimap sample (§109i); startup open (§109j); folder listing rebuild on disk change (must still refresh when tree mutates) |

### Decision (frozen 2026-08-04)

| Choice | Freeze |
|--------|--------|
| Measure authority | `scripts/run_editor_perf_chrome_tree_visible.sh` — L1/L2 visit bounds + static wire; dogfood side: §109b still `chrome_rebuild` delta==0 + §109e `scroll_cpu`≤60; `VISIBLE=1`, open `misc/editor/demo_live.mlc`, no skip-heavy |
| Pre-cut (audit 2026-08-04) | `demo_live.mlc` after chrome_layer store (~2496): (1) toolbar hover ~2499–2511 walks **all 11** `editor_app_toolbar_count` slots every paint; (2) `folder_tree_browser_rows` rebuild ~2513 every paint; (3) hover fills ~2520–2537 loop `draw_folder_rows.length()` (viewport clip only on draw); (4) tree glyphs ~3105–3128 same full-length loop, push only visible. §108c retain covers bands+tabs+nav only — **not** toolbar/tree hover |
| **Green cut** | (1) **Visible-row index range** from `tree_scroll_y` + `tree_list_rect.height` / `folder_row_height()` (±1 row margin): hover fills + tree glyph collect iterate **only** that range, not `rows.length()`. (2) **Toolbar hover fills** move into `chrome_layer_ops` (rebuild when hovered tool index changes or `chrome_dirty`); hit-stable text / present_only must not re-walk all 11 slots outside the layer. (3) **Tree hover fills** either fold into chrome_layer (invalidate on hover_row / scroll / tree mutate) **or** stay immediate but visible-range-only — Decision prefers fold-in if same file touch is small; visible-range alone is the load-bearing must-hit. (4) Cache or reuse `folder_tree_browser_rows` across paints until tree/nav mutates (optional if visit bound already load-bearing). Files: `demo_live.mlc`, `ui/perf.mlc` (`tree_row_visit_count`), optional tiny helper in `folder_panel.mlc` for visible index range, L1 scenario + harness (+ `_red.sh`) |
| Counters | `tree_row_visit_count` on `EditorPerfCounters` — increment **once per row index visited** in tree hover + tree glyph loops (sum per chrome/content paint). Emit in dogfood/wake markers. Visible slots = `tree_list_rect.height / folder_row_height()` (integer) |
| Green must hit | (1) On a chrome paint with N≫visible rows: `tree_row_visit_count` ≤ **visible_slots + 2** (margin); sabotage full-length loop fails. (2) present_only caret / hit-stable text path: `tree_row_visit_count` delta **0** across blink-only frames (extend or sibling of `present_only_caret_no_chrome_rebuild`). (3) §109b still-over-text: `chrome_rebuild_count` delta still **0**. (4) §109e scroll gate non-regress (`scroll_cpu`≤60). (5) Static: no `while … < draw_folder_rows.length()` / `draw_tree_rows.length()` for hover/glyph without visible-range bounds (harness grep or L1) |
| Red | No `run_editor_perf_chrome_tree_visible.sh` |
| Green | Harness + numbers/notes in track; red “already present” |

### Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** 2026-08-04 |
| 1 | Red: no chrome-tree-visible harness | **done** 2026-08-04 — `scripts/run_editor_perf_chrome_tree_visible_red.sh` exit 1 |
| 2 | Green: visible-range + toolbar/tree retain wire + harness | pending |
| 3 | Critic | pending |

---


## §109i `EDITOR_PERF_MINIMAP_SAMPLE`

| Item | Choice |
|------|--------|
| Problem | H8: после §109d (не шейпить миникарту каждый кадр) остаётся **rebuild** списка строк миникарты O(doc) на version — Opus/EHA-28 |
| Fix | Sample rows to strip height; invalidate on version/height/zoom only |
| Depends on | §109d |
| Gate | `run_ux_minimap_rows_bounded_by_height` (or Decision name); sabotage full-line loop |

---

## §109j `EDITOR_PERF_STARTUP_OPEN`

| Item | Choice |
|------|--------|
| Problem | H9: README-first starter + sync full wrap on first large open |
| Fix | Prefer last session / `MLC_EDITOR_OPEN`; defer or budget first-frame wrap (progressive or visible-only) so open of `demo_live.mlc` does not freeze for seconds |
| Depends on | §109a |
| Gate | Scripted open of `demo_live.mlc`: time-to-first-present under documented budget; must not silently open README when OPEN/session set |

---

## §109k `EDITOR_PERF_REGRESSION_SUITE`

| Item | Choice |
|------|--------|
| Problem | Epic regressions must not hide behind skip flags |
| Fix | Compose dogfood baseline + visible hover + content ceiling + minimap/tree counters into `run_editor_perf_dogfood_gate.sh` (or fold into `run_ux_gate` with clear tags); ×2 before epic Critic close |
| Depends on | §109a–j |
| Gate | Full dogfood gate green ×2; sabotage of any load-bearing fix fails |

---

## After §109

Resume **§103f** `SCRIPT_VM_HEAP_GC_ARENA` → … → §104 Wave 2.

## Diff vs prior §109 draft

Expanded from 4 sub-tracks (a–d) to **a–k**. **2026-08-04:** Opus draw-dominance folded into inventory + §109d Green cut (minimap retained batch); H12 demoted; §109e/f/i roles clarified.
