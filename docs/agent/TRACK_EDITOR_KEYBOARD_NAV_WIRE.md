# Track: Editor keyboard nav wire (arrows / Home / End / Page)

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#2**.
Model already in `document/navigation.mlc` (`nav_move_*`, `nav_line_*`,
`nav_page_*`); not called from `demo_live`. GLFW binding keys today omit
arrows/Home/End/Page (`glfw_gl_take_binding_key` only letters/tab).

## Status: **active** (2026-07-18) — STEP=2 done; STEP=3 Critic next

## Next step

**STEP=3** — Critic: gates; archive; `next` = Planner (§46 #3).

### STEP=2 done (2026-07-18)

- GLFW: `left`/`right`/`up`/`down`/`home`/`end`/`page_up`/`page_down` edges
- `editor_ux_apply_nav_key` → `nav_*` (Shift→Extend)
- `demo_live`: editor-focused nav before command bus
- Gates: both scenarios ok + `demo_live_fs_compile_ok`

### STEP=1 done (2026-07-18)

- Stub `ux/nav_key.mlc` + L2 red harness

### STEP=0 done (2026-07-18)

- Decision frozen below; PLAN §46 + UX_BACKLOG #2 → active.

## Decision (STEP=0) — **frozen** 2026-07-18

| Item | Choice |
|------|--------|
| Problem | Caret nav model exists (`nav_move_left/right/up/down`, `nav_line_home/end`, `nav_page_up/down` + `Collapse`/`Extend`) but live demo never calls it; `glfw_gl_take_binding_key` does not surface arrow/Home/End/Page keys |
| Input | Extend GLFW binding-key edge set to report stable strings: `left`/`right`/`up`/`down`/`home`/`end`/`page_up`/`page_down` (same `take_binding_key` / test_push path used by command bus). Prefer not inventing a second poll API |
| Apply path | Thin `ux/` or `app/` helper (e.g. `editor_ux_apply_nav_key` / `editor_app_apply_nav_key`) mapping key + shift→`Extend` else `Collapse` → `nav_*` with current `LineIndex` + `EditorViewMetrics`; `demo_live` calls it when editor focused and key is a nav key (before or beside command-bus dispatch) |
| Shift | Shift+nav → `Extend` (selection); bare → `Collapse`. Match `navigation_unit` |
| Page metrics | Use existing `view_metrics.page_rows` from editor viewport |
| Scenarios | L2: `arrow_keys_move_caret`, `home_end_caret` (named in backlog). PageUp/Down covered in unit and/or one of the scenarios if cheap; not a separate backlog gate |
| Standing discipline | Scenario-first; name tokens in SESSION `done` |
| Module touch | `runtime` GLFW binding edges (minimal) + `misc/editor` ux/app + `demo_live`. No `compiler/` / `lib/mlc/` |
| REG | no `lib/mlc/`; no `compiler/` |

### Gates

| Step | Token |
|------|-------|
| 1–2 | `ux_ok arrow_keys_move_caret` + `ux_ok home_end_caret` (+ run scripts) |
| every | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog / SESSION trim | **done** (2026-07-18) |
| 1 | L2 scenarios first (`arrow_keys_move_caret`, `home_end_caret`) | **done** (red harness) |
| 2 | GLFW nav keys + ux/app apply + `demo_live` wire | **done** |
| 3 | Critic: gates; archive | close |

### Sub-steps (Driver)

**STEP=1** — **done**

**STEP=2** — **done**

**STEP=3** — Critic; `next` = Planner (§46 #3).

## Out of scope

Word boundaries (#3); mouse word/line select (#4); Ctrl+arrows; `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
