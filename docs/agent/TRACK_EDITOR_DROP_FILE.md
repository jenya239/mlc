# Track: Editor Drop File

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#27**.
No OS drag-and-drop → open file; only tree/session open. Needs GLFW drop
ABI. Review gate: `drop_file_opens_tab` (L1). Size **M**.

## Status: **active** (2026-07-19) — STEP=3 done; next Critic

## Next step

**STEP=4** — Critic: gates; archive

## Decision (STEP=0) — **frozen** 2026-07-19

| Item | Choice |
|------|--------|
| Problem | Dragging a file onto the editor window does nothing; no GLFW drop ABI |
| ABI | `glfwSetDropCallback` in `glfw_gl_context_begin`; queue UTF-8 paths; `glfw_gl_take_drop_path() -> string` (empty if none — one path per take, same pattern as `take_text` / binding key); `glfw_gl_drop_path_test_push(path)` for headless; declare in `gl_window.mlc` |
| Multi-drop | Callback may enqueue many paths; demo_live drains with `while take` non-empty each frame |
| Open | For each path: `open_buffer_from_path` (inherits #13 encoding/binary guard + #12 CRLF). If `open_buffer_has_error` → skip path (no tab). Else open/activate via existing `tab_set_open` + document text (reuse session/tree pattern; path already open → activate only) |
| Directories | Skip (open fails or no tab); no recursive expand this track |
| Selection | New/activated tab → `selection_collapsed(0)`; history reset for that switch (same as other open paths in demo_live) |
| API | Thin `editor_ux_open_dropped_path(state, path)` (or batch) in `misc/editor/ux/`; demo_live polls take → open |
| Scenarios | L1: `drop_file_opens_tab` — write fixture file; `drop_path_test_push`; call open/take path; assert tab for that path active + buffer text; token `ux_ok drop_file_opens_tab` |
| Standing discipline | Scenario-first; name tokens in SESSION `done` |
| Module touch | `runtime/` + `lib/mlc/.../gl_window.mlc` + `misc/editor` ux/demo + scenario/script |
| REG | **yes** — `lib/mlc/` touched → `scripts/regression_gate.sh` before Critic close |

### Gates

| Step | Token |
|------|-------|
| 1–3 | `ux_ok drop_file_opens_tab` (+ run script) |
| every | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |
| regression | `bash scripts/run_ux_indent_width_config.sh` → `ux_ok indent_width_config` |
| Critic | `scripts/regression_gate.sh` (lib/mlc) |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog | **done** (2026-07-19) |
| 1 | L1 scenario first (`drop_file_opens_tab`) + stub | **done** (2026-07-19) — stub red: `ux_fail drop_file_opens_tab no_tab` |
| <!-- sub-steps: 1) scenario + run script; 2) stub take/open → red; 3) indent_width + demo_live_fs_compile green --> |
| 2 | GLFW drop ABI + take/test_push + open helper | **done** (2026-07-19) — `ux_ok drop_file_opens_tab` |
| <!-- sub-steps: 1) runtime callback + queue; 2) gl_window extern; 3) ux open + L1 green --> |
| 3 | demo_live drain take_drop_path → open tabs | **done** (2026-07-19) — poll + REG |
| <!-- sub-steps: 1) poll loop; 2) demo_live_fs_compile; 3) gates + REG --> |
| 4 | Critic: gates; archive | pending |

## Out of scope

Folder drop / recursive import; URL drops; custom drop UI chrome; drag-out export; `compiler/` (except via existing open); changing encoding/CRLF policy.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
