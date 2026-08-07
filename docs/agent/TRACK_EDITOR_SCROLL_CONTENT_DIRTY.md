# §111a0 `EDITOR_SCROLL_CONTENT_DIRTY` — **CLOSED** 2026-08-07

Authorized **2026-08-07** by user dogfood: scroll/caret/nav glitch; crooked
lines; minimap not dragging. Preempted §104-6 and §111a study until CLOSED.

## Status: **closed** (Critic OK) 2026-08-07

### Decision (frozen 2026-08-07)

| Choice | Freeze |
|--------|--------|
| Root cause | During minimap/scrollbar **drag**, `frame_input` sets only `pointer_dirty`; `demo_live` sets `layout_skip=1` then changes `scroll_offset_*` without clearing skip → `visual_rows` stay cached; §110e glyph adjust desyncs caret/hit |
| Invariant | Any live change to `scroll_offset_y` or `scroll_offset_x` ⇒ `content_dirty=1`, `layout_skip=0`, `cached_visual_rows_valid=0` **before** visible-row collect in the same frame |
| Helper | `misc/editor/app/scroll_frame.mlc`: `EditorScrollFrameFlags` + `editor_scroll_frame_note_changed` + `editor_scroll_offsets_changed` + `editor_scroll_frame_after_offsets` |
| Wire | `editor_live_note_scroll_changed` in `demo_live.mlc` on minimap click/drag, content scrollbar Y/X, wheel editor, session restore, dogfood scroll, text autoscroll |
| Gate | `scripts/run_editor_scroll_content_dirty.sh` + scenario `scroll_content_dirty_forces_collect` |
| Sabotage | Helper no-op → scenario fails (`content_dirty`); red-after `already present` |
| Out of scope | Full `EditorShellLayout` (§111b+); rename demo_live; rollback §110e API; §104-6 |

### Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** 2026-08-07 |
| 1 | Red: no helper / harness | **done** — exit 1 until green present |
| 2 | Green: helper + demo_live wire + harness | **done** — green ok; demo_live wire ≥4 sites |
| 3 | Critic | **done** — sabotage exit 1; green restore ok; red-after already present |

### Path note

Precursor to §111a inventory — scroll/layout_skip class documented. Resume
§111a study or §104-6 per CONTINUITY after this close.
