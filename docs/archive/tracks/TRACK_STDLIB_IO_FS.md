# Track: Stdlib IO/FS platform gaps (list_dir + friends)

Parent: [../../EDITOR.md](../../EDITOR.md), [../../GUI_ARCHITECTURE.md](../../GUI_ARCHITECTURE.md),
[../../PLAN.md](../../PLAN.md) §36.

## Status: **closed** (2026-07-16) — Critic OK

**Critic 2026-07-16 (STEP=8):** Re-ran FS smokes + demo_live compile + REG.
Anti-false-done: `e05ae187`…`ecf8568f` (STEP=0–6). STEP=7 content_scale
**deferred** (Decision). **reopen: none**. Residuals: content_scale;
recursive glob / watch / Fontconfig out of scope.

| Gate | Result |
|------|--------|
| `run_stdlib_fs_stat_smoke.sh` | `fs_stat_ok` |
| `run_stdlib_list_dir_smoke.sh` | `list_dir_ok` |
| `run_stdlib_list_dir_safe_smoke.sh` | `list_dir_safe_ok` |
| `run_stdlib_mkdir_smoke.sh` | `mkdir_ok` |
| `run_editor_file_tree_fs_unit.sh` | `file_tree_fs_unit ok` |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` |
| `scripts/regression_gate.sh` | 20 passed, 0 failed; sweep ok=147 fail=0 skip=1 |

## Next step

**closed** — Critic OK. Queue → Planner.

### STEP=8 done (2026-07-16)

- Critic gates + REG; archive.

### STEPs 0–6 done in git (2026-07-16)

| Step | Commit (abbrev) | Gate |
|------|-----------------|------|
| 0 | `e05ae187` | Decision freeze |
| 1 | `9cc4cc95` | `fs_stat_ok` |
| 2 | `74d4bf50` | `list_dir_ok` |
| 3 | `3c58413b` | `list_dir_safe_ok` |
| 4 | `78b7d680` | `mkdir_ok` |
| 5 | `2416f334` | `file_tree_fs_unit ok` |
| 6 | `ecf8568f` | `demo_live_fs_compile_ok` |

## Decision (frozen) — summary

`FsDirEntry`; lex `list_dir`; parallel ABI arrays; bare list → `[]` on error;
`safe_list_dir` → Result; `create_directories` parents+idempotent; editor
`file_tree_expand_from_disk`; demo_live root = `MLC_EDITOR_ROOT` \| argv \| `.`
with mock `/demo` fallback. No `compiler/`. STEP=7 deferred.

## Out of scope (unchanged)

Recursive glob; file watch; Fontconfig; content_scale; SCRIPT_VM; MIR Epic 5.
