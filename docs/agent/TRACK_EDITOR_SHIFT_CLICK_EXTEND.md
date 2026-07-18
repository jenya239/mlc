# Track: Editor Shift-click extend selection

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#5**.
Text mouse-down collapses caret (or multi-click word/line via #4); Shift held
does not extend selection from the current anchor. `selection_set_focus` /
`nav_drag` already model Extend.

## Status: **active** (2026-07-18) — STEP=1 done; STEP=2 next

## Next step

**STEP=2** — Shift branch in text-click path + `demo_live` wire; scenario green
+ compile.

### STEP=1 done (2026-07-18)

- Stub `ux/shift_click.mlc` (`editor_ux_shift_click_extend` → always caret)
- L2 red: `shift_click_extends_selection` (+ run script)
- `demo_live_fs_compile_ok`

### STEP=0 done (2026-07-18)

- Decision frozen below; PLAN §46 + UX_BACKLOG #5 → active.

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
| 2 | Shift branch in text-click path + `demo_live` wire | scenario + compile |
| 3 | Critic: gates; archive | close |

### Sub-steps (Driver)

**STEP=1** — **done**

**STEP=2**
1. Implement Shift-extend helper; wire `demo_live` text mouse-down (Shift before multi-click).
2. Gates: scenario ok + `demo_live_fs_compile_ok`.

**STEP=3** — Critic; `next` = Planner (§46 #6).

## Out of scope

Drag-autoscroll (#9); undo coalesce (#6); `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
