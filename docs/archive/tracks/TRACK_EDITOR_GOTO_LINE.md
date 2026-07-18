# Track: Editor Go-to-line (Ctrl+G → scroll+caret)

Parent: [../agent/TRACK_EDITOR_UX_BACKLOG.md](../agent/TRACK_EDITOR_UX_BACKLOG.md) §46 **#8**.
No go-to-line: cannot jump caret to a numeric line and scroll it into view.
Review gate: `goto_line_scrolls_caret` (L1). Size **S**.

## Status: **closed** (2026-07-18) — Critic OK

**Critic 2026-07-18 (STEP=3):** Re-ran L1 + compile. Anti-false-done:
`586b99ce`…`1be689c8` (STEP=0–2). Wire present: `editor_ux_goto_line`
(1-based clamp + `ensure_caret_visible`); `editor_ux_goto_line_from_selection`
(ASCII digits); `CmdGotoLine` Ctrl+G; demo_live; GLFW `g`. **reopen: none**.

Honest residual: no goto-panel chrome (Decision); target seed from digit
selection only (empty/invalid → no-op); no last-target persistence; L1 calls
goto API directly (not full Ctrl+G inject path); column-within-line out of scope.

| Gate | Result |
|------|--------|
| `run_ux_goto_line_scrolls_caret.sh` | `ux_ok goto_line_scrolls_caret` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§46 `#9 EDITOR_DRAG_AUTOSCROLL`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `586b99ce` | Decision freeze + open |
| 1 | `d4e4ec20` | L1 red harness |
| 2 | `1be689c8` | goto + CmdGotoLine + demo_live + GLFW g |
| 3 | this Critic | close + archive |

## Decision (STEP=0) — **frozen** 2026-07-18

| Item | Choice |
|------|--------|
| Problem | No Ctrl+G / go-to-line; caret cannot jump to a chosen line with scroll |
| Line numbers | **1-based** (Sublime-like). Clamp to `[1, line_count]`. Line 1 = first line |
| Apply | `editor_ux_goto_line(state, metrics, line_number_1based)` → collapsed selection at `line_index_line_start` for that line; then `editor_ux_ensure_caret_visible` (existing) so scroll brings caret into viewport |
| Query / entry | **Seed from non-empty selection** if selected text parses as decimal integer (ASCII digits only). Persist last target in thin session/state if useful. **No goto panel chrome** this track (S; same pattern as #7 Find seed). Invalid/empty → no-op (selection/scroll unchanged) |
| Commands | `CmdGotoLine` (Ctrl+G). Wire command bus + `demo_live`. GLFW binding key `"g"` (like `"f"` for Find) |
| Scenarios | L1: `goto_line_scrolls_caret` — multi-line doc; goto mid/far line; assert caret line + `scroll_offset_y` such that caret is in viewport (reuse ensure_caret_visible semantics from wheel scenario) |
| Standing discipline | Scenario-first; name tokens in SESSION `done` |
| Module touch | `misc/editor` ux goto + commands + `demo_live` (+ scenario/script; GLFW `g` if needed). No `compiler/` / `lib/mlc/` |
| REG | no `lib/mlc/`; no `compiler/` |

### Gates

| Step | Token |
|------|-------|
| 1–2 | `ux_ok goto_line_scrolls_caret` (+ run script) |
| every | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog | **done** (2026-07-18) |
| 1 | L1 scenario first (`goto_line_scrolls_caret`) | **done** (red: `ux_fail goto caret line`) |
| 2 | goto helper + CmdGotoLine + demo wire (+ ensure visible) | **done** (`ux_ok`; `demo_live_fs_compile_ok`) |
| 3 | Critic: gates; archive | **done** (closed) |

## Out of scope

Find panel (#7 residual); drag-autoscroll (#9); column-within-line goto; `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
