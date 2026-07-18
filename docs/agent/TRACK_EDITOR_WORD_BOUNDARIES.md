# Track: Editor word boundaries (Ctrl+arrows / Ctrl+Backspace/Delete)

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#3**.
Char-level nav wired (#2); no word-boundary helper; Ctrl+Left/Right still
move one byte; Ctrl+Backspace/Delete not word-scoped.

## Status: **active** (2026-07-18) — STEP=2 done; STEP=3 Critic next

## Next step

**STEP=3** — Critic: gates; archive; `next` = Planner (§46 #4).

### STEP=2 done (2026-07-18)

- `document/word_boundary.mlc` + unit
- `nav_word_*` / `edit_delete_word_*`; `ux/word_nav` wired
- `demo_live`: Ctrl+left/right word nav; Ctrl+Backspace; Ctrl+Delete edge
- Gates: both scenarios ok + `demo_live_fs_compile_ok`

### STEP=1 done (2026-07-18)

- Stub `ux/word_nav.mlc`; L2 red harness

### STEP=0 done (2026-07-18)

- Decision frozen below; PLAN §46 + UX_BACKLOG #3 → active.

## Decision (STEP=0) — **frozen** 2026-07-18

| Item | Choice |
|------|--------|
| Problem | No word-boundary API; Ctrl+Left/Right behave as char moves when Ctrl held; Ctrl+Backspace/Delete do not delete by word |
| Word class | ASCII letter/digit/`_` = word; everything else = separator. Jump skips runs of separators then lands on word edge (Sublime-like ASCII). Multi-byte UTF-8 / emoji: treat non-ASCII bytes as separator for this track (honest residual → `#28` if needed) |
| Helper | New `document/word_boundary.mlc`: `word_boundary_prev` / `word_boundary_next` (byte offset on `LineIndex` text or string + length). Unit-testable without GLFW |
| Nav | `nav_word_left` / `nav_word_right` in `navigation.mlc` (or thin wrappers) using helper + `Collapse`/`Extend` (Shift+Ctrl → Extend) |
| Edit | `edit_delete_word_backward` / `edit_delete_word_forward` in `edit.mlc` (selection non-empty → same as selection delete; else delete to prev/next boundary) |
| Input | Reuse binding keys `left`/`right` + `glfw_gl_mod_ctrl_down`. Extend `editor_ux_apply_nav_key` (or sibling) so Ctrl+left/right → word nav. Ctrl+Backspace: branch on `ctrl_down` in existing backspace edge path. Ctrl+Delete: add `delete` binding-key edge (or key poll) + wire; prefer same `take_binding_key` path as nav |
| Scenarios | L2: `ctrl_arrow_word_jump`, `ctrl_backspace_deletes_word` (backlog names). Ctrl+Delete covered in unit and/or one scenario if cheap |
| Standing discipline | Scenario-first; name tokens in SESSION `done` |
| Module touch | `misc/editor` document/ux/app + `demo_live`; minimal `runtime` GLFW only if Delete edge missing. No `compiler/` / `lib/mlc/` |
| REG | no `lib/mlc/`; no `compiler/` |

### Gates

| Step | Token |
|------|-------|
| 1–2 | `ux_ok ctrl_arrow_word_jump` + `ux_ok ctrl_backspace_deletes_word` (+ run scripts) |
| every | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog | **done** (2026-07-18) |
| 1 | L2 scenarios first (`ctrl_arrow_word_jump`, `ctrl_backspace_deletes_word`) | **done** (red harness) |
| 2 | word_boundary + nav/edit + ux/`demo_live` wire | **done** |
| 3 | Critic: gates; archive | close |

### Sub-steps (Driver)

**STEP=1** — **done**

**STEP=2** — **done**

**STEP=3** — Critic; `next` = Planner (§46 #4).

## Out of scope

Mouse word/line select (#4); Shift-click (#5); wide-char columns (#28); `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
