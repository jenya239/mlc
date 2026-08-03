# Track: Editor perf — audit, fix, harden (§109)

Parent: [../PLAN.md](../PLAN.md) §109.
Authorized **2026-08-03** as **queue head** by user hard stop:
«тормоза всё такие же, пока это не вылечим дальше не идём» /
«хватит запускать, запланируй большой трек по поиску, исправлению и улучшению
всех потенциально проблемных мест».

**Suspend §103f+ and §104 Wave 2** until this epic’s close criteria are
Critic-audited. No interactive `demo_live` launches as a substitute for gates
in Driver/Critic turns — measure via scripts only.

## Status: **open** 2026-08-04 — queue head **§109a** (Driver STEP=0)

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
| H1 | Mouse over text still schedules frames | 32% CPU still | §109b |
| H2 | §108d L2 false-green (cpu=0 probe) | track §108d Critic notes | §109c |
| H3 | Content scroll/type frame cost | 55%+ scroll; PERF_FULL ~1.5s/frame | §109d |
| H4 | Glyph / `StaticTextLine` rebuild every content paint | paint path; §108b retain only chrome-only | §109e |
| H5 | Syntax spans / highlight full buffer | §107i residual class | §109f |
| H6 | Snapshot / flatten on non-incremental dirty | §107f fallbacks; open/replace | §109g |
| H7 | Tree: `folder_tree_browser_rows` + per-row hover every chrome paint | demo_live loops | §109h |
| H8 | Minimap full-doc sampling | EHA-28 / B9; `frame_layout_tick_minimap` | §109i |
| H9 | First open / session → README; cold wrap of large file | starter order; sync wrap | §109j |
| H10 | Coarse hit slots (header/tab/toolbar) thrash at boundaries | measured header_tab_cross | §109b |
| H11 | `chrome_dirty` while overlay/menu visible every poll | frame_input §108a | §109b |
| H12 | Paint-ops flatten cost vs direct GL | §107q | §109d/e |
| H13 | Toolbar/tree hover still outside retained chrome batch | demo_live after flatten | §109h |
| H14 | Gate honesty (PERF skip, idle-without-pointer, cpu=0) | §107d/§108d | §109c, §109k |
| H15 | P2 perf: overlay idle retick B7, minimap B9 | hygiene backlog | §109b/i (pull in) |

---

# Sub-tracks (strict order)

## §109a `EDITOR_PERF_BASELINE_DOGFOOD` — **queue head**

| Item | Choice |
|------|--------|
| Problem | No frozen, reproducible visible-window numbers for the real file |
| Fix | Harness `scripts/run_editor_perf_dogfood_baseline.sh`: build once, run with `MLC_GLFW_VISIBLE=1`, force open `misc/editor/demo_live.mlc` (session file or new `MLC_EDITOR_OPEN` — Decision picks one), sample idle-away / still-over-text / text-jitter / scroll / type; write report under `.tmp/` **and** paste summary table into this track |
| Gate | Script exits 0 and prints all five metrics; fails if open path is README or file missing |
| Out of scope | Optimizations |

### Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | pending |
| 1 | Red: harness absent / opens README | pending |
| 2 | Green: harness + numbers in track | pending |
| 3 | Critic | pending |

---

## §109b `EDITOR_PERF_WAKE_ON_HOVER`

| Item | Choice |
|------|--------|
| Problem | H1/H10/H11: mouse over text without move still ~32% CPU — hit-stable failed in dogfood; possible always-`chrome_dirty`, present path falling through to full paint, or blink path reshaping |
| Fix | Instrument frame reasons (`content`/`chrome`/`present`/`idle_wait` counts per second). Fix until still-over-text matches blink-only budget. Include: overlay/menu must not raise dirty every poll without visual change; hit id for text must not flap; present_only must not rebuild chrome/text layers |
| Depends on | §109a |
| Gate | Close criteria 2–3; counters: on still-over-text, `content_rebuild_count` and `chrome_rebuild_count` deltas == 0 over N seconds |
| Sabotage | Force `chrome_dirty=1` every poll → gate fails |

---

## §109c `EDITOR_PERF_GATE_HONESTY`

| Item | Choice |
|------|--------|
| Problem | H2/H14: §108d L2 cpu=0; idle gate ignores pointer; PERF baseline skips heavy path |
| Fix | Replace/augment hover L2 to require `MLC_GLFW_VISIBLE=1` + dogfood file; delete or demote cpu=0 ceiling; ensure PERF_FULL (or successor) is what “perf green” means for content budgets |
| Depends on | §109a–b |
| Gate | Sabotage restoring full content rebuild on hover fails L2 under visible load; old “cpu=0 OK” path removed or cannot pass alone |

---

## §109d `EDITOR_PERF_CONTENT_FRAME_BUDGET`

| Item | Choice |
|------|--------|
| Problem | H3/H12: scroll/type frames too expensive |
| Fix | Profile `layout_us`/`draw_us`/phase counters on content frames after §109a; cut dominant phase until close criteria 4–5. Prefer visible-line work only |
| Depends on | §109a, §109b (wake fixed so content is measurable) |
| Gate | Dogfood 4–5 + measured-then-written content-frame ceiling that fails today’s tree |

---

## §109e `EDITOR_PERF_GLYPH_LAYER_BUDGET`

| Item | Choice |
|------|--------|
| Problem | H4: text/glyph batch rebuilt on every content paint; retain only helped chrome-only |
| Fix | Retain/reuse glyph/`StaticTextLine` (or atlas) across scroll when line set unchanged; invalidate on version/zoom/wrap/theme; damage visible range on scroll |
| Depends on | §109d diagnosis (if glyphs not dominant, Decision may narrow/skip with written proof) |
| Gate | Counter `text_layer_rebuild_count` on pure scroll of unchanged doc stays flat or grows O(newly visible) only |

---

## §109f `EDITOR_PERF_SPANS_VISIBLE_ONLY`

| Item | Choice |
|------|--------|
| Problem | H5: syntax highlight / spans still pay for offscreen work |
| Fix | Highlight / span tick only visible line range (or cached by version+visible window) |
| Depends on | §109d |
| Gate | Scenario: N idle content frames after scroll → span work bounded by visible lines |

---

## §109g `EDITOR_PERF_SNAPSHOT_EDIT_COVERAGE`

| Item | Choice |
|------|--------|
| Problem | H6: many dirty paths still full `document_frame_snapshot` flatten |
| Fix | Audit remaining `frame_layout_tick_snapshot(..., 1)` / `document_to_string` call sites in editor; route known edits through apply_edit; document unavoidable full rebuilds |
| Depends on | §109d |
| Gate | Extend `edit_no_full_flatten` (or sibling) to cover typed insert/delete and paste; sabotage full flatten |

---

## §109h `EDITOR_PERF_CHROME_TREE_VISIBLE`

| Item | Choice |
|------|--------|
| Problem | H7/H13: chrome paint walks all folder rows + toolbar slots every chrome/content paint |
| Fix | Visible-row-only tree hover fills; toolbar/tree hover inside retained chrome layer; no row walk on present_only / hit-stable text |
| Depends on | §109b |
| Gate | `chrome_rebuild_count == 0` on stable text hover; tree paint row visits ≤ visible slots |

---

## §109i `EDITOR_PERF_MINIMAP_SAMPLE`

| Item | Choice |
|------|--------|
| Problem | H8/H15-B9: minimap rebuild scales with document lines |
| Fix | Sample rows to strip height (EHA-28); invalidate on version/height/zoom only |
| Depends on | §109d |
| Gate | `run_ux_minimap_rows_bounded_by_height` (or name from Decision); sabotage full-line loop |

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

Expanded from 4 sub-tracks (a–d) to **a–k**: wake-on-hover, gate honesty, glyph, spans, snapshot coverage, minimap, startup, regression suite — covering inventory H1–H15.
