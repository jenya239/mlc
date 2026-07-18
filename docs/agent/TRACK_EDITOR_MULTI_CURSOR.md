# Track: Editor Multi Cursor

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#23**
(`#22` superseded by `#1d`). Single `EditorSelection` only; no Ctrl+click
extra carets; typing cannot fan out. Review gates: `multi_cursor_model`
(+ L1 insert). Size **L**.

## Status: **active** (2026-07-19) — STEP=0 done; next STEP=1

## Next step

**STEP=1** — L1 scenario first (`multi_cursor_model`) red against stub.

## Decision (STEP=0) — **frozen** 2026-07-19

| Item | Choice |
|------|--------|
| Problem | One selection only; Sublime multi-cursor missing |
| Model | `EditorMultiSelection { selections: [EditorSelection], primary_index: i32 }` — length ≥ 1; primary is the “main” selection |
| Add caret | `multi_selection_add_collapsed(multi, byte_offset)` — if offset already a caret of any selection, no-op; else append collapsed selection; primary unchanged |
| Clear | `multi_selection_clear_to(multi, byte_offset)` → single collapsed at offset (primary 0) |
| Primary bridge | `multi_selection_primary(multi) -> EditorSelection`; existing single-selection callers keep working via primary |
| Sync edit (this track) | Collapsed carets only: insert same `text` at each caret **descending byte offset** (preserve earlier offsets); non-collapsed multi-ranges **out of scope** (primary-only edit if any selection non-empty) |
| Input | Ctrl+click in editor text → add collapsed caret at click byte offset (no drag-add) |
| Esc / plain click | Collapse to single caret at click / primary focus (plain click replaces multi) |
| Live | demo_live: Ctrl+click add; typing uses multi-insert when all collapsed |
| Draw | Multiple caret glyphs **best-effort**; honest residual if only primary caret drawn this track |
| Scenarios | L1: `multi_cursor_model` — start single; add second caret; assert count=2 + offsets; insert `"x"` at both → text/`carets` updated; token `ux_ok multi_cursor_model` |
| Standing discipline | Scenario-first; name tokens in SESSION `done` |
| Module touch | `misc/editor/document/` (+ ux/edit + thin demo_live). No `compiler/` / `lib/mlc/` |
| REG | no `lib/mlc/`; no `compiler/` |

### Gates

| Step | Token |
|------|-------|
| 1–3 | `ux_ok multi_cursor_model` (+ run script) |
| every | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |
| regression | `bash scripts/run_ux_dirty_close_overlay_click.sh` → `ux_ok dirty_close_overlay_click` |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog | **done** (2026-07-19) |
| 1 | L1 scenario first (`multi_cursor_model`) | pending |
| <!-- sub-steps: 1) scenario + run script; 2) stub multi API → red; 3) demo_live_fs_compile + dirty_close green --> |
| 2 | `EditorMultiSelection` + add/clear/primary + insert-multi (collapsed) | pending |
| <!-- sub-steps: 1) new module or extend selection; 2) edit_insert_text_multi; 3) L1 green for model+insert --> |
| 3 | ux/demo_live Ctrl+click add + typing path when multi | pending |
| <!-- sub-steps: 1) editor_ux_add_cursor / insert wire; 2) demo_live Ctrl+click; 3) gates green --> |
| 4 | Critic: gates; archive | pending |

## Out of scope

Column/rect select; select-all-occurrences; multi-range (non-collapsed) synchronized replace; Cmd+D / soft undo per caret; full multi-caret GL draw polish; `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
