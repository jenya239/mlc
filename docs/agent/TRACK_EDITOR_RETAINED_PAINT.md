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

## Status: **open** 2026-08-03 — §108a–§108c **CLOSED**; queue head **§108d `EDITOR_HOVER_CPU_GATE`** (STEP=0 Decision done; Red next)

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

## §108b `EDITOR_RETAIN_TEXT_LAYER` — **CLOSED**

### Decision (**frozen** 2026-08-03, Driver STEP=0)

Baseline after §108a: chrome-only / `layout_skip` frames still rebuild gutter+editor `StaticTextLine` lists and call `static_text_draw_lines_colored` for the text viewport every paint (`demo_live.mlc`). `text_layer_rebuild_count` already bumps when `layout_skip == 0`; chrome-only still redraws glyphs. No editor-viewport FBO in product path (FBO only in text-renderer smokes).

| Item | Choice |
|------|--------|
| Problem | Chrome-only / present-only frames still rebuild visible glyph+gutter draw work |
| Mechanism | **Retain last text+gutter draw batch** (cached `[StaticTextLine]` for gutter+editor text plus gutter fill rect params). On chrome-only / present-only frames: **replay** the retained batch (no line rebuild, no HarfBuzz/span retick). **Not** an FBO / render-to-texture for the text viewport |
| Why not FBO | No product FBO plumbing in `demo_live`; batch reuse fits existing `EditorPaintOp` / `StaticTextLine` / `text_layer_rebuild_count` (§107q/§108a); FBO deferred unless batch reuse proves insufficient |
| Invalidate | `document.version`, scroll_x/scroll_y, zoom/font size, wrap width, text/gutter theme RGB, viewport rect size change |
| Who rebuilds | Content frames (`content_dirty` / `layout_skip == 0`) rebuild + replace retained batch and bump `text_layer_rebuild_count`. Chrome-only / present-only must **not** bump `text_layer_rebuild_count` |
| Module touch | `misc/editor/demo_live.mlc` (retain/replay around text+gutter draw); small helper under `misc/editor/ux/` if extractable; counters already in `ui/perf.mlc` |
| Gate | `run_ux_hover_no_text_layer_rebuild`: after one content frame, N chrome-only / stable-hit hover frames → `text_layer_rebuild_count` stays at the post-content value (delta == 0) |
| Sabotage | Drop / clear retained batch every frame (force rebuild on chrome-only) → gate fails |
| REG | no |
| Out of scope | FBO text viewport; chrome compose layers (§108c); CPU ceilings (§108d); SceneNode chrome |

### Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** 2026-08-03 |
| 1 | Red: `run_ux_hover_no_text_layer_rebuild_red.sh` | **done** 2026-08-03 — FAIL exit 1 (no retain/replay; `editor_lines` rebuilt each paint) |
| 2 | Green + `run_ux_gate` ×2 + `dev_gate_fast` | **done** 2026-08-03 — `text_layer_batch` retain/replay; green `ux_ok`; UX 134/134 ×2 |
| 3 | Critic | **done** 2026-08-03 — Decision sabotage was not load-bearing (demo invalidate-before-replay still green); gate strengthened (arch: no invalidate immediately before document-path `can_replay`); sab1/sab2 fail; UX 134/134 ×2 |

---

## §108c `EDITOR_COMPOSE_PRESENT` — **CLOSED**

### Decision (**frozen** 2026-08-03, Driver STEP=0)

Baseline after §108b: text+gutter batch is retained across chrome-only frames, and idle caret blink already early-continues with `solid_renderer_*` overlay rects (`demo_live.mlc` ~938–988) without layout. Gaps: (1) chrome `EditorPaintOp` lists are still rebuilt every chrome/content paint (`editor_paint_ops_chrome_bands` / tab / nav flatten) — no chrome retain + no `chrome_rebuild_count`; (2) present_only is an ad-hoc branch, not a named compose step with a gate that fails if chrome rebuild runs; (3) chrome-only still redraws chrome from scratch even when only overlay carets should change.

| Item | Choice |
|------|--------|
| Problem | Present / chrome-only still means «rebuild chrome paint ops + full present path»; present_only caret is ungated |
| Layers | **text** = retained §108b `TextLayerBatch`; **chrome** = `EditorPaintOp` chrome bands + tab/nav fills (+ chrome label draws already in the paint block); **overlay** = caret rects / dirty-close / context menu |
| Compose order | text → chrome → overlay (single immediate GL path; **no** FBO / render-to-texture) |
| present_only | Overlay blit only (existing caret blink XOR erase/draw on `last_caret_rects`). Must **not** rebuild chrome ops, must **not** bump `chrome_rebuild_count`, must **not** invalidate / rebuild text batch |
| chrome_dirty | Rebuild chrome layer + replay text batch; overlay cache may refresh. No content / text-batch rebuild |
| content_dirty | Rebuild text + chrome; refresh overlay caret cache |
| Mechanism | **Retain last chrome `[EditorPaintOp]` batch** (bands + tab slots + nav fills that §107q already builds) across present_only frames — mirror `text_layer_batch`. Replay on present_only if a full frame is forced; preferred path stays early-continue overlay-only. **Not** FBO |
| Why not FBO | Same as §108b; overlay blit already works without a texture; FBO deferred |
| Counter | `chrome_rebuild_count` on `EditorPerfCounters` — bump only when chrome op lists are freshly built (not on replay / present_only) |
| Gate | `run_ux_present_only_caret_no_chrome_rebuild`: after chrome layer exists, N present_only caret blink ticks → `chrome_rebuild_count` delta == 0; one chrome_dirty → count bumps once. Plus small unit asserting compose order text→chrome→overlay |
| Sabotage | Call chrome band builder / bump chrome rebuild on the present_only path → gate fails |
| Module touch | `misc/editor/demo_live.mlc`, `misc/editor/ui/perf.mlc`, small helper under `misc/editor/ux/` (chrome layer batch), gate + unit under `scripts/` / `ux_scenarios/` / `tests/` |
| REG | no |
| Out of scope | FBO; SceneNode chrome; finishing remaining §107q paint regions; numeric CPU ceilings (§108d) |
| Depends on | §108b |

### Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** 2026-08-03 |
| 1 | Red: `run_ux_present_only_caret_no_chrome_rebuild_red.sh` | **done** 2026-08-03 — FAIL exit 1 (no `chrome_rebuild_count`; no chrome retain; green gate absent) |
| 2 | Green + unit compose order + `run_ux_gate` ×2 + `dev_gate_fast` | **done** 2026-08-03 — `chrome_layer_batch` + `chrome_rebuild_count`; present_only arch; green `ux_ok`; UX 135/135 ×2 |
| 3 | Critic | **done** 2026-08-03 — sab1 present_only `note_chrome_rebuild` fails; sab2 `can_replay→0` fails; UX 135/135 ×2 |

---

## §108d `EDITOR_HOVER_CPU_GATE` — **queue head**

| Item | Choice |
|------|--------|
| Problem | No behavioural gate for hover CPU; §106/`idle_cpu_budget_stable` measure idle without pointer motion |
| Fix | Behavioural L1/L2: idle (mouse away), hover jitter (same hit), scroll — document measured ceilings after Green, then write them (honesty rule as §102g/§107d). Sabotage: force full content paint on hover → must exceed ceiling / fail counter gate |
| Depends on | §108a–c (ceilings meaningless before retain) |
| Note | May absorb / unblock the hover half of §107r; do not invent ceilings before measure |

---

## After §108

Resume **§107q** from Critic q3 / disk-verify (or Green q4 if Critic q3 already closed by then) → q5 → q6 → **§107r**, then §103a → §104 Wave 2.
