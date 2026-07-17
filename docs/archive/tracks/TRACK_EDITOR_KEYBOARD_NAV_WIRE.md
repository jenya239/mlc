# Track: Editor keyboard nav wire (arrows / Home / End / Page)

Parent: [../agent/TRACK_EDITOR_UX_BACKLOG.md](../agent/TRACK_EDITOR_UX_BACKLOG.md) §46 **#2**.
Model already in `document/navigation.mlc` (`nav_move_*`, `nav_line_*`,
`nav_page_*`); not called from `demo_live`. GLFW binding keys today omit
arrows/Home/End/Page (`glfw_gl_take_binding_key` only letters/tab).

## Status: **closed** (2026-07-18) — Critic OK

**Critic 2026-07-18 (STEP=3):** Re-ran L2 + compile. Anti-false-done:
`79472401`…`925e1420` (STEP=0–2). Wire present: GLFW edges
`left`/`right`/`up`/`down`/`home`/`end`/`page_up`/`page_down` via
`glfw_gl_take_binding_key`; `editor_ux_apply_nav_key` → `nav_*`
(Shift→Extend); `demo_live` applies when editor focused before command bus.
**reopen: none**.

Honest residual: no dedicated L2 for PageUp/Down (Decision: unit/cheap
coverage OK); Ctrl+arrows/word nav → §46 `#3`.

| Gate | Result |
|------|--------|
| `run_ux_arrow_keys_move_caret.sh` | `ux_ok arrow_keys_move_caret` EXIT=0 |
| `run_ux_home_end_caret.sh` | `ux_ok home_end_caret` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§46 `#3 EDITOR_WORD_BOUNDARIES`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `79472401` | Decision freeze + open |
| 1 | `68857747` | L2 red harness |
| 2 | `925e1420` | GLFW + apply + demo_live wire |
| 3 | this Critic | close + archive |

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
| 3 | Critic: gates; archive | **done** (closed) |

## Out of scope

Word boundaries (#3); mouse word/line select (#4); Ctrl+arrows; `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
