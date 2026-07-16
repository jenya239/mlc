# Track: Stdlib IO/FS platform gaps (list_dir + friends)

Parent: [../EDITOR.md](../EDITOR.md), [../GUI_ARCHITECTURE.md](../GUI_ARCHITECTURE.md),
[../PLAN.md](../PLAN.md) §36.
Source: EDITOR gaps table; MVP P1 deferred; live tree still mock
(`demo_live` `demo_dir_entries`).

Clipboard / scissor / cursor — **already** in §33b LIVE_POLISH — do not redo.

## Status: **active** (2026-07-16) — PRIORITY OVERRIDE / queue head

User 2026-07-16: priority stdlib expansion for real file tree / FS.
§35 FORM_LIVE closed (`66baa403`); this track is queue head.

## Next step

**STEP=0** — Freeze Decision (API names, DirEntry shape, ABI) + PLAN sync.

## Decision (STEP=0) — draft until Driver freezes

| Item | Direction |
|------|-----------|
| Placement | `lib/mlc/common/stdlib/io/file.mlc` + `runtime/include/mlc/io/file.hpp` + `file_abi.hpp` by-value helpers (same pattern as `exists_value`) |
| `DirEntry` | `{ name: string, is_directory: i32 }` — match `misc/editor/workspace/file_tree.mlc` or thin convert |
| `list_dir` | Non-recursive; skip `.` / `..`; order freeze in STEP=0 |
| Errors | `list_dir` + `safe_list_dir` → `Result<[DirEntry], string>` |
| `is_directory` / `is_regular_file` | Separate predicates; `exists` already present |
| `create_directories` | For `.tmp/` fixtures / session dirs |
| Editor wire | `file_tree_expand` fed by `list_dir`; `demo_live` real root (argv / `.` / `MLC_EDITOR_ROOT`) |
| Ignore | Keep `ignore_list.mlc` in editor; stdlib does not hardcode `.git` |
| REG | Touching `lib/mlc/` → `scripts/regression_gate.sh` before Critic; prefer **no** `compiler/` |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + PLAN/CONTINUITY | doc |
| 1 | Runtime + ABI: `is_directory` / `is_regular_file` | `run_stdlib_fs_stat_smoke.sh` → `fs_stat_ok` |
| 2 | Runtime + ABI + stdlib: `list_dir` → `[DirEntry]` | `run_stdlib_list_dir_smoke.sh` → `list_dir_ok` |
| 3 | `safe_*` Result wrappers + missing-path Err | `list_dir_ok` / safe smoke |
| 4 | `create_directories` (+ smoke write then list) | `run_stdlib_mkdir_smoke.sh` → `mkdir_ok` |
| 5 | Editor: expand-from-disk helper; unit with temp fixture | `run_editor_file_tree_fs_unit.sh` |
| 6 | `demo_live`: real tree via `list_dir`; mock fallback if root missing | compile smoke |
| 7 | Optional: `glfw_gl_window_content_scale` | smoke or **skip** if deferred in STEP=0 |
| 8 | Critic: gates + REG if required; archive | close |

### Sub-steps (Driver)

**STEP=0**
1. Freeze table; name exports exactly.
2. Non-goals: recursive glob, file watch, symlink policy (document: follow or not).

**STEP=1–4** — runtime/stdlib only; explicit `git add`; no editor yet.

**STEP=5–6** — `misc/editor/` + demo; `demo_live` in scope for STEP=6 only.

**STEP=7** — skip if STEP=0 marks optional deferred.

**STEP=8** — Critic; `next` = Planner.

## Out of scope

- Fontconfig / system font enumeration
- OS path watcher
- Promoting editor ignore rules into stdlib
- `compiler/` changes (escalate only if ABI impossible)
- LANG_AUTO_CYCLE / SCRIPT_VM / MIR Epic 5

## Verify discipline

- One STEP per turn; commit + push.
- Foreign: SCRIPT_VM / `.tmp` junk — do not commit.
- Anti-false-done on smokes.
