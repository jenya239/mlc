# Track: Editor keyboard nav wire (arrows / Home / End / Page)

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#2**.
Model already in `document/navigation.mlc` (`nav_move_*`, `nav_line_*`,
`nav_page_*`); not called from `demo_live`. GLFW binding keys today omit
arrows/Home/End/Page (`glfw_gl_take_binding_key` only letters/tab).

## Status: **active** (2026-07-18) — STEP=0 done; STEP=1 next

## Next step

**STEP=1** — L2 scenarios first: `arrow_keys_move_caret` + `home_end_caret`
(Standing discipline); red harness if needed.

### STEP=0 done (2026-07-18)

- Decision frozen below; PLAN §46 + UX_BACKLOG #2 → active.
- SESSION trim: older turns → `docs/archive/SESSION_HISTORY.md`.

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
| 1 | L2 scenarios first (`arrow_keys_move_caret`, `home_end_caret`) | scenario fails or harness ready |
| 2 | GLFW nav keys + ux/app apply + `demo_live` wire | scenarios + compile |
| 3 | Critic: gates; archive | close |

### Sub-steps (Driver)

**STEP=1**
1. Add `ux_scenarios/arrow_keys_move_caret.mlc` + `home_end_caret.mlc` (+ run scripts).
2. Prefer red before STEP=2 wiring (stub apply returns unchanged selection).

**STEP=2**
1. Extend `glfw_gl_take_binding_key` edges for nav keys.
2. Implement apply helper; wire `demo_live` when editor focused.
3. Gates: both scenarios ok + `demo_live_fs_compile_ok`.

**STEP=3** — Critic; `next` = Planner (§46 #3).

## Out of scope

Word boundaries (#3); mouse word/line select (#4); Ctrl+arrows; `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
