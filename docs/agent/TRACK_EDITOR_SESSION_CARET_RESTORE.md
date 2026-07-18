# Track: Editor Session Caret Restore

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#20**.
`WorkspaceSession` stores root + paths + active index only; reopen always
puts caret at 0 and scroll at 0. Review gate: `session_restore_caret`
(L0+L1). Size **S**.

## Status: **active** (2026-07-19) — STEP=0 done; next STEP=1

## Next step

**STEP=1** — L1 scenario first (`session_restore_caret`) red against stub.

## Decision (STEP=0) — **frozen** 2026-07-19

| Item | Choice |
|------|--------|
| Problem | Session reopen restores paths/active only; caret + scroll reset |
| Stored per path | Parallel arrays on `WorkspaceSession`: `carets` / `scroll_ys` / `scroll_xs` (same length as `paths`) |
| Values | Collapsed caret **byte** offset; `scroll_offset_y`; `scroll_offset_x` (i32) |
| Selection range | **Not** persisted — restore always collapsed caret (anchor=caret) |
| Encode | One `view=<caret>,<scroll_y>,<scroll_x>` line per `file=` line, same order; omit/`view=0,0,0` ok |
| Decode compat | Missing `view=` lines → fill 0; extra/malformed → 0 for that slot |
| Clamp | On restore: caret ∈ `[0, document_byte_length]`; scroll ≥ 0 (no viewport clamp in L1) |
| API | Extend session encode/decode/from_tabs (or `with_views`); restore helper yields tabs + **active** caret/scroll for ux; L1 calls without GLFW |
| Live | `save_demo_session` / `load_demo_session` (+ app start) write/apply **active** tab view from `app.ux` |
| Scenarios | L1: `session_restore_caret` — session with path + caret≠0 + scroll_y≠0 → encode/decode → restore → assert caret + scroll_y; token `ux_ok session_restore_caret` |
| Standing discipline | Scenario-first; name tokens in SESSION `done` |
| Module touch | `misc/editor/workspace/session.mlc` (+ scenario/script); thin demo_live/app wire. No `compiler/` / `lib/mlc/` |
| REG | no `lib/mlc/`; no `compiler/` |

### Gates

| Step | Token |
|------|-------|
| 1–2 | `ux_ok session_restore_caret` (+ run script) |
| every | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |
| regression | `bash scripts/run_ux_toggle_line_comment.sh` → `ux_ok toggle_line_comment`; existing `session_unit` still ok |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog | **done** (2026-07-19) |
| 1 | L1 scenario first (`session_restore_caret`) | pending |
| <!-- sub-steps: 1) `ux_scenarios/session_restore_caret.mlc` + `scripts/run_ux_session_restore_caret.sh`; 2) stub restore ignores views → red `ux_fail session_restore_caret`; 3) demo_live_fs_compile + toggle_line_comment still green --> |
| 2 | session view arrays + encode/decode/restore + demo_live wire | pending |
| <!-- sub-steps: 1) extend `WorkspaceSession` + encode/decode; 2) restore applies active caret/scroll; 3) demo save/load; 4) L1 green + session_unit compat --> |
| 3 | Critic: gates; archive | pending |

## Out of scope

Non-collapsed selection persistence; in-memory per-tab view cache across tab switches (today selection resets on activate); dirty-buffer body in session; `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
