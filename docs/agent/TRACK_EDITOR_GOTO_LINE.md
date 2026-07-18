# Track: Editor Go-to-line (Ctrl+G → scroll+caret)

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#8**.
No go-to-line: cannot jump caret to a numeric line and scroll it into view.
Review gate: `goto_line_scrolls_caret` (L1). Size **S**.

## Status: **active** (2026-07-18) — STEP=1 done; STEP=2 next

## Next step

**STEP=2** — implement `editor_ux_goto_line` + CmdGotoLine + demo_live wire;
gates green.

### STEP=1 done (2026-07-18)

- Stub `ux/goto_line.mlc`; L1 red harness `goto_line_scrolls_caret` (+ run script);
  `demo_live_fs_compile_ok`.

### STEP=0 done (2026-07-18)

- Decision frozen below; PLAN §46 + UX_BACKLOG #8 → active.

## Decision (STEP=0) — **frozen** 2026-07-18

| Item | Choice |
|------|--------|
| Problem | No Ctrl+G / go-to-line; caret cannot jump to a chosen line with scroll |
| Line numbers | **1-based** (Sublime-like). Clamp to `[1, line_count]`. Line 1 = first line |
| Apply | `editor_ux_goto_line(line_index, line_number_1based)` → collapsed selection at `line_index_line_start` for that line; then `editor_ux_ensure_caret_visible` (existing) so scroll brings caret into viewport |
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
| 2 | goto helper + CmdGotoLine + demo wire (+ ensure visible) | scenario + compile |
| 3 | Critic: gates; archive | close |

### Sub-steps (Driver)

**STEP=1**
1. Add `ux_scenarios/goto_line_scrolls_caret.mlc` (+ run script).
2. Prefer red before STEP=2 (stub goto no-ops / wrong line).

**STEP=2**
1. Implement `editor_ux_goto_line` (+ seed-from-selection); wire CmdGotoLine + `demo_live`; GLFW `g` if missing.
2. Gates: scenario ok + `demo_live_fs_compile_ok`.

**STEP=3** — Critic; `next` = Planner (§46 #9).

## Out of scope

Find panel (#7 residual); drag-autoscroll (#9); column-within-line goto; `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
