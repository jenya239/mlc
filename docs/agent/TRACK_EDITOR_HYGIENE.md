# Track: Editor hygiene — audit 2026-08-01 remediation (§107)

Parent: [../PLAN.md](../PLAN.md) §107.
Source audit: `mlc-support/responses/editor_hygiene_audit_20260801_103839.md`
(Opus 5, 2026-08-01, findings EHA-01 … EHA-28).
Authorized 2026-08-01 as queue head, ahead of §103a Script VM and §104 Wave 2
(standing directive: производительность / архитектура / тестирование — приоритет).

## Status: **open** 2026-08-01 — §107a/§107b **CLOSED**; §107c Decision+Red in progress (Driver STEP=1 done / STEP=2 Green next)

Sub-track order is strict: §107a → §107b → §107c → §107d → §107e (P0),
then §107f … §107r (P1, audit roadmap order). P2 is a backlog table at the
bottom — **not** opened as PLAN rows; pull one only after §107r closes or on
explicit user override.

## Why this epic exists

The audit found three defects of the class "the product does not do what it
claims", none caught by any gate:

1. **Ctrl+S does not save the open file** — `CmdSave` is mapped to
   `save_demo_session`, which writes copies into `.tmp/editor_live_*`;
   `open_buffer_save` is never called for the active tab. Data loss, P0.
2. **Scroll is O(document) with HarfBuzz per frame** —
   `collect_visible_visual_rows_pixel_budget` walks from line 0 and ignores
   the `prefix_visual_rows` table §101 already builds.
3. **The perf gate measures a path with the hot phases switched off** —
   `MLC_EDITOR_PERF=1` skips pixel-wrap, syntax spans and minimap, so a green
   `draw_us` says nothing about a real frame.

Structural root cause behind most of the perf findings: **cache invalidation by
full-text comparison** (4 independent caches, up to 4 × O(n) memcmp per frame,
including hover frames) instead of a document version counter.

## Global non-goals (binding for every sub-track)

- **Do not rewrite `demo_live.mlc` wholesale.** §107a/b/e fix the product without
  a structural refactor; §107q (`EDITOR_DRAW_OPS`) is the only large one and must
  be behaviour-preserving, one paint region at a time.
- **Do not migrate editor chrome to `SceneNode`** — the Deviation in
  `GUI_ARCHITECTURE.md` stands; migrating would mix two trees and block §107c/e/i.
- **Do not treat `SwapInterval(1)` as a perf fix** — it caps frames, not frame cost.
- **No second toolkit / Taffy / CSS layout.** `layout/shell.mlc` primitives suffice.
- **Do not pull `TRACK_MLC_SCRIPT_VM` (§103) into the editor** — unrelated.
- **Do not close a sub-track on unit-only green where a side effect exists**
  (file on disk, fd, process). EHA-01/02/08/09 exist precisely because the model
  was green.
- **Do not add new `*_stable.sh` grep gates** in place of behavioural scenarios
  (EHA-19); grep may stay as arch-lint, never as a UX scenario.
- **Do not touch `compiler/**` `.mlc`** in this epic. If a sub-track needs it,
  stop and re-freeze the Decision.

## Verification discipline (every sub-track)

Scenario-first (red before green), sabotage every load-bearing assertion and
revert it with a `git diff`-confirmed-empty check, `scripts/run_ux_gate.sh` ×2
before Critic close, `scripts/dev_gate_fast.sh` green. `scripts/regression_gate.sh`
only if `lib/mlc/**` is touched. Critic must re-verify independently (fresh
`mlcc` rebuild, separate out dir, own sabotages distinct from the Driver's).

---

# P0 sub-tracks

## §107a `EDITOR_SAVE_ACTIVE_FILE` (EHA-01) — **queue head**

### Decision (frozen 2026-08-02 — confirms Planner proposal)

| Item | Choice |
|------|--------|
| Problem | `CmdSave` maps to `save_demo_session`; `dump_tab_file` writes `.tmp/editor_live_<name>` copies. `open_buffer_save` (`document/save.mlc`, correct: CRLF preserve, BOM re-prefix, §12/§13) is called from nowhere else, so Ctrl+S and the toolbar Save button never write the file the user is editing, and the active tab's dirty flag is never cleared |
| Fix | `CmdSave` → `editor_app_save_active_buffer` → `open_buffer_save(active.buffer)` on the active tab's **original** path, then `tab_set_update_active_buffer` with the saved buffer (dirty cleared, `last_error` propagated). Session dumping moves to a distinct command (`CmdSaveSession`, bound to the existing session toolbar button / a chord that does not collide with `command_bus_default_bindings()`) |
| Error path | Save failure must not be silent: set `buffer.last_error`, keep the tab dirty. Surfacing it in the status bar is EHA-24 (P2) — this sub-track only guarantees the flag is not falsely cleared |
| Module touch | `misc/editor/demo_live.mlc` (dispatch arm), `misc/editor/app/state.mlc` (`editor_app_save_active_buffer`, `editor_app_set_*` discipline — no scattered locals), `misc/editor/commands/bus.mlc` (`CmdSaveSession` + binding), new `misc/editor/ux_scenarios/save_writes_file_to_disk.mlc`, new `scripts/run_ux_save_writes_file_to_disk.sh` |
| Gate | `run_ux_save_writes_file_to_disk`: write a fixture file under `.tmp`, open it through the real open path, apply an edit, dispatch save through `editor_app_save_active_buffer` (the CmdSave command path — not `open_buffer_save` directly), **read the file back from disk**, assert the content matches the edited buffer, assert the active tab is no longer dirty, and assert no `.tmp/editor_live_*` copy was created for it |
| Sabotage (required before close) | (1) revert `CmdSave` / `editor_app_save_active_buffer` to `save_demo_session` dump → gate must fail; (2) make `open_buffer_save` a no-op returning success → gate must fail on the disk read, not only on the dirty flag |
| REG | no (`compiler/**` untouched; `lib/mlc/**` untouched unless the file-write helper is changed — if it is, run `regression_gate.sh`) |
| Out of scope | Atomic write (`tmp` + `rename`) and `fsync` → P2 backlog `EDITOR_SAVE_ATOMIC_WRITE`; autosave / crash recovery; status-bar error display (EHA-24) |

### Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** 2026-08-02 |
| 1 | Red: new scenario fails on today's tree (disk content unchanged after save path) | **done** 2026-08-02 — stub `editor_app_save_active_buffer` still dumps `.tmp/editor_live_*` (CmdSave bug shape); `run_ux_save_writes_file_to_disk.sh` exits non-zero (`ux_fail … disk_content`) |
| 2 | Green: `CmdSave` → active-buffer save; `CmdSaveSession` split out; scenario green; `run_ux_gate.sh` ×2; `dev_gate_fast.sh` | **done** 2026-08-02 — `editor_app_save_active_buffer` → `open_buffer_save` + `tab_set_update_active_buffer`; `CmdSaveSession` (Ctrl+Shift+S + toolbar tool 9); `CmdSave` (Ctrl+S) wires live path; `run_ux_save_writes_file_to_disk` ok; `dev_gate_fast` 1471/0; `run_ux_gate` ×2 = 116/116 |
| 3 | Critic: independent rebuild + rerun, own sabotages, gate ×2 | **done** 2026-08-03 — see Critic section below |

### §107a Critic (closed 2026-08-03)

Independent re-audit of Green commit `03179bcc` (9 files, Decision touch list).
Separate out dir `.tmp/critic_107a_save`: `ux_ok save_writes_file_to_disk`.
Sabotages distinct from Driver (both fail on `disk_content` exit 5, then
reverted — `git checkout` clean for track files):
1. `editor_app_save_active_buffer` dumps `.tmp/editor_live_save_red.txt` instead
   of writing the original path.
2. `open_buffer_save` no-op clearing dirty without `file_write_string`.
`dev_gate_fast` 1471/0; `run_ux_gate` ×2 = 116/116. Final scenario rebuild after
restore: `ux_ok`.

**§107a CLOSED.** Next: §107b `EDITOR_SESSION_ORIGINAL_PATHS`.

---

## §107b `EDITOR_SESSION_ORIGINAL_PATHS` (EHA-02)

### Decision (frozen 2026-08-03)

| Item | Choice |
|------|--------|
| Problem | `save_demo_session` → `dump_tab_file` collapses tabs onto colliding `.tmp/editor_live_*` names; `workspace_session_restore_tabs` reopens those copies; `tab_set_open_buffer` dedupes identical dump paths → tabs lost; restored session points at scratch copies, not project files |
| Fix | `save_demo_session` / `CmdSaveSession` stores **original absolute tab paths** via `workspace_session_from_tabs` on the live `TabSet` — delete the `dump_tab_file` loop from the session-save path (function may be deleted if unused) |
| Module touch | `misc/editor/demo_live.mlc` (`save_demo_session`), `misc/editor/workspace/session.mlc` (`workspace_session_save_tabs`), `misc/editor/ux_scenarios/session_restores_original_paths.mlc`, `scripts/run_ux_session_restores_original_paths.sh`; update `run_ux_demo_save_no_double_stringify_stable.sh` (old dump/flatten L2 → assert no dump) |
| Gate | `run_ux_session_restores_original_paths`: write ≥2 distinct fixture files under `.tmp`, open both through the real open path, save session through the **same** `save_demo_session` path `CmdSaveSession` uses, restore into a fresh `TabSet`, assert tab count matches, each restored path equals the original absolute path, and no restored path contains `editor_live_` |
| Sabotage (required before close) | (1) reintroduce `dump_tab_file` into session save → gate must fail (count and/or path); (2) rewrite one restored path to `.tmp/editor_live_*` after save → gate must fail path assert |
| REG | no |
| Out of scope | Session path validation against root → P2 `EDITOR_SESSION_PATH_VALIDATION` (EHA-20); changing `workspace_session_from_tabs` itself (already path-faithful) |

### Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** 2026-08-03 |
| 1 | Red: scenario fails on today's dump-based `save_demo_session` | **done** 2026-08-03 — two `.txt` fixtures collide to `.tmp/editor_live_buffer.txt`; restore loses a tab / paths are `editor_live_*` |
| 2 | Green: remove dump from session save; scenario green; `run_ux_gate` ×2; `dev_gate_fast` | **done** 2026-08-03 — `workspace_session_save_tabs` + `save_demo_session` no dump; `dump_tab_file` deleted; scenario `ux_ok`; `demo_save_no_double_stringify_stable` retargeted (0 flatten); `dev_gate_fast` 1471/0; `run_ux_gate` ×2 = 117/117 |
| 3 | Critic: independent rebuild + rerun, own sabotages, gate ×2 | **done** 2026-08-03 — see Critic section below |

### §107b Critic (closed 2026-08-03)

Independent re-audit of Green commit `4de554f8`. Separate out dir
`.tmp/critic_107b_session`: `ux_ok session_restores_original_paths`.
Sabotages distinct from Driver (both on `workspace_session_save_tabs`, then
`git checkout` clean):
1. Collapse all session paths to `.tmp/editor_live_buffer.txt` → `count` exit 6.
2. Rewrite first path to `.tmp/editor_live_sabotage.txt` → `editor_live_path` exit 7.
`dev_gate_fast` 1471/0; `run_ux_gate` ×2 = 117/117. Final rebuild after restore:
`ux_ok`.

**§107b CLOSED.** Next: §107c `EDITOR_VISIBLE_ROWS_PREFIX_JUMP`.

---

## §107c `EDITOR_VISIBLE_ROWS_PREFIX_JUMP` (EHA-03)

### Decision (frozen 2026-08-03)

| Item | Choice |
|------|--------|
| Problem | `collect_visible_visual_rows_pixel_budget` walks `line` from 0 every frame, shaping via `visual_rows_for_line_pixel_budget` → `text_shaping_shape`, and ignores `prefix_visual_rows` already built by §101 (`DocumentWrapCountCache`). Deep scroll re-shapes every preceding line |
| Fix | Add `collect_visible_visual_rows_pixel_budget_cached(..., prefix_visual_rows, ...)` that finds the first document line whose prefix covers `first_visual_row` and shapes only from that line through the visible window; fall back to the uncached walk when `prefix_visual_rows` is empty or too short (same fallback shape as §101 caret). Wire call sites in `demo_live` that already hold the wrap-count cache |
| Module touch | `misc/editor/layout/word_wrap.mlc`, `misc/editor/demo_live.mlc` (wire at Green), `misc/editor/ux_scenarios/visible_rows_prefix_jump.mlc`, `scripts/run_ux_visible_rows_prefix_jump.sh`; deep-scroll ceiling may extend `run_editor_demo_live_perf_smoke.sh` at Green after measure |
| Gate | `run_ux_visible_rows_prefix_jump`: ≥80-line fixture, warm `prefix_visual_rows`, deep `first_visual_row`; assert cached rows == uncached rows; with covering prefix assert `lines_shaped <= max_rows + 2`; empty-prefix fallback rows still match uncached |
| Sabotage (required before close) | (1) ignore prefix in cached collect → `lines_shaped` bound fails; (2) force wrong jump line → `rows_mismatch` |
| Perf add-on | Deep-scroll (~90% of 100k) ceiling measured then written at Green (not guessed); full-path mode stays §107d |
| REG | no |
| Out of scope | §107j (triple collect → single); §107d full perf mode; changing wrap-cache invalidation (§107e) |

### Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** 2026-08-03 |
| 1 | Red: cached collect ignores prefix; `lines_shaped` bound fails | **done** 2026-08-03 — stub `collect_visible_visual_rows_pixel_budget_cached` walks from line 0; `ux_fail … lines_shaped` |
| 2 | Green: prefix jump + demo_live wire; scenario green; `run_ux_gate` ×2; `dev_gate_fast` | pending |
| 3 | Critic | pending |

---

## §107d `EDITOR_PERF_SMOKE_FULL_PATH` (EHA-04)

| Item | Choice |
|------|--------|
| Problem | Under `MLC_EDITOR_PERF=1`, `demo_live.mlc` sets `skip_full_pixel_wrap_now = 1`, skips `frame_layout_tick_spans`, and skips the minimap. The three most expensive frame consumers are off, so `draw_us` from the current smoke does not characterise a real frame |
| Fix | Add `MLC_EDITOR_PERF_FULL=1`: same fixture and frame count, **no** skip branches (wrap, spans, minimap all live). Rename/retag the existing mode as the `*_baseline` path and keep its budget; the full path gets its own measured ceiling |
| Gate | New `scripts/run_editor_demo_live_perf_full_smoke.sh` with a ceiling set from the first real measurement (same honesty rule as §102g: measure, then write the number). Existing baseline smoke must stay green |
| Note | §107c's deep-scroll assertion should run in the **full** mode once this exists |

---

## §107e `EDITOR_DOCUMENT_VERSION` (EHA-05) — unblocks §107f/§107i/§107j and the minimap work

| Item | Choice |
|------|--------|
| Problem | Four caches invalidate by comparing the whole document text: `wrap_count_cache_needs_recompute_pixel` (`layout/wrap_cache.mlc`), `max_line_columns_cache_needs_recompute` (`ux/overflow.mlc`, §105), `editor_ux_syntax_span_cache_tick` (`ux/syntax_span_cache.mlc`, `state.text == text`), `editor_ux_minimap_needs_rebuild` (`ux/minimap.mlc`). Up to 4 × O(n) memcmp per frame; the spans tick runs even on hover frames (outside `layout_skip`) |
| Fix | Monotonic `version: i64` on `TextDocument`, incremented in `document_insert` / `document_delete` (and any other mutating primitive). All four caches key on `(version, parameters)` instead of on the text |
| Gate | `run_ux_cache_keys_by_version`: instrument a compare counter; assert **zero** full-text comparisons across N idle frames and N hover frames, exactly one recompute per edit, and that a same-content-different-history document still invalidates (version changed) — i.e. the key is the version, not the bytes |
| Sabotage | Freeze the version counter → caches must go stale and the gate must fail |

---

# P1 sub-tracks (audit roadmap order)

## §107f `EDITOR_INCREMENTAL_LINE_INDEX` (EHA-06)
`document_frame_snapshot` = `document_to_string` + `line_index_from_string`;
`frame_layout_tick_snapshot(..., 1)` from 25+ sites — every keystroke flattens
the piece table and rebuilds `starts[]`.
**Fix:** `line_index_apply_edit(start, removed, inserted)`; snapshot without
flatten (`document_byte_slice` / lazy `byte_at` already exist).
**Gate:** `run_ux_edit_no_full_flatten` — a `flatten_count` counter must not grow
across N insertions. Residual of §46 #1d.

## §107g `EDITOR_TERMINAL_TEARDOWN` (EHA-08 + EHA-09)
`terminal_panel_session_close` is reached only from `CmdCloseTab`; the tab-strip
"x" (`editor_app_click_tab_strip` → `tab_set_request_close`) and window close both
skip it, leaking the master fd and the `vterm`. `pty_close` does
`waitpid(WNOHANG)` + `close` with no signal, so `sh` is orphaned, never reaped.
**Fix:** teardown in `editor_app_click_tab_strip` when
`terminal_panel_is_tab_path`; `editor_app_close_terminal` before
`glfw_gl_context_end`; `pty_close` → `SIGHUP` → short `waitpid` → `SIGKILL`
fallback. **Gate:** `run_ux_terminal_tab_close_releases_pty` — `/proc/self/fd`
count before/after, plus a `waitpid` check that the child is reaped.
Residual of §102f/§102b.

## §107h `EDITOR_SHAPE_SEGMENT_BUDGET` (EHA-07)
`line_codepoint_advances_px` falls back, on HarfBuzz cluster mismatch (ligatures),
to shaping **each codepoint separately** — a 5MB minified single-line file becomes
millions of `text_shaping_shape` calls. Only real untrusted-content DoS vector.
**Fix:** chunk the shaped segment (N codepoints) and hard-cap line length for wrap.
**Gate:** `run_ux_long_single_line_budget` — one 1MB line, frame under a documented budget.

## §107i `EDITOR_SPANS_TICK_UNDER_LAYOUT_SKIP` (EHA-05 hover half)
`frame_layout_tick_spans` is called in the paint phase outside `layout_skip`, so
every hover frame pays a full-buffer comparison (and a full `highlight_range` when
it fires). **Fix:** move it under `layout_skip` / key it on the §107e version.
**Gate:** `run_ux_hover_no_full_compare`. Residual of §97c and of §106.

## §107j `EDITOR_VISIBLE_ROWS_SINGLE_COLLECT` (EHA-18)
`collect_visible_visual_rows_pixel_budget` runs up to 3× per content frame
(early / scroll branch / late). **Fix:** one call after the final
`frame_layout_tick_pixel`; the early phase reuses `cached_visual_rows`.
**Gate:** a counter in `ui/perf.mlc` asserted == 1 in the perf smoke.
Residual of §97b.

## §107k `EDITOR_TERMINAL_DAMAGE_REPAINT` (EHA-11)
`terminal_grid_text_lines` / `terminal_grid_draw_backgrounds` rebuild every frame
(60×200 → up to 12000 string concats + 12000 rects) while `vterm_damage_count`
(§102a) is never consumed. **Fix:** cache `[StaticTextLine]` + the rect batch,
invalidate on `vterm_damage_count`. **Gate:** `run_ux_terminal_idle_no_rebuild`.

## §107l `EDITOR_INDENT_REPLACE_NO_STRINGIFY` (EHA-12)
`edit_indent_tab` / `edit_outdent_lines` (`document/indent.mlc`) do
`document_to_string` + full `rebuild_lines_with_transform` + `document_from_string`;
`find_replace_one` / `_all` (`ux/find.mlc`) splice on the full string.
**Fix:** route through `document_line_start_before` / range edits, as §93/§98/§99/§100
already did for `edit.mlc`. **Gates:** `run_ux_indent_no_full_stringify`,
`run_ux_replace_no_full_stringify`.

## §107m `EDITOR_OPEN_SIZE_GUARD` (EHA-13)
`open_buffer_from_path` reads any size, then makes 4 more full passes
(`text_contains_nul`, `utf8_text_is_valid`, 2× `replace` for newline normalisation).
**Fix:** `MLC_EDITOR_MAX_OPEN_BYTES` (default ~64MB) with an explicit refusal;
merge the guard passes into one. **Gate:** `run_ux_oversized_file_refused`.

## §107n `EDITOR_PROBE_GEOMETRY_PARITY` (EHA-14)
`editor_app_state_new` uses `tab_strip_height: 0`, `editor_ux_state_new` uses `28`,
so every scenario built on `text_viewport_rect` validates geometry the product does
not have (class: false-green). **Fix:** one state constructor with an explicit
`tab_strip_height`, used by both product and scenarios. **Gate:**
`run_ux_probe_geometry_matches_live` + revision of the ~10 affected scenarios.
Residual of 2026-07-15 #7.

## §107o `EDITOR_VIEWPORT_RECT_SINGLE` (EHA-15)
8 copies of `fn text_viewport_rect(state)` across `ux/*` (was 5 in 2026-07-17 — got
worse). **Fix:** one exported `editor_ux_text_viewport_rect` in `ux/probe.mlc`,
delete the rest. **Gate:** `dev_gate_fast` + a grep arch-lint on the duplicate name
(arch-lint only, not a UX scenario — see §107r).

## §107p `EDITOR_TOOLBAR_COMMAND_TABLE` (EHA-16)
`demo_live` hit-tests `while tool < 10` plus a magic index 10 for SessLd, while
`editor_app_toolbar_command_at` covers 0..9 and the `tools` array has 11 entries.
**Fix:** one `[(label, CommandId)]` table driving both hit-test and draw.
**Gate:** `run_ux_toolbar_table_single_source`. Residual of 2026-07-15 #5.

## §107q `EDITOR_DRAW_OPS` (EHA-17) — largest, strictly incremental
Paint lives as direct `solid_renderer_rect` / `push_line` calls in `demo_live`,
while `ux/draw_frame.mlc::editor_ux_draw_frame` → `UxDrawOp` exists and is used only
by scenarios. Nothing can assert what the product actually draws.
**Fix:** paint builds `[UxDrawOp]`, one flatten pass to GL. **Behaviour-preserving,
one region per commit** (chrome → tab strip → gutter → text → overlays), each with
its own green. **Gate:** `run_ux_draw_ops_from_live_state`. Residual of 2026-07-15 #4/#6.

## §107r `EDITOR_UX_GATE_BEHAVIORAL` (EHA-19)
`run_ux_wheel_hover_focus_independent_stable.sh`, `run_editor_frame_layout_*_stable.sh`
grep the source (`zero editor_focused in block`, `exactly 1 frame_layout_tick_pixel(`):
they pin code shape, not behaviour — refactors go red, real regressions pass.
**Fix:** replace with behavioural L1/L2 where possible; keep grep as arch-lint only.
Also add the gate types the audit lists as missing: idle-CPU **under pointer load**
(the §106 gap), deep-scroll perf (§107c), untrusted-content DoS (§107h), resource
lifecycle (§107g), filesystem side effects (§107a/b), path/UTF-8 security (P2),
multi-frame L1.5 for hover-paint and overlay-idle.
**Gate:** revised `run_ux_gate.sh` composition, ×2 stable.

---

# P2 backlog (not opened as PLAN rows)

Pull one only after §107r closes, or on explicit user override.

| ID | Item | Finding | Gate |
|---|---|---|---|
| B1 | `EDITOR_SESSION_PATH_VALIDATION` — session `file=` entries go straight into `open_buffer_from_path`; validate existence / regular file / under saved root | EHA-20 | `run_ux_session_rejects_outside_root` |
| B2 | `EDITOR_PASTE_UTF8_SANITIZE` — clipboard/PTY text reaches `edit_paste` unvalidated; replace invalid sequences with U+FFFD at the input boundary | EHA-21 | `run_ux_paste_invalid_utf8_sanitized` |
| B3 | `GUI_INPUT_ASTRAL_UTF8` — `append_utf8` in `glfw_window_gl.cpp` has no branch for codepoints > 0xFFFF; emoji/astral input is silently dropped | EHA-22 | extend `run_gui_text_input_smoke` |
| B4 | `GUI_BINDING_KEY_MULTI_EDGE` — `glfw_gl_take_binding_key` returns on the first edge, leaving other keys' `previous_down` stale; holding one tracked key freezes edge detection for the rest | EHA-23 | `run_gui_binding_key_multi_edge` |
| B5 | `EDITOR_OPEN_ERROR_STATUS` — drop/binary/bad-UTF-8/permission failures return the original tabs with no UI; show `buffer.last_error` in the status bar | EHA-24 | `run_ux_open_error_shown_in_status` |
| B6 | `EDITOR_WORKSPACE_TREE_DECOUPLE` — `ShellPanels.tree: FileTree`; the panel owns the workspace model (invariant №6) | EHA-25 | `run_ux_tree_shared_between_views` |
| B7 | `EDITOR_OVERLAY_IDLE_NO_RETICK` — `frame_input_poll` raises `content_dirty` while a context menu / overlay is visible, forcing full frames with no events | EHA-26 | `run_ux_overlay_idle_no_retick` |
| B8 | `EDITOR_SEMANTIC_NODE_STUB` — frozen invariant №10 (`SemanticNode {role,label,bounds}`) still unimplemented | EHA-27 | compile-time unit |
| B9 | `EDITOR_MINIMAP_ROW_SAMPLING` — minimap rebuild builds a `StaticTextLine` per document line; sample to the strip height instead | EHA-28 | `run_ux_minimap_rows_bounded_by_height` |
| B10 | `EDITOR_SAVE_ATOMIC_WRITE` — no `tmp`+`rename`, no `fsync`; an interrupted write truncates the user's file (out of scope of §107a by decision) | audit §6.1 | `run_ux_save_atomic_on_interrupt` |
| B11 | `TERMINAL_PTY_SPAWN_ARGV` — `pty_spawn` execs `"/bin/sh","-c",command_line`; command injection the moment a UI-derived string is passed. Catalog severity **P1**, absent from the audit's own roadmap — recorded here so it is not lost. Either add `pty_spawn_argv` or state the "trusted literal only" contract in the header and lint it | EHA-10 | `run_editor_terminal_pty_argv_unit` |

## Diff vs the audit

The audit's roadmap items 1–18 map 1:1 to §107a–§107r in order. Its P2 items 19–25
map to B1–B9 above; B10/B11 are additions recorded from audit §6.1 / EHA-10 so that
no catalog finding is dropped on the floor.
