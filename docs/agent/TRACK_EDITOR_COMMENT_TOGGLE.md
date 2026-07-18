# Track: Editor Comment Toggle (Ctrl+/)

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#19**.
No line-comment toggle; Ctrl+/ does nothing. Status bar already exposes
language from path (`status_bar_language_from_path`). Review gate:
`toggle_line_comment` (L1). Size **S**.

## Status: **active** (2026-07-19) — STEP=2 done; next Critic

## Next step

**STEP=3** — Critic: gates; archive.

## Decision (STEP=0) — **frozen** 2026-07-19

| Item | Choice |
|------|--------|
| Problem | No Ctrl+/ line-comment toggle; language already on status bar unused for edit |
| Prefix | From language string: `mlc`/`cpp` → `//`; `ruby` → `#`; **else** → `//` (incl. `markdown`/`text`) |
| Scope | Lines intersecting selection (collapsed → caret line only) |
| Toggle rule | If **every** targeted line is already commented (after leading indent) → uncomment all; else comment all (Sublime-style) |
| Comment | Insert prefix after leading ASCII spaces/tabs (`"  foo"` → `"  //foo"`) |
| Uncomment | Remove one leading prefix after indent (`"  //foo"` → `"  foo"`); lines without prefix unchanged |
| Caret/selection | Preserve selection byte range as well as practical after edit (collapsed caret stays on same line when possible); L1 asserts text |
| API | `editor_ux_toggle_line_comment(state, language) -> EditorUxState` (+ thin document helper); L1 calls API without GLFW |
| Command | `CmdToggleLineComment`; chord Ctrl+`/` (`key: "/"`); editor focus only |
| Live | demo_live dispatch + `command_bus_default_bindings`; GLFW edge for `GLFW_KEY_SLASH` → `"/"` in `runtime/src/gl/glfw_window_gl.cpp` if missing |
| Scenarios | L1: `toggle_line_comment` — language `mlc`; body `foo` → toggle → `//foo` → toggle → `foo`; token `ux_ok toggle_line_comment` |
| Standing discipline | Scenario-first; name tokens in SESSION `done` |
| Module touch | `misc/editor` (+ scenario/script); may touch `runtime/src/gl` for slash edge. No `compiler/` / `lib/mlc/` |
| REG | no `lib/mlc/`; no `compiler/` |

### Gates

| Step | Token |
|------|-------|
| 1–2 | `ux_ok toggle_line_comment` (+ run script) |
| every | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |
| regression | `bash scripts/run_ux_typing_paren_autocloses.sh` → `ux_ok typing_paren_autocloses` |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog | **done** (2026-07-19) |
| 1 | L1 scenario first (`toggle_line_comment`) | **done** (2026-07-19) — stub red: `ux_fail toggle_line_comment comment` |
| 2 | toggle API + CmdToggleLineComment + live/GLFW wire | **done** (2026-07-19) — L1 green `ux_ok toggle_line_comment` |
| 3 | Critic: gates; archive | pending |

## Out of scope

Block comments `/* */`; language-specific markdown HTML comments; column-0-only mode; `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
