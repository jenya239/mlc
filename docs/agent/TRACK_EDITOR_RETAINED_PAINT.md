# Track: Editor retained paint / damage (§108)

Parent: [../PLAN.md](../PLAN.md) §108.
Authorized **2026-08-03** as **queue head**, ahead of remaining §107q/§107r
(user: dogfood still «чудовищно тормозит» on hover; measured idle-away ~0.5%,
hover ~77% — `layout_skip` skips HarfBuzz but still full-frame paints glyphs/chrome).
Implements the present-pipeline already frozen in
[GUI_ARCHITECTURE.md](../GUI_ARCHITECTURE.md) §2/§6/§7 and
[misc/editor/ARCHITECTURE.md](../../misc/editor/ARCHITECTURE.md)
(`invalidate → dirty views → layout → scene fragments → flatten → render`).
Design note: Cursor plan `editor_paint_damage` (2026-08-03).

## Status: **open** 2026-08-03 — §108a **CLOSED**; queue head **§108b `EDITOR_RETAIN_TEXT_LAYER`** (Driver STEP=0 Decision next)

Strict order: §108a → §108b → §108c → §108d. §107q (q4–q6) and §107r resume
**after** §108d closes (or on a new explicit override). §107q q3 Critic remains
disk-blocked unfinished — do not silently mark it closed.

## Why

Mature UI stacks do **not** rebuild the whole framebuffer on every mousemove.
They separate invalidate → layout → paint-dirty → present, and keep retained
layers. MLC still immediate-paints the full window on hover.

## Global non-goals (binding)

- **No** chrome migration onto `SceneNode` (GUI_ARCHITECTURE Deviation).
- **No** Flutter/Qt/Skia dependency.
- **No** treating `SwapInterval(1)` as a perf win (§106).
- **No** wholesale `demo_live.mlc` rewrite in one commit — one sub-track / layer at a time.
- **No** `compiler/**` changes unless a Decision re-freeze says otherwise.

## Precedent in-tree

- `pointer_dirty` / `layout_skip` / idle caret overlay (§106) — layout skip only.
- `EditorPaintOp` (§107q q1–q3) — region ops, still flattened every frame.
- Terminal `vterm_damage_count` cache (§107k) — domain damage → reuse paint cache.
- Document `version` (§107e) — content invalidation key.

---

## §108a `EDITOR_DIRTY_TAXONOMY` — **CLOSED** 2026-08-03 (Critic)

### Decision (**frozen** 2026-08-03, Driver STEP=0)

Baseline today (`app/frame_input.mlc` + `demo_live.mlc`): mouse move → `pointer_dirty=1` → still runs the full paint block (`layout_skip` only skips HarfBuzz/wrap retick). `context_menu_visible` / `overlay_visible` force `content_dirty=1` (EHA-26 / §107 B7). No `content_rebuild_count` / `text_layer_rebuild_count` yet.

| Item | Choice |
|------|--------|
| Problem | `content_dirty` + `pointer_dirty` cannot express chrome-only vs text vs present-only; hover still full-paints glyphs+chrome |
| Frame classes | **`content_dirty`**: document edit / scroll / zoom / wrap / resize / key/text/drop that changes buffer or viewport layout. **`chrome_dirty`**: tabs/tree/toolbar/breadcrumb/scrollbar-thumb hover style, panel open that only restyles chrome. **`present_only`**: caret blink / overlay blit with no chrome restyle. Mutually exclusive priority when combining: content > chrome > present_only |
| Hit-stable no-frame | Compute a stable **chrome hit-style id** (string or i32 enum covering none/tab/tool/tree_row/breadcrumb/nav/scrollbar_*/overlay_item — same regions that already call `editor_ux_chrome_hover_draw_entry`). Pointer move with **unchanged** id → schedule **no** editor frame (GLFW cursor update only; both dirty flags stay 0). Id change → `chrome_dirty=1` only |
| Overlay / context menu | Open/visible must **not** raise `content_dirty` — `chrome_dirty` or `present_only` only (fixes the §106 residual / B7) |
| `pointer_dirty` | **Green:** keep as raw mouse-moved; `demo_live` hit-filters via `editor_ux_chrome_hit_pointer_plan` before idle (stable id → clear `pointer_dirty`) |
| Module touch | `misc/editor/app/frame_input.mlc`, `misc/editor/demo_live.mlc` (loop branch + idle wait), hit-id helper near `ux/chrome_hover.mlc` (or small sibling), `misc/editor/ui/perf.mlc` (two counters) |
| Counters | `content_rebuild_count`, `text_layer_rebuild_count` on `EditorPerfCounters` — bumped only on the corresponding rebuild paths (Green wires them; Red gate reads them) |
| Gate | `run_ux_hover_stable_hit_no_content_frame`: N pointer moves over the same chrome hit → `content_rebuild_count == 0` and `text_layer_rebuild_count == 0`; move that changes hover target → chrome path once (no content bump) |
| Sabotage | Force `content_dirty = 1` on every pointer move → gate fails |
| REG | no |
| Out of scope | FBO / retained batches (§108b); compose (§108c); numeric CPU ceilings (§108d); SceneNode chrome |

### Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** 2026-08-03 |
| 1 | Red: `run_ux_hover_stable_hit_no_content_frame_red.sh` | **done** 2026-08-03 — FAIL exit 1 (`pointer_dirty` always-on; no counters/green gate) |
| 2 | Green + `run_ux_gate` ×2 + `dev_gate_fast` | **done** 2026-08-03 — hit-stable + chrome_dirty + counters; green `ux_ok`; UX 133/133 ×2 |
| 3 | Critic | **done** 2026-08-03 — Decision sabotage was not load-bearing; gate strengthened (mouse-move ≠ `content_dirty`); sab1/sab2 fail; UX 133/133 ×2 |

---

## §108b `EDITOR_RETAIN_TEXT_LAYER` — **queue head**

| Item | Choice |
|------|--------|
| Problem | Even chrome-only frames rebuild visible glyph/gutter draw work |
| Fix | Retain last text+gutter `EditorPaintOp` batch **or** an FBO for the text viewport across chrome-only / present-only frames; invalidate on `document.version`, scroll, zoom, wrap, text theme. Prefer batch reuse first if FBO plumbing is heavy — Decision at STEP=0 must pick one and stick |
| Gate | `run_ux_hover_no_text_layer_rebuild`: after one content frame, N hover frames → `text_layer_rebuilds == 0` |
| Depends on | §108a |
| Sabotage | Drop retained batch every frame → gate fails |

---

## §108c `EDITOR_COMPOSE_PRESENT`

| Item | Choice |
|------|--------|
| Problem | Present still means «re-run the whole paint block» |
| Fix | Compose three layers: text+gutter / chrome / overlay. Caret blink stays on the present_only path already started in §106 idle overlay |
| Gate | `run_ux_present_only_caret_no_chrome_rebuild` + unit on compose order |
| Depends on | §108b |

---

## §108d `EDITOR_HOVER_CPU_GATE`

| Item | Choice |
|------|--------|
| Problem | No behavioural gate for hover CPU; §106/`idle_cpu_budget_stable` measure idle without pointer motion |
| Fix | Behavioural L1/L2: idle (mouse away), hover jitter (same hit), scroll — document measured ceilings after Green, then write them (honesty rule as §102g/§107d). Sabotage: force full content paint on hover → must exceed ceiling / fail counter gate |
| Depends on | §108a–c (ceilings meaningless before retain) |
| Note | May absorb / unblock the hover half of §107r; do not invent ceilings before measure |

---

## After §108

Resume **§107q** from Critic q3 / disk-verify (or Green q4 if Critic q3 already closed by then) → q5 → q6 → **§107r**, then §103a → §104 Wave 2.
