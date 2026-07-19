# Track: Editor UX backlog — atomic items from TDD/UX review

Parent: [../PLAN.md](../PLAN.md) §46. Source: [archive/tracks/TRACK_EDITOR_CLEAN_ARCHITECTURE](../archive/tracks/TRACK_EDITOR_CLEAN_ARCHITECTURE.md)
(§45) STEP=3. Full detail/evidence per item:
[`mlc-support/responses/editor_tdd_ux_20260717_114221.md`](../../../mlc-support/responses/editor_tdd_ux_20260717_114221.md)
(Opus review, 2026-07-17, $2.24).

## Status: **active** (2026-07-19) — #1…#1d+#2+#3+#4+#5+#6+#7+#8+#9+#10+#11+#12+#13+#14+#15+#16+#17+#18+#19+#20+#21+#23+#24+#25+#26+#27+#28+#29+#30 done; `#31a EDITOR_COLUMN_PIXEL_MODEL_FIX` **active** (STEP=2 next); `#31`–`#35` queued (#22 superseded)

## How to use this backlog

This is **not** one track — each numbered item below, when picked up by a
Driver/Planner turn, gets its own Decision→Driver→Driver→Critic cycle same
as §36-45, and its own `TRACK_EDITOR_<NAME>.md` file created at that time
(do not pre-create all 30 now — matches how the §21-29 compiler backlog in
`agent/CONTINUITY.md` is handled: one line here, a real TRACK file only when
opened). **Standing discipline applies to every item** (`../GUI_UX_TESTING.md`
§"Standing discipline"): scenario-first, same commit as implementation, name
the scenario in SESSION `done`.

Order below is fixed (user value first, foundational/rare-path last per the
review's §4 rationale) — Planner picks the next **pending** item in order,
does not jump ahead unless the current one is blocked (same rule as the
compiler backlog).

**Priority rule (2026-07-17, user directive — see `../EDITOR.md` §Product
target):** feature ceiling is Sublime Text, not VS Code/IDE. Any item that
trades stability (crash/data-loss safety) or speed (startup, frame latency,
large-file responsiveness) against feature breadth loses — pull it forward
out of strict order; log the jump in SESSION `done` same as a blocker-skip.
`#1d` below is the first such pull-forward (large-file per-frame full
restringify is a direct speed regression, not a nice-to-have). `#31a` is the
second (2026-07-19): a coordinate-model correctness bug (selection/caret/
click position drift from actual rendered glyphs), not cosmetic polish.

| # | Track name | Scope (one line) | Gate scenario | Size | Status |
|---|-----------|-------------------|----------------|------|--------|
| 1 | `EDITOR_DEMO_ORCHESTRATOR` | Replace `demo_live.main()` inline command/edit/tree-hit logic with calls to existing tested `ux/edit_apply`/`ux/scroll`/`ux/tab_strip`/`ux/tree_hit` — stop duplicating a second untested implementation | existing gates (`ux_scenarios`+`tests`) + `demo_live_fs_compile` unchanged | L | **done** (2026-07-18) Critic OK — [archive/tracks/TRACK_EDITOR_DEMO_ORCHESTRATOR](../archive/tracks/TRACK_EDITOR_DEMO_ORCHESTRATOR.md) |
| 1b | `EDITOR_LIVE_SOLARIZED_TEXT` | Found 2026-07-17 (screenshot audit): live demo used dimmed syntax **background** tint + white glyphs; dark chrome workaround for stale “glyph shader is white” comment. Delivered: per-tag `StaticTextLine` rgba + Solarized Light chrome; L2 `syntax_glyph_color_matches_theme` | `ux_ok syntax_glyph_color_matches_theme` + `demo_live_fs_compile_ok` | M | **done** — [archive/tracks/TRACK_EDITOR_LIVE_SOLARIZED_TEXT](../archive/tracks/TRACK_EDITOR_LIVE_SOLARIZED_TEXT.md) |
| 1c | `EDITOR_STALE_HELP_TEXT` | Found 2026-07-17: in-app README/guide/changelog described tree expand/collapse; live UI is breadcrumbs + back/forward (§38). Delivered copy-only fix in `demo_live` sample helpers + header | `demo_live_fs_compile_ok` | S | **done** — [archive/tracks/TRACK_EDITOR_STALE_HELP_TEXT](../archive/tracks/TRACK_EDITOR_STALE_HELP_TEXT.md) |
| 1d | `EDITOR_LARGE_FILE_NO_FULL_STRINGIFY` (pulled forward from #22, 2026-07-17 priority rule above) | Remove per-frame full-buffer `document_to_string` in demo — direct frame-latency regression on large files, Sublime-parity blocker. Delivered: `document_frame_snapshot` + demo_live reuse; L2 `large_file_no_full_stringify` | `ux_ok large_file_no_full_stringify` + `demo_live_fs_compile_ok` | M | **done** — [archive/tracks/TRACK_EDITOR_LARGE_FILE_NO_FULL_STRINGIFY](../archive/tracks/TRACK_EDITOR_LARGE_FILE_NO_FULL_STRINGIFY.md) |
| 2 | `EDITOR_KEYBOARD_NAV_WIRE` | Wire arrow keys/Home/End/PageUp/PageDown (model exists in `document/navigation.mlc`, not called from demo) | `arrow_keys_move_caret`, `home_end_caret` | M | **done** — [archive/tracks/TRACK_EDITOR_KEYBOARD_NAV_WIRE](../archive/tracks/TRACK_EDITOR_KEYBOARD_NAV_WIRE.md) |
| 3 | `EDITOR_WORD_BOUNDARIES` | Word-boundary helper in `document/`; Ctrl+Left/Right, Ctrl+Backspace/Delete | `ctrl_arrow_word_jump`, `ctrl_backspace_deletes_word` | M | **done** — [archive/tracks/TRACK_EDITOR_WORD_BOUNDARIES](../archive/tracks/TRACK_EDITOR_WORD_BOUNDARIES.md) |
| 4 | `EDITOR_MOUSE_WORD_LINE_SELECT` | Double-click selects word, triple-click selects line, click timing via `UxDriver` | `double_click_selects_word`, `triple_click_selects_line` | M | **done** — [archive/tracks/TRACK_EDITOR_MOUSE_WORD_LINE_SELECT](../archive/tracks/TRACK_EDITOR_MOUSE_WORD_LINE_SELECT.md) |
| 5 | `EDITOR_SHIFT_CLICK_EXTEND` | Shift-click extends selection from anchor | `shift_click_extends_selection` | S | **done** — [archive/tracks/TRACK_EDITOR_SHIFT_CLICK_EXTEND](../archive/tracks/TRACK_EDITOR_SHIFT_CLICK_EXTEND.md) |
| 6 | `EDITOR_UNDO_COALESCE` | Group consecutive typing into one undo step (currently every keystroke = separate undo) | `typing_coalesces_into_one_undo` (L1.5) | M | **done** — [archive/tracks/TRACK_EDITOR_UNDO_COALESCE](../archive/tracks/TRACK_EDITOR_UNDO_COALESCE.md) |
| 7 | `EDITOR_FIND` | Find + next/prev, highlight matches in visible range | `find_highlights_matches` | M | **done** — [archive/tracks/TRACK_EDITOR_FIND](../archive/tracks/TRACK_EDITOR_FIND.md) |
| 8 | `EDITOR_GOTO_LINE` | Ctrl+G → line number → scroll+caret | `goto_line_scrolls_caret` | S | **done** — [archive/tracks/TRACK_EDITOR_GOTO_LINE](../archive/tracks/TRACK_EDITOR_GOTO_LINE.md) |
| 9 | `EDITOR_DRAG_AUTOSCROLL` | Drag past viewport edge auto-scrolls (multi-frame stateful) | `drag_past_edge_autoscrolls` (L1.5) | M | **done** — [archive/tracks/TRACK_EDITOR_DRAG_AUTOSCROLL](../archive/tracks/TRACK_EDITOR_DRAG_AUTOSCROLL.md) |
| 10 | `EDITOR_AUTO_INDENT` | Enter preserves leading indent of current line | `enter_keeps_indent` | S | **done** — [archive/tracks/TRACK_EDITOR_AUTO_INDENT](../archive/tracks/TRACK_EDITOR_AUTO_INDENT.md) |
| 11 | `EDITOR_CURRENT_LINE_HL` | Highlight current line in draw report | `current_line_highlight_draws` (L2) | S | **done** — [archive/tracks/TRACK_EDITOR_CURRENT_LINE_HL](../archive/tracks/TRACK_EDITOR_CURRENT_LINE_HL.md) |
| 12 | `EDITOR_CRLF_PRESERVE` | Detect CRLF/LF on open, preserve style on save | `crlf_preserved_on_save` (L0) | M | **done** — [archive/tracks/TRACK_EDITOR_CRLF_PRESERVE](../archive/tracks/TRACK_EDITOR_CRLF_PRESERVE.md) |
| 13 | `EDITOR_ENCODING_GUARD` | BOM detect + UTF-8 validity + refuse binary files | `binary_file_refused`, `utf8_bom_detected` | M | **done** — [archive/tracks/TRACK_EDITOR_ENCODING_GUARD](../archive/tracks/TRACK_EDITOR_ENCODING_GUARD.md) |
| 14 | `EDITOR_REPLACE` | Replace-one / replace-all on top of Find | `replace_one_and_all` | M | **done** — [archive/tracks/TRACK_EDITOR_REPLACE](../archive/tracks/TRACK_EDITOR_REPLACE.md) |
| 15 | `EDITOR_TAB_REORDER` | Drag-to-reorder tabs in strip | `drag_reorder_tabs` | M | **done** — [archive/tracks/TRACK_EDITOR_TAB_REORDER](../archive/tracks/TRACK_EDITOR_TAB_REORDER.md) |
| 16 | `EDITOR_FONT_ZOOM` | Ctrl+wheel changes font size (+state), reflows wrap | `ctrl_wheel_font_zoom` | S | **done** — [archive/tracks/TRACK_EDITOR_FONT_ZOOM](../archive/tracks/TRACK_EDITOR_FONT_ZOOM.md) |
| 17 | `EDITOR_HORIZONTAL_SCROLL` | Shift+wheel scrolls `scroll_offset_x`, hit-test accounts for it | `shift_wheel_scrolls_horizontal` | S | **done** — [archive/tracks/TRACK_EDITOR_HORIZONTAL_SCROLL](../archive/tracks/TRACK_EDITOR_HORIZONTAL_SCROLL.md) |
| 18 | `EDITOR_BRACKET_AUTOCLOSE` | Auto-close `()[]{}""`, skip-over closing char | `typing_paren_autocloses` | M | **done** — [archive/tracks/TRACK_EDITOR_BRACKET_AUTOCLOSE](../archive/tracks/TRACK_EDITOR_BRACKET_AUTOCLOSE.md) |
| 19 | `EDITOR_COMMENT_TOGGLE` | Ctrl+/ line-comment toggle (by language from status bar) | `toggle_line_comment` | S | **done** — [archive/tracks/TRACK_EDITOR_COMMENT_TOGGLE](../archive/tracks/TRACK_EDITOR_COMMENT_TOGGLE.md) |
| 20 | `EDITOR_SESSION_CARET_RESTORE` | Session stores caret+scroll per tab, restores on reopen | `session_restore_caret` (L0+L1) | S | **done** — [archive/tracks/TRACK_EDITOR_SESSION_CARET_RESTORE](../archive/tracks/TRACK_EDITOR_SESSION_CARET_RESTORE.md) |
| 21 | `EDITOR_DIRTY_CLOSE_L1` | L1 scenario for dirty-close overlay discard/cancel click (currently L0-only) | `dirty_close_overlay_click` | S | **done** — [archive/tracks/TRACK_EDITOR_DIRTY_CLOSE_L1](../archive/tracks/TRACK_EDITOR_DIRTY_CLOSE_L1.md) |
| 22 | `EDITOR_LARGE_FILE_NO_FULL_STRINGIFY` | **superseded by `#1d` above (2026-07-17 pull-forward)** — do not do twice | — | — | superseded |
| 23 | `EDITOR_MULTI_CURSOR` | `[EditorSelection]` model, Ctrl+click adds cursor, synchronized edit | `multi_cursor_model` + L1 | L | **done** (2026-07-19) Critic OK — [archive/tracks/TRACK_EDITOR_MULTI_CURSOR](../archive/tracks/TRACK_EDITOR_MULTI_CURSOR.md) |
| 24 | `EDITOR_TRAILING_WS_VIZ` | Visualize trailing whitespace | `trailing_ws_visualized` (L2) | S | **done** (2026-07-19) Critic OK — [archive/tracks/TRACK_EDITOR_TRAILING_WS_VIZ](../archive/tracks/TRACK_EDITOR_TRAILING_WS_VIZ.md) |
| 25 | `EDITOR_CONTEXT_MENU` | Right-click → cut/copy/paste menu | `context_menu_opens` | M | **done** — [archive/tracks/TRACK_EDITOR_CONTEXT_MENU](../archive/tracks/TRACK_EDITOR_CONTEXT_MENU.md) |
| 26 | `EDITOR_INDENT_MODEL` | Tabs-vs-spaces, indent width, Tab/Shift-Tab | `indent_width_config` (L0) + L1 | M | **done** — [archive/tracks/TRACK_EDITOR_INDENT_MODEL](../archive/tracks/TRACK_EDITOR_INDENT_MODEL.md) |
| 27 | `EDITOR_DROP_FILE` | Runtime GLFW drop-callback → open file (needs ABI) | `drop_file_opens_tab` | M | **done** — [archive/tracks/TRACK_EDITOR_DROP_FILE](../archive/tracks/TRACK_EDITOR_DROP_FILE.md) |
| 28 | `EDITOR_WIDE_CHAR_COLUMNS` | wcwidth-style column width for emoji/CJK (only if taken on) | `wide_char_column_width` (L0) | M | **done** — [archive/tracks/TRACK_EDITOR_WIDE_CHAR_COLUMNS](../archive/tracks/TRACK_EDITOR_WIDE_CHAR_COLUMNS.md) |
| 29 | `LANG_RECORD_UPDATE` (compiler, not editor) | Functional record-update syntax (`{ ...state, field: v }`); removes copy-all-fields boilerplate across `misc/editor`/`misc/gui/scene` | Tier B + self-host | M | **done** — [archive/tracks/TRACK_LANG_RECORD_UPDATE](../archive/tracks/TRACK_LANG_RECORD_UPDATE.md) |
| 30 | `EDITOR_UX_L1_BACKFILL` | Backfill L1 scenarios for §38/§40/§42/§43 (closed on unit+compile-gate only, per review §2.1) | 4 new `ux_scenarios/*` | M | **done** — [archive/tracks/TRACK_EDITOR_UX_L1_BACKFILL](../archive/tracks/TRACK_EDITOR_UX_L1_BACKFILL.md) |
| 31a (pulled forward, 2026-07-19 priority rule — coordinate-model correctness bug) | `EDITOR_COLUMN_PIXEL_MODEL_FIX` | Found 2026-07-19 (user: selection background drifts off the letters, lines don't highlight to the end). Root cause #1: `editor_app_char_width()` (`app/chrome.mlc`) is a **hand-tuned constant** (`7`, changed from `9` this same track), unrelated to the real glyph advance HarfBuzz reports (`text_shaping_glyph_advance_at`, used by the actual glyph draw in `ui/static_text.mlc:147`) for the loaded font at the configured `pixel_size` (`13`) — every `column * char_width` computation (selection highlight, caret x, `nav_offset_at_point` click hit-test — self-documented as `"Mono hit-test"` — gutter, wrap width) drifts from the real rendered glyph position, worse the further right on the line. Root cause #2: multi-line selection's full-line fallback (`demo_live.mlc:1721`) sets `col_end` to `line_text_at(...).byte_size()` — **byte count**, not the character/column count every other `column` value in this codebase uses (confirmed unit by `line_index_unit.mlc`) — wrong on any non-ASCII line, over/under-extending that line's highlight. Fix: derive `char_width` from the real font advance once at load/zoom time instead of a guess; fix the byte_size()→column unit bug | `selection_highlight_matches_glyph_x`; `selection_full_line_uses_display_columns` | L | **active** — [TRACK_EDITOR_COLUMN_PIXEL_MODEL_FIX](TRACK_EDITOR_COLUMN_PIXEL_MODEL_FIX.md) STEP=2 next |
| 31 | `EDITOR_TAB_STRIP_CLASSIC_LAYOUT` | Found 2026-07-19 (live-demo audit): `tab_strip_title_rect` (`ux/tab_strip.mlc`) divides strip width equally across all open tabs, and `demo_live` draws each title with `push_line` unclamped to that slot — with >5 tabs, labels overflow into neighboring slots (garbled overlap). Replace stretch-to-fill with intrinsic/max-width per tab + ellipsis truncation, no overflow regardless of tab count | new L1/L2: tab rects never overlap for any `tab_count`; long name truncates with ellipsis | M | pending |
| 32 | `EDITOR_CHROME_HOVER_STATE` | Found 2026-07-19: `misc/gui/input.mlc` already exposes `gui_is_hovered`/`GuiButton` (`misc/gui/button.mlc`), but `demo_live` toolbar/tab-strip/tree-row rendering hand-rolls flat-colored rects and never calls it — zero hover feedback anywhere in chrome. Wire existing primitive into those three draw paths only; no new framework component needed | new L2 draw-report: hovered rect color ≠ non-hovered rect color, same frame | M | pending |
| 33 | `EDITOR_TREE_EXPAND_COLLAPSE` | Found 2026-07-19 (user directive) — **revises** frozen §38 Decision (`../archive/tracks/TRACK_EDITOR_FOLDER_BROWSER.md`): flat breadcrumb-only listing was intentional then, product owner now wants a real expandable tree. Model layer already supports it unused (`file_tree_expand`/`node.expanded`/`folder_tree_browser_rows` in `workspace/file_tree.mlc`). New click contract: single click on a directory row toggles expand/collapse in place (immediate visible feedback, replaces today's silent "arm, wait for 2nd click within 25 frames" state — the concrete cause of "click/double-click feel unstable"); single click on a file row still opens it; double-click on a directory row still re-roots breadcrumb nav (`folder_click_enter_dir`, unchanged) | new L1: `tree_row_click_toggles_expand`; keep `tree_click_opens_tab` green | M | pending |
| 34 | `EDITOR_SYNTAX_HIGHLIGHT_MLC_RICHER` | Found 2026-07-19: `syntax/highlighter.mlc` is a 3-tag stub — keyword/string/comment only, MLC-only by design (no tree-sitter, frozen non-goal). No number-literal, builtin-type (`i32`/`bool`/`Shared`/…), or operator tag, so real `.mlc` source reads mostly monochrome. Non-`.mlc` buffers (`.cpp`/`.md`/…) have zero rules at all and render fully flat — decide explicitly (scope at Decision step) whether that stays a documented limitation or gets a minimal generic fallback tag set. Add number/type/operator tags for `.mlc` at minimum | extend `syntax_glyph_color_matches_theme` for new tags | M | pending |
| 35 | `EDITOR_MINIMAP` | User question 2026-07-19: Sublime-style minimap feasibility. Existing glyph-shaping/text-renderer pipeline can render real reduced-scale text (not just color blocks), so it's a second small-scale render pass + viewport-indicator overlay + click/drag-to-scroll hit-test, not new rendering tech — main risk is re-render cost on large files, must reuse `document_frame_snapshot` caching from `#1d` rather than reshaping the full buffer every frame. Previously listed under non-goals as "separate later queue" (§ below) — promoted to a numbered item, still last in order (lowest value-per-cost of this batch) | new L2: minimap draws once per edit, not per frame, on an unchanged buffer | L | pending |

Items 1-2 are the required foundation per the review: without #1, demo and
model diverge (every new UX behavior has to be fixed twice, once in `ux/*`
and once inline in `demo_live`); without #2, keyboard navigation is dead in
the live app despite the model already supporting it.

## Explicit non-goals (from review §5)

- No big-bang editor rewrite — #1 is a behavior-preserving refactor onto
  existing tested `ux/*`, not "rewrite `main` clean".
- No second scene/widget framework — chrome stays on `shell_panels`/
  `EditorAppState` (frozen `GUI_ARCHITECTURE.md` §"Live editor chrome vs
  Scene"); do not fold tabs/tree into `SceneNode`.
- Not chasing 100% VS Code parity — ceiling is **Sublime Text** (2026-07-17
  user directive, `../EDITOR.md` §Product target); block-selection, inline
  widgets, LSP/debugger stay a separate later queue, do not block #1-30.
  Multi-cursor is `#23` (done); minimap is `#35` (both promoted out of this
  bucket, no longer non-goals). Where feature breadth and stability/speed
  conflict, stability/speed wins (priority rule above).
- No accessibility tree before a real a11y story exists for GL-rendered
  text (`SemanticNode` stays a stub, frozen non-goal). RTL/bidi: non-goal.
  IME: non-goal v1 (ABI only exposes committed UTF-8, no preedit without a
  new GLFW bridge — a separate design track if ever taken on, not "polish").
- Do not close any of #1-28 on unit-only green — Standing discipline applies.
