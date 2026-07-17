# Track: Editor UX backlog — atomic items from TDD/UX review

Parent: [../PLAN.md](../PLAN.md) §46. Source: [archive/tracks/TRACK_EDITOR_CLEAN_ARCHITECTURE](../archive/tracks/TRACK_EDITOR_CLEAN_ARCHITECTURE.md)
(§45) STEP=3. Full detail/evidence per item:
[`mlc-support/responses/editor_tdd_ux_20260717_114221.md`](../../../mlc-support/responses/editor_tdd_ux_20260717_114221.md)
(Opus review, 2026-07-17, $2.24).

## Status: **active** (2026-07-18) — #1 done; **#1b `EDITOR_LIVE_SOLARIZED_TEXT` opened** (STEP=0 done)

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
restringify is a direct speed regression, not a nice-to-have).

| # | Track name | Scope (one line) | Gate scenario | Size | Status |
|---|-----------|-------------------|----------------|------|--------|
| 1 | `EDITOR_DEMO_ORCHESTRATOR` | Replace `demo_live.main()` inline command/edit/tree-hit logic with calls to existing tested `ux/edit_apply`/`ux/scroll`/`ux/tab_strip`/`ux/tree_hit` — stop duplicating a second untested implementation | existing gates (`ux_scenarios`+`tests`) + `demo_live_fs_compile` unchanged | L | **done** (2026-07-18) Critic OK — [archive/tracks/TRACK_EDITOR_DEMO_ORCHESTRATOR](../archive/tracks/TRACK_EDITOR_DEMO_ORCHESTRATOR.md) |
| 1b | `EDITOR_LIVE_SOLARIZED_TEXT` | Found 2026-07-17 (screenshot audit): live demo used dimmed syntax **background** tint + white glyphs; dark chrome workaround for stale “glyph shader is white” comment. Delivered: per-tag `StaticTextLine` rgba + Solarized Light chrome; L2 `syntax_glyph_color_matches_theme` | `ux_ok syntax_glyph_color_matches_theme` + `demo_live_fs_compile_ok` | M | **done** — [archive/tracks/TRACK_EDITOR_LIVE_SOLARIZED_TEXT](../archive/tracks/TRACK_EDITOR_LIVE_SOLARIZED_TEXT.md) |
| 1c | `EDITOR_STALE_HELP_TEXT` | Found 2026-07-17: `demo_live.mlc` `sample_readme_text()` (shown in-app as `editor_live_README.md` tab) says "Tree: click folder to expand/collapse; click file to open" — but `demo_live` never calls `file_tree_expand`/`file_tree_collapse` (§38 `TRACK_EDITOR_FOLDER_BROWSER` Decision replaced expand-in-tree with breadcrumbs, intentionally, not a bug). Fix the in-app copy to describe actual breadcrumb+back/forward behavior | n/a — copy-only fix, no scenario required unless bundled with behavior change | S | **active** — [TRACK_EDITOR_STALE_HELP_TEXT](TRACK_EDITOR_STALE_HELP_TEXT.md) STEP=1 next |
| 1d | `EDITOR_LARGE_FILE_NO_FULL_STRINGIFY` (pulled forward from #22, 2026-07-17 priority rule above) | Remove per-frame full-buffer `document_to_string` in demo — direct frame-latency regression on large files, Sublime-parity blocker | `large_file_no_full_stringify` (L2) | M | pending |
| 2 | `EDITOR_KEYBOARD_NAV_WIRE` | Wire arrow keys/Home/End/PageUp/PageDown (model exists in `document/navigation.mlc`, not called from demo) | `arrow_keys_move_caret`, `home_end_caret` | M | pending |
| 3 | `EDITOR_WORD_BOUNDARIES` | Word-boundary helper in `document/`; Ctrl+Left/Right, Ctrl+Backspace/Delete | `ctrl_arrow_word_jump`, `ctrl_backspace_deletes_word` | M | pending |
| 4 | `EDITOR_MOUSE_WORD_LINE_SELECT` | Double-click selects word, triple-click selects line, click timing via `UxDriver` | `double_click_selects_word`, `triple_click_selects_line` | M | pending |
| 5 | `EDITOR_SHIFT_CLICK_EXTEND` | Shift-click extends selection from anchor | `shift_click_extends_selection` | S | pending |
| 6 | `EDITOR_UNDO_COALESCE` | Group consecutive typing into one undo step (currently every keystroke = separate undo) | `typing_coalesces_into_one_undo` (L1.5) | M | pending |
| 7 | `EDITOR_FIND` | Find + next/prev, highlight matches in visible range | `find_highlights_matches` | M | pending |
| 8 | `EDITOR_GOTO_LINE` | Ctrl+G → line number → scroll+caret | `goto_line_scrolls_caret` | S | pending |
| 9 | `EDITOR_DRAG_AUTOSCROLL` | Drag past viewport edge auto-scrolls (multi-frame stateful) | `drag_past_edge_autoscrolls` (L1.5) | M | pending |
| 10 | `EDITOR_AUTO_INDENT` | Enter preserves leading indent of current line | `enter_keeps_indent` | S | pending |
| 11 | `EDITOR_CURRENT_LINE_HL` | Highlight current line in draw report | `current_line_highlight_draws` (L2) | S | pending |
| 12 | `EDITOR_CRLF_PRESERVE` | Detect CRLF/LF on open, preserve style on save | `crlf_preserved_on_save` (L0) | M | pending |
| 13 | `EDITOR_ENCODING_GUARD` | BOM detect + UTF-8 validity + refuse binary files | `binary_file_refused`, `utf8_bom_detected` | M | pending |
| 14 | `EDITOR_REPLACE` | Replace-one / replace-all on top of Find | `replace_one_and_all` | M | pending |
| 15 | `EDITOR_TAB_REORDER` | Drag-to-reorder tabs in strip | `drag_reorder_tabs` | M | pending |
| 16 | `EDITOR_FONT_ZOOM` | Ctrl+wheel changes font size (+state), reflows wrap | `ctrl_wheel_font_zoom` | S | pending |
| 17 | `EDITOR_HORIZONTAL_SCROLL` | Shift+wheel scrolls `scroll_offset_x`, hit-test accounts for it | `shift_wheel_scrolls_horizontal` | S | pending |
| 18 | `EDITOR_BRACKET_AUTOCLOSE` | Auto-close `()[]{}""`, skip-over closing char | `typing_paren_autocloses` | M | pending |
| 19 | `EDITOR_COMMENT_TOGGLE` | Ctrl+/ line-comment toggle (by language from status bar) | `toggle_line_comment` | S | pending |
| 20 | `EDITOR_SESSION_CARET_RESTORE` | Session stores caret+scroll per tab, restores on reopen | `session_restore_caret` (L0+L1) | S | pending |
| 21 | `EDITOR_DIRTY_CLOSE_L1` | L1 scenario for dirty-close overlay discard/cancel click (currently L0-only) | `dirty_close_overlay_click` | S | pending |
| 22 | `EDITOR_LARGE_FILE_NO_FULL_STRINGIFY` | **superseded by `#1d` above (2026-07-17 pull-forward)** — do not do twice | — | — | superseded |
| 23 | `EDITOR_MULTI_CURSOR` | `[EditorSelection]` model, Ctrl+click adds cursor, synchronized edit | `multi_cursor_model` + L1 | L | pending |
| 24 | `EDITOR_TRAILING_WS_VIZ` | Visualize trailing whitespace | `trailing_ws_visualized` (L2) | S | pending |
| 25 | `EDITOR_CONTEXT_MENU` | Right-click → cut/copy/paste menu | `context_menu_opens` | M | pending |
| 26 | `EDITOR_INDENT_MODEL` | Tabs-vs-spaces, indent width, Tab/Shift-Tab | `indent_width_config` (L0) + L1 | M | pending |
| 27 | `EDITOR_DROP_FILE` | Runtime GLFW drop-callback → open file (needs ABI) | `drop_file_opens_tab` | M | pending |
| 28 | `EDITOR_WIDE_CHAR_COLUMNS` | wcwidth-style column width for emoji/CJK (only if taken on) | `wide_char_column_width` (L0) | M | pending |
| 29 | `LANG_RECORD_UPDATE` (compiler, not editor) | Functional record-update syntax (`{ ...state, field: v }`); removes copy-all-fields boilerplate across `misc/editor`/`misc/gui/scene` | Tier B + self-host | M | pending |
| 30 | `EDITOR_UX_L1_BACKFILL` | Backfill L1 scenarios for §38/§40/§42/§43 (closed on unit+compile-gate only, per review §2.1) | 4 new `ux_scenarios/*` | M | pending |

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
  user directive, `../EDITOR.md` §Product target); multi-cursor (#23),
  block-selection, minimap, inline widgets, LSP/debugger are a separate
  later queue, do not block #1-22. Where feature breadth and
  stability/speed conflict, stability/speed wins (priority rule above).
- No accessibility tree before a real a11y story exists for GL-rendered
  text (`SemanticNode` stays a stub, frozen non-goal). RTL/bidi: non-goal.
  IME: non-goal v1 (ABI only exposes committed UTF-8, no preedit without a
  new GLFW bridge — a separate design track if ever taken on, not "polish").
- Do not close any of #1-28 on unit-only green — Standing discipline applies.
