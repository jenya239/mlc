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

**STEP=5** — Editor: expand-from-disk helper; unit with temp fixture.

### STEP=4 done (2026-07-16)

- `create_directories` / `create_directories_value`; export on `file.mlc`.
- Smoke `mkdir_ok` (nested create, idempotent, write+list, fail on file).

### STEP=3 done (2026-07-16)

- `safe_list_dir` → `Result`; Err missing / not-a-dir.
- Smoke `list_dir_safe_ok`.

### STEP=2 done (2026-07-16)

- Parallel ABI `list_dir_names` / `list_dir_is_directory_flags`; `FsDirEntry` + `list_dir` zip.
- Smoke `list_dir_ok` (lex order alpha/mid/zebra).

### STEP=1 done (2026-07-16)

- `is_directory` / `is_regular_file` in `file.hpp` + `*_value` ABI; exports on `file.mlc`.
- Smoke `fs_stat_ok`.

### STEP=0 done (2026-07-16)

- Decision frozen below; PLAN §36 → STEP=1 next.
- STEP=7 content_scale deferred.

## Decision (STEP=0) — **frozen** 2026-07-16

Grounded in existing `mlc::file::{exists,exists_value}` + editor
`DirEntry { name, is_directory: i32 }` (`misc/editor/workspace/file_tree.mlc`).

| Item | Choice |
|------|--------|
| Placement | `runtime/include/mlc/io/file.hpp` + `file_abi.hpp` by-value helpers; MLC `lib/mlc/common/stdlib/io/file.mlc` (+ thin wrappers if needed) |
| Stdlib type | `FsDirEntry = { name: string, is_directory: i32 }` in stdlib IO — **not** reuse editor type name (avoid clash); editor converts field-wise |
| `is_directory` | `i32`/`bool` via ABI `is_directory_value(String) -> bool`; **false** if path missing; follows symlinks (`std::filesystem::is_directory`) |
| `is_regular_file` | same pattern `is_regular_file_value`; false if missing; follows symlinks |
| `create_directories` | `create_directories_value(String) -> bool` = `std::filesystem::create_directories` (parents ok); true if created or already exists as dir |
| `list_dir` | Non-recursive; skip `.` and `..`; **lexicographic** by `name` (byte order); follows symlink targets for `is_directory` flag |
| `list_dir` errors | Bare `list_dir` → empty `[FsDirEntry]` on missing/not-a-dir/IO fail (match bool-style IO); `safe_list_dir` → `Result<[FsDirEntry], string>` with Err message |
| ABI for list | Parallel arrays (mlcc-friendly): `list_dir_names_value` → `Array<String>`, `list_dir_is_directory_value` → `Array<i32>` same length/order; MLC zips to `[FsDirEntry]` |
| Symlinks | Follow for type checks; listing shows symlink **name**, flag = target is_directory |
| Editor wire | STEP=5–6: convert `FsDirEntry` → editor `DirEntry`; `file_tree_expand`; `demo_live` root = argv / `.` / `MLC_EDITOR_ROOT`; mock fallback if root missing |
| Ignore | Editor `ignore_list` only; stdlib never filters `.git` |
| STEP=7 | **deferred** — `glfw_gl_window_content_scale` out of MVP (revisit later track) |
| REG | `lib/mlc/` touch → `scripts/regression_gate.sh` before Critic; **no** `compiler/` |
| Namespace | C++ `mlc::file::*`; MLC exports bare names on `file.mlc` module |

### Exact exports (STEPs 1–4)

```text
is_directory(path: string) -> bool          # or i32 0|1 matching project bool style
is_regular_file(path: string) -> bool
create_directories(path: string) -> bool
list_dir(path: string) -> [FsDirEntry]
safe_list_dir(path: string) -> Result<[FsDirEntry], string>
```

ABI (file_abi.hpp): `*_value` siblings for the four predicates/mkdir + two list arrays.

### Non-goals (Decision)

Recursive glob; file watch; symlink-as-entry type flag; promoting ignore rules into stdlib; Fontconfig; `compiler/` changes.

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + PLAN/CONTINUITY | **done** (2026-07-16) |
| 1 | Runtime + ABI: `is_directory` / `is_regular_file` | **done** (2026-07-16): `fs_stat_ok` |
| 2 | Runtime + ABI + stdlib: `list_dir` → `[FsDirEntry]` | **done** (2026-07-16): `list_dir_ok` |
| 3 | `safe_list_dir` + missing-path Err | **done** (2026-07-16): `list_dir_safe_ok` |
| 4 | `create_directories` (+ smoke write then list) | **done** (2026-07-16): `mkdir_ok` |
| 5 | Editor: expand-from-disk helper; unit with temp fixture | `run_editor_file_tree_fs_unit.sh` |
| 6 | `demo_live`: real tree via `list_dir`; mock fallback if root missing | compile smoke |
| 7 | Optional content_scale | **deferred** (STEP=0) |
| 8 | Critic: gates + REG if required; archive | close |

### Sub-steps (Driver)

**STEP=0** — **done**
1. Freeze table; name exports exactly — done.
2. Non-goals + symlink + order + STEP=7 deferred — done.

**STEP=1** — **done**
1. `is_directory` / `is_regular_file` in `file.hpp` + `*_value` in `file_abi.hpp` — done.
2. MLC extern + smoke `fs_stat_ok` — done.

**STEP=2** — **done**
1. Parallel ABI `list_dir_names_value` / `list_dir_is_directory_value`; MLC zip to `[FsDirEntry]` — done.
2. Smoke `list_dir_ok` — done.

**STEP=3** — **done**
1. `safe_list_dir` → Err on missing/not-a-dir — done.
2. Smoke `list_dir_safe_ok` — done.

**STEP=4** — **done**
1. `create_directories` + smoke `mkdir_ok` — done.

**STEP=5**
1. Editor expand-from-disk + unit.

**STEP=8** — Critic; `next` = Planner.

## Out of scope

- Fontconfig / system font enumeration
- OS path watcher
- Promoting editor ignore rules into stdlib
- `compiler/` changes (escalate only if ABI impossible)
- LANG_AUTO_CYCLE / SCRIPT_VM / MIR Epic 5
- GLFW content scale (deferred STEP=7)

## Verify discipline

- One STEP per turn; commit + push.
- Foreign: SCRIPT_VM / `.tmp` junk — do not commit.
- Anti-false-done on smokes.
