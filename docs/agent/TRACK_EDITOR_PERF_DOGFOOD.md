# Track: Editor perf dogfood — real lag still unacceptable (§109)

Parent: [../PLAN.md](../PLAN.md) §109.
Authorized **2026-08-03** as **queue head** by user: «тормоза всё такие же, пока
это не вылечим дальше не идём» — **suspend §103 Script VM and all other tracks**
until dogfood editor feel is acceptable.

## Status: **open** 2026-08-03 — queue head **§109a** (Driver STEP=0)

## Why (facts, not gate green)

- §106–§108 closed; hover L1 counters and L2 `HOVER_CPU_BUDGET_PERCENT=10`
  (measured **cpu_percent=0** under `MLC_EDITOR_HOVER_CPU_PROBE`) — **false comfort**:
  probe path ≠ interactive dogfood with a real document.
- §107d `PERF_FULL` documented first measure **total_us≈7.3e6 / 5 frames** on a
  10k-line fixture (~1.5s/frame order) with ceiling **20e6** — still unusable as an editor.
- User dogfood 2026-08-03 evening: lag unchanged; README open is not the workload.

## Hard stop

Do **not** advance §103f+ / §104 Wave 2 / new features until §109 close criteria below
are met and Critic-audited. Explicit user override required to resume §103 earlier.

## Close criteria (dogfood)

All must pass on a **visible** live binary (`MLC_GLFW_VISIBLE=1`) with
`misc/editor/demo_live.mlc` (or larger) open — not only README:

1. Idle, mouse outside window: process CPU ≲ **5%** over 5s.
2. Stable-hit hover over text (1px jitter): CPU ≲ **15%** over 5s.
3. Continuous scroll wheel 2s on that file: no multi-second freezes; frame work
   dominated by visible lines only (document counters / perf tags).
4. Typing a burst of 20 chars: no multi-second stall after last key.
5. Gates that claimed success while measuring `cpu=0` / absurd ceilings must be
   **tightened or replaced** so sabotage that restores full content rebuild on
   hover fails L2 under visible load.

## Sub-tracks (strict)

### §109a `EDITOR_PERF_BASELINE_DOGFOOD` — queue head

| Item | Choice |
|------|--------|
| Problem | No honest visible-window baseline for the file users actually edit |
| Fix | Scripted measure harness: launch `demo_live` with `MLC_EDITOR_PERF_OPEN` → `misc/editor/demo_live.mlc`, `MLC_GLFW_VISIBLE=1`; sample idle / text-hover / scroll / type into a report file; write numbers into this track before any optimization claim |
| Gate | `run_editor_perf_dogfood_baseline.sh` prints the four numbers; fails if editor fails to open the target file |
| Out of scope | Optimizations (those are §109b+) |

### §109b `EDITOR_PERF_CONTENT_FRAME_BUDGET`

| Item | Choice |
|------|--------|
| Problem | Content frames (scroll/type) still too expensive on multi-kLOC files |
| Fix | Profile `layout_us`/`draw_us` / counters on content frames; cut the dominant cost (likely glyph rebuild, tree row hover loop, paint-ops flatten, snapshot) until close criteria 3–4 hold |
| Depends on | §109a numbers |
| Gate | Dogfood criteria 3–4 + tightened perf-full or sibling smoke with **measured-then-written** ceiling that would fail today's tree |

### §109c `EDITOR_PERF_HOVER_VISIBLE_GATE`

| Item | Choice |
|------|--------|
| Problem | §108d L2 green at cpu=0 is not load-bearing for dogfood |
| Fix | Re-measure hover under `MLC_GLFW_VISIBLE=1` + real file; replace ceiling; ensure stable-hit truly schedules no chrome/content paint |
| Depends on | §109a |
| Gate | Close criterion 2 + sabotage force content rebuild on pointer move |

### §109d `EDITOR_PERF_CHROME_ROW_COST`

| Item | Choice |
|------|--------|
| Problem | Chrome paint still walks all folder rows / toolbar slots even when only text layer should present |
| Fix | Only hover-highlight visible tree rows; skip chrome rebuild when hit-stable (already intended — verify live) |
| Depends on | §109a–b as needed |
| Gate | Counter: chrome_rebuild_count == 0 on stable text hover over N frames |

## Non-goals

- SceneNode chrome migration
- Continuing §103 / Wave 2
- Claiming SwapInterval as a fix
