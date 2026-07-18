# Track: Editor Shift-click extend selection

Parent: [../agent/TRACK_EDITOR_UX_BACKLOG.md](../agent/TRACK_EDITOR_UX_BACKLOG.md) §46 **#5**.
Text mouse-down collapses caret (or multi-click word/line via #4); Shift held
does not extend selection from the current anchor. `selection_set_focus` /
`nav_drag` already model Extend.

## Status: **closed** (2026-07-18) — Critic OK

**Critic 2026-07-18 (STEP=3):** Re-ran L2 + compile. Anti-false-done:
`f701e34c`…`2355a46e` (STEP=0–2). Wire present: `editor_ux_shift_click_extend`
→ `selection_set_focus`+clamp when `shift_down != 0`; `demo_live` text
mouse-down: Shift before multi-click. **reopen: none**.

Honest residual: Shift skips multi-click by Decision (not a bug); #4 residual
unused `nav_click` import still present in `demo_live`.

| Gate | Result |
|------|--------|
| `run_ux_shift_click_extends_selection.sh` | `ux_ok shift_click_extends_selection` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§46 `#6 EDITOR_UNDO_COALESCE`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `f701e34c` | Decision freeze + open |
| 1 | `a3bc4957` | L2 red harness |
| 2 | `2355a46e` | shift_click + demo_live wire |
| 3 | this Critic | close + archive |

## Decision (STEP=0) — **frozen** 2026-07-18

| Item | Choice |
|------|--------|
| Problem | Bare / multi-click text mouse-down always collapses or reselects word/line; Shift+click should keep `anchor` and move `focus` to click offset (Sublime-like) |
| Extend | `selection_set_focus(selection, click_offset)` + clamp to document length. Reuse existing selection model; no new GLFW ABI (`glfw_gl_mod_shift_down` already exists) |
| Interaction with #4 | If Shift held on text mouse-down: **skip** multi-click detector (do not bump word/line count); always Extend from current selection. Without Shift: keep #4 multi-click path unchanged |
| Apply path | Thin `ux/` helper (e.g. `editor_ux_shift_click_extend` / branch in text-click path) taking `LineIndex` + current selection + offset + `shift_down`; `demo_live` checks Shift before `editor_ux_apply_multi_click` |
| Scenarios | L2: `shift_click_extends_selection` (backlog name). Assert anchor preserved, focus = click offset, non-empty range |
| Standing discipline | Scenario-first; name tokens in SESSION `done` |
| Module touch | `misc/editor` ux + `demo_live`. No `compiler/` / `lib/mlc/`; no new runtime |
| REG | no `lib/mlc/`; no `compiler/` |

### Gates

| Step | Token |
|------|-------|
| 1–2 | `ux_ok shift_click_extends_selection` (+ run script) |
| every | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog | **done** (2026-07-18) |
| 1 | L2 scenario first (`shift_click_extends_selection`) | **done** (red harness) |
| 2 | Shift branch in text-click path + `demo_live` wire | **done** |
| 3 | Critic: gates; archive | **done** (closed) |

## Out of scope

Drag-autoscroll (#9); undo coalesce (#6); `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
